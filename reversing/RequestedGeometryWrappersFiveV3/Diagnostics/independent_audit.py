"""Read-only artifact preflight, optionally audit preserved native replay."""
import argparse,hashlib,json,sys
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE))
import run_local
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    p=argparse.ArgumentParser();p.add_argument('--run',type=Path);a=p.parse_args()
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text());proof=json.loads((HERE/'Diagnostics/required-callchain-proof.json').read_text())
    frozen=json.loads((HERE/'prepared-freeze.json').read_text())
    for name,digest in frozen.items():assert sha(ROOT/name)==digest,name
    for name,digest in manifest['bank_sources'].items():assert sha(ROOT/name)==digest,name
    for ext in ('asi','pdb'):assert sha(HERE/f'artifacts/gta_reversed-geometry-wrappers-five-v3.{ext}')==manifest[ext]==proof[ext+'_sha256']
    assert manifest['asi']=='7310777cdb24a960977bf160fe07f80433df9657b90dd9bafe617b268ed166f7'
    assert proof['run_ready'] is True and proof['CEntity_named_export_vptr']['matched'] is True
    assert proof['symbol_table_sha256']==sha(HERE/'Diagnostics/compiled-symbols.json')
    assert set(proof['calls_cross_patched_entry'])=={'Corners','CentreAll','PlanesAll','SegmentAll'}
    patched={v['patched_entry'] for v in proof['actualDLLhelperpatchentry_from_Update'].values()}
    assert all(call['patched_entry'] in patched for calls in proof['calls_cross_patched_entry'].values() for call in calls)
    result=dict(artifact_preflight_passed=True,asi_sha256=manifest['asi'],pdb_sha256=manifest['pdb'],frozen_inputs=len(frozen),callchain_proof_sha256=sha(HERE/'Diagnostics/required-callchain-proof.json'),scope='Isolated wrapper forwarding/payload/pointer identity only; no primitive, constructor, destructor, physics or FP-state accreditation.')
    if a.run:
        run=a.run.resolve();report=json.loads((run/'enriched-summary.json').read_text());assert report['natural_exit'] is True and report['exit_code']==0 and report['passed'] is True
        assert report['manifest']==manifest
        analyzed=run_local.base_analyze(run/'comparisons.jsonl');assert analyzed['all_equal'] is True and analyzed['state_restored'] is True
        rows=[json.loads(x) for x in (run/'comparisons.jsonl').read_text().splitlines()];pairs=[r for r in rows if r['event']=='comparison']
        assert len(pairs)==5120 and [r['pair'] for r in pairs]==list(range(5120))
        for r in pairs:
            assert r['wrapper']==r['pair']//1024 and r['case']==r['pair']%1024
            for side in ('original','candidate'):
                pointers=r['raw_pointer_audit'][0 if side=='original' else 1]['pointers']
                expected=['corners_producer']
                if r['wrapper'] in (1,3,4):expected+=['corners_consumer','centre_producer']
                if r['wrapper']==2:expected+=['corners_consumer']
                if r['wrapper'] in (3,4):expected+=['corners_consumer','centre_consumer']
                assert [q['role'] for q in pointers]==expected
                assert all(type(q['address']) is int and 0<q['address']<=0xffffffff for q in pointers)
        result.update(runtime_passed=True,pairs=5120,differences=0,fullarena_model_trace_and_pointer_roles_checked=True,natural_exit0=True,run_summary_sha256=sha(run/'enriched-summary.json'),log_sha256=sha(run/'comparisons.jsonl'))
    (HERE/'Diagnostics'/('independent-runtime-audit.json' if a.run else 'independent-preflight-audit.json')).write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2))
if __name__=='__main__':main()
