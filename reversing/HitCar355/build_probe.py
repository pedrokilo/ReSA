import json, sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from probe_build import build
G='source/game_sa/'
SOURCES=['source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexHitPedWithCar.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleKillPedWithCar.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexEvasiveStep.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleHitFromBehind.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexFallAndGetUp.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleLeaveGroup.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleNone.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleNone.h', 'source/game_sa/PedGeometryAnalyser.cpp', 'source/game_sa/PedGeometryAnalyser.h']
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'hit-car355','HitCar355Live',SOURCES)))

