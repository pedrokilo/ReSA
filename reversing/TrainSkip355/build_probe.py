import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Train.cpp','Entity/Vehicle/Train.h','TrainNode.h','TheZones.cpp','TheZones.h','Stats.cpp','Stats.h','Streaming.cpp','Streaming.h','GameLogic.cpp','GameLogic.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'train-skip355','TrainSkip355Live',SOURCES)))

