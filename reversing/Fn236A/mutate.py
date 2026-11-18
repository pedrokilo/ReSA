"""Prueba negativa del banco Fn236A (lote 236A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (LibJpegDecompressApi.cpp).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (4 por ronda): cambia lo que el banco observa
(bytes escritos, eventos de los falsos, valor devuelto). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real
no produce y el banco si: la estructura jpeg_decompress_struct (cinfo) con direccion desalineada (familia 1 y parte de las 2 y 3).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
NL = chr(10)
LJ = 'source/game_sa/Plugins/JPegCompressPlugin/LibJpegDecompressApi.cpp'
SOURCES = [LJ]
UN = '(reinterpret_cast<uintptr_t>(cinfo) & 3)'   # estructura desalineada: solo la pone el banco (familia 1 y parte de las 2 y 3)


def round1():
    m = []
    m.append((LJ, 'At<uint32>(cinfo, 0x10C) = 0;', 'At<uint32>(cinfo, 0x10C) = ' + UN + ' ? 1 : 0;'))                                                                       # 0 jpeg_CreateDecompress
    m.append((LJ, 'At<int32>(cinfo, 0x14) = DSTATE_READY;', 'At<int32>(cinfo, 0x14) = ' + UN + ' ? 0xCB : DSTATE_READY;'))                                                  # 1 jpeg_consume_input
    m.append((LJ, 'CallCinfo(0x5C6B50, cinfo);' + NL + '    return 1;', 'CallCinfo(0x5C6B50, cinfo);' + NL + '    return ' + UN + ' ? 0 : 1;'))                                       # 2 jpeg_finish_decompress
    m.append((LJ, 'return 2;', 'return ' + UN + ' ? 3 : 2;'))                                                                                                              # 3 jpeg_read_header
    return m


def round2():
    m = []
    m.append((LJ, 'At<uint8>(cinfo, 0x10) = 1;', 'At<uint8>(cinfo, 0x10) = ' + UN + ' ? 2 : 1;'))                                                                            # 0 jpeg_CreateDecompress
    m.append((LJ, 'At<int32>(cinfo, 0x3C) = 0x3FF00000;', 'At<int32>(cinfo, 0x3C) = 0x3FF00000 + (' + UN + ' ? 1 : 0);'))                                                    # 1 jpeg_consume_input
    m.append((LJ, 'if (At<uint32>(cinfo, 0x78) < At<uint32>(cinfo, 0x60)) {', 'if (At<uint32>(cinfo, 0x78) < At<uint32>(cinfo, 0x60) + (' + UN + ' ? 1 : 0)) {'))          # 2 jpeg_finish_decompress
    m.append((LJ, 'if (requireImage) {', 'if (requireImage || ' + UN + ') {'))                                                                                               # 3 jpeg_read_header
    return m


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(n):
    GOOD.mkdir(parents=True, exist_ok=True)
    state = GOOD / 'state.json'
    if state.exists():
        raise SystemExit('Hay una ronda aplicada: restaurar antes')
    saved = {}
    for rel in SOURCES:
        dst = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(ROOT / rel, dst)
        saved[rel] = sha(dst)
    state.write_text(json.dumps(saved, indent=2))
    for rel, old, new in ROUNDS[n]:
        p = ROOT / rel
        text = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in text
        norm = text.replace('\r\n', '\n')
        if norm.count(old) != 1:
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:80], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
        # varias mutaciones en el mismo archivo se aplican en cadena: la siguiente lee el texto ya mutado
    print('ronda', n, 'aplicada:', len(ROUNDS[n]), 'mutaciones')


def restore():
    state = GOOD / 'state.json'
    saved = json.loads(state.read_text())
    for rel, digest in saved.items():
        src = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(src, ROOT / rel)
        if sha(ROOT / rel) != digest:
            raise SystemExit('Fuente restaurada distinta: ' + rel)
    state.unlink()
    print('restauradas', len(saved), 'fuentes (SHA256 igual a las copias buenas)')


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
