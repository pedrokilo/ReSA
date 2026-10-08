import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Models/PedModelInfo.cpp','source/game_sa/Models/PedModelInfo.h','source/game_sa/WaterLevel.cpp','source/game_sa/WaterLevel.h','source/game_sa/PedIntelligence.cpp','source/game_sa/PedIntelligence.h','source/game_sa/Collision/CollisionData.h','source/game_sa/Collision/ColSphere.h']
SOURCES += ['source/game_sa/Camera.cpp', 'source/game_sa/Camera.h']
SOURCES += ['source/game_sa/Core/Matrix.cpp', 'source/game_sa/Core/Matrix.h']
SOURCES += ['source/game_sa/Camera.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'camera-improve355', 'CameraImprove355Live', SOURCES)))
