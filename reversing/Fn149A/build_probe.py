"""Construye unicamente el banco Fn149A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/app/securom/Securom.cpp',
    'source/app/securom/Securom.h',
    'source/game_sa/Audio/Hardware/AEAudioChannel.cpp',
    'source/game_sa/Audio/Hardware/AEAudioChannel.h',
    'source/game_sa/Entity/Building.cpp',
    'source/game_sa/Events/EventBuildingCollision.cpp',
    'source/game_sa/Events/EventBuildingCollision.h',
    'source/game_sa/Events/EventDraggedOutCar.cpp',
    'source/game_sa/PedGroupIntelligence.cpp',
    'source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.cpp',
    'source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorRandom.h',
    'source/game_sa/Tasks/TaskTypes/TaskComplexEnterAnyCarAsDriver.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexEnterCar.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCower.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimplePause.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForBus.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleWaitForPizza.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn149A/LiveComparison.hpp',
    'reversing/Fn149A/Core.hpp',
    'reversing/Fn149A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn149a', 'F149ALive', SOURCES)))
