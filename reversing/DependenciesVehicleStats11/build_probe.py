"""Production RH OFF/ON probe; complete globals and ABI capture."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesvehiclestats11','DependenciesVehicleStats11Live',
        ['source/game_sa/Timer.cpp', 'source/game_sa/Timer.h', 'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Entity/Vehicle/Bmx.h', 'source/game_sa/Entity/Vehicle/Bike.h', 'source/game_sa/Models/ModelInfo.h', 'source/game_sa/Models/VehicleModelInfo.cpp', 'source/game_sa/Models/VehicleModelInfo.h', 'source/game_sa/Stats.cpp', 'source/game_sa/Stats.h', 'source/game_sa/Hud.cpp', 'source/game_sa/Hud.h', 'source/game_sa/World.cpp', 'source/game_sa/World.h', 'source/game_sa/Pad.cpp', 'source/game_sa/Pad.h', 'source/game_sa/PlayerInfo.cpp', 'source/game_sa/PlayerInfo.h', 'source/game_sa/Cheat.cpp', 'source/game_sa/Cheat.h', 'source/game_sa/Messages.cpp', 'source/game_sa/Messages.h', 'source/game_sa/MenuManager.cpp', 'source/game_sa/MenuManager.h', 'source/game_sa/CutsceneMgr.cpp', 'source/game_sa/CutsceneMgr.h', 'source/game_sa/Garages.cpp', 'source/game_sa/Garages.h', 'source/game_sa/Replay.cpp', 'source/game_sa/Replay.h', 'source/game_sa/Text/Text.cpp', 'source/game_sa/Text/Text.h', 'source/game_sa/ControllerConfigManager.cpp', 'source/game_sa/ControllerConfigManager.h', 'source/game_sa/GxtChar.cpp', 'source/game_sa/GxtChar.h', 'source/game_sa/common.h', 'source/game_sa/common.cpp', 'source/game_sa/Enums/eHud.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/DependenciesVehicleStats11/HelperRoutes.hpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))

