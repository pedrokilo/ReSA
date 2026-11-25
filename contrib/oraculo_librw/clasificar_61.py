#!/usr/bin/env python3
r"""Clasifica las 61 funciones de docs/RenderWareExe/no-reconstruibles.csv frente a librw. Uso: python clasificar_61.py <raiz del repo>
Salida: docs/RenderWareExe/no-reconstruibles-librw.csv (va, bytes, modulo, clasificacion, evidencia). Reglas por modulo/motivo, con la evidencia revisada a mano contra D:\Original\librw-test\librw."""
import csv, sys, pathlib
raiz = pathlib.Path(sys.argv[1]); d = raiz / 'docs/RenderWareExe'
size = {r['va'].lower(): int(r['bytes']) for r in csv.DictReader(open(d / 'funciones.csv', encoding='utf-8'))}
SIN, IRR, ADAP, DIR = 'sin equivalente', 'irrelevante', 'equivalente con adaptacion', 'equivalente directo'
def clasif(r):
    m, motivo = r['modulo'], r['motivo']
    if m.startswith('D3DX9'):
        return SIN, 'librw no trae D3DX9 ni compilador HLSL (sus shaders son .h precompilados en src/d3d/shaders); son rutinas internas de D3DX9 (3DNow!/MMX/XMM por registros). Se sustituyen enlazando D3DX9 de Microsoft, no con librw.'
    if m.startswith('QuickTime'):
        return IRR, 'QuickTime/QTML: librw no tiene ningun equivalente; ya sustituido por Media Foundation (CAEMFDecoder). Nada del juego lo llama.' if 'Sustituida' in motivo else 'Puente de QuickTime/QTML (ID por EAX + jmp): librw no tiene nada equivalente; el audio de video ya usa Media Foundation.'
    if m.startswith('Carga retardada'):
        return IRR, 'Ayudante del enlazador MSVC (delay-load / DirectDrawCreateEx): no es RenderWare; librw no lo cubre ni lo necesita (DirectDraw ya sustituido por D3D9).'
    if m == 'RpSkin':
        return IRR, 'Subrutina/epilogo de _rpSkinGenericMatrixBlend (0x7CB6A0), ya reconstruida entera en C++; en librw el mezclado es el del shader (src/d3d/d3d9skin.cpp) y no hay funcion homologa. No hay nada que cubrir.'
    if m == 'RtAnim':
        return IRR, 'Fragmento intermedio de RtAnimAnimationRead (0x7CCF30), reconstruida entera; librw tiene Animation::streamRead (src/anim.cpp) pero no hay un fragmento que sustituir.'
    if m.startswith('Controlador D3D9'):
        return SIN, 'Rutinas del dispositivo D3D9 de RenderWare (CameraClear/BeginUpdate de _rwD3D9...), ya enganchadas en WindowedMode.cpp; el backend D3D9 de librw (src/d3d/d3d9.cpp, d3ddevice.cpp) tiene otra estructura de motor y no expone estas funciones: seria un puente nuevo, no un equivalente.'
    return SIN, ''
rows = []
for r in csv.DictReader(open(d / 'no-reconstruibles.csv', encoding='utf-8')):
    c, ev = clasif(r)
    rows.append((r['va'], size.get(r['va'].lower(), 0), r['modulo'], c, ev))
with open(d / 'no-reconstruibles-librw.csv', 'w', encoding='utf-8', newline='') as f:
    w = csv.writer(f); w.writerow(['va', 'bytes', 'modulo', 'clasificacion_librw', 'evidencia']); w.writerows(rows)
import collections
t = collections.Counter(); b = collections.Counter()
for va, sz, m, c, e in rows: t[c] += 1; b[c] += sz
for c in t: print(c, t[c], b[c])
