import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Bike.cpp','Entity/Vehicle/Bike.h','Entity/Entity.cpp','Entity/Entity.h','Models/VehicleModelInfo.h','Collision/ColSphere.cpp','Collision/ColSphere.h','RenderWare/rw/rwcore.cpp','RenderWare/rw/rwcore.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'bike-wheel355','BikeWheel355Live',SOURCES)))
