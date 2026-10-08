import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'small-pending', 'RequestedParallelSmallLive', ['source/game_sa/PedDamageResponseCalculator.cpp', 'source/game_sa/PedDamageResponseCalculator.h'], install=False)))
