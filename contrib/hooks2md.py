"""Generate the progress dashboard from recorded hook flags (not test coverage)."""
import argparse
import csv
import re
import json
import datetime
import os
from dataclasses import dataclass
from pathlib import Path
import exe_coverage
import registry_store
import group_a_section
from validation_registry import evaluate
from source_status import read_classes, source_index, update_comments
from urllib.parse import quote


@dataclass
class Klass:
    name: str
    reversed: int = 0
    total: int = 0
    locked: int = 0
    validated: int = 0

    @property
    def pending(self):
        return self.total - self.reversed


def address_state(root, input_path, pendientes):
    """{direccion: validada?} de las direcciones del EXE con fila reversed=1 en hooks.csv (virtuales incluidos); `pendientes` son las direcciones que interesan."""
    state = {}
    virtual_rows = {}
    with open(input_path, encoding='utf-8-sig', newline='') as stream:
        for row in csv.DictReader(stream):
            if row['reversed'] == '1':
                try:
                    address = int(row['address'], 16)
                except ValueError:
                    continue
                if address:
                    state[address] = row['validated'] == '1'
                else:
                    # Los virtuales figuran en hooks.csv con direccion 0x00000000: su direccion del EXE esta en el registro de validacion.
                    virtual_rows[(row['class'], row['fn_name'])] = row['validated'] == '1'
    if virtual_rows and registry_store.registry_dir(root).is_dir():
        for entry in registry_store.load_registry(root).get('functions', []):
            hook = entry.get('hook', {})
            key = (hook.get('class'), hook.get('fn_name'))
            original = entry.get('evidence', {}).get('original_address', '')
            if key in virtual_rows and original:
                try:
                    state[int(original, 16)] = virtual_rows[key]
                except ValueError:
                    pass

    # La mayoria de los virtuales no guarda la direccion en el registro: se toma del RH_ScopedVMTInstall del fuente de su clase.
    install = re.compile(r'RH_ScopedVMT(?:Overloaded)?Install\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+)')
    for path in (root / 'source').rglob('*.cpp'):
        try:
            text = path.read_text(encoding='utf-8', errors='replace')
        except OSError:
            continue
        for fn_name, address in install.findall(text):
            address = int(address, 16)
            if address in state or address not in pendientes:
                continue
            for (cls, name), validated in virtual_rows.items():
                if name == fn_name and f'{cls}::' in text:
                    state[address] = validated
                    break
    return state


def cohort_499_state(root, input_path):
    """Estado actual de los destinos del juego del cohorte inicial de 499 (reversing/OriginalDependencies499/progress.csv) frente a hooks.csv."""
    path = root / 'reversing/OriginalDependencies499/progress.csv'
    if not path.exists():
        return None
    with path.open(encoding='utf-8-sig', newline='') as stream:
        rows = [r for r in csv.DictReader(stream) if r['category'] == 'game']
    state = address_state(root, input_path, {int(r['address'], 16) for r in rows})
    validated = [r for r in rows if state.get(int(r['address'], 16)) is True]
    built = [r for r in rows if state.get(int(r['address'], 16)) is False]
    open_ = [r for r in rows if int(r['address'], 16) not in state]
    return dict(total=len(rows), validated=len(validated), built=len(built), open=open_,
                passed=sum(r['runtime_status'] == 'passed' for r in rows))


def difficulty_section(root, input_path, number='', fecha=''):
    """Sección del cohorte nuevo del 17/10/2026 (reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv): estado medido y lista por dificultad."""
    source = root / 'reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv'
    if not source.exists():
        return []
    with source.open(encoding='utf-8-sig', newline='') as stream:
        items = list(csv.DictReader(stream))
    pendientes = {int(r['address'], 16) for r in items if r['address'].startswith('0x')}
    state = address_state(root, input_path, pendientes)

    def status(row):
        try:
            validated = state.get(int(row['address'], 16))
        except ValueError:
            validated = None
        return 'pendiente' if validated is None else ('✅ validada' if validated else 'reconstruida')

    def cell(text):
        return str(text).replace('|', '\\|').replace('<', '&lt;').replace('>', '&gt;')

    items = [r for r in items if r['categoria'] == 'juego']   # el informe solo trata el código del juego; las bibliotecas aparecen únicamente en la tabla de cobertura
    levels = sorted({r['nivel'] for r in items})
    counts = {s: sum(status(r) == s for r in items) for s in ('✅ validada', 'reconstruida', 'pendiente')}
    closed = counts['reconstruida'] == 0 and counts['pendiente'] == 0
    summary = f"{len(items)} destinos del juego del cohorte: {counts['✅ validada']} validadas, {counts['reconstruida']} reconstruidas por validar y {counts['pendiente']} sin reconstruir"
    lines = ['', '<a id="cohorte-nuevo"></a>',
             f"## {number}📋 Cohorte del 17/10/2026: {'CERRADO' if closed else 'en curso'} ({summary}, al {fecha})", '']
    if closed:
        lines += [f'> [!NOTE]\n> **Cohorte cerrado.** Al {fecha}, los {len(items)} destinos del código del juego del cohorte del 17/10/2026 (funciones del juego que `source/` llamaba por dirección y que no estaban reconstruidas ni validadas) '
                  f'tienen fila `reversed=1` y `validated=1` en [hooks.csv](hooks.csv): quedan **0 pendientes y 0 por validar**. Las {sum(1 for _ in open(source, encoding="utf-8-sig")) - 1 - len(items)} entradas restantes del cohorte son de bibliotecas (RenderWare, D3D9, CRT, QuickTime) y quedan fuera del objetivo ([sección 7](#bibliotecas)). '
                  'La lista de abajo se conserva como histórico; ya no es trabajo pendiente.', '']
    else:
        lines += [f'Faltan {counts["reconstruida"] + counts["pendiente"]} destinos del juego del cohorte por validar o reconstruir al {fecha}.', '']
    lines += ['Destinos originales explícitos que `source/` seguía referenciando y que no estaban reconstruidos ni validados el 17/10/2026. La lista va ordenada de mayor a menor tamaño en bytes (la columna «Rango» conserva la posición por dificultad, de menor a mayor: 1 es la más fácil; '
              'puntuación de tamaño y riesgos —llamadas sin catalogar, indirectas, x87, tablas de saltos y SEH—). Solo se listan funciones del juego. '
              f'Lista completa y descargable: [lista-por-dificultad.csv](../{source.relative_to(root).as_posix()}) · metodología: [README del cohorte](../reversing/OriginalDependencies2026-10-17/README.md). '
              'La columna «Estado» se calcula con el catálogo actual: una dirección con fila reversed=1 en hooks.csv figura como reconstruida o validada.', '',
              '| Nivel de dificultad | Funciones del juego | Validadas | Reconstruidas por validar | Sin reconstruir |', '| --- | ---: | ---: | ---: | ---: |']
    for level in levels:
        game = [r for r in items if r['nivel'] == level]
        lines.append(f"| {cell(level)} | {len(game)} | {sum(status(r) == '✅ validada' for r in game)} | {sum(status(r) == 'reconstruida' for r in game)} | {sum(status(r) == 'pendiente' for r in game)} |")
    lines.append(f"| **Total** | **{len(items)}** | **{counts['✅ validada']}** | **{counts['reconstruida']}** | **{counts['pendiente']}** |")
    shown = items if closed else [r for r in items if status(r) != '✅ validada']
    rows = sorted(shown, key=lambda r: (-int(r['bytes']), int(r['rango'])))
    title = (f'Histórico: los {len(rows)} destinos del juego del cohorte, todos validados' if closed
             else f'Funciones del juego pendientes o por validar ({len(rows)}), de mayor a menor tamaño en bytes')
    lines += ['', '<details>', f'<summary>{title}, de mayor a menor tamaño en bytes</summary>' if closed else f'<summary>{title}</summary>', '',
              '| Rango | Nivel | Subcategoría | Función | Dirección | Bytes | Estado |', '| ---: | --- | --- | --- | --- | ---: | --- |']
    for r in rows:
        lines.append(f"| {r['rango']} | {cell(r['nivel'])} | {cell(r['subcategoria'])} | {cell(r['identity'])} | `{r['address']}` | {r['bytes']} | {status(r)} |")
    lines += ['', '</details>']
    first = cohort_499_state(root, input_path)
    if first:
        lines += ['', f"**Cohorte inicial de 499 dependencias (comprobado el {fecha}).** De sus {first['total']} destinos del juego, {first['validated']} tienen fila `reversed=1` y `validated=1` en hooks.csv "
                      f"({first['passed']} figuran como aprobadas en su `progress.csv` y el resto se validó después con otros bancos), {first['built']} están reconstruidas por validar y **{len(first['open'])} abiertas**"
                  + (': ' + ', '.join(f"`{r['identity']}` `{r['address']}`" for r in first['open']) + '.' if first['open'] else '.')
                  + ' El «124 entradas del juego pendientes» que repite la cabecera de estado de otros documentos sale del `progress.json` de ese cohorte, que no se ha refrescado; no es la medida actual.']
    return lines


def generate(input_path, output_path, update_source_status=False):
    root = Path(__file__).resolve().parents[1]
    recorded = read_classes(input_path)
    index = source_index(root, recorded)
    if update_source_status:
        changed = update_comments(root, input_path, recorded, index)
        print(f'Comentarios: {len(index)} clases/grupos; {changed} archivos actualizados.')
    validation = evaluate(root, input_path)

    def class_link(name):
        paths = index[name]
        links = []
        for i, path in enumerate(paths):
            target = quote(Path(os.path.relpath(path, Path(output_path).resolve().parent)).as_posix())
            label = name.replace('<', '&lt;').replace('>', '&gt;') if i == 0 else f'archivo {i + 1}'
            links.append(f'[{label}]({target})')
        return ' · '.join(links)
    classes = {}
    with open(input_path, encoding="utf-8-sig", newline="") as stream:
        for row in csv.DictReader(stream):
            item = classes.setdefault(row["class"], Klass(row["class"]))
            item.total += 1
            item.reversed += int(row["reversed"])
            item.locked += int(row["locked"])
            item.validated += int(row["validated"])
    if not classes:
        raise ValueError("hooks.csv no contiene funciones")
    # El informe trata solo el código del juego: las clases de RenderWare y del CRT se excluyen de todas las listas y cuentas
    # (solo se mencionan en la tabla de cobertura del EXE). La comprobación de evidencia usa todo el catálogo.
    validated_count_all = sum(c.validated for c in classes.values())
    library_rows = sum(classes[n].total for n in classes if n in exe_coverage.LIBRARY_CLASSES)
    for _n in [n for n in classes if n in exe_coverage.LIBRARY_CLASSES]:
        del classes[_n]
    ordered = sorted(classes.values(), key=lambda c: c.name.casefold())
    # Funciones del juego que siguen delegando en el EXE (docs/implementaciones/envolturas-pendientes.csv, contrib/pending_wrappers.py).
    # Una clase del catálogo con envolturas pendientes ya no es «completa»: pasa a parciales. Las clases con envolturas que no figuran
    # en el catálogo aparecen en «sin revertir» con su cantidad de envolturas.
    wrappers = {}
    wrap_addrs = {}
    wrappers_file = Path(input_path).with_name('implementaciones') / 'envolturas-pendientes.csv'
    if wrappers_file.exists():
        with wrappers_file.open(encoding='utf-8-sig', newline='') as stream:
            for row in csv.DictReader(stream):
                if row['class'].startswith('(biblioteca)'):
                    continue   # RenderWare, controlador D3D9, CRT y QuickTime: fuera del informe
                wrappers.setdefault(row['class'], []).append(row['function'])
                wrap_addrs.setdefault(row['class'], []).append(row.get('address', ''))
    wrap_count = {name: len(fns) for name, fns in wrappers.items()}
    # Tamaño (bytes del cuerpo según el análisis estático) de las envolturas pendientes por clase: suma y función más grande.
    _sizes = exe_coverage.function_sizes(root)
    def _wrap_sizes(name):
        out = []
        for addr in wrap_addrs.get(name, []):
            try:
                out.append(_sizes.get(int(addr, 16)))
            except ValueError:
                out.append(None)
        return [x for x in out if x is not None]
    wrap_bytes = {name: sum(_wrap_sizes(name)) for name in wrappers}
    wrap_big = {name: (max(_wrap_sizes(name)) if _wrap_sizes(name) else None) for name in wrappers}
    def _size_cells(name):
        big = wrap_big.get(name)
        return f"{wrap_bytes.get(name, 0):,} | {f'{big:,}' if big is not None else '—'} | {exe_coverage.size_label(big)}"
    pending_functions = sum(wrap_count.values())
    full = [c for c in ordered if not c.pending and not wrap_count.get(c.name)]
    partial = [c for c in ordered if c.reversed and (c.pending or wrap_count.get(c.name))]
    empty = [c for c in ordered if not c.reversed]
    uncatalogued = sorted((name for name in wrappers if name not in classes), key=str.casefold)
    class_total = len(ordered) + len(uncatalogued)
    total = sum(c.total for c in ordered)
    reversed_count = sum(c.reversed for c in ordered)
    locked = sum(c.locked for c in ordered)
    validated_count = sum(c.validated for c in ordered)
    validated_classes = [c for c in full if c.validated == c.total]  # clases completas con todas sus funciones validated=1
    notsa_file = Path(input_path).with_name('hooks_notsa.csv')
    notsa_count = 0
    if notsa_file.exists():
        with notsa_file.open(encoding='utf-8-sig', newline='') as stream:
            notsa_count = sum(1 for _ in csv.DictReader(stream))
    if validated_count_all != validation['documented']:
        raise ValueError(
            f"hooks.csv validated=1 ({validated_count_all}) no coincide con evidencia aprobada "
            f"({validation['documented']}). Ejecuta: python contrib/sync_validated_flags.py"
        )
    # Funciones reconstruidas del catálogo cuyo cuerpo fuente (inventario de implementaciones) está escrito solo en C++, sin __asm ni naked.
    # Mide todo el catálogo (game_sa, plataforma y aplicación). Las entradas sin fuente identificada de forma inequívoca no se cuentan como C++.
    cpp_total = reversed_count
    cpp_only = 0
    inventory = Path(input_path).with_name('implementaciones') / 'funciones.csv'
    if inventory.exists():
        with inventory.open(encoding='utf-8-sig', newline='') as stream:
            # Una entrada del catálogo cuenta una sola vez aunque varios cuerpos (p. ej. constructor y su envoltura Constructor) compartan su dirección;
            # cuenta como C++ solo si todos los cuerpos enlazados están escritos solo en C++.
            linked = {}
            for row in csv.DictReader(stream):
                if row['reversed'] == '1' and row['catalog'] == Path(input_path).name:  # solo hooks.csv; hooks_notsa.csv queda fuera
                    if row['catalog_identity'].split('::')[0] in exe_coverage.LIBRARY_CLASSES:
                        continue
                    key = (row['catalog_identity'], row['catalog_address'])
                    linked[key] = linked.get(key, True) and row['language'] == 'C++'
        shared_file = Path(input_path).with_name('implementaciones') / 'catalogo-cuerpo-compartido.csv'
        if shared_file.exists():  # entradas servidas por el cuerpo genérico de una plantilla (instanciaciones, clases derivadas)
            with shared_file.open(encoding='utf-8-sig', newline='') as stream:
                for row in csv.DictReader(stream):
                    if row['reversed'] == '1' and row['catalog'] == Path(input_path).name:
                        if row['catalog_identity'].split('::')[0] in exe_coverage.LIBRARY_CLASSES:
                            continue
                        key = (row['catalog_identity'], row['catalog_address'])
                        linked[key] = linked.get(key, True) and row['language'] == 'C++'
        cpp_only = sum(linked.values())
    cpp_row = ([f"| Entradas del juego reconstruidas escritas solo en C++ (sin ensamblador) | `{'█' * round(20*cpp_only/cpp_total)}{'░' * (20-round(20*cpp_only/cpp_total))}` | {cpp_only:,}/{cpp_total:,} · {cpp_only/cpp_total:.2%} |"]
               if inventory.exists() and cpp_total else [])
    try:
        _cov = exe_coverage.compute(root)['regions']['juego']
    except (OSError, KeyError):
        _cov = None
    def _bar(part, whole):
        n = round(20 * part / whole) if whole else 0
        return '█' * n + '░' * (20 - n)
    cov_rows = ([f"| Funciones del juego reemplazadas por C++ en el EXE (meta del proyecto) | `{_bar(_cov['replaced'], _cov['functions'])}` | {_cov['replaced']:,}/{_cov['functions']:,} · {_cov['replaced']/_cov['functions']:.2%} |",
                 f"| Bytes de código del juego reemplazados | `{_bar(_cov['replaced_bytes'], _cov['bytes'])}` | {_cov['replaced_bytes']:,}/{_cov['bytes']:,} · {_cov['replaced_bytes']/_cov['bytes']:.2%} |"]
                if _cov else [])
    now = datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    asm_total = None
    summary_path = Path(input_path).with_name('implementaciones') / 'resumen.json'
    if summary_path.exists():
        import json as _json
        _totals = _json.loads(summary_path.read_text(encoding='utf-8')).get('totals', {})
        asm_total = _totals.get('assembly_occurrences', 0) + _totals.get('naked_declarations', 0)
    migration_text = (
        "**Migración de ensamblador a C++:** en `source/game_sa` ya no queda ningún `__asm` ni función `naked`: todo lo reconstruido está escrito en C++, con resultados validados contra el EXE dentro del juego. Regla del proyecto: todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)). Historial y excepciones documentadas: [estado de la migración](../reversing/CppOnlyMigration/README.md) · [FILES.md](../reversing/CppOnlyMigration/FILES.md)."
        if asm_total == 0 else
        "**Migración de ensamblador a C++:** algunas clases ya reconstruidas y validadas todavía contienen ensamblador en su fuente (`__asm` o funciones `naked`), porque se reconstruyeron conservando exactamente el comportamiento del EXE. Pasarán por un proceso de conversión de ensamblador a C++ que conserve resultados idénticos y se vuelva a validar dentro del juego. Hasta entonces, «reconstruida y validada» no implica «escrita en C++». Regla del proyecto: todo lo que se reconstruya va en C++, dos archivos por clase (`.cpp` y `.h`) y sin duplicados ([proceso](../reversing/CppOnlyMigration/PROCESO.md)). Lista de archivos pendientes: [FILES.md](../reversing/CppOnlyMigration/FILES.md) · [estado de la migración](../reversing/CppOnlyMigration/README.md).")
    _dep_row = exe_coverage.dependency_row(root)
    fecha = datetime.datetime.now(datetime.timezone.utc).strftime('%d/%m/%Y')
    try:
        _d = exe_coverage.compute(root)
    except (OSError, KeyError):
        _d = None
    lib_counts = exe_coverage.library_wrapper_counts(root)
    lib_total = sum(v[0] for v in lib_counts.values())
    game_wrappers = exe_coverage.game_wrappers(root)
    entries_total = total + pending_functions
    pct = lambda a, b: f'{a/b:.2%}' if b else 'n/d'
    def dim_row(unit, name, what, num, den, of, bar=True):
        return f"| **{unit}** | {name} | {what} | {num:,} | {den:,} | {pct(num, den)} de {of} | " + (f"`{_bar(num, den)}` |" if bar else "|")
    dimension_rows = []
    if _cov:
        _missing = _d['game_remaining_total']
        dimension_rows += [
            dim_row('Funciones', 'Funciones del juego reemplazadas por C++ en el EXE (**meta del proyecto**)', 'Funciones del EXE en 0x401000–0x74FFFF que ya tienen su hook reconstruido (análisis estático del EXE)', _cov['replaced'], _cov['functions'], 'las funciones del juego'),
            dim_row('Funciones', 'Funciones del juego que faltan', 'Complemento de la fila anterior: grupos A–D de [lo que falta](#que-falta)', _missing['functions'], _cov['functions'], 'las funciones del juego'),
            dim_row('Bytes', 'Bytes de código del juego reemplazados', 'Suma de los cuerpos de esas mismas funciones, en bytes de código', _cov['replaced_bytes'], _cov['bytes'], 'los bytes de código del juego'),
        ]
    dimension_rows += [
        dim_row('Clases', 'Clases del juego completas', 'Clases o grupos del catálogo con todas sus entradas reconstruidas y sin envolturas del juego abiertas', len(full), class_total, 'las clases del juego'),
        dim_row('Validación', 'Clases del juego validadas', 'Clases completas cuyas entradas tienen todas `validated=1`', len(validated_classes), len(full), 'las clases completas'),
        dim_row('Entradas del catálogo', 'Entradas del juego reconstruidas', 'Filas de `hooks.csv` con `reversed=1` más las envolturas del juego abiertas, que cuentan como no reconstruidas', reversed_count, entries_total, 'las entradas del juego del catálogo'),
        dim_row('Validación', 'Entradas del juego validadas', 'Entradas reconstruidas con evidencia aprobada vinculada (`validated=1`)', validated_count, reversed_count, 'las entradas reconstruidas'),
    ]
    if cpp_row:
        dimension_rows.append(dim_row('Entradas del catálogo', 'Entradas reconstruidas escritas solo en C++', 'Entradas reconstruidas cuyo cuerpo fuente no tiene `__asm` ni `naked` (las de fuente no identificada de forma inequívoca no cuentan)', cpp_only, cpp_total, 'las entradas reconstruidas'))
    cohort_number = '9. '
    lots_lines = group_a_section.lots_table(root, fecha)
    lib_text = ', '.join(f"{k.replace('(biblioteca) ', '')} {v[0]}" for k, v in sorted(lib_counts.items(), key=lambda kv: -kv[1][0]))
    if game_wrappers:
        wrappers_text = (f"**Envolturas del juego abiertas: {len(game_wrappers)}** (comprobado el {fecha}; {sum(w[3] or 0 for w in game_wrappers):,} bytes): "
                         + ', '.join(f"`{w[1]}` `{w[2]}`" for w in game_wrappers) + ". Son código del juego (D3DResourceSystem y lectura de streams en 0x72E000–0x735000): la DLL llama al EXE por dirección y no cuentan como reconstruidas hasta validarse. "
                         "Detalle en la [sección 7](#no-en-cpp).")
    else:
        wrappers_text = f"**Envolturas del juego abiertas: 0** (comprobado el {fecha}): todas las funciones del juego con hook están reconstruidas en C++."
    lines = ["# Progreso del reversing", "",
             "[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)", "",
             f"> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** {now}", "",
             "**Índice:** [1. Objetivo y dimensiones](#objetivo) · [2. Siguientes mediciones](#mediciones) · [3. Siguiente fase](#siguiente-fase) · [4. Vista general](#vista-general) · [5. Cobertura real del EXE](#cobertura-exe) · "
             "[6. Lo que falta del código del juego](#que-falta) · [7. Funciones del juego aún no en C++](#no-en-cpp) · [8. Completas](#completas) · [9. Cohorte 17/10/2026](#cohorte-nuevo) · [10. Dependencias directas (grupo A)](#grupo-a) · "
             "[11. Parciales](#parciales) · [12. Sin revertir](#sin-revertir) · [13. Cómo leer y actualizar](#metodo)", "",
             '<a id="objetivo"></a>', "",
             "## 1. Objetivo y dimensiones de progreso", "",
             "La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. "
             + (f"**La meta de este informe es completar todas las funciones del juego** ({_cov['functions']:,} funciones y {_cov['bytes']/1e6:.2f} MB de código en el EXE: el código propio del juego y su capa de plataforma). " if _cov else "**La meta de este informe es completar todas las funciones del juego.** ")
             + "RenderWare, Direct3D y el CRT son bibliotecas enlazadas dentro del EXE: no forman parte de la meta, no se cuentan en las tablas y solo aparecen en la [cobertura real del EXE](#cobertura-exe) y en la lista de [bibliotecas fuera del objetivo](#bibliotecas) para aclarar qué queda fuera. Completar el juego es un hito hacia **ReSA — Zero Original Function Dependency**.", "",
             f"Cifras medidas el {fecha}. **Cada porcentaje de este informe lleva escrita la dimensión de la que habla**: *funciones* (del EXE), *bytes* (de código del EXE), *clases*, *entradas del catálogo* (filas de `hooks.csv`) o *validación* (cuántas de las ya hechas tienen evidencia aprobada). "
             "Dimensiones distintas dan porcentajes distintos y no son comparables entre sí.", "",
             "| Tipo | Dimensión | Qué cuenta | Numerador | Denominador | Porcentaje (de qué) | Progreso |",
             "| --- | --- | --- | ---: | ---: | --- | --- |",
             *dimension_rows, "",
             (f"**Por qué hay un {_cov['replaced']/_cov['functions']:.0%} y un {_cov['replaced_bytes']/_cov['bytes']:.0%}.** No se contradicen: miden cosas distintas. El {_cov['replaced']/_cov['functions']:.2%} cuenta *funciones* y el {_cov['replaced_bytes']/_cov['bytes']:.2%} cuenta *bytes* de código; las funciones que faltan son más pequeñas que las ya reemplazadas, así que quedan muchas funciones pero pocos bytes. " if _cov else "") +
             "Las filas «Funciones» y «Bytes» miden el propio EXE (análisis estático, ver [cobertura real del EXE](#cobertura-exe)) y son la medida del avance hacia la meta; las filas de «Entradas del catálogo», «Clases» y «Validación» miden las filas registradas de `hooks.csv`, que solo conocen una parte del código del juego, por eso su porcentaje es mayor. "
             "Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL." + (" La última fila cuenta, de las entradas reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md)." if cpp_row else ""), "",
             migration_text, "",
             f"Hay {total:,} entradas catalogadas del juego, {locked:,} bloqueadas y {total-locked:,} sin bloqueo. No constituyen un inventario exhaustivo del código del juego; las filas de RenderWare y del CRT del catálogo ({library_rows}) quedan fuera de este informe. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks."
             + (f" Las extensiones o adaptaciones NOTSA ({notsa_count}, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta." if notsa_count else ""), "",
             '<a id="mediciones"></a>', "",
             "## 2. Siguientes mediciones", "",
             f"Estado al {fecha}. Lo que ya se midió y lo que todavía no tiene un porcentaje útil:", "",
             "| Dimensión | Estado actual | Qué hay que hacer |",
             "| --- | --- | --- |",
             f"| Validación vigente para el árbol actual (validación) | {validation['current']}/{validated_count:,} entradas validadas confirmadas como vigentes ({pct(validation['current'], validated_count)} de las entradas validadas) | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). El 04/11/2026 se repitieron 20 bancos desfasados sin regresión de producción (todos eran bancos desfasados o artefactos del entorno): falta consolidarlos en el registro. |",
             f"| Dependencias de funciones originales eliminadas (funciones) | {_dep_row or 'No medido'} | Resolver las llamadas indirectas (tablas de punteros, callbacks) para convertir la cota inferior en un valor exacto, y eliminar o sustituir las dependencias directas que quedan: lista viva en la [sección 10](#grupo-a). |",
             *([f"| Trabajo real que falta (funciones y bytes) | {_d['game_remaining_total']['functions']:,} funciones y {_d['game_remaining_total']['bytes']:,} bytes del juego sin reemplazar ({pct(_d['game_remaining_total']['functions'], _cov['functions'])} de las funciones y {pct(_d['game_remaining_total']['bytes'], _cov['bytes'])} de los bytes del juego); grupo A {_d['game_remaining']['A']['functions']:,} funciones y {_d['game_remaining']['A']['bytes']:,} bytes | Reconstruir y validar de menor a mayor coste, empezando por el grupo A ([sección 6](#que-falta)). |"] if _d and _cov else []),
             f"| Coste por categoría de lote (tiempo de reconstrucción y validación) | Medido para las categorías 1, 2, 3 y 5 en los lotes 90A, 90B, 92A, 92B y 92C (tabla de abajo); sin medir la categoría 4 (funciones con FPU) | Cronometrar un lote piloto con FPU (método x87 a 24 bits) y los siguientes lotes de lógica para convertir la estimación en un coste por función. |", "",
             *lots_lines, "",
             '<a id="siguiente-fase"></a>', "",
             "## 3. Siguiente fase: después de las envolturas", "",
             f"**¿Quedan envolturas de código del juego en el catálogo?** {'Sí.' if game_wrappers else 'No.'} " + wrappers_text, "",
             f"Las envolturas de biblioteca no cuentan: {lib_total:,} funciones con envoltura al EXE ({lib_text}) son RenderWare, controlador D3D9, CRT y QuickTime, que se sustituyen según la [hoja de ruta](RutaExeIndependiente.md) y no son trabajo del juego ([lista](#bibliotecas)). "
             "La clasificación sale de la dirección del destino: dentro de 0x401000–0x74FFFF es juego, fuera es biblioteca.", "",
             "**Lotes en curso:** " + ("lotes 95A y 95C, que reconstruyen en C++ las "
                                      + f"{sum(1 for w in game_wrappers if w[4] != '—')} envolturas del juego de D3DResourceSystem y de las lecturas de streams ([lotes-en-curso.csv](JuegoExe/lotes-en-curso.csv)); cuando se validen dejarán de figurar solas en la lista de la sección 7." if any(w[4] != '—' for w in game_wrappers) else "ninguno registrado para las envolturas del juego."), "",
             f"Cerrar las envolturas del juego no cierra el juego: quedan además las funciones del EXE sin reemplazar (grupos A–D, {_d['game_remaining_total']['functions']:,} funciones al {fecha}; [sección 6](#que-falta)). El plan de trabajo sale de [AnalisisGrupoA.md](AnalisisGrupoA.md) y su [lista completa](JuegoExe/grupoA.csv), y la lista viva está en la [sección 10](#grupo-a):" if _d else "Cerrar las envolturas del juego no cierra el juego: quedan además las funciones del EXE sin reemplazar (grupos A–D).", "",
             "1. **Grupo A, por categorías y de menor a mayor coste:** helpers triviales y thunks muy llamados (hecho en el lote 90A: `CPad::GetPad` 0x53FB70, `CRGBA::Constructor` 0x7170C0...), pequeñas sin FPU (lote 90B), lógica sin FPU (empezando por las más llamadas y por clases enteras), funciones con FPU con el método x87 a 24 bits, y por último las de 0x300 bytes o más, una por lote (92B, 92C).",
             "2. **Lote piloto de cada categoría** para medir el coste real: ya medidos los de las categorías 1, 2, 3 y 5 (tabla de la sección 2); falta la 4, con FPU.",
             "3. **Revalidación vigente:** repetir los bancos sobre el árbol actual, priorizando los de fuentes cambiadas por lotes posteriores. Motivo: el banco `CVehD` llevaba roto desde el lote 83C (se cambió `m_fSteerAngle` por `m_GasPedal` en `ProcessDrivingAnims`) y no se vio hasta repetirlo.",
             "4. **Después del código del juego:** `librw`, pipelines de SA y puentes `RwD3D9*`, QuickTime, DDraw y CRT, según la [hoja de ruta](RutaExeIndependiente.md); el cambio de método de validación sin el exe como oráculo.", "",
             f"**Validación consolidada parcialmente:** {validated_count} funciones del juego con evidencia aprobada vinculada, {validation['source_matches']} con fuentes registradas coincidentes y {validation['current']} con vigencia confirmada incluyendo revisión de dependencias (cifras al {fecha}). Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).", "",
             '<a id="vista-general"></a>', "",
             "## 4. Vista general", "",
             f"Al {fecha}. Cada columna indica la dimensión y el denominador de su porcentaje.", "",
             "| Clases validadas (validación) | Clases completas por validar (validación) | Entradas validadas (validación) | Entradas reconstruidas por validar (validación) | Entradas por reconstruir (entradas del catálogo) |",
             "| :---: | :---: | :---: | :---: | :---: |",
             f"| **{len(validated_classes)} / {len(full)} clases** | **{len(full)-len(validated_classes)} / {len(full)} clases** | **{validated_count:,} / {reversed_count:,} entradas** | **{reversed_count-validated_count:,} / {reversed_count:,} entradas** | **{total-reversed_count+pending_functions:,} / {entries_total:,} entradas** |",
             f"| {len(validated_classes)/len(full):.2%} de las clases completas | {(len(full)-len(validated_classes))/len(full):.2%} de las clases completas | {validated_count/reversed_count:.2%} de las entradas reconstruidas | {(reversed_count-validated_count)/reversed_count:.2%} de las entradas reconstruidas | {(total-reversed_count+pending_functions)/entries_total:.2%} de las entradas del juego del catálogo |", "",
             f"Reconstrucción por clases: {len(full)} completas, {len(partial)} parciales y {len(empty) + len(uncatalogued)} sin revertir de {class_total} clases o grupos"
             + (f" (incluye {sum(wrap_count.values()):,} funciones del juego con envoltura al EXE aún sin reconstruir, en {len(wrappers)} clases o grupos)." if wrappers else "."), "",
             "> [!NOTE]", "> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego ni el avance sobre todo el EXE (para eso, las filas «Funciones» y «Bytes» de la sección 1). Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.", ""]
    repo, sha = os.environ.get("GITHUB_REPO_URL"), os.environ.get("GITHUB_SHA")
    if repo and sha and sha != "unknown":
        lines += [f"Generación de CI: [{sha[:12]}]({repo}/commit/{sha}).", ""]
    lines += exe_coverage.coverage_section(root, _d, '5. ')
    lines += ['<a id="que-falta"></a>', ''] + exe_coverage.missing_section(root, _d, '6. ', fecha)
    lines += exe_coverage.pending_section(root, '7. ', fecha)
    lines += ['<a id="completas"></a>', f"## 8. 🟢 Completas · {len(full)} clases", "",
              f"Al {fecha}: {len(full)} de {class_total} clases o grupos están completas (todas sus entradas reconstruidas y ninguna envoltura del juego abierta). Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 indica hooks cuya conmutación sigue bloqueada. Los bloqueos no son pendientes de validación.", "",
              "<details>", "<summary>Mostrar catálogo de clases completas</summary>", "",
              "| Clase (funciones) | Clase (funciones) | Clase (funciones) |", "| --- | --- | --- |"]
    height = (len(full) + 2) // 3
    for i in range(height):
        cells = []
        for column in range(3):
            idx = i + column * height
            if idx < len(full):
                c = full[idx]
                cells.append(
                    f"{class_link(c.name)} ({c.total})"
                    + (f" ✅ {c.validated}" if c.validated else "")
                    + (f" 🔒 {c.locked}" if c.locked else "")
                )
            else:
                cells.append("")
        lines.append("| " + " | ".join(cells) + " |")
    lines += ["", "</details>", ""]
    lines += difficulty_section(root, input_path, '9. ', fecha)
    lines += [''] + group_a_section.group_a_section(root, '10. ', fecha, _d['game_remaining']['A']['functions'] if _d else None)
    lines += ['<a id="parciales"></a>', f"## 11. 🟡 Parciales · {len(partial)} clases", "",
              "Orden de mayor a menor tamaño de las envolturas al EXE pendientes (suma de bytes de sus cuerpos; a igualdad, alfabético). «Función más grande» y «Tamaño» (muy grande ≥ 2.000 B, grande 500–1.999 B, mediana 100–499 B, pequeña < 100 B) indican cuánto pesa la función más difícil de la clase. Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).", "",
              "Incluye las clases del catálogo que ya tienen todas sus entradas reconstruidas pero conservan funciones del juego que siguen delegando en el EXE (columna «Envolturas del juego al EXE», que cuenta solo código del juego; las de biblioteca no entran): no cuentan como completas hasta reconstruirlas. Detalle por función en [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).", "",
              "| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |", "| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |"]
    if not partial:
        lines.append(f"| *(ninguna clase parcial al {fecha})* | | | | | | | | | |")
    for c in sorted(partial, key=lambda c: (-wrap_bytes.get(c.name, 0), c.name.casefold())):
        lines.append(f"| {class_link(c.name)} | {c.reversed}/{c.total} | **{c.pending}** | **{wrap_count.get(c.name, 0)}** | {_size_cells(c.name)} | {c.reversed/c.total:.2%} | {c.validated} | {c.locked} |")
    lines += ["", '<a id="sin-revertir"></a>', f"## 12. 🔴 Sin revertir · {len(empty) + len(uncatalogued)} clases", "",
              "Orden de mayor a menor tamaño de las envolturas pendientes. Clases o grupos sin ninguna entrada reconstruida en el catálogo. Las que figuran con envolturas al EXE no están en [hooks.csv](hooks.csv): sus funciones del juego se conocen por el código fuente o por las cohortes de dependencias y siguen delegando en el original.", "",
              "| Clase | Revertidas / total | Pendientes | Envolturas del juego al EXE (sin bibliotecas) | Bytes de envolturas | Función más grande (B) | Tamaño | Avance | Validated | Locked |", "| --- | ---: | ---: | ---: | ---: | ---: | --- | ---: | ---: | ---: |"]
    for c in sorted(empty, key=lambda c: (-wrap_bytes.get(c.name, 0), c.name.casefold())):
        lines.append(f"| {class_link(c.name)} | 0/{c.total} | **{c.pending}** | **{wrap_count.get(c.name, 0)}** | {_size_cells(c.name)} | 0.00% | {c.validated} | {c.locked} |")
    for name in sorted(uncatalogued, key=lambda n: (-wrap_bytes.get(n, 0), n.casefold())):
        label = name.replace('<', '&lt;').replace('>', '&gt;').replace('|', '\\|')
        lines.append(f"| {label} | 0/0 | — | **{len(wrappers[name])}** | {_size_cells(name)} | 0.00% | 0 | 0 |")
    lines += ["", '<a id="metodo"></a>', "## 13. Cómo leer y actualizar", "",
              "- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.",
              "- **Validated:** `1` indica evidencia aprobada vinculada para esa entrada. No implica vigencia actual ni independencia del EXE.",
              f"- **Locked:** indicador de bloqueo del hook; hay {locked:,} entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.",
              "- **Porcentajes:** cada porcentaje del informe indica la dimensión de la que habla (funciones, bytes, clases, entradas del catálogo o validación); las categorías cuentan clases y el avance de cada clase cuenta funciones. Se muestran dos decimales.",
              "- **Alcance:** solo incluye lo registrado en el CSV; completar una fila no certifica todo el comportamiento de una clase.", "",
              "- **Acceso al código:** pulsa el nombre para abrir el archivo que registra sus hooks. Si el registro está repartido, los enlaces adicionales abren los demás archivos. Algunas clases registran sus hooks en una cabecera.",
              "- **Comentarios de estado:** resumen por clase/grupo, con nombres de pendientes y bloqueadas. Los conteos abarcan la clase completa aunque su implementación esté repartida entre archivos.", "",
              "Sincronizar primero `validated` desde el registro de evidencia:", "",
              "```powershell", "python contrib/sync_validated_flags.py", "```", "",
              "Actualizar los flags de reconstrucción solo después de comparar con el original dentro del motor y regenerar desde la raíz:", "",
              "```powershell", "python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md", "```", "",
              "Para sincronizar también los comentarios de los archivos fuente:", "",
              "```powershell", "python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md --update-source-status", "python contrib/validation_inventory.py", "```", "",
              "Los comentarios se generan desde el CSV y la evidencia vinculada; no modifican flags ni acreditan validación. La vigencia se consulta en el [inventario](ValidationInventory.md). Añadir comentarios cambia las huellas de los archivos: el auditor conserva la evidencia histórica y exige revisar su vigencia, sin declarar fallidas las pruebas ni actualizar sus hashes de referencia.", "",
              "El formato vive en [contrib/hooks2md.py](../contrib/hooks2md.py). La [automatización de CI](../.github/workflows/update-re-progress.yml) también usa este generador. La guía de prioridades es una evaluación separada y requiere revisión cuando cambian las funciones pendientes.", ""]
    Path(output_path).write_text("\n".join(lines), encoding="utf-8", newline="\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", help="Ruta de hooks.csv")
    parser.add_argument("--output", help="Ruta del Markdown generado")
    parser.add_argument("--update-source-status", action="store_true", help="Sincronizar comentarios de estado en los archivos fuente")
    args = parser.parse_args()
    if args.input is None or args.output is None:
        from tkinter import filedialog
        args.input = args.input or filedialog.askopenfilename(title="Seleccionar hooks.csv")
        args.output = args.output or filedialog.asksaveasfilename(title="Guardar informe", defaultextension=".md")
    if not args.input or not args.output:
        parser.error("Se requieren las rutas de entrada y salida")
    generate(args.input, args.output, args.update_source_status)


if __name__ == "__main__":
    main()
