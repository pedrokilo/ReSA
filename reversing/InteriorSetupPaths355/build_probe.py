import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Interior/InteriorGroup_c.cpp', 'source/game_sa/Interior/InteriorGroup_c.h', 'source/game_sa/Interior/Interior_c.h', 'source/game_sa/Interior/InteriorInfo_t.h']
SOURCES += ['source/game_sa/PathFind.cpp', 'source/game_sa/PathFind.h', 'source/game_sa/NodeAddress.h']
SOURCES += ['source/game_sa/Core/Matrix.h', 'source/game_sa/Entity/Placeable.cpp', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/SimpleTransform.cpp', 'source/game_sa/Models/BaseModelInfo.h']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'interior-setup-paths355', 'InteriorSetupPaths355Live', SOURCES)))
