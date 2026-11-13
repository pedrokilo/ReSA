"""Construye unicamente el banco Fn181A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Entity/Entity.cpp',
    'source/game_sa/Remote.cpp',
    'source/game_sa/Remote.h',
    'source/game_sa/Core/Matrix.cpp',
    'source/game_sa/Entity/Physical.cpp',
    'source/game_sa/Events/EventKnockOffBike.cpp',
    'source/game_sa/Events/EventKnockOffBike.h',
    'source/game_sa/Events/EventGunAimedAt.cpp',
    'source/game_sa/VehicleAnimGroupData.cpp',
    'source/game_sa/DecisionMakers/DecisionMaker.cpp',
    'source/game_sa/DecisionMakers/DecisionMakerTypes.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDieInCar.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleDrownInCar.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDie.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDie.h',
    'source/game_sa/TxdStore.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn181A/LiveComparison.hpp',
    'reversing/Fn181A/Core.hpp',
    'reversing/Fn181A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn181a', 'F181ALive', SOURCES)))
