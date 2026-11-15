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
}
BLOCK = 0x10000
GAP = 0x3000  # distancia máxima (bytes) a las funciones con nombre vecinas para dar confianza «alta»
FILE_MODULE = {
    'rwcore': 'RwCore', 'rwplcore': 'RwPlCore', 'rpworld': 'RpWorld', 'rpmatfx': 'RpMatFX', 'rpuvanim': 'RpUVAnim',
    'rtanim': 'RtAnim', 'rphanim': 'RpHAnim', 'rpskin': 'RpSkin', 'rtdict': 'RtDict',
}
ADDR = re.compile(r'0x([78][0-9A-Fa-f]{5})\b')
NAME = re.compile(r'([A-Za-z_]\w*)\s*(?:<[^>]*>)?\s*\(')
SKIP_NAMES = {'Call', 'CallMethod', 'CallAndReturn', 'CallMethodAndReturn', 'if', 'sizeof', 'static_cast', 'reinterpret_cast',
              'RH_ScopedInstall', 'RH_ScopedGlobalInstall', 'RH_ScopedNamedGlobalInstall', 'RH_ScopedOverloadedInstall'}
PREFIX = (('_rwD3D9', 'RwD3D9'), ('RwD3D9', 'RwD3D9'), ('rwD3D9', 'RwD3D9'), ('_rwD3D', 'RwD3D9'), ('Rw', 'RwCore'), ('_rw', 'RwCore'),
          ('Rp', 'RpWorld'), ('Rt', 'RtAnim'), ('Rs', 'Rs'), ('Ps', 'Ps'))


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
        names = [n for n in NAME.findall(text) if n not in SKIP_NAMES]
        name = names[0] if names else ''
        mod = FILE_MODULE.get(base) or _module_of_name(name)
        if mod and addrs[0] not in res:
            res[addrs[0]] = (name, mod)
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
    known = {}
    for va in rw:
        names = funcs[va]['names']
        if names and va in covered:
            known[va] = (names[0], names[0].split('::')[0])
        elif va in anchors:
            known[va] = anchors[va]
        elif names:
            mod = _module_of_name(names[0].split('::')[0])
            if mod:
                known[va] = (names[0], mod)
    keys = sorted(known)

    def module(va):
        if va in known:
            return known[va][1], 'exacta', known[va][0]
        i = bisect.bisect_left(keys, va)
        prev = known[keys[i - 1]][1] if i > 0 else None
        nxt = known[keys[i]][1] if i < len(keys) else None
        near = lambda k: abs(k - va) <= GAP
        if prev and prev == nxt and near(keys[i - 1]) and near(keys[i]):
            return prev, 'alta', ''
        return (prev or nxt or '?'), 'baja', ''

    rows = []
    for va in rw:
        f = funcs[va]
        if va in pending:
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
         '[Lista completa (CSV)](funciones.csv) · [Pendientes por clase](pendientes-por-clase.md) · [Mapa del código del juego](../JuegoExe/README.md)', '',
         f'Región **0x750000–0x81FFFF**: {n:,} funciones, {nb:,} bytes. Es RenderWare 3.7 y los envoltorios D3D9 enlazados estáticamente. '
         'Generado por `contrib/rw_function_map.py` (mismos datos y cotas que `contrib/exe_coverage.py`).', '',
         'Estas funciones **no cuentan** en las cifras de reconstrucción del juego: para un ejecutable independiente se sustituyen enlazando una implementación '
         'equivalente (por ejemplo librw) y reconstruyendo solo los pipelines propios de San Andreas. Este mapa sirve para saber qué hay y por dónde empezar si se quiere reconstruir.', '',
         '## Estado', '']
    body = []
    for k in ['A', 'B', 'C', 'D', 'envoltura', 'reemplazada']:
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


if __name__ == '__main__':
    r = write(Path(__file__).resolve().parents[1])
    print(len(r), 'funciones')
