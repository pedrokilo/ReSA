import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/MonsterTruck.cpp','Entity/Vehicle/MonsterTruck.h','Entity/Vehicle/Automobile.cpp','Entity/Vehicle/Automobile.h','Timer.h','Entity/Physical.cpp','Entity/Physical.h','tHandlingData.h','Collision/ColPoint.h','Core/Vector.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'monster-suspension355','MonsterSuspension355Live',SOURCES)))
