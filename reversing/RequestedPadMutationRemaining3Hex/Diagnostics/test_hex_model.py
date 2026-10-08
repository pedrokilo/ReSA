"""Synthetic scheduling/oracle checks, no native execution or game directory writes."""
import copy,hashlib,importlib.util,json,tempfile
from pathlib import Path
HERE=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('hex_runner_under_test',HERE/'run_local.py')
base=importlib.util.module_from_spec(spec);spec.loader.exec_module(base)
def leg(bank,seed,candidate):
    meta=base.metadata(bank,seed)
    pad=[0x49]*16+[(seed*73+i*29)&255 for i in range(308)]+[0xC3]*16
    controllers=[[0x31]*16+[(seed*19+i*7+role)&255 for i in range(48)]+[0xE7]*16 for role in range(3)]
    globals=[[0]*16,[0]*33,[0]*33]
    globals[0][8:12]=meta['clock'].to_bytes(4,'little');globals[1][16]=int(bool(meta['gates']&1));globals[2][16]=int(bool(meta['gates']&2))
    seeds=[[(seed*73+i*29+role*101)&255 for i in range(48)] for role in range(2)]
    scratch=[[0x41]*16+v+[0x42]*16 for v in seeds]
    active=candidate if bank=='reconcile' else 0 if bank=='update' else None
    if active is not None:scratch[int(active)][16:64]=[(seed+i*5)&255 for i in range(48)]
    return dict(pad=pad,controllers=controllers,globals=globals,return_reference_exact=True,fp=dict(rounding=0,exceptions=0,mxcsr=0x1F80),scratch_physical=scratch,scratch_guards_unchanged=True,inactive_scratch_unchanged=True)
def row(bank,seed,original,candidate):
    name,address,total=base.TARGETS[bank]
    result=dict(event='comparison',function=name,address=address,seed=seed,**base.metadata(bank,seed),switch_exact=True,guards_unchanged=True,inactive_scratch_unchanged=True,return_reference_exact=True,globals_unchanged=True,helper_entries_original=True,original=original,candidate=candidate,scratch_seed_bytes=[[(seed*73+i*29+role*101)&255 for i in range(48)] for role in range(2)])
    result['equal']=base.payload_contract(original,bank,False)==base.payload_contract(candidate,bank,True)
    return result
def encode(row):return (json.dumps(row,sort_keys=True,separators=(',',':'))+'\n').encode()
def scheduled(bank,total):
    for start in range(0,total,1024):
        end=min(total,start+1024);originals=[(seed,leg(bank,seed,False)) for seed in range(start,end)]
        assert len(originals)==end-start
        for seed in range(start,end):
            original_seed,original=originals[seed-start]
            assert original_seed==seed
            yield row(bank,seed,original,leg(bank,seed,True))
from hex_wire import decode_row,encode_comparison_for_test
checks=[]
for bank in base.TARGETS:
    total=base.TARGETS[bank][2];legacy=hashlib.sha256();phased=hashlib.sha256();wire_digest=hashlib.sha256();seen=[]
    for seed,value in enumerate(scheduled(bank,total)):
        expected=row(bank,seed,leg(bank,seed,False),leg(bank,seed,True))
        wire=encode_comparison_for_test(value);decoded=decode_row(wire)
        assert encode(decoded)==encode(expected)
        legacy.update(encode(expected));phased.update(encode(decoded));wire_digest.update(encode(wire));seen.append(seed)
    assert seen==list(range(total)) and legacy.digest()==phased.digest()
    checks.append(dict(bank=bank,pairs=total,legacy_sha256=legacy.hexdigest(),phased_sha256=phased.hexdigest(),comparison_bytes_identical=True,wire_sha256=wire_digest.hexdigest()))
# Test actual strict analyzer in an isolated temporary GAME tree using five synthetic cases.
with tempfile.TemporaryDirectory() as temp:
    base.runner.GAME=Path(temp);base.BANK='shake';name,address,_=base.TARGETS['shake'];base.TARGETS['shake']=(name,address,5)
    base.MANIFEST=dict(asi='0'*64,pdb='1'*64)
    base.LAYOUT=dict(functions=[dict(rva=1,length=2,scratch_operands=[],highlow_relocations=[])],scratch=[dict(rva=3)])
    proof=dict(artifact_sha256='0'*64,pdb_sha256='1'*64,function_rva=1,function_bytes=2,scratch_rva=3,scratch_bytes=48,encoded_operands=0,explicit_highlow_relocations=0,mapped_body_exact=True,artifact_hash_exact=True,actual_module_base=0)
    harness=[dict(event='preflight_entered'),dict(event='before_task_bank'),dict(event='task_bank_returned'),dict(event='complete',snapshot_ranges=17692,changed_ranges=[],state_restored=True,entries_restored=True,flags_restored=True)]
    (Path(temp)/'preflight-pad-remaining3-hex-shake-harness.jsonl').write_text(''.join(json.dumps(r)+'\n' for r in harness))
    terminal=dict(event='complete',comparisons=5,state_restored=True,floating_environment_restored=True,switch_exact=True,guards_unchanged=True,inactive_scratch_unchanged=True,return_reference_exact=True,globals_unchanged=True,helper_entries_original=True,phased_chunk_pairs=1024,phased_original_buffer_estimate_bytes=16384,phased_original_buffer_bound=True)
    rows=[dict(event='start',bank='shake',function=name,address=address,cases=5),dict(event='current_artifact_proof',proof=proof)]+list(scheduled('shake',5))+[terminal]
    log=Path(temp)/'synthetic.jsonl'
    def wire_rows(values):
        return [dict(r,wire_encoding='lowercasehex-v1') if r['event']=='start' else encode_comparison_for_test(r) if r['event']=='comparison' else r for r in values]
    def analyze_wire(values):log.write_text(''.join(json.dumps(r)+'\n' for r in values));return base.analyze(log)
    def analyze(values):return analyze_wire(wire_rows(values))
    assert analyze(rows)['passed'] is True
    negatives={}
    for case in ['missing_original','chunk_duplicate','seed_mapping','not_restored','gate_false','native_equal_false']:
        values=copy.deepcopy(rows)
        if case=='missing_original':del values[2]['original']
        if case=='chunk_duplicate':values[3]=copy.deepcopy(values[2])
        if case=='seed_mapping':values[2]['seed']=1
        if case=='not_restored':values[-1]['state_restored']=False
        if case=='gate_false':values[2]['switch_exact']=False
        if case=='native_equal_false':values[2]['candidate']['pad'][16]^=1;values[2]['equal']=False
        try:report=analyze(values)
        except (KeyError,RuntimeError,ValueError):negatives[case]='rejected_exception'
        else:assert report['passed'] is False;negatives[case]='rejected_passed_false'
    for case in ['corrupt_hex','upper_hex','hex_length','hex_whitespace','extra_leg_field','controller_shape','fp_type','fp_extra','wire_seed_bool','wire_gate_false','terminal_extra','terminal_not_restored','original_missing']:
        values=copy.deepcopy(wire_rows(rows));leg=values[2]['original']
        if case=='corrupt_hex':leg['pad']='g'+leg['pad'][1:]
        if case=='upper_hex':leg['pad']='A'+leg['pad'][1:]
        if case=='hex_length':leg['pad']=leg['pad'][:-2]
        if case=='hex_whitespace':leg['pad']=' '+leg['pad'][1:]
        if case=='extra_leg_field':leg['unexpected']=0
        if case=='controller_shape':leg['controllers'].append(leg['controllers'][0])
        if case=='fp_type':leg['fp']['rounding']=False
        if case=='fp_extra':leg['fp']['unexpected']=0
        if case=='wire_seed_bool':values[2]['seed']=False
        if case=='wire_gate_false':values[2]['helper_entries_original']=False
        if case=='terminal_extra':values[-1]['unexpected']=0
        if case=='terminal_not_restored':values[-1]['state_restored']=False
        if case=='original_missing':del values[2]['original']
        try:report=analyze_wire(values)
        except (KeyError,RuntimeError,ValueError):negatives[case]='rejected_exception'
        else:assert report['passed'] is False;negatives[case]='rejected_passed_false'
    (HERE/'Diagnostics/hex-synthetic-tests.json').write_text(json.dumps(dict(full_corpus_checks=checks,negative_cases=negatives,synthetic_only_no_native_execution=True),indent=2))
print('95052 HEXcanonical rows byteexact;19 adversarial negatives rejected')
