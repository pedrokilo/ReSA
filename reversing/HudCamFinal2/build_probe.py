"""Compila la sonda temporal del banco HudCamFinal2 (CHud: 6 funciones; CCam: 2; CPickups: 1)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
# Ruta real de la copia de prueba (el junction D:/JUEGOS ya no existe). Se redirige en memoria, antes de importar
# probe_build (que enlaza GAME al importarse), sin tocar runtime_runner.py (hash congelado).
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + 'Hud.cpp', G + 'Hud.h', G + 'Cam.cpp', G + 'Cam.h', G + 'Pickups.cpp', G + 'Pickups.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'hudcamfinal2', 'HudCamFinal2Live', SOURCES)))
