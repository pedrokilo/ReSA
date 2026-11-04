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
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import registry_store

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

    if registry_store.registry_dir(root).is_dir():
        for entry in registry_store.load_registry(root).get('functions', []):
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


def _data_rows(root, name):
    """Filas de un CSV de docs/JuegoExe/ escrito a mano (lotes cerrados y en curso); vacío si no existe."""
    path = Path(root) / 'docs/JuegoExe' / name
    return list(_rows(path)) if path.exists() else []


def coverage_section(root, d=None, number=''):
    """Sección «Cobertura real del EXE»: tabla por región (cada porcentaje dice si es de funciones o de bytes)."""
    if not (Path(root) / 'docs/MemoryMap/binary_functions.csv').exists():
        return []
    d = d or compute(root)
    reg = d['regions']
    names = dict(REGIONS)
    lines = ['<a id="cobertura-exe"></a>', '', f'## {number}Cobertura real del EXE (estimación)', '',
             'Esta tabla es la única parte del informe que habla de lo que no es código del juego: sirve para aclarar qué queda fuera de la meta. Las dos primeras filas de la tabla de dimensiones salen de aquí (región del juego); las demás miden filas del catálogo (`hooks.csv`). Se mide el propio ejecutable original con el análisis estático '
             f'([MemoryMap](MemoryMap/README.md): {d["ghidra_entries"]:,} entradas de función detectadas y {d["body_bytes"]/1e6:.2f} MB de cuerpos de función), cruzado con '
             f'el catálogo, el registro de validación y las envolturas pendientes. {d["catalogue_rows_reversed"]:,} filas reconstruidas del catálogo equivalen a **{d["distinct_exe_functions_replaced"]:,} funciones distintas del EXE '
             'ya reemplazadas** (varias filas son sobrescrituras virtuales que comparten cuerpo). En esta tabla, «% de funciones» es la parte de las funciones de esa región que ya está en C++ y «% de bytes» la parte de los bytes de código de esa región.', '',
             '| Región del EXE | Funciones (total de la región) | Bytes de código (total de la región) | Reemplazadas por C++: funciones y % de funciones | Reemplazadas: % de bytes | Funciones de la región llamadas directamente desde código ya reemplazado |', '| --- | ---: | ---: | --- | --- | ---: |']
    tf = tb = rf = rb = 0
    for key in sorted(reg, key=lambda k: -reg[k]['replaced'] / reg[k]['functions']):
        r = reg[key]
        lines.append(f"| {names[key]} | {r['functions']:,} | {r['bytes']:,} | `{_bar(r['replaced'], r['functions'])}` {r['replaced']:,} · {r['replaced']/r['functions']:.1%} de funciones | "
                     f"`{_bar(r['replaced_bytes'], r['bytes'])}` {r['replaced_bytes']/r['bytes']:.1%} de bytes | {r['direct_dependencies']:,} |")
        tf += r['functions']; tb += r['bytes']; rf += r['replaced']; rb += r['replaced_bytes']
    lines.append(f"| **Total** | **{tf:,}** | **{tb:,}** | `{_bar(rf, tf)}` {rf:,} · {rf/tf:.1%} de funciones | `{_bar(rb, tb)}` {rb/tb:.1%} de bytes | **{d['direct_dependencies_total']:,}** |")
    lines += ['', '**Qué significa para un ejecutable independiente.** No hace falta reconstruir las ' f'{tf:,} funciones: RenderWare/D3D y el CRT son bibliotecas '
              f'enlazadas dentro del EXE ({reg["renderware"]["functions"] + reg["crt"]["functions"]:,} entradas, {(reg["renderware"]["bytes"] + reg["crt"]["bytes"])/tb:.0%} de los bytes del EXE) que se sustituirían '
              'enlazando una implementación equivalente, no reescribiéndolas desde el original. El trabajo propio del proyecto es el código del juego '
              f'({reg["juego"]["functions"]:,} funciones, {reg["juego"]["bytes"]/tb:.0%} de los bytes del EXE), y de él ya está reemplazado el **{reg["juego"]["replaced_bytes"]/reg["juego"]["bytes"]:.0%} de los bytes del juego** '
              f'y el {reg["juego"]["replaced"]/reg["juego"]["functions"]:.0%} de las funciones del juego: no se contradicen, las funciones que quedan son más pequeñas que la media. Entre las {reg["juego"]["functions"]:,} funciones del juego hay {d["game_tiny_helpers_le16"]:,} de 16 bytes o menos (stubs, thunks y helpers).', '']
    return lines


def _wrapper_note(root):
    wrappers = game_wrappers(root)
    if not wrappers:
        return 'No hay envolturas del juego abiertas (sección siguiente).'
    sized = sum(1 for w in wrappers if w[3] is not None)
    text = f'Las {len(wrappers)} envolturas del juego abiertas de la sección siguiente son funciones del EXE que la DLL llama por dirección: {sized} están incluidas en estas cifras'
    return text + (f'; las otras {len(wrappers) - sized} no figuran como entrada de función en el análisis estático (sin tamaño), así que no entran en el recuento.' if sized < len(wrappers) else '.')


def missing_section(root, d=None, number='', fecha=''):
    """Sección «Lo que falta del código del juego» (grupos A–D)."""
    if not (Path(root) / 'docs/MemoryMap/binary_functions.csv').exists():
        return []
    d = d or compute(root)
    reg = d['regions']
    g, t = d['game_remaining'], d['game_remaining_total']
    lines = [f'## {number}Lo que falta del código del juego: {t["functions"]:,} funciones, {t["bytes"]/1e3:,.0f} KB', '',
             f'Al {fecha}: {t["functions"]:,} funciones ({t["functions"]/reg["juego"]["functions"]:.2%} de las funciones del juego) y {t["bytes"]:,} bytes ({t["bytes"]/reg["juego"]["bytes"]:.2%} de los bytes del juego) siguen sin reemplazar. '
             'Este es el trabajo real que falta del código del juego, lo mida o no el catálogo. Mapa completo de las funciones del juego, con lista por estado, bloque de direcciones y módulo estimado: [JuegoExe](JuegoExe/README.md).', '',
             '| Grupo (de mayor a menor tamaño) | Funciones | Bytes | Lectura |', '| --- | ---: | ---: | --- |',
             *[row for _, row in sorted(((g[k]['bytes'], line) for k, line in (
                 ('D', f'| D. Sin llamadas directas ni vtable | {g["D"]["functions"]:,} | {g["D"]["bytes"]:,} | Código muerto o alcanzado por tablas de punteros, callbacks o scripts: hay que revisarlo, no contarlo sin más. |'),
                 ('C', f'| C. Llamadas directas solo desde código aún no reemplazado | {g["C"]["functions"]:,} | {g["C"]["bytes"]:,} | Dependen de reconstruir antes A y B; parte puede ser código muerto. |'),
                 ('B', f'| B. Métodos virtuales (en una vtable) no alcanzados por llamada directa | {g["B"]["functions"]:,} | {g["B"]["bytes"]:,} | Probablemente necesarios: se ejecutan por despacho virtual. |'),
                 ('A', f'| A. Llamadas directamente desde código ya reemplazado (cierre transitivo) | {g["A"]["functions"]:,} | {g["A"]["bytes"]:,} | **Trabajo necesario**: la DLL sigue dependiendo de ellas. Es el piso para la independencia. Lista viva: [dependencias directas (grupo A)](#grupo-a). |'),
             )), key=lambda kv: -kv[0])], '',
             f'Del código del juego ya reemplazado, **{reg["juego"]["direct_dependencies"]:,} funciones del juego siguen llamándose directamente desde el EXE**. Esa es la lista corta de dependencias reales que hay que eliminar. Las que caen en RenderWare/D3D o el CRT se muestran solo en la última columna de la tabla de cobertura. '
             + _wrapper_note(root), '',
             f'> [!NOTE]\n> **Cotas y límites.** Es un análisis estático: hay {d["indirect_call_sites"]:,} llamadas indirectas sin resolver que no están en el grafo, así que A es una **cota inferior** del trabajo. '
             'La partición por regiones usa la dirección, no una clasificación función a función. «Reemplazada» significa que el hook existe y pasó su banco; no acredita independencia del EXE ni '
             'que la validación siga vigente para el árbol actual. Se regenera con `contrib/exe_coverage.py` al sincronizar la documentación.', '']
    return lines


def section(root, number=('', '', ''), fecha=''):
    """Secciones de cobertura, lo que falta y envolturas del juego juntas (compatibilidad)."""
    d = compute(root) if (Path(root) / 'docs/MemoryMap/binary_functions.csv').exists() else None
    return coverage_section(root, d, number[0]) + missing_section(root, d, number[1], fecha) + pending_section(root, number[2], fecha)


def game_wrappers(root):
    """Envolturas abiertas de código del juego: [(clase o grupo, función, dirección, bytes|None, lote)] de mayor a menor tamaño.

    La clasificación sale de la dirección (contrib/pending_wrappers.py): 0x401000-0x74FFFF es juego; fuera, biblioteca.
    """
    path = Path(root) / 'docs/implementaciones/envolturas-pendientes.csv'
    if not path.exists():
        return []
    sizes = function_sizes(root)
    lots = defaultdict(set)
    for r in _data_rows(root, 'lotes-en-curso.csv'):
        lots[_hex(r['direccion'])].add(r['lote'])
    out = []
    for r in _rows(path):
        if r['class'].startswith('(biblioteca)'):
            continue
        addr = _hex(r['address'])
        out.append((r['class'], r['function'], r['address'], sizes.get(addr) if addr else None, ' / '.join(sorted(lots.get(addr, ()))) or '—'))
    out.sort(key=lambda x: (-(x[3] if x[3] is not None else -1), x[2], x[1]))
    return out


def library_wrapper_counts(root):
    """{clase de biblioteca: (funciones, de ellas procedentes de «helpers sin clase» reclasificados por dirección)}."""
    path = Path(root) / 'docs/implementaciones/envolturas-pendientes.csv'
    counts = {}
    if path.exists():
        for r in _rows(path):
            if r['class'].startswith('(biblioteca)'):
                total, moved = counts.get(r['class'], (0, 0))
                counts[r['class']] = (total + 1, moved + (r['origins'] == 'envoltura en el código fuente'))
    return counts


LIBRARY_PLANS = (
    ('RenderWare SDK', 'Sustituir por **librw** (o una implementación equivalente de RenderWare 3.7) y reconstruir los pipelines propios de SA; no se reescribe desde el EXE.'),
    ('Controlador D3D9', 'Sustituir por el **backend D3D9** de la biblioteca de sustitución (puentes `RwD3D9*`); fuera de la reconstrucción del juego.'),
    ('CRT', 'Usar la **CRT del compilador** (malloc/free, operator new/delete, funciones matemáticas); fuera de la reconstrucción del juego.'),
    ('QuickTime', 'Reemplazar o desactivar el reproductor de vídeo (QuickTime/QTML) en el ejecutable propio; fuera de la reconstrucción del juego.'),
)


def pending_section(root, number='', fecha=''):
    """Funciones del juego que ya tenemos (hook y envoltura) y aún no están en C++, más las bibliotecas que quedan fuera del objetivo."""
    path = Path(root) / 'docs/implementaciones/envolturas-pendientes.csv'
    if not path.exists():
        return []
    game = game_wrappers(root)
    total_bytes = sum(w[3] or 0 for w in game)
    lines = ['<a id="no-en-cpp"></a>', '', f'## {number}Funciones del juego que ya tenemos y aún no están en C++: {len(game):,}', '']
    if game:
        lines += [f'Al {fecha}, estas {len(game):,} funciones **del código del juego** (0x401000–0x74FFFF) tienen envoltura abierta: la DLL ya tiene la función y su hook, pero el cuerpo todavía llama al EXE original por dirección ({total_bytes:,} bytes en total). '
                  'Mientras no estén reconstruidas y validadas **son envolturas del juego**, aunque el catálogo las trate como funciones sin dirección. '
                  'La clasificación sale de la dirección del destino (juego = 0x401000–0x74FFFF, biblioteca = fuera de ese rango, tomada de la llamada `plugin::Call<0x…>` del código), no de una lista a mano: cuando se validen dejarán de aparecer solas. '
                  'La tabla va **de mayor a menor tamaño**: «Bytes» es el cuerpo de la función en el EXE (análisis estático) y «Tamaño» lo clasifica (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B). '
                  'Las funciones del EXE sin nombre ni hook (el resto de lo que falta) están en el [mapa del código del juego](JuegoExe/README.md). Fuente: [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv); lotes: [lotes-en-curso.csv](JuegoExe/lotes-en-curso.csv).', '',
                  '| Función | Dirección | Bytes | Tamaño | Clase o grupo | Lote en curso |', '| --- | --- | ---: | --- | --- | --- |']
        for cls, fn, addr, size, lot in game:
            lines.append(f'| `{fn}` | `{addr or "sin dirección"}` | {f"{size:,}" if size is not None else "—"} | {size_label(size)} | {cls} | {lot} |')
        lines += ['']
    else:
        lines += [f'Al {fecha}, el catálogo **no tiene envolturas abiertas de código del juego**: todas las funciones del juego con hook están reconstruidas en C++. Las de biblioteca (abajo) no cuentan.', '']
    libs = library_wrapper_counts(root)
    lines += ['<a id="bibliotecas"></a>', '', f'### Bibliotecas, fuera del objetivo ({sum(v[0] for v in libs.values()):,} envolturas)', '',
              'Estas envolturas llaman al EXE, pero su destino es una biblioteca enlazada dentro del ejecutable (RenderWare, controlador D3D9, CRT, QuickTime) y **no cuentan** como trabajo pendiente del juego ni en las cifras de este informe. '
              'La clase sale de la dirección del destino (fuera de 0x401000–0x74FFFF) o de la clasificación del cohorte; no se reconstruyen: se sustituyen según la [hoja de ruta](RutaExeIndependiente.md).', '',
              '| Biblioteca | Envolturas | De ellas, antes en «helpers sin clase» | Plan |', '| --- | ---: | ---: | --- |']
    for key, plan in LIBRARY_PLANS:
        matches = [(c, v) for c, v in libs.items() if key.lower() in c.lower()]
        if matches:
            lines.append(f'| {matches[0][0].replace("(biblioteca) ", "")} | {sum(v[0] for _, v in matches)} | {sum(v[1] for _, v in matches)} | {plan} |')
    lines += ['']
    moved = [r for r in _rows(path) if r['class'].startswith('(biblioteca)') and r['origins'] == 'envoltura en el código fuente']
    if moved:
        lines += ['<details>', f'<summary>Envolturas de biblioteca detectadas en el código fuente y reclasificadas por su dirección ({len(moved)})</summary>', '']
        lines += [f'- `{r["function"]}` `{r["address"]}` · {r["class"].replace("(biblioteca) ", "")}' for r in sorted(moved, key=lambda r: (r['address'], r['function']))]
        lines += ['', '</details>', '']
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
