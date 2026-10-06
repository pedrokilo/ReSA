#!/usr/bin/env python3
"""Genera la cola de validación (funciones reconstruidas y sin validar) en docs/ReversingPriorities.md.

Solo cuenta funciones con reversed=1 y validated=0 de docs/hooks.csv. Las líneas por
función son una estimación: líneas del .cpp que más define la clase, repartidas entre
las clases que define ese fichero, divididas por sus funciones reconstruidas. No es un
coste medido. La sección se reescribe entre los marcadores START/END.

Uso: python contrib/validation_queue.py [--dry-run]
"""
import argparse
import collections
import csv
import datetime
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
DOC = ROOT / 'docs/ReversingPriorities.md'
START = '<!-- cola-validacion:start -->'
END = '<!-- cola-validacion:end -->'
TOP_START = '<!-- cola-validacion-resumen:start -->'
TOP_END = '<!-- cola-validacion-resumen:end -->'
ENFOQUE = {
    1: 'Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda.',
    2: 'Reenvíos, accesores y datos de plugin; pocas líneas por función pero hay que simular los destinos.',
    3: 'Estado compartido, pools, cámara, audio o RenderWare; fixtures propios.',
    4: 'Muchas funciones o muy largas; partir en varios bancos y dejar para el final.',
}

# No se pueden validar por pares: crean objetos o hilos reales, son el punto de entrada
# del juego o no tienen llamadores en el EXE.
NO_VALIDABLES = {
    'CAEAudioHardware': 'Initialise: crea canales y cargadores reales, DirectSoundCreate8 e hilos',
    'CPlayerPed': 'SetupPlayerPed: construye un CPlayerPed real con pools, audio y grupos',
    'Win': 'NOTSA_WinMain: punto de entrada del juego, no vuelve hasta cerrar',
    'CCollision': 'SphereVsEntity: el EXE no la llama y lee una esfera local sin inicializar',
}

DEF_RE = re.compile(r'^[\w][^;=\n]*?\b([A-Za-z_]\w*(?:<\w+>)?)::~?\w+\s*\(', re.M)


def load_stats():
    total, rec, val = collections.Counter(), collections.Counter(), collections.Counter()
    with (ROOT / 'docs/hooks.csv').open(encoding='utf-8-sig', newline='') as f:
        for row in csv.DictReader(f):
            c = row['class']
            total[c] += 1
            rec[c] += row['reversed'] == '1'
            val[c] += row['validated'] == '1'
    return total, rec, val


def estimate_lines():
    per_file = {}
    for p in (ROOT / 'source').rglob('*.cpp'):
        text = p.read_text(encoding='utf-8-sig', errors='replace')
        per_file[p] = (len(text.split('\n')), collections.Counter(m.group(1) for m in DEF_RE.finditer(text)))
    return per_file


def lines_for(cls, per_file):
    base = cls.replace('<T>', '')
    best = None
    for lines, counts in per_file.values():
        k = counts.get(base, 0)
        if k and (best is None or k > best[0]):
            best = (k, lines, sum(counts.values()))
    return None if best is None else round(best[1] * best[0] / best[2])


def tier(pending, lpf):
    if lpf is None:
        return 3
    if pending >= 90 or lpf > 50:
        return 4
    if pending <= 12 and lpf <= 35:
        return 1
    if pending > 12 and lpf <= 25:
        return 2
    return 3


TITLES = {
    1: ('Nivel 1: muy barato', 'hasta 12 funciones por validar y hasta 35 líneas por función'),
    2: ('Nivel 2: baratas por función, pero muchas', 'más de 12 funciones y hasta 25 líneas por función'),
    3: ('Nivel 3: moderadas', 'el resto'),
    4: ('Nivel 4: caras', '90 o más funciones, o más de 50 líneas por función'),
}


def build():
    total, rec, val = load_stats()
    per_file = estimate_lines()
    items = []
    for cls in total:
        pending = rec[cls] - val[cls]
        if pending <= 0:
            continue
        if cls in NO_VALIDABLES:
            pending -= 1
            if pending <= 0:
                continue
        lines = lines_for(cls, per_file)
        lpf = round(lines / max(rec[cls], 1)) if lines else None
        items.append((cls, pending, total[cls] - rec[cls], lpf))
    groups = collections.defaultdict(list)
    for item in items:
        groups[tier(item[1], item[3])].append(item)

    today = datetime.date.today().strftime('%d/%m/%Y')
    all_pending = sum(i[1] for i in items)
    out = [
        START, '',
        '<a id="cola-validacion"></a>', '',
        '## Cola de validación: funciones reconstruidas y sin validar',
        '',
        f'> **Corte {today}.** {all_pending} funciones con `reversed=1` y `validated=0` en {len(items)} clases, '
        'de `docs/hooks.csv`. No incluye las que no se pueden validar por pares (abajo). '
        'Se regenera con `python contrib/validation_queue.py`.',
        '',
        'Validar no exige que la clase esté completa: se valida cada función ya reconstruida. '
        '"Sin reconstruir" es informativo y no cuenta como trabajo de esta cola. '
        '"Líneas/func." es una **estimación** (líneas del `.cpp` repartidas entre las clases que define, '
        'divididas por sus funciones reconstruidas), no un coste medido; `?` = no se pudo medir.',
        '',
    ]
    for k in (1, 2, 3, 4):
        rows = sorted(groups[k], key=lambda d: (d[1], d[3] if d[3] is not None else 999, d[0]))
        title, rule = TITLES[k]
        out += [f'<a id="nivel-{k}"></a>', '', f'### {title} ({len(rows)} clases, {sum(r[1] for r in rows)} funciones)', '',
                f'Criterio: {rule}.', '',
                '| # | Clase | Por validar | Sin reconstruir | Líneas/func. |', '|--:|---|--:|--:|--:|']
        for i, (cls, pend, nrev, lpf) in enumerate(rows, 1):
            out.append(f'| {i} | {cls} | {pend} | {nrev or ""} | {lpf if lpf is not None else "?"} |')
        out.append('')
    out += ['<a id="fuera-de-la-cola"></a>', '', '### Fuera de la cola (no validables por pares)', '',
            '| Clase | Función | Motivo |', '|---|---|---|']
    for cls, why in NO_VALIDABLES.items():
        fn, reason = why.split(': ', 1)
        out.append(f'| {cls} | `{fn}` | {reason} |')
    out += ['', END, '']
    icons = {1: '🟢', 2: '🟡', 3: '🟠', 4: '🔴'}
    top = [TOP_START, '',
           '| Cola de validación | Clases | Por validar | Enfoque |', '| --- | :---: | :---: | --- |']
    for k in (1, 2, 3, 4):
        rows = groups[k]
        top.append(f'| [{icons[k]} {TITLES[k][0]}](#nivel-{k}) | {len(rows)} | {sum(r[1] for r in rows)} | {ENFOQUE[k]} |')
    top += [f'| **Total** | **{len(items)}** | **{all_pending}** | Funciones reconstruidas sin validar; ver la [cola completa](#cola-validacion). |',
            '', TOP_END, '', '']
    return '\n'.join(top), '\n'.join(out)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    top, section = build()
    raw = DOC.read_bytes().decode('utf-8')
    nl = '\r\n' if '\r\n' in raw else '\n'
    text = raw.replace('\r\n', '\n')
    pattern = re.compile(re.escape(START) + r'.*?' + re.escape(END) + r'\n?', re.S)
    if pattern.search(text):
        text = pattern.sub(lambda _m: section, text, count=1)
    else:
        text = text.rstrip('\n') + '\n\n' + section
    top_pat = re.compile(re.escape(TOP_START) + r'.*?' + re.escape(TOP_END) + r'\n?', re.S)
    if top_pat.search(text):
        text = top_pat.sub(lambda _m: top, text, count=1)
    else:
        anchor = '**Navegación:**'
        if anchor not in text:
            raise RuntimeError('No se encontró la línea de navegación de ReversingPriorities.md')
        text = text.replace(anchor, top + anchor, 1)
    nav_old = '· [Top 10](#top-10)'
    if nav_old in text and '[Cola de validación](#cola-validacion)' not in text:
        text = text.replace(nav_old, nav_old + ' · [Cola de validación](#cola-validacion)', 1)
    if args.dry_run:
        print(top)
        print(section)
        return
    DOC.write_bytes(text.replace('\n', nl).encode('utf-8'))
    print(f'{DOC.name}: cola de validación actualizada')


if __name__ == '__main__':
    main()
