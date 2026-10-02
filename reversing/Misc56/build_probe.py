"""Compila la sonda temporal del banco de clases sueltas de la lista de 56 (misc56)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
P = 'Tasks/TaskTypes/SeekEntity/PosCalculators/'
SOURCES = [G + p for p in (P + 'EntitySeekPosCalculator.h', P + 'EntitySeekPosCalculatorFixedPos.cpp', P + 'EntitySeekPosCalculatorRadiusAngleOffset.cpp',
                           P + 'EntitySeekPosCalculatorStandard.cpp', P + 'EntitySeekPosCalculatorXYOffset.cpp', 'Attractors/PedAttractorPedPlacer.cpp',
                           'RegisteredCorona.cpp', 'Checkpoint.cpp', 'Debug.cpp', 'Audio/Loaders/AETrackLoader.cpp', 'RenderWare/rw/rtanim.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'misc56', 'Misc56Live', SOURCES)))
