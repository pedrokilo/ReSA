"""Map hook owners to source files and maintain generated Spanish status comments."""
import csv
import re
from collections import Counter, defaultdict
from pathlib import Path

from validation_registry import evaluate


SCOPE = re.compile(
    r'^\s*RH_Scoped(?:VirtualClass|Class|Namespace|NamespaceName)'
    r'\(\s*"?([^",)\n]+)"?\s*[,)]', re.MULTILINE
)
BLOCK = re.compile(
    rb'/\* BEGIN GENERATED HOOK STATUS\r?\n.*?'
    rb'END GENERATED HOOK STATUS \*/\r?\n(?:\r?\n)?', re.DOTALL
)


def read_classes(input_path):
    classes = defaultdict(list)
    with Path(input_path).open(encoding='utf-8-sig', newline='') as stream:
        for row in csv.DictReader(stream):
            if row['reversed'] not in ('0', '1') or row['locked'] not in ('0', '1'):
                raise ValueError(f'Invalid hook flags: {row}')
            classes[row['class']].append(row)
    if not classes:
        raise ValueError('hooks.csv no contiene funciones')
    return classes


def source_index(root, classes):
    """Use hook registration owners, not ambiguous filename guesses or uses."""
    result = defaultdict(list)
    for path in sorted((Path(root) / 'source').rglob('*')):
        if path.suffix not in ('.cpp', '.h', '.hpp'):
            continue
        data = BLOCK.sub(b'', path.read_bytes()).decode('utf-8-sig')
        # Ignore commented-out registrations while retaining line boundaries.
        data = re.sub(r'/\*.*?\*/|//[^\n]*', '', data, flags=re.DOTALL)
        owners = {m[1].strip() for m in SCOPE.finditer(data)}
        for name in owners & classes.keys():
            result[name].append(path)
    missing = sorted(classes.keys() - result.keys())
    if missing:
        raise ValueError(f'Clases sin archivo de registro de hooks: {missing}')
    return dict(result)


def status_block(names, classes, evidence):
    lines = ['/* BEGIN GENERATED HOOK STATUS',
             ' * Generado desde docs/hooks.csv; no editar este bloque manualmente.',
             ' * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv',
             ' *   --output docs/ReversedClasses.md --update-source-status',
             ' * Conteos por clase/grupo completo, no solo por este archivo.',
             ' * Pendientes y bloqueadas pueden coincidir; no se suman.']
    for name in sorted(names, key=str.casefold):
        rows = classes[name]
        pending = [r for r in rows if r['reversed'] == '0']
        locked = [r for r in rows if r['locked'] == '1']
        done = len(rows) - len(pending)
        state = 'COMPLETA' if not pending else 'SIN RECONSTRUIR' if not done else 'INCOMPLETA'
        lines += [' *', f' * ESTADO: {name}',
                  f' * Reconstrucción: {state} — {done}/{len(rows)} funciones reconstruidas.',
                  f' * Pendientes de reconstruir: {len(pending)}.',
                  f' * Funciones bloqueadas: {len(locked)}.',
                  f' * Evidencia aprobada vinculada: {evidence[name]}/{len(rows)} entradas.',
                  ' * Validación: consultar vigencia en docs/ValidationInventory.md y',
                  ' *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada',
                  ' *   no significa nunca probada ni fallida. Reconstruida no implica validada.']
        for label, entries in [('PENDIENTES', pending), ('BLOQUEADAS', locked)]:
            if entries:
                lines.append(f' * {label} (nombre | dirección | tipo):')
                for row in sorted(entries, key=lambda r: (r['fn_name'], r['address'], r['type'])):
                    lines.append(f" *   {row['fn_name']} | {row['address']} | {row['type']}")
    lines += [' * Alcance: solo entradas catalogadas; no certifica independencia del EXE.',
              'END GENERATED HOOK STATUS */', '', '']
    # Do not let a malformed catalog entry terminate a C++ comment.
    if any('*/' in line for line in lines[:-3]):
        raise ValueError('Unsafe comment content in hook catalog')
    return '\n'.join(lines)


def update_comments(root, input_path, classes, index):
    # Only evidence integrity is embedded: source/current hashes would become
    # stale through this very write. The dashboard evaluates those AFTER writes.
    validation = evaluate(root, input_path)
    evidence = Counter(r['hook']['class'] for r in validation['functions'] if r['evidence_ok'])
    owners = defaultdict(list)
    for name, paths in index.items():
        for path in paths:
            owners[path].append(name)
    changes = []
    for path in sorted((Path(root) / 'source').rglob('*')):
        if path.suffix not in ('.cpp', '.h', '.hpp'):
            continue
        original = path.read_bytes()
        if path not in owners and not BLOCK.search(original):
            continue
        stripped = BLOCK.sub(b'', original)
        bom = b'\xef\xbb\xbf' if stripped.startswith(b'\xef\xbb\xbf') else b''
        body = stripped[len(bom):]
        newline = '\r\n' if b'\r\n' in body else '\n'
        comment = status_block(owners[path], classes, evidence) if path in owners else ''
        updated = bom + comment.replace('\n', newline).encode('utf-8') + body
        if original != updated:
            changes.append((path, updated))
    # Finish planning and validation before the first write.
    for path, updated in changes:
        path.write_bytes(updated)
    return len(changes)
