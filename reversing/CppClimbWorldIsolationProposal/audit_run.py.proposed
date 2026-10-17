"""Independently inspect every pair before admitting a new original dependency."""
import argparse
import csv
import gzip
import hashlib
import json
import shutil
import re
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SELECTED = {'CTaskSimpleClimb::GetCameraStickModifier': 512, 'CTaskSimpleClimb::GetCameraTargetPos': 512, 'CTaskSimpleClimb::ProcessPed': 2048, 'CTaskSimpleClimb::TestForClimb': 1024, 'CTaskSimpleClimb::TestForStandUp': 256, 'CTaskSimpleClimb::TestForVault': 256}
EXE = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('bank')
    parser.add_argument('run', type=Path)
    parser.add_argument('--archive', action='store_true')
    parser.add_argument('--require-registered', action='store_true')
    args = parser.parse_args()
    assert args.bank == 'CppTrigClimb'
    bank = ROOT / 'reversing' / args.bank
    table = (bank / 'LiveComparison.hpp').read_text(encoding='utf-8')
    entries = re.findall(r'\{\"([^\"]+)\",\"([^\"]+)\",(?:0x[0-9A-Fa-f]+|0),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}', table)
    entries = [(cls,fn,count) for cls,fn,count in entries if cls+'::'+fn in SELECTED]
    expected = {cls+'::'+fn:int(count) for cls,fn,count in entries}
    assert expected == SELECTED
    assert expected and len(expected)==len(entries), 'Expected table must contain unique function identities'
    total = sum(expected.values())
    summary = json.loads((args.run / 'summary.json').read_text(encoding='utf-8'))
    assert all(summary[key] is True for key in
               ('passed', 'all_equal', 'state_restored', 'saves_unchanged', 'configs_restored'))
    assert summary['original_exe_sha256'] == EXE
    assert summary['pairs'] == total and summary['pairs_per_function'] == expected
    raw = (args.run / 'comparisons.jsonl').read_bytes()
    assert hashlib.sha256(raw).hexdigest() == summary['log_sha256']
    rows = [json.loads(line) for line in raw.decode('utf-8').splitlines()]
    pairs = [row for row in rows if row.get('event') == 'comparison']
    complete = [row for row in rows if row.get('event') == 'complete']
    assert len(pairs) == total and Counter(row['function'] for row in pairs) == expected
    assert {row['pair'] for row in pairs} == set(range(total))
    assert len({(row['function'], row['case']) for row in pairs}) == total
    assert len(complete) == 1 and complete[0]['state_restored'] is True
    assert complete[0]['pairs'] == total
    assert not any(row.get('event') in ('error', 'assertion', 'first_chance_av', 'first_chance_av_raw') for row in rows)
    starts=[row for row in rows if row.get('event')=='begin']
    assert len(starts)==1 and starts[0].get('world_isolation')=={'regular_sectors':14400,'repeat_sectors':256,'fixture_sectors':9,'heads_only':True}, 'Full world isolation not demonstrated'
    for row in pairs:
        assert row.get('original') is not None and row.get('candidate') is not None, row['pair']
        assert row['accepted'] is True and row['equal'] is True and row['original'] == row['candidate'], row['pair']
        for side in ('original', 'candidate'):
            assert isinstance(row[side],dict), (row['pair'], side)
            assert row[side].get('switches') is True, (row['pair'], side)
            assert 'ret' in row[side] and isinstance(row[side].get('trace'),list), (row['pair'], side)
            assert row[side].get('clean') is True, (row['pair'],side)
            for call in row[side]['trace']:
                if call[0]=='get_bound_centre':
                    assert len(call)==2 and call[1] in {'ent'+str(i)+'+0' for i in range(6)}, (row['pair'],side,'External world entity reached',call)
    for name,count in expected.items():
        assert {row['case'] for row in pairs if row['function']==name} == set(range(count)),name
    coverage_cases=[row['case'] for row in pairs if row['function']=='CTaskSimpleClimb::GetCameraStickModifier' and any(isinstance(event,list) and event and event[0]=='process_vertical_line' for event in row['candidate']['trace'])]
    assert coverage_cases, 'No GetCameraStickModifier trace proves entry through the sin/cos offset branch'
    fsum = [row for row in rows if row.get('event') == 'function_summary']
    if args.require_registered:
        begin = [row for row in rows if row.get('event') == 'begin']
        assert len(begin) == 1 and begin[0]['registered_hook'] is True
        assert begin[0]['functions']==len(SELECTED) and begin[0]['pairs']==sum(SELECTED.values())
        assert begin[0]['collaborators_stubbed'] is True
    assert len(fsum) == len(expected)
    assert {row['function']:row['cases'] for row in fsum} == expected
    assert all(row['failures']==0 for row in fsum)
    manifest = json.loads((bank / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
    assert summary['executed_probe_sha256'] == manifest['binaries']['asi']
    assert sha(ROOT / 'source/game_sa/Game.cpp') == manifest['game_cpp']
    for path, digest in manifest['sources'].items():
        assert sha(ROOT / path) == digest, path
    for path, digest in manifest['instrumentation'].items():
        assert sha(bank / path) == digest, path
    result = dict(bank=args.bank, run=args.run.name, functions=len(expected), pairs=total, pairs_per_function=expected,
                  differences=0, null_comparison_records=0, state_restored=True,
                  saves_unchanged=True, configs_restored=True, hook_routes_verified=True,
                  log_sha256=summary['log_sha256'], tested_sources=manifest['sources'],
                  bank_sources=manifest['instrumentation'], original_exe_sha256=EXE,
                  executed_probe_sha256=summary['executed_probe_sha256'])
    result['registered_hook_verified'] = args.require_registered
    result['auditor_sha256'] = sha(Path(__file__))
    result['trig_coverage_cases'] = coverage_cases
    result['trig_coverage_evidence'] = 'Control-flow inference: GetCameraStickModifier ProcessVerticalLine occurs only after GetClimbOffset3D, whose sine and cosine calls are unconditional. Not hardware counters.'
    result['route_field'] = 'switches'
    result['world_isolation_verified'] = starts[0]['world_isolation']
    result['external_bound_centre_calls'] = 0
    result['scope'] = 'Legacy consumer fixtures, results/state/traces; collaborators instrumented; no full FP108/EFLAGS/register ABI claim'
    if args.archive:
        dest = bank / 'runs' / args.run.name
        assert not dest.exists()
        dest.mkdir(parents=True)
        for name in ('summary.json', 'RESULTADO.txt'):
            shutil.copy2(args.run / name, dest / name)
            assert sha(dest / name) == sha(args.run / name)
        with (dest / 'comparisons.jsonl.gz').open('wb') as stream:
            with gzip.GzipFile(filename='', mode='wb', fileobj=stream, mtime=0) as zipped:
                zipped.write(raw)
        with gzip.open(dest / 'comparisons.jsonl.gz', 'rb') as stream:
            assert hashlib.sha256(stream.read()).hexdigest() == summary['log_sha256']
        (dest / 'approval-audit.json').write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
