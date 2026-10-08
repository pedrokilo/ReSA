"""Stream-audit the prepared pair bank; preserve failures and natural exit evidence."""
import collections
import gzip
import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'reversing'))
import runtime_runner as runner

PREFIX = 'reference-anims-corrected'
EXPECTED={'InteriorGroup_c::ReferenceAnims':720}
diagnostics={}
def analyze(log):
    harness=runner.GAME/'preflight-reference-anims-corrected-harness.jsonl'
    if not runner.terminal_event(log) or not runner.terminal_event(harness):return None
    pairs=set();fixtures=set();errors=[];differences=0;models=0;final=None
    def reject(message):
        if len(errors)<100:errors.append(message)
    with log.open(encoding='utf8') as stream:
        for line in stream:
            x=json.loads(line)
            if x.get('event')!='comparison':
                if x.get('event')!='complete' or final is not None:reject('Unexpected bank event')
                final=x;continue
            if final is not None:reject('Comparison after complete')
            if x.get('function')!='InteriorGroup_c::ReferenceAnims':reject('Unscoped target')
            n=x.get('pair');key=tuple(x.get(k) for k in ['group','referenced','index','loaded','poison'])
            if n in pairs or key in fixtures:reject('Duplicate pair/fixture')
            pairs.add(n);fixtures.add(key);o=x.get('original',{});c=x.get('candidate',{});matched=o==c and all(y.get('gate') is True and y.get('collaborators_valid') is True for y in [o,c])
            model=[key[4]]*188;model[14]=key[0];model[186]=key[1]&255;calls=[]
            if key[1]==0:
                calls.append({'event':'anim_index','name':['int_house','int_shop','int_office'][key[0]],'valid':True,'index':key[2]})
                if key[3]==1:model[186]=1;calls.append({'event':'anim_ref','index':key[2],'valid':True})
                else:calls.append({'event':'request_model','model':key[2]+25575,'flags':8,'valid':True})
            matched=matched and x.get('model_bytes')==o.get('bytes')==model and x.get('model_trace')==o.get('trace')==calls
            differences+=x.get('equal') is not True;models+=not matched
    expected={(g,r,i,l,p) for g in range(3) for r in [0,1,-1] for i in [0,1,127,179] for l in range(5) for p in [0,85,170,255]}
    if pairs!=set(range(720)) or fixtures!=expected:reject('Incomplete720-product')
    if differences or models:reject('Nonzero differential/model mismatch')
    if not final or final.get('pairs')!=720 or final.get('differences')!=differences or final.get('state_restored') is not True:reject('Bank restoration/count failed')
    rows=[json.loads(line) for line in harness.read_text(encoding='utf8').splitlines()];last=rows[-1] if rows else {}
    if [x.get('event') for x in rows]!=['preflight_entered','before_task_bank','task_bank_returned','complete']:reject('Wrong preflight sequence')
    if not all(last.get(k) is True for k in ['state_restored','entries_restored','tables_restored','flags_restored','timer_restored']) or last.get('snapshot_ranges',0)<17692 or last.get('changed_ranges')!=[]:reject('Harness restoration failed')
    diagnostics.update(pairs=len(pairs),pairs_per_function=EXPECTED,differences=differences,model_mismatches=models,validation_errors=errors,terminal_bank_record=final,terminal_harness_record=last)
    return dict(passed=not errors,all_equal=not differences and not models,state_restored=bool(final and final.get('state_restored')),harness_restored=last.get('state_restored') is True,**diagnostics,log_sha256=runner.digest(log),harness_sha256=runner.digest(harness),scope='Identical original720 private reference fixtures and isolated lookup/ref/request traces; raw loaded byte ==1. No actual assets or helper lifecycle/dependency accreditation')

def main():
    frozen=json.loads((HERE/'prepared-freeze.json').read_text(encoding='utf8'))
    for name,expected in frozen['inputs'].items():assert runner.digest(ROOT/name)==expected,name
    manifest = json.loads((HERE / 'artifacts/manifest.json').read_text(encoding='utf-8'))
    for label in ('tested_sources', 'bank_sources'):
        for path, expected in manifest[label].items():
            assert runner.digest(ROOT / path) == expected, path
    harness = runner.GAME / 'preflight-reference-anims-corrected-harness.jsonl'
    if harness.exists():
        shutil.copy2(harness, HERE / 'artifacts' / ('previous-harness-' + runner.digest(harness) + '.jsonl'))
        harness.unlink()
    for ext in ('asi', 'pdb'):
        source = HERE / f'artifacts/gta_reversed-reference-anims-corrected.{ext}'
        assert runner.digest(source) == manifest[ext]
        target = runner.GAME / f'_baseline/gta_reversed-{PREFIX}-probe.{ext}'
        if target.exists():
            previous = target.with_name(target.name + '.previous-' + runner.digest(target))
            if not previous.exists():
                shutil.copy2(target, previous)
        shutil.copy2(source, target)
    configuration = runner.probe_configuration
    runner.probe_configuration = lambda data: configuration(data).replace(b'SaveGameToLoad = 2\n', b'SaveGameToLoad = -2\n')
    try:
        status = runner.run_case(PREFIX, manifest['asi'], analyze, 'ReferenceAnims720',
                                 timeout=120, stop_before_analysis=False, unhook='', wait_for_exit=True)
    finally:
        runner.probe_configuration = configuration
    run = max((runner.GAME / f'resultados-{PREFIX}').glob('*/summary.json'),
              key=lambda p: p.stat().st_mtime_ns).parent
    destination = HERE / 'runs' / run.name
    destination.mkdir(parents=True, exist_ok=True)
    for name in ('comparisons.jsonl', 'summary.json', 'RESULTADO.txt'):
        if (run / name).exists():
            shutil.copy2(run / name, destination / name)
    if harness.exists():
        shutil.copy2(harness, destination / 'harness.jsonl')
    result = json.loads((run / 'summary.json').read_text(encoding='utf-8'))
    result.update(manifest=manifest, runner_sha256=runner.digest(Path(__file__)),
                  reproduction_diagnostics=diagnostics)
    raw = destination / 'comparisons.jsonl'
    if raw.exists() and raw.stat().st_size > 90_000_000:
        compressed = destination / 'comparisons.jsonl.gz'
        with raw.open('rb') as source, compressed.open('wb') as target:
            with gzip.GzipFile(filename='', mode='wb', fileobj=target, mtime=0) as zipped:
                shutil.copyfileobj(source, zipped, length=4 * 1024 * 1024)
        decoded = hashlib.sha256()
        with gzip.open(compressed, 'rb') as source:
            for block in iter(lambda: source.read(4 * 1024 * 1024), b''):
                decoded.update(block)
        raw_hash = runner.digest(raw)
        assert decoded.hexdigest() == raw_hash, 'Lossless compression verification failed'
        (destination / '.gitignore').write_text('# Full raw log preserved locally; deterministic gzip is the tracked lossless evidence.\n/comparisons.jsonl\n', encoding='utf-8')
        result['comparison_storage'] = dict(uncompressed_path=raw.relative_to(ROOT).as_posix(),
                                            uncompressed_sha256=raw_hash,
                                            uncompressed_bytes=raw.stat().st_size,
                                            compressed_path=compressed.relative_to(ROOT).as_posix(),
                                            compressed_sha256=runner.digest(compressed),
                                            compression='gzip', gzip_mtime=0,
                                            lossless_roundtrip_sha256=decoded.hexdigest())

    (destination / 'enriched-summary.json').write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
    print('PRESERVED', destination, 'passed', result['passed'], 'errors', diagnostics.get('validation_errors'))
    return status

if __name__ == '__main__':
    raise SystemExit(main())
