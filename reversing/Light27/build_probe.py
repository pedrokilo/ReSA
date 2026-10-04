"""Compila la sonda temporal del banco de luces RW Light (lista de 27)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/app/app_light.cpp', 'source/app/app_light.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'light27', 'Light27Live', SOURCES)))
