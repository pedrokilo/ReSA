"""Compuertas independientes del banco Dependencies25A (grupo g25a de BRIEF_21): nueve entradas del EXE de nivel "1 Muy facil", 2048 pares cada una (18432).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI (menos la 0x734A70, que no tiene hook y se llama
directamente), colaboradores sustituidos, todas las compuertas de ABI/ruta/x87/trampas en AMBOS lados, fp_difference vacio, la salida de CADA lado igual a la que modela
fixture_oracle.py desde el desensamblado a partir de la fila de entrada, la cobertura de las fixtures de estres y la restauracion final (hooks, entradas de codigo vigiladas,
ms_activeGroups, tabla de refugios, x87). No relaja ninguna compuerta: kCompareDeadX87Registers debe ser true. EAX/ECX/EDX solo se comparan en CTask::Destructor (ahi forman
parte de `observed`); en las demas ningun llamador del EXE los lee (ver g25a-classification.md) y quedan registrados en original_residue/candidate_residue.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = oracle.NAMES
EXPECTED = {name: oracle.CASES for name in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'route_ok', 'traps_zero', 'globals_restored', 'floating_state_restored')
DIRECT = 'RwHelper::GetAnimHierarchyFromFrameCallback'
HOOKS = {'CTask::Destructor': 0x61A3A0, 'CVehiclePool::GetAt': 0x41CC10, 'CAutoPilot::RemoveOnePathNode': 0x41B950, 'CPedGroups::RemoveAllFollowersFromGroup': 0x5FB8A0,
         'CDecisionMakerTypes::FlushDecisionMakerEventResponse': 0x604490, 'CGarages::CountCarsInHideoutGarage': 0x44A210, 'CVehicle::FindReplacementUpgradeCB': 0x6D3490,
         'CAEStreamingDecoder::ScalarDeletingDestructor': 0x4F2860}


def need(counter, key, minimum, label, shortfalls):
    if counter.get(key, 0) < minimum:
        shortfalls[label] = (counter.get(key, 0), minimum)


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    if b['pairs'] != len(NAMES) * oracle.CASES or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()):
        raise RuntimeError('Hooks RH sin registrar: %s' % b['registered'])
    if set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Cuerpo RH sin resolver')
    if b['direct_functions'] != [DIRECT] or not b['direct_dll']:
        raise RuntimeError('La 0x734A70 debe correr como funcion directa de la DLL')
    seen = {name: set() for name in NAMES}
    cov = {name: {} for name in NAMES}

    def bump(name, key, n=1):
        cov[name][key] = cov[name].get(key, 0) + n

    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        name, case = row['function'], row['case']
        if name not in seen or case in seen[name]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (name, case))
        seen[name].add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (name, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (name, case, row['fp_difference']))
        want, stats = oracle.expected(name, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s):\n  obtenido %s\n  esperado %s' % (name, case, label, side['observed'], want))
        ro, rc = row['original_residue'], row['candidate_residue']
        bump(name, 'residue_differs', ro != rc)
        inp = row['input']
        # ------------------------------------------------------------------ cobertura por funcion (calculada desde las entradas y el modelo, no desde la DLL)
        if name == NAMES[0]:
            bump(name, 'offset%d' % inp['offset'])
            bump(name, 'already_base_vtable', inp['object_in'][:8] == '8cd48600')
            bump(name, 'eax_zero', inp['eax'] == 0); bump(name, 'edx_ffff', inp['edx'] == 0xFFFFFFFF)
        elif name == NAMES[1]:
            bump(name, 'null' if stats['null'] else 'nonnull'); bump(name, 'kind%d' % inp['state_kind'])
            bump(name, 'idx0', inp['idx'] == 0); bump(name, 'idx255', inp['idx'] == 255)
            bump(name, 'storage_special', inp['storage'] in (0, 0x1000, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0xFFFFF000, 0x0F000000, 0xC0000A18))
        elif name == NAMES[2]:
            first = oracle.s16((inp['count'] - 1) & 0xFFFF)
            bump(name, 'skipped' if stats['skipped'] else 'ran')
            if 1 <= first <= 8:
                bump(name, 'c%d' % first)
            bump(name, 'ge9', first >= 9); bump(name, 'aliased', stats['aliased']); bump(name, 'big', stats['iterations'] >= 1000)
            bump(name, 'wrapped_zero', inp['count'] == 0)
        elif name == NAMES[3]:
            bump(name, 'called' if stats['called'] else 'not_called'); bump(name, 'in_range' if stats['in_range'] else 'out_of_range')
            bump(name, 'called_out_of_range', stats['called'] and not stats['in_range'])
            if stats['in_range']:
                bump(name, 'id%d' % inp['id'])
        elif name == NAMES[4]:
            bump(name, 'a_in_range', 0 <= inp['a'] < 20); bump(name, 'a_extreme', not 0 <= inp['a'] < 20)
            bump(name, 'b_negative', inp['b'] < 0); bump(name, 'b_large', inp['b'] >= 100)
        elif name == NAMES[5]:
            bump(name, 'eax%d' % stats['count']); bump(name, 'in_table' if stats['in_table'] else 'outside_table')
            seen_bytes = cov[name].setdefault('type_bytes', set()); seen_bytes.add(inp['type_arg'] & 0xFF)
            bump(name, 'high_bits', inp['type_arg'] > 0xFF)
        elif name == NAMES[6]:
            bump(name, stats['branch'])
        elif name == NAMES[7]:
            bump(name, 'nodes%d' % inp['node_count']); bump(name, 'direct_root', stats['direct']); bump(name, 'depth%d' % min(stats['depth'], 3))
            bump(name, 'returned_null' if stats['null'] else 'returned_frame')
            traces = cov[name].setdefault('traces', set()); traces.add(json.dumps(a['observed']['trace']))
        elif name == NAMES[8]:
            bump(name, 'present' if stats['present'] else 'absent'); bump(name, 'bit0' if stats['bit0'] else 'no_bit0')
            if stats['present']:
                bump(name, 'kind%d' % stats['kind'])
            bump(name, 'combo_%d_%d' % (stats['present'], stats['bit0']))
    shortfalls = {}
    for name in NAMES:
        if seen[name] != set(range(oracle.CASES)):
            raise RuntimeError('Cobertura incompleta de ' + name)
    n = NAMES
    for k in range(4):
        need(cov[n[0]], 'offset%d' % k, 400, '%s offset%d' % (n[0], k), shortfalls)
    need(cov[n[0]], 'already_base_vtable', 100, n[0] + ' vtable base previa', shortfalls)
    need(cov[n[0]], 'eax_zero', 80, n[0] + ' eax cero', shortfalls); need(cov[n[0]], 'edx_ffff', 80, n[0] + ' edx ffffffff', shortfalls)
    for key, m in (('null', 300), ('nonnull', 300), ('kind0', 300), ('kind1', 300), ('kind2', 300), ('kind3', 300), ('kind4', 300), ('idx0', 5), ('idx255', 5), ('storage_special', 600)):
        need(cov[n[1]], key, m, '%s %s' % (n[1], key), shortfalls)
    for key, m in (('skipped', 150), ('ran', 1000), ('ge9', 150), ('aliased', 40), ('big', 20), ('wrapped_zero', 40)):
        need(cov[n[2]], key, m, '%s %s' % (n[2], key), shortfalls)
    for k in range(1, 9):
        need(cov[n[2]], 'c%d' % k, 40, '%s c%d' % (n[2], k), shortfalls)
    for key, m in (('called', 600), ('not_called', 400), ('in_range', 1200), ('out_of_range', 100), ('called_out_of_range', 40)):
        need(cov[n[3]], key, m, '%s %s' % (n[3], key), shortfalls)
    for k in range(8):
        need(cov[n[3]], 'id%d' % k, 100, '%s id%d' % (n[3], k), shortfalls)
    for key, m in (('a_in_range', 1200), ('a_extreme', 300), ('b_negative', 150), ('b_large', 100)):
        need(cov[n[4]], key, m, '%s %s' % (n[4], key), shortfalls)
    for k in range(5):
        need(cov[n[5]], 'eax%d' % k, 80, '%s eax%d' % (n[5], k), shortfalls)
    need(cov[n[5]], 'in_table', 1500, n[5] + ' in_table', shortfalls); need(cov[n[5]], 'outside_table', 100, n[5] + ' outside_table', shortfalls)
    if len(cov[n[5]].get('type_bytes', ())) != 256:
        shortfalls[n[5] + ' bytes bajos del tipo'] = (len(cov[n[5]].get('type_bytes', ())), 256)
    need(cov[n[5]], 'high_bits', 1500, n[5] + ' bits altos del argumento', shortfalls)
    for key, m in (('not_atomic', 300), ('upgrade_bit', 300), ('index_minus_one', 150), ('stored', 300)):
        need(cov[n[6]], key, m, '%s %s' % (n[6], key), shortfalls)
    for k in range(1, 9):
        need(cov[n[7]], 'nodes%d' % k, 100, '%s nodes%d' % (n[7], k), shortfalls)
    for key, m in (('direct_root', 300), ('depth0', 300), ('depth1', 300), ('depth2', 100), ('depth3', 20), ('returned_null', 300), ('returned_frame', 300)):
        need(cov[n[7]], key, m, '%s %s' % (n[7], key), shortfalls)
    if len(cov[n[7]].get('traces', ())) < 150:
        shortfalls[n[7] + ' trazas distintas'] = (len(cov[n[7]].get('traces', ())), 150)
    for key, m in (('present', 1200), ('absent', 300), ('bit0', 800), ('no_bit0', 800), ('kind0', 100), ('kind1', 100), ('kind2', 100), ('kind3', 100),
                   ('combo_0_0', 80), ('combo_0_1', 80), ('combo_1_0', 400), ('combo_1_1', 400)):
        need(cov[n[8]], key, m, '%s %s' % (n[8], key), shortfalls)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != len(NAMES) * oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['active_groups'] != b['initial_active_groups'] or state[0]['safehouse_hash'] != b['initial_safehouse_hash']
            or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True):
        raise RuntimeError('ms_activeGroups, la tabla de refugios, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = True
    summary['coverage'] = {name: {k: (len(v) if isinstance(v, set) else v) for k, v in cov[name].items()} for name in NAMES}
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies25a', manifest['binaries']['asi'], analyze, 'Dependencies25A', timeout=1800))
