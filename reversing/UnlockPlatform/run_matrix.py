"""Matriz serial y autónoma de arranque, con restauración entre procesos."""
import json
import subprocess
import sys
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import GAME

MASKS = [127, 0, 126, 125, 123, 119, 111, 95, 63]


def main():
    here = Path(__file__).resolve().parent
    folder = here / 'artifacts' / datetime.now().strftime('matrix-%Y%m%d-%H%M%S')
    folder.mkdir(parents=True)
    report = dict(passed=False, masks=MASKS, runs=[])
    try:
        for mask in MASKS:
            before = set((GAME / 'resultados-unlock-platform').glob('*/summary.json'))
            print(f'INICIO máscara {mask}', flush=True)
            process = subprocess.run([sys.executable, str(here / 'run_local.py'), '--mask', str(mask)])
            fresh = set((GAME / 'resultados-unlock-platform').glob('*/summary.json')) - before
            if len(fresh) != 1:
                raise RuntimeError('Falta un informe único de la ejecución.')
            path = fresh.pop()
            summary = json.loads(path.read_text(encoding='utf-8'))
            report['runs'].append(dict(mask=mask, evidence_directory=str(path.parent), summary=summary))
            if process.returncode or not summary['passed']:
                raise RuntimeError(f'Falló máscara {mask}; se detiene la matriz.')
        report['passed'] = True
        report['pairs'] = sum(r['summary']['pairs'] for r in report['runs'])
    except Exception as error:
        report['error'] = str(error)
    finally:
        (folder / 'summary.json').write_text(json.dumps(report, indent=2), encoding='utf-8')
        print('Informe de matriz: ' + str(folder / 'summary.json'), flush=True)
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    sys.exit(main())
