"""Compila la sonda temporal del banco de FIX_BUGS quitados en funciones validadas."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
G = 'source/game_sa/'
T = 'Tasks/TaskTypes/'
SOURCES = [G + p for p in ('Glass.cpp', 'Tasks/TaskManager.cpp', 'PopCycle.cpp', 'CoverPoint.cpp', 'Cover.cpp', 'RenderWare/rw/rtanim.cpp',
                           'Animation/AnimBlendHierarchy.cpp', 'CdStreamInfo.cpp', 'Directory.cpp', 'EntryExitManager.cpp', 'LoadedCarGroup.cpp',
                           'Checkpoint.cpp', T + 'TaskSimpleCarFallOut.cpp', T + 'TaskComplexFleeAnyMeans.cpp', T + 'TaskComplexPassObject.cpp',
                           T + 'TaskComplexDestroyCarArmed.cpp', T + 'Interior/TaskInteriorLieInBed.cpp', T + 'Interior/TaskInteriorLieInBed.h',
                           'Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorStandStill.h',
                           'Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowLimited.h',
                           'Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocatorFollowAnyMeans.h', 'Core/Pool.h', 'Tasks/Task.cpp')]
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'fixbugs', 'FixBugsLive', SOURCES)))
