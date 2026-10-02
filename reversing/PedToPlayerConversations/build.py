"""Build a timestamped candidate, preserving deferred sources and normal binaries."""
import hashlib, json, shutil, subprocess, sys
from datetime import datetime
from pathlib import Path
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE.parent))
from ShutdownLists.build_isolated import ISOLATE
from runtime_runner import digest

def main():
    probe = '--probe' in sys.argv
    folder = HERE/'artifacts'/datetime.now().strftime('%Y%m%d-%H%M%S')
    folder.mkdir(parents=True)
    target = 'source/game_sa/Conversations/PedToPlayerConversations.cpp'
    game = 'source/game_sa/Game.cpp'
    paths = [*ISOLATE, target, game, 'source/game_sa/Conversations/PedToPlayerConversations.h']
    saved = {p: (ROOT/p).read_bytes() for p in paths}
    for p, b in saved.items():
        (folder/(Path(p).name+'.before')).write_bytes(b)
    for ext in ('asi','pdb'):
        shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', folder/f'before.{ext}')
    try:
        for p, ref in ISOLATE.items():
            (ROOT/p).write_bytes(subprocess.check_output(['git','show',f'{ref}:{p}'], cwd=ROOT))
        if probe:
            from instrument import instrument
            for p in (target, game):
                text = (ROOT/p).read_text(encoding='utf-8')
                (ROOT/p).write_text(instrument(p, text), encoding='utf-8', newline='\n')
        manifest = {
            'head': subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),
            'probe': probe, 'isolated_files': ISOLATE,
            'workspace_sources': {p: hashlib.sha256(b).hexdigest() for p,b in saved.items()},
            'instrumentation': {p.name:digest(p) for p in HERE.iterdir() if p.suffix in ('.hpp','.py')},
            'normal_binaries_before': {ext:digest(folder/f'before.{ext}') for ext in ('asi','pdb')},
            'compiled_sources': {p: digest(ROOT/p) for p in subprocess.check_output(['git','ls-files','source'],cwd=ROOT,text=True).splitlines()},
        }
        with (folder/'build.log').open('w') as log:
            subprocess.run([str(ROOT/'reversing/CVehicle/conan-cache/p/cmake20ac4e0137fee/p/bin/cmake.exe'), '--build', str(ROOT/'build'), '--config','Debug'], cwd=ROOT, stdout=log, stderr=subprocess.STDOUT, check=True)
        for ext in ('asi','pdb'):
            dest = folder/f'gta_reversed.{ext}'
            shutil.copy2(ROOT/f'bin/Debug/gta_reversed.{ext}', dest)
            manifest[ext] = digest(dest)
        (folder/'manifest.json').write_text(json.dumps(manifest,indent=2),encoding='utf-8')
        (HERE/('latest-probe.json' if probe else 'latest-normal.json')).write_text(json.dumps({'folder':str(folder)}),encoding='utf-8')
        print(folder, flush=True)
    finally:
        for p,b in saved.items(): (ROOT/p).write_bytes(b)
        for ext in ('asi','pdb'): shutil.copy2(folder/f'before.{ext}', ROOT/f'bin/Debug/gta_reversed.{ext}')
        assert all((ROOT/p).read_bytes()==b for p,b in saved.items())

if __name__ == '__main__': main()
