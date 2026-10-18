"""Genera docs/implementaciones/envolturas-pendientes.csv: funciones del juego que siguen delegando en el EXE.

Une cuatro fuentes, sin inflar ni ocultar:
  1. docs/implementaciones/funciones.csv: definiciones del código fuente cuya implementación es «Envoltura al EXE».
  2. reversing/OriginalDependencies499/progress.csv: funciones del juego de la cohorte de 499 que siguen sin aprobar.
  3. reversing/OriginalDependencies2026-10-17/classification.csv: funciones del juego del cohorte nuevo.
  4. reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv: bibliotecas (RenderWare, controlador D3D9, CRT, QuickTime) del cohorte nuevo,
     agrupadas como «(biblioteca) <subcategoría>».
Una dirección que ya figure en docs/hooks.csv con reversed=1 se descarta (ya está reconstruida). Todo lo que queda cuenta como función por
reconstruir en los porcentajes de docs/ReversedClasses.md (funciones y clases).

Uso: python contrib/pending_wrappers.py
"""
import csv
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'docs/implementaciones/envolturas-pendientes.csv'
HELPERS = '(helpers y globales sin clase)'


def read(path):
    with path.open(encoding='utf-8-sig', newline='') as stream:
        return list(csv.DictReader(stream))


def norm_addr(value):
    value = (value or '').strip()
    if not re.fullmatch(r'0x[0-9a-fA-F]+', value):
        return ''
    return f'0x{int(value, 16):08X}'


def split_identity(identity):
    """Devuelve (clase, función); sin «Clase::» la clase es el cajón de helpers."""
    identity = re.sub(r'\s*\(.*$', '', identity.strip())  # descripciones entre paréntesis
    if '::' in identity and not identity.startswith(('helper', 'CPool<', 'CPool ')):
        cls, _, fn = identity.rpartition('::')
        return cls.strip(), fn.strip()
    return HELPERS, identity


def main():
    done = set()
    done_names = set()
    for row in read(ROOT / 'docs/hooks.csv'):
        if row['reversed'] == '1':
            done_names.add((row['class'].casefold(), row['fn_name'].casefold()))
            if norm_addr(row['address']) and int(row['address'], 16):
                done.add(norm_addr(row['address']))
    found = {}
    names = {}

    def add(cls, fn, address, source, origin):
        address = norm_addr(address)
        fn = fn.split('::')[-1].strip() if cls != HELPERS else fn.strip()  # «Clase::Función» y «Función» son la misma función
        if (address and address in done) or (cls.casefold(), fn.casefold()) in done_names:
            return
        # Una misma función puede venir del código fuente (sin dirección) y de una cohorte (con dirección): se funden por clase y nombre.
        name_key = (cls.casefold(), fn.casefold())
        key = names.get(name_key) or address or f'{cls}::{fn}'
        if address and key != address and key in found:
            found[address] = found.pop(key)
            key = address
        names[name_key] = key
        item = found.setdefault(key, dict(class_=cls, function=fn, address=address, source=source, origins=set()))
        item['origins'].add(origin)
        if not item['address'] and address:
            item['address'] = address

    for row in read(ROOT / 'docs/implementaciones/funciones.csv'):
        if not row['implementation'].startswith('Envoltura'):
            continue
        owner = row['owner']
        cls = HELPERS if owner.startswith('Globales y helpers') else owner
        add(cls, row['function'], row.get('catalog_address', ''), f"{row['file']}:{row['line']}", 'envoltura en el código fuente')
    for row in read(ROOT / 'reversing/OriginalDependencies499/progress.csv'):
        if row['category'] == 'game' and row['runtime_status'] != 'passed':
            cls, fn = split_identity(row['identity'])
            add(cls, fn, row['address'], row['source'], 'cohorte 499')
    for row in read(ROOT / 'reversing/OriginalDependencies2026-10-17/classification.csv'):
        if row['category'] == 'game':
            cls, fn = split_identity(row['identity'])
            add(cls, fn, row['address'], row['source'], 'cohorte nuevo 2026-10-17')

    for row in read(ROOT / 'reversing/OriginalDependencies2026-10-17/lista-por-dificultad.csv'):
        if row['categoria'] == 'biblioteca':
            identity = re.sub(r'\s*\(.*$', '', row['identity'].strip())
            add('(biblioteca) ' + row['subcategoria'], identity, row['address'], row['fuente'], 'cohorte nuevo 2026-10-17 (biblioteca)')

    rows = sorted(found.values(), key=lambda r: (r['class_'].casefold(), r['function'].casefold(), r['address']))
    with OUT.open('w', encoding='utf-8', newline='') as stream:
        writer = csv.writer(stream)
        writer.writerow(['class', 'function', 'address', 'source', 'origins'])
        for r in rows:
            writer.writerow([r['class_'], r['function'], r['address'], r['source'], ' + '.join(sorted(r['origins']))])
    classes = {r['class_'] for r in rows}
    print(f'{len(rows)} funciones con envoltura pendiente en {len(classes)} clases/grupos -> {OUT.relative_to(ROOT)}')


if __name__ == '__main__':
    main()
