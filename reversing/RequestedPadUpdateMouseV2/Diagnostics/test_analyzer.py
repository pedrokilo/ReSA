"""Synthetic analyzer contract: rejects matching wrong models, bad gates and duplicates."""
import copy,importlib.util,json,tempfile,sys
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];sys.path.insert(0,str(HERE))
spec=importlib.util.spec_from_file_location('local_runner',HERE/'run_local.py');local=importlib.util.module_from_spec(spec);spec.loader.exec_module(local)
old=local.runner.GAME
with tempfile.TemporaryDirectory(dir=HERE/'Diagnostics') as tmp:
    local.runner.GAME=Path(tmp);h=local.runner.GAME/'preflight-pad-update-mouse-harness.jsonl'
    stages=[{'event':e} for e in ('preflight_entered','before_task_bank','task_bank_returned','complete')];stages[-1].update(snapshot_ranges=17692,changed_ranges=[],**{k:True for k in ('state_restored','entries_restored','flags_restored','timer_restored','tables_restored')});h.write_text('\n'.join(json.dumps(x) for x in stages)+'\n',encoding='utf8')
    def write(mode):
        log=Path(tmp)/('comparisons-'+mode+'.jsonl');begin={'event':'begin','helpers_known':True,'mouse_address':123456,'mouse_guard_before':[1]*16,'mouse_guard_after':[2]*24}
        with log.open('w',encoding='utf8') as stream:
            stream.write(json.dumps(begin)+'\n')
            for i in range(local.COUNT):
                f=local.fixture(i);payload=local.expected(f,begin);leg={'payload':payload,**{k:True for k in local.REQUIRED}};x={'event':'comparison','function':'CPad::UpdateMouse','fixture':f,'pair':i,'equal':True,'original':copy.deepcopy(leg),'candidate':copy.deepcopy(leg)}
                if i==0:
                    if mode=='matching_wrong_model':
                        for name in ('original','candidate'):x[name]['payload']['mouse'][7]^=1
                    elif mode=='bad_gate':x['candidate']['abi_valid']=False
                    elif mode=='duplicate':x['pair']=1
                stream.write(json.dumps(x,separators=(',',':'))+'\n')
            stream.write(json.dumps({'event':'complete','comparisons':9216,'differences':0,'all_equal':True,'state_restored':True,'fp_restored':True})+'\n')
        return local.analyze(log)
    outcomes={}
    try:
        for mode in ('valid','matching_wrong_model','bad_gate','duplicate'):
            r=write(mode);outcomes[mode]={'passed':r['passed'],'models':r['model_mismatches'],'errors':r['errors']};assert r['passed'] is (mode=='valid'),mode
    finally:local.runner.GAME=old
(HERE/'Diagnostics/analyzer-contract-test.json').write_text(json.dumps(outcomes,indent=2)+'\n',encoding='utf8');print(json.dumps(outcomes,indent=2))
