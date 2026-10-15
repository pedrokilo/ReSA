"""Build temporary instrumentation and restore sources even when compilation fails."""
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

def main():
    paths = [ROOT/'source/game_sa/General.cpp', ROOT/'source/game_sa/Game.cpp']
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
            content = data.decode('utf-8').replace('\r\n','\n')
            if p.name=='General.cpp':
                content=content.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'Probe.hpp').as_posix()+'"')
                content=content.replace('        push ecx','        inc GeneralProbeCalls\n        push ecx',1)
                # Con el cuerpo ya en C++ (sin ensamblador) el contador va al principio de GetRandomNumberInRange<float>
                content=content.replace('    return GeneralRandomInRangeCpp(','    ++GeneralProbeCalls;\n    return GeneralRandomInRangeCpp(',1)
            else:
                content=content.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'LiveComparison.hpp').as_posix()+'"')
                content=content.replace('void CGame::Process() {','void CGame::Process() {\n    GeneralLive::Update();')
            p.write_text(content, encoding='utf-8', newline='\n')
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        hashes = {}
        for ext in ('asi','pdb'):
            dest = artifacts/f'gta_reversed-general-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(binaries=hashes, sources={str(p.relative_to(ROOT)):hashlib.sha256(data).hexdigest() for p,data in saved.items()}, instrumentation={p.name:digest(p) for p in (HERE/'Probe.hpp',HERE/'LiveComparison.hpp',HERE/'build_probe.py')})
        manifest['dependencies']={'source/game_sa/General.h':digest(ROOT/'source/game_sa/General.h')}
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(hashes))
    finally:
        for p,data in saved.items(): p.write_bytes(data)

if __name__ == '__main__': main()
