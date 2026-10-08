import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = ['source/game_sa/Interior/InteriorGroup_c.cpp', 'source/game_sa/Interior/InteriorGroup_c.h', 'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorBeInOffice.cpp', 'source/game_sa/Tasks/TaskManager.cpp', 'source/game_sa/General.cpp', 'source/game_sa/Clock.cpp', 'source/game_sa/Models/PedModelInfo.h', 'source/game_sa/Entity/Placeable.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'interior-office355', 'InteriorOffice355Live', SOURCES)))
