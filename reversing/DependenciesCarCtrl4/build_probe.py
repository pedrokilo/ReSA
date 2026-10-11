"""Sonda de dos hojas CCarCtrl, después de aplicar el parche propuesto."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/CarCtrl.cpp','source/game_sa/CarCtrl.h',
         'source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Entity/Entity.h',
         'source/game_sa/Entity/Physical.h','source/game_sa/AutoPilot.h','source/game_sa/Timer.h',
         'source/game_sa/Enums/eEntityStatus.h','source/game_sa/Enums/eEntityType.h',
         'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciescarctrl4','DependenciesCarCtrl4Live',SOURCES)))
