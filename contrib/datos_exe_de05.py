"""Lote DE05: variables de .bss del juego (estado en ejecucion) como definiciones propias (source/game_sa/DatosExe/DataBss<G>.h/.cpp,
namespace DatosExe::Bss), inicializadas a cero como el .bss del EXE.

Mismo criterio de seguridad que DE04 (datos_exe_de04.py), con el .bss y estos tipos permitidos: primitivos y alias (RwInt32...),
punteros a clase conocida o primitivo, arrays de primitivos (con N numerico) y las estructuras triviales CVector, CVector2D, CRGBA,
RwRGBA. Los tipos con constructor o destructor (CColModel, CLinkList...) y los enumerados quedan fuera.
Ademas se excluyen las direcciones que usan las pruebas de humo (prueba_agua.py, prueba_ropero.py, captura.py: jugador, estado de
juego, area, UnderWaterness...) y un margen alrededor de ellas, porque esas pruebas leen y escriben el .bss del proceso del EXE.

Uso: python contrib/datos_exe_de05.py <E1|E2|E3|E4|E5|T> [--aplicar]
  E1 enteros pequenos y bool; E2 enteros de 32/64 bits; E3 float y double; E4 punteros; E5 arrays y estructuras triviales.
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
LO, HI = 0x8E4000, 0xC9E000
SC = {'bool': 1, 'uint8': 1, 'int8': 1, 'char': 1, 'GxtChar': 1, 'RwUInt8': 1, 'int16': 2, 'uint16': 2, 'RwInt16': 2, 'RwUInt16': 2,
      'int32': 4, 'uint32': 4, 'RwInt32': 4, 'RwUInt32': 4, 'RwBool': 4, 'size_t': 4, 'DWORD': 4, 'int': 4, 'float': 4, 'RwReal': 4,
      'double': 8, 'uint64': 8, 'int64': 8}
KIND = {'bool': 'E1', 'uint8': 'E1', 'int8': 'E1', 'char': 'E1', 'GxtChar': 'E1', 'RwUInt8': 'E1', 'int16': 'E1', 'uint16': 'E1',
        'RwInt16': 'E1', 'RwUInt16': 'E1', 'int32': 'E2', 'uint32': 'E2', 'RwInt32': 'E2', 'RwUInt32': 'E2', 'RwBool': 'E2',
        'size_t': 'E2', 'DWORD': 'E2', 'int': 'E2', 'uint64': 'E2', 'int64': 'E2', 'float': 'E3', 'RwReal': 'E3', 'double': 'E3'}
STRUCT = {'CVector': 12, 'CVector2D': 8, 'CRGBA': 4, 'RwRGBA': 4}
HARNESS = [0x8D37D4, 0x8D37E0, 0x8D37E4, 0x8D514C, 0x8D621C, 0x8E4BA0, 0x8E4CB4, 0x8E4CB8, 0x8E4CC0, 0xA9B0C8, 0xB6F028, 0xB72914,
           0xB74498, 0xB7449C, 0xB744A0, 0xB76838, 0xB7683C, 0xB76840, 0xB76844, 0xB79E3C, 0xB79E48, 0xB79FD0, 0xB7C4A0, 0xB7C508,
           0xB7CB10, 0xB7CD74, 0xB7CD98, 0xC1703C, 0xC2116C, 0xC22884, 0xC22888, 0xC2288C, 0xC228E8, 0xC402D3, 0xC7DD58, 0xC8132C,
           0xC81330, 0xC81444, 0xC8D4C0]
# rangos que las pruebas recorren con indices (jugadores 0xB7CD98 + i*0x190; juego de colores 0xB7C4A0 de 172 bytes)
HARNESS_RANGOS = [(0xB7CD70, 0xB7D0B0), (0xB7C490, 0xB7C560)]
MARGEN = 16
# Excluidas por biseccion (ropero MAL con la variable propia, causa sin explicar): CTheScripts::SwitchDefaultAddress.
CULPABLES = {0xA43F54}
EXCL = ('/RenderWare/', '/Crt', '/crt', 'ExeProtectionStub', 'DatosExe/')
CLASES_PTR = re.compile(r'^(C[A-Z]\w+|Rw\w+|Rp\w+|Rx\w+|Rt\w+)$')


_DECL = None


def declaracion(base):
    """'class' o 'struct' segun la declaracion real en source/, o None si es un alias (using/typedef) o no se encuentra."""
    global _DECL
    if _DECL is None:
        _DECL = {}
        alias = set()
        for p in (RAIZ / 'source').rglob('*'):
            if p.suffix not in ('.h', '.hpp'):
                continue
            t = p.read_bytes().decode('utf-8', errors='replace')
            for m in re.finditer(r'(?m)^\s*(?:typedef\s+)?(class|struct)\s+(\w+)\s*(?:final\s*)?[{:;]', t):
                _DECL.setdefault(m.group(2), set()).add(m.group(1))
            for m in re.finditer(r'(?m)^\s*using\s+(\w+)\s*=', t):
                alias.add(m.group(1))
        for a in alias:
            _DECL[a] = {'alias'}
    k = _DECL.get(base)
    return next(iter(k)) if k and len(k) == 1 and 'alias' not in k else None


def clasificar(t):
    """(clase, tam, nombre_base_ptr) o None. clase: E1..E5."""
    t = re.sub(r'\s+', ' ', t.strip())
    if t in SC:
        return KIND[t], SC[t], None
    if t in STRUCT:
        return 'E5', STRUCT[t], None
    m = re.fullmatch(r'(?:const )?([\w:]+)(?: ?\*)+', t)
    if m:
        base = m.group(1)
        if base in SC or base in ('void',):
            return 'E4', 4, None
        if CLASES_PTR.match(base) and declaracion(base):
            return 'E4', 4, base
        return None
    m = re.fullmatch(r'std::array<\s*(\w+)\s*,\s*(\d+)\s*>', t) or re.fullmatch(r'(\w+) ?\[(\d+)\]', t)
    if m and m.group(1) in SC:
        return 'E5', SC[m.group(1)] * int(m.group(2)), None
    return None


def main():
    grupo = sys.argv[1]
    aplicar = '--aplicar' in sys.argv
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    refs = de.escanear_fuente('source')
    reempl, rangos = de.cargar_funciones()
    cod = de.escanear_exe(pe, reempl, rangos)
    por = defaultdict(list)
    for r in refs:
        por[r['va']].append(r)
    todas = sorted(set(por) | set(cod))
    cand = {}
    for va in sorted(por):
        if not (LO <= va < HI):
            continue
        rs = por[va]
        if va in CULPABLES:
            continue
        if any(any(x in r['archivo'] for x in EXCL) for r in rs):
            continue
        if {r['forma'] for r in rs} != {'StaticRef'}:
            continue
        ts = {re.sub(r'\s+', ' ', r['tipo'].strip()) for r in rs}
        if len(ts) != 1:
            continue
        t = next(iter(ts))
        cl = clasificar(t)
        if not cl:
            continue
        kind, tam, base = cl
        i = bisect.bisect_right(todas, va)
        if i < len(todas) and todas[i] < va + tam:
            continue
        c = cod.get(va)
        if c and any(c['funcs'].values()):
            continue
        if any(va - MARGEN <= h < va + tam + MARGEN for h in HARNESS) or any(a <= va < b or va <= a < va + tam for a, b in HARNESS_RANGOS):
            continue
        cand[va] = (kind, tam, t, base, rs)
    # un vecino anterior cuya extension llegue a este objeto tampoco vale
    ordenadas = sorted(cand)
    sel = []
    for va in ordenadas:
        i = bisect.bisect_left(todas, va)
        prev = todas[i - 1] if i > 0 else None
        if prev in cand and prev + cand[prev][1] > va:
            continue
        sel.append((va,) + cand[va])
    if grupo != 'T':
        sel = [s for s in sel if s[1] == grupo]
    if '--solo' in sys.argv:   # lista de VA (una por linea) a la que se limita el lote: biseccion de fallos
        solo = {int(x.strip(), 16) for x in Path(sys.argv[sys.argv.index('--solo') + 1]).read_text().split() if x.strip()}
        sel = [s for s in sel if s[0] in solo]
    print(f'DE05-{grupo}: {len(sel)} objetos, {sum(len(s[5]) for s in sel)} referencias, {sum(s[2] for s in sel)} bytes')
    out = RAIZ / 'docs/DatosExe/lotes'
    out.mkdir(parents=True, exist_ok=True)
    with (out / f'DE05{grupo}.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'tipo', 'bytes', 'refs', 'archivos', 'nombre'])
        for va, kind, tam, t, base, rs in sel:
            w.writerow([f'0x{va:08X}', t, tam, len(rs), ';'.join(sorted({Path(r['archivo']).name for r in rs}))[:100], f'b{va:X}'])
    if not aplicar:
        return
    nom = 'DataBss' + grupo
    fwd = sorted({s[4] for s in sel if s[4]})
    cab = ['#pragma once', '', '/*', f' * Lote DE05-{grupo} (docs/DatosExe): variables de .bss del juego como definiciones propias (a cero, como el .bss',
           f' * del EXE). Estado vivo: las lee y escribe solo C++. Generado por contrib/datos_exe_de05.py; definidas en {nom}.cpp.', ' */', '']
    for b in fwd:
        cab.append(declaracion(b) + ' ' + b + ';')
    if any('GxtChar' in s[3] for s in sel):
        cab += ['#include "GxtChar.h"', '']
    cab += ['', 'namespace DatosExe::Bss {']
    cpp = ['#include "StdInc.h"', '', f'#include "{nom}.h"', '', 'namespace DatosExe::Bss {']
    for va, kind, tam, t, base, rs in sel:
        arr = re.fullmatch(r'(\w+) ?\[(\d+)\]', t)
        if arr:
            cab.append(f'extern {arr.group(1)} b{va:X}[{arr.group(2)}]; // 0x{va:08X}')
            cpp.append(f'{arr.group(1)} b{va:X}[{arr.group(2)}]{{}}; // 0x{va:08X}')
        else:
            cab.append(f'extern {t} b{va:X}; // 0x{va:08X}')
            cpp.append(f'{t} b{va:X}{{}}; // 0x{va:08X}')
    cab += ['', '} // namespace DatosExe::Bss', '']
    cpp += ['', '} // namespace DatosExe::Bss', '']
    d = RAIZ / 'source/game_sa/DatosExe'
    (d / (nom + '.h')).write_text(NL.join(cab), encoding='utf-8', newline=NL)
    (d / (nom + '.cpp')).write_text(NL.join(cpp), encoding='utf-8', newline=NL)
    por_f = defaultdict(list)
    for va, kind, tam, t, base, rs in sel:
        for r in rs:
            por_f[r['archivo']].append((va, t))
    inc = f'#include "DatosExe/{nom}.h"'
    patron = re.compile(r'StaticRef\s*<\s*(.+?)\s*>\s*\(\s*0[xX]([0-9A-Fa-f]{6,8})\s*\)')
    for arch, lst in sorted(por_f.items()):
        p = RAIZ / arch
        raw = p.read_bytes()
        txt = raw.decode('utf-8', errors='surrogateescape')
        bl = de.blanquear_comentarios(txt)
        wanted = {va: re.sub(r'\s+', '', t) for va, t in lst}
        piezas, pos, cnt = [], 0, 0
        for m in patron.finditer(bl):
            va = int(m.group(2), 16)
            if va in wanted and re.sub(r'\s+', '', m.group(1)) == wanted[va]:
                piezas += [txt[pos:m.start()], f'DatosExe::Bss::b{va:X}']
                pos = m.end(); cnt += 1
        piezas.append(txt[pos:])
        if cnt == 0:
            continue
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
        print(arch, cnt)


if __name__ == '__main__':
    main()
