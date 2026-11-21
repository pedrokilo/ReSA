"""Lote DE04: variables de .data inicializado del juego (escalares y arrays de tipos primitivos) como definiciones propias
(source/game_sa/DatosExe/DataInicializada.h/.cpp, namespace DatosExe::Data) con el valor inicial del EXE.

Solo se convierte un objeto cuando es seguro con las cotas de la medicion (se re-mide la fuente actual en cada ejecucion):
 - todas las referencias de la DLL a esa direccion son `StaticRef<T>(0x..)` con el mismo T (si no, quedarian dos copias);
 - ninguna otra direccion referenciada (DLL o EXE) cae dentro de su extension (sin aritmetica de punteros ni vecinos);
 - ninguna funcion viva del EXE la lee, la escribe ni le toma la direccion;
 - T es primitivo (float, double, int8..int32, uint8..uint32, bool) o std::array<prim, N> / prim[N], y vive en la parte
   inicializada del .data (0x8A4000-0x8E4000).
Uso: python contrib/datos_exe_de04.py <GRUPO> [--aplicar]   GRUPO: A (float), B (otros escalares), C (arrays), T (todo)
"""
import bisect
import csv
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
from datos_exe_lote import RAIZ  # noqa: E402
from pe_inventory import PE  # noqa: E402

NL = chr(10)
PRIMS = {'float': 4, 'double': 8, 'int32': 4, 'uint32': 4, 'int16': 2, 'uint16': 2, 'int8': 1, 'uint8': 1, 'bool': 1}
STRUCT = {'float': 'f', 'double': 'd', 'int32': 'i', 'uint32': 'I', 'int16': 'h', 'uint16': 'H', 'int8': 'b', 'uint8': 'B', 'bool': 'B'}
EXCL = ('/RenderWare/', '/Crt', '/crt', 'ExeProtectionStub', 'DatosExe/')


def parsear(t):
    """(elemento, n_o_None) si T es primitivo, std::array<prim,N> o prim[N]; si no None."""
    t = re.sub(r'\s+', '', t)
    if t in PRIMS:
        return t, None
    m = re.fullmatch(r'std::array<(\w+),(\d+)>', t) or re.fullmatch(r'(\w+)\[(\d+)\]', t)
    if m and m.group(1) in PRIMS:
        return m.group(1), int(m.group(2))
    return None


def literal_elem(t, v):
    import struct as st
    if t == 'float':
        return 'std::bit_cast<float>(0x%08Xu)' % st.unpack('<I', st.pack('<f', v))[0]
    if t == 'double':
        return 'std::bit_cast<double>(0x%016Xull)' % st.unpack('<Q', st.pack('<d', v))[0]
    if t == 'bool':
        return 'true' if v else 'false'
    if t in ('uint32', 'uint16', 'uint8'):
        return '0x%X' % v if v > 9 else str(v)
    return str(v)


def main():
    import struct as st
    grupo = sys.argv[1]
    aplicar = '--aplicar' in sys.argv
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    refs = de.escanear_fuente('source')
    reempl, rangos = de.cargar_funciones()
    cod = de.escanear_exe(pe, reempl, rangos)
    por_va = defaultdict(list)
    for r in refs:
        por_va[r['va']].append(r)
    todas = sorted(set(por_va) | set(cod))
    sel = []
    for va in sorted(por_va):
        if not (0x8A4000 <= va < 0x8E4000):
            continue
        rs = por_va[va]
        if any(any(x in r['archivo'] for x in EXCL) for r in rs):
            continue
        if {r['forma'] for r in rs} != {'StaticRef'}:
            continue
        ts = {re.sub(r'\s+', '', r['tipo']) for r in rs}
        if len(ts) != 1:
            continue
        pt = parsear(next(iter(ts)))
        if not pt:
            continue
        el, n = pt
        tam = PRIMS[el] * (n or 1)
        i = bisect.bisect_right(todas, va)
        if i < len(todas) and todas[i] < va + tam:
            continue
        # alguna direccion anterior cuya extension llegue a va: la cubre la comprobacion simetrica de ese vecino
        c = cod.get(va)
        if c and (any(v for v in c['funcs'].values()) or c['escribe'] and any(v for v in c['funcs'].values()) or c['direccion'] and any(v for v in c['funcs'].values())):
            continue
        if va + tam > 0x8E4000:
            continue
        sel.append((va, el, n, next(iter(ts)), rs))
    # un vecino anterior con extension que llegue a este objeto tampoco vale
    sel_ok = []
    for va, el, n, t, rs in sel:
        i = bisect.bisect_left(todas, va)
        if i > 0:
            prev = todas[i - 1]
            for q in sel:
                if q[0] == prev and prev + PRIMS[q[1]] * (q[2] or 1) > va:
                    break
            else:
                sel_ok.append((va, el, n, t, rs))
        else:
            sel_ok.append((va, el, n, t, rs))
    sel = sel_ok
    if grupo == 'A':
        sel = [s for s in sel if s[2] is None and s[1] == 'float']
    elif grupo == 'B':
        sel = [s for s in sel if s[2] is None and s[1] != 'float']
    elif grupo == 'C':
        sel = [s for s in sel if s[2] is not None]
    nref = sum(len(s[4]) for s in sel)
    print(f'DE04-{grupo}: {len(sel)} objetos, {nref} referencias, {sum(PRIMS[s[1]] * (s[2] or 1) for s in sel)} bytes')
    out = RAIZ / 'docs/DatosExe/lotes'
    out.mkdir(parents=True, exist_ok=True)
    with (out / f'DE04{grupo}.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'tipo', 'bytes', 'refs', 'archivos', 'nombre'])
        for va, el, n, t, rs in sel:
            w.writerow([f'0x{va:08X}', t, PRIMS[el] * (n or 1), len(rs), ';'.join(sorted({Path(r['archivo']).name for r in rs}))[:100], f'v{va:X}'])
    if not aplicar:
        return
    nom = 'DataInicializada' + grupo
    cab = ['#pragma once', '', '/*', f' * Lote DE04-{grupo} (docs/DatosExe): variables de .data inicializado del juego como definiciones propias con el',
           ' * valor inicial del EXE canonico (gta-sa.exe 1.0 US). Estado vivo: las lee y escribe solo C++.',
           f' * Generado por contrib/datos_exe_de04.py; definidas en {nom}.cpp.', ' */', '', 'namespace DatosExe::Data {']
    cpp = ['#include "StdInc.h"', '', f'#include "{nom}.h"', '', '#include <bit>', '', 'namespace DatosExe::Data {']
    for va, el, n, t, rs in sel:
        off = pe.offset(va, PRIMS[el] * (n or 1))
        raw = pe.data[off:off + PRIMS[el] * (n or 1)]
        vals = [st.unpack_from('<' + STRUCT[el], raw, k * PRIMS[el])[0] for k in range((n or 1))]
        tt = re.sub(r'\s+', ' ', t)
        arr = re.fullmatch(r'(\w+)\[(\d+)\]', tt.replace(' ', ''))
        if n is None:
            cab.append(f'extern {el if el != "int8" else "int8"} v{va:X}; // 0x{va:08X}')
            cpp.append(f'{el} v{va:X} = {literal_elem(el, vals[0])}; // 0x{va:08X}')
        elif arr:
            cab.append(f'extern {el} v{va:X}[{n}]; // 0x{va:08X}')
            cpp.append(f'{el} v{va:X}[{n}] = {{{", ".join(literal_elem(el, v) for v in vals)}}}; // 0x{va:08X}')
        else:
            cab.append(f'extern std::array<{el}, {n}> v{va:X}; // 0x{va:08X}')
            cpp.append(f'std::array<{el}, {n}> v{va:X} = {{{{{", ".join(literal_elem(el, v) for v in vals)}}}}}; // 0x{va:08X}')
    cab += ['', '} // namespace DatosExe::Data', '']
    cpp += ['', '} // namespace DatosExe::Data', '']
    d = RAIZ / 'source/game_sa/DatosExe'
    (d / (nom + '.h')).write_text(NL.join(cab), encoding='utf-8', newline=NL)
    (d / (nom + '.cpp')).write_text(NL.join(cpp), encoding='utf-8', newline=NL)
    por_f = defaultdict(list)
    for va, el, n, t, rs in sel:
        for r in rs:
            por_f[r['archivo']].append((va, t))
    inc = f'#include "DatosExe/{nom}.h"'
    for arch, lst in sorted(por_f.items()):
        p = RAIZ / arch
        raw = p.read_bytes()
        txt = raw.decode('utf-8', errors='surrogateescape')
        bl = de.blanquear_comentarios(txt)
        wanted = {va: t for va, t in lst}
        patron = re.compile(r'StaticRef\s*<\s*(.+?)\s*>\s*\(\s*0[xX]([0-9A-Fa-f]{6,8})\s*\)')
        piezas, pos, cnt = [], 0, 0
        for m in patron.finditer(bl):
            va = int(m.group(2), 16)
            if va in wanted and re.sub(r'\s+', '', m.group(1)) == re.sub(r'\s+', '', wanted[va]):
                piezas += [txt[pos:m.start()], f'DatosExe::Data::v{va:X}']
                pos = m.end(); cnt += 1
        piezas.append(txt[pos:])
        nuevo = ''.join(piezas)
        if cnt == 0:
            continue
        crlf = b'\r\n' in raw
        nl = '\r\n' if crlf else NL
        if inc not in nuevo:
            if arch.endswith('.cpp'):
                mm = re.search(r'#include\s+"StdInc.h"[^' + NL + r']*' + NL, nuevo)
            else:
                mm = re.search(r'#pragma once[^' + NL + r']*' + NL, nuevo)
            if not mm:
                raise SystemExit('sin punto de insercion en ' + arch)
            nuevo = nuevo[:mm.end()] + inc + nl + nuevo[mm.end():]
        p.write_bytes(nuevo.encode('utf-8', errors='surrogateescape'))
        print(arch, cnt)


if __name__ == '__main__':
    main()
