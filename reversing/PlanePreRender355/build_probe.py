import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Entity/Vehicle/Plane.cpp', 'source/game_sa/Entity/Vehicle/Plane.h', 'source/game_sa/Entity/Vehicle/Automobile.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/Core/Matrix.cpp', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Quaternion.cpp', 'source/game_sa/Models/VehicleModelInfo.cpp', 'source/game_sa/Fx/FxSystem.cpp', 'source/game_sa/Fx/FxManager.cpp', 'source/game_sa/Fx/FxPrtMult.cpp', 'source/game_sa/BouncingPanel.cpp', 'source/game_sa/MotionBlurStreaks.cpp', 'source/game_sa/Shadows.cpp', 'source/game_sa/RenderWare/rw/rwcore.cpp']
if __name__ == "__main__": print(json.dumps(build(Path(__file__).parent, "plane-prerender355", "PlanePreRender355Live", SOURCES)))
