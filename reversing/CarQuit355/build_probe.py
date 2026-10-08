import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Entity/Ped/PlayerPed.cpp','source/game_sa/Entity/Ped/PlayerPed.h','source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/General.cpp','source/game_sa/Core/Matrix.h']
SOURCES += ['source/game_sa/CarEnterExit.cpp', 'source/game_sa/CarEnterExit.h', 'source/game_sa/Events/EventPedEnteredMyVehicle.cpp', 'source/game_sa/Events/EventPedEnteredMyVehicle.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'car-quit355', 'CarQuit355Live', SOURCES)))
