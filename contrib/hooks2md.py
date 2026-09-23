"""Generate the progress dashboard from recorded hook flags (not test coverage)."""
import argparse
import csv
import datetime
import os
from dataclasses import dataclass
from pathlib import Path


@dataclass
class Klass:
    name: str
    reversed: int = 0
    total: int = 0
    locked: int = 0

    @property
    def pending(self):
        return self.total - self.reversed


def generate(input_path, output_path):
    classes = {}
    with open(input_path, encoding="utf-8-sig", newline="") as stream:
        for row in csv.DictReader(stream):
            item = classes.setdefault(row["class"], Klass(row["class"]))
            item.total += 1
            item.reversed += int(row["reversed"])
            item.locked += int(row["locked"])
    if not classes:
        raise ValueError("hooks.csv no contiene funciones")
    ordered = sorted(classes.values(), key=lambda c: c.name.casefold())
    full = [c for c in ordered if not c.pending]
    partial = [c for c in ordered if c.reversed and c.pending]
    empty = [c for c in ordered if not c.reversed]
    total = sum(c.total for c in ordered)
    reversed_count = sum(c.reversed for c in ordered)
    locked = sum(c.locked for c in ordered)
    now = datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%d %H:%M UTC")
    lines = ["# Progreso del reversing", "",
             "[Inicio](../README.md) · [Prioridades técnicas](ReversingPriorities.md) · [Pruebas y evidencia](../reversing/README.md)", "",
             f"> **Fuente:** [hooks.csv](hooks.csv) · **Generado:** {now}", "",
             "## Vista general", "",
             "| Clases completas | Clases parciales | Sin revertir | Funciones revertidas | Funciones pendientes |",
             "| :---: | :---: | :---: | :---: | :---: |",
             f"| **{len(full)} / {len(ordered)}** | **{len(partial)} / {len(ordered)}** | **{len(empty)} / {len(ordered)}** | **{reversed_count:,} / {total:,}** | **{total-reversed_count:,}** |",
             f"| {len(full)/len(ordered):.2%} | {len(partial)/len(ordered):.2%} | {len(empty)/len(ordered):.2%} | {reversed_count/total:.2%} | {(total-reversed_count)/total:.2%} |", "",
             "**Ir a:** [Completas](#completas) · [Parciales](#parciales) · [Sin revertir](#sin-revertir) · [Cómo leer y actualizar](#metodo)", "",
             "> [!NOTE]", "> Estos porcentajes miden entradas registradas como `reversed`, no cobertura de pruebas ni estabilidad del juego. Una clase completa puede conservar hooks bloqueados o funciones todavía no registradas.", ""]
    repo, sha = os.environ.get("GITHUB_REPO_URL"), os.environ.get("GITHUB_SHA")
    if repo and sha and sha != "unknown":
        lines += [f"Generación de CI: [{sha[:12]}]({repo}/commit/{sha}).", ""]
    lines += ['<a id="completas"></a>', f"## 🟢 Completas · {len(full)} clases", "",
              "Orden alfabético. El número entre paréntesis indica funciones registradas; 🔒 señala que al menos una sigue bloqueada.", "",
              "<details>", "<summary>Mostrar catálogo de clases completas</summary>", "",
              "| Clase (funciones) | Clase (funciones) | Clase (funciones) |", "| --- | --- | --- |"]
    height = (len(full) + 2) // 3
    for i in range(height):
        cells = []
        for column in range(3):
            idx = i + column * height
            if idx < len(full):
                c = full[idx]
                cells.append(f"`{c.name}` ({c.total})" + (f" 🔒 {c.locked}" if c.locked else ""))
            else:
                cells.append("")
        lines.append("| " + " | ".join(cells) + " |")
    lines += ["", "</details>", "", '<a id="parciales"></a>', f"## 🟡 Parciales · {len(partial)} clases", "",
              "Orden alfabético para localizar clases. Para decidir qué abordar primero, consultar la [estimación por dificultad](ReversingPriorities.md).", "",
              "| Clase | Revertidas / total | Pendientes | Avance | Locked |", "| --- | ---: | ---: | ---: | ---: |"]
    for c in partial:
        lines.append(f"| `{c.name}` | {c.reversed}/{c.total} | **{c.pending}** | {c.reversed/c.total:.2%} | {c.locked} |")
    lines += ["", '<a id="sin-revertir"></a>', f"## 🔴 Sin revertir · {len(empty)} clases", "",
              "| Clase | Revertidas / total | Pendientes | Avance | Locked |", "| --- | ---: | ---: | ---: | ---: |"]
    for c in empty:
        lines.append(f"| `{c.name}` | 0/{c.total} | **{c.pending}** | 0.00% | {c.locked} |")
    lines += ["", '<a id="metodo"></a>', "## Cómo leer y actualizar", "",
              "- **Pendientes:** total menos revertidas. Cada fila del CSV cuenta como una entrada, incluidas sobrecargas y virtuales.",
              f"- **Locked:** indicador de bloqueo del hook; hay {locked:,} entradas bloqueadas en el CSV. No equivale por sí solo a pendiente ni informa si el hook está activo durante una ejecución.",
              "- **Porcentajes:** las categorías cuentan clases; el avance de cada clase cuenta funciones. Se muestran dos decimales.",
              "- **Alcance:** solo incluye lo registrado en el CSV; completar una fila no certifica todo el comportamiento de una clase.", "",
              "Actualizar los flags solo después de comparar con el original dentro del motor y regenerar desde la raíz:", "",
              "```powershell", "python contrib/hooks2md.py --input docs/hooks.csv --output docs/ReversedClasses.md", "```", "",
              "El formato vive en [contrib/hooks2md.py](../contrib/hooks2md.py). La [automatización de CI](../.github/workflows/update-re-progress.yml) también usa este generador. La guía de prioridades es una evaluación separada y requiere revisión cuando cambian las funciones pendientes.", ""]
    Path(output_path).write_text("\n".join(lines), encoding="utf-8", newline="\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", help="Ruta de hooks.csv")
    parser.add_argument("--output", help="Ruta del Markdown generado")
    args = parser.parse_args()
    if args.input is None or args.output is None:
        from tkinter import filedialog
        args.input = args.input or filedialog.askopenfilename(title="Seleccionar hooks.csv")
        args.output = args.output or filedialog.asksaveasfilename(title="Guardar informe", defaultextension=".md")
    if not args.input or not args.output:
        parser.error("Se requieren las rutas de entrada y salida")
    generate(args.input, args.output)


if __name__ == "__main__":
    main()
