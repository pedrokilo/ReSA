"""Compuertas del banco PlayerAnim80D (lote 80D): CPlayerPed::ProcessAnimGroups (EXE 0x6098F0, hook S).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, que CADA lado de cada par coincida con el oraculo independiente (fixture_oracle.py), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las
entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CPlayerPed::ProcessAnimGroups']
BASE_CASES = 16384
FAMILY_SIZES = {16: 1024, 17: 1024, 18: 1024, 19: 1024}   # familias 16..19; las 16 primeras: 1024 cada una
CASES = BASE_CASES + sum(FAMILY_SIZES.values())
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6098F0}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp
STUBBED = {NAMES[0]: [(0x50A850, True), (0x5DF090, True), (0x732AC0, True), (0x601110, True), (0x5A7950, True), (0x5A81B0, True), (0x609650, True), (0x5E6580, True),
                      (0x743C60, True), (0x4D3A30, True), (0x4D3940, True), (0x4D3FB0, True), (0x4D3FD0, False), (0x4087E0, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6098F0, 0x86D1D0, 0x86D1D4}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COVER_FLOOR = 1   # todo contador medido debe alcanzarse al menos una vez; coverage.json sube los minimos


def exp_class(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return 'nan' if m else 'inf'
    if e == 0:
        return 'zero' if m == 0 else 'denormal'
    return 'normal'


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def tally(inp, events, cov):
    """cobertura de ramas desde la entrada y la traza de eventos del candidato"""
    k = [e[0] for e in events]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('hdg_' + exp_class(inp['hdg']))
    add('hdg_negative', inp['hdg'] & 0x80000000 != 0)
    add('hdg_limit_exact', inp['hdg'] in (0x3F5F66F3, 0xBF5F66F3))
    add('camera_checked', 1 in k)
    add('camera_skipped', 1 not in k)
    add('mouse_false', 1 in k and inp['mouse'] & 0xFF == 0)
    add('mouse_hi_garbage', 1 in k and inp['mouse'] & 0xFF == 0 and inp['mouse'] != 0)
    add('strafe_asked', 2 in k)
    add('strafe_false', 2 in k and inp['strafe'] & 0xFF == 0)
    add('strafe_hi_garbage', 2 in k and inp['strafe'] & 0xFF == 0 and inp['strafe'] != 0)
    add('strafe_true_group_36', 2 in k and inp['strafe'] & 0xFF != 0 and inp['anim_group'] == 0x36)
    add('strafe_true_group_other', 2 in k and inp['strafe'] & 0xFF != 0 and inp['anim_group'] != 0x36)
    add('cam_idx_above_2', 1 in k and inp['cam_idx'] > 2)
    add('clump_asked', 3 in k)
    add('clump_null', 3 in k and not inp['clump_ret'])
    add('model_type_asked', 4 in k)
    add('model_type_weapon', 4 in k and inp['mtype'] & 0xFF == 4)
    add('model_type_weapon_hi_garbage', 4 in k and inp['mtype'] & 0xFF == 4 and inp['mtype'] != 4)
    add('model_type_other', 4 in k and inp['mtype'] & 0xFF != 4)
    add('weapon_rocket', 4 in k and inp['mtype'] & 0xFF == 4 and inp['wtype'] in (0x23, 0x24))
    add('jetpack_asked', 5 in k)
    add('jetpack_true', 5 in k and inp['jet'] != 0)
    add('jetpack_low_byte_zero', 5 in k and inp['jet'] != 0 and inp['jet'] & 0xFF == 0)
    for name, lst in (('list_melee', (5, 6, 7)), ('list_heavy', (9, 0x25, 0x26)), ('list_rifle', (0x1F, 0x1E, 0x1B, 0x19, 0x22, 0x21))):
        add(name, 4 in k and inp['mtype'] & 0xFF == 4 and inp['wtype'] in lst and 5 in k and inp['jet'] == 0 and 6 not in k and 7 in k)
    add('weapon_other_wtype', 4 in k and inp['mtype'] & 0xFF == 4 and inp['wtype'] not in (0x23, 0x24, 5, 6, 7, 9, 0x25, 0x26, 0x1F, 0x1E, 0x1B, 0x19, 0x22, 0x21) and 5 in k)
    add('balaclava_asked', 6 in k)
    add('balaclava_true', 6 in k and inp['bal'] & 0xFF != 0)
    add('balaclava_false', 6 in k and inp['bal'] & 0xFF == 0)
    add('balaclava_hi_garbage', 6 in k and inp['bal'] & 0xFF == 0 and inp['bal'] != 0)
    add('ped2_group_36', 6 in k and inp['bal'] & 0xFF == 0 and inp['ped_type'] == 1 and inp['anim_type'] == 0x36 and 7 in k)
    add('ped2_group_model', 6 in k and inp['bal'] & 0xFF == 0 and inp['ped_type'] == 1 and inp['anim_type'] != 0x36)
    add('ped_type_wide_not_1', inp['ped_type'] in (0x100, 0x101))
    add('default_group', 7 in k)
    add('reapply', 8 in k)
    add('no_reapply', 8 not in k)
    add('slot_negative', s8(inp['slot']) < 0)
    add('weapon_info', 10 in k)
    add('nonmelee_or_vehicle', inp['fire'] != 0 or inp['pflags'] & 0x100 != 0)
    add('in_vehicle_melee', inp['fire'] == 0 and inp['pflags'] & 0x100 != 0)
    add('melee_path', inp['fire'] == 0 and inp['pflags'] & 0x100 == 0)
    add('combo_standard', inp['fire'] == 0 and inp['pflags'] & 0x100 == 0 and s8(inp['base_combo']) == 4)
    add('style_standard', inp['fire'] == 0 and inp['pflags'] & 0x100 == 0 and inp['style'] & 0xFF == 4)
    add('block_name_lookup', 11 in k)
    add('block_add', 13 in k)
    add('block_add_twice', k.count(13) == 2)
    add('block_request', 15 in k)
    add('block_request_twice', k.count(15) == 2)
    add('block_remove', 14 in k)
    add('block_remove_twice', k.count(14) == 2)
    add('block_remove_null_block', 14 in k and any(e[0] == 14 and e[1] >= 0x80000000 for e in events))
    add('loaded_not_one_nonzero', 15 in k and any(inp['loaded%d' % i] not in (0, 1) for i in range(8)))
    add('trap_slot_called', any(x >= 90 for x in k))
    add('mut_site_%d' % inp['mut_site'], inp['mut_site'] != 0)
    for b in (1, 2, 4, 8, 16):
        add('mut_kind_bit_%d' % b, inp['mut_site'] != 0 and inp['mut_kind'] & b != 0)


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
        if any(s['hooked'] and s['dll'] == 0 for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador con hook sin resolver en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    consts = {w['address']: w['bytes'] for w in b['watched_entries']}
    if consts[0x86D1D0] != 'f3665f3f' or consts[0x86D1D4] != 'f3665fbf':
        raise RuntimeError('Las constantes 0x86D1D0 / 0x86D1D4 ya no son +-0.8726646')
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
        for sname in ('original', 'candidate'):
            msg = oracle.check(row['input'], row[sname], b)
            if msg:
                raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (sname, fn, case, msg))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed']['events'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        expected_fam = {f: 1024 for f in range(16)}
        expected_fam.update(FAMILY_SIZES)
        if len(fam[fn]) != 20 or fam[fn] != expected_fam:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in cov[fn]}
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
        sys.exit(runtime_runner.run_case('playeranim80d', manifest['binaries']['asi'], analyze, 'PlayerAnim80D', timeout=3600))
