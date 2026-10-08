import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Plane.cpp','Entity/Vehicle/Plane.h','Entity/Vehicle/Automobile.cpp','Entity/Vehicle/Automobile.h','Entity/Vehicle/Vehicle.cpp','Entity/Vehicle/Vehicle.h','DamageManager.cpp','DamageManager.h','Fx/FxSystem.cpp','Fx/FxSystem.h','Fx/FxPrtMult.cpp','Fx/FxPrtMult.h','Pad.h','Timer.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'plane-control355','PlaneControl355Live',SOURCES)))
