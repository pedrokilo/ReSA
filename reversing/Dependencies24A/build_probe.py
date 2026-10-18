"""Banco par a par RH produccion para Win::__MainWndProc (0x747EB0). Ver README.md."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies24a','Dependencies24ALive',
        ['source/app/platform/win/WndProc.cpp','source/app/platform/win/WndProc.h','source/app/platform/win/WinPlatform.cpp','source/game_sa/X87Math.cpp','source/game_sa/X87Math.h','source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies24A/Core.hpp','reversing/Dependencies24A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
