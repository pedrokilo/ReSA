"""Lote PLAN1: tabla de raices de codigo (direcciones del EXE original que el codigo propio todavia toma).

Solo lectura y analisis estatico: NO ejecuta el juego ni compila.
Entradas:
  --ind0a  carpeta datos/ de IND0A (dependencias_codigo.csv, refs_codigo.csv [no versionado, se lee de wv-6], excluidas_estado.csv)
  --exe    gta_sa.exe original (solo lectura, para desensamblar con capstone)
  --repo   raiz del repositorio (docs/*.csv, source/)
Salidas:
  docs/ExeIndependiente/raices.csv     una fila por direccion sin cuerpo propio (890 filas: 779 A+B y 111 C)
  reversing/FnPLAN1/datos/grafo_raices.csv   aristas raiz -> raiz (para ordenar lotes)
  reversing/FnPLAN1/datos/resumen_raices.json

Uso:  python -I raices.py --exe "D:/Grand Theft Auto San Andreas/gta_sa.exe" --ind0a D:/Original/wv-6/reversing/FnIND0A/datos --repo <raiz>
"""
import argparse, bisect, collections, csv, json, re, struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

SECS = [('.text', 0x401000, 0x857000), ('_rwcseg', 0x857000, 0x858000)]
RAW = {'.text': (0x401000, 1024, 4546048), '_rwcseg': (0x857000, 4547072, 1536), '.rdata': (0x858000, 4548608, 308224),
       '.data': (0x8A4000, 4856832, 262144)}
IAT = (0x858000, 0x858000 + 888)

# Direcciones que IND0A dejo como ruido (C) pero que la revision manual PLAN1 comprobo que son funciones reales
# usadas como puntero a funcion en el codigo propio (9 de las 13 con uso en codigo fuente; ver muestra y plan-sustitucion.md)
PROMOVER = {0x5C2FF0, 0x5C61E0, 0x734970, 0x801C30, 0x807C60, 0x807D80, 0x810D20, 0x8194D0, 0x856BC0}

# familias de archivos .obj de RenderWare reconstruido -> modulo
OBJ_RW = {'rpworld': 'RpWorld', 'rwcore': 'RwCore', 'rwplcore': 'RwPlCore', 'rpmatfx': 'RpMatFX', 'rphanim': 'RpHAnim',
          'rpskin': 'RpSkin', 'rpuvanim': 'RpUVAnim', 'rtanim': 'RtAnim', 'rpanisot': 'RpAnisot', 'rtdict': 'RtDict',
          'rtquat': 'RtQuat', 'rtbmp': 'RtBMP', 'rtslerp': 'RtSlerp', 'rwtexdict': 'RwTexDict'}


def rc(p):
    with open(p, encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def wcsv(path, rows, fields):
    Path(path).parent.mkdir(parents=True, exist_ok=True)
    with open(path, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fields, extrasaction='ignore')
        w.writeheader()
        w.writerows(rows)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', required=True)
    ap.add_argument('--ind0a', required=True)
    ap.add_argument('--repo', default=str(Path(__file__).resolve().parents[2]))
    a = ap.parse_args()
    repo = Path(a.repo)
    R = repo / 'docs'
    D = Path(a.ind0a)
    OUT = repo / 'reversing/FnPLAN1/datos'
    exe = Path(a.exe).read_bytes()

    def off(va):
        for n, (v, o, s) in RAW.items():
            if v <= va < v + s:
                return o + va - v
        return None

    def u32(va):
        o = off(va)
        return struct.unpack_from('<I', exe, o)[0] if o is not None and o + 4 <= len(exe) else None

    def intext(v):
        return 0x401000 <= v < 0x858000

    # ---------------------------------------------------------------- tablas del proyecto
    H = {int(r['direccion'], 16) for r in rc(R / 'direcciones.csv')}
    H |= {int(r['address'], 16) for r in rc(R / 'hooks.csv')}
    H |= {int(r['address'], 16) for r in rc(R / 'hooks_notsa.csv')}
    hooks_info = {}
    for r in rc(R / 'hooks.csv'):
        hooks_info[int(r['address'], 16)] = (r['class'], r['fn_name'], r['validated'])
    G = {int(r['va'], 16): r for r in rc(R / 'JuegoExe/funciones.csv')}
    W = {int(r['va'], 16): r for r in rc(R / 'RenderWareExe/funciones.csv')}
    NR = {int(r['va'], 16): r for r in rc(R / 'RenderWareExe/no-reconstruibles.csv')}
    ANC = {int(r['va'], 16): r for r in rc(R / 'RenderWareExe/anclas.csv')}
    EXC = {int(r['va'], 16): r for r in rc(R / 'JuegoExe/excluidas.csv')}
    BF = {int(r['va'], 16): r for r in rc(R / 'MemoryMap/binary_functions.csv')}
    ranges = sorted((int(r['start_va'], 16), int(r['end_va'], 16), int(r['function_va'], 16)) for r in rc(R / 'MemoryMap/binary_function_ranges.csv'))
    rstarts = [x[0] for x in ranges]

    def func_of(v):
        i = bisect.bisect_right(rstarts, v) - 1
        if i >= 0 and ranges[i][0] <= v <= ranges[i][1]:
            return ranges[i][2]
        return None

    # vecinos de la tabla de RenderWare para estimar el modulo por vecindad
    wsorted = sorted(W)

    def neighbour_module(v):
        i = bisect.bisect_left(wsorted, v)
        lo = W[wsorted[i - 1]]['clase_modulo'] if i > 0 else ''
        hi = W[wsorted[i]]['clase_modulo'] if i < len(wsorted) else ''
        return (lo if lo == hi else '', lo, hi)

    callers_exe = collections.defaultdict(set)
    for r in rc(R / 'MemoryMap/binary_calls.csv'):
        if r['target_va'] and r['caller_va']:
            callers_exe[int(r['target_va'], 16)].add(int(r['caller_va'], 16))

    # ---------------------------------------------------------------- IND0A
    dep = rc(D / 'dependencias_codigo.csv')
    refs = rc(D / 'refs_codigo.csv')
    excl_est = {int(r['va'], 16): r for r in rc(D / 'excluidas_estado.csv')}
    by_addr = collections.defaultdict(list)
    want = {int(r['direccion'], 16) for r in dep}
    for r in refs:
        v = int(r['direccion'], 16)
        if v in want:
            by_addr[v].append(r)

    # ---------------------------------------------------------------- donde se escribe cada direccion en la fuente
    rx = re.compile(r'0[xX]([0-9A-Fa-f]{5,8})\b')
    src_hits = collections.defaultdict(list)  # va -> [(archivo, linea, texto, es_codigo)]
    for p in sorted((repo / 'source').rglob('*')):
        if p.suffix.lower() not in ('.cpp', '.h', '.hpp', '.inc', '.c'):
            continue
        try:
            lines = p.read_text(encoding='utf-8', errors='replace').splitlines()
        except Exception:
            continue
        rel = p.relative_to(repo).as_posix()
        inblock = False
        for n, line in enumerate(lines, 1):
            code = line
            comment_only = False
            if inblock:
                k = line.find('*/')
                if k < 0:
                    comment_only = True
                    code = ''
                else:
                    inblock = False
                    code = line[k + 2:]
            # quitar comentarios de bloque en la misma linea y de linea
            code2 = re.sub(r'/\*.*?\*/', ' ', code)
            if '/*' in code2:
                code2 = code2[:code2.find('/*')]
                inblock = True
            k = code2.find('//')
            if k >= 0:
                code2 = code2[:k]
            for m in rx.finditer(line):
                v = int(m.group(1), 16)
                if v in want:
                    in_code = bool(rx.search(code2)) and v in {int(x, 16) for x in rx.findall(code2)}
                    src_hits[v].append((rel, n, line.strip()[:110], in_code))

    # ---------------------------------------------------------------- desensamblado
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    memo = {}

    def descend(entry):
        """(bytes, call directos, n indirectas, usos IAT, 1ra instruccion, n instrucciones, llega a ret)"""
        if entry in memo:
            return memo[entry]
        seen, work, targets, indirect, iat_use = set(), [entry], set(), 0, set()
        lo = hi = entry
        hit_ret = False
        fl = set()
        while work:
            p = work.pop()
            while p not in seen:
                o = off(p)
                if o is None or not intext(p):
                    break
                ins = next(md.disasm_lite(exe[o:o + 15], p), None)
                if not ins:
                    break
                _, size, mn, ops = ins
                seen.add(p)
                lo, hi = min(lo, p), max(hi, p + size)
                if mn[0] == 'f' and mn not in ('fs',):
                    fl.add('x87')
                if 'fs:[' in ops:
                    fl.add('seh')
                if 'xmm' in ops:
                    fl.add('sse')
                if mn in ('cpuid', 'rdtsc', 'int', 'int3') and mn != 'int3':
                    fl.add(mn)
                if mn == 'call':
                    if ops.startswith('0x'):
                        targets.add(int(ops, 16))
                    else:
                        m = re.search(r'dword ptr \[(0x[0-9a-f]+)\]', ops)
                        if m and IAT[0] <= int(m.group(1), 16) < IAT[1]:
                            iat_use.add(int(m.group(1), 16))
                        else:
                            indirect += 1
                elif mn in ('ret', 'retn'):
                    hit_ret = True
                    break
                elif mn == 'jmp':
                    if ops.startswith('0x'):
                        t = int(ops, 16)
                        if t in H or (t in BF and t != entry and abs(t - p) > 0x2000):
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
                                if e is None or not (p - 0x4000 <= e <= p + 0x8000) or not intext(e):
                                    break
                                work.append(e)
                    else:
                        m = re.search(r'dword ptr \[(0x[0-9a-f]+)\]', ops)
                        if m and IAT[0] <= int(m.group(1), 16) < IAT[1]:
                            iat_use.add(int(m.group(1), 16))
                        else:
                            indirect += 1
                    break
                elif mn.startswith('j') and ops.startswith('0x'):
                    work.append(int(ops, 16))
                p += size
        memo[entry] = (hi - lo if seen else 0, targets, indirect, iat_use, len(seen), hit_ret, fl)
        return memo[entry]

    def first_insns(v, n=8):
        out = []
        p = v
        for _ in range(n):
            o = off(p)
            if o is None:
                break
            ins = next(md.disasm_lite(exe[o:o + 15], p), None)
            if not ins:
                out.append('(no decodifica)')
                break
            _, size, mn, ops = ins
            out.append((mn + ' ' + ops).strip())
            p += size
            if mn in ('ret', 'retn'):
                break
        return out

    PRO = {0x55, 0x53, 0x56, 0x57, 0x83, 0x81, 0x8B, 0x6A, 0x68, 0x51, 0x52, 0x64, 0xA1, 0x33, 0x0F, 0xD9, 0xDD, 0xDB, 0xF6, 0x80,
           0x8A, 0x31, 0x85, 0x3B, 0xB8, 0xC3, 0xC2, 0x8D, 0xE9, 0xFF, 0xD8, 0xDC, 0xDE, 0x32, 0x2B, 0x0B, 0x66, 0x6B, 0xB9, 0x89, 0xC7}

    def estructura(v):
        o = off(v)
        if o is None:
            return dict(prev='fuera_de_texto', thunk='', prologo=0, decod=0)
        b = exe[o - 1]
        b3 = exe[o - 3]
        prev = 'relleno' if b in (0xCC, 0x90, 0x00) else ('ret' if b == 0xC3 else ('ret_imm' if b3 == 0xC2 else 'otro'))
        fi = first_insns(v, 3)
        thunk = ''
        if fi and (fi[0].startswith('jmp 0x') or fi[0].startswith('jmp dword ptr')):
            thunk = fi[0]
        elif len(fi) >= 2 and fi[0].startswith('mov ecx') and fi[1].startswith('jmp 0x'):
            thunk = fi[0] + ' ; ' + fi[1]
        return dict(prev=prev, thunk=thunk, prologo=int(exe[o] in PRO), decod=int('(no decodifica)' not in fi))

    # ---------------------------------------------------------------- categoria / modulo
    QT = (0x818000, 0x81B000)

    def objs_of(v):
        return sorted({r['obj'] for r in by_addr.get(v, [])})

    def objfam(o):
        n = o.lower().replace('.obj', '')
        return OBJ_RW.get(n)

    rows = []
    for d in dep:
        v = int(d['direccion'], 16)
        conf = 'B' if v in PROMOVER else d['confianza']
        objs = objs_of(v)
        fams = {objfam(o) for o in objs if objfam(o)}
        wd = W.get(v)
        w_mod = wd['clase_modulo'] if wd else ''
        w_est = wd['estado'] if wd else ''
        nb, nlo, nhi = neighbour_module(v) if 0x750000 <= v < 0x820000 else ('', '', '')
        mod, src_mod = '', ''
        if wd and wd['clase_modulo']:
            mod, src_mod = wd['clase_modulo'], 'mapa RenderWare (%s)' % wd['confianza']
        elif v in BF and BF[v]['name'] and not BF[v]['name'].startswith('FUN_') and not BF[v]['name'].startswith('thunk_FUN'):
            src_mod = 'Ghidra'
        # biblioteca / categoria
        lib = ''
        if QT[0] <= v < QT[1] or 'QuickTime' in w_mod or any('QTML' in o for o in objs):
            lib = 'QuickTime'
        elif v >= 0x820000:
            lib = 'CRT'
        elif 'D3DX9' in (w_mod or '') or 'd3dx9math.obj' in objs or (not wd and nb == 'D3DX9 (compilador de shaders)'):
            lib = 'D3DX9'
        elif 'libpng' in (w_mod or '') or 'rtpng.obj' in objs or (not wd and nb.startswith('libpng')) or 'Zlib' in (w_mod or ''):
            lib = 'libpng+zlib'
        if not mod:
            if fams:
                mod, src_mod = sorted(fams)[0], 'archivo llamador (%s)' % ','.join(sorted(o for o in objs if objfam(o)))
            elif nb:
                mod, src_mod = nb, 'vecindad en el mapa RenderWare'
            elif 0x750000 <= v < 0x820000:
                mod, src_mod = '', ''
        if 0x401000 <= v < 0x750000 and not mod:
            g = G.get(v)
            if g and g['modulo']:
                mod, src_mod = g['modulo'], 'mapa del juego (%s)' % g['confianza_modulo']
        if not mod and objs:
            mod = objs[0].replace('.obj', '')
            src_mod = 'archivo llamador'
        if lib:
            mod, src_mod = {'QuickTime': 'QuickTime/QTML (puentes)', 'CRT': 'CRT y bibliotecas estaticas', 'D3DX9': 'D3DX9 (compilador de shaders)',
                            'libpng+zlib': 'libpng + zlib'}[lib], src_mod or 'region/archivo llamador'
        elif any(o.startswith(('LibJpeg', 'JPegCompress')) for o in objs):
            mod, src_mod = 'libjpeg (IJG, callbacks de LibJpeg*.cpp)', 'archivo llamador'
        no_rec = v in NR or w_est == 'no_reconstruible'
        if conf == 'C':
            cat = 'ruido'
        elif no_rec:
            cat = 'RenderWare no reconstruible'
        elif v in EXC:
            cat = 'excluida'
        elif lib:
            cat = 'biblioteca: ' + lib
        elif w_est == 'enlazada':
            cat = 'biblioteca: ' + (w_mod or 'enlazada')
        elif fams or (0x749000 <= v < 0x820000):
            cat = 'RenderWare interno'
        else:
            cat = 'juego'
        rows.append(dict(v=v, d=d, conf=conf, cat=cat, mod=mod, src_mod=src_mod, objs=objs, lib=lib, w_est=w_est))

    # ---------------------------------------------------------------- cierre por raiz
    root_set_f = {}
    for r in rows:
        if r['conf'] in ('A', 'B'):
            v = r['v']
            root_set_f[v] = v if v in BF else (func_of(v) or v)
    rootf = set(root_set_f.values())

    def closure(seed):
        seen = {}
        stack = [seed]
        ind = 0
        while stack:
            f = stack.pop()
            if f in seen or f in H:
                continue
            nb, tg, indc, iu, _, _, _ = descend(f)
            seen[f] = nb
            ind += indc
            for t in tg:
                if t not in seen:
                    stack.append(t)
        return seen, ind

    cl = {}
    reach_count = collections.Counter()
    for v, f in root_set_f.items():
        if f not in cl:
            cl[f] = closure(f)
            for x in cl[f][0]:
                reach_count[x] += 1
    edges = []
    for v, f in root_set_f.items():
        seen, ind = cl[f]
        for x in seen:
            if x in rootf and x != f:
                edges.append((f, x))
    edges = sorted(set(edges))
    wcsv(OUT / 'grafo_raices.csv', [dict(raiz=f'0x{a:08X}', depende_de=f'0x{b:08X}') for a, b in edges], ['raiz', 'depende_de'])
    dep_of = collections.defaultdict(list)
    for a_, b_ in edges:
        dep_of[a_].append(b_)

    wcsv(OUT / 'cierre_por_raiz.csv', [dict(raiz=f'0x{f:08X}', funcion=f'0x{x:08X}', bytes=nb_) for f in sorted(cl) for x, nb_ in sorted(cl[f][0].items())],
         ['raiz', 'funcion', 'bytes'])

    # ---------------------------------------------------------------- filas finales
    out = []
    for r in rows:
        v, d = r['v'], r['d']
        st = estructura(v)
        nbytes, targets, indc, iu, ninst, hit_ret, flags = descend(v)
        bf = BF.get(v)
        contenida = d['contenida_en']
        size = int(bf['body_bytes']) if bf and bf['body_bytes'] else nbytes
        size_src = 'Ghidra' if bf and bf['body_bytes'] else 'recorrido de flujo'
        f = root_set_f.get(v)
        seen = cl[f][0] if f in cl else {}
        excl = sum(1 for x in seen if reach_count[x] == 1)
        # nombre y fuente
        name, name_src = '', ''
        if bf and bf['name'] and not bf['name'].startswith('FUN_') and not bf['name'].startswith('thunk_FUN'):
            name, name_src = bf['name'], 'Ghidra/simbolos'
        elif v in ANC and ANC[v]['evidencia']:
            name, name_src = 'cadena: ' + ANC[v]['evidencia'][:40].strip(), 'strings'
        elif bf and bf['name']:
            name, name_src = bf['name'], 'Ghidra (automatico)'
        hits = src_hits.get(v, [])
        code_hits = [h for h in hits if h[3]]
        wrapper = ''
        texts = ' '.join(h[2] for h in code_hits)
        if code_hits:
            if re.search(r'plugin::Call|CallMethod|CallAndReturn', texts):
                wrapper = 'envoltura plugin::Call'
            elif re.search(r'reinterpret_cast|\(\s*\*?\s*\(?\w[\w:<>\s\*&,]*\s*\(\s*__\w+\s*\*\s*\)', texts) or re.search(r'\)\s*0x', texts) or '0x' in texts:
                wrapper = 'puntero por direccion fija'
        elif hits:
            wrapper = 'solo en comentarios'
        else:
            wrapper = 'no aparece en la fuente (escrita en forma distinta)'
        fmts = d['formas']
        callers = '; '.join('%s:%d' % (h[0].replace('source/game_sa/', ''), h[1]) for h in code_hits[:4])
        if len(code_hits) > 4:
            callers += '; +%d mas' % (len(code_hits) - 4)
        if not callers and hits:
            callers = '(comentario) %s:%d' % (hits[0][0].replace('source/game_sa/', ''), hits[0][1])
        # equivalente propia sin hook: una definicion con la direccion en un comentario encima cuyo cuerpo no llama al EXE
        equiv = ''
        if v in EXC:
            equiv = 'excluida (' + EXC[v]['categoria'] + ')'
        else:
            for h in [h for h in hits if not h[3]][:3]:
                lines = (repo / h[0]).read_text(encoding='utf-8', errors='replace').splitlines()
                i = h[1]  # indice base 0 de la linea siguiente al comentario
                k = i
                while k < min(len(lines), i + 8) and '{' not in lines[k]:
                    k += 1
                if k >= min(len(lines), i + 8) or lines[k].lstrip().startswith('//'):
                    continue
                depth, body, j = 0, [], k
                while j < min(len(lines), k + 60):
                    depth += lines[j].count('{') - lines[j].count('}')
                    body.append(lines[j])
                    j += 1
                    if depth <= 0:
                        break
                txt = chr(10).join(body)
                if len(body) > 2 and not re.search(r'plugin::Call|CallMethod|CallAndReturn|0[xX][0-9A-Fa-f]{6}', txt) and re.search(r'(return|if|for|while)', txt):
                    equiv = 'posible cuerpo propio sin hook: %s:%d (revisar)' % (h[0].replace('source/game_sa/', ''), k + 1)
                    break
        if not equiv:
            hk = [h for h in code_hits if re.search(r'HookInstall|RH_Scoped\w*Install|RH_ScopedGlobalInstall', h[2])]
            if hk:
                equiv = 'hook propio instalado y fuera del catalogo: %s:%d (catalogar y validar)' % (hk[0][0].replace('source/game_sa/', ''), hk[0][1])
        m_t = re.match(r'jmp (0x[0-9a-f]+)$', st['thunk'])
        if m_t and not equiv:
            tv = int(m_t.group(1), 16)
            if tv in H:
                equiv = 'thunk a 0x%08X, que ya tiene hook propio (%s)' % (tv, '::'.join(hooks_info[tv][:2]) if tv in hooks_info else 'catalogado')
            else:
                equiv = 'thunk a 0x%08X (sin hook)' % tv
        # veredicto automatico
        if r['conf'] == 'C':
            auto = 'ruido' if not (st['prev'] in ('relleno', 'ret', 'ret_imm') and st['prologo'] and st['decod']) else 'ruido_con_forma_de_funcion'
        elif contenida:
            auto = 'interior_de_funcion'
        elif bf and bf['body_bytes']:
            auto = 'funcion'
        elif st['prev'] in ('relleno', 'ret', 'ret_imm') and st['prologo'] and st['decod'] and hit_ret:
            auto = 'funcion'
        elif st['decod'] and st['prologo']:
            auto = 'dudosa'
        else:
            auto = 'no_funcion'
        out.append(dict(
            direccion=f'0x{v:08X}', nombre=name, fuente_nombre=name_src, modulo=('' if r['conf'] == 'C' else r['mod']), fuente_modulo=('' if r['conf'] == 'C' else r['src_mod']), bytes=size,
            fuente_bytes=size_src, categoria=r['cat'], confianza=('probable (promovida de ruido)' if r['v'] in PROMOVER else {'A': 'segura', 'B': 'probable', 'C': 'ruido'}[r['conf']]),
            llamadas_desde='; '.join(r['objs'][:4]) + ('; +%d' % (len(r['objs']) - 4) if len(r['objs']) > 4 else ''),
            llamadoras_fuente=callers, texto_fuente=(code_hits[0][2] if code_hits else (hits[0][2] if hits else '')), forma_uso=fmts, tipo_uso=wrapper, es_thunk=st['thunk'], funcion_propia_sin_hook=equiv,
            cierre_total=(len(seen) if f in cl else 0), cierre_bytes=(sum(seen.values()) if f in cl else 0), cierre_exclusivo=excl,
            flags_cuerpo=','.join(sorted(flags)), flags_cierre=','.join(sorted(set().union(*[memo[x][6] for x in seen if x in memo]) if seen else set())),
            llamadoras_en_exe=len(callers_exe.get(v, ())), llamadoras_en_exe_sin_hook=sum(1 for c_ in callers_exe.get(v, ()) if c_ not in H),
            raices_de_las_que_depende=len(dep_of.get(f, [])), llamadas_indirectas=indc, excluida=('si' if v in EXC else ''),
            contenida_en=contenida, es_inicio_ghidra=d['es_inicio_ghidra'], prev=st['prev'], prologo=st['prologo'],
            veredicto_auto=auto, primeras=' ; '.join(first_insns(v, 4))[:160]))
    out.sort(key=lambda x: x['direccion'])
    fields = list(out[0])
    wcsv(R / 'ExeIndependiente/raices.csv', out, fields)
    resumen = dict(
        filas=len(out), por_categoria=dict(collections.Counter((o['categoria'], o['confianza']) .__str__() for o in out)),
        por_confianza=dict(collections.Counter(o['confianza'] for o in out)),
        por_veredicto=dict(collections.Counter((o['confianza'], o['veredicto_auto']).__str__() for o in out)),
        hooked_ahora=[o['direccion'] for o in out if int(o['direccion'], 16) in H])
    (OUT / 'resumen_raices.json').write_text(json.dumps(resumen, indent=1, ensure_ascii=False), encoding='utf-8')
    print(json.dumps(resumen, indent=1, ensure_ascii=False))


if __name__ == '__main__':
    main()
