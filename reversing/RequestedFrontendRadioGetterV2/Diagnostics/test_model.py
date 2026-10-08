"""Synthetic strict analyzer coverage, never uses the installed game."""
import copy,json,sys,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];sys.path.insert(0,str(HERE))
import run_local as bank
fixture=dict(event='fixture',arena=[0]*524,globals=[0]*18,fp=dict(rounding=0,exceptions=0,mxcsr=8064))
rows=[dict(event='start',cases=2048,function='CAEFrontendAudioEntity::IsRadioTuneSoundActive',address=0x4DD480),fixture]
for seed in range(2048):
    # Independent explicit truth table: index [user,code,normal,paused].
    b=seed%16;expected=[0,0,0,0,1,0,0,0,0,1,1,1,1,1,1,1][b]
    arena=fixture['arena'].copy();globals_=fixture['globals'].copy()
    arena[:16]=[(31*seed+j)%256 for j in range(16)];arena[-16:]=[(47*seed+j)%256 for j in range(16)]
    arena[16+0x124]=int(bool(b&4));arena[16+0x1CC]=int(bool(b&8));globals_[8]=int(bool(b&2));globals_[9]=int(bool(b&1))
    leg=dict(raw_al=expected,expected_al=expected,switch_exact=True,arena_unchanged=True,globals_unchanged=True,arena=arena,globals=globals_,fp=fixture['fp'])
    rows.append(dict(event='comparison',function=rows[0]['function'],pair=seed,case=seed,equal=True,accepted=True,original=copy.deepcopy(leg),candidate=copy.deepcopy(leg)))
rows.append(dict(event='complete',pairs=2048,failures=0,floating_environment_restored=True,state_restored=True,passed=True))
with tempfile.TemporaryDirectory() as tmp:
    root=Path(tmp);bank.runner.GAME=root;log=root/'bank.jsonl'
    h=[dict(event=v) for v in ['preflight_entered','before_task_bank','task_bank_returned','complete']]
    h[-1].update(snapshot_ranges=17692,changed_ranges=[],state_restored=True,entries_restored=True,flags_restored=True,floating_environment_restored=True,timer_unchanged=True)
    harness=root/'preflight-frontend-radio-v2-harness.jsonl';harness.write_text('\n'.join(map(json.dumps,h))+chr(10))
    def check(values):
        log.write_text('\n'.join(map(json.dumps,values))+chr(10));return bank.analyze(log)
    assert check(rows)['passed'] is True
    rejected=[]
    for name in ['bad_al','both_wrong_al','padding_byte','gate_false','fp_corrupt','missing_pair','duplicate_case','nonboolean_active','terminal_restore_false']:
        bad=copy.deepcopy(rows)
        if name=='bad_al':bad[2]['candidate']['raw_al']=1
        elif name=='both_wrong_al':bad[2]['original']['raw_al']=bad[2]['candidate']['raw_al']=1
        elif name=='padding_byte':bad[2]['original']['arena'][25]=bad[2]['candidate']['arena'][25]=1
        elif name=='gate_false':bad[2]['original']['switch_exact']=bad[2]['candidate']['switch_exact']=False
        elif name=='fp_corrupt':bad[2]['candidate']['fp']['mxcsr']=0
        elif name=='missing_pair':bad.pop(2)
        elif name=='duplicate_case':bad[3]['case']=0
        elif name=='nonboolean_active':bad[2]['original']['arena'][16+0x124]=bad[2]['candidate']['arena'][16+0x124]=128
        else:bad[-1]['state_restored']=False
        try:check(bad)
        except (AssertionError,RuntimeError):rejected.append(name)
        else:raise AssertionError('False acceptance '+name)
    result=dict(passed=True,synthetic_pairs=2048,negative_tests_rejected=rejected,game_or_build_executed=False)
(HERE/'Diagnostics/model-tests.json').write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result))
