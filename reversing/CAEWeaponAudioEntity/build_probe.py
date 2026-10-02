"""Compila la sonda temporal de CAEWeaponAudioEntity y restaura las fuentes por
bytes aunque la compilación falle.

Instrumentación temporal:
  - AEWeaponAudioEntity.cpp: `++WeaponAudioProbe::calls[i];` al principio de cada
    una de las 20 funciones registradas (contadores de ejecución del candidato).
  - Game.cpp: llamada a WeaponAudioLive::Update() al principio de CGame::Process.
Nunca deja estos cambios en el árbol: el ASI normal se compila aparte.
"""
import hashlib
import json
import re
import shutil
import subprocess
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import digest
ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
CMAKE = ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'

ENTITY_CPP = ROOT/'source/game_sa/Audio/Entities/AEWeaponAudioEntity.cpp'
GAME_CPP = ROOT/'source/game_sa/Game.cpp'
NAMES = [
    'Constructor', 'Destructor', 'Initialise', 'Terminate', 'Reset', 'WeaponFire', 'WeaponReload',
    'PlayGunSounds', 'PlayGoggleSound', 'ReportStealthKill', 'ReportChainsawEvent',
    'PlayChainsawStopSound', 'PlayWeaponLoopSound', 'PlayMiniGunFireSounds', 'PlayMiniGunStopSound',
    'PlayFlameThrowerSounds', 'PlayFlameThrowerIdleGasLoop', 'StopFlameThrowerIdleGasLoop',
    'PlayCameraSound', 'UpdateParameters',
]


def instrument_entity(text):
    assert text.count('#include "StdInc.h"') == 1
    text = text.replace('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../../../reversing/CAEWeaponAudioEntity/Probe.hpp"')
    for index, name in enumerate(NAMES):
        pattern = re.compile(r'^(?:void|CAEWeaponAudioEntity\*) CAEWeaponAudioEntity::' + name + r'\(', re.MULTILINE)
        matches = list(pattern.finditer(text))
        assert len(matches) == 1, f'definición no encontrada o repetida: {name}'
        brace = text.index('{', matches[0].end())
        text = text[:brace + 1] + f'\n    ++WeaponAudioProbe::calls[{index}];' + text[brace + 1:]
    return text


def patch(path, replacements):
    text = path.read_text(encoding='utf-8')
    for old, new in replacements:
        assert text.count(old) == 1, f'ancla no encontrada en {path.name}: {old!r}'
        text = text.replace(old, new)
    path.write_text(text, encoding='utf-8')


def main():
    paths = [ENTITY_CPP, GAME_CPP]
    saved = {p: p.read_bytes() for p in paths}
    artifacts = HERE/'artifacts'; artifacts.mkdir(exist_ok=True)
    try:
        ENTITY_CPP.write_text(instrument_entity(saved[ENTITY_CPP].decode('utf-8')), encoding='utf-8')
        patch(GAME_CPP, [
            ('#include "StdInc.h"', '#include "StdInc.h"\n#include "../../reversing/CAEWeaponAudioEntity/LiveComparison.hpp"'),
            ('void CGame::Process() {', 'void CGame::Process() {\n    WeaponAudioLive::Update();'),
        ])
        with (artifacts/'probe-build.log').open('w') as log:
            subprocess.run([str(CMAKE), '--build', str(ROOT/'build'), '--config', 'Debug'], stdout=log, stderr=subprocess.STDOUT, check=True)
        hashes = {}
        for ext in ('asi', 'pdb'):
            dest = artifacts/f'gta_reversed-weaponaudio-probe.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest); hashes[ext] = digest(dest)
        manifest = dict(
            binaries=hashes,
            sources={str(p.relative_to(ROOT)): hashlib.sha256(data).hexdigest() for p, data in saved.items()},
            instrumentation={p.name: digest(p) for p in (HERE/'Probe.hpp', HERE/'LiveComparison.hpp', HERE/'build_probe.py')},
        )
        (artifacts/'build-manifest.json').write_text(json.dumps(manifest, indent=2))
        print(json.dumps(hashes))
    finally:
        for p, data in saved.items():
            p.write_bytes(data)
        assert all(p.read_bytes() == data for p, data in saved.items()), 'fuentes no restauradas'


if __name__ == '__main__':
    main()
