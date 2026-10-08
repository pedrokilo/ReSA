import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Formation.cpp','Formation.h','PointList.h','PedList.h','Entity/Ped/Ped.h','Entity/Placeable.h','Core/Matrix.h','Core/Pool.h','Pools/VehiclePool.h','Pools/ObjectPool.h','Entity/Vehicle/Vehicle.h','Entity/Object/Object.h','Collision/ColModel.h')]
SOURCES += [G+p for p in ('Core/Vector.cpp','Core/Vector.h','Core/Matrix.cpp','Entity/Placeable.cpp','Entity/Entity.h','Models/BaseModelInfo.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'formation-gather355','FormationGather355Live',SOURCES)))
