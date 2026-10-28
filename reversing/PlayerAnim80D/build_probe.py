"""Construye unicamente el banco PlayerAnim80D (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'playeranim80d', 'Pa80Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Entity/Ped/PlayerPed.cpp', 'source/game_sa/Entity/Ped/PlayerPed.h',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/PlayerAnim80D/LiveComparison.hpp', 'reversing/PlayerAnim80D/Core.hpp', 'reversing/PlayerAnim80D/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
