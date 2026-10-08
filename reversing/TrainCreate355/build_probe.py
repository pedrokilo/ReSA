import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Entity/Vehicle/Train.cpp','Entity/Vehicle/Train.h','TrainNode.h','TrainNode.cpp','Population.cpp','Population.h','Entity/Entity.cpp','Entity/Entity.h','General.cpp','General.h','Tasks/Task.cpp','Tasks/Task.h','Tasks/TaskManager.cpp','Tasks/TaskManager.h','PedIntelligence.cpp','PedIntelligence.h','Entity/Vehicle/Vehicle.cpp','Entity/Vehicle/Vehicle.h','Core/Matrix.cpp','Core/Matrix.h','Models/BaseModelInfo.h','Collision/ColModel.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'train-create355','TrainCreate355Live',SOURCES)))

