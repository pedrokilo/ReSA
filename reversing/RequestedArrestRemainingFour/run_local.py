"""Reject missing, duplicated, unequal, model-divergent or incompletely restored cases."""
import collections,hashlib,json,sys,shutil,gzip
from pathlib import Path
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
sys.path.insert(0,str(ROOT/'reversing'));import runtime_runner as runner
from model import COUNTS,expected,validate_fixture
PREFIX="arrest-remaining-four"
diagnostics={}
def analyze(log):
    harness=runner.GAME/'preflight-arrest-remaining-four-harness.jsonl'
    if not runner.terminal_event(log) or not runner.terminal_event(harness):return None
    counts=collections.Counter();ids=set();pairs=set();errors=[];differences=0;model_failures=0;terminal=None;begin=None
    for line in log.open(encoding='utf-8'):
        row=json.loads(line)
        if row.get('event')=='begin':begin=row
        if row.get('event')=='error':errors.append(row)
        if row.get('event')=='complete':terminal=row
        if row.get('event')!='comparison':continue
        name=row['function'].split('::')[1];f=row['fixture'];key=(name,f['index']);pair=row['pair']
        if key in ids or pair in pairs:errors.append({'duplicate':key})
        ids.add(key);pairs.add(pair);counts[name]+=1;validate_fixture(name,f)
        differences+=not row['equal']
        for leg in ('original','candidate'):
            value=row[leg];model=expected(name,f,value);actual=value['payload']
            table_key=('native_' if leg=='original' else 'candidate_')+('task_table' if name=='Destructor' else 'arrest_table')
            if not begin or value['raw_vptr']!=begin[table_key] or value['expected_vptr']!=begin[table_key]:errors.append({'vptr_role':key,'leg':leg})
            model_failures+=any(actual[k]!=v for k,v in model.items())
            if not all(value[k] is True for k in ('vptr_role_exact','helper_gates','target_gate','private_immutable')):errors.append({'bad_gate':key,'leg':leg})
    if dict(counts)!=COUNTS or pairs!=set(range(sum(COUNTS.values()))):errors.append({'coverage':dict(counts)})
    if not terminal or terminal.get('comparisons')!=2816 or terminal.get('differences')!=differences or not all(terminal.get(k) is True for k in ('all_equal','state_restored','fp_restored')):errors.append({'terminal':terminal})
    stages=[json.loads(x) for x in harness.read_text(encoding='utf-8').splitlines()]
    if [x['event'] for x in stages]!=['preflight_entered','before_task_bank','task_bank_returned','complete']:errors.append({'harness_sequence':stages})
    end=stages[-1]
    if not all(end.get(k) is True for k in ('state_restored','entries_restored','flags_restored','timer_restored','tables_restored')) or end.get('changed_ranges')!=[] or end.get('snapshot_ranges')!=17692:errors.append({'harness_restore':end})
    return {'passed':not errors and differences==0 and model_failures==0,'comparisons':sum(counts.values()),'counts':dict(counts),'differences':differences,'model_mismatches':model_failures,'errors':errors,'harness_sha256':runner.digest(harness),'comparison_sha256':runner.digest(log)}
def main():
    frozen=json.loads((HERE/'prepared-freeze.json').read_text(encoding='utf8'))
    for name,expected in frozen['inputs'].items():assert runner.digest(ROOT/name)==expected,name
    manifest = json.loads((HERE / 'artifacts/manifest.json').read_text(encoding='utf-8'))
    for label in ('tested_sources', 'bank_sources'):
        for path, expected in manifest[label].items():
            assert runner.digest(ROOT / path) == expected, path
    harness = runner.GAME / 'preflight-arrest-remaining-four-harness.jsonl'
    if harness.exists():
        shutil.copy2(harness, HERE / 'artifacts' / ('previous-harness-' + runner.digest(harness) + '.jsonl'))
        harness.unlink()
    for ext in ('asi', 'pdb'):
        source = HERE / f'artifacts/gta_reversed-arrest-remaining-four.{ext}'
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
        status = runner.run_case(PREFIX, manifest['asi'], analyze, 'ArrestRemainingFour2816',
                                 timeout=300, stop_before_analysis=False, unhook='', wait_for_exit=True)
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
