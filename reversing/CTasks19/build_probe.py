"""Compila la sonda temporal del banco de tareas complejas y repartidor (lista de 19): CTask, CTaskAllocatorKillOnFoot,
CTaskComplexCarDriveMissionFleeScene/KillPed, CTaskComplexClimb, CTaskComplexInAirAndLand, CTaskComplexOpenDriverDoor,
CTaskComplexWanderGang y CTaskComplexWanderMedic."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
SOURCES = [G + f'Tasks/Task.{e}' for e in ('cpp', 'h')] + [G + 'Pools/TaskPool.h'] + \
          [G + f'Tasks/Allocators/TaskAllocatorKillOnFoot.{e}' for e in ('cpp', 'h')] + \
          [T + f'TaskComplex{c}.{e}' for c in ('CarDriveMission', 'CarDrive', 'Climb', 'InAirAndLand', 'OpenDriverDoor',
                                               'Wander', 'WanderGang', 'WanderMedic') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks19', 'CTasks19Live', SOURCES)))
