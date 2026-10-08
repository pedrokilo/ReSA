import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Plane.cpp','Entity/Vehicle/Plane.h','Entity/Entity.cpp','Entity/Entity.h','Models/VehicleModelInfo.h','Collision/ColSphere.cpp','Collision/ColSphere.h','RenderWare/rw/rwcore.cpp','RenderWare/rw/rwcore.h','SurfaceInfos_c.cpp','SurfaceInfos_c.h','General.cpp','General.h','CarCtrl.cpp','CarCtrl.h','Audio/Entities/AEVehicleAudioEntity.cpp','Audio/Entities/AEVehicleAudioEntity.h','Entity/Physical.cpp','Entity/Physical.h','World.cpp','World.h','Collision/ColPoint.h','Core/Matrix.cpp','Core/Matrix.h','Core/Vector.cpp','Core/Vector.h','Camera.cpp','Camera.h','Explosion.cpp','Explosion.h','Darkel.cpp','Darkel.h','VisibilityPlugins.cpp','VisibilityPlugins.h','Entity/Vehicle/Vehicle.cpp','Entity/Vehicle/Vehicle.h','Entity/Vehicle/Automobile.cpp','Entity/Vehicle/Automobile.h','DamageManager.cpp','DamageManager.h','FireManager.cpp','FireManager.h','Stats.cpp','Stats.h','PlayerInfo.h','CarCtrl.cpp','CarCtrl.h','RenderWare/rw/rwcore.cpp','RenderWare/rw/rwcore.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'plane-blow355','PlaneBlow355Live',SOURCES)))
