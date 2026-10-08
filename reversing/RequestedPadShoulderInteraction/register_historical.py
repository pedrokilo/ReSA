"""Approve only the reproduced-and-corrected GetLookRight identity."""
import csv,hashlib,json
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];RUN=HERE/'runs/20261008-131626-078538'
def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def main():
    report=json.loads((RUN/'enriched-summary.json').read_text());assert report['passed'] and report['all_equal'] and report['differences']==0 and report['natural_exit'] and report['exit_code']==0
    assert report['bank_restored'] and report['harness_restored'] and report['saves_unchanged'] and report['configs_restored']
    seen=set()
    for line in (RUN/'comparisons.jsonl').open(encoding='utf-8'):
        row=json.loads(line)
        if row.get('event')!='comparison':continue
        assert row['function']=='GetLookRight' and row['address']==0x53FE10 and row['switch_exact'] and row['globals_unchanged'] and row['equal'] and row['original']==row['candidate']
        assert len(row['original']['pad'])==0x134 and type(row['original']['return_al_bool']) is bool and row['seed'] not in seen;seen.add(row['seed'])
    assert seen==set(range(512))
    terminal=json.loads((RUN/'harness.jsonl').read_text().splitlines()[-1]);assert terminal['state_restored'] and terminal['entries_restored'] and terminal['flags_restored'] and not terminal['changed_ranges']
    assert digest(RUN/'harness.jsonl')==report['harness_sha256']
    sources=report['manifest']['tested_sources'];assert all(digest(ROOT/p)==h for p,h in sources.items())
    summary=dict(report,comparison_log_sha256=digest(RUN/'comparisons.jsonl'),enriched_summary_sha256=digest(RUN/'enriched-summary.json'),harness_terminal=terminal,scope='CPad::GetLookRight only:512 private-pad Cartesian shoulder old/new truth fixtures, four modes, disabled/enabled controls, pressures0/1/128/255. Bool AL and full private payload equal, globals unchanged, all hook/table/flag bytes restored and natural exit0. Prior12 interaction failures preserved. No general input integration, exhaustive selector behavior, or current dependency review.')
    target=HERE/'validation-summary.json';target.write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    path=ROOT/'reversing/validation-registry.json';before=path.read_bytes();data=json.loads(before);rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')));matches=[r for r in rows if r['class']=='CPad' and r['fn_name']=='GetLookRight' and int(r['address'],16)==0x53FE10];assert len(matches)==1
    row=matches[0];key=('class','fn_name','address','type');identity={k:row[k] for k in key};known=any(r['hook']==identity for r in data['functions'])
    if not known:data['functions'].append({'hook':identity,'evidence':{'path':target.relative_to(ROOT).as_posix(),'sha256':digest(target),'passed_path':['passed'],'pairs':512,'original_exe_sha256':report['original_exe_sha256']},'tested_sources':sources,'source_provenance':'Seven-fix CPad source compiled in ASI '+report['executed_probe_sha256']+'; exact manifest and complete raw comparison/harness hashes retained. Prior ce8a92a baseline12 failures and exact six-fix source archived.','dependency_review':None,'scope':summary['scope']})
    assert path.read_bytes()==before
    temporary=path.with_name('validation-registry.pad-shoulder.tmp');temporary.write_text(json.dumps(data,indent=2,ensure_ascii=False)+'\n',encoding='utf-8');temporary.replace(path)
    print('Added',int(not known),'GetLookRight historical identity; registry',len(data['functions']))
if __name__=='__main__':main()
