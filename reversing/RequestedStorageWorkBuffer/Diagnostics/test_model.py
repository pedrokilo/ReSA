import copy,json,sys,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];sys.path.insert(0,str(HERE));import run_local as bank
start=dict(event='start',pairs=2048,work_address=0x10000000,caller_address=0x20000000,globals=bytes(range(24)).hex(),size_globals=bytes(range(32)).hex(),fp=dict(rounding=0,exceptions=0,mxcsr=8096))
def row(fn,seed):
    leg=bank.expected_leg(start,fn,seed)
    return dict(event='comparison',fn=fn,pair=fn*1024+seed,case=seed,pattern=seed%8,size=[-1,0,1,2,3,4,7,16][(seed//8)%8],position=[0,1,2,3,16,51168,51176,51184][(seed//64)%8],prior_failed=seed//512!=0,equal=True,accepted=True,original=leg,candidate=leg)
with tempfile.TemporaryDirectory()as temp:
    bank.runner.GAME=Path(temp);h=[dict(event=e)for e in ['preflight_entered','before_task_bank','task_bank_returned','complete']];h[-1].update(snapshot_ranges=17692,changed_ranges=[],state_restored=True,entries_restored=True,flags_restored=True,floating_environment_restored=True,timer_unchanged=True,bank_fp_unchanged=True)
    (bank.runner.GAME/'preflight-storage-work-buffer-harness.jsonl').write_text('\n'.join(map(json.dumps,h))+'\n');p=bank.runner.GAME/'model.jsonl'
    with p.open('w')as f:
        f.write(json.dumps(start)+'\n')
        for fn in range(2):
            for seed in range(1024):f.write(json.dumps(row(fn,seed))+'\n')
        f.write(json.dumps(dict(event='complete',pairs=2048,failures=0,passed=True,state_restored=True))+'\n')
    assert bank.analyze(p)['pairs']==2048
    rejected=[]
    for name in ['wrong_both_AL','wrong_both_byte','wrong_position','wrong_FP','false_gate','false_accepted','duplicate_seed','invalid_bool_type','uppercase_hex','missing_pairs']:
        r=copy.deepcopy(row(0,0))
        if name=='wrong_position':r['position']=1
        elif name=='false_accepted':r['accepted']=False
        elif name=='duplicate_seed':r['case']=1
        elif name=='invalid_bool_type':r['prior_failed']=0
        else:
            for side in ('original','candidate'):
                leg=r[side]
                if name=='wrong_both_AL':leg['returned_al']=0
                elif name=='wrong_both_byte':leg['work']='00'+leg['work'][2:]
                elif name=='wrong_FP':leg['fp']['mxcsr']=0
                elif name=='false_gate':leg['switch_exact']=False
                elif name=='uppercase_hex':leg['work']=leg['work'].upper()
        p.write_text(json.dumps(start)+'\n'+json.dumps(r)+'\n'+json.dumps(dict(event='complete',pairs=2048,failures=0,passed=True,state_restored=True))+'\n')
        try:bank.analyze(p)
        except (AssertionError,StopIteration):rejected.append(name)
        else:raise AssertionError(name)
(HERE/'Diagnostics/model-tests.json').write_text(json.dumps(dict(passed=True,synthetic_pairs=2048,negative_rejected=rejected,no_game_or_build=True),indent=2));print('passed2048',rejected)
