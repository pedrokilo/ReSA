import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('WaterLevel.cpp','WaterLevel.h','X87Math.cpp','X87Math.h','Camera.h','Core/Vector.h','Core/Matrix.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'water-nearest355','WaterNearest355Live',SOURCES)))
