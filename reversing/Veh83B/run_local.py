"""Compuertas del banco Veh83B (lote 83B): CVehicle::InflictDamage (EXE 0x6D7C90, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: traza de colaboradores y virtuales, campos escritos, jugador del mundo y semilla de rand),
los microtests de FCOMP del hardware (el maximo de 0x420800 depende de `fld 1.0f; fcomp b; test ah, 41h`), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y el
modelo (KEYS con minimo COVER_FLOOR y coverage.json = minimos calibrados).
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

NAMES = ['CVehicle::InflictDamage']
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
FAMILIES = 18
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6D7C90}
# (direccion del EXE, hooked=False): CanVehicleBeDamaged 0x6D1280, GetPercentageProgress 0x5591E0, FindPlayerPed 0x56E210, FindPlayerVehicle 0x56E0D0, IncrementStat 0x55C180, IsPlayer 0x5DF8F0,
# GetVehicleAppearance 0x6D1080, FindTyreNearestPoint 0x6D7BC0, RegisterReference 0x571B70, CleanUpOldReference 0x571A00, CEventGroup::Add 0x4AB420, ReportCrime 0x532010, SetEngineStatus 0x6C22A0 y
# SetWantedLevelNoDrop 0x609F30 (cada uno lleva el mismo falso en el cuerpo de la DLL; sus hooks RH, si los hay, quedan apagados). Los virtuales BlowUpCar (+0xA4) y BurstTyre (+0xB0) se atienden
# desde la vtabla falsa del vehiculo (sin parche de entrada).
STUBBED = {NAMES[0]: [(a, False) for a in (0x6D1280, 0x5591E0, 0x56E210, 0x56E0D0, 0x55C180, 0x5DF8F0, 0x6D1080, 0x6D7BC0, 0x571B70, 0x571A00, 0x4AB420, 0x532010, 0x6C22A0, 0x609F30)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6D7C90, 0x821B1E}
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}

KEYS = ['cannot_damage', 'progress_halved', 'havoc', 'burst_bike', 'burst_car', 'gas_cap', 'aircraft', 'events', 'engine', 'crime', 'detonate', 'blowup', 'wanted',
        'dmg_null', 'dmg_not_ped', 'status_wrecked', 'tyres_dont', 'fp_ped_null_equal', 'can_fire_garbage',
        'health_nan', 'intensity_nan', 'intensity_inf', 'intensity_denormal', 'coord_nan', 'lethal_equal', 'old_250_edge']
FINE = []


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    exp = oracle.expected(inp)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    for tok in exp['path'].split('+'):
        add(tok)
    add('dmg_null', inp['dmg_kind'] == 0)
    add('dmg_not_ped', inp['dmg_kind'] == 2 or (inp['dmg_kind'] == 1 and (inp['dmg_type'] & 7) != 3))
    add('status_wrecked', (inp['status'] & 31) == 5)
    add('tyres_dont', bool(inp['tyres_dont']))
    add('fp_ped_null_equal', inp['dmg_kind'] == 0 and inp['fp_ped'] == 0)
    add('can_fire_garbage', (inp['can_fire'] & 0xFF) not in (0, 1))
    add('health_nan', is_special(inp['health']) and (inp['health'] & 0x7FFFFF) != 0)
    add('intensity_nan', is_special(inp['intensity']) and (inp['intensity'] & 0x7FFFFF) != 0)
    add('intensity_inf', is_special(inp['intensity']) and (inp['intensity'] & 0x7FFFFF) == 0)
    add('intensity_denormal', is_denormal(inp['intensity']))
    add('coord_nan', any(is_special(x) and (x & 0x7FFFFF) != 0 for x in inp['coord']))
    add('lethal_equal', inp['health'] == inp['intensity'])
    add('old_250_edge', inp['health'] == 0x437A0000)


def check_micro(b):
    """los microtests de la x87 del hardware (en `begin`): FSW & 0x4500 de `fld a; fcomp b` frente al orden IEEE"""
    micro = b.get('x87_microtests')
    if not micro or len(micro) < 400:
        raise RuntimeError('Faltan los microtests de la x87')
    wrong = []
    for op, a, c2, result in micro:
        if oracle.is_nan(a) or oracle.is_nan(c2):
            want = 0x4500
        else:
            x, y = oracle.load(a), oracle.load(c2)
            if oracle.greater(x, y):
                want = 0x0000
            elif oracle.greater(y, x):
                want = 0x0100
            else:
                want = 0x4000
        if want != result:
            wrong.append((op, hex(a), hex(c2), hex(result), hex(want)))
    if wrong:
        raise RuntimeError('La x87 del hardware no sigue el modelo del oraculo: %s' % wrong[:5])
    return len(micro)


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
        # los ocho colaboradores tienen hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
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
        bad = oracle.check_row(row)
        if bad:
            raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (bad[0], fn, case, bad[1]))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS}
        need.update({k: 20 for k in FINE})
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['x87_microtests'] = check_micro(b)
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    summary['independent_oracle'] = 'fixture_oracle.py: cada lado de cada par coincide con el modelo'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('veh83b', manifest['binaries']['asi'], analyze, 'Veh83B', timeout=3600))
