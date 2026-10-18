p = 'D:/Original/gta-reversed-master/contrib/inventory_implementations.py'
s = open(p, encoding='utf-8').read()
def rep(old, new, count=1):
    global s
    assert old in s, old
    s = s.replace(old, new, count)

rep("GAME_SA = 'source/game_sa/'\n", """GAME_SA = 'source/game_sa/'
# Bodies are scanned in game_sa and in the application/platform layer (hooks of Rs, Ps, Light... live there).
# The lexical assembly inventory (audit_cpp_assembly.py) keeps covering game_sa only; app files are classified here.
SCAN_ROOTS = ('source/game_sa', 'source/app')
# NOTSA file with real inline assembly in macros (outside any definition); its hooks are not part of hooks.csv.
SKIPPED_FILES = {'source/app/platform/win/WindowedMode.cpp'}
# Attribute macro placed between `class` and the class name; it would hide the class from the parser.
CLASS_MACROS = re.compile(rb'\bNOTSA_EXPORT_VTABLE\b')
""")
rep("def sanitized(raw):\n    clean = ", "def sanitized(raw):\n    raw = CLASS_MACROS.sub(lambda m: blank(m[0]), raw)\n    clean = ")
rep("        if not r['file'].startswith(GAME_SA):\n            continue\n        keyed", "        if r['file'] not in scanned:\n            continue\n        keyed")
rep("def link_by_registration(functions, catalog, registrations, matched):", "def link_by_registration(functions, catalog, registrations, matched, scanned):")
rep("    link_by_registration(functions, catalog, registrations, matched)\n", "    link_by_registration(functions, catalog, registrations, matched, scanned)\n")
rep("""def outside_game_sa(catalog, registrations, matched):
    \"\"\"Unlinked catalog rows whose hook is installed only from a file outside source/game_sa.\"\"\"""", """def outside_game_sa(catalog, registrations, matched):
    \"\"\"Catalog rows still without a body whose hook is installed only from a file outside source/game_sa.\"\"\"""")
rep("    paths = sorted(p for p in (ROOT / 'source/game_sa').rglob('*') if p.suffix in {'.cpp', '.h', '.hpp', '.inc'})\n",
    "    paths = sorted(p for root in SCAN_ROOTS for p in (ROOT / root).rglob('*')\n                   if p.suffix in {'.cpp', '.h', '.hpp', '.inc'} and p.relative_to(ROOT).as_posix() not in SKIPPED_FILES)\n    scanned = {p.relative_to(ROOT).as_posix() for p in paths}\n")
rep("""        if (path.suffix in {'.cpp', '.h', '.hpp', '.inc'} and not relative.startswith(GAME_SA)
                and not relative.startswith('source/reversiblehooks/')):""", """        if (path.suffix in {'.cpp', '.h', '.hpp', '.inc'} and relative not in scanned
                and not relative.startswith('source/reversiblehooks/')):""")
rep("    # Reconstructed hooks installed from files outside source/game_sa (platform, application...): not part of this measurement.\n",
    "    # Catalog rows installed from outside source/game_sa (platform, application...) that still have no identifiable body.\n")
rep("        actual = Counter((r['file'], r['line'], r['kind']) for r in occurrences)", "        actual = Counter((r['file'], r['line'], r['kind']) for r in occurrences)") if False else None
rep("    actual = Counter((r['file'], r['line'], r['kind']) for r in occurrences)", "    actual = Counter((r['file'], r['line'], r['kind']) for r in occurrences if r['file'].startswith(GAME_SA))")
rep("[Entradas cuyo hook se instala fuera de source/game_sa (plataforma, aplicación...); quedan fuera de la medición «solo C++»]", "[Entradas cuyo hook se instala fuera de source/game_sa (plataforma, aplicación...) y siguen sin cuerpo enlazado]")
rep("'scope': 'source/game_sa, definitions", "'scope': 'source/game_sa and source/app (WindowedMode.cpp excluded), definitions")
open(p, 'w', encoding='utf-8').write(s)

p = 'D:/Original/gta-reversed-master/contrib/hooks2md.py'
s = open(p, encoding='utf-8').read()
a = s.index("    # Funciones reconstruidas del catálogo cuyo cuerpo")
b = s.index("    now = datetime")
new = '''    # Funciones reconstruidas del catálogo cuyo cuerpo fuente (inventario de implementaciones) está escrito solo en C++, sin __asm ni naked.
    # Mide todo el catálogo (game_sa, plataforma y aplicación). Las entradas sin fuente identificada de forma inequívoca no se cuentan como C++.
    cpp_total = reversed_count
    cpp_only = 0
    inventory = Path(input_path).with_name('implementaciones') / 'funciones.csv'
    if inventory.exists():
        with inventory.open(encoding='utf-8-sig', newline='') as stream:
            # Una entrada del catálogo cuenta una sola vez aunque varios cuerpos (p. ej. constructor y su envoltura Constructor) compartan su dirección;
            # cuenta como C++ solo si todos los cuerpos enlazados están escritos solo en C++.
            linked = {}
            for row in csv.DictReader(stream):
                if row['reversed'] == '1':
                    key = (row['catalog_identity'], row['catalog_address'])
                    linked[key] = linked.get(key, True) and row['language'] == 'C++'
            cpp_only = sum(linked.values())
    cpp_row = ([f"| Funciones reconstruidas escritas solo en C++ (sin ensamblador) | `{'█' * round(20*cpp_only/cpp_total)}{'░' * (20-round(20*cpp_only/cpp_total))}` | {cpp_only:,}/{cpp_total:,} · {cpp_only/cpp_total:.2%} |"]
               if inventory.exists() and cpp_total else [])
'''
s = s[:a] + new + s[b:]
i = s.index('(f" La última barra mide')
j = s.index('if cpp_row else ""', i)
s = s[:i] + '(" La última barra cuenta, de las funciones reconstruidas, las que tienen fuente identificada y sin `__asm` ni `naked`; las entradas sin fuente identificada de forma inequívoca no se cuentan como C++. El detalle está en [ImplementacionesDelJuego.md](ImplementacionesDelJuego.md)." ' + s[j:]
open(p, 'w', encoding='utf-8').write(s)
