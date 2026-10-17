"""Autonomous original/candidate cloud-rendering comparisons in GTA."""
import itertools
import json
import math
import sys
from collections import Counter
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
PROBE_HASH='548c54113a6673f7db9aee8ee55fe38582bf71b36d97c8dad859f6eefee99e50'
HEIGHTS=(-100.,-90.,-89.99,0.,120.,160.,190.,199.99,200.,200.01,250.,400.,800.)

def compare(a,b):
    if isinstance(a,dict):
        if not isinstance(b,dict) or a.keys()!=b.keys():raise ValueError('Object shape differs')
        error=0.
        for k in a:
            if k=='exact':
                if a[k]!=b[k]:raise ValueError('Discrete mismatch')
            else:
                if k=='numbers' and any(not isinstance(x,(float,int)) or not math.isfinite(x) for x in a[k]+b[k]):raise ValueError('Invalid number')
                error=max(error,compare(a[k],b[k]))
        return error
    if isinstance(a,list):
        if not isinstance(b,list) or len(a)!=len(b):raise ValueError('Array shape differs')
        return max((compare(x,y) for x,y in zip(a,b)),default=0.)
    if isinstance(a,float) or isinstance(b,float):
        if not isinstance(a,(float,int)) or not isinstance(b,(float,int)) or not math.isfinite(a) or not math.isfinite(b):raise ValueError('Nonfinite number')
        error=abs(a-b)
        if error!=0:raise ValueError('Numeric mismatch')
        return error
    if a!=b:raise ValueError('Value mismatch')
    return 0.

def analyze(path):
    if not path.exists():return None
    data=path.read_text(encoding='utf-8')
    rows=[json.loads(line) for line in data.splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('error','assertion') for r in rows):raise RuntimeError('Error dentro de GTA; revisar comparisons.jsonl.')
    pairs=[r for r in rows if r['event']=='comparison'];coverage=Counter();max_error=0.
    for r in pairs:
        if not r['equal'] or not r['input_restored'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia en caso '+str(r['pair']))
        max_error=max(max_error,compare(r['original'],r['candidate']),compare(r['draw_original'],r['draw_candidate']))
        coverage.update(c['kind'] for c in r['draw_original'])
        if r['z'] < -90 and r['draw_original']:raise RuntimeError('Salida temprana incorrecta.')
    completed=[r for r in rows if r['event']=='complete']
    if not completed:return None
    expected=set(itertools.product(HEIGHTS,range(2),(0.,.9,1.,127.75,255.),range(3),range(3)))
    actual={(round(r['z'],2),r['xy'],round(r['alpha'],2),r['weather'],r['color']) for r in pairs}
    if len(pairs)!=1170 or actual!=expected or {r['pair'] for r in pairs}!=set(range(1170)):raise RuntimeError('Cobertura incompleta o duplicada.')
    if len(completed)!=1 or completed[0]['pairs']!=1170 or not completed[0]['state_restored']:raise RuntimeError('Estado no restaurado.')
    if sum(r['event']=='player_ready' for r in rows)!=1 or any(coverage[k]<=0 for k in ('geometry','indices','end3d','sprite','draw2d','state')):raise RuntimeError('Falta dibujo real o jugador.')
    ready=next(r for r in rows if r['event']=='player_ready')
    if ready['fp_control'] & 0x30000 != 0x20000:raise RuntimeError('Precision x87 inesperada.')
    return dict(pairs=1170,all_equal=True,candidate_calls=1170,tolerance=0,max_error=max_error,state_restored=True,coverage=dict(coverage),fp_control=ready['fp_control'],log_sha256=digest(path))

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        result=analyze(Path(sys.argv[2]))
        if result is None:raise RuntimeError('Registro incompleto.')
        print(json.dumps(result,indent=2))
    else:sys.exit(run_case('clouds',PROBE_HASH,analyze,'CClouds::RenderBottomFromHeight',timeout=1500))
