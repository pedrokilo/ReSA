"""Compila la sonda temporal del banco de la capa de plataforma Ps (lista de 22)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/app/platform/win/WinPs.cpp', 'source/app/platform/platform.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ps22', 'Ps22Live', SOURCES)))
