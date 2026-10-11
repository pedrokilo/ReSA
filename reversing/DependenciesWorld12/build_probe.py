"""Root-only build; proposal must first be applied."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'dependenciesworld12','DependenciesWorld12Live',[
 'source/game_sa/RwHelper.cpp','source/game_sa/RwHelper.h',
 'source/game_sa/Pools/Pools.h','source/game_sa/Pools/PedPool.h','source/game_sa/Pools/VehiclePool.h',
 'source/game_sa/Entity/Entity.h','source/game_sa/Entity/Ped/Ped.h','source/game_sa/Entity/Vehicle/Vehicle.h',
 'source/game_sa/RenderWare/rw/rphanim.h',
 'source/game_sa/Core/Pool.h','source/game_sa/RenderWare/RenderWare.h',
 'source/game_sa/RenderWare/rw/rwcore.h','source/game_sa/RenderWare/rw/rwplcore.h',
 'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp','reversing/DependenciesWorld10/NewGameConfiguration.py'])))
