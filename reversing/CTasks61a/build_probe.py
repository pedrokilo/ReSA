"""Compila la sonda temporal del banco CTasks61a: tareas de coche (conducir, bajar, subir de conductor/pasajero, esperar, ir a punto,
apoyarse en el vehiculo y puerta de pasajero), 9 clases de tareas."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
CLASSES = ('TaskSimpleCarDrive', 'TaskSimpleCarDriveTimed', 'TaskSimpleCarSetPedInAsDriver', 'TaskSimpleCarSetPedInAsPassenger',
           'TaskSimpleCarSetPedOut', 'TaskComplexDriveToPoint', 'TaskComplexEnterCarAsPassengerWait', 'TaskComplexOpenPassengerDoor',
           'TaskLeanOnVehicle')
SOURCES = [T + f'{c}.{e}' for c in CLASSES for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks61a', 'CTasks61aLive', SOURCES)))
