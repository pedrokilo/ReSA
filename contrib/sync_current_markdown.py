"""Sincroniza el aviso de estado actual sin reescribir el cuerpo histórico de los Markdown."""
import argparse
import csv
import hashlib
import json
import posixpath
import re
import subprocess
import time
from collections import Counter
from datetime import datetime
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BLOCK = re.compile(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*', re.S)
EXCLUDED = {'artifacts', 'build', 'thirdparty', 'third-party', 'runs', 'evidence', 'preparation-archive',
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
    extras_path = ROOT/'docs/hooks_notsa.csv'
    extras = list(csv.DictReader(extras_path.open(encoding='utf-8-sig'))) if extras_path.exists() else []
    notsa_note = ('Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. '
                  if any((r['class'], r['fn_name']) == ('Win', 'NOTSA_WinMain') for r in extras) else '')
    from validation_registry import evaluate
    current = evaluate(ROOT, ROOT/'docs/hooks.csv')['current']
    coverage_path = ROOT/'reversing/BinaryCoverageAudit/summary.json'
    coverage = json.loads(coverage_path.read_text(encoding='utf-8')) if coverage_path.exists() else None
    dependencies_path = ROOT/'reversing/OriginalDependencies499/progress.json'
    dependencies = json.loads(dependencies_path.read_text(encoding='utf-8')) if dependencies_path.exists() else None
    dependencies_note = (
        f"Cohorte inicial de 499 dependencias: {dependencies['runtime_passed']} aprobadas dentro del juego; "
        f"{dependencies['game_runtime_pending']} entradas del juego pendientes, "
        f"{dependencies['categories']['library']} de bibliotecas y "
        f"{dependencies['categories']['unknown']} inciertas. " if dependencies else '')
    frozen = json.loads((ROOT/'reversing/RequestedValidation20261008/documentation-frozen-exceptions.json').read_text(encoding='utf-8'))['frozen_markdown_exceptions']
    def check_frozen():
        for name, info in frozen.items():
            assert hashlib.sha256((ROOT/name).read_bytes()).hexdigest() == info['sha256'], name
    check_frozen()
    files = subprocess.check_output(['git', 'ls-files', '--cached', '--others', '--exclude-standard'], cwd=ROOT, text=True).splitlines()
    changed = 0
    for name in sorted(set(files)):
        parts = Path(name).parts
        if not name.endswith('.md') or name in frozen or name == 'docs/CurrentStatus.md' or name.startswith('docs/Reorganizacion/'):
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
        coverage_link = posixpath.relpath('docs/BinaryCoverageAudit.md', posixpath.dirname(name) or '.')
        coverage_note = (
            f"El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: "
            f"{coverage['entries_without_catalogue_names']} entradas binarias sin nombre catalogado; "
            f"{coverage['explicit_original_targets_without_catalogue_name']} destinos originales explícitos fuera del catálogo, "
            f"{coverage['explicit_targets_referenced_from_catalogued_entries']} referenciados desde entradas catalogadas. "
            f"Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. "
            f"[Cobertura fuera del catálogo]({coverage_link}). " if coverage else '')
        prefix = (
            '<!-- RESA-CURRENT-STATUS:BEGIN -->\n'
            f'> **Estado al {date} (Ciudad de México):** {len(groups)} grupos / {len(rows)} funciones reconstruidas; '
            f'{valid} con evidencia histórica y {sum(pending.values())} pendientes de validación en {len(pending)} grupos. '
            'CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). '
            f'Vigencia acreditada: {current}; la evidencia histórica no acredita dependencias ni independencia del EXE. '
            f'{notsa_note}'
            f'{coverage_note}'
            f'{dependencies_note}'
            f'[Estado actual y alcance]({link}). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.\n'
            '<!-- RESA-CURRENT-STATUS:END -->\n\n'
        )
        result = (prefix + BLOCK.sub('', text)).replace('\r\n', '\n').encode('utf-8')
        if result != raw:
            for attempt in range(5):
                try:
                    path.write_bytes(result)
                    break
                except OSError:
                    if attempt == 4:
                        raise
                    time.sleep(0.1 * (attempt + 1))
            changed += 1
    check_frozen()
    print(json.dumps({'markdown_updated': changed, 'frozen_intact': len(frozen), 'validated': valid, 'pending': dict(pending)}))

if __name__ == '__main__':
    main()
