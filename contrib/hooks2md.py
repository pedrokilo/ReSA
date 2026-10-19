"""Generate the progress dashboard from recorded hook flags (not test coverage)."""
import argparse
import csv
import json
import datetime
import os
from dataclasses import dataclass
from pathlib import Path
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


def difficulty_section(root, input_path):
    """Sección con la lista por dificultad del cohorte nuevo (reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv)."""
    source = root / 'reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv'
    if not source.exists():
        return []
    with source.open(encoding='utf-8-sig', newline='') as stream:
        items = list(csv.DictReader(stream))
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
    registry = root / 'reversing/validation-registry.json'
    if virtual_rows and registry.exists():
        for entry in json.loads(registry.read_text(encoding='utf-8')).get('functions', []):
            hook = entry.get('hook', {})
            key = (hook.get('class'), hook.get('fn_name'))
            original = entry.get('evidence', {}).get('original_address', '')
            if key in virtual_rows and original:
                try:
                    state[int(original, 16)] = virtual_rows[key]
                except ValueError:
                    pass

    def status(row):
        try:
            validated = state.get(int(row['address'], 16))
        except ValueError:
            validated = None
        return 'pendiente' if validated is None else ('✅ validada' if validated else 'reconstruida')

    def cell(text):
        return str(text).replace('|', '\\|').replace('<', '&lt;').replace('>', '&gt;')

    levels = sorted({r['nivel'] for r in items})
    lines = ['', '<a id="cohorte-nuevo"></a>', '## 📋 Cohorte nuevo (17/10/2026): lista por dificultad', '',
             'Destinos originales explícitos que `source/` sigue referenciando y que no estaban reconstruidos ni validados el 17/10/2026, las dos listas completas van ordenadas de menor a mayor tamaño en bytes (la columna «Rango» conserva la posición por dificultad, de menor a mayor: 1 es la más fácil) '
             '(juego: puntuación de tamaño y riesgos —llamadas sin catalogar, indirectas, x87, tablas de saltos y SEH—; biblioteca: solo tamaño). '
             f'Lista completa y descargable: [lista-por-dificultad.csv](../{source.relative_to(root).as_posix()}) · metodología: [README del cohorte](../reversing/OriginalDependencies2026-10-17/README.md). '
             'La columna «Estado» se calcula con el catálogo actual: una dirección con fila reversed=1 en hooks.csv figura como reconstruida o validada.', '',
             '| Nivel | Juego | de ellas pendientes | Biblioteca | de ellas pendientes |', '| --- | ---: | ---: | ---: | ---: |']
    for level in levels:
        game = [r for r in items if r['nivel'] == level and r['categoria'] == 'juego']
        lib = [r for r in items if r['nivel'] == level and r['categoria'] == 'biblioteca']
        lines.append(f"| {cell(level)} | {len(game)} | {sum(status(r) == 'pendiente' for r in game)} | {len(lib)} | {sum(status(r) == 'pendiente' for r in lib)} |")
    for categoria, titulo in (('juego', 'Funciones del juego'), ('biblioteca', 'Bibliotecas (RenderWare, controlador D3D9, CRT, QuickTime)')):
        rows = sorted((r for r in items if r['categoria'] == categoria), key=lambda r: (int(r['bytes']), int(r['rango'])))
        lines += ['', '<details>', f'<summary>{titulo} ({len(rows)}), de menor a mayor tamaño en bytes</summary>', '',
                  '| Rango | Nivel | Subcategoría | Función | Dirección | Bytes | Estado |', '| ---: | --- | --- | --- | --- | ---: | --- |']
        for r in rows:
            lines.append(f"| {r['rango']} | {cell(r['nivel'])} | {cell(r['subcategoria'])} | {cell(r['identity'])} | `{r['address']}` | {r['bytes']} | {status(r)} |")
        lines += ['', '</details>']
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
    ordered = sorted(classes.values(), key=lambda c: c.name.casefold())
    # Funciones del juego que siguen delegando en el EXE (docs/implementaciones/envolturas-pendientes.csv, contrib/pending_wrappers.py).
    # Una clase del catálogo con envolturas pendientes ya no es «completa»: pasa a parciales. Las clases con envolturas que no figuran
    # en el catálogo aparecen en «sin revertir» con su cantidad de envolturas.
    wrappers = {}
    wrappers_file = Path(input_path).with_name('implementaciones') / 'envolturas-pendientes.csv'
    if wrappers_file.exists():
        with wrappers_file.open(encoding='utf-8-sig', newline='') as stream:
            for row in csv.DictReader(stream):
                wrappers.setdefault(row['class'], []).append(row['function'])
    wrap_count = {name: len(fns) for name, fns in wrappers.items()}
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
    if validated_count != validation['documented']:
        raise ValueError(
            f"hooks.csv validated=1 ({validated_count}) no coincide con evidencia aprobada "
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
                    key = (row['catalog_identity'], row['catalog_address'])
                    linked[key] = linked.get(key, True) and row['language'] == 'C++'
        shared_file = Path(input_path).with_name('implementaciones') / 'catalogo-cuerpo-compartido.csv'
        if shared_file.exists():  # entradas servidas por el cuerpo genérico de una plantilla (instanciaciones, clases derivadas)
            with shared_file.open(encoding='utf-8-sig', newline='') as stream:
                for row in csv.DictReader(stream):
                    if row['reversed'] == '1' and row['catalog'] == Path(input_path).name:
                        key = (row['catalog_identity'], row['catalog_address'])
                        linked[key] = linked.get(key, True) and row['language'] == 'C++'
        cpp_only = sum(linked.values())
    cpp_row = ([f"| Funciones reconstruidas escritas solo en C++ (sin ensamblador) | `{'█' * round(20*cpp_only/cpp_total)}{'░' * (20-round(20*cpp_only/cpp_total))}` | {cpp_only:,}/{cpp_total:,} · {cpp_only/cpp_total:.2%} |"]
               if inventory.exists() and cpp_total else [])
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
    lines = ["# Progreso del reversing", "",
             "[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)", "",
             f"> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** {now}", "",
             "## Objetivo y dimensiones de progreso", "",
             "La [hoja de ruta de ReSA](ProjectRoadmap.md) define una reconstrucción funcionalmente equivalente, validada y progresivamente independiente del EXE, seguida de un ejecutable propio. Completar este catálogo es un hito intermedio hacia **ReSA — Zero Original Function Dependency**.", "",
             "| Dimensión | Progreso | Medición |",
             "| --- | --- | --- |",
             f"| Clases completas | `{'█' * round(20*len(full)/class_total)}{'░' * (20-round(20*len(full)/class_total))}` | {len(full)}/{class_total} · {len(full)/class_total:.2%} |",
             f"| Clases validadas (de las completas) | `{'█' * round(20*len(validated_classes)/len(full))}{'░' * (20-round(20*len(validated_classes)/len(full)))}` | {len(validated_classes)}/{len(full)} · {len(validated_classes)/len(full):.2%} |",
             f"| Funciones reconstruidas | `{'█' * round(20*reversed_count/(total+pending_functions))}{'░' * (20-round(20*reversed_count/(total+pending_functions)))}` | {reversed_count:,}/{total+pending_functions:,} · {reversed_count/(total+pending_functions):.2%} |",
             f"| Funciones validadas (de las reconstruidas) | `{'█' * round(20*validated_count/reversed_count)}{'░' * (20-round(20*validated_count/reversed_count))}` | {validated_count:,}/{reversed_count:,} · {validated_count/reversed_count:.2%} |", *cpp_row, "",
             "Las clases y funciones validadas se miden sobre lo ya completado o reconstruido: solo se valida lo que existe en la DLL." + (" La última barra cuenta, de las funciones reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md)." if cpp_row else ""), "",
             migration_text, "",
             f"Hay {total:,} entradas catalogadas, {locked:,} bloqueadas y {total-locked:,} sin bloqueo. No constituyen un inventario exhaustivo del EXE. Desbloqueada no significa validada, activa ni independiente. No se deducen hooks eliminados del contador de locks."
             + (f" Las extensiones o adaptaciones NOTSA ({notsa_count}, en [hooks_notsa.csv](hooks_notsa.csv)) quedan fuera de la cuenta." if notsa_count else ""), "",
             "### Siguientes mediciones", "",
             "Estas dimensiones todavía no tienen un porcentaje útil: indican el trabajo que falta para poder medirlas.", "",
             "| Dimensión | Estado actual | Qué hay que hacer |",
             "| --- | --- | --- |",
             f"| Validaciones vigentes para el árbol actual | {validation['current']}/{validated_count:,} validadas confirmadas como vigentes | Volver a correr los bancos de cada función validada sobre el árbol actual y revisar que sus dependencias (funciones llamadas) no hayan cambiado desde la evidencia; registrar el resultado en el [registro de validaciones](../reversing/ValidationRegistry/README.md). |",
             "| Dependencias de funciones originales eliminadas | No medido | Auditar, para cada función reconstruida, las llamadas directas y transitivas que todavía saltan a código del EXE (`plugin::Call*`, direcciones fijas, hooks sin reconstruir) y contar las que ya no dependen de ninguna. |", "",
             f"**Validación consolidada parcialmente:** {validation['documented']} funciones con evidencia aprobada vinculada, {validation['source_matches']} con fuentes registradas coincidentes y {validation['current']} con vigencia confirmada incluyendo revisión de dependencias. Las restantes no se clasifican como fallidas: falta consolidarlas o revisar su vigencia. [Registro, alcance y procedimiento](../reversing/ValidationRegistry/README.md).", "",
             "## Vista general", "",
             "| Clases validadas | Completas por validar | Funciones validadas | Reconstruidas por validar | Funciones por reconstruir |",
             "| :---: | :---: | :---: | :---: | :---: |",
             f"| **{len(validated_classes)} / {len(full)}** | **{len(full)-len(validated_classes)} / {len(full)}** | **{validated_count:,} / {reversed_count:,}** | **{reversed_count-validated_count:,} / {reversed_count:,}** | **{total-reversed_count+pending_functions:,} / {total+pending_functions:,}** |",
             f"| {len(validated_classes)/len(full):.2%} | {(len(full)-len(validated_classes))/len(full):.2%} | {validated_count/reversed_count:.2%} | {(reversed_count-validated_count)/reversed_count:.2%} | {(total-reversed_count+pending_functions)/(total+pending_functions):.2%} |", "",
             f"Reconstrucción por clases: {len(full)} completas, {len(partial)} parciales y {len(empty) + len(uncatalogued)} sin revertir de {class_total}"
             + (f" (incluye {sum(wrap_count.values()):,} funciones del juego con envoltura al EXE aún sin reconstruir, en {len(wrappers)} clases o grupos)." if wrappers else "."), "",
             "**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cohorte nuevo por dificultad](#cohorte-nuevo) · [Cómo leer y actualizar](#metodo)", "",
             "> [!NOTE]", "> Los porcentajes de reconstrucción miden entradas registradas como `reversed`; los de validación, entradas con evidencia aprobada (`validated=1`). Ninguno mide estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.", ""]
    repo, sha = os.environ.get("GITHUB_REPO_URL"), os.environ.get("GITHUB_SHA")
    if repo and sha and sha != "unknown":
        lines += [f"Generación de CI: [{sha[:12]}]({repo}/commit/{sha}).", ""]
    lines += ['<a id="completas"></a>', f"## 🟢 Completas · {len(full)} clases", "",
              "Orden alfabético. El número entre paréntesis indica funciones registradas; ✅ indica entradas con validated=1 y 🔒 indica hooks cuya conmutación sigue bloqueada. Los bloqueos no son pendientes de validación.", "",
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
    lines += ["", "</details>", "", '<a id="parciales"></a>', f"## 🟡 Parciales · {len(partial)} clases", "",
              "Orden de menor a mayor número de envolturas al EXE pendientes (a igualdad, de menor a mayor número de entradas pendientes y alfabético). Para decidir qué abordar primero, consultar la [cola de validación por niveles](ReversingPriorities.md).", "",
              "Incluye las clases del catálogo que ya tienen todas sus entradas reconstruidas pero conservan funciones del juego que siguen delegando en el EXE (columna «Envolturas al EXE»): no cuentan como completas hasta reconstruirlas. Detalle por función en [envolturas-pendientes.csv](implementaciones/envolturas-pendientes.csv).", "",
              "| Clase | Revertidas / total | Pendientes | Envolturas al EXE | Avance | Validated | Locked |", "| --- | ---: | ---: | ---: | ---: | ---: | ---: |"]
    for c in sorted(partial, key=lambda c: (wrap_count.get(c.name, 0), c.pending, c.name.casefold())):
        lines.append(f"| {class_link(c.name)} | {c.reversed}/{c.total} | **{c.pending}** | **{wrap_count.get(c.name, 0)}** | {c.reversed/c.total:.2%} | {c.validated} | {c.locked} |")
    lines += ["", '<a id="sin-revertir"></a>', f"## 🔴 Sin revertir · {len(empty) + len(uncatalogued)} clases", "",
              "Clases o grupos sin ninguna entrada reconstruida en el catálogo. Las que figuran con envolturas al EXE no están en [hooks.csv](hooks.csv): sus funciones del juego se conocen por el código fuente o por las cohortes de dependencias y siguen delegando en el original.", "",
              "| Clase | Revertidas / total | Pendientes | Envolturas al EXE | Avance | Validated | Locked |", "| --- | ---: | ---: | ---: | ---: | ---: | ---: |"]
    for c in sorted(empty, key=lambda c: (wrap_count.get(c.name, 0), c.pending, c.name.casefold())):
        lines.append(f"| {class_link(c.name)} | 0/{c.total} | **{c.pending}** | **{wrap_count.get(c.name, 0)}** | 0.00% | {c.validated} | {c.locked} |")
    for name in sorted(uncatalogued, key=lambda n: (len(wrappers[n]), n.casefold())):
        label = name.replace('<', '&lt;').replace('>', '&gt;').replace('|', '\\|')
        lines.append(f"| {label} | 0/0 | — | **{len(wrappers[name])}** | 0.00% | 0 | 0 |")
    lines += difficulty_section(root, input_path)
    lines += ["", '<a id="metodo"></a>', "## Cómo leer y actualizar", "",
              "- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.",
              "- **Validated:** `1` indica evidencia aprobada vinculada para esa entrada. No implica vigencia actual ni independencia del EXE.",
              f"- **Locked:** indicador de bloqueo del hook; hay {locked:,} entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.",
              "- **Porcentajes:** las categorías cuentan clases; el avance de cada clase cuenta funciones. Se muestran dos decimales.",
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
