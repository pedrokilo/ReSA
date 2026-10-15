"""Sonda del banco CppX87Precision1: ayudantes de X87Math.cpp frente al EXE bajo cada palabra de control x87 (precision 24/53/64 y los cuatro redondeos)."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/X87Math.cpp','source/game_sa/X87Math.h','source/game_sa/General.cpp','source/game_sa/General.h','source/game_sa/common.cpp',
         'source/game_sa/cHandlingDataMgr.cpp','source/game_sa/cHandlingDataMgr.h','source/game_sa/Entity/Ped/Ped.cpp','source/game_sa/RenderWare/rw/rwplcore.h']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'cppx87precision1','CppX87Precision1Live',SOURCES)))
