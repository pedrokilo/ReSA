"""Compuertas independientes del banco CarCtrlAir63 (lote 63, CCarCtrl parte A): helicopteros, aviones, lanchas y combate aereo.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que las entradas de los colaboradores sustituidos sean codigo original del EXE y sus
cuerpos de la DLL esten en el ASI), fp_difference vacio, la cobertura de las fixtures (familias y caminos distintos medidos por la secuencia de colaboradores y los cambios de la arena)
y la restauracion final (hooks, entradas vigiladas, x87). EAX/ECX/EDX solo se registran: ningun llamador del EXE los lee (ver README.md).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
No hay oraculo independiente en Python: la referencia es el EXE (ver README.md, apartado Suposiciones y limites).
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

CASES = 4096
FAMILIES = 12
# (indice de Fn, nombre, direccion, colaboradores sustituidos en orden)
_ATAN, _FHD, _FHF, _FP, _DOG, _FPV, _FPP, _FPC = 0x53CC70, 0x429A70, 0x423940, 0x423000, 0x429890, 0x56E0D0, 0x56E210, 0x56E010
_NORM, _RAND, _SPEED, _APP, _MISSILE, _TEST, _TRIG = 0x59C910, 0x821B1E, 0x56E090, 0x6D1080, 0x429600, 0x429520, 0x429300
_ALLOC, _UPD, _TASKMGR, _FOLLOW, _FPE, _ROCKETS, _BOAT = 0x54F560, 0x54F1B0, 0x681720, 0x42A750, 0x56E120, 0x42B270, 0x73DF00
_DISTLINE, _ADDPROJ, _LOS = 0x412970, 0x737C80, 0x56BA00
_PROBE, _GEAR = 0x422F80, 0x6CAC20
FUNCS = {
    0: ('CCarCtrl::GetAIHeliToFlyInDirection', 0x42A730, [_FHD]),
    1: ('CCarCtrl::GetAIPlaneToDoDogFightAgainstPlayer', 0x42F370, [_FPV, _FPP, _DOG]),
    2: ('CCarCtrl::SteerAIHeliToCrashAndBurn', 0x4238E0, []),
    3: ('CCarCtrl::SteerAIPlaneToCrashAndBurn', 0x423880, []),
    4: ('CCarCtrl::SteerAIPlaneTowardsTargetCoors', 0x423790, [_ATAN, _FP]),
    5: ('CCarCtrl::SteerAIPlaneToFollowEntity', 0x4237F0, [_ATAN, _FP]),
    6: ('CCarCtrl::SteerAIHeliTowardsTargetCoors', 0x42A630, [_ATAN, _FHD, _FHF]),
    7: ('CCarCtrl::SteerAIHeliFlyingAwayFromPlayer', 0x42ACB0, [_FPC, _ATAN, _FHD]),
    8: ('CCarCtrl::SteerAIHeliToLand', 0x42AD30, [_ATAN, _FHD]),
    9: ('CCarCtrl::SteerAIHeliAsPoliceHeli', 0x42AAD0, [_ATAN, _FHD]),
    10: ('CCarCtrl::TestWhetherToFirePlaneGuns', 0x429520, [_NORM]),
    11: ('CCarCtrl::TriggerDogFightMoves', 0x429300, [_NORM, _RAND]),
    12: ('CCarCtrl::GetAIPlaneToAttackPlayer', 0x429780, [_SPEED, _FPC, _ATAN, _FP, _FPV, _APP, _TRIG, _TEST, _MISSILE]),
    13: ('CCarCtrl::GetAIPlaneToDoDogFight', 0x429890, [_ATAN, _RAND, _FP, _TEST, _MISSILE]),
    14: ('CCarCtrl::SteerAIHeliToFollowEntity', 0x42A750, [_ATAN, _FHD, _FHF, _ALLOC, _UPD, _TASKMGR]),
    15: ('CCarCtrl::SteerAIHeliToKeepEntityInView', 0x42AEB0, [_ATAN, _RAND, _FOLLOW]),
    16: ('CCarCtrl::GetAIHeliToAttackPlayer', 0x42F3C0, [_FPC, _ATAN, _FHD, _TEST, _FPE, _ROCKETS]),
    17: ('CCarCtrl::SteerAIBoatWithPhysicsHeadingForTarget', 0x428BE0, [_ATAN]),
    18: ('CCarCtrl::SteerAIBoatWithPhysicsAttackingPlayer', 0x428DE0, [_FPC, _SPEED, _ATAN, _BOAT]),
    19: ('CCarCtrl::SteerAIBoatWithPhysicsCirclingPlayer', 0x429090, [_FPC, _NORM, _ATAN]),
    20: ('CCarCtrl::FireHeliRocketsAtTarget', 0x42B270, [_DISTLINE, _ADDPROJ]),
    21: ('CCarCtrl::FlyAIHeliToTarget_FixedOrientation', 0x423940, [_NORM, _LOS, _ATAN, _RAND]),
    22: ('CCarCtrl::FlyAIPlaneInCertainDirection', 0x423000, [_ATAN, _PROBE, _GEAR]),
    23: ('CCarCtrl::FlyAIHeliInCertainDirection', 0x429A70, [_NORM, _LOS, _ATAN, _RAND]),
}
ACTIVE = list(range(24))   # funciones de esta corrida (los mismos indices que kActive de Core.hpp)
NAMES = [FUNCS[i][0] for i in ACTIVE]
EXPECTED = {n: CASES for n in NAMES}
HOOKS = {FUNCS[i][0]: FUNCS[i][1] for i in ACTIVE}
STUBBED = {FUNCS[i][0]: FUNCS[i][2] for i in ACTIVE}
WATCHED = {a for v in STUBBED.values() for a in v}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')

# Minimos de cobertura: cada familia >= 256 casos y un numero minimo de caminos distintos por funcion (calibrado con una corrida real: ~50 % de lo medido).
MIN_FAMILY = 256
MIN_PATHS = {}
MIN_SEQ = {}
try:
    MIN_PATHS = json.loads((HERE / 'minimums.json').read_text(encoding='utf-8'))
    MIN_SEQ = json.loads((HERE / 'minimums_seq.json').read_text(encoding='utf-8'))
except FileNotFoundError:
    pass


def cls(v):
    """clase de un valor de 32 bits: 0, -0, NaN silencioso/senalizador, infinitos, subnormal, +-1.0, resto por signo; los punteros y enteros pequenos conservan su valor"""
    if isinstance(v, str) or v is None:
        return v
    if v < 0x10000:
        return v
    e, m = (v >> 23) & 0xFF, v & 0x7FFFFF
    sign = v >> 31
    if e == 255:
        return 'inf%d' % sign if m == 0 else ('qnan%d' % sign if m >> 22 else 'snan%d' % sign)
    if e == 0:
        return 'den%d' % sign if m else 'zero%d' % sign
    return 'one%d' % sign if (v & 0x7FFFFFFF) == 0x3F800000 else 'n%d' % sign


def signature(obs):
    """camino del caso: tipos de los eventos de los falsos EN ORDEN + (region, palabra, clase del valor) de lo que cambia en la arena"""
    return json.dumps([[e[0]] for e in obs['events']]) + '|' + json.dumps(sorted((c[0], c[1], str(cls(c[2]))) for c in obs['changes']))


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        stubbed = col[fn]['stubbed']
        if col[fn]['dll_real'] or [s['exe'] for s in stubbed] != STUBBED[fn] or any(s['hooked'] for s in stubbed):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    paths = {fn: set() for fn in NAMES}
    seqs = {fn: {} for fn in NAMES}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
            if side['observed']['nevents'] > 56:
                raise RuntimeError('Demasiados eventos en %s %d' % (fn, case))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        for side in (a, c):
            if any(ch[0] in ('X',) for ch in side['observed']['changes']):
                raise RuntimeError('Cambio fuera de los objetos del caso (guardas) en %s %d: %s' % (fn, case, side['observed']['changes']))
        f = row['input']['fam']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        paths[fn].add(signature(a['observed']))
        seq = json.dumps([e[0] for e in a['observed']['events']])
        seqs[fn][seq] = seqs[fn].get(seq, 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < MIN_FAMILY}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Familias con pocos casos en %s: %s' % (fn, short))
        if len(paths[fn]) < MIN_PATHS.get(fn, 1):
            raise RuntimeError('Pocos caminos distintos en %s: %d < %d' % (fn, len(paths[fn]), MIN_PATHS.get(fn, 1)))
    for fn in NAMES:
        for seq, minimum in MIN_SEQ.get(fn, {}).items():
            if seqs[fn].get(seq, 0) < minimum:
                raise RuntimeError('Camino de colaboradores %s poco cubierto en %s: %d < %d' % (seq, fn, seqs[fn].get(seq, 0), minimum))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = {fn: {'paths': len(paths[fn]), 'families': fam[fn], 'event_sequences': seqs[fn]} for fn in NAMES}
    summary['coverage_method'] = 'families_and_distinct_paths(events+arena_changes); no_independent_oracle'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('carctrlair63', manifest['binaries']['asi'], analyze, 'CarCtrlAir63', timeout=3600))
