"""Root-only native replay; offline --analyze never starts GTA."""
import argparse,json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
PREFIX='requested-frontend-radio-v2'
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
    assert rows[0]==dict(event='start',cases=2048,function='CAEFrontendAudioEntity::IsRadioTuneSoundActive',address=0x4DD480)
    assert rows[1]['event']=='fixture' and len(rows[1]['arena'])==524 and len(rows[1]['globals'])==18
    fixture=rows[1];pairs=rows[2:-1];assert len(pairs)==2048
    assert set(fixture)=={'event','arena','globals','fp'}
    assert all(type(v) is int and 0<=v<=255 for v in fixture['arena']+fixture['globals'])
    assert set(fixture['fp'])=={'rounding','exceptions','mxcsr'} and all(type(v) is int for v in fixture['fp'].values())
    for seed,row in enumerate(pairs):
        assert set(row)=={'event','function','pair','case','equal','accepted','original','candidate'}
        assert row['event']=='comparison' and row['pair']==row['case']==seed and row['function']=='CAEFrontendAudioEntity::IsRadioTuneSoundActive'
        bits=seed%16;user=bool(bits&1);code=bool(bits&2);normal=bool(bits&4);paused=bool(bits&8);expected=int(paused if user or code else normal)
        arena=list(fixture['arena']);globals_=list(fixture['globals'])
        arena[:16]=[(seed*31+i)&255 for i in range(16)];arena[-16:]=[(seed*47+i)&255 for i in range(16)]
        arena[16+0x124:16+0x126]=[int(normal),0];arena[16+0x1CC:16+0x1CE]=[int(paused),0]
        globals_[8]=int(code);globals_[9]=int(user)
        expected_leg=dict(raw_al=expected,expected_al=expected,switch_exact=True,arena_unchanged=True,globals_unchanged=True,arena=arena,globals=globals_,fp=fixture['fp'])
        assert row['original']==row['candidate']==expected_leg and row['equal'] is True and row['accepted'] is True
    final=rows[-1];assert final==dict(event='complete',pairs=2048,failures=0,floating_environment_restored=True,state_restored=True,passed=True)
    harness=runner.GAME/'preflight-frontend-radio-v2-harness.jsonl'
    if not runner.terminal_event(harness):return None
    h=[json.loads(x) for x in harness.read_text().splitlines()]
    assert [r['event'] for r in h]==['preflight_entered','before_task_bank','task_bank_returned','complete']
    assert h[-1]['snapshot_ranges']>=17692 and h[-1]['changed_ranges']==[] and all(h[-1][k] is True for k in ('state_restored','entries_restored','flags_restored','floating_environment_restored','timer_unchanged'))
    return dict(passed=True,all_equal=True,pairs=2048,differences=0,functions=1,full_byte_model_checked=True,raw_al_contract_checked=True,state_restored=True,floating_environment_restored=True,harness_restored=True,log_sha256=runner.digest(path),harness_sha256=runner.digest(harness))
def main():
    parser=argparse.ArgumentParser();parser.add_argument('--analyze',type=Path);a=parser.parse_args()
    if a.analyze:print(json.dumps(analyze(a.analyze),indent=2));return
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text());proof_path=HERE/'Diagnostics/constructor-proof.json';proof=json.loads(proof_path.read_text())
    assert proof['reviewed'] is True and proof['default_constructor_has_no_external_side_effects'] is True
    assert proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb']
    for group in ('bank_sources','tested_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    frozen=json.loads((HERE/'prepared-freeze.json').read_text())
    for p,h in frozen.items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-frontend-radio-v2.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'FrontendRadioGetter2048',timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    shutil.copy2(runner.GAME/'preflight-frontend-radio-v2-harness.jsonl',dest/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,constructor_proof_sha256=runner.digest(proof_path),runner_sha256=runner.digest(Path(__file__)))
    if report.get('natural_exit') is not True or report.get('exit_code')!=0:report['passed']=False;status=1
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));return status
if __name__=='__main__':raise SystemExit(main())
