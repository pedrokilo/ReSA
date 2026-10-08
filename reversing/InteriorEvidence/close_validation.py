"""Guarded Interior-only publication. Default: read-only preflight. --apply writes docs/evidence only.
Implements probe_summary's summary/record schema without invoking its per-bank writes;
all eight reports are checked before staging any publication. Does not activate hooks or alter CSV.
"""
import argparse, csv, hashlib, json, os, re, subprocess, sys, time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'reversing'))
from runtime_runner import GAME, EXE_HASH
BANKS=('InteriorBase','InteriorSpatial','InteriorLifecycleCore','InteriorLifecycleRuntime','InteriorPlacement','InteriorHome','InteriorCommerce','InteriorAbi')
STATUS_BEFORE={'InteriorBase': 'Prepared; not compiled or run. Six functions, 25,600 paired cases; canonical GTA SA 1.0 US EXE. REA pseudocode and complete exact ASM: [InteriorEvidence](../InteriorEvidence/).', 'InteriorSpatial': 'Estado: banco preparado, sin compilación ni ejecución por este agente. No declara aprobaciones. La reconstrucción e integración de cuerpos y cambios ABI pertenecen al coordinador y world355; deben estar integrados antes de ejecutar este banco.', 'InteriorLifecycleCore': 'Replay pendiente. El primer runtime completo produjo comparaciones iguales para las seis funciones (6144 pares), pero el analizador no pudo cerrar el resultado porque faltaba `state_restored`. Ese resultado no acredita el banco actual: se debe repetir tras agregar las comprobaciones de restauracion.', 'InteriorLifecycleRuntime': 'Replay pendiente. El primer runtime completo produjo comparaciones iguales para las cuatro funciones (4096 pares), pero el analizador no pudo cerrar el resultado porque faltaba `state_restored`. Ese resultado no acredita el banco actual: se debe repetir tras agregar las comprobaciones de restauracion.', 'InteriorPlacement': 'Estado: cuatro reconstrucciones y banco preparados; validación pendiente. Los primeros intentos runtime fallaron antes de completar cobertura positiva. El informe `D:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-interior-placement/20261008-090002-346332/summary.json` tiene `passed=false`: sólo tres comparaciones de PlaceFurniture iguales con retorno nulo antes del fallo del caso 3. Estos resultados históricos no aprueban ninguna de las cuatro identidades ni validan las correcciones posteriores de ABI, callbacks o precisión.', 'InteriorHome': 'Estado: reconstrucción y banco preparados. Los nueve cuerpos ya están presentes en el cpp compartido por integración del coordinador; este agente no ha compilado ni ejecutado GTA. Ningún resultado se declara aprobado para este banco.', 'InteriorCommerce': 'Estado: preparado para compilación y comparación por el coordinador; no compilado ni ejecutado por este agente. Los hooks permanecen desactivados hasta la aprobación.', 'InteriorAbi': 'Prepared bonus ABI audit, outside the 52-function catalog. Constructor0x5921D0 and destructor0x591360,1024 paired cases each,2048 total. No catalog rows or flags are changed.'}
def blob(value):return (json.dumps(value,indent=2,ensure_ascii=False)+'\n').encode('utf-8')
def sha(value):return hashlib.sha256(value).hexdigest()
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--apply',action='store_true');args=ap.parse_args();guards={};cache={};writes={}
 def digest(path):
  p=Path(path).resolve();st=p.stat();identity=(st.st_dev,st.st_ino,st.st_size,st.st_mtime_ns,st.st_ctime_ns)
  key=identity if st.st_ino else identity+(str(p),)
  if key not in cache:
   h=hashlib.sha256()
   with p.open('rb') as f:
    for chunk in iter(lambda:f.read(1024*1024),b''):h.update(chunk)
   end=p.stat();post=(end.st_dev,end.st_ino,end.st_size,end.st_mtime_ns,end.st_ctime_ns)
   if post!=identity:raise RuntimeError('Changed while hashing: '+str(p))
   cache[key]=h.hexdigest()
  guards.setdefault(p,cache[key]);return cache[key]
 def raw(path):
  expected=digest(path);data=Path(path).read_bytes()
  if sha(data)!=expected:raise RuntimeError('Changed while reading '+str(path))
  return data
 def load(path):return json.loads(raw(path).decode('utf-8-sig'))
 def local(name):
  p=(ROOT/name).resolve()
  if not p.is_relative_to(ROOT):raise RuntimeError('Outside repository path '+str(p))
  return p
 scope=load(ROOT/'reversing/InteriorEvidence/scope.json')
 expected={r['cls']+'::'+r['fn']:int(r['a'],0)if isinstance(r['a'],str)else r['a'] for r in scope if r['fn']not in('Constructor','Destructor')}
 if len(expected)!=52 or any(not k.startswith('Interior_c::')for k in expected):raise RuntimeError('Expected exact Interior52 scope')
 catalog_path=ROOT/'docs/hooks.csv';catalog_bytes=raw(catalog_path);rows=list(csv.DictReader(catalog_bytes.decode('utf-8-sig').splitlines()));indexed={}
 for row in rows:indexed.setdefault(row['class']+'::'+row['fn_name'],[]).append(row)
 for label,address in expected.items():
  hits=indexed.get(label,[])
  if len(hits)!=1 or int(hits[0]['address'],16)!=address or hits[0]['type']!='S':raise RuntimeError('Catalog identity mismatch '+label)
 registry_path=ROOT/'reversing/validation-registry.json';registry=load(registry_path)
 untouched=[r for r in registry['functions'] if r['hook']['class']!='Interior_c']
 source_files={p.relative_to(ROOT/'source').as_posix():digest(p)for p in(ROOT/'source').rglob('*')if p.is_file()}
 tree=sha(json.dumps(sorted(source_files.items()),ensure_ascii=True,separators=(',',':')).encode())
 headers={'source/'+p for p in source_files if Path(p).suffix in('.h','.hpp','.inl','.inc')}
 modified=subprocess.check_output(['git','diff','HEAD','--name-only','--','source'],cwd=ROOT,text=True).splitlines()+subprocess.check_output(['git','ls-files','--others','--exclude-standard','--','source'],cwd=ROOT,text=True).splitlines()
 required=headers|{p.replace('\\','/')for p in modified if p.endswith('.cpp')and p.replace('\\','/')!='source/game_sa/Game.cpp'}
 for p in(ROOT/'source/game_sa/Game.cpp',GAME/'gta_sa.exe',Path('D:/Grand Theft Auto San Andreas/gta-sa.exe')):
  value=digest(p)
  if p.suffix.lower()=='.exe' and value!=EXE_HASH:raise RuntimeError('Canonical/runtime EXE mismatch')
 configs={p:digest(ROOT/p)for p in('CMakeLists.txt','CMakePresets.json','ConanPresets.json','conanfile.py')if(ROOT/p).is_file()}
 plans=[];covered=set();all_labels=set();begin='<!-- BEGIN INTERIOR FINAL VALIDATION -->';end='<!-- END INTERIOR FINAL VALIDATION -->'
 for name in BANKS:
  bank=ROOT/'reversing'/name;builder=raw(bank/'build_probe.py').decode('utf-8-sig');match=re.search(r"build\([^,]+,\s*['\"]([^'\"]+)",builder)
  if not match:raise RuntimeError('Prefix missing '+name)
  prefix=match[1];header=raw(bank/'LiveComparison.hpp').decode('utf-8-sig');fixture={c+'::'+f:int(n)for c,f,n in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[\dA-Fa-f]+|0),(?:0x[\dA-Fa-f]+|0),-?\d+,(\d+)\}',header)}
  reports=sorted((GAME/f'resultados-{prefix}').glob('*/summary.json'))
  if not reports:raise RuntimeError('No runtime reports '+name)
  report=reports[-1];result=load(report);mp=bank/'artifacts/build-manifest.json';manifest_bytes=raw(mp);manifest=json.loads(manifest_bytes.decode('utf-8-sig'))
  if result.get('passed')is not True or report.stat().st_mtime<mp.stat().st_mtime:raise RuntimeError('Latest report not passed/current '+name)
  if result.get('pairs_per_function')!=fixture or result.get('pairs')!=sum(fixture.values()):raise RuntimeError('Fixture pair coverage differs '+name)
  if all_labels&set(fixture):raise RuntimeError('Duplicate coverage')
  all_labels|=set(fixture);covered|=set(fixture)&set(expected)
  if not manifest.get('full_source_manifest')or not required.issubset(manifest['sources']):raise RuntimeError('Not full-source final manifest '+name)
  if manifest.get('game_cpp')!=digest(ROOT/'source/game_sa/Game.cpp'):raise RuntimeError('Normal Game source differs '+name)
  for p,value in manifest['sources'].items():
   if digest(local(p))!=value:raise RuntimeError('Source differs '+p)
  dependencies={}
  for p,value in manifest['instrumentation'].items():
   target=(bank/p).resolve()
   if not target.is_relative_to(ROOT)or digest(target)!=value:raise RuntimeError('Instrumentation differs '+str(target))
   dependencies[target.relative_to(ROOT).as_posix()]=value
  if result.get('executed_probe_sha256')!=manifest['binaries']['asi']or result.get('original_exe_sha256')!=EXE_HASH:raise RuntimeError('Observed EXE/ASI differs '+name)
  for ext,value in manifest['binaries'].items():
   if digest(bank/'artifacts'/f'gta_reversed-{prefix}-probe.{ext}')!=value:raise RuntimeError('Archived binary differs '+name)
  readme_path=bank/'README.md';readme=raw(readme_path).decode('utf-8-sig').replace('\r\n','\n')
  status=f'Estado: validación final de fixtures aprobada por el coordinador; informe `{report.parent.name}`, {result["pairs"]} pares. '+('Dos entradas ABI fuera del catálogo.'if name=='InteriorAbi'else'El alcance se limita a los fixtures y colaboradores documentados.')
  if STATUS_BEFORE[name] in readme:readme=readme.replace(STATUS_BEFORE[name],status,1)
  elif begin in readme:
   paragraphs=readme.split('\n\n');
   if len(paragraphs)<2 or not paragraphs[1].startswith('Estado: validación final de fixtures aprobada por el coordinador;'):raise RuntimeError('Unexpected README status '+name)
   paragraphs[1]=status;readme='\n\n'.join(paragraphs)
  else:raise RuntimeError('Prepared status text changed; review exact replacement '+name)
  design=re.sub(re.escape(begin)+r'.*?'+re.escape(end),'',readme,flags=re.S).strip()
  limits=['Paired isolated fixtures only; symmetric collaborators do not certify collaborator internals or full-game behavior.','Fixture domain and collaborator boundaries from README: '+design]
  section=f'{begin}\n\nFinal paired-fixture validation: **PASSED**. Actual report `{report.parent.name}`: {result["pairs"]:,} pairs across {len(fixture)} functions. Exact full-source/instrument/archived-binary manifests and observed EXE/ASI hashes verified. '+('Bonus ABI evidence outside the52 catalog rows.'if name=='InteriorAbi'else'Cataloged Interior_c methods only.')+f'\n\n{end}'
  if begin in readme:
   if readme.count(begin)!=1 or readme.count(end)!=1:raise RuntimeError('README markers malformed')
   updated=re.sub(re.escape(begin)+r'.*?'+re.escape(end),lambda _:section,readme,flags=re.S)
  else:updated=readme.rstrip()+'\n\n'+section+'\n'
  writes[readme_path]=updated.encode('utf-8');current=bank/'current-build-manifest.json';writes[current]=manifest_bytes
  dependencies[current.relative_to(ROOT).as_posix()]=sha(manifest_bytes);dependencies[readme_path.relative_to(ROOT).as_posix()]=sha(writes[readme_path])
  summary=dict(result,evidence_directory=str(report.parent),original_exe_sha256=EXE_HASH,covered_functions={k:True for k in fixture},probe_build_manifest=manifest,limitations=limits)
  summary_path=bank/'validation-summary.json';summary_bytes=blob(summary);writes[summary_path]=summary_bytes
  dependencies[summary_path.relative_to(ROOT).as_posix()]=sha(summary_bytes)
  plans.append(dict(bank=name,prefix=prefix,report=report,result=result,manifest=manifest,limits=limits,dependencies=dependencies,summary_path=summary_path,summary_bytes=summary_bytes))
 if covered!=set(expected)or all_labels!=set(expected)|{'Interior_c::Constructor','Interior_c::Destructor'}:raise RuntimeError('Expected exact52catalog+2bonus coverage')
 proof_path=ROOT/'reversing/InteriorEvidence/dependency-review.json'
 proof=dict(kind='Interior52 final paired-fixture dependency snapshot',cataloged_functions=52,bonus_abi_functions=2,full_game_validation=False,stub_implementations_reviewed=False,source_files=source_files,source_tree_sha256=tree,banks=[dict(bank=p['bank'],report=str(p['report']),report_sha256=digest(p['report']),observed_asi_sha256=p['result']['executed_probe_sha256'],original_exe_sha256=EXE_HASH,files=p['dependencies'],limitations=p['limits'])for p in plans])
 proof_bytes=blob(proof);writes[proof_path]=proof_bytes
 registry['functions']=untouched.copy()
 for p in plans:
  for label,count in p['result']['pairs_per_function'].items():
   if label not in expected:continue
   row=indexed[label][0];registry['functions'].append(dict(hook={k:row[k]for k in('class','fn_name','address','type')},evidence=dict(path=p['summary_path'].relative_to(ROOT).as_posix(),sha256=sha(p['summary_bytes']),passed_path=['covered_functions',label],pairs=count,original_exe_sha256=EXE_HASH),tested_sources=p['manifest']['sources'],source_provenance='Final Interior paired-fixture full-manifest replay '+p['report'].parent.name,dependency_review=dict(evidence_path=proof_path.relative_to(ROOT).as_posix(),evidence_sha256=sha(proof_bytes),files={**configs,**p['dependencies']},trees={'source':tree},scope='Exact isolated fixture snapshots; collaborator internals and full game not reviewed'),scope=' '.join(p['limits']),review_reference=f'reversing/{p["bank"]}/README.md'))
 if [r for r in registry['functions']if r['hook']['class']!='Interior_c']!=untouched:raise RuntimeError('Non-Interior registry modification')
 writes[registry_path]=blob(registry)
 doc='# Interior_c: reconstrucción y validación\n\n52/52 funciones catalogadas aprobadas en la repetición final. Los dos wrappers ABI de constructor y destructor aprobaron por separado y no amplían el catálogo. Se certifican los casos documentados y sus fuentes e instrumentos congelados; los colaboradores aislados y una campaña completa del juego quedan fuera de este alcance.\n\n| Banco | Informe final | Funciones | Pares |\n|---|---|---:|---:|\n'
 for p in plans:doc+=f'| [{p["bank"]}](../reversing/{p["bank"]}/README.md) | `{p["report"].parent.name}` | {len(p["result"]["pairs_per_function"])} | {p["result"]["pairs"]} |\n'
 doc+='\nLímite de REA: el pseudo almacenado de Shop_FurnishCeiling corresponde a FurnishOffice; el ASM propio de 0x59A130 es RET. El wrapper ABI del constructor conserva NodeId; la construcción normal C++ inicializa adicionalmente ese campo y queda fuera de esa aprobación.\n'
 writes[ROOT/'docs/InteriorValidation.md']=doc.encode('utf-8')
 for output_path in writes:
  if output_path.exists():digest(output_path)
 result=dict(mode='apply'if args.apply else'preflight',banks=8,cataloged=52,bonus=2,pairs=sum(p['result']['pairs']for p in plans),reports={p['bank']:p['report'].parent.name for p in plans},writes=[p.relative_to(ROOT).as_posix()for p in writes]);print(json.dumps(result,ensure_ascii=True))
 if not args.apply:return
 # Final guards include every input, complete source inventory and the latest-report selection.
 cache.clear()
 for path,value in list(guards.items()):
  if not path.is_file()or digest(path)!=value:raise RuntimeError('Concurrent input change '+str(path))
 if source_files!={p.relative_to(ROOT/'source').as_posix():digest(p)for p in(ROOT/'source').rglob('*')if p.is_file()}:raise RuntimeError('Source inventory changed')
 for p in plans:
  if sorted((GAME/f'resultados-{p["prefix"]}').glob('*/summary.json'))[-1]!=p['report']:raise RuntimeError('New report arrived')
 originals={p:p.read_bytes()if p.exists()else None for p in writes};snapshot=ROOT/'reversing/InteriorEvidence/artifacts'/('publication-'+str(time.time_ns()));snapshot.mkdir();staged={}
 for i,(p,value)in enumerate(writes.items()):
  if originals[p]is not None:(snapshot/f'{i}.before').write_bytes(originals[p])
  p.parent.mkdir(parents=True,exist_ok=True);temporary=p.with_name(p.name+'.interior.tmp');temporary.write_bytes(value);staged[p]=temporary
 (snapshot/'journal.json').write_bytes(blob([dict(path=p.relative_to(ROOT).as_posix(),before_sha256=sha(originals[p])if originals[p]is not None else None,after_sha256=sha(value))for p,value in writes.items()]));done=[]
 try:
  for p,temporary in staged.items():os.replace(temporary,p);done.append(p)
 except Exception:
  for p in reversed(done):
   if originals[p]is None:p.unlink(missing_ok=True)
   else:p.write_bytes(originals[p])
  raise
if __name__=='__main__':main()
