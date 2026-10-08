import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/IdleCam.cpp', 'source/game_sa/IdleCam.h', 'source/game_sa/Attractors/PedAttractor.cpp', 'source/game_sa/Attractors/PedAttractor.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'idle-attractor-pending', 'IdleAttractorPendingLive', SOURCES)))
