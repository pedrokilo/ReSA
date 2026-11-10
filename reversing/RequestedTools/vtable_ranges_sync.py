#!/usr/bin/env python3
"""vtable_ranges_sync.py [--apply] [--prefix CEvent] [Banco ...]
Compara el VTableRanges.hpp de cada banco con los RH_ScopedVirtualClass(Clase, 0xDIR, N) de source/game_sa
y (con --apply) anade las clases ausentes (por defecto cualquier clase; --prefix limita) en orden alfabetico,
manteniendo el formato {"Clase",{0xDIRECCION,N}},. Informa de clases con direccion/ranuras distintas."""
import re, sys, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]
rx = re.compile(r'RH_ScopedVirtualClass\(\s*([^,()]+?)\s*,\s*(0x[0-9A-Fa-f]+)\s*,\s*(\d+)\s*\)')
def source_classes():
    d = {}
    for p in (ROOT/'source').rglob('*'):
        if p.suffix not in ('.cpp', '.h', '.hpp'): continue
        try: t = p.read_text(encoding='utf-8', errors='replace')
        except OSError: continue
        for m in rx.finditer(t):
            d[m.group(1)] = (int(m.group(2),16), int(m.group(3)))
    return d
row = re.compile(r'^\{"([^"]+)",\{0x([0-9A-Fa-f]+),(\d+)\}\},\s*$')
def main():
    args = sys.argv[1:]; apply = '--apply' in args
    prefix = ''
    if '--prefix' in args: prefix = args[args.index('--prefix')+1]
    names = [a for a in args if not a.startswith('--') and a != prefix]
    src = source_classes()
    files = sorted(ROOT.glob('reversing/*/VTableRanges.hpp'))
    for f in files:
        bank = f.parent.name
        if names and bank not in names: continue
        lines = f.read_text(encoding='utf-8').split('\n')
        eol_crlf = '\r\n' in f.read_text(encoding='utf-8')
        rows = {}; first = last = None
        for i, l in enumerate(lines):
            m = row.match(l.rstrip('\r'))
            if m:
                rows[m.group(1)] = (int(m.group(2),16), int(m.group(3)))
                first = i if first is None else first; last = i
        missing = {k: v for k, v in src.items() if k not in rows and k.startswith(prefix)}
        diff = {k: (rows[k], src[k]) for k in rows if k in src and rows[k] != src[k]}
        print(f'{bank}: filas={len(rows)} ausentes={len(missing)} distintas={len(diff)}')
        if diff:
            for k, (a, b) in diff.items(): print(f'   DISTINTA {k} banco={a[0]:#x},{a[1]} fuente={b[0]:#x},{b[1]}')
        if apply and missing:
            rows.update(missing)
            body = ['{"%s",{0x%08X,%d}},' % (k, v[0], v[1]) for k, v in sorted(rows.items())]
            new = lines[:first] + body + lines[last+1:]
            txt = '\n'.join(new)
            f.write_text(txt, encoding='utf-8', newline='')
main()
