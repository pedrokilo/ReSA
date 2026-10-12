"""Compila la sonda temporal del banco PowEquivalence (pow de la CRT de la DLL frente a _CIpow del EXE)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'pow-equivalence', 'PowEquivalenceLive', SOURCES)))
