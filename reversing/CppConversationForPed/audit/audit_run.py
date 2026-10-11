"""Independently inspect every pair of both CConversationForPed functions before admitting the class compiled with /O2."""
import argparse
import gzip
import hashlib
import json
import shutil
import re
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
POSITION = 'CConversationForPed::IsPlayerInPositionForConversation'
UPDATE = 'CConversationForPed::Update'
SELECTED = {POSITION:2414, UPDATE:2288}
FAMILIES_BY_FUNCTION = {
    POSITION:{'distance':84,'facing':10,'speed':60,'damage':48,'availability':32,'placement-early-out':2,
              'mixed-distance':1080,'mixed-speed':1080,'x87-precision':18},
    UPDATE:{'state-timer':1200,'reply-branches':1024,'question-audio':32,'inactive-gates':32},
}
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
    assert args.bank == 'CppConversationForPed'
    bank = ROOT / 'reversing' / args.bank
    table = (bank / 'LiveComparison.hpp').read_text(encoding='utf-8')
    entries = re.findall(r'\{\"([^\"]+)\",\"([^\"]+)\",(?:0x[0-9A-Fa-f]+|0),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}', table)
    entries = [(cls,fn,count) for cls,fn,count in entries if cls+'::'+fn in SELECTED]
    expected = {cls+'::'+fn:int(count) for cls,fn,count in entries}
    assert expected == SELECTED
    assert expected and len(expected)==len(entries), 'Expected table must contain unique function identities'
    total = sum(expected.values())
    assert total == 4702 and all(sum(FAMILIES_BY_FUNCTION[name].values()) == count for name,count in expected.items())
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
    assert len(complete) == 1 and complete[0]['state_restored'] is True and rows[-1] == complete[0]
    assert complete[0]['pairs'] == total and complete[0]['expected'] == total
    assert not any(row.get('event') in ('error', 'assertion', 'first_chance_av', 'first_chance_av_raw') for row in rows)
    # Position pairs precede Update pairs, as in the historical bank (global pair order).
    assert [row['function'] for row in pairs] == [POSITION]*expected[POSITION] + [UPDATE]*expected[UPDATE]
    for name, families in FAMILIES_BY_FUNCTION.items():
        got = Counter(row['name'].rsplit('-', 1)[0] for row in pairs if row['function'] == name)
        assert got == Counter(families), f'Historical family split missing for {name}'
    for row in pairs:
        assert row.get('original') is not None and row.get('candidate') is not None, row['pair']
        assert row['accepted'] is True and row['equal'] is True and row['original'] == row['candidate'], row['pair']
        for side in ('original', 'candidate'):
            assert isinstance(row[side],dict), (row['pair'], side)
            assert row[side].get('switches') is True, (row['pair'], side)
            assert row[side].get('inputs_intact') is True, (row['pair'], side)
            assert isinstance(row[side].get('calls'),list), (row['pair'], side)
            assert isinstance(row[side].get('result'),bool) and all(key in row[side] for key in ('state','last','close','calls','string','fp_control')),(row['pair'],side)
        if row['function'] == UPDATE:
            assert row['original']['result'] is False, row['pair']
    results = Counter(row['original']['result'] for row in pairs if row['function'] == POSITION)
    assert results[True] > 0 and results[False] > 0, 'Both position outcomes must be exercised'
    # Update must exercise both advancing (trace/state change) and non-advancing cases.
    def advanced(row):
        original, case = row['original'], row['case']
        return bool(original['calls']) or original['current'] != 0
    update_rows = [row for row in pairs if row['function'] == UPDATE]
    assert any(advanced(row) for row in update_rows) and any(not advanced(row) for row in update_rows), 'Update outcomes not exercised'
    for name,count in expected.items():
        assert {row['case'] for row in pairs if row['function']==name} == set(range(count)),name
    fsum = [row for row in rows if row.get('event') == 'function_summary']
    assert not args.require_registered, 'This bank does not serialize a registered_hook begin flag; cannot certify it independently'
    assert len(fsum) == len(expected)
    assert {row['function']:row['cases'] for row in fsum} == expected
    assert all(row['failures']==0 and row['positive']>0 and row['negative']>0 for row in fsum)
    manifest = json.loads((bank / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
    assert summary['executed_probe_sha256'] == manifest['binaries']['asi']
    required = {'source/game_sa/Conversations/ConversationForPed.cpp','source/game_sa/Conversations/ConversationForPed.h',
                'source/game_sa/Conversations/Conversations.h','source/game_sa/Conversations/ConversationNode.h',
                'source/game_sa/Conversations/ConversationNode.cpp','source/game_sa/Pools/Pools.h',
                'source/game_sa/CompressedVector.h','source/StdInc.h','source/game_sa/Entity/Ped/Ped.h',
                'source/game_sa/Pad.h','source/CMakeLists.txt','CMakeLists.txt','source/InjectHooksMain.cpp',
                'source/reversiblehooks/RootHookCategory.h','source/reversiblehooks/ReversibleHook/Base.h'}
    assert required <= set(manifest['sources']), 'Required helper/options/routes source hashes missing'
    assert not any('ConversationMathCpp' in path or 'ConversationNodeCpp' in path for path in manifest['sources']), 'Removed helper files must not be hashed'
    assert sha(ROOT / 'source/game_sa/Game.cpp') == manifest['game_cpp']
    for path, digest in manifest['sources'].items():
        assert sha(ROOT / path) == digest, path
    for path, digest in manifest['instrumentation'].items():
        assert sha(bank / path) == digest, path
    result = dict(bank=args.bank, run=args.run.name, functions=len(expected), pairs=total, pairs_per_function=expected,
                  families=FAMILIES_BY_FUNCTION, differences=0, null_comparison_records=0, state_restored=True,
                  saves_unchanged=True, configs_restored=True, hook_routes_verified=True,
                  log_sha256=summary['log_sha256'], tested_sources=manifest['sources'],
                  bank_sources=manifest['instrumentation'], original_exe_sha256=EXE,
                  executed_probe_sha256=summary['executed_probe_sha256'])
    result['registered_hook_verified'] = args.require_registered
    result['auditor_sha256'] = sha(Path(__file__))
    result['route_field'] = 'switches (both entries 0x43AC40 and 0x43C190)'
    result['scope'] = ('Historical position and Update fixtures/results/state of CConversationForPed (class compiled whole with /O2); '
                       'yes/no keys driven by the real CPad state (inline in the candidate), so their calls are not in the trace; '
                       'no exhaustive x87 status/NaN contract, no raw register contract, Clear(bool) has no hook and is not tested directly')
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
