"""Compuertas independientes del banco ObjectCtor28 (CObject::CObject(int32, bool), 0x5A1D70), 6144 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, los cuatro colaboradores (CPhysical::CPhysical 0x542260, CEntity::SetModelIndex
0x532AE0, CEntity::SetModelIndexNoCreate 0x533700, CObject::Init 0x59F840) sustituidos por falsos en AMBOS lados con sus cuerpos de la DLL resueltos, el vptr de la DLL capturado dentro del
ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio, la salida de CADA lado (objeto final de 0x17C bytes con el vptr propio etiquetado, guardas,
traza de colaboradores, EAX = this) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hooks, entradas de codigo vigiladas, x87).
EAX/ECX/EDX de salida solo se registran (ningun llamador del EXE lee ECX/EDX; EAX = this se exige aparte). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
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

NAMES = oracle.NAMES
EXPECTED = dict(oracle.CASES_BY_FN)
GATES = ('stack_restored', 'nonvolatile_preserved', 'seh_chain_restored', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {'CObject::Constructor': 0x5A1D70}
COLLABORATORS_EXE = [0x542260, 0x532AE0, 0x533700, 0x59F840]
FN = oracle.NAME
MIRROR_KEYS = ('model_kind', 'create_kind', 'model_id', 'create', 'mode', 'obj_off', 'seed_p', 'seed_i', 'eax', 'edx', 'stack_seed', 'pre')

# Minimos de cobertura: calibrados con el espejo EXACTO del generador (fixture_oracle.make, contrastado con el generador C++ en los 6144 casos; el conteo es determinista)
# y con ~15-25 % de margen.
MINIMUMS = {
    'create_branch': 2600, 'nocreate_branch': 2600,
    'create_kind0': 2000, 'create_kind1': 2000, 'create_kind2': 500, 'create_kind3': 500,
    'create_noncanon_true': 500, 'create_noncanon_false': 500, 'create_high_bits_set': 800,
    'model_neg': 1300, 'model_ge_65536_pos': 500, 'model_real': 2600, 'model_20000_65535': 600, 'model_u16_trunc': 1900, 'model_zero': 40,
    'model_minus1': 35, 'model_int_min': 35, 'model_int_max': 35, 'model_low16_zero': 150, 'model_low16_sign': 1600,
    'pre_all_zero': 900, 'pre_all_ff': 850, 'eax_zero': 1000, 'edx_ff': 1000,
}
for _k in range(8):
    MINIMUMS['model_kind%d' % _k] = 620
for _k in range(6):
    MINIMUMS['mode%d' % _k] = 820
for _k in (0x40, 0x44, 0x48, 0x4C):
    MINIMUMS['obj_off_%x' % _k] = 1250


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
        raise RuntimeError('Inicio sin hook registrado, o sin colaboradores sustituidos (este banco sustituye los cuatro colaboradores)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    coll = b['collaborators']
    if coll['exe'] != COLLABORATORS_EXE or len(coll['dll']) != 4 or not all(coll['dll']) or len(set(coll['dll'])) != 4:
        raise RuntimeError('Colaboradores sustituidos inesperados: %s' % coll)
    resolved = b['resolved']
    keys = ('dll_ctor', 'phys_from_ctor', 'phys_from_wrapper', 'set_model_index', 'set_model_index_no_create', 'init')
    if not all(resolved.get(k) for k in keys) or resolved['phys_from_ctor'] != resolved['phys_from_wrapper']:
        raise RuntimeError('Cuerpos de la DLL sin resolver o inconsistentes: %s' % resolved)
    vp = b['dll_vptr']
    if b['exe_vptr'] != oracle.EXE_VPTR or vp['vptr_int_bool'] != vp['vptr_default'] or not vp['vptr_int_bool'] or vp['in_asi'] is not True or vp['vptr_int_bool'] == oracle.EXE_VPTR:
        raise RuntimeError('vptr de la DLL no capturado o igual al del EXE: %s' % vp)
    watched = {w['address'] for w in b['watched_entries']}
    if not ({0x5A1D70, *COLLABORATORS_EXE, *coll['dll']} <= watched):
        raise RuntimeError('Entradas vigiladas inesperadas: %s' % sorted(watched))
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
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s):\n  obtenido %s\n  esperado %s' % (fn, case, label, side['observed'], want))
        # la fila input debe ser exactamente la que reproduce el espejo del generador (detecta un generador C++ distinto del espejo con el que se calibro la cobertura)
        mirror = oracle.make(case)
        if any(row['input'][k] != mirror[k] for k in MIRROR_KEYS):
            raise RuntimeError('La fila input del caso %d no coincide con el espejo del generador' % case)
        for key, value in stats.items():
            if value is True:
                cov[key] = cov.get(key, 0) + 1
    if seen != set(range(EXPECTED[FN])):
        raise RuntimeError('Cobertura incompleta en ' + FN)
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('objectctor28', manifest['binaries']['asi'], analyze, 'ObjectCtor28', timeout=1800))
