import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/VehicleAnimGroupData.cpp','source/game_sa/VehicleAnimGroupData.h','source/game_sa/Models/VehicleModelInfo.h','source/game_sa/Entity/Vehicle/Bike.cpp','source/game_sa/Entity/Vehicle/Bike.h','source/game_sa/tHandlingData.h','source/game_sa/Core/Matrix.h']
SOURCES += ['source/game_sa/CarEnterExit.cpp', 'source/game_sa/CarEnterExit.h', 'source/game_sa/Events/EventPedEnteredMyVehicle.cpp', 'source/game_sa/Events/EventPedEnteredMyVehicle.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'car-door-position355', 'CarDoorPosition355Live', SOURCES)))
