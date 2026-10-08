"""Build only when coordinator permits; all restoration steps run independently."""
import hashlib,json,os,shutil,subprocess,sys,time
from datetime import datetime
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];ART=HERE/'artifacts';sys.path.insert(0,str(ROOT/'reversing'));from runtime_runner import digest,GAME,EXE_HASH
TESTED=['source/game_sa/Pad.cpp','source/game_sa/Pad.h','source/game_sa/MouseControllerState.cpp','source/game_sa/MouseControllerState.h','source/game_sa/MenuManager.h','source/game_sa/Timer.h','source/app/platform/win/WinInput.cpp','source/app/platform/win/WinInput.h','source/app/platform/win/WinPlatform.cpp','source/app/platform/win/WinPlatform.h','source/app/platform/win/VideoMode.cpp','source/app/platform/win/VideoMode.h']
INSTRUMENTS=['LiveComparison.hpp','Preflight.hpp','VTableRanges.hpp','model.py','build_probe.py','run_local.py','inspect_compiled.py','preparation-plan.json','DESIGN.md','evidence/UpdateMouse-fresh-combo.txt','evidence/NativeCheckForInput.asm.txt','evidence/NativeGetMouseState.asm.txt','evidence/native-constant-zero.json']
def restored_write(path,data):
    for i in range(50):
        try:
            with path.open('wb') as handle:handle.write(data)
            if path.read_bytes()==data:return
        except OSError:
            if i==49:raise
        time.sleep(.15)
    raise RuntimeError('restore bytes mismatch '+str(path))
def main():
    freeze=json.loads((HERE/'prepared-freeze.json').read_text(encoding='utf8'))
    for p,h in freeze['inputs'].items():assert digest(ROOT/p)==h,p
    ART.mkdir(exist_ok=True)
    old=[p for p in ART.iterdir() if p.is_file()]
    if old:
        history=ART/'build-history'/datetime.now().strftime('%Y%m%d-%H%M%S-%f');history.mkdir(parents=True)
        for p in old:shutil.copy2(p,history/p.name)
    win=ROOT/'source/app/platform/win/WinMain.cpp';saved=win.read_bytes();(ART/'before-WinMain.cpp.bin').write_bytes(saved);bins={ext:ROOT/f'bin/Debug/gta_reversed.{ext}' for ext in ('asi','pdb')}
    before={ext:digest(p) for ext,p in bins.items()}
    for ext,p in bins.items():shutil.copy2(p,ART/f'before.{ext}')
    files=[HERE/name for name in INSTRUMENTS]+[HERE/'prepared-freeze.json']+[ROOT/p for p in ('reversing/RequestedParallelSmall/HookSnapshots.hpp','reversing/ProbeSupport.hpp','reversing/ProbeRuntime.hpp','reversing/runtime_runner.py')]
    manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'tested_sources':{p:digest(ROOT/p) for p in TESTED},'bank_sources':{p.relative_to(ROOT).as_posix():digest(p) for p in files},'exe_sha256':digest(GAME/'gta_sa.exe'),'original_winmain_sha256':hashlib.sha256(saved).hexdigest(),'expected_pairs':9216,'fixture_scope':freeze['scope'],'compiled_callpath_review_required':True};assert manifest['exe_sha256']==EXE_HASH
    lock=ROOT/'reversing/.pad-update-mouse-build.lock'
    with lock.open('x') as handle:handle.write('Root-coordinated serial build')
    try:
        source=saved.decode('utf8');signature='INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {';assert source.count(signature)==1;start=source.index(signature);anchor='    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0u, nullptr, 2);';offset=source.index(anchor,start);source=source[:offset]+'    RequestedPreflightPadUpdateMouseLive::Update();\n    return 0;\n'+source[offset:];source=source.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'Preflight.hpp').as_posix()+'"',1);win.write_text(source,encoding='utf8',newline='\n');manifest['compiled_instruments']={'source/app/platform/win/WinMain.cpp':digest(win)};env=os.environ.copy();env['_CL_']='/MP6';cmake=next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
        with (ART/'build.log').open('w',encoding='utf8') as log:subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug','--parallel','1'],stdout=log,stderr=subprocess.STDOUT,env=env,check=True)
        for label in ('tested_sources','bank_sources'):
            for p,h in manifest[label].items():assert digest(ROOT/p)==h,p
        for ext,p in bins.items():out=ART/f'gta_reversed-pad-update-mouse.{ext}';shutil.copy2(p,out);manifest[ext]=digest(out)
        (ART/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf8');print('Compiled',manifest['asi'],'STATICCALLPROOF REQUIRED BEFORE GTA')
    finally:
        errors=[]
        try:restored_write(win,saved)
        except Exception as e:errors.append('WinMain '+repr(e))
        for ext,p in bins.items():
            try:
                restored_write(p,(ART/f'before.{ext}').read_bytes());assert digest(p)==before[ext]
            except Exception as e:errors.append(ext+' '+repr(e))
        lock.unlink(missing_ok=True);report={'errors':errors,'WinMain_restored':win.read_bytes()==saved,'binaries_restored':{ext:digest(p)==before[ext] for ext,p in bins.items()}};(ART/'restoration.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8');assert not errors,errors
if __name__=='__main__':main()
