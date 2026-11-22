"""Lote IND0A: mapa de dependencias de codigo y de datos del codigo propio respecto del EXE original.

Entradas (todas solo lectura):
  datos/refs_codigo.csv   salida de scan_obj.py (direcciones fijas escritas a mano en el codigo de los .obj)
  docs/*.csv, docs/MemoryMap/*.csv, docs/JuegoExe/*.csv, docs/RenderWareExe/*.csv
  el gta_sa.exe original (--exe) para leer bytes (cadenas, tablas, vtables, _initterm)
Salidas en datos/: dependencias_codigo.csv, dependencias_datos.csv, excluidas_estado.csv,
  audit819_estado.csv, tablas_de_punteros.csv, initterm.csv, cierre_codigo.csv, resumen.json

Uso: python -I analizar.py --exe <gta_sa.exe> [--repo <raiz>]
"""
import argparse, bisect, collections, csv, json, re, struct, sys
from pathlib import Path

SECS = [('.text', 0x401000, 0x857000), ('_rwcseg', 0x857000, 0x858000), ('.rdata', 0x858000, 0x8A4000),
        ('.data', 0x8A4000, 0xC9E000), ('_TEXT_HA', 0xC9E000, 0xCAF000), ('_rwdseg', 0xCAF000, 0xCB0000),
        ('.rsrc', 0xCB0000, 0xCB1000)]
RAW = {'.text': (0x401000, 1024, 4546048), '_rwcseg': (0x857000, 4547072, 1536), '.rdata': (0x858000, 4548608, 308224),
       '.data': (0x8A4000, 4856832, 262144), '_TEXT_HA': (0xC9E000, 5118976, 68608), '_rwdseg': (0xCAF000, 5187584, 512),
       '.rsrc': (0xCB0000, 5188096, 1536)}
DATA_SECS = ('.rdata', '.data', '_TEXT_HA', '_rwdseg', '.rsrc')
BSS_START = 0x8A4000 + 262144


def sec(v):
    for n, a, b in SECS:
        if a <= v < b:
            return n
    return 'fuera'


def rc(p):
    with open(p, encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def wcsv(path, rows, fields):
    with open(path, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fields, extrasaction='ignore')
        w.writeheader()
        w.writerows(rows)


def region(v):
    if 0x401000 <= v < 0x750000:
        return 'juego'
    if v < 0x820000:
        return 'renderware'
    if v < 0x857000:
        return 'crt'
    return 'otro'


def demangle(names):
    """Demangla simbolos MSVC con dbghelp (si no esta disponible, devuelve el simbolo)."""
    out = {}
    try:
        import ctypes
        dbg = ctypes.WinDLL('dbghelp')
        buf = ctypes.create_string_buffer(1024)
        for n in names:
            if n.startswith('?'):
                k = dbg.UnDecorateSymbolName(n.encode('latin1'), buf, 1024, 0x1000 | 0x0800 | 0x0400)  # nombre completo sin tipos extra
                out[n] = buf.value.decode('latin1') if k else n
            else:
                out[n] = n
    except Exception:
        out = {n: n for n in names}
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', required=True)
    ap.add_argument('--repo', default=str(Path(__file__).resolve().parents[2]))
    a = ap.parse_args()
    repo = Path(a.repo)
    D = repo / 'reversing/FnIND0A/datos'
    exe = Path(a.exe).read_bytes()

    def off(va):
        for n, (v, o, s) in RAW.items():
            if v <= va < v + s:
                return o + va - v
        return None

    def u32(va):
        o = off(va)
        return struct.unpack_from('<I', exe, o)[0] if o is not None and o + 4 <= len(exe) else None

    R = repo / 'docs'
    refs = rc(D / 'refs_codigo.csv')
    direcciones = rc(R / 'direcciones.csv')
    H = {int(r['direccion'], 16) for r in direcciones}
    H |= {int(r['address'], 16) for r in rc(R / 'hooks.csv')}
    H |= {int(r['address'], 16) for r in rc(R / 'hooks_notsa.csv')}
    gf = R / 'CodigoVivoExe/fase8-experimento/ganchos-fuente.csv'   # ronda 4: ganchos segun el codigo fuente (reversing/FnFase8/ganchos_fuente.py)
    if gf.exists():
        H |= {int(r['address'], 16) for r in rc(gf)}
    G = {int(r['va'], 16): r for r in rc(R / 'JuegoExe/funciones.csv')}
    W = {int(r['va'], 16): r for r in rc(R / 'RenderWareExe/funciones.csv')}
    BF = {int(r['va'], 16): r for r in rc(R / 'MemoryMap/binary_functions.csv')}
    calls = collections.defaultdict(set)
    T = set()
    for r in rc(R / 'MemoryMap/binary_calls.csv'):
        if r['target_va'] and r['caller_va']:
            c, t = int(r['caller_va'], 16), int(r['target_va'], 16)
            calls[c].add(t)
            T.add(t)
    ranges = sorted((int(r['start_va'], 16), int(r['end_va'], 16), int(r['function_va'], 16)) for r in rc(R / 'MemoryMap/binary_function_ranges.csv'))
    rstarts = [x[0] for x in ranges]

    def func_of(v):
        i = bisect.bisect_right(rstarts, v) - 1
        if i >= 0 and ranges[i][0] <= v <= ranges[i][1]:
            return ranges[i][2]
        return None

    # ---------------------------------------------------------------- agrupar referencias
    by_addr = collections.defaultdict(list)
    for r in refs:
        by_addr[int(r['direccion'], 16)].append(r)
    nombres = demangle({r['funcion'] for r in refs})

    # ---------------------------------------------------------------- CODIGO
    def conf_code(v, rs):
        forms = {r['forma'] for r in rs}
        if v in BF or v in W or v in G or v in T or 'mov_call' in forms:
            return 'A'
        mn = {r['instruccion'].split()[0] for r in rs}
        # B: almacena la direccion como puntero (mov [mem], imm / push) y el destino decodifica como inicio de funcion
        if mn <= {'mov', 'push'} and (exe[off(v) - 1] in (0xC3, 0xCC, 0x90) or exe[off(v) - 3] == 0xC2 if off(v) else False):
            first = exe[off(v)]
            if first in (0x55, 0x53, 0x56, 0x57, 0x83, 0x81, 0x8B, 0x6A, 0x68, 0x51, 0x52, 0x64, 0xA1, 0x33, 0x0F, 0xD9, 0xDD, 0xDB, 0xF6, 0x80, 0x8A, 0x31, 0x85, 0x3B, 0xB8, 0xC3, 0xC2, 0x8D):
                return 'B'
        return 'C'

    cod = []
    for v, rs in sorted(by_addr.items()):
        if sec(v) not in ('.text', '_rwcseg'):
            continue
        hooked = v in H
        forms = collections.Counter(r['forma'] for r in rs)
        cod.append(dict(v=v, hooked=hooked, rs=rs, forms=forms, conf=conf_code(v, rs)))
    sin_cuerpo = [c for c in cod if not c['hooked']]

    def modulo(v):
        w = W.get(v)
        if w:
            return w['clase_modulo'], w['estado']
        g = G.get(v)
        if g:
            return g['modulo'], g['estado']
        return '', ''

    code_rows = []
    for c in sin_cuerpo:
        v = c['v']
        mod, est = modulo(v)
        bf = BF.get(v)
        fo = func_of(v)
        objs = sorted({r['obj'] for r in c['rs']})
        funcs = sorted({nombres.get(r['funcion'], r['funcion']) for r in c['rs']})
        cat = ''
        if c['conf'] == 'C':
            cat = 'ruido_probable'
        elif est == 'enlazada':
            cat = 'rw_enlazada_biblioteca'
        elif est == 'no_reconstruible':
            cat = 'rw_no_reconstruible'
        elif region(v) == 'juego':
            cat = 'juego_sin_hook'
        elif region(v) == 'renderware':
            cat = 'rw_sin_hook'
        elif region(v) == 'crt':
            cat = 'crt_o_biblioteca'
        else:
            cat = 'otro'
        code_rows.append(dict(
            direccion=f'0x{v:08X}', region=region(v), seccion=sec(v), categoria=cat, confianza=c['conf'],
            es_inicio_ghidra=int(v in BF), contenida_en=(f'0x{fo:08X}' if fo and fo != v else ''),
            nombre_ghidra=(bf['name'] if bf else ''), bytes=(bf['body_bytes'] if bf else ''),
            modulo=mod, estado_mapa=est,
            formas=';'.join(f'{k}={n}' for k, n in sorted(c['forms'].items())), referencias=len(c['rs']),
            archivos=len(objs), ejemplo_archivo=';'.join(objs[:3]), ejemplo_funcion=' | '.join(funcs[:2])[:200],
            ejemplo_instruccion=c['rs'][0]['instruccion']))
    code_rows.sort(key=lambda r: (r['confianza'], r['categoria'], r['direccion']))
    wcsv(D / 'dependencias_codigo.csv', code_rows, list(code_rows[0]))

    # --------------------------------------------------- cierre transitivo del codigo original necesario
    roots = {c['v'] for c in sin_cuerpo if c['conf'] in ('A', 'B')}
    # un destino que cae en medio de una funcion se atribuye a la funcion que lo contiene
    roots_f = set()
    for v in roots:
        roots_f.add(v if v in BF else (func_of(v) or v))

    from capstone import Cs, CS_ARCH_X86, CS_MODE_32
    md0 = Cs(CS_ARCH_X86, CS_MODE_32)
    IAT = (0x858000, 0x858000 + 888)
    memo = {}

    def descend(entry):
        """Recorrido de flujo desde `entry`: devuelve (bytes del cuerpo, destinos de call directos, llamadas indirectas, usos de la IAT)."""
        if entry in memo:
            return memo[entry]
        seen_ins, work, targets, indirect, iat_use = set(), [entry], set(), 0, set()
        lo = hi = entry
        while work:
            p = work.pop()
            while p not in seen_ins:
                o = off(p)
                if o is None or sec(p) != '.text':
                    break
                ins = next(md0.disasm_lite(exe[o:o + 15], p), None)
                if not ins:
                    break
                _, size, mn, ops = ins
                seen_ins.add(p)
                lo, hi = min(lo, p), max(hi, p + size)
                if mn == 'call':
                    if ops.startswith('0x'):
                        targets.add(int(ops, 16))
                    elif ops.startswith('dword ptr [0x'):
                        x = int(ops[11:].strip('[]'), 16) if False else int(re.search(r'0x[0-9a-f]+', ops).group(0), 16)
                        if IAT[0] <= x < IAT[1]:
                            iat_use.add(x)
                        else:
                            indirect += 1
                    else:
                        indirect += 1
                elif mn in ('ret', 'retn'):
                    break
                elif mn == 'jmp':
                    if ops.startswith('0x'):
                        t = int(ops, 16)
                        if t in H or t in BF and t != entry and abs(t - p) > 0x2000:
                            targets.add(t)
                        elif lo - 0x4000 <= t <= hi + 0x8000:
                            work.append(t)
                        else:
                            targets.add(t)
                    elif 'dword ptr [' in ops and '*4' in ops:
                        m = re.search(r'0x[0-9a-f]+', ops)
                        if m:
                            tb = int(m.group(0), 16)
                            for k in range(0, 512):
                                e = u32(tb + 4 * k)
                                if e is None or not (p - 0x4000 <= e <= p + 0x8000) or sec(e) != '.text':
                                    break
                                work.append(e)
                    else:
                        indirect += 1
                    break
                elif mn.startswith('j') and ops.startswith('0x'):
                    work.append(int(ops, 16))
                p += size
        memo[entry] = (hi - lo if seen_ins else 0, targets, indirect, iat_use)
        return memo[entry]

    def closure(seed):
        seen, stack = {}, list(seed)
        ind = 0
        iat_all = set()
        while stack:
            f = stack.pop()
            if f in seen or f in H or f in excl_set:
                continue
            nb, tg, indc, iu = descend(f)
            seen[f] = nb
            ind += indc
            iat_all |= iu
            for t in tg:
                if t not in seen:
                    stack.append(t)
        return seen, ind, iat_all

    excl_set = set()  # (las excluidas no tienen hook: si el codigo las alcanza, cuentan como dependencia)
    cl_calls, ind_calls, iat_calls = closure(roots_f)
    # ranuras de vtables originales cuyo codigo no esta enganchado: lo alcanzan los objetos creados por codigo original
    slots = rc(R / 'MemoryMap/binary_slots.csv')
    vt_free = {int(s['function_va'], 16) for s in slots if int(s['function_va'], 16) not in H and 0x401000 <= int(s['function_va'], 16) < 0x857000}
    cl_vt, ind_vt, iat_vt = closure(roots_f | vt_free)

    def size_of(fs):
        return sum(fs.values())

    def split(fs):
        d = collections.Counter()
        for f in fs:
            d[region(f)] += 1
        return dict(d)

    def split_bytes(fs):
        d = collections.Counter()
        for f, n in fs.items():
            d[region(f)] += n
        return dict(d)

    cierre = []
    for f in sorted(cl_vt):
        r = BF.get(f, {})
        mod, est = modulo(f)
        cierre.append(dict(funcion=f'0x{f:08X}', region=region(f), nombre=r.get('name', ''), bytes=cl_vt[f],
                           via=('raiz' if f in roots_f else ('llamadas' if f in cl_calls else 'solo_vtable')), modulo=mod, estado_mapa=est))
    wcsv(D / 'cierre_codigo.csv', cierre, ['funcion', 'region', 'nombre', 'bytes', 'via', 'modulo', 'estado_mapa'])

    # ---------------------------------------------------------------- DATOS
    vt_rows = rc(R / 'MemoryMap/binary_vtables.csv')
    vt_ranges = sorted((int(r['va'], 16), int(r['va'], 16) + 4 * int(r['declared_slots']), r['owner']) for r in vt_rows)
    vstarts = [x[0] for x in vt_ranges]

    def vtable_of(v):
        i = bisect.bisect_right(vstarts, v) - 1
        if i >= 0 and vt_ranges[i][0] <= v < vt_ranges[i][1]:
            return vt_ranges[i][2]
        return ''

    def is_string(v):
        o = off(v)
        if o is None:
            return False
        s = exe[o:o + 64]
        n = s.find(b'\0')
        return n >= 3 and all(32 <= ch < 127 or ch in (9, 10, 13) for ch in s[:n])

    # tipos y tamanos por el codigo fuente (StaticRef<T>(0x...), reinterpret_cast<T*>(0x...))
    SIZES = {'bool': 1, 'char': 1, 'int8': 1, 'uint8': 1, 'int16': 2, 'uint16': 2, 'short': 2, 'int': 4, 'int32': 4, 'uint32': 4,
             'float': 4, 'uint': 4, 'unsigned': 4, 'DWORD': 4, 'size_t': 4, 'int64': 8, 'uint64': 8, 'double': 8, 'RwRGBA': 4,
             'CRGBA': 4, 'CVector': 12, 'CVector2D': 8, 'RwV3d': 12, 'RwV2d': 8, 'CRect': 16, 'RwBool': 4, 'RwInt32': 4, 'RwReal': 4,
             'RwUInt32': 4, 'RwUInt8': 1, 'RwInt16': 2, 'RwUInt16': 2, 'RwChar': 1, 'RwTexture': 0}

    def sizeof(t):
        t = t.strip()
        t = re.sub(r'\bconst\b|\bvolatile\b|\bstruct\b|\benum\b', '', t).strip()
        if t.endswith('*') or '(*' in t or t.endswith('*>'):
            return 4
        m = re.match(r'^(.*)\[(\d+)\]$', t)
        if m:
            e = sizeof(m.group(1))
            return e * int(m.group(2)) if e else 0
        m = re.match(r'^std::array<(.+),\s*(\d+)>$', t)
        if m:
            e = sizeof(m.group(1))
            return e * int(m.group(2)) if e else 0
        return SIZES.get(t.split('::')[-1], 0)

    src_info = collections.defaultdict(list)  # va -> [(clase, tipo, tam, archivo:linea, nombrada)]
    pat = re.compile(r'(StaticRef|ScopedStaticRef|StaticRefArr)\s*<')
    named = re.compile(r'(auto|[\w:<>]+)\s*&\s*(\w+)\s*=\s*$')
    for p in (repo / 'source').rglob('*'):
        if p.suffix not in ('.cpp', '.h') or not p.is_file():
            continue
        txt = p.read_text(encoding='utf-8', errors='replace')
        for m in pat.finditer(txt):
            i, depth = m.end(), 1
            while i < len(txt) and depth:
                depth += (txt[i] == '<') - (txt[i] == '>')
                i += 1
            tipo = txt[m.end():i - 1]
            mm = re.match(r'\s*\(\s*(0x[0-9A-Fa-f]+)', txt[i:i + 40])
            if not mm:
                continue
            tipo1 = tipo.split(',')[0] if m.group(1) != 'StaticRefArr' else tipo
            ln = txt.count('\n', 0, m.start()) + 1
            pre = txt[max(0, m.start() - 80):m.start()]
            src_info[int(mm.group(1), 16)].append(('StaticRef', tipo1.strip(), sizeof(tipo1), f'{p.relative_to(repo).as_posix()}:{ln}', int(bool(named.search(pre.split('\n')[-1])))))
        for m in re.finditer(r'reinterpret_cast<\s*(const\s+)?([\w:]+)\s*\*\s*>\s*\(\s*(0x[0-9A-Fa-f]+)\s*\)', txt):
            ln = txt.count('\n', 0, m.start()) + 1
            src_info[int(m.group(3), 16)].append(('cast_ptr', m.group(2), sizeof(m.group(2)), f'{p.relative_to(repo).as_posix()}:{ln}', 0))
        for m in re.finditer(r'RH_ScopedVirtualClass\(\s*(\w+)\s*,\s*(0x[0-9A-Fa-f]+)', txt):
            ln = txt.count('\n', 0, m.start()) + 1
            src_info[int(m.group(2), 16)].append(('vtable_decl', m.group(1), 0, f'{p.relative_to(repo).as_posix()}:{ln}', 0))

    data_rows = []
    cnt_sec = collections.Counter()
    for v, rs in sorted(by_addr.items()):
        s = sec(v)
        if s not in DATA_SECS:
            continue
        forms = collections.Counter(r['forma'] for r in rs)
        info = src_info.get(v, [])
        vt = vtable_of(v)
        if vt or any(i[0] == 'vtable_decl' for i in info):
            tipo = 'vtable'
        elif s == '.rdata' and is_string(v):
            tipo = 'cadena'
        elif s == '.rdata' or s == '_rwdseg' or s == '.rsrc':
            tipo = 'constante_rdata'
        elif v >= BSS_START and s == '.data':
            tipo = 'variable_bss'
        elif s == '.data':
            tipo = 'variable_data_inicializada'
        else:
            tipo = 'variable_' + s
        objs = sorted({r['obj'] for r in rs})
        sizes = [i[2] for i in info if i[2]]
        data_rows.append(dict(
            direccion=f'0x{v:08X}', seccion=s, tipo=tipo, tam_conocido=(max(sizes) if sizes else ''),
            tipos_fuente=';'.join(sorted({i[1] for i in info if i[0] != 'vtable_decl'}))[:80],
            en_fuente=int(bool(info)), con_nombre_cpp=int(any(i[4] for i in info)),
            formas=';'.join(f'{k}={n}' for k, n in sorted(forms.items())), referencias=len(rs), archivos=len(objs),
            ejemplo_archivo=';'.join(objs[:3]), ejemplo_instruccion=rs[0]['instruccion']))
        cnt_sec[(s, tipo)] += 1
    wcsv(D / 'dependencias_datos.csv', data_rows, list(data_rows[0]))

    # ---------------------------------------------------- uso de datos por TODO el codigo original (cota superior)
    md = md0
    o, n = off(0x401000), 4546048
    text = exe[o:o + n]
    HEX = re.compile(r'0x[0-9a-fA-F]+')
    orig_data = collections.Counter()
    pos = 0
    while pos < len(text):
        any_ = False
        for addr, size, mn, ops in md.disasm_lite(text[pos:], 0x401000 + pos):
            any_ = True
            pos = addr - 0x401000 + size
            if '0x' in ops:
                for m in HEX.findall(ops):
                    x = int(m, 16)
                    if 0x858000 <= x < 0xCB1000:
                        orig_data[x] += 1
        if not any_:
            pos += 1
    own_data = {int(r['direccion'], 16) for r in data_rows}
    orig_set = set(orig_data)

    # ---------------------------------------------------- tablas de punteros a codigo dentro de .rdata/.data
    fnstarts = set(BF) | set(W) | set(G)
    ptr_rows = []
    tabla_total = collections.Counter()
    for sname in ('.rdata', '.data'):
        v0, o0, s0 = RAW[sname]
        for i in range(0, s0 - 3, 4):
            x = struct.unpack_from('<I', exe, o0 + i)[0]
            if 0x401000 <= x < 0x857000 and x in fnstarts:
                va = v0 + i
                tabla_total[(sname, 'vtable' if vtable_of(va) else 'otra_tabla', 'enganchado' if x in H else 'sin_hook')] += 1
                if not vtable_of(va):
                    ptr_rows.append(dict(direccion=f'0x{va:08X}', seccion=sname, destino=f'0x{x:08X}', region=region(x), enganchado=int(x in H),
                                         referenciada_por_codigo_propio=int(va in own_data)))
    wcsv(D / 'tablas_de_punteros.csv', ptr_rows, ['direccion', 'seccion', 'destino', 'region', 'enganchado', 'referenciada_por_codigo_propio'])

    # ---------------------------------------------------- _initterm
    ini = []
    for lo, hi, nombre in ((0x8A5A14, 0x8A5A30, 'C (XIA..XIZ)'), (0x8A4000, 0x8A5A10, 'C++ (XCA..XCZ)')):
        for va in range(lo, hi, 4):
            t = u32(va)
            if t:
                bf = BF.get(t, {})
                ini.append(dict(tabla=nombre, entrada=f'0x{va:08X}', destino=f'0x{t:08X}', region=region(t), bytes=bf.get('body_bytes', ''),
                                nombre=bf.get('name', ''), enganchado=int(t in H)))
    wcsv(D / 'initterm.csv', ini, ['tabla', 'entrada', 'destino', 'region', 'bytes', 'nombre', 'enganchado'])

    # ---------------------------------------------------- excluidas (118) y auditoria de 819
    ex = rc(R / 'JuegoExe/excluidas.csv')
    ex_rows = []
    for e in ex:
        v = int(e['va'], 16)
        rs = by_addr.get(v, [])
        ex_rows.append(dict(va=e['va'], bytes=e['bytes'], modulo=e['modulo'], categoria=e['categoria'], enganchada=int(v in H),
                            referencias_codigo_propio=len(rs), llamadoras_en_exe=sum(1 for c, ts in calls.items() if v in ts)))
    wcsv(D / 'excluidas_estado.csv', ex_rows, ['va', 'bytes', 'modulo', 'categoria', 'enganchada', 'referencias_codigo_propio', 'llamadoras_en_exe'])
    a819 = rc(repo / 'reversing/BinaryCoverageAudit/review-source-calls.csv')
    r819 = []
    for r in a819:
        v = int(r['address'], 16)
        rs = by_addr.get(v, [])
        r819.append(dict(address=r['address'], ghidra_name=r['ghidra_name'], hoy_enganchada=int(v in H), hoy_referenciada=int(bool(rs)),
                         hoy_confianza=(conf_code(v, rs) if rs else ''), clasificacion_2026_10=r['review_classification']))
    wcsv(D / 'audit819_estado.csv', r819, ['address', 'ghidra_name', 'hoy_enganchada', 'hoy_referenciada', 'hoy_confianza', 'clasificacion_2026_10'])

    # ---------------------------------------------------- resumen
    cc = collections.Counter((r['categoria'], r['confianza']) for r in code_rows)
    res = dict(
        referencias_totales=len(refs), destinos_distintos=len(by_addr), objs=len({r['obj'] for r in refs}),
        hooks_direcciones=len(H),
        codigo=dict(
            destinos_texto=len(cod), con_hook=sum(c['hooked'] for c in cod), sin_hook=len(sin_cuerpo),
            sin_hook_por_confianza=dict(collections.Counter(c['conf'] for c in sin_cuerpo)),
            por_categoria={f'{k[0]}|{k[1]}': v for k, v in sorted(cc.items())},
            dentro_401000_81FFFF_AB=sum(1 for c in sin_cuerpo if c['conf'] in 'AB' and 0x401000 <= c['v'] < 0x820000),
            fuera_AB=sum(1 for c in sin_cuerpo if c['conf'] in 'AB' and c['v'] >= 0x820000),
            cierre_por_llamadas=dict(funciones=len(cl_calls), bytes=size_of(cl_calls), por_region=split(cl_calls), bytes_por_region=split_bytes(cl_calls), llamadas_indirectas_sin_resolver=ind_calls, funciones_de_la_iat=len(iat_calls)),
            cierre_con_vtables=dict(funciones=len(cl_vt), bytes=size_of(cl_vt), por_region=split(cl_vt), bytes_por_region=split_bytes(cl_vt), ranuras_sin_hook=len(vt_free), llamadas_indirectas_sin_resolver=ind_vt, funciones_de_la_iat=len(iat_vt)),
            raices_funcion=len(roots_f)),
        datos=dict(
            destinos_datos=len(data_rows), por_seccion_tipo={f'{k[0]}|{k[1]}': v for k, v in sorted(cnt_sec.items())},
            referencias_datos=sum(int(r['referencias']) for r in data_rows),
            con_tamano_conocido=sum(1 for r in data_rows if r['tam_conocido'] != ''),
            bytes_conocidos=sum(int(r['tam_conocido']) for r in data_rows if r['tam_conocido'] != ''),
            en_fuente=sum(int(r['en_fuente']) for r in data_rows), con_nombre_cpp=sum(int(r['con_nombre_cpp']) for r in data_rows),
            direcciones_datos_usadas_por_todo_el_codigo_original=len(orig_set),
            de_ellas_usadas_tambien_por_codigo_propio=len(orig_set & own_data),
            usadas_solo_por_codigo_propio=len(own_data - orig_set)),
        tablas_punteros={f'{k[0]}|{k[1]}|{k[2]}': v for k, v in sorted(tabla_total.items())},
        initterm=dict(entradas=len(ini), por_tabla=dict(collections.Counter(r['tabla'] for r in ini)),
                      enganchadas=sum(r['enganchado'] for r in ini),
                      por_region=dict(collections.Counter(r['region'] for r in ini)),
                      bytes=sum(int(r['bytes']) for r in ini if str(r['bytes']).isdigit())),
        excluidas=dict(total=len(ex_rows), enganchadas=sum(r['enganchada'] for r in ex_rows),
                       referenciadas_por_codigo_propio=sum(1 for r in ex_rows if r['referencias_codigo_propio']),
                       por_categoria=dict(collections.Counter(r['categoria'] for r in ex_rows))),
        audit819=dict(total=len(r819), hoy_enganchadas=sum(r['hoy_enganchada'] for r in r819),
                      hoy_referenciadas=sum(r['hoy_referenciada'] for r in r819),
                      hoy_ref_confianza=dict(collections.Counter(r['hoy_confianza'] for r in r819 if r['hoy_referenciada']))),
    )
    (D / 'resumen.json').write_text(json.dumps(res, indent=1, ensure_ascii=False), encoding='utf-8')
    print(json.dumps(res, indent=1, ensure_ascii=False))


if __name__ == '__main__':
    main()
