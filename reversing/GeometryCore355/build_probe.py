import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES=['source/game_sa/PedGeometryAnalyser.cpp', 'source/game_sa/PedGeometryAnalyser.h', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Vector.h', 'source/game_sa/Core/Matrix.cpp', 'source/game_sa/Core/Matrix.h', 'source/game_sa/Entity/Placeable.cpp', 'source/game_sa/Entity/Placeable.h', 'source/game_sa/Collision/ColSphere.cpp', 'source/game_sa/Collision/ColSphere.h', 'source/game_sa/Collision/Collision.cpp', 'source/game_sa/Collision/Collision.h', 'source/game_sa/World.cpp', 'source/game_sa/World.h', 'source/game_sa/Pools.cpp', 'source/game_sa/Pools/Pools.h', 'source/game_sa/PedIntelligence.cpp', 'source/game_sa/PedIntelligence.h', 'source/game_sa/Tasks/TaskManager.cpp', 'source/game_sa/Tasks/TaskManager.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'geometry-core355','GeometryCore355Live',SOURCES)))
