import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Camera.cpp', 'source/game_sa/Camera.h', 'source/game_sa/Cam.cpp', 'source/game_sa/Cam.h', 'source/game_sa/Core/Matrix.cpp', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/Collision/Collision.cpp', 'source/game_sa/World.cpp', 'source/game_sa/CullZones.cpp', 'source/game_sa/WaterLevel.cpp', 'source/game_sa/Draw.cpp', 'source/game_sa/Weather.h', 'source/game_sa/Entity/Ped/Ped.cpp', 'source/game_sa/PedIntelligence.cpp', 'source/game_sa/Tasks/TaskManager.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'source/game_sa/Garages.cpp', 'source/game_sa/Garage.cpp', 'source/game_sa/Pad.cpp', 'source/game_sa/GameLogic.cpp', 'source/game_sa/Audio/AudioEngine.cpp', 'source/game_sa/common.cpp', 'source/game_sa/RenderWare/rw/rwcore.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cam-control355', 'CamControl355Live', SOURCES)))
