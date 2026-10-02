"""Compila una sonda temporal que solo engancha CGame::Process y restaura Game.cpp aunque falle.

Uso desde cada banco:  build(Path(__file__).parent, prefix='inform', namespace='InformLive')
El banco aporta LiveComparison.hpp (namespace con Update()); se archivan ASI/PDB y un manifiesto.
"""
import hashlib
import json
import os
import shutil
import subprocess
from pathlib import Path
from runtime_runner import digest, GAME

ROOT = Path(__file__).resolve().parents[1]
CMAKE = next((ROOT/'reversing/CVehicle/conan-cache/p').glob('cmake*/p/bin/cmake.exe'))
GAME_CPP = ROOT/'source/game_sa/Game.cpp'

def build(here: Path, prefix: str, namespace: str, tested_sources=(), install=True):
    here = Path(here).resolve()
    saved = GAME_CPP.read_bytes()
    artifacts = here/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        content = saved.decode('utf-8').replace('\r\n', '\n')
        anchor = 'void CGame::Process() {'
        if content.count(anchor) != 1: raise RuntimeError('Game.cpp')
        rel = os.path.relpath(here/'LiveComparison.hpp', GAME_CPP.parent).replace('\\', '/')
        content = content.replace(anchor, anchor + f'\n    {namespace}::Update();')
        content = content.replace('#include "StdInc.h"', f'#include "StdInc.h"\n#include "{rel}"', 1)
        GAME_CPP.write_text(content, encoding='utf-8', newline='\n')
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE), '--build', str(ROOT/'build'), '--config', 'Debug'], stdout=log, stderr=subprocess.STDOUT, check=True)
    finally:
        GAME_CPP.write_bytes(saved)
    hashes = {}
    for ext in ('asi', 'pdb'):
        dest = artifacts/f'gta_reversed-{prefix}-probe.{ext}'
        shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        if install: shutil.copy2(dest, GAME/'_baseline'/dest.name)
    manifest = dict(binaries=hashes,
                    sources={str(Path(p)).replace('\\', '/'): hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in tested_sources},
                    game_cpp=hashlib.sha256(saved).hexdigest(),
                    instrumentation={p.name: digest(p) for p in sorted(here.glob('*.hpp')) + sorted(here.glob('*.py'))})
    (artifacts/'build-manifest.json').write_text(json.dumps(manifest, indent=2))
    return hashes
