#!/usr/bin/env python3
"""Auditoria de TAMANOS de los objetos propios DatosExe (source/game_sa/DatosExe/*.h: `extern T nombre; // 0xDIR`).

Clase de fallo que busca: un objeto definido con MENOS bytes que el dato real del EXE (el codigo lo usa como array o
estructura), de modo que en el mapa de enlace del independiente se solapa con los objetos vecinos y se pisan
(vehiculos negros: ms_lightsOn/ms_currentCol de 1 byte usados como uint8[4], m_sfLogLookup de 4 bytes usado como float[50][2]).
Documento: docs/DatosExe/auditoria-tamanos.md.

Ordenes:
  lint      Sin datos externos (apto para CI): (1) todo reinterpret_cast / cast de C sobre un objeto DatosExe debe pasar por
            DATOSEXE_VISTA (DatosExe/Vista.h: comprueba sizeof en compilacion); (2) VerifTamanos.cpp coincide con
            docs/DatosExe/tamanos-minimos.csv; (3) cada objeto de la tabla existe en las cabeceras.  Sale con 1 si falla.
  generar   Regenera source/game_sa/DatosExe/VerifTamanos.cpp (static_assert(sizeof(obj) >= minimo)) desde tamanos-minimos.csv.
  medir     Medicion completa con los artefactos de compilacion (si faltan, avisa y sale con 0 salvo --exigir):
              --pdb/--exe   PDB y EXE del independiente (tamano compilado de cada objeto; dbghelp, solo lectura)
              --obj-dir     .obj del ASI Debug (build/source/gta_reversed.dir/Debug): relocaciones COFF
              --map         .map del independiente (vecinos reales de los objetos insuficientes)
              --pdb-orig    gta_sa.pdb original (solo simbolos publicos: nombre y direccion, SIN tamanos)
            Escribe docs/DatosExe/tamanos-objetos.csv y tamanos-minimos.csv y sale con 1 si hay objetos insuficientes.
Evidencias de tamano minimo real de un objeto (todas medidas, ninguna estimada):
  * VISTA: variables de referencia/puntero estaticas (`static inline auto& x = reinterpret_cast<T&>(obj)`) cuyo valor inicial
    es el objeto: el tipo T de la referencia sale del nombre decorado (undname) y su tamano de la cadena de tipos o del PDB.
  * RELOC: relocaciones COFF DIR32 con sumando >= tamano del objeto (acceso con desplazamiento constante fuera del objeto).
  * TIPO: tamano del tipo de la StaticRef original (docs/DatosExe/objetos.csv con tam_origen=tipo).
Evidencias solo informativas (SOSPECHOSO, no fallan): objeto escalar pequeno que el EXE indexa con registro y que el C++ usa;
huecos del PDB original; accesos del EXE dentro del simbolo del PDB original pero fuera del objeto.
"""
import argparse, bisect, collections, csv, glob, os, re, struct, sys

RAIZ = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))
DX = os.path.join(RAIZ, 'source', 'game_sa', 'DatosExe')
DOCS = os.path.join(RAIZ, 'docs', 'DatosExe')
CSV_MIN = os.path.join(DOCS, 'tamanos-minimos.csv')
CSV_OBJ = os.path.join(DOCS, 'tamanos-objetos.csv')
VERIF = os.path.join(DX, 'VerifTamanos.cpp')
DECL = re.compile(r'^\s*extern\s+(?:const\s+)?(?:alignas\(\d+\)\s+)?(.+?)\s+(\w+)((?:\s*\[[^\]]*\])*)\s*;\s*//\s*0x([0-9A-Fa-f]{6,8})')
EXCLUIR_CABECERAS = ('Verif.h', 'StaticInit.h')


# ---------------------------------------------------------------- declaraciones
def declaraciones():
    """{nombre completo DatosExe::ns::obj: (va, cabecera, tipo)} de las cabeceras propias."""
    res = {}
    for h in sorted(glob.glob(os.path.join(DX, '*.h'))):
        base = os.path.basename(h)
        if base in EXCLUIR_CABECERAS:
            continue
        ns = None
        for linea in open(h, encoding='utf-8', errors='replace'):
            m = re.match(r'\s*namespace\s+(DatosExe::\w+)\s*\{', linea)
            if m:
                ns = m.group(1)
                continue
            m = DECL.match(linea)
            if m and ns:
                res[ns + '::' + m.group(2)] = (int(m.group(4), 16), base, m.group(1) + (m.group(3) or '').replace(' ', ''))
    return res


# ---------------------------------------------------------------- lint
RAW1 = re.compile(r'reinterpret_cast\s*<[^;{}]*?>\s*\(\s*&?\s*DatosExe::\w+::\w+')
RAW2 = re.compile(r'\(\s*[A-Za-z_][\w:\s]*\*+\s*\)\s*\(?\s*&\s*DatosExe::\w+::\w+')


def fuentes():
    for p in glob.glob(os.path.join(RAIZ, 'source', '**', '*'), recursive=True):
        if p.endswith(('.h', '.cpp', '.inc', '.hpp')) and os.path.basename(os.path.dirname(p)) != 'DatosExe':
            yield p


def sin_comentarios(s):
    s = re.sub(r'/\*.*?\*/', lambda m: re.sub(r'[^\n]', ' ', m.group(0)), s, flags=re.S)
    return re.sub(r'//[^\n]*', '', s)


def cabeceras_de(entradas, decl):
    return sorted({decl[n][1] for n in entradas if n in decl})


def leer_minimos():
    if not os.path.exists(CSV_MIN):
        return []
    return list(csv.DictReader(open(CSV_MIN, encoding='utf-8')))


def texto_verif(minimos, decl):
    out = ['// GENERADO por contrib/auditoria_datos/tamanos_objetos.py (orden generar) desde docs/DatosExe/tamanos-minimos.csv; no editar a mano.',
           '// Cada static_assert fija el tamano MINIMO real (medido) de un objeto DatosExe: si alguien lo reduce, falla la compilacion',
           '// (un objeto mas pequeno que el dato del EXE se solapa con sus vecinos en el enlace del independiente).',
           '#include "StdInc.h"']
    for h in cabeceras_de([m['objeto'] for m in minimos], decl):
        out.append('#include "%s"' % h)
    out.append('')
    for m in minimos:
        if m['objeto'] in decl and '[]' not in decl[m['objeto']][2]:   # los arrays sin cota (cadenas) no admiten sizeof
            out.append('static_assert(sizeof(%s) >= %s, "%s: %s");' % (m['objeto'], m['minimo'], m['va'], m['fuente']))
    return '\n'.join(out) + '\n'


def orden_lint():
    decl = declaraciones()
    fallos = []
    for p in fuentes():
        s = sin_comentarios(open(p, encoding='utf-8', errors='replace').read())
        for rx in (RAW1, RAW2):
            for m in rx.finditer(s):
                if 'uintptr_t' in m.group(0):
                    continue
                linea = s.count('\n', 0, m.start()) + 1
                fallos.append('%s:%d: cast sin DATOSEXE_VISTA sobre un objeto DatosExe: %s' % (os.path.relpath(p, RAIZ), linea, m.group(0)[:100]))
    minimos = leer_minimos()
    for m in minimos:
        if m['objeto'] not in decl:
            fallos.append('tamanos-minimos.csv: el objeto %s no esta declarado en las cabeceras' % m['objeto'])
    esperado = texto_verif(minimos, decl)
    actual = open(VERIF, encoding='utf-8').read() if os.path.exists(VERIF) else ''
    if actual.replace('\r\n', '\n') != esperado:
        fallos.append('VerifTamanos.cpp no coincide con tamanos-minimos.csv: ejecutar `python contrib/auditoria_datos/tamanos_objetos.py generar`')
    for f in fallos:
        print('FALLO', f)
    print('lint: %d objetos declarados, %d minimos verificados con static_assert, %d fallos' % (len(decl), len(minimos), len(fallos)))
    return 1 if fallos else 0


def orden_generar():
    decl = declaraciones()
    minimos = leer_minimos()
    open(VERIF, 'w', encoding='utf-8', newline='\n').write(texto_verif(minimos, decl))
    print('VerifTamanos.cpp:', len(minimos), 'static_assert')
    return 0


# ---------------------------------------------------------------- medicion: dbghelp
def abrir_dbghelp(exe, base=0x400000):
    import ctypes
    from ctypes import wintypes
    ns = {}
    MAXN = 2000

    class SYMBOL_INFO(ctypes.Structure):
        _fields_ = [('SizeOfStruct', wintypes.ULONG), ('TypeIndex', wintypes.ULONG), ('Reserved', ctypes.c_uint64 * 2),
                    ('Index', wintypes.ULONG), ('Size', wintypes.ULONG), ('ModBase', ctypes.c_uint64), ('Flags', wintypes.ULONG),
                    ('Value', ctypes.c_uint64), ('Address', ctypes.c_uint64), ('Register', wintypes.ULONG), ('Scope', wintypes.ULONG),
                    ('Tag', wintypes.ULONG), ('NameLen', wintypes.ULONG), ('MaxNameLen', wintypes.ULONG), ('Name', ctypes.c_char * MAXN)]
    CB = ctypes.WINFUNCTYPE(wintypes.BOOL, ctypes.POINTER(SYMBOL_INFO), wintypes.ULONG, ctypes.c_void_p)
    dh = ctypes.WinDLL('dbghelp.dll')
    dh.SymLoadModuleExW.restype = ctypes.c_uint64
    dh.SymLoadModuleExW.argtypes = [wintypes.HANDLE, wintypes.HANDLE, wintypes.LPCWSTR, wintypes.LPCWSTR, ctypes.c_uint64, wintypes.DWORD, ctypes.c_void_p, wintypes.DWORD]
    dh.SymEnumSymbols.argtypes = [wintypes.HANDLE, ctypes.c_uint64, ctypes.c_char_p, CB, ctypes.c_void_p]
    dh.SymEnumTypesByName.argtypes = [wintypes.HANDLE, ctypes.c_uint64, ctypes.c_char_p, CB, ctypes.c_void_p]
    dh.UnDecorateSymbolName.argtypes = [ctypes.c_char_p, ctypes.c_char_p, wintypes.DWORD, wintypes.DWORD]
    h = ctypes.c_void_p(0x5151)
    dh.SymSetOptions(0x2 | 0x10000 | 0x100000)   # UNDNAME + NO_PROMPTS (sin diferir: los tipos hacen falta)
    if not dh.SymInitializeW(h, None, False):
        raise OSError('SymInitialize')
    mod = dh.SymLoadModuleExW(h, None, exe, None, base, 0x4000000, None, 0)
    if not mod:
        raise OSError('SymLoadModuleEx')
    cache = {}

    def simbolos(mascara):
        res = {}

        def cb(si, tam, ctx):
            s = si.contents
            res[s.Name[:s.NameLen].decode('latin1')] = (s.Address - base, tam or s.Size)
            return True
        f = CB(cb)
        dh.SymEnumSymbols(h, mod, mascara.encode(), f, None)
        return res

    def tam_tipo(nombre):
        if nombre in cache:
            return cache[nombre]
        out = []

        def cb(si, tam, ctx):
            s = si.contents
            if s.Name[:s.NameLen].decode('latin1') == nombre:
                out.append(tam or s.Size)
            return True
        f = CB(cb)
        dh.SymEnumTypesByName(h, mod, nombre.encode(), f, None)
        cache[nombre] = max(out) if out else None
        return cache[nombre]

    def undname(m):
        b = ctypes.create_string_buffer(2048)
        return b.value.decode('latin1') if dh.UnDecorateSymbolName(m.encode(), b, 2048, 0) else None
    return simbolos, tam_tipo, undname


PRIM = {'char': 1, 'signed char': 1, 'unsigned char': 1, 'bool': 1, 'short': 2, 'unsigned short': 2, 'int': 4, 'unsigned int': 4,
        'long': 4, 'unsigned long': 4, 'float': 4, 'double': 8, '__int64': 8, 'unsigned __int64': 8, 'long double': 8, 'wchar_t': 2}


def args_plantilla(s):
    out, d, cur = [], 0, ''
    for ch in s:
        d += (ch == '<') - (ch == '>')
        if ch == ',' and d == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    out.append(cur.strip())
    return out


def tam_texto(t, tam_tipo):
    t = re.sub(r'\b(const|volatile)\b', '', t).strip()
    t = re.sub(r'^(class|struct|enum|union)\s+', '', t)
    if t.endswith('*') or t.endswith('^') or ('(' in t and '*' in t):
        return 4
    if t in PRIM:
        return PRIM[t]
    m = re.match(r'^std::array<(.*)>$', t)
    if m:
        a = args_plantilla(m.group(1))
        e = tam_texto(a[0], tam_tipo)
        try:
            n = int(a[1])
        except ValueError:
            return None
        return None if e is None else e * n
    return tam_tipo(re.sub(r'\s+', ' ', t))


def tam_vista(mangled, undname, tam_tipo):
    """tamano del tipo apuntado por una variable de referencia (nombre decorado) o None si no se puede resolver."""
    d = undname(mangled)
    if not d:
        return None
    d = re.sub(r'^(?:(?:public|private|protected): )?(?:static )?', '', d)
    m = re.match(r'^(.*?) \(& .*?\)((?:\[\d+\])+)$', d)
    if m:
        base = tam_texto(m.group(1), tam_tipo)
        if base is None:
            return None
        n = 1
        for x in re.findall(r'\[(\d+)\]', m.group(2)):
            n *= int(x)
        return base * n
    m = re.match(r'^(.*?) ?& ', d)
    return tam_texto(m.group(1), tam_tipo) if m else None


# ---------------------------------------------------------------- medicion: COFF
def escanear_obj(directorio, undname, tam_tipo):
    """devuelve (reloc, vistas): reloc[objeto]=max sumando de relocaciones en codigo/datos; vistas[objeto]=[(tam de la vista, variable)]"""
    reloc, vistas = collections.defaultdict(int), collections.defaultdict(dict)
    cache_vista = {}
    for p in sorted(glob.glob(os.path.join(directorio, '*.obj'))):
        d = open(p, 'rb').read()
        if d[:4] == b'\0\0\xff\xff':
            nsec, symptr, nsym = struct.unpack_from('<III', d, 44)
            hdr, tsym = 56, 20
        else:
            nsec, _, symptr, nsym = struct.unpack_from('<HIII', d, 2)
            hdr, tsym = 20, 18
        strtab = symptr + nsym * tsym

        def nombre(o):
            if d[o:o + 4] == b'\0\0\0\0':
                so = struct.unpack_from('<I', d, o + 4)[0]
                return d[strtab + so:d.index(b'\0', strtab + so)].decode('latin1')
            return d[o:o + 8].rstrip(b'\0').decode('latin1')
        secsyms = collections.defaultdict(list)
        i = 0
        while i < nsym:
            o = symptr + i * tsym
            val, sec, ty, cls, naux = struct.unpack_from('<IiHBB', d, o + 8) if tsym == 20 else (struct.unpack_from('<IhHBB', d, o + 8))
            if sec > 0 and cls in (2, 3) and naux == 0:
                n = nombre(o)
                if not n.startswith('.'):
                    secsyms[sec].append((val, n))
            i += 1 + naux
        cn = {}
        for s in range(nsec):
            nm, vs, va, rs, rp, lp, ll, nr, nl, ch = struct.unpack_from('<8sIIIIIIHHI', d, hdr + 40 * s)
            if nr == 0 or rp == 0 or ch & 0x1000000:
                continue
            esdatos = nm.rstrip(b'\0').decode('latin1').startswith(('.data', '.rdata'))
            for r in range(nr):
                v, si, ty = struct.unpack_from('<IIH', d, lp + 10 * r)
                if ty not in (6, 0x14):
                    continue
                if si not in cn:
                    cn[si] = nombre(symptr + si * tsym)
                n = cn[si]
                if 'DatosExe' not in n:
                    continue
                m = re.match(r'\?(\w+)@(\w+)@DatosExe@@3', n)
                if not m:
                    continue
                obj = 'DatosExe::%s::%s' % (m.group(2), m.group(1))
                add = struct.unpack_from('<i', d, rp + v)[0] + (4 if ty == 0x14 else 0)
                if add > reloc[obj]:
                    reloc[obj] = add
                if esdatos and ty == 6:
                    var = next((x for vv, x in secsyms.get(s + 1, []) if vv == v), None)
                    if var and var.startswith('?') and not var.startswith('?.'):
                        k = var
                        if k not in cache_vista:
                            cache_vista[k] = tam_vista(var, undname, tam_tipo)
                        if cache_vista[k] is not None:
                            vistas[obj][var] = cache_vista[k] + add
    return reloc, vistas


# ---------------------------------------------------------------- medicion: PDB original (solo publicos)
def publicos_pdb_original(ruta):
    d = open(ruta, 'rb').read()
    bs, _, _, dirsize, _, bmap = struct.unpack_from('<IIIIII', d, 0x20)
    nbm = (dirsize + bs - 1) // bs
    blocks = struct.unpack_from('<%dI' % nbm, d, bmap * bs)
    dd = b''.join(d[b * bs:(b + 1) * bs] for b in blocks)[:dirsize]
    ns = struct.unpack_from('<I', dd, 0)[0]
    sizes = struct.unpack_from('<%dI' % ns, dd, 4)
    off, st = 4 + 4 * ns, []
    for s in sizes:
        n = 0 if s == 0xFFFFFFFF else (s + bs - 1) // bs
        st.append((s, struct.unpack_from('<%dI' % n, dd, off)))
        off += 4 * n
    # el flujo de registros de simbolos es el 8 en este PDB (gta_sa.pdb: S_PUB32 con segmento:desplazamiento)
    x = b''.join(d[b * bs:(b + 1) * bs] for b in st[8][1])[:st[8][0]]
    base = {3: 0x858000, 4: 0x8A4000}   # .rdata y .data/.bss del EXE 1.0 (los segmentos 3 y 4 de este PDB)
    res = {}
    pos = 0
    while pos + 4 <= len(x):
        ln, ty = struct.unpack_from('<HH', x, pos)
        if ty == 0x110e:
            _, o, seg = struct.unpack_from('<IIH', x, pos + 4)
            if seg in base:
                res.setdefault(base[seg] + o, x[pos + 14:pos + 2 + ln].split(b'\0')[0].decode('latin1'))
        pos += 2 + ln
    return res


# ---------------------------------------------------------------- medicion: mapa de enlace
def leer_map(ruta):
    """lista ordenada (seccion, desplazamiento, nombre decorado) de los simbolos DatosExe del .map del independiente"""
    pat = re.compile(r'^\s*([0-9a-f]{4}):([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})\s')
    res = []
    for linea in open(ruta, errors='replace'):
        m = pat.match(linea)
        if m and 'DatosExe' in m.group(3):
            res.append((m.group(1), int(m.group(2), 16), m.group(3)))
    res.sort()
    return res


def orden_medir(a):
    faltan = [x for x in (a.pdb, a.exe) if not x or not os.path.exists(x)]
    if faltan or not a.obj_dir or not os.path.isdir(a.obj_dir):
        print('SKIP medir: faltan artefactos de compilacion (--pdb/--exe del independiente y --obj-dir del ASI Debug).')
        return 1 if a.exigir else 0
    decl = declaraciones()
    simbolos, tam_tipo, undname = abrir_dbghelp(a.exe)
    sy = simbolos('DatosExe::*')
    tam = {n: sy[n][1] for n in decl if n in sy}
    print('objetos declarados %d, con tamano compilado %d' % (len(decl), len(tam)))
    reloc, vistas = escanear_obj(a.obj_dir, undname, tam_tipo)
    original = publicos_pdb_original(a.pdb_orig) if a.pdb_orig and os.path.exists(a.pdb_orig) else {}
    pva = sorted(original)
    objs = {}
    ruta_objs = os.path.join(DOCS, 'objetos.csv')
    if os.path.exists(ruta_objs):
        objs = {int(r['va'], 16): r for r in csv.DictReader(open(ruta_objs, encoding='utf-8'))}
    ova = sorted(objs)
    usos = collections.Counter()
    nombres_cortos = {n.split('::', 1)[1]: n for n in decl}
    rx = re.compile(r'DatosExe::(\w+::\w+)')
    for p in fuentes():
        for m in rx.finditer(sin_comentarios(open(p, encoding='utf-8', errors='replace').read())):
            usos[m.group(1)] += 1
    filas, minimos = [], []
    cnt = collections.Counter()
    insuf = []
    for n in sorted(decl, key=lambda k: decl[k][0]):
        va, hdr, tipo = decl[n]
        t = tam.get(n)
        if t is None:
            cnt['sin_tamano_compilado'] += 1
            continue
        cnt['revisados'] += 1
        r = objs.get(va)
        vmax = max(vistas[n].values()) if n in vistas else 0
        rmax = reloc.get(n, 0)
        tipo_csv = int(r['tam']) if r and r['tam_origen'] == 'tipo' else 0
        pubn = original.get(va, '')
        hueco_pdb = ''
        if pubn:
            i = bisect.bisect_right(pva, va)
            hueco_pdb = (pva[i] - va) if i < len(pva) else ''
            cnt['con_simbolo_pdb_original'] += 1
        indexado = bool(r and r['tabla_indexada'] == '1')
        minimo_real = max(vmax, tipo_csv)
        estado = 'OK'
        fuente = ''
        if vmax > t:
            estado, fuente = 'INSUFICIENTE', 'vista de %d bytes (%s)' % (vmax, sorted(vistas[n])[0])
        elif rmax >= t and rmax > 0:
            estado, fuente = 'INSUFICIENTE', 'relocacion con sumando %d' % rmax
        elif indexado and t <= 8 and usos.get(n.split('::', 1)[1], 0) > 0 and (hueco_pdb == '' or hueco_pdb > t):
            estado = 'SOSPECHOSO'
        if estado == 'INSUFICIENTE':
            insuf.append((n, va, t, max(vmax, rmax)))
        cnt[estado] += 1
        if vmax:
            cnt['con_vista'] += 1
        if rmax:
            cnt['con_reloc'] += 1
        if minimo_real and '[]' not in tipo:
            fu = ('vista %d' % vmax) if vmax >= tipo_csv else ('tipo %d' % tipo_csv)
            minimos.append({'va': '0x%08X' % va, 'objeto': n, 'minimo': str(minimo_real), 'fuente': fu})
        filas.append({'va': '0x%08X' % va, 'objeto': n, 'cabecera': hdr, 'tam_declarado': t, 'vista_max': vmax or '', 'reloc_max': rmax or '',
                      'tipo_csv': tipo_csv or '', 'pdb_original': pubn, 'hueco_pdb': hueco_pdb, 'indexado_exe': int(indexado),
                      'usos_cpp': usos.get(n.split('::', 1)[1], 0), 'estado': estado})
    with open(CSV_OBJ, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=list(filas[0]))
        w.writeheader()
        w.writerows(filas)
    minimos.sort(key=lambda m: (m['va'], m['objeto']))
    with open(CSV_MIN, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=['va', 'objeto', 'minimo', 'fuente'])
        w.writeheader()
        w.writerows(minimos)
    # vecinos reales en el mapa de enlace del independiente para cada insuficiente
    if a.map and os.path.exists(a.map) and insuf:
        mp = leer_map(a.map)
        for n, va, t, ext in insuf:
            m = re.match(r'DatosExe::(\w+)::(\w+)', n)
            clave = '?%s@%s@DatosExe@@' % (m.group(2), m.group(1))
            for i, (sec, off, nm) in enumerate(mp):
                if nm.startswith(clave):
                    vict = [x[2] for x in mp[i + 1:i + 40] if x[0] == sec and off + t <= x[1] < off + ext]
                    print('  vecinos pisados por %s (%d -> %d bytes): %s' % (n, t, ext, ', '.join(vict) or 'ninguno'))
                    break
    print('resumen:', dict(cnt))
    print('tamanos-minimos.csv: %d objetos con minimo medido; ejecute `generar` para actualizar VerifTamanos.cpp' % len(minimos))
    for n, va, t, ext in insuf:
        print('INSUFICIENTE %s 0x%08X declarado %d, real >= %d' % (n, va, t, ext))
    return 1 if insuf else 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('orden', choices=['lint', 'generar', 'medir'])
    ap.add_argument('--pdb', default=os.path.join(RAIZ, 'reversing', 'FnFase8', 'salida', 'gta_sa_standalone.pdb'))
    ap.add_argument('--exe', default=os.path.join(RAIZ, 'reversing', 'FnFase8', 'salida', 'gta_sa_standalone.exe'))
    ap.add_argument('--obj-dir', default=os.path.join(RAIZ, 'build', 'source', 'gta_reversed.dir', 'Debug'))
    ap.add_argument('--map', default=os.path.join(RAIZ, 'reversing', 'FnFase8', 'salida', 'gta_sa_standalone.map'))
    ap.add_argument('--pdb-orig', default=r'D:\Grand Theft Auto San Andreas\gta_sa.pdb')
    ap.add_argument('--exigir', action='store_true', help='medir: fallar (1) si faltan los artefactos en vez de omitir')
    a = ap.parse_args()
    return {'lint': orden_lint, 'generar': orden_generar, 'medir': lambda: orden_medir(a)}[a.orden]()


if __name__ == '__main__':
    sys.exit(main())
