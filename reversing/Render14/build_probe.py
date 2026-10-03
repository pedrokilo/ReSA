"""Compila la sonda temporal del banco de secuencias de animacion y pipeline dia/noche de edificios (lista de 14)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Animation/AnimBlendSequence.cpp', 'Animation/AnimBlendSequence.h',
                           'Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp', 'Pipelines/CustomBuilding/CustomBuildingDNPipeline.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'render14', 'Render14Live', SOURCES)))
