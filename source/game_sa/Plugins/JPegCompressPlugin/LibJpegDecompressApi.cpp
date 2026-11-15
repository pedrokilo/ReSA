#include "StdInc.h"

#include "LibJpegDecompressApi.h"

// Lote 236A (sin validar): jdapimin de la libjpeg 6b del EXE (jpeg_CreateDecompress, jpeg_consume_input, jpeg_finish_decompress, jpeg_read_header y default_decompress_parms).
// Desplazamientos usados del jpeg_decompress_struct: +0x00 err, +0x04 mem, +0x08 progress, +0x0C client_data, +0x10 is_decompressor, +0x14 global_state, +0x18 src ({+0x08 init_source, +0x18 term_source}),
// +0x1C image_width, +0x24 num_components, +0x28 jpeg_color_space, +0x2C out_color_space, +0x30 scale_num, +0x34 scale_denom, +0x38 output_gamma (double), +0x40 buffered_image, +0x41 raw_data_out,
// +0x44 dct_method, +0x48 do_fancy_upsampling, +0x49 do_block_smoothing, +0x4A quantize_colors, +0x4C dither_mode, +0x50 two_pass_quantize, +0x54 desired_number_of_colors, +0x58..0x5A enable_*_quant,
// +0x60 output_height, +0x74 colormap, +0x78 output_scanline, +0x90 quant_tbl_ptrs[4], +0xA0 dc_huff_tbl_ptrs[4], +0xB0 ac_huff_tbl_ptrs[4], +0xC4 comp_info (0x54 bytes cada uno; component_id en +0),
// +0x100 saw_JFIF_marker, +0x108 saw_Adobe_marker, +0x109 Adobe_transform, +0x10C marker_list, +0x180 master (finish_output_pass en +4), +0x190 inputctl ({+0 consume_input, +4 reset_input_controller,
// +0x11 eoi_reached}). Tamano de la estructura: 0x1B0. jpeg_error_mgr: +0x00 error_exit, +0x04 emit_message, +0x14 msg_code, +0x18 msg_parm.i[0], +0x1C i[1], +0x20 i[2]. Todo lo que se llama es cdecl.
namespace {
template<typename T>
inline T& At(void* base, uint32 offset) {
    return *reinterpret_cast<T*>(static_cast<uint8*>(base) + offset);
}

// ERREXIT*: err->msg_code (+ parametros); (*err->error_exit)(cinfo). Como el EXE, `err` se relee en cada acceso.
inline void ErrExit(void* cinfo, int32 code) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

inline void ErrExit1(void* cinfo, int32 code, int32 parm) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<int32>(At<void*>(cinfo, 0x00), 0x18) = parm;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

inline void ErrExit2(void* cinfo, int32 code, int32 parm0, int32 parm1) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<int32>(At<void*>(cinfo, 0x00), 0x18) = parm0;
    At<int32>(At<void*>(cinfo, 0x00), 0x1C) = parm1;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

constexpr int32 JERR_BAD_LIB_VERSION = 0x0C;
constexpr int32 JERR_BAD_STATE = 0x14;
constexpr int32 JERR_BAD_STRUCT_SIZE = 0x15;
constexpr int32 JERR_NO_IMAGE = 0x33;
constexpr int32 JERR_TOO_LITTLE_DATA = 0x43;
constexpr int32 JTRC_UNKNOWN_IDS = 0x6F;
constexpr int32 JWRN_ADOBE_XFORM = 0x72;

constexpr int32 DSTATE_START = 0xC8;
constexpr int32 DSTATE_INHEADER = 0xC9;
constexpr int32 DSTATE_READY = 0xCA;
constexpr int32 DSTATE_SCANNING = 0xCD;
constexpr int32 DSTATE_RAW_OK = 0xCE;
constexpr int32 DSTATE_BUFPOST = 0xCF;
constexpr int32 DSTATE_STOPPING = 0xD2;

// Rutinas del EXE sin reconstruir en este lote, por direccion fija (todas cdecl con el cinfo): jinit_memory_mgr (0x5CF6D0), jinit_marker_reader (0x5CC7D0), jinit_input_controller (0x5CA840, enganchada
// en el lote 233A) y jpeg_abort (0x5C6B50).
inline void CallCinfo(uint32 address, void* cinfo) {
    reinterpret_cast<void(__cdecl*)(void*)>(address)(cinfo);
}

// WARNMS1: err->msg_code = JWRN_ADOBE_XFORM; err->msg_parm.i[0] = Adobe_transform (byte +0x109); (*err->emit_message)(cinfo, -1).
inline void WarnAdobeXform(void* cinfo) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = JWRN_ADOBE_XFORM;
    At<int32>(At<void*>(cinfo, 0x00), 0x18) = At<uint8>(cinfo, 0x109);
    At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x00), 0x04)(cinfo, -1);
}

// 0x5CFE20 (ESI = cinfo de entrada, devuelve EAX = 1 que nadie lee): default_decompress_parms de jdapimin. C++ no puede cargar ESI, asi que NO se engancha (saltada): se reproduce como funcion propia para
// 0x5CFFB0. Fija jpeg_color_space (+0x28) y out_color_space (+0x2C) segun num_components (+0x24) y los marcadores JFIF (+0x100) / Adobe (+0x108, +0x109), y despues los parametros de salida por defecto.
void DefaultDecompressParms(void* cinfo) {
    switch (At<int32>(cinfo, 0x24)) {
    case 1:
        At<int32>(cinfo, 0x28) = 1; // JCS_GRAYSCALE
        At<int32>(cinfo, 0x2C) = 1;
        break;
    case 3: {
        bool ycc = true; // JCS_YCbCr -> JCS_RGB salvo los casos que lo cambian
        if (!At<uint8>(cinfo, 0x100)) {
            if (At<uint8>(cinfo, 0x108)) {
                const uint8 transform = At<uint8>(cinfo, 0x109);
                if (transform == 0) {
                    At<int32>(cinfo, 0x28) = 2; // JCS_RGB
                    At<int32>(cinfo, 0x2C) = 2;
                    ycc = false;
                } else if (transform != 1) {
                    WarnAdobeXform(cinfo);
                }
            } else {
                uint8* const comp = At<uint8*>(cinfo, 0xC4);
                const int32 id0 = At<int32>(comp, 0x00);
                const int32 id1 = At<int32>(comp, 0x54);
                const int32 id2 = At<int32>(comp, 0xA8);
                if (id0 == 1 && id1 == 2 && id2 == 3) {
                    // YCbCr: el EXE escribe jpeg_color_space = id2 (3) y out_color_space = id1 (2), mismos valores que el caso comun
                } else if (id0 == 0x52 && id1 == 0x47 && id2 == 0x42) { // 'R', 'G', 'B'
                    At<int32>(cinfo, 0x28) = 2;
                    At<int32>(cinfo, 0x2C) = 2;
                    ycc = false;
                } else {
                    // TRACEMS3(cinfo, 1, JTRC_UNKNOWN_IDS, id0, id1, id2); err se relee entre accesos como en el EXE
                    At<int32>(At<void*>(cinfo, 0x00), 0x18) = id0;
                    At<int32>(At<void*>(cinfo, 0x00), 0x1C) = id1;
                    At<int32>(At<void*>(cinfo, 0x00), 0x20) = id2;
                    At<int32>(At<void*>(cinfo, 0x00), 0x14) = JTRC_UNKNOWN_IDS;
                    At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x00), 0x04)(cinfo, 1);
                }
            }
        }
        if (ycc) {
            At<int32>(cinfo, 0x28) = 3; // JCS_YCbCr
            At<int32>(cinfo, 0x2C) = 2; // JCS_RGB
        }
        break;
    }
    case 4:
        if (!At<uint8>(cinfo, 0x108) || At<uint8>(cinfo, 0x109) == 0) {
            At<int32>(cinfo, 0x28) = 4; // JCS_CMYK
            At<int32>(cinfo, 0x2C) = 4;
        } else {
            if (At<uint8>(cinfo, 0x109) != 2) {
                WarnAdobeXform(cinfo);
            }
            At<int32>(cinfo, 0x28) = 5; // JCS_YCCK
            At<int32>(cinfo, 0x2C) = 4; // JCS_CMYK
        }
        break;
    default:
        At<int32>(cinfo, 0x28) = 0; // JCS_UNKNOWN
        At<int32>(cinfo, 0x2C) = 0;
        break;
    }
    At<int32>(cinfo, 0x38) = 0; // output_gamma = 1.0 (doble 0x3FF0000000000000: parte baja a cero aqui, alta mas abajo)
    At<uint8>(cinfo, 0x40) = 0;
    At<uint8>(cinfo, 0x41) = 0;
    At<int32>(cinfo, 0x44) = 0;
    At<uint8>(cinfo, 0x4A) = 0;
    At<uint8>(cinfo, 0x50) = 0;
    At<int32>(cinfo, 0x74) = 0;
    At<uint8>(cinfo, 0x58) = 0;
    At<uint8>(cinfo, 0x59) = 0;
    At<uint8>(cinfo, 0x5A) = 0;
    At<int32>(cinfo, 0x30) = 1;
    At<int32>(cinfo, 0x34) = 1;
    At<int32>(cinfo, 0x3C) = 0x3FF00000;
    At<uint8>(cinfo, 0x48) = 1;
    At<uint8>(cinfo, 0x49) = 1;
    At<int32>(cinfo, 0x4C) = 2;
    At<int32>(cinfo, 0x54) = 0x100;
}

// 0x5CFD20 (cdecl, tres argumentos, 218 bytes; el listado decia FxEmitterBP_c): jpeg_CreateDecompress(cinfo, version, structsize) de jdapimin. mem (+4) = NULL; version != 0x3E -> ERREXIT2(0x0C, 0x3E, version) y
// SIGUE; structsize != 0x1B0 -> ERREXIT2(0x15, 0x1B0, structsize) y SIGUE; pone a cero los 0x1B0 bytes conservando err (+0) y client_data (+0x0C); is_decompressor (+0x10) = 1; jinit_memory_mgr (0x5CF6D0);
// progress (+8) = src (+0x18) = 0, los cuatro quant_tbl_ptrs (+0x90), dc (+0xA0) y ac (+0xB0) a cero, marker_list (+0x10C) = 0; jinit_marker_reader (0x5CC7D0); jinit_input_controller (0x5CA840);
// global_state (+0x14) = DSTATE_START.
void ExeJpegCreateDecompress(void* cinfo, int32 version, int32 structSize) {
    At<uint32>(cinfo, 0x04) = 0;
    if (version != 0x3E) {
        ErrExit2(cinfo, JERR_BAD_LIB_VERSION, 0x3E, version);
    }
    if (structSize != 0x1B0) {
        ErrExit2(cinfo, JERR_BAD_STRUCT_SIZE, 0x1B0, structSize);
    }
    void* const err = At<void*>(cinfo, 0x00);
    const uint32 clientData = At<uint32>(cinfo, 0x0C);
    memset(cinfo, 0, 0x1B0);
    At<void*>(cinfo, 0x00) = err;
    At<uint32>(cinfo, 0x0C) = clientData;
    At<uint8>(cinfo, 0x10) = 1;
    CallCinfo(0x5CF6D0, cinfo);
    At<uint32>(cinfo, 0x08) = 0;
    At<uint32>(cinfo, 0x18) = 0;
    for (uint32 i = 0; i < 4; ++i) {
        At<uint32>(cinfo, 0x90 + 4 * i) = 0;
    }
    for (uint32 i = 0; i < 4; ++i) {
        At<uint32>(cinfo, 0xA0 + 4 * i) = 0;
        At<uint32>(cinfo, 0xB0 + 4 * i) = 0;
    }
    At<uint32>(cinfo, 0x10C) = 0;
    CallCinfo(0x5CC7D0, cinfo);
    CallCinfo(0x5CA840, cinfo);
    At<int32>(cinfo, 0x14) = DSTATE_START;
}

// 0x5CFFB0 (cdecl, un argumento, 151 bytes; el listado decia FxEmitterBP_c): jpeg_consume_input(cinfo) de jdapimin -> int (0 = suspendido, 1 = REACHED_SOS, 2 = REACHED_EOI). Segun global_state (+0x14) - 0xC8:
// DSTATE_START: inputctl->reset_input_controller (+0x190 ranura 1), src->init_source (+0x18 ranura 2), global_state = DSTATE_INHEADER y sigue en el caso siguiente; DSTATE_INHEADER: retcode =
// inputctl->consume_input (ranura 0); con 1 -> default_decompress_parms (0x5CFE20, ESI = cinfo, reproducida arriba) y global_state = DSTATE_READY, y devuelve retcode en cualquier caso; DSTATE_READY: devuelve 1;
// 0xCB..0xD0 y 0xD2: devuelve consume_input; 0xD1 y fuera de rango: ERREXIT1(0x14, global_state) y SIGUE devolviendo 0 (tabla de saltos 0x5D0048 + tabla de indices 0x5D005C).
int32 ExeJpegConsumeInput(void* cinfo) {
    const uint32 idx = static_cast<uint32>(At<int32>(cinfo, 0x14) - DSTATE_START);
    if (idx > 0x0A || idx == 9) {
        ErrExit1(cinfo, JERR_BAD_STATE, At<int32>(cinfo, 0x14));
        return 0;
    }
    if (idx == 2) {
        return 1;
    }
    if (idx > 2) {
        return At<int32(__cdecl*)(void*)>(At<void*>(cinfo, 0x190), 0x00)(cinfo);
    }
    if (idx == 0) {
        At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x190), 0x04)(cinfo);
        At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x18), 0x08)(cinfo);
        At<int32>(cinfo, 0x14) = DSTATE_INHEADER;
    }
    const int32 retcode = At<int32(__cdecl*)(void*)>(At<void*>(cinfo, 0x190), 0x00)(cinfo);
    if (retcode == 1) {
        DefaultDecompressParms(cinfo);
        At<int32>(cinfo, 0x14) = DSTATE_READY;
    }
    return retcode;
}

// 0x5D0110 (cdecl, un argumento, 187 bytes; el listado decia FxEmitterBP_c): jpeg_finish_decompress(cinfo) de jdapimin -> bool (AL). Con global_state (+0x14) SCANNING o RAW_OK y buffered_image (+0x40) a cero:
// output_scanline (+0x78) < output_height (+0x60) (sin signo) -> ERREXIT(0x43) y SIGUE; master->finish_output_pass (+0x180 ranura 1); global_state = DSTATE_STOPPING. Con BUFPOST: DSTATE_STOPPING. Con otro valor
// distinto de STOPPING: ERREXIT1(0x14, global_state) y SIGUE. Despues, mientras inputctl->eoi_reached (+0x190 byte +0x11) sea 0: consume_input (ranura 0); 0 -> devuelve 0 sin mas. Al final src->term_source
// (+0x18 ranura 6), jpeg_abort (0x5C6B50) y devuelve 1.
uint8 ExeJpegFinishDecompress(void* cinfo) {
    const int32 state = At<int32>(cinfo, 0x14);
    if ((state == DSTATE_SCANNING || state == DSTATE_RAW_OK) && !At<uint8>(cinfo, 0x40)) {
        if (At<uint32>(cinfo, 0x78) < At<uint32>(cinfo, 0x60)) {
            ErrExit(cinfo, JERR_TOO_LITTLE_DATA);
        }
        At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x180), 0x04)(cinfo);
        At<int32>(cinfo, 0x14) = DSTATE_STOPPING;
    } else if (state == DSTATE_BUFPOST) {
        At<int32>(cinfo, 0x14) = DSTATE_STOPPING;
    } else if (state != DSTATE_STOPPING) {
        ErrExit1(cinfo, JERR_BAD_STATE, At<int32>(cinfo, 0x14));
    }
    while (!At<uint8>(At<void*>(cinfo, 0x190), 0x11)) {
        if (!At<int32(__cdecl*)(void*)>(At<void*>(cinfo, 0x190), 0x00)(cinfo)) {
            return 0;
        }
    }
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x18), 0x18)(cinfo);
    CallCinfo(0x5C6B50, cinfo);
    return 1;
}

// 0x5D01D0 (cdecl, dos argumentos, 112 bytes; el listado decia FxEmitterBP_c): jpeg_read_header(cinfo, require_image BYTE) de jdapimin -> int (EAX). global_state (+0x14) distinto de START e INHEADER ->
// ERREXIT1(0x14, global_state) y SIGUE. retcode = jpeg_consume_input (0x5CFFB0). 1 (REACHED_SOS) -> devuelve 1; 2 (REACHED_EOI) -> con require_image, ERREXIT(0x33) y SIGUE; jpeg_abort (0x5C6B50) y devuelve 2;
// otro valor (0, suspendido) -> lo devuelve tal cual.
int32 ExeJpegReadHeader(void* cinfo, uint8 requireImage) {
    const int32 state = At<int32>(cinfo, 0x14);
    if (state != DSTATE_START && state != DSTATE_INHEADER) {
        ErrExit1(cinfo, JERR_BAD_STATE, At<int32>(cinfo, 0x14));
    }
    const int32 retcode = ExeJpegConsumeInput(cinfo);
    if (retcode == 1) {
        return 1;
    }
    if (retcode != 2) {
        return retcode;
    }
    if (requireImage) {
        ErrExit(cinfo, JERR_NO_IMAGE);
    }
    CallCinfo(0x5C6B50, cinfo);
    return 2;
}
} // namespace

/**
 * Registra las rutinas de jdapimin de la libjpeg del EXE (lote 236A, sin validar).
 */
void LibJpegDecompressApi::InjectHooks() {
    RH_ScopedClass(LibJpegDecompressApi);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(ExeJpegCreateDecompress, "jpeg_CreateDecompress", 0x5CFD20);    // lote 236A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegConsumeInput, "jpeg_consume_input", 0x5CFFB0);           // lote 236A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegFinishDecompress, "jpeg_finish_decompress", 0x5D0110);   // lote 236A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegReadHeader, "jpeg_read_header", 0x5D01D0);               // lote 236A (sin validar)
}
