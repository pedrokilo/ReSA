"""
Compuertas del banco Fn229A (lote 229A): 23 funciones del modulo RpWorld de RenderWare (lista de materiales, geometrias dinamicas y tuberias de D3D9, sectores de BSP y RpWorld) con hook sin validar
(RpMaterialStreamGetSize, _rpMaterialListDeinitialize, _rpMaterialListAppendMaterial, _rpMaterialListStreamGetSize, _rpMaterialListStreamRead, _rpD3D9AddDynamicGeometry, _rpD3D9RemoveDynamicGeometry,
_rpD3D9PluginAttach, _rpD3D9MaterialPipelineOpen/Close, _rpD3D9WorldSectorPipelineOpen/Close, _rpD3D9AtomicPipelineOpen/Close, _rpWorldSectorDeinstanceAll, _rpWorldSectorDestroyRecurse,
_rpSectorDefaultRenderCallBack, RpWorldUnlock, RpWorldSectorGetWorld, RpWorldDestroy, RpWorldCreate, RpWorldForAllMaterials y RpWorldForAllWorldSectors).

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
CASES = int(os.environ.get('FN229A_CASES', '16384'))   # 16384 salvo en las rondas de la prueba negativa (el banco lee la misma variable)
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ENTRIES))
SLOTS = ()
# colaboradores sustituidos por falsos: (direccion del EXE, hooked). Todos tienen hook RH en la DLL (apagado durante el banco; el mismo falso va en el cuerpo de la DLL). REAL: _rpMaterialListDeinitialize en
# _rpMaterialListStreamRead (el EXE lo lleva en linea; la DLL corre su cuerpo). Ver LiveComparison.hpp (StubLists y RealLists; el orden es el de esas listas).
DLL_BODY_WITHOUT_HOOK = set()
STUBBED = {n: [] for n in NAMES}
REAL = {n: [] for n in NAMES}
_S = {
    0: [0x8045A0, 0x808B00],
    1: [0x74DA20],
    2: [0x8088D0, 0x808820],
    3: [0x74E010],
    4: [0x7ED2D0, 0x7ED540, 0x8088D0, 0x808820, 0x74DD30, 0x74E350, 0x74DA20],
    5: [0x801980],
    7: [0x755B90, 0x7587D0, 0x7FAE30, 0x7FAE20],
    10: [0x8057B0, 0x806990, 0x75E9F0, 0x806BE0, 0x805F40, 0x75E270, 0x805820],
    11: [0x75E270, 0x805820],
    12: [0x8057B0, 0x806990, 0x7582E0, 0x806BE0, 0x805F40, 0x75E2A0, 0x755D80, 0x760CF0, 0x805820],
    13: [0x801B80, 0x75EE60, 0x755FE0, 0x75E2A0, 0x805820],
    14: [0x807DE0, 0x74FFF0, 0x74FFA0, 0x808740, 0x758BC0],
    15: [0x807DE0, 0x74FFF0, 0x74FFA0, 0x808740, 0x758BC0],
    16: [0x805710],
    17: [0x758A90, 0x758C00, 0x75D970, 0x758B80],
    19: [0x758BC0, 0x74E150, 0x74ECA0, 0x74ED50, 0x808740],
    20: [0x74E1B0, 0x8088D0, 0x808820, 0x8086E0, 0x74F210, 0x758BC0, 0x74E150, 0x74ED50, 0x808740],
}
for _i, _l in _S.items():
    STUBBED[NAMES[_i]] = [(a, True) for a in _l]
REAL[NAMES[4]] = [(0x74E150, True)]
WATCHED = set(HOOKS.values()) | set(SLOTS) | {a for lst in STUBBED.values() for a, _ in lst} | {a for lst in REAL.values() for a, _ in lst}
COVER_FLOOR = 50 if CASES >= 4096 else 1
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
        need = {k: COVER_FLOOR for k in KEYS[fn]} if CASES >= 4096 else {}
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
        sys.exit(runtime_runner.run_case('fn229a', manifest['binaries']['asi'], analyze, 'Fn229A', timeout=int(os.environ.get('FN229A_TIMEOUT','9000'))))
