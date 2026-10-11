"""Production RH OFF/ON probe with complete state restoration."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciestaskleaf3','DependenciesTaskLeaf3Live',
        ['source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp','source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h','source/game_sa/Entity/Ped/PedSaveStructure.cpp','source/game_sa/Entity/Ped/PedSaveStructure.h','source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Entity.h','source/game_sa/References.h','source/game_sa/Reference.h','source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))

