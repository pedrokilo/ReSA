"""Build the rendering probe, restoring all instrumented sources in finally."""
import json
import hashlib
import shutil
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CMAKE = ROOT / 'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'

def main():
    paths = [ROOT/'source/game_sa/Clouds.cpp', ROOT/'source/game_sa/Sprite.cpp', ROOT/'source/app/app_game.cpp']
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
            content = data.decode('utf-8')
            include = 'LiveComparison.hpp' if p.name == 'app_game.cpp' else 'Probe.hpp'
            content = content.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/CClouds/'+include+'"')
            if p.name == 'Clouds.cpp':
                content = content.replace('void CClouds::RenderBottomFromHeight() {', 'void CClouds::RenderBottomFromHeight() {\n    ++CloudsProbe::calls;')
            elif p.name == 'Sprite.cpp':
                projection = 'bool CSprite::CalcScreenCoors(const RwV3d& posn, RwV3d* out, float* w, float* h, bool checkMaxVisible, bool checkMinVisible) {'
                content = content.replace(projection, projection+'\n    CloudsProbe::Record("projection", {posn.x,posn.y,posn.z}, {checkMaxVisible,checkMinVisible});')
                signature = 'void CSprite::RenderBufferedOneXLUSprite(CVector pos, CVector2D size, uint8 r, uint8 g, uint8 b, int16 intensity, float recipNearZ, uint8 alpha) {'
                content = content.replace(signature, signature+'\n    CloudsProbe::Record("sprite", {pos.x,pos.y,pos.z,size.x,size.y,recipNearZ}, {r,g,b,intensity,alpha});')
            else:
                content = content.replace('        CClouds::RenderBottomFromHeight();', '        CloudsLive::Update();\n        CClouds::RenderBottomFromHeight();')
            if content == data.decode('utf-8'): raise RuntimeError('Instrumentation anchor missing: '+str(p))
            p.write_text(content, encoding='utf-8')
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        hashes={}
        for ext in ('asi','pdb'):
            dest=artifacts/f'gta_reversed-clouds-probe.{ext}';shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}',dest);hashes[ext]=digest(dest)
        (artifacts/'probe-hashes.json').write_text(json.dumps(hashes,indent=2));print(json.dumps(hashes))
        manifest = {'binaries': hashes, 'sources': {str(p.relative_to(ROOT)): hashlib.sha256(data).hexdigest() for p, data in saved.items()},
                    'instrumentation': {p.name: digest(p) for p in (HERE/'Probe.hpp', HERE/'LiveComparison.hpp', HERE/'build_probe.py')}}
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest,indent=2))
    finally:
        for p,data in saved.items():p.write_bytes(data)

if __name__ == '__main__':main()
