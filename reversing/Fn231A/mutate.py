"""Prueba negativa del banco Fn231A (lote 231A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (LibJpegCompress.cpp).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (18 por ronda): cambia lo que el banco observa
(bytes escritos, eventos de los falsos, valor devuelto). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real
no produce y el banco si: la estructura jpeg_compress_struct (cinfo) con direccion desalineada (familia 1 y parte de las 2 y 3).
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
LJ = 'source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompress.cpp'
SOURCES = [LJ]
UN = '(reinterpret_cast<uintptr_t>(cinfo) & 3)'   # estructura desalineada: solo la pone el banco
FNS = 'reinterpret_cast<void(__cdecl*)(ExeJpegCompress*)>'


def round1():
    m = []
    m.append((LJ, '        qtbl->sent_table = 1;', '        qtbl->sent_table = ' + UN + ' ? 2 : 1;'))                                                                           # 0 EmitDqt
    m.append((LJ, '        htbl->sent_table = 1;', '        htbl->sent_table = ' + UN + ' ? 0 : 1;'))                                                                           # 1 EmitDht
    m.append((LJ, '    Emit2Bytes(cinfo, 4);', '    Emit2Bytes(cinfo, ' + UN + ' ? 5 : 4);'))                                                                                # 2 EmitDri
    m.append((LJ, '    EmitByte(cinfo, cinfo->num_components);', '    EmitByte(cinfo, cinfo->num_components + (' + UN + ' ? 1 : 0));'))                                              # 3 EmitSof
    m.append((LJ, '    EmitByte(cinfo, cinfo->Ss);', '    EmitByte(cinfo, cinfo->Ss + (' + UN + ' ? 1 : 0));'))                                                                 # 4 EmitSos
    m.append((LJ, '    EmitByte(cinfo, cinfo->JFIF_major_version);', '    EmitByte(cinfo, cinfo->JFIF_major_version + (' + UN + ' ? 1 : 0));'))                                         # 5 EmitJfifApp0
    m.append((LJ, '    Emit2Bytes(cinfo, 100); // version', '    Emit2Bytes(cinfo, ' + UN + ' ? 101 : 100); // version'))                                                           # 6 EmitAdobeApp14
    m.append((LJ, '    EmitMarker(cinfo, 0xD8); // SOI', '    EmitMarker(cinfo, ' + UN + ' ? 0xD7 : 0xD8); // SOI'))                                                              # 7 WriteFileHeader
    m.append((LJ, '            cinfo->err->msg_code = 0x4B; // JTRC_16BIT_TABLES', '            cinfo->err->msg_code = 0x4B + (' + UN + ' ? 1 : 0); // JTRC_16BIT_TABLES'))                          # 8 WriteFrameHeader
    m.append((LJ, '    EmitMarker(cinfo, 0xD9); // EOI', '    EmitMarker(cinfo, ' + UN + ' ? 0xD8 : 0xD9); // EOI'))                                                                   # 9 WriteFileTrailer
    m.append((LJ, '    marker->write_marker_byte = reinterpret_cast<void*>(0x5C6070);', '    marker->write_marker_byte = reinterpret_cast<void*>(' + UN + ' ? 0x5C6071 : 0x5C6070);'))                     # 10 JinitMarkerWriter
    m.append((LJ, '        cinfo->progress->completed_passes = master->pass_number;', '        cinfo->progress->completed_passes = master->pass_number + (' + UN + ' ? 1 : 0);'))                         # 11 PrepareForPass
    m.append((LJ, '    cinfo->master->call_pass_startup = false;' + NL + '    ' + FNS, '    cinfo->master->call_pass_startup = (' + UN + ' != 0);' + NL + '    ' + FNS))                                      # 12 PassStartup
    m.append((LJ, '    master->scan_number = 0;', '    master->scan_number = ' + UN + ' ? 1 : 0;'))                                                                                # 13 JinitCMasterControl
    m.append((LJ, '            cinfo->global_state = 100; // CSTATE_START', '            cinfo->global_state = ' + UN + ' ? 101 : 100; // CSTATE_START'))                                          # 14 JpegAbort
    m.append((LJ, '    cinfo->global_state = 0;', '    cinfo->global_state = ' + UN + ' ? 1 : 0;'))                                                                               # 15 JpegDestroy
    m.append((LJ, 'sizeof(ExeJpegQuantTable)));' + NL + '    table->sent_table = 0;', 'sizeof(ExeJpegQuantTable)));' + NL + '    table->sent_table = ' + UN + ' ? 1 : 0;'))                      # 16 JpegAllocQuantTable
    m.append((LJ, 'sizeof(ExeJpegHuffTable)));' + NL + '    table->sent_table = 0;', 'sizeof(ExeJpegHuffTable)));' + NL + '    table->sent_table = ' + UN + ' ? 1 : 0;'))                      # 17 JpegAllocHuffTable
    return m


def round2():
    m = []
    m.append((LJ, '        if (qtbl->quantval[i] > 255) {', '        if (qtbl->quantval[i] > (' + UN + ' ? 254 : 255)) {'))                                                                 # 0
    m.append((LJ, '        Emit2Bytes(cinfo, length + 2 + 1 + 16);', '        Emit2Bytes(cinfo, length + 2 + 1 + 16 + (' + UN + ' ? 1 : 0));'))                                                  # 1
    m.append((LJ, '    Emit2Bytes(cinfo, static_cast<int32>(cinfo->restart_interval));', '    Emit2Bytes(cinfo, static_cast<int32>(cinfo->restart_interval) + (' + UN + ' ? 1 : 0));'))                    # 2
    m.append((LJ, '    EmitByte(cinfo, cinfo->data_precision);', '    EmitByte(cinfo, cinfo->data_precision + (' + UN + ' ? 1 : 0));'))                                                        # 3
    m.append((LJ, '    EmitByte(cinfo, cinfo->Se);', '    EmitByte(cinfo, cinfo->Se + (' + UN + ' ? 1 : 0));'))                                                                   # 4
    m.append((LJ, '    EmitByte(cinfo, cinfo->density_unit);', '    EmitByte(cinfo, cinfo->density_unit + (' + UN + ' ? 1 : 0));'))                                                         # 5
    m.append((LJ, '    Emit2Bytes(cinfo, 2 + 5 + 2 + 2 + 2 + 1);', '    Emit2Bytes(cinfo, 2 + 5 + 2 + 2 + 2 + 1 + (' + UN + ' ? 1 : 0));'))                                                       # 6
    m.append((LJ, '    if (cinfo->write_Adobe_marker) {', '    if (cinfo->write_Adobe_marker && !' + UN + ') {'))                                                                       # 7
    m.append((LJ, '        EmitSof(cinfo, 0xC1);', '        EmitSof(cinfo, ' + UN + ' ? 0xC3 : 0xC1);'))                                                                         # 8
    m.append((LJ, '    EmitMarker(cinfo, 0xD9); // EOI' + NL + '}', '    EmitMarker(cinfo, 0xD9); // EOI' + NL + '    if (' + UN + ') EmitByte(cinfo, 0);' + NL + '}'))                     # 9
    m.append((LJ, '    marker->last_restart_interval = 0;' + NL + '}', '    marker->last_restart_interval = ' + UN + ' ? 1 : 0;' + NL + '}'))                                                    # 10
    m.append((LJ, '        cinfo->progress->total_passes = master->total_passes;', '        cinfo->progress->total_passes = master->total_passes + (' + UN + ' ? 1 : 0);'))                              # 11
    m.append((LJ, '    cinfo->master->call_pass_startup = false;' + NL + '    ' + FNS + '(cinfo->marker->write_frame_header)(cinfo);',
              '    cinfo->master->call_pass_startup = false;' + NL + '    if (' + UN + ') ' + FNS + '(cinfo->marker->write_scan_header)(cinfo);' + NL + '    ' + FNS + '(cinfo->marker->write_frame_header)(cinfo);'))   # 12
    m.append((LJ, '    master->pass_number = 0;', '    master->pass_number = ' + UN + ' ? 1 : 0;'))                                                                               # 13
    m.append((LJ, '        cinfo->mem->free_pool(cinfo, 1); // JPOOL_IMAGE', '        cinfo->mem->free_pool(cinfo, ' + UN + ' ? 2 : 1); // JPOOL_IMAGE'))                                              # 14
    m.append((LJ, '    cinfo->mem = nullptr;', '    cinfo->mem = ' + UN + ' ? cinfo->mem : nullptr;'))                                                                          # 15
    m.append((LJ, 'alloc_small(cinfo, 0, sizeof(ExeJpegQuantTable)));', 'alloc_small(cinfo, ' + UN + ' ? 1 : 0, sizeof(ExeJpegQuantTable)));'))                                                # 16
    m.append((LJ, 'alloc_small(cinfo, 0, sizeof(ExeJpegHuffTable)));', 'alloc_small(cinfo, ' + UN + ' ? 1 : 0, sizeof(ExeJpegHuffTable)));'))                                                  # 17
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
