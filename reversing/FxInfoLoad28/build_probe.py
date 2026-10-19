"""Construye unicamente el banco FxInfoLoad28 (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fxinfoload28', 'FxInfoLoad28Live',
        ['source/CMakeLists.txt', 'source/InjectHooksMain.cpp',
         'source/game_sa/Fx/FxInfoManager.cpp', 'source/game_sa/Fx/FxInfoManager.h',
         'source/game_sa/Fx/FxMemoryPool.cpp', 'source/game_sa/Fx/FxMemoryPool.h',
         'source/game_sa/Fx/FxManager.cpp', 'source/game_sa/Fx/FxManager.h',
         'source/game_sa/Fx/FxTools.cpp', 'source/game_sa/Fx/FxTools.h',
         'source/game_sa/Fx/Blueprints/FxInfo.h', 'source/game_sa/Fx/eFxInfoType.h',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/FxInfoLoad28/LiveComparison.hpp', 'reversing/FxInfoLoad28/Core.hpp', 'reversing/FxInfoLoad28/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
