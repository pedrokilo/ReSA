"""Compare both CConversationForPed functions in a new game through the shared runner."""
import json
import shutil
import sys
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
FUNCTIONS = frozenset(('Update', 'IsPlayerInPositionForConversation'))
TARGET_SOURCES = (
    'source/game_sa/Conversations/ConversationForPed.cpp',
    'source/game_sa/Conversations/ConversationForPed.h',
)
PREFIX = 'conversation-for-ped'
sys.path.insert(0, str(HERE.parent))
import runtime_runner as runtime


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def verify_manifest(folder, probe):
    """Reject stale artifacts before any game files are touched."""
    manifest = json.loads((folder / 'manifest.json').read_text(encoding='utf-8'))
    require(manifest['probe'] is probe, 'El tipo de compilación no corresponde a esta ejecución.')
    expected = {**manifest['compiled_sources'], **manifest['workspace_sources']}
    for path, sha256 in expected.items():
        require((ROOT / path).is_file() and runtime.digest(ROOT / path) == sha256,
                f'Fuentes distintas de la compilación: {path}')
    if probe:
        for name, sha256 in manifest['instrumentation'].items():
            require((HERE / name).is_file() and runtime.digest(HERE / name) == sha256,
                    f'Instrumentación distinta de la compilación: {name}')
    for ext in ('asi', 'pdb'):
        require(runtime.digest(folder / f'gta_reversed.{ext}') == manifest[ext],
                f'Hash de artefacto .{ext} incorrecto.')
    return manifest


def analyze(path):
    if not runtime.terminal_event(path):
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    require(not any(row.get('event') in ('error', 'assertion') for row in rows),
            'El banco informó un error; consultar comparisons.jsonl.')
    pairs = [row for row in rows if row.get('event') == 'comparison']
    ends = [row for row in rows if row.get('event') == 'complete']
    require(len(ends) == 1 and rows[-1] == ends[0], 'Falta un único evento final completo.')
    end = ends[0]
    require(end.get('state_restored') is True, 'El banco no restauró el estado del motor.')
    expected = end.get('expected')
    require(type(expected) is int and expected > 0 and len(pairs) == end.get('pairs') == expected,
            'Cantidad de pares incompleta.')
    require([row.get('pair') for row in pairs] == list(range(expected)), 'Pares omitidos o duplicados.')
    counts = Counter(row.get('function') for row in pairs)
    require(set(counts) == FUNCTIONS, 'Deben compararse las dos funciones y solamente ellas.')
    require(dict(counts) == end.get('function_pairs'), 'Los conteos por función no coinciden con el cierre.')
    for row in pairs:
        original = row.get('original')
        candidate = row.get('candidate')
        require(row.get('equal') is True and isinstance(original, dict) and original == candidate,
                f'Diferencia en {row.get("function")}, par {row.get("pair")}.')
        require(original.get('inputs_intact') is True, f'Entradas alteradas en el par {row["pair"]}.')
        require(row.get('original_candidate_calls') == 0 and row.get('candidate_calls') == 1,
                f'El aislamiento original/candidato falló en el par {row["pair"]}.')
    return {
        'pairs': len(pairs), 'function_pairs': dict(counts), 'all_equal': True,
        'state_restored': True, 'log_sha256': runtime.digest(path),
    }


def main():
    folder = Path(json.loads((HERE / 'latest-probe.json').read_text(encoding='utf-8'))['folder'])
    manifest = verify_manifest(folder, probe=True)
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
        code = runtime.run_case(PREFIX, manifest['asi'], analyze, 'CConversationForPed: Update y posición',
                                timeout=180, unhook='', wait_for_exit=True)
    finally:
        runtime.probe_configuration = configure
    if code == 0:
        reports = set((runtime.GAME / f'resultados-{PREFIX}').glob('*/summary.json')) - reports_before
        require(len(reports) == 1, 'No se pudo identificar el informe de esta ejecución.')
        report = reports.pop()
        summary = json.loads(report.read_text(encoding='utf-8'))
        require(summary.get('natural_exit') is True and summary.get('exit_code') == 0,
                'La evidencia requiere cierre natural del juego.')
        summary.update(
            build_manifest=str(folder / 'manifest.json'), report=str(report),
            source_sha256={p: manifest['workspace_sources'][p] for p in TARGET_SOURCES},
            scope='Both functions with independent original/candidate executions, controlled dependency providers, '
                  'exact state and trace comparison. This does not establish audio playback or prolonged gameplay stability.',
        )
        (HERE / 'validation-summary.json').write_text(json.dumps(summary, indent=2), encoding='utf-8')
    return code


if __name__ == '__main__':
    sys.exit(main())
