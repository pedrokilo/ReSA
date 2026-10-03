"""Compila la sonda temporal del banco de tareas complejas B (lista de 8): CTaskComplexGetOnBoatSeat,
CTaskComplexHitByGunResponse, CTaskComplexInvestigateDisturbance, CTaskComplexLeaveBoat, CTaskComplexLeaveCarAndFlee
y CTaskComplexLeaveCarAndWander."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('GetOnBoatSeat', 'HitByGunResponse', 'InvestigateDisturbance', 'LeaveBoat',
                                               'LeaveCarAndFlee', 'LeaveCarAndWander') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks8b', 'CTasks8bLive', SOURCES)))
