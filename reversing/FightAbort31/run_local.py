"""Compuertas independientes del banco FightAbort31 (CTaskSimpleFight::MakeAbortable 0x6239F0), 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x6239F0 salte a la DLL solo en el candidato), fp_difference vacio, la salida de CADA lado (AL de retorno, traza de los falsos en orden:
GetEventPriority x1/x2, SetDeleteCallback de la anim y del reposo, BlendAnimation con los bits de los argumentos, IsPlayer, FindTargetOnGround; y todos los cambios de bytes en `this`, las dos
asociaciones, el ped, los datos del jugador y el evento, guardas incluidas) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion
final (hook, entradas vigiladas, x87). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NAME: 0x6239F0}
FN = oracle.NAME
WATCHED = {0x4CEBC0, 0x4D4610, 0x5DF8F0, 0x61C9B0}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    FN: {
        'anim_delta_written': 480,
        'anim_only': 290,
        'anim_released': 980,
        'event_absent': 1200,
        'event_present': 1050,
        'finished_was_1': 570,
        'finished_was_not1': 1700,
        'idle_blend_immediate': 165,
        'idle_blend_urgent': 180,
        'idle_blended': 350,
        'idle_not_blended': 630,
        'idle_only': 280,
        'idle_released': 970,
        'kind_accepted': 1580,
        'kind_idle_priority': 285,
        'kind_rejected_3c': 150,
        'kind_rejected_low': 270,
        'next_was_not1': 2200,
        'ped_not_player': 930,
        'ped_null': 140,
        'player_zeroed': 870,
        'prio_0': 75,
        'prio_1': 1000,
        'prio_2': 1000,
        'prio_high_bit': 115,
        'prio_other': 210,
        'type_1f': 65,
        'type_20': 16,
        'type_3c': 155,
        'type_neg': 95,
    },
}
for _k in range(16):
    MINIMUMS[FN]['family%d' % _k] = 288


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
        raise RuntimeError('Inicio sin hook registrado, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    stubbed = b['collaborators']['stubbed']
    if {w['address'] for w in b['watched_entries']} != WATCHED or {s['exe'] for s in stubbed} != WATCHED or len(stubbed) != 4 or b['collaborators']['exe_real']:
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    if any(not s['dll'] for s in stubbed if s['exe'] != 0x61C9B0) or any(s['dll'] for s in stubbed if s['exe'] == 0x61C9B0):
        raise RuntimeError('Cuerpos de la DLL de los colaboradores inesperados (0x61C9B0 no existe en la DLL; los otros tres si)')
    if b['collaborators']['fake_event_slots'] != 16:
        raise RuntimeError('Vtable falsa inesperada')
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn != FN or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen.add(case)
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
                cov[key] = cov.get(key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov['residue_differs'] = cov.get('residue_differs', 0) + 1
    if seen != set(range(EXPECTED[FN])):
        raise RuntimeError('Cobertura incompleta en ' + FN)
    shortfalls = {}
    for key, m in MINIMUMS[FN].items():
        if cov.get(key, 0) < m:
            shortfalls['%s:%s' % (FN, key)] = (cov.get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, el hook, la entrada vigilada o la ruta no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fightabort31', manifest['binaries']['asi'], analyze, 'FightAbort31', timeout=1800))
