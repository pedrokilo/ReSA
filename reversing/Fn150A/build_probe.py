"""Construye unicamente el banco Fn150A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Audio/Hardware/AEStreamingChannel.cpp',
    'source/game_sa/Collision/ColModel.cpp',
    'source/game_sa/Core/PtrNodeDoubleLink.cpp',
    'source/game_sa/Core/PtrNodeDoubleLink.h',
    'source/game_sa/Core/QuadTreeNode.h',
    'source/game_sa/Core/Vector.cpp',
    'source/game_sa/Entity/Entity.cpp',
    'source/game_sa/EntryExit.cpp',
    'source/game_sa/Garages.cpp',
    'source/game_sa/Garages.h',
    'source/game_sa/Models/BaseModelInfo.cpp',
    'source/game_sa/Pools/PtrNodeDoubleLinkPool.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleAnim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGiveCPR.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.h',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp',
    'source/game_sa/X87Math.cpp',
    'source/game_sa/X87Math.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn150A/LiveComparison.hpp',
    'reversing/Fn150A/Core.hpp',
    'reversing/Fn150A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn150a', 'F150ALive', SOURCES)))
