"""Compila la sonda temporal del banco de datos de armas, plantillas de efectos y estar en grupo (lista de 10):
CWeaponInfo, FxSystemBP_c y CTaskComplexBeInGroup."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('WeaponInfo.cpp', 'WeaponInfo.h', 'Fx/FxSystemBP.cpp', 'Fx/FxSystemBP.h',
                           'Tasks/TaskTypes/TaskComplexBeInGroup.cpp', 'Tasks/TaskTypes/TaskComplexBeInGroup.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'data10', 'Data10Live', SOURCES)))
