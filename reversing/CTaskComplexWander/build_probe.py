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
    paths = [ROOT/'source/game_sa/Tasks/TaskTypes/TaskComplexWander.cpp', ROOT/'source/game_sa/Game.cpp', ROOT/'source/game_sa/PathFind.cpp']
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
            content = data.decode('utf-8')
            header = 'LiveComparison.hpp' if p.name == 'Game.cpp' else 'Probe.hpp'
            anchor = 'CTask* CTaskComplexWander::CreateNextSubTask(CPed* ped) {' if p.name == 'TaskComplexWander.cpp' else 'void CGame::Process() {'
            if p.name == 'PathFind.cpp': anchor = '#include \"StdInc.h\"'
            injected = '\n    ++WanderProbe::calls;' if p.name == 'TaskComplexWander.cpp' else '\n    WanderLive::Update();'
            if p.name == 'PathFind.cpp': injected = ''
            if content.count(anchor) != 1 or content.count('#include "StdInc.h"') != 1:
                raise RuntimeError('Instrumentation anchor missing or duplicated: '+str(p))
            content = content.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/CTaskComplexWander/'+header+'"').replace(anchor, anchor+injected)
            if p.name == 'TaskComplexWander.cpp':
                content = content.replace('../../reversing/CTaskComplexWander/Probe.hpp', '../../../../reversing/CTaskComplexWander/Probe.hpp')
                signature = 'void CTaskComplexWander::ComputeTargetPos(const CPed* ped, CVector& outTargetPos, const CNodeAddress& targetNodeAddress) {'
                assert content.count(signature)==1
                content=content.replace(signature,signature+'\n    if(WanderProbe::recording) { WanderProbe::events.push_back({"position",targetNodeAddress.m_wAreaId,targetNodeAddress.m_wNodeId}); outTargetPos=CVector{1.25f,-2.5f,3.75f}; return; }')
                signature = 'float CTaskComplexWander::ComputeTargetHeading(CPed* ped) {'
                assert content.count(signature)==1
                content=content.replace(signature,signature+'\n    if(WanderProbe::recording) { WanderProbe::events.push_back({"heading"}); return .75f; }')
            if p.name == 'PathFind.cpp':
                for name,value in [('TestForPedTrafficLight','light'),('TestCrossesRoad','road')]:
                    signature='bool CPathFind::'+name+'(CNodeAddress startNodeAddress, CNodeAddress targetNodeAddress) {'
                    assert content.count(signature)==1
                    content=content.replace(signature,signature+'\n    if(WanderProbe::recording) { WanderProbe::events.push_back({"'+name+'",startNodeAddress.m_wAreaId,startNodeAddress.m_wNodeId,targetNodeAddress.m_wAreaId,targetNodeAddress.m_wNodeId}); return WanderProbe::'+value+'; }')
            p.write_bytes(content.encode('utf-8'))
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        hashes = {}
        for ext in ('asi','pdb'):
            dest = artifacts/f'gta_reversed-wander-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(binaries=hashes, sources={str(p.relative_to(ROOT)):hashlib.sha256(data).hexdigest() for p,data in saved.items()}, instrumentation={p.name:digest(p) for p in (HERE/'Probe.hpp',HERE/'LiveComparison.hpp',HERE/'build_probe.py')})
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(hashes))
    finally:
        for p,data in saved.items(): p.write_bytes(data)

if __name__ == '__main__': main()

