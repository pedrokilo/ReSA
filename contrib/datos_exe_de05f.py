"""Lote DE05-F: el resto de .bss del juego (tipos que DE05 E1..E5 no admitia) como definiciones propias.

Una variable pasa a ser nuestra (DatosExe::Bss::b<DIR>, definida en DataBssF.cpp) cuando:
  * todas sus referencias son StaticRef<T>(dir) con el mismo T, en archivos propios (no RenderWare ni CRT ni DatosExe);
  * T solo usa nombres declarados en cabeceras de source/ (clases, enumerados, alias, plantillas) o de la biblioteca estandar;
  * ninguna otra direccion referenciada ni tocada por codigo vivo cae dentro de su extension;
  * ningun codigo del EXE que pueda ejecutarse (funciones no reemplazadas y huecos ALCANZABLES de huecos-alcanzables.csv) la lee,
    la escribe ni le toma la direccion;
  * no esta en la lista de las pruebas de humo (ropero, agua, marcadores) ni en la lista de excluidos de la biseccion.
Las comprobaciones que dependen del compilador (tamano, destructor trivial, constructible por defecto) las hace el propio
DataBssF.cpp con static_assert y una etiqueta con la direccion; el modo `sonda` solo genera ese archivo (sin tocar consumidores),
se compila, y las direcciones que fallan se devuelven con `--excluir lista.txt` en el modo `aplicar`.

Uso:
  python contrib/datos_exe_de05f.py estadisticas
  python contrib/datos_exe_de05f.py sonda [--excluir lista.txt]
  python contrib/datos_exe_de05f.py aplicar [--excluir lista.txt] [--solo lista.txt]
"""
import bisect
import csv
import re
import sys
from collections import defaultdict, Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
import datos_exe_de05 as d5  # noqa: E402
from datos_exe_lote import RAIZ  # noqa: E402
from pe_inventory import PE  # noqa: E402

NL = chr(10)
LO, HI = d5.LO, d5.HI
STD_OK = {'std', 'array', 'notsa', 'mdarray', 'uint8', 'int8', 'uint16', 'int16', 'uint32', 'int32', 'uint64', 'int64', 'float',
          'double', 'bool', 'char', 'void', 'unsigned', 'const', 'short', 'long', 'int', 'size_t', 'wchar_t', 'u8', 'u16', 'u32'}
EXTRA_EXCL = ('/RenderWare/', '/Crt', '/crt', 'ExeProtectionStub', 'DatosExe/', 'StandaloneExe', 'SDLWrapper', 'app/platform')


def nombres_declarados():
    nombres = set()
    pats = [re.compile(r'(?m)\b(?:class|struct|union|enum(?:\s+class|\s+struct)?)\s+(?:\[\[\w+\]\]\s+)?(\w+)\s*(?:final\s*)?[{:;]'),
            re.compile(r'(?m)\busing\s+(\w+)\s*='), re.compile(r'(?m)\btypedef\s+[^;]*?\b(\w+)\s*;'),
            re.compile(r'(?m)template\s*<[^>]*>\s*(?:class|struct)\s+(\w+)')]
    for p in (RAIZ / 'source').rglob('*'):
        if p.suffix not in ('.h', '.hpp'):
            continue
        t = p.read_bytes().decode('utf-8', errors='replace')
        for r in pats:
            nombres.update(r.findall(t))
    return nombres


def identificadores_ok(t, decl):
    for tok in re.findall(r'[A-Za-z_]\w*', re.sub(r'\b\d[\w\']*', '', t)):
        if tok in STD_OK or tok in decl:
            continue
        if tok.isupper() or tok.startswith(('MAX_', 'NUM_', 'TOTAL_')):   # constantes de tamano (enumerados/constexpr de cabecera)
            continue
        return False
    return True


def candidatos(modo):
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    refs = de.escanear_fuente('source')
    cubre = de.cubierto_por_vecino(refs)   # campos de una estructura mayor que el C++ referencia suelta (TheCamera...): no son independientes
    reempl, rangos = de.cargar_funciones()
    # huecos muertos fuera del codigo vivo: solo cuentan los ALCANZABLES (huecos-alcanzables.csv)
    ok = RAIZ / 'docs/DatosExe/huecos-alcanzables.csv'
    if ok.exists():
        vivos = set()
        muertos = set()
        with ok.open(encoding='utf-8', newline='') as s:
            for r in csv.DictReader(s):
                (vivos if r['clase'].startswith('ALCANZABLE') else muertos).add(int(r['va'], 16))
        rangos = [r for r in rangos if not (r[0] == r[1] and r[0] in muertos)]
    cod = de.escanear_exe(pe, reempl, rangos)
    por = defaultdict(list)
    for r in refs:
        por[r['va']].append(r)
    todas = sorted(set(por) | set(cod))
    decl = nombres_declarados()
    motivos = Counter()
    sel = []
    for va in sorted(por):
        if not (LO <= va < HI):
            continue
        rs = por[va]
        if cubre(va) is not None:
            continue
        if any(any(x in r['archivo'] for x in EXTRA_EXCL) for r in rs):
            motivos['archivo excluido'] += 1
            continue
        if {r['forma'] for r in rs} != {'StaticRef'}:
            motivos['forma no StaticRef'] += 1
            continue
        ts = {re.sub(r'\s+', ' ', r['tipo'].strip()) for r in rs}
        if len(ts) != 1:
            motivos['tipos mixtos'] += 1
            continue
        t = next(iter(ts))
        if d5.clasificar(t):
            motivos['ya cubierto por E1..E5'] += 1
            continue
        if va in d5.CULPABLES:
            motivos['culpable de biseccion'] += 1
            continue
        if not identificadores_ok(t, decl):
            motivos['tipo con nombre no declarado en cabeceras'] += 1
            continue
        if any(r['archivo'].endswith(('.h', '.hpp', '.inc')) for r in rs):
            motivos['referencia en cabecera'] += 1
            continue
        i = bisect.bisect_right(todas, va)
        gap = (todas[i] - va) if i < len(todas) else 0x1000
        c = cod.get(va)
        if c and any(c['funcs'].values()):
            motivos['lector o escritor vivo'] += 1
            continue
        if any(va - d5.MARGEN <= h < va + min(gap, 0x4000) + d5.MARGEN for h in d5.HARNESS) or any(a < va + min(gap, 0x4000) and va < b for a, b in d5.HARNESS_RANGOS):
            motivos['humo'] += 1
            continue
        sel.append((va, t, gap, rs))
    return sel, motivos


def main():
    modo = sys.argv[1]
    excl = set()
    if '--excluir' in sys.argv:
        excl = {int(x.strip(), 16) for x in Path(sys.argv[sys.argv.index('--excluir') + 1]).read_text().split() if x.strip()}
    sel, motivos = candidatos(modo)
    sel = [s for s in sel if s[0] not in excl]
    if '--solo' in sys.argv:
        solo = {int(x.strip(), 16) for x in Path(sys.argv[sys.argv.index('--solo') + 1]).read_text().split() if x.strip()}
        sel = [s for s in sel if s[0] in solo]
    # un vecino anterior que llegue a este objeto (por tamano real desconocido, se vigila con static_assert contra el hueco)
    print(f'DE05-F: {len(sel)} objetos, {sum(len(s[3]) for s in sel)} referencias')
    for k, v in motivos.most_common():
        print(f'  descartados: {v:5d}  {k}')
    if modo == 'estadisticas':
        c = Counter(re.sub(r'<.*', '<>', s[1]) for s in sel)
        for k, v in c.most_common(40):
            print(f'  {v:4d} {k}')
        return
    out = RAIZ / 'docs/DatosExe/lotes'
    with (out / 'DE05F.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'tipo', 'hueco_max', 'refs', 'archivos', 'nombre'])
        for va, t, gap, rs in sel:
            w.writerow([f'0x{va:08X}', t, gap, len(rs), ';'.join(sorted({Path(r["archivo"]).name for r in rs}))[:100], f'b{va:X}'])
    nom = 'DataBssF'
    cab = ['#pragma once', '', '/*', ' * Lote DE05-F (docs/DatosExe): resto de variables de .bss del juego como objetos propios. Cada uno es un',
           ' * almacen en cero que ConstructF() construye con el constructor de su tipo (como el _initterm del EXE) y cuyo destructor, si no es',
           ' * trivial, se registra con el atexit del CRT del EXE. Se accede con b<DIR><T>() (devuelve T&; T es el tipo de la StaticRef original). Generado por',
           ' * contrib/datos_exe_de05f.py; definidas en DataBssF.cpp. Se incluye despues de StdInc.h (los tipos ya estan declarados).', ' */', '',
           'namespace DatosExe::Bss {', 'void ConstructF(); // construye todos (se llama sola al cargar la DLL, antes de InjectHooks)', '']
    import os
    incs = []
    for va, t_, gap, rs in sel:
        for r in rs:
            for ln in (RAIZ / r['archivo']).read_bytes().decode('utf-8', errors='replace').splitlines():
                m = re.match(r'\s*#\s*include\s+"([^"]+)"', ln)
                if m and 'StdInc' not in m.group(1) and 'DatosExe/' not in m.group(1):
                    cand = (RAIZ / r['archivo']).parent / m.group(1)
                    rel = os.path.relpath(cand, RAIZ / 'source/game_sa/DatosExe').replace(chr(92), '/') if cand.exists() else m.group(1)
                    if rel not in incs:
                        incs.append(rel)
    cpp = ['#include "StdInc.h"'] + [f'#include "{i}"' for i in incs] + ['', f'#include "{nom}.h"', '#include "Crt/CrtOnexit.h"', '', 'namespace DatosExe::Bss {', 'namespace {',
           'template <class T> struct Slot {', '    alignas(16) unsigned char raw[sizeof(T)]{};', '    T& ref() { return *std::launder(reinterpret_cast<T*>(raw)); }', '};',
           'template <class T> void Build(Slot<T>& s, void (__cdecl* dtor)()) {',
           '    std::construct_at(reinterpret_cast<T*>(s.raw));',
           '    if constexpr (!std::is_trivially_destructible_v<T>) {', '        ExeCrt::Onexit(reinterpret_cast<void*>(dtor));', '    }', '}', '}  // namespace', '']
    cons = []
    lineas = {}
    for va, t, gap, rs in sel:
        arr = re.fullmatch(r'(.+?) ?\[(\d+)\]', t)
        nm = f'b{va:X}'
        if arr:
            et, nn = arr.group(1), arr.group(2)
            ty = f'std::array<{et}, {nn}>'
            acc = f'reinterpret_cast<Ty_{va:X}&>(sl_{va:X}.ref())'
        else:
            ty = t
            acc = f'sl_{va:X}.ref()'
        lineas[len(cpp) + 1] = va
        cpp.append(f'static Slot<{ty}> sl_{va:X}; // 0x{va:08X}, hueco hasta el siguiente objeto conocido: {gap} bytes')
        cpp.append(f'static_assert(sizeof({ty}) <= {gap}, "TAM 0x{va:08X}");')
        cpp.append(f'static void __cdecl dt_{va:X}() {{ std::destroy_at(&sl_{va:X}.ref()); }}')
        cab.append(f'template <class T> T& {nm}(); // 0x{va:08X}; se especializa en DataBssF.cpp para {t}')
        cpp.append(f'using Ty_{va:X} = {t};')
        cpp.append(f'template <> Ty_{va:X}& {nm}<Ty_{va:X}>() {{ return {acc}; }}')
        cons.append(f'    Build(sl_{va:X}, &dt_{va:X});')
    cab += ['', '} // namespace DatosExe::Bss', '']
    cpp += ['', 'void ConstructF() {'] + cons + ['}', '', 'namespace {', 'const bool kConstructF = (ConstructF(), true);', '}', '', '} // namespace DatosExe::Bss', '']
    d = RAIZ / 'source/game_sa/DatosExe'
    (d / (nom + '.h')).write_text(NL.join(cab), encoding='utf-8', newline=NL)
    (d / (nom + '.cpp')).write_text(NL.join(cpp), encoding='utf-8', newline=NL)
    # mapa linea -> direccion para excluir por mensajes de error del compilador
    with (RAIZ / 'docs/DatosExe/lotes/DE05F.lineas.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        for ln in sorted(lineas):
            w.writerow([ln, f'0x{lineas[ln]:08X}'])
    if modo == 'sonda':
        print('sonda escrita (sin tocar consumidores)')
        return
    por_f = defaultdict(list)
    for va, t, gap, rs in sel:
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
                piezas += [txt[pos:m.start()], f'DatosExe::Bss::b{va:X}<{m.group(1).strip()}>()']
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
            # despues del ultimo #include del bloque inicial (antes de la primera linea de codigo que no sea preprocesador o comentario)
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
