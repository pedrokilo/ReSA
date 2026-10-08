import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES=['source/game_sa/Pickups.cpp','source/game_sa/Pickups.h','source/game_sa/tPickupMessage.h','source/game_sa/Pickup.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'pickups-pending','PickupsPendingLive',SOURCES)))
