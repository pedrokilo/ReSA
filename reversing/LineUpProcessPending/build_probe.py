import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.cpp', 'Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h', 'Entity/Placeable.cpp','Entity/Placeable.h','General.cpp','General.h','Core/Matrix.cpp', 'Core/Matrix.h', 'VehicleAnimGroupData.cpp', 'VehicleAnimGroupData.h', 'Entity/Vehicle/Bike.cpp', 'Entity/Vehicle/Bike.h', 'Models/VehicleModelInfo.h', 'tHandlingData.h','Core/Quaternion.h','RenderWare/rw/rtslerp.h','RenderWare/rw/rtquat.h','PedPlacement.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'line-up-process-pending','LineUpProcessPendingLive',SOURCES)))
