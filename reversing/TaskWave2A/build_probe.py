"""Compila la sonda temporal del banco TaskWave2A (CTaskComplexKillCriminal, CTaskComplexKillPedOnFootArmed y 4 funciones de CTaskSimplePlayerOnFoot)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
# Ruta real de la copia de prueba (el junction D:/JUEGOS ya no existe). Se redirige en memoria, antes de importar
# probe_build (que enlaza GAME al importarse), sin tocar runtime_runner.py (hash congelado).
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'{c}.{e}' for c in ('TaskComplexKillCriminal', 'TaskComplexKillPedOnFootArmed', 'TaskSimplePlayerOnFoot') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'taskwave2a', 'TaskWave2ALive', SOURCES)))
