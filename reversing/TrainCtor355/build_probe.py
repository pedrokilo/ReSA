import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Train.cpp','Entity/Vehicle/Train.h','TrainNode.h','TheZones.cpp','TheZones.h','Stats.cpp','Stats.h','Streaming.cpp','Streaming.h','GameLogic.cpp','GameLogic.h','FileMgr.cpp','FileMgr.h','TrainNode.cpp','Entity/Vehicle/Vehicle.cpp','Entity/Vehicle/Vehicle.h','Models/VehicleModelInfo.cpp','Models/VehicleModelInfo.h','Models/ClumpModelInfo.cpp','Models/ClumpModelInfo.h','Audio/Entities/AEVehicleAudioEntity.cpp','Audio/Entities/AEVehicleAudioEntity.h','cHandlingDataMgr.cpp','cHandlingDataMgr.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'train-ctor355','TrainCtor355Live',SOURCES)))

