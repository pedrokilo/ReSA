"""Plan de reconstrucción de RenderWare y D3D (0x750000–0x81FFFF) por partes y lotes: docs/RenderWareExe/plan-lotes.md y plan-lotes.csv.

Parte de docs/RenderWareExe/funciones.csv (contrib/rw_function_map.py). Entran todas las funciones que faltan (estados A–D) y las
envolturas pendientes. Reglas, las mismas que el plan del código del juego:
  * un lote tiene como máximo 32 funciones y unos 5 KB; una función de 3 KB o más va sola en su lote («grande»);
  * las funciones de un lote son contiguas dentro de su clase o módulo (mismo módulo, direcciones seguidas);
  * seis lotes forman una parte;
  * orden de trabajo: primero el SDK de RenderWare propiamente dicho (lo que sustituiría librw), después el controlador D3D9, y al
    final las bibliotecas de terceros (libpng + zlib y D3DX9), que se sustituyen enlazando la biblioteca original.
Los lotes se numeran desde LOTE_INICIAL para continuar la numeración del plan del juego (191A–239A).
"""
import csv
import sys
from collections import defaultdict
from pathlib import Path

LOTE_INICIAL = 240
MAX_FUNCS = 32
MAX_BYTES = 5000
GRANDE = 3000
POR_PARTE = 6
ORDEN = ['RwPlCore', 'RwCore', 'Controlador D3D9 (RwD3D9)', 'RpWorld', 'RpMatFX', 'RpSkin', 'RpHAnim', 'RpUVAnim', 'RtAnim', 'RtDict',
         'RtQuat', 'RtSlerp', 'RtBMP', 'RpAnisot', 'Rs', 'Ps', 'libpng + zlib', 'D3DX9 (compilador de shaders)']
GRUPO = {'libpng + zlib': 'Bibliotecas de terceros', 'D3DX9 (compilador de shaders)': 'Bibliotecas de terceros',
         'Controlador D3D9 (RwD3D9)': 'Controlador D3D9'}


def cargar(root):
    with open(Path(root) / 'docs/RenderWareExe/funciones.csv', encoding='utf-8', newline='') as f:
        rows = [r for r in csv.DictReader(f) if r['estado'] not in ('reemplazada', 'enlazada', 'no_reconstruible')]
    for r in rows:
        r['va_int'] = int(r['va'], 16)
        r['bytes'] = int(r['bytes'])
    return rows


def lotes(rows):
    por_mod = defaultdict(list)
    for r in rows:
        por_mod[r['clase_modulo']].append(r)
    mods = [m for m in ORDEN if m in por_mod] + sorted(m for m in por_mod if m not in ORDEN)
    out = []
    for m in mods:
        fs = sorted(por_mod[m], key=lambda r: r['va_int'])
        cur = []
        size = 0

        def cerrar():
            nonlocal cur, size
            if cur:
                out.append((m, cur))
                cur, size = [], 0
        for r in fs:
            if r['bytes'] >= GRANDE:
                cerrar()
                out.append((m, [r]))
                continue
            if cur and (len(cur) >= MAX_FUNCS or size + r['bytes'] > MAX_BYTES):
                cerrar()
            cur.append(r)
            size += r['bytes']
        cerrar()
    return out


def tbl(head, body):
    return ['| ' + ' | '.join(head) + ' |', '| ' + ' | '.join('---:' if i else '---' for i in range(len(head))) + ' |'] + \
           ['| ' + ' | '.join(str(c) for c in r) + ' |' for r in body]


def write(root):
    root = Path(root)
    rows = cargar(root)
    ls = lotes(rows)
    n, nb = len(rows), sum(r['bytes'] for r in rows)
    partes = [ls[i:i + POR_PARTE] for i in range(0, len(ls), POR_PARTE)]
    nombre = lambda i: f'{LOTE_INICIAL + i}A'
    with open(root / 'docs/RenderWareExe/plan-lotes.csv', 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['lote', 'parte', 'clase_modulo', 'grupo', 'va', 'bytes', 'estado', 'confianza', 'nombre'])
        for i, (m, fs) in enumerate(ls):
            for r in fs:
                w.writerow([nombre(i), i // POR_PARTE + 1, m, GRUPO.get(m, 'SDK de RenderWare'), r['va'], r['bytes'], r['estado'], r['confianza'], r['nombre']])
    L = ['# Plan de reconstrucción de RenderWare y D3D por partes y lotes', '',
         '[Mapa de la región](README.md) · [Pendientes por clase](pendientes-por-clase.md) · [Lista de funciones por lote (CSV)](plan-lotes.csv)', '',
         f'Región **0x750000–0x81FFFF**: {n:,} funciones por reconstruir ({nb:,} bytes) repartidas en **{len(ls)} lotes** y **{len(partes)} partes**. '
         'Generado por `contrib/rw_plan.py` a partir de `funciones.csv`.', '',
         'Reglas: lotes de hasta 32 funciones y unos 5 KB; una función de 3 KB o más va sola («grande»); las funciones de un lote son contiguas '
         'dentro de su clase o módulo; seis lotes por parte. Los lotes se numeran desde 240A para continuar el plan del código del juego (191A–239A).', '',
         'Orden de trabajo: primero el SDK de RenderWare (RwPlCore, RwCore, RpWorld y los plugins), después el controlador D3D9 y al final las bibliotecas '
         'de terceros (libpng + zlib y D3DX9). Estas últimas no son código de Rockstar ni de RenderWare: se sustituyen enlazando la biblioteca original; '
         'están en el plan por completitud y van al final.', '',
         'Aviso: la clase o módulo de cada función es exacta solo si la confianza lo dice; en el resto es una estimación por vecindad (ver README).', '',
         '## Partes', '']
    body = []
    for pi, p in enumerate(partes):
        ids = [LOTE_INICIAL + pi * POR_PARTE + k for k in range(len(p))]
        f = sum(len(fs) for _, fs in p)
        b = sum(r['bytes'] for _, fs in p for r in fs)
        mods = []
        for m, _ in p:
            if m not in mods:
                mods.append(m)
        body.append([f'Parte {pi + 1}', f'{ids[0]}A–{ids[-1]}A', f'{f:,}', f'{b:,}', ', '.join(mods)])
    L += tbl(['Parte', 'Lotes', 'Funciones', 'Bytes', 'Clases / módulos'], body) + ['']
    grupos = defaultdict(lambda: [0, 0, 0])
    for m, fs in ls:
        g = grupos[GRUPO.get(m, 'SDK de RenderWare')]
        g[0] += 1
        g[1] += len(fs)
        g[2] += sum(r['bytes'] for r in fs)
    L += ['## Por grupo', ''] + tbl(['Grupo', 'Lotes', 'Funciones', 'Bytes'], [[k, v[0], f'{v[1]:,}', f'{v[2]:,}'] for k, v in grupos.items()]) + ['']
    for pi, p in enumerate(partes):
        ids = [LOTE_INICIAL + pi * POR_PARTE + k for k in range(len(p))]
        L += [f'## Parte {pi + 1} · lotes {ids[0]}A–{ids[-1]}A', '']
        body = []
        for k, (m, fs) in enumerate(p):
            i = pi * POR_PARTE + k
            b = sum(r['bytes'] for r in fs)
            tipo = 'grande' if len(fs) == 1 and fs[0]['bytes'] >= GRANDE else ('pequeña' if b < 1500 else 'mediana')
            env = sum(1 for r in fs if r['estado'] == 'envoltura')
            conf = sum(1 for r in fs if r['confianza'] == 'exacta')
            body.append([nombre(i), m, f"{fs[0]['va']}–{fs[-1]['va']}", len(fs), f'{b:,}', tipo, env, f'{conf}/{len(fs)}'])
        L += tbl(['Lote', 'Clase / módulo', 'Rango', 'Funciones', 'Bytes', 'Tamaño', 'Envolturas', 'Clase exacta'], body) + ['']
    (root / 'docs/RenderWareExe/plan-lotes.md').write_text('\n'.join(L) + '\n', encoding='utf-8')
    return ls, partes


if __name__ == '__main__':
    ls, partes = write(Path(__file__).resolve().parents[1])
    print(len(ls), 'lotes', len(partes), 'partes')
