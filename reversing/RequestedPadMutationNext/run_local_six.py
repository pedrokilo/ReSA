import gzip,hashlib,json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-pad-mutation-six'

base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
TARGETS={'StopShaking':(0x53FB50,2048),'SetTouched':(0x53F200,2048),'ClearMouseHistory':(0x541BD0,256),'ClearKeyBoardHistory':(0x53F1E0,256),'Clear':(0x541A70,1024),'Initialise':(0x541D90,256)}
CLOCKS=[0,1,127,65535,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF]
ARGS=[-32768,-128,-1,0,1,127,255,32767]
SIZES=[16,0x750,4,0x3C,24,0x268,16,16]

def pattern(size,poison,role):return [0 if poison==0 else 255 if poison==1 else 165 if poison==2 else 90 if poison==3 else (poison*73+i*29+(i>>3)+role*47)&255 for i in range(size)]
def canonical_pad(data,start=0):
    for offset in range(0x118,0x120):data[start+offset]&=1

def fixture(name,seed):
    poison=seed//8 if name in ('StopShaking','SetTouched') else seed//4 if name=='Clear' else seed
    clock=CLOCKS[seed%8] if name=='SetTouched' else 0
    pad=pattern(0x134,poison,0);canonical_pad(pad)
    values=[pattern(size,poison,role+1) for role,size in enumerate(SIZES)]
    for base in (0,20,40):values[3][base+3]&=1;values[3][base+4]&=1
    canonical_pad(values[5]);canonical_pad(values[5],0x134)
    values[7][8:12]=list(clock.to_bytes(4,'little'))
    return pad,values,clock,ARGS[seed%8] if name=='StopShaking' else ARGS[0],bool(seed&1) if name=='Clear' else False,bool(seed&2) if name=='Clear' else False

# Write masks independently transcribed from canonicalEXE541A70 and clear helpers; never sourced from candidate implementation.
def clear_pad(pad,disabled,phase):
    spans=[(0,0x60),(0x60,0x18),(0x78,0x90),(0x10C,2),(0x110,9),(0x120,17)]
    if phase:spans.append((0x108,1))
    if disabled:spans.extend([(0x10E,2),(0x119,7)])
    for offset,size in spans:pad[offset:offset+size]=[0]*size

def native_oracle(name,seed):
    pad,values,clock,arg,disabled,phase=fixture(name,seed)
    if name=='SetTouched':pad[0x120:0x124]=list(clock.to_bytes(4,'little'))
    if name in ('ClearKeyBoardHistory','Clear','Initialise'):
        for base in (0,0x270,0x4E0):
            enter=values[1][base+0x23C:base+0x23E];values[1][base:base+0x270]=[0]*0x270;values[1][base+0x23C:base+0x23E]=enter
    if name in ('ClearMouseHistory','Clear','Initialise'):
        for base in (0,20,40):values[3][base:base+7]=[0]*7
    if name=='Clear':clear_pad(pad,disabled,phase)
    if name=='Initialise':
        for base in (0,0x134):
            one=values[5][base:base+0x134];clear_pad(one,True,True);one[0x10A:0x10C]=[0,0];values[5][base:base+0x134]=one
        values[2][0:2]=[0,0]
    return pad,values

def analyze(log):
    if not runner.terminal_event(log):return None
    counts={name:0 for name in TARGETS};seen=set();failures=[];first=None;final=None
    with log.open(encoding='utf8') as stream:
        for line in stream:
            row=json.loads(line);event=row.get('event')
            if first is None:
                first=row
                if event!='start' or (row.get('functions'),row.get('comparisons'),row.get('global_ranges'))!=(6,5888,8):raise RuntimeError('Wrong mutation corpus')
                continue
            if event=='comparison':
                name=row['function'];seed=row['seed'];key=(name,seed)
                if name not in TARGETS or type(seed) is not int or not 0<=seed<TARGETS[name][1] or key in seen or row['address']!=TARGETS[name][0]:raise RuntimeError('Wrong identity, address, duplicate or seed')
                _,_,clock,arg,disabled,phase=fixture(name,seed)
                if (row['clock'],row['stop_argument'],row['clear_disabled'],row['reset_phase'])!=(clock,arg,disabled,phase) or row.get('switch_exact') is not True:raise RuntimeError('Wrong fixture or entry switch')
                for side in ('original','candidate'):
                    value=row[side]
                    if len(value['pad'])!=0x134 or [len(v) for v in value['globals']]!=SIZES:raise RuntimeError('Missing full object/global payload')
                    if any(type(byte) is not int or not 0<=byte<=255 for span in [value['pad']]+value['globals'] for byte in span):raise RuntimeError('Invalid payload bytes')
                expected_pad,expected_globals=native_oracle(name,seed)
                if row['original']['pad']!=expected_pad or row['original']['globals']!=expected_globals:raise RuntimeError('Native effects differ from primary-body write mask; stop for diagnosis')
                if type(row['equal']) is not bool or row['equal']!=(row['original']==row['candidate']):raise RuntimeError('Invalid comparison verdict')
                if not row['equal']:failures.append({'function':name,'seed':seed})
                seen.add(key);counts[name]+=1
            elif event=='complete':final=row
            else:raise RuntimeError('Unexpected mutation event')
    if final is None or final.get('comparisons')!=5888 or final.get('functions')!=6 or any(final.get(field) is not True for field in ('state_restored','globals_restored','floating_environment_restored','switch_exact')):raise RuntimeError('Incomplete bank or global/FP restoration')
    if counts!={name:total for name,(_,total) in TARGETS.items()}:raise RuntimeError('Missing mutation cases')
    harness=runner.GAME/'preflight-pad-mutation-six-harness.jsonl'
    if not runner.terminal_event(harness):return None
    rows=[json.loads(line) for line in harness.read_text().splitlines()];last=rows[-1]
    if [r['event'] for r in rows]!=['preflight_entered','before_task_bank','task_bank_returned','complete'] or last.get('snapshot_ranges',0)<17692 or last.get('changed_ranges')!=[] or any(last.get(field) is not True for field in ('state_restored','entries_restored','flags_restored')):raise RuntimeError('Harness all-entry/vtable/flag restoration failed')
    return {'passed':not failures,'all_equal':not failures,'comparisons':5888,'functions':6,'cases_by_function':counts,'differences':len(failures),'failures':failures,'native_write_mask_verified':True,'bank_restored':True,'floating_environment_restored':True,'harness_restored':True,'harness_sha256':runner.digest(harness),'scope':'Six mutation contracts, private pad and complete seeded globals only; no helper/startup/input accreditation'}

def main():
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    frozen=json.loads((HERE/'corpus-six-frozen.json').read_text())
    for name,h in frozen['inputs'].items():assert runner.digest(ROOT/name)==h,name
    for p,h in manifest['tested_sources'].items():assert runner.digest(ROOT/p)==h
    for p,h in manifest['bank_sources'].items():assert runner.digest(ROOT/p)==h
    previous=runner.GAME/'preflight-pad-mutation-six-harness.jsonl'
    if previous.exists():
        target=HERE/'artifacts'/('previous-harness-'+runner.digest(previous)+'.jsonl')
        shutil.copy2(previous,target);previous.unlink()
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-preflight-pad-mutation-six.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():
            backup=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not backup.exists():shutil.copy2(target,backup)
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'CPad mutation six5888',timeout=600,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    destination=HERE/'runs'/run.name;destination.mkdir(parents=True,exist_ok=True)
    for filename in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/filename).exists():shutil.copy2(run/filename,destination/filename)
    if previous.exists():shutil.copy2(previous,destination/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    if report.get('passed') and (report.get('differences',1)!=0 or report.get('all_equal') is not True):
        report['passed']=False;report['error']='Invalid approval: explicit zero differences and all_equal required';status=1
    (destination/'enriched-summary.json').write_text(json.dumps(report,indent=2))
    raw=destination/'comparisons.jsonl'
    if raw.exists():
        from archive_log import archive_preserved_log
        info=archive_preserved_log(raw)
        (destination/'log-archive.json').write_text(json.dumps(info,indent=2)+'\n')
    print('Preserved',destination,'passed',report['passed'])
    return status
if __name__=='__main__':raise SystemExit(main())
