"""Five previous banks: strict final preflight before any publication; --apply opt-in."""
import argparse,csv,hashlib,json,os,re,subprocess,sys,time
from datetime import datetime
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(HERE.parent))
from runtime_runner import GAME,EXE_HASH
BANKS=('CTaskDead','IdleAttractorPending','FightingControlPending','PlayerFighterPending','RoadBlockPending')
BEGIN='<!-- BEGIN PENDING BATCH REGRESSION -->'
END='<!-- END PENDING BATCH REGRESSION -->'
def sha(raw):return hashlib.sha256(raw).hexdigest()
def load(p):return json.loads(Path(p).read_text(encoding='utf-8-sig'))
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--apply',action='store_true');args=ap.parse_args()
 cache={}
 def digest(path):
  p=Path(path);s=p.stat();key=(s.st_dev,s.st_ino,s.st_size,s.st_mtime_ns,s.st_ctime_ns)
  if not s.st_ino:key+=(str(p.resolve()),)
  if key not in cache:
   value=hashlib.sha256()
   with p.open('rb') as stream:
    for chunk in iter(lambda:stream.read(1024*1024),b''):value.update(chunk)
   post=p.stat()
   if (post.st_dev,post.st_ino,post.st_size,post.st_mtime_ns,post.st_ctime_ns)!=(s.st_dev,s.st_ino,s.st_size,s.st_mtime_ns,s.st_ctime_ns):raise RuntimeError('File changed while hashing: '+str(p))
   cache[key]=value.hexdigest()
  return cache[key]
 def local(path):
  p=(ROOT/path).resolve()
  if not p.is_relative_to(ROOT):raise RuntimeError('Manifest source outside repository')
  return p
 headers={p.relative_to(ROOT).as_posix()for p in(ROOT/'source').rglob('*')if p.is_file()and p.suffix in('.h','.hpp','.inl','.inc')}
 changed=subprocess.check_output(['git','diff','HEAD','--name-only','--','source'],cwd=ROOT,text=True).splitlines()+subprocess.check_output(['git','ls-files','--others','--exclude-standard','--','source'],cwd=ROOT,text=True).splitlines()
 required=headers|{p.replace('\\','/')for p in changed if p.endswith('.cpp')and p.replace('\\','/')!='source/game_sa/Game.cpp'}
 if digest(GAME/'gta_sa.exe')!=EXE_HASH:raise RuntimeError('Actual EXE differs from canonical hash')
 plans=[];guards={p:digest(p)for p in(ROOT/'docs/hooks.csv',ROOT/'source/game_sa/Game.cpp',GAME/'gta_sa.exe')};originals={};writes={};all_labels=set()
 registry_path=ROOT/'reversing/validation-registry.json';registry=load(registry_path);originals[registry_path]=registry_path.read_bytes()
 hookrows={(r['class'],r['fn_name']):r for r in csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig',newline=''))}
 # Complete all five preflights before constructing or writing publications.
 for name in BANKS:
  bank=HERE.parent/name;runner=bank/'run_local.py';match=re.search(r"main\((?:Path\(__file__\)\.parent|here),\s*['\"]([^'\"]+)",runner.read_text(encoding='utf-8-sig'))
  if not match:raise RuntimeError('Unknown runner prefix: '+name)
  prefix=match[1];reports=sorted((GAME/f'resultados-{prefix}').glob('*/summary.json'))
  if not reports:raise RuntimeError('No actual reports: '+name)
  report=reports[-1];result=load(report);mp=bank/'artifacts/build-manifest.json';manifest=load(mp)
  if result.get('passed')is not True or report.stat().st_mtime<mp.stat().st_mtime:raise RuntimeError('Latest report not passed/current: '+name)
  if not manifest.get('full_source_manifest')or not required.issubset(manifest['sources']):raise RuntimeError('Incomplete full-source manifest: '+name)
  if manifest.get('game_cpp')!=digest(ROOT/'source/game_sa/Game.cpp'):raise RuntimeError('Normal Game.cpp changed')
  if result.get('executed_probe_sha256')!=manifest['binaries']['asi']or result.get('original_exe_sha256')!=EXE_HASH:raise RuntimeError('Observed ASI/EXE mismatch: '+name)
  for path,value in manifest['sources'].items():
   p=local(path)
   if digest(p)!=value:raise RuntimeError('Current source differs: '+path)
   guards[p]=value
  for path,value in manifest['instrumentation'].items():
   p=(bank/path).resolve()
   if not p.is_relative_to(ROOT)or digest(p)!=value:raise RuntimeError('Instrumentation changed: '+path)
   guards[p]=value
  for ext,value in manifest['binaries'].items():
   p=bank/'artifacts'/f'gta_reversed-{prefix}-probe.{ext}'
   if digest(p)!=value:raise RuntimeError('Archived binary changed: '+name)
   guards[p]=value
  labels=set(result.get('pairs_per_function',{}))
  if not labels or all_labels&labels:raise RuntimeError('Missing/duplicate function coverage')
  all_labels|=labels
  if sum(result['pairs_per_function'].values())!=result['pairs']:raise RuntimeError('Pair totals differ')
  old=load(bank/'validation-summary.json')
  if labels!=set(old.get('pairs_per_function',{})) or result['pairs_per_function']!=old['pairs_per_function']:raise RuntimeError('Five-bank function identity/case coverage changed; explicit review required')
  if not old.get('limitations'):raise RuntimeError('Existing fixture limitations required')
  for p in(report,mp,runner,bank/'README.md',bank/'validation-summary.json'):guards[p]=digest(p)
  plans.append((bank,prefix,report,result,manifest,old['limitations']))
 if sum(x[3]['pairs']for x in plans)!=9984:raise RuntimeError('Expected exact9984 five-bank pairs')
 results=[]
 for bank,prefix,report,result,manifest,limits in plans:
  summary=dict(result,evidence_directory=str(report.parent),covered_functions={k:True for k in result['pairs_per_function']},probe_build_manifest=manifest,limitations=limits)
  summary_path=bank/'validation-summary.json';summary_bytes=(json.dumps(summary,indent=2,ensure_ascii=False)+'\n').encode();writes[summary_path]=summary_bytes
  keys={tuple(k.split('::'))for k in result['pairs_per_function']}
  registry['functions']=[r for r in registry['functions']if(r['hook']['class'].removesuffix('<T>'),r['hook']['fn_name'])not in keys]
  for label,pairs in result['pairs_per_function'].items():
   cls,fn=label.split('::');row=hookrows.get((cls,fn))or hookrows.get((cls+'<T>',fn))
   if not row:raise RuntimeError('Missing exact catalog row: '+label)
   registry['functions'].append(dict(hook={k:row[k]for k in('class','fn_name','address','type')},evidence=dict(path=summary_path.relative_to(ROOT).as_posix(),sha256=sha(summary_bytes),passed_path=['covered_functions',label],pairs=pairs,original_exe_sha256=EXE_HASH),tested_sources=manifest['sources'],source_provenance='Strict final five-bank regression; actual report '+report.parent.name,dependency_review=None,scope=f'{pairs} paired fixture cases. '+' '.join(limits),review_reference=(bank/'README.md').relative_to(ROOT).as_posix()))
  readme=bank/'README.md';text=readme.read_text(encoding='utf-8-sig');date=datetime.fromtimestamp(report.stat().st_mtime).astimezone().strftime('%d/%m/%Y')
  block=BEGIN+f'\n\nRegresión del {date}: **APROBADA**, '+f'{result["pairs"]:,}'.replace(',','.')+' pares exactos. '+f'[Resumen actualizado](validation-summary.json); [informe completo](<{report.parent.as_posix()}/summary.json>).\n'+END
  if BEGIN in text or END in text:
   if text.count(BEGIN)!=1 or text.count(END)!=1 or text.index(END)<text.index(BEGIN):raise RuntimeError('Ambiguous README markers')
   start=text.index(BEGIN);end=text.index(END)+len(END);text=text[:start]+block+text[end:]
  else:text=text.rstrip()+'\n\n'+block+'\n'
  writes[readme]=text.replace('\r\n','\n').encode();results.append(dict(bank=bank.name,passed=True,pairs=result['pairs'],evidence_directory=str(report.parent),executed_probe_sha256=result['executed_probe_sha256'],original_exe_sha256=EXE_HASH))
 writes[registry_path]=(json.dumps(registry,indent=2,ensure_ascii=False)+'\n').encode()
 aggregate_path=HERE/'regression-summary.json';writes[aggregate_path]=(json.dumps(dict(passed=True,strict_final=True,pairs=9984,banks=results),indent=2,ensure_ascii=False)+'\n').encode()
 for p in writes:originals.setdefault(p,p.read_bytes()if p.exists()else None)
 print(json.dumps(dict(kind='five-bank strict regression proposal',passed=True,pairs=9984,banks=results,apply_requested=args.apply),ensure_ascii=True))
 if not args.apply:return
 for p,value in guards.items():
  if digest(p)!=value:raise RuntimeError('Concurrent preflight input change: '+str(p))
 for p,value in originals.items():
  if(p.read_bytes()if p.exists()else None)!=value:raise RuntimeError('Concurrent publication target change')
 snapshot=HERE/'artifacts'/('regression-publish-'+str(time.time_ns()));snapshot.mkdir(parents=True);staged={}
 for n,(p,raw)in enumerate(writes.items()):
  prior=originals[p]
  if prior is not None:(snapshot/f'{n}.before').write_bytes(prior)
  temp=p.with_name(p.name+'.regression.tmp');temp.write_bytes(raw);staged[p]=temp
 (snapshot/'journal.json').write_text(json.dumps([dict(path=p.relative_to(ROOT).as_posix(),before_sha256=sha(originals[p])if originals[p]is not None else None,after_sha256=sha(v))for p,v in writes.items()],indent=2)+'\n',encoding='utf-8')
 replaced=[]
 try:
  for p,temp in staged.items():os.replace(temp,p);replaced.append(p)
 except Exception:
  for p in reversed(replaced):
   if originals[p]is None:p.unlink(missing_ok=True)
   else:p.write_bytes(originals[p])
  raise
if __name__=='__main__':main()
