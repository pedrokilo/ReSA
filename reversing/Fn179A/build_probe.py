"""Construye unicamente el banco Fn179A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Collision/ColStore.cpp',
    'source/game_sa/Entity/Ped/PlayerPed.cpp',
    'source/game_sa/Events/EventDontJoinPlayerGroup.cpp',
    'source/game_sa/Events/EventDontJoinPlayerGroup.h',
    'source/game_sa/Events/EventNewGangMember.cpp',
    'source/game_sa/Events/EventNewGangMember.h',
    'source/game_sa/Events/EventPlayerCommandToGroupGather.cpp',
    'source/game_sa/Events/EventPlayerCommandToGroupGather.h',
    'source/game_sa/Tasks/Allocators/TaskAllocator.cpp',
    'source/game_sa/Tasks/Allocators/TaskAllocatorAttack.cpp',
    'source/game_sa/Tasks/Allocators/TaskAllocatorAttack.h',
    'source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.cpp',
    'source/game_sa/Tasks/Allocators/TaskAllocatorKillOnFoot.h',
    'source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsBasic.cpp',
    'source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.cpp',
    'source/game_sa/Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h',
    'source/game_sa/Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPointFine.cpp',
    'source/game_sa/Weather.cpp',
    'source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn179A/LiveComparison.hpp',
    'reversing/Fn179A/Core.hpp',
    'reversing/Fn179A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn179a', 'F179ALive', SOURCES)))
