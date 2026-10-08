import json,sys,shutil,re
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
PREFIX='requested-group-placer-preflight'
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def analyze(path):
    if not runner.terminal_event(path):return None
    rows=[json.loads(s) for s in path.read_text().splitlines()];end=rows[-1]
    chars=[r for r in rows if r['event']=='characterization']
    expected={0,4,128,252}
    native_proven=len(chars)==4 and {r['origin_low_byte'] for r in chars}==expected and all(r['exact_abi_and_state'] and r['raw_eax']==r['expected_raw_eax'] for r in chars)
    before=[r for r in rows if r['event']=='before_candidate']
    fatal=native_proven and len(before)==1 and before[0]['candidate_gate'] and before[0]['guard_installed'] and end.get('unreachable_guard_observed') and not end.get('candidate_returned',True) and end.get('state_restored') and end.get('iat_restored')
    # Return evidence metadata while passed remains FALSE: this bank must never accredit the function.
    return {'passed':False,'baseline_difference_observed':bool(fatal),'original_characterizations':len(chars),'state_restored':bool(end.get('state_restored')),'candidate_returned':end.get('candidate_returned'),'log_sha256':runner.digest(path),'reason':'valid enum4 source unreachable; guarded fatal reproduction only'}
def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for group in ('tested_sources','bank_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-group-placer-preflight.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'GroupPlacer enum4 baseline fatal',timeout=60,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)),scope='fatalbaseline notapproval',expected_exit_code=3)
    raw=dest/'comparisons.jsonl'
    if raw.exists():
        diagnostic=analyze(raw)
        if diagnostic:report.update(diagnostic)
    exit_match=re.search(r'(?:Cierre normal fallido: |antes de completar \()(\d+)',report.get('error',''))
    if exit_match:report['observed_exit_code']=int(exit_match.group(1))
    report['passed']=False
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('Preserved',dest,'baseline',report.get('baseline_difference_observed'),'exit',report.get('exit_code'));return status
if __name__=='__main__':raise SystemExit(main())
