"""Preserve successful completion or full mismatch reproduction; never approve mismatches."""
import collections,json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
PREFIX='additional-lifetime';EXPECTED={'CTaskSimpleFightingControl::Constructor':256,'CTaskComplexProstituteSolicit::Constructor':128,'CTaskSimpleFightingControl::Destructor':32,'CTaskComplexProstituteSolicit::Destructor':256}
diagnostics={};launched=[]
def parse_bank(log):
    rows=[json.loads(line) for line in log.read_text().splitlines()];pairs=[r for r in rows if r['event']=='comparison'];final=rows[-1]
    count=dict(collections.Counter(r['function'] for r in pairs));failures=[r for r in pairs if not r['equal']]
    diagnostics.update(pairs=len(pairs),pairs_per_function=count,mismatches=len(failures),mismatches_per_function=dict(collections.Counter(r['function'] for r in failures)),differing_payload_offsets=dict(collections.Counter(str(offset) for r in failures for offset in r.get('differing_payload_offsets',[]))),terminal_bank_record=final)
    if {r['pair'] for r in pairs}!=set(range(672)) or count!=EXPECTED:raise RuntimeError('Incomplete672 bank coverage')
    if not all(final.get(key) is True for key in ('state_restored','flags_restored','entries_restored','candidates_restored','tables_restored','collaborator_entries_restored')):raise RuntimeError('Frozen bank restoration failure')
    return rows,pairs,failures,final
def analyze(log):
    harness=runner.GAME/'preflight-additional-harness.jsonl'
    if not runner.terminal_event(log) or not runner.terminal_event(harness):return None
    rows,pairs,failures,final=parse_bank(log);hrows=[json.loads(line) for line in harness.read_text().splitlines()];hfinal=hrows[-1]
    diagnostics.update(terminal_harness_record=hfinal,harness_sha256=runner.digest(harness))
    if [r['event'] for r in hrows]!=['preflight_entered','crt_assertion_snapshot','before_lifetime_bank','lifetime_bank_returned','crt_assertion_after','complete'] or not all(hfinal.get(key) is True for key in ('state_restored','entries_restored','tables_restored','flags_restored','crt_restored')):raise RuntimeError('Preflight harness restoration failure')
    snapshot=hrows[1];after=hrows[4]
    if snapshot.get('owned_span_bytes')!=5 or snapshot.get('diagnostic_span_bytes')!=52 or not snapshot.get('module_found') or snapshot.get('virtual_query_bytes')!=28:raise RuntimeError('CRT diagnostic metadata failure')
    before_hex=snapshot.get('before_hex52','');after_hex=after.get('after_hex52','')
    if len(before_hex)!=104 or before_hex!=after_hex or after.get('owned_entry_restored') is not True or after.get('owned_before_hex5')!=before_hex[:10] or after.get('owned_after_hex5')!=after_hex[:10] or hfinal.get('crt_assertion_restored') is not True or hfinal.get('crt_assertion_changed_offsets')!=[]:raise RuntimeError('CRT exact52/owned5 restoration failure')
    # Wait for actual natural process completion before rejecting mismatches. This
    # prevents the shared runner's failure cleanup from terminating a valid return0.
    if not launched:raise RuntimeError('Process identity unavailable')
    code=launched[-1].wait(timeout=30);diagnostics.update(natural_exit=True,exit_code=code)
    if code!=0:raise RuntimeError('Preflight natural exit is not0: '+str(code))
    if failures:raise RuntimeError(f'Reproduced{len(failures)} mismatches; full bytes, offsets, traces and terminal restoration preserved')
    if final['event']!='complete' or any(r['event'] in ('error','assertion') for r in rows):raise RuntimeError('Frozen bank did not approve')
    if not all(r['trace_original']==r['trace_candidate']==r['trace_expected'] and r['original_gate'] and r['candidate_gate'] and r['vptr_valid'] for r in pairs):raise RuntimeError('Trace/entry/vptr failure')
    return dict(pairs=672,pairs_per_function=EXPECTED,all_equal=True,state_restored=True,harness_restored=True,harness_sha256=runner.digest(harness),log_sha256=runner.digest(log),scope=final['scope'])
def main():
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for label in ('tested_sources','bank_sources'):
        for path,h in manifest[label].items():assert runner.digest(ROOT/path)==h,path
    harness=runner.GAME/'preflight-additional-harness.jsonl'
    if harness.exists():shutil.copy2(harness,HERE/'artifacts'/('previous-harness-'+runner.digest(harness)+'.jsonl'));harness.unlink()
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-preflight-additional.{ext}';assert runner.digest(source)==manifest[ext];target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        assert target.resolve().is_relative_to((runner.GAME/'_baseline').resolve())
        if target.exists():
            previous=target.with_name(target.name+'.previous-'+runner.digest(target))
            if not previous.exists():shutil.copy2(target,previous)
        shutil.copy2(source,target)
    original_cfg=runner.probe_configuration;runner.probe_configuration=lambda data:original_cfg(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
    original_popen=runner.subprocess.Popen
    def tracked_popen(args,*a,**kw):
        process=original_popen(args,*a,**kw)
        if isinstance(args,(tuple,list)) and str(args[0]).lower()==str(runner.GAME/'gta_sa.exe').lower():launched.append(process)
        return process
    runner.subprocess.Popen=tracked_popen
    try:status=runner.run_case(PREFIX,manifest['asi'],analyze,'Additional lifetime preflight672',timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    finally:runner.subprocess.Popen=original_popen
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent;dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    if harness.exists():shutil.copy2(harness,dest/'harness.jsonl')
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)),reproduction_diagnostics=diagnostics)
    if launched:report['observed_process_exit_code']=launched[-1].poll()
    if diagnostics.get('natural_exit'):report.update(natural_exit=True,exit_code=diagnostics['exit_code'])
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('PRESERVED',dest,'passed',report['passed'],'diagnostics',diagnostics);return status
if __name__=='__main__':raise SystemExit(main())
