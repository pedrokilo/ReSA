"""Lote DE03: cadenas de .rdata que el C++ lee por direccion fija (reinterpret_cast<const char*>(0x..)) como cadenas propias
(source/game_sa/DatosExe/RdataCadenas.h/.cpp). Los textos salen del EXE canonico (NUL-terminados, ASCII imprimible).

Solo se reescribe la forma `reinterpret_cast<const char*|const RwChar*>(0xDIR)`; el resto (GaKey, kExePedEventFile, ayudantes
con direccion, GUID) se hace a mano en el mismo lote y se anota en docs/DatosExe/lotes/DE03.csv.
Uso: python contrib/datos_exe_de03.py [--aplicar]
"""
import csv
import re
import struct
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
    GARAGE = [0x8599C0, 0x8599DC, 0x8599E4, 0x8599EC, 0x8599F4, 0x8599FC, 0x859A04, 0x859A0C, 0x859A14, 0x859A1C, 0x859A24, 0x859A2C,
              0x859A34, 0x859A48, 0x859A50, 0x859A58]   # claves GXT de Garage.cpp (se reescriben a mano con ga_reescribir)
    GUIDS = [0x85EF9C, 0x85EFEC]                          # GUID de EAX (AEAudioChannel.cpp, AEAudioHardware.cpp)
    EXTRA = GARAGE + [0x86CD44]                          # + PedEvent.txt (DecisionMakerTypes.cpp)
    objs = {}
    for va in EXTRA:
        objs[va] = f'kS{va:X}'
    for _, _, _, va in cand:
        objs[va] = f'kS{va:X}'
    print(f'DE03 cadenas: {len(cand)} referencias, {len(objs)} cadenas, {len({c[0] for c in cand})} archivos')
    out = RAIZ / 'docs/DatosExe/lotes'
    out.mkdir(parents=True, exist_ok=True)
    cnt = defaultdict(int)
    for c in cand:
        cnt[c[3]] += 1
    for va in EXTRA:
        cnt[va] += 0
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
    for va in GUIDS:
        off = pe.offset(va, 16)
        b = pe.data[off:off + 16]
        d1, d2, d3 = struct.unpack('<IHH', b[:8])
        ini = '{0x%08X, 0x%04X, 0x%04X, {%s}}' % (d1, d2, d3, ', '.join('0x%02X' % x for x in b[8:]))
        cab.append(f'extern const GUID kG{va:X}; // 0x{va:08X}')
        cpp.append(f'const GUID kG{va:X} = {ini}; // 0x{va:08X}')
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
    manual(GARAGE)


def escribir(p, t):
    """Escribe conservando CRLF si el archivo lo usaba."""
    crlf = b'\r\n' in p.read_bytes()
    t = t.replace('\r\n', '\n')
    if crlf:
        t = t.replace('\n', '\r\n')
    p.write_bytes(t.encode('utf-8', errors='surrogateescape'))


def leer(p):
    return p.read_bytes().decode('utf-8', errors='surrogateescape')


def manual(garage):
    """Ediciones que no son un reinterpret_cast: claves GXT de Garage.cpp, PedEvent.txt y los GUID de EAX."""
    inc = '#include "DatosExe/RdataCadenas.h"\n'
    p = RAIZ / 'source/game_sa/Garage.cpp'
    txt = leer(p)
    bl = de.blanquear_comentarios(txt)
    out, pos, n = [], 0, 0
    for m in re.finditer(r'0[xX]([0-9A-Fa-f]{6})u?\b', bl):
        v = int(m.group(1), 16)
        if v in garage:
            out += [txt[pos:m.start()], 'DatosExe::Rdata::kS%X' % v]
            pos = m.end(); n += 1
    out.append(txt[pos:])
    t = ''.join(out).replace('\r\n', '\n')
    t = t.replace('inline const char* GaKey(uint32 address) { return reinterpret_cast<const char*>(address); }',
                  'inline const char* GaKey(const char* key) { return key; }')
    t = t.replace('inline void GaMessage(uint32 key)', 'inline void GaMessage(const char* key)')
    t = t.replace('        uint32       key;\n        if (type == 2 || type == 3) {', '        const char*  key;\n        if (type == 2 || type == 3) {')
    if inc not in t:
        t = t.replace('#include "StdInc.h"\n', '#include "StdInc.h"\n' + inc, 1)
    escribir(p, t)
    print('Garage.cpp', n)
    p = RAIZ / 'source/game_sa/DecisionMakers/DecisionMakerTypes.cpp'
    t = leer(p).replace('\r\n', '\n')
    t = t.replace('constexpr uintptr_t kExePedEventFile = 0x86CD44; // "PedEvent.txt"',
                  '// "PedEvent.txt": cadena propia DatosExe::Rdata::kS86CD44 (antes la del EXE en 0x86CD44)')
    t = t.replace('reinterpret_cast<const char*>(kExePedEventFile)', 'DatosExe::Rdata::kS86CD44')
    if inc not in t:
        t = t.replace('#include "StdInc.h"\n', '#include "StdInc.h"\n' + inc, 1)
    escribir(p, t)
    for rel, va in (('source/game_sa/Audio/Hardware/AEAudioChannel.cpp', 0x85EF9C), ('source/game_sa/Audio/Hardware/AEAudioHardware.cpp', 0x85EFEC)):
        p = RAIZ / rel
        t = leer(p).replace('\r\n', '\n')
        t = t.replace('StaticRef<GUID>(0x%X)' % va, 'DatosExe::Rdata::kG%X' % va)
        if inc not in t:
            t = t.replace('#include "StdInc.h"\n', '#include "StdInc.h"\n' + inc, 1)
        escribir(p, t)


if __name__ == '__main__':
    main()
