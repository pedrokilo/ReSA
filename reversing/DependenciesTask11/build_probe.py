"""Preparación de dos entradas del juego; no ejecuta aprobación por sí sola."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=[
 'source/game_sa/DecisionMakers/DecisionMakerTypes.cpp','source/game_sa/DecisionMakers/DecisionMakerTypes.h',
 'source/game_sa/DecisionMakers/DecisionMaker.cpp','source/game_sa/DecisionMakers/DecisionMaker.h',
 'source/game_sa/DecisionMakers/Decision.cpp','source/game_sa/DecisionMakers/Decision.h',
 'source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.cpp','source/game_sa/Tasks/TaskTypes/TaskComplexSunbathe.h',
 'source/game_sa/Clock.h','source/game_sa/Weather.h','source/game_sa/Population.cpp',
 'source/InjectHooksMain.cpp','source/reversiblehooks/ReversibleHook/Simple.cpp',
 'source/reversiblehooks/ReversibleHook/Simple.h','reversing/OriginalDependencies499/HookRoute.hpp',
]
if __name__=='__main__':
    root=Path(__file__).resolve().parents[2]
    absent=[name for name in SOURCES if not (root/name).is_file()]
    if absent:raise RuntimeError('Missing source manifest paths: '+str(absent))
    print(json.dumps(build(Path(__file__).parent,'dependenciestask11','DependenciesTask11Live',SOURCES)))
