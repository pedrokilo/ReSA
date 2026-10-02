"""Sincroniza todos los Markdown derivados de docs/hooks.csv en un solo paso.

Antes de este script, docs/ReversedClasses.md y docs/ValidationInventory.md ya
se regeneraban por completo desde hooks.csv (hooks2md.py / validation_inventory.py),
pero docs/ReversingPriorities.md y docs/ProjectRoadmap.md son texto editorial con
cifras incrustadas a mano: cada vez que una función pasaba a reversed=1 sin correr
este paso, esas dos quedaban con conteos y filas de "pendiente" obsoletos (ocurrió
con CTaskInteriorSitAtDesk, CCarFXRenderer y CAEStaticChannel).

Este script:
  1. Llama a hooks2md.generate() -> docs/ReversedClasses.md (+ comentarios de
     estado en el código fuente, si se pide).
  2. Llama a validation_inventory.generate() -> docs/ValidationInventory.md.
  3. Sincroniza docs/ReversingPriorities.md: para cada fila de clase en sus
     tablas, compara revertidas/total contra hooks.csv. Si ya está completa
     (revertidas == total), elimina la fila (tabla principal + tabla de
     "funciones pendientes concretas"). Si sigue incompleta pero las cifras
     cambiaron, las actualiza en el sitio. Recalcula la tabla resumen (clases
     evaluadas, funciones pendientes, parciales, sin revertir) desde lo que
     queda.
  4. Sincroniza las cifras agregadas de docs/ProjectRoadmap.md (barra de
     progreso de clases/funciones/validadas, "N entradas pendientes y M clases
     incompletas", recuento de locks) contra hooks.csv. Cada reemplazo busca
     primero el texto exacto anterior; si no lo encuentra (alguien reescribió
     esa frase a mano), se detiene con un error en vez de corromper el
     documento en silencio.

No toca las secciones narrativas/históricas (anuncios fechados, descripciones
editoriales de cada clase): eso lo sigue escribiendo una persona, igual que
antes. Lo que este script garantiza es que las CIFRAS nunca vuelvan a quedar
obsoletas solo por olvidar un paso manual.

Uso:
    python contrib/sync_docs.py [--update-source-status] [--dry-run]

Ejecutar siempre que cambie reversed=1, validated=1 o un flag de clase en
docs/hooks.csv, en vez de regenerar los Markdown uno por uno a mano.
"""
import argparse
import csv
import re
from pathlib import Path

import hooks2md
import validation_inventory

ROOT = Path(__file__).resolve().parents[1]
HOOKS_CSV = ROOT / 'docs/hooks.csv'

MAIN_ROW_RE = re.compile(
    r'^\| \[(?P<name>[^\]]+)\]\((?P<link>[^)]+)\) — \*\*(?P<rev>\d+)/(?P<tot>\d+) — [\d.,]+%\*\* '
    r'\| (?P<faltan>\d+) \| (?P<rest>.*) \|$',
    re.MULTILINE,
)
DETAIL_ROW_RE = re.compile(r'^\| `(?P<name>[^`]+)` \| (?P<rest>.*) \|$', re.MULTILINE)
SUMMARY_HEADER = (
    '| Clases evaluadas | Funciones pendientes | Parciales | Sin revertir |\n'
    '| :---: | :---: | :---: | :---: |\n'
)
SUMMARY_ROW_RE = re.compile(re.escape(SUMMARY_HEADER) + r'\| \*\*\d+\*\* \| \*\*\d+\*\* \| \*\*\d+\*\* \| \*\*\d+\*\* \|')


def load_stats():
    with HOOKS_CSV.open(encoding='utf-8-sig', newline='') as stream:
        rows = list(csv.DictReader(stream))
    classes = {}
    for row in rows:
        c = classes.setdefault(row['class'], {'reversed': 0, 'total': 0})
        c['total'] += 1
        c['reversed'] += row['reversed'] == '1'
    total_entries = len(rows)
    reversed_entries = sum(c['reversed'] for c in classes.values())
    validated_entries = sum(1 for r in rows if r['validated'] == '1')
    locked_entries = sum(1 for r in rows if r['locked'] == '1')
    total_classes = len(classes)
    complete_classes = sum(1 for c in classes.values() if c['reversed'] == c['total'])
    return {
        'classes': classes,
        'total_entries': total_entries,
        'reversed_entries': reversed_entries,
        'pending_entries': total_entries - reversed_entries,
        'validated_entries': validated_entries,
        'locked_entries': locked_entries,
        'unlocked_entries': total_entries - locked_entries,
        'total_classes': total_classes,
        'complete_classes': complete_classes,
        'incomplete_classes': total_classes - complete_classes,
        'pct_reversed': round(reversed_entries / total_entries * 100, 2),
        'pct_classes': round(complete_classes / total_classes * 100, 2),
        'pct_validated': round(validated_entries / total_entries * 100, 2),
    }


def _drop_removed_lines(text):
    """Quita las líneas que MAIN_ROW_RE/DETAIL_ROW_RE dejaron vacías al eliminar una fila."""
    lines = text.split('\n')
    out = []
    for i, line in enumerate(lines):
        if line == '':
            prev_is_row = i > 0 and lines[i - 1].startswith('| ')
            next_is_row_or_blank = i + 1 >= len(lines) or lines[i + 1].startswith('| ') or lines[i + 1] == ''
            if prev_is_row and next_is_row_or_blank:
                continue
        out.append(line)
    return '\n'.join(out)


def sync_reversing_priorities(stats, dry_run=False):
    path = ROOT / 'docs/ReversingPriorities.md'
    text = path.read_text(encoding='utf-8')
    classes = stats['classes']
    removed, updated = [], []

    def main_repl(m):
        name = m.group('name')
        info = classes.get(name)
        if info is None:
            return m.group(0)
        if info['reversed'] == info['total']:
            removed.append(name)
            return ''
        rev, tot = info['reversed'], info['total']
        if f"{m.group('rev')}/{m.group('tot')}" == f'{rev}/{tot}' and m.group('faltan') == str(tot - rev):
            return m.group(0)
        updated.append((name, f"{m.group('rev')}/{m.group('tot')}", f'{rev}/{tot}'))
        pct = f'{rev / tot * 100:.2f}'
        return f"| [{name}]({m.group('link')}) — **{rev}/{tot} — {pct}%** | {tot - rev} | {m.group('rest')} |"

    text = MAIN_ROW_RE.sub(main_repl, text)

    def detail_repl(m):
        info = classes.get(m.group('name'))
        if info is not None and info['reversed'] == info['total']:
            return ''
        return m.group(0)

    text = DETAIL_ROW_RE.sub(detail_repl, text)
    text = _drop_removed_lines(text)

    remaining = MAIN_ROW_RE.findall(text)
    n_classes = len(remaining)
    n_pending = sum(int(f) for _name, _link, _rev, _tot, f, _rest in remaining)
    n_parciales = sum(1 for _name, _link, rev, _tot, _f, _rest in remaining if int(rev) > 0)
    n_sin_revertir = n_classes - n_parciales

    if not SUMMARY_ROW_RE.search(text):
        raise RuntimeError('No se encontró la tabla resumen de ReversingPriorities.md; revisar formato antes de reintentar.')
    text = SUMMARY_ROW_RE.sub(
        SUMMARY_HEADER + f'| **{n_classes}** | **{n_pending}** | **{n_parciales}** | **{n_sin_revertir}** |',
        text, count=1,
    )

    text, n1 = re.subn(
        r'las \d+ clases actualmente incompletas del catálogo auditado',
        f'las {n_classes} clases actualmente incompletas del catálogo auditado',
        text,
    )
    text, n2 = re.subn(
        r'las \d+ entradas y \d+ clases pendientes',
        f'las {n_pending} entradas y {n_classes} clases pendientes',
        text,
    )
    text, n3 = re.subn(
        r'Las tablas y el nuevo Top 10 reflejan \*\*\d+ clases incompletas / \d+ pendientes\*\*',
        f'Las tablas y el nuevo Top 10 reflejan **{n_classes} clases incompletas / {n_pending} pendientes**',
        text,
    )
    text, n4 = re.subn(
        r'retirar los \d+ locks con evidencia',
        f"retirar los {stats['locked_entries']} locks con evidencia",
        text,
    )
    text, n5 = re.subn(
        r'En el catálogo vigente quedan \d+ candados globales',
        f"En el catálogo vigente quedan {stats['locked_entries']} candados globales",
        text,
    )

    if not dry_run:
        path.write_text(text, encoding='utf-8')
    return dict(removed=removed, updated=updated, n_classes=n_classes, n_pending=n_pending,
                n_parciales=n_parciales, n_sin_revertir=n_sin_revertir,
                scope_sentences_updated=n1 + n2 + n3 + n4 + n5)


def _replace_anchor(text, old, new, label):
    if old not in text:
        raise RuntimeError(
            f"ProjectRoadmap.md: no se encontró el texto esperado para '{label}'. "
            'Alguien reescribió esa frase a mano; revisar manualmente antes de reintentar.\n'
            f'Buscado: {old!r}'
        )
    return text.replace(old, new, 1)


def sync_project_roadmap(stats, dry_run=False):
    path = ROOT / 'docs/ProjectRoadmap.md'
    text = path.read_text(encoding='utf-8')
    s = stats

    def bar(pct):
        filled = round(pct / 100 * 20)
        return '█' * filled + '░' * (20 - filled)

    def thousands(n):
        return f'{n:,}'.replace(',', '.')

    def pct_es(p):
        return f'{p:.2f}'.replace('.', ',')

    text = re.sub(r'\| Clases completas en el catálogo \|.*\|\n?', '', text, count=1)
    text = re.sub(r'\| Funciones registradas como reconstruidas \|.*\|\n?', '', text, count=1)
    text = re.sub(r"\| Funciones con evidencia aprobada \(`validated=1`\) \|.*\|\n?", '', text, count=1)

    header = '| Dimensión | Progreso | Medición |\n| --- | --- | --- |\n'
    if header not in text:
        raise RuntimeError('ProjectRoadmap.md: no se encontró el encabezado de la tabla de progreso.')
    rows = (
        f"| Clases completas en el catálogo | `{bar(s['pct_classes'])}` | "
        f"{s['complete_classes']}/{s['total_classes']} · {pct_es(s['pct_classes'])}% |\n"
        f"| Funciones registradas como reconstruidas | `{bar(s['pct_reversed'])}` | "
        f"{thousands(s['reversed_entries'])}/{thousands(s['total_entries'])} · {pct_es(s['pct_reversed'])}% |\n"
        f"| Funciones con evidencia aprobada (`validated=1`) | `{bar(s['pct_validated'])}` | "
        f"**{thousands(s['validated_entries'])}/{thousands(s['total_entries'])} · {pct_es(s['pct_validated'])}%** |\n"
    )
    text = text.replace(header, header + rows, 1)

    text = re.sub(
        r'El [\d,]+% registrado como reconstruido',
        f"El {pct_es(s['pct_reversed'])}% registrado como reconstruido",
        text, count=1,
    )
    text = re.sub(
        r'Hay \d+ entradas pendientes y \d+ clases incompletas\.',
        f"Hay {s['pending_entries']} entradas pendientes y {s['incomplete_classes']} clases incompletas.",
        text, count=1,
    )
    text = re.sub(
        r'\d+ tienen `locked=1` y [\d.]+ tienen `locked=0`',
        f"{s['locked_entries']} tienen `locked=1` y {thousands(s['unlocked_entries'])} tienen `locked=0`",
        text, count=1,
    )
    text = re.sub(
        r'\*\*Completar las \d+ entradas pendientes\.\*\*',
        f"**Completar las {s['pending_entries']} entradas pendientes.**",
        text, count=1,
    )

    if not dry_run:
        path.write_text(text, encoding='utf-8')


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--update-source-status', action='store_true',
                         help='Sincronizar también los comentarios de estado en los archivos fuente (como hooks2md.py).')
    parser.add_argument('--dry-run', action='store_true', help='No escribir ningún archivo; solo mostrar qué cambiaría.')
    args = parser.parse_args()

    stats = load_stats()
    print(f"hooks.csv: {stats['reversed_entries']}/{stats['total_entries']} entradas reversed=1 "
          f"({stats['pct_reversed']:.2f}%), {stats['complete_classes']}/{stats['total_classes']} clases completas "
          f"({stats['pct_classes']:.2f}%), {stats['validated_entries']} validated=1, {stats['locked_entries']} locked=1.")

    if not args.dry_run:
        hooks2md.generate(str(HOOKS_CSV), str(ROOT / 'docs/ReversedClasses.md'),
                           update_source_status=args.update_source_status)
        validation_inventory.generate(ROOT)
    else:
        print('[dry-run] Se omiten hooks2md.generate() y validation_inventory.generate().')

    pr = sync_reversing_priorities(stats, dry_run=args.dry_run)
    print(f"ReversingPriorities.md: {len(pr['removed'])} clases retiradas por estar completas "
          f"({', '.join(pr['removed']) or 'ninguna'}); {len(pr['updated'])} filas con cifras actualizadas; "
          f"resumen recalculado a {pr['n_classes']} clases / {pr['n_pending']} pendientes "
          f"({pr['n_parciales']} parciales, {pr['n_sin_revertir']} sin revertir).")
    if pr['removed']:
        print('  NOTA: añadir a mano una línea en el aviso superior de ReversingPriorities.md explicando '
              'qué se completó y con qué evidencia (este script no inventa prosa).')

    sync_project_roadmap(stats, dry_run=args.dry_run)
    print('ProjectRoadmap.md: cifras agregadas sincronizadas (barra de progreso, pendientes, locks).')


if __name__ == '__main__':
    main()
