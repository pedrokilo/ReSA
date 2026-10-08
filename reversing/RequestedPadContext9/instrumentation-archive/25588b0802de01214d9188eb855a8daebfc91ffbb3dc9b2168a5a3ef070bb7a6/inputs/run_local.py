import gzip,hashlib,json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-pad-context9'

base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
TARGETS={
 'AimWeaponLeftRight':(0x541040,6048,16),'AimWeaponUpDown':(0x5410C0,6048,16),
 'GetPedWalkLeftRight-ped':(0x540DC0,6048,16),'GetPedWalkUpDown-ped':(0x540E20,6048,16),
 'LookAroundLeftRight':(0x540BD0,6048,16),'LookAroundUpDown':(0x540CC0,6048,16),
 'GetDisplayVitalStats':(0x5408B0,15480,16),'GetWeapon':(0x540180,15480,32),'WeaponJustDown':(0x540250,15480,1)}

def expected_metadata(name,seed):
    if TARGETS[name][1]==6048:
        return seed//864,0,(seed//54)%16
    if seed<15360:
        return (seed//1280)%3,(seed//256)%5,0
    return ((seed-15360)//10)%3,((seed-15360)//2)%5,0

def analyze(log):
    if not runner.terminal_event(log):return None
    counts={name:0 for name in TARGETS};seen=set();failures=[];start=None;final=None
    with log.open(encoding='utf8') as stream:
        for line in stream:
            row=json.loads(line);event=row.get('event')
            if start is None:
                start=row
                if event!='start' or (row.get('functions'),row.get('spatial_cases'),row.get('gun_cases'))!=(9,6048,15480):raise RuntimeError('Wrong bank start or corpus')
                continue
            if event=='comparison':
                name=row['function'];seed=row['seed']
                if name not in TARGETS:raise RuntimeError('Unscoped identity')
                address,total,width=TARGETS[name];key=(name,seed)
                if type(seed) is not int or not 0<=seed<total or key in seen or row['address']!=address:raise RuntimeError('Duplicate, wrong address or out-of-corpus seed')
                if row.get('switch_exact') is not True or row.get('actor_task_immutable') is not True:raise RuntimeError('Entry switching or private actor/task immutability failed')
                if (row['actor_state'],row['gun_state'],row['global_flags'])!=expected_metadata(name,seed):raise RuntimeError('Fixture context differs from frozen corpus')
                for side in ('original_bits','candidate_bits'):
                    if type(row[side]) is not int or not 0<=row[side]<(1<<width):raise RuntimeError('Typed return bits outside declared ABI')
                if type(row['equal']) is not bool:raise RuntimeError('Malformed comparison verdict')
                if not row['equal']:
                    for side in ('original','candidate'):
                        outcome=row[side]
                        if outcome['bits']!=row[side+'_bits'] or len(outcome['pad'])!=0x134 or [len(value) for value in outcome['globals']]!=[2,1,1]:raise RuntimeError('Incomplete mismatch payload')
                    if row['original']==row['candidate']:raise RuntimeError('False mismatch')
                    failures.append({'function':name,'seed':seed,'original_bits':row['original_bits'],'candidate_bits':row['candidate_bits']})
                elif row['original_bits']!=row['candidate_bits']:raise RuntimeError('False equality of typed return')
                seen.add(key);counts[name]+=1
            elif event=='complete':final=row
            else:raise RuntimeError('Unexpected bank event: '+str(event))
    if final is None or final.get('state_restored') is not True or final.get('floating_environment_restored') is not True or final.get('comparisons')!=82728 or final.get('functions')!=9:raise RuntimeError('Incomplete bank or global/FP restoration failed')
    if counts!={name:data[1] for name,data in TARGETS.items()}:raise RuntimeError('Missing fixture cases')
    harness=runner.GAME/'preflight-pad-context9-harness.jsonl'
    if not runner.terminal_event(harness):return None
    rows=[json.loads(line) for line in harness.read_text(encoding='utf8').splitlines()];last=rows[-1]
    if [r['event'] for r in rows]!=['preflight_entered','before_task_bank','task_bank_returned','complete']:raise RuntimeError('Unexpected preflight stages')
    if any(last.get(field) is not True for field in ('state_restored','entries_restored','flags_restored')) or last.get('changed_ranges')!=[] or last.get('snapshot_ranges',0)<17692:raise RuntimeError('All-entry/vtable/flag restoration failed')
    return {'comparisons':82728,'functions':9,'cases_by_function':counts,'differences':len(failures),'failures':failures,'passed':not failures,'all_equal':not failures,'bank_restored':True,'floating_environment_restored':True,'actor_task_immutable':True,'harness_restored':True,'harness_sha256':runner.digest(harness),'scope':'Private readonly ABI field contexts and task tags; no helper, real actor lifecycle or real input accreditation'}

def main():
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    frozen=json.loads((HERE/'corpus-frozen.json').read_text())
    for name,h in frozen['inputs'].items():assert runner.digest(ROOT/name)==h,name
    for p,h in manifest['tested_sources'].items():assert runner.digest(ROOT/p)==h
    for p,h in manifest['bank_sources'].items():assert runner.digest(ROOT/p)==h
    previous=runner.GAME/'preflight-pad-context9-harness.jsonl'
    if previous.exists():
        target=HERE/'artifacts'/('previous-harness-'+runner.digest(previous)+'.jsonl')
        shutil.copy2(previous,target);previous.unlink()
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-preflight-pad-context9.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():
            backup=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not backup.exists():shutil.copy2(target,backup)
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'CPad context nine82728',timeout=600,stop_before_analysis=False,unhook='',wait_for_exit=True)
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
