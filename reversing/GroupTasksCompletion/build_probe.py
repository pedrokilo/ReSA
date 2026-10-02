"""Build temporary instrumentation and restore sources even when compilation fails."""
import hashlib
import json
import os
import shutil
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CMAKE = next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
ALLOC = ROOT/'source/game_sa/Tasks/Allocators/PedGroup'
# archivo -> (texto de anclaje, contador insertado tras el anclaje)
COUNTERS = {
    ALLOC/'PedGroupDefaultTaskAllocatorRandom.h': ('void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {', 0),
    ALLOC/'PedGroupDefaultTaskAllocatorSitInLeaderCar.h': ('void AllocateDefaultTasks(CPedGroup* pedGroup, CPed* ped) const override {', 1),
    ROOT/'source/game_sa/Tasks/TaskTypes/TaskComplexGangFollower.h': ('void SetCanEnterLeadersCar(bool canEnter) {', 2),
}
GAME_CPP = ROOT/'source/game_sa/Game.cpp'

def include(path, header):
    rel = os.path.relpath(HERE/header, path.parent).replace('\\', '/')
    return f'#include "{rel}"'

def main():
    paths = [*COUNTERS, GAME_CPP]
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            content = data.decode('utf-8').replace('\r\n', '\n')
            if p in COUNTERS:
                anchor, n = COUNTERS[p]
                if content.count(anchor) != 1 or content.count('#pragma once') != 1: raise RuntimeError(p.name)
                content = content.replace(anchor, anchor + f' ++GroupAllocProbe::calls[{n}];')
                content = content.replace('#pragma once', '#pragma once\n' + include(p, 'Probe.hpp'))
            else:
                anchor = 'void CGame::Process() {'
                if content.count(anchor) != 1: raise RuntimeError('Game.cpp')
                content = content.replace(anchor, anchor + '\n    GroupAllocLive::Update();')
                content = content.replace('#include "StdInc.h"', '#include "StdInc.h"\n' + include(p, 'LiveComparison.hpp'), 1)
            p.write_text(content, encoding='utf-8', newline='\n')
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE), '--build', str(ROOT/'build'), '--config', 'Debug'], stdout=log, stderr=subprocess.STDOUT, check=True)
        hashes = {}
        for ext in ('asi', 'pdb'):
            dest = artifacts/f'gta_reversed-group-alloc-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(binaries=hashes,
                        sources={str(p.relative_to(ROOT)): hashlib.sha256(data).hexdigest() for p, data in saved.items()},
                        instrumentation={n: digest(HERE/n) for n in ('Probe.hpp', 'LiveComparison.hpp', 'build_probe.py')})
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest, indent=2))
        print(json.dumps(hashes))
    finally:
        for p, data in saved.items(): p.write_bytes(data)

if __name__ == '__main__': main()
