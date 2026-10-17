"""Construye únicamente el banco; no acredita aprobación."""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_build import build
SOURCES = [
    'source/game_sa/Entity/Vehicle/Vehicle.cpp', 'source/game_sa/Entity/Vehicle/Vehicle.h', 'source/game_sa/Entity/Vehicle/Automobile.h',
    'source/game_sa/Coronas.cpp', 'source/game_sa/Coronas.h', 'source/game_sa/DamageManager.cpp', 'source/game_sa/DamageManager.h', 'source/game_sa/PointLights.cpp', 'source/game_sa/PointLights.h',
    'source/game_sa/Models/VehicleModelInfo.h', 'source/game_sa/tHandlingData.h', 'source/game_sa/Camera.cpp', 'source/game_sa/Camera.h', 'source/game_sa/Cam.h',
    'source/game_sa/X87Math.cpp', 'source/game_sa/X87Math.h',
    'source/InjectHooksMain.cpp', 'source/reversiblehooks/ReversibleHook/Simple.cpp', 'source/reversiblehooks/ReversibleHook/Simple.h',
    'reversing/OriginalDependencies499/HookRoute.hpp',
]
if __name__ == '__main__':
    root = Path(__file__).resolve().parents[2]
    absent = [name for name in SOURCES if not (root / name).is_file()]
    if absent:
        raise RuntimeError('Missing manifest source paths: ' + str(absent))
    print(json.dumps(build(Path(__file__).parent, 'dependencies23a', 'Dependencies23ALive', SOURCES)))
