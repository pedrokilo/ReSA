"""Compuertas del banco Car84I (lote 84I): CCarCtrl::SteerAICarWithPhysics_OnlyMission (EXE 0x436A90, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: traza de colaboradores EN ORDEN con el estado del vehiculo en cada llamada, estado final del vehiculo y la matriz;
arenas sin otros cambios), los microtests de la x87 del hardware, la cobertura por familia de las fixtures y la cobertura de ramas (KEYS con minimo COVER_FLOOR y coverage.json = minimos calibrados).
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
ALL = [('CCarCtrl::SteerAICarWithPhysics_OnlyMission', 0x436A90)]
NAMES = [n for n, _ in ALL[:NFUNC]]
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(ALL[:NFUNC])
# (direccion del EXE, hooked): colaboradores con hook RH en la DLL (apagados durante el banco: la entrada del EXE es codigo original y la DLL tiene cuerpo propio)
STUB_ADDRS = (0x434900, 0x435830, 0x432DD0, 0x428DE0, 0x429090, 0x422B20, 0x433EA0, 0x433BA0,
              0x42A630, 0x423790, 0x42F3C0, 0x429780, 0x4237F0, 0x42A730, 0x42A750, 0x42AAD0, 0x42ACB0, 0x42AD30, 0x42AEB0, 0x423880, 0x4238E0, 0x429890, 0x42F370,
              0x423000, 0x433280, 0x4335E0, 0x428990, 0x428BE0, 0x4336D0, 0x56E010, 0x56E0D0, 0x56E090, 0x56E120, 0x6D2A10, 0x5EFFE0, 0x59C910, 0x59C890)
STUBBED = {'CCarCtrl::SteerAICarWithPhysics_OnlyMission': [(a, False) for a in STUB_ADDRS]}
# CMatrix * CVector (0x59C890) lo llama el C++ por direccion: su falso va solo en el EXE (sin cuerpo de la DLL)
EXE_ONLY = {0x59C890}
# funciones reales del EXE con el hook apagado (0x40FE60 resta, 0x4082C0 modulo, 0x40FDB0 producto escalar), constantes de .rdata que lee el EXE (float) y las tablas de salto de la mision (0x437B24, 44 destinos,
# y su tabla de indices 0x437BD4): se vigilan sus bytes
EXTRA_WATCHED = {0x40FE60, 0x4082C0, 0x40FDB0, 0x858B8C, 0x858CC4, 0x858CCC, 0x858CA0, 0x859050, 0x85904C, 0x858EE8, 0x85862C, 0x858B34, 0x858C80, 0x858EF4, 0x858624, 0x858C28, 0x858F48, 0x858B50,
                 0x437B24, 0x437BD4}
WATCHED = {a for fn in NAMES for a, _ in STUBBED.get(fn, [])} | set(HOOKS.values()) | EXTRA_WATCHED
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


KEYS = ['m3', 'm3_nopv', 'm3_A', 'm3_B', 'm3_side', 'm3_noside', 'm3_near', 'm3_far', 'm3_turn', 'm3_dotneg', 'm3_law', 'm3_fast',
        'm44', 'm44_stop', 'm44_nopv', 'm44_posside', 'm44_negside', 'm44_front', 'm44_back', 'm53', 'm53_far', 'm53_gas', 'm53_brake', 'm53_gapneg', 'm53_gappos', 'm60', 'm60_flip', 'm60_noflip', 'mut_applied']
FINE = (['m3_tact_busy', 'm3_say', 'm3_nopass', 'm53_gasclamp', 'm53_brakeclamp', 'm60_null', 'mission_hi', 'nan_input', 'inf_input', 'denormal_input', 'negzero_input']
        + ['mission_%d' % i for i in range(0x45)] + ['mut_kind_%d' % i for i in range(1, 17)])


def special(b):
    return is_nan(b) or is_inf(b) or is_den(b)


def tally(fn, inp, obs, cov, marks):
    """cobertura de ramas desde las marcas del oraculo y las entradas"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    for tok in set(marks):
        add(tok)
    flt = (list(inp['mf']) + list(inp['mp']) + list(inp['pp']) + list(inp['speed']) + list(inp['dest']) + list(inp['tpp']) + list(inp['tmp']) + list(inp['tspeed']) + list(inp['pf0']) + list(inp['pf1'])
           + list(inp['pr0']) + list(inp['pr1']) + list(inp['pspeed0']) + list(inp['pspeed1']) + list(inp['pc0']) + list(inp['pc1']) + list(inp['psp0']) + list(inp['psp1']) + list(inp['mt0']) + list(inp['mt1'])
           + list(inp['nv']) + list(inp['cmx']) + list(inp['cmy']))
    add('nan_input', any(is_nan(x) for x in flt))
    add('inf_input', any(is_inf(x) for x in flt))
    add('denormal_input', any(is_den(x) for x in flt))
    add('negzero_input', any(x == 0x80000000 for x in flt))
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
                bad, marks = oracle.check_row_obj(row)
                if bad:
                    raise RuntimeError('El oraculo independiente no coincide en %s %d: %s' % (fn, case, bad[0]))
                tally(fn, row['input'], row['candidate']['observed'], cov[fn], marks)
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
        if [s['dll'] for s in col[fn]['stubbed']] != [a not in EXE_ONLY for a, _ in STUBBED[fn]]:
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
        need.update({'mut_kind_%d' % i: 15 for i in range(1, 17)})
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
        sys.exit(runtime_runner.run_case('car84i', manifest['binaries']['asi'], analyze, 'Car84I', timeout=3600))
