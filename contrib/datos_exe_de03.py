"""Lote DE03: cadenas de .rdata que el C++ lee por direccion fija (reinterpret_cast<const char*>(0x..)) como cadenas propias
(source/game_sa/DatosExe/RdataCadenas.h/.cpp). Los textos salen del EXE canonico (NUL-terminados, ASCII imprimible).

Solo se reescribe la forma `reinterpret_cast<const char*|const RwChar*>(0xDIR)`; el resto (GaKey, kExePedEventFile, ayudantes
con direccion, GUID) se hace a mano en el mismo lote y se anota en docs/DatosExe/lotes/DE03.csv.
Uso: python contrib/datos_exe_de03.py [--aplicar]
"""
import csv
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
from datos_exe_lote import EXCLUIR_ARCHIVOS, RAIZ  # noqa: E402
from pe_inventory import PE  # noqa: E402

NL = chr(10)
RE = re.compile(r'reinterpret_cast\s*<\s*const\s+(?:char|RwChar)\s*\*\s*>\s*\(\s*0[xX]([0-9A-Fa-f]{6,8})\s*\)')


def cadena(pe, va):
    off = pe.offset(va, 1)
    m = re.match(rb'[\x20-\x7e]{1,200}\x00', pe.data[off:off + 201])
    return m.group()[:-1].decode('ascii') if m else None


def cpp_lit(s):
    return '"' + s.replace(chr(92), chr(92) * 2).replace('"', chr(92) + '"') + '"'


def main():
    aplicar = '--aplicar' in sys.argv
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    cand = []
    for p in sorted((RAIZ / 'source').rglob('*')):
        if p.suffix not in ('.cpp', '.h', '.hpp', '.inc'):
            continue
        rel = p.relative_to(RAIZ).as_posix()
        if any(x in rel for x in EXCLUIR_ARCHIVOS) or 'DatosExe/' in rel:
            continue
        txt = p.read_bytes().decode('utf-8', errors='surrogateescape')
        if 'reinterpret_cast' not in txt:
            continue
        blank = de.blanquear_comentarios(txt)
        for m in RE.finditer(blank):
            va = int(m.group(1), 16)
            if 0x858378 <= va < 0x8A4000 and cadena(pe, va) is not None:
                cand.append((rel, m.start(), m.end(), va))
    objs = {}
    for _, _, _, va in cand:
        objs[va] = f'kS{va:X}'
    print(f'DE03 cadenas: {len(cand)} referencias, {len(objs)} cadenas, {len({c[0] for c in cand})} archivos')
    out = RAIZ / 'docs/DatosExe/lotes'
    out.mkdir(parents=True, exist_ok=True)
    cnt = defaultdict(int)
    for c in cand:
        cnt[c[3]] += 1
    with (out / 'DE03.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'tipo', 'valor', 'refs', 'nombre'])
        for va, nom in sorted(objs.items()):
            w.writerow([f'0x{va:08X}', 'cadena', cadena(pe, va), cnt[va], nom])
    if not aplicar:
        return
    cab = ['#pragma once', '', '/*', ' * Lote DE03 (docs/DatosExe): cadenas de .rdata del EXE como definiciones propias (texto extraido del EXE).',
           ' * Generado por contrib/datos_exe_de03.py; definidas en RdataCadenas.cpp.', ' */', '', 'namespace DatosExe::Rdata {']
    cpp = ['#include "StdInc.h"', '', '#include "RdataCadenas.h"', '', 'namespace DatosExe::Rdata {']
    for va, nom in sorted(objs.items()):
        t = cadena(pe, va)
        cab.append(f'extern const char {nom}[]; // 0x{va:08X} = {cpp_lit(t)}')
        cpp.append(f'const char {nom}[] = {cpp_lit(t)}; // 0x{va:08X}')
    cab += ['', '} // namespace DatosExe::Rdata', '']
    cpp += ['', '} // namespace DatosExe::Rdata', '']
    d = RAIZ / 'source/game_sa/DatosExe'
    (d / 'RdataCadenas.h').write_text(NL.join(cab), encoding='utf-8', newline=NL)
    (d / 'RdataCadenas.cpp').write_text(NL.join(cpp), encoding='utf-8', newline=NL)
    por_f = defaultdict(list)
    for c in cand:
        por_f[c[0]].append(c)
    inc = '#include "DatosExe/RdataCadenas.h"'
    for arch, cs in sorted(por_f.items()):
        p = RAIZ / arch
        raw = p.read_bytes()
        txt = raw.decode('utf-8', errors='surrogateescape')
        piezas, pos = [], 0
        for _, ini, fin, va in cs:
            piezas += [txt[pos:ini], f'DatosExe::Rdata::{objs[va]}']
            pos = fin
        piezas.append(txt[pos:])
        nuevo = ''.join(piezas)
        nl = '\r\n' if b'\r\n' in raw else NL
        if inc not in nuevo:
            if arch.endswith('.cpp'):
                mm = re.search(r'#include\s+"StdInc.h"[^' + NL + r']*' + NL, nuevo)
            else:
                mm = re.search(r'#pragma once[^' + NL + r']*' + NL, nuevo)
            if not mm:
                raise SystemExit('sin punto de insercion en ' + arch)
            nuevo = nuevo[:mm.end()] + inc + nl + nuevo[mm.end():]
        p.write_bytes(nuevo.encode('utf-8', errors='surrogateescape'))
        print(arch, len(cs))


if __name__ == '__main__':
    main()
