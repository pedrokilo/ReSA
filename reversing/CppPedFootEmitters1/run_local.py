"""Exige 3072 pares EXE-vs-DLL (1536 por emisor), trazas identicas, ABI/restauracion por par, cobertura de ramas y cero nulos."""
import json
import re
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
# El juego completo solo arranca desde la ruta real de E:; se redirige en memoria sin tocar runtime_runner.py.
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
src = (HERE / 'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED = {f'{cls}::{name}': int(cases) for cls, name, cases in re.findall(
    r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', src)}
# Cobertura minima por emisor (pares con esa ruta); no se retira para que el banco pase.
MIN_PATHS = {
    'CPed::EmitFootRainParticles': {'offscreen': 50, 'far': 50, 'emit': 400},
    'CPed::EmitFootDustParticles': {'offscreen': 50, 'far': 50, 'no_dust': 50, 'blocked_by_byte': 50, 'emit': 150},
}
MIN_CLASS = 50
MIN_RAND_CALLS_EMIT = 4
CLASSES = ['near0', 'near1', 'near2', 'near3', 'threshold', 'far', 'huge', 'pos_x_special', 'pos_y_special', 'pos_z_special',
           'pos_xy_special', 'camera_special', 'forward_special', 'forward_large', 'near_mixed', 'offscreen']


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [row for row in rows if row['event'] == 'begin']
    if len(begin) != 1 or begin[0]['registered_hook'] or not (
            begin[0]['direct_dll_call'] and begin[0]['exe_unhooked'] and begin[0]['exe_bytes_ok']
            and begin[0]['layout_ok'] and begin[0]['collaborators_ok']):
        raise RuntimeError('Precondiciones del banco incumplidas (ruta directa, EXE sin hook y con sus bytes, disposicion de datos o colaboradores)')
    cases = {name: set() for name in EXPECTED}
    paths = {name: {} for name in EXPECTED}
    classes = {name: {} for name in EXPECTED}
    add_particles = {name: 0 for name in EXPECTED}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido durante el banco')
        if row['event'] != 'comparison':
            continue
        if not row['state_restored']:
            raise RuntimeError('Estado sin restaurar en par ' + str(row['pair']))
        name = row['function']
        cases[name].add(row['case'])
        for side in ('original', 'candidate'):
            result = row[side]
            if result is None or not all(result[key] for key in (
                    'route_ok', 'stack_restored', 'nonvolatile_registers_preserved', 'protected_state_unchanged', 'x87_empty')):
                raise RuntimeError('Resultado nulo o ABI/estado invalido en par ' + str(row['pair']))
        fx = row['fixture']
        paths[name][fx['path']] = paths[name].get(fx['path'], 0) + 1
        classes[name][fx['class']] = classes[name].get(fx['class'], 0) + 1
        trace = row['original']['trace']
        emitted = sum(1 for t in trace if t[0] == 'add_particle')
        if fx['path'] == 'emit':
            if row['original']['rand_calls'] < MIN_RAND_CALLS_EMIT:
                raise RuntimeError('Ruta de emision sin llamadas a rand en par ' + str(row['pair']))
            add_particles[name] += emitted
        elif emitted:
            raise RuntimeError('Emision en una ruta de salida en par ' + str(row['pair']))
    if cases != {name: set(range(count)) for name, count in EXPECTED.items()}:
        raise RuntimeError('Casos por funcion incompletos o repetidos')
    for name, minimum in MIN_PATHS.items():
        for key, need in minimum.items():
            if paths[name].get(key, 0) < need:
                raise RuntimeError(f'Cobertura insuficiente en {name}: ruta {key} {paths[name].get(key, 0)} < {need}')
        for cls in CLASSES:
            if classes[name].get(cls, 0) < MIN_CLASS:
                raise RuntimeError(f'Cobertura insuficiente en {name}: clase {cls}')
        if add_particles[name] == 0:
            raise RuntimeError('Sin ninguna llamada a AddParticle en ' + name)
    summary['paths'] = paths
    summary['add_particle_calls'] = add_particles
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('cpppedfootemitters1', manifest['binaries']['asi'], analyze,
                 'CPed::EmitFootRainParticles / EmitFootDustParticles, EXE 0x5E3630/0x5E37C0 vs DLL', timeout=1800))
