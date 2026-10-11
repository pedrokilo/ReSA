"""Production RH OFF/ON probe; complete globals and ABI capture."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesvehicle12','DependenciesVehicle12Live',
        ['source/game_sa/CutsceneMgr.cpp', 'source/game_sa/CutsceneMgr.h', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Entity/Vehicle/Heli.h', 'source/game_sa/Core/Pool.h', 'source/game_sa/Pools/VehiclePool.h', 'source/game_sa/Pools.cpp', 'source/game_sa/Pools/Pools.h', 'source/game_sa/MemoryMgr.cpp', 'source/game_sa/MemoryMgr.h', 'source/game_sa/Memory/MemoryHeap.cpp', 'source/game_sa/Memory/MemoryHeap.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/DependenciesVehicle12/HelperRoutes.hpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
