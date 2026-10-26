"""Compuertas independientes del banco PickUpFirst68 (CTaskComplexGoPickUpEntity::CreateFirstSubTask 0x693610), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x693610 salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos con hook (0x54F560, 0x59C890, 0x61A5A0) y el
constructor real 0x668120 sean saltos a la DLL en ambos lados y que los cuatro sin hook (0x59B020, 0x59AD70, 0x54EF40, 0x54F1B0) sigan siendo codigo del EXE, antes de instalar los falsos, y que
sus cuerpos de la DLL esten en el ASI), fp_difference vacio, la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos, bytes cambiados de la tarea y de la entidad,
palabras del objeto construido) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de ramas (coverage.json = minimos calibrados) y la restauracion final.
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

NAMES = oracle.NAMES
EXPECTED = dict(oracle.CASES_BY_NAME)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
FN = oracle.NAME
HOOKS = {FN: 0x693610}
# (direccion del EXE, hooked): SetRotateZOnly, CMatrix::UpdateRwMatrix, CSimpleTransform::UpdateRwMatrix, UpdateMatrix (sin hook); AllocateMatrix, CMatrix * CVector, CTask::operator new (con hook)
STUBBED = [(0x59B020, False), (0x59AD70, False), (0x54EF40, False), (0x54F1B0, False), (0x54F560, True), (0x59C890, True), (0x61A5A0, True)]
REAL = (0x668120,)          # constructor de CTaskComplexGoToPointAndStandStill: salto a la DLL en ambos lados
WATCHED = {a for a, _ in STUBBED} | set(REAL)
FAMILIES = 16
BASE_KEYS = ('ent_null', 'heading_a_mat', 'heading_a_placement', 'heading_b_mat', 'heading_b_placement', 'heading_c_mat', 'c_set', 'c_skip', 'alloc_reserved', 'object_static',
             'rw_mat', 'rw_placement', 'rw_null', 'pick_right', 'pick_left', 'pick_front', 'pick_back', 'built', 'new_null')
# Minimos de cobertura: coverage.json = {clave: minimo} calibrado con la primera corrida (~50 % de lo medido); mientras no exista, cada clave base exige al menos 1 caso.
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {k: 1 for k in BASE_KEYS}


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
    if ({w['address'] for w in b['watched_entries']} != WATCHED or col['dll_real'] != list(REAL)
            or [(s['exe'], s['hooked']) for s in stubbed] != STUBBED or not all(s['dll'] for s in stubbed)):
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    seen = set()
    fam = {}
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
        for label, side in (('EXE', a), ('DLL', c)):
            bad, stats = oracle.check(fn, row['input'], case, side['observed'])
            if bad:
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s' % (fn, case, label, bad, side['observed']))
        for key, value in stats.items():
            if value is True:
                cov[key] = cov.get(key, 0) + 1
        f = row['input']['family']
        fam[f] = fam.get(f, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov['residue_differs'] = cov.get('residue_differs', 0) + 1
    if seen != set(range(EXPECTED[FN])):
        raise RuntimeError('Cobertura incompleta en ' + FN)
    if len(fam) != FAMILIES or any(v != EXPECTED[FN] // FAMILIES for v in fam.values()):
        raise RuntimeError('Reparto por familia inesperado: %s' % fam)
    shortfalls = {k: (cov.get(k, 0), m) for k, m in COVER.items() if cov.get(k, 0) < m}
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
    summary['families'] = fam
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('pickupfirst68', manifest['binaries']['asi'], analyze, 'PickUpFirst68', timeout=3600))
