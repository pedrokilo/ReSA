"""Compila la sonda temporal del banco de helicoptero, ir a la puerta del coche y nadar (lista de 11):
CHeli, CTaskComplexGoToCarDoorAndStandStill y CTaskSimpleSwim."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/game_sa/Entity/Vehicle/Heli.cpp', 'source/game_sa/Entity/Vehicle/Heli.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexGoToCarDoorAndStandStill.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSwim.h',
]
SOURCES += ['reversing/CppTrigCompound6Swim/LiveComparison.hpp', 'reversing/CppTrigCompound6Swim/build_probe.py', 'source/game_sa/TrigCompound6Cpp.cpp', 'source/game_sa/TrigCompound6Cpp.h', 'source/CMakeLists.txt', 'CMakeLists.txt', 'source/InjectHooksMain.cpp', 'reversing/ProbeSupport.hpp', 'reversing/ProbeRuntime.hpp', 'reversing/CppTrigCompound6Swim/audit_nested.py', 'reversing/CppTrigCompound6Swim/run_local.py']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cpptrigcompound6swim', 'CppTrigCompound6SwimLive', SOURCES)))
