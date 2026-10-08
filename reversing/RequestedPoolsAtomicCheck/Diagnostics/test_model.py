import copy,json,sys,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];sys.path.insert(0,str(HERE));import run_local as bank
fixture=dict(event='fixture',pool_globals=[0]*68,fp=dict(rounding=0,exceptions=0,mxcsr=8064))
rows=[dict(event='start',cases=256,function='CPools::CheckBuildingAtomics',address=0x550170),fixture]
for seed in range(256):
    leg=dict(returned=0xffffffff,stack_balanced=True,callee_saved=True,fp=fixture['fp'],switch_exact=True,guard=[(seed*47+i*29)%256 for i in range(64)],guard_unchanged=True,pool_globals=[0]*68,globals_unchanged=True)
    rows.append(dict(event='comparison',function=rows[0]['function'],pair=seed,case=seed,eax_seed=(seed*2654435769)%4294967296,equal=True,accepted=True,original=copy.deepcopy(leg),candidate=copy.deepcopy(leg)))
rows.append(dict(event='complete',pairs=256,failures=0,state_restored=True,floating_environment_restored=True,passed=True))
with tempfile.TemporaryDirectory() as tmp:
    root=Path(tmp);bank.runner.GAME=root;log=root/'bank.jsonl'
    h=[dict(event=v) for v in ['preflight_entered','before_task_bank','task_bank_returned','complete']];h[-1].update(snapshot_ranges=17692,changed_ranges=[],state_restored=True,entries_restored=True,flags_restored=True,floating_environment_restored=True,timer_unchanged=True)
    (root/'preflight-pools-atomic-check-harness.jsonl').write_text('\n'.join(map(json.dumps,h))+'\n')
    def check(values):log.write_text('\n'.join(map(json.dumps,values))+'\n');return bank.analyze(log)
    assert check(rows)['passed'] is True
    rejected=[]
    for name in ['both_wrong_return','ABI_saved_false','ABI_stack_false','gate_false','globals_corrupted','guard_corrupted','FP_corrupted','duplicate_seed','terminal_false']:
        bad=copy.deepcopy(rows)
        for side in ('original','candidate'):
            leg=bad[2][side]
            if name=='both_wrong_return':leg['returned']=0
            elif name=='ABI_saved_false':leg['callee_saved']=False
            elif name=='ABI_stack_false':leg['stack_balanced']=False
            elif name=='gate_false':leg['switch_exact']=False
            elif name=='globals_corrupted':leg['pool_globals'][67]=1
            elif name=='guard_corrupted':leg['guard'][63]=0
            elif name=='FP_corrupted':leg['fp']['mxcsr']=0
        if name=='duplicate_seed':bad[3]['case']=0
        if name=='terminal_false':bad[-1]['state_restored']=False
        try:check(bad)
        except AssertionError:rejected.append(name)
        else:raise AssertionError(name)
    result=dict(passed=True,synthetic_pairs=256,negative_tests_rejected=rejected,game_or_build_executed=False)
(HERE/'Diagnostics/model-tests.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
