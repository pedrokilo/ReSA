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

PREFIX = 'anim-projectile-pair'
EXPECTED={'InteriorGroup_c::ReferenceAnims':720,'CProjectileInfo::IsProjectileInRange':59904}
diagnostics={}
def analyze(log):
    harness=runner.GAME/'preflight-anim-projectile-pair-harness.jsonl'
    if not runner.terminal_event(log) or not runner.terminal_event(harness):return None
    counts=collections.Counter();pairs=set();fixtures=set();errors=[];differences=0;models=0;terminal=None
    def reject(message):
        if len(errors)<100:errors.append(message)
    with log.open(encoding='utf8') as stream:
        for line in stream:
            x=json.loads(line)
            if x.get('event')!='comparison':
                if x.get('event')!='complete' or terminal is not None:reject('Unexpected terminal event')
                terminal=x;continue
            if terminal is not None:reject('Comparison after completion')
            f=x.get('function');counts[f]+=1;n=x.get('pair')
            if n in pairs:reject('Duplicate pair')
            pairs.add(n);o=x.get('original',{});c=x.get('candidate',{});matched=o==c and all(y.get('gate') is True and y.get('collaborators_valid') is True for y in [o,c]);differences+=x.get('equal') is not True
            if f=='InteriorGroup_c::ReferenceAnims':
                key=(f,x.get('group'),x.get('referenced'),x.get('index'),x.get('loaded'),x.get('poison'))
                matched=matched and len(x.get('model_bytes',[]))==188 and o.get('bytes')==x.get('model_bytes')
            elif f=='CProjectileInfo::IsProjectileInRange':
                key=(f,x.get('slot'),x.get('weapon'),x.get('active'),x.get('matrix'),x.get('destroy'),x.get('fx'),x.get('position'))
                matched=matched and o.get('private_unchanged') is True and o.get('return')==x.get('model_return') and len(x.get('model_info_bytes',[]))==1152 and o.get('info_bytes')==x.get('model_info_bytes')
                for field,width in [('pointer_bytes',128),('projectile_bytes',412),('matrix_bytes',72),('states',32)]:
                    matched=matched and len(o.get(field,[]))==width
            else:key=(f,n);reject('Unknown function');matched=False
            matched=matched and o.get('trace')==x.get('model_trace')
            if key in fixtures:reject('Duplicate fixture')
            fixtures.add(key);models+=not matched
    expected={( 'InteriorGroup_c::ReferenceAnims',g,r,i,l,p) for g in range(3) for r in [0,1,-1] for i in [0,1,127,179] for l in range(5) for p in [0,85,170,255]}
    expected.update({('CProjectileInfo::IsProjectileInRange',slot,weapon,active,matrix,destroy,fx,position) for slot in range(32) for weapon in [0,15,16,17,18,19,20,21,55] for active in [False,True] for matrix in [False,True] for destroy in [False,True] for fx in [False,True] for position in range(13)})
    if fixtures!=expected or pairs!=set(range(60624)) or dict(counts)!=EXPECTED:reject('Incomplete corpus/product/identities')
    if differences or models:reject('Nonzero differential/model mismatch')
    if not terminal or terminal.get('pairs')!=60624 or terminal.get('differences')!=differences or terminal.get('state_restored') is not True:reject('Bank completion/restoration failed')
    rows=[json.loads(line) for line in harness.read_text(encoding='utf8').splitlines()];last=rows[-1] if rows else {}
    if [x.get('event') for x in rows]!=['preflight_entered','before_task_bank','task_bank_returned','complete']:reject('Wrong preflight sequence')
    if not all(last.get(k) is True for k in ['state_restored','entries_restored','tables_restored','flags_restored','timer_restored']) or last.get('snapshot_ranges',0)<17692 or last.get('changed_ranges')!=[]:reject('Harness restoration failed')
    diagnostics.update(pairs=sum(counts.values()),pairs_per_function=dict(counts),differences=differences,model_mismatches=models,validation_errors=errors,terminal_bank_record=terminal,terminal_harness_record=last)
    return dict(passed=not errors,all_equal=not differences and not models,**diagnostics,log_sha256=runner.digest(log),harness_sha256=runner.digest(harness),scope='Finite private animation/projectile ABI fields, isolated traced collaborators; no real object lifecycle/helper accreditation')

def main():
    frozen=json.loads((HERE/'prepared-freeze.json').read_text(encoding='utf8'))
    for name,expected in frozen['inputs'].items():assert runner.digest(ROOT/name)==expected,name
    manifest = json.loads((HERE / 'artifacts/manifest.json').read_text(encoding='utf-8'))
    for label in ('tested_sources', 'bank_sources'):
        for path, expected in manifest[label].items():
            assert runner.digest(ROOT / path) == expected, path
    harness = runner.GAME / 'preflight-anim-projectile-pair-harness.jsonl'
    if harness.exists():
        shutil.copy2(harness, HERE / 'artifacts' / ('previous-harness-' + runner.digest(harness) + '.jsonl'))
        harness.unlink()
    for ext in ('asi', 'pdb'):
        source = HERE / f'artifacts/gta_reversed-anim-projectile-pair.{ext}'
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
        status = runner.run_case(PREFIX, manifest['asi'], analyze, 'Anim/projectile pair60624',
                                 timeout=600, stop_before_analysis=False, unhook='', wait_for_exit=True)
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
