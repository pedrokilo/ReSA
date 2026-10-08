import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Scripts/RunningScript.cpp', 'source/game_sa/Scripts/RunningScript.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h']
SOURCES += ['source/game_sa/Tasks/TaskTypes/TaskSimpleHoldEntity.cpp', 'source/game_sa/Tasks/PedScriptedTaskRecord.cpp', 'source/game_sa/Tasks/TaskManager.cpp', 'source/game_sa/Tasks/TaskComplexSequence.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.cpp', 'source/game_sa/Tasks/TaskSequences.h', 'source/game_sa/Animation/AnimManager.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'script-pickup355', 'ScriptPickup355Live', SOURCES)))
