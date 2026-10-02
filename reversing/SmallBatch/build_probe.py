"""Compila la sonda temporal del banco de clases pequenas (KeyGen, KeyArray, ... LodAtomicModelInfo)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Core/KeyGen.cpp', 'Text/KeyArray.cpp', 'KeyboardState.cpp', 'Models/VehicleModelInfo.cpp', 'LoadedCarGroup.cpp', 'Acquaintance.cpp',
                           'AccidentManager.cpp', 'Localisation.cpp', 'ActiveOccluder.cpp', 'Models/LodAtomicModelInfo.cpp', 'Models/LodAtomicModelInfo.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'small', 'SmallLive', SOURCES)))
