"""Compila la sonda temporal del banco de respuesta editable, conducir, ser arrastrado y huir a un punto (lista de 11):
CEventEditableResponse, CTaskComplexCarDrive, CTaskComplexCarSlowBeDraggedOut y CTaskComplexFleePoint."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
SOURCES = [G + f'Events/EventEditableResponse.{e}' for e in ('cpp', 'h')] + \
          [T + f'TaskComplex{c}.{e}' for c in ('CarDrive', 'CarSlowBeDraggedOut', 'FleePoint') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks11', 'CTasks11Live', SOURCES)))
