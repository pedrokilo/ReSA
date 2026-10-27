"""Compuertas del banco FightHitObj72 (lote 72): CTaskSimpleFight::FightHitObj (EXE 0x61D400, hook S), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CTaskSimpleFight::FightHitObj']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x61D400}
# (direccion del EXE, hooked; las dos de audio van sin hook por --unhook=Audio): CTaskSimpleFight::FightStrikeDamage, CPhysical::AddToMovingList, CPhysical::ApplyForce, CObject::ObjectDamage, CAEPedWeaponAudioEntity::AddAudioEvent,
# CAEPedAudioEntity::AddAudioEvent, Fx_c::AddPunchImpact
STUBBED = {NAMES[0]: [(0x61C740, True), (0x542800, True), (0x542B50, True), (0x5A0D90, True), (0x4E69F0, False), (0x4E2BB0, False), (0x49F670, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x61D400}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
EV_DAMAGE, EV_STATIC, EV_MOVING, EV_APPLY, EV_OBJDAMAGE, EV_WEAPON_AE, EV_PED_AE, EV_PUNCH = 1, 2, 3, 4, 5, 6, 7, 8


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def is_denormal_bits(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def tally(inp, events, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    k = [e[0] for e in events]
    live = inp['effect'] < 0xC8 and inp['phys_flags'] & 4 == 0

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('damage_called_once', k.count(EV_DAMAGE) == 1)
    add('gate_effect_fail', inp['effect'] >= 0xC8)
    add('gate_physical_fail', inp['phys_flags'] & 4 != 0 and inp['effect'] < 0xC8)
    add('gate_mult_fail', live and not f32(inp['mult']) < 99.9)
    add('mult_nan', live and is_nan(inp['mult']))
    add('mult_inf', live and is_inf(inp['mult']))
    add('mult_near_threshold', live and abs(inp['mult'] - 0x42C7CCCD) <= 3)
    add('inner_set_static', EV_STATIC in k)
    add('inner_moving_list', EV_MOVING in k)
    add('inner_flags_4', EV_STATIC in k and inp['flags'] & 4 != 0)
    add('inner_flags_40000', EV_STATIC in k and inp['flags'] & 0x40000 != 0 and inp['flags'] & 4 == 0)
    add('inner_uproot_not_met', live and f32(inp['mult']) < 99.9 and (inp['flags'] & 0x40004) != 0 and EV_STATIC not in k)
    add('inner_uproot_nan', live and is_nan(inp['uproot']) and (inp['flags'] & 0x40004) != 0 and EV_STATIC not in k)
    add('inner_uproot_zero', (inp['uproot'] & 0x7FFFFFFF) == 0 and EV_STATIC in k)
    add('force_applied', EV_APPLY in k)
    add('flags_reread_blocks_force', inp['mut_flags'] != 0 and EV_STATIC in k and (inp['flags_after'] & 0x40004) != 0 and EV_APPLY not in k)
    add('flags_reread_enables_force', inp['mut_flags'] != 0 and EV_STATIC in k and (inp['flags_after'] & 0x40004) == 0 and EV_APPLY in k)
    add('force_matrix', EV_APPLY in k and inp['obj_mat'] == 1)
    add('force_placement', EV_APPLY in k and inp['obj_mat'] == 0)
    add('force_low_factor', EV_APPLY in k and inp['phys_flags'] & 0x80 != 0)
    add('force_high_factor', EV_APPLY in k and inp['phys_flags'] & 0x80 == 0)
    ap = [e for e in events if e[0] == EV_APPLY]
    if ap:
        a = ap[0]
        add('force_denormal_component', any(is_denormal_bits(v) for v in a[2:8]))
        add('force_nan_component', any(is_nan(v) for v in a[2:8]))
        add('force_inf_component', any(is_inf(v) for v in a[2:8]))
    add('damage_nan', is_nan(inp['damage']))
    add('damage_inf', is_inf(inp['damage']))
    add('damage_denormal', is_denormal_bits(inp['damage']))
    add('objdamage_called_once', k.count(EV_OBJDAMAGE) == 1)
    od = [e for e in events if e[0] == EV_OBJDAMAGE]
    if od:
        add('objdamage_dir_is_old_copy', inp['mut_normal'] == 1 and od[0][8:11] == [inp['n2_x'], inp['n2_y'], inp['n2_z']])
        add('objdamage_origin_is_new_posn1', inp['mut_posn1'] == 1 and od[0][4:7] == [inp['mutp_x'], inp['mutp_y'], inp['mutp_z']])
    add('chainsaw_audio', EV_WEAPON_AE in k)
    add('chainsaw_only_after_mutation', EV_WEAPON_AE in k and inp['mut_weapon'] != 0 and inp['weapon_type'] != 9)
    add('chainsaw_lost_after_mutation', EV_WEAPON_AE not in k and inp['mut_weapon'] == 1 and inp['weapon_type'] == 9 and (inp['slot'] & 0xFF) <= 12)
    add('no_chainsaw_audio', EV_WEAPON_AE not in k)
    add('slot_out_of_range', (inp['slot'] & 0xFF) > 12 and (inp['slot'] & 0x80) == 0)
    add('slot_negative', (inp['slot'] & 0x80) != 0)
    add('ped_audio_event', k.count(EV_PED_AE) == 1)
    add('punch_called', k.count(EV_PUNCH) == 1)
    add('mut_normal', inp['mut_normal'] == 1)
    add('mut_posn1', inp['mut_posn1'] == 1)
    add('surface_high', inp['surface'] >= 0x80)
    add('combo_negative', (inp['combo'] & 0x80000000) != 0 or (inp['move'] & 0x80000000) != 0)


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
        tally(row['input'], obs['events'], cov[fn])
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
        sys.exit(runtime_runner.run_case('fighthitobj72', manifest['binaries']['asi'], analyze, 'FightHitObj72', timeout=3600))
