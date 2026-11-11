"""Construye unicamente el banco Fn161A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/Pools/IplDefPool.h',
    'source/game_sa/CdStreamInfo.cpp',
    'source/game_sa/CdStreamInfo.h',
    'source/game_sa/Streaming.cpp',
    'source/game_sa/Entity/Dummy/Dummy.cpp',
    'source/game_sa/Entity/Dummy/Dummy.h',
    'source/game_sa/Entity/Dummy/DummyObject.cpp',
    'source/game_sa/Core/MatrixLinkList.cpp',
    'source/game_sa/Entity/Placeable.cpp',
    'source/game_sa/Models/AtomicModelInfo.cpp',
    'source/game_sa/Models/DamageAtomicModelInfo.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn161A/LiveComparison.hpp',
    'reversing/Fn161A/Core.hpp',
    'reversing/Fn161A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn161a', 'F161ALive', SOURCES)))
