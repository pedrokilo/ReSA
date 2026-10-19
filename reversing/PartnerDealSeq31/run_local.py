"""Compuertas independientes del banco PartnerDealSeq31 (CTaskComplexPartnerDeal::GetPartnerSequence 0x682440), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x682440 salte a la DLL solo en el candidato; collaborators_dll exige que operator new 0x61A5A0, los constructores 0x632BD0/0x66B890/0x61A8B0,
AddTask 0x632D10 y RegisterReference 0x571B70 sean saltos a la DLL en ambos), fp_difference vacio, la salida de CADA lado (retorno, pool, tareas nuevas con todos sus bytes, referencias,
cambios en `this` y partner) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hook, entradas vigiladas, x87, pool).
Si el oraculo discrepa SOLO en bytes de los colaboradores (campo `tasks`), el defecto es de un constructor reconstruido (p. ej. CTaskSimpleRunAnim), no de GetPartnerSequence: ver README.md.
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
HOOKS = {oracle.NAME: 0x682440}
FN = oracle.NAME
COLLABORATORS = {0x61A5A0, 0x632BD0, 0x632D10, 0x66B890, 0x61A8B0, 0x571B70}
WATCHED = COLLABORATORS

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    FN: {
        'alloc1': 138, 'alloc2': 136, 'alloc3': 1773, 'building_ipl': 130, 'building_proc': 32, 'building_temp': 98, 'cursor_zero_after_fail': 274, 'dup_ref': 139,
        'etype0': 12, 'etype1': 273, 'etype2': 186, 'etype3': 808, 'etype4': 198, 'etype5': 14, 'etype6': 19, 'etype7': 15, 'id_wrap': 223, 'last_at_cap1': 344,
        'lead_high': 246, 'lead_noncanon': 500, 'lead_nonzero': 1017, 'lead_zero': 1030, 'partner_null': 520, 'refs0': 430, 'refs1': 504, 'refs2': 493,
        'registered1': 1228, 'run_null': 274, 'search_wraps': 850, 'skip_building': 59, 'turn_null': 138,
    },
}
for _k in range(16):
    MINIMUMS[FN]['family%d' % _k] = 256
for _k, _m in enumerate((220, 110, 180, 160, 130, 60, 110)):
    MINIMUMS[FN]['pool_mode%d' % _k] = _m


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hook registrado, o con colaboradores sustituidos (este banco no sustituye ninguno)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    if {w['address'] for w in b['watched_entries']} != WATCHED or set(col['dll_real']) != COLLABORATORS:
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    if b['pool_capacity'] < 256 or b['free_references'] < 8:
        raise RuntimeError('Pool de tareas o lista de referencias libres insuficientes')
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
                diff = [k for k in want if side['observed'].get(k) != want[k]]
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
        raise RuntimeError('El x87, el hook, las entradas vigiladas o el pool no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('partnerdealseq31', manifest['binaries']['asi'], analyze, 'PartnerDealSeq31', timeout=1800))
