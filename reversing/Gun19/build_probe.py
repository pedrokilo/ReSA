"""Compila la sonda temporal del banco de CTaskSimpleUseGun (lista de 19)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + 'TaskSimpleUseGun.cpp', T + 'TaskSimpleUseGun.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'gun19', 'Gun19Live', SOURCES)))
