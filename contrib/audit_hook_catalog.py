"""Audita la coherencia entre registros de hooks, InjectHooksMain y docs/hooks.csv."""

import argparse
import csv
import re
from collections import defaultdict
from pathlib import Path


SCOPE = re.compile(
    r'^\s*RH_Scoped(?:VirtualClass|Class|Namespace|NamespaceName)'
    r'\(\s*"?([^",)\n]+)"?\s*[,)]',
    re.MULTILINE,
)
INSTALL = re.compile(
    r'^\s*RH_Scoped(?:Overloaded)?Install\s*\(',
    re.MULTILINE,
)
MAIN_CALL = re.compile(
    r'\b([A-Za-z_][A-Za-z0-9_:<>]*)::InjectHooks\s*\(\s*\)\s*;',
)
COMMENT = re.compile(r'/\*.*?\*/|//[^\n]*', re.DOTALL)


def strip_comments(text: str) -> str:
    return COMMENT.sub('', text)


def read_catalog(path: Path) -> set[str]:
    with path.open(encoding='utf-8-sig', newline='') as stream:
        rows = list(csv.DictReader(stream))
    if not rows:
        raise ValueError(f'{path} no contiene entradas')
    return {row['class'] for row in rows}


def read_main_calls(path: Path) -> set[str]:
    return set(MAIN_CALL.findall(strip_comments(path.read_text(encoding='utf-8-sig'))))


def scan_source(root: Path):
    owner_files = defaultdict(set)
    owners_with_installs = defaultdict(set)

    for path in sorted((root / 'source').rglob('*')):
        if path.suffix not in ('.cpp', '.h', '.hpp'):
            continue

        data = strip_comments(path.read_text(encoding='utf-8-sig', errors='replace'))
        owners = {m.group(1).strip() for m in SCOPE.finditer(data)}
        if not owners:
            continue

        relative = path.relative_to(root).as_posix()
        for owner in owners:
            owner_files[owner].add(relative)

        if INSTALL.search(data):
            for owner in owners:
                owners_with_installs[owner].add(relative)

    return owner_files, owners_with_installs


def print_group(title: str, entries: list[tuple[str, list[str]]]):
    print(f'\n{title}: {len(entries)}')
    for owner, paths in entries:
        where = ', '.join(paths)
        print(f'  - {owner}: {where}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', default=None, help='Raíz del repositorio')
    parser.add_argument('--csv', default='docs/hooks.csv', help='Catálogo de hooks')
    parser.add_argument(
        '--strict',
        action='store_true',
        help='Salir con código 1 si se detectan inconsistencias',
    )
    args = parser.parse_args()

    root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parents[1]
    catalog = read_catalog(root / args.csv)
    main_calls = read_main_calls(root / 'source' / 'InjectHooksMain.cpp')
    owner_files, owners_with_installs = scan_source(root)

    # Caso más importante: hay funciones instaladas bajo un owner que no existe
    # como clase/grupo en hooks.csv.
    missing_catalog = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner not in catalog
    )

    # Hooks definidos en fuente que nunca se registran globalmente.
    missing_main = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner not in main_calls
    )

    # Clases del catálogo sin scope localizable. source_status.py también valida
    # este caso, pero se muestra aquí para tener una auditoría bidireccional.
    missing_source = sorted(
        (owner, [])
        for owner in catalog
        if owner not in owner_files
    )

    # Llamadas globales sin clase/grupo correspondiente en el catálogo. Puede
    # incluir owners sin funciones instaladas; se separa del caso crítico anterior.
    main_without_catalog = sorted(
        (owner, sorted(owner_files.get(owner, ())))
        for owner in main_calls
        if owner not in catalog
    )

    print('Auditoría de catálogo de hooks')
    print(f'  clases/grupos en hooks.csv: {len(catalog)}')
    print(f'  owners con RH_ScopedInstall en fuente: {len(owners_with_installs)}')
    print(f'  llamadas ::InjectHooks() en InjectHooksMain: {len(main_calls)}')

    print_group('Owners con funciones instaladas pero ausentes de hooks.csv', missing_catalog)
    print_group('Owners con funciones instaladas pero ausentes de InjectHooksMain', missing_main)
    print_group('Clases/grupos del CSV sin scope encontrado en fuente', missing_source)
    print_group('Llamadas de InjectHooksMain sin clase/grupo en hooks.csv', main_without_catalog)

    inconsistent = missing_catalog or missing_main or missing_source
    if args.strict and inconsistent:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
