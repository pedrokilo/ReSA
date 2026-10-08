import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('ClothesBuilder.cpp','ClothesBuilder.h','PedClothesDesc.cpp','PedClothesDesc.h','Clothes.cpp','Clothes.h','Enums/eClothesModelPart.h','Enums/eClothesTexturePart.h','Streaming.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'clothes-rules355','ClothesRules355Live',SOURCES)))
