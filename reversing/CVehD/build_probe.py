"""Compila la sonda temporal del banco CVehicle grupo D (dibujo, luces y partículas):
PreRender ... ProcessWheelRotation (26 funciones de CVehicle)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [f'source/game_sa/Entity/Vehicle/Vehicle.{e}' for e in ('cpp', 'h')]
SOURCES = [s for s in SOURCES if (Path(__file__).resolve().parents[2]/s).exists()]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'cvehd', 'CVehDLive', SOURCES)))
