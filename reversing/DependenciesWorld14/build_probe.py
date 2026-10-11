"""Root-only build after proposal; real exterior fixture."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'dependenciesworld14','DependenciesWorld14Live',[
 'source/game_sa/CarCtrl.cpp','source/game_sa/CarCtrl.h','source/game_sa/PathFind.cpp','source/game_sa/PathFind.h',
 'source/game_sa/NodeAddress.h','source/game_sa/AutoPilot.h','source/game_sa/Entity/Vehicle/Vehicle.h',
 'source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Placeable.h','source/game_sa/SimpleTransform.h',
 'source/game_sa/Core/MatrixLink.h','source/game_sa/Core/MatrixLinkList.cpp','source/game_sa/Core/MatrixLinkList.h',
 'source/game_sa/Core/Matrix.h','source/game_sa/Pools/Pools.h','source/game_sa/Pools/VehiclePool.h',
 'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp',
 'reversing/DependenciesWorld10/NewGameConfiguration.py'])))
