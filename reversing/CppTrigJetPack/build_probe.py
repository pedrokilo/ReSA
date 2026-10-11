"""Compila la sonda temporal del banco de sostener objeto y jetpack (lista de 14):
CTaskSimpleHoldEntity y CTaskSimpleJetPack."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskSimple{c}.{e}' for c in ('HoldEntity', 'JetPack') for e in ('cpp', 'h')]
SOURCES += ['source/game_sa/TrigSimpleCpp.cpp', 'source/game_sa/TrigSimpleCpp.h', 'source/CMakeLists.txt', 'CMakeLists.txt', 'source/InjectHooksMain.cpp', 'source/reversiblehooks/RootHookCategory.h', 'source/reversiblehooks/ReversibleHook/Base.h', 'reversing/OriginalDependencies499/HookRoute.hpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cpptrigjetpack', 'CppTrigJetPackLive', SOURCES)))
