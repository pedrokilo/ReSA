"""Production RH OFF/ON probe; complete globals and ABI capture."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesstatsfile15','DependenciesStatsFile15Live',
        ['source/game_sa/Stats.cpp', 'source/game_sa/Stats.h', 'source/game_sa/FileMgr.cpp', 'source/game_sa/FileMgr.h', 'source/game_sa/FileLoader.cpp', 'source/game_sa/FileLoader.h', 'source/game_sa/CutsceneMgr.cpp', 'source/game_sa/CutsceneMgr.h', 'source/InjectHooksMain.cpp', 'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/DependenciesStatsFile15/HelperRoutes.hpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
