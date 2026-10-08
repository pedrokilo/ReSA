import json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'))
import runtime_runner as runner
from probe_case import analyzer
PREFIX='requested-geometry-planes-v2'
base_analyze=analyzer({'CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanesUncached':2048})
def analyze(path):
    result=base_analyze(path)
    if result is None:return None
    rows=[json.loads(line) for line in path.read_text().splitlines()]
    proof=[x for x in rows if x['event']=='helper_proof']
    if len(proof)!=1 or not proof[0]['callsite_proof'] or not proof[0]['native_readonly'] or proof[0]['registered_Vector_hooks_activated']:raise RuntimeError('Native helper CALLsite proof failed')
    if proof[0]['normalise_callsite']!=proof[0]['plane_body']+0x7A:raise RuntimeError('Unreviewed CALLsite')
    for x in rows:
        if x['event']!='comparison':continue
        if x['original_adapter_calls']!=0 or x['original_adapter_trace'] or x['candidate_adapter_calls']!=4:raise RuntimeError('Adapter call count failed')
        traces=x['candidate_adapter_trace']
        if len(traces)!=4 or [t['index'] for t in traces]!=list(range(4)):raise RuntimeError('Adapter trace incomplete')
        if any(not t['self_address'] or len(t['input_bytes'])!=12 or len(t['output_bytes'])!=12 for t in traces):raise RuntimeError('Adapter vector ABI trace failed')
        if not all(x[side]['adapter_contract'] and x[side]['control_unchanged'] and x[side]['mxcsr_control_unchanged'] for side in ('original','candidate')):raise RuntimeError('Helper/FP gate failed')
    end=rows[-1]
    for field in ('entries_restored','manual_helper_entries_restored','native_readonly_unchanged','globals_restored','fp_environment_restored','state_restored'):
        if not end[field]:raise RuntimeError('Restore failed '+field)
    return dict(result,native_helper_readonly=True,manual_helper_restored=True,helper_callsite_proven=True,callback_traces_checked=True)
base_config=runner.probe_configuration
runner.probe_configuration=lambda data:base_config(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
def main():
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2));return 0
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    for group in ('tested_sources','bank_sources'):
        for p,h in manifest[group].items():assert runner.digest(ROOT/p)==h
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-geometry-planes-v2.{ext}';assert runner.digest(source)==manifest[ext]
        target=runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():shutil.copy2(target,target.with_name(target.name+'.previous-'+runner.digest(target)))
        shutil.copy2(source,target)
    status=runner.run_case(PREFIX,manifest['asi'],analyze,'Geometry planes2048',timeout=180,stop_before_analysis=False,unhook='',wait_for_exit=True)
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent
    dest=HERE/'runs'/run.name;dest.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,dest/name)
    report=json.loads((run/'summary.json').read_text());report.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)))
    (dest/'enriched-summary.json').write_text(json.dumps(report,indent=2));print('Preserved',dest,'passed',report['passed']);return status
if __name__=='__main__':raise SystemExit(main())
