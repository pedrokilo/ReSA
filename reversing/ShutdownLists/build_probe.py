"""Instrument the real shutdown path, then restore source bytes."""
import json,shutil,subprocess,sys,hashlib
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2];HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import digest
def main():
    art=HERE/'artifacts';art.mkdir(exist_ok=True)
    paths=[ROOT/'source/game_sa/Game.cpp',ROOT/'source/dllmain.cpp']
    saved={p:p.read_bytes() for p in paths}
    try:
        for p,data in saved.items():
            s=data.decode('utf-8').replace('\r\n','\n')
            header=HERE/'LiveComparison.hpp' if p.name=='Game.cpp' else HERE.parent/'ProbeRuntime.hpp'
            s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+header.as_posix()+'"',1)
            if p.name=='Game.cpp':
                s=s.replace('bool CGame::Shutdown() {','bool CGame::Shutdown() {\n    ShutdownLive::BeforeCleanup();',1)
            edits=[('void CGame::Process() {','\n    ShutdownLive::Update();'),('    CCover::ClearProcessedBuildings();','\n    ShutdownLive::AfterCleanup();'),('    D3DResourceSystem::Shutdown();','\n    ShutdownLive::Completed();')] if p.name=='Game.cpp' else [('        LoadConfigurations();','\n        ReversingProbe::InstallFailureCapture("shutdown-lists");')]
            for anchor,addition in edits:
                if s.count(anchor)!=1:raise RuntimeError(anchor)
                s=s.replace(anchor,anchor+addition,1)
            p.write_text(s,encoding='utf-8',newline='\n')
        cmake=ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'
        with (art/'build.log').open('w') as log:
            subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        manifest={'sources':{str(p.relative_to(ROOT)):hashlib.sha256(b).hexdigest() for p,b in saved.items()},'fix_sources':{name:digest(ROOT/name) for name in ['source/game_sa/Cover.cpp','source/game_sa/Cover.h']}}
        for ext in ('asi','pdb'):
            dest=art/f'gta_reversed-shutdown-lists-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest);manifest[ext]=digest(dest)
        (art/'manifest.json').write_text(json.dumps(manifest,indent=2));print(json.dumps(manifest))
    finally:
        for p,data in saved.items():p.write_bytes(data)
if __name__=='__main__':main()
