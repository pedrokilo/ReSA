"""Audit and register exactly six scoped historical identities; no dependency/helper credit."""
import csv,gzip,hashlib,importlib.util,json,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];RUN=HERE/'runs/20261008-142018-565169'
sys.path.insert(0,str(HERE))
def digest(path):
 with path.open('rb') as f:return hashlib.file_digest(f,'sha256').hexdigest()
def main():
 report=json.loads((RUN/'enriched-summary.json').read_text());manifest=report['manifest']
 assert report['passed'] and report['all_equal'] and report['differences']==0 and report['natural_exit'] and report['exit_code']==0 and report['saves_unchanged'] and report['configs_restored']
 assert report['executed_probe_sha256']==manifest['asi']==digest(HERE/'artifacts/gta_reversed-preflight-pad-mutation-six.asi')
 assert all(digest(ROOT/name)==h for name,h in manifest['tested_sources'].items())
 assert all(digest(ROOT/name)==h for name,h in manifest['bank_sources'].items())
 archive=json.loads((RUN/'log-archive.json').read_text());zipped=RUN/'comparisons.jsonl.gz';assert archive['lossless_verified'] and digest(zipped)==archive['gzip_sha256']
 rawhash=hashlib.sha256();size=0
 with gzip.open(zipped,'rb') as f:
  while block:=f.read(1024*1024):rawhash.update(block);size+=len(block)
 assert rawhash.hexdigest()==archive['raw_sha256'] and size==archive['raw_bytes']
 spec=importlib.util.spec_from_file_location('pad_six_analyzer',HERE/'run_local_six.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
 class PreservedLog:
  def open(self,encoding='utf8'):return gzip.open(zipped,'rt',encoding=encoding)
 module.runner.GAME=RUN
 module.runner.terminal_event=lambda path:True # Complete/error stages remain strictly checked by analyzer, from preserved files.
 # Analyzer expects the named harness; use preserved one without touching game logs.
 alias=RUN/'preflight-pad-mutation-six-harness.jsonl'
 assert not alias.exists() or digest(alias)==digest(RUN/'harness.jsonl')
 if not alias.exists():alias.write_bytes((RUN/'harness.jsonl').read_bytes())
 try:checked=module.analyze(PreservedLog())
 finally:alias.unlink()
 assert checked['passed'] and checked['differences']==0 and checked['comparisons']==5888 and checked['native_write_mask_verified']
 assert digest(RUN/'harness.jsonl')==report['harness_sha256']
 scope='Six CPad mutation identities, 5888 cases: StopShaking2048, SetTouched2048, ClearMouseHistory256, ClearKeyBoardHistory256, Clear1024, Initialise256. Exact full308-byte private pad and2624 seeded global bytes incl guards, keyboard enter preserved, mouse floats/padding preserved and all Clear bool combinations. Original-only output masks independently bound to canonical EXE. Pregraphics only; all entries/vtables/flags/globals/FP restored and natural exit0; no helper, OS input or game startup accreditation.'
 summary=dict(report,approved_scope=scope,raw_log_archive=archive,strict_preserved_reaudit=checked,enriched_summary_sha256=digest(RUN/'enriched-summary.json'))
 target=HERE/'validation-summary-six.json';target.write_text(json.dumps(summary,indent=2)+'\n')
 registry=ROOT/'reversing/validation-registry.json';before=registry.read_bytes();data=json.loads(before);prior_count=len(data['functions']);keys=('class','fn_name','address','type');known={tuple(item['hook'][k] for k in keys) for item in data['functions']}
 rows=list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')));added=[]
 for name,count in checked['cases_by_function'].items():
  matches=[r for r in rows if r['class']=='CPad' and r['fn_name']==name and int(r['address'],16)==module.TARGETS[name][0] and r['type']=='S'];assert len(matches)==1;row=matches[0];key=tuple(row[k] for k in keys)
  if key in known:continue
  data['functions'].append({'hook':{k:row[k] for k in keys},'evidence':{'path':target.relative_to(ROOT).as_posix(),'sha256':digest(target),'passed_path':['passed'],'pairs':count,'original_exe_sha256':report['original_exe_sha256']},'tested_sources':manifest['tested_sources'],'source_provenance':'Exact compiled manifest bound to ASI '+manifest['asi']+'; preserved lossless raw gzip independently reaudited, full private/global payload and native write masks checked. Native helpers are scope collaborators, no separate validation inferred.','dependency_review':None,'scope':scope,'review_reference':'reversing/RequestedPadMutationNext/FIXTURES-SIX.md'})
  known.add(key);added.append(name)
 assert registry.read_bytes()==before,'Concurrent registry writer'
 temp=registry.with_name('validation-registry.pad-mutation-six.tmp');temp.write_text(json.dumps(data,indent=2,ensure_ascii=False)+'\n',encoding='utf8');temp.replace(registry)
 committed=json.loads(registry.read_bytes());assert len(committed['functions'])==prior_count+len(added)
 print(json.dumps({'added':added,'prior_count':prior_count,'new_count':len(committed['functions']),'comparisons':5888,'preserved_raw_sha256':archive['raw_sha256']}))
if __name__=='__main__':main()
