import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Train.cpp','Entity/Vehicle/Train.h','Entity/Vehicle/Vehicle.h','Core/Pool.h','Pools/VehiclePool.h','World.cpp','World.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'train-remove355','TrainRemove355Live',SOURCES)))
