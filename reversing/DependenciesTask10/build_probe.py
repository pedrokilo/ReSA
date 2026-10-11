"""Preparation for three complete PedGroups leaves, with real private pools."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=[
 'source/game_sa/PedGroups.cpp','source/game_sa/PedGroups.h',
 'source/game_sa/PedGroup.cpp','source/game_sa/PedGroup.h',
 'source/game_sa/PedGroupMembership.cpp','source/game_sa/PedGroupMembership.h',
 'source/game_sa/PedGroupIntelligence.cpp','source/game_sa/PedGroupIntelligence.h',
 'source/game_sa/PedTaskPair.cpp','source/game_sa/PedTaskPair.h',
 'source/game_sa/References.cpp','source/game_sa/References.h','source/game_sa/Reference.h',
 'source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Entity.h',
 'source/game_sa/Entity/Ped/Ped.cpp','source/game_sa/Entity/Ped/Ped.h',
 'source/game_sa/Entity/Ped/PlayerPed.cpp','source/game_sa/Entity/Ped/PlayerPed.h',
 'source/game_sa/Tasks/Task.cpp','source/game_sa/Tasks/Task.h',
 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp',
 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h',
 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h',
 'source/game_sa/Pools.cpp','source/game_sa/Pools/Pools.h',
 'source/game_sa/Pools/TaskPool.h','source/game_sa/Core/Pool.h',
 'source/InjectHooksMain.cpp','source/reversiblehooks/ReversibleHook/Simple.cpp',
 'source/reversiblehooks/ReversibleHook/Simple.h',
 'reversing/OriginalDependencies499/HookRoute.hpp',
]
if __name__=='__main__':
    root=Path(__file__).resolve().parents[2]
    absent=[name for name in SOURCES if not (root/name).is_file()]
    if absent:raise RuntimeError('Missing source manifest paths: '+str(absent))
    print(json.dumps(build(Path(__file__).parent,'dependenciestask10','DependenciesTask10Live',SOURCES)))
