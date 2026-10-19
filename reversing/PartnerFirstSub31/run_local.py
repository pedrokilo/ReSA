"""Compuertas independientes del banco PartnerFirstSub31 (CTaskComplexPartner::CreateFirstSubTask 0x681F20), 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x681F20 salte a la DLL solo en el candidato), fp_difference vacio, la salida de CADA lado (retorno como etiqueta, traza de los falsos de
RequestPedConversation / StopPlayingHandSignal / operator new con el estado de +0x74/+0x75/+0x76 en cada llamada, cambios en `this` y guardas, bytes del objeto reservado y de sus guardas)
igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hook, entradas vigiladas, x87).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NAME: 0x681F20}
FN = oracle.NAME
WATCHED = {0x4E50E0, 0x5E0480, 0x61A5A0}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    FN: {
        'alloc_null': 300, 'chat': 1700, 'chat_conv': 1200, 'chat_no_conv': 480, 'conv_noncanon': 950, 'f75_noncanon': 500, 'lead_noncanon': 950, 'lead_one': 950, 'lead_zero': 330,
        'loaded_already1': 30, 'loaded_set': 440, 'object_built': 1800, 'partner_ffff': 70, 'partner_zero': 75, 'ped_ffff': 75, 'ped_zero': 70, 'req_high_nonzero': 4000,
        'req_noncanon': 320, 'request_false_f75': 590, 'request_false_null': 150, 'request_true': 470, 'type_chat': 2000, 'type_other': 320,
    },
}
for _k in range(16):
    MINIMUMS[FN]['family%d' % _k] = 288


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
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    stubbed = b['collaborators']['stubbed']
    if ({w['address'] for w in b['watched_entries']} != WATCHED or {w['exe'] for w in stubbed} != WATCHED or len(stubbed) != 3 or not all(w['dll'] for w in stubbed)
            or b['collaborators']['exe_real'] != [0x62F310]):
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
        raise RuntimeError('El x87, el hook, la entrada vigilada o la ruta no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('partnerfirstsub31', manifest['binaries']['asi'], analyze, 'PartnerFirstSub31', timeout=1800))
