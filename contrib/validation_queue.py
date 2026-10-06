#!/usr/bin/env python3
"""Genera docs/ReversingPriorities.md: la cola de validación por niveles.

El documento completo sale de docs/hooks.csv y de un recuento de líneas del código
fuente; no se edita a mano. Solo cuenta funciones con reversed=1 y validated=0.

Las líneas por función son una estimación: líneas del .cpp que más define la clase,
repartidas entre las clases que define ese fichero y divididas por las funciones
reconstruidas de la clase. No es un coste medido.

Uso: python contrib/validation_queue.py [--dry-run]
(sync_docs.py lo llama en cada sincronización.)
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

# No se pueden validar por pares: crean objetos o hilos reales, son el punto de entrada
# del juego o no tienen llamadores en el EXE.
NO_VALIDABLES = {
    'CAEAudioHardware': ('Initialise', 'crea canales y cargadores reales, llama a DirectSoundCreate8 y arranca hilos'),
    'CPlayerPed': ('SetupPlayerPed', 'construye un CPlayerPed real con pools, audio, inteligencia y grupos'),
    'Win': ('NOTSA_WinMain', 'punto de entrada del juego: no vuelve hasta cerrar'),
    'CCollision': ('SphereVsEntity', 'el EXE no la llama y lee una esfera local sin inicializar'),
}

DEF_RE = re.compile(r'^[\w][^;=\n]*?\b([A-Za-z_]\w*(?:<\w+>)?)::~?\w+\s*\(', re.M)

TITLES = {
    1: ('🟢', 'Nivel 1: muy barato', 'hasta 12 funciones por validar y hasta 35 líneas por función'),
    2: ('🟡', 'Nivel 2: baratas por función, pero muchas', 'más de 12 funciones y hasta 25 líneas por función'),
    3: ('🟠', 'Nivel 3: moderadas', 'el resto'),
    4: ('🔴', 'Nivel 4: caras', '90 o más funciones, o más de 50 líneas por función'),
}
ENFOQUE = {
    1: 'Clases de tareas y utilidades pequeñas: un banco de unas 50 funciones por tanda.',
    2: 'Reenvíos, accesores y datos de plugin; pocas líneas por función, pero hay que simular los destinos.',
    3: 'Estado compartido, pools, cámara, audio o RenderWare; fixtures propios.',
    4: 'Muchas funciones o muy largas; partir en varios bancos y dejar para el final.',
}


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


def fmt(n):
    return f'{n:,}'.replace(',', '.')


def build_document():
    total, rec, val = load_stats()
    per_file = estimate_lines()
    items = []
    for cls in total:
        pending = rec[cls] - val[cls]
        if cls in NO_VALIDABLES and pending > 0:
            pending -= 1
        if pending <= 0:
            continue
        lines = lines_for(cls, per_file)
        lpf = round(lines / max(rec[cls], 1)) if lines else None
        items.append((cls, pending, total[cls] - rec[cls], lpf, total[cls]))
    groups = collections.defaultdict(list)
    for item in items:
        groups[tier(item[1], item[3])].append(item)

    n_total, n_rec, n_val = sum(total.values()), sum(rec.values()), sum(val.values())
    queue = sum(i[1] for i in items)
    incomplete = sum(1 for c in total if rec[c] < total[c])
    today = datetime.date.today().strftime('%d/%m/%Y')

    out = [
        '# Prioridades técnicas de reversing', '',
        '[Inicio](../README.md) · [Progreso registrado](ReversedClasses.md) · [Proceso y continuidad](../reversing/MEJORAS_DEL_PROCESO.md)',
        '<!-- Documento generado por contrib/validation_queue.py (lo llama contrib/sync_docs.py). No editar a mano. -->',
        '',
        f'> **Corte {today}.** Esta guía ordena **qué validar primero**: las funciones ya reconstruidas (`reversed=1`) que '
        'todavía no tienen evidencia aprobada (`validated=0`) en [`hooks.csv`](hooks.csv).',
        '',
        '**Objetivo:** contribuir a una reconstrucción funcionalmente equivalente, validada y progresivamente independiente '
        'del ejecutable original, conforme a la [hoja de ruta](ProjectRoadmap.md). `validated=1` indica evidencia aprobada '
        'vinculada y no acredita por sí sola que siga vigente para el árbol actual.',
        '',
        '| Dimensión | Cifra |', '| --- | ---: |',
        f'| Entradas en el catálogo | {fmt(n_total)} |',
        f'| Reconstruidas | {fmt(n_rec)} |',
        f'| Validadas | {fmt(n_val)} |',
        f'| Reconstruidas sin validar | {fmt(n_rec - n_val)} |',
        f'| De ellas, en esta cola (validables por pares) | {fmt(queue)} |',
        f'| Sin reconstruir (en {incomplete} clases incompletas) | {fmt(n_total - n_rec)} |',
        '',
        'Validar no exige que la clase esté completa: se valida cada función ya reconstruida. '
        '"Sin reconstruir" es informativo y no cuenta como trabajo de esta cola.',
        '',
        '**Navegación:** ' + ' · '.join(f'[{TITLES[k][0]} Nivel {k}](#nivel-{k})' for k in (1, 2, 3, 4))
        + ' · [Fuera de la cola](#fuera-de-la-cola) · [Cómo se ordena](#como-se-ordena)',
        '',
        '| Nivel | Clases | Por validar | Enfoque |', '| --- | :---: | :---: | --- |',
    ]
    for k in (1, 2, 3, 4):
        rows = groups[k]
        out.append(f'| [{TITLES[k][0]} {TITLES[k][1]}](#nivel-{k}) | {len(rows)} | {sum(r[1] for r in rows)} | {ENFOQUE[k]} |')
    out += [f'| **Total** | **{len(items)}** | **{queue}** | |', '']

    for k in (1, 2, 3, 4):
        rows = sorted(groups[k], key=lambda d: (d[1], d[3] if d[3] is not None else 999, d[0]))
        icon, title, rule = TITLES[k]
        out += [f'<a id="nivel-{k}"></a>', '',
                f'## {icon} {title} ({len(rows)} clases, {sum(r[1] for r in rows)} funciones)', '',
                f'Criterio: {rule}.', '',
                '| # | Clase | Funciones totales | Por validar | Sin reconstruir | Líneas/func. |', '|--:|---|--:|--:|--:|--:|']
        for i, (cls, pend, nrev, lpf, tot) in enumerate(rows, 1):
            out.append(f'| {i} | {cls} | {tot} | {pend} | {nrev or ""} | {lpf if lpf is not None else "?"} |')
        out.append('')

    out += ['<a id="fuera-de-la-cola"></a>', '', '## Fuera de la cola (no validables por pares)', '',
            '| Clase | Función | Motivo |', '|---|---|---|']
    for cls, (fn, reason) in NO_VALIDABLES.items():
        out.append(f'| {cls} | `{fn}` | {reason} |')
    out += [
        '', 'Se comprueban de forma indirecta arrancando el juego o con una prueba en partida.', '',
        '<a id="como-se-ordena"></a>', '', '## Cómo se ordena', '',
        'Los niveles salen de una regla fija, no de una opinión:', '',
    ]
    for k in (1, 2, 4, 3):
        out.append(f'- **{TITLES[k][1].split(":")[0]}:** {TITLES[k][2]}.')
    out += [
        '',
        '"Líneas/func." es una **estimación**: líneas del `.cpp` que más define la clase, repartidas entre las clases que '
        'define ese fichero y divididas por las funciones reconstruidas de la clase. No es un coste medido y no promete horas '
        'ni fechas; `?` indica que no se pudo medir. Dentro de cada nivel el orden es por funciones por validar y después '
        'por líneas por función.',
        '',
        'Para regenerar este documento: `python contrib/validation_queue.py`, o `python contrib/sync_docs.py`, que además '
        'sincroniza el resto de Markdown derivado de `hooks.csv`.',
        '',
    ]
    return '\n'.join(out), len(items), queue


def generate(dry_run=False):
    text, n_classes, queue = build_document()
    if not dry_run:
        raw = DOC.read_bytes().decode('utf-8') if DOC.exists() else ''
        nl = '\r\n' if '\r\n' in raw else '\n'
        DOC.write_bytes(text.replace('\n', nl).encode('utf-8'))
    return n_classes, queue


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--dry-run', action='store_true')
    args = parser.parse_args()
    if hasattr(sys.stdout, 'reconfigure'):
        sys.stdout.reconfigure(encoding='utf-8')
    n_classes, queue = generate(args.dry_run)
    print(f'{DOC.name}: {queue} funciones por validar en {n_classes} clases' + (' (dry-run)' if args.dry_run else ''))


if __name__ == '__main__':
    main()
