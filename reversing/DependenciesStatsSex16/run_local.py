"""Compuertas independientes de CStats::UpdateSexAppealStat (0x55BF20): rutas, ABI, FP, cobertura de fixtures y resultado esperado."""
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

FUNCTION = 'CStats::UpdateSexAppealStat'
EXPECTED = {FUNCTION: oracle.CASES}
# FindPlayerVehicle, FindPlayerCoors, RegisterReference, CheckForStatsMessage (los cuatro colaboradores reales que el cuerpo llama), CPad::GetPad, FindPlayerEntity y FindPlayerPed.
HELPERS = {0x56E0D0, 0x56E010, 0x571B70, 0x559760, 0x53FB70, 0x56E120, 0x56E210}
MANDATORY = {0x56E0D0, 0x56E010, 0x571B70, 0x559760}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'helper_routes_ok', 'globals_restored', 'floating_state_restored')
STAT_BLOCK_START, STAT_ADDRESS = 0xB78200, 0xB793E4   # el bloque 0 del estado es 0xB78200..0xB79537: ahi esta STAT_SEX_APPEAL
PADS_0 = 0xB73458                                     # CPad::Pads[0]: CheckForStatsMessage sale pronto por JustOutOfFrontEnd y deja EAX = &Pads[0]


def stat_bits(row_side):
    block = row_side['state'][0]
    if not isinstance(block, str):
        raise RuntimeError('El bloque de estadisticas debe ir completo en el registro')
    at = (STAT_ADDRESS - STAT_BLOCK_START) * 2
    return int.from_bytes(bytes.fromhex(block[at:at + 8]), 'little')


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hook registrado o con colaboradores falsos')
    if begin[0].get('cases') != oracle.CASES or begin[0].get('pairs') != oracle.CASES:
        raise RuntimeError('Numero de casos inesperado')
    helpers = begin[0]['helper_routes']
    if len(helpers) != len(HELPERS) or {h['entry'] for h in helpers} != HELPERS or not all(h['route_ok'] for h in helpers):
        raise RuntimeError('Faltan colaboradores reales demostrados')
    for h in helpers:
        if h['entry'] in MANDATORY and not (h['registered'] and h['reconstructed'] and h['enabled']):
            raise RuntimeError('Colaborador %#x no esta reconstruido y activo en la DLL' % h['entry'])
        if h['registered'] and h['enabled'] and not h['reconstructed']:
            raise RuntimeError('Wrapper original habilitado en DLL')
    if begin[0]['target_initial'].get('route_ok') is not True or begin[0].get('refs_pool_valid') is not True:
        raise RuntimeError('Ruta del objetivo o nodos de referencia no validos')
    coverage, health, tattoo, distance = set(), set(), set(), set()
    ecx_high, edx_values, checked_bits = set(), set(), 0
    slot = begin[0]['unused_model_slot']
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
        expected_input = oracle.fixture(case)
        for side in ('original', 'candidate'):
            result = row[side]
            if result is None or not all(result.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o gate fallido en %d (%s)' % (case, side))
            if result['input'] != expected_input:
                raise RuntimeError('La fixture registrada no es la del oraculo en %d' % case)
            if result['eax'] != PADS_0 or (result['ecx'] & 0xFF) != 1:
                raise RuntimeError('CheckForStatsMessage no salio por JustOutOfFrontEnd (EAX=&Pads[0], CL=1) en %d' % case)
        original = row['original']
        bits, paths = oracle.result(expected_input)
        coverage |= paths
        if bits is not None:
            for side in ('original', 'candidate'):
                if stat_bits(row[side]) != bits:
                    raise RuntimeError('Estadistica %#x distinta de la del oraculo en %d (%s): %#x' % (STAT_ADDRESS, case, side, stat_bits(row[side])))
            checked_bits += 1
        if 'distance_below' in paths:
            health.add(expected_input['health_bits'])
        tattoo.add(expected_input['tattoo_bits'])
        if expected_input['player_mode'] in (1, 2) and expected_input['cache_mode'] != 0:
            distance.add(expected_input['distance_bits'])
        # Residuo hacia CheckForStatsMessage: sin cache -> ECX = 0 antes de su `mov cl`; distancia < 35 -> ECX = modelo (sin signo de 16 bits extendido).
        if 'cached_null' in paths:
            if original['ecx'] != 1:
                raise RuntimeError('ECX distinto de 1 sin cache de vehiculo en %d: %#x' % (case, original['ecx']))
        elif 'distance_below' in paths and (original['ecx'] >> 8) != (slot >> 8):
            raise RuntimeError('ECX no lleva el modelo del vehiculo con distancia < 35 en %d: %#x' % (case, original['ecx']))
        ecx_high.add(original['ecx'] >> 8)
        edx_values.add(original['edx'])
    if pairs != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    if oracle.REQUIRED - coverage:
        raise RuntimeError('Faltan rutas: ' + ', '.join(sorted(oracle.REQUIRED - coverage)))
    for name, wanted, seen in (('salud', oracle.STRESS_HEALTH, health), ('apariencia', oracle.STRESS_TATTOO, tattoo), ('distancia', oracle.STRESS_DISTANCE, distance)):
        if not wanted <= seen:
            raise RuntimeError('Falta estres de %s en rutas activas: %s' % (name, sorted(hex(x) for x in wanted - seen)))
    if len(ecx_high) < 2 or len(edx_values) < 2:
        raise RuntimeError('El residuo ECX/EDX no varia entre rutas: la prueba no distingue colaboradores')
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != oracle.CASES or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'helper_routes_ok', 'target_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    summary['complete_paths'] = sorted(coverage)
    summary['oracle_checked_cases'] = checked_bits
    summary['compare_dead_x87_registers'] = begin[0].get('compare_dead_x87_registers')
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciesstatssex16', manifest['binaries']['asi'], analyze, 'DependenciesStatsSex16', timeout=1800))
