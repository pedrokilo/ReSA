import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_build import build
if __name__ == '__main__':
    print(json.dumps(build(Path(__file__).parent, 'cpphandsignal1', 'CppHandSignal1Live', [
        'source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp',
        'source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h',
        'source/game_sa/Entity/Ped/Ped.h', 'source/InjectHooksMain.cpp',
        'reversing/OriginalDependencies499/HookRoute.hpp'])))
