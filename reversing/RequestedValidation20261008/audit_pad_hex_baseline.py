"""Read preserved negative baseline; independently audit native model and exact differences."""
import argparse,collections,gzip,hashlib,importlib.util,json,sys
from pathlib import Path
if not __debug__:raise RuntimeError('Audit requires assertions enabled')
ROOT=Path(__file__).resolve().parents[2];BANK=ROOT/'reversing/RequestedPadDistanceTrainNopHex'
def sha(path):
    with Path(path).open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--run',type=Path,required=True);args=parser.parse_args();run=args.run.resolve()
    report=json.loads((run/'enriched-summary.json').read_text());manifest=report['manifest']
    assert report['passed'] is False and report['negative_baseline'] is True and report['no_runtime_approval'] is True
    assert all(report.get(k) is True for k in ('natural_exit','saves_unchanged','configs_restored')) and report['exit_code']==0
    assert report['negative_launch_sha256']==sha(BANK/'run_negative_baseline.py')
    assert report['runner_sha256']==sha(BANK/'run_local.py')
    assert report['executed_probe_sha256']==manifest['asi']==sha(BANK/'artifacts/gta_reversed-pad-distance-train-nop-hex.asi')
    assert manifest['pdb']==sha(BANK/'artifacts/gta_reversed-pad-distance-train-nop-hex.pdb')
    assert report['original_exe_sha256']==manifest['exe_sha256']=='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
    assert all(sha(ROOT/p)==h for label in ('tested_sources','bank_sources') for p,h in manifest[label].items())
    assert sha(ROOT/'source/app/platform/win/WinMain.cpp')==manifest['original_winmain_sha256']
    source_inventory={p.relative_to(ROOT).as_posix() for p in (ROOT/'source').rglob('*') if p.is_file() and p.suffix in ('.cpp','.h','.hpp','.inl','.inc') and p.relative_to(ROOT).as_posix()!='source/app/platform/win/WinMain.cpp'}
    assert source_inventory=={p for p in manifest['tested_sources'] if p.startswith('source/')}
    proofpath=BANK/'artifacts/train-compiled-abi.json';assert sha(proofpath)==manifest['train_abi_proof_sha256'];proof=json.loads(proofpath.read_text())
    assert proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb'] and len(proof['functions'])==1
    returns=proof['functions'][0]['return_operands'];assert returns and all(int(x,0)==8 for x in returns)
    sys.path.insert(0,str(BANK));from hex_wire import strict_loads,decode_row
    from oracle import EXPECTED,expected_row
    counts=collections.Counter();differences=collections.Counter();fields={k:collections.Counter() for k in EXPECTED};byte_offsets={k:collections.Counter() for k in EXPECTED};examples={k:[] for k in EXPECTED};pairs=set();fixtures=set();terminal=None
    raw=run/'comparisons.jsonl';compressed=run/'comparisons.jsonl.gz';raw_hash=hashlib.sha256()
    opener=raw.open if raw.exists() else lambda mode:gzip.open(compressed,mode)
    with opener('rb') as stream:
        for line in stream:
            raw_hash.update(line);row=decode_row(strict_loads(line))
            if row['event']!='comparison':assert terminal is None;terminal=row;continue
            assert terminal is None;key=row['function'];case=row['case'];pair=row['pair']
            assert pair not in pairs and (key,case) not in fixtures;pairs.add(pair);fixtures.add((key,case));counts[key]+=1
            # This verifies native bytes/traces/model metadata independently. Candidate is assessed below;
            # its mismatch must not prevent auditing the original native model.
            native_view=dict(row,candidate=row['original']);expected=expected_row(native_view);assert row['original']==expected
            changed=[k for k in expected if row['candidate'][k]!=expected[k]]
            fp=row['candidate']['fp'];assert fp['control']&0x30300==0x20000 and fp['mxcsr']&0x6000==0
            assert row['equal'] is (not changed)
            if changed:
                differences[key]+=1;fields[key].update(changed)
                offsets=[i for i,(a,b) in enumerate(zip(expected['pad'],row['candidate']['pad'])) if a!=b];byte_offsets[key].update(offsets)
                if len(examples[key])<8:examples[key].append(dict(pair=pair,case=case,geometry=row['geometry'],gates=row['gates'],mode=row['mode'],vehicle_role=row['vehicle_role'],old_duration=row['old_duration'],fields=changed,pad_offsets=offsets,original_fp=row['original']['fp'],candidate_fp=fp))
    assert dict(counts)==EXPECTED and pairs==set(range(13952)) and fixtures=={(k,i) for k,n in EXPECTED.items() for i in range(n)}
    assert terminal['event']=='error' and terminal['pairs']==13952 and terminal['differences']==sum(differences.values())
    assert all(terminal[k] is True for k in ('state_restored','entries_restored','collaborator_entries_restored','globals_restored','flags_restored','fp_restored'))
    for before,after in (('fp_status_before','fp_status_after'),('fp_control_before','fp_control_after'),('mxcsr_before','mxcsr_after')):assert terminal[before]==terminal[after]
    assert report['pairs']==13952 and report['differences']==terminal['differences']
    hrows=[strict_loads(line) for line in (run/'harness.jsonl').read_text().splitlines()]
    assert [x['event'] for x in hrows]==['preflight_entered','before_task_bank','task_bank_returned','complete']
    assert all(hrows[-1][k] is True for k in ('state_restored','entries_restored','tables_restored','flags_restored','timer_restored'))
    storage=report.get('comparison_storage')
    if storage:
        assert raw_hash.hexdigest()==storage['uncompressed_sha256']==storage['lossless_roundtrip_sha256']
        assert sha(compressed)==storage['compressed_sha256']
        decoded=hashlib.sha256()
        with gzip.open(compressed,'rb') as stream:
            for block in iter(lambda:stream.read(4*1024*1024),b''):decoded.update(block)
        assert decoded.hexdigest()==raw_hash.hexdigest()
    result=dict(audit_complete=True,no_runtime_approval=True,registry_written=False,pairs=dict(counts),native_independent_oracle_cases=13952,decoded_types_exact=True,differences={k:differences[k] for k in EXPECTED},different_fields={k:dict(v) for k,v in fields.items()},pad_byte_difference_counts={k:{hex(i):n for i,n in v.items()} for k,v in byte_offsets.items()},examples=examples,terminal_restoration=terminal,harness_restoration=hrows[-1],natural_exit=True,exit_code=0,raw_log_sha256=raw_hash.hexdigest(),compressed_sha256=sha(compressed) if compressed.exists() else None,parent_summary_sha256=sha(run/'enriched-summary.json'),compiled_abi_proof_sha256=sha(proofpath),asi_sha256=manifest['asi'],pdb_sha256=manifest['pdb'])
    output=run/'independent-negative-audit.json';assert not output.exists();output.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps({k:result[k] for k in ('audit_complete','pairs','differences','different_fields','native_independent_oracle_cases','raw_log_sha256','no_runtime_approval')}))
if __name__=='__main__':main()
