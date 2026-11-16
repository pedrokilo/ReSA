"""Compuertas del banco Fn204A (lote 204A): 28 funciones del EXE (CTaskComplexAvoidEntity, CTaskInteriorSitInChair/LieInBed, escaneres de CPedIntelligence, eventos; hooks S y V), 16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila, exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo
independiente (fixture_oracle.py: bytes cambiados de las arenas y de la memoria global, eventos de los colaboradores, valor devuelto, x87 vacia), la cobertura por familia de las fixtures y la cobertura de ramas
medida sobre las entradas y el modelo (KEYS con minimo). Cuenta ademas, solo como informacion, los pares con EAX/ECX/EDX distintos tras la llamada. El analisis es en continuo (no guarda las filas: el registro es enorme).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
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
CASES = int(os.environ.get('FN204A_CASES', '16384'))   # 16384 salvo en las rondas de la prueba negativa (el banco lee la misma variable)
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ENTRIES))
SLOTS = ()
# colaboradores sustituidos por falsos: (direccion del EXE, hooked). Todos tienen hook RH en la DLL (apagado durante el banco; el mismo falso va en el cuerpo de la DLL). Los REALES (REAL) corren con el hook apagado.
DLL_BODY_WITHOUT_HOOK = {0x821B1E}   # rand(): la DLL lo llama por CGeneral::GetRandomNumber (sin hook; falso en ambos)
AV = [0x53FB70, 0x56E210, 0x534540, 0x6181D0, 0x681810, 0x618970, 0x6181A0, 0x618280]
STUBBED = {n: [] for n in NAMES}
STUBBED.update({
    NAMES[0]: [(a, True) for a in AV],
    NAMES[4]: [(a, True) for a in AV],
    NAMES[1]: [(0x6181A0, True), (0x618280, True)], NAMES[2]: [(0x6181A0, True), (0x618280, True)], NAMES[3]: [(0x6181A0, True), (0x618280, True)],
    NAMES[5]: [(0x61A5A0, True), (0x66AA20, True)],
    NAMES[6]: [(0x66FD30, True), (0x61A5A0, True), (0x6181A0, True), (0x618280, True)],
    NAMES[7]: [(0x675C90, True), (0x61A5B0, True)],
    NAMES[9]: [(0x675E90, True), (0x61A5B0, True)],
    NAMES[10]: [(0x5FF9D0, True)], NAMES[11]: [(0x5FF9D0, True)], NAMES[13]: [(0x5FF9D0, True)],
    NAMES[8]: [(0x821B1E, False)] + [(a, True) for a in (0x5DEC00, 0x4D68B0, 0x5980F0, 0x4CEBC0, 0x4D4610, 0x4CEBE0, 0x59C890, 0x53CB50, 0x59B020, 0x4241C0, 0x420E30, 0x53CBE0, 0x6181A0, 0x618280, 0x598240, 0x594A50, 0x618970)],
    NAMES[14]: [(a, True) for a in (0x5E0170, 0x6819D0, 0x61A360, 0x601D70, 0x40FD10, 0x4100E0, 0x4AB420, 0x571B70, 0x571A00)],
    NAMES[15]: [(a, True) for a in (0x4AB420, 0x5F7470, 0x605550, 0x441C10, 0x603AF0, 0x5F7E80, 0x571B70, 0x571A00)],
    NAMES[17]: [(0x5E0170, True)],
    NAMES[18]: [(0x4ADF70, True)], NAMES[19]: [(0x571A00, True)], NAMES[20]: [(0x4AE270, True)], NAMES[21]: [(0x4AE3C0, True)], NAMES[22]: [(0x4AE670, True)], NAMES[23]: [(0x4AE7A0, True)],
    NAMES[24]: [(0x4AEA50, True)], NAMES[25]: [(0x4AEB90, True)], NAMES[26]: [(0x4AED60, True)], NAMES[27]: [(0x4AEEA0, True)]})
REAL = {n: [] for n in NAMES}
REAL.update({
    NAMES[0]: [(0x40FE60, True), (0x406DA0, True), (0x59C910, True)],
    NAMES[4]: [(0x66AB40, True), (0x40FE60, True), (0x406DA0, True), (0x59C910, True)],
    NAMES[8]: [(0x407180, True)],
    NAMES[6]: [(0x667CD0, True), (0x61A390, True)],
    NAMES[14]: [(0x40FE00, True), (0x406DA0, True), (0x40FDB0, True), (0x40FEC0, True), (0x4EEA80, True), (0x4AE6E0, True), (0x4AE7A0, True)],
    NAMES[15]: [(0x4AF820, True), (0x4AF890, True), (0x420E70, True)]})
for _i in range(18, 28):
    REAL[NAMES[_i]] = [(0x4B5630, True)]
REAL[NAMES[19]] = [(0x4B5630, True), (0x4AE070, True)]
WATCHED = set(HOOKS.values()) | set(SLOTS) | {a for lst in STUBBED.values() for a, _ in lst} | {a for lst in REAL.values() for a, _ in lst}
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
KEYS = {n: oracle.KEYS[i] for i, n in enumerate(NAMES)}


def analyze(path):
    path = Path(path)
    if not path.exists() or not terminal_event(path):
        return None
    total = sum(EXPECTED.values())
    begin = None
    complete = []
    state = []
    errors = []
    fails = []
    pair_seen = bytearray(total)
    seen = {fn: bytearray(CASES) for fn in NAMES}
    counts = {fn: 0 for fn in NAMES}
    resid = {fn: 0 for fn in NAMES}
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
                if fn not in seen or seen[fn][case]:
                    raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
                seen[fn][case] = 1
                if not (0 <= row['pair'] < total) or pair_seen[row['pair']]:
                    raise RuntimeError('Par repetido o fuera de rango')
                pair_seen[row['pair']] = 1
                if not (row['equal'] and row['accepted'] and row['original'] == row['candidate']):
                    raise RuntimeError('Diferencia en %s %d' % (fn, case))
                counts[fn] += 1
                for side in (row['original'], row['candidate']):
                    if not all(side.get(k) is True for k in GATES):
                        raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
                if row['fp_difference']:
                    raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
                bad = oracle.check_row(row)
                if bad:
                    raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (bad[0], fn, case, bad[1]))
                resid[fn] += row['original_residue'] != row['candidate_residue']
                f = row['input']['family']
                fam[fn][f] = fam[fn].get(f, 0) + 1
                for m in set(oracle.expected(row['input'])['marks']):
                    cov[fn][m] = cov[fn].get(m, 0) + 1
            elif ev.startswith('first_chance_av'):
                raise RuntimeError('Acceso invalido')
            elif ev == 'begin':
                if begin is not None:
                    raise RuntimeError('Varios inicios')
                begin = row
            elif ev == 'function_summary':
                if row['failures']:
                    fails.append('%s: %d/%d' % (row['function'], row['failures'], row['cases']))
            elif ev == 'complete':
                complete.append(row)
            elif ev == 'global_state_final':
                state.append(row)
            elif ev in ('assertion', 'error'):
                errors.append(row)
    if fails:
        raise RuntimeError('Funciones con diferencias: ' + '; '.join(fails))
    if errors:
        raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
    if not complete:
        return None
    if begin is None or begin.get('registered_hook') is not True or begin.get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = begin
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != total or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if sum(pair_seen) != total or counts != EXPECTED:
        raise RuntimeError('Cobertura incompleta o reparto de pares inesperado: ' + json.dumps(counts))
    col = b['collaborators']
    for fn in NAMES:
        got = sorted((s['exe'], s['hooked']) for s in col[fn]['stubbed'])
        if got != sorted(STUBBED.get(fn, [])):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        got = sorted((s['exe'], s['hooked']) for s in col[fn]['real'])
        if got != sorted(REAL.get(fn, [])):
            raise RuntimeError('Colaboradores reales inesperados en ' + fn)
        # los colaboradores con hook RH en la DLL (apagado durante el banco) llevan el mismo falso en su cuerpo del ASI; los demas (rutinas de RenderWare y del CRT) se llaman por su direccion del EXE
        if any(s['dll'] is not (s['hooked'] or s['exe'] in DLL_BODY_WITHOUT_HOOK) for s in col[fn]['stubbed'] + col[fn]['real']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    for fn in NAMES:
        short = {k: v for k, v in fam[fn].items() if v < min(200, CASES // FAMILIES)}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS[fn]}
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short and CASES == 16384:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    if len(complete) != 1 or complete[0]['pairs'] != total or not all(complete[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    return dict(pairs=total, pairs_per_function=counts, all_equal=True, state_restored=True, log_sha256=digest(path), compare_dead_x87_registers=False, pairs_with_different_eax_ecx_edx_info_only=resid,
                families=fam, branch_coverage=cov, independent_oracle='fixture_oracle.py: cada lado de cada par coincide con el modelo')


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fn204a', manifest['binaries']['asi'], analyze, 'Fn204A', timeout=9000))
