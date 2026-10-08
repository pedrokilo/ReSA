import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Bike.cpp','Entity/Vehicle/Bike.h','Entity/Entity.cpp','Entity/Entity.h','Models/VehicleModelInfo.h','Collision/ColSphere.cpp','Collision/ColSphere.h','RenderWare/rw/rwcore.cpp','RenderWare/rw/rwcore.h','SurfaceInfos_c.cpp','SurfaceInfos_c.h','General.cpp','General.h','CarCtrl.cpp','CarCtrl.h','Audio/Entities/AEVehicleAudioEntity.cpp','Audio/Entities/AEVehicleAudioEntity.h','Entity/Physical.cpp','Entity/Physical.h','World.cpp','World.h','Collision/ColPoint.h','Core/Matrix.cpp','Core/Matrix.h','Core/Vector.cpp','Core/Vector.h','RwHelper.cpp','RwHelper.h','Entity/Ped/Ped.cpp','Entity/Ped/Ped.h','RenderWare/rw/rphanim.cpp','RenderWare/rw/rphanim.h','Models/ModelInfo.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'bike-hands355','BikeHands355Live',SOURCES)))
