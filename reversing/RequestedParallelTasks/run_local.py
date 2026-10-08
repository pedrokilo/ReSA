"""Count/branch-aware analyzer and root-owned runner for the frozen frontend bank."""
import collections,json,os,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
EXPECTED={
 'lifetime':{f'{name}::{fn}':128 for name in ('CTaskComplexGangJoinRespond','CTaskComplexGoToAttractor','CTaskComplexSmartFleePoint') for fn in ('Constructor','Destructor')}|{'CTaskComplexFollowLeaderInFormation::Destructor':128},
 'expanded':{'CTaskComplexGangJoinRespond::Destructor':64,'CTaskComplexFollowLeaderInFormation::Destructor':64,'CTaskComplexGoToAttractor::Destructor':32,'CTaskComplexSmartFleePoint::Destructor':32}}
PREFIX={'lifetime':'requested-task-lifetime','expanded':'requested-task-expanded'}
def analyzer(mode):
 def analyze(path):
    if not runner.terminal_event(path):return None
    rows=[json.loads(line) for line in path.read_text().splitlines()]
    if any(r['event'] in ('error','assertion') for r in rows):raise RuntimeError('Probe failure: preserve complete log')
    pairs=[r for r in rows if r['event']=='comparison'];complete=[r for r in rows if r['event']=='complete']
    if len(complete)!=1 or dict(collections.Counter(r['function'] for r in pairs))!=EXPECTED[mode]:raise RuntimeError('Unexpected per-function coverage')
    if not all(r['equal'] for r in pairs):raise RuntimeError('Comparison mismatch')
    done=complete[0];checks=['state_restored','flags_restored','entries_restored','candidates_restored','tables_restored']
    if mode=='expanded':checks.append('collaborator_entries_restored')
    if not all(done.get(key) is True for key in checks) or done['pairs']!=len(pairs):raise RuntimeError('Restoration/count failure')
    if mode=='lifetime':
        if any({r['seed'] for r in pairs if r['function']==f}!=set(range(128)) for f in EXPECTED[mode]):raise RuntimeError('Seed coverage failure')
    else:
        for f,count in EXPECTED[mode].items():
            active=f.startswith(('CTaskComplexGangJoinRespond','CTaskComplexFollowLeaderInFormation'))
            wanted={(seed,child,branch) for seed in range(16) for child in (False,True) for branch in ((False,True) if active else (False,))}
            got={(r['seed'],r['child_nonnull'],r['branch_active']) for r in pairs if r['function']==f}
            if got!=wanted or len(got)!=count:raise RuntimeError('Branch coverage failure for '+f)
        if not all(r['trace_original']==r['trace_candidate']==r['trace_expected'] and r['terminal_vptr_valid'] and r['subtask_cleared'] and r['original_gate'] and r['candidate_gate'] for r in pairs):raise RuntimeError('Trace/entry/vptr evidence failure')
    return {'pairs':len(pairs),'pairs_per_function':EXPECTED[mode],'all_equal':True,'state_restored':True,'log_sha256':runner.digest(path),'scope':done.get('scope','declared limited fixtures')}
 return analyze
def main():
    mode=sys.argv[1]
    if len(sys.argv)==4 and sys.argv[2]=='--analyze':print(json.dumps(analyzer(mode)(Path(sys.argv[3])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text());prefix=PREFIX[mode]
    for p,h in manifest['frozen_inputs'].items():assert runner.digest(ROOT/p)==h,p
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-requested-tasks.{ext}';assert runner.digest(source)==manifest[ext]
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
    try:status=runner.run_case(prefix,manifest['asi'],analyzer(mode),mode,timeout=180,stop_before_analysis=True,unhook='Win/Win/NOTSA_WinMain')
    finally:bank_marker.unlink(missing_ok=True)
    run=max((runner.GAME/f'resultados-{prefix}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/mode/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    if bank_log.exists():shutil.copy2(bank_log,dest/'raw-bank.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('PRESERVED',dest);return status
if __name__=='__main__':raise SystemExit(main())
