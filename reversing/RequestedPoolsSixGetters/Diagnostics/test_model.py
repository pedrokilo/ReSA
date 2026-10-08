"""Full synthetic model product and adversarial acceptance checks; no game."""
import copy,json,sys,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];sys.path.insert(0,str(HERE));import run_local as bank
rows=[dict(event='start',cases=3072,functions=6,domain='two in-bounds slots')];pair=0
for family,stride in enumerate([1988,2584,412]):
    addr=0x10000000+family*0x10000;state=addr+0x8000;pool=addr+0x9000
    storage=b'\xa5'*16+b'\xfd'*(stride*2)+b'\xa5'*16;states=b'\xd3'*16+b'\x80\x80'+b'\xd3'*16
    header=b'\xb6'*16+addr.to_bytes(4,'little')+state.to_bytes(4,'little')+(2).to_bytes(4,'little')+b'\xff'*4+b'\0\0\xb6\xb6'+b'\xb6'*16
    globals_=bytearray(68);offset=[12,16,24][family];globals_[offset:offset+4]=pool.to_bytes(4,'little');fp=dict(rounding=0,exceptions=0,mxcsr=8064)
    rows.append(dict(event='fixture',family=family,stride=stride,capacity=2,storage_address=addr,state_address=state,pool_address=pool,storage=storage.hex(),states=states.hex(),header=header.hex(),globals=globals_.hex(),fp=fp))
    for fn,total in enumerate([256,768]):
        for seed in range(total):
            index=seed%2;id_=(seed//2)%128;variant=seed//256 if fn else 0;tag=[id_,id_^1,id_^128][variant];pointer=addr+index*stride;argument=(index*256+tag)if fn else pointer;result=(pointer if tag==id_ else 0)if fn else index*256+id_
            body=storage[:16]+bytes((seed*47+i*29)%256 for i in range(2*stride))+storage[-16:];states_=bytearray(states);states_[16]=(id_+17)%128;states_[17]=(id_+31)%128;states_[16+index]=id_
            leg=dict(returned=result,switch_exact=True,storage_unchanged=True,states_unchanged=True,header_unchanged=True,globals_unchanged=True,storage=body.hex(),states=states_.hex(),header=header.hex(),globals=globals_.hex(),fp=fp)
            rows.append(dict(event='comparison',function='CPools::'+bank.NAMES[family][fn],family=family,fn=fn,pair=pair,case=seed,index=index,id=id_,variant=variant,argument=argument,expected=result,equal=True,accepted=True,original=copy.deepcopy(leg),candidate=copy.deepcopy(leg)));pair+=1
    rows.append(dict(event='family_restore',family=family,state_restored=True))
rows.append(dict(event='complete',pairs=3072,failures=0,passed=True,state_restored=True))
with tempfile.TemporaryDirectory() as temp:
    root=Path(temp);bank.runner.GAME=root;log=root/'synthetic.jsonl';h=[dict(event=e)for e in ['preflight_entered','before_task_bank','task_bank_returned','complete']];h[-1].update(snapshot_ranges=17692,changed_ranges=[],state_restored=True,entries_restored=True,flags_restored=True,floating_environment_restored=True,timer_unchanged=True)
    (root/'preflight-pools-six-getters-harness.jsonl').write_text('\n'.join(map(json.dumps,h))+'\n')
    def check(data):log.write_text('\n'.join(map(json.dumps,data))+'\n');return bank.analyze(log)
    assert check(rows)['pairs']==3072
    rejected=[]
    for name in ['wrong_both_return','raw_pointer_identity','padding_byte','ownership_flag','wrong_stale_id','false_gate','false_FP','case_duplicate','restore_false','uppercase_hex']:
        bad=copy.deepcopy(rows)
        if name=='ownership_flag':bad[1]['header']=bad[1]['header'][:64]+'01'+bad[1]['header'][66:]
        elif name=='case_duplicate':bad[3]['case']=0
        elif name=='restore_false':bad[-1]['state_restored']=False
        elif name=='wrong_stale_id':bad[2]['argument']+=1
        else:
            for side in ('original','candidate'):
                leg=bad[2][side]
                if name in ('wrong_both_return','raw_pointer_identity'):leg['returned']+=1
                elif name=='padding_byte':leg['header']='00'+leg['header'][2:]
                elif name=='false_gate':leg['switch_exact']=False
                elif name=='false_FP':leg['fp']['mxcsr']=0
                elif name=='uppercase_hex':leg['storage']=leg['storage'].upper()
        try:check(bad)
        except (AssertionError,IndexError):rejected.append(name)
        else:raise AssertionError(name)
result=dict(passed=True,synthetic_pairs=3072,per_family_reference_pairs=256,per_family_handle_pairs=768,negative_tests_rejected=rejected,game_or_build_executed=False)
(HERE/'Diagnostics/model-tests.json').write_text(json.dumps(result,indent=2));print(json.dumps(result))
