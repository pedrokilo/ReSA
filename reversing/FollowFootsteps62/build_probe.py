"""Construye unicamente el banco FollowFootsteps62 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'followfootsteps62', 'FollowFootsteps62Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskComplexFollowPedFootsteps.h',
         'source/game_sa/Tasks/TaskManager.cpp',
         'source/game_sa/Tasks/TaskManager.h',
         'source/game_sa/Tasks/Task.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleGoToPoint.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleHitHead.cpp',
         'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp',
         'source/game_sa/Entity/Ped/Ped.cpp',
         'source/game_sa/Entity/Ped/Ped.h',
         'source/game_sa/World.cpp',
         'source/game_sa/Core/Vector.cpp',
         'source/game_sa/Entity/Entity.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/FollowFootsteps62/LiveComparison.hpp', 'reversing/FollowFootsteps62/Core.hpp', 'reversing/FollowFootsteps62/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
         'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp'])))
