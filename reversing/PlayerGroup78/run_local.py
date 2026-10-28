"""Compuertas del banco PlayerGroup78 (lote 78B): CPlayerPed::ProcessGroupBehaviour (EXE 0x60D350, hook S).

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

NAMES = ['CPlayerPed::ProcessGroupBehaviour']
BASE_CASES = 16384
FAMILY_SIZES = {16: 1024, 17: 1024, 18: 1024, 19: 1024}   # familias 16..19; las 16 primeras: 1024 cada una
CASES = BASE_CASES + sum(FAMILY_SIZES.values())
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x60D350}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp
STUBBED = {NAMES[0]: [(0x56E0D0, True), (0x541190, True), (0x5411B0, True), (0x60A0A0, True), (0x60A1D0, True), (0x60C840, True), (0x609380, False), (0x56E210, True),
                      (0x608830, True), (0x608DA0, True), (0x608980, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x60D350, 0x858B38, 0x858C4C}
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


def tally(inp, events, cov, c1, c2):
    """cobertura de ramas desde la entrada y la traza de eventos del candidato"""
    k = [e[0] for e in events]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    ones = [i for i, x in enumerate(k) if x == 1]
    second = ones[1] if len(ones) > 1 else len(k)
    first_part, second_part = k[:second], k[second:]
    add('block1_skipped_vehicle', inp['veh1'] != 0)
    add('block1_ran', 2 in k)
    add('block2_skipped_flags', len(ones) == 1)
    add('block2_vehicle', len(ones) == 2 and inp['veh2'] != 0)
    add('block2_ran', 3 in k)
    add('up_pressed', 2 in k and inp['up'] & 1 == 1)
    add('up_released', 2 in k and inp['up'] & 1 == 0)
    add('up_hi_garbage', 2 in k and inp['up_hi'] != 0)
    add('down_hi_garbage', 3 in k and inp['down_hi'] != 0)
    add('disband_up', 4 in first_part)
    add('disband_down', 4 in second_part)
    add('join_up', 6 in first_part)
    add('tell_up', 5 in first_part)
    add('join_down', 6 in second_part and 3 in k)
    add('tell_down', 5 in second_part and 3 in k)
    add('join_flags_called', 7 in k)
    add('join_flags_true', 7 in k and inp['jf'] & 1 == 1)
    add('join_flags_false', 7 in k and inp['jf'] & 1 == 0)
    add('join_flags_hi_garbage', 7 in k and inp['jf_hi'] != 0)
    add('target_mouse', any(e[0] == 6 and e[2] == oracle.MOUSE for e in events))
    add('target_targeted', any(e[0] == 6 and e[2] == oracle.TGT for e in events))
    add('pool_loop', 8 in k)
    add('pool_wanted_positive', 10 in k)
    add('pool_wanted_nonpositive', 11 in k)
    add('pool_wanted_negative', 11 in k and inp['wanted'] & 0x80000000 != 0)
    add('pool_size_zero_frame6', inp['frame'] & 0x1F == 6 and inp['pool_size'] == 0 and 3 in k)
    add('pool_frame_not_6', 3 in k and inp['frame'] & 0x1F != 6)
    add('pool_empty_slot_skipped', 3 in k and inp['frame'] & 0x1F == 6 and any(inp['slot%d' % i] & 0x80 and i < inp['pool_size'] for i in range(6)))
    add('pool_other_type_skipped', 3 in k and inp['frame'] & 0x1F == 6 and any(not inp['slot%d' % i] & 0x80 and inp['spt%d' % i] != 8 and i < inp['pool_size'] for i in range(6)))
    add('pool_type8_wide_not_8', 3 in k and inp['frame'] & 0x1F == 6 and any(inp['spt%d' % i] in (0x108, 0x80000008) and i < inp['pool_size'] for i in range(6)))
    add('pool_all_six', k.count(8) == 6)
    if (2 in k and inp['up'] & 1) or (3 in k and inp['down'] & 1):
        add('step_' + exp_class(inp['ts']))
        add('step_negative', inp['ts'] & 0x80000000 != 0 and exp_class(inp['ts']) in ('normal', 'denormal'))
        a = oracle.step_add(inp['ts'], c1, c2)
        add('step_add_zero', a == 0)
        add('step_add_nonzero', a != 0)
        add('step_add_wrapped_negative', a >= 0x8000)
    add('mut_site_%d' % inp['mut_site'], inp['mut_site'] != 0)
    for b in (1, 2, 4, 8):
        add('mut_kind_bit_%d' % b, inp['mut_site'] != 0 and inp['mut_kind'] & b != 0)
    add('flags_bit_set_initial', inp['flags'] & 0x100 != 0)


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
    if consts[0x858B38] != '0ad7a33c' or consts[0x858C4C] != '00007a44':
        raise RuntimeError('Las constantes 0x858B38 / 0x858C4C ya no son 0.02f / 1000.0f')
    c1, c2 = oracle.consts(b)
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
        tally(row['input'], row['candidate']['observed']['events'], cov[fn], c1, c2)
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
        sys.exit(runtime_runner.run_case('playergroup78', manifest['binaries']['asi'], analyze, 'PlayerGroup78', timeout=3600))
