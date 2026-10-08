import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp', 'Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h', 'Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h', 'Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h','Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp', 'Cover.h','CoverPoint.h','Tasks/TaskTypes/TaskSimpleGoToPoint.cpp','Tasks/TaskTypes/TaskSimpleGoToPoint.h','Tasks/TaskTypes/TaskSimplePause.cpp', 'Tasks/TaskTypes/TaskSimplePause.h', 'Tasks/TaskTypes/TaskSimpleStandStill.cpp', 'Tasks/TaskTypes/TaskSimpleStandStill.h', 'Tasks/TaskTypes/TaskSimpleDuck.cpp', 'Tasks/TaskTypes/TaskSimpleDuck.h', 'Tasks/TaskTypes/TaskSimpleGunControl.cpp', 'Tasks/TaskTypes/TaskSimpleGunControl.h', 'Tasks/TaskTypes/TaskSimpleThrowControl.cpp', 'Tasks/TaskTypes/TaskSimpleThrowControl.h', 'Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp', 'Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'armed-next-pending','ArmedNextPendingLive',SOURCES)))
