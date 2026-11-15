"""Cruce entre la API de RenderWare que usa el código de ReSA y lo que tiene `librw`: docs/RenderWareExe/librw-brechas.md y librw-brechas.csv.

Entrada:
  * la API de RenderWare/D3D que el código del juego LLAMA (identificadores Rw*/Rp*/Rt*/_rw*/_rp* fuera de source/game_sa/RenderWare/rw), con su número
    de usos: es lo que una biblioteca de sustitución tiene que ofrecer;
  * el código de `librw` (por defecto D:/Original/librw-test/librw/src, variable LIBRW_SRC).
Método (heurístico, a nombre): `RwFrameCreate` -> objeto `Frame`, verbo `Create`; se busca `Frame::create` (o el sinónimo del verbo) en `librw`, y si el
objeto no tiene método con ese verbo se busca el verbo como identificador en el archivo del objeto. Lo que no se resuelve queda como «sin equivalente
claro» y hay que revisarlo a mano: la lista no es una prueba de que falte, solo de que el nombre no se encontró.
"""
import csv
import os
import re
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

LIBRW = Path(os.environ.get('LIBRW_SRC', 'D:/Original/librw-test/librw/src'))
OBJETOS = ['TexDictionary', 'Texture', 'Raster', 'Image', 'MaterialList', 'Material', 'Frame', 'Geometry', 'Clump', 'Atomic', 'Light', 'Camera', 'Matrix',
           'V3d', 'V2d', 'BBox', 'Sphere', 'Line', 'Im2D', 'Im3D', 'Stream', 'MeshHeader', 'Mesh', 'MorphTarget', 'WorldSector', 'World', 'Skin', 'HAnim',
           'MatFX', 'UVAnim', 'UserData', 'AnimInterpolator', 'Anim', 'Engine', 'Error', 'Mem', 'Os', 'RGBA', 'Quat', 'Slerp', 'Pipeline', 'Render',
           'Charset', 'Font', 'Plugin', 'Bmp', 'Png', 'Tga', 'Gamma', 'Fsys', 'FreeList', 'LinkList', 'Fixed', 'Cache', 'Frustum', 'Dict', 'Anisot', 'Pat',
           'Metrics', 'D3D9', 'D3D8', 'Object', 'Sky', 'Resources', 'Device', 'Version', 'Vector']
SINONIMOS = {'create': ['create', 'make'], 'destroy': ['destroy', 'dtor', 'release'], 'multiply': ['mult', 'multiply'], 'transform': ['transform'],
             'streamread': ['streamRead', 'read'], 'streamwrite': ['streamWrite', 'write'], 'streamgetsize': ['streamGetSize', 'getSize'],
             'getsize': ['streamGetSize', 'getSize'], 'foralltextures': ['forAllTextures'], 'getcurrent': ['getCurrent', 'current'],
             'setcurrent': ['setCurrent', 'current'], 'setframe': ['setFrame'], 'getframe': ['getFrame', 'frame']}
RW = re.compile(r'\b(_?(?:Rw|Rp|Rt|rw|rp|rt)[A-Z0-9][A-Za-z0-9_]*)\b')


def librw_texto():
    t = {}
    for p in LIBRW.rglob('*'):
        if p.suffix in ('.cpp', '.h') and 'glad' not in str(p) and 'lodepng' not in str(p):
            t[p.name] = p.read_text(encoding='utf-8', errors='replace')
    return t


def api_usada(root):
    out = subprocess.run(['git', 'grep', '-n', '-o', '-E', r'\b_?(Rw|Rp|Rt|rw|rp|rt)[A-Z0-9][A-Za-z0-9_]*\(', '--', 'source', ':!source/game_sa/RenderWare/rw'],
                         cwd=root, capture_output=True, text=True, encoding='utf-8', errors='replace').stdout.split('\n')
    cuenta = Counter()
    for l in out:
        m = re.match(r'[^:]+:\d+:(.*)\($', l)
        if m:
            cuenta[m.group(1)] += 1
    return cuenta


def separar(nombre):
    base = re.sub(r'^_?(Rw|Rp|Rt|rw|rp|rt)', '', nombre)
    for o in OBJETOS:
        if base.startswith(o) and (len(base) == len(o) or not base[len(o)].islower()):
            return o, base[len(o):]
    return '', base


def buscar(nombre, textos):
    obj, verbo = separar(nombre)
    todo = '\n'.join(textos.values())
    cand = []
    if verbo:
        v = verbo[0].lower() + verbo[1:]
        cand += [v] + SINONIMOS.get(verbo.lower(), [])
    if obj:
        for c in cand:
            if re.search(r'\b' + re.escape(obj) + r'::' + re.escape(c) + r'\b', todo):
                return 'si', f'{obj}::{c}'
        for c in cand:
            if re.search(r'\b' + re.escape(c) + r'\s*\(', todo):
                return 'parcial', f'{c}(...) sin objeto {obj}'
    base = re.sub(r'^_?(Rw|Rp|Rt|rw|rp|rt)', '', nombre)
    if re.search(r'\b' + re.escape(base) + r'\b', todo, re.I):
        return 'parcial', f'identificador {base}'
    return 'no', ''


def modulo(nombre):
    n = nombre
    for pre, m in (('RpHAnim', 'RpHAnim'), ('RpSkin', 'RpSkin'), ('RpMatFX', 'RpMatFX'), ('RpUVAnim', 'RpUVAnim'), ('RpAnisot', 'RpAnisot'), ('RpWorld', 'RpWorld'),
                   ('RpCollis', 'RpCollis'), ('RpMorph', 'RpMorph'), ('RtAnim', 'RtAnim'), ('RtDict', 'RtDict'), ('RtQuat', 'RtQuat'), ('RtSlerp', 'RtSlerp'),
                   ('RtBMP', 'RtBMP'), ('RtPNG', 'RtPNG'), ('RtCharset', 'RtCharset'), ('RpClump', 'RpWorld'), ('RpAtomic', 'RpWorld'), ('RpGeometry', 'RpWorld'),
                   ('RpMaterial', 'RpWorld'), ('RpLight', 'RpWorld'), ('RpMesh', 'RpWorld'), ('Rp', 'RpWorld'), ('Rt', 'Rt (otros)'), ('_rwD3D9', 'D3D9'),
                   ('RwD3D9', 'D3D9'), ('rwD3D9', 'D3D9'), ('RwIm2D', 'Im2D/Im3D'), ('RwIm3D', 'Im2D/Im3D'), ('Rw', 'RwCore'), ('_rw', 'RwCore'), ('_rp', 'RpWorld')):
        if n.startswith(pre):
            return m
    return 'otros'


PROPIO = re.compile(r'^(RpAnimBlend|RpNan|RpHAnimIDGetIndex|RwHelper|RwCore\w*InjectHooks|Rp\w*InjectHooks|Rt\w*InjectHooks|_rw(Object|Frame)\w*Cpp|\w*Cpp$|\w*Thunk$|\w*Macro$)')
ACCESOR = re.compile(r'^(Rw|Rp|Rt)\w*?(Get|Set)(Raster|ZRaster|Width|Height|Depth|Name|Matrix|Pos|At|Up|Right|NearClipPlane|FarClipPlane|ViewWindow|Geometry|Clump|Material|Texture|MorphTarget|NumVertices|PreLightColors|VertexTexCoords|BoundingSphere|Vertices|VertexNormals|SurfaceProperties|Addressing|Pixels|Effects|Skin|NumBones|Frame)$')


def clasificar(n, r):
    if PROPIO.match(n):
        return 'codigo_propio'
    if r != 'si' and ACCESOR.match(n):
        return 'acceso_a_campo'
    return r


def main(root='.'):
    root = Path(root)
    if not LIBRW.exists():
        sys.exit(f'No existe {LIBRW}: define LIBRW_SRC')
    textos = librw_texto()
    usadas = api_usada(root)
    # se descartan tipos, enumeraciones y macros: solo cuentan los identificadores que se usan como llamada
    filas = []
    for n, c in usadas.items():
        r, d = buscar(n, textos)
        filas.append((n, modulo(n), c, clasificar(n, r), d))
    filas.sort(key=lambda x: (x[1], -x[2], x[0]))
    with open(root / 'docs/RenderWareExe/librw-brechas.csv', 'w', encoding='utf-8', newline='') as f:
        w = csv.writer(f, lineterminator='\n')
        w.writerow(['funcion', 'modulo', 'usos_en_el_codigo', 'en_librw', 'equivalente'])
        w.writerows(filas)
    return filas


if __name__ == '__main__':
    f = main(Path(__file__).resolve().parents[1])
    c = Counter(x[3] for x in f)
    print(len(f), 'funciones de la API usadas;', dict(c))
    por = defaultdict(Counter)
    for x in f:
        por[x[1]][x[3]] += 1
    for m, v in sorted(por.items()):
        print(m, dict(v))
