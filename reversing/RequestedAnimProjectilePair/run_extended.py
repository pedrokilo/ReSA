"""Explicit runtime-only1200-second retry of the exact frozen600-second runner.
Does not rebuild, edit base instruments or change any other run_case argument.
"""
import hashlib,json,os,sys,traceback
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE))
import run_local as base

def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    frozen=json.loads((HERE/'prepared-freeze.json').read_text(encoding='utf8'))
    for path,expected in frozen['inputs'].items():assert digest(base.ROOT/path)==expected,path
    manifest_path=HERE/'artifacts/manifest.json'
    manifest_hash=digest(manifest_path)
    manifest=json.loads(manifest_path.read_text(encoding='utf8'))
    assert manifest['asi']=='43c7b63599ffc4046a9ac1a0db3c96a10a9fe9bd40ce837227dbb1262f8fe4d8'
    original=base.runner.run_case
    environment=dict(os.environ)
    before=set((HERE/'runs').glob('*')) if (HERE/'runs').exists() else set()
    calls=[];status=None;exception=None
    def extended(*args,**kwargs):
        assert len(args)==4 and args[0]==base.PREFIX and args[1]==manifest['asi']
        assert args[2] is base.analyze and kwargs.get('timeout')==600
        revised=dict(kwargs);revised['timeout']=1200
        assert {key:value for key,value in revised.items() if key!='timeout'}=={key:value for key,value in kwargs.items() if key!='timeout'}
        calls.append({'original_timeout_seconds':600,'effective_timeout_seconds':1200,'only_changed_argument':'timeout','original_kwargs':kwargs,'effective_kwargs':revised,'same_positional_arguments':True,'same_original_analyzer':True,'same_asi_sha256':args[1]})
        return original(*args,**revised)
    base.runner.run_case=extended
    try:
        status=base.main()
        return status
    except BaseException:
        exception=traceback.format_exc()
        raise
    finally:
        base.runner.run_case=original
        assert dict(os.environ)==environment,'Execution wrapper changed environment'
        assert digest(manifest_path)==manifest_hash,'Execution wrapper changed build manifest'
        for path,expected in frozen['inputs'].items():assert digest(base.ROOT/path)==expected,path
        after=set((HERE/'runs').glob('*')) if (HERE/'runs').exists() else set()
        produced=after-before
        metadata={'execution_wrapper_path':Path(__file__).relative_to(base.ROOT).as_posix(),'execution_wrapper_sha256':digest(Path(__file__)),'base_runner_path':(HERE/'run_local.py').relative_to(base.ROOT).as_posix(),'base_runner_sha256':digest(HERE/'run_local.py'),'base_manifest_sha256':manifest_hash,'effective_timeout_seconds':1200,'original_timeout_seconds':600,'overrides':calls,'environment_unchanged':True,'frozen_inputs_unchanged':True,'base_manifest_unchanged':True,'wrapper_status':status,'wrapper_exception':exception,'previous_rejected_run':'reversing/RequestedAnimProjectilePair/runs/20261008-142758-927336/enriched-summary.json'}
        for folder in produced:
            target=folder/'enriched-summary.json'
            if target.exists():
                summary=json.loads(target.read_text(encoding='utf8'));summary.update(metadata);target.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf8')
            (folder/'execution-wrapper.json').write_text(json.dumps(metadata,indent=2)+'\n',encoding='utf8')
        if not produced:
            (HERE/'artifacts/extended-wrapper-last-error.json').write_text(json.dumps(metadata,indent=2)+'\n',encoding='utf8')
if __name__=='__main__':raise SystemExit(main())
