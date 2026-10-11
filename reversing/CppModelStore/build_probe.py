"""Banco de migración C++; aplicar propuesta solo por coordinación antes de compilar."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'cppmodelstore','CppModelStoreLive',
        ['source/game_sa/Models/ModelInfo.cpp','source/game_sa/Models/ModelInfo.h','source/game_sa/MemoryMgr.cpp','source/game_sa/MemoryMgr.h','source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp','source/CMakeLists.txt','CMakeLists.txt'])))

