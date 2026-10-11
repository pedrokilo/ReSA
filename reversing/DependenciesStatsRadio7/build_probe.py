"""Sonda de dos funciones de estadisticas de radio, después de aplicar el parche propuesto."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Stats.cpp', 'source/game_sa/Stats.h', 'source/game_sa/Enums/eRadioID.h', 'source/game_sa/Audio/AudioEngine.cpp', 'source/game_sa/Audio/AudioEngine.h', 'source/game_sa/Audio/Managers/AERadioTrackManager.cpp', 'source/game_sa/Audio/Managers/AERadioTrackManager.h', 'source/InjectHooksMain.cpp', 'source/reversiblehooks/ReversibleHook/Simple.cpp', 'source/reversiblehooks/ReversibleHook/Simple.h', 'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/DependenciesStatsRadio7/HelperHookRoute.hpp']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesstatsradio7','DependenciesStatsRadio7Live',SOURCES)))
