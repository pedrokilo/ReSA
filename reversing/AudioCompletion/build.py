"""Compila una sonda aislada del lote de audio y restaura fuentes y binarios normales byte a byte.

Mismo esquema que ConversationForPed/build.py: aplica ISOLATE, instrumenta las tres
funciones candidatas (sólo sustituye proveedores externos) y CGame::Process, compila y
archiva el ASI/PDB con un manifiesto de hashes.
"""
import hashlib
import json
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
TARGETS = (
    'source/game_sa/Audio/Managers/AEAmbienceTrackManager.cpp',
    'source/game_sa/Audio/Hardware/AEStaticChannel.cpp',
    'source/game_sa/Audio/Entities/AEWeatherAudioEntity.cpp',
)
GAME_SOURCE = 'source/game_sa/Game.cpp'
sys.path.insert(0, str(HERE.parent))
sys.path.insert(0, str(HERE))
from ShutdownLists.build_isolated import ISOLATE
from runtime_runner import digest
from instrument import instrument


def main():
    folder = HERE / 'artifacts' / datetime.now().strftime('%Y%m%d-%H%M%S-%f')
    folder.mkdir(parents=True)
    saved = {p: (ROOT / p).read_bytes() for p in (*ISOLATE, *TARGETS, GAME_SOURCE)}
    for path, data in saved.items():
        (folder / (Path(path).name + '.before')).write_bytes(data)
    for ext in ('asi', 'pdb'):
        shutil.copy2(ROOT / f'bin/Debug/gta_reversed.{ext}', folder / f'before.{ext}')
    try:
        for path, revision in ISOLATE.items():
            (ROOT / path).write_bytes(subprocess.check_output(['git', 'show', f'{revision}:{path}'], cwd=ROOT))
        for path in (*TARGETS, GAME_SOURCE):
            source = (ROOT / path).read_bytes().decode('utf-8').replace('\r\n', '\n')
            (ROOT / path).write_text(instrument(path, source), encoding='utf-8', newline='\n')
        sources = subprocess.check_output(
            ['git', 'ls-files', 'source', 'cmake', 'conan', 'CMakeLists.txt', 'CMakePresets.json', 'conanfile.py'],
            cwd=ROOT, text=True,
        ).splitlines()
        manifest = {
            'head': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
            'probe': True,
            'isolated_files': ISOLATE,
            'workspace_sources': {p: hashlib.sha256(b).hexdigest() for p, b in saved.items()},
            'instrumentation': {p.name: digest(p) for p in HERE.iterdir() if p.suffix in ('.hpp', '.py')},
            'normal_binaries_before': {ext: digest(folder / f'before.{ext}') for ext in ('asi', 'pdb')},
            'compiled_sources': {p: digest(ROOT / p) for p in sources if (ROOT / p).is_file()},
        }
        (folder / 'manifest.json').write_text(json.dumps(manifest, indent=2), encoding='utf-8')
        with (folder / 'build.log').open('w', encoding='utf-8') as log:
            subprocess.run([
                str(ROOT / 'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'),
                '--build', str(ROOT / 'build'), '--config', 'Debug',
            ], cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
        for ext in ('asi', 'pdb'):
            dest = folder / f'gta_reversed.{ext}'
            shutil.copy2(ROOT / f'bin/Debug/gta_reversed.{ext}', dest)
            manifest[ext] = digest(dest)
        (folder / 'manifest.json').write_text(json.dumps(manifest, indent=2), encoding='utf-8')
        (HERE / 'latest-probe.json').write_text(json.dumps({'folder': str(folder)}, indent=2), encoding='utf-8')
        print(folder, flush=True)
    finally:
        for path, data in saved.items():
            (ROOT / path).write_bytes(data)
        for ext in ('asi', 'pdb'):
            shutil.copy2(folder / f'before.{ext}', ROOT / f'bin/Debug/gta_reversed.{ext}')
        if not all((ROOT / p).read_bytes() == data for p, data in saved.items()):
            raise RuntimeError('No se restauraron todas las fuentes.')
        for ext in ('asi', 'pdb'):
            if digest(ROOT / f'bin/Debug/gta_reversed.{ext}') != digest(folder / f'before.{ext}'):
                raise RuntimeError(f'No se restauró el binario normal .{ext}.')


if __name__ == '__main__':
    main()
