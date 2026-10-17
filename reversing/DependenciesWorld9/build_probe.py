"""Root-only build after source-unapplied.patch; test copy only."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':print(json.dumps(build(Path(__file__).parent,'dependenciesworld9','DependenciesWorld9Live',[
    'source/game_sa/WaterLevel.cpp','source/game_sa/WaterLevel.h','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h',
    'source/game_sa/Events/EventHandler.cpp','source/game_sa/Events/EventHandler.h',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.cpp',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.h',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h',
    'source/game_sa/Tasks/TaskTimer.h','source/game_sa/Tasks/TaskTimer.cpp',
    'source/game_sa/Tasks/Task.h','source/game_sa/Tasks/Task.cpp',
    'source/game_sa/Tasks/TaskComplex.h',
    'source/game_sa/Entity/Entity.h','source/game_sa/Entity/Entity.cpp',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculatorXYOffset.cpp',
    'source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.h',
    'source/StdInc.h',
    'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp'])))
