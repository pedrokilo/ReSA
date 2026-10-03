"""Compila la sonda temporal del banco de tareas complejas A (lista de 6): CTaskAllocatorKillThreatsBasic,
CTaskComplexBeStill, CTaskComplexDieInCar, CTaskComplexDriveFireTruck, CTaskComplexFallToDeath,
CTaskComplexGoToBoatSteeringWheel y CTaskComplexGoToPointAndStandStillTimed."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
A = 'source/game_sa/Tasks/Allocators/'
SOURCES = [A + f'TaskAllocatorKillThreatsBasic.{e}' for e in ('cpp', 'h')] + \
          [T + f'TaskComplex{c}.{e}' for c in ('BeStill', 'DieInCar', 'DriveFireTruck', 'FallToDeath', 'GoToBoatSteeringWheel',
                                               'GoToPointAndStandStillTimed', 'LeaveCarAndDie') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks6a', 'CTasks6aLive', SOURCES)))
