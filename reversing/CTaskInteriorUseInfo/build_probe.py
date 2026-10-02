"""Compila la sonda temporal de AreAnimsLoaded/CreateFirstSubTask/CreateNextSubTask
y restaura las fuentes por bytes aunque la compilación falle."""
import hashlib
import json
import shutil
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CMAKE = ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'

USEINFO_CPP = ROOT/'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorUseInfo.cpp'
MANAGER_CPP = ROOT/'source/game_sa/Interior/InteriorManager_c.cpp'
GAME_CPP = ROOT/'source/game_sa/Game.cpp'
PROBE_INCLUDE = '#include "StdInc.h"\n#include "{}reversing/CTaskInteriorUseInfo/Probe.hpp"'


def patch(path, replacements):
    text = path.read_text(encoding='utf-8')
    for old, new in replacements:
        assert text.count(old) == 1, f'ancla no encontrada en {path.name}: {old!r}'
        text = text.replace(old, new)
    path.write_text(text, encoding='utf-8')


def main():
    paths = [USEINFO_CPP, MANAGER_CPP, GAME_CPP]
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
        patch(USEINFO_CPP, [
            ('#include "StdInc.h"', PROBE_INCLUDE.format('../../../../../')),
            ('CTask* CTaskInteriorUseInfo::CreateNextSubTask(CPed* ped) {', 'CTask* CTaskInteriorUseInfo::CreateNextSubTask(CPed* ped) {\n    ++InteriorUseInfoProbe::nextCalls;'),
            ('CTask* CTaskInteriorUseInfo::CreateFirstSubTask(CPed* ped) {', 'CTask* CTaskInteriorUseInfo::CreateFirstSubTask(CPed* ped) {\n    ++InteriorUseInfoProbe::firstCalls;'),
        ])
        patch(MANAGER_CPP, [
            ('#include "StdInc.h"', PROBE_INCLUDE.format('../../../')),
            ('bool InteriorManager_c::AreAnimsLoaded(int32 animBlock) const {', 'bool InteriorManager_c::AreAnimsLoaded(int32 animBlock) const {\n    ++InteriorUseInfoProbe::animsCalls;'),
        ])
        patch(GAME_CPP, [
            ('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/CTaskInteriorUseInfo/LiveComparison.hpp"'),
            ('void CGame::Process() {', 'void CGame::Process() {\n    UseInfoLive::Update();'),
        ])
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE), '--build', str(ROOT/'build'), '--config', 'Debug'], stdout=log, stderr=subprocess.STDOUT, check=True)
        hashes = {}
        for ext in ('asi', 'pdb'):
            dest = artifacts/f'gta_reversed-interioruseinfo-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(
            binaries=hashes,
            sources={str(p.relative_to(ROOT)): hashlib.sha256(data).hexdigest() for p, data in saved.items()},
            instrumentation={p.name: digest(p) for p in (HERE/'Probe.hpp', HERE/'LiveComparison.hpp', HERE/'build_probe.py')},
        )
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest, indent=2))
        print(json.dumps(hashes))
    finally:
        for p, data in saved.items():
            p.write_bytes(data)

if __name__ == '__main__':
    main()
