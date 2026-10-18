"""Construye unicamente el banco FileLoaderLoad2dEffect27 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fileloader2dfx27', 'FileLoader2dFx27Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/FileLoader.cpp', 'source/game_sa/FileLoader.h',
         'source/game_sa/Models/BaseModelInfo.h', 'source/game_sa/Models/ModelInfo.h', 'source/game_sa/TxdStore.h',
         'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/FileLoaderLoad2dEffect27/LiveComparison.hpp', 'reversing/FileLoaderLoad2dEffect27/Core.hpp', 'reversing/FileLoaderLoad2dEffect27/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
