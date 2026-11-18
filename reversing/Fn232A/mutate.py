"""Prueba negativa del banco Fn232A (lote 232A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (JPegCompress.cpp).

    python -I mutate.py apply 1        guarda la copia buena en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba que cada texto original aparece exactamente una vez en las fuentes actuales (sin tocar nada)
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los
falsos) en las 15 funciones. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si (un cinfo o un
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
JP = 'source/game_sa/Plugins/JPegCompressPlugin/JPegCompress.cpp'
SOURCES = [JP]
NL = chr(10)


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def round1():
    out = []
    # 0 jpeg_add_quant_table: sent_table distinto de cero con un cinfo desalineado
    out.append((JP, '    (*qtblptr)->sent_table = 0;', '    (*qtblptr)->sent_table = ' + U('cinfo') + ' ? 1 : 0;'))
    # 1 jpeg_set_colorspace: guarda otro espacio de color con un cinfo desalineado
    out.append((JP, '    cinfo->jpeg_color_space   = colorspace;', '    cinfo->jpeg_color_space   = ' + U('cinfo') + ' ? colorspace + 1 : colorspace;'))
    # 2 jpeg_default_colorspace: RGB/YCbCr eligen RGB con un cinfo desalineado
    out.append((JP, 'jpeg_set_colorspace(cinfo, kCsYCbCr); break;', 'jpeg_set_colorspace(cinfo, ' + U('cinfo') + ' ? kCsRGB : kCsYCbCr); break;'))
    # 3 jpeg_set_defaults: precision de 9 bits con un cinfo desalineado
    out.append((JP, '    cinfo->data_precision = 8;', '    cinfo->data_precision = ' + U('cinfo') + ' ? 9 : 8;'))
    # 4 jinit_c_prep_controller: otro start_pass con un cinfo desalineado
    out.append((JP, '    prep->start_pass = 0x5C76A0;', '    prep->start_pass = ' + U('cinfo') + ' ? 0x5C76A1 : 0x5C76A0;'))
    # 5 expand_right_edge: un byte menos de relleno con una fila desalineada
    out.append((JP, '            std::memset(ptr, ptr[-1], static_cast<size_t>(numcols));', '            std::memset(ptr, ptr[-1], static_cast<size_t>(numcols) - (' + U('ptr') + ' == 1 ? 1 : 0));'))
    # 6 jinit_downsampler: otro start_pass con un cinfo desalineado
    out.append((JP, '    downsample->start_pass = 0x5C79E0;', '    downsample->start_pass = ' + U('cinfo') + ' ? 0x5C79E1 : 0x5C79E0;'))
    # 7 jpeg_read_scanlines: devuelve una fila de mas con un cinfo desalineado
    out.append((JP, '    return row_ctr;', '    return ' + U('cinfo') + ' ? row_ctr + 1 : row_ctr;'))
    # 8 jpeg_start_decompress: devuelve 0 con un cinfo desalineado
    out.append((JP, '    return output_pass_setup(cinfo);', '    return static_cast<uint8>(output_pass_setup(cinfo) ^ (' + U('cinfo') + ' ? 1 : 0));'))
    # 9 term_destination: no llama a fflush con un cinfo desalineado
    out.append((JP, '    ExeFflush(dest->outfile);', '    if (!' + U('cinfo') + ') ExeFflush(dest->outfile);'))
    # 10 jinit_d_coef_controller: otro start_input_pass con un cinfo desalineado
    out.append((JP, '    coef->start_input_pass = 0x5C8B40;', '    coef->start_input_pass = ' + U('cinfo') + ' ? 0x5C8B41 : 0x5C8B40;'))
    # 11 build_ycc_rgb_table: Cr_r sube una unidad con un cinfo desalineado
    out.append((JP, '        cconvert->Cr_r_tab[i] = (91881 * x + 0x8000) >> 16;', '        cconvert->Cr_r_tab[i] = ((91881 * x + 0x8000) >> 16) + (' + U('cinfo') + ' ? 1 : 0);'))
    # 12 null_convert: la primera columna se pone a cero con un cinfo desalineado
    out.append((JP, '                *outptr = *inptr++;', '                { const uint8 v = *inptr++; *outptr = (' + U('cinfo') + ' && col == 0) ? 0 : v; }'))
    # 13 jinit_color_deconverter: otro start_pass con un cinfo desalineado
    out.append((JP, '    cconvert->start_pass = 0x5C9340;', '    cconvert->start_pass = ' + U('cinfo') + ' ? 0x5C9341 : 0x5C9340;'))
    # 14 jinit_inverse_dct: otro start_pass con un cinfo desalineado
    out.append((JP, '    idct->start_pass = 0x5C9510;', '    idct->start_pass = ' + U('cinfo') + ' ? 0x5C9511 : 0x5C9510;'))
    return out


def round2():
    out = []
    # 0 jpeg_add_quant_table: el primer bit de cada entrada cambia con un cinfo desalineado
    out.append((JP, '        (*qtblptr)->quantval[i] = static_cast<uint16>(temp);', '        (*qtblptr)->quantval[i] = static_cast<uint16>(temp ^ (' + U('cinfo') + ' ? 1 : 0));'))
    # 1 jpeg_set_colorspace: la cabecera JFIF empieza a 1 con un cinfo desalineado
    out.append((JP, '    cinfo->write_JFIF_header  = 0;', '    cinfo->write_JFIF_header  = ' + U('cinfo') + ' ? 1 : 0;'))
    # 2 jpeg_default_colorspace: el espacio desconocido se trata como gris con un cinfo desalineado
    out.append((JP, 'jpeg_set_colorspace(cinfo, kCsUNKNOWN); break;', 'jpeg_set_colorspace(cinfo, ' + U('cinfo') + ' ? kCsGRAYSCALE : kCsUNKNOWN); break;'))
    # 3 jpeg_set_defaults: X_density = 2 con un cinfo desalineado
    out.append((JP, '    cinfo->X_density = 1;', '    cinfo->X_density = ' + U('cinfo') + ' ? 2 : 1;'))
    # 4 jinit_c_prep_controller: otro pre_process_data con un cinfo desalineado
    out.append((JP, '    prep->pre_process_data = 0x5C7710;', '    prep->pre_process_data = ' + U('cinfo') + ' ? 0x5C7711 : 0x5C7710;'))
    # 5 expand_right_edge: no rellena con un numero impar de columnas de entrada
    out.append((JP, '    if (numcols > 0) {', '    if (numcols > 0 && !(input_cols & 1)) {'))
    # 6 jinit_downsampler: need_context_rows = 1 con un cinfo desalineado
    out.append((JP, '    downsample->need_context_rows = 0;', '    downsample->need_context_rows = ' + U('cinfo') + ' ? 1 : 0;'))
    # 7 jpeg_read_scanlines: output_scanline avanza una fila mas con un cinfo desalineado
    out.append((JP, '    cinfo->output_scanline += row_ctr;', '    cinfo->output_scanline += ' + U('cinfo') + ' ? row_ctr + 1 : row_ctr;'))
    # 8 jpeg_start_decompress: output_scan_number se desplaza con un cinfo desalineado
    out.append((JP, '        cinfo->output_scan_number = cinfo->input_scan_number;', '        cinfo->output_scan_number = cinfo->input_scan_number + (' + U('cinfo') + ' ? 1 : 0);'))
    # 9 term_destination: el recuento a escribir baja una unidad con un cinfo desalineado
    out.append((JP, '    const uint32 datacount = 0x1000 - dest->free_in_buffer;', '    const uint32 datacount = 0x1000 - dest->free_in_buffer - (' + U('cinfo') + ' ? 1 : 0);'))
    # 10 jinit_d_coef_controller: coef_arrays apunta al controlador con un cinfo desalineado
    out.append((JP, '    coef->coef_arrays = nullptr;', '    coef->coef_arrays = ' + U('cinfo') + ' ? reinterpret_cast<void*>(coef) : nullptr;'))
    # 11 build_ycc_rgb_table: Cb_g sube una unidad con un cinfo desalineado
    out.append((JP, '        cconvert->Cb_g_tab[i] = -22554 * x + 0x8000;', '        cconvert->Cb_g_tab[i] = -22554 * x + 0x8000 + (' + U('cinfo') + ' ? 1 : 0);'))
    # 12 null_convert: lee la fila siguiente con un cinfo desalineado
    out.append((JP, '            const uint8* inptr = input_buf[ci][input_row];', '            const uint8* inptr = input_buf[ci][input_row + (' + U('cinfo') + ' ? 1 : 0)];'))
    # 13 jinit_color_deconverter: output_components sube una unidad con un cinfo desalineado (sin cuantizacion)
    out.append((JP, '        cinfo->output_components = cinfo->out_color_components;', '        cinfo->output_components = cinfo->out_color_components + (' + U('cinfo') + ' ? 1 : 0);'))
    # 14 jinit_inverse_dct: cur_method = -2 con un cinfo desalineado
    out.append((JP, '        idct->cur_method[ci] = -1;', '        idct->cur_method[ci] = ' + U('cinfo') + ' ? -2 : -1;'))
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
