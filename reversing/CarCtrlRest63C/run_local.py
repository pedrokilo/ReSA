"""Compuertas del banco CarCtrlRest63C (lote 63, CCarCtrl parte C, tanda 3): 4096 pares por funcion.

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

NAMES = ['CCarCtrl::PickNextNodeAccordingStrategy', 'CCarCtrl::JoinCarWithRoadSystem', 'CCarCtrl::JoinCarWithRoadSystemGotoCoors']
EXPECTED = {n: 4096 for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x432B10, NAMES[1]: 0x42F5A0, NAMES[2]: 0x42F870}
STUBBED = {
    NAMES[0]: [(0x56E010, True), (0x56E0D0, True), (0x426EF0, False), (0x427740, False), (0x42DE80, False)],
    NAMES[1]: [(0x44FCE0, True), (0x42B470, True)],
    NAMES[2]: [(0x4515D0, True), (0x44E4F0, False), (0x41B950, True), (0x42B470, True), (0x42F5A0, False)],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}
# Cobertura minima (calibrada con la primera corrida; con margen): [funcion] -> {clave: minimo de casos}
COVER = {
    NAMES[0]: {'player_chase': 400, 'target_chase': 400, 'follow_path': 400, 'random': 1000, 'kind_8_9': 600, 'kind_other': 1500, 'negative_mission': 400},
    NAMES[1]: {'early_return': 1000, 'links_called': 1200, 'start_unloaded': 150, 'start_invalid_area': 150, 'zero_forward': 150, 'current_is_start': 350, 'next_is_start': 500},
    NAMES[2]: {'long_path': 1200, 'short_path': 1000, 'vortex': 600, 'bad_modified': 1000, 'same_lane_flag': 1000, 'alias': 150},
}


def kinds(row_side):
    return [e[0] for e in row_side['observed'].get('events', [])]


def tally(fn, row, cov):
    ev = row['candidate']['observed'].get('events', [])
    k = [e[0] for e in ev]
    inp = row['input']
    def add(name, cond=True):
        if cond:
            cov.setdefault(name, 0)
            cov[name] += 1
    if fn == NAMES[0]:
        add('player_chase', 62 in k and k.count(60) == 3)
        add('target_chase', 62 in k and 60 not in k)
        add('follow_path', 63 in k)
        add('random', 64 in k)
        add('kind_8_9', (inp['b1a'][0] >> 4) in (8, 9))
        add('kind_other', (inp['b1a'][0] >> 4) not in (8, 9))
        add('negative_mission', inp['mission'] >= 128)
    elif fn == NAMES[1]:
        add('links_called', 51 in k)
        add('early_return', 51 not in k)
        add('start_invalid_area', inp['start_sel'] == 5)
        add('start_unloaded', inp['start_sel'] == 4)
        add('zero_forward', inp['fwd'][0] in (0, 0x80000000) and inp['fwd'][1] in (0, 0x80000000))
        link_ev = ev[k.index(51)] if 51 in k else None
        def is_start(word):
            return (word & 0xFFFF) == inp['area_ids'][inp['start_slot']] and (word >> 16) == inp['start_node']
        add('current_is_start', link_ev is not None and is_start(link_ev[2]) and not is_start(link_ev[3]))
        add('next_is_start', link_ev is not None and is_start(link_ev[3]) and not is_start(link_ev[2]))
    elif fn == NAMES[2]:
        add('long_path', 72 in k and k.count(72) == 2)
        add('short_path', 74 in k)
        add('vortex', ev[0][0] == 70 and ev[0][19] == 1)
        add('bad_modified', 71 in k and inp['bad_modify'] == 1)
        add('alias', inp['alias'] == 1)
        add('same_lane_flag', ev[0][0] == 70 and ev[0][17] == 1)


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
        sys.exit(runtime_runner.run_case('carctrlrest63c', manifest['binaries']['asi'], analyze, 'CarCtrlRest63C', timeout=3600))
