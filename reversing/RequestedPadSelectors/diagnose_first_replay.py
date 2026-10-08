"""Stream immutable baseline results; never trust its overwritten passed=true."""
import collections,hashlib,json
from pathlib import Path
HERE=Path(__file__).resolve().parent;RUN=HERE/'runs/20261008-124241-603231'
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def domain(seed):return 'pressure' if seed<2048 else 'onehot' if seed<3328 else 'flags' if seed<3424 else 'analog' if seed<3748 else 'signed_short_stress'
def main():
    summary=json.loads((RUN/'enriched-summary.json').read_text());counts=collections.Counter();domains=collections.defaultdict(collections.Counter);samples=collections.defaultdict(list);total=0
    for line in (RUN/'comparisons.jsonl').open(encoding='utf-8'):
        row=json.loads(line)
        if row.get('event')!='comparison':continue
        total+=1;assert row['switch_exact']
        if row['equal']:continue
        fn=row['function'];counts[fn]+=1;domains[fn][domain(row['seed'])]+=1
        assert row['pad_equal'] and row['globals_equal'],'Unexpected side-effect mismatch'
        if len(samples[fn])<8:samples[fn].append({k:row[k] for k in ('seed','original_bits','candidate_bits')})
    assert total==201552 and sum(counts.values())==590
    harness=json.loads((RUN/'harness.jsonl').read_text().splitlines()[-1]);assert harness['state_restored'] and harness['entries_restored'] and harness['flags_restored'] and not harness['changed_ranges']
    result={'accepted':False,'reason':'590 original/candidate return mismatches; baseline generic runner overwrote false verdict. Original summary preserved.','comparisons':total,'differences':590,'normal_domain_differences':584,'signed_short_stress_differences':6,'natural_exit':summary['natural_exit'],'exit_code':summary['exit_code'],'restoration':harness,'enriched_summary_sha256':digest(RUN/'enriched-summary.json'),'comparison_log_sha256':digest(RUN/'comparisons.jsonl'),'failures_by_function':dict(counts),'fixture_domains':dict(domains),'examples':dict(samples),'baseline_runner_source':'reversing/RequestedPadSelectors/sources-archive/run_local-first-replay.py','baseline_runner_sha256':digest(HERE/'sources-archive/run_local-first-replay.py')}
    (HERE/'FIRST_REPLAY_DIAGNOSIS.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8');print(json.dumps({'normal':584,'stress':6,'failures':counts}))
if __name__=='__main__':main()
