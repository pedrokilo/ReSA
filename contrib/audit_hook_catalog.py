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

# Cabecera de función que puede contener los RH_Scoped* de un owner
# (Owner::InjectHooks, InjectXxxHooks libres, lambdas no incluidas).
FUNC_HEAD = re.compile(
    r'^[ \t]*(?:static\s+|inline\s+|template\s*<[^>]*>\s*)*'
    r'(?:void|auto|bool)\s+([A-Za-z_][\w:<>]*)\s*\([^;{)]*\)\s*(?:const\s*)?\{',
    re.MULTILINE,
)

# Llamada sin argumentos `Nombre();` o `A::B();`, sin `.`/`->`/`::` delante.
CALL_NO_ARGS = re.compile(r'(?<![\w.>:])([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)\s*\(\s*\)\s*;')

# Cabecera de clase/struct/namespace con cuerpo; el cuerpo se delimita después
# contando llaves. Se omiten declaraciones adelantadas (`class X;`).
TYPE_HEAD = re.compile(
    r'\b(class|struct|namespace)\s+(?:NOTSA_EXPORT_VTABLE\s+|alignas\s*\([^)]*\)\s+)*'
    r'([A-Za-z_]\w*)\s*(?:final\s*)?(?::[^;{]*)?\{'
)

OWN_INJECT = re.compile(r'\bInjectHooks\s*\(')


def strip_comments(text: str) -> str:
    return COMMENT.sub('', text)


def notsa_path(path: Path) -> Path:
    """Catálogo de extensiones o adaptaciones NOTSA, fuera de los contadores de progreso."""
    return path.with_name('hooks_notsa.csv')


def read_rows(path: Path) -> list[dict]:
    rows = []
    for item in (path, notsa_path(path)):
        if item.exists():
            with item.open(encoding='utf-8-sig', newline='') as stream:
                rows += list(csv.DictReader(stream))
    return rows


def read_catalog(path: Path) -> set[str]:
    rows = read_rows(path)
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


def find_never_registered(root: Path, owners: set[str]) -> list[tuple[str, list[str]]]:
    """Owners cuya función de registro (la que contiene su RH_Scoped*) no se llama nunca.

    Un owner catalogado puede registrarse de forma indirecta (CText::InjectHooks
    registra CData/CKeyArray; funciones libres como InjectCommonHooks registran
    namespaces). Lo que importa es si la función que contiene su scope se invoca
    desde algún sitio; si no, sus hooks no se instalan nunca en tiempo de
    ejecución aunque hooks.csv los marque reversed=1.
    """
    texts = {}
    for path in sorted((root / 'source').rglob('*')):
        if path.suffix in ('.cpp', '.h', '.hpp'):
            texts[path] = strip_comments(path.read_text(encoding='utf-8-sig', errors='replace'))
    called_names = set()
    for data in texts.values():
        called_names.update(CALL_NO_ARGS.findall(data))

    enclosing = defaultdict(set)
    for path, data in texts.items():
        heads = list(FUNC_HEAD.finditer(data))
        for scope in SCOPE.finditer(data):
            owner = scope.group(1).strip()
            if owner not in owners:
                continue
            before = [h for h in heads if h.start() < scope.start()]
            if before:
                enclosing[owner].add((before[-1].group(1), path.relative_to(root).as_posix()))

    result = []
    for owner, funcs in enclosing.items():
        if not any(func in called_names for func, _ in funcs):
            result.append((owner, sorted(f'{path} ({func})' for func, path in funcs)))
    return sorted(result)


def find_inherited_calls(root: Path, main_calls: set[str]) -> list[tuple[str, list[str]]]:
    """Llamadas X::InjectHooks() cuyo X no declara InjectHooks propio.

    En C++ la llamada compila igualmente y resuelve al InjectHooks heredado de
    una base, que entonces se ejecuta dos veces (assert por nombres duplicados)
    o registra los hooks bajo el owner equivocado. Antes de este control se
    detectaba solo al arrancar; ejemplo histórico: CAEPedWeaponAudioEntity
    llamaba al InjectHooks de CAEWeaponAudioEntity.
    """
    own = set()
    bases = {}
    for path in sorted((root / 'source').rglob('*')):
        if path.suffix not in ('.cpp', '.h', '.hpp'):
            continue
        data = strip_comments(path.read_text(encoding='utf-8-sig', errors='replace'))
        for name in INJECT_DEF.findall(data):
            own.add(name.split('::')[-1].split('<')[0])
        for head in TYPE_HEAD.finditer(data):
            name = head.group(2)
            depth, end = 1, head.end()
            while depth and end < len(data):
                char = data[end]
                depth += char == '{'
                depth -= char == '}'
                end += 1
            if OWN_INJECT.search(data, head.end(), end):
                own.add(name)
            inherit = re.search(r':\s*(?:public|protected|private)?\s*([A-Za-z_]\w*)', head.group(0))
            if inherit and head.group(1) != 'namespace':
                bases.setdefault(name, inherit.group(1))

    result = []
    for call in sorted(main_calls):
        name = call.split('::')[-1].split('<')[0]
        # Solo clases con base conocida: sin base, una llamada sin InjectHooks
        # propio no compilaría, así que no es este caso.
        if name in own or name not in bases:
            continue
        base, chain = bases[name], []
        while base and base not in own and base not in chain:
            chain.append(base)
            base = bases.get(base)
        if base in own:
            result.append((call, [f'resuelve a {base}::InjectHooks heredado']))
    return result


def compare_runtime(catalog_path: Path, runtime_path: Path) -> list[tuple[str, list[str]]]:
    """Compara docs/hooks.csv con el runtime-hooks.csv que escribe el juego al arrancar.

    El registro real puede diferir del catálogo aunque las demás comprobaciones pasen:
    hooks registrados por funciones que no se llaman X::InjectHooks() (p. ej.
    notsa::InjectWindowedModeHooks), un reversed distinto en el código y en el CSV, o
    una dirección cambiada. Las filas V con dirección 0 en el CSV no se comparan por
    dirección.
    """
    def load(path):
        with path.open(encoding='utf-8-sig', newline='') as stream:
            return {(r['class'], r['fn_name']): r for r in csv.DictReader(stream)}
    catalog, runtime = {(r['class'], r['fn_name']): r for r in read_rows(catalog_path)}, load(runtime_path)
    result = []
    for key in sorted(set(runtime) - set(catalog)):
        result.append(('/'.join(key), ['registrado en el juego, ausente de hooks.csv']))
    for key in sorted(set(catalog) - set(runtime)):
        result.append(('/'.join(key), ['en hooks.csv, no registrado en el juego']))
    for key in sorted(set(catalog) & set(runtime)):
        c, r = catalog[key], runtime[key]
        if c['reversed'] != r['reversed']:
            result.append(('/'.join(key), [f"reversed: hooks.csv={c['reversed']}, juego={r['reversed']}"]))
        if int(c['address'], 16) and c['address'].upper() != r['address'].upper():
            result.append(('/'.join(key), [f"dirección: hooks.csv={c['address']}, juego={r['address']}"]))
    return result


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
    parser.add_argument(
        '--runtime',
        default=None,
        help='runtime-hooks.csv generado por el juego (scripts/runtime-hooks.csv); compara el registro real con el catálogo',
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

    never_registered = find_never_registered(
        root, {owner for owner in owners_with_installs if owner in catalog}
    )

    inherited_calls = find_inherited_calls(root, main_calls)
    runtime_mismatches = compare_runtime(root / args.csv, Path(args.runtime)) if args.runtime else []

    inconsistent = (
        active_missing_catalog
        or direct_with_own_installs_missing_catalog
        or indirect_missing_catalog
        or missing_source
        or never_registered
        or inherited_calls
        or runtime_mismatches
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
    print(f'  owners catalogados cuya función de registro nunca se invoca: {len(never_registered)}')
    print(f'  llamadas que resuelven a un InjectHooks heredado: {len(inherited_calls)}')
    if args.runtime:
        print(f'  diferencias con el registro real (runtime-hooks.csv): {len(runtime_mismatches)}')
    print()
    print('Resultado: ' + ('ERROR' if inconsistent else 'OK'))
    if never_registered:
        print_group('Owners catalogados que NUNCA instalan sus hooks (su registro no se llama)', never_registered)
    if inherited_calls:
        print_group('Llamadas X::InjectHooks() sin InjectHooks propio en X (usan el heredado)', inherited_calls)
    if runtime_mismatches:
        print_group('Diferencias entre hooks.csv y el registro real del juego', runtime_mismatches)

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
