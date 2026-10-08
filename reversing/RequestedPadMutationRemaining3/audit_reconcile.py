"""Preserved-log independent native oracle; preparation only, no registry mutation."""
import csv,hashlib,importlib.util,json,struct,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
RUN=HERE/'runs/reconcile/20261008-151115-198747'
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def controller(n):return [([-128,-1,0,1,128][(n+i)%5] if i<4 else [0,1,128,255][(n*3+i)%4]) for i in range(24)]
def inputs(seed):
 a=[0]*24;b=[0]*24;n=seed//3
 if n<196:
  field,pair=divmod(n,49);a[field]=[-32768,-128,-1,0,1,128,32767][pair//7];b[field]=[-32768,-128,-1,0,1,128,32767][pair%7]
 elif n<516:
  field,pair=divmod(n-196,16);a[field+4]=[0,1,128,255][pair//4];b[field+4]=[0,1,128,255][pair%4]
 elif n<708:
  n-=516;a[n//96]=[-128,-36,-1,1,36,128][(n//16)%6]
  for i in range(4):a[8+i]=255 if n%16&(1<<i) else 0
 else:a=controller(n-708);b=controller((n-708)*7+3)
 return a,b
def native_oracle(a,b):
 # Canonical EXE 53F530: Clear48; signed extrema when same sign;
 # nonzero button saturation; opposing left-axis/D-pad cancellation; REP MOVSD12.
 result=[0]*24
 for i in range(4):
  x,y=a[i],b[i]
  if x>=0 and y>=0:result[i]=max(x,y)
  if x<=0 and y<=0:result[i]=min(x,y)
 for i in range(4,24):result[i]=255 if a[i] or b[i] else 0
 for pos,neg,axis in ((8,9,1),(10,11,0)):
  if (result[pos] or result[axis]<0) and (result[neg] or result[axis]>0):result[pos]=result[neg]=result[axis]=0
 return result
def main():
 report=json.loads((RUN/'enriched-summary.json').read_text());manifest=report['manifest'];layout=json.loads((RUN/'layout-proof.json').read_text())
 assert all(report[k] is True for k in ('passed','all_equal','natural_exit','saves_unchanged','configs_restored','layout_file_restored')) and report['exit_code']==0 and report['differences']==0
 assert report['executed_probe_sha256']==manifest['asi']==digest(HERE/'artifacts/reconcile/gta_reversed-preflight-pad-remaining3-reconcile.asi')
 assert digest(RUN/'layout-proof.json')==manifest['layout_proof_sha256']
 assert all(digest(ROOT/p)==h for p,h in manifest['tested_sources'].items())
 assert all(digest(ROOT/p)==h for p,h in manifest['bank_sources'].items())
 spec=importlib.util.spec_from_file_location('strict_remaining3',HERE/'run_local.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
 module.BANK='reconcile';module.MANIFEST=manifest;module.LAYOUT=layout;module.runner.GAME=RUN
 alias=RUN/'preflight-pad-remaining3-reconcile-harness.jsonl';assert not alias.exists();alias.write_bytes((RUN/'harness.jsonl').read_bytes())
 try:checked=module.analyze(RUN/'comparisons.jsonl')
 finally:alias.unlink()
 assert checked['passed'] and checked['comparisons']==2892
 cases=0
 for line in (RUN/'comparisons.jsonl').open():
  row=json.loads(line)
  if row.get('event')!='comparison':continue
  seed=row['seed'];a,b=inputs(seed);out=controller(seed+7);expected=native_oracle(a,b);alias=seed%3
  controllers=[a,b,out];controllers[{0:2,1:0,2:1}[alias]]=expected
  for role,side in enumerate(('original','candidate')):
   value=row[side]
   assert value['pad'][16:324]==[(seed*73+i*29+(i>>3))&1 if 0x118<=i<=0x11F else (seed*73+i*29+(i>>3))&255 for i in range(308)]
   for index,words in enumerate(controllers):assert list(struct.unpack('<24h',bytes(value['controllers'][index][16:64])))==words
   assert list(struct.unpack('<24h',bytes(value['scratch_physical'][role][16:64])))==expected
  cases+=1
 assert cases==2892
 keys=('class','fn_name','address','type');matches=[r for r in csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')) if r['class']=='CPad' and r['fn_name']=='ReconcileTwoControllersInput' and int(r['address'],16)==0x53F530 and r['type']=='S'];assert len(matches)==1
 scope='2892 declared private controller cases: four signed axes, twenty buttons, left-axis/D-pad conflicts, combined patterns and out/A/B aliases; exact full payload, scratch active-role mapping only, inactive scratch/guards unchanged. Independent canonical-EXE output oracle verifies both legs. Native helpers remain uncredited. No real input/game startup/general dependency accreditation.'
 summary=dict(report,strict_preserved_reaudit=checked,independent_native_oracle_cases=cases,approved_scope=scope,raw_log_sha256=digest(RUN/'comparisons.jsonl'),native_oracle_evidence_sha256=digest(HERE/'evidence/ReconcileTwoControllersInput.exe.json'))
 target=HERE/'validation-summary-reconcile.json';target.write_text(json.dumps(summary,indent=2)+'\n')
 proposal={'hook':{k:matches[0][k] for k in keys},'evidence':{'path':target.relative_to(ROOT).as_posix(),'sha256':digest(target),'passed_path':['passed'],'pairs':cases,'original_exe_sha256':report['original_exe_sha256']},'tested_sources':manifest['tested_sources'],'source_provenance':'Exact compiled manifest and independently reaudited preserved raw payload; canonical EXE output oracle verified all cases. Current mapped fullbody/scratch operands verified.','dependency_review':None,'scope':scope,'review_reference':'reversing/RequestedPadMutationRemaining3/DESIGN.md'}
 (HERE/'registration-proposal-reconcile.json').write_text(json.dumps(proposal,indent=2)+'\n');print(json.dumps({'audited':cases,'differences':0,'registry_written':False}))
if __name__=='__main__':main()
