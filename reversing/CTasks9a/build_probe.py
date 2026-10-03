"""Compila la sonda temporal del banco de tareas complejas A (lista de 9): CTaskComplexCopInCar,
CTaskComplexEnterCarAsDriverTimed, CTaskComplexEnterCarAsPassengerTimed, CTaskComplexEvasiveDiveAndGetUp y
CTaskComplexFacial."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('CopInCar', 'EnterCarAsDriverTimed', 'EnterCarAsPassengerTimed',
                                               'EvasiveDiveAndGetUp', 'Facial') for e in ('cpp', 'h')] + [T + 'TaskSimpleFacial.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks9a', 'CTasks9aLive', SOURCES)))
