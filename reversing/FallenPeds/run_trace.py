"""Capture naturally occurring bad pedestrian positions; not an equivalence test."""
import json
import shutil
import sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
from runtime_runner import run_case, digest, terminal_event, GAME

def main():
    manifest = json.loads((HERE / 'artifacts/trace/manifest.json').read_text())
    for ext in ('asi', 'pdb'):
        src = HERE / f'artifacts/trace/gta_reversed-fallen-peds-probe.{ext}'
        assert digest(src) == manifest[ext]
        shutil.copy2(src, GAME / f'_baseline/{src.name}')
    def analyze(path):
        if not terminal_event(path):
            return None
        rows = [json.loads(line) for line in path.read_text().splitlines()]
        if rows[-1]['event'] != 'complete':
            raise RuntimeError(str(rows[-1]))
        return dict(scope='Diagnostic observation, not equivalence', probe_sha256=manifest['asi'], trace=rows[-1], fallen=sum(r['event']=='fallen' for r in rows))
    return run_case('fallen-peds', manifest['asi'], analyze, 'Captura de posiciones de peatones', timeout=420, stop_before_analysis=True, unhook='')

if __name__ == '__main__':
    sys.exit(main())
