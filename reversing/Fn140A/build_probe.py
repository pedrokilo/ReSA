"""Construye unicamente el banco Fn140A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'source/game_sa/Maths.cpp',
    'source/game_sa/Bridge.cpp',
    'source/game_sa/Bridge.h',
    'source/game_sa/Timer.cpp',
    'source/game_sa/cTransmission.cpp',
    'source/game_sa/Core/Matrix.cpp',
    'source/game_sa/Entity/Placeable.cpp',
    'source/game_sa/Entity/Physical.cpp',
    'source/game_sa/Entity/AnimatedBuilding.cpp',
    'source/game_sa/Entity/Vehicle/Trailer.cpp',
    'source/game_sa/Models/BaseModelInfo.cpp',
    'source/game_sa/Models/TimeModelInfo.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexCarDrive.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexPolicePursuit.h',
    'source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskGangHasslePed.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn140A/LiveComparison.hpp',
    'reversing/Fn140A/Core.hpp',
    'reversing/Fn140A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn140a', 'F140ALive', SOURCES)))
