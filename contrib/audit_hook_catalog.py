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
    r'^\s*RH_Scoped(?:'
    r'Install|OverloadedInstall|VMTInstall|OverloadedVMTInstall|'
    r'GlobalInstall|GlobalOverloadedInstall'
    r')\s*\(',
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
    owner_calls = defaultdict(set)

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

        # Detecta llamadas Owner::InjectHooks() realizadas desde cualquier otro
        # archivo/función, no sólo desde InjectHooksMain.cpp. Esto captura registros
        # indirectos como CAEStreamTransformer desde CAEDataStream o clases auxiliares
        # registradas por la clase contenedora.
        for match in MAIN_CALL.finditer(data):
            owner = match.group(1)
            line_start = data.rfind('\n', 0, match.start()) + 1
            prefix = data[line_start:match.start()]
            # Excluye la propia definición "void Owner::InjectHooks()".
            if re.search(r'\b(?:void|auto|bool|int|int32|uint32)\s+


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
    owner_files, owners_with_installs, owner_calls = scan_source(root)

    # Funciones instaladas bajo owners que no aparecen en el catálogo.
    # Se separan los owners activos de los que existen en fuente pero no están
    # registrados globalmente (por ejemplo, clases deliberadamente desactivadas).
    active_missing_catalog = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner in main_calls and owner not in catalog
    )
    inactive_missing_catalog = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner not in main_calls and owner not in catalog
    )

    # Separa los owners llamados directamente desde InjectHooksMain de los que
    # llegan por una llamada indirecta desde otro InjectHooks/helper.
    indirect_registration = sorted(
        (owner, sorted(owner_calls[owner]))
        for owner in owners_with_installs
        if owner not in main_calls and owner in owner_calls
    )
    no_known_registration = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner not in main_calls and owner not in owner_calls
    )

    # Clases del catálogo sin scope localizable. source_status.py también valida
    # este caso, pero se muestra aquí para tener una auditoría bidireccional.
    missing_source = sorted(
        (owner, [])
        for owner in catalog
        if owner not in owner_files
    )

    # Llamadas directas desde InjectHooksMain sin owner propio en el catálogo.
    # Esto es informativo: puede tratarse de un agregador vacío o de un wrapper
    # que registra bajo otro scope/namespace (por ejemplo RpAnimBlendPlugin -> RpAnimBlend).
    main_without_catalog = sorted(
        (owner, sorted(owner_files.get(owner, ())))
        for owner in main_calls
        if owner not in catalog
    )

    print('Auditoría de catálogo de hooks')
    print(f'  clases/grupos en hooks.csv: {len(catalog)}')
    print(f'  owners con RH_ScopedInstall en fuente: {len(owners_with_installs)}')
    print(f'  llamadas ::InjectHooks() en InjectHooksMain: {len(main_calls)}')

    print_group('Owners ACTIVOS con funciones instaladas pero ausentes de hooks.csv', active_missing_catalog)
    print_group('Owners INACTIVOS con funciones instaladas y ausentes de hooks.csv', inactive_missing_catalog)
    print_group('Owners registrados INDIRECTAMENTE (no es error)', indirect_registration)
    print_group('Owners con funciones instaladas sin llamada Owner::InjectHooks() conocida', no_known_registration)
    print_group('Clases/grupos del CSV sin scope encontrado en fuente', missing_source)
    print_group('Llamadas de InjectHooksMain sin clase/grupo en hooks.csv', main_without_catalog)

    # --strict falla sólo por incoherencias del registro activo o por entradas del
    # catálogo sin fuente. Los owners inactivos se informan para revisión, pero no
    # se consideran error automáticamente porque muchas clases están deshabilitadas
    # de forma deliberada en InjectHooksMain.
    inconsistent = active_missing_catalog or missing_source
    if args.strict and inconsistent:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
, prefix):
                continue
            owner_calls[owner].add(relative)

    return owner_files, owners_with_installs, owner_calls


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

    # Funciones instaladas bajo owners que no aparecen en el catálogo.
    # Se separan los owners activos de los que existen en fuente pero no están
    # registrados globalmente (por ejemplo, clases deliberadamente desactivadas).
    active_missing_catalog = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner in main_calls and owner not in catalog
    )
    inactive_missing_catalog = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner not in main_calls and owner not in catalog
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

    print_group('Owners ACTIVOS con funciones instaladas pero ausentes de hooks.csv', active_missing_catalog)
    print_group('Owners INACTIVOS con funciones instaladas y ausentes de hooks.csv', inactive_missing_catalog)
    print_group('Owners con funciones instaladas pero ausentes de InjectHooksMain', missing_main)
    print_group('Clases/grupos del CSV sin scope encontrado en fuente', missing_source)
    print_group('Llamadas de InjectHooksMain sin clase/grupo en hooks.csv', main_without_catalog)

    # --strict falla sólo por incoherencias del registro activo o por entradas del
    # catálogo sin fuente. Los owners inactivos se informan para revisión, pero no
    # se consideran error automáticamente porque muchas clases están deshabilitadas
    # de forma deliberada en InjectHooksMain.
    inconsistent = active_missing_catalog or missing_source
    if args.strict and inconsistent:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
