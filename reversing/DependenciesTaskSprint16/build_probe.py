"""Construye únicamente el banco; no acredita aprobación."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=[
 'source/game_sa/Entity/Ped/PlayerPed.cpp','source/game_sa/Entity/Ped/PlayerPed.h',
 'source/game_sa/Entity/Ped/Ped.h','source/game_sa/Entity/Entity.h',
 'source/game_sa/PlayerPedData.cpp','source/game_sa/PlayerPedData.h',
 'source/game_sa/PlayerInfo.cpp','source/game_sa/PlayerInfo.h',
 'source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/ControllerState.h',
 'source/game_sa/World.cpp','source/game_sa/World.h',
 'source/game_sa/Timer.cpp','source/game_sa/Timer.h','source/game_sa/Enums/eSprintType.h',
 'source/game_sa/Stats.cpp','source/game_sa/Stats.h',
 'source/game_sa/X87Math.cpp','source/game_sa/X87Math.h',
 'source/InjectHooksMain.cpp','source/reversiblehooks/ReversibleHook/Simple.cpp',
 'source/reversiblehooks/ReversibleHook/Simple.h','reversing/OriginalDependencies499/HookRoute.hpp',
]
if __name__=='__main__':
 root=Path(__file__).resolve().parents[2]
 absent=[name for name in SOURCES if not(root/name).is_file()]
 if absent:raise RuntimeError('Missing manifest source paths: '+str(absent))
 print(json.dumps(build(Path(__file__).parent,'dependenciestasksprint16','DependenciesTaskSprint16Live',SOURCES)))
