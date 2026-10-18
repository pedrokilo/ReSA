"""Sincroniza todos los Markdown derivados de docs/hooks.csv en un solo paso.

Antes de este script, docs/ReversedClasses.md y docs/ValidationInventory.md ya
se regeneraban por completo desde hooks.csv (hooks2md.py / validation_inventory.py),
pero docs/ProjectRoadmap.md es texto editorial con
cifras incrustadas a mano: cada vez que una función pasaba a reversed=1 sin correr
este paso, esas dos quedaban con conteos y filas de "pendiente" obsoletos (ocurrió
con CTaskInteriorSitAtDesk, CCarFXRenderer y CAEStaticChannel).

Este script:
  1. Llama a hooks2md.generate() -> docs/ReversedClasses.md (+ comentarios de
     estado en el código fuente, si se pide).
  2. Llama a validation_inventory.generate() -> docs/ValidationInventory.md.
  3. Llama a validation_queue.generate() -> docs/ReversingPriorities.md, la cola de
     validación por niveles (funciones reconstruidas sin validar), generada entera.
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
import pending_wrappers
import validation_inventory
import validation_queue

ROOT = Path(__file__).resolve().parents[1]
HOOKS_CSV = ROOT / 'docs/hooks.csv'

def load_stats():
    with HOOKS_CSV.open(encoding='utf-8-sig', newline='') as stream:
        rows = list(csv.DictReader(stream))
    classes = {}
    for row in rows:
        c = classes.setdefault(row['class'], {'reversed': 0, 'total': 0, 'validated': 0})
        c['total'] += 1
        c['reversed'] += row['reversed'] == '1'
        c['validated'] += row['validated'] == '1'
    total_entries = len(rows)
    reversed_entries = sum(c['reversed'] for c in classes.values())
    validated_entries = sum(1 for r in rows if r['validated'] == '1')
    locked_entries = sum(1 for r in rows if r['locked'] == '1')
    # Las funciones del juego que siguen delegando en el EXE (envolturas-pendientes.csv) impiden que su clase cuente como completa
    # y las clases que solo existen ahí (sin entradas en el catálogo) se suman como clases sin revertir (misma regla que hooks2md.py).
    wrappers_file = HOOKS_CSV.with_name('implementaciones') / 'envolturas-pendientes.csv'
    wrapped = set()
    if wrappers_file.exists():
        with wrappers_file.open(encoding='utf-8-sig', newline='') as stream:
            wrapped = {row['class'] for row in csv.DictReader(stream)}
    total_classes = len(classes) + len([name for name in wrapped if name not in classes])
    complete_classes = sum(1 for n, c in classes.items() if c['reversed'] == c['total'] and n not in wrapped)
    validated_classes = sum(1 for n, c in classes.items() if c['validated'] == c['total'] == c['reversed'] and n not in wrapped)  # de las completas
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
        'validated_classes': validated_classes,
        'pct_validated_classes': round(validated_classes / complete_classes * 100, 2),
        'incomplete_classes': total_classes - complete_classes,
        'pct_reversed': round(reversed_entries / total_entries * 100, 2),
        'pct_classes': round(complete_classes / total_classes * 100, 2),
        'pct_validated': round(validated_entries / reversed_entries * 100, 2),  # de las reconstruidas
    }


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
    text = re.sub(r'\| Clases validadas (en el catálogo|de las completas) \|.*\|\n?', '', text, count=1)
    text = re.sub(r'\| Funciones registradas como reconstruidas \|.*\|\n?', '', text, count=1)
    text = re.sub(r"\| Funciones con evidencia aprobada \(`validated=1`\)( de las reconstruidas)? \|.*\|\n?", '', text, count=1)

    header = '| Dimensión | Progreso | Medición |\n| --- | --- | --- |\n'
    if header not in text:
        raise RuntimeError('ProjectRoadmap.md: no se encontró el encabezado de la tabla de progreso.')
    rows = (
        f"| Clases completas en el catálogo | `{bar(s['pct_classes'])}` | "
        f"{s['complete_classes']}/{s['total_classes']} · {pct_es(s['pct_classes'])}% |\n"
        f"| Clases validadas de las completas | `{bar(s['pct_validated_classes'])}` | "
        f"{s['validated_classes']}/{s['complete_classes']} · {pct_es(s['pct_validated_classes'])}% |\n"
        f"| Funciones registradas como reconstruidas | `{bar(s['pct_reversed'])}` | "
        f"{thousands(s['reversed_entries'])}/{thousands(s['total_entries'])} · {pct_es(s['pct_reversed'])}% |\n"
        f"| Funciones con evidencia aprobada (`validated=1`) de las reconstruidas | `{bar(s['pct_validated'])}` | "
        f"**{thousands(s['validated_entries'])}/{thousands(s['reversed_entries'])} · {pct_es(s['pct_validated'])}%** |\n"
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
        pending_wrappers.main()
        hooks2md.generate(str(HOOKS_CSV), str(ROOT / 'docs/ReversedClasses.md'),
                           update_source_status=args.update_source_status)
        validation_inventory.generate(ROOT)
    else:
        print('[dry-run] Se omiten hooks2md.generate() y validation_inventory.generate().')

    n_classes, n_queue = validation_queue.generate(dry_run=args.dry_run)
    print(f"ReversingPriorities.md: cola de validación regenerada ({n_queue} funciones por validar en {n_classes} clases).")

    sync_project_roadmap(stats, dry_run=args.dry_run)
    print('ProjectRoadmap.md: cifras agregadas sincronizadas (barra de progreso, pendientes, locks).')


if __name__ == '__main__':
    main()
