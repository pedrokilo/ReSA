"""Sonda de los ayudantes x87 de CCam: bloques __asm originales de Cam.cpp (adaptadores de ensamblador dentro del banco) frente a los ayudantes de X87Math."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Cam.cpp','source/game_sa/Cam.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'cppcamx87stress1','CppCamX87Stress1Live',SOURCES)))
