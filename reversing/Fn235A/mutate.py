"""Prueba negativa del banco Fn235A (lote 235A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (LibJpegDecompressUpsample.cpp, LibJpegError.cpp, LibJpegMemory.cpp y LibJpegCompressApi.cpp).

    python -I mutate.py apply 1        guarda la copia buena en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba que cada texto original aparece exactamente una vez en las fuentes actuales (sin tocar nada)
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los
falsos) en las 30 funciones. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si (un cinfo o un
puntero desalineado a 4 bytes, un campo impar). Ninguna mutacion cambia la convencion de llamada (una pila desbalanceada rompe la compuerta del banco antes de poder compararla).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
JD = 'source/game_sa/Plugins/JPegCompressPlugin/'
UP = JD + 'LibJpegDecompressUpsample.cpp'
ER = JD + 'LibJpegError.cpp'
ME = JD + 'LibJpegMemory.cpp'
CA = JD + 'LibJpegCompressApi.cpp'
SOURCES = [UP, ER, ME, CA]
NL = chr(10)


def U(name, mask='3'):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & ' + mask + ')'


def round1():
    out = []
    # 0 jinit_post_controller: otro start_pass con un cinfo desalineado
    out.append((UP, '    At<uint32>(post, 0x00) = 0x5CCD70;', '    At<uint32>(post, 0x00) = ' + U('cinfo') + ' ? 0x5CCD71 : 0x5CCD70;'))
    # 1 sep_upsample: next_row_out arranca en 1 con un cinfo desalineado
    out.append((UP, '        At<uint32>(up, 0x2C) = 0;', '        At<uint32>(up, 0x2C) = ' + U('cinfo') + ' ? 1 : 0;'))
    # 2 jinit_upsampler: otro start_pass con un cinfo desalineado
    out.append((UP, '    At<uint32>(up, 0x00) = 0x5CCE60;', '    At<uint32>(up, 0x00) = ' + U('cinfo') + ' ? 0x5CCE61 : 0x5CCE60;'))
    # 3 error_exit: exit(2) con un cinfo desalineado
    out.append((ER, '    reinterpret_cast<void(__cdecl*)(int32)>(0x823CAE)(1);', '    reinterpret_cast<void(__cdecl*)(int32)>(0x823CAE)(' + U('cinfo') + ' ? 2 : 1);'))
    # 4 output_message: otro FILE con un cinfo desalineado
    out.append((ER, 'reinterpret_cast<void*>(0x8E2C70), reinterpret_cast<const char*>(0x862EB0), buffer);', 'reinterpret_cast<void*>(' + U('cinfo') + ' ? 0x8E2C71 : 0x8E2C70), reinterpret_cast<const char*>(0x862EB0), buffer);'))
    # 5 jpeg_std_error: last_jpeg_message = 2 con un err desalineado
    out.append((ER, '    At<uint32>(err, 0x74) = 1;', '    At<uint32>(err, 0x74) = ' + U('err') + ' ? 2 : 1;'))
    # 6 alloc_small: devuelve un byte mas con un cinfo desalineado
    out.append((ME, '    return dataPtr;', '    return dataPtr + (' + U('cinfo') + ' ? 1 : 0);'))
    # 7 alloc_large: devuelve un byte mas con un cinfo desalineado
    out.append((ME, '    return hdr + 0x10;', '    return hdr + 0x10 + (' + U('cinfo') + ' ? 1 : 0);'))
    # 8 alloc_sarray: el paso entre filas crece con un cinfo desalineado
    out.append((ME, '                    workspace += samplesperrow;', '                    workspace += samplesperrow + (' + U('cinfo') + ' ? 1 : 0);'))
    # 9 alloc_barray: el paso entre filas crece con un cinfo desalineado
    out.append((ME, '                    workspace += rowBytes;', '                    workspace += rowBytes + (' + U('cinfo') + ' ? 1 : 0);'))
    # 10 request_virt_sarray: el enlace next apunta 4 bytes mas alla con un cinfo desalineado
    out.append((ME, '    At<uint32>(result, 0x24) = At<uint32>(mem, 0x44);', '    At<uint32>(result, 0x24) = At<uint32>(mem, 0x44) + (' + U('cinfo') + ' ? 4 : 0);'))
    # 11 request_virt_barray: igual con la lista de bloques
    out.append((ME, '    At<uint32>(result, 0x24) = At<uint32>(mem, 0x48);', '    At<uint32>(result, 0x24) = At<uint32>(mem, 0x48) + (' + U('cinfo') + ' ? 4 : 0);'))
    # 12 realize_virt_arrays: cur_start_row = 1 con un cinfo desalineado (matrices de muestras)
    out.append((ME, '        At<uint32>(sptr, 0x18) = 0;', '        At<uint32>(sptr, 0x18) = ' + U('cinfo') + ' ? 1 : 0;'))
    # 13 free_pool: el total se resta con un byte menos al liberar la lista grande con un cinfo desalineado
    out.append((ME, '        ExeJpegFreeLarge(cinfo, lhdr, spaceFreed);' + NL + '        At<uint32>(mem, 0x4C) -= spaceFreed;', '        ExeJpegFreeLarge(cinfo, lhdr, spaceFreed);' + NL + '        At<uint32>(mem, 0x4C) -= spaceFreed - (' + U('cinfo') + ' ? 1 : 0);'))
    # 14 jinit_memory_mgr: max_alloc_chunk distinto con un cinfo desalineado
    out.append((ME, '    At<uint32>(mem, 0x30) = EXE_MAX_ALLOC_CHUNK;', '    At<uint32>(mem, 0x30) = EXE_MAX_ALLOC_CHUNK + (' + U('cinfo') + ' ? 1 : 0);'))
    # 15 jpeg_get_small: pide un byte mas con un cinfo desalineado
    out.append((ME, 'void* ExeJpegGetSmall(void* cinfo, uint32 sizeofobject) {' + NL + '    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject, 0);', 'void* ExeJpegGetSmall(void* cinfo, uint32 sizeofobject) {' + NL + '    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject + (' + U('cinfo') + ' ? 1 : 0), 0);'))
    # 16 jpeg_free_small: libera otro puntero con un cinfo desalineado
    out.append((ME, 'void ExeJpegFreeSmall(void* cinfo, void* object, uint32 sizeofobject) {' + NL + '    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(object);', 'void ExeJpegFreeSmall(void* cinfo, void* object, uint32 sizeofobject) {' + NL + '    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(static_cast<uint8*>(object) + (' + U('cinfo') + ' ? 1 : 0));'))
    # 17 jpeg_get_large: pide un byte mas con un cinfo desalineado
    out.append((ME, 'void* ExeJpegGetLarge(void* cinfo, uint32 sizeofobject) {' + NL + '    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject, 0);', 'void* ExeJpegGetLarge(void* cinfo, uint32 sizeofobject) {' + NL + '    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject + (' + U('cinfo') + ' ? 1 : 0), 0);'))
    # 18 jpeg_free_large: libera otro puntero con un cinfo desalineado
    out.append((ME, 'void ExeJpegFreeLarge(void* cinfo, void* object, uint32 sizeofobject) {' + NL + '    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(object);', 'void ExeJpegFreeLarge(void* cinfo, void* object, uint32 sizeofobject) {' + NL + '    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(static_cast<uint8*>(object) + (' + U('cinfo') + ' ? 1 : 0));'))
    # 19 jpeg_mem_available: devuelve un valor mas con un cinfo desalineado
    out.append((ME, '    return maxBytesNeeded;', '    return maxBytesNeeded + (' + U('cinfo') + ' ? 1 : 0);'))
    # 20 jpeg_open_backing_store: otro codigo de error con un cinfo desalineado
    out.append((ME, '    ErrExit(cinfo, JERR_NO_BACKING_STORE);' + NL + '}' + NL + NL + '// 0x5CF870', '    ErrExit(cinfo, JERR_NO_BACKING_STORE + (' + U('cinfo') + ' ? 1 : 0));' + NL + '}' + NL + NL + '// 0x5CF870'))
    # 21 jpeg_mem_init: devuelve 1 con un cinfo desalineado
    out.append((ME, 'int32 ExeJpegMemInit(void* cinfo) {' + NL + '    return 0;', 'int32 ExeJpegMemInit(void* cinfo) {' + NL + '    return ' + U('cinfo') + ' ? 1 : 0;'))
    # 22 jpeg_mem_term: deja un valor en la pila x87 con un cinfo desalineado (la funcion no escribe nada observable y su argumento puede ser un puntero crudo: no se desreferencia)
    out.append((ME, 'void ExeJpegMemTerm(void* cinfo) {' + NL + '}', 'void ExeJpegMemTerm(void* cinfo) {' + NL + '    if (' + U('cinfo') + ') { __asm { fld1 } }   // deja un valor en la pila x87' + NL + '}'))
    # 23 jdiv_round_up: suma uno con a negativo
    out.append((ME, '    return (a + b - 1) / b;', '    return (a + b - 1) / b + (a < 0 ? 1 : 0);'))
    # 24 jround_up: suma uno con a negativo
    out.append((ME, '    return sum - sum % b;', '    return sum - sum % b + (a < 0 ? 1 : 0);'))
    # 25 jcopy_sample_rows: el ultimo byte de la cola cambia con una fila de origen en direccion impar
    out.append((ME, '            *dst++ = *src++;', '            *dst++ = *src++ ^ ' + U('src', '1') + ';'))
    # 26 jzero_far: la primera palabra no se pone a cero con un destino en direccion impar
    out.append((ME, '        *reinterpret_cast<uint32*>(dst) = 0;', '        *reinterpret_cast<uint32*>(dst) = ' + U('target', '1') + ';'))
    # 27 jpeg_CreateCompress: la parte alta de input_gamma cambia con un cinfo desalineado
    out.append((CA, '    At<uint32>(cinfo, 0x34) = 0x3FF00000;', '    At<uint32>(cinfo, 0x34) = ' + U('cinfo') + ' ? 0x3FF00001 : 0x3FF00000;'))
    # 28 jpeg_suppress_tables: la tabla de cuantificacion recibe otro valor con un cinfo desalineado
    out.append((CA, '            At<uint8>(qtbl, 0x80) = value;', '            At<uint8>(qtbl, 0x80) = value ^ (' + U('cinfo') + ' ? 1 : 0);'))
    # 29 jpeg_finish_compress: no llama a jpeg_abort con un cinfo desalineado
    out.append((CA, '    reinterpret_cast<void(__cdecl*)(void*)>(0x5C6B50)(cinfo); // jpeg_abort', '    if (!' + U('cinfo') + ') reinterpret_cast<void(__cdecl*)(void*)>(0x5C6B50)(cinfo); // jpeg_abort'))
    return out


def round2():
    out = []
    # 0 jinit_post_controller: whole_image = 1 con un cinfo desalineado
    out.append((UP, '    At<uint32>(post, 0x08) = 0;', '    At<uint32>(post, 0x08) = ' + U('cinfo') + ' ? 1 : 0;'))
    # 1 sep_upsample: in_row_group_ctr avanza dos con un cinfo desalineado
    out.append((UP, '        ++(*inRowGroupCtr);', '        *inRowGroupCtr += ' + U('cinfo') + ' ? 2 : 1;'))
    # 2 jinit_upsampler: otro metodo upsample con un cinfo desalineado
    out.append((UP, '    At<uint32>(up, 0x04) = 0x5CCE80;', '    At<uint32>(up, 0x04) = ' + U('cinfo') + ' ? 0x5CCE81 : 0x5CCE80;'))
    # 3 error_exit: jpeg_destroy recibe otro cinfo con un cinfo desalineado
    out.append((ER, '    reinterpret_cast<void(__cdecl*)(void*)>(0x5C6B90)(cinfo);', '    reinterpret_cast<void(__cdecl*)(void*)>(0x5C6B90)(static_cast<uint8*>(cinfo) + (' + U('cinfo') + ' ? 1 : 0));'))
    # 4 output_message: format_message escribe un byte mas alla con un cinfo desalineado
    out.append((ER, 'At<void(__cdecl*)(void*, char*)>(At<void*>(cinfo, 0x00), 0x0C)(cinfo, buffer);', 'At<void(__cdecl*)(void*, char*)>(At<void*>(cinfo, 0x00), 0x0C)(cinfo, buffer + (' + U('cinfo') + ' ? 1 : 0));'))
    # 5 jpeg_std_error: num_warnings = 1 con un err desalineado
    out.append((ER, '    At<uint32>(err, 0x6C) = 0;', '    At<uint32>(err, 0x6C) = ' + U('err') + ' ? 1 : 0;'))
    # 6 alloc_small: bytes_in_use crece uno de mas con un cinfo desalineado
    out.append((ME, '    At<uint32>(hdr, 0x04) += sizeofobject;', '    At<uint32>(hdr, 0x04) += sizeofobject + (' + U('cinfo') + ' ? 1 : 0);'))
    # 7 alloc_large: bytes_left = 1 con un cinfo desalineado
    out.append((ME, '    At<uint32>(hdr, 0x08) = 0;', '    At<uint32>(hdr, 0x08) = ' + U('cinfo') + ' ? 1 : 0;'))
    # 8 alloc_sarray: el tamano de cada alloc_large crece un byte con un cinfo desalineado
    out.append((ME, 'ExeAllocLarge(cinfo, poolId, thisRows * samplesperrow));', 'ExeAllocLarge(cinfo, poolId, thisRows * samplesperrow + (' + U('cinfo') + ' ? 1 : 0)));'))
    # 9 alloc_barray: el tamano de cada alloc_large crece un byte con un cinfo desalineado
    out.append((ME, 'ExeAllocLarge(cinfo, poolId, (thisRows * blocksperrow) << 7));', 'ExeAllocLarge(cinfo, poolId, ((thisRows * blocksperrow) << 7) + (' + U('cinfo') + ' ? 1 : 0)));'))
    # 10 request_virt_sarray: samplesperrow se guarda con uno mas con un cinfo desalineado
    out.append((ME, '    At<uint32>(result, 0x08) = samplesperrow;', '    At<uint32>(result, 0x08) = samplesperrow + (' + U('cinfo') + ' ? 1 : 0);'))
    # 11 request_virt_barray: blocksperrow se guarda con uno mas con un cinfo desalineado
    out.append((ME, '    At<uint32>(result, 0x08) = blocksperrow;', '    At<uint32>(result, 0x08) = blocksperrow + (' + U('cinfo') + ' ? 1 : 0);'))
    # 12 realize_virt_arrays: b_s_open = 2 con un cinfo desalineado (matrices de muestras)
    out.append((ME, '            At<uint8>(sptr, 0x22) = 1;', '            At<uint8>(sptr, 0x22) = ' + U('cinfo') + ' ? 2 : 1;'))
    # 13 free_pool: close_backing_store recibe otra direccion con un cinfo desalineado (matrices de muestras)
    out.append((ME, '(cinfo, sptr + 0x28);', '(cinfo, sptr + 0x28 + (' + U('cinfo') + ' ? 1 : 0));'))
    # 14 jinit_memory_mgr: total_space_allocated inicial distinto con un cinfo desalineado
    out.append((ME, '    At<uint32>(mem, 0x4C) = 0x54;', '    At<uint32>(mem, 0x4C) = 0x54 + (' + U('cinfo') + ' ? 1 : 0);'))
    # 15 jpeg_get_small: el segundo argumento de malloc es 1 con un cinfo desalineado
    out.append((ME, 'void* ExeJpegGetSmall(void* cinfo, uint32 sizeofobject) {' + NL + '    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject, 0);', 'void* ExeJpegGetSmall(void* cinfo, uint32 sizeofobject) {' + NL + '    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject, ' + U('cinfo') + ' ? 1 : 0);'))
    # 16 jpeg_free_small: no libera con un cinfo desalineado
    out.append((ME, 'void ExeJpegFreeSmall(void* cinfo, void* object, uint32 sizeofobject) {' + NL + '    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(object);', 'void ExeJpegFreeSmall(void* cinfo, void* object, uint32 sizeofobject) {' + NL + '    if (!' + U('cinfo') + ') reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(object);'))
    # 17 jpeg_get_large: el segundo argumento de malloc es 1 con un cinfo desalineado
    out.append((ME, 'void* ExeJpegGetLarge(void* cinfo, uint32 sizeofobject) {' + NL + '    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject, 0);', 'void* ExeJpegGetLarge(void* cinfo, uint32 sizeofobject) {' + NL + '    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject, ' + U('cinfo') + ' ? 1 : 0);'))
    # 18 jpeg_free_large: no libera con un cinfo desalineado
    out.append((ME, 'void ExeJpegFreeLarge(void* cinfo, void* object, uint32 sizeofobject) {' + NL + '    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(object);', 'void ExeJpegFreeLarge(void* cinfo, void* object, uint32 sizeofobject) {' + NL + '    if (!' + U('cinfo') + ') reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(object);'))
    # 19 jpeg_mem_available: resta uno con already_allocated negativo
    out.append((ME, '    return maxBytesNeeded;', '    return maxBytesNeeded - (alreadyAllocated < 0 ? 1 : 0);'))
    # 20 jpeg_open_backing_store: no llama a ERREXIT con un cinfo desalineado
    out.append((ME, '    ErrExit(cinfo, JERR_NO_BACKING_STORE);' + NL + '}' + NL + NL + '// 0x5CF870', '    if (!' + U('cinfo') + ') ErrExit(cinfo, JERR_NO_BACKING_STORE);' + NL + '}' + NL + NL + '// 0x5CF870'))
    # 21 jpeg_mem_init: devuelve 7 con un cinfo impar
    out.append((ME, 'int32 ExeJpegMemInit(void* cinfo) {' + NL + '    return 0;', 'int32 ExeJpegMemInit(void* cinfo) {' + NL + '    return ' + U('cinfo', '1') + ' ? 7 : 0;'))
    # 22 jpeg_mem_term: deja otro valor en la pila x87 con un cinfo desalineado a 2
    out.append((ME, 'void ExeJpegMemTerm(void* cinfo) {' + NL + '}', 'void ExeJpegMemTerm(void* cinfo) {' + NL + '    if (' + U('cinfo', '2') + ') { __asm { fldz } }   // deja un valor en la pila x87' + NL + '}'))
    # 23 jdiv_round_up: suma uno con b negativo
    out.append((ME, '    return (a + b - 1) / b;', '    return (a + b - 1) / b + (b < 0 ? 1 : 0);'))
    # 24 jround_up: resta uno con b negativo
    out.append((ME, '    return sum - sum % b;', '    return sum - sum % b - (b < 0 ? 1 : 0);'))
    # 25 jcopy_sample_rows: no copia con 0x40 en num_cols
    out.append((ME, '    if (numRows <= 0) {' + NL + '        return;' + NL + '    }' + NL + '    do {' + NL + '        const uint8* src = *inptr;', '    if (numRows <= 0 || (numCols & 0x40)) {' + NL + '        return;' + NL + '    }' + NL + '    do {' + NL + '        const uint8* src = *inptr;'))
    # 26 jzero_far: el ultimo byte de la cola no se pone a cero con un destino en direccion impar
    out.append((ME, '        *dst++ = 0;', '        *dst++ = ' + U('dst', '1') + ';'))
    # 27 jpeg_CreateCompress: is_decompressor = 1 con un cinfo desalineado
    out.append((CA, '    At<uint8>(cinfo, 0x10) = 0;', '    At<uint8>(cinfo, 0x10) = ' + U('cinfo') + ' ? 1 : 0;'))
    # 28 jpeg_suppress_tables: la tabla Huffman DC recibe otro valor con un cinfo desalineado
    out.append((CA, '            At<uint8>(dctbl, 0x111) = value;', '            At<uint8>(dctbl, 0x111) = value ^ (' + U('cinfo') + ' ? 1 : 0);'))
    # 29 jpeg_finish_compress: no escribe el trailer con un cinfo desalineado
    out.append((CA, '    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x14C), 0x0C)(cinfo);', '    if (!' + U('cinfo') + ') At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x14C), 0x0C)(cinfo);'))
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    bad = 0
    for n, rows in ROUNDS.items():
        for i, (rel, old, new) in enumerate(rows):
            text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            c = text.count(old)
            if c != 1 or old == new:
                bad += 1
                print('RONDA', n, 'mutacion', i, rel, 'apariciones', c, repr(old[:90]))
    print('texto original no unico o vacio:', bad)
    return bad


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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
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
    elif sys.argv[1] == 'check':
        sys.exit(1 if check() else 0)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
