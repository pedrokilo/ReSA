p='contrib/inventory_implementations.py'
s=open(p,encoding='utf-8').read()
def rep(old,new,cnt=1):
    global s
    assert s.count(old)==cnt,(old,s.count(old))
    s=s.replace(old,new)
# totals restricted to game_sa + exception keys
rep("""    totals = {'source_definition_count': len(functions), 'groups': len(groups),
        'assembly_occurrences': sum(r['asm_occurrences'] for r in functions),
        'naked_declarations': sum(r['naked_declarations'] for r in functions),
        'definitions_with_assembly': sum(bool(r['asm_occurrences']) for r in functions),
        'definitions_with_naked': sum(bool(r['naked_declarations']) for r in functions),
        'mixed_definitions': sum(r['language'] == 'Mixta (C++ + ASM)' for r in functions),
        'asm_only_definitions': sum(r['language'] == 'Ensamblador' for r in functions),
""","""    # Assembly counters keep their historical scope (source/game_sa, the lexical audit). Assembly outside game_sa is the documented
    # NOTSA exception (project-own code, not a game function): it is counted separately and listed, never mixed in nor hidden.
    in_game = [r for r in functions if r['file'].startswith(GAME_SA)]
    exceptions = notsa_asm_exceptions(functions)
    with (out / 'catalogo-excepciones-notsa-asm.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.writer(stream)
        writer.writerow(['file', 'line', 'function', 'language', 'asm_occurrences', 'naked_declarations', 'catalog', 'catalog_identity', 'catalog_address', 'counts_as_cpp_only'])
        for r in exceptions:
            writer.writerow([r['file'], r['line'], r['function'], r['language'], r['asm_occurrences'], r['naked_declarations'],
                             r['catalog'], r['catalog_identity'], r['catalog_address'], 'no'])
    totals = {'source_definition_count': len(functions), 'groups': len(groups),
        'assembly_occurrences': sum(r['asm_occurrences'] for r in in_game),
        'naked_declarations': sum(r['naked_declarations'] for r in in_game),
        'definitions_with_assembly': sum(bool(r['asm_occurrences']) for r in in_game),
        'definitions_with_naked': sum(bool(r['naked_declarations']) for r in in_game),
        'mixed_definitions': sum(r['language'] == 'Mixta (C++ + ASM)' for r in in_game),
        'asm_only_definitions': sum(r['language'] == 'Ensamblador' for r in in_game),
        'notsa_asm_exception_definitions': len(exceptions),
        'notsa_asm_exception_assembly_occurrences': sum(r['asm_occurrences'] for r in exceptions),
        'notsa_asm_exception_naked_declarations': sum(r['naked_declarations'] for r in exceptions),
        'notsa_asm_exception_catalog_rows': len({(r['catalog_identity'], r['catalog_address']) for r in exceptions if r['catalog_identity']}),
        'notsa_asm_exception_files': sorted({r['file'] for r in exceptions}),
""")
rep("'asm_bodies_with_parse_errors': sum(bool(r['asm_occurrences']) and bool(r['parser_error']) for r in functions),",
    "'asm_bodies_with_parse_errors': sum(bool(r['asm_occurrences']) and bool(r['parser_error']) for r in in_game),")
rep("""def collect():""","""def notsa_asm_exceptions(functions):
    \"\"\"Definitions with assembly outside source/game_sa: the documented NOTSA exception (own project code, not a game function).\"\"\"
    return sorted((r for r in functions if not r['file'].startswith(GAME_SA) and (r['asm_occurrences'] or r['naked_declarations'])),
                  key=lambda r: (r['file'], r['line']))


def collect():""")
rep("""        f"- {len(unmatched)} entradas de ambos""","""        f"- **Excepción NOTSA con ensamblador fuera de source/game_sa:** {totals['notsa_asm_exception_definitions']} definición con {totals['notsa_asm_exception_assembly_occurrences']} apariciones `__asm` (contadas por expansión de macro) y {totals['notsa_asm_exception_naked_declarations']} `naked` en {', '.join(totals['notsa_asm_exception_files']) or 'ningún archivo'}: código propio del proyecto (modo ventana), no una función del juego, y su hook figura solo en hooks_notsa.csv. Se mantiene como excepción documentada, no se convierte a C++ y **no cuenta como «solo C++»**. Los contadores de ensamblador de arriba cubren source/game_sa; esta excepción se lista aparte en [el listado de excepciones NOTSA](implementaciones/catalogo-excepciones-notsa-asm.csv) y sus apariciones constan en apariciones.csv.", '',
        f"- {len(unmatched)} entradas de ambos""")
rep("""        '- [Entradas servidas por el cuerpo""","""        '- [Excepciones NOTSA con ensamblador fuera de source/game_sa (visibles y distinguidas; no cuentan como solo C++)](implementaciones/catalogo-excepciones-notsa-asm.csv).',
        '- [Entradas servidas por el cuerpo""")
rep("(WindowedMode.cpp included: NOTSA exception with __asm, see notsa_asm_exceptions)","(WindowedMode.cpp included: NOTSA exception with __asm, reported separately)",0) if False else None
open(p,'w',encoding='utf-8').write(s)
