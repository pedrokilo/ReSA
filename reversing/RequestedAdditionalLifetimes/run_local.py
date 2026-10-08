import collections,json,os,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
PREFIX='additional-lifetime';EXPECTED={'CTaskSimpleFightingControl::Constructor':256,'CTaskComplexProstituteSolicit::Constructor':128,'CTaskSimpleFightingControl::Destructor':32,'CTaskComplexProstituteSolicit::Destructor':256}
def analyze(path):
    if not runner.terminal_event(path):return None
    rows=[json.loads(line) for line in path.read_text().splitlines()];pairs=[r for r in rows if r['event']=='comparison'];final=rows[-1]
    if {r['pair'] for r in pairs}!=set(range(672)) or dict(collections.Counter(r['function'] for r in pairs))!=EXPECTED:raise RuntimeError('Coverage incomplete')
    failures=[r for r in pairs if not r['equal']]
    if failures:raise RuntimeError(f'{len(failures)} mismatches: preserve full bytes and trace outputs')
    if any(r['event'] in ('error','assertion') for r in rows):raise RuntimeError('Probe runtime failure')
    if final['event']!='complete' or final['pairs']!=672 or not all(final.get(k) is True for k in ('state_restored','flags_restored','entries_restored','candidates_restored','tables_restored','collaborator_entries_restored')):raise RuntimeError('Restoration failure')
    if not all(r['trace_original']==r['trace_candidate']==r['trace_expected'] and r['original_gate'] and r['candidate_gate'] and r['vptr_valid'] for r in pairs):raise RuntimeError('Trace or selected-target evidence failure')
    return dict(pairs=672,pairs_per_function=EXPECTED,all_equal=True,state_restored=True,log_sha256=runner.digest(path),scope=final['scope'])
def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for p,h in manifest['frozen_inputs'].items():assert runner.digest(ROOT/p)==h,p
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-additional-lifetime.{ext}';assert runner.digest(source)==manifest[ext]
        immutable=HERE/f'artifacts/{manifest[ext]}.{ext}'
        if not immutable.exists():shutil.copy2(source,immutable)
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}';assert target.resolve().is_relative_to((runner.GAME/'_baseline').resolve())
        if target.exists():
            previous=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not previous.exists():target.replace(previous)
            else:target.unlink()
        try:os.link(immutable,target)
        except OSError:shutil.copy2(immutable,target)
    original_cfg=runner.probe_configuration;runner.probe_configuration=lambda data:original_cfg(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
    status=runner.run_case(PREFIX,manifest['asi'],analyze,PREFIX,timeout=180,stop_before_analysis=True,unhook='Win/Win/NOTSA_WinMain')
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent;dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)));(dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('PRESERVED',dest);return status
if __name__=='__main__':raise SystemExit(main())
