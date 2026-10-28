"""Compuertas del banco PlayerGroup75 (lote 75): CPlayerPed::MakeThisPedJoinOurGroup (EXE 0x60C840, hook S).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, que CADA lado de cada par coincida con el oraculo independiente (fixture_oracle.py), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las
entradas y los eventos (coverage.json = minimos).
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
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = ['CPlayerPed::MakeThisPedJoinOurGroup']
BASE_CASES = 16384
FAMILY_SIZES = {16: 1024, 17: 1024, 18: 1024, 19: 512}   # familias 16..19; las 16 primeras: 1024 cada una
CASES = BASE_CASES + sum(FAMILY_SIZES.values())
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x60C840}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp
STUBBED = {NAMES[0]: [(0x5EFFE0, True), (0x681740, True), (0x5F6A10, True), (0x4E3ED0, False), (0x559A50, True), (0x558E40, True), (0x609380, False), (0x5F6AA0, True), (0x5F7E80, True),
                      (0x5FB210, True), (0x61A5A0, True), (0x61A5B0, True), (0x56E210, True), (0x4AB420, True), (0x5F8020, True), (0x5FC7E0, True), (0x5E8BE0, True), (0x4B5620, False),
                      (0x4B5630, False), (0x571B70, True), (0x571A00, True), (0x5F7470, True), (0x55C180, True), (0x55B980, True), (0x4442D0, True), (0x5839A0, True), (0x583CC0, True),
                      (0x583AB0, True), (0x583EB0, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x60C840, 0x86C6C0}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COVER_FLOOR = 1   # todo contador medido debe alcanzarse al menos una vez; coverage.json sube los minimos


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def tally(inp, events, cov, sets):
    """cobertura de ramas desde la entrada y la traza de eventos del candidato"""
    k = [e[0] for e in events]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('drugged_exit', inp['flags478'] & 0x400 != 0)
    add('task_1000_exit', 2 in k and inp['flags478'] & 0x400 == 0 and inp['has_task'] == 1 and 3 not in k)
    add('type_flags_exit', 2 in k and inp['has_task'] == 0 and 3 not in k)
    add('type8_continue', 3 in k and inp['ped_type'] == 8)
    add('flag_a_continue', 3 in k and inp['ped_type'] != 8 and inp['flag_a'] != 0)
    add('flag_b_continue', 3 in k and inp['ped_type'] != 8 and inp['flag_a'] == 0 and inp['flag_b'] != 0)
    add('flag_c_continue', 3 in k and inp['ped_type'] != 8 and inp['flag_a'] == 0 and inp['flag_b'] == 0 and inp['flag_c'] != 0)
    add('member_exit', 3 in k and 4 not in k)
    add('member_hi_garbage', 3 in k and inp['member_hi'] != 0)
    add('mood_called', 4 in k)
    add('ask_say', 4 in k)
    add('stat_first_equal', k.count(6) == 1)
    add('stat_second_checked', k.count(6) == 2)
    add('stat_first_nan', 4 in k and oracle_nan(inp['stat1']))
    add('stat_second_nan', k.count(6) == 2 and oracle_nan(inp['stat2']))
    add('stat_second_equal', k.count(6) == 2 and f32(inp['stat2']) == 2.0)
    add('stat_first_equal_1', 6 in k and f32(inp['stat1']) == 1.0)
    add('stat_first_equal_2', 6 in k and f32(inp['stat1']) == 2.0)
    add('stat_second_equal_1', k.count(6) == 2 and f32(inp['stat2']) == 1.0)
    add('join_flags_called', 7 in k)
    add('join_flags_true', 7 in k and inp['join_flags'] == 1)
    add('join_flags_false', 7 in k and inp['join_flags'] == 0)
    add('join_flags_hi_garbage', 7 in k and inp['join_hi'] != 0)
    joined = 9 in k
    toomany = 1 in k and any(e[0] == 13 and e[2] == oracle.EV_DONTJOIN for e in events)
    add('joined', joined)
    add('too_many', toomany)
    add('count_calls_1', 7 in k and k.count(8) == 1)
    add('count_calls_2', 7 in k and k.count(8) == 2)
    add('count_calls_3', 7 in k and k.count(8) == 3)
    add('join_below_seven', joined and inp['join_flags'] == 1 and (inp['count0'] & 0x80000000 or inp['count0'] < 7))
    add('join_seven_or_more_then_limit', joined and inp['join_flags'] == 1 and k.count(8) == 3)
    add('count_negative', 8 in k and any(inp['count%d' % i] & 0x80000000 for i in range(4)))
    add('peds_group_removed', 10 in k)
    add('peds_group_none', joined and 10 not in k)
    add('task_failed', joined and inp['task_fail'] == 1)
    add('task_allocated', joined and inp['task_fail'] == 0)
    add('other_player_group', 12 in k and inp['other_player'] == 1)
    add('same_player_group', 12 in k and inp['other_player'] == 0)
    add('event_failed', joined and inp['event_fail'] == 1)
    add('event_allocated', joined and inp['event_fail'] == 0)
    add('flags_mutated', joined and inp['mut_flags'] == 1)
    for i in range(1, 8):
        add('mut_site_%d' % i, inp['mut_site'] == i and ((inp['mut_group'] != inp['group']) or ((inp['mut_limit'] & 0xFF) != (inp['limit'] & 0xFF))))
    add('stat_increment', 21 in k)
    add('blip_called', 24 in k)
    add('color_nonzero_bytes', 24 in k and (inp['color_a'] or inp['color_b'] or inp['color_c']))
    add('group_slot_%d' % inp['group'], joined and inp['mut_site'] == 0)
    add('flags470_bit20_set_before', joined and inp['mut_flags'] == 0 and inp['flags470'] & 0x100000 != 0)
    add('flags470_bit20_clear_before', joined and inp['mut_flags'] == 0 and inp['flags470'] & 0x100000 == 0)
    add('flags474_bit13_set_before', joined and inp['mut_flags'] == 0 and inp['flags474'] & 0x2000 != 0)
    add('flags474_bit13_clear_before', joined and inp['mut_flags'] == 0 and inp['flags474'] & 0x2000 == 0)
    add('ref_high_bit', 23 in k and inp['ref'] & 0x80000000 != 0)
    add('blip_high_bit', 24 in k and inp['blip'] & 0x80000000 != 0)
    if joined:
        sets['groups'].add(inp['group'] if inp['mut_site'] == 0 else -1)
        sets['group_other'].add((inp['group'], inp['other_group'], inp['other_player']) if inp['mut_site'] == 0 else None)


def oracle_nan(b):
    return is_nan(b)


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
    sep = [w for w in b['watched_entries'] if w['address'] == 0x86C6C0][0]['bytes']
    if sep != '0000f042':
        raise RuntimeError('La separacion 0x86C6C0 ya no es 120.0f: el oraculo supone 0x42F00000')
    seen = {fn: set() for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    sets = {fn: {'groups': set(), 'group_other': set()} for fn in NAMES}
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
            msg = oracle.check(row['input'], row[sname])
            if msg:
                raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (sname, fn, case, msg))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed']['events'], cov[fn], sets[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        expected_fam = {f: 1024 for f in range(16)}
        expected_fam.update(FAMILY_SIZES)
        if len(fam[fn]) != 20 or short or fam[fn] != expected_fam:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        cov[fn]['distinct_group_slots'] = len({g for g in sets[fn]['groups'] if g >= 0})
        cov[fn]['distinct_group_other_triples'] = len({t for t in sets[fn]['group_other'] if t is not None})
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
        sys.exit(runtime_runner.run_case('playergroup75', manifest['binaries']['asi'], analyze, 'PlayerGroup75', timeout=3600))
