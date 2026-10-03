"""Compila la sonda temporal del banco Fx8 (efectos, objetos procedurales y tareas de interior, lista de 8)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/Interior/'
SOURCES = [G + p for p in ('Fx/Fx.cpp', 'Fx/Fx.h', 'Plant/ProcObjectMan.cpp', 'Plant/ProcObjectMan.h')] + \
          [T + p for p in ('TaskInteriorBeInHouse.cpp', 'TaskInteriorBeInHouse.h', 'TaskInteriorBeInOffice.cpp', 'TaskInteriorBeInOffice.h',
                           'TaskInteriorUseInfo.cpp', 'TaskInteriorUseInfo.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fx8', 'Fx8Live', SOURCES)))
