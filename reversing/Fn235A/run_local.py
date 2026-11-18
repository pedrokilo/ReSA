"""Compuertas del banco Fn235A (lote 235A): 30 funciones del EXE sin validar de la libjpeg 6b embebida (jdpostct, jdsample, jerror, jmemmgr, jmemnobs, jutils y jcapimin; hooks S), 16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio,
que CADA lado coincida con el oraculo independiente (fixture_oracle.py: bytes cambiados de las arenas, eventos de los colaboradores, valor devuelto, salida no local, x87 vacia), la cobertura por familia
de las fixtures y la cobertura de ramas medida sobre las entradas y el modelo (KEYS con minimo). Cuenta ademas, solo como informacion, los pares con EAX/ECX/EDX distintos tras la llamada.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
"""
import json
import os
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = list(oracle.NAMES)
CASES = int(os.environ.get('FN235A_CASES', '16384'))   # 16384 salvo en las rondas de la prueba negativa (el banco lee la misma variable)
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ENTRIES))
SLOTS = ()
# colaboradores sustituidos por falsos: (direccion del EXE, hooked). hooked = hook RH en la DLL con el MISMO falso en el cuerpo de la DLL (apagado durante el banco); los no hooked: falso solo en la direccion del EXE
# (CRT sin hook, o colaborador con hook que la DLL llama por la direccion del EXE: plugin::Call). Ver LiveComparison.hpp (Update).
DLL_BODY_WITHOUT_HOOK = set()
STUBBED = {n: [] for n in NAMES}
_S = {n: i for i, n in enumerate(NAMES)}
STUBBED.update({
    NAMES[3]: [(0x5C6B90, True), (0x823CAE, False)],
    NAMES[4]: [(0x823A30, False)],
    NAMES[2]: [(0x5CF8A0, True)],
    NAMES[6]: [(0x5CF800, True)],
    NAMES[7]: [(0x5CF820, True)],
    NAMES[8]: [(0x5CEBC0, True), (0x5CED10, True)],
    NAMES[9]: [(0x5CEBC0, True), (0x5CED10, True)],
    NAMES[10]: [(0x5CEBC0, True)],
    NAMES[11]: [(0x5CEBC0, True)],
    NAMES[12]: [(0x5CEDD0, True), (0x5CEE80, True), (0x5CF840, True), (0x5CF850, True)],
    NAMES[13]: [(0x5CF830, True), (0x5CF810, True)],
    NAMES[14]: [(0x5CF870, True), (0x5CF800, True), (0x5CF880, True), (0x8240C7, False), (0x8220AD, False)],
    NAMES[15]: [(0x72F420, True)],
    NAMES[17]: [(0x72F420, True)],
    NAMES[16]: [(0x72F430, True)],
    NAMES[18]: [(0x72F430, True)],
    NAMES[27]: [(0x5CF6D0, True)],
    NAMES[29]: [(0x5C6B50, True)]})
# colaboradores con hook RH en la DLL cuyo hook queda apagado y que corren REALES (sin falso): ninguno en este lote
REAL = {}
WATCHED = set(HOOKS.values()) | set(SLOTS) | {a for lst in STUBBED.values() for a, _ in lst} | {a for lst in REAL.values() for a, _ in lst}
COVER_FLOOR = 50 if CASES >= 16384 else max(1, CASES * 50 // 16384)   # en las corridas reducidas (humo, rondas negativas) el minimo baja en proporcion
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
KEYS = {n: oracle.KEYS[i] for i, n in enumerate(NAMES)}


def tally(inp, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    e = oracle.expected(inp)
    for m in set(e['marks']):
        cov[m] = cov.get(m, 0) + 1


def stream(path):
    """filas de comparacion, una a una"""
    with Path(path).open(encoding='utf-8') as fh:
        for line in fh:
            if line.startswith('{"accepted":'):
                yield json.loads(line)


def stream_common(path):
    """Mismas comprobaciones que probe_case.analyzer, pero en flujo: aquel carga las ~2,3 GB del registro como objetos JSON (16,9 GB de RAM en la corrida de 16384 casos); aqui solo se
    conservan los eventos de control y los identificadores de par."""
    path = Path(path)
    if not runtime_runner.terminal_event(path):
        return None
    total = sum(EXPECTED.values())
    ctrl = []
    pairs = set()
    npairs = 0
    counts = {f: 0 for f in EXPECTED}
    bad = None
    with path.open(encoding='utf-8') as fh:
        for line in fh:
            if not line.endswith('\n'):
                continue
            r = json.loads(line)
            if r['event'] != 'comparison':
                ctrl.append(r)
                continue
            npairs += 1
            pairs.add(r['pair'])
            if bad is None and not (r['equal'] and r['accepted'] and r['original'] == r['candidate']):
                bad = r['pair']
            counts[r['function']] = counts.get(r['function'], 0) + 1
    fails = ['%s: %s/%s' % (r['function'], r['failures'], r['cases']) for r in ctrl if r['event'] == 'function_summary' and r['failures']]
    finished = any(r['event'] in ('complete', 'error', 'assertion') for r in ctrl)
    if fails and finished:
        raise RuntimeError('Funciones con diferencias: ' + '; '.join(fails))
    if any(r['event'] in ('assertion', 'error') for r in ctrl):
        raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
    completed = [r for r in ctrl if r['event'] == 'complete']
    if not completed:
        return None
    if npairs != total or pairs != set(range(total)):
        raise RuntimeError('Cobertura incompleta.')
    if bad is not None:
        raise RuntimeError('Diferencia en ' + str(bad))
    if counts != EXPECTED:
        raise RuntimeError('Reparto de pares inesperado: ' + json.dumps(counts))
    if len(completed) != 1 or not completed[0]['state_restored']:
        raise RuntimeError('Estado no restaurado.')
    return dict(pairs=total, pairs_per_function=counts, all_equal=True, state_restored=True, log_sha256=runtime_runner.digest(path))


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = stream_common(path)
    if summary is None:
        return None
    # una sola pasada en flujo (el registro pesa ~1 GB): las filas de comparacion se comprueban al leerlas y solo se conservan los eventos de control
    ctrl = []
    with path.open(encoding='utf-8') as fh:
        for line in fh:
            if not line.startswith('{"accepted":'):
                ctrl.append(json.loads(line))
    rows = ctrl
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
        got = [(s['exe'], s['hooked']) for s in col[fn]['real']]
        if got != REAL.get(fn, []):
            raise RuntimeError('Colaboradores reales inesperados en ' + fn)
        # los colaboradores con hook RH en la DLL (apagado durante el banco) llevan el mismo falso en su cuerpo del ASI; los demas (rutinas de RenderWare) se llaman por su direccion del EXE
        if any(s['dll'] is not (s['hooked'] or s['exe'] in DLL_BODY_WITHOUT_HOOK) for s in col[fn]['stubbed'] + col[fn]['real']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    resid = {fn: 0 for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    for r in rows:
        if r['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
    for row in stream(path):
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
        e = oracle.expected(row['input'])
        for side in ('original', 'candidate'):
            why = oracle.check_side(row['input'], row[side]['observed'], e, row[side])
            if why:
                raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (side, fn, case, why))
        resid[fn] += row['original_residue'] != row['candidate_residue']
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        for m in set(e['marks']):
            cov[fn][m] = cov[fn].get(m, 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < min(200, CASES // FAMILIES)}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS[fn]}
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
    summary['pairs_with_different_eax_ecx_edx_info_only'] = resid
    summary['families'] = fam
    summary['branch_coverage'] = cov
    summary['independent_oracle'] = 'fixture_oracle.py: cada lado de cada par coincide con el modelo'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fn235a', manifest['binaries']['asi'], analyze, 'Fn235A', timeout=24000, stop_before_analysis=True, unhook='Audio,Global/SurfaceInfos_c' + os.environ.get('FN235A_UNHOOK', '')))
