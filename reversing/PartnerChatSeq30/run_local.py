"""Compuertas independientes del banco PartnerChatSeq30 (CTaskComplexPartnerChat::GetPartnerSequence 0x684380), 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x684380 salte a la DLL solo en el candidato; collaborators_dll exige que operator new 0x61A5A0, los constructores 0x632BD0/0x66B890/0x682CB0/
0x62F310/0x61ABE0, AddTask 0x632D10 y RegisterReference 0x571B70 sean saltos a la DLL en ambos), fp_difference vacio, la salida de CADA lado (retorno, pool, tareas nuevas con todos sus
bytes, referencias, cambios en `this`, partner, intelligence y otra tarea falsos, region de s_Conversation y llamadas al falso de FindTaskByType) igual a la que modela fixture_oracle.py
desde el desensamblado, la cobertura de las fixtures y la restauracion final (hook, entradas vigiladas, x87, pool, region de s_Conversation).
Si el oraculo discrepa SOLO en bytes de los colaboradores (campo `tasks`), el defecto es de un constructor reconstruido (p. ej. CTaskComplexChat con un contexto de 4 bytes, CTaskSimpleChat),
no de GetPartnerSequence: ver README.md.
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
HOOKS = {oracle.NAME: 0x684380}
FN = oracle.NAME
COLLABORATORS = {0x61A5A0, 0x632BD0, 0x632D10, 0x66B890, 0x682CB0, 0x62F310, 0x571B70, 0x61ABE0}
WATCHED_ONLY = {0x5F65A0, 0x600EE0}
WATCHED = COLLABORATORS | WATCHED_ONLY

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    FN: {
        'alloc4': 1800, 'alloc3': 140, 'alloc2': 140, 'alloc1': 140, 'turn_null': 140, 'x_null': 290, 'y_null': 440, 'partner_null': 430, 'cursor_zero_after_fail': 440,
        'lead_zero': 990, 'lead_nonzero': 1300, 'lead_noncanon': 750, 'lead_high': 370,
        'conv_path': 1240, 'simple_path': 1050, 'conv_lead': 700, 'conv_nonlead': 540, 'simple_lead': 600, 'simple_nonlead': 450,
        'conv_noncanon': 780, 'conv_overwritten': 290, 'conv_set_by_other': 180, 'reset': 430, 'n_negative': 370, 'n_zero': 280, 'n4': 270, 'n5': 180, 'nm1': 95, 'n_extreme': 40,
        'ctx_high': 550, 'ctx_ffff': 160, 'seed_zero': 380, 'seed_high': 980, 'seed_max': 10, 'seed_24bit': 140, 'seed_nonfinite': 140, 'seed_huge': 250, 'seed_neg': 1000,
        'registered1': 780, 'registered2': 120, 'registered3': 760, 'dup_ref': 110, 'skip_building': 48,
        'refs0': 520, 'refs1': 610, 'refs2': 610, 'last_at_cap1': 380, 'search_wraps': 930, 'id_wrap': 300,
        'etype0': 20, 'etype1': 225, 'etype2': 120, 'etype3': 1300, 'etype4': 115, 'etype5': 16, 'etype6': 15, 'etype7': 18,
        'building_ipl': 100, 'building_temp': 60, 'building_proc': 36,
    },
}
for _k in range(16):
    MINIMUMS[FN]['family%d' % _k] = 288
for _k, _m in enumerate((500, 250, 380, 390, 350, 150, 250)):
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
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin el falso de FindTaskByType')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    stubbed = col['stubbed']
    if ({w['address'] for w in b['watched_entries']} != WATCHED or set(col['dll_real']) != COLLABORATORS or set(col['watched_only']) != WATCHED_ONLY
            or len(stubbed) != 1 or stubbed[0]['exe'] != 0x600EE0 or not stubbed[0]['dll']):
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
        raise RuntimeError('El x87, el hook, las entradas vigiladas, el pool o s_Conversation no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('partnerchatseq30', manifest['binaries']['asi'], analyze, 'PartnerChatSeq30', timeout=1800))
