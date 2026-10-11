"""Independently inspect every pair before admitting a new original dependency."""
import argparse
import csv
import gzip
import hashlib
import json
import shutil
import re
import struct
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SELECTED = {'CTaskSimpleSwim::ProcessSwimmingResistance': 1024}
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
    assert args.bank == 'CppTrigCompound6Swim'
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
    for row in pairs:
        assert row.get('original') is not None and row.get('candidate') is not None, row['pair']
        assert row['accepted'] is True and row['equal'] is True and row['original'] == row['candidate'], row['pair']
        for side in ('original', 'candidate'):
            assert isinstance(row[side],dict), (row['pair'], side)
            assert row[side].get('switches') is True, (row['pair'], side)
            assert 'ret' in row[side] and isinstance(row[side].get('trace'),list), (row['pair'], side)
            assert row[side].get('clean') is True,(row['pair'],side)
    for name,count in expected.items():
        assert {row['case'] for row in pairs if row['function']==name} == set(range(count)),name
    subsin_cases = {'original': [], 'candidate': []}
    for row in pairs:
        fixtures = {side: row.get('fixture_'+side) for side in ('original', 'candidate')}
        assert isinstance(fixtures['original'], dict) and isinstance(fixtures['candidate'], dict), ('Missing entry fixture', row['pair'])
        assert fixtures['original'] == fixtures['candidate'], ('Different entry fixtures', row['pair'])
        for side in ('original', 'candidate'):
            fixture = fixtures[side]
            state = fixture.get('swim_state')
            assert type(state) is int and 0 <= state <= 65535, ('Invalid entry state', row['pair'], side)
            values = {}
            for field in ('rotation_x_bits', 'state_changer_bits'):
                bits = fixture.get(field)
                assert type(bits) is int and 0 <= bits <= 0xFFFFFFFF, ('Invalid raw float bits', row['pair'], side, field)
                values[field] = struct.unpack('<f', struct.pack('<I', bits))[0]
            water = [call for call in row[side]['trace'] if isinstance(call, list) and call and call[0] == 'water_level']
            assert all(len(call) == 8 and type(call[6]) is bool for call in water), ('Invalid water return trace', row['pair'], side)
            if state == 4:
                assert len(water) == 1, ('State 4 must query water exactly once', row['pair'], side)
                # State 4 remains unchanged before the query. Negative rotation
                # excludes the preceding rotation > quarter-pi branch.
                if not (values['state_changer_bits'] < 0.0) and values['rotation_x_bits'] < 0.0 and water[0][6] is True:
                    subsin_cases[side].append(row['case'])
    assert all(len(cases) >= 1 for cases in subsin_cases.values()), 'SubSin branch has no independently demonstrated case'
    assert subsin_cases['original'] == subsin_cases['candidate'], 'SubSin cases differ between sides'

    fsum = [row for row in rows if row.get('event') == 'function_summary']
    assert not args.require_registered, 'This bank does not serialize a registered_hook begin flag; cannot certify it independently'
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
    result['SubSinCases'] = {side: len(cases) for side, cases in subsin_cases.items()}
    result['observed_coverage_cases'] = subsin_cases
    result['coverage_scope'] = 'SubSin evaluation inferred independently from identical entry fixtures: swim state 4, !(stateChanger < 0), ordered rotationX < 0, and the single water_level trace returning true. Source flow reaches SubSin under these conditions; this is branch inference, not a helper execution counter or additional FP/register ABI measurement.'
    result['route_field'] = 'switches'
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
