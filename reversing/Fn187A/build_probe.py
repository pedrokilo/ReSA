"""Construye unicamente el banco Fn187A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Entity/Ped/PlayerPed.cpp',
    'source/game_sa/Entity/Ped/PlayerPed.h',
    'source/game_sa/Events/EventAttractor.cpp',
    'source/game_sa/Events/EventAttractor.h',
    'source/game_sa/Events/EventGunAimedAt.cpp',
    'source/game_sa/Events/EventInAir.cpp',
    'source/game_sa/Events/EventInAir.h',
    'source/game_sa/Events/EventKnockOffBike.cpp',
    'source/game_sa/Events/EventKnockOffBike.h',
    'source/game_sa/Events/EventVehicleThreat.cpp',
    'source/game_sa/Events/EventVehicleThreat.h',
    'source/game_sa/Events/EventVehicleToSteal.cpp',
    'source/game_sa/Events/EventVehicleToSteal.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleDuck.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleDuckWhileShotsWhizzing.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleShakeFist.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn187A/LiveComparison.hpp',
    'reversing/Fn187A/Core.hpp',
    'reversing/Fn187A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn187a', 'F187ALive', SOURCES)))
