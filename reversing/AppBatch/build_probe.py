"""Compila la sonda temporal del banco App / CIniFile / CIplDefPool."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/app/app.cpp', 'source/app/app.h', 'source/game_sa/IniFile.cpp', 'source/game_sa/IniFile.h', 'source/game_sa/Pools/IplDefPool.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'app', 'AppLive', SOURCES)))
