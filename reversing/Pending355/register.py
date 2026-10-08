"""Registra el lote únicamente después de aprobar sus fuentes e instrumentos actuales."""
import argparse
import json
import re
import sys
import subprocess
from pathlib import Path

HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME,EXE_HASH,digest
import probe_summary
from status import inventory

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--strict',action='store_true',help='Exige hash del EXE y ASI realmente ejecutados, registrado por el runner final.')
    args=parser.parse_args()
    # Caché únicamente durante esta invocación; los hardlinks NTFS comparten st_dev/st_ino.
    hash_cache={}
    uncached_digest=digest
    def memo_digest(path):
        path=Path(path);stat=path.stat()
        key=(stat.st_dev,stat.st_ino,stat.st_size,stat.st_mtime_ns,stat.st_ctime_ns)
        # Un FS sin inode fiable conserva identidad por ruta, sin fusionar archivos distintos.
        if not stat.st_ino:key+=(str(path.resolve()),)
        if key not in hash_cache:hash_cache[key]=uncached_digest(path)
        return hash_cache[key]
    # status.inventory y probe_summary siguen pasando todos sus gates; sólo comparten lectura de hashes.
    inventory.__globals__['digest']=memo_digest
    probe_summary.digest=memo_digest
    digest_local=memo_digest
    scope=json.loads((HERE/'scope.json').read_text(encoding="utf-8-sig"))
    expected={(r['class'],r['fn_name']) for r in scope['functions']}
    resolutions=json.loads((HERE/'resolutions.json').read_text(encoding="utf-8-sig"))
    corrections=resolutions['catalog_corrections']
    expected -= {(r['class'],r['fn_name']) for r in corrections}
    info=inventory()
    required_headers={p.relative_to(ROOT).as_posix() for p in (ROOT/'source').rglob('*') if p.is_file() and p.suffix in ('.h','.hpp','.inl','.inc')}
    changed_cpp=subprocess.check_output(['git','diff','HEAD','--name-only','--','source'],cwd=ROOT,text=True).splitlines()
    changed_cpp+=subprocess.check_output(['git','ls-files','--others','--exclude-standard','--','source'],cwd=ROOT,text=True).splitlines()
    required_cpp={p.replace('\\','/') for p in changed_cpp if p.endswith('.cpp') and p.replace('\\','/')!='source/game_sa/Game.cpp' and (ROOT/p).is_file()}
    plans=[];covered=set()
    for item in info['banks']:
        bank=HERE.parent/item['bank'];prefix=item['prefix']
        reports=sorted((GAME/f'resultados-{prefix}').glob('*/summary.json'))
        if not reports:raise RuntimeError('Sin ejecución: '+item['bank'])
        report=reports[-1];result=json.loads(report.read_text(encoding="utf-8-sig"))
        manifest_path=bank/'artifacts/build-manifest.json'
        if not manifest_path.is_file():raise RuntimeError('Sin manifiesto: '+item['bank'])
        manifest=json.loads(manifest_path.read_text(encoding="utf-8-sig"))
        if args.strict:
            if not manifest.get('full_source_manifest') or not (required_headers|required_cpp).issubset(manifest['sources']):
                raise RuntimeError('Manifiesto final incompleto: '+item['bank'])
            if manifest.get('game_cpp')!=digest_local(ROOT/'source/game_sa/Game.cpp'):
                raise RuntimeError('Game.cpp posterior al banco: '+item['bank'])
            if result.get('executed_probe_sha256')!=manifest['binaries']['asi']:
                raise RuntimeError('ASI ejecutada distinta o sin hash: '+item['bank'])
            if result.get('original_exe_sha256')!=EXE_HASH:
                raise RuntimeError('EXE ejecutado distinto o sin hash: '+item['bank'])
        if not result.get('passed') or report.stat().st_mtime<manifest_path.stat().st_mtime:
            raise RuntimeError('Repetir banco: '+item['bank'])
        for path,value in manifest['sources'].items():
            if digest_local(ROOT/path)!=value:raise RuntimeError('Fuente posterior al banco: '+path)
        for path,value in manifest['instrumentation'].items():
            if digest_local(bank/path)!=value:raise RuntimeError('Instrumento posterior al banco: '+str(bank/path))
        for ext,value in manifest['binaries'].items():
            if digest_local(bank/'artifacts'/f'gta_reversed-{prefix}-probe.{ext}')!=value:
                raise RuntimeError('Binario distinto: '+item['bank'])
        keys={tuple(label.split('::')) for label in result['pairs_per_function']} & expected
        if covered & keys:raise RuntimeError('Cobertura duplicada: '+str(covered & keys))
        covered |= keys
        plans.append((bank,prefix,result,report))
    if covered!=expected:raise RuntimeError('Faltan identidades válidas: '+str(expected-covered))
    # La revisión completa anterior precede cualquier escritura del registro.
    cases_path=HERE.parent/'cases.json';cases=json.loads(cases_path.read_text(encoding="utf-8-sig"))
    limitations=['Equivalencia limitada a los fixtures documentados en el banco. Las colaboraciones aisladas simétricamente no acreditan su implementación; no constituye una campaña integral de juego ni independencia del EXE.']
    for bank,prefix,result,report in plans:
        specific=[r['validation_limit']+' Candidate: '+r['candidate_behavior']
                  for r in resolutions.get('limited_deviations',[])
                  if r['class_name']+'::'+r['fn_name'] in result['pairs_per_function']]
        probe_summary.main(bank,prefix,limitations+specific)
        cases=[c for c in cases if c['id']!=prefix]
        owners=sorted({name.split('::')[0] for name in result['pairs_per_function']})
        cases.append(dict(id=prefix,name=', '.join(owners)+': pendientes 355',runner=f'{bank.name}/run_local.py'))
    cases_path.write_text(json.dumps(cases,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    aggregate=dict(requested_identities=len(scope['functions']),valid_functions=len(covered),
                   catalog_corrections=corrections,limited_deviations=resolutions.get('limited_deviations',[]),banks=len(plans),pairs=sum(r['pairs'] for _,_,r,_ in plans),passed=True,
                   results=[dict(bank=b.name,prefix=p,pairs=r['pairs'],evidence_directory=str(report.parent)) for b,p,r,report in plans])
    (HERE/'summary.json').write_text(json.dumps(aggregate,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in aggregate.items() if k not in ('results','catalog_corrections')}))

if __name__=='__main__':main()
