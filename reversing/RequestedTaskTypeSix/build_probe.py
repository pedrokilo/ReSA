import hashlib,json,os,shutil,subprocess,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
ART=HERE/'artifacts'
sys.path.insert(0,str(ROOT/'reversing'))
from runtime_runner import digest,EXE_HASH,GAME
TESTED=['source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimplePlayerOnFoot.h', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.cpp', 'source/game_sa/Tasks/TaskTypes/TaskSimpleFightingControl.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexPlayHandSignalAnim.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillPedOnFootArmed.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexKillCriminal.h', 'source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.cpp', 'source/game_sa/Tasks/TaskTypes/TaskComplexProstituteSolicit.h', 'source/game_sa/Tasks/Task.cpp', 'source/game_sa/Tasks/Task.h', 'source/game_sa/Tasks/TaskSimple.h', 'source/game_sa/Tasks/TaskComplex.h', 'source/game_sa/Entity/Entity.cpp', 'source/game_sa/Entity/Entity.h', 'source/game_sa/Animation/AnimManager.cpp', 'source/game_sa/Animation/AnimManager.h', 'source/game_sa/Timer.h', 'source/game_sa/Enums/eTaskType.h']
def main():
    ART.mkdir(exist_ok=True)
    win=ROOT/'source/app/platform/win/WinMain.cpp'
    saved=win.read_bytes()
    bins={ext:ROOT/f'bin/Debug/gta_reversed.{ext}' for ext in ('asi','pdb')}
    for ext,path in bins.items():shutil.copy2(path,ART/f'before.{ext}')
    instruments=list(HERE.glob('*.hpp'))+list(HERE.glob('*.py'))+[ROOT/'reversing/RequestedParallelSmall/HookSnapshots.hpp',ROOT/'reversing/ProbeSupport.hpp',ROOT/'reversing/runtime_runner.py',ROOT/'reversing/probe_case.py']
    manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'tested_sources':{p:digest(ROOT/p) for p in TESTED},'bank_sources':{p.relative_to(ROOT).as_posix():digest(p) for p in instruments},'exe_sha256':digest(GAME/'gta_sa.exe'),'original_winmain_sha256':hashlib.sha256(saved).hexdigest(),'fixture_scope':'Six pure getters,7680pairs:256 source-constructed manual arenas each x5actualentry/vtable/base-dispatch routes. Animation index and entity reference collaborators stubbed only during constructor fixture. No actor/pool/resource cleanup integration or constructor/destructor accreditation. Typed catalogue virtual addresses remain0 for five entries.'}
    assert manifest['exe_sha256']==EXE_HASH
    lock=ROOT/'reversing/.task-type-six-build.lock'
    with lock.open('x') as handle:handle.write('task-type-six serial build')
    try:
        source=saved.decode('utf-8')
        signature='INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {'
        assert source.count(signature)==1
        start=source.index(signature)
        anchor='    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0u, nullptr, 2);'
        offset=source.index(anchor,start)
        source=source[:offset]+'    RequestedTaskTypeSixLive::Update();\n    return 0;\n'+source[offset:]
        source=source.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'LiveComparison.hpp').as_posix()+'"',1)
        win.write_text(source,encoding='utf-8',newline='\n')
        manifest['compiled_instruments']={'source/app/platform/win/WinMain.cpp':digest(win)}
        task_env=os.environ.copy();task_env['_CL_']='/MP6'
        cmake=next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
        with (ART/'build.log').open('w',encoding='utf-8') as log:
            subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug','--parallel','1'],stdout=log,stderr=subprocess.STDOUT,env=task_env,check=True)
        for p,h in manifest['tested_sources'].items():assert digest(ROOT/p)==h
        for p,h in manifest['bank_sources'].items():assert digest(ROOT/p)==h
        for ext,path in bins.items():
            target=ART/f'gta_reversed-task-type-six.{ext}'
            shutil.copy2(path,target);manifest[ext]=digest(target)
        (ART/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
        print('Preflight built, captured and frozen:',manifest['asi'])
    finally:
        win.write_bytes(saved)
        for ext,path in bins.items():shutil.copy2(ART/f'before.{ext}',path)
        lock.unlink(missing_ok=True)
        assert win.read_bytes()==saved
        assert all(digest(path)==digest(ART/f'before.{ext}') for ext,path in bins.items())
if __name__=='__main__':main()
