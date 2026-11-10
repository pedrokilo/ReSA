"""Construye unicamente el banco Fn148A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/AttractorScanner.cpp',
    'source/game_sa/Audio/Entities/AEAudioEntity.cpp',
    'source/game_sa/Audio/Entities/AEAudioEntity.h',
    'source/game_sa/Collision/Collision.cpp',
    'source/game_sa/Core/LinkList.h',
    'source/game_sa/Fx/FxPrim.cpp',
    'source/game_sa/Fx/FxPrim.h',
    'source/game_sa/Fx/FxSphere.cpp',
    'source/game_sa/Fx/FxSphere.h',
    'source/game_sa/PedGroupIntelligence.cpp',
    'source/game_sa/PedGroupIntelligence.h',
    'source/game_sa/PedGroupMembership.cpp',
    'source/game_sa/PedGroupMembership.h',
    'source/game_sa/PedType.cpp',
    'source/game_sa/Scripts/Scripted2dEffects.cpp',
    'source/game_sa/Scripts/Scripted2dEffects.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexCarDriveMission.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDriveToPoint.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexDriveWander.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexInAirAndLand.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleChat.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleChat.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleInAir.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleLand.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn148A/LiveComparison.hpp',
    'reversing/Fn148A/Core.hpp',
    'reversing/Fn148A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp'
]
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn148a', 'F148ALive', SOURCES)))
