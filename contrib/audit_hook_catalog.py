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

INJECT_CALL = re.compile(
    r'\b([A-Za-z_][A-Za-z0-9_:<>]*)::InjectHooks\s*\(\s*\)\s*;',
)

INJECT_DEF = re.compile(
    r'\b(?:void|auto|bool|int|int32|uint32|size_t)\s+'
    r'([A-Za-z_][A-Za-z0-9_:<>]*)::InjectHooks\s*\(',
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
    data = strip_comments(path.read_text(encoding='utf-8-sig', errors='replace'))
    return set(INJECT_CALL.findall(data))


def scan_source(root: Path):
    owner_files = defaultdict(set)
    owners_with_installs = defaultdict(set)
    owner_calls = defaultdict(set)

    for path in sorted((root / 'source').rglob('*')):
        if path.suffix not in ('.cpp', '.h', '.hpp'):
            continue

        data = strip_comments(path.read_text(encoding='utf-8-sig', errors='replace'))
        relative = path.relative_to(root).as_posix()

        owners = {m.group(1).strip() for m in SCOPE.finditer(data)}
        for owner in owners:
            owner_files[owner].add(relative)

        if owners and INSTALL.search(data):
            for owner in owners:
                owners_with_installs[owner].add(relative)

        definitions = set(INJECT_DEF.findall(data))
        for owner in INJECT_CALL.findall(data):
            # Una definición Owner::InjectHooks() no es una llamada.
            if owner not in definitions:
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
        help='Salir con código 1 si se detectan inconsistencias del registro activo',
    )
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Mostrar también owners inactivos, registros indirectos y aliases informativos',
    )
    args = parser.parse_args()

    root = Path(args.root).resolve() if args.root else Path(__file__).resolve().parents[1]
    catalog = read_catalog(root / args.csv)
    main_calls = read_main_calls(root / 'source' / 'InjectHooksMain.cpp')
    owner_files, owners_with_installs, owner_calls = scan_source(root)

    # Inconsistencia crítica: el owner se registra directamente desde
    # InjectHooksMain, instala funciones, pero no existe en hooks.csv.
    active_missing_catalog = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner in main_calls and owner not in catalog
    )

    # Código de hooks presente pero no llamado directamente desde InjectHooksMain.
    # Puede ser deliberadamente inactivo o registrarse indirectamente.
    catalogued_indirect = sorted(
        (owner, sorted(owner_calls[owner]))
        for owner in owners_with_installs
        if owner not in main_calls and owner in owner_calls and owner in catalog
    )

    indirect_missing_catalog = sorted(
        (owner, sorted(owner_calls[owner]))
        for owner in owners_with_installs
        if owner not in main_calls and owner in owner_calls and owner not in catalog
    )

    inactive_missing_catalog = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner not in main_calls and owner not in owner_calls and owner not in catalog
    )

    no_known_registration = sorted(
        (owner, sorted(paths))
        for owner, paths in owners_with_installs.items()
        if owner not in main_calls and owner not in owner_calls and owner in catalog
    )

    missing_source = sorted(
        (owner, [])
        for owner in catalog
        if owner not in owner_files
    )

    # Llamadas directas sin owner propio en el catálogo. Si tampoco tienen
    # instalaciones bajo ese owner, se consideran agregadores/aliases/scopes vacíos
    # y son informativas, no una omisión de funciones.
    main_without_catalog = sorted(
        (owner, sorted(owner_files.get(owner, ())))
        for owner in main_calls
        if owner not in catalog
    )

    direct_without_own_installs = sorted(
        (owner, sorted(owner_files.get(owner, ())))
        for owner in main_calls
        if owner not in catalog and owner not in owners_with_installs
    )

    direct_with_own_installs_missing_catalog = sorted(
        (owner, sorted(owner_files.get(owner, ())))
        for owner in main_calls
        if owner not in catalog and owner in owners_with_installs
    )

    inconsistent = (
        active_missing_catalog
        or direct_with_own_installs_missing_catalog
        or indirect_missing_catalog
        or missing_source
    )

    print('Auditoría de catálogo de hooks')
    print(f'  clases/grupos en hooks.csv: {len(catalog)}')
    print(f'  owners con RH_Scoped*Install en fuente: {len(owners_with_installs)}')
    print(f'  llamadas directas ::InjectHooks() en InjectHooksMain: {len(main_calls)}')
    print()
    print('Comprobaciones críticas:')
    print(f'  hooks activos ausentes de hooks.csv: {len(active_missing_catalog)}')
    print(f'  registros indirectos ausentes de hooks.csv: {len(indirect_missing_catalog)}')
    print(f'  clases/grupos del CSV sin scope fuente: {len(missing_source)}')
    print(f'  llamadas directas con instalaciones propias sin owner catalogado: {len(direct_with_own_installs_missing_catalog)}')
    print()
    print('Resultado: ' + ('ERROR' if inconsistent else 'OK'))

    if args.verbose:
        print_group(
            'Owners ACTIVOS con funciones instaladas pero ausentes de hooks.csv',
            active_missing_catalog,
        )
        print_group(
            'Owners registrados INDIRECTAMENTE y catalogados (no es error)',
            catalogued_indirect,
        )
        print_group(
            'Owners registrados INDIRECTAMENTE pero ausentes de hooks.csv',
            indirect_missing_catalog,
        )
        print_group(
            'Owners INACTIVOS con funciones instaladas y ausentes de hooks.csv',
            inactive_missing_catalog,
        )
        print_group(
            'Owners catalogados con instalaciones pero sin llamada Owner::InjectHooks() conocida',
            no_known_registration,
        )
        print_group(
            'Clases/grupos del CSV sin scope encontrado en fuente',
            missing_source,
        )
        print_group(
            'Llamadas directas sin owner propio y SIN instalaciones propias (informativo)',
            direct_without_own_installs,
        )
        print_group(
            'Llamadas directas con instalaciones propias pero sin owner en hooks.csv',
            direct_with_own_installs_missing_catalog,
        )

    if args.strict and inconsistent:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
