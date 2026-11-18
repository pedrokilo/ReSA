"""Compuertas del banco Fn237A (lote 237A): CRunningScript::ProcessCommands1200To1299 y ProcessCommands1900To1999 (hooks S), 16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila, exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio, que CADA lado
coincida con el oraculo independiente (fixture_oracle.py: emulacion con Unicorn del codigo ORIGINAL del EXE sobre la misma escena y con los mismos falsos: bytes cambiados de las arenas y de la memoria
global, eventos de los falsos y AL), la cobertura por familia y la cobertura por comando y por colaborador (KEYS = coverage.json con minimo). Cuenta ademas, solo como informacion, los pares con
EAX/ECX/EDX distintos tras la llamada. El analizador lee el registro en flujo. `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
"""
import json
import os
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import digest, terminal_event
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = list(oracle.NAMES)
CASES = int(os.environ.get('FN237A_CASES', '16384'))   # 16384 salvo en las rondas de la prueba negativa (el banco lee la misma variable)
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
TOTAL = sum(EXPECTED.values())
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ENTRIES))
# Colaboradores: SUSTITUIDOS por falsos identicos en la entrada del EXE y en el cuerpo de la DLL (modo F de bank_spec.py) y REALES con el gancho apagado (modo R).
import bank_spec as spec
STUB_EXE = sorted(c['exe'] for c in spec.COLLABS if c['mode'] == 'F' and c['exe'])
REAL_EXE = sorted(c['exe'] for c in spec.COLLABS if c['mode'] == 'R')
HOOKS = dict(zip(NAMES, oracle.ENTRIES))
WATCHED = set(HOOKS.values()) | set(STUB_EXE) | set(REAL_EXE)
COVER_FLOOR = 50
KEYS = {n: oracle.KEYS[i] for i, n in enumerate(NAMES)}


def terminal_event_tolerant(path):
    """como runtime_runner.terminal_event, pero ignorando los avisos de acceso invalido que el controlador de DirectX deja al cerrar el juego tras el `complete`"""
    if terminal_event(path):
        return True
    if not path.exists():
        return False
    with path.open('rb') as stream:
        stream.seek(max(0, path.stat().st_size - 20000))
        lines = stream.read().splitlines()
    for raw in reversed(lines):
        if b'first_chance_av' in raw[:600]:
            continue
        try:
            return json.loads(raw).get('event') in ('complete', 'error', 'assertion')
        except ValueError:
            return False
    return False


def analyze(path):
    path = Path(path)
    if not terminal_event_tolerant(path):
        return None
    seen = {fn: set() for fn in NAMES}
    pairs_seen = bytearray(TOTAL)
    resid = {fn: 0 for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    begin = None
    complete = []
    final_state = []
    function_summaries = []
    terminal_bad = []
    post_complete_av = []
    with path.open('r', encoding='utf-8') as stream:
        for line in stream:
            if not line.endswith('\n'):
                continue
            row = json.loads(line)
            kind = row['event']
            if kind != 'comparison':
                if kind.startswith('first_chance_av'):
                    # Un aviso de acceso invalido ANTES del final (o con simbolos del banco, del EXE o de la DLL en la pila) invalida la corrida. Los que llegan despues de `complete` con la pila solo en el
                    # controlador de DirectX (Direct3DCreate9Ex / OpenAdapter / FreeLibrary, al cerrar el juego) no tocan el banco y se anotan aparte.
                    frames = ' '.join(str(x) for x in row.get('frames', []))
                    if complete and not any(m in frames for m in ('F237A', 'CRunningScript', 'CGame', 'Idle', 'MainLoop')):
                        post_complete_av.append(frames[:200])
                        continue
                    raise RuntimeError('Acceso invalido')
                if kind == 'first_chance_av_raw' and complete:
                    continue
                if kind == 'begin':
                    begin = row
                elif kind == 'complete':
                    complete.append(row)
                elif kind == 'global_state_final':
                    final_state.append(row)
                elif kind == 'function_summary':
                    function_summaries.append(row)
                elif kind in ('error', 'assertion'):
                    terminal_bad.append(row)
                continue
            fn, case = row['function'], row['case']
            if fn not in seen or case in seen[fn]:
                raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
            seen[fn].add(case)
            pair = row['pair']
            if not (0 <= pair < TOTAL) or pairs_seen[pair]:
                raise RuntimeError('Par repetido o fuera de rango: %s' % pair)
            pairs_seen[pair] = 1
            if not (row['equal'] and row['accepted'] and row['original'] == row['candidate']):
                raise RuntimeError('Diferencia en el par %d (%s %d)' % (pair, fn, case))
            for side in (row['original'], row['candidate']):
                if not all(side.get(k) is True for k in GATES):
                    raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
            if row['fp_difference']:
                raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
            e = oracle.expected(row['input'])
            bad = oracle.check_row(row, e)
            if bad:
                raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (bad[0], fn, case, bad[1]))
            resid[fn] += row['original_residue'] != row['candidate_residue']
            f = row['input']['family']
            fam[fn][f] = fam[fn].get(f, 0) + 1
            for m in set(e['marks']):
                cov[fn][m] = cov[fn].get(m, 0) + 1
    fails = [f"{r['function']}: {r['failures']}/{r['cases']}" for r in function_summaries if r['failures']]
    if fails and (complete or terminal_bad):
        raise RuntimeError('Funciones con diferencias: ' + '; '.join(fails))
    if terminal_bad:
        raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
    if not complete:
        return None
    if begin is None or begin.get('registered_hook') is not True or begin.get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados')
    b = begin
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != TOTAL or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        if sorted(s['exe'] for s in col[fn]['stubbed']) != STUB_EXE:
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        if sorted(s['exe'] for s in col[fn]['real']) != REAL_EXE:
            raise RuntimeError('Colaboradores reales inesperados en ' + fn)
        if any(s['dll'] is not s['hooked'] for s in col[fn]['real'] + col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL inesperado en un colaborador de ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    if sum(pairs_seen) != TOTAL:
        raise RuntimeError('Cobertura incompleta.')
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < min(200, CASES // FAMILIES)}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        floor = COVER_FLOOR if CASES >= 16384 else max(1, CASES // 300)
        need = {k: floor for k in KEYS[fn]}
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m} if CASES >= 1024 else {}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    if len(complete) != 1 or complete[0]['pairs'] != TOTAL or not all(complete[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    if len(final_state) != 1 or final_state[0]['floating_state'] != b['initial_fp'] or final_state[0]['watched_intact'] is not True or final_state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    return dict(pairs=TOTAL, pairs_per_function={n: EXPECTED[n] for n in NAMES}, all_equal=True, state_restored=True, log_sha256=digest(path),
                compare_dead_x87_registers=False, pairs_with_different_eax_ecx_edx_info_only=resid, families=fam, branch_coverage=cov,
                post_complete_directx_av=post_complete_av, independent_oracle='fixture_oracle.py: cada lado de cada par coincide con el modelo')


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fn237a', manifest['binaries']['asi'], analyze, 'Fn237A', timeout=int(os.environ.get('FN237A_TIMEOUT', '9000'))))
