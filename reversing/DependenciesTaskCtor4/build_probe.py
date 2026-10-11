"""Production RH OFF/ON probe with complete state restoration."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciestaskctor4','DependenciesTaskCtor4Live',
        ['source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp','source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.h','source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.cpp','source/game_sa/Tasks/TaskTypes/TaskComplexWaitForPed.h','source/game_sa/Tasks/TaskTimer.h','source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Entity.h','source/game_sa/Reference.h','source/game_sa/References.h','source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))

