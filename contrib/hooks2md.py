"""Generate the progress dashboard from recorded hook flags (not test coverage)."""
import argparse
import csv
import re
from pathlib import Path
import exe_coverage
import progress_report
from validation_registry import evaluate
from source_status import read_classes, source_index, update_comments


def generate(input_path, output_path, update_source_status=False):
    root = Path(__file__).resolve().parents[1]
    recorded = read_classes(input_path)
    index = source_index(root, recorded)
    if update_source_status:
        changed = update_comments(root, input_path, recorded, index)
        print(f'Comentarios: {len(index)} clases/grupos; {changed} archivos actualizados.')
    validation = evaluate(root, input_path)
    with open(input_path, encoding='utf-8-sig', newline='') as stream:
        rows = list(csv.DictReader(stream))
    if not rows:
        raise ValueError('hooks.csv no contiene funciones')
    validated_rows = sum(int(r['validated']) for r in rows)
    if validated_rows != validation['documented']:
        raise ValueError(
            f"hooks.csv validated=1 ({validated_rows}) no coincide con evidencia aprobada "
            f"({validation['documented']}). Ejecuta: python contrib/sync_validated_flags.py"
        )
    coverage = exe_coverage.compute(root)
    lines, readme_core = progress_report.build(root, input_path, coverage, validation)
    readme = root / 'README.md'
    marker = re.compile(r'<!-- RESA-PROGRESO:BEGIN -->.*?<!-- RESA-PROGRESO:END -->', re.S)
    if readme.exists():
        text = readme.read_text(encoding='utf-8')
        if marker.search(text):
            nl = chr(10)
            block = '<!-- RESA-PROGRESO:BEGIN -->' + nl + nl.join(readme_core).strip(nl) + nl + '<!-- RESA-PROGRESO:END -->'
            readme.write_text(marker.sub(lambda _: block, text, count=1), encoding='utf-8', newline=chr(10))
    Path(output_path).write_text(chr(10).join(lines), encoding='utf-8', newline=chr(10))


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
