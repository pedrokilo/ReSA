import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/DecisionMakers/Decision.cpp', 'source/game_sa/DecisionMakers/Decision.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'decision-pending', 'DecisionPendingLive', SOURCES)))
