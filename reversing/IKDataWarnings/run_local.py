"""Compare IK construction and repaired weather/car-group data inside GTA."""
import json
import shutil
import sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent))
from runtime_runner import GAME, digest, run_case, terminal_event

def analyze(path):
    if not terminal_event(path):
        return None
    rows = [json.loads(s) for s in path.read_text().splitlines()]
    last = rows[-1]
    chains = [r for r in rows if r['event'] == 'chain']
    if (last['event'] != 'complete' or len(chains) != 131
        or len(chains) != last['chain_pairs'] or any(not r['equal'] for r in chains)
        or last['chain_differences'] or last['timecycle_different_bytes']
        or not all(last[k] for k in ('counters_valid','bone_nodes_returned','groups_equal','state_restored','skeleton_unchanged','skimmer_defaults_explicit'))):
        raise RuntimeError(str(last))
    assert any(r['effector'] == 5 and r['pivot'] == 4 for r in chains)
    assert len({(r['effector'],r['pivot']) for r in chains}) == 131
    assert all(r['original_candidate_calls'] == 0 and r['candidate_calls'] == 1 for r in chains)
    assert [r for r in rows if r['event'] == 'timecycle'] == [dict(event='timecycle', bytes=9384, different_bytes=0)]
    return dict(validation=last, pairs=len(chains), log_sha256=digest(path),
                scope='IK SetupBones semantic pairs plus native weather-table and active car-group comparison')

def main():
    manifest = json.loads((HERE / 'artifacts/probe/manifest.json').read_text())
    data = json.loads((HERE / 'data-repair.json').read_text())
    for name in ('timecyc.dat','vehicles.ide','cargrp.dat'):
        assert digest(GAME / 'data' / name) == data[name]['after_sha256'], 'Repair data first (see README)'
    for ext in ('asi','pdb'):
        src = HERE / f'artifacts/probe/gta_reversed-ik-data-probe.{ext}'
        assert digest(src) == manifest[ext]
        shutil.copy2(src, GAME / '_baseline' / src.name)
    def check(path):
        result = analyze(path)
        if result is not None:
            result['probe_sha256'] = manifest['asi']
        return result
    return run_case('ik-data', manifest['asi'], check, 'IK y datos corregidos', timeout=180,
                    stop_before_analysis=True, unhook='')

if __name__ == '__main__':
    sys.exit(main())
