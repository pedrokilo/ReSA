"""Install only the isolated normal build that passed shutdown regression tests."""
import csv
import json
import shutil
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE.parent))
from runtime_runner import GAME, EXE_HASH, digest, saves

def main():
    normal_path, probe_path = map(Path, sys.argv[1:])
    normal, probe = [json.loads(p.read_text()) for p in (normal_path, probe_path)]
    build = HERE/'artifacts/isolated-normal'
    manifest = json.loads((build/'manifest.json').read_text())
    probe_manifest = json.loads((HERE/'artifacts/isolated-probe/manifest.json').read_text())
    assert normal['passed'] and len(normal['runs']) == 3
    assert all(r['exit_code'] == 0 for r in normal['runs'])
    assert normal['asi'] == manifest['asi'] == digest(build/'gta_reversed.asi')
    assert probe['passed'] and probe['cleanup']['cleanup_cases'] == 16
    assert probe['asi'] == probe_manifest['asi']
    assert manifest['compiled_sources'] == probe_manifest['compiled_sources']
    assert not manifest['probe'] and digest(build/'gta_reversed.pdb') == manifest['pdb']
    for name, expected in manifest['compiled_sources'].items():
        if name not in manifest['isolated_files']:
            assert digest(ROOT/name) == expected, name
    for name, expected in manifest['workspace_sources'].items():
        assert digest(ROOT/name) == expected, name
    assert digest(GAME/'gta_sa.exe') == EXE_HASH
    rows = csv.reader(subprocess.check_output(['tasklist','/FO','CSV','/NH'],text=True).splitlines())
    assert not any(r and r[0].lower() in ('gta_sa.exe','gta-sa.exe') for r in rows)
    lock = GAME/'reversed-tests.lock'
    with lock.open('x') as f:
        f.write('install-shutdown')
    before = saves()
    configs = {str(p):digest(p) for p in (GAME/'gta-reversed.ini',GAME/'scripts/gta-reversed.ini')}
    backup = GAME/'_baseline'/time.strftime('before-shutdown-%Y%m%d-%H%M%S')
    backup.mkdir(parents=True)
    targets = []
    try:
        for label, directory in [('installed', GAME/'scripts'), ('build', ROOT/'bin/Debug')]:
            for ext in ('asi','pdb'):
                target = directory/f'gta_reversed.{ext}'
                saved = backup/f'{label}.{ext}'
                shutil.copy2(target, saved)
                targets.append((target,saved))
                shutil.copy2(build/target.name, target)
                assert digest(target) == manifest[ext]
        assert saves() == before
        assert all(digest(Path(p)) == h for p,h in configs.items())
        source_names = ['source/game_sa/Game.cpp','source/game_sa/Cover.cpp','source/game_sa/Cover.h',
                        'source/app/app_debug.cpp','source/app/app_debug.h',
                        'source/app/platform/win/WinMain.cpp','source/extensions/Configs/FastLoader.hpp']
        delivery = {k:manifest[k] for k in ('head','asi','pdb','probe','isolated_files','workspace_sources')}
        delivery['source_hashes'] = {p:manifest['compiled_sources'][p] for p in source_names}
        delivery['full_manifest_sha256'] = digest(build/'manifest.json')
        report = dict(passed=True, manifest=delivery, normal=normal, probe=probe,
                      normal_evidence=str(normal_path), probe_evidence=str(probe_path),
                      backup_directory=str(backup), saves_unchanged=True, ini_unchanged=True)
        (HERE/'installation-summary.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
        print(json.dumps({k:manifest[k] for k in ('asi','pdb')}))
    except BaseException:
        for target,saved in reversed(targets):
            shutil.copy2(saved,target)
        raise
    finally:
        lock.unlink()

if __name__ == '__main__':
    main()
