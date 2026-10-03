"""Build archived instrumentation and always restore normal source files."""
import json
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CMAKE = ROOT / 'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'

def main():
    paths = [ROOT / 'source/game_sa/Fx/FxManager.cpp', ROOT / 'source/game_sa/Game.cpp']
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE / 'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts / (p.name + '.before')).write_bytes(data)
            content = data.decode('utf-8')
            if p.name == 'FxManager.cpp':
                content = content.replace('#include "FxManager.h"', '#include "FxManager.h"\n#include "../../../reversing/FxManager/Probe.hpp"')
                content = content.replace('void FxManager_c::DestroyAllFxSystems() {', 'void FxManager_c::DestroyAllFxSystems() {\n    ++FxManagerProbe::calls;')
                content = content.replace('void FxManager_c::Update(RwCamera* camera, float timeDelta) {', 'void FxManager_c::Update(RwCamera* camera, float timeDelta) {\n    ++FxManagerProbe::updateCalls;')
                content = content.replace('void FxManager_c::DestroyFxSystem(FxSystem_c* system) {', 'void FxManager_c::DestroyFxSystem(FxSystem_c* system) {\n    if (FxManagerProbe::active) FxManagerProbe::entered.push_back(system);')
                content = content.replace('    system->Exit();', '    if (FxManagerProbe::active && FxManagerProbe::invalidateLink) system->m_pNext = nullptr;\n    system->Exit();')
                content = content.replace('    delete system;', '    delete system;\n    if (FxManagerProbe::active) FxManagerProbe::destroyed.push_back(system);')
            else:
                content = content.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/FxManager/LiveComparison.hpp"')
                content = content.replace('void CGame::Process() {', 'void CGame::Process() {\n    FxManagerLive::Update();')
            p.write_bytes(content.encode('utf-8'))
        with (artifacts / 'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE), '--build', str(ROOT / 'build'), '--config', 'Debug'], stdout=log, stderr=subprocess.STDOUT, check=True)
        import shutil
        hashes = {}
        for ext in ('asi', 'pdb'):
            dest = artifacts / f'gta_reversed-fxmanager-probe.{ext}'
            shutil.copy2(ROOT / f'bin/Debug/gta_reversed.{ext}', dest)
            hashes[ext] = digest(dest)
        (artifacts / 'probe-hashes.json').write_text(json.dumps(hashes, indent=2))
        print(json.dumps(hashes))
    finally:
        for p, data in saved.items(): p.write_bytes(data)

if __name__ == '__main__': main()
