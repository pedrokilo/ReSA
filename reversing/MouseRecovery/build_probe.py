"""Build deterministic mouse acquisition observation."""
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
    paths=[ROOT/'source/game_sa/Pad.cpp',ROOT/'source/app/platform/win/WinInput.cpp',ROOT/'source/dllmain.cpp']
    saved={p:p.read_bytes() for p in paths}
    for ext in ('asi','pdb'):
        shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',art/f'normal.{ext}')
    try:
        for p,data in saved.items():
            (art/(p.name+'.before')).write_bytes(data)
            s=data.decode('utf-8').replace('\r\n','\n')
            header=HERE.parent/'ProbeRuntime.hpp' if p.name=='dllmain.cpp' else HERE/'Probe.hpp'
            s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+header.as_posix()+'"',1)
            if p.name=='dllmain.cpp':
                s=s.replace('        LoadConfigurations();','        ReversingProbe::InstallFailureCapture("mouse-recovery");\n        LoadConfigurations();',1)
            elif p.name=='WinInput.cpp':
                s=s.replace('    WIN_FCHECK(PSGLOBAL(diMouse)->Acquire());','    const auto acquisition = PSGLOBAL(diMouse)->Acquire();\n    MouseProbe::Record("initial_acquire", acquisition);\n    WIN_FCHECK(acquisition);',1)
            else:
                s=s.replace('    if (SUCCEEDED(hr)) {\n        return 0; // Success','    MouseProbe::Record("read", hr);\n    if (SUCCEEDED(hr)) {\n        return 0; // Success',1)
                s=s.replace('    while (PSGLOBAL(diMouse)->Acquire() == DIERR_INPUTLOST) {','    HRESULT acquired;\n    while ((acquired = PSGLOBAL(diMouse)->Acquire()) == DIERR_INPUTLOST) {',1)
                s=s.replace('    // Zero out the mouse state again','    MouseProbe::Record("reacquire", acquired);\n    // Zero out the mouse state again',1)
                s=s.replace('    return PSGLOBAL(diMouse)->GetDeviceState(sizeof(DIMOUSESTATE2), dm);','    const auto recovered = PSGLOBAL(diMouse)->GetDeviceState(sizeof(DIMOUSESTATE2), dm);\n    MouseProbe::Record("retry_read", recovered);\n    return recovered;',1)
                s=s.replace('    OldMouseControllerState = std::exchange(NewMouseControllerState, TempMouseControllerState);','    MouseProbe::Delivered();\n    OldMouseControllerState = std::exchange(NewMouseControllerState, TempMouseControllerState);',1)
            p.write_text(s,encoding='utf-8',newline='\n')
        with (art/'build.log').open('w') as log:
            subprocess.run([str(ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        manifest={'base_sources':{str(p.relative_to(ROOT)):__import__('hashlib').sha256(data).hexdigest() for p,data in saved.items()},'probe_sources':{p.name:digest(p) for p in (HERE/'Probe.hpp',)}}
        for ext in ('asi','pdb'):
            dest=art/f'gta_reversed-mouse-recovery-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest);manifest[ext]=digest(dest)
        (art/'manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(manifest))
    finally:
        for p,data in saved.items():p.write_bytes(data)
        for ext in ('asi','pdb'):shutil.copy2(art/f'normal.{ext}',ROOT/f'bin/Debug/gta_reversed.{ext}')

if __name__=='__main__':main()
