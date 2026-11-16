"""Construye unicamente el banco Fn205A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Attractors/PedAttractor.cpp',
    'source/game_sa/Attractors/PedShelterAttractor.cpp',
    'source/game_sa/Attractors/PedShelterAttractor.h',
    'source/game_sa/PedAttractorManager.cpp',
    'source/game_sa/Events/EventAcquaintancePed.cpp',
    'source/game_sa/Events/EventAttractor.cpp',
    'source/game_sa/Events/EventGunAimedAt.cpp',
    'source/game_sa/Events/EventKnockOffBike.cpp',
    'source/game_sa/Events/EventPedToChase.cpp',
    'source/game_sa/Events/EventPedToFlee.cpp',
    'source/game_sa/Events/EventScriptCommand.cpp',
    'source/game_sa/Events/EventSexyVehicle.cpp',
    'source/game_sa/Events/EventVehicleThreat.cpp',
    'source/game_sa/Events/EventVehicleToSteal.cpp',
    'source/game_sa/Events/Event.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidEntity.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexAvoidOtherPedWhileWandering.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexWanderGang.h',
    'source/game_sa/Tasks/Task.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn205A/LiveComparison.hpp',
    'reversing/Fn205A/Core.hpp',
    'reversing/Fn205A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn205a', 'F205ALive', SOURCES)))
