"""Sonda de Common::TransformVectors(CSimpleTransform): EXE 0x54EE30 frente a la sobrecarga de la DLL llamada directamente (sin hook)."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/common.cpp','source/game_sa/common.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'cpptransformvectors1','CppTransformVectors1Live',SOURCES)))
