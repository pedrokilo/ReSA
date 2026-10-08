"""Offline corrected replay audit; never launches or modifies the game."""
import argparse,gzip,hashlib,json,sys,tempfile,shutil
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE))
import run_local as bank
from hex_wire import decode_row,strict_loads
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def rows(p):
    with gzip.open(p/'comparisons.jsonl.gz','rb') as stream:
        for line in stream:yield line,decode_row(strict_loads(line))
class ArchivedLog:
    def __init__(self,p):self.p=p
    def open(self,mode):return gzip.open(self.p/'comparisons.jsonl.gz',mode)
def main():
    parser=argparse.ArgumentParser();parser.add_argument('run',type=Path);args=parser.parse_args();run=args.run.resolve()
    baseline=HERE/'runs/shake/20261008-160011-653030'
    report=json.loads((run/'enriched-summary.json').read_text())
    assert report['passed'] is True and report['all_equal'] is True and report['comparisons']==89600 and report['differences']==0
    assert report['natural_exit'] is True and report['exit_code']==0
    assert all(report[k] is True for k in ('bank_restored','floating_environment_restored','harness_restored','saves_unchanged','configs_restored','layout_file_restored'))
    manifest=report['manifest'];assert manifest['asi']=='015d30f78ffaf760ab56193d6040c5e7479fae42ea5cc339c95d7cde96efc75f'
    assert sha(run/'layout-proof.json')==manifest['layout_proof_sha256']=='f8c4ca915182fac5f249b4544a0ff29daf00ea09c3f8f67474ad94feb1ef7a97'
    frozen=json.loads((HERE/'corpus-frozen.json').read_text())
    for name,digest in frozen['inputs'].items():assert sha(ROOT/name)==digest
    for name,digest in manifest['bank_sources'].items():assert sha(ROOT/name)==digest
    bank.BANK='shake';bank.MANIFEST=manifest;bank.LAYOUT=json.loads((run/'layout-proof.json').read_text())
    # Rerun the unchanged strict oracle against archived bytes and a temporary
    # harness copy. No dependency on live game directories or runtime processes.
    with tempfile.TemporaryDirectory() as temp:
        bank.runner.GAME=Path(temp)
        shutil.copyfile(run/'harness.jsonl',Path(temp)/'preflight-pad-remaining3-hex-shake-harness.jsonl')
        bank.runner.terminal_event=lambda p:True
        strict=bank.analyze(ArchivedLog(run))
    assert strict['passed'] is True and strict['differences']==0
    raw=hashlib.sha256();canonical=hashlib.sha256();originals=hashlib.sha256();count=0;oldfail=0
    for (oldline,old),(line,current) in zip(rows(baseline),rows(run),strict=True):
        raw.update(line);canonical.update((json.dumps(current,sort_keys=True,separators=(',',':'))+'\n').encode())
        assert old['event']==current['event']
        if current['event']=='comparison':
            assert current['seed']==old['seed']==count
            # Independent native oracle continuity, including physical arenas,
            # guards, globals, returned-reference contract and FP payload.
            assert current['original']==old['original']
            assert current['candidate']==current['original']
            assert current['scratch_seed_bytes']==old['scratch_seed_bytes']
            for key in ('alias','clock','deadline','delay','duration','frequency','stored_frequency','gates','pad_id','domain'):assert current[key]==old[key]
            originals.update((json.dumps(current['original'],sort_keys=True,separators=(',',':'))+'\n').encode())
            count+=1;oldfail+=not old['equal']
    assert count==89600 and oldfail==17800
    archive=json.loads((run/'log-archive.json').read_text())
    assert archive['lossless_verified'] is True and sha(run/'comparisons.jsonl.gz')==archive['gzip_sha256']
    assert raw.hexdigest()==archive['raw_sha256']==report['wire_sha256']==strict['wire_sha256']
    assert canonical.hexdigest()==report['canonical_sha256']==strict['canonical_sha256']
    result=dict(passed=True,pairs=count,differences=0,baseline_differences=oldfail,native_original_full_payload_unchanged=True,candidate_equals_native_full_payload=True,strict_frozen_oracle_reexecuted=True,natural_exit0=True,all_ranges_flags_restored=True,manifest=manifest,wire_sha256=raw.hexdigest(),canonical_sha256=canonical.hexdigest(),original_payload_sha256=originals.hexdigest(),summary_sha256=sha(run/'enriched-summary.json'),harness_sha256=sha(run/'harness.jsonl'),audit_script_sha256=sha(Path(__file__)))
    (run/'independent-corrected-audit.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf8');print(json.dumps({k:v for k,v in result.items() if k!='manifest'},indent=2))
if __name__=='__main__':main()
