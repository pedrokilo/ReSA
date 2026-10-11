"""Merge bounded source reviews without granting runtime validation credit."""
import csv
import hashlib
import json
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]


def read(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def write(path, rows, fields):
    with path.open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)


def main():
    original = ROOT / 'reversing/BinaryCoverageAudit/review-source-calls.csv'
    targets_path = HERE / 'targets.csv'
    if not targets_path.exists():
        targets = [row for row in read(original)
                   if int(row['incoming_edges_from_catalogued_functions']) > 0]
        assert len(targets) == 499
        write(targets_path, targets, list(targets[0]))
        (HERE / 'baseline.json').write_text(json.dumps({
            'targets': 499, 'source': str(original.relative_to(ROOT)).replace('\\', '/'),
            'source_sha256': hashlib.sha256(original.read_bytes()).hexdigest(),
            'targets_sha256': hashlib.sha256(targets_path.read_bytes()).hexdigest(),
            'scope': '499 original explicit targets referenced from catalogue at audit 2026-10-10',
        }, indent=2)+'\n', encoding='utf-8')
    targets = read(targets_path)
    baseline = json.loads((HERE / 'baseline.json').read_text(encoding='utf-8'))
    assert hashlib.sha256(targets_path.read_bytes()).hexdigest() == baseline['targets_sha256']
    expected = {row['address'] for row in targets}
    assert len(expected) == len(targets) == 499
    reviews = {}
    for path in sorted(HERE.glob('classification-*.csv')):
        for row in read(path):
            assert row['address'] in expected, row
            assert row['address'] not in reviews, row['address']
            assert row['category'] in {'game', 'library', 'compiler', 'unknown'}, row
            assert row['implementation'] in {'wrapper', 'implemented', 'unknown'}, row
            reviews[row['address']] = row
    approved_path = HERE / 'approved.json'
    approved = json.loads(approved_path.read_text(encoding='utf-8')) if approved_path.exists() else {}
    assert set(approved) <= expected
    merged = []
    for target in targets:
        address = target['address']
        row = {'address': address, 'identity': '', 'category': 'unknown',
               'implementation': 'unknown', 'source': target['source_references'],
               'evidence': '', 'notes': 'Source review pending',
               'runtime_status': 'not_validated', 'runtime_evidence': ''}
        if address in reviews:
            row.update(reviews[address])
        if address in approved:
            row['identity'] = approved[address]['identity']
            row['implementation'] = 'implemented'
            row['runtime_status'] = 'passed'
            row['runtime_evidence'] = approved[address]['validation_summary']
            summary = json.loads((ROOT / row['runtime_evidence']).read_text(encoding='utf-8'))
            assert summary['passed'] is True and summary['differences'] == 0
            assert summary['state_restored'] is True
        merged.append(row)
    write(HERE / 'progress.csv', merged, list(merged[0]))
    summary = {'targets': 499, 'reviewed': len(reviews),
               'categories': dict(Counter(row['category'] for row in merged)),
               'implementation': dict(Counter(row['implementation'] for row in merged)),
               'runtime_passed': len(approved), 'runtime_pending': 499-len(approved),
               'scope': 'Source classification is not runtime validation; unknowns preserved.'}
    summary['game_runtime_pending'] = sum(row['category']=='game' and row['runtime_status']!='passed' for row in merged)
    summary['unknown_category_pending'] = sum(row['category']=='unknown' for row in merged)
    (HERE / 'progress.json').write_text(json.dumps(summary, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(summary))


if __name__ == '__main__':
    main()
