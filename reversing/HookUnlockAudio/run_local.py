"""Matriz de ocho contextos, seguida por ejecuciones reales sin stubs."""

import json

import re

import sys

from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

import runtime_runner

runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')

from probe_case import analyzer

HERE = Path(__file__).resolve().parent
ORIGINAL_AUDIO_PROFILE = '--original-audio-profile' in sys.argv[1:]
ARGS = [arg for arg in sys.argv[1:] if arg != '--original-audio-profile']
UNHOOK_PROFILE = 'Audio,Global/SurfaceInfos_c' if ORIGINAL_AUDIO_PROFILE else 'Global/SurfaceInfos_c'


tables = ''.join(p.read_text(encoding='utf-8') for p in sorted(HERE.glob('CasesG*.hpp')))

EXPECTED = {f'{m[0]}::{m[1]}': int(m[2])*8 for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', tables)}

EXPECTED = {k:v for k,v in EXPECTED.items() if k.split("::")[-1] in {"ProcessPlayerVehicleEngine", "ProcessVehicle", "Service"}}

base = analyzer(EXPECTED)

def analyze(path):

    result = base(path)

    if result is None:

        return None

    rows = [json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]

    frames = [r for r in rows if r.get('event') == 'live_audio_execution']

    if len(frames) != 960 or not all(r['route_ok'] and r['stack_ok'] and r['transition_ok'] and r.get('inputs_restored') is True for r in frames):

        raise RuntimeError('Ejecuci�n real incompleta o fallo de pila/ruta/transici�n')

    if any(r['original'] is None or r['candidate'] is None for r in rows if r.get('event') == 'comparison'):

        raise RuntimeError('Comparaci�n nula')

    for kind in range(4):

        for mask in range(8):

            selected = [r for r in frames if r['mask'] == mask and r['vehicle_kind'] == kind]

            counts = [sum(c['function'] == i for r in selected for c in r['calls']) for i in range(3)]

            expected = [20 if kind < 2 else 0, 30, 30]

            if len(selected) != 30 or counts != expected or (kind < 2 and not any(r['bank_loaded'] and r['player_driver'] for r in selected)):

                raise RuntimeError(f'Cobertura insuficiente: tipo {kind}, m�scara {mask}, {counts}')

            if kind < 2:
                loaded = [r for r in selected if r['bank_loaded'] and r['player_driver'] and any(c['function'] == 0 for c in r['calls'])]
                inputs = {r['accelerate_actual'] for r in loaded}
                if not {0, 128, 255}.issubset(inputs):
                    raise RuntimeError(f'Acelerador real sin cobertura cargada: tipo {kind}, mascara {mask}, valores {sorted(inputs)}')
                if not any(r['brake_actual'] > 0 for r in selected if r['player_driver']):
                    raise RuntimeError(f'Freno real sin cobertura: tipo {kind}, mascara {mask}')
                if not any(channel['sound'] != 0 for r in loaded for call in r['calls'] if call['function'] == 0 for channel in call['channels']):
                    raise RuntimeError(f'Motor sin sonidos reales: tipo {kind}, mascara {mask}')
                if any(r['player_driver'] != (r['local_frame'] < 10 or r['local_frame'] >= 20) for r in selected):
                    raise RuntimeError(f'Salida/reentrada real incompleta: tipo {kind}, mascara {mask}')
    if any(r.get('event', '').startswith('first_chance_av') for r in rows):

        raise RuntimeError('Acceso inválido observado')

    result.update(live_frames=960, live_explicit_calls=2240, isolated_pair_contexts=8, live_mixed_contexts=32,
        audio_profile='original_audio' if ORIGINAL_AUDIO_PROFILE else 'active_audio', unhook_profile=UNHOOK_PROFILE)

    return result

if __name__ == '__main__':

    if len(ARGS) == 2 and ARGS[0] == '--analyze':

        print(json.dumps(analyze(Path(ARGS[1])), indent=2))

    else:

        manifest = json.loads((HERE/'artifacts/build-manifest.json').read_text())

        sys.exit(runtime_runner.run_case('hookunlockaudio', manifest['binaries']['asi'], analyze,

            'Conmutación individual y combinada del audio del vehículo', timeout=7200,

            unhook=UNHOOK_PROFILE))

