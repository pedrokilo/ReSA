"""Compuertas del banco ForcePedOut71 (lote 71): constructor de CTaskSimpleCarForcePedOut (EXE 0x647710, thiscall (vehiculo, puerta), `ret 8`, hook S), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

NAMES = ['CTaskSimpleCarForcePedOut::Constructor']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x647710}
# (direccion del EXE, hooked): CTaskSimple::CTaskSimple (sin hook ni cuerpo en la DLL: solo EXE), CEntity::RegisterReference
STUBBED = {NAMES[0]: [(0x61A390, False), (0x571B70, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x647710}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def is_denormal_value(v):
    return v == v and v not in (float('inf'), float('-inf')) and v != 0 and abs(v) < 1.1754943508222875e-38


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y las observaciones"""
    ev = obs['events']
    veh, door = inp['veh_kind'], inp['door']

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('veh_null', veh == 0 and not ev)
    add('veh_arena', veh == 1 and len(ev) == 1)
    add('veh_raw', veh == 2 and len(ev) == 1)
    add('regref_called', len(ev) == 1)
    add('regref_skipped', len(ev) == 0)
    add('regref_ref_is_field_8', len(ev) == 1 and ev[0][2] == 0x10000000 + 0x40 + 8)
    add('veh_arena_offset', veh == 1 and inp['veh_off'] != 0)
    add('door_extreme', door >= 0x80000000 or door in (0, 0xFF, 0x100))
    add('door_valid', door < 10)
    add('prefill_seed', inp['prefill'] == 0)
    add('prefill_cd', inp['prefill'] == 1)
    add('prefill_zero', inp['prefill'] == 2)
    add('prefill_ff', inp['prefill'] == 3)
    add('ret_is_this', obs['eax'] == 0x10000000 + 0x40)
    add('type_0x32f', obs.get('type') == 0x32F)
    add('vptr_ok', obs['vptr_ok'] is True)
    add('base_ctor_ok', obs['base_ctor_ok'] is True)
    add('eax_seed_special', inp['eax'] in (0, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 1, 0xFFFFFF00, 0xFF, 0xDEADBEEF))
    add('edx_seed_special', inp['edx'] in (0, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 1, 0xFFFFFF00, 0xFF, 0xDEADBEEF))


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
        if any(s['hooked'] != (s['dll'] != 0) for s in col[fn]['stubbed']):
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
        tally(row['input'], obs, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
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
        sys.exit(runtime_runner.run_case('forcepedout71', manifest['binaries']['asi'], analyze, 'ForcePedOut71', timeout=3600))
