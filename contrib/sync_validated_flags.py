"""Sincroniza docs/hooks.csv con la evidencia aprobada del registro de validación."""

import argparse
import csv
from pathlib import Path

from validation_registry import KEY, evaluate


FIELD = 'validated'


def sync(root: Path, hooks_path: Path) -> dict:
    root = Path(root).resolve()
    hooks_path = Path(hooks_path)
    if not hooks_path.is_absolute():
        hooks_path = root / hooks_path

    result = evaluate(root, hooks_path)
    validated = {
        tuple(entry['hook'][key] for key in KEY)
        for entry in result['functions']
        if entry['evidence_ok']
    }

    with hooks_path.open(encoding='utf-8-sig', newline='') as stream:
        reader = csv.DictReader(stream)
        rows = list(reader)
        fields = list(reader.fieldnames or [])

    if not rows:
        raise ValueError('hooks.csv no contiene funciones')

    if FIELD not in fields:
        type_idx = fields.index('type') if 'type' in fields else len(fields)
        fields.insert(type_idx, FIELD)

    changed = 0
    ones = 0
    for row in rows:
        key = tuple(row[name] for name in KEY)
        value = '1' if key in validated else '0'
        if row.get(FIELD) != value:
            changed += 1
        row[FIELD] = value
        ones += value == '1'

    tmp = hooks_path.with_suffix(hooks_path.suffix + '.tmp')
    with tmp.open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields, lineterminator='\n')
        writer.writeheader()
        writer.writerows(rows)
    tmp.replace(hooks_path)

    if ones != result['documented']:
        raise ValueError(
            f'validated=1 ({ones}) no coincide con evidencia aprobada ({result["documented"]})'
        )

    return {
        'total': len(rows),
        'validated': ones,
        'changed': changed,
        'current': result['current'],
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--hooks', default='docs/hooks.csv', help='Ruta de hooks.csv')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    result = sync(root, Path(args.hooks))
    print(
        f"hooks.csv: {result['validated']}/{result['total']} validated=1; "
        f"{result['current']} vigentes; {result['changed']} filas actualizadas."
    )


if __name__ == '__main__':
    main()
