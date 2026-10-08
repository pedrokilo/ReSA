"""Root-authorized serial build of the frozen additional bank before graphics."""
import hashlib,json,os,shutil,subprocess,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];ART=HERE/'artifacts'
sys.path.insert(0,str(ROOT/'reversing'));from runtime_runner import digest,EXE_HASH,GAME
TESTED=['source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.cpp','source/game_sa/Tasks/TaskTypes/TaskComplexArrestPed.h','source/game_sa/Tasks/TaskComplex.h','source/game_sa/Tasks/Task.h','source/game_sa/Tasks/Task.cpp','source/game_sa/Entity/Entity.h','source/game_sa/Entity/Entity.cpp','source/game_sa/Entity/Ped/Ped.h','source/game_sa/Entity/Ped/Ped.cpp','source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Entity/Vehicle/Vehicle.cpp','source/game_sa/Weapon.h','source/game_sa/Weapon.cpp','source/game_sa/Timer.h']
def main():
    frozen=json.loads((HERE/'prepared-freeze.json').read_text(encoding='utf8'))
    for name,expected in frozen['inputs'].items():assert digest(ROOT/name)==expected,name
    ART.mkdir(exist_ok=True);win=ROOT/'source/app/platform/win/WinMain.cpp';saved=win.read_bytes();bins={ext:ROOT/f'bin/Debug/gta_reversed.{ext}' for ext in ('asi','pdb')}
    for ext,path in bins.items():shutil.copy2(path,ART/f'before.{ext}')
    instruments=[HERE/x for x in ('LiveComparison.hpp','Preflight.hpp','VTableRanges.hpp','build_probe.py','run_local.py','model.py','prepared-freeze.json','preparation-plan.json')]+[ROOT/p for p in ('reversing/RequestedParallelSmall/HookSnapshots.hpp','reversing/ProbeSupport.hpp','reversing/ProbeRuntime.hpp','reversing/runtime_runner.py')]
    manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'tested_sources':{p:digest(ROOT/p) for p in TESTED},'bank_sources':{p.relative_to(ROOT).as_posix():digest(p) for p in instruments},'exe_sha256':digest(GAME/'gta_sa.exe'),'original_winmain_sha256':hashlib.sha256(saved).hexdigest(),'expected_pairs':2816,'fixture_scope':'Four ArrestPed pending entries, 2816 paired isolated cases; raw native/DLL vptr role checked, full task/ped/vehicle/private payload and ordered helper traces; no real actors, assets, task delegation or helper accreditation. All mismatches remain failure.'}
    assert manifest['exe_sha256']==EXE_HASH;lock=ROOT/'reversing/.arrest-remaining-four-build.lock'
    with lock.open('x') as handle:handle.write('remaining leaf pair preflight serial build')
    try:
        source=saved.decode('utf-8');signature='INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {';assert source.count(signature)==1
        start=source.index(signature);anchor='    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0u, nullptr, 2);';offset=source.index(anchor,start)
        source=source[:offset]+'    RequestedPreflightArrestRemainingFourLive::Update();\n    return 0;\n'+source[offset:];source=source.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/'Preflight.hpp').as_posix()+'"',1);win.write_text(source,encoding='utf-8',newline='\n')
        manifest['compiled_instruments']={'source/app/platform/win/WinMain.cpp':digest(win)};env=os.environ.copy();env['_CL_']='/MP6';cmake=next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
        with (ART/'build.log').open('w',encoding='utf-8') as log:subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug','--parallel','1'],stdout=log,stderr=subprocess.STDOUT,env=env,check=True)
        for p,h in manifest['tested_sources'].items():assert digest(ROOT/p)==h
        for p,h in manifest['bank_sources'].items():assert digest(ROOT/p)==h
        for ext,path in bins.items():target=ART/f'gta_reversed-arrest-remaining-four.{ext}';shutil.copy2(path,target);manifest[ext]=digest(target)
        (ART/'manifest.json').write_text(json.dumps(manifest,indent=2));print('Anim/projectile pair frozen',manifest['asi'])
    finally:
        win.write_bytes(saved)
        for ext,path in bins.items():shutil.copy2(ART/f'before.{ext}',path)
        lock.unlink(missing_ok=True);assert win.read_bytes()==saved;assert all(digest(path)==digest(ART/f'before.{ext}') for ext,path in bins.items())
if __name__=='__main__':main()
