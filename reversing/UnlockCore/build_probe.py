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
    paths = [ROOT/p for p in ('source/game_sa/Cover.cpp','source/game_sa/EntryExitManager.cpp','source/game_sa/DecisionMakers/DecisionMakerTypesFileLoader.cpp','source/game_sa/Core/EntryInfoNode.cpp','source/game_sa/Curves.cpp','source/game_sa/VehicleRecording.cpp','source/game_sa/Pools/EntryInfoNodePool.h','source/game_sa/Game.cpp')]
    paths.append(ROOT/'source/dllmain.cpp')
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
            content = data.decode('utf-8').replace('\r\n','\n')
            if p.name == 'dllmain.cpp':
                content=content.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(ROOT/'reversing/ProbeRuntime.hpp').as_posix()+'"')
                content=content.replace('        LoadConfigurations();','        ReversingProbe::InstallFailureCapture("unlock-core");\n        LoadConfigurations();')
                p.write_text(content,encoding='utf-8',newline='\n')
                continue
            header = 'LiveComparison.hpp' if p.name == 'Game.cpp' else 'Probe.hpp'
            import re
            targets={'Cover.cpp':[(0,'FindDirFromVector_Reversed'),(1,'FindVectorFromDir_Reversed')], 'EntryExitManager.cpp':[(2,'GetPositionRelativeToOutsideWorld')], 'DecisionMakerTypesFileLoader.cpp':[(3,'UnloadDecisionMaker')], 'EntryInfoNode.cpp':[(4,'operator new'),(5,'operator delete')], 'Curves.cpp':[(6,'TestCurves')], 'VehicleRecording.cpp':[(7,'Render')]}
            if p.name=='EntryInfoNodePool.h':
                content=content.replace('CEntryInfoNode* New() {','CEntryInfoNode* New() {\n        ++UCProbe::calls[8];').replace('#pragma once','#pragma once\n#include "'+(HERE/'Probe.hpp').as_posix()+'"')
            elif p.name in targets:
                for n,name in targets[p.name]:
                    pattern=r'([\w:* ]+::'+re.escape(name)+r'\([^)]*\)(?: const)?\s*\{)'
                    content,count=re.subn(pattern,lambda m:m[1]+'\n    ++UCProbe::calls['+str(n)+'];',content)
                    if count!=1:raise RuntimeError(name)
            else:content=content.replace('void CGame::Process() {','void CGame::Process() {\n    UCLive::Update();')
            content=content.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/header).as_posix()+'"')
            p.write_text(content, encoding='utf-8', newline='\n')
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        hashes = {}
        for ext in ('asi','pdb'):
            dest = artifacts/f'gta_reversed-unlock-core-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(binaries=hashes, sources={str(p.relative_to(ROOT)):hashlib.sha256(data).hexdigest() for p,data in saved.items()}, instrumentation={p.name:digest(p) for p in (HERE/'Probe.hpp',HERE/'LiveComparison.hpp',HERE/'build_probe.py')})
        manifest['dependencies']={name:digest(ROOT/name) for name in ('source/InjectHooksMain.cpp','source/game_sa/Cover.h','source/game_sa/Curves.h','source/game_sa/Core/EntryInfoNode.h','source/game_sa/Core/Pool.h','source/game_sa/DecisionMakers/DecisionMakerTypes.cpp','source/game_sa/EntryExit.cpp')}

        (artifacts/'build-manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(hashes))
    finally:
        for p,data in saved.items(): p.write_bytes(data)

if __name__ == '__main__': main()
