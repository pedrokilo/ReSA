import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES=['source/game_sa/Events/GroupEventHandler.cpp','source/game_sa/Events/GroupEventHandler.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'group-event-pending','GroupEventPendingLive',SOURCES)))
