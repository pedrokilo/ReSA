"""External output-path adapter. Frozen bank/runner/ASI remain byte-exact."""
import gzip,hashlib,importlib.util,json,shutil,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
def digest(p):
    with p.open('rb') as stream:return hashlib.file_digest(stream,'sha256').hexdigest()
def main():
    bank=ROOT/'reversing'/sys.argv[1]
    assert bank.name in ('RequestedArrestRemainingFour','RequestedFxEmitterTwo')
    sys.path.insert(0,str(bank));sys.path.insert(0,str(ROOT/'reversing'))
    spec=importlib.util.spec_from_file_location('frozen_run_local',bank/'run_local.py');local=importlib.util.module_from_spec(spec);spec.loader.exec_module(local)
    runner=local.runner;prefix=local.PREFIX;custom=runner.GAME/(prefix+'.jsonl');manifest_path=bank/'artifacts/manifest.json';manifest_sha=digest(manifest_path);frozen_sha=digest(bank/'prepared-freeze.json');base_run=runner.run_case
    assert prefix in ('arrest-remaining-four','fx-emitter-two')
    archive=bank/'Diagnostics/output-path-adapter';archive.mkdir(parents=True,exist_ok=True)
    if custom.exists():
        old=archive/('previous-custom-'+digest(custom)+'.jsonl')
        if not old.exists():shutil.copy2(custom,old)
        custom.unlink()
    def adapted(received_prefix,asi,analyzer,*args,**kwargs):
        assert received_prefix==prefix
        return base_run(received_prefix,asi,lambda ignored_runtime_path:analyzer(custom),*args,**kwargs)
    before_runs=set((bank/'runs').glob('*/enriched-summary.json'))
    runner.run_case=adapted
    try:return local.main()
    finally:
        runner.run_case=base_run
        assert digest(manifest_path)==manifest_sha and digest(bank/'prepared-freeze.json')==frozen_sha
        candidates=[p for p in (bank/'runs').glob('*/enriched-summary.json') if p not in before_runs]
        if candidates:
            enriched=max(candidates,key=lambda p:p.stat().st_mtime_ns);destination=enriched.parent;result=json.loads(enriched.read_text(encoding='utf8'))
            harness=runner.GAME/('preflight-'+prefix+'-harness.jsonl')
            if harness.exists():shutil.copy2(harness,destination/'harness.jsonl')
            if custom.exists():
                raw=destination/'comparisons.jsonl';shutil.copy2(custom,raw);raw_sha=digest(raw)
                result['comparison_sha256']=raw_sha
                if raw.stat().st_size>90_000_000:
                    compressed=destination/'comparisons.jsonl.gz'
                    with raw.open('rb') as source,compressed.open('wb') as target:
                        with gzip.GzipFile(filename='',mode='wb',fileobj=target,mtime=0) as zipped:shutil.copyfileobj(source,zipped,4*1024*1024)
                    decoded=hashlib.sha256()
                    with gzip.open(compressed,'rb') as stream:
                        for block in iter(lambda:stream.read(4*1024*1024),b''):decoded.update(block)
                    assert decoded.hexdigest()==raw_sha
                    result['comparison_storage']={'uncompressed_path':raw.relative_to(ROOT).as_posix(),'uncompressed_sha256':raw_sha,'uncompressed_bytes':raw.stat().st_size,'compressed_path':compressed.relative_to(ROOT).as_posix(),'compressed_sha256':digest(compressed),'compression':'gzip','gzip_mtime':0,'lossless_roundtrip_sha256':decoded.hexdigest()}
                    (destination/'.gitignore').write_text('/comparisons.jsonl\n',encoding='utf8')
            result['execution_wrapper']={'path':Path(__file__).relative_to(ROOT).as_posix(),'sha256':digest(Path(__file__)),'original_manifest_sha256':manifest_sha,'original_frozen_sha256':frozen_sha,'original_runner_sha256':digest(bank/'run_local.py'),'only_override':'analyzer input path: prefix-runtime.jsonl -> prefix.jsonl; original arguments/kwargs/environment unchanged','actual_comparison_path':str(custom)}
            enriched.write_text(json.dumps(result,indent=2)+'\n',encoding='utf8')
            print('OUTPUT-PATH ADAPTER PRESERVED',destination,'passed',result.get('passed'))
if __name__=='__main__':raise SystemExit(main())
