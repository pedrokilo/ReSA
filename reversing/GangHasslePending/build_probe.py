import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Tasks/TaskTypes/TaskGangHasslePed.cpp','Tasks/TaskTypes/TaskGangHasslePed.h','Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h','Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.cpp','Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityStandard.h','Tasks/TaskTypes/TaskComplexKillPedOnFoot.cpp','Tasks/TaskTypes/TaskComplexKillPedOnFoot.h','Tasks/TaskTypes/TaskSimpleRunAnim.cpp','Tasks/TaskTypes/TaskSimpleRunAnim.h','Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp','Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'gang-hassle-pending','GangHasslePendingLive',SOURCES)))
