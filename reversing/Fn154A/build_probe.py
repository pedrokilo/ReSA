"""Construye unicamente el banco Fn154A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Audio/Entities/AEAudioEntity.cpp',
    'source/game_sa/Core/Matrix.cpp',
    'source/game_sa/Core/Matrix.h',
    'source/game_sa/Events/EventHandlerHistory.cpp',
    'source/game_sa/Events/EventObjectCollision.cpp',
    'source/game_sa/Events/EventObjectCollision.h',
    'source/game_sa/PedIntelligence.cpp',
    'source/game_sa/PedIntelligence.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexSignalAtPed.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleFall.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleFall.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn154A/LiveComparison.hpp',
    'reversing/Fn154A/Core.hpp',
    'reversing/Fn154A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn154a', 'F154ALive', SOURCES)))
