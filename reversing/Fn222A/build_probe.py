"""Construye unicamente el banco Fn222A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/app/app.cpp',
    'source/game_sa/ProtectionStub.cpp',
    'source/game_sa/ProtectionStub.h',
    'source/game_sa/Entity/Building.cpp',
    'source/game_sa/Audio/Entities/AEAudioEntity.cpp',
    'source/game_sa/Audio/Entities/AEAudioEntity.h',
    'source/game_sa/AutoPilot.cpp',
    'source/game_sa/DecisionMakers/DecisionMaker.cpp',
    'source/game_sa/Entity/Entity.cpp',
    'source/game_sa/Scripts/TheScripts.cpp',
    'source/game_sa/Core/PtrNodeSingleLink.cpp',
    'source/game_sa/Core/PtrNodeSingleLink.h',
    'source/game_sa/Core/PtrNodeDoubleLink.cpp',
    'source/game_sa/Core/PtrNodeDoubleLink.h',
    'source/game_sa/Pools/PtrNodeSingleLinkPool.h',
    'source/game_sa/Pools/PtrNodeDoubleLinkPool.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn222A/LiveComparison.hpp',
    'reversing/Fn222A/Core.hpp',
    'reversing/Fn222A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn222a', 'F222ALive', SOURCES)))
