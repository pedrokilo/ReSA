"""Compila la sonda temporal del banco de colas de eventos de grupo."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/InformFriendsEventQueue.cpp', 'source/game_sa/InformFriendsEventQueue.h',
           'source/game_sa/InformGroupEvent.cpp', 'source/game_sa/InformGroupEvent.h',
           'source/game_sa/InformGroupEventQueue.cpp', 'source/game_sa/InformGroupEventQueue.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'inform', 'InformLive', SOURCES)))
