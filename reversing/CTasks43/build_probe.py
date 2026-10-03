"""Compila la sonda temporal del banco de tareas complejas (lista de 43): CTaskComplexAttractorPartnerWait,
CTaskComplexCarDriveMission, CTaskComplexCarDriveWander, CTaskComplexDie, CTaskComplexEnterAnyCarAsDriver,
CTaskComplexGetUpAndStandStill, CTaskComplexObserveTrafficLightsAndAchieveHeading y CTaskComplexSeekEntityAnyMeansXYOffset."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
S = T + 'SeekEntity/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('AttractorPartnerWait', 'CarDriveMission', 'CarDrive', 'DriveWander', 'Die',
                                               'EnterAnyCarAsDriver', 'GetUpAndStandStill',
                                               'ObserveTrafficLightsAndAchieveHeading') for e in ('cpp', 'h')] + \
          [S + f'TaskComplexSeekEntityAnyMeansXYOffset.{e}' for e in ('cpp', 'h')] + [S + 'TaskComplexSeekEntityAnyMeans.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks43', 'CTasks43Live', SOURCES)))
