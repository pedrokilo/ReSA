"""Prueba negativa del banco Fn234A (lote 234A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (LibJpegDecompress.cpp).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (10 por ronda): cambia lo que el banco observa
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
LJ = 'source/game_sa/Plugins/JPegCompressPlugin/LibJpegDecompress.cpp'
SOURCES = [LJ]
UN = '(reinterpret_cast<uintptr_t>(cinfo) & 3)'   # estructura desalineada: solo la pone el banco (familia 1 y parte de las 2 y 3)


def round1():
    m = []
    m.append((LJ, 'std::memcpy(htbl, bits, 17);', 'std::memcpy(htbl, bits, ' + UN + ' ? 16 : 17);'))                                                                           # 0 get_dht
    m.append((LJ, 'At<uint32>(cinfo, 0xFC) = tmp;', 'At<uint32>(cinfo, 0xFC) = tmp + (' + UN + ' ? 1 : 0);'))                                                                  # 1 get_dri
    m.append((LJ, 'At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x18), 0x10)(cinfo, length);', 'At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x18), 0x10)(cinfo, length + (' + UN + ' ? 1 : 0));'))   # 2 skip_variable
    m.append((LJ, 'At<int32>(At<void*>(cinfo, 0x194), 0x14) += 2;', 'At<int32>(At<void*>(cinfo, 0x194), 0x14) += ' + UN + ' ? 3 : 2;'))                                         # 3 next_marker
    m.append((LJ, 'At<int32>(cinfo, 0x17C) = static_cast<int32>(c2);', 'At<int32>(cinfo, 0x17C) = static_cast<int32>(c2) ^ (' + UN + ' ? 1 : 0);'))                           # 4 first_marker
    m.append((LJ, 'At<int32>(marker, 0x10) = (At<int32>(marker, 0x10) + 1) & 7;', 'At<int32>(marker, 0x10) = (At<int32>(marker, 0x10) + (' + UN + ' ? 2 : 1)) & 7;'))           # 5 read_restart_marker
    m.append((LJ, 'At<uint32>(marker, 0x04) = 0x5CC290;', 'At<uint32>(marker, 0x04) = ' + UN + ' ? 0x5CC291 : 0x5CC290;'))                                                  # 6 jinit_marker_reader
    m.append((LJ, 'At<int32>(cinfo, 0x6C) = 1;', 'At<int32>(cinfo, 0x6C) = ' + UN + ' ? 2 : 1;'))                                                                            # 7 jpeg_calc_output_dimensions
    m.append((LJ, 'At<int32>(progress, 0x0C) = At<int32>(master, 0x0C);', 'At<int32>(progress, 0x0C) = At<int32>(master, 0x0C) + (' + UN + ' ? 1 : 0);'))                    # 8 prepare_for_output_pass
    m.append((LJ, 'At<uint32>(master, 0x04) = 0x5CCC90;', 'At<uint32>(master, 0x04) = ' + UN + ' ? 0x5CCC91 : 0x5CCC90;'))                                                    # 9 jinit_master_decompress
    return m


def round2():
    m = []
    m.append((LJ, 'TraceMs(cinfo, JTRC_DHT, 1, {static_cast<int32>(index)});', 'TraceMs(cinfo, JTRC_DHT, ' + UN + ' ? 2 : 1, {static_cast<int32>(index)});'))              # 0
    m.append((LJ, 'TraceMs(cinfo, JTRC_DRI, 1, {static_cast<int32>(tmp)});', 'TraceMs(cinfo, JTRC_DRI, ' + UN + ' ? 2 : 1, {static_cast<int32>(tmp)});'))                  # 1
    m.append((LJ, 'TraceMs(cinfo, JTRC_SKIP_VARIABLE, 1,', 'TraceMs(cinfo, JTRC_SKIP_VARIABLE, ' + UN + ' ? 2 : 1,'))                                                          # 2
    m.append((LJ, 'At<int32>(cinfo, 0x17C) = static_cast<int32>(c);' + NL + '            in.Sync();', 'At<int32>(cinfo, 0x17C) = static_cast<int32>(c) ^ (' + UN + ' ? 1 : 0);' + NL + '            in.Sync();'))                           # 3
    m.append((LJ, 'ErrExit2(cinfo, JERR_NO_SOI, static_cast<int32>(c), static_cast<int32>(c2));', 'ErrExit2(cinfo, JERR_NO_SOI, static_cast<int32>(' + UN + ' ? c2 : c), static_cast<int32>(c2));'))   # 4
    m.append((LJ, 'TraceMs(cinfo, JTRC_RST, 3,', 'TraceMs(cinfo, JTRC_RST, ' + UN + ' ? 4 : 3,'))                                                                              # 5
    m.append((LJ, 'At<uint32>(marker2, 0xA0) = 0;', 'At<uint32>(marker2, 0xA0) = ' + UN + ' ? 1 : 0;'))                                                                       # 6
    m.append((LJ, 'At<int32>(cinfo, 0x5C) = width;', 'At<int32>(cinfo, 0x5C) = width + (' + UN + ' ? 1 : 0);'))                                                                # 7
    m.append((LJ, 'At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x184), 0x00)(cinfo, 0);', 'At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x184), 0x00)(cinfo, ' + UN + ' ? 1 : 0);'))   # 8
    m.append((LJ, 'At<uint8>(master, 0x08) = 0;' + NL + '    MasterSelection(cinfo);', 'At<uint8>(master, 0x08) = ' + UN + ' ? 1 : 0;' + NL + '    MasterSelection(cinfo);'))                  # 9
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
