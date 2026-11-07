"""Genera docs/JuegoExe/pendientes-por-clase.md (todas las funciones del EXE aun sin reemplazar, por clase) a partir de docs/JuegoExe/funciones.csv.
Devuelve el resumen por clase para la seccion Parciales de docs/ReversedClasses.md (contrib/hooks2md.py)."""
import csv
from collections import defaultdict
from pathlib import Path

def build(root, fecha):
    src = Path(root) / 'docs' / 'JuegoExe' / 'funciones.csv'
    if not src.exists():
        return []
    rows = list(csv.DictReader(src.open(encoding='utf-8-sig', newline='')))
    miss = [x for x in rows if x['estado'] in ('A', 'B', 'C', 'D', 'envoltura')]
    g = defaultdict(list)
    for x in miss:
        g[x['modulo'] or '(sin clase)'].append(x)
    order = sorted(g, key=lambda m: (sum(int(x['bytes']) for x in g[m]), len(g[m]), m.casefold()))
    n_wrap = sum(x['estado'] == 'envoltura' for x in miss)
    L = ['# Funciones pendientes del EXE, por clase', '',
         f'Generado de `docs/JuegoExe/funciones.csv` ({fecha}). Total: **{len(miss):,}** funciones en {len(order)} clases/módulos (rango 0x401000–0x74FFFF): '
         f'{len(miss) - n_wrap:,} sin reemplazar (A–D) + {n_wrap} envolturas pendientes. Ordenado de menor a mayor tamaño (bytes pendientes). Se resume en la [sección 11 de ReversedClasses](../ReversedClasses.md#parciales).', '',
         'Estados: **A** llamada desde código ya en C++ · **B** método virtual · **C** solo la llaman funciones pendientes · **D** sin llamadas ni vtable · **envoltura** ya tiene hook pero llama al EXE.', '',
         'Aviso: la clase se atribuye por cercanía de direcciones y puede ser errónea cuando la confianza es baja; un nombre de catálogo sí es fiable.', '',
         '## Índice', '', '| Clase | Funciones | Bytes |', '|---|---:|---:|']
    summary = []
    for m in order:
        b = sum(int(x['bytes']) for x in g[m])
        L.append(f'| {m} | {len(g[m])} | {b} |')
        big = max(int(x['bytes']) for x in g[m])
        summary.append((m, len(g[m]), b, big))
    for m in order:
        xs = sorted(g[m], key=lambda x: x['va'])
        L += ['', f'## {m} ({len(xs)})', '',
              '| Dirección | Bytes | Estado | Nombre catálogo | Llamadas | Desde C++ | Vtable | Confianza clase |', '|---|---:|---|---|---:|---:|---|---|']
        for x in xs:
            L.append(f'| {x["va"]} | {x["bytes"]} | {x["estado"]} | {x["nombre_catalogo"]} | {x["llamadas_directas"]} | {x["llamadas_desde_reemplazadas"]} | {x["en_vtable"]} | {x["confianza_modulo"]} |')
    (Path(root) / 'docs' / 'JuegoExe' / 'pendientes-por-clase.md').write_text('\n'.join(L) + '\n', encoding='utf-8', newline='\n')
    return summary
