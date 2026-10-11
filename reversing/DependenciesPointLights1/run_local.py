"""Runner en juego completo; exige 2560 pares no nulos y restauración por par."""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer

HERE = Path(__file__).resolve().parent
EXPECTED = {'CPointLights::AddLight': 2560}

def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    pairs = [row for row in rows if row['event'] == 'comparison']
    for row in pairs:
        if row['original'] is None or row['candidate'] is None or not row['state_restored']:
            raise RuntimeError('Par nulo o sin restauración: ' + str(row['pair']))
        for result in (row['original'], row['candidate']):
            if not all(result[key] for key in ('route_ok','camera_unchanged','matrix_unchanged','entry_restored')):
                raise RuntimeError('Ruta o estado inválido: ' + str(row['pair']))
    if any(row['event'].startswith('first_chance_av') for row in rows):
        raise RuntimeError('Acceso inválido durante el banco')
    return summary

if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciespointlights1', manifest['binaries']['asi'], analyze,
                 'CPointLights::AddLight (EXE contra reconstrucción independiente)', timeout=1500))
