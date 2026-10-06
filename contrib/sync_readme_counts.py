"""Actualiza las cifras de validacion escritas a mano en README.md, reversing/README.md,
reversing/ValidationRegistry/README.md a partir de docs/hooks.csv,
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
    vclasses = sum(all(x['validated'] == '1' and x['reversed'] == '1' for x in v) for v in classes.values())  # de las completas
    locked = sum(r['locked'] == '1' for r in rows)
    notsa = ROOT/'docs/hooks_notsa.csv'
    notsa_count = len(list(csv.DictReader(notsa.open(encoding='utf-8-sig')))) if notsa.exists() else 0
    cases = json.loads((ROOT/'reversing/cases.json').read_text(encoding='utf-8'))
    isolated = sum('isolated' in c['id'] for c in cases)
    subs = {
        'README.md': [],
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
    }
    block = chr(10).join([
        '#### Dimensiones de progreso', '',
        '| Dimensión | Estado |', '| --- | --- |',
        f'| Clases completas | {fmt(complete)}/{fmt(len(classes))} · {pct(complete, len(classes))}% |',
        f'| Clases validadas (de las completas) | {fmt(vclasses)}/{fmt(complete)} · {pct(vclasses, complete)}% |',
        f'| Funciones reconstruidas | {fmt(rev)}/{fmt(total)} · {pct(rev, total)}% |',
        f'| Funciones validadas (de las reconstruidas) | **{fmt(val)}/{fmt(rev)} · {pct(val, rev)}%** · evidencia aprobada vinculada |', '',
        'La validación se mide sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL.'
        + (f' Los {notsa_count} extras NOTSA sin equivalente en el EXE (modo ventana, [hooks_notsa.csv](docs/hooks_notsa.csv)) quedan fuera de la cuenta.' if notsa_count else ''), '',
        '##### Siguientes mediciones', '',
        'Todavía no tienen un porcentaje útil; la tabla explica qué falta hacer para poder medirlas.', '',
        '| Dimensión | Estado actual | Qué hay que hacer |', '| --- | --- | --- |',
        f'| Validaciones vigentes para el árbol actual | 0/{fmt(val)} validadas confirmadas como vigentes | Volver a correr los bancos de cada función validada sobre el árbol actual, comprobar que sus dependencias no cambiaron desde la evidencia y registrar el resultado en el [registro de validaciones](reversing/ValidationRegistry/README.md). |',
        '| Dependencias de funciones originales eliminadas | No medido | Auditar, por cada función reconstruida, las llamadas directas y transitivas que aún saltan al EXE (`plugin::Call*`, direcciones fijas, hooks sin reconstruir) y contar las que ya no dependen de ninguna. |', '',
        f'Quedan {fmt(total - rev)} entradas pendientes, {fmt(len(classes) - complete)} clases incompletas y {fmt(locked)} locks.',
    ])
    readme = ROOT/'README.md'; t = readme.read_bytes().decode('utf-8')
    a = t.index('#### Dimensiones de progreso'); b = t.index('Quedan ', a); e = t.index('locks.', b) + len('locks.')
    readme.write_bytes((t[:a] + block + t[e:]).encode('utf-8'))
    for f, pairs in subs.items():
        p = ROOT/f; s = p.read_bytes().decode('utf-8'); n = 0
        for a, b in pairs:
            s, k = re.subn(a, b, s); n += k
        p.write_bytes(s.encode('utf-8'))
        print(f'{f}: {n} cifras')
    print(f'total={total} reconstruidas={rev} validadas={val} clases completas={complete}/{len(classes)} casos={len(cases)}')

if __name__ == '__main__':
    main()
