"""Archiva una corrida aprobada y su estado exacto antes de regenerar documentación."""
import argparse
import gzip
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HERE = ROOT/'reversing/HookUnlockAudio'
RESULTS = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-hookunlockaudio')


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('run')
    parser.add_argument('--profile', required=True, choices=['active_audio', 'original_audio'])
    args = parser.parse_args()
    origin = RESULTS/args.run
    summary = json.loads((origin/'summary.json').read_text(encoding='utf-8'))
    for key in ('passed', 'all_equal', 'state_restored', 'saves_unchanged', 'configs_restored'):
        assert summary.get(key) is True, key
    assert summary['audio_profile'] == args.profile
    manifest_raw = (HERE/'artifacts/build-manifest.json').read_bytes()
    manifest = json.loads(manifest_raw)
    assert summary['executed_probe_sha256'] == manifest['binaries']['asi']
    snapshots = []
    for folder, collection, base in [('tested-sources', manifest['sources'], ROOT),
                                     ('tested-instrumentation', manifest['instrumentation'], HERE)]:
        for relative, expected in collection.items():
            data = (base/relative).read_bytes()
            assert sha(data) == expected, relative
            snapshots.append((folder, relative, data))
    raw = (origin/'comparisons.jsonl').read_bytes()
    assert sha(raw) == summary['log_sha256']
    rows = [json.loads(line) for line in raw.splitlines()]
    pairs = [r for r in rows if r.get('event') == 'comparison']
    frames = [r for r in rows if r.get('event') == 'live_audio_execution']
    assert len(pairs) == 19840 and {r['pair'] for r in pairs} == set(range(19840))
    assert all(r['original'] is not None and r['candidate'] is not None and
               r['equal'] and r['accepted'] and r['original'] == r['candidate'] for r in pairs)
    assert not any(r.get('event', '').startswith('first_chance_av') or r.get('event') == 'error' for r in rows)
    assert len(frames) == 960 and sum(len(r['calls']) for r in frames) == 2240
    contexts = []
    for kind in range(4):
        for mask in range(8):
            selected = [r for r in frames if (r['vehicle_kind'], r['mask']) == (kind, mask)]
            assert len(selected) == 30
            assert all(r['route_ok'] and r['stack_ok'] and r['transition_ok'] and r['inputs_restored'] for r in selected)
            counts = [sum(c['function'] == i for r in selected for c in r['calls']) for i in range(3)]
            assert counts == [20 if kind < 2 else 0, 30, 30]
            loaded = [r for r in selected if r['bank_loaded'] and r['player_driver']]
            levels = sorted({r['accelerate_actual'] for r in loaded})
            sounds = sum(channel['sound'] != 0 for r in loaded for call in r['calls']
                         if call['function'] == 0 for channel in call['channels'])
            if kind < 2:
                assert {0, 128, 255}.issubset(levels) and sounds > 0
                assert any(r['brake_actual'] > 0 for r in selected if r['player_driver'])
                assert all(r['player_driver'] == (r['local_frame'] < 10 or r['local_frame'] >= 20) for r in selected)
            contexts.append({'vehicle_kind': kind, 'mask': mask, 'frames': len(selected), 'calls': counts,
                             'loaded_accelerator_values': levels, 'real_engine_sounds_when_loaded': sounds})
    assert rows[-1]['event'] == 'complete' and rows[-1]['state_restored'] and rows[-1]['fixture_cleanup']
    archive = HERE/'runs'/args.run
    archive.mkdir(parents=True, exist_ok=True)
    for folder, relative, data in snapshots:
        target = archive/folder/relative
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(data)
    (archive/'build-manifest.json').write_bytes(manifest_raw)
    for filename in ('summary.json', 'RESULTADO.txt'):
        (archive/filename).write_bytes((origin/filename).read_bytes())
    with (archive/'comparisons.jsonl.gz').open('wb') as stream:
        with gzip.GzipFile(fileobj=stream, mode='wb', mtime=0, filename='') as compressed:
            compressed.write(raw)
    assert gzip.decompress((archive/'comparisons.jsonl.gz').read_bytes()) == raw
    verification = {'run': args.run, 'audio_profile': args.profile,
                    'executed_probe_sha256': summary['executed_probe_sha256'],
                    'log_sha256': sha(raw), 'gzip_sha256': sha((archive/'comparisons.jsonl.gz').read_bytes()),
                    'manifest_sha256': sha(manifest_raw), 'source_snapshots': len(manifest['sources']),
                    'instrumentation_snapshots': len(manifest['instrumentation']), 'functions': 3, 'pairs': 19840,
                    'differences': 0, 'passing_null_records': 0, 'live_frames': 960, 'live_calls': 2240,
                    'contexts': contexts, 'state_restored': True, 'fixture_cleanup': True, 'first_chance_av_records': 0}
    (archive/'independent-verification.json').write_text(json.dumps(verification, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({'snapshot_ready': archive.relative_to(ROOT).as_posix(), 'profile': args.profile,
                      'sources': len(manifest['sources']), 'instrumentation': len(manifest['instrumentation']),
                      'asi': summary['executed_probe_sha256']}, indent=2))


if __name__ == '__main__':
    main()
