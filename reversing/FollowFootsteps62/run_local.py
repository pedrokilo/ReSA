"""Compuertas independientes del banco FollowFootsteps62 (CTaskComplexFollowPedFootsteps::ControlSubTask 0x695090, CreateSubTask 0x695E40, CreateNextSubTask 0x694EE0,
CreateFirstSubTask 0x695000, MakeAbortable 0x694ED0), 5 x 4096 = 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: los cinco hooks virtuales registrados y con cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada de la funcion salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos con hook (0x5E0170, 0x56A490, 0x6819D0, 0x645700, 0x61A5A0)
y los constructores reales (0x667CD0, 0x62F310, 0x653060, 0x46AC10) sean saltos a la DLL en ambos lados, y 0x61A360 (sin hook) codigo del EXE, antes de instalar los falsos), fp_difference vacio, la salida de
CADA lado (eventos de los falsos EN ORDEN con sus argumentos, cambios en `this`, en la ruta y en la subtarea, objeto construido, EAX) igual a la que modela fixture_oracle.py desde el desensamblado, la
cobertura de las fixtures y la restauracion final (hooks, entradas vigiladas, x87, temporizador global).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
EXPECTED = dict(oracle.CASES_BY_NAME)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {
    'CTaskComplexFollowPedFootsteps::ControlSubTask': 0x695090,
    'CTaskComplexFollowPedFootsteps::CreateSubTask': 0x695E40,
    'CTaskComplexFollowPedFootsteps::CreateNextSubTask': 0x694EE0,
    'CTaskComplexFollowPedFootsteps::CreateFirstSubTask': 0x695000,
    'CTaskComplexFollowPedFootsteps::MakeAbortable': 0x694ED0,
}
STUBBED = (0x5E0170, 0x56A490, 0x6819D0, 0x61A360, 0x645700, 0x61A5A0)
REAL_CTORS = (0x667CD0, 0x62F310, 0x653060, 0x46AC10)
WATCHED = set(STUBBED) | set(REAL_CTORS)

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    'CTaskComplexFollowPedFootsteps::ControlSubTask': {
        'abort_false': 488, 'abort_true': 525, 'dot_nan': 4, 'family0': 256, 'family1': 256, 'family10': 256, 'family11': 256, 'family12': 256, 'family13': 256, 'family14': 256, 'family15': 256, 'family2': 256, 'family3': 256, 'family4': 256, 'family5': 256, 'family6': 256, 'family7': 256, 'family8': 256, 'family9': 256, 'los_ret_even_nonzero': 116, 'nt_1f4': 198, 'nt_384': 62, 'nt_38b': 539, 'nt_516': 174, 'nt_cb': 40, 'p_2d_z': 239, 'p_dead': 49, 'p_far': 128, 'p_first_add': 90, 'p_flag': 285, 'p_hithead': 198, 'p_inveh': 172, 'p_keep': 73, 'p_los_skipped': 830, 'p_los_updated': 542, 'p_move_written': 160, 'p_near_but_stand': 59, 'p_near_return': 58, 'p_near_stand': 40, 'p_normal': 906, 'p_null': 125, 'p_remove': 98, 'p_run': 519, 'p_seek_sub': 131, 'p_stand_goto': 62, 'p_step': 196, 'p_step_skip': 423, 'p_update_point': 150, 'p_wait': 197, 'p_walk': 190, 'ret_sub': 1522, 'route_changed': 604, 'sq_nan': 2, 'this_changed': 1018,
    },
    'CTaskComplexFollowPedFootsteps::CreateSubTask': {
        'family0': 256, 'family1': 256, 'family10': 256, 'family11': 256, 'family12': 256, 'family13': 256, 'family14': 256, 'family15': 256, 'family2': 256, 'family3': 256, 'family4': 256, 'family5': 256, 'family6': 256, 'family7': 256, 'family8': 256, 'family9': 256, 'goto_count_0': 18, 'goto_count_1': 35, 'goto_count_2': 27, 'goto_count_3': 32, 'goto_count_4': 25, 'goto_count_5': 34, 'goto_count_6': 34, 'goto_count_7': 32, 'goto_count_8': 159, 'goto_neg_move': 53, 'kind_0x1f4': 276, 'kind_0x384': 397, 'kind_0x38b': 399, 'kind_0xcb': 271, 'kind_none': 447, 'new_null': 206, 'null_args': 256, 'ret_alloc': 1138, 'route_changed': 397, 'seek_veh': 180, 'seek_walk': 167,
    },
    'CTaskComplexFollowPedFootsteps::CreateNextSubTask': {
        'create_alloc': 523, 'create_null': 523, 'create_value': 1001, 'family0': 256, 'family1': 256, 'family10': 256, 'family11': 256, 'family12': 256, 'family13': 256, 'family14': 256, 'family15': 256, 'family2': 256, 'family3': 256, 'family4': 256, 'family5': 256, 'family6': 256, 'family7': 256, 'family8': 256, 'family9': 256, 'far': 238, 'from_goto': 229, 'from_hit': 173, 'from_none': 166, 'from_seek': 425, 'from_stand': 236, 'near': 187, 'other_type': 690, 'route_empty': 147, 'route_ok': 318, 'sum_nan': 3, 'tgt_null': 128, 'ty_384': 318, 'ty_38b': 238, 'ty_516': 984, 'ty_cb': 507,
    },
    'CTaskComplexFollowPedFootsteps::CreateFirstSubTask': {
        'create_alloc': 523, 'create_null': 523, 'create_value': 1001, 'family0': 256, 'family1': 256, 'family10': 256, 'family11': 256, 'family12': 256, 'family13': 256, 'family14': 256, 'family15': 256, 'family2': 256, 'family3': 256, 'family4': 256, 'family5': 256, 'family6': 256, 'family7': 256, 'family8': 256, 'family9': 256, 'far': 1381, 'near': 539, 'sum_nan': 16, 'tgt_null': 128, 'ty_38b': 1381, 'ty_516': 128, 'ty_cb': 539,
    },
    'CTaskComplexFollowPedFootsteps::MakeAbortable': {
        'ev_null': 678, 'family0': 256, 'family1': 256, 'family10': 256, 'family11': 256, 'family12': 256, 'family13': 256, 'family14': 256, 'family15': 256, 'family2': 256, 'family3': 256, 'family4': 256, 'family5': 256, 'family6': 256, 'family7': 256, 'family8': 256, 'family9': 256, 'ped_null': 509, 'prio_nonstd': 1541, 'ret_false': 979, 'ret_hi_bits': 869, 'ret_noncanon': 1064, 'ret_true': 1068,
    },
}


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(EXPECTED):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    stubbed = col['stubbed']
    if ({w['address'] for w in b['watched_entries']} != WATCHED or col['dll_real'] != list(REAL_CTORS) or [s['exe'] for s in stubbed] != list(STUBBED) or not all(s['dll'] for s in stubbed)
            or [s['hooked'] for s in stubbed] != [True, True, True, False, True, True]):
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    seen = {n: set() for n in NAMES}
    cov = {n: {} for n in NAMES}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        spec = want.pop('alloc_spec', None)
        for label, side in (('EXE', a), ('DLL', c)):
            obs = dict(side['observed'])
            alloc = obs.pop('alloc', None)
            if obs != want:
                diff = [k for k in want if obs.get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, obs, want))
            if spec:
                if alloc is None:
                    raise RuntimeError('Falta el objeto construido en %s %d (%s)' % (fn, case, label))
                oracle.check_alloc(row['input'], alloc, spec, case)
        for key, value in stats.items():
            if value is True:
                cov[fn][key] = cov[fn].get(key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov[fn]['residue_differs'] = cov[fn].get('residue_differs', 0) + 1
    shortfalls = {}
    for fn in NAMES:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        for key, m in MINIMUMS[fn].items():
            if cov[fn].get(key, 0) < m:
                shortfalls['%s:%s' % (fn, key)] = (cov[fn].get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('followfootsteps62', manifest['binaries']['asi'], analyze, 'FollowFootsteps62', timeout=3600))
