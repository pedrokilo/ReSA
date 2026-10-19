"""Compuertas independientes del banco CBDNRenderCB30 (CCustomBuildingDNPipeline::CustomPipeRenderCB 0x5D6480), 6144 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados
(route_ok incluye que el _ftol2 del EXE 0x821B40 no este enganchado), fp_difference vacio, la salida de CADA lado (traza completa de los doce colaboradores sustituidos y de
fpRenderStateSet, matriz de textura global 0xC02C28 y memoria intacta con guardas) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la
restauracion final (hook, entradas de codigo vigiladas, tabla del motor, matriz, x87).

`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe).
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
EXPECTED = dict(oracle.CASES)
NAME = oracle.NAME
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {NAME: 0x5D6480}
STUBBED = {0x756D90, 0x7FC320, 0x7FDE70, 0x7FC2D0, 0x7FC340, 0x7FA1C0, 0x7FA090, 0x7F9F70, 0x7FE0A0, 0x7FC4D0, 0x5DA640, 0x5DA6A0}
WATCHED = STUBBED | {0x5D6480, 0x821B40}

# Minimos de cobertura, calibrados con la autoprueba cruzada (fixture_check.py: Fixtures.hpp real compilado con cl + este oraculo) dejando ~50 % de margen:
# la corrida real usa los desplazamientos de plugin del juego, pero el oraculo no depende de ellos para estas cuentas.
MINIMUMS = {
    'alpha_color': 700, 'alpha_opaque': 330, 'alpha_vertex': 1800, 'cover_env': 1300, 'env_mesh': 1300, 'env_meshes_ge2': 700, 'env_off_mesh': 1300,
    'env_off_slot_garbage': 600, 'env_off_slot_null': 450, 'env_off_slot_valid': 450, 'flags_hi_prelit_clear': 850, 'decl_null': 750, 'ib': 1500, 'no_ib': 1500,
    'light_low_byte_zero_nonzero': 550, 'light_on': 1900, 'light_zero_prelit': 620, 'nm1': 500, 'nm2': 500, 'nm3': 500, 'nm4': 500, 'nm_int_min': 500, 'nm_le0': 1000, 'nm_neg': 500,
    'nolight': 500, 'obj_null': 300, 'obj_raw': 300, 'render_lighting': 1700, 'render_nolight': 330, 'shin0': 130, 'shin_max': 125, 'surf_props': 1250, 'sx_neg': 700, 'sy_neg': 650,
    'type_garbage_hi': 1450,
}
for _k in range(12):
    MINIMUMS['g%d' % _k] = 500


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    text = path.read_text(encoding='utf-8')
    rows = [json.loads(line) for line in text.splitlines() if line.endswith('}')]
    if not any(r.get('event') in ('complete', 'error', 'assertion') for r in rows):
        return None                                   # corrida sin terminar
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin la marca de colaboradores sustituidos (este banco sustituye doce colaboradores y fpRenderStateSet; el _ftol2 del EXE se ejecuta de verdad)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    col = b['collaborators']
    if set(col['exe_real']) != {0x821B40} or {p['exe'] for p in col['stubbed']} != STUBBED or any(p['exe'] == 0x756D90 and not p['dll'] for p in col['stubbed']):
        raise RuntimeError('Lista de colaboradores inesperada o cuerpo de la DLL sin resolver: %s' % json.dumps(col))
    po = b['plugin_offsets']
    if not (0x14 <= po['env'] < 0x7C and po['env'] % 4 == 0):
        raise RuntimeError('Desplazamientos de plugin inesperados: %s' % json.dumps(po))
    seen, cov = set(), {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        inp = row['input']
        if inp['env_off'] != po['env']:
            raise RuntimeError('Desplazamiento de plugin distinto al de `begin` en %d' % case)
        want, stats = oracle.expected(NAME, inp, case)      # RuntimeError = fixture invalido
        for label, side in (('EXE', a), ('DLL', c)):
            diff = oracle.compare(side['observed'], want)
            if diff:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s), claves %s:\n  obtenido %s\n  esperado %s' % (
                    case, label, diff, {k: side['observed'].get(k) for k in diff}, {k: want[k] for k in diff}))
        for key, value in stats.items():
            if value is True:
                cov[key] = cov.get(key, 0) + 1
        cov['g%d' % inp['group']] = cov.get('g%d' % inp['group'], 0) + 1
        cov['residue_differs'] = cov.get('residue_differs', 0) + (row['original_residue'] != row['candidate_residue'])
    if seen != set(range(EXPECTED[NAME])):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, el hook o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('cbdnrendercb30', manifest['binaries']['asi'], analyze, 'CBDNRenderCB30', timeout=1800))
