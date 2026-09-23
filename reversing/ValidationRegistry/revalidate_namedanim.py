"""Rebuild and rerun the existing bank, retaining historical probes and inputs."""
import json
import argparse
import importlib.util
import os
import shutil
import subprocess
import sys
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
sys.path.insert(0, str(ROOT / 'reversing'))
from validation_registry import digest, tree_digest
from runtime_runner import GAME, saves, run_case


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--case', choices=('namedanim-init', 'ik-data', 'rpanimblend'), default='namedanim-init')
    case = parser.parse_args().case
    folder = {'namedanim-init': 'FallenPeds', 'ik-data': 'IKDataWarnings', 'rpanimblend': 'RpAnimBlend'}[case]
    bank = ROOT / 'reversing' / folder
    env = dict(os.environ, PYTHONDONTWRITEBYTECODE='1')
    stamp = datetime.now().strftime('%Y%m%d-%H%M%S')
    out = ROOT / 'reversing/ValidationRegistry/artifacts' / stamp
    out.mkdir(parents=True, exist_ok=False)
    probe = bank / 'artifacts/probe'
    if probe.exists():
        shutil.copytree(probe, out / 'previous-probe')
    baseline = [GAME / f'_baseline/gta_reversed-{case}-probe.{ext}' for ext in ('asi', 'pdb')]
    for p in baseline:
        if p.exists():
            shutil.copy2(p, out / ('baseline-' + p.name))
    protected = [ROOT / f'bin/Debug/gta_reversed.{ext}' for ext in ('asi', 'pdb')]
    protected += [GAME / f'scripts/gta_reversed.{ext}' for ext in ('asi', 'pdb')]
    protected += [GAME / p for p in ('gta-reversed.ini', 'scripts/gta-reversed.ini')]
    if case == 'ik-data':
        protected += [GAME / 'data' / p for p in ('timecyc.dat', 'vehicles.ide', 'cargrp.dat')]
    protected_hashes = {str(p): digest(p) for p in protected}
    before_saves = saves()
    trees = {p: tree_digest(ROOT / p) for p in ('source', 'cmake', 'conan') if (ROOT / p).is_dir()}
    inputs = ['CMakeLists.txt', 'CMakePresets.json', 'conanfile.py',
              f'reversing/{folder}/LiveComparison.hpp', f'reversing/{folder}/Probe.hpp',
              f'reversing/{folder}/build_probe.py', f'reversing/{folder}/run_local.py',
              'reversing/runtime_runner.py', 'reversing/ProbeSupport.hpp', 'reversing/ProbeRuntime.hpp']
    if case == 'ik-data':
        inputs.append('reversing/IKDataWarnings/data-repair.json')
    files = {p: digest(ROOT / p) for p in inputs}
    result = dict(passed=False, trees=trees, files=files, saves_before=before_saves,
                  protected_before=protected_hashes, head=subprocess.check_output(
                      ['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip())
    cache = ROOT / 'build/CMakeCache.txt'
    shutil.copy2(cache, out / 'CMakeCache.txt')
    result['cmake_cache_sha256'] = digest(cache)
    reports = set((GAME / f'resultados-{case}').glob('*/summary.json'))
    try:
        subprocess.run([sys.executable, str(bank / 'build_probe.py')],
                       cwd=ROOT, env=env, check=True)
        shutil.copytree(probe, out / 'tested-probe')
        result['manifest'] = json.loads((probe / 'manifest.json').read_text())
        if case == 'rpanimblend':
            # Keep the archived runner's fixed historical hash unchanged.
            for ext in ('asi', 'pdb'):
                shutil.copy2(probe / f'gta_reversed-{case}-probe.{ext}',
                             GAME / f'_baseline/gta_reversed-{case}-probe.{ext}')
            spec = importlib.util.spec_from_file_location('rp_bank', bank / 'run_local.py')
            module = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(module)
            if run_case(case, result['manifest']['asi'], module.analyze,
                        'RpAnimBlend: revalidacion actual', timeout=180, stop_before_analysis=True):
                raise RuntimeError('Runtime comparison failed')
        else:
            subprocess.run([sys.executable, str(bank / 'run_local.py')],
                           cwd=ROOT, env=env, check=True)
        new = set((GAME / f'resultados-{case}').glob('*/summary.json')) - reports
        if len(new) != 1:
            raise RuntimeError('Expected exactly one new runtime report')
        report = new.pop()
        result['runtime_report'] = str(report)
        result['runtime'] = json.loads(report.read_text())
        if not result['runtime']['passed']:
            raise RuntimeError('Runtime comparison failed')
        result['passed'] = True
    finally:
        # Retain the historical bank as the default runner input.
        if (out / 'previous-probe').exists():
            shutil.copytree(out / 'previous-probe', probe, dirs_exist_ok=True)
        for p in baseline:
            saved = out / ('baseline-' + p.name)
            if saved.exists():
                shutil.copy2(saved, p)
            elif p.exists():
                p.unlink()
        result['sources_restored'] = all(tree_digest(ROOT / p) == h for p, h in trees.items())
        result['inputs_unchanged'] = all(digest(ROOT / p) == h for p, h in files.items())
        result['protected_restored'] = all(digest(Path(p)) == h for p, h in protected_hashes.items())
        result['saves_unchanged'] = saves() == before_saves
        result['passed'] &= all(result[k] for k in ('sources_restored', 'inputs_unchanged', 'protected_restored', 'saves_unchanged'))
        (out / 'summary.json').write_text(json.dumps(result, indent=2), encoding='utf-8')
        print('REVALIDATION:', out / 'summary.json', 'PASSED:', result['passed'])
    if not result['passed']:
        raise RuntimeError('Revalidation or restoration failed')


if __name__ == '__main__':
    main()
