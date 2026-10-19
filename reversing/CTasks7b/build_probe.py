"""Compila la sonda temporal del banco de tareas complejas B (lista de 7): CTaskComplexKillPedOnFootStealth,
CTaskComplexLeaveAnyCar, CTaskComplexPartnerGreet, CTaskComplexSeekEntityAiming y CTaskComplexSmartFleeEntity."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
T = 'source/game_sa/Tasks/TaskTypes/'
SOURCES = [T + f'TaskComplex{c}.{e}' for c in ('KillPedOnFootStealth', 'LeaveAnyCar', 'PartnerGreet', 'SeekEntityAiming',
                                               'SmartFleeEntity') for e in ('cpp', 'h')]
# La base CTaskComplexPartner::CreateFirstSubTask (0x681F20, hook Virtual) y los hooks Virtual/Simple forman parte de lo probado.
SOURCES += [T + 'TaskComplexPartner.cpp', T + 'TaskComplexPartner.h',
            'source/reversiblehooks/ReversibleHook/Simple.h', 'source/reversiblehooks/ReversibleHook/Simple.cpp',
            'source/reversiblehooks/ReversibleHook/Virtual.h', 'source/reversiblehooks/ReversibleHook/Virtual.cpp']
if __name__ == '__main__': print(json.dumps(build(Path(__file__).parent, 'ctasks7b', 'CTasks7bLive', SOURCES)))
