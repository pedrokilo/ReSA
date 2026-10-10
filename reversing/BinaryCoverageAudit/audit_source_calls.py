"""Regenera el cruce estático de llamadas originales sin catalogue_names.

Limitaciones: patrones por línea, sin preprocesador ni análisis C++; no cubre
multilínea, asm, llamadas indirectas ni macros. Excluye sólo comentarios // al
inicio; no interpreta comentarios de bloque. Las aristas pueden ser tail jumps.
Las diez clasificaciones son revisión manual; el resto queda sin clasificar.
Los resultados NO cuantifican funciones pendientes ni acreditan runtime.
Uso: python -B reversing/BinaryCoverageAudit/audit_source_calls.py --check
Sin --check escribe review-source-calls.csv; --output permite otro destino.
"""
import argparse
import csv
import json
import re
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIELDS = ['address', 'ghidra_name', 'body_bytes', 'catalogue_names_empty',
          'source_references', 'wrapper_styles', 'sample_source_expression',
          'incoming_edges_from_catalogued_functions', 'sample_catalogued_caller',
          'sample_call_instruction', 'review_classification', 'runtime_evidence']
PLUGIN = re.compile(r'plugin::Call\w*\s*<[^;\n]*?\b(0x[0-9A-Fa-f]+)')
CAST = re.compile(r'\(\([^;\n]*?\*[^;\n]*?\)\s*(0x[0-9A-Fa-f]+)\s*\)\s*\(')
GAME = {0x5F7E80, 0x559AF0, 0x42F5A0, 0x7000E0, 0x420C40, 0x4684F0, 0x6E2D50}
CRT = {0x821B1E, 0x82119A}


def read_csv(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def generate():
    functions = read_csv(ROOT / 'docs/MemoryMap/binary_functions.csv')
    by_va = {int(row['va'], 16): row for row in functions}
    empty = {va for va, row in by_va.items() if not row['catalogue_names']}
    refs = defaultdict(list)
    styles = defaultdict(set)
    for path in (ROOT / 'source').rglob('*'):
        if not path.is_file() or path.suffix not in {'.cpp', '.h'}:
            continue
        for line_no, line in enumerate(path.read_text(encoding='utf-8', errors='replace').splitlines(), 1):
            if line.lstrip().startswith('//'):
                continue
            for pattern, style in ((PLUGIN, 'plugin_call'), (CAST, 'direct_function_pointer_cast')):
                for match in pattern.finditer(line):
                    va = int(match.group(1), 16)
                    if va in empty:
                        ref = (f'{path.relative_to(ROOT).as_posix()}:{line_no}', line.strip())
                        if ref not in refs[va]:
                            refs[va].append(ref)
                        styles[va].add(style)
    incoming = defaultdict(list)
    for edge in read_csv(ROOT / 'docs/MemoryMap/binary_calls.csv'):
        if not edge['target_va'] or not edge['caller_va']:
            continue
        target, caller = int(edge['target_va'], 16), int(edge['caller_va'], 16)
        if target in refs and caller in by_va and by_va[caller]['catalogue_names']:
            incoming[target].append((by_va[caller]['catalogue_names'], edge['instruction_va']))
    rows = []
    for va in sorted(refs, key=lambda va: (-len(incoming[va]), va)):
        classification = ('logica_juego_revisada' if va in GAME else
                          'biblioteca_CRT_identificada' if va in CRT else
                          'biblioteca_RenderWare_identificada' if va == 0x7F3AC0 else
                          'sin_clasificar_semanticamente')
        caller, instruction = incoming[va][0] if incoming[va] else ('', '')
        rows.append(dict(zip(FIELDS, [f'0x{va:08X}', by_va[va]['name'], by_va[va]['body_bytes'],
            'True', '; '.join(ref[0] for ref in refs[va]), '; '.join(sorted(styles[va])),
            refs[va][0][1], str(len(incoming[va])), caller, instruction, classification,
            'Ninguna nueva: cruce estático source/binario, no certifica reconstrucción ni equivalencia'])))
    counts = dict(binary_functions=len(functions), uncatalogued=len(empty),
                  plugin_targets=sum('plugin_call' in s for s in styles.values()),
                  cast_targets=sum('direct_function_pointer_cast' in s for s in styles.values()),
                  matched_targets=len(rows), references=sum(map(len, refs.values())),
                  reached_from_catalogued=sum(bool(incoming[va]) for va in refs))
    return rows, counts


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--check', action='store_true', help='Compara filas con el CSV existente sin escribir')
    parser.add_argument('--output', type=Path, default=Path(__file__).with_name('review-source-calls.csv'))
    args = parser.parse_args()
    rows, counts = generate()
    print(json.dumps(counts, ensure_ascii=False))
    if args.check:
        existing = read_csv(args.output)
        # Orden de referencias depende del recorrido del sistema de archivos.
        def normalized(row):
            copy = dict(row)
            copy['source_references'] = '; '.join(sorted(copy['source_references'].split('; ')))
            copy.pop('sample_source_expression')
            return copy
        assert [normalized(r) for r in rows] == [normalized(r) for r in existing], 'CSV difiere del cruce regenerado'
        print('CSV verificado: destinos, referencias, aristas y clasificaciones coinciden.')
    else:
        with args.output.open('w', encoding='utf-8', newline='') as stream:
            writer = csv.DictWriter(stream, fieldnames=FIELDS)
            writer.writeheader()
            writer.writerows(rows)


if __name__ == '__main__':
    main()
