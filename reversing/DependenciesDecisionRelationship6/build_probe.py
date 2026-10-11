"""Production RH OFF/ON probe with complete state restoration."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesdecisionrelationship6','DependenciesDecisionRelationship6Live',
        ['source/game_sa/DecisionMakers/DecisionMakerTypes.cpp', 'source/game_sa/DecisionMakers/DecisionMakerTypes.h', 'source/game_sa/DecisionMakers/Decision.cpp', 'source/game_sa/DecisionMakers/Decision.h', 'source/game_sa/DecisionMakers/DecisionMaker.h', 'source/game_sa/PlayerRelationshipRecorder.cpp', 'source/game_sa/PlayerRelationshipRecorder.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGunControl.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleThrowControl.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h', 'source/game_sa/Tasks/TaskManager.cpp', 'source/game_sa/Tasks/TaskManager.h', 'source/game_sa/Tasks/TaskCategories.cpp', 'source/game_sa/Tasks/TaskCategories.h', 'source/game_sa/Entity/Ped/Ped.h', 'source/game_sa/Entity/Ped/CopPed.h', 'source/game_sa/PedIntelligence.cpp', 'source/game_sa/PedIntelligence.h', 'source/game_sa/PedGroupIntelligence.cpp', 'source/game_sa/PedGroupIntelligence.h', 'source/game_sa/PedGroup.h', 'source/game_sa/PedGroups.h', 'source/game_sa/Pools.cpp', 'source/game_sa/Pools/Pools.h', 'source/game_sa/Pools/PedPool.h', 'source/game_sa/Core/Pool.h', 'source/game_sa/FileMgr.cpp', 'source/game_sa/FileMgr.h', 'source/game_sa/Timer.cpp', 'source/game_sa/Timer.h', 'source/game_sa/References.h', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/Entity/Entity.h', 'source/game_sa/Tasks/Task.cpp', 'source/game_sa/Tasks/Task.h', 'source/game_sa/Tasks/TaskSimple.h', 'source/game_sa/Tasks/TaskTimer.cpp', 'source/game_sa/Tasks/TaskTimer.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp'])))

