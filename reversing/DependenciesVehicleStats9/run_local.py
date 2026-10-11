"""Gates completos y cobertura de dos actualizaciones de habilidad de vehiculo."""
import json
import math
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
EXPECTED = {'CStats::UpdateStatsWhenDriving': 2048, 'CStats::UpdateStatsWhenFlying': 2048}
HELPERS = {0x53FB70, 0x588B50, 0x558E40, 0x6A0050, 0x588BE0, 0x559760, 0x821B40, 0x55C180, 0x55B980}
GATES = ('stack_restored', 'nonvolatile_preserved', 'route_ok', 'helper_route_ok',
         'fixture_unchanged', 'globals_restored', 'floating_state_restored')
COUNTERS = (0, 1, 999, 1000, 0x7fffffff, 0x80000000, 0xfffffffe, 0xffffffff)
SPEEDS = (0,0x3dcccccd,0x3e4ccccc,0x3e4ccccd,0x3e4cccce,0x3effffff,0x3f000000,0x3f000001,
          0x3f4ccccc,0x3f4ccccd,0x3f4cccce,0x3fa66665,0x3fa66666,0x3fa66667,0xbf800000,0x7fc01234)
LIMITS = (0xbf800000,0,0x3f800000,0x3f7fffff,0x3f800001,0x49800000,0x4a800000,0x7f800000,0x7fc01234)
STEPS = (0., .01, .5, 1., 1.999, 3., 8., -1.)
UPS = (0xbf800000,0,0x3f800000,0x7fc01234)

def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]

def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]

def from_bits(value):
    return struct.unpack('<f', struct.pack('<I', value))[0]

def effect(fn, inp):
    """Independent defined counter effect; arithmetic rounds at each x87 PC24 operation."""
    counter = f32(float(inp['counter']))
    if 'Flying' in fn and inp['subtype'] != 0:
        return inp['counter'], 'subtype_skip'
    limit = f32(from_bits(inp['limit_bits']) * 1000.)
    if limit < counter:
        return 0, 'grant'
    if 'Flying' in fn and inp['contact_wheels']:
        return inp['counter'], 'contact_skip'
    x = from_bits(inp['speed_bits'])
    speed = f32(math.sqrt(f32(x*x)))
    if 'Driving' in fn:
        fast = speed > from_bits(0x3f4ccccd) or (inp['subtype']==0 and not inp['contact_wheels'])
        slow = speed > from_bits(0x3e4ccccd)
    else:
        fast = speed > from_bits(0x3fa66666) or from_bits(inp['up_bits']) < 0.
        slow = speed > .5
    if not fast and not slow:
        return inp['counter'], 'unchanged'
    # EXE 858B38 is the float 0.02, then *1000 and truncation to an unsigned dword.
    step = from_bits(inp['step_bits'])
    milliseconds = math.trunc(f32(f32(step * from_bits(0x3ca3d70a)) * 1000.)) & 0xffffffff
    after = math.trunc(f32(f32(f32(float(milliseconds)) * (1.5 if fast else .5)) + counter)) & 0xffffffff
    return after, 'fast' if fast else 'slow'

def expected_input(case):
    profile, value = case % 16, case // 16
    return dict(profile=profile, value=value, subtype=5 if profile//8 else 0,
                contact_wheels=1+value%4 if (profile//4)%2 else 0, up_bits=UPS[profile%4], speed_bits=SPEEDS[value%16],
                counter=COUNTERS[(value//16)%8], limit_bits=LIMITS[(value//8)%9],
                step_bits=bits(STEPS[(value//3)%8]), increment_bits=bits(float(value%3)*25.))

def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or not all(begin[0].get(k) is True for k in ('registered_hook','helper_routes_ok','floating_state_restored')):
        raise RuntimeError('Inicio sin rutas/estado flotante demostrados')
    helpers = begin[0]['helper_routes']
    if len(helpers)!=9 or {h['entry'] for h in helpers}!=HELPERS or not all(h['route_ok'] for h in helpers):
        raise RuntimeError('Colaboradores incompletos')
    for h in helpers:
        if h['registered'] and not h['reconstructed'] and h['enabled']:
            raise RuntimeError('Wrapper original habilitado')
        if h['entry'] in (0x55C180,0x55B980) and not all(h[k] for k in ('registered','reconstructed','enabled')):
            raise RuntimeError('Colaborador directo fuera de cuerpo DLL real')
    cases = {fn:set() for fn in EXPECTED}
    pairs = set()
    paths = {fn:set() for fn in EXPECTED}
    active = {fn:[] for fn in EXPECTED}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if row['pair'] in pairs or case in cases[fn]:
            raise RuntimeError('Identificador repetido')
        pairs.add(row['pair']); cases[fn].add(case)
        if row['original'] != row['candidate'] or not row.get('equal') or not row.get('accepted'):
            raise RuntimeError('Diferencia real o rechazado')
        for side in ('original','candidate'):
            value = row[side]
            if value is None or not all(value.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o gate fallido')
            if value['input'] != expected_input(case):
                raise RuntimeError('Fixture/cobertura incorrectos')
            if len(value['state']) != 18:
                raise RuntimeError('Snapshot incompleto')
            expected_counter, path_name = effect(fn, value['input'])
            if value['counter_after'] != expected_counter:
                raise RuntimeError('Efecto definido del contador incorrecto')
        paths[fn].add(path_name)
        if path_name in ('fast','slow','unchanged'):
            active[fn].append(row['original']['input'])
    if pairs != set(range(4096)) or cases != {fn:set(range(2048)) for fn in EXPECTED}:
        raise RuntimeError('Cobertura incompleta')
    for fn in EXPECTED:
        required = {'grant','fast','slow','unchanged'}
        if 'Flying' in fn:
            required |= {'subtype_skip','contact_skip'}
        if paths[fn] != required:
            raise RuntimeError('Faltan rutas completas de habilidad')
        thresholds = (0x3e4ccccd,0x3f4ccccd) if 'Driving' in fn else (0x3f000000,0x3fa66666)
        for threshold in thresholds:
            if not {threshold-1,threshold,threshold+1} <= {i['speed_bits'] for i in active[fn]}:
                raise RuntimeError('Falta borde velocidad en ruta activa')
        if 'Flying' in fn and not {0xbf800000,0,0x3f800000} <= {i['up_bits'] for i in active[fn]}:
            raise RuntimeError('Falta borde inclinacion en ruta activa')
    finals = [r for r in rows if r['event']=='complete']
    if len(finals)!=1 or finals[0]['pairs']!=4096 or not all(finals[0].get(k) is True for k in ('state_restored','floating_state_restored','helper_route_ok')):
        raise RuntimeError('Final no restaurado')
    summary['complete_paths'] = {fn:sorted(paths[fn]) for fn in EXPECTED}
    summary['coverage_method'] = 'serialized_inputs_and_independent_counter_effects; no_instruction_branch_tracing'
    return summary

if __name__ == '__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciesvehiclestats9',manifest['binaries']['asi'],analyze,'DependenciesVehicleStats9',timeout=1500))
