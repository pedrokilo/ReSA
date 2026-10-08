"""Explicit runtime-only2400-second retry of the exact frozen1200-second shake runner.
Does not rebuild, edit base instruments or change any other run_case argument.
"""
import hashlib,json,os,sys,traceback
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE))
import run_local as base

def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    frozen=json.loads((HERE/'corpus-frozen.json').read_text(encoding='utf8'))
    for path,expected in frozen['inputs'].items():assert digest(base.ROOT/path)==expected,path
    manifest_path=HERE/'artifacts/shake/manifest.json'
    manifest_hash=digest(manifest_path)
    manifest=json.loads(manifest_path.read_text(encoding='utf8'))
    assert manifest['selected_bank']=='shake' and manifest['comparisons']==89600
    assert manifest['asi']=='b0b9a0cf11df831577a224c901a30ecf84b947df02110795af39e9cf7bc5a094'
    assert sys.argv[1:]==['--bank','shake']
    original=base.runner.run_case
    environment=dict(os.environ)
    before=set((HERE/'runs/shake').glob('*')) if (HERE/'runs/shake').exists() else set()
    calls=[];status=None;exception=None
    def extended(*args,**kwargs):
        assert len(args)==4 and args[0]=='requested-pad-remaining3-shake' and args[1]==manifest['asi']
        assert args[2] is base.analyze and kwargs.get('timeout')==1200
        revised=dict(kwargs);revised['timeout']=2400
        assert {key:value for key,value in revised.items() if key!='timeout'}=={key:value for key,value in kwargs.items() if key!='timeout'}
        calls.append({'original_timeout_seconds':1200,'effective_timeout_seconds':2400,'only_changed_argument':'timeout','original_kwargs':kwargs,'effective_kwargs':revised,'same_positional_arguments':True,'same_original_analyzer':True,'same_asi_sha256':args[1]})
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
        after=set((HERE/'runs/shake').glob('*')) if (HERE/'runs/shake').exists() else set()
        produced=after-before
        metadata={'execution_wrapper_path':Path(__file__).relative_to(base.ROOT).as_posix(),'execution_wrapper_sha256':digest(Path(__file__)),'base_runner_path':(HERE/'run_local.py').relative_to(base.ROOT).as_posix(),'base_runner_sha256':digest(HERE/'run_local.py'),'base_manifest_sha256':manifest_hash,'effective_timeout_seconds':2400,'original_timeout_seconds':1200,'overrides':calls,'environment_unchanged':True,'frozen_inputs_unchanged':True,'base_manifest_unchanged':True,'wrapper_status':status,'wrapper_exception':exception,'retry_scope':'exact frozenStartShake89600, timeoutonly, no partial approval'}
        for folder in produced:
            target=folder/'enriched-summary.json'
            if target.exists():
                summary=json.loads(target.read_text(encoding='utf8'));summary.update(metadata);target.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf8')
            (folder/'execution-wrapper.json').write_text(json.dumps(metadata,indent=2)+'\n',encoding='utf8')
        if not produced:
            (HERE/'artifacts/shake/extended2400-wrapper-last-error.json').write_text(json.dumps(metadata,indent=2)+'\n',encoding='utf8')
if __name__=='__main__':raise SystemExit(main())
