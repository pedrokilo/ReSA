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
SELECTED = {'CClouds::Update': 1024, 'CClouds::RenderSkyPolys': 1024, 'CClouds::MovingFogRender': 1024}
EXE = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def decode_numeric(raw, mxcsr):
    assert isinstance(raw, str) and len(raw) == 216, 'FP108 must contain exactly 108 physical bytes'
    image = bytes.fromhex(raw)
    word = lambda pos: int.from_bytes(image[pos:pos + 2], 'little')
    status, tags = word(4), word(8)
    top = (status >> 11) & 7
    live = []
    for index in range(8):
        physical = (top + index) & 7
        classification = (tags >> (physical * 2)) & 3
        if classification != 3:
            live.append(dict(logical=index, physical=physical, tag=classification,
                             bits80=image[28 + index * 10:38 + index * 10].hex()))
    assert isinstance(mxcsr, int) and not isinstance(mxcsr, bool) and 0 <= mxcsr <= 0xffffffff
    return image, dict(control_word=word(0), status_word=status, tag_word=tags,
                       live_registers80=live, mxcsr=mxcsr)


def verify_fp(row, side):
    output = row[side]
    image, numeric = decode_numeric(row[side + '_fp108'], row[side + '_mxcsr'])
    assert output['post_fp_numeric'] == numeric, (row['pair'], side, 'numeric payload differs from physical FP108')
    assert numeric['control_word'] & 0xf00 == 0, (row['pair'], side, 'Requires PC24 and nearest rounding')
    metadata = row[side + '_fp_metadata']
    for name, start, length in [('fip',12,4), ('fdp',20,4), ('instruction_selector',16,2), ('opcode',18,2), ('data_selector',24,2)]:
        assert metadata[name] == int.from_bytes(image[start:start+length], 'little'), (row['pair'], side, name)
    assert isinstance(metadata['fip_module'], int) and metadata['fip_module'] > 0
    assert isinstance(metadata['fip_code16'], str) and len(bytes.fromhex(metadata['fip_code16'])) == 16
    # No masking/normalizing status, TOP, live numeric registers or MXCSR occurs here.
    # FIP/FDP identify relocated code/data; their raw bytes and runtime instruction evidence remain retained.


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('bank')
    parser.add_argument('run', type=Path)
    parser.add_argument('--archive', action='store_true')
    parser.add_argument('--require-registered', action='store_true')
    args = parser.parse_args()
    assert args.bank == 'CppCloudsMath3', 'This independent audit only accepts CppCloudsMath3'
    bank = ROOT / 'reversing' / args.bank
    table = (bank / 'LiveComparison.hpp').read_text(encoding='utf-8')
    entries = re.findall(r'\{\"([^\"]+)\",\"([^\"]+)\",(?:0x[0-9A-Fa-f]+|0),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}', table)
    expected = {cls+'::'+fn:int(count) for cls,fn,count in entries if cls+'::'+fn in SELECTED}
    selected_entries = [(cls,fn,count) for cls,fn,count in entries if cls+'::'+fn in SELECTED]
    assert expected == SELECTED and len(selected_entries) == 3, 'Expected exactly the three 1024-case consumers'
    assert expected and len(expected)==len(selected_entries), 'Expected table must contain unique function identities'
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
    for name, count in SELECTED.items():
        assert {row['case'] for row in pairs if row['function'] == name} == set(range(count)), name
    for row in pairs:
        assert row.get('original') is not None and row.get('candidate') is not None, row['pair']
        assert row['accepted'] is True and row['equal'] is True and row['original'] == row['candidate'], row['pair']
        for side in ('original', 'candidate'):
            assert isinstance(row[side], dict), (row['pair'], side, 'Full object payload required')
            for gate in ('route_ok', 'switches', 'abi_ok', 'side_fp_restored', 'numeric_abi_ok'):
                assert row[side].get(gate) is True, (row['pair'], side, gate)
            verify_fp(row, side)
    fsum = [row for row in rows if row.get('event') == 'function_summary']
    begin = [row for row in rows if row.get('event') == 'begin']
    assert len(begin) == 1 and begin[0]['registered_hook'] is True
    assert begin[0]['functions'] == 3 and begin[0]['pairs'] == 3072 and begin[0]['collaborators_stubbed'] is True
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
    result['registered_hook_verified'] = True
    result['fp108_independently_decoded'] = True
    result['fp_numeric_exact_fields'] = ['control_word','status_word','tag_word','live_registers80','mxcsr']
    result['scope'] = 'Three C++ helper conversions plus faithful x87 consumer comparisons; instrumented collaborators; no transitive approval'
    result['independent_audit_sha256'] = sha(Path(__file__))
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
