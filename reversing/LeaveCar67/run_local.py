"""Compuertas del banco LeaveCar67 (lote 67): CTaskSimpleGangDriveBy::SetupAnim (0x627B20), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks registrados y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, que la salida del EXE (eventos EN ORDEN de los falsos, AL y cambios de las arenas) sea la que modela fixture_oracle.py desde el desensamblado, la cobertura por familia
de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza
(devuelve None si el registro aun no existe o no esta completo).
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

NAMES = ['CTaskSimpleGangDriveBy::SetupAnim']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x627B20}
# (direccion del EXE, hooked): GetAnimBlockName, GetAnimationBlock(const char*), RequestModel, AddAnimBlockRef, BlendAnimation, SetDeleteCallback, rand() del EXE (sin hook), SetCurrentTime, IsPlayer
STUBBED = {NAMES[0]: [(0x4D3A30, True), (0x4D3940, True), (0x4087E0, True), (0x4D3FB0, True), (0x4D4610, True), (0x4CEBC0, True), (0x821B1E, False), (0x4CEA80, True), (0x5DF8F0, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}
# Cobertura minima de ramas (calibrada con la primera corrida, con margen): coverage.json = {funcion: {clave: minimo de casos}}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def kinds_of(events):
    return [e[0] for e in events]


def tally(inp, events, task, al, cov):
    """cobertura de ramas desde la entrada y la traza de eventos (sirve igual para el juego y para fixture_selfcheck.py con el modelo)"""
    k = kinds_of(events)
    has = bool(inp['has_assoc'])
    sameid = oracle.s16(inp['a_id']) == oracle.s32(inp['req_id'])

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    undefined = 3 not in k and inp['req_id'] == 0xBF and not has
    add('not_loaded', 3 in k)
    add('not_loaded_group_block', 3 in k and inp['via_group'] == 1)
    add('not_loaded_by_name', 3 in k and inp['via_group'] == 0)
    add('not_loaded_exe_block', 3 in k and inp['blk'] == 2)
    add('loaded_negative_index', 3 not in k and inp['blk'] == 1)
    add('loaded_by_name', 3 not in k and inp['via_group'] == 0)
    add('loaded_byte_odd', inp['blk'] == 0 and (inp['loaded'] & 0xFF) not in (0, 1))
    add('add_ref', 4 in k)
    add('ref_exists', 3 not in k and 4 not in k)
    add('ref_byte_odd', 3 not in k and (inp['refd'] & 0xFF) not in (0, 1))
    add('undefined_exit', undefined)
    add('undefined_with_assoc', 3 not in k and inp['req_id'] == 0xBF and has)
    add('replace_no_assoc', 5 in k and not has)
    add('replace_with_assoc', 5 in k and has)
    add('no_replace_no_assoc', 3 not in k and not undefined and 5 not in k and not has)
    add('no_replace_timer', 3 not in k and not undefined and 5 not in k and not has and (inp['timer'] & 0xFFFF) < 0x8000 and (inp['timer'] & 0xFFFF) != 0)
    add('no_replace_range', 3 not in k and not undefined and 5 not in k and not has and (inp['timer'] & 0xFFFF) >= 0x8000 and (inp['in_range'] & 0xFF) == 0)
    add('timer_negative_replaces', 5 in k and not has and (inp['timer'] & 0xFFFF) >= 0x8000)
    add('timer_zero_replaces', 5 in k and not has and (inp['timer'] & 0xFFFF) == 0)
    add('same_anim', 3 not in k and has and 5 not in k and sameid)
    add('same_low_bits_replaces', 5 in k and has and (inp['a_id'] & 0xFFFF) == (inp['req_id'] & 0xFFFF))
    add('no_replace_command', 3 not in k and has and 5 not in k and not sameid and (inp['next'] & 0xFF) not in (1, 2, 3))
    add('no_replace_last', 3 not in k and has and 5 not in k and not sameid and (inp['next'] & 0xFF) in (1, 2, 3))
    add('last_negative_replaces', 5 in k and has and (inp['last'] & 0xFF) >= 0x80)
    add('burst_roll', 7 in k)
    add('burst_clip_negative', 7 in k and (inp['clip'] & 0x8000) != 0)
    add('burst_clip_small', 7 in k and (inp['clip'] & 0xFFFF) < 2)
    add('burst_signed_trap', 7 in k and (inp['burst'] & 0xFF) >= 0x80)
    add('burst_skipped_big', 5 in k and (inp['next'] & 0xFF) == 3 and 7 not in k)
    add('set_time', 8 in k)
    add('player_true', 9 in k and (inp['is_player_ret'] & 0xFF) != 0)
    add('player_false', 9 in k and (inp['is_player_ret'] & 0xFF) == 0)
    add('timer_set_100', any(c[0] == 0x16 for c in task))
    add('result_true', al == 1)
    add('result_false_after_work', al == 0 and 3 not in k and not undefined)
    cur = 'b' if 5 in k else ('a' if has else None)
    if cur:
        amp, dl = inp[cur + '_amp'], inp[cur + '_del']
        add('amp_nan', is_nan(amp))
        add('amp_inf', is_inf(amp))
        add('amp_at_limit', amp == 0x3F666666)
        add('amp_above_limit_neg_delta', 0x3F666666 < amp < 0x7F800000 and (dl & 0x80000000) != 0 and dl != 0x80000000 and not is_nan(dl) and al == 0)
        add('delta_negative_zero', dl == 0x80000000)
        add('delta_denormal', is_denormal(dl))
        add('delta_nan', is_nan(dl))


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
        diff = oracle.check(row)   # modelo del desensamblado contra el lado ORIGINAL (EXE); original == candidato ya esta comprobado arriba
        if diff:
            raise RuntimeError('El oraculo difiere del EXE en %s %d: %s' % (fn, case, diff))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs['changes'][0], obs['al'], cov[fn])
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
        sys.exit(runtime_runner.run_case('leavecar67', manifest['binaries']['asi'], analyze, 'LeaveCar67', timeout=3600))
