"""Construye unicamente el banco StatsWrappers60 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'statswrappers60', 'StatsWrappers60Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Stats.cpp', 'source/game_sa/Stats.h',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/StatsWrappers60/LiveComparison.hpp', 'reversing/StatsWrappers60/Core.hpp', 'reversing/StatsWrappers60/Gate.hpp', 'reversing/StatsWrappers60/Cases.hpp',
         'reversing/StatsWrappers60/Fakes.hpp', 'reversing/StatsWrappers60/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
