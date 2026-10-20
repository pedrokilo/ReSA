"""Compuertas independientes del banco PartnerNext60 (CTaskComplexPartner::CreateNextSubTask 0x683AD0), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x683AD0 salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos 0x681740, 0x53CDC0, 0x61A5A0, 0x681AF0 (con hook en HEAD) y 0x681FE0 (sin reconstruir, parche sólo en el EXE)
sean saltos a la DLL en ambos lados (los sin hook, codigo del EXE) antes de instalar los falsos y que sus cuerpos de la DLL esten en el ASI), fp_difference vacio, la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos,
cambios en `this` y entrada de CAnimBlock intacta) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hook, entradas vigiladas, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle


def _is_nan(b):
    return isinstance(b, int) and (b & 0x7F800000) == 0x7F800000 and (b & 0x007FFFFF) != 0


def _ordered(b):
    return b if b < 0x80000000 else -(b & 0x7FFFFFFF)


def _close(a, b, ulps):
    if _is_nan(a) or _is_nan(b):
        return _is_nan(a) and _is_nan(b)
    return isinstance(a, int) and isinstance(b, int) and abs(_ordered(a) - _ordered(b)) <= ulps


def same(obs, want, key=''):
    """Igualdad con el oraculo: exacta salvo (1) los bits de NaN (signo y carga) del desplazamiento `shift` y (2) los argumentos de los eventos 'node~' (seno/coseno del x87 frente a
    math.sin: 2 ulp, o ambos NaN). EXE == DLL se exige estrictamente en bits aparte (comparacion de filas del banco)."""
    if isinstance(want, dict):
        return isinstance(obs, dict) and obs.keys() == want.keys() and all(same(obs[k], want[k], k) for k in want)
    if isinstance(want, list):
        if not isinstance(obs, list) or len(obs) != len(want):
            return False
        if want and want[0] == 'node~':
            return obs[0] == 'node' and _close(obs[1], want[1], 2) and _close(obs[2], want[2], 2)
        if key == 'shift':
            return all(o == w or (_is_nan(o) and _is_nan(w)) for o, w in zip(obs, want))
        return all(same(o, w, key if key == 'events' else '') for o, w in zip(obs, want))
    return obs == want

NAMES = oracle.NAMES
EXPECTED = dict(oracle.CASES_BY_NAME)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NAME: 0x683AD0}
FN = oracle.NAME
STUBBED = (0x681740, 0x53CDC0, 0x61A5A0, 0x681AF0, 0x681FE0)
REAL = (0x62F310, 0x48E4F0, 0x668120, 0x66B890)        # constructores reales (con hook): saltos a la DLL en ambos lados
HELPERS = (0x6822B0, 0x41CCB0, 0x44E480, 0x66B150)     # ayudantes internos del EXE sin hook
WATCHED = set(STUBBED) | set(REAL) | set(HELPERS)

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    FN: {
        'alloc0_built': 1080, 'alloc0_null_used': 191, 'alloc1_built': 94, 'alloc1_null_used': 17, 'first_noncanon': 426, 'heading_matrix': 51, 'heading_sincos': 60, 'lead': 78, 'nonface_def_skip': 19, 'path_arrived': 83, 'path_default': 136, 'path_face_first1': 132, 'path_find_null': 140, 'path_nonface_high': 19, 'path_nonface_low': 132, 'path_other_ped': 140, 'path_partner_null': 138, 'path_s1_lead': 78, 'path_s1_nolead': 37, 'path_s1_null': 25, 'path_s2_goto': 65, 'path_s2_goto_null': 10, 'path_s2_null': 27, 'path_s2_stand': 37, 'path_s3_null': 28, 'path_s3_to4': 50, 'path_s3_turn': 47, 'path_s3_turn_null': 11, 'path_s4_both': 83, 'path_s4_not': 56, 'path_seq': 101, 'path_seq_blocked': 40, 'path_shift': 317, 'path_wander': 112, 'ret_null': 919, 'ret_obj': 1063, 'ret_seq': 29, 'seq_blocked_streamed': 34, 'seq_blocked_upd': 6, 'shift_nan': 49, 'shift_normalised': 200, 'state_noncanon': 239, 'time_wrap': 10,
    },
}
for _k in range(16):
    MINIMUMS[FN]['family%d' % _k] = 256


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
        raise RuntimeError('Inicio sin hook registrado, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    stubbed = col['stubbed']
    if ({w['address'] for w in b['watched_entries']} != WATCHED or col['dll_real'] != list(REAL) or col['exe_helpers'] != list(HELPERS)
            or [s['exe'] for s in stubbed] != list(STUBBED) or not all(s['dll'] or not s['hooked'] for s in stubbed) or [s['hooked'] for s in stubbed] != [True, True, True, True, False]):
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn != FN or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if not same(side['observed'], want):
                diff = [k for k in want if not same(side['observed'].get(k), want[k], k)]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, side['observed'], want))
        for key, value in stats.items():
            if value is True:
                cov[key] = cov.get(key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov['residue_differs'] = cov.get('residue_differs', 0) + 1
    if seen != set(range(EXPECTED[FN])):
        raise RuntimeError('Cobertura incompleta en ' + FN)
    shortfalls = {}
    for key, m in MINIMUMS[FN].items():
        if cov.get(key, 0) < m:
            shortfalls['%s:%s' % (FN, key)] = (cov.get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, el hook o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('partnernext60', manifest['binaries']['asi'], analyze, 'PartnerNext60', timeout=1800))
