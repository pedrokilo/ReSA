"""Construye unicamente el banco Fn98A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn98a', 'F98ALive',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/CarAI.cpp', 'source/game_sa/CarAI.h', 'source/game_sa/CarCtrl.cpp', 'source/game_sa/CarCtrl.h',
         'source/app/platform/win/WinInput.cpp', 'source/app/platform/win/WinInput.h',
         'source/game_sa/PathFind.cpp', 'source/game_sa/PathFind.h',
         'source/game_sa/Models/VehicleModelInfo.cpp', 'source/game_sa/Models/VehicleModelInfo.h',
         'source/game_sa/Entity/Placeable.cpp', 'source/game_sa/Entity/Placeable.h',
         'source/game_sa/ScriptsForBrains.cpp', 'source/game_sa/ScriptsForBrains.h',
         'source/game_sa/Scripts/TheScripts.cpp', 'source/game_sa/Scripts/TheScripts.h',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Fn98A/LiveComparison.hpp', 'reversing/Fn98A/Core.hpp', 'reversing/Fn98A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
