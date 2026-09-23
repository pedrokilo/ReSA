"""Repair the identified data records in the dedicated test installation only."""
import argparse
import hashlib
import json
import csv
import subprocess
from datetime import datetime
from pathlib import Path

GAME = Path(r'E:\JUEGOS\GTA San Andreas - CVehicle Test')
HERE = Path(__file__).resolve().parent

def sha(data):
    return hashlib.sha256(data).hexdigest()

def prepare():
    changes = []
    for name in ('timecyc.dat', 'vehicles.ide', 'cargrp.dat'):
        path = GAME / 'data' / name
        before = path.read_bytes()
        lines = before.decode('ascii').splitlines(keepends=True)
        edits = []
        for index, line in enumerate(lines):
            body = line.rstrip('\r\n')
            ending = line[len(body):]
            tokens = body.replace(',', ' ').split()
            if name == 'timecyc.dat' and tokens and not body.lstrip().startswith('//'):
                if len(tokens) == 49:
                    assert tokens[:4] == ['255', '167', '198', '223'], 'Unknown malformed weather row'
                    # Explicit neutral white ambient RGB; the missing historical values cannot be recovered.
                    body = body.replace('255', '255 255 255', 1)
                    tokens = body.split()
                assert len(tokens) in (51, 52), 'Unexpected weather format'
                if len(tokens) == 51:
                    # Both native and reconstructed readers request this 52nd field.
                    body += '\t1.0'  # Neutral directional multiplier, previously unspecified.
            elif name == 'vehicles.ide' and tokens[:2] == ['460', 'skimmer']:
                assert len(tokens) in (11, 14), 'Unexpected skimmer definition'
                if len(tokens) == 11:
                    body += ', -1, 0.7, 0.7'  # Existing reconstructed loader defaults.
            elif name == 'cargrp.dat' and tokens and not body.lstrip().startswith('#'):
                data, sep, comment = body.partition('#')
                models = [s.strip() for s in data.split(',') if s.strip()]
                if len(models) > 23:
                    # Keep precisely the prefix consumed by the fixed-size native group.
                    body = ', '.join(models[:23]) + ('\t#' + comment if sep else '')
            if body + ending != line:
                edits.append(dict(line=index + 1, before=line.rstrip(), after=body))
                lines[index] = body + ending
        after = ''.join(lines).encode('ascii')
        changes.append((path, before, after, edits))
    return changes

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--apply', action='store_true')
    args = parser.parse_args()
    if args.apply:
        processes = csv.reader(subprocess.check_output(['tasklist','/FO','CSV','/NH'], text=True).splitlines())
        if any(r and r[0].lower() in ('gta_sa.exe','gta-sa.exe') for r in processes):
            raise RuntimeError('Cerrar GTA antes de modificar sus datos.')
    changes = prepare()  # Validate every file before writing any file.
    manifest = HERE / 'data-repair.json'
    if manifest.exists():
        known = json.loads(manifest.read_text())
        for path, before, after, _ in changes:
            assert sha(before) in (known[path.name]['before_sha256'], known[path.name]['after_sha256']), 'Unknown data version'
            assert sha(after) == known[path.name]['after_sha256'], 'Unexpected repair result'
    report = {p.name: dict(before_sha256=sha(a), after_sha256=sha(b), edits=e) for p,a,b,e in changes}
    if args.apply and any(a != b for _,a,b,_ in changes):
        backup = GAME / '_baseline' / datetime.now().strftime('before-ik-data-%Y%m%d-%H%M%S')
        backup.mkdir()
        for p,a,_,_ in changes:
            (backup / p.name).write_bytes(a)
        try:
            for p,a,b,_ in changes:
                assert p.read_bytes() == a, 'File changed after validation'
                p.write_bytes(b)
        except BaseException:
            for p,a,_,_ in changes:
                p.write_bytes(a)
            raise
        report['backup'] = str(backup)
        (backup / 'manifest.json').write_text(json.dumps(report, indent=2))
        # Full before/after records remain beside the private backups.
        compact = {k:dict(before_sha256=v['before_sha256'], after_sha256=v['after_sha256'],
                         edited_lines=[e['line'] for e in v['edits']])
                   for k,v in report.items() if isinstance(v,dict)}
        compact['backup'] = str(backup)
        manifest.write_text(json.dumps(compact, indent=2))
    print(json.dumps({k:len(v['edits']) for k,v in report.items() if isinstance(v,dict)}))

if __name__ == '__main__':
    main()
