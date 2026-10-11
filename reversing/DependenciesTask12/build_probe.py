"""Preparación offline del destructor Fight completo con animación real."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h', 'source/game_sa/Tasks/Task.cpp', 'source/game_sa/Tasks/Task.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h', 'source/game_sa/Tasks/TaskSimple.h', 'source/game_sa/Pools.cpp', 'source/game_sa/Pools/Pools.h', 'source/game_sa/Pools/TaskPool.h', 'source/game_sa/Core/Pool.h', 'source/game_sa/References.cpp', 'source/game_sa/References.h', 'source/game_sa/Reference.h', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/Entity/Entity.h', 'source/game_sa/Entity/Ped/PlayerPed.h', 'source/game_sa/Entity/Ped/Ped.h', 'source/game_sa/Animation/AnimManager.cpp', 'source/game_sa/Animation/AnimManager.h', 'source/game_sa/Animation/AnimBlendAssociation.cpp', 'source/game_sa/Animation/AnimBlendAssociation.h', 'source/game_sa/Animation/AnimBlendStaticAssociation.cpp', 'source/game_sa/Animation/AnimBlendStaticAssociation.h', 'source/game_sa/Animation/AnimBlendAssocGroup.cpp', 'source/game_sa/Animation/AnimBlendAssocGroup.h', 'source/game_sa/Animation/AnimBlendHierarchy.h', 'source/game_sa/Animation/AnimBlendNode.cpp', 'source/game_sa/Animation/AnimBlendNode.h', 'source/game_sa/Animation/AnimBlock.h', 'source/game_sa/MemoryMgr.cpp', 'source/game_sa/MemoryMgr.h', 'source/InjectHooksMain.cpp', 'source/reversiblehooks/ReversibleHook/Simple.cpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__':
    root=Path(__file__).resolve().parents[2]
    absent=[name for name in SOURCES if not (root/name).is_file()]
    if absent:raise RuntimeError('Missing source manifest paths: '+str(absent))
    print(json.dumps(build(Path(__file__).parent,'dependenciestask12','DependenciesTask12Live',SOURCES)))
