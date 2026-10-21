"""Cobertura real del EXE: cuántas funciones y bytes del original ya están reemplazados por C++ y qué falta.

Las barras de docs/ReversedClasses.md miden filas del catálogo (hooks.csv). Este módulo mide el propio EXE con el análisis estático de
docs/MemoryMap (Ghidra: 17.765 entradas de función, 3,72 MB de cuerpos), cruzado con hooks.csv, el registro de validación y las
envolturas pendientes. Es una estimación con cotas explícitas, no una certificación de independencia:

- La partición por regiones es por dirección (el EXE enlaza estáticamente RenderWare, D3D y el CRT de MSVC al final del .text).
  Los límites salen de las 963 direcciones que el proyecto ya clasificó como «juego» o «biblioteca».
- Las llamadas indirectas (6.004 sitios sin resolver) no están en el grafo: todo cierre transitivo es una cota INFERIOR.
- Una función cuenta como reemplazada si su dirección tiene fila reversed=1 en hooks.csv, o si su nombre de catálogo está reversed=1 (los
  virtuales figuran con dirección 0x00000000), salvo que siga siendo una envoltura pendiente que llama al EXE.
"""
import csv
import json
from collections import defaultdict
from pathlib import Path

# Clases del catálogo que son bibliotecas enlazadas en el EXE (RenderWare y CRT): ReversedClasses.md no las cuenta; solo aparecen en la tabla de cobertura.
LIBRARY_CLASSES = frozenset({'CRT', 'RpHAnim', 'RpSkin', 'RpUVAnim', 'RpWorld', 'RtAnim', 'RtDict', 'RwCore', 'RwPlCore'})

# Límites de región (ver la nota del módulo).
GAME_END = 0x750000      # 0x401000..0x74FFFF: código del juego y capa de plataforma (Win, input, D3D de arranque)
RW_END = 0x820000        # 0x750000..0x81FFFF: RenderWare y envoltorios D3D
REGIONS = (('juego', 'Juego y plataforma (0x401000–0x74FFFF)'),
           ('renderware', 'RenderWare y D3D (0x750000–0x81FFFF)'),
           ('crt', 'CRT/MSVC y runtime (0x820000 en adelante)'))


def region(va):
    if va < GAME_END:
        return 'juego'
    return 'renderware' if va < RW_END else 'crt'


def _rows(path):
    with open(path, encoding='utf-8-sig', newline='') as stream:
        yield from csv.DictReader(stream)


def _hex(value):
    try:
        return int(str(value).strip(), 16)
    except (ValueError, AttributeError):
        return 0


def function_sizes(root):
    """{direccion: bytes} de cada funcion del analisis estatico (docs/MemoryMap/binary_functions.csv)."""
    path = Path(root) / 'docs/MemoryMap/binary_functions.csv'
    if not path.exists():
        return {}
    return {_hex(r['va']): int(r['body_bytes'] or 0) for r in _rows(path)}


def size_label(size):
    """Etiqueta de tamano por bytes de cuerpo de la funcion (instrucciones x86 de 1 a 15 bytes: ~3-4 bytes por instruccion de media)."""
    if size is None:
        return 'sin dato'
    if size >= 2000:
        return 'muy grande'
    if size >= 500:
        return 'grande'
    if size >= 100:
        return 'mediana'
    return 'pequeña'


def compute(root, internals=False):
    root = Path(root)
    mm = root / 'docs/MemoryMap'
    funcs = {}
    for r in _rows(mm / 'binary_functions.csv'):
        va = _hex(r['va'])
        funcs[va] = dict(size=int(r['body_bytes'] or 0), thunk=r['thunk'] == 'true',
                         names=[n for n in r['catalogue_names'].split(';') if n],
                         incoming=int(r['direct_call_sites_to_entry'] or 0))

    hooks = list(_rows(root / 'docs/hooks.csv'))
    reversed_rows = [r for r in hooks if r['reversed'] == '1']
    addresses = {_hex(r['address']) for r in reversed_rows} - {0}
    names = {f"{r['class']}::{r['fn_name'].split('-')[0]}" for r in reversed_rows}

    registry = root / 'reversing/validation-registry.json'
    if registry.exists():
        for entry in json.loads(registry.read_text(encoding='utf-8')).get('functions', []):
            hook = entry.get('hook', {})
            if _hex(hook.get('address')) == 0:
                original = _hex(entry.get('evidence', {}).get('original_address', ''))
                if original:
                    addresses.add(original)

    pending = {_hex(r.get('address', '')) for r in _rows(root / 'docs/implementaciones/envolturas-pendientes.csv')} - {0}
    covered = ({va for va in funcs if va in addresses or any(n in names for n in funcs[va]['names'])}) - pending

    calls = defaultdict(set)
    indirect = 0
    for r in _rows(mm / 'binary_calls.csv'):
        if r['kind'] == 'direct':
            calls[_hex(r['caller_va'])].add(_hex(r['target_va']))
        else:
            indirect += 1
    vtable_targets = {_hex(r['function_va']) for r in _rows(mm / 'binary_slots.csv')}

    # Dependencias directas: código NO reemplazado que llama el código ya reemplazado; y su cierre transitivo.
    direct = {t for c in covered for t in calls.get(c, ()) if t in funcs and t not in covered}
    reach, stack = set(direct), list(direct)
    while stack:
        for t in calls.get(stack.pop(), ()):
            if t in funcs and t not in covered and t not in reach:
                reach.add(t)
                stack.append(t)

    out = {'ghidra_entries': len(funcs), 'body_bytes': sum(f['size'] for f in funcs.values()), 'indirect_call_sites': indirect,
           'catalogue_rows_reversed': len(reversed_rows), 'distinct_exe_functions_replaced': len(covered),
           'pending_wrapper_addresses': len(pending), 'regions': {}}
    for key, _ in REGIONS:
        vas = [va for va in funcs if region(va) == key]
        out['regions'][key] = dict(functions=len(vas), bytes=sum(funcs[v]['size'] for v in vas),
                                   replaced=sum(1 for v in vas if v in covered), replaced_bytes=sum(funcs[v]['size'] for v in vas if v in covered),
                                   direct_dependencies=sum(1 for v in vas if v in direct), reachable=sum(1 for v in vas if v in reach),
                                   reachable_bytes=sum(funcs[v]['size'] for v in vas if v in reach))
    game = [va for va in funcs if region(va) == 'juego' and va not in covered]
    not_reach = [va for va in game if va not in reach]
    groups = {'A': [va for va in game if va in reach],
              'B': [va for va in not_reach if va in vtable_targets],
              'C': [va for va in not_reach if va not in vtable_targets and funcs[va]['incoming'] > 0],
              'D': [va for va in not_reach if va not in vtable_targets and funcs[va]['incoming'] == 0]}
    out['game_remaining'] = {k: dict(functions=len(v), bytes=sum(funcs[x]['size'] for x in v)) for k, v in groups.items()}
    out['game_remaining_total'] = dict(functions=len(game), bytes=sum(funcs[x]['size'] for x in game))
    out['game_tiny_helpers_le16'] = sum(1 for va in funcs if region(va) == 'juego' and funcs[va]['size'] <= 16)
    out['direct_dependencies_total'] = len(direct)
    if internals:
        return out, dict(funcs=funcs, covered=covered, pending=pending, reach=reach, direct=direct, vtable_targets=vtable_targets, groups=groups)
    return out


def _bar(part, whole, width=20):
    n = round(width * part / whole) if whole else 0
    return '█' * n + '░' * (width - n)


def section(root):
    """Líneas Markdown de la sección «Cobertura real del EXE»; vacío si faltan los datos del análisis estático."""
    if not (Path(root) / 'docs/MemoryMap/binary_functions.csv').exists():
        return []
    d = compute(root)
    reg = d['regions']
    names = dict(REGIONS)
    lines = ['<a id="cobertura-exe"></a>', '', '## Cobertura real del EXE (estimación)', '',
             'Esta tabla es la única parte del informe que habla de lo que no es código del juego: sirve para aclarar qué queda fuera de la meta. Las dos primeras barras de progreso salen de aquí (región del juego); las demás miden filas del catálogo (`hooks.csv`). Se mide el propio ejecutable original con el análisis estático '
             f'([MemoryMap](MemoryMap/README.md): {d["ghidra_entries"]:,} entradas de función detectadas y {d["body_bytes"]/1e6:.2f} MB de cuerpos de función), cruzado con '
             f'el catálogo, el registro de validación y las envolturas pendientes. {d["catalogue_rows_reversed"]:,} filas reconstruidas equivalen a **{d["distinct_exe_functions_replaced"]:,} funciones distintas del EXE '
             f'ya reemplazadas** (varias filas son sobrescrituras virtuales que comparten cuerpo).', '',
             '| Región del EXE | Funciones | Bytes de código | Reemplazadas por C++ (funciones) | Reemplazadas (bytes) | Funciones llamadas directamente desde código ya reemplazado |', '| --- | ---: | ---: | --- | --- | ---: |']
    tf = tb = rf = rb = 0
    for key in sorted(reg, key=lambda k: -reg[k]['replaced'] / reg[k]['functions']):
        r = reg[key]
        lines.append(f"| {names[key]} | {r['functions']:,} | {r['bytes']:,} | `{_bar(r['replaced'], r['functions'])}` {r['replaced']:,} · {r['replaced']/r['functions']:.1%} | "
                     f"`{_bar(r['replaced_bytes'], r['bytes'])}` {r['replaced_bytes']/r['bytes']:.1%} | {r['direct_dependencies']:,} |")
        tf += r['functions']; tb += r['bytes']; rf += r['replaced']; rb += r['replaced_bytes']
    lines.append(f"| **Total** | **{tf:,}** | **{tb:,}** | `{_bar(rf, tf)}` {rf:,} · {rf/tf:.1%} | `{_bar(rb, tb)}` {rb/tb:.1%} | **{d['direct_dependencies_total']:,}** |")
    lines += ['', '**Qué significa para un ejecutable independiente.** No hace falta reconstruir las ' f'{tf:,} funciones: RenderWare/D3D y el CRT son bibliotecas '
              f'enlazadas dentro del EXE ({reg["renderware"]["functions"] + reg["crt"]["functions"]:,} entradas, {(reg["renderware"]["bytes"] + reg["crt"]["bytes"])/tb:.0%} de los bytes) que se sustituirían '
              'enlazando una implementación equivalente, no reescribiéndolas desde el original. El trabajo propio del proyecto es el código del juego '
              f'({reg["juego"]["functions"]:,} funciones, {reg["juego"]["bytes"]/tb:.0%} de los bytes), y de él ya está reemplazado el **{reg["juego"]["replaced_bytes"]/reg["juego"]["bytes"]:.0%} de los bytes** '
              f'({reg["juego"]["replaced"]/reg["juego"]["functions"]:.0%} de las funciones: las que quedan son más pequeñas que la media). Entre las {reg["juego"]["functions"]:,} entradas del juego hay {d["game_tiny_helpers_le16"]:,} de 16 bytes o menos (stubs, thunks y helpers).', '']
    g, t = d['game_remaining'], d['game_remaining_total']
    lines += [f'### Lo que falta del código del juego: {t["functions"]:,} funciones, {t["bytes"]/1e3:,.0f} KB', '',
              'Mapa completo de las funciones del juego, con lista por estado, bloque de direcciones y módulo estimado: [JuegoExe](JuegoExe/README.md).', '',
              '| Grupo (de mayor a menor tamaño) | Funciones | Bytes | Lectura |', '| --- | ---: | ---: | --- |',
              *[row for _, row in sorted(((g[k]['bytes'], line) for k, line in (
                  ('D', f'| D. Sin llamadas directas ni vtable | {g["D"]["functions"]:,} | {g["D"]["bytes"]:,} | Código muerto o alcanzado por tablas de punteros, callbacks o scripts: hay que revisarlo, no contarlo sin más. |'),
                  ('C', f'| C. Llamadas directas solo desde código aún no reemplazado | {g["C"]["functions"]:,} | {g["C"]["bytes"]:,} | Dependen de reconstruir antes A y B; parte puede ser código muerto. |'),
                  ('B', f'| B. Métodos virtuales (en una vtable) no alcanzados por llamada directa | {g["B"]["functions"]:,} | {g["B"]["bytes"]:,} | Probablemente necesarios: se ejecutan por despacho virtual. |'),
                  ('A', f'| A. Llamadas directamente desde código ya reemplazado (cierre transitivo) | {g["A"]["functions"]:,} | {g["A"]["bytes"]:,} | **Trabajo necesario**: la DLL sigue dependiendo de ellas. Es el piso para la independencia. |'),
              )), key=lambda kv: -kv[0])], '',
              f'Del código del juego ya reemplazado, **{reg["juego"]["direct_dependencies"]:,} funciones del juego siguen llamándose directamente desde el EXE**. Esa es la lista corta de dependencias reales que hay que eliminar. Las que caen en RenderWare/D3D o el CRT se muestran solo en la última columna de la tabla de cobertura.', '',
              f'> [!NOTE]\n> **Cotas y límites.** Es un análisis estático: hay {d["indirect_call_sites"]:,} llamadas indirectas sin resolver que no están en el grafo, así que A es una **cota inferior** del trabajo. '
              'La partición por regiones usa la dirección, no una clasificación función a función. «Reemplazada» significa que el hook existe y pasó su banco; no acredita independencia del EXE ni '
              'que la validación siga vigente para el árbol actual. Se regenera con `contrib/exe_coverage.py` al sincronizar la documentación.', '']
    lines += _pending_list(root)
    return lines


def _anchor(name):
    return 'env-' + ''.join(ch.lower() if ch.isalnum() else '-' for ch in name).strip('-')


def _pending_list(root):
    """Funciones catalogadas que aún llaman al EXE (envolturas): son las que «tenemos» pero todavía no están en C++."""
    path = Path(root) / 'docs/implementaciones/envolturas-pendientes.csv'
    if not path.exists():
        return []
    sizes = function_sizes(root)
    groups = defaultdict(list)
    for r in _rows(path):
        addr = _hex(r['address'])
        groups[r['class']].append((r['function'], r['address'], sizes.get(addr) if addr else None))

    def total(fns):
        return sum(f[2] or 0 for f in fns)

    def biggest(fns):
        known = [f[2] for f in fns if f[2] is not None]
        return max(known) if known else None

    def order(item):
        return (-total(item[1]), item[0])

    def fn_text(fn):
        name, addr, size = fn
        return f'`{name}` {addr} · {size:,} B' if size is not None else f'`{name}` {addr or "sin dirección"}'

    def kind(cls):
        if not cls.startswith('(biblioteca)'):
            return 'juego'
        return 'crt' if ('CRT' in cls or 'QuickTime' in cls) else 'renderware'

    parts = {'juego': ('Código del juego', {}), 'renderware': ('RenderWare y D3D (biblioteca)', {}), 'crt': ('CRT y runtime (biblioteca)', {})}
    for cls, fns in groups.items():
        parts[kind(cls)][1][cls] = fns
    library_total = counts_lib = sum(len(v) for k in ('renderware', 'crt') for v in parts[k][1].values())
    parts['renderware'] = parts['renderware'][0], {}
    parts['crt'] = parts['crt'][0], {}
    total_fns = sum(len(v) for v in parts['juego'][1].values())
    counts = {k: sum(len(v) for v in parts[k][1].values()) for k in parts}
    lines = ['<a id="no-en-cpp"></a>', '', f'### Funciones del juego que ya tenemos y aún no están en C++: {total_fns:,}', '',
             'Son envolturas: la DLL ya tiene la función y su hook, pero el cuerpo todavía llama al EXE original. Solo se listan funciones del juego, que son la meta del proyecto. '
             'Todas las tablas van **de mayor a menor tamaño**: «Bytes» es la suma de los cuerpos de las funciones pendientes (análisis estático) y «Tamaño» clasifica la función más grande de la fila '
             '(muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B; una instrucción x86 ocupa entre 1 y 15 bytes, unos 3–4 de media). '
             'Las funciones del EXE sin nombre ni hook (el resto de lo que falta) están en el [mapa del código del juego](JuegoExe/README.md). Fuente: [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).', '']
    for key in ('juego',):
        title, grp = parts[key]
        if not grp:
            continue
        ordered = sorted(grp.items(), key=order)
        lines += [f'#### {title}: {counts[key]} funciones en {len(ordered)} clases o grupos · {total([f for _, v in ordered for f in v]):,} bytes', '',
                  '| Clase o grupo | Funciones | Bytes | Función más grande | Tamaño | Funciones (de mayor a menor) |', '| --- | ---: | ---: | ---: | --- | --- |']
        for cls, fns in ordered:
            fns = sorted(fns, key=lambda f: (-(f[2] if f[2] is not None else -1), f[1], f[0]))
            big = biggest(fns)
            shown = ', '.join(fn_text(f) for f in fns) if key == 'juego' else ', '.join(fn_text(f) for f in fns[:6]) + (f' … y {len(fns) - 6} más' if len(fns) > 6 else '')
            lines.append(f'| {cls} | {len(fns)} | {total(fns):,} | {f"{big:,}" if big is not None else "—"} | {size_label(big)} | ' + (shown.replace(', `', '<br>`') if key == 'juego' else shown) + ' |')
        if key != 'juego':
            lines += ['', '<details>', f'<summary>Lista completa: {title.lower()}</summary>', '']
            for cls, fns in ordered:
                fns = sorted(fns, key=lambda f: (-(f[2] if f[2] is not None else -1), f[1], f[0]))
                lines += [f'**{cls}** ({len(fns)})', ''] + [f'- {fn_text(f)}' for f in fns] + ['']
            lines += ['</details>']
        lines += ['']
    return lines

def dependency_row(root):
    """Texto de la fila «Dependencias de funciones originales eliminadas» (solo código del juego)."""
    if not (Path(root) / 'docs/MemoryMap/binary_functions.csv').exists():
        return None
    d = compute(root)
    reg = d['regions']
    return (f"Medido (cota inferior): {reg['juego']['direct_dependencies']:,} funciones del juego siguen llamándose directamente desde código del juego ya reemplazado; "
            f"cierre transitivo: {d['game_remaining']['A']['functions']:,} funciones / {d['game_remaining']['A']['bytes']:,} bytes. Ver [Cobertura real del EXE](#cobertura-exe)")


if __name__ == '__main__':
    import sys
    result = compute(Path(__file__).resolve().parents[1])
    print(json.dumps(result, indent=2, ensure_ascii=False))
    if '--check' in sys.argv:
        assert result['distinct_exe_functions_replaced'] > 0
