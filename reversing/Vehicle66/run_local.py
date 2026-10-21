"""Compuertas independientes del banco Vehicle66 (lote 66): RemoveVehicleUpgrade (0x6DF930), AddUpgrade (0x6DFA20), AddVehicleUpgrade (0x6E3290), PossiblyDropFreeFallBombForPlayer (0x6E07E0)
y ProcessSirenAndHorn (0x6E0950), 5 x 4096 = 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos sean codigo original del EXE y su cuerpo de la DLL este en el ASI),
fp_difference vacio, la salida de CADA lado (EAX de AddVehicleUpgrade, eventos EN ORDEN de los falsos, cambios de la arena) igual a la que modela fixture_oracle.py desde el desensamblado,
la cobertura de las fixtures y la restauracion final (hooks, entradas vigiladas, x87, globales).
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
HOOKS = {oracle.NAME_REMOVE: 0x6DF930, oracle.NAME_ADDUP: 0x6DFA20, oracle.NAME_ADDVEH: 0x6E3290, oracle.NAME_DROP: 0x6E07E0, oracle.NAME_SIREN: 0x6E0950}
# colaboradores sustituidos por funcion, EN ORDEN (direccion del EXE; hooked=False: su hook queda apagado durante el banco)
_CLEAR, _FIND, _RREPL, _FORALL = 0x6D3210, 0x4C74D0, 0x6D39E0, 0x749B70
_FRAME, _CREATE, _OBJS, _CHILD, _PLATE = 0x4C53C0, 0x6D3510, 0x7F1200, 0x7F0DC0, 0x6FDFE0
_SETF, _GREPL, _AREPL, _GETUP, _ADDUP = 0x6D30E0, 0x6D3A50, 0x6D3830, 0x6D3650, 0x6DFA20
_RATE, _ORD, _PROJ, _SHAKE, _SIREN, _HORN = 0x6D4590, 0x6D46E0, 0x737C80, 0x53F920, 0x6D8470, 0x53FEE0
STUBBED = {oracle.NAME_REMOVE: [_CLEAR, _FIND, _RREPL, _FORALL],
           oracle.NAME_ADDUP: [_FRAME, _FORALL, _CREATE, _OBJS, _CHILD, _PLATE],
           oracle.NAME_ADDVEH: [_SETF, _FIND, _GREPL, _AREPL, _GETUP, _ADDUP],
           oracle.NAME_DROP: [_RATE, _ORD, _PROJ, _SHAKE],
           oracle.NAME_SIREN: [_SIREN, _HORN]}
# colaboradores con hook RH en la DLL (su cuerpo en el ASI debe estar parcheado: dll=true); los demas los llama la DLL por la direccion del EXE (dll=false).
# AddUpgrade (0x6DFA20) es una de las funciones probadas: su cuerpo es el de su ruta.
DLL_HOOKED = {_CLEAR, _FIND, _RREPL, _FRAME, _CREATE, _PLATE, _SETF, _GREPL, _GETUP, _ADDUP, _RATE, _ORD, _PROJ, _SHAKE, _SIREN, _HORN}
WATCHED = {a for lst in STUBBED.values() for a in lst}
DLL_REAL = ['CPed::IsPlayer 0x5DF8F0', 'CPad::GetPad 0x53FB70']

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~50 % de lo medido.
MINIMUMS = {
    oracle.NAME_REMOVE: {'carmod2': 340, 'cleared': 420, 'linked_atomic': 560, 'linked_none': 540, 'linked_vehdummy': 510, 'main_atomic': 810, 'main_vehdummy': 800, 'slot_multi': 1210, 'slot_none': 150, 'slot_one': 250},
    oracle.NAME_ADDUP: {'alt_kept': 250, 'alt_used': 570, 'frame_null': 520, 'frame_set': 1520, 'parent1': 770, 'plain': 440},
    oracle.NAME_ADDVEH: {'atomic': 780, 'c2_auto_off': 40, 'c2_keep': 40, 'c2_offroad': 100, 'flags_done': 460, 'linked_atomic': 540, 'linked_none': 500, 'linked_vd': 520, 'no_store': 150, 'repl_in_slots': 1530, 'set_writes': 810, 'stored_free': 870, 'vehdummy': 800, 'vehdummy_c2': 360},
    oracle.NAME_DROP: {'blocked': 560, 'checked': 1020, 'driver_none': 360, 'driver_other': 310, 'fired': 1480, 'nan_out': 190, 'null_matrix': 380, 'shake0': 390, 'shake1': 400, 'side0': 520, 'side1': 950, 'type1': 970, 'type_other': 1070, 'wrap': 150},
    oracle.NAME_SIREN: {'arg0_false': 310, 'blocked': 80, 'counter1': 470, 'cur_off': 500, 'cur_on': 910, 'horn_off': 120, 'horn_on': 100, 'idx_over4': 760, 'nosiren': 620, 'siren': 1420, 'toggle': 190, 'zero': 760},
}
for _fn in NAMES:
    for _k in range(oracle.FAMILIES):
        MINIMUMS[_fn]['family%d' % _k] = 256
    MINIMUMS[_fn]['stack_seed'] = 2048


def check_minimums(cov):
    """cobertura medida {funcion: {clave: cuenta}} frente a MINIMUMS: devuelve {clave: (medido, minimo)} de lo que no llega"""
    short = {}
    for fn in NAMES:
        for key, m in MINIMUMS[fn].items():
            if cov[fn].get(key, 0) < m:
                short['%s:%s' % (fn, key)] = (cov[fn].get(key, 0), m)
    return short


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
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        stubbed = col[fn]['stubbed']
        if col[fn]['dll_real'] != DLL_REAL or [s['exe'] for s in stubbed] != STUBBED[fn] or any(s['hooked'] for s in stubbed) or any(s['dll'] != (s['exe'] in DLL_HOOKED) for s in stubbed):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
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
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, side['observed'], want))
        for key, value in stats.items():
            if value is True:
                cov[fn][key] = cov[fn].get(key, 0) + 1
        cov[fn]['family%d' % row['input']['fam']] = cov[fn].get('family%d' % row['input']['fam'], 0) + 1
        cov[fn]['stack_seed'] = cov[fn].get('stack_seed', 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta en ' + fn)
    shortfalls = check_minimums(cov)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact', 'region_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['region_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas o las globales escritas por el banco (ms_modelInfoPtrs[15..19], MI_OFFROAD_WHEEL, historial de bocina del pad 0, CTimer) no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('vehicle66', manifest['binaries']['asi'], analyze, 'Vehicle66', timeout=3600))
