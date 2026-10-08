"""Sincroniza el cuadro editorial del mapa con su extracción final."""
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
folder = ROOT/'docs/MemoryMap'
summary = json.loads((folder/'summary.json').read_text())
assert not summary['source_files_changed_during_generation']
labels = {
    'Entradas de `docs/hooks.csv`':'catalogue_entries',
    'Clases, namespaces o grupos del catálogo':'catalogue_owners',
    'Entradas registradas como reconstruidas':'reversed',
    'Pendientes de reconstrucción':'pending',
    'Evidencias aprobadas vinculadas, verificadas por el auditor':'approved_evidence',
    'Validaciones con vigencia acreditada para su alcance':'current_validation',
    'Registros con evidencia y coincidencia de fuentes probadas, sin acreditar dependencias':'source_matches',
    'Definiciones de class/struct en source, con repeticiones':'source_type_definitions',
    'Nombres distintos de tipos captados':'source_unique_type_names',
    'Declaraciones y cuerpos candidatos de función, con repeticiones':'source_function_candidates',
    'Referencias de vtable declaradas en el código':'source_vtable_references',
    'Referencias de globales StaticRef/StaticArray':'source_global_references',
    'Registros explícitos de hooks captados':'source_hook_references',
    'Archivos fuente examinados':'source_files',
    'Extras NOTSA, fuera del catálogo principal':'notsa_entries',
}
path = folder/'README.md'
text = path.read_text(encoding='utf-8')
for label, key in labels.items():
    text, count = re.subn(r'(?m)^\| '+re.escape(label)+r' \| [\d.]+ \|$',
                         f'| {label} | {summary[key]:,} |'.replace(',','.'), text)
    assert count == 1, label
path.write_text(text, encoding='utf-8')
print('README del mapa sincronizado')
