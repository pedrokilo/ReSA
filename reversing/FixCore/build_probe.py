"""Compila la sonda temporal del banco de arreglos de nucleo fuera de listas (CMatrix, CColBox, CPool y llamadores)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
SOURCES = [G + p for p in ('Core/Matrix.cpp', 'Core/Matrix.h', 'Core/Pool.h', 'Collision/ColBox.cpp', 'Collision/ColBox.h',
                           'Entity/Vehicle/Vehicle.cpp', 'Tasks/TaskTypes/TaskSimpleSwim.cpp', 'TxdStore.cpp', 'Entity/Ped/Ped.cpp',
                           'Pools/PedPool.h', 'common.h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fixcore', 'FixCoreLive', SOURCES)))
