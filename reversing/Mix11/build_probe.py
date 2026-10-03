"""Compila la sonda temporal del banco de helicoptero, ir a la puerta del coche y nadar (lista de 11):
CHeli, CTaskComplexGoToCarDoorAndStandStill y CTaskSimpleSwim."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [
    'source/game_sa/Entity/Vehicle/Heli.cpp', 'source/game_sa/Entity/Vehicle/Heli.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h',
]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'mix11', 'Mix11Live', SOURCES)))
