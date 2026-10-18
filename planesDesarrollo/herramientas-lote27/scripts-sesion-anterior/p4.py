p = 'D:/Original/gta-reversed-master/contrib/inventory_implementations.py'
s = open(p, encoding='utf-8').read()


def rep(old, new):
    global s
    assert old in s, old
    s = s.replace(old, new, 1)


rep("    return functions, occurrences, catalog, source_hashes, file_errors, outside\n",
    "    shared = [(c, f, via) for f in functions for c, via in f.get('shared', [])]\n"
    "    return functions, occurrences, catalog, source_hashes, file_errors, outside, shared\n")
rep("def write(functions, occurrences, catalog, hashes, errors, outside):",
    "def write(functions, occurrences, catalog, hashes, errors, outside, shared=()):")
rep("    functions, occurrences, catalog, hashes, errors, outside = collect()", "    functions, occurrences, catalog, hashes, errors, outside, shared = collect()")
rep("write(functions, occurrences, catalog, hashes, errors, outside)", "write(functions, occurrences, catalog, hashes, errors, outside, shared)")
rep("    matched = {(r['catalog_identity'], r['catalog_address']) for r in functions}\n",
    "    matched = {(r['catalog_identity'], r['catalog_address']) for r in functions}\n"
    "    matched |= {(c['class'] + '::' + c['fn_name'], canonical(c['address'])) for c, _, _ in shared}\n")
rep("    # Catalog rows installed from outside source/game_sa",
    """    # Entries served by a template body that is already the link of another entry (one per instantiation / derived class).
    with (out / 'catalogo-cuerpo-compartido.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.writer(stream)
        writer.writerow(['catalog_identity', 'catalog_address', 'catalog', 'reversed', 'validated', 'file', 'line', 'language', 'via'])
        for c, f, via in shared:
            writer.writerow([c['class'] + '::' + c['fn_name'], canonical(c['address']), c['catalog'], c['reversed'], c['validated'],
                             f['file'], f['line'], f['language'], via])
    # Catalog rows installed from outside source/game_sa""")
rep("        'catalog_rows_installed_outside_game_sa': len(outside),\n",
    "        'catalog_rows_installed_outside_game_sa': len(outside),\n        'catalog_rows_sharing_a_template_body': len(shared),\n")
rep("        '- [Entradas cuyo hook se instala fuera de source/game_sa",
    "        '- [Entradas servidas por el cuerpo genérico de una plantilla ya enlazado a otra instanciación o clase derivada](implementaciones/catalogo-cuerpo-compartido.csv).',\n        '- [Entradas cuyo hook se instala fuera de source/game_sa")
open(p, 'w', encoding='utf-8').write(s)

p = 'D:/Original/gta-reversed-master/contrib/hooks2md.py'
s = open(p, encoding='utf-8').read()
rep("            cpp_only = sum(linked.values())\n",
    """        shared_file = Path(input_path).with_name('implementaciones') / 'catalogo-cuerpo-compartido.csv'
        if shared_file.exists():  # entradas servidas por el cuerpo genérico de una plantilla (instanciaciones, clases derivadas)
            with shared_file.open(encoding='utf-8-sig', newline='') as stream:
                for row in csv.DictReader(stream):
                    if row['reversed'] == '1' and row['catalog'] == Path(input_path).name:
                        key = (row['catalog_identity'], row['catalog_address'])
                        linked[key] = linked.get(key, True) and row['language'] == 'C++'
            cpp_only = sum(linked.values())
""")
open(p, 'w', encoding='utf-8').write(s)
