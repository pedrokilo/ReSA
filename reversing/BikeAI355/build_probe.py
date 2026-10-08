import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES=['source/game_sa/Entity/Vehicle/Bike.cpp','source/game_sa/Entity/Vehicle/Bike.h','source/game_sa/Entity/Vehicle/Vehicle.cpp','source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Entity/Ped/Ped.cpp','source/game_sa/Entity/Ped/Ped.h','source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Entity.h','source/game_sa/Entity/Physical.cpp','source/game_sa/Entity/Physical.h','source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Placeable.h','source/game_sa/Core/Vector.cpp','source/game_sa/Core/Vector.h','source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h','source/game_sa/CarAI.cpp','source/game_sa/CarAI.h','source/game_sa/CarCtrl.cpp','source/game_sa/CarCtrl.h','source/game_sa/cTransmission.cpp','source/game_sa/cTransmission.h','source/game_sa/Stats.cpp','source/game_sa/Stats.h','source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/Models/VehicleModelInfo.h','source/game_sa/tHandlingData.h','source/game_sa/tBikeHandlingData.h']
if __name__=='__main__': print(json.dumps(build(Path(__file__).parent,'bike-ai355','BikeAI355Live',SOURCES)))

