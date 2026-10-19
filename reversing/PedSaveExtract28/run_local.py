"""Compuertas independientes del banco PedSaveExtract28 (CPedSaveStructure::Extract 0x5D44B0), 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados
(route_ok), fp_difference vacio, la salida de CADA lado (ped: cabecera, vida/armadura, armas, estilo, resto resumido, entrada/salida resuelta, matriz, guardas, estructura guardada sin
tocar, estados del pool sin tocar, traza de los seis colaboradores con su resumen de estado) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las
fixtures y la restauracion final (hook, entradas de codigo vigiladas, x87).
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
EXPECTED = dict(oracle.CASES)
NAME = oracle.NAME
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {NAME: 0x5D44B0}
WATCHED = {0x5D44B0, 0x5E47E0, 0x743C60, 0x4087E0, 0x40EA10, 0x5E6080, 0x5E61F0}
STUBBED = {0x5E47E0, 0x743C60, 0x4087E0, 0x40EA10, 0x5E6080, 0x5E61F0}

# Minimos de cobertura. Calibrados con fixture_mirror.py sobre el volcado del propio Fixtures.hpp compilado aparte (fixture_dump.cpp: mismo generador, no un espejo), con ~45 % de margen.
MINIMUMS = {
    'matrix': 1200, 'no_matrix': 1200,
    'ped_mode0': 250, 'ped_mode1': 250, 'ped_mode2': 1500, 'ped_mode3': 250,
    'mat_mode0': 250, 'mat_mode1': 250, 'mat_mode2': 1500, 'mat_mode3': 250,
    'states_mode0': 250, 'states_mode1': 250, 'states_mode2': 1500, 'states_mode3': 250,
    'wcount0': 200, 'wcount2': 10, 'wcount3': 30, 'wcount4': 100, 'wcount5': 200, 'wcount6': 240, 'wcount7': 270, 'wcount8': 240, 'wcount9': 180, 'wcount10': 150, 'wcount11': 100,
    'wcount12': 50, 'wcount13': 500,
    'm1_none': 1600, 'm1_req': 2000, 'm1_odd': 800, 'm2_none': 1600, 'm2_req': 2000, 'm2_odd': 800,
    'type_gt46': 400, 'type_big': 400, 'ammo_edge': 400,
    'idx_none': 700, 'idx_free': 800, 'idx_used': 800, 'idx_neg_other': 300, 'idx_0': 10, 'idx_2047': 10, 'state_edge': 400,
    'slot_ge128': 60, 'created_ge4': 400, 'style_ge128': 1500,
    'pos_nan': 200, 'pos_inf': 70, 'pos_denormal': 140, 'pos_negzero': 35, 'hp_nan': 130,
    'events_ge20': 2000, 'events_ge50': 1100,
}
for _k in range(16):
    MINIMUMS['fam%d' % _k] = 250


def coverage(inp, stats):
    keys = [k for k, v in stats.items() if v is True]
    keys += ['fam%d' % inp['family'], 'ped_mode%d' % inp['ped_mode'], 'mat_mode%d' % inp['mat_mode'], 'states_mode%d' % inp['states_mode']]
    if stats['events'] >= 50:
        keys.append('events_ge50')
    if stats['events'] >= 20:
        keys.append('events_ge20')
    return keys


def analyze(path):
    if not path.exists():
        return None                                   # el registro aun no existe
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines() if line.endswith('}')]
    if not any(r.get('event') in ('complete', 'error', 'assertion') for r in rows):
        return None                                   # corrida sin terminar
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin la marca de colaboradores sustituidos (este banco sustituye 6 colaboradores)')
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
    if col['exe_real'] or {p['exe'] for p in col['stubbed']} != STUBBED or any(not p['dll'] for p in col['stubbed']):
        raise RuntimeError('Lista de colaboradores inesperada o cuerpo de la DLL sin resolver: %s' % json.dumps(col))
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
        want, stats = oracle.expected(NAME, inp, case)      # RuntimeError = fixture invalido
        for label, side in (('EXE', a), ('DLL', c)):
            diff = oracle.compare(side['observed'], want)
            if diff:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s), claves %s, familia %d:\n  obtenido %s\n  esperado %s' % (
                    case, label, diff, inp['family'], {k: side['observed'].get(k) for k in diff}, {k: want[k] for k in diff}))
        for key in coverage(inp, stats):
            cov[key] = cov.get(key, 0) + 1
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
        sys.exit(runtime_runner.run_case('pedsaveextract28', manifest['binaries']['asi'], analyze, 'PedSaveExtract28', timeout=1800))
