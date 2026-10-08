import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Interior/Interior_c.cpp','Interior/Interior_c.h','Interior/Furniture_c.h','Interior/FurnitureManager_c.h','Interior/InteriorInfo_t.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'interior-lifecycle-core','InteriorLifecycleCoreLive',SOURCES)))
