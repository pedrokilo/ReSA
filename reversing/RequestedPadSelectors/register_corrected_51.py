"""Register only reviewed 51 identities; GetLookRight has a known uncovered bug."""
import collections,csv,hashlib,json
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];RUN=HERE/'runs/20261008-130252-828987'
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def main():
    report=json.loads((RUN/'enriched-summary.json').read_text());manifest=report['manifest']
    assert report['passed'] and report['all_equal'] and report['differences']==0 and report['comparisons']==201552
    assert report['natural_exit'] and report['exit_code']==0 and report['harness_restored'] and report['bank_restored'] and report['saves_unchanged'] and report['configs_restored']
    assert all(digest(ROOT/p)==h for p,h in manifest['tested_sources'].items())
    scope=json.loads((HERE/'scope.json').read_text());expected={r['fn_name']:int(r['address'],16) for r in scope['entries'] if r['phase']=='pure'}
    seen=set();counts=collections.Counter()
    for line in (RUN/'comparisons.jsonl').open(encoding='utf-8'):
        row=json.loads(line)
        if row.get('event')!='comparison':continue
        key=(row['function'],row['seed']);assert key not in seen and expected[row['function']]==row['address'] and row['equal'] and row['switch_exact'] and row['pad_equal'] and row['globals_equal']
        assert row['original_bits']==row['candidate_bits'];seen.add(key);counts[row['function']]+=1
    assert seen=={(fn,seed) for fn in expected for seed in range(3876)}
    harness=json.loads((RUN/'harness.jsonl').read_text().splitlines()[-1]);assert harness['state_restored'] and harness['entries_restored'] and harness['flags_restored'] and not harness['changed_ranges']
    assert digest(RUN/'harness.jsonl')==report['harness_sha256']
    approved={fn:count for fn,count in counts.items() if fn!='GetLookRight'};assert len(approved)==51
    summary={'passed':True,'all_equal':True,'differences':0,'functions':51,'pairs':sum(approved.values()),'pairs_per_function':approved,'excluded_known_bug':'GetLookRight: original LeftShoulder2 release versus source RightShoulder2 release; corpus does not cover combined held/released shoulder interaction. No approval from this run.','natural_exit':True,'exit_code':0,'bank_restored':True,'harness_restored':True,'harness_terminal':harness,'raw_run':RUN.relative_to(ROOT).as_posix(),'enriched_summary_sha256':digest(RUN/'enriched-summary.json'),'comparison_log_sha256':digest(RUN/'comparisons.jsonl'),'harness_sha256':digest(RUN/'harness.jsonl'),'manifest':manifest,'scope':'Bounded historical CPad selector approval only: 3876 private-pad fixtures per approved identity; modes0..3, button pressure0..255, single-field transitions, disable-mask/flags, analogue-history boundaries and short ABI stress. Full private payload and touched global Pads/history/clock bytes compared and restored; all hooks/vtables restored before natural exit. No general input/controller integration, combined-button exhaustive coverage, game startup or current dependency review.'}
    target=HERE/'validation-summary-51.json';target.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    path=ROOT/'reversing/validation-registry.json';before=path.read_bytes();registry=json.loads(before);rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')));key=('class','fn_name','address','type');known={tuple(r['hook'][k] for k in key) for r in registry['functions']};added=0
    for fn,pairs in approved.items():
        matches=[r for r in rows if r['class']=='CPad' and r['fn_name']==fn and int(r['address'],16)==expected[fn]];assert len(matches)==1;row=matches[0];identity=tuple(row[k] for k in key)
        if identity in known:continue
        registry['functions'].append({'hook':{k:row[k] for k in key},'evidence':{'path':target.relative_to(ROOT).as_posix(),'sha256':digest(target),'passed_path':['passed'],'pairs':pairs,'original_exe_sha256':report['original_exe_sha256']},'tested_sources':manifest['tested_sources'],'source_provenance':'Corrected six-defect CPad probe '+report['executed_probe_sha256']+'; exact compiled manifest retained. Runtime raw rows, unique identities, switch bytes, all private/global equality flags and final restoration independently audited; GetLookRight explicitly excluded.','dependency_review':None,'scope':summary['scope'],'review_reference':'reversing/RequestedPadSelectors/FIRST_REPLAY_DIAGNOSIS.json'});known.add(identity);added+=1
    assert path.read_bytes()==before,'Concurrent registry mutation'
    temporary=path.with_name('validation-registry.pad51.tmp');temporary.write_text(json.dumps(registry,indent=2,ensure_ascii=False)+'\n',encoding='utf-8');temporary.replace(path)
    print('Added',added,'historical CPad entries; registry',len(registry['functions']))
if __name__=='__main__':main()
