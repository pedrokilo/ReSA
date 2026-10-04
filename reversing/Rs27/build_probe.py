"""Compila la sonda temporal del banco de la capa Rs del esqueleto de RenderWare (lista de 27)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/app/platform/platform.cpp', 'source/app/platform/platform.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'rs27', 'Rs27Live', SOURCES)))
