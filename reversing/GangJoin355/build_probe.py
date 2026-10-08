import json, sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Tasks/TaskTypes/TaskComplexGangJoinRespond.cpp','Tasks/TaskTypes/TaskComplexGangJoinRespond.h','Tasks/TaskTypes/TaskComplexGangLeader.cpp','Tasks/TaskTypes/TaskComplexGangLeader.h','Tasks/TaskTypes/TaskSimpleRunAnim.cpp','Tasks/TaskTypes/TaskSimpleRunAnim.h','Tasks/TaskTypes/TaskSimpleStandStill.cpp','Tasks/TaskTypes/TaskSimpleStandStill.h','Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp','Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h','General.cpp','General.h','Core/Vector.cpp','Core/Vector.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'gang-join355','GangJoin355Live',SOURCES)))

