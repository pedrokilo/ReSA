"""Actualiza las cifras de validacion escritas a mano en README.md, reversing/README.md,
reversing/ValidationRegistry/README.md y docs/ReversingPriorities.md a partir de docs/hooks.csv,
reversing/cases.json y el registro de validacion. Complementa sync_docs.py (que no toca esos textos).

Uso: python contrib/sync_readme_counts.py
"""
import csv
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def fmt(n):  # miles con punto, como el resto de la documentacion
    return f'{n:,}'.replace(',', '.')

def pct(a, b):
    return f'{100 * a / b:.2f}'.replace('.', ',')

def main():
    rows = list(csv.DictReader((ROOT/'docs/hooks.csv').open(encoding='utf-8-sig')))
    total = len(rows); rev = sum(r['reversed'] == '1' for r in rows); val = sum(r['validated'] == '1' for r in rows)
    classes = {}
    for r in rows: classes.setdefault(r['class'], []).append(r)
    complete = sum(all(x['reversed'] == '1' for x in v) for v in classes.values())
    cases = json.loads((ROOT/'reversing/cases.json').read_text(encoding='utf-8'))
    isolated = sum('isolated' in c['id'] for c in cases)
    subs = {
        'README.md': [
            (r'\| Clases completas \| [\d.]+/[\d.]+ · [\d,]+% \|', f'| Clases completas | {fmt(complete)}/{fmt(len(classes))} · {pct(complete, len(classes))}% |'),
            (r'\| Funciones reconstruidas \| [\d.]+/[\d.]+ · [\d,]+% \|', f'| Funciones reconstruidas | {fmt(rev)}/{fmt(total)} · {pct(rev, total)}% |'),
            (r'\| Funciones con `validated=1` \| \*\*[\d.]+/[\d.]+ · [\d,]+%\*\*', f'| Funciones con `validated=1` | **{fmt(val)}/{fmt(total)} · {pct(val, total)}%**'),
            (r'0/[\d.]+ · 0,00%; las [\d.]+ conservan', f'0/{fmt(total)} · 0,00%; las {fmt(val)} conservan'),
        ],
        'reversing/README.md': [
            (r'ya vincula [\d.]+ funciones', f'ya vincula {fmt(val)} funciones'),
            (r'Las [\d.]+ conservan evidencia', f'Las {fmt(val)} conservan evidencia'),
            (r'\[inventario de las [\d.]+ reconstruidas\]', f'[inventario de las {fmt(rev)} reconstruidas]'),
            (r'identifica también las [\d.]+ todavía', f'identifica también las {fmt(rev - val)} todavía'),
            (r'\*\*[\d.]+ clases/grupos y [\d.]+ entradas\*\*: [\d.]+ reconstruidas y [\d.]+ pendientes', f'**{fmt(len(classes))} clases/grupos y {fmt(total)} entradas**: {fmt(rev)} reconstruidas y {fmt(total - rev)} pendientes'),
            (r'De ellas, \*\*[\d.]+ tienen `validated=1`\*\*', f'De ellas, **{fmt(val)} tienen `validated=1`**'),
            (r'\*\*\d+ casos: \d+ dentro del motor y uno aislado\*\*', f'**{len(cases)} casos: {len(cases) - isolated} dentro del motor y uno aislado**'),
        ],
        'reversing/ValidationRegistry/README.md': [
            (r'Hay \*\*[\d.]+ funciones con `validated=1`\*\*', f'Hay **{fmt(val)} funciones con `validated=1`**'),
            (r'clasifica las [\d.]+ entradas: 0 vigentes, [\d.]+ históricas por revisar', f'clasifica las {fmt(rev)} entradas: 0 vigentes, {fmt(val)} históricas por revisar'),
            (r'La barra es \*\*0/[\d.]+ · 0,00%\*\*', f'La barra es **0/{fmt(total)} · 0,00%**'),
        ],
        'docs/ReversingPriorities.md': [
            (r'registra actualmente \*\*[\d.]+ entradas con `validated=1`\*\*', f'registra actualmente **{fmt(val)} entradas con `validated=1`**'),
        ],
    }
    for f, pairs in subs.items():
        p = ROOT/f; s = p.read_bytes().decode('utf-8'); n = 0
        for a, b in pairs:
            s, k = re.subn(a, b, s); n += k
        p.write_bytes(s.encode('utf-8'))
        print(f'{f}: {n} cifras')
    print(f'total={total} reconstruidas={rev} validadas={val} clases completas={complete}/{len(classes)} casos={len(cases)}')

if __name__ == '__main__':
    main()
