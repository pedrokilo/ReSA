import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Cam.cpp', 'source/game_sa/Cam.h', 'source/game_sa/Camera.h']
SOURCES += ['source/game_sa/WaterLevel.cpp', 'source/game_sa/World.cpp', 'source/game_sa/Collision/ColModel.h', 'source/game_sa/Models/BaseModelInfo.h']
SOURCES += ['source/game_sa/WeaponInfo.cpp', 'source/game_sa/Entity/Ped/Ped.cpp']
SOURCES += ['source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Matrix.h', 'source/game_sa/Entity/Placeable.cpp', 'source/game_sa/Entity/Entity.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cam-dead355', 'CamDead355Live', SOURCES)))
