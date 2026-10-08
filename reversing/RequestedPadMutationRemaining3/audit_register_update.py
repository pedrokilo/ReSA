"""Reaudit corrected Update from preserved payload, then register one scoped identity."""
import csv,hashlib,importlib.util,json,struct
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];RUN=HERE/'runs/update/20261008-151627-333706'
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 report=json.loads((RUN/'enriched-summary.json').read_text());manifest=report['manifest'];layout=json.loads((RUN/'layout-proof.json').read_text())
 assert all(report[k] is True for k in ('passed','all_equal','natural_exit','saves_unchanged','configs_restored','layout_file_restored')) and report['exit_code']==0 and report['differences']==0
 assert report['executed_probe_sha256']==manifest['asi']==sha(HERE/'artifacts/update/gta_reversed-preflight-pad-remaining3-update.asi')
 assert sha(RUN/'layout-proof.json')==manifest['layout_proof_sha256']
 assert all(sha(ROOT/p)==h for p,h in manifest['tested_sources'].items()) and all(sha(ROOT/p)==h for p,h in manifest['bank_sources'].items())
 spec=importlib.util.spec_from_file_location('strict_remaining_update',HERE/'run_local.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module);module.BANK='update';module.MANIFEST=manifest;module.LAYOUT=layout;module.runner.GAME=RUN
 alias=RUN/'preflight-pad-remaining3-update-harness.jsonl';assert not alias.exists();alias.write_bytes((RUN/'harness.jsonl').read_bytes())
 try:checked=module.analyze(RUN/'comparisons.jsonl')
 finally:alias.unlink()
 assert checked['passed'] and checked['comparisons']==2560
 count=0
 for line in (RUN/'comparisons.jsonl').open():
  row=json.loads(line)
  if row.get('event')!='comparison':continue
  seed=row['seed'];initial=[i*101+seed for i in range(10)];expected=[initial[0]]+initial[:9]
  for side in ('original','candidate'):assert list(struct.unpack('<10h',bytes(row[side]['pad'][16+0x60:16+0x74])))==expected
  count+=1
 assert count==2560
 scope='2560 private Update cases: four modes, control disable toggle, five horn positions, four frontend byte counters and sixteen input patterns. Full pad/controllers/globals/physical scratch exact; independent canonical-EXE descending-history oracle verifies both legs. Native Reconcile/Clear/CheckForInput helpers remain uncredited. Pregraphics only; no OS input/game startup/general dependency accreditation.'
 target=HERE/'validation-summary-update.json';target.write_text(json.dumps(dict(report,strict_preserved_reaudit=checked,independent_native_history_oracle_cases=count,approved_scope=scope,raw_log_sha256=sha(RUN/'comparisons.jsonl'),native_evidence_sha256=sha(HERE/'evidence/Update.exe.json')),indent=2)+'\n')
 keys=('class','fn_name','address','type');rows=[r for r in csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')) if r['class']=='CPad' and r['fn_name']=='Update' and int(r['address'],16)==0x541C40 and r['type']=='S'];assert len(rows)==1
 proposal={'hook':{k:rows[0][k] for k in keys},'evidence':{'path':target.relative_to(ROOT).as_posix(),'sha256':sha(target),'passed_path':['passed'],'pairs':count,'original_exe_sha256':report['original_exe_sha256']},'tested_sources':manifest['tested_sources'],'source_provenance':'Exact compiled manifest and independently reaudited preserved full payload, current mapped scratch/body proof; canonical EXE descending-history oracle verifies both legs.','dependency_review':None,'scope':scope,'review_reference':'reversing/RequestedPadMutationRemaining3/DESIGN.md'}
 p=ROOT/'reversing/validation-registry.json';before=p.read_bytes();data=json.loads(before);n=len(data['functions']);assert n==6842 and not any(x['hook']==proposal['hook'] for x in data['functions']);data['functions'].append(proposal);assert p.read_bytes()==before;t=p.with_name('validation-registry.update.tmp');t.write_text(json.dumps(data,indent=2,ensure_ascii=False)+'\n',encoding='utf8');t.replace(p);assert len(json.loads(p.read_bytes())['functions'])==n+1;print(json.dumps({'registered':1,'cases':count,'prior':n,'current':n+1}))
if __name__=='__main__':main()
