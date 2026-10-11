"""Inventario léxico de ensamblador fuente; no concede validación ni cuenta funciones."""
import csv
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'reversing/CppOnlyMigration'
LITERALS = re.compile(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'')
TOKENS = re.compile(r'\b(?:__asm|_asm|asm)\b|__declspec\s*\(\s*naked\s*\)')

def main():
    rows = []
    counts = {}
    for path in sorted((ROOT / 'source/game_sa').rglob('*')):
        if not path.is_file():
            continue
        counts[path.suffix or '(sin extensión)'] = counts.get(path.suffix or '(sin extensión)', 0) + 1
        if path.suffix not in {'.cpp', '.h', '.hpp', '.inc'}:
            continue
        text = path.read_text(encoding='utf-8-sig', errors='replace')
        code = LITERALS.sub(lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), text)
        for match in TOKENS.finditer(code):
            rows.append({'file': path.relative_to(ROOT).as_posix(),
                         'line': code.count('\n', 0, match.start()) + 1,
                         'kind': 'naked' if match[0].startswith('__declspec') else 'inline_assembly',
                         'status': 'pending_cpp_conversion'})
    OUT.mkdir(exist_ok=True)
    with (OUT / 'inventory.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=['file', 'line', 'kind', 'status'])
        writer.writeheader()
        writer.writerows(rows)
    approvals_path = OUT / 'approvals.json'
    approvals = json.loads(approvals_path.read_text(encoding='utf-8')) if approvals_path.exists() else {}
    summary = {'scope': 'source/game_sa; lexical occurrences, not functions',
               'files_with_assembly_or_naked': len({r['file'] for r in rows}),
               'inline_assembly_occurrences': sum(r['kind'] == 'inline_assembly' for r in rows),
               'naked_declarations': sum(r['kind'] == 'naked' for r in rows),
               'extensions': counts, 'conversion_requirement': 'C++ only; exact original EXE results; revalidate in full game',
               'converted_and_revalidated': sum(entry.get('converted_source_functions', 1) for entry in approvals.values())}
    (OUT / 'summary.json').write_bytes((json.dumps(summary, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))
    grouped = {}
    for row in rows:
        group = grouped.setdefault(row['file'], {'inline_assembly': 0, 'naked': 0})
        group[row['kind']] += 1
    with (OUT / 'files.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.writer(stream)
        writer.writerow(['file', 'inline_assembly_occurrences', 'naked_declarations'])
        for path, counts in sorted(grouped.items()):
            writer.writerow([path, counts['inline_assembly'], counts['naked']])
    lines = ['# Archivos pendientes de convertir a C++', '',
             f"{len(grouped)} archivos: {summary['inline_assembly_occurrences']} apariciones de ensamblador y {summary['naked_declarations']} declaraciones naked.",
             '', 'Conteo lexico de source/game_sa, excluyendo comentarios y cadenas. Las dos columnas no se suman como funciones: un cuerpo naked puede contener ensamblador.',
             '', '| Archivo | Ensamblador | naked |', '| --- | ---: | ---: |']
    lines.extend(f"| [{path}](../../{path}) | {counts['inline_assembly']} | {counts['naked']} |" for path, counts in sorted(grouped.items()))
    (OUT / 'FILES.md').write_text('\n'.join(lines) + '\n', encoding='utf-8')
    print(json.dumps(summary, ensure_ascii=False))

if __name__ == '__main__':
    main()
