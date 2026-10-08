import json, sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=['source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.cpp', 'source/game_sa/Tasks/TaskTypes/TaskGoToVehicleAndLean.h', 'source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.cpp', 'source/game_sa/Tasks/TaskTypes/TaskLeanOnVehicle.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleStandStill.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleAchieveHeading.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.h', 'source/game_sa/Core/Vector.cpp', 'source/game_sa/Core/Vector.h', 'source/game_sa/Core/Matrix.cpp', 'source/game_sa/Core/Matrix.h', 'source/game_sa/General.cpp', 'source/game_sa/General.h', 'source/game_sa/Entity/Physical.cpp', 'source/game_sa/Entity/Physical.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'vehicle-lean355','VehicleLean355Live',SOURCES)))

