"""Refresh current numerical tables from the generated memory map summaries."""
import json
import re
from pathlib import Path

root = Path(__file__).resolve().parents[1] / 'docs/MemoryMap'
source = json.loads((root / 'summary.json').read_text(encoding='utf-8'))
binary = json.loads((root / 'binary_summary.json').read_text(encoding='utf-8'))
path = root / 'README.md'
text = path.read_text(encoding='utf-8')
rows = {
    'Entradas con nombres cruzados del catálogo o sus registros fuente': binary['named_by_catalogue_or_source'],
    'Entradas sin nombre del catálogo': binary['without_catalogue_names'],
    'Entradas de `docs/hooks.csv`': source['catalogue_entries'],
    'Entradas registradas como reconstruidas': source['reversed'],
    'Registros con evidencia y coincidencia de fuentes probadas. sin acreditar dependencias': source['source_matches'],
    'Declaraciones y cuerpos candidatos de función. con repeticiones': source['source_function_candidates'],
    'Referencias de globales StaticRef/StaticArray': source['source_global_references'],
    'Extras NOTSA. fuera del catálogo principal': source['notsa_entries'],
}
for label, value in rows.items():
    pattern = r'(?m)^\| ' + re.escape(label) + r' \| [^\n]*\|$'
    text, count = re.subn(pattern, '| ' + label + ' | ' + f'{value:,}'.replace(',', '.') + ' |', text)
    assert count == 1, label
text = re.sub(r'\*\*[\d.]+ es el tamaño del catálogo de ReSA',
              '**' + f"{source['catalogue_entries']:,}".replace(',', '.') + ' es el tamaño del catálogo de ReSA', text)
text = re.sub(r'\*\*[\d.]+ entradas tienen dirección distinta de cero y [\d.]+ tienen dirección cero\*\*',
              '**' + f"{source['addressed_catalogue_entries']:,}".replace(',', '.') +
              ' entradas tienen dirección distinta de cero y ' +
              f"{source['catalogue_zero_addresses']:,}".replace(',', '.') + ' tienen dirección cero**', text)
text = text.replace('Extras propios sin equivalente en el EXE', 'Adaptaciones NOTSA fuera del catálogo principal')
text = text.replace('Extras propios sin equivalente en el EXE'.lower(), 'Adaptaciones NOTSA fuera del catálogo principal')
if '[Auditoría de cobertura fuera del catálogo]' not in text:
    text += '\n## Cobertura fuera del catálogo\n\n[Auditoría de cobertura fuera del catálogo](../BinaryCoverageAudit.md): funciones, slots y tipos sin registro, con ejemplos contrastados contra el EXE. Estas listas no son automáticamente funciones pendientes de reconstrucción.\n'
path.write_text(text, encoding='utf-8')
