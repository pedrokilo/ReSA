"""Compila la sonda temporal del banco de BreakObject_c (CalcGroupCenter, SetGroupData, SetBreakInfo, Exit)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [f'source/game_sa/BreakObject_c.{e}' for e in ('cpp', 'h')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'breakobj', 'BreakObjLive', SOURCES)))
