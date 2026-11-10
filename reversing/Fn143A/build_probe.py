"""Construye unicamente el banco Fn143A (ASI de sonda); no acredita aprobacion. Ver README.md."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES = [
    'source/CMakeLists.txt',
    'source/InjectHooksMain.cpp',
    'source/game_sa/RwHelper.cpp',
    'source/game_sa/RwHelper.h',
    'source/game_sa/Models/BaseModelInfo.cpp',
    'source/game_sa/Models/AtomicModelInfo.cpp',
    'source/game_sa/Models/VehicleModelInfo.cpp',
    'source/game_sa/Events/EventGlobalGroup.cpp',
    'source/game_sa/Events/EventGunShot.cpp',
    'source/game_sa/Events/EventVehicleDamage.cpp',
    'source/game_sa/Events/EventVehicleDamageCollision.cpp',
    'source/game_sa/Events/EventVehicleDamageWeapon.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexTreatAccident.cpp',
    'source/game_sa/Tasks/TaskTypes/TaskComplexMedicTreatInjuredPed.cpp',
    'reversing/OriginalDependencies499/HookRoute.hpp',
    'reversing/Fn143A/LiveComparison.hpp',
    'reversing/Fn143A/Core.hpp',
    'reversing/Fn143A/Fixtures.hpp',
    'source/reversiblehooks/ReversibleHook/Simple.h',
    'source/reversiblehooks/ReversibleHook/Simple.cpp',
    'source/reversiblehooks/ReversibleHook/Virtual.h',
    'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'fn143a', 'F143ALive', SOURCES)))
