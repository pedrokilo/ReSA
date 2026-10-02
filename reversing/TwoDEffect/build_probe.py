"""Compila la sonda temporal del banco C2dEffect."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
P = 'source/game_sa/Plugins/TwoDEffectPlugin/'
SOURCES = [P + '2dEffect.cpp', P + '2dEffect.h', P + '2dEffectStream.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'twod', 'TwoDLive', SOURCES)))
