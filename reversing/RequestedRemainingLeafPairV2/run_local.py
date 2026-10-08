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

PREFIX = 'remaining-leaf-pair-v2'
EXPECTED = {'CInterestingEvents::InvalidateEvent': 720,
            'CPedDamageResponseCalculator::ComputeWillKillPed': 18432}
diagnostics = {}

def analyze(log):
    harness = runner.GAME / 'preflight-remaining-leaf-pair-v2-harness.jsonl'
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
        if len(errors) < 100:
            errors.append(message)
    with log.open(encoding='utf-8') as stream:
        for line_number, line in enumerate(stream, 1):
            row = json.loads(line)
            if row.get('event') != 'comparison':
                if row.get('event') != 'complete' or final is not None:
                    reject('Unexpected bank event at line ' + str(line_number))
                final = row
                continue
            if final is not None:
                reject('Comparison after terminal record')
            function = row.get('function')
            counts[function] += 1
            pair = row.get('pair')
            if pair in pairs:
                reject('Duplicate pair ' + str(pair))
            pairs.add(pair)
            failures += row.get('equal') is not True
            original, candidate = row.get('original', {}), row.get('candidate', {})
            matched = original == candidate and all(
                route.get('gate') is True and route.get('collaborators_valid') is True
                for route in (original, candidate))
            if function == 'CInterestingEvents::InvalidateEvent':
                fixture = (function, row.get('selection'), row.get('active'),
                           row.get('nonnull'), row.get('poison'))
                matched = matched and isinstance(row.get('model_bytes'), list) and len(row['model_bytes']) == 0x158 and original.get('bytes') == row['model_bytes']
            elif function == 'CPedDamageResponseCalculator::ComputeWillKillPed':
                fixture = (function, row.get('ped_type'), row.get('controls'),
                           row.get('weapon'), row.get('boundary'))
                matched = matched and row.get('model_equal') is True and all(
                    isinstance(row.get('model_' + key), list) and len(row['model_' + key]) == size and original.get(key) == row['model_' + key]
                    for key, size in (('ped_bytes', 0x79C), ('calculator_bytes', 0x14), ('response_bytes', 0xC)))
            else:
                fixture = (function, pair)
                reject('Unknown target ' + str(function))
                matched = False
            matched = matched and isinstance(row.get('model_trace'), list) and original.get('trace') == candidate.get('trace') == row['model_trace']
            if not matched:
                model_failures += 1
            if fixture in fixture_ids:
                reject('Duplicate fixture ' + str(fixture))
            fixture_ids.add(fixture)
    if dict(counts) != EXPECTED or pairs != set(range(19152)):
        reject('Incomplete or extra function/pair coverage')
    expected_fixtures = {
        ('CInterestingEvents::InvalidateEvent', selection, active, nonnull, poison)
        for selection in range(10) for active in range(-1, 8)
        for nonnull in (False, True) for poison in range(4)}
    expected_fixtures.update({
        ('CPedDamageResponseCalculator::ComputeWillKillPed', ped, control, weapon, boundary)
        for ped in range(3) for control in range(64)
        for weapon in (0, 1, 46, 47, 53, 54, 22, 55) for boundary in range(12)})
    if fixture_ids != expected_fixtures:
        reject('Fixture product differs from the declared model')
    if final is None or final.get('event') != 'complete' or final.get('pairs') != 19152:
        reject('Bank did not complete exactly 19152 pairs')
    final = final or {}
    if final.get('differences') != failures or failures or model_failures:
        reject('Nonzero differences or mismatch with model')
    if not all(final.get(key) is True for key in ('state_restored', 'entries_restored',
                                                'collaborator_entries_restored', 'cheats_restored', 'flags_restored')):
        reject('Bank restoration failed')
    hrows = [json.loads(line) for line in harness.read_text(encoding='utf-8').splitlines()]
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
    return dict(passed=not errors, pairs=sum(counts.values()), pairs_per_function=dict(counts),
                differences=failures, model_mismatches=model_failures, validation_errors=errors,
                all_equal=not failures and not model_failures,
                state_restored=final.get('state_restored') is True,
                harness_restored=hfinal.get('state_restored') is True,
                log_sha256=runner.digest(log), harness_sha256=runner.digest(harness),
                scope=final.get('scope', 'No approved scope'))

def main():
    manifest = json.loads((HERE / 'artifacts/manifest.json').read_text(encoding='utf-8'))
    for label in ('tested_sources', 'bank_sources'):
        for path, expected in manifest[label].items():
            assert runner.digest(ROOT / path) == expected, path
    harness = runner.GAME / 'preflight-remaining-leaf-pair-v2-harness.jsonl'
    if harness.exists():
        shutil.copy2(harness, HERE / 'artifacts' / ('previous-harness-' + runner.digest(harness) + '.jsonl'))
        harness.unlink()
    for ext in ('asi', 'pdb'):
        source = HERE / f'artifacts/gta_reversed-remaining-leaf-pair-v2.{ext}'
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
        status = runner.run_case(PREFIX, manifest['asi'], analyze, 'Remaining leaf pair19152',
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
