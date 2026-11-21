"""Lote DE02: constantes de .rdata leidas por ayudante (f(0x..), ExeFloat(0x..)...), por reinterpret_cast o por *(T*)0x.. como
definiciones propias (source/game_sa/DatosExe/RdataAyudantes.h/.cpp). Escanea la fuente actual (no los CSV).

El tipo de la lectura sale del ayudante (se verifica por archivo que todas sus definiciones devuelven float) o del cast.
Uso: python contrib/datos_exe_de02.py [--aplicar]
"""
import csv
import re
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
from datos_exe_lote import literal, TIPO_CPP, EXCLUIR_ARCHIVOS, RAIZ  # noqa: E402
from pe_inventory import PE  # noqa: E402

LO, HI = 0x858378, 0x8A4000   # .rdata tras la IAT
TAM = {'float': 4, 'double': 8, 'int32': 4, 'uint32': 4}
AYUDANTES = ('ExeFloat', 'f', 'RadarExeFloat', 'constant', 'CamExeFloat', 'InstantHitConstant')
SUFIJO = {'float': '', 'double': '_d', 'int32': '_i', 'uint32': '_u'}
NL = chr(10)
T4 = r'(float|double|int32|uint32)'
H = r'0[xX]([0-9A-Fa-f]{6,8})'
RE_CAST = re.compile(r'\*\s*reinterpret_cast\s*<\s*(?:const\s+)?' + T4 + r'\s*\*\s*>\s*\(\s*' + H + r'\s*\)')
RE_CAST_C = re.compile(r'\*\s*\(\s*(?:const\s+)?' + T4 + r'\s*\*\s*\)\s*' + H + r'\b')
RE_STATIC = re.compile(r'StaticRef\s*<\s*' + T4 + r'\s*>\s*\(\s*' + H + r'\s*\)')
RE_HELP = re.compile(r'(?<![\w:.])(' + '|'.join(AYUDANTES) + r')\s*\(\s*' + H + r'\s*\)')


def ayudantes_float(blank):
    ok = set()
    for n in AYUDANTES:
        lam = re.findall(r'auto\s+' + n + r'\s*=\s*\[[^\]]*\]\s*\([^)]*\)\s*(?:->\s*[\w:]+\s*)?\{\s*return\s+([^;]+);', blank)
        fun = re.findall(r'(?:inline\s+|static\s+)*(\w+)\s+' + n + r'\s*\(\s*(?:uintptr_t|uint32|uintptr)\s+\w+\s*\)\s*\{\s*return\s+([^;]+);', blank)
        cuerpos = list(lam) + [c for _, c in fun]
        if cuerpos and all('float' in c for c in cuerpos) and all(t in ('float', 'auto') for t, _ in fun):
            ok.add(n)
    return ok


def escanear():
    cand = []
    for p in sorted((RAIZ / 'source').rglob('*')):
        if p.suffix not in ('.cpp', '.h', '.hpp', '.inc'):
            continue
        rel = p.relative_to(RAIZ).as_posix()
        if any(x in rel for x in EXCLUIR_ARCHIVOS) or 'DatosExe/' in rel or 'ExeProtectionStub' in rel:
            continue
        txt = p.read_bytes().decode('utf-8', errors='surrogateescape')
        if '0x8' not in txt and '0X8' not in txt:
            continue
        blank = de.blanquear_comentarios(txt)
        hel = ayudantes_float(blank)
        for rx in (RE_STATIC, RE_CAST, RE_CAST_C, RE_HELP):
            for m in rx.finditer(blank):
                if rx is RE_HELP:
                    if m.group(1) not in hel:
                        continue
                    tipo = 'float'
                else:
                    tipo = m.group(1)
                va = int(m.group(2), 16)
                if LO <= va and va + TAM[tipo] <= HI:
                    cand.append((rel, m.start(), m.end(), va, tipo))
    return cand


def main():
    aplicar = '--aplicar' in sys.argv
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    cand = escanear()
    cand.sort(key=lambda c: (c[0], c[1], -c[2]))
    sin, ult = [], None
    for c in cand:
        if ult and c[0] == ult[0] and c[1] < ult[2]:
            continue
        sin.append(c); ult = c
    de01 = {}
    for r in csv.DictReader((RAIZ / 'docs/DatosExe/lotes/DE01.csv').open(encoding='utf-8')):
        de01[(int(r['va'], 16), r['tipo'])] = r['nombre']
    objs = {}
    for _, _, _, va, tipo in sin:
        objs[(va, tipo)] = de01.get((va, tipo)) or f'k{va:X}{SUFIJO[tipo]}'
    nuevas = {k: v for k, v in objs.items() if k not in de01}
    print(f'DE02: {len(sin)} referencias, {len(objs)} constantes ({len(nuevas)} nuevas), {len({c[0] for c in sin})} archivos')
    cnt = defaultdict(int)
    for c in sin:
        cnt[(c[3], c[4])] += 1
    out = RAIZ / 'docs/DatosExe/lotes'
    out.mkdir(parents=True, exist_ok=True)
    with (out / 'DE02.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'tipo', 'valor_inicial', 'refs', 'nombre'])
        for (va, tipo), nom in sorted(objs.items()):
            off = pe.offset(va, TAM[tipo])
            w.writerow([f'0x{va:08X}', tipo, pe.data[off:off + TAM[tipo]].hex(), cnt[(va, tipo)], nom])
    if not aplicar:
        return
    cab = ['#pragma once', '', '/*',
           ' * Lote DE02 (docs/DatosExe): constantes de .rdata del EXE leidas por ayudante, reinterpret_cast o *(T*)0x..',
           ' * como definiciones propias (valores del EXE canonico). Sufijo: _d double, _i int32, _u uint32; sin sufijo, float.',
           ' * Generado por contrib/datos_exe_de02.py; definidas en RdataAyudantes.cpp (extern: no se pliegan al compilar).',
           ' */', '', 'namespace DatosExe::Rdata {']
    cpp = ['#include "StdInc.h"', '', '#include "RdataAyudantes.h"', '', '#include <bit>', '', 'namespace DatosExe::Rdata {']
    for (va, tipo), nom in sorted(nuevas.items()):
        off = pe.offset(va, TAM[tipo])
        lit, human = literal(tipo, pe.data[off:off + TAM[tipo]].hex())
        cab.append(f'extern const {TIPO_CPP[tipo]} {nom}; // 0x{va:08X} = {human}')
        cpp.append(f'const {TIPO_CPP[tipo]} {nom} = {lit}; // 0x{va:08X}')
    cab += ['', '} // namespace DatosExe::Rdata', '']
    cpp += ['', '} // namespace DatosExe::Rdata', '']
    d = RAIZ / 'source/game_sa/DatosExe'
    (d / 'RdataAyudantes.h').write_text(NL.join(cab), encoding='utf-8', newline=NL)
    (d / 'RdataAyudantes.cpp').write_text(NL.join(cpp), encoding='utf-8', newline=NL)
    por_f = defaultdict(list)
    for c in sin:
        por_f[c[0]].append(c)
    inc1 = '#include "DatosExe/RdataConstantes.h"'
    inc2 = '#include "DatosExe/RdataAyudantes.h"'
    for arch, cs in sorted(por_f.items()):
        p = RAIZ / arch
        raw = p.read_bytes()
        txt = raw.decode('utf-8', errors='surrogateescape')
        piezas, pos = [], 0
        for _, ini, fin, va, tipo in cs:
            piezas += [txt[pos:ini], f'DatosExe::Rdata::{objs[(va, tipo)]}']
            pos = fin
        piezas.append(txt[pos:])
        nuevo = ''.join(piezas)
        nl = '\r\n' if b'\r\n' in raw else NL
        # DE01 define sus constantes en RdataConstantes.h: el archivo necesita el include que corresponda a lo que usa
        need = []
        if any((va, tipo) in de01 for _, _, _, va, tipo in cs) and inc1 not in nuevo:
            need.append(inc1)
        if any((va, tipo) not in de01 for _, _, _, va, tipo in cs) and inc2 not in nuevo:
            need.append(inc2)
        if need:
            if arch.endswith('.cpp'):
                mm = re.search(r'#include\s+"StdInc.h"[^' + NL + r']*' + NL, nuevo)
            else:
                mm = re.search(r'#pragma once[^' + NL + r']*' + NL, nuevo)
            if not mm:
                raise SystemExit('sin punto de insercion en ' + arch)
            nuevo = nuevo[:mm.end()] + nl.join(need) + nl + nuevo[mm.end():]
        p.write_bytes(nuevo.encode('utf-8', errors='surrogateescape'))
        print(arch, len(cs))


if __name__ == '__main__':
    main()
