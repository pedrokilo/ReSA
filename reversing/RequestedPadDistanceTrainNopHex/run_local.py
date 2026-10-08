"""Stream-audit the prepared pair bank; preserve failures and natural exit evidence."""
import collections
import gzip
import hashlib
import json
import shutil
import sys
if not __debug__:raise RuntimeError("Strict bank requires Python assertions enabled")
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / 'reversing'))
import runtime_runner as runner

PREFIX = 'pad-distance-train-nop-hex'
from oracle import EXPECTED, KEYS, expected_row
from hex_wire import strict_loads,decode_row
diagnostics = {}

def analyze(log):
    harness = runner.GAME / 'preflight-pad-distance-train-nop-hex-harness.jsonl'
    if not runner.terminal_event(log) or not runner.terminal_event(harness):
        return None
    counts = collections.Counter()
    pairs = set()
    fixture_ids = set()
    failures = 0
    model_failures = 0
    errors = []
    final = None
    def reject(message):
        if len(errors)<100:errors.append(message)
    with log.open(encoding='utf-8') as stream:
        for line_number,line in enumerate(stream,1):
            row=decode_row(strict_loads(line))
            if row.get('event')!='comparison':
                if row.get('event')!='complete' or final is not None:reject('Unexpected bank event '+str(line_number))
                final=row;continue
            if final is not None:reject('Comparison after terminal record')
            key=row.get('function');case=row.get('case');pair=row.get('pair')
            counts[key]+=1
            if type(pair) is not int or pair in pairs:reject('Invalid/duplicate pair')
            pairs.add(pair);fixture=(key,case)
            if fixture in fixture_ids:reject('Duplicate fixture')
            fixture_ids.add(fixture)
            failures+=row.get('equal') is not True
            try:
                model=expected_row(row)
                matched=row.get('original')==row.get('candidate')==model
                if not matched:model_failures+=1
            except (AssertionError,KeyError,ValueError,TypeError):
                model_failures+=1;reject('Independent oracle rejected pair '+str(pair))
    if dict(counts)!=EXPECTED or pairs!=set(range(13952)):reject('Incomplete/extra pairs')
    if fixture_ids!={(k,i) for k,n in EXPECTED.items() for i in range(n)}:reject('Incomplete fixture product')
    if final is None or final.get('event')!='complete' or final.get('pairs')!=13952:reject('No exact complete event')
    final=final or {}
    if final.get('differences')!=failures or failures or model_failures:reject('Nonzero differences or oracle mismatch')
    if not all(final.get(k) is True for k in ('state_restored','entries_restored','collaborator_entries_restored','flags_restored','fp_restored','globals_restored')):reject('Incomplete bank restoration')
    for before,after in (('fp_control_before','fp_control_after'),('fp_status_before','fp_status_after'),('mxcsr_before','mxcsr_after')):
        if type(final.get(before)) is not int or final.get(before)!=final.get(after):reject('FP state differs')
    hrows = [strict_loads(line) for line in harness.read_text(encoding='utf-8').splitlines()]
    stages = [row.get('event') for row in hrows]
    hfinal = hrows[-1] if hrows else {}
    if stages != ['preflight_entered', 'before_task_bank', 'task_bank_returned', 'complete']:
        reject('Unexpected preflight stages')
    if not all(hfinal.get(key) is True for key in ('state_restored', 'entries_restored',
                                                 'tables_restored', 'flags_restored', 'timer_restored')):
        reject('Harness restoration failed')
    diagnostics.update(pairs=sum(counts.values()), pairs_per_function=dict(counts),
                       differences=failures, model_mismatches=model_failures,
                       validation_errors=errors, terminal_bank_record=final,
                       terminal_harness_record=hfinal)
    if errors:raise RuntimeError('Strict Pad distance/train/NOP analyzer rejected: '+'; '.join(errors))
    return dict(passed=True, pairs=sum(counts.values()), pairs_per_function=dict(counts),
                differences=failures, model_mismatches=model_failures, validation_errors=errors,
                all_equal=not failures and not model_failures,
                state_restored=final.get('state_restored') is True,
                harness_restored=hfinal.get('state_restored') is True,
                log_sha256=runner.digest(log), harness_sha256=runner.digest(harness),
                scope=final.get('scope', 'No approved scope'))

def main():
    manifest = json.loads((HERE / 'artifacts/manifest.json').read_text(encoding='utf-8'))
    assert manifest['exe_sha256']==runner.EXE_HASH
    proof_path=HERE/'artifacts/train-compiled-abi.json'
    assert runner.digest(proof_path)==manifest['train_abi_proof_sha256']
    proof=json.loads(proof_path.read_text(encoding='utf-8'))
    assert proof['asi_sha256']==manifest['asi'] and proof['pdb_sha256']==manifest['pdb']
    assert len(proof['functions'])==1 and proof['functions'][0]['return_operands']
    assert all(int(x,0)==8 for x in proof['functions'][0]['return_operands'])
    assert runner.digest(ROOT/'source/app/platform/win/WinMain.cpp')==manifest['original_winmain_sha256']
    source_inventory={p.relative_to(ROOT).as_posix() for p in (ROOT/'source').rglob('*') if p.is_file() and p.suffix in ('.cpp','.h','.hpp','.inl','.inc') and p.relative_to(ROOT).as_posix()!='source/app/platform/win/WinMain.cpp'}
    recorded_source={p for p in manifest['tested_sources'] if p.startswith('source/')}
    if source_inventory!=recorded_source:raise RuntimeError('Source file inventory changed since compile')
    for label in ('tested_sources', 'bank_sources'):
        for path, expected in manifest[label].items():
            assert runner.digest(ROOT / path) == expected, path
    harness = runner.GAME / 'preflight-pad-distance-train-nop-hex-harness.jsonl'
    if harness.exists():
        shutil.copy2(harness, HERE / 'artifacts' / ('previous-harness-' + runner.digest(harness) + '.jsonl'))
        harness.unlink()
    for ext in ('asi', 'pdb'):
        source = HERE / f'artifacts/gta_reversed-pad-distance-train-nop-hex.{ext}'
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
        status = runner.run_case(PREFIX, manifest['asi'], analyze, 'Pad distance/train/NOP13952',
                                 timeout=900, stop_before_analysis=False, unhook='', wait_for_exit=True)
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
