"""Secciones de docs/ReversedClasses.md sobre el trabajo real que falta: lotes medidos y lista viva del grupo A.

Datos: docs/JuegoExe/funciones.csv (estado de cada función del juego, lo regenera contrib/game_function_map.py), docs/JuegoExe/grupoA.csv
(categoría, llamadores y llamadas directas de las funciones del grupo A en el momento del análisis), docs/JuegoExe/lotes.csv (lotes cerrados
con su coste medido, a mano) y docs/JuegoExe/lotes-en-curso.csv (direcciones con lote asignado que aún no está validado, a mano).
"""
import csv
from collections import defaultdict
from pathlib import Path


def _read(path):
    path = Path(path)
    if not path.exists():
        return []
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def _hex(value):
    try:
        return int(str(value).strip(), 16)
    except ValueError:
        return 0


def _cell(text):
    return str(text).replace('|', '\\|').replace('<', '&lt;').replace('>', '&gt;')


def load_lots(root):
    """Lotes cerrados de docs/JuegoExe/lotes.csv."""
    return _read(Path(root) / 'docs/JuegoExe/lotes.csv')


def category_of(row):
    """Categoría de coste del grupo A (docs/AnalisisGrupoA.md, de menor a mayor coste): 1 triviales y thunks, 2 pequeñas sin FPU,
    3 lógica sin FPU, 4 con FPU, 5 de 0x300 B o más."""
    if row['tamano'] == '>=0x300':
        return 5
    if row['tipo'].startswith('trivial') or row['tipo'] == 'thunk':
        return 1
    if int(row['fpu'] or 0) > 0:
        return 4
    return 2 if row['tipo'] == 'pequena' else 3


CATEGORY_NAMES = {
    1: 'Triviales y thunks (≤ 0x20 B sin llamadas, o un solo `jmp`)',
    2: 'Pequeñas sin FPU (≤ 0x40 B, ≤ 1 llamada)',
    3: 'Lógica sin FPU (< 0x300 B)',
    4: 'Con FPU (método x87 a 24 bits; < 0x300 B)',
    5: 'De 0x300 B o más (una por lote)',
}


def lots_table(root, fecha=''):
    """Tabla de costes medidos por categoría de lote (lotes 90A, 90B, 92A–92C)."""
    lots = load_lots(root)
    if not lots:
        return []
    lines = [f'Costes medidos en los lotes cerrados (datos al {fecha}, [lotes.csv](JuegoExe/lotes.csv)). Cada lote reconstruye en C++ puro funciones que antes se llamaban al EXE por dirección y las valida dentro del juego (pares EXE/DLL con cero diferencias).', '',
             '| Lote | Categoría de coste (del grupo A) | Funciones | Bytes | Pares validados | Coste medido |', '| --- | --- | ---: | ---: | ---: | --- |']
    for r in lots:
        lines.append(f"| {_cell(r['lote'])} | {_cell(r['categoria'])} | {r['funciones']} | {int(r['bytes']):,} | {int(r['pares']):,} | {_cell(r['coste_medido'])} |")
    total_f = sum(int(r['funciones']) for r in lots)
    total_b = sum(int(r['bytes']) for r in lots)
    lines.append(f"| **Total** | | **{total_f}** | **{total_b:,}** | **{sum(int(r['pares']) for r in lots):,}** | Solo hay tiempo de pared de las categorías 2, 3 y 5; la 1 no se cronometró y la 4 (FPU) no se ha medido. |")
    return lines


def group_a_section(root, number='', fecha='', a_total=None):
    """Sección «Dependencias directas de la DLL al EXE (grupo A)»: composición y lista viva."""
    root = Path(root)
    functions = _read(root / 'docs/JuegoExe/funciones.csv')
    snapshot = {r['va']: r for r in _read(root / 'docs/JuegoExe/grupoA.csv')}
    if not functions or not snapshot:
        return []
    live = [r for r in functions if r['estado'] == 'A']
    in_progress = defaultdict(set)
    for r in _read(root / 'docs/JuegoExe/lotes-en-curso.csv'):
        in_progress[_hex(r['direccion'])].add(r['lote'])
    rows = [(r, snapshot[r['va']]) for r in live if r['va'] in snapshot]
    direct = [(r, s) for r, s in rows if s['directo'] == 'si']
    gone = len(snapshot) - len(rows)
    lines = ['<a id="grupo-a"></a>', '', f'## {number}Dependencias directas de la DLL al EXE (grupo A)', '',
             f'Lista viva del trabajo real. Al {fecha}, el grupo A de [JuegoExe](JuegoExe/README.md) tiene **{len(live):,} funciones del juego** ({sum(int(r["bytes"]) for r in live):,} bytes) que la DLL ya necesita: '
             f'{len(direct):,} se llaman **directamente** desde código ya reemplazado (llamadas detectadas con dirección, bytes y llamador) y {len(rows) - len(direct):,} solo de forma transitiva, a través de otras funciones aún no reemplazadas. '
             f'La categoría y los llamadores salen del análisis de [AnalisisGrupoA.md](AnalisisGrupoA.md) ([grupoA.csv](JuegoExe/grupoA.csv), {len(snapshot):,} funciones en el momento del análisis; {gone} ya no son del grupo A: se reconstruyeron desde entonces o pasaron a envoltura abierta). '
             + (f'El cierre transitivo de la [sección 6](#que-falta) cuenta {a_total:,} funciones: la diferencia ({a_total - len(live):,}) son funciones del grupo A que además tienen una envoltura abierta y figuran como envolturas abiertas ([sección 7](#no-en-cpp) y bibliotecas), no en esta lista. ' if a_total else '')
             + 'Es una cota inferior: las llamadas indirectas sin resolver no entran en el grafo.', '',
             '### Composición por categoría de coste', '',
             '| Categoría de coste | Funciones | % de funciones del grupo A | Bytes | % de bytes del grupo A | Directas desde código reemplazado | Lote medido |',
             '| --- | ---: | ---: | ---: | ---: | ---: | --- |']
    lots = load_lots(root)
    total_f = len(rows) or 1
    total_b = sum(int(r['bytes']) for r, _ in rows) or 1
    for cat, name in CATEGORY_NAMES.items():
        selected = [(r, s) for r, s in rows if category_of(s) == cat]
        measured = ', '.join(l['lote'] for l in lots if l['categoria'].startswith(f'{cat}.')) or 'sin medir'
        b = sum(int(r['bytes']) for r, _ in selected)
        lines.append(f"| {cat}. {name} | {len(selected):,} | {len(selected)/total_f:.1%} | {b:,} | {b/total_b:.1%} | {sum(s['directo'] == 'si' for _, s in selected):,} | {measured} |")
    lines += ['', '### Llamadas directas detectadas', '',
              'Ordenadas por número de sitios de llamada desde código ya reemplazado (los más llamados ganan más por función) y, a igualdad, de mayor a menor tamaño. '
              '«Llamadores» son los primeros llamadores del código ya reemplazado (nombre del catálogo o dirección); «Lote» es el lote en curso o asignado que la reconstruye ([lotes-en-curso.csv](JuegoExe/lotes-en-curso.csv)); «—» significa sin lote asignado. '
              'Los lotes ya validados (90A, 90B, 92A–92C) dejaron de figurar porque estas funciones ya no están pendientes.', '',
              '<details>', f'<summary>Llamadas directas del grupo A pendientes ({len(direct):,}), de más a menos llamadas</summary>', '',
              '| Dirección | Bytes | Módulo estimado | Categoría | Llamadas desde código reemplazado | Llamadores | Lote |', '| --- | ---: | --- | ---: | ---: | --- | --- |']
    for r, s in sorted(direct, key=lambda x: (-int(x[1]['n_llamadores_repl'] or 0), -int(x[0]['bytes']), x[0]['va'])):
        callers = s['llamadores_repl'].split()
        extra = int(s['n_llamadores_repl'] or 0) - 3
        shown = ', '.join(f'`{_cell(c)}`' for c in callers[:3]) + (f' (+{extra} más)' if extra > 0 else '')
        lot = ' / '.join(sorted(in_progress.get(_hex(r['va']), ()))) or '—'
        lines.append(f"| `{r['va']}` | {int(r['bytes']):,} | {_cell(r['modulo'])} ({r['confianza_modulo']}) | {category_of(s)} | {s['n_llamadores_repl']} | {shown} | {lot} |")
    lines += ['', '</details>', '']
    return lines
