"""Compuertas independientes del banco Helpers65C: Fx_c::TriggerWaterHydrant (0x4A0D70), Fx_c::TriggerTankFire (0x4A0FA0), Fx_c::AddTyreBurst (0x49F300), CProjectile::CProjectile (0x5A4030)
y CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB (0x63C840), 5 x 4096 = 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, colaboradores (12) sustituidos, todas las compuertas de ABI/ruta/DF/SEH/
restauracion en AMBOS lados, fp_difference vacio, la salida de CADA lado (traza de colaboradores, matriz M, objeto, tarea, vehiculos, resto de la arena, guardas, globales) igual a la que
modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hooks, entradas de codigo vigiladas, globales del juego, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta terminado).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'seh_chain_restored', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ADDRESS))
NCOLLAB = 12

# Minimos de cobertura, calibrados con fixture_selfcheck.py (volcado compilado de Fixtures.hpp + oraculo) dejando ~35-45 % de margen.
# Claves 'fN_<estadistica>' = por funcion N.
MINIMUMS = {}
for _f in (0, 1, 2):
    MINIMUMS.update({'f%d_skip' % _f: 1200, 'f%d_inrange' % _f: 1250, 'f%d_nan_sum' % _f: 320, 'f%d_inf_sum' % _f: 180, 'f%d_near625' % _f: 460, 'f%d_exact625' % _f: 120,
                     'f%d_just_over' % _f: 150, 'f%d_just_under' % _f: 270, 'f%d_cam_mode0' % _f: 1500, 'f%d_cam_mode1' % _f: 900, 'f%d_create_null' % _f: 800,
                     'f%d_create_nonnull' % _f: 1600, 'f%d_this0' % _f: 1200, 'f%d_this1' % _f: 600, 'f%d_this2' % _f: 600})
MINIMUMS.update({'f3_model_kind%d' % k: 300 for k in range(8)})
MINIMUMS.update({'f3_mode%d' % k: 400 for k in range(6)})
MINIMUMS.update({'f3_off%d' % k: 600 for k in range(4)})
MINIMUMS.update({'f4_veh_null': 300, 'f4_driver_null': 850, 'f4_driver_present': 1300, 'f4_mut_a': 480, 'f4_mut_b': 330, 'f4_mut_c': 330, 'f4_veh_changed': 220, 'f4_veh_changed_ab': 500,
                 'f4_flags_ff': 600})
for _f in range(5):
    MINIMUMS.update({'f%d_fam%d' % (_f, _k): oracle.CASES // 16 for _k in range(16)})


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != 5 * oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 5:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if len(b['collaborators']['exe']) != NCOLLAB or len(b['collaborators']['dll']) != NCOLLAB or not all(b['collaborators']['dll']) or not all(b['collaborators']['exe']):
        raise RuntimeError('Colaboradores sin sustituir')
    lay = b['layout']
    for key, want in (('fx_arena', oracle.LAYOUT['fx_arena']), ('prj_arena', oracle.LAYOUT['prj_arena']), ('roll_arena', oracle.LAYOUT['roll_arena']), ('obj_bytes', oracle.LAYOUT['obj_bytes']),
                      ('task_bytes', oracle.LAYOUT['task_bytes']), ('veh_bytes', oracle.LAYOUT['veh_bytes']), ('hand_bytes', oracle.LAYOUT['hand_bytes'])):
        if lay[key] != want:
            raise RuntimeError('La arena del banco no coincide con las constantes del oraculo: %s' % key)
    dv = b['dll_vptr']
    if not dv.get('in_asi') or dv['vptr'] in (0, 0x867030, oracle.BASE_MARKER):
        raise RuntimeError('vptr de la DLL sin capturar')
    names = {v: k for k, v in oracle.NAME_OF.items()}
    seen = set()
    cov = {}

    def bump(key, n=1):
        if n:
            cov[key] = cov.get(key, 0) + n

    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        if row['function'] not in names:
            raise RuntimeError('Funcion inesperada %s' % row['function'])
        func = names[row['function']]
        key = (func, row['case'])
        if key in seen or row['input']['func'] != func or row['input']['n'] != row['case']:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], row['case']))
        seen.add(key)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (row['function'], row['case'], {k: side.get(k) for k in GATES}))
            if side['observed'].get('guard_ok') is not True or (func != 3 and side['observed'].get('rest_ok') is not True):
                raise RuntimeError('Guardas o resto de la arena alterados en %s %d' % (row['function'], row['case']))
            if func <= 2 and side['observed'].get('globals_unchanged') is not True:
                raise RuntimeError('La funcion altero un global del juego en %s %d' % (row['function'], row['case']))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (row['function'], row['case'], row['fp_difference']))
        want, stats = oracle.expected(row['input'])
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s):\n  obtenido %s\n  esperado %s' % (row['function'], row['case'], label, side['observed'], want))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for k, v in stats.items():
            if k == 'family':
                bump('f%d_fam%d' % (func, v))
            else:
                bump('f%d_%s' % (func, k), bool(v))
    if seen != {(f, n) for f in range(5) for n in range(oracle.CASES)}:
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != 5 * oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87 o las entradas vigiladas no se restauraron')
    if state[0]['globals'] != b['initial_globals']:
        raise RuntimeError('Los globales del juego (camara, humo) no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('helpers65c', manifest['binaries']['asi'], analyze, 'Helpers65C', timeout=1800))
