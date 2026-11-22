"""Lote DE04-D: .data inicializado del juego con tipos que DE04 A/B/C no admitia (estructuras trivialmente copiables CVector, CRGBA,
CRect..., ModelIndex y arrays de ellos) como definiciones propias (DataInicializadaD.h/.cpp, namespace DatosExe::Data) con los bytes
iniciales exactos del EXE canonico.

Mismo criterio de seguridad que DE04 (todas las referencias StaticRef con el mismo tipo, en archivos .cpp propios; ningun vecino
referenciado dentro de su extension; ningun codigo del EXE que pueda ejecutarse la toca: funciones no reemplazadas y huecos
ALCANZABLES de docs/DatosExe/huecos-alcanzables.csv). El tamano de cada tipo se comprueba con static_assert contra la tabla de
abajo y contra el hueco hasta el siguiente objeto conocido.

Uso: python contrib/datos_exe_de04d.py [--aplicar]
"""
import bisect
import csv
import os
import re
import sys
from collections import defaultdict, Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
import datos_exe_de04 as d4  # noqa: E402
from datos_exe_lote import RAIZ  # noqa: E402
from pe_inventory import PE  # noqa: E402

NL = chr(10)
LO, HI = 0x8A4000, 0x8E4000
TAM = {'CVector': 12, 'CVector2D': 8, 'CRGBA': 4, 'RwRGBA': 4, 'RwRGBAReal': 16, 'CRect': 16, 'RwV2d': 8, 'RwV3d': 12, 'ModelIndex': 2,
       'RwInt32': 4, 'RwUInt32': 4, 'RwBool': 4, 'RwReal': 4, 'uintptr_t': 4, 'size_t': 4, 'int': 4, 'unsigned': 4, 'RwInt16': 2,
       'RwUInt8': 1, 'RwUInt16': 2, 'DWORD': 4, 'char': 1}
EXTRA_EXCL = ('/RenderWare/', '/Crt', '/crt', 'ExeProtectionStub', 'DatosExe/', 'StandaloneExe', 'SDLWrapper', 'app/platform')


def parsear(t):
    """(elemento, n_o_None, bytes) para X o std::array<X,N> con X en TAM; si no None."""
    t = re.sub(r'\s+', '', t)
    if t in TAM:
        return t, None, TAM[t]
    m = re.fullmatch(r'std::array<(\w+),(\d+)>', t)
    if m and m.group(1) in TAM:
        return m.group(1), int(m.group(2)), TAM[m.group(1)] * int(m.group(2))
    return None


def main():
    aplicar = '--aplicar' in sys.argv
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    refs = de.escanear_fuente('source')
    cubre = de.cubierto_por_vecino(refs)   # campos de una estructura mayor que el C++ referencia suelta (TheCamera...): no son independientes
    reempl, rangos = de.cargar_funciones()
    ok = RAIZ / 'docs/DatosExe/huecos-alcanzables.csv'
    if ok.exists():
        with ok.open(encoding='utf-8', newline='') as s:
            muertos = {int(r['va'], 16) for r in csv.DictReader(s) if r['clase'].startswith('INALCANZABLE')}
        rangos = [r for r in rangos if not (r[0] == r[1] and r[0] in muertos)]
    cod = de.escanear_exe(pe, reempl, rangos)
    por = defaultdict(list)
    for r in refs:
        por[r['va']].append(r)
    todas = sorted(set(por) | set(cod))
    sel, motivos = [], Counter()
    for va in sorted(por):
        if not (LO <= va < HI) or va in d4.HARNESS:
            continue
        rs = por[va]
        if cubre(va) is not None:
            continue
        if any(any(x in r['archivo'] for x in EXTRA_EXCL) for r in rs):
            continue
        if {r['forma'] for r in rs} != {'StaticRef'}:
            continue
        ts = {re.sub(r'\s+', '', r['tipo']) for r in rs}
        if len(ts) != 1:
            continue
        t = next(iter(ts))
        if d4.parsear(t):
            continue          # lo cubre DE04 A/B/C
        pt = parsear(t)
        if not pt:
            continue
        if any(r['archivo'].endswith(('.h', '.hpp', '.inc')) for r in rs):
            motivos['referencia en cabecera'] += 1
            continue
        el, n, nb = pt
        i = bisect.bisect_right(todas, va)
        if i < len(todas) and todas[i] < va + nb:
            motivos['vecino dentro de la extension'] += 1
            continue
        if va + nb > HI:
            continue
        c = cod.get(va)
        if c and any(c['funcs'].values()):
            motivos['codigo vivo'] += 1
            continue
        sel.append((va, t, el, n, nb, rs))
    # un vecino anterior cuya extension llegue a este objeto tampoco vale
    keep = []
    ext = {s[0]: s[0] + s[4] for s in sel}
    for s in sel:
        i = bisect.bisect_left(todas, s[0])
        prev = todas[i - 1] if i > 0 else None
        if prev in ext and ext[prev] > s[0]:
            motivos['extension del vecino anterior'] += 1
            continue
        keep.append(s)
    sel = keep
    print(f'DE04-D: {len(sel)} objetos, {sum(len(s[5]) for s in sel)} referencias, {sum(s[4] for s in sel)} bytes')
    for k, v in motivos.most_common():
        print(f'  descartados: {v:4d}  {k}')
    print('  por tipo:', dict(Counter(re.sub(r'<.*', '<>', s[1]) for s in sel).most_common(12)))
    out = RAIZ / 'docs/DatosExe/lotes'
    with (out / 'DE04D.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'tipo', 'bytes', 'refs', 'archivos', 'nombre'])
        for va, t, el, n, nb, rs in sel:
            w.writerow([f'0x{va:08X}', t, nb, len(rs), ';'.join(sorted({Path(r["archivo"]).name for r in rs}))[:100], f'v{va:X}'])
    if not aplicar:
        return
    nom = 'DataInicializadaD'
    incs = []
    for va, t, el, n, nb, rs in sel:
        for r in rs:
            for ln in (RAIZ / r['archivo']).read_bytes().decode('utf-8', errors='replace').splitlines():
                m = re.match(r'\s*#\s*include\s+"([^"]+)"', ln)
                if m and 'StdInc' not in m.group(1) and 'DatosExe/' not in m.group(1):
                    cand = (RAIZ / r['archivo']).parent / m.group(1)
                    rel = os.path.relpath(cand, RAIZ / 'source/game_sa/DatosExe').replace(chr(92), '/') if cand.exists() else m.group(1)
                    if rel not in incs:
                        incs.append(rel)
    cab = ['#pragma once', '', '/*', ' * Lote DE04-D (docs/DatosExe): .data inicializado del juego con estructuras trivialmente copiables (CVector, CRGBA, CRect...,',
           ' * ModelIndex y arrays de ellos) como definiciones propias con los bytes iniciales del EXE. Generado por contrib/datos_exe_de04d.py;',
           ' * definidas en DataInicializadaD.cpp. Se incluye despues de los demas #include del consumidor (los tipos ya estan declarados).', ' */', '',
           'namespace DatosExe::Data {']
    cpp = ['#include "StdInc.h"'] + [f'#include "{i}"' for i in incs] + ['', f'#include "{nom}.h"', '', 'namespace DatosExe::Data {', 'namespace {',
           'template <class T, size_t K> T FromBytes(const uint8 (&bytes)[K]) {',
           '    static_assert(sizeof(T) == K, "tamano distinto del EXE");',
           '    static_assert(std::is_trivially_destructible_v<T> && std::is_standard_layout_v<T>);',
           '    T value;', '    std::memcpy(static_cast<void*>(&value), bytes, K);', '    return value;', '}', '}  // namespace', '']
    for va, t, el, n, nb, rs in sorted(sel):
        o = pe.offset(va, nb)
        data = pe.data[o:o + nb]
        i = bisect.bisect_right(todas, va)
        gap = (todas[i] - va) if i < len(todas) else 0x1000
        cab.append(f'extern {t} v{va:X}; // 0x{va:08X}')
        cpp.append(f'static constexpr uint8 k{va:X}[{nb}] = {{' + ', '.join(f'0x{b:02X}' for b in data) + '};')
        cpp.append(f'static_assert(sizeof({t}) <= {gap}, "TAM 0x{va:08X}");')
        cpp.append(f'{t} v{va:X} = FromBytes<{t}>(k{va:X}); // 0x{va:08X}')
    cab += ['', '} // namespace DatosExe::Data', '']
    cpp += ['', '} // namespace DatosExe::Data', '']
    d = RAIZ / 'source/game_sa/DatosExe'
    (d / (nom + '.h')).write_text(NL.join(cab), encoding='utf-8', newline=NL)
    (d / (nom + '.cpp')).write_text(NL.join(cpp), encoding='utf-8', newline=NL)
    por_f = defaultdict(list)
    for va, t, el, n, nb, rs in sel:
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
                piezas += [txt[pos:m.start()], f'DatosExe::Data::v{va:X}']
                pos = m.end()
                cnt += 1
        piezas.append(txt[pos:])
        if cnt == 0:
            continue
        nuevo = ''.join(piezas)
        nl = '\r\n' if b'\r\n' in raw else NL
        if inc not in nuevo:
            ms = list(re.finditer(r'(?m)^[ \t]*#[ \t]*include[^\r\n]*(\r?\n)', nuevo))
            if not ms:
                raise SystemExit('sin punto de insercion en ' + arch)
            fin_bloque = ms[0].end()
            for m_ in ms:
                previo = nuevo[fin_bloque:m_.start()]
                if re.search(r'(?m)^[ \t]*(?!#|//|/\*|\*|$)\S', previo):
                    break
                fin_bloque = m_.end()
            nuevo = nuevo[:fin_bloque] + inc + nl + nuevo[fin_bloque:]
        p.write_bytes(nuevo.encode('utf-8', errors='surrogateescape'))
        print(arch, cnt)


if __name__ == '__main__':
    main()
