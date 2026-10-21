"""Compuertas del banco CarCtrlRest63B (lote 63, CCarCtrl parte C, tanda 2): 4096 pares por funcion.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio (el resultado en ST0 forma parte del estado x87 comparado), cobertura por familia de las fixtures y cobertura de ramas medida sobre los
eventos de los colaboradores (los falsos registran sus argumentos, en orden). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro
aun no existe o no esta completo).
"""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

NAMES = ['CCarCtrl::SetUpDriverAndPassengersForVehicle', 'CCarCtrl::SetCoordsOfScriptCar', 'CCarCtrl::ScanForPedDanger', 'CCarCtrl::FindMaximumSpeedForThisCarInTraffic']
EXPECTED = {n: 4096 for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x4217C0, NAMES[1]: 0x4342A0, NAMES[2]: 0x42CE40, NAMES[3]: 0x434400}
STUBBED = {
    NAMES[0]: [(0x6D1A50, True), (0x6D1AA0, True), (0x5E4390, True), (0x5E01A0, True), (0x4C5AA0, True), (0x4D3990, True), (0x821B1E, False)],
    NAMES[1]: [(0x569660, True), (0x536BE0, True), (0x463C40, True), (0x486B00, True), (0x6AF420, True), (0x6BEEB0, True), (0x432CB0, True)],
    NAMES[2]: [(0x56E0D0, True), (0x563470, True), (0x425440, True)],
    NAMES[3]: [(0x563470, True), (0x432420, True), (0x425440, True), (0x42D4F0, True)],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}
# Cobertura minima (calibrada con la primera corrida; con margen): [funcion] -> {clave: minimo de casos}
COVER = {
    NAMES[0]: {'driver_give': 800, 'seat_calls': 1500, 'seat_calls_2plus': 600, 'is_car_true_anim': 1500, 'rand_calls_5plus': 450, 'update_stat': 1200, 'passenger_give': 400, 'no_seats': 900},
    NAMES[1]: {'distance': 1200, 'join': 2000, 'no_place': 1000, 'ground_z': 900, 'boat': 300, 'auto': 600, 'bike': 280},
    NAMES[2]: {'list_calls': 2000, 'player_called': 4096, 'multi_sector': 1200, 'many_calls': 300, 'no_list_calls': 400, 'clear_scan': 40},
    NAMES[3]: {'no_list_calls': 900, 'list_calls': 1200, 'multi_sector': 800, 'cars_list': 1000, 'objects_list': 1200, 'many_calls': 700, 'clear_scan': 25},
}


def kinds(row_side):
    return [e[0] for e in row_side['observed'].get('events', [])]


def tally(fn, row, cov):
    ev = row['candidate']['observed'].get('events', [])
    k = [e[0] for e in ev]
    def add(name, cond=True):
        if cond:
            cov.setdefault(name, 0)
            cov[name] += 1
    if fn == NAMES[0]:
        add('driver_give', 2 in k)
        add('seat_calls', 5 in k)
        add('seat_calls_2plus', k.count(5) >= 2)
        add('is_car_true_anim', 4 in k)
        add('van_clamp', 8 in k and k.count(5) <= 1 and 4 in k)
        add('no_seats', 5 not in k)
        add('rand_calls_5plus', k.count(7) >= 5)
        add('update_stat', 6 in k)
        add('passenger_give', k.count(2) >= (2 if 2 in k[:3] else 1) and 6 in k)
    elif fn == NAMES[1]:
        add('ground_z', 23 in k)
        add('distance', 24 in k)
        add('boat', 26 in k and 27 not in k and 28 not in k and 29 not in k)
        add('auto', 27 in k)
        add('bike', 28 in k)
        add('join', 29 in k)
        add('no_place', 26 in k and 29 in k and 27 not in k and 28 not in k)
    elif fn in (NAMES[2], NAMES[3]):
        lists = [e for e in ev if e[0] in (40, 41, 42)]
        add('list_calls', len(lists) > 0)
        add('no_list_calls', len(lists) == 0)
        add('multi_sector', len([e for e in lists if e[0] == 41]) >= 2)
        add('clear_scan', 31 in k)
        add('player_called', 30 in k)
        add('cars_list', 40 in k)
        add('objects_list', 42 in k)
        add('many_calls', len(lists) >= 6)


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    b = [r for r in rows if r['event'] == 'begin']
    if len(b) != 1 or b[0].get('registered_hook') is not True or b[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = b[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if col[fn]['dll_real'] or got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
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
        for side in (row['original'], row['candidate']):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(fn, row, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(4096)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 16 or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        for key, minimum in COVER.get(fn, {}).items():
            if cov[fn].get(key, 0) < minimum:
                raise RuntimeError('Cobertura de ramas insuficiente en %s: %s %d < %d' % (fn, key, cov[fn].get(key, 0), minimum))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('carctrlrest63b', manifest['binaries']['asi'], analyze, 'CarCtrlRest63B', timeout=3600))
