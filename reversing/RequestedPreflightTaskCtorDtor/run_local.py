import json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-task-ctor-dtor'
import runpy
base_analyze=runpy.run_path(str(ROOT/'reversing/RequestedParallelReplay/TaskCtorDtor/run_local.py'))['analyze']
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyze(log):
    result=base_analyze(log)
    if result is None:return None
    harness=runner.GAME/'preflight-task-ctor-dtor-harness.jsonl'
    if not runner.terminal_event(harness):return None
    rows=[json.loads(line) for line in harness.read_text(encoding='utf8').splitlines()]
    final=rows[-1]
    if final['event']!='complete' or not final['state_restored']:raise RuntimeError('Preflight harness failed restoration')
    if [r['event'] for r in rows]!=['preflight_entered','before_task_bank','task_bank_returned','complete']:raise RuntimeError('Unexpected preflight stages')
    return dict(result,harness_restored=True,harness_sha256=runner.digest(harness))
def main():
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for p,h in manifest['tested_sources'].items():assert runner.digest(ROOT/p)==h
    for p,h in manifest['bank_sources'].items():assert runner.digest(ROOT/p)==h
    previous=runner.GAME/'preflight-task-ctor-dtor-harness.jsonl'
    if previous.exists():
        target=HERE/'artifacts'/('previous-harness-'+runner.digest(previous)+'.jsonl')
        shutil.copy2(previous,target);previous.unlink()
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-preflight-task-ctor-dtor.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():
            backup=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not backup.exists():shutil.copy2(target,backup)
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'TaskCtorDtor preflight1536',timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    destination=HERE/'runs'/run.name;destination.mkdir(parents=True,exist_ok=True)
    for filename in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/filename).exists():shutil.copy2(run/filename,destination/filename)
    if previous.exists():shutil.copy2(previous,destination/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    (destination/'enriched-summary.json').write_text(json.dumps(report,indent=2))
    print('Preserved',destination,'passed',report['passed'])
    return status
if __name__=='__main__':raise SystemExit(main())
