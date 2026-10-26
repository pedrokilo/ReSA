"""Compuertas independientes del banco VehicleWeapons67 (lote 67): CVehicle::ProcessWeapons (0x6E3950), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos sean codigo original del EXE y, si la DLL
tiene hook, que su cuerpo este en el ASI), fp_difference vacio, la salida observada (eventos EN ORDEN de los falsos, cambios de la arena) igual en ambos lados, cobertura de ramas medida
sobre las entradas y los eventos de los falsos con minimos fijos (COVER_FLOOR) y los calibrados en coverage.json, y la restauracion final (hooks, entradas vigiladas, x87).
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

NAME = 'ProcessWeapons'
NAMES = [NAME]
EXPECTED = {NAME: 16384}
FAMILIES = 16
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAME: 0x6E3950}
# colaboradores sustituidos, EN ORDEN (direccion del EXE; hooked=False: su hook queda apagado durante el banco)
_FINDVEH, _CLEAR, _FIRING, _SELECT, _GUNS, _BOMB, _HEAT, _UNGUIDED, _NUMGUNS, _SETMATRIX = 0x56E0D0, 0x742CA0, 0x6E3440, 0x6D4900, 0x6D4D30, 0x6E07E0, 0x6E05C0, 0x6D5110, 0x6D3F30, 0x4AA630
STUBBED = {NAME: [_FINDVEH, _CLEAR, _FIRING, _SELECT, _GUNS, _BOMB, _HEAT, _UNGUIDED, _NUMGUNS, _SETMATRIX]}
# colaboradores con hook RH en la DLL (su cuerpo en el ASI debe estar parcheado: dll=true); GetPlaneWeaponFiringStatus (0x6E3440) y FireHeatSeakingMissile (0x6E05C0) no lo tienen:
# la DLL los llama por la direccion del EXE (dll=false).
DLL_HOOKED = {_FINDVEH, _CLEAR, _SELECT, _GUNS, _BOMB, _UNGUIDED, _NUMGUNS, _SETMATRIX}
WATCHED = set(STUBBED[NAME])
DLL_REAL = []

# claves de cobertura (minimo fijo COVER_FLOOR aunque coverage.json no exista)
KEYS = ['crosshair', 'plane_no_crosshair', 'destroyed', 'player_path', 'remote_path', 'ai_fire', 'ai_idle', 'hydra_lock', 'guns', 'bomb', 'heat', 'unguided', 'heat_player', 'heat_ai_mission',
        'heat_ai_target', 'heat_ai_null', 'tail_heli', 'tail_plane', 'tail_list_null', 'numguns_nonpositive', 'setmatrix', 'setmatrix_no_clump', 'setmatrix_null_frame', 'setmatrix_null_fx_skipped',
        'weapon_out_of_range', 'select_changed_weapon', 'status_not_written', 'ordnance_zero_skip']
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def tally(row, cov):
    inp = row['input']
    v = inp['v']
    ev = row['candidate']['observed']['events']
    k = [e[0] for e in ev]
    sub, phys, stat, f42, model, weapon, mission = v[0], v[1], v[2], v[4], v[5], v[6], v[7]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('crosshair', 2 in k)
    add('plane_no_crosshair', sub == 4 and 1 in k and 2 not in k)
    destroyed = bool(phys & 0x20000000)
    add('destroyed', destroyed)
    add('player_path', 3 in k and stat == 0)
    add('remote_path', 3 in k and stat == 8)
    ai = (not destroyed) and 3 not in k
    add('ai_fire', ai and bool(f42 & 0x10))
    add('ai_idle', ai and not (f42 & 0x10))
    add('hydra_lock', ai and bool(f42 & 0x10) and model == 0x208 and weapon == 3)
    add('guns', 5 in k)
    add('bomb', 6 in k)
    add('heat', 7 in k)
    add('unguided', 8 in k)
    add('heat_player', 7 in k and 3 in k)
    add('heat_ai_mission', 7 in k and 3 not in k and mission in (0x23, 0x3B, 0x3F))
    add('heat_ai_target', 7 in k and 3 not in k and mission not in (0x23, 0x3B, 0x3F))
    add('heat_ai_null', ai and weapon == 3 and v[4] & 0x10 and 7 not in k and 8 not in k and 5 not in k and 6 not in k)
    add('tail_heli', 9 in k and sub == 3)
    add('tail_plane', 9 in k and sub == 4)
    add('tail_list_null', (not destroyed) and sub in (3, 4) and 9 not in k)
    ng = [e for e in ev if e[0] == 9]
    add('numguns_nonpositive', bool(ng) and (v[31] == 0 or v[31] >= 1 << 31))
    sm = [e for e in ev if e[0] == 10]
    add('setmatrix', bool(sm))
    add('setmatrix_no_clump', bool(sm) and v[12] == 0)
    add('setmatrix_null_frame', bool(sm) and v[12] == 1 and v[13] == 0)
    add('setmatrix_null_fx_skipped', bool(ng) and 1 <= v[31] < 1 << 31 and len(sm) < min(v[31], 8))
    add('weapon_out_of_range', (not destroyed) and weapon not in (1, 2, 3, 4) and 3 not in k)
    add('select_changed_weapon', 4 in k and v[29] == 1)
    add('status_not_written', 3 in k and v[24] == 0)
    add('ordnance_zero_skip', (not destroyed) and weapon in (2, 3, 4) and 3 in k and v[26] == 0)


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
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        stubbed = col[fn]['stubbed']
        if col[fn]['dll_real'] != DLL_REAL or [s['exe'] for s in stubbed] != STUBBED[fn] or any(s['hooked'] for s in stubbed) or any(s['dll'] != (s['exe'] in DLL_HOOKED) for s in stubbed):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn != NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
            if side['observed']['nevents'] > 40:
                raise RuntimeError('Demasiados eventos en el caso %d' % case)
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if a['observed'] != c['observed']:
            raise RuntimeError('Salida distinta en el caso %d' % case)
        tally(row, cov)
        cov['family%d' % row['input']['fam']] = cov.get('family%d' % row['input']['fam'], 0) + 1
        cov['stack_seed'] = cov.get('stack_seed', 0) + 1
    if seen != set(range(EXPECTED[NAME])):
        raise RuntimeError('Cobertura incompleta')
    short = {}
    for key in KEYS + ['family%d' % i for i in range(FAMILIES)]:
        need = max(COVER_FLOOR if key in KEYS else 256, COVER.get(key, 0))
        if cov.get(key, 0) < need:
            short[key] = (cov.get(key, 0), need)
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short) + ' medida: ' + json.dumps(cov))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact', 'region_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['region_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = {NAME: cov}
    summary['coverage_method'] = 'serialized_inputs_and_fake_collaborator_events; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('vehicleweapons67', manifest['binaries']['asi'], analyze, 'VehicleWeapons67', timeout=3600))
