import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=[G+p for p in ('Tasks/TaskTypes/TaskComplexGoToAttractor.cpp','Tasks/TaskTypes/TaskComplexGoToAttractor.h','Tasks/TaskComplexSequence.cpp','Tasks/TaskComplexSequence.h','Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.cpp','Tasks/TaskTypes/TaskComplexGoToPointAndStandStill.h','Tasks/TaskTypes/TaskSimpleSlideToCoord.cpp','Tasks/TaskTypes/TaskSimpleSlideToCoord.h','Tasks/TaskTypes/TaskSimpleStandStill.cpp','Tasks/TaskTypes/TaskSimpleStandStill.h','Tasks/TaskTypes/TaskSimpleRunNamedAnim.cpp','Tasks/TaskTypes/TaskSimpleRunNamedAnim.h')]
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'attractor-task-pending','AttractorTaskPendingLive',SOURCES)))
