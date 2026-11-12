"""Compuertas del banco Fn174A (lote 174A): 20 funciones del EXE sin hook (CPad::IsMouse*Pressed, NoDiskInDriveMessage, tres de CHeli, cuerpos de destructor de CMonsterTruck y CTrain, CFontChar::Set, StripTildeTags,
MapExtendedChar, TransformPointsSimple, TransformVectors (CMatrix), DoesInfiniteLineCrossFiniteLine, constructor de tHandlingData, CTrainNode::GetPosn, CTrain::SetModelIndex y PreRender; hooks S y V), 16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio,
que CADA lado coincida con el oraculo independiente (fixture_oracle.py: bytes cambiados de las arenas y de la memoria global, eventos de los colaboradores, valor devuelto, x87 vacia), la cobertura por familia
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
CASES = int(os.environ.get('FN174A_CASES', '16384'))   # 16384 salvo en las rondas de la prueba negativa (el banco lee la misma variable)
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ENTRIES))
SLOTS = ()
# colaboradores sustituidos por falsos: (direccion del EXE, hooked). Con hook RH (S o V) en la DLL (apagado durante el banco): el mismo falso va en el cuerpo de la DLL (DoRWStuffStartOfFrame 0x53D690, MessageLoop 0x746870,
# MessageScreen 0x579330, DrawFonts 0x71A210, DoRWStuffEndOfFrame 0x53D840, GetPad 0x53FB70, RsEventHandler 0x619B60, GetPlaneNumGuns 0x6D3F30, FxSystem_c::Kill 0x4AA3F0, DestroyFxSystem 0x4A9810, Terminate 0x4FB8C0,
# ~CAutomobile 0x6A61E0, CopyToRwMatrix 0x59B8B0, el cuerpo de cTransmission 0x6D0450, ~CVehicle 0x6E2B40 (llamado por una funcion propia de la DLL), CVehicle::SetModelIndex 0x6D6A40 y PreRender 0x6D6480 (hooks V),
# FillFrameArray 0x4C5440 y DoVehicleLights 0x6E1A60; 0x6C4340 es una funcion probada de este lote y reutiliza su ruta). Sin hook (hooked False) y con funcion propia de la DLL (DLL_BODY_WITHOUT_HOOK:
# CPad::UpdatePads 0x541DD0 y CVehicle::operator delete 0x6E2D90): el falso va tambien en esa funcion. Sin hook y sin funcion propia (RenderWare y CRT: 0x7F18A0, 0x7EDDC0 y operator delete[] 0x8213AE): solo en la
# direccion del EXE, que la DLL llama por direccion.
DLL_BODY_WITHOUT_HOOK = {0x6E2D90, 0x541DD0}
STUBBED = {n: [] for n in NAMES}
STUBBED.update({NAMES[4]: [(0x53D690, True), (0x746870, True), (0x541DD0, False), (0x579330, True), (0x71A210, True), (0x53D840, True), (0x53FB70, True), (0x619B60, True)],
                NAMES[5]: [(0x6D3F30, True), (0x4AA3F0, True), (0x4A9810, True), (0x8213AE, False), (0x4FB8C0, True), (0x6A61E0, True)],
                NAMES[7]: [(0x6C4340, True), (0x6E2D90, False)],
                NAMES[8]: [(0x6A61E0, True)],
                NAMES[13]: [(0x59B8B0, True), (0x7F18A0, False), (0x7EDDC0, False)],
                NAMES[15]: [(0x6D0450, True)],
                NAMES[17]: [(0x4FB8C0, True), (0x6E2B40, True)],
                NAMES[18]: [(0x6D6A40, True), (0x4C5440, True)],
                NAMES[19]: [(0x6D6480, True), (0x6E1A60, True)]})
REAL = {NAMES[4]: [(0x719840, True)]}   # RenderFontBuffer: destino del `jmp` que es DrawFonts en el EXE (ruta con el hook apagado; no se llama)
WATCHED = set(HOOKS.values()) | set(SLOTS) | {a for lst in STUBBED.values() for a, _ in lst} | {a for lst in REAL.values() for a, _ in lst}
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
KEYS = {n: oracle.KEYS[i] for i, n in enumerate(NAMES)}


def tally(inp, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    e = oracle.expected(inp)
    for m in set(e['marks']):
        cov[m] = cov.get(m, 0) + 1


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
        bad = oracle.check_row(row)
        if bad:
            raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (bad[0], fn, case, bad[1]))
        resid[fn] += row['original_residue'] != row['candidate_residue']
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], cov[fn])
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
        sys.exit(runtime_runner.run_case('fn174a', manifest['binaries']['asi'], analyze, 'Fn174A', timeout=3600))
