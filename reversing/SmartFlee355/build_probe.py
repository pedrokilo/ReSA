import json, sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=['source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexSmartFleePoint.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexLeaveAnyCar.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWanderFlee.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexWander.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleTired.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleTired.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h', 'source/game_sa/Tasks/TaskComplexSequence.cpp', 'source/game_sa/Tasks/TaskComplexSequence.h', 'source/game_sa/Tasks/TaskTimer.cpp', 'source/game_sa/Tasks/TaskTimer.h', 'source/game_sa/General.cpp', 'source/game_sa/General.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'smart-flee355','SmartFlee355Live',SOURCES)))

