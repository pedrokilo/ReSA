"""Sonda de los ayudantes x87 de CCamera: secuencias del EXE (adaptadores de ensamblador dentro del banco) frente a los ayudantes de X87Math."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Camera.cpp','source/game_sa/Camera.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'cppcamerax87stress1','CppCameraX87Stress1Live',SOURCES)))
