"""Sonda de cuatro hojas de juego, después de aplicar el parche propuesto."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
SOURCES=['source/game_sa/CarCtrl.cpp','source/game_sa/CarCtrl.h',
         'source/game_sa/PathFind.cpp','source/game_sa/PathFind.h','source/game_sa/NodeAddress.h',
         'source/game_sa/Entity/Physical.cpp','source/game_sa/Entity/Physical.h',
         'source/game_sa/Entity/Placeable.h','source/game_sa/Core/Matrix.h','source/game_sa/Core/Vector.cpp',
         'source/game_sa/Core/Vector.h','source/game_sa/ScriptsForBrains.cpp','source/game_sa/ScriptsForBrains.h',
         'source/game_sa/StreamedScripts.h','source/game_sa/Scripts/TheScripts.h',
         'source/InjectHooksMain.cpp','reversing/OriginalDependencies499/HookRoute.hpp',
         'source/reversiblehooks/ReversibleHook/Simple.h','source/reversiblehooks/ReversibleHook/Simple.cpp',
         'reversing/DependenciesVehicleLeaf5/HelperHookRoute.hpp']
if __name__=='__main__':
    print(json.dumps(build(Path(__file__).parent,'dependenciesvehicleleaf5','DependenciesVehicleLeaf5Live',SOURCES)))
