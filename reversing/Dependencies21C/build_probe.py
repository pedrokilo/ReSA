"""Construye unicamente el banco Dependencies21C; no acredita aprobacion."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=[
 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp','source/game_sa/Tasks/TaskTypes/TaskSimpleFight.h',
 'source/game_sa/Tasks/TaskTypes/TaskGangHassleVehicle.cpp','source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.cpp',
 'source/game_sa/X87Math.cpp','source/game_sa/X87Math.h',
 'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp',
]
if __name__=='__main__':
 root=Path(__file__).resolve().parents[2]
 absent=[name for name in SOURCES if not(root/name).is_file()]
 if absent:raise RuntimeError('Missing manifest source paths: '+str(absent))
 print(json.dumps(build(Path(__file__).parent,'dependencies21c','Dependencies21CLive',SOURCES)))
