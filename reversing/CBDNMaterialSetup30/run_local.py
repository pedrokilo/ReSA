"""Compuertas independientes del banco CBDNMaterialSetup30 (CCustomBuildingDNPipeline::CustomPipeMaterialSetup 0x5D7120), 5120 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados
(route_ok incluye que 0x812140 no este enganchado), fp_difference vacio, la salida de CADA lado (banderas, ranuras, ventana del material, datos de entorno, texturas, datos de MatFX,
guardas y traza del falso de SetFxEnvTexture) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final.

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
HOOKS = {NAME: 0x5D7120}
WATCHED = {0x5D7120, 0x5D9570, 0x812140}
STUBBED = {0x5D9570}

# Minimos de cobertura, calibrados con la autoprueba cruzada (fixture_check.py: Fixtures.hpp real compilado con cl + este oraculo) dejando ~50 % de margen:
# la corrida real usa los desplazamientos de plugin del juego, pero el oraculo no depende de ellos para estas cuentas.
MINIMUMS = {
    'eff2': 1800, 'eff_not2': 750, 'eff_other_nonzero': 550, 'eff_low_bits_2': 70, 'mfx_null': 120,
    'mode0': 330, 'mode1': 420, 'mode2': 280, 'mode3': 150, 'mode4': 65, 'mode5': 200, 'mode6': 320,
    'tex_written': 900, 'tex_null': 250, 'tex_zero': 430, 'shin0': 280, 'has1': 1250, 'has0': 1300,
    'flags_hi_kept': 230, 'ret_null': 600, 'ret_ne_slot': 240, 'slot_changed': 700, 'slot_final_null': 600,
}
for _k in range(10):
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
        raise RuntimeError('Inicio sin hook registrado, o sin la marca de colaboradores sustituidos (este banco sustituye SetFxEnvTexture y ejecuta RpMatFXMaterialGetEffects real)')
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
    if set(col['exe_real']) != {0x812140} or {p['exe'] for p in col['stubbed']} != STUBBED or any(not p['dll'] for p in col['stubbed']):
        raise RuntimeError('Lista de colaboradores inesperada o cuerpo de la DLL sin resolver: %s' % json.dumps(col))
    po = b['plugin_offsets']
    if not (0x14 <= po['env'] < 0x7C and 0x14 <= po['mfx'] < 0x7C and po['env'] % 4 == 0 and po['mfx'] % 4 == 0 and po['env'] != po['mfx']):
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
        if inp['env_off'] != po['env'] or inp['mfx_off'] != po['mfx']:
            raise RuntimeError('Desplazamientos de plugin distintos a los de `begin` en %d' % case)
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
        sys.exit(runtime_runner.run_case('cbdnmatsetup30', manifest['binaries']['asi'], analyze, 'CBDNMaterialSetup30', timeout=1800))
