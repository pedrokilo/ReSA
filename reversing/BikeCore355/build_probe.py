import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Bike.cpp','Entity/Vehicle/Bike.h','Entity/Physical.cpp','Entity/Physical.h','Entity/Entity.cpp','Entity/Entity.h','SurfaceInfos_c.cpp','SurfaceInfos_c.h','Core/Vector.cpp','Core/Vector.h','Plugins/RpAnimBlendPlugin/RpAnimBlend.cpp','Plugins/RpAnimBlendPlugin/RpAnimBlend.h','Entity/Ped/Ped.cpp','Entity/Ped/Ped.h','Timer.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'bike-core355','BikeCore355Live',SOURCES)))
