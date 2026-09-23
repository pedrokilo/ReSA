"""FxManager comparison using the shared guarded, time-limited runner."""
import itertools
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case, digest
PROBE_HASH = '9815524b4a0519cbcc6aea3c85aa2bdf733bf4558a134d8240158989e301146f'

def analyze(path):
    if not path.exists(): return None
    data = path.read_text(encoding='utf-8')
    rows = [json.loads(line) for line in data.splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('error', 'assertion') for r in rows): raise RuntimeError('Error registrado dentro de GTA.')
    pairs = [r for r in rows if r['event'] == 'comparison']
    for r in pairs:
        if not r['equal'] or not r['independent_inputs'] or not r['same_input']: raise RuntimeError('Diferencia en caso ' + str(r['pair']))
        a, b = r['original'].copy(), r['candidate'].copy()
        if a.pop('candidate_calls') != 0 or b.pop('candidate_calls') != 2 or a != b: raise RuntimeError('Diferencia original/candidato.')
        if a['remaining_systems'] != 0 or not all(a[k] for k in ('head_null', 'tail_null', 'payload_unchanged', 'idempotent')): raise RuntimeError('Estado final incorrecto.')
        if a['entered'] != list(reversed(range(r['count']))) or a['destroyed'] != a['entered']: raise RuntimeError('Orden o numero de destrucciones incorrecto.')
        lists = [a['free'], *a['active']]
        ids = [i for group in lists for i in group['ids']]
        if any(not group['links'] or group['count'] != len(group['ids']) for group in lists): raise RuntimeError('Lista inconsistente.')
        if sorted(ids) != list(range(len(a['owners']))): raise RuntimeError('Particulas perdidas o duplicadas.')
        if any(a['owners'][i] == -2 for i in a['free']['ids']) or any(a['owners'][i] != -2 for group in a['active'] for i in group['ids']): raise RuntimeError('Propiedad de particulas incorrecta.')
    updates = [r for r in rows if r['event'] == 'comparison_update']
    for r in updates:
        a, b = r['original'], r['candidate']
        if not all(r[k] for k in ('equal', 'independent_inputs', 'same_input')) or not a['hook_selected'] or not b['hook_selected'] or a['candidate_calls'] != 0 or b['candidate_calls'] != 3 or a['stages'] != b['stages']: raise RuntimeError('Diferencia en Update ' + str(r['pair']))
        first = [i for i in reversed(range(r['count'])) if r['pattern'] == 0 or (r['pattern'] == 2 and i % 2 == 0)]
        later = [i for i in reversed(range(r['count'])) if i not in first]
        stages = a['stages']
        if len(stages) != 3 or stages[0]['destroyed'] != first or stages[0]['remaining'] != [[i, 3] for i in later] or stages[1]['destroyed'] != first + later or stages[1] != stages[2] or stages[1]['remaining_systems'] != 0: raise RuntimeError('Transicion de vida incorrecta.')
        for stage in stages:
            groups = [stage['free'], *stage['active']]
            ids = [i for group in groups for i in group['ids']]
            if not stage['payload_unchanged'] or any(not g['links'] or g['count'] != len(g['ids']) for g in groups) or sorted(ids) != list(range(len(stage['owners']))): raise RuntimeError('Conservacion de particulas incorrecta en Update.')
            live = {i for i, state in stage['remaining']}
            if any(stage['owners'][i] not in live | {-2} for g in stage['active'] for i in g['ids']) or any(stage['owners'][i] in live | {-2} for i in stage['free']['ids']): raise RuntimeError('Propiedad incorrecta en Update.')
    completed = [r for r in rows if r['event'] == 'complete']
    if not completed: return None
    expected = set(itertools.product((0, 1, 2, 8, 32), range(3), range(3), (False, True), (False, True)))
    actual = {(r['count'], r['mode'], r['density'], r['owned_parent'], r['invalidate_detached_link']) for r in pairs}
    if len(pairs) != 180 or actual != expected or {r['pair'] for r in pairs} != set(range(180)): raise RuntimeError('Cobertura incompleta o duplicada.')
    expected_updates = set(itertools.product((0, 1, 2, 8, 32), range(3), range(3), (False, True), (False, True), (0., .016, .2)))
    actual_updates = {(r['count'], r['pattern'], r['density'], r['owned_parent'], r['invalidate_detached_link'], round(r['step'], 3)) for r in updates}
    if len(updates) != 540 or actual_updates != expected_updates or {r['pair'] for r in updates} != set(range(540)): raise RuntimeError('Cobertura de Update incompleta.')
    if len(completed) != 1 or completed[0]['pairs'] != 180 or completed[0]['update_pairs'] != 540 or not completed[0]['state_restored']: raise RuntimeError('Estado no restaurado.')
    if sum(r['event'] == 'player_ready' for r in rows) != 1: raise RuntimeError('Sin jugador real.')
    return dict(pairs=720, destroy_all_pairs=180, update_pairs=540, all_equal=True, candidate_calls=1980, independent_inputs=True, state_restored=True, destruction_order_equal=True, particle_conservation=True, idempotent=True, systems_destroyed_per_version=sum(r['count'] for r in pairs + updates), log_sha256=digest(path))

if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        result = analyze(Path(sys.argv[2]))
        if result is None: raise RuntimeError('Registro incompleto.')
        print(json.dumps(result, indent=2))
    else: sys.exit(run_case('fxmanager', PROBE_HASH, analyze, 'FxManager_c::DestroyAllFxSystems / Update'))
