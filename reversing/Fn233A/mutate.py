"""Prueba negativa del banco Fn233A (lote 233A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (LibJpegDecompress.cpp).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (12 por ronda): cambia lo que el banco observa
(bytes escritos, eventos de los falsos, valor devuelto). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real
no produce y el banco si: la estructura jpeg_decompress_struct (cinfo) o el estado de relleno de bits con direccion desalineada (familia 1 y parte de las 2 y 3).
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
LJ = 'source/game_sa/Plugins/JPegCompressPlugin/LibJpegDecompress.cpp'
SOURCES = [LJ]
UN = '(reinterpret_cast<uintptr_t>(cinfo) & 3)'   # estructura desalineada: solo la pone el banco
UNS = '(reinterpret_cast<uintptr_t>(state) & 3)'  # estado de jpeg_fill_bit_buffer / jpeg_huff_decode desalineado


def round1():
    m = []
    m.append((LJ, '    At<int32>(dtbl, 0x44) = 0xFFFFF;', '    At<int32>(dtbl, 0x44) = ' + UN + ' ? 0xFFFFE : 0xFFFFF;'))                                                              # 0 jpeg_make_d_derived_tbl
    m.append((LJ, '    At<int32>(state, 0x0C) = bitsLeft;' + NL + '    return true;', '    At<int32>(state, 0x0C) = bitsLeft + (' + UNS + ' ? 1 : 0);' + NL + '    return true;'))      # 1 jpeg_fill_bit_buffer
    m.append((LJ, '    At<int32>(state, 0x0C) = bits;' + NL + '    if (l > 16) {', '    At<int32>(state, 0x0C) = bits + (' + UNS + ' ? 1 : 0);' + NL + '    if (l > 16) {'))                 # 2 jpeg_huff_decode
    m.append((LJ, '    At<uint32>(entropy, 0x04) = 0x5C9CE0;', '    At<uint32>(entropy, 0x04) = ' + UN + ' ? 0x5C9CE1 : 0x5C9CE0;'))                                                       # 3 jinit_huff_decoder
    m.append((LJ, '    At<uint32>(At<void*>(cinfo, 0x190), 0x00) = At<uint32>(At<void*>(cinfo, 0x188), 0x04);', '    At<uint32>(At<void*>(cinfo, 0x190), 0x00) = At<uint32>(At<void*>(cinfo, 0x188), 0x04) + (' + UN + ' ? 1 : 0);'))   # 4 start_input_pass
    m.append((LJ, '        At<uint8>(inputctl, 0x11) = 1;', '        At<uint8>(inputctl, 0x11) = ' + UN + ' ? 2 : 1;'))                                                                 # 5 consume_markers
    m.append((LJ, '    At<uint32>(cinfo, 0x8C) = 0;', '    At<uint32>(cinfo, 0x8C) = ' + UN + ' ? 1 : 0;'))                                                                       # 6 reset_input_controller
    m.append((LJ, '    At<uint32>(inputctl, 0x0C) = 0x5CA820;', '    At<uint32>(inputctl, 0x0C) = ' + UN + ' ? 0x5CA821 : 0x5CA820;'))                                                  # 7 jinit_input_controller
    m.append((LJ, '            xbuf0[i - rgroup] = xbuf0[0];', '            xbuf0[i - rgroup] = xbuf0[0] + (' + UN + ' ? 1u : 0u);'))                                                           # 8 make_funny_pointers
    m.append((LJ, '            xbuf1[rgroup * (M + 2) + i] = xbuf1[i];', '            xbuf1[rgroup * (M + 2) + i] = xbuf1[i] + (' + UN + ' ? 1u : 0u);'))                                           # 9 set_wraparound_pointers
    m.append((LJ, '        At<int32>(main, 0x30) = (rowsLeft - 1) / rgroup + 1;', '        At<int32>(main, 0x30) = (rowsLeft - 1) / rgroup + 1 + (' + UN + ' ? 1 : 0);'))                                  # 10 set_bottom_pointers
    m.append((LJ, '    At<uint32>(main, 0x00) = 0x5CADC0;', '    At<uint32>(main, 0x00) = ' + UN + ' ? 0x5CADC1 : 0x5CADC0;'))                                                           # 11 jinit_d_main_controller
    return m


def round2():
    m = []
    m.append((LJ, '    std::memset(dtbl + 0x90, 0, 0x400);', '    std::memset(dtbl + 0x90, ' + UN + ' ? 1 : 0, 0x400);'))                                                                 # 0
    m.append((LJ, '        buf <<= ((25 - bitsLeft) & 31);', '        buf <<= ((25 - bitsLeft + (' + UNS + ' ? 1 : 0)) & 31);'))                                                         # 1
    m.append((LJ, '    return *reinterpret_cast<uint8*>(addr);', '    return *reinterpret_cast<uint8*>(addr) + (' + UNS + ' ? 1 : 0);'))                                                       # 2
    m.append((LJ, '    At<uint32>(entropy, 0x00) = 0x5CA110;', '    At<uint32>(entropy, 0x00) = ' + UN + ' ? 0x5CA111 : 0x5CA110;'))                                                         # 3
    m.append((LJ, '    LatchQuantTables(cinfo);', '    if (!' + UN + ') LatchQuantTables(cinfo);'))                                                                                # 4
    m.append((LJ, '            InitialSetup(cinfo);' + NL + '            At<uint8>(inputctl, 0x14) = 0;', '            InitialSetup(cinfo);' + NL + '            At<uint8>(inputctl, 0x14) = ' + UN + ' ? 1 : 0;'))      # 5
    m.append((LJ, '    uint8* const inputctl = At<uint8*>(cinfo, 0x190);' + NL + '    At<uint32>(inputctl, 0x00) = 0x5CA720;', '    uint8* const inputctl = At<uint8*>(cinfo, 0x190);' + NL + '    At<uint32>(inputctl, 0x00) = ' + UN + ' ? 0x5CA721 : 0x5CA720;'))   # 6
    m.append((LJ, 'AllocSmall(cinfo, 0, 0x18)', 'AllocSmall(cinfo, ' + UN + ' ? 1 : 0, 0x18)'))                                                                                # 7
    m.append((LJ, '            xbuf1[rgroup * M + i] = buf[rgroup * (M - 2) + i];', '            xbuf1[rgroup * M + i] = buf[rgroup * (M - 2) + i] + (' + UN + ' ? 1u : 0u);'))                          # 8
    m.append((LJ, '            xbuf0[i - rgroup] = xbuf0[rgroup * (M + 1) + i];', '            xbuf0[i - rgroup] = xbuf0[rgroup * (M + 1) + i] + (' + UN + ' ? 1u : 0u);'))                            # 9
    m.append((LJ, '            xbuf[rowsLeft + i] = xbuf[rowsLeft - 1];', '            xbuf[rowsLeft + i] = xbuf[rowsLeft - 1] + (' + UN + ' ? 1u : 0u);'))                                          # 10
    m.append((LJ, '        const int32 height = rgroup * ngroups;', '        const int32 height = rgroup * ngroups + (' + UN + ' ? 1 : 0);'))                                                    # 11
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
