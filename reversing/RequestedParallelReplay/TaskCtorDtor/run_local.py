"""Prepared serial runner. No build or runtime is performed on import."""
import collections,json,os,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[2]
sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
CLASSES=('CTaskComplexKillPedFromBoat','CTaskComplexHitPedWithCar','CTaskComplexSunbathe')
EXPECTED={f'{cls}::{fn}':256 for cls in CLASSES for fn in ('Constructor','Destructor')}
def expected_trace(function,seed):
    sun=function.startswith('CTaskComplexSunbathe');dtor=function.endswith('Destructor');out=[]
    field=0x34 if sun else 0xC
    if seed&1:
        out.append(['cleanup',True,field,True,5 if sun else -1] if dtor else ['register',True,field,True])
    if sun:
        if dtor:
            if seed&4:out.append(['remove_ref',3])
            if seed&8:out.append(['remove_ref',5])
        else:out += [['block','beach'],['index','beach'],['block','sunbathe'],['index','sunbathe']]
    if dtor and seed&2:out.append(['delete_sub',True,1])
    return out
def analyze(path):
    if not runner.terminal_event(path):return None
    rows=[json.loads(line) for line in path.read_text().splitlines()]
    if any(r['event'] in ('error','assertion') for r in rows):raise RuntimeError('Probe failure; preserve full log')
    pairs=[r for r in rows if r['event']=='comparison'];end=[r for r in rows if r['event']=='complete']
    if len(end)!=1 or len(pairs)!=1536 or dict(collections.Counter(r['function'] for r in pairs))!=EXPECTED:raise RuntimeError('Function coverage mismatch')
    if {r['pair'] for r in pairs}!=set(range(1536)):raise RuntimeError('Pair identity mismatch')
    for function in EXPECTED:
        matches=[r for r in pairs if r['function']==function]
        if {r['seed'] for r in matches}!=set(range(256)):raise RuntimeError('Seed coverage failure '+function)
        for r in matches:
            wanted=expected_trace(function,r['seed']);terminal='CTask' if function.endswith('Destructor') else 'derived'
            if not (r['equal'] is True and r['accepted'] is True and r['original']==r['candidate']):raise RuntimeError('Comparison mismatch '+function)
            for side in ('original','candidate'):
                result=r[side]
                if result['trace']!=wanted or result['vptr']!=terminal or result['switched'] is not True or result['return_ok'] is not True:raise RuntimeError('ABI/trace/terminal vptr gate '+function)
    summaries=[r for r in rows if r['event']=='function_summary']
    if len(summaries)!=6 or any(r['cases']!=256 or r['failures']!=0 for r in summaries):raise RuntimeError('Summary mismatch')
    if {r['function'] for r in summaries}!=set(EXPECTED):raise RuntimeError('Summary identities mismatch')
    done=end[0]
    if done.get('state_restored') is not True or done.get('bytes_restored') is not True or done.get('failed_ranges')!=[] or done.get('pairs')!=1536:raise RuntimeError('Restoration mismatch')
    return {'pairs':1536,'pairs_per_function':EXPECTED,'all_equal':True,'state_restored':True,'log_sha256':runner.digest(path),'scope':'private task storage; reference/animation collaborators stubbed; subtask destructor trace only; exact terminal vptr and bytes checked'}
def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text());prefix='requested-task-ctor-dtor'
    for p,h in manifest['frozen_inputs'].items():assert runner.digest(ROOT/p)==h,p
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-requested-task-ctor-dtor.{ext}';assert runner.digest(source)==manifest[ext]
        immutable=HERE/f'artifacts/{manifest[ext]}.{ext}'
        if not immutable.exists():shutil.copy2(source,immutable)
        target=runner.GAME/f'_baseline/gta_reversed-{prefix}-probe.{ext}';assert target.resolve().is_relative_to((runner.GAME/'_baseline').resolve())
        if target.exists():
            old=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not old.exists():target.replace(old)
            else:target.unlink()
        try:os.link(immutable,target)
        except OSError:shutil.copy2(immutable,target)
    bank_marker=runner.GAME/f'{prefix}.start';bank_log=runner.GAME/f'{prefix}.jsonl'
    assert not bank_marker.exists(),'Preexisting frozen-bank marker'
    if bank_log.exists():
        previous=HERE/'artifacts'/f'previous-{prefix}-{runner.digest(bank_log)}.jsonl'
        if not previous.exists():shutil.copy2(bank_log,previous)
        bank_log.unlink()
    original_cfg=runner.probe_configuration
    runner.probe_configuration=lambda data:original_cfg(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
    try:status=runner.run_case(prefix,manifest['asi'],analyze,'three task constructor/destructor contracts',timeout=180,stop_before_analysis=True,unhook='Win/Win/NOTSA_WinMain')
    finally:bank_marker.unlink(missing_ok=True)
    run=max((runner.GAME/f'resultados-{prefix}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    if bank_log.exists():shutil.copy2(bank_log,dest/'raw-bank.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('PRESERVED',dest);return status
if __name__=='__main__':raise SystemExit(main())
