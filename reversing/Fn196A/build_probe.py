"""Construye unicamente el banco Fn196A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/app/platform/win/VideoMode.cpp',
    'source/app/platform/win/VideoMode.h',
    'source/app/platform/win/WinInput.cpp',
    'source/app/platform/win/WinInput.h',
    'source/app/platform/win/WinPs.cpp',
    'source/game_sa/Core/Matrix.cpp',
    'source/game_sa/Entity/Vehicle/Vehicle.cpp',
    'source/game_sa/Entity/Vehicle/Vehicle.h',
    'source/game_sa/Events/EventAcquaintancePed.cpp',
    'source/game_sa/Events/EventAcquaintancePed.h',
    'source/game_sa/Events/EventHealthLow.cpp',
    'source/game_sa/Events/EventHealthLow.h',
    'source/game_sa/Events/EventSeenCop.cpp',
    'source/game_sa/Events/EventSeenCop.h',
    'source/game_sa/GangWarsSaveStructure.cpp',
    'source/game_sa/GangWarsSaveStructure.h',
    'source/game_sa/Pad.cpp',
    'source/game_sa/Pad.h',
    'source/game_sa/PlayerInfo.cpp',
    'source/game_sa/PlayerInfo.h',
    'source/game_sa/PlayerInfoSaveStructure.cpp',
    'source/game_sa/PlayerInfoSaveStructure.h',
    'source/game_sa/Timer.cpp',
    'source/game_sa/Timer.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn196A/LiveComparison.hpp',
    'reversing/Fn196A/Core.hpp',
    'reversing/Fn196A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn196a', 'F196ALive', SOURCES)))
