"""Compuertas del banco Fn709CF0 (caso CASO-CONGELADOS, parte B): CShadows::ShadowRenderTriangleCB (0x709CF0, __usercall del EXE; la version C++ de Shadows.cpp es cdecl), 16384 pares.

No hay hook RH: el EXE se invoca por su direccion con un envoltorio propio del banco (GateUser) y la version C++ se invoca directamente. Ademas de la igualdad EXE/DLL fila a fila
(probe_case.analyzer), exige que la funcion del EXE sea codigo original (sin salto a la DLL), que la candidata este en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: bytes cambiados de las cuatro arenas, eventos de los colaboradores, valor devuelto, x87 vacia),
la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y el modelo (KEYS con minimo).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
"""
import json
import os
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = list(oracle.NAMES)
CASES = int(os.environ.get('FN709CF0_CASES', '16384'))   # 16384 salvo en las rondas de la prueba negativa (el banco lee la misma variable)
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
# colaboradores sustituidos: (direccion del EXE, tiene hook RH, el falso va tambien en la funcion de la DLL)
STUBBED = [(0x7EDD90, False, False), (0x708290, True, True), (0x7077A0, True, True)]
WATCHED = {0x709CF0} | {a for a, _, _ in STUBBED}
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
KEYS = {n: oracle.KEYS[i] for i, n in enumerate(NAMES)}


def tally(inp, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    e = oracle.expected(inp)
    for m in set(e['marks']):
        cov[m] = cov.get(m, 0) + 1


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    b = [r for r in rows if r['event'] == 'begin']
    if len(b) != 1 or b[0].get('registered_hook') is not True or b[0].get('collaborators_stubbed') is not True or b[0].get('no_rh_hook') is not True:
        raise RuntimeError('Inicio sin la ruta EXE/DLL lista, o sin los falsos de los colaboradores')
    b = b[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES) or b['direct_functions'] != NAMES:
        raise RuntimeError('Numero de pares inesperado')
    if not all(b['bodies'].values()) or set(b['bodies']) != set(NAMES):
        raise RuntimeError('Cuerpo de la DLL sin resolver')
    got = [(s['exe'], s['hooked'], s['dll']) for s in b['stubbed']]
    if got != STUBBED:
        raise RuntimeError('Colaboradores sustituidos inesperados: %s' % got)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = set()
    fam = {}
    cov = {}
    fn = NAMES[0]
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != fn or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        for side in (row['original'], row['candidate']):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        if row['original'] != row['candidate']:
            raise RuntimeError('EXE y DLL difieren en %d' % case)
        bad = oracle.check_row(row)
        if bad:
            raise RuntimeError('El oraculo independiente no coincide (%s) en %d: %s' % (bad[0], case, bad[1]))
        f = row['input']['family']
        fam[f] = fam.get(f, 0) + 1
        tally(row['input'], cov)
    if seen != set(range(CASES)):
        raise RuntimeError('Cobertura incompleta')
    short = {k: v for k, v in fam.items() if v < min(200, CASES // FAMILIES)}
    if len(fam) != FAMILIES or short:
        raise RuntimeError('Cobertura por familia insuficiente: %s' % fam)
    need = {k: COVER_FLOOR for k in KEYS[fn]}
    for key, minimum in COVER.get(fn, {}).items():
        need[key] = max(need.get(key, 0), minimum)
    short = {k: (cov.get(k, 0), m) for k, m in need.items() if cov.get(k, 0) < m}
    if short:
        raise RuntimeError('Cobertura de ramas insuficiente: %s medida: %s' % (json.dumps(short), json.dumps(cov)))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = {fn: fam}
    summary['branch_coverage'] = {fn: cov}
    summary['independent_oracle'] = 'fixture_oracle.py: cada lado de cada par coincide con el modelo'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fn709cf0', manifest['binaries']['asi'], analyze, 'Fn709CF0', timeout=3600))
