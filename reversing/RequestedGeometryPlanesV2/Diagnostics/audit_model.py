"""Offline model for frozen GeometryPlanesV2 baseline; does not run the game."""
from pathlib import Path
import json,struct,hashlib,collections
HERE=Path(__file__).resolve().parent
BANK=HERE.parent
RUN=BANK/'runs/20261008-140227-969549'
ROOT=BANK.parents[1]
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def floats(bs):return list(struct.unpack('<'+str(len(bs)//4)+'f',bytes(bs)))
def bits(value):return struct.pack('<f',value)
def f32(value):return struct.unpack('<f',bits(value))[0]
def main():
    report=json.loads((RUN/'enriched-summary.json').read_text())
    assert digest(RUN/'comparisons.jsonl')==json.loads((BANK/'evidence/replay-dot-audit.json').read_text())['log_sha256']
    rows=[json.loads(x) for x in (RUN/'comparisons.jsonl').read_text().splitlines()]
    counts=collections.Counter();classification=[]
    pairs=[r for r in rows if r['event']=='comparison']
    assert len(pairs)==2048 and {r['pair'] for r in pairs}==set(range(2048))
    assert {(r['seed'],r['alias']) for r in pairs}=={(seed,alias) for seed in range(1024) for alias in range(2)}
    for r in pairs:
        a,b=r['original'],r['candidate'];corners=floats(r['input_corners']);normals=floats(a['normals']);prev=corners[9:12];x87=[];sse=[];mismatch_slots=[]
        for slot in range(4):
            plane=normals[3*slot:3*slot+3]
            # Runtime canonical PC53: each product/sum binary64, sole final store binary32.
            extended=-(prev[2]*plane[2]+prev[1]*plane[1]+prev[0]*plane[0])
            intermediate=-f32(f32(f32(prev[2]*plane[2])+f32(prev[1]*plane[1]))+f32(prev[0]*plane[0]))
            x87.extend(bits(extended));sse.extend(bits(intermediate))
            if a['dots'][4*slot:4*slot+4]!=b['dots'][4*slot:4*slot+4]:mismatch_slots.append(slot)
            # Original snapshots the current corner after its output stores. Whole-array alias
            # therefore uses current written plane as the next previous corner.
            prev=plane if r['alias'] else corners[3*slot:3*slot+3]
        counts['pairs']+=1;counts['failed_pairs']+=not r['equal'];counts['failed_separate']+=not r['equal'] and not r['alias'];counts['failed_whole_alias']+=not r['equal'] and bool(r['alias']);counts['normal_arrays_equal']+=a['normals']==b['normals'];counts['x87_matches_original']+=list(x87)==a['dots'];counts['sse_matches_candidate']+=list(sse)==b['dots'];counts['different_dot_slots']+=len(mismatch_slots)
        for slot in mismatch_slots:counts['dot_slot_'+str(slot)]+=1
        assert a['normals']==b['normals'] and list(x87)==a['dots'] and list(sse)==b['dots']
        assert r['original_adapter_calls']==0 and r['original_adapter_trace']==[] and r['candidate_adapter_calls']==4 and [t['index'] for t in r['candidate_adapter_trace']]==list(range(4))
        assert all(a[k] and b[k] for k in ['gate','adapter_contract','control_unchanged','mxcsr_control_unchanged','outside_output_unchanged','plane_z_pluszero'])
        classification.append({'pair':r['pair'],'seed':r['seed'],'alias':r['alias'],'equal':r['equal'],'different_dot_slots':mismatch_slots,'original_dots_hex':bytes(a['dots']).hex(),'candidate_dots_hex':bytes(b['dots']).hex(),'x87_model_hex':bytes(x87).hex(),'sse_model_hex':bytes(sse).hex(),'normals_equal':True,'x87_match':True,'sse_match':True})
    terminal=rows[-1];assert all(terminal[k] for k in ['state_restored','entries_restored','manual_helper_entries_restored','native_readonly_unchanged','globals_restored','fp_environment_restored'])
    metadata=json.loads((HERE/'proposal-metadata.json').read_text())
    source=ROOT/'source/game_sa/PedGeometryAnalyser.cpp'
    assert digest(source)==metadata['source_after_sha256'],'Functional source changed after the reviewed fix'
    result={'model_script_sha256':digest(Path(__file__)),'log_sha256':digest(RUN/'comparisons.jsonl'),'enriched_sha256':digest(RUN/'enriched-summary.json'),'baseline_compiled_source_sha256':report['manifest']['tested_sources']['source/game_sa/PedGeometryAnalyser.cpp'],'post_fix_source_sha256':digest(source),'baseline_asi_sha256':report['executed_probe_sha256'],'counts':dict(counts),'terminal_restoration':terminal,'coverage':'2048unique IDs and1024seeds x2aliasmodes verified','natural_exit_status':'Not serialized for this failed replay: generic analyzer raised before exit check. No exit0 claim. External restoration saves/configs true.','scope':'PC53 nearest fixture; finite stored input and native normalization outputs. Model does not independently validate normalization/helper integration. No alias traversal discrepancy within separate/exact wholearray scope.','classifications':classification}
    (HERE/'audit-model-results.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(result['counts']))
if __name__=='__main__':main()
