"""Build an isolated delivery; preserve deferred work and existing local edits."""
import hashlib, json, shutil, subprocess, sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE.parent))
from runtime_runner import digest, GAME, EXE_HASH
ART = HERE/'artifacts'
CPP = 'source/game_sa/Tasks/TaskTypes/TaskComplexGangLeader.cpp'
HEADER = CPP[:-3]+'h'
ISOLATE = {
    'source/game_sa/Pipelines/CustomBuilding/CustomBuildingRenderer.cpp': '11fd6912f759d3bc93208a9def001b6dd751dbdf',
    'source/game_sa/Pipelines/CustomBuilding/CustomBuildingDNPipeline.cpp': '11fd6912f759d3bc93208a9def001b6dd751dbdf',
    'source/game_sa/Game.cpp': 'aef6bcb^',
    'source/game_sa/Cover.cpp': 'aef6bcb^',
    'source/game_sa/Cover.h': 'aef6bcb^',
    'source/game_sa/Tasks/TaskTypes/TaskSimpleClimb.cpp': 'aef6bcb',
}
def main():
    normal = '--normal' in sys.argv
    ART.mkdir(exist_ok=True)
    assert digest(GAME/'gta_sa.exe') == EXE_HASH
    paths = set(ISOLATE) | {CPP, HEADER, 'source/app/app_game.cpp', 'source/dllmain.cpp'}
    saved = {p:(ROOT/p).read_bytes() for p in paths}
    folder = ART/('normal' if normal else 'probe'); folder.mkdir(exist_ok=True)
    for p, data in saved.items(): (folder/(Path(p).name+'.before')).write_bytes(data)
    for ext in ('asi','pdb'): shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', folder/f'before.{ext}')
    try:
        for p, ref in ISOLATE.items():
            (ROOT/p).write_bytes(subprocess.check_output(['git','show',f'{ref}:{p}'],cwd=ROOT))
        manifest = {'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(), 'isolated_files':ISOLATE, 'workspace_sources':{p:hashlib.sha256(data).hexdigest() for p,data in saved.items()}}
        if not normal:
            p=ROOT/HEADER
            s=p.read_text(encoding='utf-8').replace('#pragma once','#pragma once\nnamespace GangLeaderProbe { inline unsigned calls{}; }',1).replace('return Type;','++GangLeaderProbe::calls; return Type;',1)
            p.write_text(s,encoding='utf-8',newline='\n')
            p=ROOT/'source/app/app_game.cpp'
            s=p.read_text(encoding='utf-8').replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'LiveComparison.hpp').as_posix()+'"',1).replace('void FrontendIdle() {','void FrontendIdle() {\n    GangLeaderLive::Update();',1)
            p.write_text(s,encoding='utf-8',newline='\n')
            p=ROOT/'source/dllmain.cpp'
            s=p.read_text(encoding='utf-8').replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE.parent/'ProbeRuntime.hpp').as_posix()+'"',1).replace('        LoadConfigurations();','        ReversingProbe::InstallFailureCapture("gang-leader");\n        LoadConfigurations();',1)
            p.write_text(s,encoding='utf-8',newline='\n')
        manifest['compiled_sources']={p:digest(ROOT/p) for p in paths}
        manifest['bank_sources']={p.name:digest(p) for p in (HERE/'LiveComparison.hpp',Path(__file__))}
        with (folder/'build.log').open('w') as log:
            subprocess.run([str(ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        for ext in ('asi','pdb'):
            dest=folder/f'gta_reversed-gang-leader.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest)
            manifest[ext]=digest(dest)
        (folder/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
        print(json.dumps(manifest))
    finally:
        for p,data in saved.items(): (ROOT/p).write_bytes(data)
        for ext in ('asi','pdb'): shutil.copy2(folder/f'before.{ext}',ROOT/f'bin/Debug/gta_reversed.{ext}')
        assert all((ROOT/p).read_bytes()==data for p,data in saved.items())
if __name__=='__main__':main()
