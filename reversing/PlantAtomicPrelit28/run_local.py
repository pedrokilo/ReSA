"""Compuertas independientes del banco PlantAtomicPrelit28 (CPlantMgr::AtomicCreatePrelitIfNeeded 0x5DD100), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados,
fp_difference vacio, la salida de CADA lado (traza de los cinco colaboradores de RenderWare sustituidos, hash de la arena de la geometria nueva, punteros de la arena nueva intactos,
arena vieja y atomo sin tocar) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hook, entradas de codigo vigiladas, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o la corrida no ha terminado).
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
HOOKS = {NAME: 0x5DD100}
WATCHED = {0x5DD100, 0x74CA90, 0x74C760, 0x74C6C0, 0x74C800, 0x749D40}
STUBBED = {0x74CA90, 0x74C760, 0x74C6C0, 0x74C800, 0x749D40}

# Minimos de cobertura. Calibrados con `fixture_oracle.py --check-dump` sobre el volcado del generador C++ REAL (dump_fixtures.cpp, estandar, sin juego): el generador es
# determinista, asi que los conteos medidos son exactos; los minimos dejan ~15-35 % de margen.
MINIMUMS = {
    'early': 400, 'not_early': 3000, 'flags_hi': 1000, 'flags_all_ones': 40, 'new_flags_has2': 1700, 'new_flags_no2': 1700,
    'v0': 900, 'v1': 100, 't0': 900, 't1': 550, 'v0_t0': 400, 'v0_t_pos': 450, 'v_pos_t0': 450,
    'v_ge32': 1400, 't_ge32': 1100, 'v_ge64': 750, 't_ge64': 550, 'v_max': 60, 't_max': 60, 'v_not_mult4': 1800, 'mat_ffff': 500,
}
for _k in range(8):
    MINIMUMS['g%d' % _k] = 450
    MINIMUMS['mode%d' % _k] = 450
for _k in range(16):
    MINIMUMS['fam%d' % _k] = 220


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    text = path.read_text(encoding='utf-8')
    rows = [json.loads(line) for line in text.splitlines() if line.endswith('}')]
    if not any(r.get('event') in ('complete', 'error', 'assertion') for r in rows):
        return None                                   # corrida sin terminar
    bad = [r for r in rows if r.get('event') == 'comparison' and not r.get('equal')]
    if bad:
        ex = bad[0]
        keys = sorted(k for k in set(ex['original']) | set(ex['candidate']) if ex['original'].get(k) != ex['candidate'].get(k))
        okeys = sorted(k for k in set(ex['original'].get('observed', {})) | set(ex['candidate'].get('observed', {}))
                       if ex['original'].get('observed', {}).get(k) != ex['candidate'].get('observed', {}).get(k))
        raise RuntimeError('Diferencias EXE/DLL: %d pares; primero %d (grupo %s familia %s), claves %s %s' % (len(bad), ex['pair'], ex['input']['group'], ex['input']['family'], keys, okeys))
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin la marca de colaboradores sustituidos (este banco sustituye 5 funciones de RenderWare)')
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
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s), claves %s, grupo %s familia %s, V=%s T=%s:\n  obtenido %s\n  esperado %s' % (
                    case, label, diff, inp['group'], inp['family'], inp['num_v'], inp['num_t'],
                    {k: str(side['observed'].get(k))[:300] for k in diff}, {k: str(want[k])[:300] for k in diff}))
        for key, value in stats.items():
            if value is True:
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
        sys.exit(runtime_runner.run_case('plantprelit28', manifest['binaries']['asi'], analyze, 'PlantAtomicPrelit28', timeout=1800))
