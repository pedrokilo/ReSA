"""Ejecuta las comparaciones de AreAnimsLoaded (fase A) y CreateFirstSubTask/
CreateNextSubTask (fase B) con el runner compartido de la copia de pruebas."""
import json
import sys
from collections import Counter
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case, digest
PROBE_HASH = '1486eeaf27dbaa12e66923b4f88f8c57f16826b8ef4702ec15520cd2f142f368'
TOTAL_B = 412


def analyze(path):
    if not path.exists(): return None
    rows = [json.loads(line) for line in path.read_text().splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('assertion', 'error') for r in rows): raise RuntimeError('Error dentro de GTA; revisar comparisons.jsonl.')
    completed = [r for r in rows if r['event'] == 'complete']
    if not completed: return None
    ready = [r for r in rows if r['event'] == 'player_ready']
    if len(ready) != 1 or not ready[0]['independent_inputs']: raise RuntimeError('Entradas no independientes.')
    a = [r for r in rows if r['event'] == 'comparison' and r['phase'] == 'A']
    b = [r for r in rows if r['event'] == 'comparison' and r['phase'] == 'B']
    num_blocks = ready[0]['num_anim_blocks']
    expected_a = num_blocks + 8 * (3 + sum(1 for i in ready[0]['block_idx'] if i >= 3))
    if len(a) != expected_a or [r['pair'] for r in a] != list(range(expected_a)): raise RuntimeError('Cobertura A incompleta.')
    if len(b) != TOTAL_B or [r['pair'] for r in b] != list(range(TOTAL_B)): raise RuntimeError('Cobertura B incompleta.')
    for r in a:
        if not (r['passed'] and r['equal'] and r['original_esp_delta'] == 0 and r['candidate_esp_delta'] == 0
                and r['original_candidate_calls'] == 0 and r['candidate_calls'] == 1):
            raise RuntimeError('Caso A divergente: ' + str(r['pair']))
    for r in b:
        if not (r['passed'] and r['equal'] and r['original'] == r['candidate']
                and r['original_post_delete'] == r['candidate_post_delete']
                and r['original_candidate_calls'] == 0 and r['candidate_calls'] == 1
                and r['original_anims_candidate_calls'] == 0):
            raise RuntimeError('Caso B divergente: ' + str(r['pair']))
    c = completed[-1]
    if len(completed) != 1 or c['pairs_a'] != expected_a or c['pairs_b'] != TOTAL_B or not c['state_restored']:
        raise RuntimeError('Estado no restaurado o recuento distinto.')
    a_results = Counter(str(r['original']) for r in a)
    b_types = Counter()
    for r in b:
        res = r['original']['result']
        b_types[('null' if res is None else str(r['original']['allocations'][0]['taskType']) if r['original']['allocations'] else 'otro')] += 1
    return dict(pairs_a=expected_a, pairs_b=TOTAL_B, all_equal=True, state_restored=True,
                num_anim_blocks=num_blocks, block_idx=ready[0]['block_idx'], loaded_before=ready[0]['loaded_before'],
                a_results=dict(a_results), b_first_allocation_types=dict(b_types),
                b_say_calls=sum(len(r['original']['say']) for r in b),
                b_keeper_serve=sum(1 for r in b if r['original']['keeperStartServeAnim'] is True),
                log_sha256=digest(path))


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        result = analyze(Path(sys.argv[2]))
        if result is None: raise RuntimeError('Registro incompleto.')
        print(json.dumps(result, indent=2))
    else:
        sys.exit(run_case('interioruseinfo', PROBE_HASH, analyze, 'InteriorManager_c::AreAnimsLoaded + CTaskInteriorUseInfo::CreateFirstSubTask/CreateNextSubTask'))
