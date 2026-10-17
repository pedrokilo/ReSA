"""Sonda de CMatrix (10 funciones antes naked): EXE por direccion frente a los metodos de la DLL llamados directamente (sin hook)."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/Core/Matrix.cpp','source/game_sa/Core/Matrix.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'cppmatrixexact1','CppMatrixExact1Live',SOURCES)))
