"""Compila la sonda temporal del banco de sostener objeto y jetpack (lista de 14):
CTaskSimpleHoldEntity y CTaskSimpleJetPack."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskSimple{c}.{e}' for c in ('HoldEntity', 'JetPack') for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks14b', 'CTasks14bLive', SOURCES)))
