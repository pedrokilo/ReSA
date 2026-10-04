"""Compila la sonda temporal del banco GameIdle (Game::Idle y Game::FrontendIdle)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/app/app_game.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'gameidle', 'GameIdleLive', SOURCES)))
