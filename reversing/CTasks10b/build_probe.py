"""Compila la sonda temporal del banco de tareas de cobertura, cambio de asiento y coche (lista de 10):
CTaskComplexSeekCoverUntilTargetDead, CTaskComplexShuffleSeats, CTaskSimpleCarAlign y CTaskSimpleCarCloseDoorFromInside."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
TASKS = ('TaskComplexSeekCoverUntilTargetDead', 'TaskComplexShuffleSeats', 'TaskSimpleCarAlign', 'TaskSimpleCarCloseDoorFromInside')
SOURCES = [T + f'{c}.{e}' for c in TASKS for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks10b', 'CTasks10bLive', SOURCES)))
