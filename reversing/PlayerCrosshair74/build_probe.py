"""Construye unicamente el banco PlayerCrosshair74 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'playercrosshair74', 'Pc74Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'source/game_sa/PlayerInfo.cpp', 'source/game_sa/PlayerInfo.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PlayerCrosshair74/LiveComparison.hpp', 'reversing/PlayerCrosshair74/Core.hpp', 'reversing/PlayerCrosshair74/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
