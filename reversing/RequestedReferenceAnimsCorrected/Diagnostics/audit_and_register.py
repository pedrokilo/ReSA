"""Read-only720 audit by default; --register explicitly records this sole identity."""
import argparse,collections,hashlib,itertools,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def load(p):return json.loads(p.read_text(encoding='utf8'))
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--run',required=True);parser.add_argument('--register',action='store_true');args=parser.parse_args();run=ROOT/args.run;parent=run/'enriched-summary.json';s=load(parent)
    assert s['passed'] is True and s['all_equal'] is True and s['natural_exit'] is True and s['exit_code']==0
    assert not s.get('restore_error') and not s.get('error') and s['saves_unchanged'] is True and s['configs_restored'] is True
    assert s['pairs']==720 and s['differences']==s['model_mismatches']==0 and s['validation_errors']==[]
    assert s['state_restored'] is True and s['harness_restored'] is True
    assert s['manifest']['tested_sources']['source/game_sa/Interior/InteriorGroup_c.cpp']=='6c55e17625a1f0b826b9b3e07b9a40c63c655134f90f4c92a19db113375d5078'
    for group in ['tested_sources','bank_sources']:
        for name,h in s['manifest'][group].items():assert digest(ROOT/name)==h,name
    assert s['executed_probe_sha256']==s['manifest']['asi']
    assert digest(run/'comparisons.jsonl')==s['log_sha256'] and digest(run/'harness.jsonl')==s['harness_sha256']
    seen=set();pairs=set();tracecounts=collections.Counter();terminal=None
    for line in (run/'comparisons.jsonl').open(encoding='utf8'):
        x=json.loads(line)
        if x['event']!='comparison':assert x['event']=='complete' and terminal is None;terminal=x;continue
        assert terminal is None and x['function']=='InteriorGroup_c::ReferenceAnims';key=tuple(x[k] for k in ['group','referenced','index','loaded','poison']);assert key not in seen and x['pair'] not in pairs;seen.add(key);pairs.add(x['pair'])
        group,referenced,index,loaded,poison=key;model=[poison]*188;model[14]=group;model[186]=referenced&255;trace=[]
        if referenced==0:
            trace.append({'event':'anim_index','name':['int_house','int_shop','int_office'][group],'valid':True,'index':index})
            if loaded==1:model[186]=1;trace.append({'event':'anim_ref','index':index,'valid':True})
            else:trace.append({'event':'request_model','model':index+25575,'flags':8,'valid':True})
        assert x['equal'] is True and x['original']==x['candidate']
        for side in ['original','candidate']:assert x[side]['gate'] is True and x[side]['collaborators_valid'] is True
        assert x['original']['bytes']==x['model_bytes']==model and x['original']['trace']==x['model_trace']==trace
        tracecounts.update(t['event'] for t in trace)
    assert seen==set(itertools.product(range(3),[0,1,-1],[0,1,127,179],range(5),[0,85,170,255])) and pairs==set(range(720))
    assert dict(tracecounts)=={'anim_index':240,'request_model':192,'anim_ref':48}
    assert terminal['pairs']==720 and terminal['differences']==0 and terminal['state_restored'] is True
    h=[json.loads(line) for line in (run/'harness.jsonl').read_text(encoding='utf8').splitlines()];assert [x['event'] for x in h]==['preflight_entered','before_task_bank','task_bank_returned','complete'];last=h[-1];assert last['snapshot_ranges']==17692 and last['changed_ranges']==[] and all(last[k] is True for k in ['state_restored','entries_restored','tables_restored','flags_restored','timer_restored'])
    result={'audit_passed':True,'pairs':720,'trace_counts':dict(tracecounts),'registered':False}
    if args.register:
        summary=dict(s);summary.update(historical_only=True,independent_audit=result.copy(),audit_script_path=Path(__file__).relative_to(ROOT).as_posix(),audit_script_sha256=digest(Path(__file__)),parent_summary_path=parent.relative_to(ROOT).as_posix(),parent_summary_sha256=digest(parent),scope='Historical720 finite valid-group private reference fixtures:3types xreferenced0/1/-1 x4indices0/1/127/179 xrawloadedbyte0..4 x4poisons. Independent188byte arena and exact lookup240/ref48/request192 traces match original/candidate, strict target/helper entry gates. Reads animationblock rawbyte exactly1 via public contiguous underlying180array pointer even takeviewcount0; modelstreamingglobal is not substituted. Lookup/ref/request are isolated collaborators, no real assets/refcount/helper or invalidenum/indices/currentdependency accreditation. Naturalexit0/all17692entry-vtable/hooks/flags/timer/FP/blockloadbytes/configs restored; previousmixed48fail and initialtimeout retained.')
        output=ROOT/'reversing/RequestedReferenceAnimsCorrected/validation-summary-reference720.json';output.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf8');registry=ROOT/'reversing/validation-registry.json';r=load(registry);assert len(r['functions'])==6840;hook={'class':'InteriorGroup_c','fn_name':'ReferenceAnims','address':'0x005950D0','type':'S'};assert all(x['hook']!=hook for x in r['functions'])
        r['functions'].append({'hook':hook,'evidence':{'path':output.relative_to(ROOT).as_posix(),'sha256':digest(output),'passed_path':['passed'],'pairs':720,'original_exe_sha256':s['original_exe_sha256']},'tested_sources':s['manifest']['tested_sources'],'source_provenance':'Exact compiled source6c55e176/testedmanifest and8frozeninstrument/evidence hashes audited; independentfullbyte/trace reconstruction. OldmixedASI43c7 and48Referencefail preserved; private-arrayaccess failedsource9c4/buildlog archived; publicdata getter fix does not normalize raw2..4 bytes. ASI'+s['executed_probe_sha256'],'dependency_review':None,'scope':summary['scope'],'review_reference':'reversing/RequestedAnimProjectilePair/Diagnostics/reference-loadstate-diagnostic.json'})
        temporary=registry.with_suffix('.tmp');temporary.write_text(json.dumps(r,indent=2)+'\n',encoding='utf8');temporary.replace(registry);result.update(registered=True,registry_count=len(r['functions']),summary_sha256=digest(output))
    print(json.dumps(result))
if __name__=='__main__':main()
