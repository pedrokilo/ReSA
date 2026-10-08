import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Camera.cpp', 'source/game_sa/Camera.h']
SOURCES += ['source/game_sa/Core/Matrix.cpp', 'source/game_sa/Core/Matrix.h']
SOURCES += ['source/game_sa/Camera.h', 'source/game_sa/Collision/ColModel.h', 'source/game_sa/Collision/CollisionData.h', 'source/game_sa/Entity/Placeable.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'camera-collision355', 'CameraCollision355Live', SOURCES)))
