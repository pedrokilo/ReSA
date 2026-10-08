"""Root-only native replay; offline --analyze never starts GTA."""
import argparse,json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
PREFIX='requested-pools-atomic-check'
def strict_json(value):
    def pairs(items):
        out={}
        for key,v in items:
            if key in out:raise ValueError('Duplicate key')
            out[key]=v
        return out
    return json.loads(value,object_pairs_hook=pairs,parse_constant=lambda x:(_ for _ in ()).throw(ValueError('Nonfinite JSON')))
base_configuration=runner.probe_configuration
runner.probe_configuration=lambda data:base_configuration(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyze(path):
    if not runner.terminal_event(path):return None
    rows=[strict_json(x) for x in path.read_text().splitlines()]
    assert rows[0]==dict(event='start',cases=256,function='CPools::CheckBuildingAtomics',address=0x550170)
    fixture=rows[1];assert fixture['event']=='fixture' and len(fixture['pool_globals'])==68 and all(type(x) is int and 0<=x<=255 for x in fixture['pool_globals'])
    assert set(fixture)=={'event','pool_globals','fp'} and set(fixture['fp'])=={'rounding','exceptions','mxcsr'} and all(type(x) is int for x in fixture['fp'].values())
    pairs=rows[2:-1];assert len(pairs)==256
    for seed,row in enumerate(pairs):
        assert row['event']=='comparison' and row['pair']==row['case']==seed and row['eax_seed']==(seed*0x9E3779B9)&0xffffffff and row['function']=='CPools::CheckBuildingAtomics'
        assert set(row)=={'event','function','pair','case','eax_seed','equal','accepted','original','candidate'}
        expected=dict(returned=0xffffffff,stack_balanced=True,callee_saved=True,fp=fixture['fp'],switch_exact=True,guard=[(seed*47+i*29)&255 for i in range(64)],guard_unchanged=True,pool_globals=fixture['pool_globals'],globals_unchanged=True)
        assert row['original']==row['candidate']==expected and row['accepted'] is True and row['equal'] is True
    assert rows[-1]==dict(event='complete',pairs=256,failures=0,state_restored=True,floating_environment_restored=True,passed=True)
    harness=runner.GAME/'preflight-pools-atomic-check-harness.jsonl'
    if not runner.terminal_event(harness):return None
    h=[strict_json(x) for x in harness.read_text().splitlines()];assert [r['event'] for r in h]==['preflight_entered','before_task_bank','task_bank_returned','complete']
    assert h[-1]['snapshot_ranges']>=17692 and h[-1]['changed_ranges']==[] and all(h[-1][k] is True for k in ('state_restored','entries_restored','flags_restored','floating_environment_restored','timer_unchanged'))
    return dict(passed=True,pairs=256,differences=0,all_equal=True,state_restored=True,harness_restored=True,floating_environment_restored=True,exact_return_and_ABI_checked=True,log_sha256=runner.digest(path),harness_sha256=runner.digest(harness))
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--analyze',type=Path);a=parser.parse_args()
    if a.analyze:print(json.dumps(analyze(a.analyze),indent=2));return
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for group in ('bank_sources','tested_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    frozen=json.loads((HERE/'prepared-freeze.json').read_text())
    for p,h in frozen.items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-pools-atomic-check.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'PoolsAtomicCheck256',timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    shutil.copy2(runner.GAME/'preflight-pools-atomic-check-harness.jsonl',dest/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    if report.get('natural_exit') is not True or report.get('exit_code')!=0:report['passed']=False;status=1
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));return status
if __name__=='__main__':raise SystemExit(main())
