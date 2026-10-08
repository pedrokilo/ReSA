import importlib.util,json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
spec=importlib.util.spec_from_file_location('frozen_task_analyzer',ROOT/'reversing/RequestedParallelTasks/run_local.py')
bank=importlib.util.module_from_spec(spec);spec.loader.exec_module(bank)
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyzer(mode):
    base=bank.analyzer(mode)
    def analyze(log):
        result=base(log)
        if result is None:return None
        harness=runner.GAME/'preflight-tasks-harness.jsonl'
        if not runner.terminal_event(harness):return None
        rows=[json.loads(line) for line in harness.read_text(encoding='utf8').splitlines()];final=rows[-1]
        if final['event']!='complete' or not final['state_restored'] or not final['timer_unchanged']:raise RuntimeError('Preflight restoration/timer failure')
        if [r['event'] for r in rows]!=['preflight_entered','timer_fixture','before_task_bank','task_bank_returned','complete']:raise RuntimeError('Unexpected preflight stages')
        return dict(result,harness_restored=True,timer_unchanged=True,harness_sha256=runner.digest(harness),mapped_timer_fixture=rows[1]['mapped_timer_bytes'])
    return analyze
def main():
    mode=sys.argv[1];prefix=bank.PREFIX[mode]
    if len(sys.argv)==4 and sys.argv[2]=='--analyze':print(json.dumps(analyzer(mode)(Path(sys.argv[3])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for p,h in manifest['tested_sources'].items():assert runner.digest(ROOT/p)==h
    for p,h in manifest['bank_sources'].items():assert runner.digest(ROOT/p)==h
    harness=runner.GAME/'preflight-tasks-harness.jsonl'
    if harness.exists():shutil.copy2(harness,HERE/'artifacts'/('previous-harness-'+runner.digest(harness)+'.jsonl'));harness.unlink()
    for frozen_prefix in bank.PREFIX.values():
        assert not (runner.GAME/(frozen_prefix+'.start')).exists(),'Preexisting bank marker'
        assert not (runner.GAME/(frozen_prefix+'-runtime.start')).exists(),'Preexisting runtime marker'
    raw=runner.GAME/(prefix+'.jsonl')
    if raw.exists():shutil.copy2(raw,HERE/'artifacts'/('previous-bank-'+runner.digest(raw)+'.jsonl'));raw.unlink()
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-preflight-tasks.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{prefix}-probe.{ext}'
        if target.exists():
            backup=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not backup.exists():shutil.copy2(target,backup)
        shutil.copy2(source,target)
    marker=runner.GAME/(prefix+'.start')
    try:status=runner.run_case(prefix,manifest['asi'],analyzer(mode),'Tasks preflight '+mode,timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    finally:marker.unlink(missing_ok=True)
    run=max((runner.GAME/f'resultados-{prefix}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    destination=HERE/'runs'/mode/run.name;destination.mkdir(parents=True,exist_ok=True)
    for filename in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/filename).exists():shutil.copy2(run/filename,destination/filename)
    if raw.exists():shutil.copy2(raw,destination/'raw-bank.jsonl')
    if harness.exists():shutil.copy2(harness,destination/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    (destination/'enriched-summary.json').write_text(json.dumps(report,indent=2))
    print('Preserved',destination,'passed',report['passed']);return status
if __name__=='__main__':raise SystemExit(main())
