import json, sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=['source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.h', 'source/game_sa/Tasks/TaskTypes/TaskSimplePause.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimplePause.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleRunAnim.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleRunTimedAnim.h', 'source/game_sa/Tasks/TaskTimer.cpp', 'source/game_sa/Tasks/TaskTimer.h', 'source/game_sa/InterestingEvents.cpp', 'source/game_sa/InterestingEvents.h', 'source/game_sa/Population.cpp', 'source/game_sa/Population.h', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Vector.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'sunbathe355','Sunbathe355Live',SOURCES)))

