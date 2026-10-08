import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('InterestingEvents.cpp','InterestingEvents.h','Timer.h','Camera.h','Cam.h','Core/MatrixLink.h','Core/Vector.cpp','Core/Vector.h','World.h','Entity/Placeable.cpp','Entity/Placeable.h','Entity/Entity.cpp','Entity/Entity.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'interesting-core355','InterestingCore355Live',SOURCES)))
