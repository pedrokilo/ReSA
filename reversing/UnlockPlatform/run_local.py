import argparse,json,os,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='491b96afcc8e40d50b825d352b762d3bbf64d6c0e8ba50940f53c1fda9865034'
REQUIRE_UNLOCKED=True
COUNTS={2:4,4:16,6:16}
def analyze(path):
 if not terminal_event(path):return None
 rows=[json.loads(line) for line in path.read_text().splitlines()];comparisons=[r for r in rows if r['event']=='comparison'];complete=[r for r in rows if r['event']=='complete'];startup=[r for r in rows if r['event']=='startup'];configured=[r for r in rows if r['event']=='configured'];routes=[r for r in rows if r['event']=='input_route']
 if any(r['event'] in ('error','assertion') for r in rows):raise RuntimeError('Fallo de plataforma; consultar comparisons.jsonl')
 if len(configured)!=1 or len(startup)!=1 or len(complete)!=1:raise RuntimeError('Registro incompleto')
 config=configured[0];start=startup[0];expected=[bool(config['mask']&(1<<i)) for i in range(7)]
 if start['enabled']!=expected or not(start['ps_static'] and start['sdl_attached'] and start['rw_camera']) or start['ps_size']!=40:raise RuntimeError('Estado inicial de plataforma incorrecto')
 if REQUIRE_UNLOCKED and any(config['source_locked']):raise RuntimeError('Persisten bloqueos en las fuentes')
 for i in [3,5]:
  if bool(start['calls'][i])!=expected[i]:raise RuntimeError('No se verifico la implementacion de arranque solicitada')
 if len(comparisons)!=36 or {(r['function'],r['case']) for r in comparisons}!={(f,i) for f,n in COUNTS.items() for i in range(n)}:raise RuntimeError('Cobertura incorrecta')
 for r in comparisons:
  if not r['equal'] or r['original']!=r['candidate'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=(2 if r['function']==2 else 1):raise RuntimeError('Diferencia de comportamiento o contador')
 if len(routes)!=1 or not all(r['passed'] for r in routes) or not complete[0]['state_restored']:raise RuntimeError('Entrada o restauracion incorrecta')
 return dict(pairs=36,pairs_per_function=COUNTS,mask=config['mask'],startup=start,input_routes=routes,all_equal=True,state_restored=True,log_sha256=digest(path))
def main():
 parser=argparse.ArgumentParser();parser.add_argument('--mask',type=int,default=127);a=parser.parse_args();assert 0<=a.mask<=127;os.environ['GTA_REVERSED_PLATFORM_MASK']=str(a.mask)
 return run_case('unlock-platform',PROBE_HASH,analyze,'Plataforma: arranque, menu y entrada',timeout=300,stop_before_analysis=True)
if __name__=='__main__':sys.exit(main())
