"""Root-owned frontend build; this script has not been executed by its author."""
import json,os,shutil,subprocess,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
from runtime_runner import digest,EXE_HASH,GAME
ART=HERE/'artifacts'
CLASSES=('TaskComplexGangJoinRespond','TaskComplexGoToAttractor','TaskComplexSmartFleePoint','TaskComplexFollowLeaderInFormation')
def inputs():
    paths=[ROOT/f'source/game_sa/Tasks/TaskTypes/{name}.{ext}' for name in CLASSES for ext in ('cpp','h')]
    paths += [ROOT/p for p in ('source/game_sa/Tasks/Task.h','source/game_sa/Tasks/TaskComplex.h','source/game_sa/Tasks/TaskTimer.h','source/game_sa/Animation/AnimManager.cpp','source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Entity.h','reversing/ProbeSupport.hpp','reversing/ProbeRuntime.hpp','reversing/runtime_runner.py')]
    paths += [p for p in HERE.rglob('*') if p.suffix in ('.hpp','.py') and 'artifacts' not in p.parts and 'runs' not in p.parts]
    return {p.relative_to(ROOT).as_posix():digest(p) for p in sorted(set(paths))}
def main():
    ART.mkdir(exist_ok=True)
    paths=('source/app/app_game.cpp','source/dllmain.cpp');saved={p:(ROOT/p).read_bytes() for p in paths}
    bins={ext:ROOT/f'bin/Debug/gta_reversed.{ext}' for ext in ('asi','pdb')}
    for ext,p in bins.items():shutil.copy2(p,ART/f'before.{ext}')
    manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'frozen_inputs':inputs(),'exe_sha256':digest(GAME/'gta_sa.exe'),'frontend_entry':'FrontendIdle','expected_pairs':{'lifetime':896,'expanded':192},'fixture_scope':'isolated task lifetime contracts; no FindPlayerPed, no CGame::Process hook, no automatic save load','compiler_options':{'configuration':'Debug','_CL_':'/MP6','parallel':1}}
    assert manifest['exe_sha256']==EXE_HASH
    try:
        p=ROOT/paths[0];s=saved[paths[0]].decode('utf-8');anchor='void FrontendIdle() {';assert s.count(anchor)==1
        s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'Frontend.hpp').as_posix()+'"',1).replace(anchor,anchor+'\n    RequestedTaskFrontend::Update();');p.write_text(s,encoding='utf-8')
        p=ROOT/paths[1];s=saved[paths[1]].decode('utf-8');anchor='        LoadConfigurations();';assert s.count(anchor)==1
        s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(ROOT/'reversing/ProbeRuntime.hpp').as_posix()+'"',1).replace(anchor,'        ReversingProbe::InstallFailureCapture("requested-task-lifetime");\n        ReversingProbe::InstallFailureCapture("requested-task-expanded");\n'+anchor);p.write_text(s,encoding='utf-8')
        manifest['compiled_instruments']={p:digest(ROOT/p) for p in paths}
        env=os.environ.copy();env['_CL_']='/MP6';cmake=next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
        with (ART/'build.log').open('w',encoding='utf-8') as log:subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug','--parallel','1'],stdout=log,stderr=subprocess.STDOUT,env=env,check=True)
        assert inputs()==manifest['frozen_inputs'],'Source/bank drift during build'
        for ext,p in bins.items():out=ART/f'gta_reversed-requested-tasks.{ext}';shutil.copy2(p,out);manifest[ext]=digest(out)
        (ART/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8');print('Frozen frontend build preserved')
    finally:
        for p,data in saved.items():(ROOT/p).write_bytes(data)
        for ext,p in bins.items():shutil.copy2(ART/f'before.{ext}',p)
        assert all((ROOT/p).read_bytes()==data for p,data in saved.items())
if __name__=='__main__':main()
