"""Compila la sonda temporal del banco de tareas complejas A (lista de 7): CTaskComplexChat,
CTaskComplexDiveFromAttachedEntityAndGetUp, CTaskComplexDrivePointRoute, CTaskComplexGoToPointAndStandStill,
CTaskComplexHitResponse y CTaskComplexInWater."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('Chat', 'DiveFromAttachedEntityAndGetUp', 'DrivePointRoute',
                                               'GoToPointAndStandStill', 'HitResponse', 'InWater') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks7a', 'CTasks7aLive', SOURCES)))
