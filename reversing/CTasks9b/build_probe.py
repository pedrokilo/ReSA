"""Compila la sonda temporal del banco de tareas complejas B (lista de 9): CTaskComplexFallAndGetUp,
CTaskComplexInvestigateDeadPed, CTaskComplexKillPedGroupOnFoot, CTaskComplexMedicTreatInjuredPed y CTaskComplexOnFire."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('FallAndGetUp', 'InvestigateDeadPed', 'KillPedGroupOnFoot', 'MedicTreatInjuredPed',
                                               'OnFire') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks9b', 'CTasks9bLive', SOURCES)))
