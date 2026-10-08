"""Register six approved exact runtime identities; no current dependency credit."""
import csv,hashlib,json
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN=HERE/'runs/20261008-122535-223756'
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    report=json.loads((RUN/'enriched-summary.json').read_text())
    assert report['passed'] and report['all_equal'] and report['state_restored'] and report['harness_restored'] and report['natural_exit'] and report['exit_code']==0
    assert digest(RUN/'comparisons.jsonl')==report['log_sha256']
    assert digest(RUN/'harness.jsonl')==report['harness_sha256']
    terminal=json.loads((RUN/'harness.jsonl').read_text().splitlines()[-1])
    assert terminal['state_restored'] and terminal['entries_restored'] and terminal['flags_restored'] and not terminal['changed_ranges']
    sources=report['manifest']['tested_sources']
    assert all(digest(ROOT/p)==h for p,h in sources.items())
    assert report['pairs']==1536 and len(report['pairs_per_function'])==6 and set(report['pairs_per_function'].values())=={256}
    summary=dict(report,approved_scope='Six constructor/destructor identities, 256 cases each; exact full payload, native terminal CTask vptr and collaborator trace comparison. Pregraphics only. Reference and animation collaborators stubbed, no game startup, pools or general task integration accredited.',evidence_run='reversing/RequestedPreflightTaskCtorDtor/runs/20261008-122535-223756',enriched_summary_sha256=digest(RUN/'enriched-summary.json'))
    target=HERE/'validation-summary.json';target.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    registry=ROOT/'reversing/validation-registry.json';before=registry.read_bytes();data=json.loads(before)
    rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')));key=('class','fn_name','address','type')
    known={tuple(r['hook'][k] for k in key) for r in data['functions']};added=0
    for identity,count in report['pairs_per_function'].items():
        cls,fn=identity.split('::');matches=[r for r in rows if r['class']==cls and r['fn_name']==fn]
        assert len(matches)==1;row=matches[0];identitykey=tuple(row[k] for k in key)
        if identitykey in known:continue
        data['functions'].append({'hook':{k:row[k] for k in key},'evidence':{'path':target.relative_to(ROOT).as_posix(),'sha256':digest(target),'passed_path':['passed'],'pairs':count,'original_exe_sha256':report['original_exe_sha256']},'tested_sources':sources,'source_provenance':'Compiled manifest from ASI '+report['executed_probe_sha256']+' retained in approved summary; exact raw comparison and harness hashes verified. Base Task.h and Task.cpp are compiled source evidence; no independent base-destructor validation inferred.','dependency_review':None,'scope':summary['approved_scope'],'review_reference':'reversing/RequestedPreflightTaskCtorDtor/TaskBaseReview/CTASK_EXPLICIT_DESTRUCTOR_ASM.md'})
        known.add(identitykey);added+=1
    assert registry.read_bytes()==before,'Concurrent registry mutation'
    temporary=registry.with_name('validation-registry.task-ctor-dtor.tmp')
    temporary.write_text(json.dumps(data,indent=2,ensure_ascii=False)+'\n',encoding='utf-8')
    temporary.replace(registry)
    print('Added historical identities:',added)
if __name__=='__main__':main()
