"""RpAnimBlend case for the shared runner; no UI or AI services."""
import json
import math
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
PROBE_HASH='8ccf79d6b4c96ec9a120fadfda459e099dd3b42e1fc8adb548c2168ad31354a6'
EXPECTED=652
def analyze(path):
    rows=[]
    if path.exists():
        for line in path.read_text(encoding='utf-8').splitlines(keepends=True):
            if line.endswith('\n'):rows.append(json.loads(line))
    if any(r.get('event') in ('error','assertion') for r in rows):raise RuntimeError('Error registrado dentro de GTA.')
    pairs=[r for r in rows if r.get('event')=='comparison']
    for r in pairs:
        a,b=r['original'],r['candidate']
        if not r.get('independent_inputs') or not r['equal'] or r['max_error']>.00001 or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia en caso '+str(r['pair']))
        if any(a[k]!=b[k] for k in ('exact','callbacks','global_matches')) or not a['global_matches']:raise RuntimeError('Diferencia discreta.')
        if len(a['numbers'])!=len(b['numbers']) or any(not isinstance(x,(float,int)) or not isinstance(y,(float,int)) or not math.isfinite(x) or not math.isfinite(y) or abs(x-y)>.00001 for x,y in zip(a['numbers'],b['numbers'])):raise RuntimeError('Diferencia numerica.')
    completed=[r for r in rows if r.get('event')=='complete']
    if not completed:return None
    expected={(bool(s),bool(c),1,8,0,.016) for s in range(2) for c in range(2)}
    expected|={(bool(s),bool(c),v,m,e,t) for s in range(2) for c in range(2) for e in range(3) if not(c and e==1) for v in range(3) for m in range(8) for t in (0.,.016,.2) if not(not s and e and v==0)}
    actual={(r['skin'],r['compressed'],r['visible'],r['mode'],r['extract'],round(r['step'],3)) for r in pairs}
    if len(completed)!=1 or completed[0]['pairs']!=EXPECTED or not completed[0]['state_restored'] or not completed[0].get('player_pose_unchanged') or len(pairs)!=EXPECTED or {r['pair'] for r in pairs}!=set(range(EXPECTED)) or actual!=expected:raise RuntimeError('Cobertura incompleta o duplicada.')
    if sum(r.get('event')=='player_ready' for r in rows)!=1:raise RuntimeError('Sin jugador real.')
    return dict(pairs=EXPECTED,all_equal=True,tolerance=.00001,max_error=max(r['max_error'] for r in pairs),state_restored=True,player_pose_unchanged=True,independent_inputs=True,log_sha256=digest(path),candidate_calls=EXPECTED)
if __name__=='__main__':
    try:
        if len(sys.argv)==3 and sys.argv[1]=='--analyze':
            result=analyze(Path(sys.argv[2]))
            if result is None:raise RuntimeError('Registro incompleto.')
            print(json.dumps(result,indent=2))
        else:sys.exit(run_case('rpanimblend',PROBE_HASH,analyze,'RpAnimBlendClumpUpdateAnimations'))
    except Exception as error:print('No se pudo completar:',error);sys.exit(1)
