"""Construye unicamente el banco Helpers65A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'helpers65a', 'H65Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Hud.cpp',
         'source/game_sa/RwHelper.cpp', 'source/game_sa/RwHelper.h',
         'source/game_sa/DecisionMakers/DecisionMaker.cpp', 'source/game_sa/DecisionMakers/DecisionMaker.h',
         'source/game_sa/DecisionMakers/DecisionMakerTypes.cpp', 'source/game_sa/DecisionMakers/DecisionMakerTypes.h',
         'source/game_sa/Audio/Managers/AEUserRadioTrackManager.cpp', 'source/game_sa/Audio/Managers/AEUserRadioTrackManager.h',
         'reversing/OriginalDependencies499/HookRoute.hpp',
         'reversing/Helpers65A/LiveComparison.hpp', 'reversing/Helpers65A/Core.hpp', 'reversing/Helpers65A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
