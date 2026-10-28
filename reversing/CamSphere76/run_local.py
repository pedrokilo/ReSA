"""Compuertas del banco CamSphere76 (lote 76): CCamera::IsSphereVisible(const CVector&, float) (EXE 0x420D40, hook S).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, que CADA lado de cada par coincida con el oraculo independiente (fixture_oracle.py), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las
entradas y los eventos (coverage.json = minimos).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = ['CCamera::IsSphereVisible']
BASE_CASES = 16384
FAMILY_SIZES = {16: 1024, 17: 1024}   # familias 16 y 17; las 16 primeras: 1024 cada una
CASES = BASE_CASES + sum(FAMILY_SIZES.values())
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x420D40}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp
STUBBED = {NAMES[0]: [(0x420C40, True)]}
WATCHED = {0x420C40, 0x420D40}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COVER_FLOOR = 1   # todo contador medido debe alcanzarse al menos una vez; coverage.json sube los minimos


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_snan(b):
    return is_nan(b) and not (b & 0x400000)


def tally(fn, inp, events, cov):
    """cobertura de ramas desde la entrada y la traza de eventos del candidato"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    calls = len(events)
    mirror = (inp['mut_val'] if inp['mutate'] else inp['mirror']) & 0xFF
    add('one_call', calls == 1)
    add('two_calls', calls == 2)
    add('first_true_short_circuit', inp['res1'] & 0xFF != 0)
    add('first_true_byte_gt1', inp['res1'] & 0xFF > 1)
    add('first_false_no_mirror', calls == 1 and inp['res1'] & 0xFF == 0)
    add('first_false_mirror_second_true', calls == 2 and inp['res2'] & 0xFF != 0)
    add('first_false_mirror_second_false', calls == 2 and inp['res2'] & 0xFF == 0)
    add('mirror_byte_gt1', inp['mirror'] & 0xFF > 1)
    add('mirror_byte_zero', inp['mirror'] & 0xFF == 0)
    add('mirror_byte_one', inp['mirror'] & 0xFF == 1)
    add('mutated_to_nonzero', inp['mutate'] and inp['res1'] & 0xFF == 0 and inp['mirror'] & 0xFF == 0 and mirror != 0)
    add('mutated_to_zero', inp['mutate'] and inp['res1'] & 0xFF == 0 and inp['mirror'] & 0xFF != 0 and mirror == 0)
    add('mutated_other_nonzero', inp['mutate'] and inp['res1'] & 0xFF == 0 and inp['mirror'] & 0xFF != 0 and mirror != 0 and mirror != inp['mirror'] & 0xFF)
    add('radius_nan', is_nan(inp['radius']))
    add('radius_snan', is_snan(inp['radius']))
    add('radius_inf', inp['radius'] & 0x7FFFFFFF == 0x7F800000)
    add('radius_denormal', inp['radius'] & 0x7F800000 == 0 and inp['radius'] & 0x7FFFFF != 0)
    add('radius_neg_zero', inp['radius'] == 0x80000000)
    add('radius_negative', inp['radius'] & 0x80000000 != 0 and not is_nan(inp['radius']))
    add('origin_special', any(is_nan(w) or w & 0x7FFFFFFF == 0x7F800000 for w in inp['origin']))
    add('origin_snan', any(is_snan(w) for w in inp['origin']))
    for k in range(5):
        add('origin_kind_%d' % k, inp['origin_kind'] == k)
    add('hi_garbage_first', inp['hi1'] & 0xFFFFFF00 != 0)
    add('hi_garbage_second', calls == 2 and inp['hi2'] & 0xFFFFFF00 != 0)


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
        if any(s['hooked'] and s['dll'] == 0 for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador con hook sin resolver en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
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
        for sname in ('original', 'candidate'):
            msg = oracle.check(row['input'], row[sname])
            if msg:
                raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (sname, fn, case, msg))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(fn, row['input'], row['candidate']['observed']['events'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        expected_fam = {f: 1024 for f in range(16)}
        expected_fam.update(FAMILY_SIZES)
        if len(fam[fn]) != 18 or short or fam[fn] != expected_fam:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in cov[fn]}
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
    summary['families'] = fam
    summary['branch_coverage'] = cov
    summary['independent_oracle'] = 'fixture_oracle.py: cada lado de cada par coincide con el modelo'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('camsphere76', manifest['binaries']['asi'], analyze, 'CamSphere76', timeout=3600))
