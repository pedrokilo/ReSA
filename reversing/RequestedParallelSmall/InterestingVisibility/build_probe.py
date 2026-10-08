import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[2]))
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'interesting-visibility-pending','InterestingVisibilityPendingLive',['source/game_sa/InterestingEvents.cpp','source/game_sa/InterestingEvents.h'],install=False)))
