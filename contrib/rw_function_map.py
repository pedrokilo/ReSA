"""Mapa de las funciones de RenderWare y D3D del EXE (0x750000–0x81FFFF) en docs/RenderWareExe/.

Misma idea que docs/JuegoExe/ (contrib/game_function_map.py), pero para la región de bibliotecas enlazadas: RenderWare 3.7 y los
envoltorios D3D9. Genera funciones.csv (todas las funciones de la región), README.md (resumen, por bloque y por clase/módulo) y
pendientes-por-clase.md (lo que falta, agrupado por clase o módulo de RenderWare).
Como el EXE no trae símbolos, la clase/módulo de una función se obtiene así:
  1. «exacta»: la función aparece con nombre en el catálogo (ya reemplazada) o en las cabeceras/archivos de source/game_sa/RenderWare/rw
     (cada archivo rw*.h/.cpp corresponde a un módulo de RenderWare) o en el resto del código fuente con prefijo Rw/Rp/Rt/Rs/_rw.
  2. «alta»/«baja»: HEURÍSTICA de vecindad: el SDK se enlazó por unidad de compilación, así que se toma el módulo de las funciones con
     nombre más cercanas (anterior y siguiente); si coinciden la confianza es «alta», si no «baja». No es una identificación.
"""
import bisect
import csv
import re
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import exe_coverage as ec  # noqa: E402

STATUS = {
    'reemplazada': 'Reemplazada por C++',
    'envoltura': 'Envoltura pendiente (la DLL aún llama al EXE)',
    'A': 'A · Falta: llamada desde código ya reemplazado',
    'B': 'B · Falta: método virtual / por tabla',
    'C': 'C · Falta: solo la llaman funciones aún no reemplazadas',
    'D': 'D · Falta: sin llamadas ni vtable (¿muerto o por puntero?)',
    'enlazada': 'Biblioteca de terceros: se enlaza la original, no se reconstruye',
    'no_reconstruible': 'No reconstruible en C++ puro (ABI de registros, 3DNow!, fragmento) o sustituida por otra API (QuickTime, DirectDraw) · documentada',
}
ENLAZADAS = ('libpng + zlib', 'D3DX9 (compilador de shaders)')  # decision del usuario (2026-11-16): se enlazan, no se reconstruyen
BLOCK = 0x10000
GAP = 0x4000  # distancia máxima (bytes) a las funciones con nombre vecinas para dar confianza «alta»
FILE_MODULE = {
    'rwcore': 'RwCore', 'rwplcore': 'RwPlCore', 'rpworld': 'RpWorld', 'rpmatfx': 'RpMatFX', 'rpuvanim': 'RpUVAnim',
    'rtanim': 'RtAnim', 'rphanim': 'RpHAnim', 'rpskin': 'RpSkin', 'rtdict': 'RtDict', 'rtbmp': 'RtBMP', 'rtquat': 'RtQuat',
    'rtslerp': 'RtSlerp', 'rpanisot': 'RpAnisot', 'rwtexdict': 'RwCore', 'rpdbgerr': 'RwCore',
}
ADDR = re.compile(r'0x([78][0-9A-Fa-f]{5})\b')
NAME = re.compile(r'([A-Za-z_]\w*)\s*(?:<[^>]*>)?\s*\(')
SKIP_NAMES = {'Call', 'CallMethod', 'CallAndReturn', 'CallMethodAndReturn', 'if', 'sizeof', 'static_cast', 'reinterpret_cast',
              'RH_ScopedInstall', 'RH_ScopedGlobalInstall', 'RH_ScopedNamedGlobalInstall', 'RH_ScopedOverloadedInstall'}
D3D9 = 'Controlador D3D9 (RwD3D9)'
PREFIX = (('_rwD3D9', D3D9), ('RwD3D9', D3D9), ('rwD3D9', D3D9), ('_rwD3D', D3D9), ('RpHAnim', 'RpHAnim'), ('RpSkin', 'RpSkin'),
          ('RpMatFX', 'RpMatFX'), ('RpUVAnim', 'RpUVAnim'), ('RtAnim', 'RtAnim'), ('RtDict', 'RtDict'), ('Rw', 'RwCore'), ('_rw', 'RwCore'),
          ('Rp', 'RpWorld'), ('Rt', 'RtAnim'), ('Rs', 'Rs'), ('Ps', 'Ps'))
# Evidencia de cadenas (el EXE no trae símbolos): una función que referencia cadenas de estos grupos pertenece a ese módulo.
# Cada grupo solo se aplica dentro de su rango de direcciones, para descartar coincidencias casuales de valores de 32 bits.
D3DX = 'D3DX9 (compilador de shaders)'
PNG = 'libpng + zlib'
STRING_RULES = (
    (D3DX, 0x764000, 0x7C4600, ('internal error', 'semantic', 'register', 'instruction', 'macro', 'preprocessor', 'shader', 'texcoord',
                                'swizzle', 'pixel shader', 'D3DX', 'DIRECT3D', 'fragment', 'not yet implemented', 'cannot ', 'expression',
                                'GenuineIntel', 'PSGP', 'DisableMMX', 'not clamped', 'syntax error', 'unexpected', 'argument pool')),
    (PNG, 0x7D0000, 0x7EA000, ('IHDR', 'PLTE', 'IDAT', 'IEND', 'gAMA', 'sRGB', 'iCCP', 'sPLT', 'sBIT', 'cHRM', 'tRNS', 'bKGD', 'hIST',
                               'tEXt', 'zTXt', 'oFFs', 'pCAL', 'sCAL', 'pHYs', '1.0.12')),
)
PNG_FILE = {'rtpng': 'libpng + zlib'}
# Clase de hooks.csv -> módulo real (las funciones ya reconstruidas se clasifican por la clase con la que se enganchan, no por vecindad).
CLASS_MODULE = {'ZlibInflate': 'libpng + zlib', 'D3DX9': 'D3DX9 (compilador de shaders)', 'RwD3D9': D3D9, 'RpUVAnim': 'RpUVAnim', 'RpSkin': 'RpSkin',
                'RpHAnim': 'RpHAnim', 'RtAnim': 'RtAnim', 'RtSlerp': 'RtSlerp', 'RtQuat': 'RtQuat', 'RtBMP': 'RtBMP', 'RtDict': 'RtDict',
                'RpMatFX': 'RpMatFX', 'RpWorld': 'RpWorld', 'RwCore': 'RwCore', 'RwPlCore': 'RwPlCore', 'RpAnisot': 'RpAnisot'}
EXE_CANDIDATES = ('RESA_EXE', 'E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe', 'E:/JUEGOS/GTA San Andreas/gta_sa.exe')


def _module_of_name(name):
    for pre, mod in PREFIX:
        if name.startswith(pre):
            return mod
    return None


def source_anchors(root):
    """{va: (nombre, módulo)} leído del código fuente (cabeceras y .cpp con la dirección en el comentario o en plugin::Call)."""
    out = subprocess.run(['git', 'grep', '-n', '-E', r'0x[78][0-9A-Fa-f]{5}\b', '--', 'source'], cwd=root, capture_output=True,
                         text=True, encoding='utf-8', errors='replace').stdout.split('\n')
    res = {}
    for line in out:
        m = re.match(r'([^:]+):\d+:(.*)', line)
        if not m:
            continue
        path, text = m.group(1), m.group(2)
        addrs = [int(a, 16) for a in ADDR.findall(text)]
        if len(addrs) != 1 or not (ec.GAME_END <= addrs[0] < ec.RW_END):
            continue
        base = Path(path).stem.lower()
        names = [n for n in NAME.findall(text) if n not in SKIP_NAMES and _module_of_name(n)]
        name = names[0] if names else ''
        by_name = _module_of_name(name)
        mod = by_name if by_name == D3D9 else (PNG_FILE.get(base) or FILE_MODULE.get(base) or by_name)
        if mod and addrs[0] not in res:
            res[addrs[0]] = (name, mod)
    return res


def _exe_bytes():
    import os
    for c in EXE_CANDIDATES:
        path = os.environ.get(c) if c == 'RESA_EXE' else c
        if path and Path(path).exists():
            return Path(path).read_bytes()
    return None


def string_anchors(root, rw, funcs):
    """{va: (módulo, evidencia)} por cadenas del .rdata que referencia el cuerpo de cada función.
    Necesita gta_sa.exe (variable RESA_EXE o las rutas de EXE_CANDIDATES); sin él se usa el caché docs/RenderWareExe/anclas.csv."""
    cache = Path(root) / 'docs/RenderWareExe/anclas.csv'
    exe = _exe_bytes()
    if exe is None:
        if not cache.exists():
            return {}
        with open(cache, encoding='utf-8', newline='') as f:
            return {ec._hex(r['va']): (r['modulo'], r['evidencia']) for r in csv.DictReader(f)}
    import struct
    ranges = defaultdict(list)
    with open(Path(root) / 'docs/MemoryMap/binary_function_ranges.csv', encoding='utf-8-sig', newline='') as f:
        for r in csv.DictReader(f):
            ranges[ec._hex(r['function_va'])].append((ec._hex(r['start_va']), ec._hex(r['end_va'])))
    text_va, text_off, text_size = 0x401000, 1024, 4546560
    rd_va, rd_off, rd_end = 0x858000, 4548608, 0x8A4000
    res = {}
    for va in rw:
        rules = [r for r in STRING_RULES if r[1] <= va < r[2]]
        if not rules:
            continue
        found = defaultdict(set)
        for a, b in ranges.get(va, [(va, va + funcs[va]['size'])]):
            if not (text_va <= a < text_va + text_size):
                continue
            o = a - text_va + text_off
            body = exe[o:o + (b - a + 1)]
            for i in range(len(body) - 3):
                v = struct.unpack_from('<I', body, i)[0]
                if rd_va <= v < rd_end:
                    po = v - rd_va + rd_off
                    txt = exe[po:po + 120].split(b'\x00')[0]
                    if len(txt) >= 4 and all(32 <= c < 127 for c in txt):
                        text = txt.decode()
                        for mod, lo, hi, kws in rules:
                            if any(k in text for k in kws):
                                found[mod].add(text[:40])
        if found:
            mod = max(found, key=lambda m: len(found[m]))
            res[va] = (mod, ' | '.join(sorted(found[mod])[:3]))
    with open(cache, 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['va', 'modulo', 'evidencia'])
        for va, (m, e) in sorted(res.items()):
            w.writerow([f'0x{va:08X}', m, e])
    return res


def build(root):
    root = Path(root)
    out, st = ec.compute(root, internals=True)
    funcs, covered, pending, groups = st['funcs'], st['covered'], st['pending'], st['groups']
    rw = sorted(va for va in funcs if ec.region(va) == 'renderware')
    group_of = {va: k for k, v in groups.items() for va in v}
    anchors = source_anchors(root)
    callers_total, callers_repl = Counter(), Counter()
    with open(root / 'docs/MemoryMap/binary_calls.csv', encoding='utf-8-sig', newline='') as f:
        for r in csv.DictReader(f):
            if r['kind'] != 'direct':
                continue
            caller, target = ec._hex(r['caller_va']), ec._hex(r['target_va'])
            callers_total[target] += 1
            if caller in covered:
                callers_repl[target] += 1
    strs = string_anchors(root, rw, funcs)
    known = {}
    for va in rw:
        names = funcs[va]['names']
        if names and va in covered:
            known[va] = (names[0], names[0].split('::')[0], 'exacta')
        elif va in anchors:
            known[va] = (anchors[va][0], anchors[va][1], 'exacta')
        elif names and _module_of_name(names[0].split('::')[0]):
            known[va] = (names[0], _module_of_name(names[0].split('::')[0]), 'exacta')
        elif va in strs:
            known[va] = ('', strs[va][0], 'cadenas')
    keys = sorted(known)
    calls = defaultdict(set)
    with open(root / 'docs/MemoryMap/binary_calls.csv', encoding='utf-8-sig', newline='') as f:
        for r in csv.DictReader(f):
            if r['kind'] == 'direct':
                a, b = ec._hex(r['caller_va']), ec._hex(r['target_va'])
                if a != b and ec.GAME_END <= a < ec.RW_END and ec.GAME_END <= b < ec.RW_END:
                    calls[a].add(b)
                    calls[b].add(a)

    def module(va):
        if va in known:
            return known[va][1], known[va][2], known[va][0]
        i = bisect.bisect_left(keys, va)
        pk = keys[i - 1] if i > 0 else None
        nk = keys[i] if i < len(keys) else None
        prev = known[pk][1] if pk else None
        nxt = known[nk][1] if nk else None
        near = lambda k: abs(k - va) <= GAP
        if prev and prev == nxt and near(pk) and near(nk):
            return prev, 'alta', ''
        # llamadas directas con funciones identificadas: si todas coinciden (al menos dos), se toma ese módulo
        votes = Counter(known[n][1] for n in calls[va] if n in known)
        if sum(votes.values()) >= 2 and len(votes) == 1:
            return next(iter(votes)), 'media', ''
        if prev and prev == nxt:
            return prev, 'media', ''
        if pk is not None and nk is not None:
            return (prev if va - pk <= nk - va else nxt), 'baja', ''
        return (prev or nxt or '?'), 'baja', ''

    hookclass = {}
    with open(root / 'docs/hooks.csv', encoding='utf-8-sig', newline='') as f:
        for r in csv.DictReader(f):
            a = ec._hex(r['address'])
            if a and r['reversed'] == '1':
                hookclass.setdefault(a, r['class'])
    noreco = {}
    nr = root / 'docs/RenderWareExe/no-reconstruibles.csv'
    if nr.exists():
        with open(nr, encoding='utf-8', newline='') as f:
            noreco = {ec._hex(r['va']): r for r in csv.DictReader(f)}
    rows = []
    for va in rw:
        f = funcs[va]
        if va in noreco:
            status = 'no_reconstruible'
        elif va in pending:
            status = 'envoltura'
        elif va in covered:
            status = 'reemplazada'
        elif callers_repl[va]:
            status = 'A'
        elif va in st['vtable_targets']:
            status = 'B'
        else:
            status = 'C' if callers_total[va] else 'D'
        mod, conf, name = module(va)
        if hookclass.get(va) in CLASS_MODULE:
            mod, conf = CLASS_MODULE[hookclass[va]], 'exacta'
        if va in noreco:
            mod, conf = noreco[va]['modulo'], 'exacta'
        if mod == 'RpWorld' and 0x765200 <= va <= 0x76FFFF:
            mod, conf = 'D3DX9 (compilador de shaders)', 'media'  # RpWorld real acaba en ~0x765200 (R21A); lo que sigue es D3DX9 atribuido por vecindad
        if mod in ENLAZADAS and status not in ('reemplazada', 'no_reconstruible'):
            status = 'enlazada'
        rows.append(dict(va=va, size=f['size'], status=status, module=mod, confidence=conf, name=name or ';'.join(f['names'][:3]),
                         callers=callers_total[va], callers_replaced=callers_repl[va], thunk='sí' if f['thunk'] else '',
                         vtable='sí' if va in st['vtable_targets'] else ''))
    return out, rows


def _tbl(head, body):
    return ['| ' + ' | '.join(head) + ' |', '| ' + ' | '.join('---:' if i else '---' for i in range(len(head))) + ' |'] + \
           ['| ' + ' | '.join(str(c) for c in r) + ' |' for r in body]


def write(root):
    root = Path(root)
    _, rows = build(root)
    d = root / 'docs/RenderWareExe'
    d.mkdir(exist_ok=True)
    n, nb = len(rows), sum(r['size'] for r in rows)
    with open(d / 'funciones.csv', 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['va', 'bytes', 'estado', 'clase_modulo', 'confianza', 'nombre', 'llamadas_directas', 'llamadas_desde_reemplazadas', 'thunk', 'en_vtable'])
        for r in rows:
            w.writerow([f"0x{r['va']:08X}", r['size'], r['status'], r['module'], r['confidence'], r['name'], r['callers'], r['callers_replaced'], r['thunk'], r['vtable']])
    by = defaultdict(list)
    for r in rows:
        by[r['status']].append(r)
    L = [f'# RenderWare y D3D en el EXE: mapa de las {n:,} funciones', '',
         '[Lista completa (CSV)](funciones.csv) · [Pendientes por clase](pendientes-por-clase.md) · [Plan de validación](plan-validacion.md) · [D3DX9: enlace y dependencia de DLL](d3dx9-enlace.md) · [Mapa del código del juego](../JuegoExe/README.md)', '',
         f'Región **0x750000–0x81FFFF**: {n:,} funciones, {nb:,} bytes. Es RenderWare 3.7 y los envoltorios D3D9 enlazados estáticamente. '
         'Generado por `contrib/rw_function_map.py` (mismos datos y cotas que `contrib/exe_coverage.py`).', '',
         'Estas funciones **no cuentan** en las cifras de reconstrucción del juego: para un ejecutable independiente se sustituyen enlazando una implementación '
         'equivalente (por ejemplo librw) y reconstruyendo solo los pipelines propios de San Andreas. Este mapa sirve para saber qué hay y por dónde empezar si se quiere reconstruir.', '',
         '**Hallazgo:** gran parte de la región (≈0x765000–0x7C4000, más de 800 funciones y 320 KB) no es RenderWare sino **D3DX9** (compilador de shaders HLSL, preprocesador y ensamblador) enlazado estáticamente; '
         'entre 0x7D0000 y 0x7EA000 hay **libpng + zlib**. Ninguno de los dos es código de Rockstar ni de RenderWare: se sustituyen enlazando las bibliotecas originales.', '',
         '## Estado', '']
    body = []
    for k in ['A', 'B', 'C', 'D', 'no_reconstruible', 'enlazada', 'envoltura', 'reemplazada']:
        v = by[k]
        body.append([STATUS[k], f'{len(v):,}', f'{len(v)/n:.1%}', f'{sum(x["size"] for x in v):,}', f'{sum(x["size"] for x in v)/nb:.1%}'])
    L += _tbl(['Estado', 'Funciones', '% func.', 'Bytes', '% bytes'], body)
    exact = sum(1 for r in rows if r['confidence'] == 'exacta')
    L += ['', f'Clase/módulo identificado por nombre: {exact:,} de {n:,} funciones; el resto se estima por vecindad (columna `confianza`).', '',
          '## Por bloque de direcciones (64 KB)', '']
    blocks = defaultdict(list)
    for r in rows:
        blocks[r['va'] // BLOCK].append(r)
    body = []
    for b in sorted(blocks):
        v = blocks[b]
        miss = [x for x in v if x['status'] in 'ABCD']
        top = Counter(x['module'] for x in miss).most_common(3)
        body.append([f'0x{b*BLOCK:06X}', f'{len(v):,}', f'{len(miss):,}', f'{sum(x["size"] for x in miss):,}', ', '.join(f'{m} ({c})' for m, c in top) or '—'])
    L += _tbl(['Bloque', 'Funciones', 'Faltan', 'Bytes que faltan', 'Clases/módulos con más faltantes'], body)
    mods = defaultdict(lambda: [0, 0, 0, 0])
    for r in rows:
        m = mods[r['module']]
        m[0] += 1
        if r['status'] == 'reemplazada':
            m[1] += 1
        elif r['status'] in 'ABCD':
            m[2] += 1
            m[3] += r['size']
    body = [[k, v[0], v[1], v[2], f'{v[3]:,}'] for k, v in sorted(mods.items(), key=lambda kv: (-kv[1][2], kv[0]))]
    L += ['', '## Por clase o módulo', '', 'Ordenado por funciones que faltan.', '']
    L += _tbl(['Clase / módulo', 'Funciones', 'Reemplazadas', 'Faltan', 'Bytes que faltan'], body)
    (d / 'README.md').write_text('\n'.join(L) + '\n', encoding='utf-8')

    miss_by = defaultdict(list)
    for r in rows:
        if r['status'] in 'ABCD':
            miss_by[r['module']].append(r)
    total_miss = sum(len(v) for v in miss_by.values())
    P = ['# Funciones pendientes de RenderWare y D3D, por clase', '',
         f'Generado de `docs/RenderWareExe/funciones.csv`. Total: **{total_miss:,}** funciones sin reemplazar en {len(miss_by):,} clases/módulos (rango 0x750000–0x81FFFF).', '',
         'Estados: **A** llamada desde código ya en C++ · **B** método virtual o por tabla · **C** solo la llaman funciones pendientes · **D** sin llamadas ni vtable.', '',
         'Aviso: cuando la confianza no es «exacta» la clase es una estimación por vecindad de direcciones (en tramos largos sin ninguna función con nombre es poco fiable y casi todo cae en el módulo conocido más cercano).', '', '## Índice', '']
    order = sorted(miss_by, key=lambda k: (len(miss_by[k]), k))
    P += _tbl(['Clase / módulo', 'Funciones', 'Bytes'], [[k, len(miss_by[k]), f'{sum(x["size"] for x in miss_by[k]):,}'] for k in order]) + ['']
    for k in order:
        v = miss_by[k]
        P += [f'## {k}', '', f'{len(v)} funciones, {sum(x["size"] for x in v):,} bytes.', '']
        P += _tbl(['Dirección', 'Bytes', 'Estado', 'Confianza', 'Nombre', 'Llamadas'],
                  [[f'0x{x["va"]:08X}', x['size'], x['status'], x['confidence'], x['name'] or '—', x['callers']] for x in v]) + ['']
    (d / 'pendientes-por-clase.md').write_text('\n'.join(P) + '\n', encoding='utf-8')
    return rows


def reversedclasses_section(root):
    """Líneas Markdown para docs/ReversedClasses.md: RenderWare y D3D (0x750000–0x81FFFF) por clase o módulo, leído de funciones.csv."""
    path = Path(root) / 'docs/RenderWareExe/funciones.csv'
    if not path.exists():
        return []
    with open(path, encoding='utf-8', newline='') as f:
        rows = list(csv.DictReader(f))
    mods = defaultdict(lambda: Counter())
    for r in rows:
        m = mods[r['clase_modulo']]
        m['n'] += 1
        m['bytes'] += int(r['bytes'])
        st = r['estado']
        if st == 'reemplazada':
            m['rep'] += 1
        elif st == 'envoltura':
            m['env'] += 1
        elif st in ('enlazada', 'no_reconstruible'):
            m['fuera'] += 1
        else:
            m['falta'] += 1
            m['falta_bytes'] += int(r['bytes'])
        m['exacta' if r['confianza'] == 'exacta' else ('cad' if r['confianza'] == 'cadenas' else 'est')] += 1
    n = len(rows)
    L = ['<a id="renderware-d3d"></a>', '### RenderWare y D3D por clase o módulo (0x750000–0x81FFFF)', '',
         f'Las {n:,} funciones de esta región, agrupadas por la clase o módulo al que pertenecen. El EXE no trae símbolos, así que la clase sale de nombres '
         'ya conocidos (exacta), de cadenas del `.rdata` que referencia cada función (cadenas) o de la vecindad de direcciones y del grafo de llamadas (estimada). '
         'Detalle por función en [docs/RenderWareExe](RenderWareExe/README.md) y lista de pendientes en [pendientes-por-clase](RenderWareExe/pendientes-por-clase.md).', '',
         '| Clase / módulo | Funciones | Reemplazadas | Envolturas | Faltan | Bytes que faltan | Clase exacta / por cadenas / estimada |',
         '| --- | ---: | ---: | ---: | ---: | ---: | --- |']
    for k, m in sorted(mods.items(), key=lambda kv: (-kv[1]['falta'], kv[0])):
        L.append(f"| {k} | {m['n']:,} | {m['rep']:,} | {m['env']:,} | {m['falta']:,} | {m['falta_bytes']:,} | {m['exacta']} / {m['cad']} / {m['est']} |")
    L.append('')
    return L


if __name__ == '__main__':
    r = write(Path(__file__).resolve().parents[1])
    print(len(r), 'funciones')
