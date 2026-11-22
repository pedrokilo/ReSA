"""Medicion de los DATOS del EXE (.rdata, .data, .bss, IAT) que el proyecto sigue leyendo o escribiendo por direccion fija.

Dos fuentes, ambas estaticas (no se ejecuta el juego):

1. La DLL: cada literal hexadecimal del codigo C++ (sin comentarios) que cae en 0x858000-0xCB0FFF (.rdata, .data, .bss,
   _TEXT_HA, _rwdseg, .rsrc) es una referencia a un dato del EXE. Se clasifica por forma (StaticRef, cast, ayudante, vtable de
   hook, literal) y por tipo declarado cuando se conoce.
2. El EXE: se desensambla (capstone) cada funcion de docs/MemoryMap/binary_function_ranges.csv y se recogen los operandos de
   memoria absolutos e inmediatos que apuntan a esos datos. Una funcion es «viva» si no esta reemplazada por C++ (funciones.csv
   del juego y de RenderWare; todo lo demas, CRT incluido, es vivo). Un dato que lee una funcion viva sigue necesitando el EXE.

Salida en docs/DatosExe/: referencias-dll.csv, objetos.csv, por-seccion.csv, por-clase.csv, resumen.json.

Uso: python contrib/datos_exe.py [--exe RUTA] [--fuente source] [--salida docs/DatosExe]
Cotas: los tamanos de los objetos salen del tipo declarado o del ancho de acceso del EXE; si ninguno existe se estima por el
hueco hasta el siguiente objeto (campo tam_origen lo dice). No es una certificacion de independencia.
"""
import argparse
import csv
import json
import os
import re
import struct
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from pe_inventory import PE  # noqa: E402

LO, HI = 0x858000, 0xCB1000
SEC = [('.rdata', 0x858000, 0x8A4000), ('.data', 0x8A4000, 0x8E4000), ('.bss', 0x8E4000, 0xC9E000),
       ('_TEXT_HA', 0xC9E000, 0xCAF000), ('_rwdseg', 0xCAF000, 0xCB0000), ('.rsrc', 0xCB0000, 0xCB1000)]
GAME_END, RW_END = 0x750000, 0x820000
PRIM = {'uint8': 1, 'int8': 1, 'bool': 1, 'char': 1, 'uchar': 1, 'uint8_t': 1, 'int8_t': 1, 'BYTE': 1,
        'int16': 2, 'uint16': 2, 'short': 2, 'int16_t': 2, 'uint16_t': 2, 'WORD': 2, 'wchar_t': 2,
        'int32': 4, 'uint32': 4, 'float': 4, 'int': 4, 'unsigned': 4, 'long': 4, 'DWORD': 4, 'int32_t': 4, 'uint32_t': 4,
        'size_t': 4, 'RwRGBA': 4, 'CRGBA': 4, 'RwInt32': 4, 'RwReal': 4, 'RwUInt32': 4, 'RwBool': 4, 'eModelID': 4,
        'double': 8, 'int64': 8, 'uint64': 8, 'int64_t': 8, 'uint64_t': 8, 'CVector2D': 8, 'RwV2d': 8,
        'CVector': 12, 'RwV3d': 12, 'RwRaster': 0, 'CRect': 16, 'CMatrix': 72, 'RwMatrix': 64}
HELPERS = ('LoadWord', 'StoreWord', 'f(', 'constant(', 'CAM_LF', 'name(', 'GaMessage', 'LoadFloat', 'LoadDword')


def definidas_datosexe(excluir_prefijo=None):
    """Direcciones que ya define algun .cpp de source/game_sa/DatosExe (DataBss*/DataInicializada*), salvo el lote que se esta generando."""
    out = set()
    d = Path(__file__).resolve().parent.parent / 'source/game_sa/DatosExe'
    for p in d.glob('*.cpp'):
        if not p.name.startswith(('DataBss', 'DataInicializada')) or (excluir_prefijo and p.name.startswith(excluir_prefijo)):
            continue
        for ln in p.read_text(encoding='utf-8', errors='replace').split(chr(10)):
            m = re.search(r'// 0x([0-9A-F]{8})\s*$', ln)
            if m and not ln.startswith(('extern', 'static_assert', '#', 'template', 'using')):
                out.add(int(m.group(1), 16))
    return out


def seccion(va):
    for n, a, b in SEC:
        if a <= va < b:
            return n
    return None


def region_codigo(va):
    return 'juego' if va < GAME_END else ('renderware' if va < RW_END else 'crt')


def tam_tipo(t):
    """Tamano en bytes de un tipo C++ simple, o None."""
    t = t.strip()
    t = re.sub(r'\b(const|volatile|struct|class|enum|unsigned|signed)\b\s*', lambda m: 'unsigned ' if m.group(1) == 'unsigned' else '', t).strip()
    t = t.replace('unsigned ', 'uint') if t.startswith('unsigned ') and t.split()[-1] in ('char', 'short', 'int') else t
    if t.endswith('*') or t.endswith('&') and False:
        return 4
    m = re.fullmatch(r'(.+?)\s*\[\s*(\d+)\s*\]', t)
    if m:
        b = tam_tipo(m.group(1))
        return b * int(m.group(2)) if b else None
    m = re.fullmatch(r'std::array<\s*(.+?)\s*,\s*(\d+)u?\s*>', t)
    if m:
        b = tam_tipo(m.group(1))
        return b * int(m.group(2)) if b else None
    t = t.replace('uintchar', 'uint8').replace('uintshort', 'uint16').replace('uintint', 'uint32')
    return PRIM.get(t) or tam_clase(t)


_CLASES = None


def tam_clase(nombre):
    """Tamano de una clase/estructura del proyecto segun VALIDATE_SIZE(Clase, N) o static_assert(sizeof(Clase) == N) en source/."""
    global _CLASES
    if _CLASES is None:
        _CLASES = {}
        pats = (re.compile(r'VALIDATE_SIZE\(\s*([\w:]+)\s*,\s*(0[xX][0-9A-Fa-f]+|\d+)\s*\)'),
                re.compile(r'static_assert\(\s*sizeof\(\s*([\w:]+)\s*\)\s*==\s*(0[xX][0-9A-Fa-f]+|\d+)'))
        for p in (Path(__file__).resolve().parent.parent / 'source').rglob('*'):
            if p.suffix not in ('.h', '.hpp', '.cpp'):
                continue
            txt = p.read_text(encoding='utf-8', errors='replace')
            if 'VALIDATE_SIZE' not in txt and 'sizeof(' not in txt:
                continue
            for pt in pats:
                for m in pt.finditer(txt):
                    _CLASES.setdefault(m.group(1), int(m.group(2), 0))
    return _CLASES.get(nombre.strip())


def cubierto_por_vecino(refs):
    """Devuelve f(va) -> direccion del objeto referenciado (de tamano conocido) cuya extension cubre `va` sin empezar en `va`, o None.
    Detecta los campos de una estructura grande (p. ej. TheCamera 0xB6F028, 0xD78 bytes) que el C++ tambien referencia suelta."""
    import bisect
    ext = {}
    for r in refs:
        n = r.get('tam')
        if n is None and r.get('tipo'):
            n = tam_tipo(r['tipo'])
        if n:
            ext[r['va']] = max(ext.get(r['va'], 0), n)
    claves = sorted(ext)

    def f(va):
        i = bisect.bisect_left(claves, va) - 1
        while i >= 0 and va - claves[i] < 0x20000:
            a = claves[i]
            if a + ext[a] > va:
                return a
            i -= 1
        return None
    return f


def quitar_comentarios(texto):
    """Quita // y /* */ conservando los saltos de linea (para numerar bien)."""
    out, i, n = [], 0, len(texto)
    while i < n:
        c = texto[i]
        if c == '"' or c == "'":
            j = i + 1
            while j < n and texto[j] != c and texto[j] != '\n':
                j += 2 if texto[j] == '\\' else 1
            out.append(texto[i:j + 1]); i = j + 1
        elif texto.startswith('//', i):
            j = texto.find('\n', i); j = n if j < 0 else j
            i = j
        elif texto.startswith('/*', i):
            j = texto.find('*/', i + 2); j = n - 2 if j < 0 else j
            out.append('\n' * texto.count('\n', i, j + 2)); i = j + 2
        else:
            out.append(c); i += 1
    return ''.join(out)


def blanquear_comentarios(texto):
    """Igual que quitar_comentarios pero conserva la longitud (los comentarios pasan a espacios) para reescribir por posicion."""
    out, i, n = [], 0, len(texto)
    while i < n:
        c = texto[i]
        if c == '"' or c == "'":
            j = i + 1
            while j < n and texto[j] != c and texto[j] != chr(10):
                j += 2 if texto[j] == chr(92) else 1
            out.append(texto[i:j + 1]); i = j + 1
        elif texto.startswith('//', i):
            j = texto.find(chr(10), i); j = n if j < 0 else j
            out.append(' ' * (j - i)); i = j
        elif texto.startswith('/*', i):
            j = texto.find('*/', i + 2); j = n - 2 if j < 0 else j
            out.append(''.join(chr(10) if ch == chr(10) else ' ' for ch in texto[i:j + 2])); i = j + 2
        else:
            out.append(c); i += 1
    return ''.join(out)


HEX = re.compile(r'(?<![0-9A-Za-z_])0[xX]([0-9A-Fa-f]{6,8})(?![0-9A-Za-z_])')
RE_STATIC = re.compile(r'StaticRef\s*<\s*((?:[\w:]+\s+)*[\w:]+(?:\s*<[^<>()]*>)?(?:\s*\*)*(?:\s*\[\s*\d+\s*\])?)\s*>\s*\(\s*$')
RE_CAST = re.compile(r'(?:\*\s*)?\(\s*(?:volatile\s+|const\s+)*([A-Za-z_][\w:<>, ]*?)\s*\*\s*\)\s*(?:\(\s*(?:uintptr_t|uint32|unsigned)\s*\)\s*)?\(?\s*$')
RE_RCAST = re.compile(r'(?:reinterpret_cast|bit_cast)\s*<\s*(?:const\s+|volatile\s+)*([A-Za-z_][\w:<>, ]*?)\s*\*\s*>\s*\(\s*\(?\s*(?:uintptr_t|uint32)?\s*\)?\s*$')


def escanear_fuente(raiz):
    refs = []
    for dp, _, fs in os.walk(raiz):
        for f in sorted(fs):
            if not f.endswith(('.h', '.cpp', '.hpp', '.inc', '.c')):
                continue
            p = os.path.join(dp, f)
            rel = p.replace('\\', '/')
            if '/game_sa/DatosExe/' in rel:
                continue      # las definiciones propias de DatosExe no son una dependencia del EXE (sus direcciones solo estan en comentarios y mensajes)
            try:
                txt = quitar_comentarios(open(p, encoding='utf-8', errors='replace').read())
            except OSError:
                continue
            for ln, linea in enumerate(txt.split('\n'), 1):
                if '0x' not in linea and '0X' not in linea:
                    continue
                for m in HEX.finditer(linea):
                    va = int(m.group(1), 16)
                    if not (LO <= va < HI):
                        continue
                    pre = linea[:m.start()]
                    forma, tipo = 'literal', ''
                    m1 = RE_STATIC.search(pre)
                    m2 = RE_CAST.search(pre)
                    m3 = RE_RCAST.search(pre)
                    if m1:
                        forma, tipo = 'StaticRef', m1.group(1)
                    elif m3:
                        forma, tipo = 'reinterpret_cast', m3.group(1)
                    elif m2 and not pre.rstrip().endswith('sizeof'):
                        forma, tipo = 'cast', m2.group(1)
                    elif any(h in pre[-24:] for h in HELPERS):
                        forma = 'ayudante'
                    elif re.search(r'RH_\w+\s*\(', linea):
                        forma = 'hook_vtable'
                    elif re.search(r'\bpatch|\bmemcpy|\bmemset', linea):
                        forma = 'parche'
                    refs.append(dict(va=va, archivo=rel, linea=ln, forma=forma, tipo=tipo.strip(),
                                     tam=tam_tipo(tipo) if tipo else None))
    return refs


def cargar_funciones():
    base = Path(__file__).resolve().parent.parent / 'docs'
    reemplazadas = set()
    for fn in ('JuegoExe/funciones.csv', 'RenderWareExe/funciones.csv'):
        with (base / fn).open(encoding='utf-8-sig', newline='') as s:
            for r in csv.DictReader(s):
                if r['estado'] == 'reemplazada':
                    reemplazadas.add(int(r['va'], 16))
    # docs/CodigoVivoExe: las funciones con decision DESCARTAR (funclets de excepciones, D3DX9, protector, excluidas del juego, CRT sin
    # alcance) y las CUBIERTA (ya enganchadas) no cuentan como vivas: no se ejecutaran en el EXE propio.
    cv = base / 'CodigoVivoExe/funciones-vivas.csv'
    if cv.exists():
        with cv.open(encoding='utf-8-sig', newline='') as s:
            for r in csv.DictReader(s):
                if r['decision'] in ('DESCARTAR', 'CUBIERTA'):
                    reemplazadas.add(int(r['va'], 16))
    rangos = []
    with (base / 'MemoryMap/binary_function_ranges.csv').open(encoding='utf-8-sig', newline='') as s:
        for r in csv.DictReader(s):
            rangos.append((int(r['function_va'], 16), int(r['start_va'], 16), int(r['end_va'], 16)))
    # Codigo sin funcion de Ghidra pero con referencia (docs/CodigoVivoExe/huecos.csv): bloques sueltos (cuerpos de switch, restos de
    # funciones partidas) que el escaneo por funcion no veia. Se tratan como vivos (no reemplazados) para ser conservadores: los datos
    # que tocan no se dan por libres. La biseccion de DE05 lo descubrio (SwitchDefaultAddress, 0xA43F54).
    rangos_funcion = len(rangos)
    hz = base / 'CodigoVivoExe/huecos.csv'
    # Con DATOSEXE_HUECOS=alcanzables solo cuentan los huecos que contrib/huecos_alcanzables.py da por ALCANZABLES (los de codigo
    # sin entrada, tablas de switches muertos y vtables sin constructor vivo no se ejecutan); por defecto, todos (conservador).
    muertos_h = set()
    if os.environ.get('DATOSEXE_HUECOS') == 'alcanzables':
        ha = base / 'DatosExe/huecos-alcanzables.csv'
        if ha.exists():
            with ha.open(encoding='utf-8', newline='') as s:
                muertos_h = {int(r['va'], 16) for r in csv.DictReader(s) if r['clase'].startswith('INALCANZABLE')}
    if hz.exists():
        with hz.open(encoding='utf-8-sig', newline='') as s:
            for r in csv.DictReader(s):
                if r['categoria'].startswith(('código con referencia', 'sin referencia directa', 'inicializador estático')):
                    if int(r['va'], 16) in muertos_h:
                        continue
                    rangos.append((int(r['va'], 16), int(r['va'], 16), int(r['fin'], 16) + 1))
    revivir_ganchos_inactivos(reemplazadas, rangos[:rangos_funcion])
    return reemplazadas, rangos


def revivir_ganchos_inactivos(reemplazadas, rangos):
    """Una funcion con RH_Scoped*Install(..., {.enabled = <no `true`>}) NO esta reemplazada por defecto: su cuerpo original se ejecuta
    (p. ej. las llamadas de RpAnimBlend 0x4D2B90/0x4D2E40/0x4D30A0/0x4D32D0/0x4D2E10 con DISABLE_CUSTOM_FRAME_UPDATE_FUNCTIONS). Se
    devuelven al codigo vivo, y con ellas las funciones que llaman y que solo estaban marcadas DESCARTAR/CUBIERTA (usercall como
    0x4D1680 o 0x4D1A50), hasta punto fijo. Lo descubrio el humo de la ronda DatosExe-B: gpAnimBlendClump (0xB4EA0C) en la DLL hacia
    caer 0x4D2B90 con ECX = 0."""
    raiz = Path(__file__).resolve().parent.parent / 'source'
    pat = re.compile(r'RH_Scoped\w*Install\s*\([^;]*?0[xX]([0-9A-Fa-f]{6,8})[^;]*?\{[^}]*\.enabled\s*=\s*([^}]*)\}', re.S)
    vivas = set()
    for p in raiz.rglob('*.cpp'):
        txt = quitar_comentarios(p.read_text(encoding='utf-8', errors='replace'))
        for m in pat.finditer(txt):
            if m.group(2).strip() != 'true':
                vivas.add(int(m.group(1), 16))
    if not vivas:
        return
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    por_fn = defaultdict(list)
    for f, a, b in rangos:
        por_fn[f].append((a, b))
    cola = [f for f in vivas if f in reemplazadas or f in por_fn]
    for f in vivas:
        reemplazadas.discard(f)
    vistos = set(vivas)
    while cola:
        f = cola.pop()
        for a, b in por_fn.get(f, []):
            off = pe.offset(a, 1)
            if off is None or b <= a:
                continue
            for ins in md.disasm(pe.data[off:off + (b - a)], a):
                if ins.mnemonic in ('call', 'jmp') and ins.operands and ins.operands[0].type == CS_OP_IMM:
                    d = ins.operands[0].imm & 0xFFFFFFFF
                    if d in por_fn and d not in vistos:
                        vistos.add(d)
                        if d in reemplazadas and d not in hooks_activos():
                            reemplazadas.discard(d)
                            cola.append(d)
    print(f'ganchos inactivos devueltos al codigo vivo: {len(vistos)} funciones ({", ".join(f"0x{x:X}" for x in sorted(vistos)[:12])}...)', file=sys.stderr)


_HOOKS = None


def hooks_activos():
    """Direcciones con gancho RH_Scoped* activo en el catalogo (docs/hooks.csv): una llamada a ellas va al C++."""
    global _HOOKS
    if _HOOKS is None:
        _HOOKS = set()
        with (Path(__file__).resolve().parent.parent / 'docs/hooks.csv').open(encoding='utf-8-sig', newline='') as s:
            for r in csv.DictReader(s):
                try:
                    _HOOKS.add(int(r['address'], 16))
                except ValueError:
                    pass
    return _HOOKS


def escanear_exe(pe, reemplazadas, rangos, punteros=True):
    """va dato -> {funcs:{fva:(vivo)}, ancho:int, escribe:bool, direccion:bool}"""
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_MEM, CS_OP_IMM, CS_AC_WRITE
    from capstone.x86 import X86_REG_INVALID
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    datos = defaultdict(lambda: dict(funcs={}, ancho=0, escribe=False, direccion=False, tabla=False))
    for fva, ini, fin in rangos:
        off = pe.offset(ini, 1)
        if off is None or fin <= ini:
            continue
        code = pe.data[off:off + (fin - ini + 16)]
        vivo = fva not in reemplazadas
        for ins in md.disasm(code, ini):
            if ins.address >= fin:
                break
            for op in ins.operands:
                if op.type == CS_OP_MEM:
                    d = op.mem.disp & 0xFFFFFFFF
                    if op.mem.segment == 0 and LO <= d < HI and op.mem.base == X86_REG_INVALID:
                        e = datos[d]
                        e['funcs'][fva] = vivo
                        e['ancho'] = max(e['ancho'], op.size)
                        e['escribe'] |= bool(op.access & CS_AC_WRITE)
                        e['tabla'] |= op.mem.index != X86_REG_INVALID
                elif op.type == CS_OP_IMM:
                    d = op.imm & 0xFFFFFFFF
                    if LO <= d < HI and ins.mnemonic in ('push', 'mov', 'lea', 'cmp', 'add', 'sub', 'test', 'and', 'or'):
                        if ins.mnemonic in ('push', 'mov') or op.size == 4:
                            e = datos[d]
                            e['funcs'][fva] = vivo
                            e['direccion'] = True
    # Punteros a datos guardados en otras tablas de datos (p. ej. WaterCreatureManager_c::ms_creatureInfo apunta a las ranuras de
    # ModelIndices): quien los desreferencia lee la copia del EXE. Palabras alineadas de .rdata/.data (parte inicializada) con valor en
    # el rango de datos; clave = 0x100000000 + direccion de la palabra (no es una funcion).
    import struct as _st
    for s in pe.sections:
        if not punteros or s['name'] not in ('.rdata', '.data'):
            continue
        blob = pe.data[s['raw_offset']:s['raw_offset'] + s['raw_size'] // 4 * 4]
        for k, (v,) in enumerate(_st.iter_unpack('<I', blob)):
            if LO <= v < HI and not (0x3F000000 <= v):     # los valores >= 0x3F000000 son floats o enteros grandes, no direcciones
                e = datos[v]
                e['funcs'][0x100000000 + s['va'] + k * 4] = True
                e['direccion'] = True
    return datos


def propietario(archivo):
    p = archivo.split('/')
    stem = Path(archivo).stem
    for suf in ('Cpp',):
        if stem.endswith(suf) and len(stem) > 6:
            stem = stem[:-3]
    return stem


def origen_archivo(archivo):
    a = archivo.lower()
    if re.search(r'rwd3d|renderware|/rw|rpworld|rpskin|rpmatfx|rphanim|rtanim|rtquat|rwcore|rwplcore|d3d9|d3dx|/librw', a):
        return 'renderware'
    if re.search(r'/crt|crt[a-z]*\.cpp|x87math|locale', a):
        return 'crt'
    return 'juego'


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default='D:/Grand Theft Auto San Andreas/gta-sa.exe')
    ap.add_argument('--fuente', default='source')
    ap.add_argument('--salida', default='docs/DatosExe')
    a = ap.parse_args()
    out = Path(a.salida)
    out.mkdir(parents=True, exist_ok=True)
    pe = PE(Path(a.exe))
    refs = escanear_fuente(a.fuente)
    reemplazadas, rangos = cargar_funciones()
    cod = escanear_exe(pe, reemplazadas, rangos, punteros=False)   # la medicion cuenta solo codigo (comparable con las anteriores)

    # IAT
    d = pe.data
    pep = struct.unpack_from('<I', d, 60)[0]
    iat_rva, iat_sz = struct.unpack_from('<II', d, pep + 24 + 96 + 12 * 8)
    imp_rva, imp_sz = struct.unpack_from('<II', d, pep + 24 + 96 + 1 * 8)
    iat = (pe.base + iat_rva, iat_sz, pe.base + imp_rva, imp_sz)

    # objetos: union de direcciones de fuente y de codigo
    por_va = defaultdict(list)
    for r in refs:
        por_va[r['va']].append(r)
    todas = sorted(set(por_va) | set(cod))
    objs = []
    for i, va in enumerate(todas):
        rs = por_va.get(va, [])
        c = cod.get(va)
        tams = [r['tam'] for r in rs if r['tam']]
        ancho = c['ancho'] if c else 0
        if tams:
            tam, orig = max(tams), 'tipo'
        elif ancho and not (c and c['tabla']):
            tam, orig = ancho, 'acceso'
        else:
            sig = todas[i + 1] if i + 1 < len(todas) else va + 4
            tam, orig = max(1, min(sig - va, 64)), 'hueco'
        sec = seccion(va)
        funcs = c['funcs'] if c else {}
        vivas = [f for f, v in funcs.items() if v]
        rep = [f for f, v in funcs.items() if not v]
        archivos = sorted({r['archivo'] for r in rs})
        formas = sorted({r['forma'] for r in rs})
        regs = {region_codigo(f) for f in funcs}
        if rs:
            origs = {origen_archivo(r['archivo']) for r in rs}
        else:
            origs = set()
        origen_codigo = sorted(regs)
        if 'renderware' in origs or (not origs and regs <= {'renderware'} and regs):
            origen = 'renderware'
        elif 'crt' in origs or (not origs and regs <= {'crt'} and regs):
            origen = 'crt'
        elif origs or 'juego' in regs:
            origen = 'juego'
        elif regs:
            origen = '+'.join(sorted(regs))
        else:
            origen = 'sin_lector'
        if sec == '.rdata':
            nat = 'd_constante'
        elif sec == '.bss':
            nat = 'c_estado'
        elif sec in ('.data',):
            nat = 'a_inicializado'
        else:
            nat = 'otra'
        iv = ''
        if sec in ('.rdata', '.data') and tam <= 64:
            off = pe.offset(va, tam)
            if off is not None:
                iv = pe.data[off:off + tam].hex()
        objs.append(dict(va=f'0x{va:08X}', seccion=sec, naturaleza=nat, origen=origen, tam=tam, tam_origen=orig,
                         refs_dll=len(rs), formas=';'.join(formas), tipos=';'.join(sorted({r['tipo'] for r in rs if r['tipo']}))[:120],
                         archivos=';'.join(Path(x).name for x in archivos[:6]), n_archivos=len(archivos),
                         funcs_exe_vivas=len(vivas), funcs_exe_reemplazadas=len(rep),
                         escribe_exe=int(bool(c and c['escribe'])), direccion_tomada=int(bool(c and c['direccion'])),
                         tabla_indexada=int(bool(c and c['tabla'])),
                         compartido=int(bool(rs) and bool(vivas)),
                         valor_inicial=iv))
    with (out / 'referencias-dll.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'seccion', 'archivo', 'linea', 'forma', 'tipo', 'tam', 'propietario', 'origen'])
        for r in sorted(refs, key=lambda r: (r['va'], r['archivo'], r['linea'])):
            w.writerow([f"0x{r['va']:08X}", seccion(r['va']), r['archivo'], r['linea'], r['forma'], r['tipo'], r['tam'] or '',
                        propietario(r['archivo']), origen_archivo(r['archivo'])])
    campos = list(objs[0].keys())
    with (out / 'objetos.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos)
        w.writeheader()
        w.writerows(objs)

    # agregados
    sec_tot = {n: b - a_ for n, a_, b in SEC}
    por_sec = []
    for n, a_, b in SEC:
        os_ = [o for o in objs if o['seccion'] == n]
        sf = lambda xs: sum(o['tam'] for o in xs)
        dll = [o for o in os_ if o['refs_dll']]
        solo_exe = [o for o in os_ if not o['refs_dll']]
        compart = [o for o in os_ if o['compartido']]
        iniciales = n in ('.rdata', '.data')
        por_sec.append(dict(seccion=n, va_ini=f'0x{a_:08X}', tam_seccion=b - a_, objetos=len(os_), bytes_objetos=sf(os_),
                            objetos_con_ref_dll=len(dll), bytes_con_ref_dll=sf(dll),
                            objetos_solo_exe=len(solo_exe), bytes_solo_exe=sf(solo_exe),
                            objetos_compartidos=len(compart), bytes_compartidos=sf(compart),
                            bytes_tam_fiable_dll=sum(o['tam'] for o in dll if o['tam_origen'] != 'hueco'),
                            con_valor_inicial=iniciales))
    with (out / 'por-seccion.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(por_sec[0].keys()))
        w.writeheader()
        w.writerows(por_sec)
    cls = defaultdict(lambda: defaultdict(int))
    for r in refs:
        k = (propietario(r['archivo']), origen_archivo(r['archivo']))
        cls[k]['refs'] += 1
        cls[k][seccion(r['va'])] += 1
    uniq_cls = defaultdict(set)
    for r in refs:
        uniq_cls[(propietario(r['archivo']), origen_archivo(r['archivo']))].add(r['va'])
    with (out / 'por-clase.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['propietario', 'origen', 'refs', 'direcciones_distintas', '.rdata', '.data', '.bss', 'otras'])
        for (p, o), v in sorted(cls.items(), key=lambda kv: -kv[1]['refs']):
            w.writerow([p, o, v['refs'], len(uniq_cls[(p, o)]), v['.rdata'], v['.data'], v['.bss'],
                        v['_TEXT_HA'] + v['_rwdseg'] + v['.rsrc']])

    # tabla seccion x origen (objetos con referencia de la DLL y solo-EXE)
    cruz = defaultdict(lambda: [0, 0, 0, 0])
    for o in objs:
        k = (o['seccion'], o['origen'])
        i = 0 if o['refs_dll'] else 2
        cruz[k][i] += 1
        cruz[k][i + 1] += o['tam']
    with (out / 'por-seccion-origen.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['seccion', 'origen', 'objetos_con_ref_dll', 'bytes_con_ref_dll', 'objetos_solo_exe', 'bytes_solo_exe'])
        for (sc, og), v in sorted(cruz.items()):
            w.writerow([sc, og, *v])

    # clasificacion byte a byte de .rdata y .data inicializado
    def clasificar(ini, fin):
        n = fin - ini
        cl = bytearray(n)  # 0 resto, 1 iat, 2 obj dll, 3 obj solo exe, 4 cadena, 5 ceros
        off = pe.offset(ini, 1)
        raw = pe.data[off:off + n] if off is not None else b''
        raw = raw.ljust(n, b'\0')
        for m in re.finditer(rb'[\x20-\x7e\t\r\n]{4,}\x00', raw):
            for k in range(m.start(), m.end()):
                cl[k] = 4
        for m in re.finditer(rb'\x00{8,}', raw):
            for k in range(m.start(), m.end()):
                if cl[k] == 0:
                    cl[k] = 5
        for o in objs:
            v = int(o['va'], 16)
            if ini <= v < fin:
                c = 2 if o['refs_dll'] else 3
                for k in range(v - ini, min(n, v - ini + o['tam'])):
                    if cl[k] < 2 or cl[k] in (4, 5) or (c == 2 and cl[k] == 3):
                        cl[k] = c
        for k in range(max(0, iat[0] - ini), min(n, iat[0] + iat[1] - ini)):
            cl[k] = 1
        nombres = ['resto_sin_clasificar', 'iat', 'objeto_con_ref_dll', 'objeto_solo_exe', 'cadena_ascii', 'ceros_relleno']
        return {nombres[i]: cl.count(i) for i in range(6)}
    bytes_cls = {'.rdata': clasificar(0x858000, 0x8A4000), '.data(inicializado)': clasificar(0x8A4000, 0x8E4000)}
    forma = defaultdict(int)
    for r in refs:
        forma[r['forma']] += 1
    res = dict(exe=str(a.exe), referencias_dll=len(refs), direcciones_dll=len(por_va), objetos=len(objs),
               formas=dict(forma), iat=dict(va=f'0x{iat[0]:08X}', bytes=iat[1], directorio_importacion=f'0x{iat[2]:08X}',
                                            tam_dir=iat[3]),
               funciones_exe=len(rangos), funciones_vivas=sum(1 for f, _, _ in rangos if f not in reemplazadas),
               por_seccion=por_sec, bytes_por_clase=bytes_cls)
    (out / 'resumen.json').write_text(json.dumps(res, indent=1, ensure_ascii=False), encoding='utf-8')
    print(json.dumps({k: v for k, v in res.items() if k not in ('por_seccion',)}, ensure_ascii=False))
    for p in por_sec:
        print(p)


if __name__ == '__main__':
    main()
