"""Compila la sonda temporal del banco de mundo, colision, gruas y oclusion (lista de 43)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Audio/AudioZones.cpp', 'Audio/AudioZones.h', 'Collision/ColSphere.cpp', 'Collision/ColSphere.h', 'Cranes.cpp', 'Cranes.h',
                           'Escalator.cpp', 'Escalator.h', 'GridRef.cpp', 'GridRef.h', 'HudColours.cpp', 'HudColours.h', 'Occlusion.cpp', 'Occlusion.h',
                           'SetPieces.cpp', 'SetPieces.h', 'Scripts/Scripted2dEffects.cpp', 'Scripts/Scripted2dEffects.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'world43', 'World43Live', SOURCES)))
