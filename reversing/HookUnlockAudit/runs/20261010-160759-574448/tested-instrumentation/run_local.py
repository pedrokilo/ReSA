"""Verifica inventario completo, pares, rutas reales y fotogramas tras conmutar."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer
HERE=Path(__file__).resolve().parent
base=analyzer({'CEntryExit::GetPositionRelativeToOutsideWorld':2048})
def analyze(path):
    result=base(path)
    if result is None:return None
    rows=[json.loads(s) for s in path.read_text().splitlines()]
    reviews=[r for r in rows if r['event']=='catalog_lock_review']
    if len(reviews)!=421 or len({(r['class'],r['fn_name']) for r in reviews})!=421:
        raise RuntimeError('Inventario de bloqueos incompleto o duplicado')
    if not all(r['lock_refusal_ok'] for r in reviews):raise RuntimeError('Un bloqueo no restringe la conmutación')
    frames=[r for r in rows if r['event']=='live_frame']
    if len(frames)!=240 or not all(r['route_ok'] and r['result']['globals_restored'] for r in frames):
        raise RuntimeError('Prueba entre fotogramas incompleta')
    if any(r['event'].startswith('first_chance_av') for r in rows):raise RuntimeError('Acceso inválido durante la prueba')
    if any(r['original'] is None or r['candidate'] is None for r in rows if r['event']=='comparison'):
        raise RuntimeError('Resultado par nulo')
    result.update(reviewed_catalog_locks=421,runtime_locked_in_review=sum(r['runtime_locked'] for r in reviews),
        live_frames=240,passed=True,posteffects_passed=False)
    post=[r for r in rows if r['event']=='posteffects_live_frame']
    result['posteffects_passed']=len(post)==240 and all(r['checks'].get('passed') is True and r['checks'].get('state_restored') is True for r in post)
    result['posteffects_live_frames']=len(post)
    checks=[r for r in rows if r['event']=='posteffects_live_checks']+[r['checks'] for r in post]
    samples=[c for r in checks for c in r.get('comparisons',[])]
    result['posteffects_samples']=len(samples)
    result['posteffects_passed']&=len(samples)==1446 and all(c.get('original') is not None and c.get('candidate') is not None and c.get('passed') is True and c.get('route_ok') is True for c in samples)
    return result
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('hookunlock',manifest['binaries']['asi'],analyze,
            'Auditoría de bloqueos y conmutación CEntryExit',timeout=900))
