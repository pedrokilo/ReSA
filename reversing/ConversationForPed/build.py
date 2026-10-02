"""Build an isolated candidate and restore sources and normal binaries byte for byte."""
import argparse
import hashlib
import json
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
TARGET = 'source/game_sa/Conversations/ConversationForPed.cpp'
HEADER = 'source/game_sa/Conversations/ConversationForPed.h'
GAME_SOURCE = 'source/game_sa/Game.cpp'
sys.path.insert(0, str(HERE.parent))
from ShutdownLists.build_isolated import ISOLATE
from runtime_runner import digest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--probe', action='store_true')
    args = parser.parse_args()
    folder = HERE / 'artifacts' / datetime.now().strftime('%Y%m%d-%H%M%S-%f')
    folder.mkdir(parents=True)
    saved = {p: (ROOT / p).read_bytes() for p in (*ISOLATE, TARGET, HEADER, GAME_SOURCE)}
    for path, data in saved.items():
        (folder / (Path(path).name + '.before')).write_bytes(data)
    for ext in ('asi', 'pdb'):
        shutil.copy2(ROOT / f'bin/Debug/gta_reversed.{ext}', folder / f'before.{ext}')
    try:
        for path, revision in ISOLATE.items():
            (ROOT / path).write_bytes(subprocess.check_output(['git', 'show', f'{revision}:{path}'], cwd=ROOT))
        if args.probe:
            from instrument import instrument
            for path in (TARGET, GAME_SOURCE):
                source = (ROOT / path).read_text(encoding='utf-8')
                (ROOT / path).write_text(instrument(path, source), encoding='utf-8', newline='\n')
        sources = subprocess.check_output(
            ['git', 'ls-files', 'source', 'cmake', 'conan', 'CMakeLists.txt', 'CMakePresets.json', 'conanfile.py'],
            cwd=ROOT, text=True,
        ).splitlines()
        manifest = {
            'head': subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip(),
            'probe': args.probe,
            'isolated_files': ISOLATE,
            'workspace_sources': {p: hashlib.sha256(b).hexdigest() for p, b in saved.items()},
            'instrumentation': {p.name: digest(p) for p in HERE.iterdir() if p.suffix in ('.hpp', '.py')},
            'normal_binaries_before': {ext: digest(folder / f'before.{ext}') for ext in ('asi', 'pdb')},
            'compiled_sources': {p: digest(ROOT / p) for p in sources if (ROOT / p).is_file()},
        }
        # Keep a manifest even when compilation fails; binary hashes are added only on success.
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
        latest = 'latest-probe.json' if args.probe else 'latest-normal.json'
        (HERE / latest).write_text(json.dumps({'folder': str(folder)}, indent=2), encoding='utf-8')
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
