"""Root-authorized serial build of the frozen additional bank before graphics."""
import hashlib,json,os,shutil,subprocess,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];ART=HERE/'artifacts'
sys.path.insert(0,str(ROOT/'reversing'));from runtime_runner import digest,EXE_HASH,GAME
TESTED=[f'source/game_sa/Tasks/TaskTypes/{name}.{ext}' for name in ('TaskSimpleFightingControl','TaskComplexProstituteSolicit') for ext in ('cpp','h')]+['source/game_sa/Tasks/Task.h','source/game_sa/Tasks/Task.cpp','source/game_sa/Tasks/TaskSimple.h','source/game_sa/Tasks/TaskComplex.h','source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Entity.h','source/game_sa/Entity/Ped/Ped.h','source/game_sa/PlayerPedData.h','source/game_sa/World.cpp']
def main():
    ART.mkdir(exist_ok=True);win=ROOT/'source/app/platform/win/WinMain.cpp';saved=win.read_bytes();bins={ext:ROOT/f'bin/Debug/gta_reversed.{ext}' for ext in ('asi','pdb')}
    for ext,path in bins.items():shutil.copy2(path,ART/f'before.{ext}')
    instruments=list(HERE.glob('*.hpp'))+list(HERE.glob('*.py'))+[ROOT/p for p in ('reversing/RequestedAdditionalLifetimes/LiveComparison.hpp','reversing/RequestedParallelTasks/TaskLifetimeComparison.hpp','reversing/RequestedParallelTasks/TaskLifetimeExpanded/Expanded.hpp','reversing/RequestedParallelSmall/HookSnapshots.hpp','reversing/ProbeSupport.hpp','reversing/ProbeRuntime.hpp','reversing/runtime_runner.py')]
    manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'tested_sources':{p:digest(ROOT/p) for p in TESTED},'bank_sources':{p.relative_to(ROOT).as_posix():digest(p) for p in instruments},'exe_sha256':digest(GAME/'gta_sa.exe'),'original_winmain_sha256':hashlib.sha256(saved).hexdigest(),'expected_pairs':672,'fixture_scope':'Frozen additional bank before graphics; isolated reference/player ABI fixtures and concrete native child; no WinMain/startup equivalence; mismatches remain failure and are preserved.'}
    assert manifest['exe_sha256']==EXE_HASH;lock=ROOT/'reversing/.preflight-additional-build.lock'
    with lock.open('x') as handle:handle.write('additional preflight serial build')
    try:
        source=saved.decode('utf-8');signature='INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {';assert source.count(signature)==1
        start=source.index(signature);anchor='    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0u, nullptr, 2);';offset=source.index(anchor,start)
        source=source[:offset]+'    RequestedPreflightAdditionalLive::Update();\n    return 0;\n'+source[offset:];source=source.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'LiveComparison.hpp').as_posix()+'"',1);win.write_text(source,encoding='utf-8',newline='\n')
        manifest['compiled_instruments']={'source/app/platform/win/WinMain.cpp':digest(win)};env=os.environ.copy();env['_CL_']='/MP6';cmake=next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
        with (ART/'build.log').open('w',encoding='utf-8') as log:subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug','--parallel','1'],stdout=log,stderr=subprocess.STDOUT,env=env,check=True)
        for p,h in manifest['tested_sources'].items():assert digest(ROOT/p)==h
        for p,h in manifest['bank_sources'].items():assert digest(ROOT/p)==h
        for ext,path in bins.items():target=ART/f'gta_reversed-preflight-additional.{ext}';shutil.copy2(path,target);manifest[ext]=digest(target)
        (ART/'manifest.json').write_text(json.dumps(manifest,indent=2));print('Preflight additional frozen',manifest['asi'])
    finally:
        win.write_bytes(saved)
        for ext,path in bins.items():shutil.copy2(ART/f'before.{ext}',path)
        lock.unlink(missing_ok=True);assert win.read_bytes()==saved;assert all(digest(path)==digest(ART/f'before.{ext}') for ext,path in bins.items())
if __name__=='__main__':main()
