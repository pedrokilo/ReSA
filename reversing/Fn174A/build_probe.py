"""Construye unicamente el banco Fn174A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/ControllerConfigManager.cpp',
    'source/game_sa/Entity/Vehicle/Heli.cpp',
    'source/game_sa/Entity/Vehicle/MonsterTruck.cpp',
    'source/game_sa/Entity/Vehicle/Train.cpp',
    'source/game_sa/Entity/Vehicle/Train.h',
    'source/game_sa/Font.cpp',
    'source/game_sa/Font.h',
    'source/game_sa/MenuManager.cpp',
    'source/game_sa/Pad.cpp',
    'source/game_sa/Stats.cpp',
    'source/game_sa/TrainNode.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'source/game_sa/cHandlingDataMgr.cpp',
    'source/game_sa/cTransmission.cpp',
    'source/game_sa/cTransmission.h',
    'source/game_sa/common.cpp',
    'source/game_sa/common.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn174A/LiveComparison.hpp',
    'reversing/Fn174A/Core.hpp',
    'reversing/Fn174A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn174a', 'F174ALive', SOURCES)))
