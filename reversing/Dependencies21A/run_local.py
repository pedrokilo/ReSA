"""Compuertas independientes del printf del CRT del EXE (0x821982, CRT::CrtPrintf): rutas, ABI, FP, texto producido y cobertura de fixtures."""
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

FUNCTION = 'CRT::CrtPrintf'
EXPECTED = {FUNCTION: oracle.CASES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'route_ok', 'guards_ok', 'globals_restored', 'floating_state_restored')


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True:
        raise RuntimeError('Inicio sin hook registrado')
    if begin[0].get('cases') != oracle.CASES or begin[0].get('pairs') != oracle.CASES:
        raise RuntimeError('Numero de casos inesperado')
    if begin[0].get('collaborators_stubbed') is not True or len(begin[0].get('stubbed_collaborators', [])) != 1:
        raise RuntimeError('Debe declararse exactamente el colaborador doblado (_write)')
    exact_checked, modes, formats, nonempty_flush = 0, set(), set(), 0
    pairs = set()
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['pair'] in pairs or row['function'] != FUNCTION:
            raise RuntimeError('Caso/par repetido o funcion inesperada')
        pairs.add(row['pair'])
        fmt, mode, text, kinds, exact, words, logical = oracle.arguments(case)
        for side in ('original', 'candidate'):
            result = row[side]
            if result is None or not all(result.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o gate fallido en %d (%s)' % (case, side))
            if result['format'] != text or result['mode'] != mode or result['words'] != len(words) or result['variant'] != case // 64:
                raise RuntimeError('La fixture registrada no es la del oraculo en %d' % case)
            got = bytes.fromhex(result['text_hex'])
            # El valor de retorno de printf es el numero de caracteres producidos (EAX), tambien con formatos no exactos.
            if result['eax'] != len(got):
                raise RuntimeError('EAX (%d) distinto de la longitud del texto (%d) en %d (%s)' % (result['eax'], len(got), case, side))
            if mode == 1 and len(got) and result['write_calls'] != 1:
                raise RuntimeError('El volcado del buffer temporal debe llamar una vez a _write en %d' % case)
            if mode == 0 and result['write_calls'] != 0:
                raise RuntimeError('El modo con buffer propio no debe volcar en %d' % case)
        want = oracle.expected(case)
        if want is not None:
            for side in ('original', 'candidate'):
                if bytes.fromhex(row[side]['text_hex']) != want:
                    raise RuntimeError('Texto distinto del oraculo en %d (%s): %r != %r' % (case, side, bytes.fromhex(row[side]['text_hex']), want))
            exact_checked += 1
        modes.add(mode)
        formats.add(fmt)
    if pairs != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    if modes != {0, 1} or formats != set(range(32)):
        raise RuntimeError('Faltan modos o formatos')
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != oracle.CASES or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    summary['oracle_exact_text_cases'] = exact_checked
    summary['compare_dead_x87_registers'] = begin[0].get('compare_dead_x87_registers')
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies21a', manifest['binaries']['asi'], analyze, 'Dependencies21A', timeout=1800))
