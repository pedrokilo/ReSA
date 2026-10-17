"""Only root builds after reviewing collaborator correction. No new cohort credit."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'dependenciesmatrixallocation15','DependenciesMatrixAllocation15Live',[
 'source/game_sa/Core/MatrixLinkList.cpp','source/game_sa/Core/MatrixLinkList.h',
 'source/game_sa/Core/MatrixLink.cpp','source/game_sa/Core/MatrixLink.h','source/game_sa/Core/Matrix.h',
 'source/game_sa/Entity/Placeable.cpp','source/game_sa/Entity/Placeable.h','source/game_sa/SimpleTransform.h',
 'source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Pools/Pools.h','source/game_sa/Pools/VehiclePool.h',
 'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp',
 'reversing/DependenciesWorld15/MatrixPoolProof.hpp','reversing/DependenciesWorld14/NativeProof.hpp',
 'reversing/DependenciesWorld10/NewGameConfiguration.py'])))
