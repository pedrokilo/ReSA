"""Preparación de SetCurrentZoneAsUnlocked completo, sin aplicar fuentes."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/TheZones.cpp', 'source/game_sa/TheZones.h', 'source/game_sa/Zone.h', 'source/game_sa/ZoneInfo.h', 'source/game_sa/Enums/eLevelName.h', 'source/game_sa/World.cpp', 'source/game_sa/World.h', 'source/game_sa/World_Helper.cpp', 'source/game_sa/PlayerInfo.h', 'source/game_sa/Entity/Ped/PlayerPed.h', 'source/game_sa/Entity/Ped/Ped.h', 'source/game_sa/Entity/Placeable.h', 'source/game_sa/Core/Matrix.h', 'source/game_sa/Core/MatrixLink.h', 'source/game_sa/Game.h', 'source/game_sa/Scripts/TheScripts.h', 'source/InjectHooksMain.cpp', 'source/reversiblehooks/ReversibleHook/Simple.cpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__':
    root=Path(__file__).resolve().parents[2]
    absent=[name for name in SOURCES if not (root/name).is_file()]
    if absent:raise RuntimeError('Missing source manifest paths: '+str(absent))
    print(json.dumps(build(Path(__file__).parent,'dependencieszones13','DependenciesZones13Live',SOURCES)))
