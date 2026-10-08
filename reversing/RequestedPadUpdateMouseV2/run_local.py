"""Read-only analyzer plus coordinator-gated local runner. Natural exit mandatory."""
import collections,gzip,hashlib,json,shutil,sys
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1];sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
from model import COUNT,expected,fixture
PREFIX='pad-update-mouse'
REQUIRED=('helper_gates','target_gate','abi_valid','controls_immutable','native_check_immutable','global_pads_immutable','timer_immutable','mouse_guards_immutable','fp_exact')
def analyze(log):
    harness=runner.GAME/'preflight-pad-update-mouse-harness.jsonl'
    if not runner.terminal_event(log) or not runner.terminal_event(harness):return None
    seen=set();pairs=set();errors=[];begin=None;terminal=None;difference=0;models=0
    for line in log.open(encoding='utf8'):
        row=json.loads(line);event=row.get('event')
        if event=='begin':
            if begin is not None:errors.append('duplicate begin')
            begin=row
            if begin.get('helpers_known') is not True:errors.append('helper unresolved')
        elif event=='error':errors.append(row)
        elif event=='complete':
            if terminal is not None:errors.append('duplicate terminal')
            terminal=row
        elif event=='comparison':
            if terminal:errors.append('row after terminal')
            f=row['fixture'];i=f['index'];pair=row['pair']
            if i in seen or pair in pairs or f!=fixture(i) or row['function']!='CPad::UpdateMouse':errors.append({'fixture':i})
            seen.add(i);pairs.add(pair);wanted=expected(f,begin)
            equality=row['original']['payload']==row['candidate']['payload'] and all(row[leg].get(k) is True for leg in ('original','candidate') for k in REQUIRED)
            if row['equal'] is not equality:errors.append({'equal_tag':i})
            difference+=not equality
            for leg in ('original','candidate'):models+=row[leg]['payload']!=wanted
        else:errors.append({'unknown_event':event})
    if seen!=set(range(COUNT)) or pairs!=set(range(COUNT)):errors.append('missing/excess corpus')
    if not terminal or terminal.get('comparisons')!=COUNT or terminal.get('differences')!=difference or terminal.get('all_equal') is not(difference==0) or any(terminal.get(k) is not True for k in ('state_restored','fp_restored')):errors.append('terminal')
    stages=[json.loads(x) for x in harness.read_text(encoding='utf8').splitlines()]
    if [x['event'] for x in stages]!=['preflight_entered','before_task_bank','task_bank_returned','complete']:errors.append('harness sequence')
    end=stages[-1]
    if end.get('snapshot_ranges')!=17692 or end.get('changed_ranges')!=[] or any(end.get(k) is not True for k in ('state_restored','entries_restored','flags_restored','timer_restored','tables_restored')):errors.append('harness restoration')
    return {'passed':not errors and difference==0 and models==0,'comparisons':len(seen),'differences':difference,'model_mismatches':models,'errors':errors,'comparison_sha256':runner.digest(log),'harness_sha256':runner.digest(harness)}
def main():
    freeze=json.loads((HERE/'prepared-freeze.json').read_text(encoding='utf8'));manifest=json.loads((HERE/'artifacts/manifest.json').read_text(encoding='utf8'))
    for p,h in freeze['inputs'].items():assert runner.digest(ROOT/p)==h,p
    for label in ('tested_sources','bank_sources'):
        for p,h in manifest[label].items():assert runner.digest(ROOT/p)==h,p
    proof=json.loads((HERE/'Diagnostics/compiled-runtime-proof.json').read_text(encoding='utf8'));assert proof['passed'] is True and proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb'] and proof['helper_call_paths_verified'] is True and proof['separate_getmouse_ABIs_verified'] is True
    harness=runner.GAME/'preflight-pad-update-mouse-harness.jsonl'
    if harness.exists():
        archive=HERE/'Diagnostics'/('previous-harness-'+runner.digest(harness)+'.jsonl');shutil.copy2(harness,archive);harness.unlink()
    for ext in ('asi','pdb'):
        source=HERE/f'artifacts/gta_reversed-pad-update-mouse.{ext}';assert runner.digest(source)==manifest[ext];shutil.copy2(source,runner.GAME/f'_baseline/gta_reversed-{PREFIX}-probe.{ext}')
    configuration=runner.probe_configuration;runner.probe_configuration=lambda data:configuration(data).replace(b'SaveGameToLoad = 2\n',b'SaveGameToLoad = -2\n')
    try:status=runner.run_case(PREFIX,manifest['asi'],analyze,'PadUpdateMouse9216',timeout=300,stop_before_analysis=False,unhook='',wait_for_exit=True)
    finally:runner.probe_configuration=configuration
    run=max((runner.GAME/f'resultados-{PREFIX}').glob('*/summary.json'),key=lambda p:p.stat().st_mtime_ns).parent;destination=HERE/'runs'/run.name;destination.mkdir(parents=True,exist_ok=True)
    for name in ('comparisons.jsonl','summary.json','RESULTADO.txt'):
        if (run/name).exists():shutil.copy2(run/name,destination/name)
    if harness.exists():shutil.copy2(harness,destination/'harness.jsonl')
    result=json.loads((run/'summary.json').read_text(encoding='utf8'));result.update(manifest=manifest,runner_sha256=runner.digest(Path(__file__)),compiled_callpath_proof=proof,compiled_callpath_proof_sha256=runner.digest(HERE/'Diagnostics/compiled-runtime-proof.json'))
    raw=destination/'comparisons.jsonl'
    if raw.exists() and raw.stat().st_size>90_000_000:
        compressed=destination/'comparisons.jsonl.gz'
        with raw.open('rb') as source,compressed.open('wb') as target:
            with gzip.GzipFile(filename='',mode='wb',fileobj=target,mtime=0) as zipped:shutil.copyfileobj(source,zipped,4*1024*1024)
        decoded=hashlib.sha256()
        with gzip.open(compressed,'rb') as source:
            for block in iter(lambda:source.read(4*1024*1024),b''):decoded.update(block)
        assert decoded.hexdigest()==runner.digest(raw);result['comparison_storage']={'uncompressed_path':raw.relative_to(ROOT).as_posix(),'uncompressed_sha256':runner.digest(raw),'uncompressed_bytes':raw.stat().st_size,'compressed_path':compressed.relative_to(ROOT).as_posix(),'compressed_sha256':runner.digest(compressed),'compression':'gzip','gzip_mtime':0,'lossless_roundtrip_sha256':decoded.hexdigest()};(destination/'.gitignore').write_text('/comparisons.jsonl\n',encoding='utf8')
    (destination/'enriched-summary.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf8');print('PRESERVED',destination,'passed',result['passed']);return status
if __name__=='__main__':raise SystemExit(main())
