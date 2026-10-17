"""Banco par a par RH producción para UpdateSexAppealStat (conversión a C++, ver README.md)."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesstatssex16','DependenciesStatsSex16Live',
        ['source/game_sa/Stats.cpp', 'source/game_sa/Stats.h', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h', 'source/game_sa/Pad.cpp', 'source/game_sa/Pad.h', 'source/game_sa/World_Helper.cpp', 'source/game_sa/World.cpp', 'source/game_sa/World.h', 'source/game_sa/PlayerInfo.h', 'source/game_sa/PlayerInfo.cpp', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/Entity/Entity.h', 'source/game_sa/Entity/Ped/PlayerPed.h', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Core/Matrix.h', 'source/game_sa/Models/ModelInfo.h', 'source/game_sa/Models/VehicleModelInfo.h', 'source/game_sa/References.cpp', 'source/game_sa/References.h', 'source/game_sa/Reference.h', 'source/game_sa/CutsceneMgr.cpp', 'source/game_sa/CutsceneMgr.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/DependenciesStatsSex16/HelperRoutes.hpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
