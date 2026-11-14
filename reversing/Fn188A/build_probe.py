"""Construye unicamente el banco Fn188A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Events/EventAcquaintancePed.cpp',
    'source/game_sa/Events/EventAcquaintancePedDislike.cpp',
    'source/game_sa/Events/EventAcquaintancePedDislike.h',
    'source/game_sa/Events/EventAcquaintancePedLike.cpp',
    'source/game_sa/Events/EventAcquaintancePedLike.h',
    'source/game_sa/Events/EventAcquaintancePedRespect.cpp',
    'source/game_sa/Events/EventAcquaintancePedRespect.h',
    'source/game_sa/Events/EventAttractor.cpp',
    'source/game_sa/Events/EventAttractor.h',
    'source/game_sa/Events/EventScriptedAttractor.cpp',
    'source/game_sa/Events/EventScriptedAttractor.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn188A/LiveComparison.hpp',
    'reversing/Fn188A/Core.hpp',
    'reversing/Fn188A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn188a', 'F188ALive', SOURCES)))
