"""Production RH OFF/ON probe with complete state restoration."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciestaskped5','DependenciesTaskPed5Live',
        ['source/game_sa/AttractorScanner.cpp', 'source/game_sa/AttractorScanner.h', 'source/game_sa/PedIntelligence.cpp', 'source/game_sa/PedIntelligence.h', 'source/game_sa/MentalHealth.cpp', 'source/game_sa/MentalHealth.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h', 'source/game_sa/Events/EventSource.cpp', 'source/game_sa/Events/EventSource.h', 'source/game_sa/Events/EventDamage.cpp', 'source/game_sa/Events/EventDamage.h', 'source/game_sa/Events/Event.h', 'source/game_sa/Events/EventEditableResponse.cpp', 'source/game_sa/Events/EventEditableResponse.h', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/Entity/Entity.h', 'source/game_sa/Entity/Ped/Ped.cpp', 'source/game_sa/Entity/Ped/Ped.h', 'source/game_sa/PedType.cpp', 'source/game_sa/PedType.h', 'source/game_sa/Acquaintance.h', 'source/game_sa/Reference.h', 'source/game_sa/References.h', 'source/game_sa/Tasks/TaskTimer.cpp', 'source/game_sa/Tasks/TaskTimer.h', 'source/game_sa/Timer.cpp', 'source/game_sa/Timer.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp'])))

