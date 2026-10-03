"""Compila la sonda temporal del banco Misc6 (pajaros, creditos, curvas, cronometros, horarios, muebles y ventana)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
W = 'source/app/platform/win/'
SOURCES = [G + p for p in ('Birds.cpp', 'Birds.h', 'Credits.cpp', 'Credits.h', 'Curves.cpp', 'Curves.h', 'OnscreenTimer.cpp', 'OnscreenTimer.h',
                           'Models/TimeInfo.cpp', 'Models/TimeInfo.h', 'Interior/FurnitureGroup_c.cpp', 'Interior/FurnitureGroup_c.h')] + \
          [W + p for p in ('WinPlatform.cpp', 'WinMain.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'misc6', 'Misc6Live', SOURCES)))
