"""Run CEntryExit::TransitionFinished comparisons with the shared test-copy runner."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
PROBE_HASH='48207c1f405c8448e41d28897a6bfbf44afd0e11f6257c3e09096a11acf7bbb3'
HEAVY_LIGHT_PAIRS=8

SCENARIO_COUNT,SUBFLAG_COUNT,WARP_COUNT,AREA_COUNT,POS_COUNT=5,3,2,8,4
TOTAL=SCENARIO_COUNT*SUBFLAG_COUNT*WARP_COUNT*AREA_COUNT*POS_COUNT

def analyze(path):
    if not path.exists():return None
    rows=[json.loads(line) for line in path.read_text().splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('assertion','error') for r in rows):raise RuntimeError('Error dentro de GTA; revisar comparisons.jsonl.')
    completed=[r for r in rows if r['event']=='complete']
    if not completed:return None
    pairs=[r for r in rows if r['event']=='comparison']
    if len(pairs)!=TOTAL or {r['pair'] for r in pairs}!=set(range(TOTAL)):raise RuntimeError('Cobertura incompleta o duplicada.')
    ready=[r for r in rows if r['event']=='player_ready']
    if len(ready)!=1 or not ready[0]['independent_inputs']:raise RuntimeError('Entradas no independientes.')
    by_scenario={}
    for r in pairs:
        if not r['equal'] or not r['independent_inputs'] or not r['inputs_unchanged'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:
            raise RuntimeError('Caso divergente: '+str(r['pair']))
        by_scenario.setdefault(r['scenario'],0)
        by_scenario[r['scenario']]+=1
    if len(completed)!=1 or completed[0]['pairs']!=TOTAL or not completed[0]['state_restored']:raise RuntimeError('Estado no restaurado.')

    precond=[r for r in rows if r['event']=='heavy_precondition']
    if len(precond)!=1:raise RuntimeError('Precondición de la fase 2 ausente o duplicada.')
    heavy_complete=[r for r in rows if r['event']=='heavy_complete']
    if len(heavy_complete)!=1:raise RuntimeError('Cierre de la fase 2 ausente o duplicado.')
    heavy=heavy_complete[0]
    if not heavy['attempted']:
        return dict(pairs=TOTAL,by_scenario=by_scenario,all_equal=True,candidate_calls=TOTAL,state_restored=True,
                    log_sha256=digest(path),heavy_attempted=False,heavy_pairs=0,
                    heavy_skip_reason=precond[0])
    if not heavy['player_restored']:raise RuntimeError('El jugador no quedo restaurado tras la fase 2.')
    heavy_pairs_rows=[r for r in rows if r['event']=='heavy_comparison']
    if heavy['pairs']!=len(heavy_pairs_rows) or heavy['pairs']!=HEAVY_LIGHT_PAIRS+1:raise RuntimeError('Cobertura de la fase 2 incompleta.')
    light=[r for r in heavy_pairs_rows if r['sub_case']=='light_early_return']
    full=[r for r in heavy_pairs_rows if r['sub_case']=='full_completion']
    if len(light)!=HEAVY_LIGHT_PAIRS or len(full)!=1:raise RuntimeError('Faltan subcasos de la fase 2.')
    for r in heavy_pairs_rows:
        if not r['equal'] or r['original_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Caso divergente en fase 2: '+str(r))
    if completed[0]['heavy_attempted'] is not True or completed[0]['heavy_pairs']!=heavy['pairs']:raise RuntimeError('Resumen final inconsistente con la fase 2.')
    return dict(pairs=TOTAL,by_scenario=by_scenario,all_equal=True,candidate_calls=TOTAL,state_restored=True,
                log_sha256=digest(path),heavy_attempted=True,heavy_pairs=heavy['pairs'],
                heavy_light_pairs=len(light),heavy_full_pairs=len(full),heavy_player_restored=True)

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        result=analyze(Path(sys.argv[2]))
        if result is None:raise RuntimeError('Registro incompleto.')
        print(json.dumps(result,indent=2))
    else:sys.exit(run_case('enextransition',PROBE_HASH,analyze,'CEntryExit::TransitionFinished'))
