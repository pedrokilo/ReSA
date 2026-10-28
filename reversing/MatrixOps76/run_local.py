"""Compuertas del banco MatrixOps76 (lote 76): CMatrix::Attach (EXE 0x59BD10, hook S) y CMatrix::operator= (EXE 0x59BBC0, hook S).

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

NAMES = ['CMatrix::Attach', 'CMatrix::operator=']
BASE_CASES = 16384
FAMILY_SIZES = {16: 1024, 17: 1024}   # familias 16 y 17; las 16 primeras: 1024 cada una
CASES = BASE_CASES + sum(FAMILY_SIZES.values())
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x59BD10, NAMES[1]: 0x59BBC0}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp
STUBBED = {NAMES[0]: [(0x7F2A20, False)], NAMES[1]: [(0x7F18A0, False)]}
INTERNALS = {0x59AD70, 0x59AD20, 0x59BB60, 0x59BBB0, 0x59ACF0}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | INTERNALS | {0x59BD10, 0x59BBC0}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COVER_FLOOR = 1   # todo contador medido debe alcanzarse al menos una vez; coverage.json sube los minimos
COPIED = (0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38)


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_snan(b):
    return is_nan(b) and not (b & 0x400000)


def is_denorm(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def tally(fn, inp, events, cov):
    """cobertura de ramas desde la entrada y la traza de eventos del candidato"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    if fn == NAMES[0]:
        src = {0: inp['rw_new_w'], 1: inp['this_w'], 2: inp['rw_old_w']}[inp['new_kind']]
        copied = [src[o // 4] for o in COPIED]
        destroyed = len(events) == 1
        add('destroy_called', destroyed)
        add('no_destroy', not destroyed)
        add('old_null', inp['old_kind'] == 0)
        add('old_own_arena', inp['old_kind'] == 1)
        add('old_junk', inp['old_kind'] == 2)
        add('old_same_as_new', inp['old_kind'] == 3)
        add('old_is_this', inp['old_kind'] == 4)
        add('owns_zero', inp['owns'] == 0)
        add('owns_low_zero_upper_junk', inp['owns'] != 0 and inp['owns'] & 0xFF == 0)
        add('owns_low_zero_upper_junk_destroyed', destroyed and inp['owns'] & 0xFF == 0)
        add('owns_low_nonzero', inp['owns'] & 0xFF != 0)
        add('destroy_old_null_owns_set', not destroyed and inp['old_kind'] == 0 and inp['owns'] != 0)
        add('no_destroy_old_set_owns_zero', not destroyed and inp['old_kind'] != 0 and inp['owns'] == 0)
        add('bool_arg_gt1', inp['bool_arg'] & 0xFF > 1)
        add('bool_arg_upper_junk', inp['bool_arg'] > 0xFF)
        add('bool_arg_zero_byte', inp['bool_arg'] & 0xFF == 0)
        add('bool_arg_one_byte', inp['bool_arg'] & 0xFF == 1)
        add('new_alias_this', inp['new_kind'] == 1)
        add('new_rw_old', inp['new_kind'] == 2)
        add('copied_has_nan', any(is_nan(w) for w in copied))
        add('copied_has_snan', any(is_snan(w) for w in copied))
        add('copied_has_denormal', any(is_denorm(w) for w in copied))
        add('copied_has_inf', any((w & 0x7FFFFFFF) == 0x7F800000 for w in copied))
        add('copied_has_neg_zero', any(w == 0x80000000 for w in copied))
        add('uncopied_special', any(is_nan(src[i]) for i in (3, 7, 11, 15)))
    else:
        update = len(events) == 1
        src = inp['this_w'] if inp['src_kind'] else inp['other_w']
        add('update_called', update)
        add('no_attach', not update)
        add('self_assign', inp['src_kind'] == 1)
        add('self_assign_attached', inp['src_kind'] == 1 and update)
        add('dst_own_arena', inp['dst_kind'] == 1)
        add('dst_alias_other', inp['dst_kind'] == 2)
        add('dst_alias_this', inp['dst_kind'] == 3)
        add('dst_rw_new', inp['dst_kind'] == 4)
        add('src_has_nan', any(is_nan(w) for w in src[:16]))
        add('src_has_snan', any(is_snan(w) for w in src[:16]))
        add('src_has_denormal', any(is_denorm(w) for w in src[:16]))
        add('src_has_inf', any((w & 0x7FFFFFFF) == 0x7F800000 for w in src[:16]))
        add('src_has_neg_zero', any(w == 0x80000000 for w in src[:16]))
        add('src_attach_slot_special', inp['src_kind'] == 0 and (is_nan(inp['other_w'][16]) or is_nan(inp['other_w'][17])))
        add('attached_with_snan_copied', update and any(is_snan(src[o // 4]) for o in COPIED))
        add('attached_with_uncopied_special', update and any(is_nan(src[i]) for i in (3, 7, 11, 15)))


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
            msg = oracle.check(fn, row['input'], row[sname])
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
        sys.exit(runtime_runner.run_case('matrixops76', manifest['binaries']['asi'], analyze, 'MatrixOps76', timeout=3600))
