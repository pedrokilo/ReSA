import gzip,hashlib,json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-pad-shoulder'

base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyze(log):
    if not runner.terminal_event(log):return None
    rows=[json.loads(line) for line in log.read_text(encoding='utf8').splitlines()]
    if rows[0].get('event')!='start' or rows[-1].get('event')!='complete' or not rows[-1]['state_restored']:raise RuntimeError('Shoulder bank incomplete or unrestored')
    comparisons=[r for r in rows if r.get('event')=='comparison']
    seen=set()
    for r in comparisons:
        identity=(r['function'],r['seed'])
        seed=r['seed']
        if identity in seen or r['function']!='GetLookRight' or r['address']!=0x53FE10 or not r['switch_exact'] or not r['globals_unchanged']:raise RuntimeError('Wrong identity, duplicate, switch or globals failure')
        if (r['mode'],r['disabled'],r['pressure'],r['truth_mask'])!=(seed//128,bool((seed%128)//64),[0,1,128,255][(seed%64)//16],seed%16):raise RuntimeError('Wrong shoulder fixture metadata')
        for side in ('original','candidate'):
            if type(r[side]['return_al_bool']) is not bool or len(r[side]['pad'])!=0x134:raise RuntimeError('Wrong return ABI or private object extent')
        if r['equal']!=(r['original']==r['candidate']):raise RuntimeError('Wrong raw comparison verdict')
        seen.add(identity)
    if rows[0]['cases']!=512 or rows[-1]['cases']!=512 or len(comparisons)!=512 or seen!={('GetLookRight',seed) for seed in range(512)}:raise RuntimeError('Missing shoulder fixture coverage')
    failures=[r for r in comparisons if not r['equal']]
    result={'comparisons':512,'functions':1,'function':'CPad::GetLookRight','address':'0x0053FE10','fixtures_per_function':512,'differences':len(failures),'failure_seeds':[r['seed'] for r in failures],'passed':not failures,'bank_restored':True}
    result['all_equal']=not failures
    harness=runner.GAME/'preflight-pad-shoulder-harness.jsonl'
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
    previous=runner.GAME/'preflight-pad-shoulder-harness.jsonl'
    if previous.exists():
        target=HERE/'artifacts'/('previous-harness-'+runner.digest(previous)+'.jsonl')
        shutil.copy2(previous,target);previous.unlink()
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-preflight-pad-shoulder.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():
            backup=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not backup.exists():shutil.copy2(target,backup)
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'CPad GetLookRight shoulder512',timeout=120,stop_before_analysis=False,unhook='',wait_for_exit=True)
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
    if raw.exists() and raw.stat().st_size>90*1024*1024:
        with raw.open('rb') as source,gzip.open(destination/'comparisons.jsonl.gz','wb') as target:shutil.copyfileobj(source,target)
        archive={'raw_sha256':runner.digest(raw),'raw_bytes':raw.stat().st_size,'gzip_sha256':runner.digest(destination/'comparisons.jsonl.gz'),'lossless':True}
        decoded=hashlib.sha256()
        with gzip.open(destination/'comparisons.jsonl.gz','rb') as source:
            while block:=source.read(1024*1024):decoded.update(block)
        assert decoded.hexdigest()==archive['raw_sha256']
        (destination/'log-archive.json').write_text(json.dumps(archive,indent=2))
        raw.unlink()
    print('Preserved',destination,'passed',report['passed'])
    return status
if __name__=='__main__':raise SystemExit(main())
