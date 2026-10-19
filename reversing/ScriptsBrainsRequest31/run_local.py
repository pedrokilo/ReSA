"""Compuertas independientes del banco ScriptsBrainsRequest31 (CScriptsForBrains::RequestAttractorScriptBrainWithThisName, EXE 0x0046AA80), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI (funcion y colaboradores sustituidos), todas las compuertas de
ABI/ruta/DF/restauracion en AMBOS lados (route_ok incluye que los falsos de los colaboradores esten puestos en el EXE y en el cuerpo de la DLL), fixture_ok, fp_difference vacio,
la salida de CADA lado igual a la que modela fixture_oracle.py desde el desensamblado (this_fnv, name_fnv, trazas de GetIndex y RequestModel, bytes cambiados de las ventanas reales de usuarios y estado de carga), la cobertura de las fixtures (minimos calibrados con
`python -I fixture_oracle.py`, que compila Fixtures.hpp con cl y vuelca las 4096 filas) y la restauracion final.
La compuerta kCompareDeadX87Registers es false A PROPOSITO: el estado x87 se compara normalizado (ver Core.hpp) y EAX/ECX/EDX solo se registran (ningun llamador del EXE los lee, ver README.md).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta terminado).
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
EXPECTED = {name: oracle.CASES for name in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {'CScriptsForBrains::RequestAttractorScriptBrainWithThisName': 0x46AA80, 'CScriptsForBrains::GetIndexOfScriptBrainWithThisName': 0x46AA30, 'CStreaming::RequestModel': 0x4087E0}
# Minimos de cobertura calibrados con el volcado de Fixtures.hpp (fixture_oracle.selftest) y reducidos ~15 % de margen.
MINIMUMS = {
    "idx0": 148,
    "idx69": 147,
    "kind0": 217,
    "kind1": 217,
    "kind10": 217,
    "kind11": 217,
    "kind12": 217,
    "kind13": 217,
    "kind14": 217,
    "kind15": 217,
    "kind2": 217,
    "kind3": 217,
    "kind4": 217,
    "kind5": 217,
    "kind6": 217,
    "kind7": 217,
    "kind8": 217,
    "kind9": 217,
    "loaded": 1639,
    "loaded_wrap": 379,
    "neg_minus1": 246,
    "neg_other": 188,
    "requested": 1406,
    "requested_wrap": 123,
    "ret_high_garbage": 1881,
    "ret_high_garbage_valid": 1670,
    "script0": 136,
    "script81": 130,
    "state0": 270,
    "state2_4": 370,
    "state_noncanonical": 765,
    "users_0": 113,
    "users_127_128": 216,
    "users_254": 71,
    "valid": 3046,
    "wrap": 502
}


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b.get('layout_ok') is not True:
        raise RuntimeError('Las direcciones fijas (usuarios 0xA47B64, estado de carga 0x964E08) no coinciden con los simbolos de la DLL')
    seen = set()
    cover_rows = []
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != NAMES[0] or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        if row.get('fixture_ok') is not True:
            raise RuntimeError('El fixture compilado en el ASI no coincide con el generador en %d' % case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, side['observed'], want))
        cover_rows.append((row['input'], stats))
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    cov = oracle.coverage(cover_rows)
    cov['residue_differs'] = sum(1 for r in rows if r['event'] == 'comparison' and r['original_residue'] != r['candidate_residue'])
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, las entradas vigiladas o los hooks no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('scriptsbrainsrequest31', manifest['binaries']['asi'], analyze, 'ScriptsBrainsRequest31', timeout=1800))
