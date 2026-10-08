import json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-pad-selectors'

base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyze(log):
    if not runner.terminal_event(log):return None
    rows=[json.loads(line) for line in log.read_text(encoding='utf8').splitlines()]
    if rows[0].get('event')!='start' or rows[-1].get('event')!='complete' or not rows[-1]['state_restored']:raise RuntimeError('CPad bank incomplete or unrestored')
    comparisons=[r for r in rows if r.get('event')=='comparison']
    scope=json.loads((HERE/'scope.json').read_text())
    expected={r['fn_name']:int(r['address'],16) for r in scope['entries'] if r['phase']=='pure'}
    seen=set()
    for r in comparisons:
        identity=(r['function'],r['seed'])
        if identity in seen or expected.get(r['function'])!=r['address'] or not r['switch_exact']:raise RuntimeError('Wrong identity, duplicate case or switch failure')
        seen.add(identity)
    count=rows[0]['fixtures_per_function']
    if len(comparisons)!=52*count or seen!={(f,seed) for f in expected for seed in range(count)}:raise RuntimeError('Missing CPad fixture coverage')
    failures=[r for r in comparisons if not r['equal']]
    result={'comparisons':len(comparisons),'functions':52,'fixtures_per_function':count,'differences':len(failures),'failures_by_function':{f:sum(r['function']==f for r in failures) for f in expected},'passed':not failures,'bank_restored':True}
    result['all_equal']=not failures
    harness=runner.GAME/'preflight-pad-selectors-harness.jsonl'
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
    previous=runner.GAME/'preflight-pad-selectors-harness.jsonl'
    if previous.exists():
        target=HERE/'artifacts'/('previous-harness-'+runner.digest(previous)+'.jsonl')
        shutil.copy2(previous,target);previous.unlink()
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-preflight-pad-selectors.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():
            backup=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not backup.exists():shutil.copy2(target,backup)
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'CPad selectors preflight201552',timeout=600,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    destination=HERE/'runs'/run.name;destination.mkdir(parents=True,exist_ok=True)
    for filename in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/filename).exists():shutil.copy2(run/filename,destination/filename)
    if previous.exists():shutil.copy2(previous,destination/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    if report.get('passed') and (report.get('differences',1)!=0 or report.get('all_equal') is not True):
        report['passed']=False;report['error']='Invalid approval: explicit zero differences and all_equal required';status=1
    (destination/'enriched-summary.json').write_text(json.dumps(report,indent=2))
    print('Preserved',destination,'passed',report['passed'])
    return status
if __name__=='__main__':raise SystemExit(main())
