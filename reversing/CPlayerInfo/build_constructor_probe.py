"""Build temporary instrumentation and restore sources even when compilation fails."""
import hashlib
import json
import shutil
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CMAKE = ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'

def main():
    paths = [ROOT/'source/game_sa/PlayerInfo.cpp', ROOT/'source/game_sa/Game.cpp', ROOT/'source/game_sa/PlayerPedData.cpp']
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        for p, data in saved.items():
            (artifacts/(p.name+'.before')).write_bytes(data)
            content = data.decode('utf-8').replace('\r\n','\n')
            header = 'ConstructorLive.hpp' if p.name == 'Game.cpp' else 'ConstructorProbe.hpp'
            if p.name == 'PlayerInfo.cpp':
                content=content.replace('CPlayerInfo::CPlayerInfo() {','CPlayerInfo::CPlayerInfo() {\n    ++PlayerInfoConstructorProbe::calls;')
            elif p.name == 'PlayerPedData.cpp':
                content=content.replace('void CPlayerPedData::SetInitialState() {','void CPlayerPedData::SetInitialState() {\n    ++PlayerInfoConstructorProbe::initializations;')
            else:content=content.replace('void CGame::Process() {','void CGame::Process() {\n    PlayerInfoConstructorLive::Update();')
            content=content.replace('#include "StdInc.h"','#include "StdInc.h"\n#include "../../reversing/CPlayerInfo/'+header+'"')
            p.write_text(content, encoding='utf-8', newline='\n')
        with (artifacts/'constructor-build.log').open('w') as log:
            subprocess.run([str(CMAKE),'--build',str(ROOT/'build'),'--config','Debug'],stdout=log,stderr=subprocess.STDOUT,check=True)
        hashes = {}
        for ext in ('asi','pdb'):
            dest = artifacts/f'gta_reversed-playerinfo-constructor-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(helper_source=digest(ROOT/'source/game_sa/PlayerInfoProcess.cpp.inc'), binaries=hashes, sources={str(p.relative_to(ROOT)):hashlib.sha256(data).hexdigest() for p,data in saved.items()}, instrumentation={p.name:digest(p) for p in (HERE/'ConstructorProbe.hpp',HERE/'ConstructorLive.hpp',HERE/'build_constructor_probe.py')})
        manifest['dependencies']={name:digest(ROOT/name) for name in ('source/game_sa/PlayerInfo.h','source/game_sa/PathFind.cpp','source/game_sa/Tasks/TaskTypes/TaskComplexEnterBoatAsDriver.h')}
        (artifacts/'constructor-build-manifest.json').write_text(json.dumps(manifest,indent=2))
        print(json.dumps(hashes))
    finally:
        for p,data in saved.items(): p.write_bytes(data)

if __name__ == '__main__': main()
