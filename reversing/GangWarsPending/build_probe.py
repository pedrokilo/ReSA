import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/GangWars.cpp','source/game_sa/GangWars.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'gang-wars-pending','GangWarsPendingLive',SOURCES)))
