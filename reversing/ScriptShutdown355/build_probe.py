import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Scripts/RunningScript.cpp', 'source/game_sa/Scripts/RunningScript.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFinishBrain.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'script-shutdown355', 'ScriptShutdown355Live', SOURCES)))
