"""Comparativa MEDIDA del ejecutable independiente (gta_sa_standalone.exe) contra el gta-sa.exe original: codigo, datos, estructura.

Subcomandos (todo es estatico y de solo lectura, salvo `volcados`, que lee los .bin que dejo comparar.py --fase medir):
  pe        secciones, importaciones y tamanos del independiente y del original -> estructura-*.csv
  codigo    tabla de bytes por categoria (funciones.csv + excluidas.csv), busqueda de copias identicas de funciones del original
            dentro del independiente y analisis del volcado en ejecucion del codigo (ganchos E9 / INT3 / otros)
  escaneo   desensambla (capstone) el codigo propio del independiente y recoge las referencias por direccion fija a 0x858000-0xCB0FFF
  datos     clasifica cada byte de la imagen de datos incrustada (y del .bss reservado) -> datos-*.csv
Salida: docs/CodigoVivoExe/comparativa-standalone-original/ . Informe: docs/CodigoVivoExe/comparativa-standalone-original.md
No ejecuta el juego ni modifica el gta-sa.exe (solo lo lee).
Uso: python -I contrib/comparativa_standalone.py <subcomando> [--std RUTA.exe] [--orig RUTA.exe] [--pdb RUTA.pdb] [--volcado-dir DIR]
"""
import argparse, bisect, csv, ctypes, hashlib, json, os, re, struct, sys, time
from collections import Counter, defaultdict
from ctypes import wintypes
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent
SAL = RAIZ / 'docs/CodigoVivoExe/comparativa-standalone-original'
STD = r'D:\Original\wv-int\reversing\FnFase8\salida\gta_sa_standalone.exe'
ORIG = r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
CANON = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
BASE = 0x400000
COD_INI, COD_FIN = 0x401000, 0x858000          # .text + _rwcseg del original (la reserva se rellena con INT3)
DAT_INI, DAT_FIN = 0x858000, 0xCB1000          # .rdata .. .rsrc
SEC = [('.rdata', 0x858000, 0x8A4000), ('.data', 0x8A4000, 0x8E4000), ('.bss', 0x8E4000, 0xC9E000),
       ('_TEXT_HA', 0xC9E000, 0xCAF000), ('_rwdseg', 0xCAF000, 0xCB0000), ('.rsrc', 0xCB0000, 0xCB1000)]


def seccion_de(va):
    for n, a, b in SEC:
        if a <= va < b:
            return n
    return None


def escribir_csv(ruta, filas, campos):
    ruta = Path(ruta); ruta.parent.mkdir(parents=True, exist_ok=True)
    with ruta.open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=campos, extrasaction='ignore'); w.writeheader(); w.writerows(filas)


class PEL:
    """Lector minimo de PE x86 (cualquiera; no exige el hash canonico)."""
    def __init__(self, ruta):
        self.ruta = str(ruta); self.d = Path(ruta).read_bytes()
        pe = struct.unpack_from('<I', self.d, 0x3C)[0]; self.pe = pe
        assert self.d[pe:pe + 4] == b'PE\0\0'
        mach, n, ts = struct.unpack_from('<HHI', self.d, pe + 4)
        osz = struct.unpack_from('<H', self.d, pe + 20)[0]
        self.base = struct.unpack_from('<I', self.d, pe + 52)[0]
        self.size_img = struct.unpack_from('<I', self.d, pe + 80)[0]
        self.entry = struct.unpack_from('<I', self.d, pe + 40)[0]
        self.timestamp = ts
        self.dirs = [struct.unpack_from('<II', self.d, pe + 24 + 96 + 8 * i) for i in range(16)]
        self.secs = []
        for i in range(n):
            p = pe + 24 + osz + 40 * i
            nom = self.d[p:p + 8].split(b'\0')[0].decode('ascii', 'replace')
            vs, rva, rs, rp = struct.unpack_from('<IIII', self.d, p + 8)
            fl = struct.unpack_from('<I', self.d, p + 36)[0]
            self.secs.append(dict(nombre=nom, va=self.base + rva, vsize=vs, rsize=rs, rptr=rp, flags=fl))

    def off(self, va):
        for s in self.secs:
            if s['va'] <= va < s['va'] + max(s['vsize'], s['rsize']):
                o = va - s['va']
                return s['rptr'] + o if o < s['rsize'] else None
        return None

    def leer(self, va, n):
        """n bytes desde va (lo que no esta en el archivo, cero). Solo dentro de una seccion."""
        out = bytearray(n)
        for s in self.secs:
            a, b = max(va, s['va']), min(va + n, s['va'] + s['vsize'])
            if a >= b:
                continue
            o = a - s['va']; k = max(0, min(b - a, s['rsize'] - o))
            if k > 0:
                out[a - va:a - va + k] = self.d[s['rptr'] + o:s['rptr'] + o + k]
        return bytes(out)

    def _rva_off(self, rva):
        return self.off(self.base + rva)

    def importaciones(self):
        rva, tam = self.dirs[1]
        res = []
        if not rva:
            return res
        p = self._rva_off(rva)
        while p is not None:
            oft, ts, fc, nrva, ft = struct.unpack_from('<IIIII', self.d, p)
            if not nrva:
                break
            q = self._rva_off(nrva); dll = self.d[q:self.d.index(b'\0', q)].decode('ascii', 'replace')
            t = self._rva_off(oft or ft); fn = []
            while t is not None:
                v = struct.unpack_from('<I', self.d, t)[0]
                if not v:
                    break
                if v & 0x80000000:
                    fn.append('#%d' % (v & 0xFFFF))
                else:
                    o = self._rva_off(v + 0)
                    fn.append(self.d[o + 2:self.d.index(b'\0', o + 2)].decode('ascii', 'replace'))
                t += 4
            res.append((dll, fn)); p += 20
        return res


def sha(ruta):
    h = hashlib.sha256()
    with open(ruta, 'rb') as f:
        for b in iter(lambda: f.read(1 << 20), b''):
            h.update(b)
    return h.hexdigest()


# ------------------------------------------------------------------ pe
def cmd_pe(a):
    SAL.mkdir(parents=True, exist_ok=True)
    filas = []; imp_filas = []; resumen = {}
    for et, ruta in (('standalone', a.std), ('original', a.orig)):
        p = PEL(ruta)
        resumen[et] = dict(ruta=ruta, bytes=os.path.getsize(ruta), sha256=sha(ruta), base='0x%X' % p.base, size_of_image=p.size_img, entry='0x%X' % (p.base + p.entry), secciones=len(p.secs))
        for s in p.secs:
            filas.append(dict(exe=et, seccion=s['nombre'], va='0x%08X' % s['va'], vsize=s['vsize'], raw=s['rsize'], ejecutable=bool(s['flags'] & 0x20000000), escribible=bool(s['flags'] & 0x80000000)))
        imps = p.importaciones(); resumen[et]['dlls'] = len(imps); resumen[et]['funciones_importadas'] = sum(len(f) for _, f in imps)
        for dll, fns in imps:
            imp_filas.append(dict(exe=et, dll=dll, funciones=len(fns), lista=';'.join(fns)))
    escribir_csv(SAL / 'estructura-secciones.csv', filas, ['exe', 'seccion', 'va', 'vsize', 'raw', 'ejecutable', 'escribible'])
    escribir_csv(SAL / 'estructura-importaciones.csv', imp_filas, ['exe', 'dll', 'funciones', 'lista'])
    json.dump(resumen, open(SAL / 'estructura-resumen.json', 'w'), indent=1)
    print(json.dumps(resumen, indent=1))
    for r in imp_filas:
        print(r['exe'], r['dll'], r['funciones'])


# ------------------------------------------------------------------ escaneo (capstone sobre el codigo propio del independiente)
def cmd_escaneo(a):
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_MEM, CS_OP_IMM, CS_AC_WRITE
    from capstone.x86 import X86_REG_INVALID
    p = PEL(a.std)
    s1 = p.secs[0]; assert s1['va'] == COD_INI
    ini = DAT_FIN                                                 # el codigo propio empieza tras la reserva de 0x401000-0xCB1000
    fin = s1['va'] + s1['vsize']
    o = p.off(ini); assert o is not None
    reserva = p.d[p.off(COD_INI):p.off(COD_INI) + (ini - COD_INI)]
    print('reserva 0x401000-0xCB1000 en el archivo: %d bytes, no nulos: %d' % (len(reserva), len(reserva) - reserva.count(0)))
    md = Cs(CS_ARCH_X86, CS_MODE_32); md.detail = True; md.skipdata = True
    code = p.d[o:o + (fin - ini)]
    datos = defaultdict(lambda: [0, 0, 0, 0, 0, 0])   # n, ancho_max, escribe, indice, direccion_tomada, ejemplo
    t0 = time.time(); ni = 0
    def instrucciones():
        pos = 0; N = len(code); T = 1 << 20          # por trozos de 1 MiB (capstone agota memoria con 40 MiB de una vez); se reanuda tras la ultima instruccion
        while pos < N:
            ult = pos
            for ins in md.disasm(code[pos:pos + T], ini + pos):
                ult = ins.address - ini + ins.size
                yield ins
            pos = ult if ult > pos else pos + 1
    for ins in instrucciones():
        ni += 1
        if ins.id == 0:                       # bytes de datos (skipdata)
            continue
        if not ins.operands:
            continue
        for op in ins.operands:
            if op.type == CS_OP_MEM:
                d = op.mem.disp & 0xFFFFFFFF
                if op.mem.segment == 0 and DAT_INI <= d < DAT_FIN:
                    e = datos[d]; e[0] += 1; e[1] = max(e[1], op.size)
                    if op.access & CS_AC_WRITE: e[2] = 1
                    if op.mem.index != X86_REG_INVALID: e[3] = 1
                    if not e[5]: e[5] = ins.address
            elif op.type == CS_OP_IMM:
                d = op.imm & 0xFFFFFFFF
                if DAT_INI <= d < DAT_FIN and ins.mnemonic in ('push', 'mov', 'lea', 'cmp', 'add', 'sub', 'test', 'and', 'or'):
                    e = datos[d]; e[0] += 1; e[4] = 1
                    if not e[5]: e[5] = ins.address
    print('instrucciones %d en %.0f s; direcciones distintas %d' % (ni, time.time() - t0, len(datos)))
    filas = [dict(va='0x%08X' % d, seccion=seccion_de(d), refs=e[0], ancho_max=e[1], escribe=e[2], indice=e[3], direccion_tomada=e[4], ejemplo='0x%08X' % e[5]) for d, e in sorted(datos.items())]
    escribir_csv(SAL / 'escaneo-referencias-standalone.csv', filas, ['va', 'seccion', 'refs', 'ancho_max', 'escribe', 'indice', 'direccion_tomada', 'ejemplo'])
    json.dump(dict(instrucciones=ni, direcciones=len(datos), bytes_codigo_escaneados=len(code), ini='0x%X' % ini, fin='0x%X' % fin, reserva_no_nulos=len(reserva) - reserva.count(0)), open(SAL / 'escaneo-resumen.json', 'w'), indent=1)


# ------------------------------------------------------------------ codigo
def leer_csv(ruta):
    with open(RAIZ / ruta, encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def rangos_funciones():
    return [(int(r['function_va'], 16), int(r['start_va'], 16), int(r['end_va'], 16)) for r in leer_csv('docs/MemoryMap/binary_function_ranges.csv')]


def categorias_codigo():
    cat = {}
    for r in leer_csv('docs/JuegoExe/funciones.csv'):
        cat[int(r['va'], 16)] = ('A reemplazada por C++', 'juego')
    for r in leer_csv('docs/JuegoExe/excluidas.csv'):
        c = r['categoria']
        cat[int(r['va'], 16)] = ('C excluida: cubierta por sus llamadores en C++', c) if c == 'usercall-cubierta' else ('D excluida / no reconstruible en C++ puro', c)
    for r in leer_csv('docs/RenderWareExe/funciones.csv'):
        va = int(r['va'], 16); e = r['estado']; m = r['clase_modulo']
        if e == 'reemplazada': cat[va] = ('A reemplazada por C++', 'renderware/D3D')
        elif e == 'enlazada': cat[va] = ('B biblioteca de terceros enlazada (no se reconstruye)', m)
        else: cat[va] = ('D excluida / no reconstruible en C++ puro', 'RW: ' + m)
    return cat


def cmd_codigo(a):
    SAL.mkdir(parents=True, exist_ok=True)
    rangos = rangos_funciones(); tam = defaultdict(int)
    for f, ini, fin in rangos: tam[f] += fin - ini + 1          # una funcion puede tener varios tramos (misma function_va)
    tam = dict(tam)
    cat = categorias_codigo()
    sin_rango = [va for va in cat if va not in tam]
    for f in tam:
        if f not in cat:
            cat[f] = ('E CRT/biblioteca del EXE fuera de los mapas', 'region 0x%02X0000' % (f >> 16))
    agg = defaultdict(lambda: [0, 0]); tot = [0, 0]
    for va, (c, sc) in cat.items():
        if va not in tam: continue
        agg[(c, sc)][0] += 1; agg[(c, sc)][1] += tam[va]; tot[0] += 1; tot[1] += tam[va]
    filas = [dict(categoria=c, subcategoria=sc, funciones=n, bytes=b, pct_bytes='%.2f' % (100.0 * b / tot[1])) for (c, sc), (n, b) in sorted(agg.items())]
    filas.append(dict(categoria='TOTAL', subcategoria='', funciones=tot[0], bytes=tot[1], pct_bytes='100.00'))
    escribir_csv(SAL / 'codigo-tabla-detalle.csv', filas, ['categoria', 'subcategoria', 'funciones', 'bytes', 'pct_bytes'])
    gruesa = defaultdict(lambda: [0, 0])
    for (c, sc), (n, b) in agg.items(): gruesa[c][0] += n; gruesa[c][1] += b
    filas2 = [dict(categoria=c, funciones=n, bytes=b, pct_bytes='%.2f' % (100.0 * b / tot[1])) for c, (n, b) in sorted(gruesa.items())] + [dict(categoria='TOTAL', funciones=tot[0], bytes=tot[1], pct_bytes='100.00')]
    escribir_csv(SAL / 'codigo-tabla.csv', filas2, ['categoria', 'funciones', 'bytes', 'pct_bytes'])
    for f in filas2: print(f)
    print('funciones de los CSV sin rango en binary_function_ranges:', len(sin_rango))
    base_mapas = sum(tam[v] for v in cat if v in tam and v < 0x820000)
    rep = sum(tam[v] for v, (c, _) in cat.items() if v in tam and c.startswith('A '))
    print('reproduccion README: base juego+RW (<0x820000) = %d B; reemplazada %d B = %.2f %%' % (base_mapas, rep, 100.0 * rep / base_mapas))
    print('bytes de funciones %d de %d bytes de la region de codigo (%.1f %%)' % (tot[1], COD_FIN - COD_INI, 100.0 * tot[1] / (COD_FIN - COD_INI)))
    json.dump(dict(total_funciones=tot[0], total_bytes=tot[1], base_mapas=base_mapas, bytes_region_codigo=COD_FIN - COD_INI), open(SAL / 'codigo-resumen.json', 'w'), indent=1)

    hooks = set()
    for r in leer_csv('docs/hooks.csv'):
        try: hooks.add(int(r['address'], 16))
        except ValueError: pass
    con_gancho = [f for f in tam if f in hooks]
    rep_f = [f for f in tam if cat[f][0].startswith('A ')]
    print('ganchos del catalogo %d; funciones del mapa con gancho en su inicio %d (%d B); reemplazadas segun CSV %d; reemplazadas SIN gancho en el catalogo %d; con gancho pero no reemplazadas %d' % (
        len(hooks), len(con_gancho), sum(tam[f] for f in con_gancho), len(rep_f), len([f for f in rep_f if f not in hooks]), len([f for f in con_gancho if not cat[f][0].startswith('A ')])))
    escribir_csv(SAL / 'codigo-reemplazadas-sin-gancho.csv', [dict(va='0x%08X' % f, bytes=tam[f], categoria=cat[f][1]) for f in rep_f if f not in hooks], ['va', 'bytes', 'categoria'])
    escribir_csv(SAL / 'codigo-gancho-no-reemplazada.csv', [dict(va='0x%08X' % f, bytes=tam[f], categoria=cat[f][0], sub=cat[f][1]) for f in con_gancho if not cat[f][0].startswith('A ')], ['va', 'bytes', 'categoria', 'sub'])

    orig = PEL(a.orig); std = PEL(a.std)
    assert sha(a.orig) == CANON
    buscar = {}
    for f, ini, fin in rangos:
        n = fin - ini + 1
        if n < 24: continue
        b = orig.leer(ini, n)
        buscar.setdefault(b[:24], []).append((f, b))
    print('tramos >= 24 B buscados en el archivo del independiente:', sum(len(v) for v in buscar.values()))
    data = std.d; hallazgos = []
    claves = set(buscar)
    for i in range(len(data) - 24):
        k = data[i:i + 24]
        if k in claves:
            for f, b in buscar[k]:
                if data[i:i + len(b)] == b: hallazgos.append((f, i, len(b)))
    print('copias identicas completas halladas:', len(hallazgos), 'bytes:', sum(n for _, _, n in hallazgos))
    escribir_csv(SAL / 'codigo-copias-identicas.csv', [dict(va='0x%08X' % f, offset_std=o, bytes=n, categoria=cat.get(f, ('', ''))[0], sub=cat.get(f, ('', ''))[1]) for f, o, n in hallazgos], ['va', 'offset_std', 'bytes', 'categoria', 'sub'])
    analizar_volcado_codigo(a, rangos, cat, tam)


def analizar_volcado_codigo(a, rangos, cat, tam):
    ruta = Path(a.volcado_dir) / 'volcado_codigo_std.bin'
    if not ruta.exists():
        print('sin volcado de codigo en ejecucion (%s)' % ruta); return
    v = ruta.read_bytes(); assert len(v) == COD_FIN - COD_INI
    orig = PEL(a.orig)
    est = Counter(); estb = Counter(); filas = []
    cubierto = bytearray(len(v))
    for f, ini, fin in rangos:
        if ini != f or not (COD_INI <= ini < COD_FIN): continue
        o = ini - COD_INI; b = v[o:o + 5]; n8 = min(8, fin - ini + 1)
        if b[:1] == b'\xE9':
            dest = (ini + 5 + struct.unpack('<i', b[1:5])[0]) & 0xFFFFFFFF
            k = 'gancho E9 a C++ del independiente' if dest >= DAT_FIN else 'E9 a otra direccion del EXE original'
            if dest >= DAT_FIN:
                for j in range(5): cubierto[o + j] = 1
        elif v[o:o + n8] == b'\xCC' * n8: k = 'INT3 intacto (sin gancho)'
        else: k = 'otro contenido'
        c = cat[f][0] if f in cat else '?'
        est[(k, c)] += 1; estb[(k, c)] += tam[f]
        filas.append(dict(va='0x%08X' % f, bytes=tam[f], categoria=c, estado_en_ejecucion=k))
    escribir_csv(SAL / 'codigo-ejecucion-funciones.csv', filas, ['va', 'bytes', 'categoria', 'estado_en_ejecucion'])
    resumen = [dict(estado=k[0], categoria=k[1], funciones=n, bytes=estb[k]) for k, n in sorted(est.items())]
    escribir_csv(SAL / 'codigo-ejecucion-resumen.csv', resumen, ['estado', 'categoria', 'funciones', 'bytes'])
    for r in resumen: print(r)
    nocc = sum(1 for x in v if x != 0xCC); gan = sum(cubierto)
    print('region de codigo en ejecucion: %d B; distintos de 0xCC: %d; bytes de ganchos E9 en inicios de funcion: %d; resto no-CC: %d' % (len(v), nocc, gan, nocc - gan))
    runs = []
    for i in range(len(v)):
        if v[i] != 0xCC and not cubierto[i]:
            if runs and i == runs[-1][1]: runs[-1][1] = i + 1
            else: runs.append([i, i + 1])
    igual = sum(1 for x, y in runs if v[x:y] == orig.leer(COD_INI + x, y - x))
    print('tramos no-CC fuera de ganchos de inicio:', len(runs), '; identicos a bytes del original:', igual)
    escribir_csv(SAL / 'codigo-ejecucion-tramos-no-cc.csv', [dict(va='0x%08X' % (COD_INI + x), bytes=y - x, hex=v[x:min(y, x + 16)].hex(), igual_al_original=(v[x:y] == orig.leer(COD_INI + x, y - x))) for x, y in runs[:5000]], ['va', 'bytes', 'hex', 'igual_al_original'])


# ------------------------------------------------------------------ datos
def objetos_propios(pdb, exe):
    """Variables DatosExe::* (Tag SymTagData) del PDB del independiente: (nombre, direccion_propia, tamano). Solo lectura."""
    sys.path.insert(0, str(RAIZ / 'contrib/auditoria_datos'))
    import simbolos_pdb as sp
    dh = ctypes.WinDLL('dbghelp.dll')
    dh.SymLoadModuleExW.restype = ctypes.c_uint64
    dh.SymLoadModuleExW.argtypes = [wintypes.HANDLE, wintypes.HANDLE, wintypes.LPCWSTR, wintypes.LPCWSTR, ctypes.c_uint64, wintypes.DWORD, ctypes.c_void_p, wintypes.DWORD]
    dh.SymEnumSymbols.argtypes = [wintypes.HANDLE, ctypes.c_uint64, ctypes.c_char_p, sp.ENUMCB, ctypes.c_void_p]
    h = ctypes.c_void_p(0x4321); dh.SymSetOptions(0x2 | 0x4 | 0x10000 | 0x100000); dh.SymInitializeW(h, None, False)
    m = dh.SymLoadModuleExW(h, None, exe, None, BASE, 0x4000000, None, 0)
    res = []

    def cb(si, tam, ctx):
        x = si.contents
        if x.Tag == 7:
            res.append((x.Name[:x.NameLen].decode('latin1'), x.Address, tam or x.Size))
        return True
    f = sp.ENUMCB(cb)
    dh.SymEnumSymbols(h, m, b'*DatosExe*', f, None)
    dh.SymCleanup(h)
    return res


PAT_VA = re.compile(r'^DatosExe::(Bss|Data|Rdata|R5)::(?:sl_|[a-z]{1,2})([0-9A-F]{6})(?:_[A-Za-z0-9]+)?$')


def trozos_incrustados():
    """Tabla kTrozos de source/game_sa/DatosExe/Imagen/ImagenTabla.cpp (la que usa el cargador)."""
    for base in (RAIZ, Path(r'D:\Original\wv-int')):
        t = base / 'source/game_sa/DatosExe/Imagen/ImagenTabla.cpp'
        if t.exists():
            return [(int(m.group(1), 16), int(m.group(2))) for m in re.finditer(r'\{0x([0-9A-Fa-f]{8}), (\d+), Datos\d+\}', t.read_text())]
    raise FileNotFoundError('ImagenTabla.cpp')


def cmd_datos(a):
    SAL.mkdir(parents=True, exist_ok=True)
    orig = PEL(a.orig); std = PEL(a.std)
    assert sha(a.orig) == CANON
    # 1) trozos incrustados: comprobar que sus bytes estan en el archivo del independiente
    trozos = trozos_incrustados(); filas_t = []
    img = bytearray(DAT_FIN - DAT_INI); cop = bytearray(DAT_FIN - DAT_INI)      # contenido original / marca de byte copiado
    for va, n in trozos:
        b = orig.leer(va, n); pos = std.d.find(b)
        filas_t.append(dict(va='0x%08X' % va, fin='0x%08X' % (va + n - 1), bytes=n, seccion=seccion_de(va), en_archivo_std=(pos >= 0), offset_std=pos))
        img[va - DAT_INI:va - DAT_INI + n] = b
        for i in range(n): cop[va - DAT_INI + i] = 1
    escribir_csv(SAL / 'datos-trozos-copiados.csv', filas_t, ['va', 'fin', 'bytes', 'seccion', 'en_archivo_std', 'offset_std'])
    copiados = sum(cop); print('trozos %d, bytes copiados %d, todos hallados en el archivo del independiente: %s' % (len(trozos), copiados, all(f['en_archivo_std'] for f in filas_t)))
    ceros_copiados = sum(1 for i in range(len(cop)) if cop[i] and img[i] == 0)
    print('de ellos, bytes que valen 0x00:', ceros_copiados)
    # 2) objetos propios (PDB)
    objs = objetos_propios(a.pdb, a.std)
    propios = []     # (va_orig, tam, nombre, ns)
    sin_va = Counter()
    for n, dirp, t in objs:
        m = PAT_VA.match(n)
        if m and DAT_INI <= int(m.group(2), 16) | 0x0 < DAT_FIN + 0x1000:
            propios.append((int(m.group(2), 16), t, n, m.group(1), dirp))
        elif n.startswith(('DatosExe::Bss', 'DatosExe::Data', 'DatosExe::Rdata', 'DatosExe::R5')):
            sin_va[n.split('::')[1]] += 1
    print('objetos propios con VA original en el nombre:', len(propios), '; sin VA reconocible:', dict(sin_va))
    # va de objetos con sombra de tamano 0 y duplicados
    own = bytearray(DAT_FIN - DAT_INI); por_ns = Counter(); por_sec = defaultdict(int)
    for va, t, n, ns, dirp in propios:
        if not (DAT_INI <= va < DAT_FIN):
            continue
        por_ns[ns] += t
        for i in range(va, min(va + t, DAT_FIN)): own[i - DAT_INI] = 1
    for i in range(len(own)):
        if own[i]: por_sec[seccion_de(DAT_INI + i)] += 1
    print('bytes de la imagen original cubiertos por objetos propios (union):', sum(own), dict(por_sec), 'suma bruta por ns:', dict(por_ns))
    # 3) referencias por direccion fija del codigo del independiente (escaneo)
    refs = {int(r['va'], 16): r for r in leer_csv_abs(SAL / 'escaneo-referencias-standalone.csv')}
    objetos = {}
    for r in leer_csv('docs/DatosExe/objetos.csv'):
        objetos[int(r['va'], 16)] = r
    ref = bytearray(DAT_FIN - DAT_INI)
    for va, r in refs.items():
        w = int(r['ancho_max'] or 0) or 1
        if int(r['direccion_tomada']):
            o = objetos.get(va); w = max(w, int(o['tam']) if o else 4)
        for i in range(va, min(va + w, DAT_FIN)): ref[i - DAT_INI] = 1
    # referencias que caen dentro de objetos propios (dobles: dato propio y lectura por direccion)
    dobles = [va for va in refs if own[va - DAT_INI]]
    print('direcciones referenciadas por codigo propio: %d; de ellas dentro de un objeto propio: %d' % (len(refs), len(dobles)))
    escribir_csv(SAL / 'datos-refs-en-objeto-propio.csv', [dict(va='0x%08X' % v, seccion=seccion_de(v), refs=refs[v]['refs'], escribe=refs[v]['escribe'], ejemplo=refs[v]['ejemplo']) for v in sorted(dobles)], ['va', 'seccion', 'refs', 'escribe', 'ejemplo'])
    # 4) punteros dentro de la imagen copiada (a datos y a codigo) y tablas de punteros a codigo
    ptr_dat = bytearray(DAT_FIN - DAT_INI); vt = bytearray(DAT_FIN - DAT_INI); n_ptr_cod = 0; vt_runs = []
    palabras = struct.unpack('<%dI' % ((DAT_FIN - DAT_INI) // 4), bytes(img))
    for k, w in enumerate(palabras):
        va = DAT_INI + 4 * k
        if not cop[va - DAT_INI]: continue
        if DAT_INI <= w < DAT_FIN and w < 0x3F000000:
            for j in range(w, min(w + 4, DAT_FIN)): ptr_dat[j - DAT_INI] = 1
    k = 0
    while k < len(palabras):
        if COD_INI <= palabras[k] < COD_FIN and cop[4 * k]:
            j = k
            while j < len(palabras) and COD_INI <= palabras[j] < COD_FIN: j += 1
            if j - k >= 2:
                vt_runs.append((DAT_INI + 4 * k, j - k))
                for x in range(4 * k, 4 * j): vt[x] = 1
            n_ptr_cod += j - k; k = j
        else: k += 1
    escribir_csv(SAL / 'datos-tablas-punteros-codigo.csv', [dict(va='0x%08X' % v, palabras=n, bytes=4 * n, seccion=seccion_de(v)) for v, n in vt_runs], ['va', 'palabras', 'bytes', 'seccion'])
    print('tablas de >=2 punteros consecutivos a codigo original (vtables y similares): %d tramos, %d bytes; punteros a codigo en total %d' % (len(vt_runs), sum(4 * n for _, n in vt_runs), n_ptr_cod))
    # 5) objetos que solo leen funciones del EXE (informe estatico previo): solo lectores vivos / solo lectores reemplazados
    solo_vivo = bytearray(DAT_FIN - DAT_INI); solo_repl = bytearray(DAT_FIN - DAT_INI)
    for va, r in objetos.items():
        if not (DAT_INI <= va < DAT_FIN): continue
        t = int(r['tam'] or 1); viv = int(r['funcs_exe_vivas'] or 0); rep = int(r['funcs_exe_reemplazadas'] or 0)
        tgt = solo_vivo if viv > 0 else (solo_repl if rep > 0 else None)
        if tgt is not None:
            for i in range(va, min(va + t, DAT_FIN)): tgt[i - DAT_INI] = 1
    # 6) clasificacion por byte copiado (prioridad f > e > o > a > c > b > ceros/cadenas > resto)
    IAT = (0x858000, 0x858000 + 888)
    def clase(va):
        i = va - DAT_INI; s = seccion_de(va)
        if IAT[0] <= va < IAT[1]: return 'f1 IAT (reescrita por el cargador)'
        if s in ('_TEXT_HA', '_rwdseg'): return 'f2 proteccion del EXE (_TEXT_HA, _rwdseg)'
        if s == '.rsrc': return 'f3 recursos (.rsrc)'
        if vt[i]: return 'e vtables y tablas de punteros a codigo (G9)'
        if own[i]: return 'o con objeto propio DatosExe::*'
        if ref[i]: return 'a leido por C++ por direccion fija, sin objeto propio'
        if ptr_dat[i]: return 'c apuntado desde tablas de la imagen (por puntero)'
        if solo_vivo[i]: return 'b solo lo leen funciones del EXE no sustituidas (no se ejecutan en el independiente)'
        if solo_repl[i]: return 'b2 solo lo leen funciones del EXE que ya son C++ (copia probablemente muerta)'
        if img[i] == 0: return 'd1 sin referencia estatica: ceros'
        return 'd2 sin referencia estatica: no nulo (c o d: base+registro, tablas indexadas o sin lector)'
    cls = Counter(); cls_sec = Counter(); filas_b = []
    for i in range(len(cop)):
        if not cop[i]: continue
        va = DAT_INI + i; c = clase(va); s = seccion_de(va)
        cls[c] += 1; cls_sec[(s, c)] += 1
    tot = sum(cls.values())
    filas = [dict(clase=c, bytes=n, pct_copiados='%.2f' % (100.0 * n / tot)) for c, n in sorted(cls.items())] + [dict(clase='TOTAL copiado', bytes=tot, pct_copiados='100.00')]
    escribir_csv(SAL / 'datos-clasificacion.csv', filas, ['clase', 'bytes', 'pct_copiados'])
    escribir_csv(SAL / 'datos-clasificacion-por-seccion.csv', [dict(seccion=s, clase=c, bytes=n) for (s, c), n in sorted(cls_sec.items())], ['seccion', 'clase', 'bytes'])
    for f in filas: print(f)
    # 7) propios frente a copiados: por seccion
    filas_s = []
    for nom, ini, fin in SEC:
        tam_sec = fin - ini; copi = sum(cop[ini - DAT_INI:fin - DAT_INI]); prop = sum(own[ini - DAT_INI:fin - DAT_INI])
        propio_cop = sum(1 for i in range(ini - DAT_INI, fin - DAT_INI) if own[i] and cop[i]); refd = sum(1 for i in range(ini - DAT_INI, fin - DAT_INI) if ref[i] and not own[i])
        filas_s.append(dict(seccion=nom, va='0x%08X' % ini, tam_seccion=tam_sec, copiados_incrustados=copi, cubiertos_por_objeto_propio=prop, propios_dentro_de_lo_copiado=propio_cop, referenciados_por_direccion_sin_objeto=refd,
                            pct_propio_de_seccion='%.2f' % (100.0 * prop / tam_sec), pct_copiado_de_seccion='%.2f' % (100.0 * copi / tam_sec)))
    escribir_csv(SAL / 'datos-por-seccion.csv', filas_s, ['seccion', 'va', 'tam_seccion', 'copiados_incrustados', 'cubiertos_por_objeto_propio', 'propios_dentro_de_lo_copiado', 'referenciados_por_direccion_sin_objeto', 'pct_propio_de_seccion', 'pct_copiado_de_seccion'])
    for f in filas_s: print(f)
    # .bss: bytes reservados (cero) y usados por direccion
    bss = SEC[2]; b0, b1 = bss[1] - DAT_INI, bss[2] - DAT_INI
    n_ref_bss = sum(ref[b0:b1]); n_own_bss = sum(own[b0:b1]); n_ref_noown = sum(1 for i in range(b0, b1) if ref[i] and not own[i])
    print('.bss: %d B; con objeto propio %d; referenciados por direccion y sin objeto %d; ni uno ni otro %d' % (b1 - b0, n_own_bss, n_ref_noown, b1 - b0 - n_own_bss - n_ref_noown))
    json.dump(dict(copiados=copiados, ceros_copiados=ceros_copiados, trozos=len(trozos), objetos_propios=len(propios), propios_union=sum(own), refs_direcciones=len(refs), refs_en_objeto_propio=len(dobles),
                   bss=dict(tam=b1 - b0, propio=n_own_bss, ref_sin_objeto=n_ref_noown), clases=dict(cls), por_ns=dict(por_ns)), open(SAL / 'datos-resumen.json', 'w'), indent=1)


def leer_csv_abs(ruta):
    with open(ruta, encoding='utf-8', newline='') as f:
        return list(csv.DictReader(f))


# ------------------------------------------------------------------ volcados (memoria en ejecucion, dejada por comparar.py --fase medir)
def cmd_volcados(a):
    d = Path(a.volcado_dir); orig = PEL(a.orig)
    ini = bytearray(DAT_FIN - DAT_INI)
    for nom, s0, s1 in SEC:
        n = s1 - s0
        ini[s0 - DAT_INI:s1 - DAT_INI] = orig.leer(s0, n) if nom != '.bss' else bytes(n)
    filas = []
    for modo in ('std', 'orig'):
        f = d / ('volcado_datos_%s.bin' % modo)
        if not f.exists():
            print('sin', f); continue
        v = f.read_bytes(); assert len(v) == DAT_FIN - DAT_INI
        for nom, s0, s1 in SEC:
            a0, a1 = s0 - DAT_INI, s1 - DAT_INI
            dif = sum(1 for i in range(a0, a1) if v[i] != ini[i]); nz = sum(1 for i in range(a0, a1) if v[i] != 0)
            filas.append(dict(modo=modo, seccion=nom, tam=s1 - s0, bytes_distintos_de_la_imagen_inicial=dif, bytes_no_nulos_en_ejecucion=nz))
        # codigo
        c = d / ('volcado_codigo_%s.bin' % modo)
        if c.exists():
            cv = c.read_bytes(); ref = orig.leer(COD_INI, len(cv))
            dif = sum(1 for i in range(len(cv)) if cv[i] != ref[i]); ncc = sum(1 for x in cv if x != 0xCC)
            filas.append(dict(modo=modo, seccion='CODIGO 0x401000-0x857FFF', tam=len(cv), bytes_distintos_de_la_imagen_inicial=dif, bytes_no_nulos_en_ejecucion=ncc))
    escribir_csv(SAL / 'volcados-resumen.csv', filas, ['modo', 'seccion', 'tam', 'bytes_distintos_de_la_imagen_inicial', 'bytes_no_nulos_en_ejecucion'])
    for r in filas: print(r)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('cmd', choices=['pe', 'codigo', 'escaneo', 'datos', 'volcados'])
    ap.add_argument('--std', default=STD); ap.add_argument('--orig', default=ORIG)
    ap.add_argument('--pdb', default=None); ap.add_argument('--volcado-dir', default=r'D:\Original\_comparar\volcados')
    a = ap.parse_args()
    if not a.pdb:
        a.pdb = os.path.splitext(a.std)[0] + '.pdb'
    globals()['cmd_' + a.cmd](a)


if __name__ == '__main__':
    main()
