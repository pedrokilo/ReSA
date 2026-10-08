import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Cam.cpp','source/game_sa/Cam.h','source/game_sa/Camera.h','source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/RwHelper.cpp','source/game_sa/RwHelper.h','source/game_sa/Entity/Ped/Ped.cpp','source/game_sa/Entity/Ped/Ped.h','source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Entity.cpp','source/game_sa/Core/Vector.cpp','source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h']
SOURCES += ['source/game_sa/World.cpp','source/game_sa/World.h','source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Collision/ColPoint.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cam-mouse355', 'CamMouse355Live', SOURCES)))
