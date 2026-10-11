"""Root-only build after applying source-proposal.patch; test copy only."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'cppintegerworld10','CppIntegerWorld10Live',[
 'source/game_sa/Cam.cpp','source/game_sa/Cam.h','source/game_sa/Camera.cpp','source/game_sa/Camera.h',
 'source/game_sa/CarCtrl.cpp','source/game_sa/CarCtrl.h',
 'source/game_sa/cHandlingDataMgr.cpp','source/game_sa/cHandlingDataMgr.h',
 'source/game_sa/Models/ModelInfo.h','source/game_sa/Models/VehicleModelInfo.h',
 'source/game_sa/Models/BaseModelInfo.h','source/game_sa/Entity/Entity.h',
 'source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Entity/Placeable.h',
 'source/game_sa/Core/Pool.h','source/game_sa/Pools/VehiclePool.h','source/game_sa/Pools/Pools.h',
 'source/game_sa/IntegerMinimalCpp.cpp','source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp','source/CMakeLists.txt','docs/hooks.csv','source/StdInc.h','source/game_sa/RenderWare/D3DResourceSystem.h'])))
