"""Build temporary runtime regression instrumentation; always restore source bytes."""
import json
import shutil
import subprocess
import sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import digest
CMAKE=ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'

def main():
    art=HERE/'artifacts'; art.mkdir(exist_ok=True)
    edits={
        'source/game_sa/Audio/Entities/AECollisionAudioEntity.cpp': ('Probe.hpp',
            'void CAECollisionAudioEntity::PlayBulletHitCollisionSound(eSurfaceType surface, const CVector& posn, float angleWithColPointNorm) {',
            '\n    ++BulletProbe::calls;'),
        'source/game_sa/Audio/Managers/AESoundManager.cpp': ('Probe.hpp',
            'CAESound* CAESoundManager::RequestNewSound(CAESound* pSound) {',
            '\n    if (BulletProbe::active) { BulletProbe::Capture(*pSound); return nullptr; }'),
        'source/game_sa/Audio/Hardware/AEAudioHardware.cpp': ('Probe.hpp',
            'bool CAEAudioHardware::IsSoundBankLoaded(eSoundBank bank, eSoundBankSlot slot) {',
            '\n    if (BulletProbe::active) return BulletProbe::bankLoaded;'),
        'source/game_sa/Game.cpp': ('LiveComparison.hpp','void CGame::Process() {','\n    BulletLive::Update();'),
        'source/dllmain.cpp': ('../ProbeRuntime.hpp','        LoadConfigurations();',
            '\n        ReversingProbe::InstallFailureCapture("bullet-hit");'),
    }
    saved={ROOT/p:(ROOT/p).read_bytes() for p in edits}
    try:
        for name,(header,anchor,addition) in edits.items():
            p=ROOT/name; s=saved[p].decode('utf-8').replace('\r\n','\n')
            if s.count(anchor)!=1: raise RuntimeError(f'Anchor: {name}')
            s=s.replace(anchor,anchor+addition,1)
            s=s.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "'+(HERE/header).resolve().as_posix()+'"',1)
            p.write_text(s,encoding='utf-8',newline='\n')
        with (art/'build.log').open('w') as log:
            subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        manifest={'sources':{str(p.relative_to(ROOT)):__import__('hashlib').sha256(b).hexdigest() for p,b in saved.items()}}
        for ext in ('asi','pdb'):
            dest=art/f'gta_reversed-bullet-hit-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest); manifest[ext]=digest(dest)
        (art/'manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(manifest))
    finally:
        for p,b in saved.items(): p.write_bytes(b)

if __name__=='__main__': main()
