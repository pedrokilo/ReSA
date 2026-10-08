"""Sincroniza solo el alcance solicitado y sus comentarios generados."""
import csv
import json
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT/'contrib'))
import source_status
import sync_validated_flags

scope = json.loads((Path(__file__).parent/'scope.json').read_text())
key = lambda r: tuple(r[k] for k in ('class', 'fn_name', 'address', 'type'))
expected = {key(r) for r in scope['functions']}
runtime = Path('D:/JUEGOS/GTA San Andreas - CVehicle Test/scripts/runtime-hooks.csv')
with runtime.open(encoding='utf-8-sig', newline='') as f:
    runtime_rows = list(csv.DictReader(f))
actual = {}
for row in scope['functions']:
    matches = [r for r in runtime_rows if all(r[k] == row[k] for k in ('class','fn_name','type'))]
    assert len(matches) == 1, row
    match = matches[0]
    assert int(row['address'],16) == 0 or row['address'] == match['address'], row
    actual[key(row)] = match
assert all(actual[k]['reversed'] == '1' for k in expected)
catalog = ROOT/'docs/hooks.csv'
with catalog.open(encoding='utf-8-sig', newline='') as f:
    reader = csv.DictReader(f)
    fields, rows = reader.fieldnames, list(reader)
for row in rows:
    if key(row) in expected:
        row.update({k: actual[key(row)][k] for k in ('reversed', 'locked')})
with catalog.open('w', encoding='utf-8', newline='') as f:
    writer = csv.DictWriter(f, fieldnames=fields, lineterminator='\n')
    writer.writeheader()
    writer.writerows(rows)
print(sync_validated_flags.sync(ROOT, catalog))
classes = source_status.read_classes(catalog)
index = source_status.source_index(ROOT, classes)
owners = defaultdict(list)
for name, paths in index.items():
    for path in paths:
        owners[path].append(name)
targets = {p for name in scope['classes'] for p in index[name]}
changed = 0
for path in sorted(targets):
    original = path.read_bytes()
    stripped = source_status.BLOCK.sub(b'', original)
    bom = b'\xef\xbb\xbf' if stripped.startswith(b'\xef\xbb\xbf') else b''
    body = stripped[len(bom):]
    newline = '\r\n' if b'\r\n' in body else '\n'
    comment = source_status.status_block(owners[path], classes, {})
    updated = bom + comment.replace('\n', newline).encode() + body
    if original != updated:
        path.write_bytes(updated)
        changed += 1
print('Comentarios actualizados:', changed)
