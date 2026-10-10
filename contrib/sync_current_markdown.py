"""Sincroniza el aviso de estado actual sin reescribir el cuerpo histórico de los Markdown."""
import argparse
import csv
import hashlib
import json
import posixpath
import re
import subprocess
from collections import Counter
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BLOCK = re.compile(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*', re.S)
EXCLUDED = {'artifacts', 'build', 'thirdparty', 'third-party', 'runs', 'preparation-archive',
            'baseline-v1', 'archive', 'build-history', '.git', '__pycache__'}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--date', required=True, help='Fecha local de Ciudad de México, YYYY-MM-DD.')
    args = parser.parse_args()
    date = datetime.strptime(args.date, '%Y-%m-%d').strftime('%d/%m/%Y')
    rows = list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
    groups = {r['class'] for r in rows}
    pending = Counter(r['class'] for r in rows if r['validated'] == '0')
    valid = sum(r['validated'] == '1' for r in rows)
    from validation_registry import evaluate
    current = evaluate(ROOT, ROOT/'docs/hooks.csv')['current']
    frozen = json.loads((ROOT/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text(encoding='utf-8'))['frozen_markdown_exceptions']
    def check_frozen():
        for name, info in frozen.items():
            assert hashlib.sha256((ROOT/name).read_bytes()).hexdigest() == info['sha256'], name
    check_frozen()
    files = subprocess.check_output(['git', 'ls-files', '--cached', '--others', '--exclude-standard'], cwd=ROOT, text=True).splitlines()
    changed = 0
    for name in sorted(set(files)):
        parts = Path(name).parts
        if not name.endswith('.md') or name in frozen or name == 'docs/CurrentStatus.md':
            continue
        if any(p in EXCLUDED or 'archive' in p or p.startswith('baseline-before') for p in parts):
            continue
        path = ROOT/name
        if not path.is_file():
            continue
        raw = path.read_bytes()
        try:
            text = raw.decode('utf-8-sig')
        except UnicodeDecodeError:
            text = raw.decode('cp1252')
        link = posixpath.relpath('docs/CurrentStatus.md', posixpath.dirname(name) or '.')
        prefix = (
            '<!-- RESA-CURRENT-STATUS:BEGIN -->\n'
            f'> **Estado al {date} (Ciudad de México):** {len(groups)} grupos / {len(rows)} funciones reconstruidas; '
            f'{valid} con evidencia histórica y {sum(pending.values())} pendiente de validación en {len(pending)} grupo. '
            'CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). '
            f'Vigencia acreditada: {current}; la evidencia histórica no acredita dependencias ni independencia del EXE. '
            'La única pendiente es Win::NOTSA_WinMain (punto de entrada, excepción documentada). '
            f'[Estado actual y alcance]({link}). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.\n'
            '<!-- RESA-CURRENT-STATUS:END -->\n\n'
        )
        result = (prefix + BLOCK.sub('', text)).replace('\r\n', '\n').encode('utf-8')
        if result != raw:
            path.write_bytes(result)
            changed += 1
    check_frozen()
    print(json.dumps({'markdown_updated': changed, 'frozen_intact': len(frozen), 'validated': valid, 'pending': dict(pending)}))

if __name__ == '__main__':
    main()
