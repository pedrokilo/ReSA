"""Compila la sonda temporal del quinto banco de tareas (WaitAtAttractor, WaitForBackup, WaitForBus, WaitForDryWeather, WalkAlongsidePed)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('WaitAtAttractor', 'WaitForBackup', 'WaitForBus', 'WaitForDryWeather', 'WalkAlongsidePed') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'tasks5', 'Tasks5Live', SOURCES)))
