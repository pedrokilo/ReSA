"""Root-authorized serial build of the frozen additional bank before graphics."""
import hashlib,json,os,shutil,subprocess,sys
from datetime import datetime
if not __debug__:raise RuntimeError("Strict bank requires Python assertions enabled")
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];ART=HERE/'artifacts'
sys.path.insert(0,str(ROOT/'reversing'));from runtime_runner import digest,EXE_HASH,GAME
TESTED=sorted(p.relative_to(ROOT).as_posix() for p in (ROOT/'source').rglob('*') if p.is_file() and p.suffix in ('.cpp','.h','.hpp','.inl','.inc') and p.relative_to(ROOT).as_posix()!='source/app/platform/win/WinMain.cpp')+['CMakeLists.txt','CMakePresets.json','ConanPresets.json','conanfile.py']

def main():
    frozen=json.loads((HERE/'instrument-freeze.json').read_text())
    for name,h in frozen['inputs'].items():assert digest(ROOT/name)==h,name
    header=(ROOT/'source/game_sa/Pad.h').read_text(encoding='utf-8')
    if 'StartShake_Train(const CVector2D&' in header:raise RuntimeError('Blocked: Train current source reference ABI is incompatible with native RET8; coordinator must perform authorized ABI fix first')
    ART.mkdir(exist_ok=True)
    previous=[p for p in ART.iterdir() if p.is_file()]
    if previous:
        history=ART/'build-history'/datetime.now().strftime('%Y%m%d-%H%M%S-%f');history.mkdir(parents=True)
        for p in previous:shutil.copy2(p,history/p.name)
    win=ROOT/'source/app/platform/win/WinMain.cpp';saved=win.read_bytes();bins={ext:ROOT/f'bin/Debug/gta_reversed.{ext}' for ext in ('asi','pdb')}
    for ext,path in bins.items():shutil.copy2(path,ART/f'before.{ext}')
    instruments=[ROOT/name for name in frozen['inputs']]+[HERE/'instrument-freeze.json']
    manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'tested_sources':{p:digest(ROOT/p) for p in TESTED},'bank_sources':{p.relative_to(ROOT).as_posix():digest(p) for p in instruments},'exe_sha256':digest(GAME/'gta_sa.exe'),'original_winmain_sha256':hashlib.sha256(saved).hexdigest(),'expected_pairs':13952,'fixture_scope':'Three CPU CPad targets before graphics; native Train byvalue8bytes required; private pad and vehicle type ABI fixtures; real camera position with borrowed private matrix; PC24 finite geometry and strict independent byte/FP model; exact vehicle helper multiplicity; no gameplay/helper implementation credit.'}
    assert manifest['exe_sha256']==EXE_HASH;lock=ROOT/'reversing/.pad-distance-train-nop-hex-build.lock'
    with lock.open('x') as handle:handle.write('pad distance train nop preflight serial build')
    try:
        source=saved.decode('utf-8');signature='INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {';assert source.count(signature)==1
        start=source.index(signature);anchor='    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0u, nullptr, 2);';offset=source.index(anchor,start)
        source=source[:offset]+'    RequestedPreflightPadDistanceTrainNopLive::Update();\n    return 0;\n'+source[offset:];source=source.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'Preflight.hpp').as_posix()+'"',1);win.write_text(source,encoding='utf-8',newline='\n')
        manifest['compiled_instruments']={'source/app/platform/win/WinMain.cpp':digest(win)};env=os.environ.copy();env['_CL_']='/MP6';cmake=next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
        with (ART/'build.log').open('w',encoding='utf-8') as log:subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug','--parallel','1'],stdout=log,stderr=subprocess.STDOUT,env=env,check=True)
        for p,h in manifest['tested_sources'].items():assert digest(ROOT/p)==h
        for p,h in manifest['bank_sources'].items():assert digest(ROOT/p)==h
        for ext,path in bins.items():target=ART/f'gta_reversed-pad-distance-train-nop-hex.{ext}';shutil.copy2(path,target);manifest[ext]=digest(target)
        from check_train_abi import check
        manifest['train_abi_proof_sha256']=check(ART/'gta_reversed-pad-distance-train-nop-hex.asi',ART/'gta_reversed-pad-distance-train-nop-hex.pdb',manifest,ART/'train-compiled-abi.json')
        (ART/'manifest.json').write_text(json.dumps(manifest,indent=2));print('Pad distance/train/NOP frozen',manifest['asi'])
    finally:
        restore_errors=[]
        try:win.write_bytes(saved)
        except Exception as error:restore_errors.append(str(error))
        for ext,path in bins.items():
            try:shutil.copy2(ART/f'before.{ext}',path)
            except Exception as error:restore_errors.append(str(error))
        lock.unlink(missing_ok=True)
        if restore_errors:raise RuntimeError('Restoration failures: '+repr(restore_errors))
        assert win.read_bytes()==saved;assert all(digest(path)==digest(ART/f'before.{ext}') for ext,path in bins.items())
if __name__=='__main__':main()
