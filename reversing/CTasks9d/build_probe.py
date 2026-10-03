"""Compila la sonda temporal del banco de busqueda de entidad y sentarse en escritorio (lista de 9):
CTaskComplexSeekEntityStandard, CTaskComplexSeekEntityRadiusAngleOffset,
CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset> y CTaskInteriorSitAtDesk."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
S = T + 'SeekEntity/'
SOURCES = [S + 'TaskComplexSeekEntity.h'] + \
          [S + f'TaskComplexSeekEntity{c}.{e}' for c in ('Standard', 'RadiusAngleOffset', 'XYOffset') for e in ('cpp', 'h')] + \
          [T + f'Interior/TaskInteriorSitAtDesk.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks9d', 'CTasks9dLive', SOURCES)))
