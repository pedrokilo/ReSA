"""Compuertas del banco Car84L (lote 84L): CCarCtrl::GenerateOneRandomCar (EXE 0x430050, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura por sitio de llamada del EXE (cada falso registra la direccion de retorno del lado original: los sitios
de llamada del EXE alcanzados por las fixtures; coverage.json = minimos calibrados y los sitios que las fixtures no alcanzan, con el motivo).
No hay oraculo independiente: la funcion de la DLL es una transcripcion mecanica del desensamblado (reversing/Car84L/tools/lift.py) y su unica comprobacion es la igualdad exacta con el EXE (ver README.md).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import re
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import terminal_event, digest
HERE = Path(__file__).resolve().parent
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
NFUNC = 1
ALL = [('CCarCtrl::GenerateOneRandomCar', 0x430050)]
NAMES = [n for n, _ in ALL[:NFUNC]]
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(ALL[:NFUNC])
# colaboradores sustituidos por falsos (todos con la entrada del EXE parcheada; la DLL los llama por esa direccion: dll = 0). Mismo orden que Update() en LiveComparison.hpp
STUB_ADDRS = (0x56E250, 0x56E090, 0x72DD80, 0x611B60, 0x56E230, 0x443AC0, 0x421980, 0x424CE0, 0x441C10, 0x56E0D0, 0x424210, 0x4087E0, 0x611E90, 0x568B80, 0x572400, 0x421440, 0x41CA50, 0x41CAA0,
              0x41CA40, 0x41C9D0, 0x6D1080, 0x4C8500, 0x563220, 0x5674E0, 0x6EB690, 0x4C5A70, 0x4241C0, 0x732B00, 0x6E3290, 0x534540, 0x40BA70, 0x42C2B0, 0x42C740, 0x4217C0, 0x41C070, 0x6D2330,
              0x6A2530, 0x41BE60, 0x56E210, 0x42DE80)
STUBBED = {'CCarCtrl::GenerateOneRandomCar': [(a, False) for a in STUB_ADDRS]}
# funciones reales del EXE con el hook apagado (los bytes del EXE corren en los dos lados), CRT y constantes de .rdata que lee la funcion
REAL_ADDRS = (0x407180, 0x41BD90, 0x4082C0, 0x40FE30, 0x40FE60, 0x40FEC0, 0x4119D0, 0x59C910, 0x41CCB0, 0x420800, 0x420920, 0x420950, 0x420980, 0x420A10, 0x420A60, 0x421120, 0x424130, 0x43C710,
              0x43C900, 0x44DB00)
EXTRA_WATCHED = set(REAL_ADDRS) | {0x821B1E, 0x821B40, 0x821B11, 0x858C48, 0x858624, 0x858B14, 0x858C7C, 0x858B8C, 0x858B50, 0x858C50, 0x858C58, 0x859010}
WATCHED = {a for fn in NAMES for a, _ in STUBBED.get(fn, [])} | set(HOOKS.values()) | EXTRA_WATCHED
FAMILIES = 18
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
SITE_FLOOR = 20


def expected_sites():
    """direcciones de retorno (instruccion siguiente) de las llamadas del EXE a los falsos y de las llamadas virtuales (destructor y altura sobre la carretera)"""
    lines = [l.split(': ', 1) for l in (HERE / 'tools/ins.txt').read_text().splitlines()]
    out = []
    for k, (addr, text) in enumerate(lines):
        if not text.startswith('call'):
            continue
        tgt = text.split()[-1]
        virtual = 'ptr' in text
        if virtual or int(tgt, 16) in STUB_ADDRS:
            out.append(int(lines[k + 1][0], 16))
    return sorted(set(out))


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
    site_count = {}
    ev_kinds = {}
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
                for a in set(row['sites']):
                    site_count[a] = site_count.get(a, 0) + 1
                for e in row['original']['observed']['events']:
                    ev_kinds[e[0]] = ev_kinds.get(e[0], 0) + 1
            elif ev == 'begin':
                if begin is not None:
                    raise RuntimeError('Dos inicios')
                begin = row
            elif ev == 'function_summary':
                if row['failures']:
                    fails.append('%s: %d/%d' % (row['function'], row['failures'], row['cases']))
            elif ev in ('complete', 'error', 'assertion'):
                if any(r['event'] == 'complete' for r in ends):
                    # Despues de `complete` el juego sigue corriendo hasta que el corredor lo cierra: una asercion del propio juego en ese margen no pertenece al banco; se registra en el resumen
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
        # la DLL llama a los colaboradores por la direccion del EXE: solo se parchea la entrada del EXE (dll = 0)
        if [s['dll'] for s in col[fn]['stubbed']] != [False] * len(STUBBED[fn]):
            raise RuntimeError('Parche de la DLL inesperado en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    if counts != EXPECTED or sum(pairs_seen) != total:
        raise RuntimeError('Cobertura incompleta: ' + json.dumps(counts))
    for fn in NAMES:
        short = {k: v for k, v in fam[fn].items() if v < min(200, CASES // 32)}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
    want = expected_sites()
    unreachable = {int(k, 16): why for k, why in COVER.get('unreached_sites', {}).items()}
    need = {a: SITE_FLOOR for a in want if a not in unreachable}
    for k, m in COVER.get('site_floor', {}).items():
        need[int(k, 16)] = m
    short = {'%08X' % a: (site_count.get(a, 0), m) for a, m in need.items() if site_count.get(a, 0) < m}
    extra = sorted('%08X' % a for a in site_count if a not in set(want))
    if short or extra:
        raise RuntimeError('Cobertura de sitios de llamada insuficiente: %s; sitios no esperados: %s; medida: %s' % (json.dumps(short), extra, json.dumps({'%08X' % a: v for a, v in sorted(site_count.items())})))
    end = completed
    if len(end) != 1 or end[0]['pairs'] != total or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    if len(states) != 1 or states[0]['floating_state'] != b['initial_fp'] or states[0]['watched_intact'] is not True or states[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    return dict(pairs=total, pairs_per_function=counts, all_equal=True, state_restored=True, log_sha256=digest(path), compare_dead_x87_registers=False,
                families=fam, call_sites={'%08X' % a: v for a, v in sorted(site_count.items())}, unreached_sites={'%08X' % a: why for a, why in sorted(unreachable.items())},
                event_kinds={str(k): v for k, v in sorted(ev_kinds.items())}, events_after_complete=post_complete)


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('car84l', manifest['binaries']['asi'], analyze, 'Car84L', timeout=3600))
