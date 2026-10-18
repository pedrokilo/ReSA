"""Une la clasificación con las admisiones de runtime del cohorte «2» sin conceder crédito de validación que no exista.

targets.csv y baseline.json son la instantánea fija (se verifica su SHA256, nunca se reescriben). La admisión (approved.json, mismo formato que
en el cohorte 499) sólo se acepta con un summary de banco con passed y differences==0 y state_restored; hoy está vacía.
Uso: python reversing/OriginalDependencies2026-10-17/update_progress.py
"""
import csv
import hashlib
import json
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]


def read(path):
    with path.open(encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def main():
    targets = read(HERE / 'targets.csv')
    baseline = json.loads((HERE / 'baseline.json').read_text(encoding='utf-8'))
    assert hashlib.sha256((HERE / 'targets.csv').read_bytes()).hexdigest() == baseline['targets_sha256'], 'targets.csv cambió: es una instantánea fija'
    assert len({t['address'] for t in targets}) == len(targets) == baseline['targets']
    expected = {t['address'] for t in targets}
    classification = {r['address']: r for r in read(HERE / 'classification.csv')}
    assert set(classification) == expected, 'classification.csv debe cubrir exactamente los destinos'
    approved_path = HERE / 'approved.json'
    approved = json.loads(approved_path.read_text(encoding='utf-8')) if approved_path.exists() else {}
    assert set(approved) <= expected
    rows = []
    for t in targets:
        c = classification[t['address']]
        assert c['category'] in {'game', 'library', 'compiler', 'unknown'} and c['implementation'] in {'wrapper', 'embedded_call', 'implemented', 'unknown'}
        row = {'address': t['address'], 'identity': c['identity'], 'category': c['category'], 'implementation': c['implementation'],
               'source': c['source'], 'evidence': c['evidence'], 'notes': c['notes'], 'runtime_status': 'not_validated', 'runtime_evidence': ''}
        if t['address'] in approved:
            a = approved[t['address']]
            s = json.loads((ROOT / a['validation_summary']).read_text(encoding='utf-8'))
            assert s['passed'] is True and s['differences'] == 0 and s['state_restored'] is True
            row.update(identity=a['identity'], implementation='implemented', runtime_evidence=a['validation_summary'],
                       runtime_status='library_validated' if c['category'] == 'library' else 'passed')
        rows.append(row)
    with (HERE / 'progress.csv').open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]))
        w.writeheader()
        w.writerows(rows)
    passed = sum(r['runtime_status'] == 'passed' for r in rows)
    wrappers = [t for t in targets if 'plugin_call' in t['wrapper_styles']]
    summary = {'targets': len(rows), 'reviewed': len(classification),
               'categories': dict(Counter(r['category'] for r in rows)),
               'implementation': dict(Counter(r['implementation'] for r in rows)),
               'wrapper_styles': dict(Counter(t['wrapper_styles'] for t in targets)),
               'plugin_call_targets': len(wrappers),
               'runtime_passed': passed, 'runtime_pending': len(rows) - passed,
               'scope': 'Source classification is not runtime validation; unknowns preserved.',
               'game_runtime_pending': sum(r['category'] == 'game' and r['runtime_status'] != 'passed' for r in rows),
               'unknown_category_pending': sum(r['category'] == 'unknown' for r in rows),
               'library_runtime_validated': sum(r['runtime_status'] == 'library_validated' for r in rows)}
    (HERE / 'progress.json').write_text(json.dumps(summary, indent=2) + chr(10), encoding='utf-8')
    print(json.dumps(summary))


if __name__ == '__main__':
    main()
