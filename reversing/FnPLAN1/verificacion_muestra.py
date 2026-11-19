"""Lote PLAN1: tabla de la verificacion manual de la muestra (55 filas) contra el EXE (desensamblado con capstone, muestra.py).
Une el veredicto manual (escrito a mano tras leer el desensamblado y la linea de fuente) con las columnas automaticas de raices.csv.
Salida: reversing/FnPLAN1/datos/verificacion_muestra.csv y verificacion_resumen.json (reales por estrato y por confianza).
Uso: python -I verificacion_muestra.py --repo <raiz>"""
import argparse, collections, csv, json
from pathlib import Path

F = 'funcion real'
J = 'no es funcion (constante o interior)'
# direccion: (estrato, veredicto manual, que hace, nota/riesgo)
M = {
    0x6FF4F0: ('A juego', F, 'distancia/clip de una linea 3D con x87 (fsub/fmul/faddp)', 'x87; envoltura plugin::Call'),
    0x746D80: ('A juego', F, 'configura un dispositivo DirectInput8 (estructura de 0x154 bytes, propiedades del eje)', 'llama por COM'),
    0x4CCE60: ('A juego', F, 'inicializa formato de raster D3D9 (lee tabla 0xB4E9E0); usercall-renderware', 'excluida (usercall): la llaman 2 sitios propios'),
    0x508D30: ('A juego', F, 'busca una clave de texto en un bufer (bucle de cadena) para zonas de audio', 'thiscall con 2 args'),
    0x4637C0: ('A juego', F, 'CMissionCleanup::FindFree: bucle de 75 entradas de 8 bytes buscando la primera libre', 'trivial, sin FPU'),
    0x448890: ('A juego', F, 'recorta un campo float de CVehicle a un minimo y llama a un metodo del vehiculo', 'x87 (fcomp, fnstsw)'),
    0x41B1D0: ('A juego', F, 'lectura de un chunk RwStream con prologo SEH (fs:[0], manejador 0x83723B)', 'SEH; 2 llamadas internas'),
    0x4CF9B0: ('A juego', F, 'CQuaternion::Scale: multiplica los 4 componentes por un float', 'x87; 41 bytes'),
    0x7CCDE0: ('A RW interno', F, 'busca en la tabla de interpoladores de animacion (0xC9B920, 0x30 por entrada) por tipo', 'sin FPU; datos globales'),
    0x7C60C0: ('A RW interno', F, 'mezcla/slerp de cuaterniones de huesos (RpHAnim), 1024 bytes', 'x87 pesado'),
    0x7F2E10: ('A RW interno', F, 'devuelve &engine->metrics (lee puntero global, +0x14C)', 'trivial'),
    0x810700: ('A RW interno', F, 'guarda blockSize y numBlocks de la lista libre de RpMatFX', 'trivial'),
    0x7CEF00: ('A RW interno', F, 'crea la tabla de un RtDict reservando memoria por el motor (0x809160)', 'memoria falsa'),
    0x80A450: ('A RW interno', F, 'llama a la funcion de motor [engine+0x138] con la cadena y devuelve 1', 'puntero de motor'),
    0x751E10: ('A RW interno', F, 'devuelve un puntero a datos de plugin de luz (RpLight + 0x30 via 0x808B00)', 'trivial'),
    0x75B500: ('A RW interno', F, 'recorrido de tiras de triangulos (RpTriStrip), 638 bytes', 'bucles'),
    0x8081C0: ('A RW interno', F, 'suma tamanos recorriendo una lista circular de un plugin del motor', 'listas libres'),
    0x749740: ('A RW interno', F, 'RpClumpSetFreeListCreateParams: guarda blockSize y numBlocks', 'trivial'),
    0x818970: ('A QuickTime', F, 'puente de 26 bytes: salta a [0x8E2A0C] con identificador 0x1D002E (QTML)', 'puente; no reconstruible en C++'),
    0x819390: ('A QuickTime', F, 'puente de 26 bytes: identificador 0x15003F hacia QTML', 'puente'),
    0x76B89B: ('A biblioteca', F, 'D3DX9: constructor de objeto del compilador de shaders (llama a 0x76E390)', 'miembro stdcall; 165 bytes'),
    0x823521: ('A biblioteca', F, 'CRT: cuerpo de una funcion de archivo con prologo SEH (__SEH_prolog)', 'SEH; punto de entrada por jmp'),
    0x76626B: ('A biblioteca', F, 'D3DX9: conversion de arreglo (float desde half) con prologo ebp', 'stdcall'),
    0x8213AE: ('A biblioteca', F, 'thunk jmp 0x8214BD (cadena hacia el _free del CRT)', 'cadena de thunks CRT'),
    0x8220AD: ('A biblioteca', F, 'CRT: envoltura de sscanf/vsscanf con pila local', 'varargs'),
    0x5CCF60: ('B juego', F, 'libjpeg: upsample sin cambio (copia el puntero de la fila de salida)', 'callback; 11 bytes'),
    0x5C53D0: ('B juego', F, 'libjpeg: start_pass del controlador principal de compresion', 'callback'),
    0x5CA110: ('B juego', F, 'libjpeg: inicio de la decodificacion entropica Huffman (jinit)', 'callback; 333 bytes'),
    0x61FD10: ('B juego', F, 'callback de fin de animacion de CTaskSimpleBeKickedOnGround (pone 2 campos)', 'callback'),
    0x5CF690: ('B juego', F, 'libjpeg: bucle de gestor de memoria (alloc de matrices)', 'callback'),
    0x5C9340: ('B juego', F, 'libjpeg: start_pass vacio (solo ret)', 'callback de 1 byte'),
    0x5C6030: ('B juego', F, 'libjpeg: write_marker_header', 'callback'),
    0x5C7D10: ('B juego', F, 'libjpeg: metodo de reduccion de muestreo (downsample)', 'callback; 211 bytes'),
    0x750A30: ('B RW interno', F, 'constructor del plugin de mundo de clump (pone 0 en la extension)', 'callback ctor'),
    0x752270: ('B RW interno', F, 'constructor de plugin de motor RpWorld (0x505) con lista libre', 'callback ctor'),
    0x8020B0: ('B RW interno', F, 'rwrealloc del motor: reenvia a realloc del CRT (0x824269)', 'callback de memoria'),
    0x758240: ('B RW interno', F, 'comparador de qsort (compara 16 bits) en RpWorld', 'callback de qsort'),
    0x74FEE0: ('B RW interno', F, 'constructor de plugin (0x507) que llama a 0x75E310', 'callback ctor'),
    0x7F3D00: ('B RW interno', F, 'destructor del plugin de motor 0x408 (recorre listas)', 'callback dtor; 340 bytes'),
    0x7ED7D0: ('B RW interno', F, 'rwVectorsMultFn por defecto: multiplica una tanda de vectores por matriz', 'x87 probable'),
    0x755BC0: ('B RW interno', F, 'constructor de plugin RpWorld (pone -1 en un campo)', 'callback ctor'),
    0x819700: ('B biblioteca', F, 'QTML: gancho tras fallo de carga (usa 0xC9ABDC, 0x8E2A40)', 'QuickTime'),
    0x8262B8: ('B biblioteca', F, 'CRT: funcion de archivo con prologo SEH (0x14 locales)', 'SEH'),
    0x656C62: ('C ruido', J, 'instrucciones sin sentido (aas, xor): es el medio de otra funcion (BugsDebugModule)', 'ruido'),
    0x801C30: ('C ruido', F + ' (promovida)', 'RwFreeListAlloc por defecto (kDefaultFreeListAlloc), 286 bytes', 'IND0A la dejo como ruido: falso negativo'),
    0x726F74: ('C ruido', J, 'medio de funcion (and al,0xC; je)', 'ruido'),
    0x666572: ('C ruido', J, 'medio de funcion (AudioEngine)', 'ruido'),
    0x736E75: ('C ruido', J, 'bytes que decodifican como basura (test/add)', 'ruido'),
    0x646E69: ('C ruido', J, 'medio de funcion (Cam)', 'ruido'),
    0x80FFFF: ('C ruido', J, 'etiqueta interior de funcion (sigue a un ret)', 'ruido'),
    0x6D0074: ('C ruido', J, 'medio de funcion (3dMarker)', 'ruido'),
    0x856BC0: ('C ruido', F + ' (promovida)', 'stub de inicializador estatico: mov ecx,obj; jmp ctor (0x59ACD0)', 'IND0A la dejo como ruido: falso negativo'),
    0x6C616E: ('C ruido', J, 'no decodifica (datos)', 'ruido'),
    0x78614D: ('C ruido', J, 'medio de funcion D3DX9', 'ruido'),
    0x74726F: ('C ruido', J, 'medio de funcion (RpWorld)', 'ruido'),
}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--repo', required=True)
    a = ap.parse_args()
    repo = Path(a.repo)
    rows = {r['direccion'].lower(): r for r in csv.DictReader(open(repo / 'docs/ExeIndependiente/raices.csv', encoding='utf-8'))}
    out = []
    for v, (est, ver, que, nota) in M.items():
        r = rows['0x%08x' % v]
        out.append(dict(direccion=r['direccion'], estrato=est, confianza=r['confianza'], categoria=r['categoria'], veredicto_manual=ver,
                        que_hace=que, nota=nota, veredicto_auto=r['veredicto_auto'], bytes=r['bytes'], fuente=r['llamadoras_fuente'][:80]))
    with open(repo / 'reversing/FnPLAN1/datos/verificacion_muestra.csv', 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, list(out[0]))
        w.writeheader()
        w.writerows(out)
    st = collections.defaultdict(lambda: [0, 0])
    pc = collections.defaultdict(lambda: [0, 0])
    for o in out:
        real = o['veredicto_manual'].startswith('funcion real')
        st[o['estrato']][0] += 1
        st[o['estrato']][1] += real
        k = o['estrato'].split()[0]
        pc[k][0] += 1
        pc[k][1] += real
    res = dict(total=len(out), por_estrato={k: dict(filas=v[0], reales=v[1]) for k, v in st.items()},
               por_confianza_ind0a={k: dict(filas=v[0], reales=v[1]) for k, v in pc.items()})
    (repo / 'reversing/FnPLAN1/datos/verificacion_resumen.json').write_text(json.dumps(res, indent=1, ensure_ascii=False), encoding='utf-8')
    print(json.dumps(res, indent=1, ensure_ascii=False))


if __name__ == '__main__':
    main()
