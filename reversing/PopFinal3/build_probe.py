"""Compila la sonda temporal del banco PopFinal3 (CPopulation)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
# Ruta real de la copia de prueba (el junction D:/JUEGOS ya no existe). Se redirige en memoria, antes de importar
# probe_build (que enlaza GAME al importarse), sin tocar runtime_runner.py (hash congelado).
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + 'Population.cpp', G + 'Population.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'popfinal3', 'PopFinal3Live', SOURCES)))
