import argparse,hashlib,json,os,shutil,subprocess,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
ART=HERE/'artifacts'
sys.path.insert(0,str(ROOT/'reversing'))
from runtime_runner import digest,EXE_HASH,GAME
from restore_winmain import restore_saved
TESTED=list(json.loads((HERE/'scope.json').read_text())['sources'])+['source/game_sa/Entity/Ped/Ped.h','source/game_sa/Entity/Vehicle/Vehicle.h','source/game_sa/Tasks/TaskTypes/TaskSimpleUseGun.h','source/reversiblehooks/ReversibleHook/Simple.cpp','source/reversiblehooks/ReversibleHook/Virtual.cpp']

def main():
    global ART
    parser=argparse.ArgumentParser();parser.add_argument('--bank',required=True,choices=['reconcile','update','shake']);args=parser.parse_args();index=['reconcile','update','shake'].index(args.bank);ART=HERE/'artifacts'/args.bank
    ART.mkdir(parents=True,exist_ok=True)
    previous=ART/'manifest.json'
    if previous.exists():
        old=json.loads(previous.read_text());archive=ART/'archive'/old['asi'];archive.mkdir(parents=True,exist_ok=True)
        for item in [previous,ART/'build.log',ART/'layout-proof.json']+list(ART.glob('gta_reversed-preflight-pad-remaining3-phased-'+args.bank+'.*')):
            if item.exists():
                target=archive/item.name
                if target.exists():assert digest(target)==digest(item)
                else:shutil.copy2(item,target)
    frozen=json.loads((HERE/'corpus-frozen.json').read_text())
    for name,h in frozen['inputs'].items():assert digest(ROOT/name)==h,name
    win=ROOT/'source/app/platform/win/WinMain.cpp'
    saved=win.read_bytes()
    bins={ext:ROOT/f'bin/Debug/gta_reversed.{ext}' for ext in ('asi','pdb')}
    for ext,path in bins.items():shutil.copy2(path,ART/f'before.{ext}')
    # Explicit immutable inputs only: result/audit JSON can change independently.
    instruments=[ROOT/name for name in frozen['inputs']]+[HERE/'corpus-frozen.json',ROOT/'reversing/RequestedParallelSmall/HookSnapshots.hpp',ROOT/'reversing/ProbeSupport.hpp',ROOT/'reversing/runtime_runner.py',ROOT/'reversing/probe_case.py']
    manifest={'head':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'tested_sources':{p:digest(ROOT/p) for p in TESTED},'bank_sources':{p.relative_to(ROOT).as_posix():digest(p) for p in instruments},'exe_sha256':digest(GAME/'gta_sa.exe'),'original_winmain_sha256':hashlib.sha256(saved).hexdigest(),'fixture_scope':'Revision4 phased1024 original then candidate legs, exact legacy perleg reset/FP/payload/gates; raw comparisonseedorder; originalJSONbuffer<=1024records and conservative32MiBestimate failclosed. Exactly one CPad remaining mutation subbank selected per manifest: Reconcile2892/Update2560/StartShake89600. Private bounded pad/controllers, explicit two physical scratch arenas and active-role contract only for Reconcile, current final ASI/PDB layout hash and mapped fullbody/HIGHLOW operands verified at runtime. Full guards/globals/FP and allentry/vtable/flags restored; pregraphics natural0. No helper/realinput/game startup accreditation.'}
    assert manifest['exe_sha256']==EXE_HASH
    lock=ROOT/'reversing/.preflight-pad-remaining3-phased-build.lock'
    with lock.open('x') as handle:handle.write('preflight-pad-remaining3-phased serial build')
    try:
        source=saved.decode('utf-8')
        signature='INT WINAPI NOTSA_WinMain(HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR cmdLine, INT nCmdShow) {'
        assert source.count(signature)==1
        start=source.index(signature)
        anchor='    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0u, nullptr, 2);'
        offset=source.index(anchor,start)
        source=source[:offset]+'    RequestedPreflightPadRemaining3PhasedLive::Update();\n    return 0;\n'+source[offset:]
        source=source.replace('#include "StdInc.h"','#include "StdInc.h"\n#define REQUESTED_PAD_REMAINING_BANK '+str(index)+'\n#include "'+(HERE/'PreflightHarness.hpp').as_posix()+'"',1)
        win.write_text(source,encoding='utf-8',newline='\n')
        manifest['selected_bank']=args.bank;manifest['comparisons']=[2892,2560,89600][index]
        manifest['compiled_instruments']={'source/app/platform/win/WinMain.cpp':digest(win)}
        task_env=os.environ.copy();task_env['_CL_']='/MP6'
        cmake=next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
        with (ART/'build.log').open('w',encoding='utf-8') as log:
            subprocess.run([str(cmake),'--build',str(ROOT/'build'),'--config','Debug','--parallel','1'],stdout=log,stderr=subprocess.STDOUT,env=task_env,check=True)
        for p,h in manifest['tested_sources'].items():assert digest(ROOT/p)==h
        for p,h in manifest['bank_sources'].items():assert digest(ROOT/p)==h,p
        for ext,path in bins.items():
            target=ART/f'gta_reversed-preflight-pad-remaining3-phased-{args.bank}.{ext}'
            shutil.copy2(path,target);manifest[ext]=digest(target)
        (ART/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
        # Discover CURRENT final-module scratch/body; no old RVA is embedded into source or accepted for this build.
        layout=ART/'layout-proof.json'
        subprocess.run([sys.executable,str(HERE/'locate_scratch.py'),'--manifest',str(ART/'manifest.json'),'--asi',str(ART/f'gta_reversed-preflight-pad-remaining3-phased-{args.bank}.asi'),'--pdb',str(ART/f'gta_reversed-preflight-pad-remaining3-phased-{args.bank}.pdb'),'--output',str(layout)],check=True)
        manifest['layout_proof_sha256']=digest(layout)
        (ART/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf8')
        print('Preflight built, captured and frozen:',args.bank,manifest['asi'],'current-layout',manifest['layout_proof_sha256'])
    finally:
        restoration_attempts=[]
        try:
            restore_saved(win,saved,restoration_attempts)
        finally:
            # Bin restoration and diagnostics also run if WinMain restoration exhausts retries.
            try:
                for ext,path in bins.items():shutil.copy2(ART/f'before.{ext}',path)
            finally:
                lock.unlink(missing_ok=True)
                observed_error=None
                try:winmain_byte_equal=win.read_bytes()==saved
                except OSError as error:winmain_byte_equal=False;observed_error={'errno':error.errno,'error':str(error)}
                restoration_report={'attempts':restoration_attempts,'winmain_byte_equal':winmain_byte_equal,'winmain_verification_error':observed_error,'binaries_restored':{ext:digest(path)==digest(ART/f'before.{ext}') for ext,path in bins.items()},'retry_rule':'Only errno22; initial plus3x250ms; other errors and byte mismatch propagate'}
                (ART/'build-restoration.json').write_text(json.dumps(restoration_report,indent=2),encoding='utf8')
        assert restoration_report['winmain_byte_equal']
        assert all(restoration_report['binaries_restored'].values())
if __name__=='__main__':main()
