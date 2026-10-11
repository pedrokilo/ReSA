"""Compila la sonda temporal del banco de CTaskComplexFollowNodeRoute (lista de 19)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplexFollowNodeRoute.{e}' for e in ('cpp', 'h')]
SOURCES += ['source/game_sa/TrigCompound6Cpp.cpp', 'source/game_sa/TrigCompound6Cpp.h', 'source/CMakeLists.txt', 'CMakeLists.txt', 'source/InjectHooksMain.cpp', 'reversing/ProbeSupport.hpp', 'reversing/ProbeRuntime.hpp', 'reversing/CppTrigCompound6Route/audit_nested.py', 'reversing/CppTrigCompound6Route/run_local.py']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cpptrigcompound6route', 'CppTrigCompound6RouteLive', SOURCES)))
