"""Independent lossless Hex/Phase baseline comparison; no game or shared source writes."""
import gzip,hashlib,json,sys
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE))
from hex_wire import decode_row,strict_loads
phase=ROOT/'reversing/RequestedPadMutationRemaining3Phased/runs/shake/20261008-154320-419455'
hexf=HERE/'runs/shake/20261008-160011-653030'
def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
reports=[json.loads((p/'enriched-summary.json').read_text()) for p in (phase,hexf)]
for p,j in zip((phase,hexf),reports):
    assert j['passed'] is False and j['comparisons']==89600 and j['differences']==17800 and j['natural_exit'] is True and j['exit_code']==0
    assert all(j[k] is True for k in ('bank_restored','floating_environment_restored','harness_restored','saves_unchanged','configs_restored','layout_file_restored'))
    assert j['manifest']['tested_sources']['source/game_sa/Pad.cpp']=='412b814d1a7e94b1ae4871e02de25efd5d18aa54ec507fda1cb80f26def5a1db'
    h=[json.loads(v) for v in (p/'harness.jsonl').read_text().splitlines()]
    assert [v['event'] for v in h]==['preflight_entered','before_task_bank','task_bank_returned','complete']
    assert h[-1]['snapshot_ranges']>=17692 and h[-1]['changed_ranges']==[] and all(h[-1][k] is True for k in ('state_restored','entries_restored','flags_restored'))
digests=[hashlib.sha256(),hashlib.sha256()];canonical=hashlib.sha256();comparison_hash=hashlib.sha256();failures=[];seen=[];payload_diffs=0;terminals=[]
with gzip.open(phase/'comparisons.jsonl.gz','rb') as a,gzip.open(hexf/'comparisons.jsonl.gz','rb') as b:
    while True:
        la,lb=a.readline(),b.readline()
        if not la and not lb:break
        assert bool(la)==bool(lb),'Different record count'
        digests[0].update(la);digests[1].update(lb)
        pa=json.loads(la);hb=decode_row(strict_loads(lb));canonical.update((json.dumps(hb,sort_keys=True,separators=(',',':'))+'\n').encode())
        assert pa['event']==hb['event']
        if pa['event']=='comparison':
            assert pa==hb,'Full raw canonical record differs at seed '+str(pa.get('seed'))
            seed=hb['seed'];assert seed==len(seen);seen.append(seed)
            assert all(hb[k] is True for k in ('switch_exact','guards_unchanged','inactive_scratch_unchanged','return_reference_exact','globals_unchanged','helper_entries_original'))
            comparison_hash.update((json.dumps(hb,sort_keys=True,separators=(',',':'))+'\n').encode())
            if not hb['equal']:failures.append(seed)
        elif pa['event']=='complete':
            # Artifact-dependent peak estimates may differ because of transport allocation.
            assert all(pa[k]==hb[k] for k in pa if k!='phased_original_buffer_estimate_bytes')
            terminals=[pa,hb]
        elif pa['event']=='start':assert pa==hb
        elif pa['event']=='current_artifact_proof':
            for row,report in [(pa,reports[0]),(hb,reports[1])]:
                proof=row['proof'];assert proof['artifact_sha256']==report['manifest']['asi'] and proof['pdb_sha256']==report['manifest']['pdb'] and proof['mapped_body_exact'] is True and proof['artifact_hash_exact'] is True
assert len(seen)==89600 and len(failures)==17800
assert [v['seed'] for v in reports[0]['failures']]==failures==[v['seed'] for v in reports[1]['failures']]
archives=[json.loads((p/'log-archive.json').read_text()) for p in (phase,hexf)]
for i,p in enumerate((phase,hexf)):
    assert digests[i].hexdigest()==archives[i]['raw_sha256']
    assert sha(p/'comparisons.jsonl.gz')==archives[i]['gzip_sha256'] and archives[i]['lossless_verified'] is True
assert digests[1].hexdigest()==reports[1]['wire_sha256'] and canonical.hexdigest()==reports[1]['canonical_sha256']
result=dict(passed=True,meaning='Transport baseline audit only, function remains failed',pairs=89600,differences=17800,all_comparison_payloads_and_metadata_identical=True,failed_seed_set_exact=True,failures_sha256=hashlib.sha256(json.dumps(failures).encode()).hexdigest(),wire_sha256=digests[1].hexdigest(),phase_raw_sha256=digests[0].hexdigest(),hex_canonical_sha256=canonical.hexdigest(),comparison_canonical_sha256=comparison_hash.hexdigest(),all_17692_ranges_flags_restored=True,natural_exit0_both=True,peak_retained_estimates=[v['phased_original_buffer_estimate_bytes'] for v in terminals],source_compiled_sha256=reports[1]['manifest']['tested_sources']['source/game_sa/Pad.cpp'],enriched_summary_hashes=[sha(p/'enriched-summary.json') for p in (phase,hexf)])
(HERE/'Diagnostics/baseline-transport-audit.json').write_text(json.dumps(result,indent=2));print(json.dumps(result,indent=2))
