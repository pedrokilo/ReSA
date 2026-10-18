"""Construye unicamente el banco; no acredita aprobacion."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'dependencies26a', 'Dependencies26ALive',
        ['source/game_sa/Audio/Hardware/AEAudioHardware.cpp', 'source/game_sa/Audio/Hardware/AEAudioHardware.h', 'source/game_sa/Audio/Hardware/AEAudioChannel.h', 'source/game_sa/Audio/Hardware/AEStaticChannel.h',
         'source/game_sa/Audio/Loaders/AEMP3BankLoader.h', 'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h', 'source/InjectHooksMain.cpp',
         'reversing/OriginalDependencies499/HookRoute.hpp', 'reversing/Dependencies26A/Core.hpp', 'reversing/Dependencies26A/Fixtures.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp'])))
