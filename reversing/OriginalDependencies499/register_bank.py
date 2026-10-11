"""Admit only verified game dependencies, preserving the initial cohort."""
import argparse,csv,gzip,hashlib,json,os,re,shutil,time
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent

def sha(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def load(path):return json.loads(path.read_text(encoding='utf-8'))
def replace(source,target):
 for attempt in range(20):
  try:os.replace(source,target);return
  except OSError:
   if attempt==19:raise
   time.sleep(0.25)
def main():
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('bank');parser.add_argument('run');args=parser.parse_args()
 bank=ROOT/'reversing'/args.bank;run=bank/'runs'/args.run
 audit=load(run/'approval-audit.json');summary=load(run/'summary.json')
 assert audit['registered_hook_verified'] and audit['differences']==0 and audit['null_comparison_records']==0
 assert all(summary[k] is True for k in ('passed','all_equal','state_restored','saves_unchanged','configs_restored'))
 assert all(sha(ROOT/p)==h for p,h in audit['tested_sources'].items())
 assert all(sha(bank/p)==h for p,h in audit['bank_sources'].items())
 entries=re.findall(r'\{\"([^\"]+)\",\"([^\"]+)\",(0x[0-9A-Fa-f]+),(?:0x[0-9A-Fa-f]+|0),-?\d+,(\d+)\}',(bank/'LiveComparison.hpp').read_text(encoding='utf-8'))
 expected={c+'::'+f:int(n) for c,f,a,n in entries};assert expected==audit['pairs_per_function']==summary['pairs_per_function']
 raw=gzip.decompress((run/'comparisons.jsonl.gz').read_bytes());assert hashlib.sha256(raw).hexdigest()==summary['log_sha256']==audit['log_sha256']
 records=[json.loads(line) for line in raw.splitlines()];pairs=[r for r in records if r.get('event')=='comparison']
 assert len(pairs)==sum(expected.values())
 assert all(r['original'] is not None and r['candidate'] is not None and r['original']==r['candidate'] and r['accepted'] and r['equal'] for r in pairs)
 cohort={int(r['address'],16):r for r in csv.DictReader((HERE/'progress.csv').open(encoding='utf-8-sig'))}
 registry=ROOT/'reversing/validation-registry.json';data=load(registry);registry_before=sha(registry)
 hooks_path=ROOT/'docs/hooks.csv'
 with hooks_path.open(encoding='utf-8-sig',newline='') as f:reader=csv.DictReader(f);fields=reader.fieldnames;hooks=list(reader)
 approved=load(HERE/'approved.json');additions=[]
 scope='Banco par a par en juego completo con partida cargada; misma entrada EXE con hook RH apagado/activo. Rutas y restauración verificadas. Evidencia histórica limitada a fixtures, no integración universal ni independencia transitiva del EXE. Alcance por función en README del banco.'
 evidence=dict(passed=True,pairs=len(pairs),differences=0,registered_pairs=len(pairs),registered_functions=len(entries),functions=expected,covered_functions={f:True for f in expected},scope=scope,dependency_review=None,run=run.relative_to(ROOT).as_posix(),summary_sha256=sha(run/'summary.json'),result_txt_sha256=sha(run/'RESULTADO.txt'),log_sha256=summary['log_sha256'],all_equal=True,state_restored=True,saves_unchanged=True,configs_restored=True,original_exe_sha256=summary['original_exe_sha256'],executed_probe_sha256=summary['executed_probe_sha256'],manifest={'tested_sources':audit['tested_sources'],'bank_sources':audit['bank_sources']},reaudit=audit,comparison_storage={'compressed_path':(run/'comparisons.jsonl.gz').relative_to(ROOT).as_posix(),'compressed_sha256':sha(run/'comparisons.jsonl.gz'),'uncompressed_sha256':summary['log_sha256'],'uncompressed_bytes':len(raw),'compression':'gzip','gzip_mtime':0,'lossless_roundtrip_sha256':summary['log_sha256']})
 evpath=bank/'validation-summary.json';assert not evpath.exists()
 evtext=json.dumps(evidence,ensure_ascii=False,indent=2)+'\n';evhash=hashlib.sha256(evtext.encode('utf-8')).hexdigest()
 for cls,fn,address,n in entries:
  va=int(address,16);address=f'0x{va:08X}';identity=cls+'::'+fn
  assert va in cohort and cohort[va]['category']=='game' and cohort[va]['runtime_status']!='passed'
  assert address not in approved
  assert not any(r['hook']['class']==cls and r['hook']['fn_name']==fn for r in data['functions'])
  matches=[r for r in hooks if r['class']==cls and r['fn_name']==fn];assert len(matches)<=1
  if matches:
   row=matches[0];assert int(row['address'],16)==va and row['validated']=='0'
  else:
   row=dict(class_=cls,fn_name=fn,address=address,reversed='1',locked='0',validated='0',type='S');row['class']=row.pop('class_');assert set(row)==set(fields);hooks.append(row)
  row['validated']='1';hook={k:row[k] for k in ('class','fn_name','address','type')}
  additions.append(dict(hook=hook,evidence={'path':evpath.relative_to(ROOT).as_posix(),'sha256':evhash,'passed_path':['covered_functions',identity],'pairs':int(n),'original_exe_sha256':summary['original_exe_sha256']},tested_sources=audit['tested_sources'],dependency_review=None,source_provenance='Fuente propia contrastada con EXE canónico y banco RH OFF/ON dentro del juego.',scope=scope,review_reference=(bank/'README.md').relative_to(ROOT).as_posix()))
  approved[address]={'identity':identity,'validation_summary':evpath.relative_to(ROOT).as_posix()}
 backup=ROOT/f'reversing/validation-registry.backup-before-{args.bank.lower()}.json';assert not backup.exists();assert sha(registry)==registry_before
 shutil.copy2(registry,backup);assert sha(backup)==registry_before
 evpath.write_bytes(evtext.encode('utf-8'));assert sha(evpath)==evhash
 data['functions'].extend(additions)
 temp=registry.with_suffix('.admission.tmp');temp.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8');assert sha(registry)==registry_before;replace(temp,registry)
 temp=hooks_path.with_suffix('.admission.tmp')
 with temp.open('w',encoding='utf-8',newline='') as f:w=csv.DictWriter(f,fieldnames=fields);w.writeheader();w.writerows(hooks)
 replace(temp,hooks_path)
 (HERE/'approved.json').write_text(json.dumps(approved,indent=2)+'\n',encoding='utf-8')
 print(json.dumps({'registered_functions':len(additions),'registered_pairs':len(pairs),'registry_before_sha256':registry_before,'registry_after_sha256':sha(registry)}))
if __name__=='__main__':main()
