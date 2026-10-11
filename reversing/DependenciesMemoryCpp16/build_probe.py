"""Build preparation for four PC memory leaves; never changes production sources."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build

if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesmemorycpp16','DependenciesMemoryCpp16Live',[
        'source/game_sa/MemoryMgr.cpp','source/game_sa/MemoryMgr.h','source/app/app_debug.cpp',
        'source/game_sa/Memory/MemoryHeap.cpp','source/game_sa/Memory/MemoryHeap.h',
        'source/InjectHooksMain.cpp','source/reversiblehooks/ReversibleHook/Simple.cpp',
        'source/reversiblehooks/ReversibleHook/Simple.h',
        'reversing/OriginalDependencies499/HookRoute.hpp',
        'reversing/DependenciesMemory9/CrtProof.hpp',
        'reversing/DependenciesMemoryCpp16/HardwareTrace.hpp',
        'reversing/DependenciesMemoryCpp16/TraceSupport.hpp',
        'reversing/DependenciesMemoryCpp16/OriginalCode.hpp',
        'reversing/CppMemoryTraceProposal/trace_contract.py',
    ])))
