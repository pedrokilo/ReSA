"""Compuertas del banco Heli83E (lote 83E): CHeli::ProcessControlInputs (EXE 0x6C4830, hook V), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook V registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: traza de colaboradores EN ORDEN con todos sus argumentos, modo de control, campos escritos en el
helicoptero y arenas sin otros cambios), los microtests de la x87 del hardware, la cobertura por familia de las fixtures y la cobertura de ramas (KEYS con minimo COVER_FLOOR y coverage.json = minimos calibrados).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import re
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import terminal_event, digest
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
NFUNC = 1
ALL = [('CHeli::ProcessControlInputs', 0x6C4830)]
NAMES = [n for n, _ in ALL[:NFUNC]]
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(ALL[:NFUNC])
# (direccion del EXE, hooked): colaboradores con hook RH en la DLL (apagados durante el banco: la entrada del EXE es codigo original y la DLL tiene cuerpo propio)
STUB_ADDRS = (0x5403F0, 0x540080, 0x53FB80, 0x53FBD0, 0x53FE10, 0x53FDD0, 0x53FEE0, 0x56E210, 0x60C1E0, 0x59C910, 0x59C730)
STUBBED = {'CHeli::ProcessControlInputs': [(a, False) for a in STUB_ADDRS]}
WATCHED = {a for fn in NAMES for a, _ in STUBBED.get(fn, [])} | set(HOOKS.values())
COVER_FLOOR = 100
FAMILIES = 18
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_den(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def is_snan(b):
    return is_nan(b) and (b & 0x400000) == 0


KEYS = ['cheat', 'no_cheat', 'mouse_x', 'mouse_y', 'mouse_pad', 'vml_zero', 'vml_set', 'pow_lr', 'pow_ud', 'kb_update', 'kb_after_cheat', 'idle_mouse', 'clamp_hi', 'clamp_lo', 'look_r', 'look_l',
        'horn', 'horn_up', 'horn_flat', 'stab_hi', 'stab_lo', 'stab_mid', 'dpc', 'cap', 'nocap', 'health_low']
FINE = ['mx_nan', 'my_nan', 'ts_special', 'steer_nan', 'health_nan', 'upz_special', 'ms_special', 'pad_garbage', 'look_garbage', 'arg_garbage', 'mode_garbage', 'nrm_special', 'crs_special', 'dpc_zero',
        'other_pad', 'float_special']


def special(b):
    return is_nan(b) or is_inf(b) or is_den(b)


def tally(fn, inp, obs, cov, path):
    """cobertura de ramas desde la ruta del oraculo y las entradas"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    for tok in path.split('+'):
        add(tok)
    add('mx_nan', is_nan(inp['mx']))
    add('my_nan', is_nan(inp['my']))
    add('ts_special', special(inp['ts']) or inp['ts'] == 0 or (inp['ts'] & 0x80000000) != 0)
    add('steer_nan', is_nan(inp['lr0']) or is_nan(inp['ud0']))
    add('health_nan', is_nan(inp['health']))
    add('upz_special', special(inp['mu'][2]) or inp['mu'][2] == 0x80000000)
    add('ms_special', any(special(b) for b in inp['ms']) and ('dpc' in path or 'horn_up' in path))
    add('pad_garbage', any((inp[k] & 0xFFFF0000) != 0 and (inp[k] & 0xFFFF0000) != 0xFFFF0000 for k in ('acc', 'brk', 'lr', 'ud')))
    add('look_garbage', any((inp[k] & 0xFF) > 1 for k in ('look_r', 'look_l', 'horn')))
    add('arg_garbage', inp['arg'] > 0xFF)
    add('mode_garbage', inp['mode'] > 3 and 'cheat' in path)
    add('nrm_special', any(special(b) for k in ('nrm0', 'nrm1') for b in inp[k]) and 'horn_up' in path)
    add('crs_special', any(special(b) for k in ('crs0', 'crs1') for b in inp[k]) and 'horn_up' in path)
    add('dpc_zero', inp['dpc'] == 0)
    add('other_pad', (inp['arg'] & 1) == 1)
    add('float_special', any(special(inp[k]) for k in ('skid0', 'ud0', 'lr0', 'acc0', 'mx', 'my', 'ts', 'health', 'pitch', 'roll')))
    add('family_%d' % inp['family'])


def check_micro(b):
    """los microtests de la x87 del hardware (en `begin`) siguen las reglas que supone el oraculo"""
    micro = b.get('x87_microtests')
    if not micro or len(micro) < 600:
        raise RuntimeError('Faltan los microtests de la x87')
    import fixture_oracle as model
    wrong = []
    for op, a, c2, result in micro:
        x, y = model.fl(a), model.mem(c2)
        if op == 0:
            want = model.to_f32_bits(model.add(x, y))
        elif op == 1:
            want = model.to_f32_bits(model.sub(x, y))
        elif op == 2:
            want = model.to_f32_bits(model.mul(x, y))
        elif op == 3:
            want = model.to_f32_bits(model.div(x, y))
        elif op == 4:
            want = model.to_f32_bits(model.sqrt_v(x))
        elif op == 5:
            want = model.ftol_low32(x) & 0xFFFFFFFF
        else:
            want = model.fabs_bits(a)
        if want != result:
            wrong.append((op, hex(a), hex(c2), hex(result), hex(want)))
    if wrong:
        raise RuntimeError('La x87 del hardware no sigue el modelo del oraculo: %s' % wrong[:5])
    return len(micro)


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
                f = row['input']['family']
                fam[fn][f] = fam[fn].get(f, 0) + 1
                bad, opath = oracle.check_row_obj(row)
                if bad:
                    raise RuntimeError('El oraculo independiente no coincide en %s %d: %s' % (fn, case, bad[0]))
                tally(fn, row['input'], row['candidate']['observed'], cov[fn], opath)
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
    n_micro = check_micro(b)
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
        # los colaboradores con hook RH (apagado durante el banco) llevan el mismo falso en el ASI
        if [s['dll'] for s in col[fn]['stubbed']] != [True] * len(STUBBED[fn]):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver (o inesperado) en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    if counts != EXPECTED or sum(pairs_seen) != total:
        raise RuntimeError('Cobertura incompleta: ' + json.dumps(counts))
    for fn in NAMES:
        short = {k: v for k, v in fam[fn].items() if v < min(200, CASES // 32)}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS}
        need.update({k: 20 for k in FINE})
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    end = completed
    if len(end) != 1 or end[0]['pairs'] != total or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    if len(states) != 1 or states[0]['floating_state'] != b['initial_fp'] or states[0]['watched_intact'] is not True or states[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    return dict(pairs=total, pairs_per_function=counts, all_equal=True, state_restored=True, log_sha256=digest(path), compare_dead_x87_registers=False,
                x87_microtests=n_micro, families=fam, branch_coverage=cov, events_after_complete=post_complete)


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('heli83e', manifest['binaries']['asi'], analyze, 'Heli83E', timeout=3600))
