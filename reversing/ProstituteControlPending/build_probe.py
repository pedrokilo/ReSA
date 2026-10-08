import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp','Tasks/TaskTypes/TaskComplexProstituteSolicit.h','Tasks/TaskTypes/TaskSimpleStandStill.cpp','Tasks/TaskTypes/TaskSimpleStandStill.h','Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h','Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h','Tasks/TaskTypes/TaskComplexCarDrive.cpp','Tasks/TaskTypes/TaskComplexCarDrive.h','Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.cpp','Tasks/TaskTypes/TaskComplexEnterCarAsPassenger.h','Tasks/TaskTypes/TaskComplexLeaveCar.cpp','Tasks/TaskTypes/TaskComplexLeaveCar.h','Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.cpp','Tasks/TaskTypes/TaskComplexTurnToFaceEntityOrCoord.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'prostitute-control-pending','ProstituteControlPendingLive',SOURCES)))
