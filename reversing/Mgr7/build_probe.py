"""Compila la sonda temporal del banco Mgr7 (saltos, generadores de coches, muebles, criaturas y tareas de interior)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/Interior/'
SOURCES = [G + p for p in ('StuntJumpManager.cpp', 'StuntJumpManager.h', 'TheCarGenerators.cpp', 'TheCarGenerators.h',
                           'Interior/FurnitureManager_c.cpp', 'Interior/FurnitureManager_c.h', 'WaterCreatureManager_c.cpp', 'WaterCreatureManager_c.h')] + \
          [T + p for p in ('TaskInteriorGoToInfo.cpp', 'TaskInteriorGoToInfo.h', 'TaskInteriorLieInBed.cpp', 'TaskInteriorLieInBed.h',
                           'TaskInteriorShopKeeper.cpp', 'TaskInteriorShopKeeper.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'mgr7', 'Mgr7Live', SOURCES)))
