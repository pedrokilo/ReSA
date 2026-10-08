import json, sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=['source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h', 'source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimplePlayHandSignalAnim.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h', 'source/game_sa/Tasks/TaskComplexSequence.cpp', 'source/game_sa/Tasks/TaskComplexSequence.h', 'source/game_sa/Tasks/TaskTimer.cpp', 'source/game_sa/Tasks/TaskTimer.h', 'source/game_sa/General.cpp', 'source/game_sa/General.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'hand-signal355','HandSignal355Live',SOURCES)))

