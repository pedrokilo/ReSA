"""Compuertas del banco CrosshairRender74 (lote 74): CWeaponEffects::RenderPlayerCrosshair (EXE 0x56EF90, hook S), 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CWeaponEffects::RenderPlayerCrosshair']
CASES = 20480   # 20 familias x 1024
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x56EF90}
# (direccion del EXE, hooked): CSprite::RenderOneXLUSprite_Rotate_Aspect (hooked=False: el hook queda apagado, la entrada del EXE es codigo original); ademas se sustituye la entrada fpRenderStateSet de la tabla de RenderWare
STUBBED = {NAMES[0]: [(0x70D490, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x56EF90}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def tally(inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    active = (inp['active'] & 0xFF) != 0

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('inactive', not active)
    add('inactive_garbage_hi', not active and (inp['active'] >> 8) != 0)
    add('active', active)
    add('active_byte_gt1', active and (inp['active'] & 0xFF) > 1)
    if not active:
        return
    add('player0', inp['player'] == 0)
    add('player1', inp['player'] == 1)
    add('sprites_60', sum(1 for e in events if e[0] == 2) == 60)
    sprites = [e for e in events if e[0] == 2]
    add('sprite_nan_x', any(is_special(e[1]) and (e[1] & 0x7FFFFF) for e in sprites))
    add('sprite_nan_y', any(is_special(e[2]) and (e[2] & 0x7FFFFF) for e in sprites))
    add('sprite_inf', any(is_special(e[1]) and not (e[1] & 0x7FFFFF) or is_special(e[2]) and not (e[2] & 0x7FFFFF) for e in sprites))
    add('sprite_denormal', any(is_denormal(e[1]) or is_denormal(e[2]) for e in sprites))
    add('sprite_negzero', any(e[1] == 0x80000000 or e[2] == 0x80000000 for e in sprites))
    X = [inp['hx%d' % i] for i in range(10)] + [inp['hy%d' % i] for i in range(10)] + [inp['tx'], inp['ty']]
    add('hist_special', any(is_special(v) for v in X))
    add('hist_denormal', any(is_denormal(v) for v in X))
    add('hist_nan_noncanonical', any(noncanonical_nan(v) for v in X))
    add('hist_negzero', any(v == 0x80000000 for v in X))
    add('hist_minus_one', any(v == 0xBF800000 for v in X))
    add('cross_special', is_special(inp['tx']) or is_special(inp['ty']))
    add('size_zero', inp['width'] == 0 or inp['height'] == 0)
    add('size_negative', inp['width'] & 0x80000000 or inp['height'] & 0x80000000)
    add('size_int_min', inp['width'] == 0x80000000 or inp['height'] == 0x80000000)
    add('time_hi_garbage', (inp['time'] >> 10) != 0)
    add('time_bit31', inp['time'] >> 31 == 1)
    add('time_low_zero', (inp['time'] & 0x3FF) == 0)
    add('tex_idx_0', inp['tex_idx'] == 0)
    add('tex_idx_31', inp['tex_idx'] == 31)
    add('tex_idx_other', 0 < inp['tex_idx'] < 31)
    add('raster_zero', inp['raster'] == 0)


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
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        if any(s['dll'] is not True for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
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
        if row['original'] != row['candidate']:
            raise RuntimeError('EXE y DLL difieren en %s %d' % (fn, case))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 20 or short:
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
        sys.exit(runtime_runner.run_case('crosshairrender74', manifest['binaries']['asi'], analyze, 'CrosshairRender74', timeout=3600))
