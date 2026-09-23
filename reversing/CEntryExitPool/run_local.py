"""Comparacion determinista de la demanda de CEntryExit contra el motor original."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='12a1673274333d01b296e4ec83e1d567180e494222d8193a0dd87acc696dba11'
def analyze(path):
 if not terminal_event(path):return None
 rows=[json.loads(l) for l in path.read_text().splitlines()]
 if any(r['event'] in ('error','assertion') for r in rows):raise RuntimeError('Fallo en el motor; consultar registro')
 pairs=[r for r in rows if r['event']=='comparison'];caps=[r for r in rows if r['event']=='capacity_result'];end=rows[-1]
 if end['event']!='complete' or not end['passed'] or not end['state_restored'] or end['live_capacity']!=512:raise RuntimeError('Banco incompleto o restauracion incorrecta')
 expected={(cap,i) for cap in (400,512) for i in range(469+cap+2)}
 if {(r['capacity'],r['case']) for r in pairs}!=expected or len(pairs)!=len(expected) or end['pairs']!=len(expected):raise RuntimeError('Cobertura incompleta')
 differences=0
 for r in pairs:
  a,b=r['original'].copy(),r['candidate'].copy()
  if r['equal']:
   if a!=b or r['expected_FIX_BUGS_difference']:raise RuntimeError('Igualdad incorrecta')
  else:
   key='return' if a['operation']=='full' else 'failure_return'
   if not r['expected_FIX_BUGS_difference'] or a.pop(key,None)!=0 or b.pop(key,None)!=-1 or a!=b:raise RuntimeError('Diferencia no prevista')
   differences+=1
  for value in (r['original'],r['candidate']):
   op=value['operation']
   if op=='drain' and value['used']!=0:raise RuntimeError('Fuga tras vaciar')
   if op=='refill' and (not value['allocated'] or not value['valid_slot']):raise RuntimeError('Fallo al reutilizar')
   if op=='full' and (not value['unchanged'] or value['used']!=r['capacity']):raise RuntimeError('Mutacion al saturar')
   if op=='delete' and not value['freed']:raise RuntimeError('No libero la plaza')
 if differences!=21:raise RuntimeError('Numero inesperado de diferencias FIX_BUGS')
 if len(caps)!=2 or any(r['original_candidate_calls']!=0 or r['candidate_calls']<=0 or r['original_failures']!=(19 if r['capacity']==400 else 0) or r['candidate_failures']!=r['original_failures'] for r in caps):raise RuntimeError('Control de capacidad o contadores incorrectos')
 return dict(pairs=len(pairs),equal_pairs=len(pairs)-differences,expected_FIX_BUGS_differences=differences,state_restored=True,capacity_results=caps,live_capacity=512,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('entryexit-pool',PROBE_HASH,analyze,'CEntryExit: saturacion y reserva',timeout=180,stop_before_analysis=True,unhook=''))

