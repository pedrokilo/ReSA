"""Inventario léxico reproducible; no certifica equivalencia ni validación.

Retira comentarios y literales conservando líneas. Sólo acepta mnemónicos
dentro de bloques __asm o asm de MSVC; evita contar variables fScale.
"""
import collections
import csv
import hashlib
import json
import re
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
MNEMONICS = {
    'fsin', 'fcos', 'fsincos', 'fptan', 'fpatan', 'fyl2x', 'fyl2xp1',
    'f2xm1', 'fscale', 'fsqrt', 'frndint', 'fistp', 'fisttp',
    'fldcw', 'fnstcw', 'fstcw', 'fnsave', 'frstor', 'fnstsw', 'fstsw',
}
STRIP = re.compile(r'//[^\n]*|/\*.*?\*/|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', re.S)
ASM = re.compile(r'\b(?:__asm|_asm)\b\s*')


def clean(text):
    return STRIP.sub(lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), text)


def regions(text):
    offset = 0
    while match := ASM.search(text, offset):
        begin = match.end()
        if begin < len(text) and text[begin] == '{':
            depth, end = 1, begin + 1
            while depth and end < len(text):
                depth += (text[end] == '{') - (text[end] == '}')
                end += 1
            yield begin + 1, end - 1
        else:
            end = text.find('\n', begin)
            if end < 0:
                end = len(text)
            yield begin, end
        offset = end


def main():
    rows = []
    hashes = {}
    for path in sorted((ROOT / 'source/game_sa').rglob('*')):
        if path.suffix.lower() not in {'.cpp', '.h', '.hpp', '.inl'}:
            continue
        raw = path.read_bytes()
        text = clean(raw.decode('utf-8-sig', errors='replace'))
        rel = path.relative_to(ROOT).as_posix()
        matches = []
        for begin, end in regions(text):
            for match in re.finditer(r'\b[a-zA-Z][a-zA-Z0-9]*\b', text[begin:end]):
                mnemonic = match[0].lower()
                if mnemonic in MNEMONICS:
                    line = text.count('\n', 0, begin + match.start()) + 1
                    matches.append({'path': rel, 'line': line, 'instruction': mnemonic})
        if matches:
            hashes[rel] = hashlib.sha256(raw).hexdigest()
            rows.extend(matches)
    with (HERE / 'instructions.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=['path', 'line', 'instruction'], lineterminator='\n')
        writer.writeheader()
        writer.writerows(rows)
    counts = collections.Counter(row['instruction'] for row in rows)
    summary = {
        'scope': 'source/game_sa; explicit MSVC asm only; lexical sites, not functions',
        'validated': False,
        'instruction_sites': len(rows),
        'files': len(hashes),
        'counts': dict(sorted(counts.items())),
        'input_sha256': hashes,
        'limitations': [
            'Preprocessor branches included; sites need not be compiled in every build.',
            'No compiler-generated x87, external assembler or aliases are counted.',
            'Not an inventory of naked functions or all assembler instructions.',
            'Working-tree snapshot; regenerate after concurrent source changes.',
        ],
    }
    (HERE / 'inventory.json').write_bytes((json.dumps(summary, ensure_ascii=False, indent=2) + '\n').encode('utf-8'))
    print(json.dumps({k: v for k, v in summary.items() if k != 'input_sha256'}, ensure_ascii=False))


if __name__ == '__main__':
    main()
