"""Construye unicamente el banco Fn178A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/StdInc.h',
    'source/game_sa/Entity/Dummy/DummyPed.cpp',
    'source/game_sa/Entity/Dummy/DummyPed.h',
    'source/game_sa/Entity/Ped/CopPed.cpp',
    'source/game_sa/Entity/Ped/CopPed.h',
    'source/game_sa/Entity/Ped/EmergencyPed.cpp',
    'source/game_sa/Entity/Ped/EmergencyPed.h',
    'source/game_sa/Entity/Ped/Ped.h',
    'source/game_sa/Entity/Vehicle/Plane.cpp',
    'source/game_sa/Entity/Vehicle/QuadBike.cpp',
    'source/game_sa/Events/EventAcquaintancePed.cpp',
    'source/game_sa/Events/EventAcquaintancePedHateBadlyLit.cpp',
    'source/game_sa/Events/EventAcquaintancePedHateBadlyLit.h',
    'source/game_sa/Events/EventVehicleThreat.cpp',
    'source/game_sa/Events/EventVehicleThreat.h',
    'source/game_sa/Interior/InteriorManager_c.cpp',
    'source/game_sa/Interior/InteriorManager_c.h',
    'source/game_sa/Replay.cpp',
    'source/game_sa/Replay.h',
    'source/game_sa/ReplayBlockType.cpp',
    'source/game_sa/WinsockLink.cpp',
    'source/game_sa/WinsockLink.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn178A/LiveComparison.hpp',
    'reversing/Fn178A/Core.hpp',
    'reversing/Fn178A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn178a', 'F178ALive', SOURCES)))
