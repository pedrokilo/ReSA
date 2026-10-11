"""Production RH OFF/ON probe with complete state restoration."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesdecisionped7','DependenciesDecisionPed7Live',
        ['source/game_sa/DecisionMakers/DecisionMakerTypes.cpp', 'source/game_sa/DecisionMakers/DecisionMakerTypes.h', 'source/game_sa/DecisionMakers/Decision.cpp', 'source/game_sa/DecisionMakers/Decision.h', 'source/game_sa/DecisionMakers/DecisionMaker.cpp', 'source/game_sa/DecisionMakers/DecisionMaker.h', 'source/game_sa/Pools/PedPool.h', 'source/game_sa/Pools/TaskPool.h', 'source/game_sa/Pools/Pools.h', 'source/game_sa/Pools.cpp', 'source/game_sa/Core/Pool.h', 'source/game_sa/Entity/Ped/Ped.h', 'source/game_sa/Entity/Ped/CopPed.h', 'source/game_sa/PedIntelligence.cpp', 'source/game_sa/PedIntelligence.h', 'source/game_sa/PedGroup.h', 'source/game_sa/Events/EventScriptCommand.cpp', 'source/game_sa/Events/EventScriptCommand.h', 'source/game_sa/Events/Event.cpp', 'source/game_sa/Events/Event.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleSlideToCoord.h', 'source/game_sa/Tasks/Task.cpp', 'source/game_sa/Tasks/Task.h', 'source/game_sa/Tasks/TaskSimple.h', 'source/game_sa/Tasks/TaskTimer.cpp', 'source/game_sa/Tasks/TaskTimer.h', 'source/game_sa/References.h', 'source/game_sa/Timer.cpp', 'source/game_sa/Timer.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp'])))

