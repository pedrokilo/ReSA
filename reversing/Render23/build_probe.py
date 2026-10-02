"""Compila la sonda temporal del banco de render, escaleras, puente y piel (lista de 23)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('BrightLights.cpp', 'BrightLights.h', 'MotionBlurStreaks.cpp', 'PlaneTrails.cpp', 'PlaneTrail.cpp', 'ShinyTexts.cpp',
                           'Escalators.cpp', 'Bridge.cpp', 'Entity/Ped/PlayerSkin.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'render23', 'Render23Live', SOURCES)))
