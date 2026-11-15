#include "StdInc.h"

#include "LibJpegCompressEncode.h"

// Lote 230A (sin validar): rutinas de la libjpeg 6b del EXE (compresion). Trabajan sobre el jpeg_compress_struct del EXE; se accede a sus campos por desplazamiento (los de libjpeg-turbo de la DLL no coinciden).
// Desplazamientos usados del jpeg_compress_struct: +0x00 err, +0x04 mem, +0x08 progress, +0x14 global_state, +0x18 dest, +0x1C image_width, +0x20 image_height, +0x24 input_components, +0x28 in_color_space,
// +0x3C num_components, +0x40 jpeg_color_space, +0x44 comp_info (0x54 bytes cada uno: +0x08 h_samp_factor, +0x0C v_samp_factor, +0x1C width_in_blocks, +0x20 height_in_blocks), +0x58 dc_huff_tbl_ptrs[4],
// +0x68 ac_huff_tbl_ptrs[4], +0xA8 num_scans, +0xB0 raw_data_in, +0xB1 arith_code, +0xB2 optimize_coding, +0xB8 dct_method, +0xD0 next_scanline, +0xD4 progressive_mode, +0x13C master, +0x140 main, +0x148 coef,
// +0x14C marker, +0x150 cconvert, +0x158 fdct, +0x15C entropy. jpeg_error_mgr: +0x00 error_exit, +0x04 emit_message, +0x10 reset_error_mgr, +0x14 msg_code, +0x18 msg_parm.i[0].
// jpeg_memory_mgr: +0x00 alloc_small, +0x04 alloc_large, +0x08 alloc_sarray, +0x14 request_virt_barray, +0x18 realize_virt_arrays. Todas las rutinas del EXE que se llaman son cdecl.
namespace {
template<typename T>
inline T& At(void* base, uint32 offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(base) + offset);
}

// ERREXIT: err->msg_code = code; (*err->error_exit)(cinfo). Como el EXE, `err` se lee dos veces (para escribir y para llamar).
inline void ErrExit(void* cinfo, int32 code) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

// ERREXIT1: ademas msg_parm.i[0] = parm antes de llamar.
inline void ErrExit1(void* cinfo, int32 code, int32 parm) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<int32>(At<void*>(cinfo, 0x00), 0x18) = parm;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

inline void* AllocSmall(void* cinfo, int32 pool, uint32 size) {
    return At<void*(__cdecl*)(void*, int32, uint32)>(At<void*>(cinfo, 0x04), 0x00)(cinfo, pool, size);
}

inline void* AllocLarge(void* cinfo, int32 pool, uint32 size) {
    return At<void*(__cdecl*)(void*, int32, uint32)>(At<void*>(cinfo, 0x04), 0x04)(cinfo, pool, size);
}

inline long JRoundUp(long a, long b) { // jround_up (0x5CF8A0)
    return reinterpret_cast<long(__cdecl*)(long, long)>(0x5CF8A0)(a, b);
}

constexpr int32 JERR_ARITH_NOTIMPL = 1;
constexpr int32 JERR_BAD_BUFFER_MODE = 4;
constexpr int32 JERR_BAD_DCT_COEF = 6;
constexpr int32 JERR_BAD_HUFF_TABLE = 8;
constexpr int32 JERR_BAD_IN_COLORSPACE = 9;
constexpr int32 JERR_BAD_J_COLORSPACE = 10;
constexpr int32 JERR_BAD_STATE = 0x14;
constexpr int32 JERR_CONVERSION_NOTIMPL = 0x1B;
constexpr int32 JERR_HUFF_CLEN_OVERFLOW = 0x27;
constexpr int32 JERR_NOT_COMPILED = 0x30;
constexpr int32 JWRN_TOO_MUCH_DATA = 0x7B;

constexpr int32 CSTATE_START = 100;
constexpr int32 CSTATE_SCANNING = 101;
constexpr int32 CSTATE_RAW_OK = 102;

// 0x5C3660 (null_convert), 0x5C35F0 (grayscale_convert), 0x5C36F0 (null_method), 0x5C31E0 (rgb_ycc_start), 0x5C32C0 (rgb_ycc_convert), 0x5C33E0 (rgb_gray_convert), 0x5C3490 (cmyk_ycck_convert),
// 0x5C3940 (start_pass de jcdctmgr), 0x5C3B70 (forward_DCT_float), 0x5CD860 (jpeg_fdct_float), 0x5C2FF0 (start_pass_coef), 0x5C53D0 (start_pass_main), 0x5C5090 (start_pass_huff): se guardan por direccion.

// 0x5C5420 (cdecl, dos argumentos, 134 bytes; el listado decia FxEmitterBP_c): jinit_c_main_controller(cinfo, need_full_buffer). Reserva 0x28 bytes (JPOOL_IMAGE = 1) y los guarda en +0x140; start_pass_main
// (0x5C53D0) en la ranura 0. Con raw_data_in (+0xB0, byte) termina aqui. Sin raw_data_in: need_full_buffer (byte) -> ERREXIT(JERR_BAD_BUFFER_MODE) y vuelve; si no, por cada componente (num_components, +0x3C, releido en
// cada vuelta; el contador de comp_info avanza 0x54) main->buffer[ci] (+0x18 + 4*ci) = alloc_sarray(cinfo, 1, width_in_blocks << 3, v_samp_factor << 3) (mem+8).
void ExeJinitMainController(void* cinfo, int32 needFullBuffer) {
    void* const main = AllocSmall(cinfo, 1, 0x28);
    At<void*>(cinfo, 0x140) = main;
    At<uint32>(main, 0x00) = 0x5C53D0;
    if (At<uint8>(cinfo, 0xB0)) {
        return;
    }
    if (static_cast<uint8>(needFullBuffer)) {
        ErrExit(cinfo, JERR_BAD_BUFFER_MODE);
        return;
    }
    uint8* compptr = At<uint8*>(cinfo, 0x44);
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x3C); ++ci, compptr += 0x54) {
        const uint32 vSamp = *reinterpret_cast<uint32*>(compptr + 0x0C);
        const uint32 width = *reinterpret_cast<uint32*>(compptr + 0x1C);
        void* const mem = At<void*>(cinfo, 0x04);
        At<void*>(main, 0x18 + 4 * ci) = At<void*(__cdecl*)(void*, int32, uint32, uint32)>(mem, 0x08)(cinfo, 1, width << 3, vSamp << 3);
    }
}

// 0x5C30D0 (cdecl, dos argumentos, 259 bytes; el listado decia FxEmitterBP_c): jinit_c_coef_controller(cinfo, need_full_buffer). Reserva 0x50 bytes y los guarda en +0x148; start_pass_coef (0x5C2FF0) en la ranura 0.
// need_full_buffer (byte) != 0: por cada componente (num_components releido en cada vuelta) coef->whole_image[ci] (+0x40 + 4*ci) = request_virt_barray(cinfo, 1, 0, jround_up(width_in_blocks, h_samp_factor),
// jround_up(height_in_blocks, v_samp_factor), v_samp_factor) (mem+0x14; primero se calcula la altura redondeada y despues la anchura). Si no: un bloque de 0x500 bytes (alloc_large, mem+4) repartido en las diez
// MCU_buffer[i] (+0x18 + 4*i = bloque + 0x80*i) y whole_image[0] = 0.
void ExeJinitCoefController(void* cinfo, int32 needFullBuffer) {
    void* const coef = AllocSmall(cinfo, 1, 0x50);
    At<void*>(cinfo, 0x148) = coef;
    At<uint32>(coef, 0x00) = 0x5C2FF0;
    if (static_cast<uint8>(needFullBuffer)) {
        uint8* compptr = At<uint8*>(cinfo, 0x44);
        for (int32 ci = 0; ci < At<int32>(cinfo, 0x3C); ++ci, compptr += 0x54) {
            const uint32 vSamp = *reinterpret_cast<uint32*>(compptr + 0x0C);
            const uint32 hSamp = *reinterpret_cast<uint32*>(compptr + 0x08);
            void* const mem = At<void*>(cinfo, 0x04);
            const long heightRounded = JRoundUp(*reinterpret_cast<long*>(compptr + 0x20), static_cast<long>(vSamp));
            const long widthRounded = JRoundUp(*reinterpret_cast<long*>(compptr + 0x1C), static_cast<long>(hSamp));
            At<void*>(coef, 0x40 + 4 * ci) = At<void*(__cdecl*)(void*, int32, int32, long, long, uint32)>(mem, 0x14)(cinfo, 1, 0, widthRounded, heightRounded, vSamp);
        }
    } else {
        uint8* const buffer = static_cast<uint8*>(AllocLarge(cinfo, 1, 0x500));
        for (int32 i = 0; i < 10; ++i) {
            At<uint8*>(coef, 0x18 + 4 * i) = buffer + 0x80 * i;
        }
        At<void*>(coef, 0x40) = nullptr;
    }
}

// 0x5C35F0 (cdecl, cinco argumentos, 97 bytes; el listado decia FxEmitterBP_c): grayscale_convert(cinfo, input_buf, output_buf, output_row, num_rows) de jccolor. Lee image_width (+0x1C) e input_components (+0x24)
// una sola vez; por cada fila (num_rows con signo, `--n >= 0`) copia el byte 0 de cada pixel de entrada (paso = input_components) a output_buf[0][output_row++]; el contador de columnas es sin signo.
void ExeGrayscaleConvert(void* cinfo, uint8** inputBuf, uint8*** outputBuf, uint32 outputRow, int32 numRows) {
    const uint32 width = At<uint32>(cinfo, 0x1C);
    const int32 stride = At<int32>(cinfo, 0x24);
    while (--numRows >= 0) {
        const uint8* in = *inputBuf++;
        uint8* out = outputBuf[0][outputRow++];
        for (uint32 col = 0; col < width; ++col) {
            out[col] = *in;
            in += stride;
        }
    }
}

// 0x5C3700 (cdecl, un argumento, 531 bytes; el listado decia FxEmitterBP_c): jinit_color_converter(cinfo) de jccolor con las ramas que dejo el compilador del EXE. Reserva 0xC bytes, los guarda en +0x150 y
// pone null_method (0x5C36F0) en la ranura 0. Valida input_components (+0x24) segun in_color_space (+0x28): 1 -> 1, 2 -> 4, 3 -> 3, 4 y 5 -> 4, otro -> >= 1; si no, ERREXIT(JERR_BAD_IN_COLORSPACE) y SIGUE.
// Segun jpeg_color_space (+0x40): 1 (gris) exige num_components (+0x3C) == 1 (JERR_BAD_J_COLORSPACE) y elige grayscale_convert (entrada 1 o 3), rgb_gray (entrada 2: start 0x5C31E0, convert 0x5C33E0) o
// ERREXIT(JERR_CONVERSION_NOTIMPL); 2 (RGB) comprueba num_components == 3 y SIEMPRE termina en ERREXIT(JERR_CONVERSION_NOTIMPL) sin asignar convert; 3 (YCbCr) -> entrada 2: rgb_ycc (0x5C31E0/0x5C32C0), entrada 3:
// null_convert (0x5C3660), otro: NOTIMPL; 4 (CMYK) exige 4 y entrada 4 -> null_convert, si no NOTIMPL; 5 (YCCK) exige 4 y entrada 4 -> cmyk_ycck (0x5C31E0/0x5C3490), entrada 5 -> null_convert, si no NOTIMPL; otro
// valor: NOTIMPL si jpeg_color_space != in_color_space o num_components != input_components y despues null_convert (tambien tras el error). En los NOTIMPL de las ramas 1..5 vuelve sin asignar convert.
void ExeJinitColorConverter(void* cinfo) {
    void* const cconvert = AllocSmall(cinfo, 1, 0xC);
    At<void*>(cinfo, 0x150) = cconvert;
    At<uint32>(cconvert, 0x00) = 0x5C36F0;

    switch (At<int32>(cinfo, 0x28)) {
    case 1:
        if (At<int32>(cinfo, 0x24) != 1) { ErrExit(cinfo, JERR_BAD_IN_COLORSPACE); }
        break;
    case 2:
        if (At<int32>(cinfo, 0x24) != 4) { ErrExit(cinfo, JERR_BAD_IN_COLORSPACE); }
        break;
    case 3:
        if (At<int32>(cinfo, 0x24) != 3) { ErrExit(cinfo, JERR_BAD_IN_COLORSPACE); }
        break;
    case 4:
    case 5:
        if (At<int32>(cinfo, 0x24) != 4) { ErrExit(cinfo, JERR_BAD_IN_COLORSPACE); }
        break;
    default:
        if (At<int32>(cinfo, 0x24) < 1) { ErrExit(cinfo, JERR_BAD_IN_COLORSPACE); }
        break;
    }

    switch (At<int32>(cinfo, 0x40)) {
    case 1: // JCS_GRAYSCALE
        if (At<int32>(cinfo, 0x3C) != 1) { ErrExit(cinfo, JERR_BAD_J_COLORSPACE); }
        switch (At<int32>(cinfo, 0x28)) {
        case 1:
        case 3:
            At<uint32>(cconvert, 0x04) = 0x5C35F0;
            return;
        case 2:
            At<uint32>(cconvert, 0x00) = 0x5C31E0;
            At<uint32>(cconvert, 0x04) = 0x5C33E0;
            return;
        default:
            ErrExit(cinfo, JERR_CONVERSION_NOTIMPL);
            return;
        }
    case 2: // JCS_RGB
        if (At<int32>(cinfo, 0x3C) != 3) { ErrExit(cinfo, JERR_BAD_J_COLORSPACE); }
        ErrExit(cinfo, JERR_CONVERSION_NOTIMPL);
        return;
    case 3: // JCS_YCbCr
        if (At<int32>(cinfo, 0x3C) != 3) { ErrExit(cinfo, JERR_BAD_J_COLORSPACE); }
        switch (At<int32>(cinfo, 0x28)) {
        case 2:
            At<uint32>(cconvert, 0x00) = 0x5C31E0;
            At<uint32>(cconvert, 0x04) = 0x5C32C0;
            return;
        case 3:
            At<uint32>(cconvert, 0x04) = 0x5C3660;
            return;
        default:
            ErrExit(cinfo, JERR_CONVERSION_NOTIMPL);
            return;
        }
    case 4: // JCS_CMYK
        if (At<int32>(cinfo, 0x3C) != 4) { ErrExit(cinfo, JERR_BAD_J_COLORSPACE); }
        if (At<int32>(cinfo, 0x28) == 4) {
            At<uint32>(cconvert, 0x04) = 0x5C3660;
            return;
        }
        ErrExit(cinfo, JERR_CONVERSION_NOTIMPL);
        return;
    case 5: // JCS_YCCK
        if (At<int32>(cinfo, 0x3C) != 4) { ErrExit(cinfo, JERR_BAD_J_COLORSPACE); }
        switch (At<int32>(cinfo, 0x28)) {
        case 4:
            At<uint32>(cconvert, 0x00) = 0x5C31E0;
            At<uint32>(cconvert, 0x04) = 0x5C3490;
            return;
        case 5:
            At<uint32>(cconvert, 0x04) = 0x5C3660;
            return;
        default:
            ErrExit(cinfo, JERR_CONVERSION_NOTIMPL);
            return;
        }
    default:
        if (At<int32>(cinfo, 0x40) != At<int32>(cinfo, 0x28) || At<int32>(cinfo, 0x3C) != At<int32>(cinfo, 0x24)) {
            ErrExit(cinfo, JERR_CONVERSION_NOTIMPL);
        }
        At<uint32>(cconvert, 0x04) = 0x5C3660;
        return;
    }
}

// 0x5C4380 (cdecl, un argumento, 108 bytes; el listado decia FxEmitterBP_c): jinit_forward_dct(cinfo) de jcdctmgr, solo con el DCT en coma flotante. Reserva 0x30 bytes, los guarda en +0x158 y pone 0x5C3940
// en la ranura 0. Si dct_method (+0xB8) != 2 (JDCT_FLOAT): ERREXIT(JERR_NOT_COMPILED); si no, +0x04 = 0x5C3B70 (forward_DCT) y +0x1C = 0x5CD860 (jpeg_fdct_float). Despues pone a cero los cuatro divisores
// enteros (+0x0C..+0x18) y los cuatro de coma flotante (+0x20..+0x2C), por pares en cada vuelta (incluso tras el error).
void ExeJinitForwardDct(void* cinfo) {
    void* const fdct = AllocSmall(cinfo, 1, 0x30);
    At<void*>(cinfo, 0x158) = fdct;
    At<uint32>(fdct, 0x00) = 0x5C3940;
    if (At<int32>(cinfo, 0xB8) != 2) {
        ErrExit(cinfo, JERR_NOT_COMPILED);
    } else {
        At<uint32>(fdct, 0x04) = 0x5C3B70;
        At<uint32>(fdct, 0x1C) = 0x5CD860;
    }
    for (int32 i = 0; i < 4; ++i) {
        At<uint32>(fdct, 0x0C + 4 * i) = 0;
        At<uint32>(fdct, 0x20 + 4 * i) = 0;
    }
}

// 0x5C5210 (cdecl, un argumento, 66 bytes; el listado decia FxEmitterBP_c): jinit_huff_encoder(cinfo) de jchuff. Reserva 0x6C bytes, los guarda en +0x15C y pone start_pass_huff (0x5C5090) en la ranura 0.
// Pone a cero, de cuatro en cuatro, dc_derived_tbls (+0x2C), ac_derived_tbls (+0x3C), dc_count_ptrs (+0x4C) y ac_count_ptrs (+0x5C); en cada vuelta escribe +0x3C, +0x2C, +0x5C y +0x4C.
void ExeJinitHuffEncoder(void* cinfo) {
    void* const entropy = AllocSmall(cinfo, 1, 0x6C);
    At<void*>(cinfo, 0x15C) = entropy;
    At<uint32>(entropy, 0x00) = 0x5C5090;
    for (int32 i = 0; i < 4; ++i) {
        At<uint32>(entropy, 0x3C + 4 * i) = 0;
        At<uint32>(entropy, 0x2C + 4 * i) = 0;
        At<uint32>(entropy, 0x5C + 4 * i) = 0;
        At<uint32>(entropy, 0x4C + 4 * i) = 0;
    }
}

// 0x5C5260 (cdecl, un argumento, 188 bytes; el listado decia FxEmitterBP_c): jinit_compress_master(cinfo) de jcinit. Orden: jinit_c_master_control(cinfo, 0) (0x5C6A90); sin raw_data_in (+0xB0, byte):
// jinit_color_converter, jinit_downsampler (0x5C7DF0) y jinit_c_prep_controller(cinfo, 0) (0x5C7920); jinit_forward_dct; si arith_code (+0xB1, byte) ERREXIT(JERR_ARITH_NOTIMPL) y si no, con progressive_mode
// (+0xD4, byte) ERREXIT(JERR_NOT_COMPILED) o jinit_huff_encoder; need_full = num_scans (+0xA8, con signo) > 1 || optimize_coding (+0xB2, byte); jinit_c_coef_controller(cinfo, need_full);
// jinit_c_main_controller(cinfo, 0); jinit_marker_writer (0x5C63D0); mem->realize_virt_arrays(cinfo) (mem+0x18) y marker->write_file_header(cinfo) (+0x14C, ranura 0).
void ExeJinitCompressMaster(void* cinfo) {
    reinterpret_cast<void(__cdecl*)(void*, int32)>(0x5C6A90)(cinfo, 0);
    if (!At<uint8>(cinfo, 0xB0)) {
        ExeJinitColorConverter(cinfo);
        reinterpret_cast<void(__cdecl*)(void*)>(0x5C7DF0)(cinfo);
        reinterpret_cast<void(__cdecl*)(void*, int32)>(0x5C7920)(cinfo, 0);
    }
    ExeJinitForwardDct(cinfo);
    if (At<uint8>(cinfo, 0xB1)) {
        ErrExit(cinfo, JERR_ARITH_NOTIMPL);
    } else if (At<uint8>(cinfo, 0xD4)) {
        ErrExit(cinfo, JERR_NOT_COMPILED);
    } else {
        ExeJinitHuffEncoder(cinfo);
    }
    const int32 needFull = (At<int32>(cinfo, 0xA8) > 1 || At<uint8>(cinfo, 0xB2)) ? 1 : 0;
    ExeJinitCoefController(cinfo, needFull);
    ExeJinitMainController(cinfo, 0);
    reinterpret_cast<void(__cdecl*)(void*)>(0x5C63D0)(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x04), 0x18)(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x14C), 0x00)(cinfo);
}

// 0x5C26B0 (cdecl, dos argumentos, 117 bytes; el listado decia FxEmitterBP_c): jpeg_start_compress(cinfo, write_all_tables) de jcapistd. global_state (+0x14) != CSTATE_START (100): ERREXIT1(JERR_BAD_STATE,
// global_state) y sigue. Con write_all_tables (byte) != 0: jpeg_suppress_tables(cinfo, 0) (0x5CFA50). Despues err->reset_error_mgr (+0x10), dest->init_destination (dest = +0x18, ranura +8), jinit_compress_master,
// master->prepare_for_pass (+0x13C, ranura 0), next_scanline (+0xD0) = 0 y global_state = (raw_data_in (+0xB0, byte) != 0) ? CSTATE_RAW_OK : CSTATE_SCANNING.
void ExeJpegStartCompress(void* cinfo, int32 writeAllTables) {
    if (At<int32>(cinfo, 0x14) != CSTATE_START) {
        ErrExit1(cinfo, JERR_BAD_STATE, At<int32>(cinfo, 0x14));
    }
    if (static_cast<uint8>(writeAllTables)) {
        reinterpret_cast<void(__cdecl*)(void*, int32)>(0x5CFA50)(cinfo, 0);
    }
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x10)(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x18), 0x08)(cinfo);
    ExeJinitCompressMaster(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x13C), 0x00)(cinfo);
    At<uint32>(cinfo, 0xD0) = 0;
    At<int32>(cinfo, 0x14) = At<uint8>(cinfo, 0xB0) ? CSTATE_RAW_OK : CSTATE_SCANNING;
}

// 0x5C2730 (cdecl, tres argumentos, 194 bytes; el listado decia FxEmitterBP_c): jpeg_write_scanlines(cinfo, scanlines, num_lines) de jcapistd. global_state != CSTATE_SCANNING (101): ERREXIT1(JERR_BAD_STATE, state)
// y sigue. next_scanline (+0xD0) >= image_height (+0x20), sin signo: WARNMS(JWRN_TOO_MUCH_DATA): msg_code = 0x7B y err->emit_message(cinfo, -1) (+4). Con progress (+0x08): pass_counter (+4) = next_scanline, pass_limit (+8)
// = image_height y progress_monitor (+0) (cinfo). Con master->call_pass_startup (+0x13C, byte +0xC): master->pass_startup (+4)(cinfo). rows_left = image_height - next_scanline; num_lines = min(num_lines, rows_left) sin
// signo; row_ctr = 0 y main->process_data (+0x140, ranura +4)(cinfo, scanlines, &row_ctr, num_lines); next_scanline += row_ctr; devuelve row_ctr.
uint32 ExeJpegWriteScanlines(void* cinfo, uint8** scanlines, uint32 numLines) {
    if (At<int32>(cinfo, 0x14) != CSTATE_SCANNING) {
        ErrExit1(cinfo, JERR_BAD_STATE, At<int32>(cinfo, 0x14));
    }
    if (At<uint32>(cinfo, 0xD0) >= At<uint32>(cinfo, 0x20)) {
        At<int32>(At<void*>(cinfo, 0x00), 0x14) = JWRN_TOO_MUCH_DATA;
        At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x00), 0x04)(cinfo, -1);
    }
    if (void* const progress = At<void*>(cinfo, 0x08)) {
        At<uint32>(progress, 0x04) = At<uint32>(cinfo, 0xD0);
        At<uint32>(progress, 0x08) = At<uint32>(cinfo, 0x20);
        At<void(__cdecl*)(void*)>(progress, 0x00)(cinfo);
    }
    if (void* const master = At<void*>(cinfo, 0x13C); At<uint8>(master, 0x0C)) {
        At<void(__cdecl*)(void*)>(master, 0x04)(cinfo);
    }
    const uint32 rowsLeft = At<uint32>(cinfo, 0x20) - At<uint32>(cinfo, 0xD0);
    if (numLines > rowsLeft) {
        numLines = rowsLeft;
    }
    uint32 rowCtr = 0;
    At<void(__cdecl*)(void*, uint8**, uint32*, uint32)>(At<void*>(cinfo, 0x140), 0x04)(cinfo, scanlines, &rowCtr, numLines);
    At<uint32>(cinfo, 0xD0) += rowCtr;
    return rowCtr;
}

// 0x5C43F0 (cdecl, cuatro argumentos, 520 bytes; el listado decia FxEmitterBP_c): jpeg_make_c_derived_tbl(cinfo, isDC, tblno, pdtbl) de jchuff. tblno fuera de 0..3 o tabla nula (dc_huff_tbl_ptrs +0x58 si el BYTE
// isDC != 0, si no ac_huff_tbl_ptrs +0x68): ERREXIT1(JERR_NO_HUFF_TABLE = 0x32, tblno) y sigue. Si *pdtbl es nulo lo reserva (0x500 bytes, JPOOL_IMAGE). Genera huffsize/huffcode como libjpeg; cualquier incoherencia ->
// ERREXIT(JERR_BAD_HUFF_TABLE) y sigue. Pone a cero ehufsi (+0x400, 256 bytes) y, para cada simbolo (maxsymbol 15 si isDC, si no 255), ehufco[i] (+0) = huffcode[p], ehufsi[i] = huffsize[p].
void ExeJpegMakeCDerivedTbl(void* cinfo, int32 isDC, int32 tblno, void** pdtbl) {
    if (tblno < 0 || tblno >= 4) {
        ErrExit1(cinfo, 0x32, tblno);
    }
    uint8* htbl = static_cast<uint8*>(static_cast<uint8>(isDC) ? At<void*>(cinfo, 0x58 + tblno * 4) : At<void*>(cinfo, 0x68 + tblno * 4));
    if (!htbl) {
        ErrExit1(cinfo, 0x32, tblno);
    }
    if (*pdtbl == nullptr) {
        *pdtbl = AllocSmall(cinfo, 1, 0x500);
    }
    uint8* const dtbl = static_cast<uint8*>(*pdtbl);

    uint8 huffsize[257];
    uint32 huffcode[257];
    int32 p = 0;
    for (int32 l = 1; l <= 16; ++l) {
        int32 i = htbl[l];
        if (i < 0 || p + i > 256) {
            ErrExit(cinfo, JERR_BAD_HUFF_TABLE);
        }
        while (i--) {
            huffsize[p++] = static_cast<uint8>(l);
        }
    }
    huffsize[p] = 0;
    const int32 lastp = p;

    int32 code = 0;
    int32 si = huffsize[0];
    p = 0;
    while (huffsize[p]) {
        while (static_cast<int32>(huffsize[p]) == si) {
            huffcode[p++] = static_cast<uint32>(code);
            code++;
        }
        if (code >= (1 << si)) {
            ErrExit(cinfo, JERR_BAD_HUFF_TABLE);
        }
        code <<= 1;
        si++;
    }

    std::memset(dtbl + 0x400, 0, 0x100);
    const int32 maxSymbol = static_cast<uint8>(isDC) ? 15 : 255;
    for (p = 0; p < lastp; ++p) {
        const int32 i = htbl[0x11 + p];
        if (i < 0 || i > maxSymbol || dtbl[0x400 + i]) {
            ErrExit(cinfo, JERR_BAD_HUFF_TABLE);
        }
        *reinterpret_cast<uint32*>(dtbl + i * 4) = huffcode[p];
        dtbl[0x400 + i] = huffsize[p];
    }
}

// 0x5C4D50 (cdecl, tres argumentos, 571 bytes; el listado decia FxEmitterBP_c): jpeg_gen_optimal_table(cinfo, htbl, freq) de jchuff. freq[256] = 1; en cada vuelta busca el menor (c1) y el segundo menor (c2, distinto de
// c1) de los freq != 0 con `<=` (v inicial 1000000000, indices 0..256); sin c2 termina; fusiona (freq[c1] += freq[c2], freq[c2] = 0) y aumenta codesize por las dos cadenas `others`. Cuenta longitudes en bits[]
// (>32: ERREXIT(JERR_HUFF_CLEN_OVERFLOW) y sigue), las reduce a 16 como libjpeg, copia bits[0..16] a htbl (+0x00), escribe huffval (+0x11) ordenado por longitud y simbolo y pone sent_table (+0x111) a FALSE.
void ExeJpegGenOptimalTable(void* cinfo, void* htblArg, int32* freq) {
    uint8* const htbl = static_cast<uint8*>(htblArg);
    uint8 bits[33];
    int32 codesize[257];
    int32 others[257];
    std::memset(bits, 0, sizeof(bits));
    std::memset(codesize, 0, sizeof(codesize));
    std::memset(others, 0xFF, sizeof(others));
    freq[256] = 1;

    for (;;) {
        int32 c1 = -1;
        int32 v = 1000000000;
        for (int32 i = 0; i <= 256; ++i) {
            if (freq[i] && freq[i] <= v) {
                v = freq[i];
                c1 = i;
            }
        }
        int32 c2 = -1;
        v = 1000000000;
        for (int32 i = 0; i <= 256; ++i) {
            if (freq[i] && freq[i] <= v && i != c1) {
                v = freq[i];
                c2 = i;
            }
        }
        if (c2 < 0) {
            break;
        }
        freq[c1] += freq[c2];
        freq[c2] = 0;
        codesize[c1]++;
        while (others[c1] >= 0) {
            c1 = others[c1];
            codesize[c1]++;
        }
        others[c1] = c2;
        codesize[c2]++;
        while (others[c2] >= 0) {
            c2 = others[c2];
            codesize[c2]++;
        }
    }

    for (int32 i = 0; i <= 256; ++i) {
        if (codesize[i]) {
            if (codesize[i] > 32) {
                ErrExit(cinfo, JERR_HUFF_CLEN_OVERFLOW);
            }
            bits[codesize[i]]++;
        }
    }

    int32 i = 32;
    for (; i > 16; --i) {
        while (bits[i] > 0) {
            int32 j = i - 2;
            while (bits[j] == 0) {
                j--;
            }
            bits[i] -= 2;
            bits[i - 1]++;
            bits[j + 1] += 2;
            bits[j]--;
        }
    }
    while (bits[i] == 0) {
        i--;
    }
    bits[i]--;

    std::memcpy(htbl, bits, 17);
    int32 p = 0;
    for (int32 len = 1; len <= 32; ++len) {
        for (int32 j = 0; j <= 255; ++j) {
            if (codesize[j] == len) {
                htbl[0x11 + p] = static_cast<uint8>(j);
                p++;
            }
        }
    }
    htbl[0x111] = 0;
}
} // namespace

/**
 * Registra las rutinas internas de compresion de la libjpeg del EXE (lote 230A, sin validar).
 */
void LibJpegCompressEncode::InjectHooks() {
    RH_ScopedClass(LibJpegCompressEncode);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(ExeJpegStartCompress, "jpeg_start_compress", 0x5C26B0);               // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegWriteScanlines, "jpeg_write_scanlines", 0x5C2730);             // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitCoefController, "jinit_c_coef_controller", 0x5C30D0);         // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeGrayscaleConvert, "grayscale_convert", 0x5C35F0);                  // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitColorConverter, "jinit_color_converter", 0x5C3700);           // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitForwardDct, "jinit_forward_dct", 0x5C4380);                   // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegMakeCDerivedTbl, "jpeg_make_c_derived_tbl", 0x5C43F0);         // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegGenOptimalTable, "jpeg_gen_optimal_table", 0x5C4D50);          // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitHuffEncoder, "jinit_huff_encoder", 0x5C5210);                 // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitCompressMaster, "jinit_compress_master", 0x5C5260);           // lote 230A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitMainController, "jinit_c_main_controller", 0x5C5420);         // lote 230A (sin validar)
}
