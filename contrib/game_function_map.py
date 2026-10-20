"""Mapa de las funciones del código del juego (0x401000–0x74FFFF) en docs/JuegoExe/.

Genera una lista completa (funciones.csv) y vistas Markdown: resumen, por bloque de direcciones, por módulo estimado y
una lista por grupo de lo que falta. Parte de contrib/exe_coverage.py (mismos datos y mismas cotas).
El «módulo estimado» de una función aún no reemplazada es una HEURÍSTICA: el código del juego está enlazado por unidad de
compilación, así que se toma la clase de las funciones ya reemplazadas más cercanas (si las vecinas anterior y siguiente
coinciden la confianza es «alta», si no «baja»). No es una identificación.
"""
import bisect
import csv
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import exe_coverage as ec  # noqa: E402

STATUS = {
    'reemplazada': 'Reemplazada por C++',
    'envoltura': 'Envoltura pendiente (la DLL aún llama al EXE)',
    'A': 'A · Falta: llamada desde código ya reemplazado',
    'B': 'B · Falta: método virtual',
    'C': 'C · Falta: solo la llaman funciones aún no reemplazadas',
    'D': 'D · Falta: sin llamadas ni vtable (¿muerto o por puntero?)',
}
ORDER = ['A', 'B', 'C', 'D', 'envoltura', 'reemplazada']
BLOCK = 0x10000


def _cls(name):
    return name.split('::')[0] if '::' in name else name


def build(root):
    root = Path(root)
    out, st = ec.compute(root, internals=True)
    funcs, covered, pending, groups = st['funcs'], st['covered'], st['pending'], st['groups']
    game = sorted(va for va in funcs if ec.region(va) == 'juego')
    group_of = {va: k for k, v in groups.items() for va in v}

    callers_total, callers_repl = Counter(), Counter()
    with open(root / 'docs/MemoryMap/binary_calls.csv', encoding='utf-8-sig', newline='') as f:
        for r in csv.DictReader(f):
            if r['kind'] != 'direct':
                continue
            caller, target = ec._hex(r['caller_va']), ec._hex(r['target_va'])
            callers_total[target] += 1
            if caller in covered:
                callers_repl[target] += 1

    known = [(va, _cls(funcs[va]['names'][0])) for va in game if va in covered and funcs[va]['names']]
    keys = [k for k, _ in known]

    def module(va):
        if va in covered and funcs[va]['names']:
            return _cls(funcs[va]['names'][0]), 'exacta'
        i = bisect.bisect_left(keys, va)
        prev = known[i - 1][1] if i > 0 else None
        nxt = known[i][1] if i < len(known) else None
        if prev and prev == nxt:
            return prev, 'alta'
        return (prev or nxt or '?'), 'baja'

    rows = []
    for va in game:
        f = funcs[va]
        if va in pending:
            status = 'envoltura'
        elif va in covered:
            status = 'reemplazada'
        else:
            status = group_of.get(va, 'D')
        mod, conf = module(va)
        rows.append(dict(va=va, size=f['size'], status=status, module=mod, confidence=conf,
                         name=';'.join(f['names'][:3]), callers=callers_total[va], callers_replaced=callers_repl[va],
                         thunk='sí' if f['thunk'] else '', vtable='sí' if va in st['vtable_targets'] else ''))
    return out, rows


def _tbl(head, body):
    return ['| ' + ' | '.join(head) + ' |', '| ' + ' | '.join('---:' if i else '---' for i in range(len(head))) + ' |'] + \
           ['| ' + ' | '.join(str(c) for c in r) + ' |' for r in body]


def write(root):
    root = Path(root)
    out, rows = build(root)
    d = root / 'docs/JuegoExe'
    d.mkdir(exist_ok=True)
    n, nb = len(rows), sum(r['size'] for r in rows)

    with open(d / 'funciones.csv', 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f)
        w.writerow(['va', 'bytes', 'estado', 'modulo', 'confianza_modulo', 'nombre_catalogo', 'llamadas_directas', 'llamadas_desde_reemplazadas', 'thunk', 'en_vtable'])
        for r in rows:
            w.writerow([f"0x{r['va']:08X}", r['size'], r['status'], r['module'], r['confidence'], r['name'], r['callers'], r['callers_replaced'], r['thunk'], r['vtable']])

    by = defaultdict(list)
    for r in rows:
        by[r['status']].append(r)
    L = ['# Código del juego en el EXE: mapa de las %s funciones' % f'{n:,}', '',
         '[Progreso del reversing](../ReversedClasses.md#cobertura-exe) · [Lista completa (CSV)](funciones.csv)', '',
         f'Región **0x401000–0x74FFFF**: {n:,} funciones, {nb:,} bytes. Es el código propio del juego y la capa de plataforma; RenderWare, D3D y el CRT quedan fuera '
         '(se sustituyen enlazando bibliotecas equivalentes). Generado por `contrib/game_function_map.py`; cotas y método en `contrib/exe_coverage.py`.', '',
         '## Estado', '']
    body = []
    for k in ORDER:
        v = by[k]
        body.append([STATUS[k], f'{len(v):,}', f'{len(v)/n:.1%}', f'{sum(x["size"] for x in v):,}', f'{sum(x["size"] for x in v)/nb:.1%}'])
    L += _tbl(['Estado', 'Funciones', '% func.', 'Bytes', '% bytes'], body)
    L += ['', 'Las funciones de A–D son **lo que falta**. El grupo A es el piso para la independencia (la DLL ya depende de ellas); es una cota inferior porque '
          f'hay {out["indirect_call_sites"]:,} llamadas indirectas sin resolver. Las envolturas pendientes (que ya tienen hook pero llaman al EXE) se cuentan aparte, por eso A–D suman menos que en la tabla de cobertura de ReversedClasses. Listas: ' +
          ' · '.join(f'[{k}](pendientes-{k}.md)' for k in 'ABCD') + '.', '',
          '## Por bloque de direcciones (64 KB)', '',
          'El código se enlaza por unidad de compilación, así que cada bloque suele corresponder a pocos módulos. Útil para ver dónde queda trabajo.', '']
    blocks = defaultdict(list)
    for r in rows:
        blocks[r['va'] // BLOCK].append(r)
    body = []
    for b in sorted(blocks):
        v = blocks[b]
        rep = sum(1 for x in v if x['status'] == 'reemplazada')
        miss = [x for x in v if x['status'] in 'ABCD']
        top = Counter(x['module'] for x in miss if x['confidence'] != 'baja').most_common(3)
        body.append([f'0x{b*BLOCK:06X}', f'{len(v):,}', f'{rep/len(v):.0%}', f'{len(miss):,}', f'{sum(x["size"] for x in miss):,}',
                     ', '.join(f'{m} ({c})' for m, c in top) or '—'])
    L += _tbl(['Bloque', 'Funciones', 'Reemplazadas', 'Faltan', 'Bytes que faltan', 'Módulos con más faltantes (estimado)'], body)
    L += ['', '## Por módulo (estimado)', '',
          'Módulos con más bytes por reconstruir. Para funciones sin nombre el módulo es la heurística de vecindad descrita arriba; la columna «Confianza alta» cuenta las faltantes cuyo '
          'módulo coincide en ambos vecinos.', '']
    mods = defaultdict(lambda: [0, 0, 0, 0, 0])
    for r in rows:
        m = mods[r['module']]
        m[0] += 1
        if r['status'] == 'reemplazada':
            m[1] += 1
        elif r['status'] in 'ABCD':
            m[2] += 1
            m[3] += r['size']
            m[4] += r['confidence'] == 'alta'
    top = sorted(mods.items(), key=lambda kv: -kv[1][3])[:60]
    L += _tbl(['Módulo', 'Funciones', 'Reemplazadas', 'Faltan', 'Bytes que faltan', 'Confianza alta'],
              [[m, f'{v[0]:,}', f'{v[1]:,}', f'{v[2]:,}', f'{v[3]:,}', f'{v[4]:,}'] for m, v in top])
    L += ['', f'Mostrados los 60 primeros de {len(mods):,} módulos; el resto está en el CSV.', '',
          '## Cómo usarlo', '',
          '- Filtra `funciones.csv` por `estado` = `A` y ordena por `llamadas_desde_reemplazadas` para empezar por lo más usado por código ya reconstruido.',
          '- Una función de 16 bytes o menos suele ser un stub o thunk; mira `thunk` antes de tratarla como trabajo.',
          '- Los grupos C y D pueden contener código muerto: confirma con análisis dinámico antes de reconstruir.', '']
    (d / 'README.md').write_text('\n'.join(L), encoding='utf-8', newline='\n')

    for k in 'ABCD':
        v = sorted(by[k], key=lambda x: (-x['callers_replaced'], -x['size']))
        P = [f'# Grupo {k}: {STATUS[k].split("· ")[1]}', '', '[Mapa](README.md) · [Lista completa](funciones.csv)', '',
             f'{len(v):,} funciones, {sum(x["size"] for x in v):,} bytes. Orden: más llamadas desde código ya reemplazado primero, luego más grandes.', '']
        P += _tbl(['Dirección', 'Bytes', 'Módulo estimado', 'Confianza', 'Llamadas (desde reemplazadas)', 'Vtable'],
                  [[f'`0x{x["va"]:08X}`', x['size'], x['module'], x['confidence'], f'{x["callers"]} ({x["callers_replaced"]})', x['vtable']] for x in v])
        (d / f'pendientes-{k}.md').write_text('\n'.join(P) + '\n', encoding='utf-8', newline='\n')
    return n


if __name__ == '__main__':
    print(write(Path(__file__).resolve().parents[1]), 'funciones')
