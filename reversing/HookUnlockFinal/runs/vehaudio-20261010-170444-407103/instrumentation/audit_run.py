"""Reaudita el banco completo sin conceder crédito a corridas parciales."""
import collections
import hashlib
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent

def audit(run):
    source = ''.join(p.read_text(encoding='utf-8') for p in sorted(HERE.glob('*.hpp')))
    expected = {f'{a}::{b}': int(c) for a, b, c in re.findall(
        r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}', source)}
    assert len(expected) == 102
    summary = json.loads((run / 'summary.json').read_text(encoding='utf-8'))
    per, failures, summaries = collections.Counter(), collections.Counter(), {}
    cases, pair_ids, terminals, null_pass = set(), set(), [], 0
    with (run / 'comparisons.jsonl').open(encoding='utf-8') as stream:
        for line in stream:
            row = json.loads(line)
            event = row.get('event')
            if event == 'comparison':
                name = row['function']
                per[name] += 1
                key = (name, row['case'])
                assert key not in cases, key
                assert name in expected and 0 <= row['case'] < expected[name], key
                assert row['pair'] not in pair_ids
                pair_ids.add(row['pair'])
                cases.add(key)
                if row.get('equal') is not True or row.get('accepted') is not True:
                    failures[name] += 1
                elif row.get('original') is None or row.get('candidate') is None:
                    null_pass += 1
            elif event == 'function_summary':
                assert row['function'] not in summaries
                summaries[row['function']] = row
            elif event in ('complete', 'error'):
                terminals.append(row)
    report = dict(functions=len(per), pairs=sum(per.values()), differences=sum(failures.values()),
                  failed_functions=dict(failures), null_passing_records=null_pass, terminals=terminals,
                  missing_functions=sorted(set(expected)-set(summaries)))
    print(json.dumps(report, indent=2))
    assert per == expected and len(cases) == sum(expected.values()), 'corrida incompleta'
    assert pair_ids == set(range(sum(expected.values())))
    assert not failures and null_pass == 0
    assert set(summaries) == set(expected)
    for name, row in summaries.items():
        assert row['cases'] == expected[name] and row['failures'] == 0, name
    assert len(terminals) == 1 and terminals[0]['event'] == 'complete'
    assert terminals[0]['pairs'] == sum(expected.values()) and terminals[0]['state_restored'] is True
    for flag in ('passed', 'all_equal', 'state_restored', 'saves_unchanged', 'configs_restored'):
        assert summary[flag] is True, flag
    assert summary['pairs'] == sum(expected.values()) and summary['pairs_per_function'] == expected
    assert summary['log_sha256'] == hashlib.sha256((run/'comparisons.jsonl').read_bytes()).hexdigest()
    assert summary['original_exe_sha256'] == '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
    return expected, summary

if __name__ == '__main__':
    audit(Path(sys.argv[1]))
