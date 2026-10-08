"""Root-only serial native runner, one independently manifested CPad subbank per invocation."""
import argparse,hashlib,json,shutil,sys,time
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
TARGETS={'reconcile':('ReconcileTwoControllersInput',0x53F530,2892),'update':('Update',0x541C40,2560),'shake':('StartShake',0x53F920,89600)}
CLOCKS=[0,1,127,65535,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF]
FREQUENCIES=[0,1,127,128,255];DURATIONS=[(0,100),(100,100),(200,100),(-1,100),(100,-1),(-32768,32767),(32767,-32768)];DELAYS=[0,1,100,0xFFFFFFFF]
BANK=None;MANIFEST=None;LAYOUT=None;last_progress=0
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')

def metadata(bank,seed):
 row={'alias':0,'clock':CLOCKS[seed%8],'deadline':0,'delay':0,'duration':0,'frequency':0,'stored_frequency':0,'gates':0,'pad_id':0,'domain':'normal'}
 if bank=='reconcile':
  row['alias']=seed%3;n=seed//3
  if n<196:
   pair=n%49
   if pair//7 in (0,6) or pair%7 in (0,6):row['domain']='signed-short-stress'
 elif bank=='update':
  row['pad_id']=seed%2
  if (seed//16)%4==3:row['domain']='byte-counter-stress'
 else:
  n=seed;row['delay']=DELAYS[n%4];n//=4;deadline_case=n%4;n//=4;row['clock']=CLOCKS[n%8];n//=8;row['stored_frequency']=FREQUENCIES[n%5];n//=5;row['frequency']=FREQUENCIES[n%5];n//=5;duration_case=n%7;n//=7;row['duration']=DURATIONS[duration_case][1];row['gates']=n
  clock=row['clock'];row['deadline']=[(clock-1)&0xFFFFFFFF,clock,(clock+1)&0xFFFFFFFF,0xFFFFFFFF][deadline_case]
  if duration_case>=3:row['domain']='signed-duration-stress'
 return row

def progress(log):
 global last_progress
 now=time.monotonic()
 if now-last_progress<10 or not log.exists():return
 with log.open('rb') as stream:
  stream.seek(max(0,log.stat().st_size-65536));tail=stream.read()
 lines=tail.split(b'\n')[:-1]
 for line in reversed(lines):
  try:row=json.loads(line)
  except (ValueError,UnicodeError):continue
  if row.get('event')=='comparison':print('Progress',BANK,row['seed']+1,'/',TARGETS[BANK][2],round(log.stat().st_size/1024/1024,1),'MiB',flush=True);last_progress=now;break

def payload_contract(outcome,bank,candidate):
 result={key:outcome[key] for key in ('pad','controllers','globals','return_reference_exact','fp')}
 if bank=='reconcile':result['scratch_active_role']=outcome['scratch_physical'][int(candidate)][16:64]
 else:result['scratch_physical']=outcome['scratch_physical']
 return result

def analyze(log):
 if not runner.terminal_event(log):progress(log);return None
 name,address,total=TARGETS[BANK];seen=set();failures=[];first=None;final=None;proof=None;guard_baseline=None;global_baseline=None
 with log.open(encoding='utf8') as stream:
  for line in stream:
   row=json.loads(line);event=row.get('event')
   if first is None:
    first=row
    if event!='start' or (row.get('bank'),row.get('function'),row.get('address'),row.get('cases'))!=(BANK,name,address,total):raise RuntimeError('Wrong selected bank/corpus')
    continue
   if event=='current_artifact_proof':
    if proof is not None or seen:raise RuntimeError('Wrong proof order')
    proof=row['proof'];f=LAYOUT['functions'][0];s=LAYOUT['scratch'][0]
    if proof.get('artifact_sha256')!=MANIFEST['asi'] or proof.get('pdb_sha256')!=MANIFEST['pdb'] or (proof.get('function_rva'),proof.get('function_bytes'),proof.get('scratch_rva'),proof.get('scratch_bytes'),proof.get('encoded_operands'),proof.get('explicit_highlow_relocations'))!=(f['rva'],f['length'],s['rva'],48,len(f['scratch_operands']),len(f['highlow_relocations'])) or proof.get('mapped_body_exact') is not True or proof.get('artifact_hash_exact') is not True:raise RuntimeError('Actual current artifact/body/operand proof failed')
   elif event=='comparison':
    seed=row['seed']
    if proof is None or type(seed) is not int or not 0<=seed<total or seed in seen or seed!=len(seen) or row['function']!=name or row['address']!=address:raise RuntimeError('Wrong identity/seed/proof or duplicate')
    expected=metadata(BANK,seed)
    if any(row.get(key)!=value for key,value in expected.items()):raise RuntimeError('Fixture metadata changed')
    if any(row.get(key) is not True for key in ('switch_exact','guards_unchanged','inactive_scratch_unchanged','return_reference_exact','globals_unchanged','helper_entries_original')):raise RuntimeError('Switch/guard/inactive/reference/global/helper invariant failed')
    seeds=[[((seed*73+i*29+role*101)&255) for i in range(48)] for role in range(2)]
    if row['scratch_seed_bytes']!=seeds:raise RuntimeError('Wrong scratch seeds')
    contracts=[]
    for candidate,side in enumerate(('original','candidate')):
     value=row[side]
     if len(value['pad'])!=340 or [len(x) for x in value['controllers']]!=[80]*3 or [len(x) for x in value['globals']]!=[16,33,33] or [len(x) for x in value['scratch_physical']]!=[80,80]:raise RuntimeError('Incomplete full bounded payload')
     spans=[value['pad']]+value['controllers']+value['globals']+value['scratch_physical']
     if any(type(byte) is not int or not 0<=byte<=255 for span in spans for byte in span):raise RuntimeError('Invalid raw byte value')
     if value['return_reference_exact'] is not True or value['scratch_guards_unchanged'] is not True or value['inactive_scratch_unchanged'] is not True:raise RuntimeError('Wrong per-leg gates')
     if value['pad'][:16]!=[0x49]*16 or value['pad'][-16:]!=[0xC3]*16 or any(x[:16]!=[0x31]*16 or x[-16:]!=[0xE7]*16 for x in value['controllers']):raise RuntimeError('Private guards changed')
     guards=[x[:16]+x[64:] for x in value['scratch_physical']]
     if guard_baseline is None:guard_baseline=guards
     if guards!=guard_baseline:raise RuntimeError('Physical scratch neighbour guards changed')
     for role,arena in enumerate(value['scratch_physical']):
      active=(role==candidate) if BANK=='reconcile' else (role==0) if BANK=='update' else False
      if not active and arena[16:64]!=seeds[role]:raise RuntimeError('Inactive physical scratch changed')
     current=[list(x) for x in value['globals']]
     if int.from_bytes(bytes(value['globals'][0][8:12]),'little')!=expected['clock'] or value['globals'][1][16]!=int(bool(expected['gates']&1)) or value['globals'][2][16]!=int(bool(expected['gates']&2)):raise RuntimeError('Readonly clock/gates changed')
     current[0][8:12]=[0]*4;current[1][16]=0;current[2][16]=0
     if global_baseline is None:global_baseline=current
     if current!=global_baseline:raise RuntimeError('Readonly global neighbours changed')
     if set(value['fp'])!={'rounding','exceptions','mxcsr'} or any(type(v) is not int for v in value['fp'].values()):raise RuntimeError('Incomplete FP semantic payload')
     contracts.append(payload_contract(value,BANK,bool(candidate)))
    if type(row['equal']) is not bool or row['equal']!=(contracts[0]==contracts[1]):raise RuntimeError('Wrong declared contract verdict')
    if not row['equal']:failures.append({'seed':seed,'domain':row['domain']})
    seen.add(seed)
   elif event=='complete':final=row
   else:raise RuntimeError('Unexpected bank event '+str(event))
 if final is not None and (final.get('phased_chunk_pairs')!=1024 or final.get('phased_original_buffer_bound') is not True or type(final.get('phased_original_buffer_estimate_bytes')) is not int or not 0<final['phased_original_buffer_estimate_bytes']<=32*1024*1024):raise RuntimeError('Phase buffer contract failed')
 if seen!=set(range(total)) or final is None or final.get('comparisons')!=total or any(final.get(key) is not True for key in ('state_restored','floating_environment_restored','switch_exact','guards_unchanged','inactive_scratch_unchanged','return_reference_exact','globals_unchanged','helper_entries_original')):raise RuntimeError('Missing cases or terminal restoration/gates')
 harness=runner.GAME/f'preflight-pad-remaining3-phased-{BANK}-harness.jsonl'
 if not runner.terminal_event(harness):return None
 rows=[json.loads(line) for line in harness.read_text().splitlines()];last=rows[-1]
 if [r['event'] for r in rows]!=['preflight_entered','before_task_bank','task_bank_returned','complete'] or last.get('snapshot_ranges',0)<17692 or last.get('changed_ranges')!=[] or any(last.get(key) is not True for key in ('state_restored','entries_restored','flags_restored')):raise RuntimeError('All-entry/vtable/flag restoration failed')
 return {'passed':not failures,'all_equal':not failures,'bank':BANK,'function':'CPad::'+name,'address':hex(address),'comparisons':total,'functions':1,'differences':len(failures),'failures':failures,'current_artifact_proof':proof,'bank_restored':True,'floating_environment_restored':True,'harness_restored':True,'harness_sha256':runner.digest(harness),'scope':'Declared private pad/controllers, readonly globals and two explicitly traced scratch arenas. Reconcile active scratch role only, inactive/guards immutable. No helper/OS input/game startup accreditation.'}

def main():
 global BANK,MANIFEST,LAYOUT
 parser=argparse.ArgumentParser();parser.add_argument('--bank',required=True,choices=list(TARGETS));args=parser.parse_args();BANK=args.bank;art=HERE/'artifacts'/BANK;PREFIX='requested-pad-remaining3-phased-'+BANK
 MANIFEST=json.loads((art/'manifest.json').read_text());assert MANIFEST['selected_bank']==BANK and MANIFEST['comparisons']==TARGETS[BANK][2]
 for name,h in MANIFEST['tested_sources'].items():assert runner.digest(ROOT/name)==h
 for name,h in MANIFEST['bank_sources'].items():assert runner.digest(ROOT/name)==h
 frozen=json.loads((HERE/'corpus-frozen.json').read_text())
 for name,h in frozen['inputs'].items():assert runner.digest(ROOT/name)==h
 layout=art/'layout-proof.json';assert runner.digest(layout)==MANIFEST['layout_proof_sha256'];LAYOUT=json.loads(layout.read_text());assert LAYOUT['asi_sha256']==MANIFEST['asi'] and LAYOUT['pdb_sha256']==MANIFEST['pdb']
 installed_layout=runner.GAME/'requested-pad-remaining3-phased-layout.json';saved_layout=installed_layout.read_bytes() if installed_layout.exists() else None
 harness=runner.GAME/f'preflight-pad-remaining3-phased-{BANK}-harness.jsonl'
 if harness.exists():shutil.copy2(harness,art/('previous-harness-'+runner.digest(harness)+'.jsonl'));harness.unlink()
 for ext in ('asi','pdb'):
  source=art/f'gta_reversed-preflight-pad-remaining3-phased-{BANK}.{ext}';assert runner.digest(source)==MANIFEST[ext];target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
  if target.exists():
   backup=target.with_name(target.name+'.previous-'+runner.digest(target))
   if not backup.exists():shutil.copy2(target,backup)
  shutil.copy2(source,target)
 try:
  shutil.copy2(layout,installed_layout)
  status=runner.run_case(PREFIX,MANIFEST['asi'],analyze,'CPad '+TARGETS[BANK][0]+' '+str(TARGETS[BANK][2]),timeout=1200,stop_before_analysis=False,unhook='',wait_for_exit=True)
 finally:
  if saved_layout is None:installed_layout.unlink(missing_ok=True)
  else:installed_layout.write_bytes(saved_layout)
 run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent;destination=HERE/'runs'/BANK/run.name;destination.mkdir(parents=True,exist_ok=True)
 for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
  if (run/name).exists():shutil.copy2(run/name,destination/name)
 if harness.exists():shutil.copy2(harness,destination/'harness.jsonl')
 shutil.copy2(layout,destination/'layout-proof.json')
 report=json.loads((run/'summary.json').read_text());report.update(manifest=MANIFEST,runner_sha256=runner.digest(Path(__file__)),layout_proof_sha256=runner.digest(layout),layout_file_restored=(installed_layout.read_bytes()==saved_layout if saved_layout is not None else not installed_layout.exists()))
 if report.get('passed') and (report.get('differences',1)!=0 or report.get('all_equal') is not True or report.get('natural_exit') is not True or report.get('exit_code')!=0 or not report['layout_file_restored']):report['passed']=False;report['error']='Explicit zero differences/natural0/layout restoration required';status=1
 (destination/'enriched-summary.json').write_text(json.dumps(report,indent=2)+'\n')
 raw=destination/'comparisons.jsonl'
 if raw.exists():
  from archive_log import archive_preserved_log
  archive=archive_preserved_log(raw);(destination/'log-archive.json').write_text(json.dumps(archive,indent=2)+'\n')
 print('Preserved',destination,'passed',report['passed']);return status
if __name__=='__main__':raise SystemExit(main())
