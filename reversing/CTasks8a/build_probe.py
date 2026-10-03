"""Compila la sonda temporal del banco de tareas complejas A (lista de 8): CTaskComplexEnterBoatAsDriver,
CTaskComplexExtinguishFires, CTaskComplexFleeAnyMeans, CTaskComplexFleeEntity, CTaskComplexFleeShooting y
CTaskComplexFollowLeaderAnyMeans."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('EnterBoatAsDriver', 'ExtinguishFires', 'FleeAnyMeans', 'FleeEntity',
                                               'FleeShooting', 'FollowLeaderAnyMeans') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks8a', 'CTasks8aLive', SOURCES)))
