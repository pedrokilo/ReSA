"""Build renderer and pipeline setup differential probe."""
import json
import shutil
import subprocess
import sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import digest

def main():
    art=HERE/'artifacts/probe'
    art.mkdir(parents=True,exist_ok=True)
    paths=[ROOT/'source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp',ROOT/'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp',ROOT/'source/game_sa/Game.cpp',ROOT/'source/dllmain.cpp']
    saved={p:p.read_bytes() for p in paths}
    for ext in ('asi','pdb'):
        shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',art/f'normal.{ext}')
    try:
        for p,data in saved.items():
            (art/(p.name+'.before')).write_bytes(data)
            s=data.decode('utf-8').replace('\r\n','\n')
            header=HERE/'Probe.hpp' if p.name in ('CustomBuildingRenderer.cpp','CustomBuildingDNPipeline.cpp') else HERE/'LiveComparison.hpp' if p.name=='Game.cpp' else HERE.parent/'ProbeRuntime.hpp'
            s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+header.as_posix()+'"',1)
            if p.name=='dllmain.cpp':
                s=s.replace('        LoadConfigurations();','        ReversingProbe::InstallFailureCapture("custom-building");\n        LoadConfigurations();',1)
            elif p.name=='Game.cpp':
                s=s.replace('void CGame::Process() {','void CGame::Process() {\n    BuildingLive::Update();',1)
            else:
                names=['void CCustomBuildingRenderer::AtomicSetup(RpAtomic* atomic) {','bool CCustomBuildingRenderer::IsCBPCPipelineAttached(RpAtomic* atomic) {','void CCustomBuildingRenderer::UpdateDayNightBalanceParam() {','void CCustomBuildingRenderer::Update() {','RpAtomic* CCustomBuildingDNPipeline::CustomPipeAtomicSetup(RpAtomic* atomic) {']
                for i,signature in enumerate(names):
                    if signature in s:s=s.replace(signature,signature+'\n    ++BuildingProbe::calls['+str(i)+'];',1)

            p.write_text(s,encoding='utf-8',newline='\n')
        with (art/'build.log').open('w') as log:
            subprocess.run([str(ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        manifest={'base_sources':{str(p.relative_to(ROOT)):__import__('hashlib').sha256(data).hexdigest() for p,data in saved.items()},'probe_sources':{p.name:digest(p) for p in (HERE/'LiveComparison.hpp',HERE/'Probe.hpp')}}
        for ext in ('asi','pdb'):
            dest=art/f'gta_reversed-custom-building-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest);manifest[ext]=digest(dest)
        (art/'manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(manifest))
    finally:
        for p,data in saved.items():p.write_bytes(data)
        for ext in ('asi','pdb'):shutil.copy2(art/f'normal.{ext}',ROOT/f'bin/Debug/gta_reversed.{ext}')

if __name__=='__main__':main()
