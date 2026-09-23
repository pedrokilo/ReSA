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
    paths = [ROOT/f'source/game_sa/{f}' for f in ('Audio/AEAudioEnvironment.cpp','Game.cpp')]
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
            content = data.decode('utf-8').replace('\r\n','\n')
            header = 'LiveComparison.hpp' if p.name == 'Game.cpp' else 'Probe.hpp'
            import re
            targets={'AEAudioEnvironment.cpp':[(0,'GetPositionRelativeToCamera_Vector'),(1,'GetPositionRelativeToCamera_Placeable')]}
            if p.name in targets:
                for n,name in targets[p.name]:
                    pattern=r'([\w:* ]+::'+re.escape(name)+r'\([^)]*\)(?: const)?\s*\{)'
                    content,count=re.subn(pattern,lambda m:m[1]+'\n    ++UAProbe::calls['+str(n)+'];',content)
                    if count!=1:raise RuntimeError(name)
            else:content=content.replace('void CGame::Process() {','void CGame::Process() {\n    UALive::Update();')
            content=content.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/header).as_posix()+'"')
            p.write_text(content, encoding='utf-8', newline='\n')
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        hashes = {}
        for ext in ('asi','pdb'):
            dest = artifacts/f'gta_reversed-unlock-audio-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(binaries=hashes, sources={str(p.relative_to(ROOT)):hashlib.sha256(data).hexdigest() for p,data in saved.items()}, instrumentation={p.name:digest(p) for p in (HERE/'Probe.hpp',HERE/'LiveComparison.hpp',HERE/'build_probe.py')})
        manifest['dependencies']={}



        (artifacts/'build-manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(hashes))
    finally:
        for p,data in saved.items(): p.write_bytes(data)

if __name__ == '__main__': main()
