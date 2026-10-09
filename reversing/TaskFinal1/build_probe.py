"""Compila la sonda temporal del banco TaskFinal1 (constructores y destructores de CTaskUtilityLineUpPedWithCar,
CTaskGoToVehicleAndLean, CTaskGangHassleVehicle, CTaskGangHasslePed y CTaskComplexPlayHandSignalAnim)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
# Ruta real de la copia de prueba (el junction D:/JUEGOS ya no existe). Se redirige en memoria, antes de importar
# probe_build (que enlaza GAME al importarse), sin tocar runtime_runner.py (hash congelado).
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'{c}.{e}' for c in ('TaskUtilityLineUpPedWithCar', 'TaskGoToVehicleAndLean', 'TaskGangHassleVehicle',
                                    'TaskGangHasslePed', 'TaskComplexPlayHandSignalAnim') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'taskfinal1', 'TaskFinal1Live', SOURCES)))
