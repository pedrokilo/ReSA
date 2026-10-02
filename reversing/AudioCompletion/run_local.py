"""Compara dentro del motor el lote de audio (Ambience, StaticChannel, Weather) con el runner común.

La sonda ejecuta original y candidato por separado con proveedores externos controlados
y se detiene en la primera diferencia. Los casos van en orden: primero
UpdateAmbienceTrackAndVolume, después AddAudioEvent y por último SetAudioBuffer. El
análisis informa por función qué se comparó por completo y dónde apareció la primera
diferencia, si la hubo.
"""
import json
import shutil
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
PREFIX = 'audio-completion'
FUNCTIONS = ('UpdateAmbienceTrackAndVolume', 'AddAudioEvent', 'SetAudioBuffer')
sys.path.insert(0, str(HERE.parent))
import runtime_runner as runtime


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def verify_manifest(folder):
    """Rechaza artefactos obsoletos antes de tocar archivos del juego."""
    manifest = json.loads((folder / 'manifest.json').read_text(encoding='utf-8'))
    require(manifest['probe'] is True, 'El artefacto no es una sonda.')
    for path, sha256 in {**manifest['compiled_sources'], **manifest['workspace_sources']}.items():
        require((ROOT / path).is_file() and runtime.digest(ROOT / path) == sha256, f'Fuentes distintas de la compilación: {path}')
    for name, sha256 in manifest['instrumentation'].items():
        require((HERE / name).is_file() and runtime.digest(HERE / name) == sha256, f'Instrumentación distinta: {name}')
    for ext in ('asi', 'pdb'):
        require(runtime.digest(folder / f'gta_reversed.{ext}') == manifest[ext], f'Hash de artefacto .{ext} incorrecto.')
    return manifest


def analyze(path):
    if not runtime.terminal_event(path):
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    require(not any(row.get('event') == 'assertion' for row in rows), 'Assert dentro del banco; consultar el registro.')
    pairs = [row for row in rows if row.get('event') == 'comparison']
    end = rows[-1]
    require(end.get('event') in ('complete', 'error') and end.get('state_restored') is True, 'El banco no restauró el estado del motor.')
    require([row.get('pair') for row in pairs] == list(range(len(pairs))), 'Pares omitidos o duplicados.')
    for row in pairs:
        require(row.get('original_candidate_calls') == 0 and row.get('original_entry') is True,
                f'Aislamiento original/candidato roto en el par {row["pair"]}.')
    first_difference = next((row for row in pairs if row.get('equal') is not True), None)
    counts = Counter(row.get('function') for row in pairs if row.get('equal') is True)
    complete = end.get('event') == 'complete' and first_difference is None and end.get('pairs') == end.get('expected')
    # Una función queda cubierta si todos sus casos se compararon sin diferencia: o bien el banco
    # terminó, o bien la primera diferencia apareció en una función posterior en el orden de casos.
    order = [row.get('function') for row in pairs]
    covered = {}
    for name in FUNCTIONS:
        if complete:
            covered[name] = name in counts
        elif first_difference is not None:
            later = first_difference['function'] != name and name in order and order.index(name) < order.index(first_difference['function'])
            covered[name] = later
        else:
            covered[name] = False
    return {
        'pairs': len(pairs), 'equal_pairs_per_function': dict(counts), 'expected_pairs': end.get('expected'),
        'complete': complete, 'covered_functions': covered,
        'first_difference': None if first_difference is None else {
            'pair': first_difference['pair'], 'function': first_difference['function'], 'case': first_difference['case'],
            'original': first_difference['original'], 'candidate': first_difference['candidate'],
            'candidate_calls': first_difference['candidate_calls']},
        'state_restored': True, 'log_sha256': runtime.digest(path),
    }


def main():
    folder = Path(json.loads((HERE / 'latest-probe.json').read_text(encoding='utf-8'))['folder'])
    manifest = verify_manifest(folder)
    for ext in ('asi', 'pdb'):
        shutil.copy2(folder / f'gta_reversed.{ext}', runtime.GAME / f'_baseline/gta_reversed-{PREFIX}-probe.{ext}')
    configure = runtime.probe_configuration

    def new_game(data):
        text = configure(data)
        require(b'SaveGameToLoad = 2\n' in text, 'No se encontró la opción temporal de partida.')
        return text.replace(b'SaveGameToLoad = 2\n', b'SaveGameToLoad = 0\n')

    reports_before = set((runtime.GAME / f'resultados-{PREFIX}').glob('*/summary.json'))
    runtime.probe_configuration = new_game
    try:
        code = runtime.run_case(PREFIX, manifest['asi'], analyze, 'Audio: Ambience, StaticChannel y Weather',
                                timeout=240, unhook='', wait_for_exit=True)
    finally:
        runtime.probe_configuration = configure
    reports = set((runtime.GAME / f'resultados-{PREFIX}').glob('*/summary.json')) - reports_before
    if len(reports) == 1:
        report = reports.pop()
        summary = json.loads(report.read_text(encoding='utf-8'))
        summary.update(build_manifest=str(folder / 'manifest.json'), report=str(report),
                       source_sha256={p: manifest['workspace_sources'][p] for p in manifest['workspace_sources'] if 'Audio' in p},
                       scope='Ejecuciones independientes original/candidato con proveedores externos controlados; compara '
                             'estado completo y traza de llamadas. No acredita la reproducción de audio ni la estabilidad prolongada.')
        (HERE / 'validation-summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    return code


if __name__ == '__main__':
    sys.exit(main())
