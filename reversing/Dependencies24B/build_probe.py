"""Banco par a par RH produccion para WinInput::diPadInit (0x7485C0). Ver README.md."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependencies24b','Dependencies24BLive',
        ['source/app/platform/win/WinInput.cpp','source/app/platform/win/WinInput.h','source/app/platform/win/WinPlatform.cpp',
         'source/game_sa/ControllerConfigManager.cpp','source/game_sa/ControllerConfigManager.h','source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp','reversing/Dependencies24B/Fixtures.hpp','reversing/Dependencies24B/Rows.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp'])))
