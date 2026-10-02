"""Compila la sonda temporal del banco BreakablePlugin / BreakManager_c."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.cpp', 'source/game_sa/Plugins/BreakablePlugin/BreakablePlugin.h',
           'source/game_sa/BreakManager_c.cpp', 'source/game_sa/BreakManager_c.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'break', 'BreakLive', SOURCES)))
