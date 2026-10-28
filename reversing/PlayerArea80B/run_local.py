"""Compuertas del banco PlayerArea80B (lote 80B): CPlayerPed::KeepAreaAroundPlayerClear (EXE 0x60C1E0, hook S).

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
from runtime_runner import digest, terminal_event
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = ['CPlayerPed::KeepAreaAroundPlayerClear']
BASE_CASES = 16384
FAMILY_SIZES = {16: 1024, 17: 1024, 18: 1024, 19: 1024}   # familias 16..19; las 16 primeras: 1024 cada una
CASES = BASE_CASES + sum(FAMILY_SIZES.values())
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x60C1E0}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp
STUBBED = {NAMES[0]: [(0x5E0170, True), (0x5F6A10, True), (0x534540, True), (0x600EE0, True), (0x4AB650, True), (0x61A5A0, True), (0x61A5B0, True), (0x571B70, True),
                      (0x571A00, True), (0x4AB420, True), (0x564A20, True), (0x424F80, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x60C1E0, 0x858FE8, 0x858B50, 0x86F678}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COVER_FLOOR = 1   # todo contador medido debe alcanzarse al menos una vez; coverage.json sube los minimos
REQUIRED = ['slot_null', 'slot_ped', 'skip_created', 'skip_in_vehicle', 'skip_dead', 'skip_member', 'destroy_offscreen', 'destroy_flag', 'found_task_0', 'found_task_1', 'found_task_2',
            'found_task_3', 'skip_found_task', 'pending_event_0', 'pending_event_1', 'pending_event_2', 'pending_event_3', 'skip_pending_flee', 'pending_other_type_near',
            'pending_other_type_far', 'flee_created', 'ref_vehicle', 'ref_own', 'count_neg', 'count_0', 'count_1', 'count_6', 'veh_skip_mission', 'veh_skip_status_0', 'veh_skip_status_9',
            'veh_has_matrix', 'veh_no_matrix', 'veh_wait', 'veh_reverse', 'veh_goforward', 'nan_input', 'inf_input', 'denormal_input', 'negzero_input'] + ['mut_kind_%d' % k for k in range(1, 17)]


def exp_class(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return 'nan' if m else 'inf'
    if e == 0:
        return 'negzero' if (m == 0 and bits >> 31) else 'zero' if m == 0 else 'denormal'
    return 'normal'


def tally(inp, events, cov):
    """cobertura de ramas: contadores del modelo independiente (fixture_oracle.model) y de la traza"""
    _, _, _, br = oracle.model(inp)
    for k, v in br.items():
        cov[k] = cov.get(k, 0) + v

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    kinds = [e[0] for e in events]
    for k in set(kinds):
        add('event_kind_%d' % k)
    coords = list(inp['ppos']) + list(inp['pmatpos']) + list(inp['vpos']) + list(inp['vmatpos'])
    for v in range(3):
        coords += inp['vehpos%d' % v] + inp['vehmpos%d' % v] + inp['vehfwd%d' % v]
    classes = {exp_class(c) for c in coords}
    for c in ('nan', 'inf', 'denormal', 'negzero'):
        add(c + '_input', c in classes)
    add('tol_snan', (inp['tol'] & 0x7FC00000) == 0x7F800000 and (inp['tol'] & 0x3FFFFF) != 0)
    add('mut_kind_%d' % inp['mut_kind'], inp['mut_at'] != 0)
    add('mut_fired', inp['mut_at'] != 0 and inp['mut_at'] <= len(events))
    add('ped_destroy_vcall', 4 in kinds)
    add('timer_wrap', inp['timer'] > 0xFFFFE000)


def analyze(path):
    """Analisis en una sola pasada (el registro completo pesa cientos de MB): nada se acumula salvo contadores"""
    path = Path(path)
    if not path.exists() or not terminal_event(path):
        return None
    total = sum(EXPECTED.values())
    begin = None
    ends, states, fails, post_complete = [], [], [], []
    seen = {fn: bytearray(CASES) for fn in NAMES}
    pairs_seen = bytearray(total)
    counts = {fn: 0 for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    with path.open(encoding='utf-8') as stream:
        for line in stream:
            if not line.endswith('\n'):
                continue
            row = json.loads(line)
            ev = row['event']
            if ev == 'comparison':
                fn, case = row['function'], row['case']
                if fn not in seen or seen[fn][case] or row['pair'] >= total or pairs_seen[row['pair']]:
                    raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
                seen[fn][case] = 1
                pairs_seen[row['pair']] = 1
                counts[fn] += 1
                if not (row['equal'] and row['accepted'] and row['original'] == row['candidate']):
                    raise RuntimeError('EXE y DLL difieren en %s %d' % (fn, case))
                for side in (row['original'], row['candidate']):
                    if not all(side.get(k) is True for k in GATES):
                        raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
                if row['fp_difference']:
                    raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
                for sname in ('original', 'candidate'):
                    msg = oracle.check(row['input'], row[sname], begin)
                    if msg:
                        raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (sname, fn, case, msg))
                f = row['input']['family']
                fam[fn][f] = fam[fn].get(f, 0) + 1
                tally(row['input'], row['candidate']['observed']['events'], cov[fn])
            elif ev == 'begin':
                if begin is not None:
                    raise RuntimeError('Dos inicios')
                begin = row
            elif ev == 'function_summary':
                if row['failures']:
                    fails.append('%s: %d/%d' % (row['function'], row['failures'], row['cases']))
            elif ev in ('complete', 'error', 'assertion'):
                if any(r['event'] == 'complete' for r in ends):
                    # Despues de `complete` (comparaciones hechas, hooks y estado restaurados) el juego sigue corriendo hasta que el corredor lo cierra: una asercion del propio juego
                    # (p. ej. CShadowCamera::DrawOutlineBorder) en ese margen no pertenece al banco; se registra en el resumen
                    post_complete.append({k: row.get(k) for k in ('event', 'expression', 'file', 'line')} | {'top_of_stack': (row.get('stack') or [''])[:4]})
                else:
                    ends.append(row)
            elif ev == 'global_state_final':
                states.append(row)
            elif ev.startswith('first_chance_av'):
                raise RuntimeError('Acceso invalido')
    if fails and ends:
        raise RuntimeError('Funciones con diferencias: ' + '; '.join(fails))
    if any(r['event'] in ('assertion', 'error') for r in ends):
        raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
    completed = [r for r in ends if r['event'] == 'complete']
    if not completed:
        return None
    b = begin
    if b is None or b.get('registered_hook') is not True or b.get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != total or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
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
    if consts[0x858FE8] != '0000c841' or consts[0x858B50] != '00000000' or consts[0x86F678] != 'e8030000':
        raise RuntimeError('Las constantes 0x858FE8 / 0x858B50 / 0x86F678 ya no son 25.0f / 0.0f / 1000')
    for fn in NAMES:
        if counts[fn] != CASES or any(not x for x in seen[fn]):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        expected_fam = {f: 1024 for f in range(16)}
        expected_fam.update(FAMILY_SIZES)
        if len(fam[fn]) != 20 or fam[fn] != expected_fam:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in cov[fn]}
        need.update({k: COVER_FLOOR for k in REQUIRED})
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    if len(completed) != 1 or completed[0]['pairs'] != total or not all(completed[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    if len(states) != 1 or states[0]['floating_state'] != b['initial_fp'] or states[0]['watched_intact'] is not True or states[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    return dict(pairs=total, pairs_per_function=counts, all_equal=True, state_restored=True, log_sha256=digest(path), compare_dead_x87_registers=False, families=fam,
                branch_coverage=cov, independent_oracle='fixture_oracle.py: cada lado de cada par coincide con el modelo', events_after_complete=post_complete)


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('playerarea80b', manifest['binaries']['asi'], analyze, 'PlayerArea80B', timeout=3600))
