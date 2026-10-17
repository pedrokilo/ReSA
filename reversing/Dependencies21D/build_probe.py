"""Construye únicamente el banco; no acredita aprobación."""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.h',
    'source/game_sa/GangWars.cpp',
    'source/game_sa/Entity/Ped/Ped.h', 'source/game_sa/Entity/Ped/PlayerPed.h', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Entity/Entity.h',
    'source/game_sa/Entity/Placeable.cpp', 'source/game_sa/Entity/Placeable.h', 'source/game_sa/SimpleTransform.cpp',
    'source/game_sa/Camera.cpp', 'source/game_sa/Camera.h', 'source/game_sa/Cam.cpp', 'source/game_sa/Cam.h',
    'source/game_sa/PedIK.cpp', 'source/game_sa/PedIK.h', 'source/game_sa/General.cpp', 'source/game_sa/General.h',
    'source/game_sa/GameLogic.cpp', 'source/game_sa/GameLogic.h', 'source/game_sa/PlayerInfo.h', 'source/game_sa/PlayerPedData.h',
    'source/game_sa/Animation/AnimBlendAssociation.h', 'source/game_sa/WeaponInfo.h',
    'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
    'source/InjectHooksMain.cpp', 'source/reversiblehooks/ReversibleHook/Simple.cpp', 'source/reversiblehooks/ReversibleHook/Simple.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
]
if __name__ == '__main__':
    root = Path(__file__).resolve().parents[2]
    absent = [name for name in SOURCES if not (root / name).is_file()]
    if absent:
        raise RuntimeError('Missing manifest source paths: ' + str(absent))
    print(json.dumps(build(Path(__file__).parent, 'dependencies21d', 'Dependencies21DLive', SOURCES)))
