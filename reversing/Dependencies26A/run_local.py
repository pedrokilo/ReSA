"""Compuertas independientes del banco Dependencies26A (CAEAudioHardware::PlaySound 0x4D86B0, 2048 pares).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer): hook registrado, cuerpo en el ASI, colaboradores sustituidos y bandera SSE2 del CRT del EXE activa, todas las compuertas de ABI/ruta/x87,
`fp_difference` vacio, registros x87 vacios iguales, el numero y el orden de los casos y, para CADA lado, la comparacion con un oraculo independiente hecho a partir del desensamblado
(fixture_oracle.py, que solo usa la entrada completa registrada en la fila): traza de colaboradores, cambios del arena y de los canales, TOP, FCW, FTW, banderas acumuladas (PE tras el porcentaje, sin
cambios en IE/DE/ZE/OE/UE), MXCSR y los diez bytes de ST0..ST7 (los seis primeros con el residuo inicial, T-2 = R y T-1 = fraccion tras el porcentaje). Cobertura minima por camino. No relaja ninguna
compuerta: kCompareDeadX87Registers debe ser true. `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza un registro (por ejemplo el que escribe offline/obj/h.exe).
"""
import json
import sys
from collections import Counter
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAME = 'CAEAudioHardware::PlaySound'
FAMILIES = 16
CASES = 2048
EXPECTED = {NAME: CASES}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_restored', 'floating_state_restored')
M32 = 0xFFFFFFFF
# Cobertura minima por clase (calculada con la entrada registrada y el oraculo, no con las formulas de Fixtures.hpp): ~60 % de lo medido con el ensayo sin juego (offline/) con las mismas fixtures.
MIN = {
    'path:channel_negative': 85, 'path:slot_out_of_range': 82, 'path:no_buffer': 86, 'path:index_over_64': 85, 'path:set_audio_buffer_failed': 83, 'path:full': 806, 'percent': 184,
    'percent_pe': 154, 'percent_exact': 30, 'percent_negative_length': 39, 'percent_zero_length': 15, 'percent_huge_length': 24, 'clamped': 190, 'index_63': 36, 'index_64': 41,
    'wrap32': 76, 'n3d_on': 495, 'n3d_off': 311, 'on_street': 306, 'in_room': 189, 'dirty_high_bits': 316, 'speed_qnan': 49, 'speed_snan': 36, 'speed_inf': 31, 'speed_subnormal': 47,
    'flags_negative': 74, 'fsw_distinct': 8,
}


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado o sin colaboradores sustituidos')
    b = begin[0]
    if b['pairs'] != CASES or b.get('functions') != 1 or not all(b['bodies'].values()):
        raise RuntimeError('Pares o cuerpo RH inesperados')
    if b.get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    if b.get('crt_floor_sse2_flag') in (0, None):
        raise RuntimeError('[0xC9C400] = 0: el floor del CRT del EXE tomaria la ruta x87 (0x828A60), que el banco no cubre')
    if b.get('not_ready_collaborators'):
        raise RuntimeError('Colaboradores no preparados: ' + json.dumps(b['not_ready_collaborators']))
    seen = set()
    cov = Counter()
    fsws = set()
    dead_equal = dead_total = residue_differs = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if case in seen:
            raise RuntimeError('Caso repetido %d' % case)
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en el caso %d: %s' % (case, [k for k in GATES if side.get(k) is not True]))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en el caso %d: %s' % (case, row['fp_difference']))
        dead_total += 1
        dead_equal += bool(row['dead_registers_equal'])
        if row['dead_registers_equal'] is not True:
            raise RuntimeError('Registros x87 vacios distintos en el caso %d' % case)
        if row['original_residue'] != row['candidate_residue']:
            residue_differs += 1   # informativo: el unico llamador (0x4F07F8) no lee EAX/ECX/EDX tras la llamada (g26a-audit.txt)
        inp = a['input']
        if inp['case'] != case or inp['family'] != case % FAMILIES or len(inp['args']) != 7:
            raise RuntimeError('Entrada inconsistente en el caso %d' % case)
        sim = oracle.simulate(inp)
        exp = oracle.expected_float_state(inp, sim)
        for name, side in (('original', a), ('candidate', c)):
            if side['trace'] != sim['trace']:
                raise RuntimeError('Traza de %s distinta del oraculo en el caso %d: %s / %s' % (name, case, side['trace'], sim['trace']))
            if side['arena_diff'] != sim['arena_diff']:
                raise RuntimeError('Cambios del arena de %s distintos del oraculo en el caso %d: %s / %s' % (name, case, side['arena_diff'], sim['arena_diff']))
            if side['chan_diff'] != sim['chan_diff']:
                raise RuntimeError('Cambios de canales de %s distintos del oraculo en el caso %d: %s / %s' % (name, case, side['chan_diff'], sim['chan_diff']))
            if side['post_mxcsr'] != 0x1F80:
                raise RuntimeError('MXCSR de %s distinto del inicial en el caso %d: %#x' % (name, case, side['post_mxcsr']))
            fp = oracle.parse_fp(side['post_floating_state'])
            if fp['fcw'] != 0x7F or (fp['fsw'] >> 11) & 7 != 0 or fp['ftw'] != 0xFFFF:
                raise RuntimeError('FCW/TOP/FTW de %s inesperados en el caso %d: %s' % (name, case, fp))
            if fp['fsw'] & 0x1F != exp['sticky']:
                raise RuntimeError('Banderas IE/DE/ZE/OE/UE de %s cambiadas en el caso %d: fsw=%#x esperado %#x' % (name, case, fp['fsw'], exp['sticky']))
            if ((fp['fsw'] >> 5) & 1) != int(exp['pe']):
                raise RuntimeError('Bandera PE de %s distinta de la del oraculo en el caso %d: fsw=%#x esperado PE=%s' % (name, case, fp['fsw'], exp['pe']))
            if fp['regs'] != exp['regs']:
                raise RuntimeError('Registros x87 de %s distintos del oraculo en el caso %d: %s / %s' % (name, case, [r.hex() for r in fp['regs']], [r.hex() for r in exp['regs']]))
        # ---- cobertura (con la entrada registrada y el oraculo)
        cov['path:' + sim['path']] += 1
        fsws.add(inp['fsw'])
        args = inp['args']
        if sim['path'] == 'full':
            cov['n3d_on' if inp['n3d'] & 0xFF else 'n3d_off'] += 1
            if inp['n3d'] & 0xFF:
                cov['in_room' if args[5] & 0x200 else 'on_street'] += 1
            if sim['percent'] is not None:
                cov['percent'] += 1
                cov['percent_pe' if sim['percent'][0] else 'percent_exact'] += 1
                length = oracle.sext32(inp['length'])
                cov['percent_negative_length'] += length < 0
                cov['percent_zero_length'] += length == 0
                cov['percent_huge_length'] += length >= 0x01000000
            cov['clamped'] += bool(sim['clamped'])
            cov['index_63'] += sim['idx'] == 63
            cov['index_64'] += sim['idx'] == 64
            cov['wrap32'] += oracle.sext16(args[0]) + (args[1] & 0xFFFF) >= 0x10000
            sp = args[6] & M32
            exp_bits, man = (sp >> 23) & 0xFF, sp & 0x7FFFFF
            cov['speed_qnan'] += exp_bits == 255 and man != 0 and bool(man & 0x400000)
            cov['speed_snan'] += exp_bits == 255 and man != 0 and not man & 0x400000
            cov['speed_inf'] += exp_bits == 255 and man == 0
            cov['speed_subnormal'] += exp_bits == 0 and man != 0
            cov['flags_negative'] += bool(args[5] & 0x8000)
        cov['dirty_high_bits'] += any(args[i] >> 16 for i in range(6))
    cov['fsw_distinct'] = len(fsws)
    if seen != set(range(CASES)):
        raise RuntimeError('Cobertura incompleta')
    short = {k: (cov[k], want) for k, want in MIN.items() if cov[k] < want}
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != CASES or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    gfinal = [r for r in rows if r['event'] == 'global_state_final']
    if len(gfinal) != 1 or gfinal[0].get('state_restored') is not True:
        raise RuntimeError('Estado global final no restaurado')
    summary.update(coverage=dict(cov), compare_dead_x87_registers=True, dead_registers_equal_pairs=[dead_equal, dead_total], residue_pairs_where_eax_ecx_edx_differ=residue_differs,
                   coverage_method='serialized_inputs_and_independent_oracle; no_instruction_branch_tracing')
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies26a', manifest['binaries']['asi'], analyze, 'Dependencies26A', timeout=3600))
