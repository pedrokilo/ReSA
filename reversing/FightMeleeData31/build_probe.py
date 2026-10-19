"""Construye unicamente el banco FightMeleeData31 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fightmeleedata31', 'FightMeleeData31Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h',
         'source/game_sa/FileLoader.cpp', 'source/game_sa/FileLoader.h', 'source/game_sa/FileMgr.cpp', 'source/game_sa/FileMgr.h',
         'source/game_sa/Animation/AnimManager.cpp', 'source/game_sa/Animation/AnimManager.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/FightMeleeData31/LiveComparison.hpp', 'reversing/FightMeleeData31/Core.hpp', 'reversing/FightMeleeData31/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
