"""Root-only build after literal Garage proposal review."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'dependenciesworld15','DependenciesWorld15Live',[
 'source/game_sa/Garage.cpp','source/game_sa/Garage.h','source/game_sa/Garages.cpp','source/game_sa/Garages.h',
 'source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Entity.h','source/game_sa/Entity/Vehicle/Vehicle.h',
 'source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Placeable.h',
 'source/game_sa/Core/MatrixLinkList.cpp','source/game_sa/Core/MatrixLinkList.h',
 'source/game_sa/Core/MatrixLink.cpp','source/game_sa/Core/MatrixLink.h',
 'source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h',
 'source/game_sa/SimpleTransform.cpp','source/game_sa/SimpleTransform.h',
 'source/game_sa/Collision/ColModel.h','source/game_sa/Collision/CollisionData.h','source/game_sa/Collision/ColSphere.h','source/game_sa/Collision/Sphere.h',
 'source/game_sa/Models/ModelInfo.h','source/game_sa/Models/BaseModelInfo.h','source/game_sa/Pools/Pools.h','source/game_sa/Pools/VehiclePool.h',
 'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp',
 'reversing/DependenciesWorld10/NewGameConfiguration.py'])))
