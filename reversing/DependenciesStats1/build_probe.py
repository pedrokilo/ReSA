"""Sonda completa; el único parche del nuevo método es temporal dentro del banco."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesstats1','DependenciesStats1Live',
        ['source/game_sa/Stats.cpp','source/game_sa/Stats.h','source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))

