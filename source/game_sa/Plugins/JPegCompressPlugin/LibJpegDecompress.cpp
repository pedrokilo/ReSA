/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: LibJpegDecompress
 * Reconstrucción: COMPLETA — 22/22 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/22 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "LibJpegDecompress.h"

// Lote 233A (sin validar): rutinas de la libjpeg 6b del EXE (descompresion: jdhuff, jdinput, jdmainct). Trabajan sobre el jpeg_decompress_struct del EXE; se accede a sus campos por desplazamiento (los de libjpeg-turbo de la DLL no coinciden).
// Desplazamientos usados del jpeg_decompress_struct: +0x00 err, +0x04 mem, +0x18 src ({+0 next_input_byte, +4 bytes_in_buffer, +0x0C fill_input_buffer}), +0x1C image_width, +0x20 image_height, +0x24 num_components,
// +0x7C input_scan_number, +0x84 output_scan_number, +0x8C coef_bits, +0x90 quant_tbl_ptrs[4], +0xA0 dc_huff_tbl_ptrs[4], +0xB0 ac_huff_tbl_ptrs[4], +0xC0 data_precision, +0xC4 comp_info (0x54 bytes cada uno), +0xC8 progressive_mode,
// +0x110 max_h_samp_factor, +0x114 max_v_samp_factor, +0x118 min_DCT_scaled_size, +0x11C total_iMCU_rows, +0x124 comps_in_scan, +0x128 cur_comp_info[4], +0x138 MCUs_per_row, +0x13C MCU_rows_in_scan, +0x140 blocks_in_MCU,
// +0x144 MCU_membership[10], +0x17C unread_marker, +0x184 main, +0x188 coef, +0x18C post, +0x190 inputctl, +0x194 marker, +0x198 entropy, +0x1A0 upsample.
// jpeg_component_info: +0x08 h_samp_factor, +0x0C v_samp_factor, +0x10 quant_tbl_no, +0x1C width_in_blocks, +0x20 height_in_blocks, +0x24 DCT_scaled_size, +0x28 downsampled_width, +0x2C downsampled_height, +0x30 component_needed,
// +0x34 MCU_width, +0x38 MCU_height, +0x3C MCU_blocks, +0x40 MCU_sample_width, +0x44 last_col_width, +0x48 last_row_height, +0x4C quant_table.
// jpeg_error_mgr: +0x00 error_exit, +0x04 emit_message, +0x10 reset_error_mgr, +0x14 msg_code, +0x18 msg_parm.i[0], +0x1C msg_parm.i[1]. Todas las rutinas del EXE que se llaman son cdecl.
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

// ERREXIT2: msg_parm.i[0] y msg_parm.i[1].
inline void ErrExit2(void* cinfo, int32 code, int32 parm0, int32 parm1) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<int32>(At<void*>(cinfo, 0x00), 0x18) = parm0;
    At<int32>(At<void*>(cinfo, 0x00), 0x1C) = parm1;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
}

// WARNMS: err->msg_code = code; (*err->emit_message)(cinfo, -1).
inline void WarnMs(void* cinfo, int32 code) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x00), 0x04)(cinfo, -1);
}

inline void* AllocSmall(void* cinfo, int32 pool, uint32 size) {
    return At<void*(__cdecl*)(void*, int32, uint32)>(At<void*>(cinfo, 0x04), 0x00)(cinfo, pool, size);
}

inline long JDivRoundUp(long a, long b) { // jdiv_round_up (0x5CF890)
    return reinterpret_cast<long(__cdecl*)(long, long)>(0x5CF890)(a, b);
}

constexpr int32 JERR_BAD_BUFFER_MODE = 4;
constexpr int32 JERR_BAD_HUFF_TABLE = 8;
constexpr int32 JERR_BAD_MCU_SIZE = 0x0D;
constexpr int32 JERR_BAD_PRECISION = 0x0F;
constexpr int32 JERR_BAD_SAMPLING = 0x12;
constexpr int32 JERR_COMPONENT_COUNT = 0x1A;
constexpr int32 JERR_IMAGE_TOO_BIG = 0x29;
constexpr int32 JERR_NO_HUFF_TABLE = 0x32;
constexpr int32 JERR_NO_QUANT_TABLE = 0x34;
constexpr int32 JERR_NOTIMPL = 0x2F;
constexpr int32 JERR_EOI_EXPECTED = 0x23;
constexpr int32 JERR_SOF_NO_SOS = 0x3B;
constexpr int32 JWRN_HIT_MARKER = 0x75;
constexpr int32 JWRN_HUFF_BAD_CODE = 0x76;

// 0x5CA110 (start_pass_huff_decoder), 0x5C9CE0 (decode_mcu), 0x5CA720 (consume_markers), 0x5CA7E0 (reset_input_controller), 0x5CA6E0 (start_input_pass), 0x5CA820 (finish_input_pass), 0x5CADC0 (start_pass_main): se guardan por direccion.

// 0x5CA6E0 helpers de registro: initial_setup (0x5CA2A0, ESI = cinfo), per_scan_setup (0x5CA490, ESI = cinfo), latch_quant_tables (0x5CA630, EBX = cinfo) y alloc_funny_pointers (0x5CA890, ESI = cinfo) reciben el
// cinfo en un registro que C++ no puede cargar; no se enganchan (saltadas) y aqui se reproducen como funciones propias, SIN hook, para que sus llamadores enganchados no dependan de ellas por direccion.

// 0x5CA2A0 (ESI = cinfo): initial_setup de jdinput. Comprobaciones (cada ERREXIT SIGUE tras volver) y calculo de tamanos por componente.
void InitialSetup(void* cinfo) {
    if (At<int32>(cinfo, 0x20) > 0xFFDC || At<int32>(cinfo, 0x1C) > 0xFFDC) {
        ErrExit1(cinfo, JERR_IMAGE_TOO_BIG, 0xFFDC);
    }
    if (At<int32>(cinfo, 0xC0) != 8) {
        ErrExit1(cinfo, JERR_BAD_PRECISION, At<int32>(cinfo, 0xC0));
    }
    if (At<int32>(cinfo, 0x24) > 4) {
        ErrExit2(cinfo, JERR_COMPONENT_COUNT, At<int32>(cinfo, 0x24), 4);
    }
    At<int32>(cinfo, 0x110) = 1;
    At<int32>(cinfo, 0x114) = 1;
    uint8* compptr = At<uint8*>(cinfo, 0xC4);
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x24); ++ci, compptr += 0x54) {
        const int32 h = At<int32>(compptr, 0x08);
        const int32 v = At<int32>(compptr, 0x0C);
        if (h <= 0 || h > 4 || v <= 0 || v > 4) {
            ErrExit(cinfo, JERR_BAD_SAMPLING);
        }
        const int32 maxH = At<int32>(cinfo, 0x110);
        At<int32>(cinfo, 0x110) = (maxH > h) ? maxH : h;
        const int32 maxV = At<int32>(cinfo, 0x114);
        At<int32>(cinfo, 0x114) = (maxV > v) ? maxV : v;
    }
    At<int32>(cinfo, 0x118) = 8;
    compptr = At<uint8*>(cinfo, 0xC4);
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x24); ++ci, compptr += 0x54) {
        At<int32>(compptr, 0x24) = 8;
        At<int32>(compptr, 0x1C) = static_cast<int32>(JDivRoundUp(At<int32>(compptr, 0x08) * At<int32>(cinfo, 0x1C), At<int32>(cinfo, 0x110) << 3));
        At<int32>(compptr, 0x20) = static_cast<int32>(JDivRoundUp(At<int32>(compptr, 0x0C) * At<int32>(cinfo, 0x20), At<int32>(cinfo, 0x114) << 3));
        // El EXE usa aqui image_width * h / max_h (sin el factor DCT_scaled_size) para la anchura y la altura muestreadas.
        At<int32>(compptr, 0x28) = static_cast<int32>(JDivRoundUp(At<int32>(compptr, 0x08) * At<int32>(cinfo, 0x1C), At<int32>(cinfo, 0x110)));
        At<int32>(compptr, 0x2C) = static_cast<int32>(JDivRoundUp(At<int32>(compptr, 0x0C) * At<int32>(cinfo, 0x20), At<int32>(cinfo, 0x114)));
        At<uint8>(compptr, 0x30) = 1;
        At<uint32>(compptr, 0x4C) = 0;
    }
    At<int32>(cinfo, 0x11C) = static_cast<int32>(JDivRoundUp(At<int32>(cinfo, 0x20), At<int32>(cinfo, 0x114) << 3));
    void* const inputctl = At<void*>(cinfo, 0x190);
    if (At<int32>(cinfo, 0x124) < At<int32>(cinfo, 0x24) || At<uint8>(cinfo, 0xC8)) {
        At<uint8>(inputctl, 0x10) = 1;
    } else {
        At<uint8>(inputctl, 0x10) = 0;
    }
}

// 0x5CA490 (ESI = cinfo): per_scan_setup de jdinput. Division sin signo para los restos; el ERREXIT de comps_in_scan y el de BAD_MCU_SIZE siguen tras volver.
void PerScanSetup(void* cinfo) {
    if (At<int32>(cinfo, 0x124) == 1) {
        uint8* const compptr = At<uint8*>(cinfo, 0x128);
        At<uint32>(cinfo, 0x138) = At<uint32>(compptr, 0x1C);
        At<uint32>(cinfo, 0x13C) = At<uint32>(compptr, 0x20);
        const uint32 vSamp = At<uint32>(compptr, 0x0C);
        At<uint32>(compptr, 0x40) = At<uint32>(compptr, 0x24);
        uint32 rem = At<uint32>(compptr, 0x20) % vSamp;
        At<int32>(compptr, 0x34) = 1;
        At<int32>(compptr, 0x38) = 1;
        At<int32>(compptr, 0x3C) = 1;
        At<int32>(compptr, 0x44) = 1;
        if (rem == 0) {
            rem = vSamp;
        }
        At<uint32>(compptr, 0x48) = rem;
        At<int32>(cinfo, 0x140) = 1;
        At<int32>(cinfo, 0x144) = 0;
        return;
    }
    const int32 compsInScan = At<int32>(cinfo, 0x124);
    if (compsInScan <= 0 || compsInScan > 4) {
        ErrExit2(cinfo, JERR_COMPONENT_COUNT, At<int32>(cinfo, 0x124), 4);
    }
    At<int32>(cinfo, 0x138) = static_cast<int32>(JDivRoundUp(At<int32>(cinfo, 0x1C), At<int32>(cinfo, 0x110) << 3));
    At<int32>(cinfo, 0x13C) = static_cast<int32>(JDivRoundUp(At<int32>(cinfo, 0x20), At<int32>(cinfo, 0x114) << 3));
    At<int32>(cinfo, 0x140) = 0;
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x124); ++ci) {
        uint8* const compptr = At<uint8*>(cinfo, 0x128 + 4 * ci);
        const uint32 hSamp = At<uint32>(compptr, 0x08);
        At<uint32>(compptr, 0x40) = At<uint32>(compptr, 0x24) * hSamp;
        const uint32 vSamp = At<uint32>(compptr, 0x0C);
        uint32 rem = At<uint32>(compptr, 0x1C) % hSamp;
        const int32 mcuBlocks = static_cast<int32>(vSamp * hSamp);
        At<uint32>(compptr, 0x34) = hSamp;
        At<uint32>(compptr, 0x38) = vSamp;
        At<int32>(compptr, 0x3C) = mcuBlocks;
        if (rem == 0) {
            rem = hSamp;
        }
        At<uint32>(compptr, 0x44) = rem;
        rem = At<uint32>(compptr, 0x20) % vSamp;
        if (rem == 0) {
            rem = vSamp;
        }
        At<uint32>(compptr, 0x48) = rem;
        if (At<int32>(cinfo, 0x140) + mcuBlocks > 10) {
            ErrExit(cinfo, JERR_BAD_MCU_SIZE);
        }
        for (int32 n = mcuBlocks; n > 0; --n) {
            const int32 idx = At<int32>(cinfo, 0x140);
            At<int32>(cinfo, 0x144 + 4 * idx) = ci;
            At<int32>(cinfo, 0x140) = idx + 1;
        }
    }
}

// 0x5CA630 (EBX = cinfo): latch_quant_tables de jdinput. Por cada componente de la pasada sin tabla guardada (+0x4C): numero de tabla (+0x10) fuera de 0..3 o tabla nula -> ERREXIT1(0x34) y SIGUE; reserva 0x82 bytes
// (JPOOL_IMAGE) y copia la tabla de quant_tbl_ptrs[qtblno] (se relee el puntero tras la reserva).
void LatchQuantTables(void* cinfo) {
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x124); ++ci) {
        uint8* const compptr = At<uint8*>(cinfo, 0x128 + 4 * ci);
        if (At<uint32>(compptr, 0x4C) != 0) {
            continue;
        }
        const int32 qtblno = At<int32>(compptr, 0x10);
        if (qtblno < 0 || qtblno >= 4 || At<uint32>(cinfo, 0x90 + 4 * qtblno) == 0) {
            ErrExit1(cinfo, JERR_NO_QUANT_TABLE, qtblno);
        }
        void* const qtbl = AllocSmall(cinfo, 1, 0x82);
        std::memcpy(qtbl, At<void*>(cinfo, 0x90 + 4 * static_cast<uint32>(qtblno)), 0x82);
        At<void*>(compptr, 0x4C) = qtbl;
    }
}

// 0x5CA890 (ESI = cinfo): alloc_funny_pointers de jdmainct. xbuffer[0] y xbuffer[1] (main +0x20/+0x24) = un bloque de num_components*8 bytes partido en dos; por componente reserva 2*rgroup*(M+4) punteros y
// deja xbuffer[0][ci] = bloque + rgroup, xbuffer[1][ci] = bloque + rgroup + rgroup*(M+4) (en entradas de 4 bytes).
void AllocFunnyPointers(void* cinfo) {
    const int32 M = At<int32>(cinfo, 0x118);
    uint8* const main = At<uint8*>(cinfo, 0x184);
    uint8* const xbuffers = static_cast<uint8*>(AllocSmall(cinfo, 1, static_cast<uint32>(At<int32>(cinfo, 0x24)) << 3));
    At<uint8*>(main, 0x20) = xbuffers;
    At<uint8*>(main, 0x24) = xbuffers + At<int32>(cinfo, 0x24) * 4;
    uint8* compptr = At<uint8*>(cinfo, 0xC4);
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x24); ++ci, compptr += 0x54) {
        const int32 rgroup = (At<int32>(compptr, 0x0C) * At<int32>(compptr, 0x24)) / At<int32>(cinfo, 0x118);
        const int32 span = (M + 4) * rgroup;
        uint32* xbuf = static_cast<uint32*>(AllocSmall(cinfo, 1, static_cast<uint32>(span) * 8));
        xbuf += rgroup;
        At<uint32**>(main, 0x20)[ci] = xbuf;
        xbuf += span;
        At<uint32**>(main, 0x24)[ci] = xbuf;
    }
}

// 0x5C9790 (cdecl, cuatro argumentos, 704 bytes; el listado decia FxEmitterBP_c): jpeg_make_d_derived_tbl(cinfo, isDC BYTE, tblno, pdtbl) de jdhuff. Tabla Huffman (htbl) = dc_huff_tbl_ptrs[tblno] (+0xA0) si isDC,
// si no ac_huff_tbl_ptrs[tblno] (+0xB0); tblno fuera de 0..3 o tabla nula -> ERREXIT1(0x32, tblno) y SIGUE. Reserva 0x590 (JPOOL_IMAGE) si *pdtbl es nulo; d_derived_tbl: maxcode[17] +0x00 (maxcode[17] = 0xFFFFF +0x44),
// valoffset[17] +0x48, pub +0x8C, look_nbits[256] +0x90 (int), look_sym[256] +0x490 (byte). htbl: bits[1..16] en htbl+l, huffval en htbl+0x11. Las incoherencias -> ERREXIT(8) y SIGUE. Los arrays locales se
// sobredimensionan (el EXE desborda su pila con tablas corruptas).
// Particularidad: la validacion de simbolos DC (0..15) recorre numsymbols = suma de bits[1..16] (la p de la primera pasada), no la de la tabla de lectura anticipada.
void ExeJpegMakeDDerivedTbl(void* cinfo, int32 isDC, int32 tblno, uint8** pdtbl) {
    if (tblno < 0 || tblno >= 4) {
        ErrExit1(cinfo, JERR_NO_HUFF_TABLE, tblno);
    }
    uint8* const htbl = At<uint8*>(cinfo, (static_cast<uint8>(isDC) ? 0xA0u : 0xB0u) + static_cast<uint32>(tblno) * 4u);
    if (htbl == nullptr) {
        ErrExit1(cinfo, JERR_NO_HUFF_TABLE, tblno);
    }
    if (*pdtbl == nullptr) {
        *pdtbl = static_cast<uint8*>(AllocSmall(cinfo, 1, 0x590));
    }
    uint8* const dtbl = *pdtbl;
    At<uint8*>(dtbl, 0x8C) = htbl;

    uint8 huffsize[0x1200];
    int32 huffcode[0x1200];

    // Figure C.1: huffsize
    int32 p = 0;
    for (int32 l = 1; l <= 16; ++l) {
        const int32 i = htbl[l];
        if (i < 0 || p + i > 256) {
            ErrExit(cinfo, JERR_BAD_HUFF_TABLE);
        }
        if (i != 0) {
            std::memset(huffsize + p, l, i);
            p += i;
        }
    }
    huffsize[p] = 0;
    const int32 numsymbols = p;

    // Figure C.2: huffcode
    int32 code = 0;
    int32 si = huffsize[0];
    p = 0;
    if (huffsize[0] != 0) {
        for (;;) {
            while (huffsize[p] == si) {
                huffcode[p] = code;
                ++p;
                ++code;
            }
            if (code >= (1 << (si & 31))) {
                ErrExit(cinfo, JERR_BAD_HUFF_TABLE);
            }
            code <<= 1;
            ++si;
            if (huffsize[p] == 0) {
                break;
            }
        }
    }

    // Figure F.15
    p = 0;
    for (int32 l = 1; l <= 16; ++l) {
        if (htbl[l] != 0) {
            At<int32>(dtbl, 0x48 + 4 * l) = p - huffcode[p];
            p += htbl[l];
            At<int32>(dtbl, 4 * l) = huffcode[p - 1];
        } else {
            At<int32>(dtbl, 4 * l) = -1;
        }
    }
    At<int32>(dtbl, 0x44) = 0xFFFFF;
    std::memset(dtbl + 0x90, 0, 0x400);

    // Lectura anticipada de 8 bits
    p = 0;
    for (int32 l = 1; l <= 8; ++l) {
        const int32 count = htbl[l];
        for (int32 i = 1; i <= count; ++i, ++p) {
            int32 lookbits = huffcode[p] << (8 - l);
            for (int32 ctr = 1 << (8 - l); ctr > 0; --ctr) {
                At<int32>(dtbl, 0x90 + 4 * static_cast<uint32>(lookbits)) = l;
                At<uint8>(dtbl, 0x490 + static_cast<uint32>(lookbits)) = htbl[0x11 + p];
                ++lookbits;
            }
        }
    }

    if (static_cast<uint8>(isDC)) {
        for (int32 i = 0; i < numsymbols; ++i) {
            const int32 sym = htbl[0x11 + i];
            if (sym < 0 || sym > 15) {
                ErrExit(cinfo, JERR_BAD_HUFF_TABLE);
            }
        }
    }
}

// 0x5C9A60 (cdecl, cuatro argumentos, 286 bytes; el listado decia FxEmitterBP_c): jpeg_fill_bit_buffer(state, get_buffer, bits_left, nbits) de jdhuff -> boolean (AL). state: +0 next_input_byte, +4 bytes_in_buffer,
// +8 get_buffer, +0x0C bits_left, +0x10 cinfo. Si no hay marcador pendiente (cinfo +0x17C == 0) y bits_left < 25 rellena byte a byte (src->fill_input_buffer, +0x18/+0x0C; falso -> devuelve 0 SIN escribir el estado);
// 0xFF 0x00 -> 0xFF, 0xFF 0xnn -> unread_marker = nn y sale al ramal "sin mas bytes". En ese ramal (o si ya habia marcador), si nbits > bits_left: una sola vez (entropy +0x198, byte +8) WARNMS(0x75) y
// get_buffer <<= (25 - bits_left) (contador de 5 bits como el x86) y bits_left = 25. Escribe el estado y devuelve 1.
bool ExeJpegFillBitBuffer(uint8* state, int32 getBuffer, int32 bitsLeft, int32 nbits) {
    void* const cinfo = At<void*>(state, 0x10);
    uint8* next = At<uint8*>(state, 0x00);
    int32 bytesLeft = At<int32>(state, 0x04);
    uint32 buf = static_cast<uint32>(getBuffer);
    bool noMoreBytes = At<int32>(cinfo, 0x17C) != 0;

    const auto fillInput = [&]() -> bool {
        void* const src = At<void*>(cinfo, 0x18);
        if (!At<uint8(__cdecl*)(void*)>(src, 0x0C)(cinfo)) {
            return false;
        }
        next = At<uint8*>(src, 0x00);
        bytesLeft = At<int32>(src, 0x04);
        return true;
    };

    if (!noMoreBytes && bitsLeft < 25) {
        for (;;) {
            if (bytesLeft == 0) {
                if (!fillInput()) {
                    return false;
                }
            }
            uint32 c = *next;
            --bytesLeft;
            ++next;
            if (c == 0xFF) {
                for (;;) {
                    if (bytesLeft == 0) {
                        if (!fillInput()) {
                            return false;
                        }
                    }
                    c = *next;
                    --bytesLeft;
                    ++next;
                    if (c != 0xFF) {
                        break;
                    }
                }
                if (c != 0) {
                    At<int32>(cinfo, 0x17C) = static_cast<int32>(c);
                    noMoreBytes = true;
                    break;
                }
                c = 0xFF;
            }
            buf = (buf << 8) | c;
            bitsLeft += 8;
            if (bitsLeft >= 25) {
                break;
            }
        }
    }

    if (noMoreBytes && nbits > bitsLeft) {
        if (!At<uint8>(At<void*>(cinfo, 0x198), 0x08)) {
            WarnMs(cinfo, JWRN_HIT_MARKER);
            At<uint8>(At<void*>(cinfo, 0x198), 0x08) = 1;
        }
        buf <<= ((25 - bitsLeft) & 31);
        bitsLeft = 25;
    }

    At<uint8*>(state, 0x00) = next;
    At<int32>(state, 0x04) = bytesLeft;
    At<uint32>(state, 0x08) = buf;
    At<int32>(state, 0x0C) = bitsLeft;
    return true;
}

// 0x5C9B80 (cdecl, cinco argumentos, 223 bytes; el listado decia FxEmitterBP_c): jpeg_huff_decode(state, get_buffer, bits_left, htbl, min_bits) de jdhuff -> int. Rellena con jpeg_fill_bit_buffer cuando faltan bits
// (falso -> -1 sin escribir el estado); la comparacion con maxcode[l] (+4*l) es con signo y los desplazamientos son aritmeticos con contador de 5 bits. Al salir escribe get_buffer/bits_left en el estado; l > 16 ->
// WARNMS(0x76) con el cinfo del estado (+0x10) y devuelve 0; si no, devuelve huffval[code + valoffset[l]] = byte en (pub + valoffset[l]) + code + 0x11 (pub +0x8C, valoffset +0x48 + 4*l).
int32 ExeJpegHuffDecode(uint8* state, int32 getBuffer, int32 bitsLeft, uint8* htbl, int32 minBits) {
    int32 l = minBits;
    int32 buf = getBuffer;
    int32 bits = bitsLeft;
    if (bits < l) {
        if (!ExeJpegFillBitBuffer(state, buf, bits, l)) {
            return -1;
        }
        buf = At<int32>(state, 0x08);
        bits = At<int32>(state, 0x0C);
    }
    bits -= l;
    int32 code = (buf >> (bits & 31)) & ((1 << (l & 31)) - 1);
    while (code > At<int32>(htbl, 4 * static_cast<uint32>(l))) {
        code <<= 1;
        if (bits < 1) {
            if (!ExeJpegFillBitBuffer(state, buf, bits, 1)) {
                return -1;
            }
            buf = At<int32>(state, 0x08);
            bits = At<int32>(state, 0x0C);
        }
        --bits;
        code |= (buf >> (bits & 31)) & 1;
        ++l;
    }
    At<int32>(state, 0x08) = buf;
    At<int32>(state, 0x0C) = bits;
    if (l > 16) {
        void* const cinfo = At<void*>(state, 0x10);
        At<int32>(At<void*>(cinfo, 0x00), 0x14) = JWRN_HUFF_BAD_CODE;
        void* const cinfo2 = At<void*>(state, 0x10);
        At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo2, 0x00), 0x04)(cinfo2, -1);
        return 0;
    }
    const uint32 addr = At<uint32>(htbl, 0x48 + 4 * static_cast<uint32>(l)) + At<uint32>(htbl, 0x8C) + static_cast<uint32>(code) + 0x11;
    return *reinterpret_cast<uint8*>(addr);
}

// 0x5CA260 (cdecl, un argumento, 63 bytes; el listado decia FxEmitterBP_c): jinit_huff_decoder(cinfo) de jdhuff. Reserva 0xAC bytes (JPOOL_IMAGE) en cinfo+0x198; ranura 0 = start_pass 0x5CA110, ranura 1 =
// decode_mcu 0x5C9CE0; pone a cero las ocho tablas derivadas (AC en +0x38.., DC en +0x28.., en ese orden por vuelta).
void ExeJinitHuffDecoder(void* cinfo) {
    uint8* const entropy = static_cast<uint8*>(AllocSmall(cinfo, 1, 0xAC));
    At<void*>(cinfo, 0x198) = entropy;
    At<uint32>(entropy, 0x00) = 0x5CA110;
    At<uint32>(entropy, 0x04) = 0x5C9CE0;
    for (uint32 i = 0; i < 4; ++i) {
        At<uint32>(entropy, 0x38 + 4 * i) = 0;
        At<uint32>(entropy, 0x28 + 4 * i) = 0;
    }
}

// 0x5CA6E0 (cdecl, un argumento, 59 bytes; el listado decia FxEmitterBP_c): start_input_pass(cinfo) de jdinput. per_scan_setup y latch_quant_tables (ambas con el cinfo en registro, reproducidas arriba), luego
// entropy->start_pass (cinfo+0x198, ranura 0), coef->start_input_pass (cinfo+0x188, ranura 0) y inputctl->consume_input (inputctl+0) = coef->consume_data (coef+4).
void ExeStartInputPass(void* cinfo) {
    PerScanSetup(cinfo);
    LatchQuantTables(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x198), 0x00)(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x188), 0x00)(cinfo);
    At<uint32>(At<void*>(cinfo, 0x190), 0x00) = At<uint32>(At<void*>(cinfo, 0x188), 0x04);
}

// 0x5CA720 (cdecl, un argumento, 180 bytes; el listado decia FxEmitterBP_c): consume_markers(cinfo) de jdinput -> int. inputctl = cinfo+0x190 (+0x10 has_multiple_scans, +0x11 eoi_reached, +0x14 inheaders). eoi_reached -> 2.
// val = marker->read_markers (cinfo+0x194, ranura 1). val 1 (SOS): con inheaders -> initial_setup y inheaders = 0 (en ese orden); si no, sin has_multiple_scans -> ERREXIT(0x23) (SIGUE) y start_input_pass.
// val 2 (EOI): eoi_reached = 1 ANTES de mirar inheaders; con inheaders y marker->saw_SOF (+0x0D) -> ERREXIT(0x3B); sin inheaders, output_scan_number (+0x84) se recorta a input_scan_number (+0x7C) con signo. Devuelve val.
int32 ExeConsumeMarkers(void* cinfo) {
    uint8* const inputctl = At<uint8*>(cinfo, 0x190);
    if (At<uint8>(inputctl, 0x11)) {
        return 2;
    }
    const int32 val = At<int32(__cdecl*)(void*)>(At<void*>(cinfo, 0x194), 0x04)(cinfo);
    if (val == 1) {
        if (At<uint8>(inputctl, 0x14)) {
            InitialSetup(cinfo);
            At<uint8>(inputctl, 0x14) = 0;
        } else {
            if (!At<uint8>(inputctl, 0x10)) {
                ErrExit(cinfo, JERR_EOI_EXPECTED);
            }
            ExeStartInputPass(cinfo);
        }
    } else if (val == 2) {
        const uint8 inHeaders = At<uint8>(inputctl, 0x14);
        At<uint8>(inputctl, 0x11) = 1;
        if (inHeaders) {
            if (At<uint8>(At<void*>(cinfo, 0x194), 0x0D)) {
                ErrExit(cinfo, JERR_SOF_NO_SOS);
            }
        } else {
            const int32 inputScan = At<int32>(cinfo, 0x7C);
            if (At<int32>(cinfo, 0x84) > inputScan) {
                At<int32>(cinfo, 0x84) = inputScan;
            }
        }
    }
    return val;
}

// 0x5CA7E0 (cdecl, un argumento, 59 bytes; el listado decia FxEmitterBP_c): reset_input_controller(cinfo) de jdinput. inputctl (+0x190): consume_input (+0) = consume_markers 0x5CA720, has_multiple_scans (+0x10) = 0,
// eoi_reached (+0x11) = 0, inheaders (+0x14) = 1; luego err->reset_error_mgr (+0x10), marker->reset_marker_reader (cinfo+0x194, ranura 0) y coef_bits (+0x8C) = 0.
void ExeResetInputController(void* cinfo) {
    uint8* const inputctl = At<uint8*>(cinfo, 0x190);
    At<uint32>(inputctl, 0x00) = 0x5CA720;
    At<uint8>(inputctl, 0x10) = 0;
    At<uint8>(inputctl, 0x11) = 0;
    At<uint8>(inputctl, 0x14) = 1;
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x10)(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x194), 0x00)(cinfo);
    At<uint32>(cinfo, 0x8C) = 0;
}

// 0x5CA840 (cdecl, un argumento, 65 bytes; el listado decia FxEmitterBP_c): jinit_input_controller(cinfo) de jdinput. Reserva 0x18 bytes en JPOOL_PERMANENT (pool 0), los guarda en cinfo+0x190 y rellena: ranuras
// 0..3 = consume_markers 0x5CA720, reset_input_controller 0x5CA7E0, start_input_pass 0x5CA6E0, finish_input_pass 0x5CA820; +0x10 = 0, +0x11 = 0, +0x14 = 1.
void ExeJinitInputController(void* cinfo) {
    uint8* const inputctl = static_cast<uint8*>(AllocSmall(cinfo, 0, 0x18));
    At<void*>(cinfo, 0x190) = inputctl;
    At<uint32>(inputctl, 0x00) = 0x5CA720;
    At<uint32>(inputctl, 0x04) = 0x5CA7E0;
    At<uint32>(inputctl, 0x08) = 0x5CA6E0;
    At<uint32>(inputctl, 0x0C) = 0x5CA820;
    At<uint8>(inputctl, 0x10) = 0;
    At<uint8>(inputctl, 0x11) = 0;
    At<uint8>(inputctl, 0x14) = 1;
}

// 0x5CA940 (cdecl, un argumento, 332 bytes; el listado decia FxEmitterBP_c): make_funny_pointers(cinfo) de jdmainct. main = cinfo+0x184 (buffer[ci] en +8+4*ci, xbuffer[0]/[1] en +0x20/+0x24). Por componente:
// rgroup = v_samp * DCT_scaled_size / min_DCT_scaled_size (con signo); primero copia buffer[0..rgroup*(M+2)) a xbuf1 y a xbuf0 (en ese orden por elemento), luego intercambia en xbuf1 los dos ultimos grupos
// (xbuf1[rgroup*(M-2)+i] = buf[rgroup*M+i] y xbuf1[rgroup*M+i] = buf[rgroup*(M-2)+i], i < 2*rgroup) y por ultimo xbuf0[i-rgroup] = xbuf0[0] (i < rgroup, releyendo xbuf0[0]).
void ExeMakeFunnyPointers(void* cinfo) {
    const int32 M = At<int32>(cinfo, 0x118);
    uint8* const main = At<uint8*>(cinfo, 0x184);
    uint8* compptr = At<uint8*>(cinfo, 0xC4);
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x24); ++ci, compptr += 0x54) {
        const int32 rgroup = (At<int32>(compptr, 0x0C) * At<int32>(compptr, 0x24)) / At<int32>(cinfo, 0x118);
        uint32* const xbuf0 = At<uint32**>(main, 0x20)[ci];
        uint32* const xbuf1 = At<uint32**>(main, 0x24)[ci];
        uint32* const buf = At<uint32*>(main, 0x08 + 4 * static_cast<uint32>(ci));
        for (int32 i = 0; i < (M + 2) * rgroup; ++i) {
            xbuf1[i] = buf[i];
            xbuf0[i] = buf[i];
        }
        for (int32 i = 0; i < rgroup * 2; ++i) {
            xbuf1[rgroup * (M - 2) + i] = buf[rgroup * M + i];
            xbuf1[rgroup * M + i] = buf[rgroup * (M - 2) + i];
        }
        for (int32 i = 0; i < rgroup; ++i) {
            xbuf0[i - rgroup] = xbuf0[0];
        }
    }
}

// 0x5CAA90 (cdecl, un argumento, 210 bytes; el listado decia FxEmitterBP_c): set_wraparound_pointers(cinfo) de jdmainct. Por componente (rgroup como arriba), para i < rgroup y en este orden:
// xbuf0[i-rgroup] = xbuf0[rgroup*(M+1)+i]; xbuf1[i-rgroup] = xbuf1[rgroup*(M+1)+i]; xbuf0[rgroup*(M+2)+i] = xbuf0[i]; xbuf1[rgroup*(M+2)+i] = xbuf1[i].
void ExeSetWraparoundPointers(void* cinfo) {
    const int32 M = At<int32>(cinfo, 0x118);
    uint8* const main = At<uint8*>(cinfo, 0x184);
    uint8* compptr = At<uint8*>(cinfo, 0xC4);
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x24); ++ci, compptr += 0x54) {
        const int32 rgroup = (At<int32>(compptr, 0x0C) * At<int32>(compptr, 0x24)) / At<int32>(cinfo, 0x118);
        uint32* const xbuf0 = At<uint32**>(main, 0x20)[ci];
        uint32* const xbuf1 = At<uint32**>(main, 0x24)[ci];
        for (int32 i = 0; i < rgroup; ++i) {
            xbuf0[i - rgroup] = xbuf0[rgroup * (M + 1) + i];
            xbuf1[i - rgroup] = xbuf1[rgroup * (M + 1) + i];
            xbuf0[rgroup * (M + 2) + i] = xbuf0[i];
            xbuf1[rgroup * (M + 2) + i] = xbuf1[i];
        }
    }
}

// 0x5CAB70 (cdecl, un argumento, 157 bytes; el listado decia FxEmitterBP_c): set_bottom_pointers(cinfo) de jdmainct. Por componente: iMCUheight = v_samp * DCT_scaled_size; rgroup = iMCUheight / min_DCT_scaled_size
// (con signo); rows_left = downsampled_height (+0x2C) % iMCUheight (SIN signo), 0 -> iMCUheight; con ci == 0, main->rowgroups_avail (+0x30) = (rows_left-1) / rgroup + 1 (con signo); xbuf = xbuffer[whichptr (+0x28)][ci]
// y xbuf[rows_left+i] = xbuf[rows_left-1] para i < rgroup*2.
void ExeSetBottomPointers(void* cinfo) {
    uint8* const main = At<uint8*>(cinfo, 0x184);
    uint8* compptr = At<uint8*>(cinfo, 0xC4);
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x24); ++ci, compptr += 0x54) {
        const int32 iMCUheight = At<int32>(compptr, 0x0C) * At<int32>(compptr, 0x24);
        const int32 rgroup = iMCUheight / At<int32>(cinfo, 0x118);
        int32 rowsLeft = static_cast<int32>(At<uint32>(compptr, 0x2C) % static_cast<uint32>(iMCUheight));
        if (rowsLeft == 0) {
            rowsLeft = iMCUheight;
        }
        if (ci == 0) {
            At<int32>(main, 0x30) = (rowsLeft - 1) / rgroup + 1;
        }
        uint32* const xbuf = At<uint32**>(main, 0x20 + 4 * static_cast<uint32>(At<int32>(main, 0x28)))[ci];
        for (int32 i = 0; i < rgroup * 2; ++i) {
            xbuf[rowsLeft + i] = xbuf[rowsLeft - 1];
        }
    }
}

// 0x5CAE30 (cdecl, dos argumentos, 207 bytes; el listado decia FxEmitterBP_c): jinit_d_main_controller(cinfo, need_full_buffer BYTE) de jdmainct. Reserva 0x38 (JPOOL_IMAGE) en cinfo+0x184 y start_pass (0x5CADC0) en
// la ranura 0; need_full_buffer -> ERREXIT(4) y SIGUE. Si upsample (+0x1A0) pide filas de contexto (byte +8): min_DCT_scaled_size < 2 -> ERREXIT(0x2F) y SIGUE, alloc_funny_pointers (con el cinfo en ESI, reproducida
// arriba) y ngroups = M + 2; si no, ngroups = M. Por componente (num_components releido): buffer[ci] (+8+4*ci) = alloc_sarray(cinfo, 1, width_in_blocks * DCT_scaled_size, v_samp * DCT_scaled_size / M * ngroups) (mem+8).
void ExeJinitDMainController(void* cinfo, int32 needFullBuffer) {
    uint8* const main = static_cast<uint8*>(AllocSmall(cinfo, 1, 0x38));
    At<void*>(cinfo, 0x184) = main;
    At<uint32>(main, 0x00) = 0x5CADC0;
    if (static_cast<uint8>(needFullBuffer)) {
        ErrExit(cinfo, JERR_BAD_BUFFER_MODE);
    }
    int32 ngroups;
    if (At<uint8>(At<void*>(cinfo, 0x1A0), 0x08)) {
        if (At<int32>(cinfo, 0x118) < 2) {
            ErrExit(cinfo, JERR_NOTIMPL);
        }
        AllocFunnyPointers(cinfo);
        ngroups = At<int32>(cinfo, 0x118) + 2;
    } else {
        ngroups = At<int32>(cinfo, 0x118);
    }
    uint8* compptr = At<uint8*>(cinfo, 0xC4);
    for (int32 ci = 0; ci < At<int32>(cinfo, 0x24); ++ci, compptr += 0x54) {
        const int32 dct = At<int32>(compptr, 0x24);
        const int32 rgroup = (At<int32>(compptr, 0x0C) * dct) / At<int32>(cinfo, 0x118);
        void* const mem = At<void*>(cinfo, 0x04);
        const int32 height = rgroup * ngroups;
        const int32 width = At<int32>(compptr, 0x1C) * dct;
        At<void*>(main, 0x08 + 4 * static_cast<uint32>(ci)) = At<void*(__cdecl*)(void*, int32, uint32, uint32)>(mem, 0x08)(cinfo, 1, static_cast<uint32>(width), static_cast<uint32>(height));
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Lote 234A (sin validar): jdmarker (lectura de marcadores) y jdmaster (seleccion de modulos de descompresion) de la misma libjpeg 6b. Mas desplazamientos del jpeg_decompress_struct del EXE (boolean = BYTE):
// +0x08 progress ({+0x0C completed_passes, +0x10 total_passes}), +0x14 global_state, +0x28 jpeg_color_space, +0x2C out_color_space, +0x40 buffered_image, +0x41 raw_data_out, +0x4A quantize_colors, +0x50 two_pass_quantize,
// +0x58 enable_1pass_quant, +0x59 enable_external_quant, +0x5A enable_2pass_quant, +0x5C output_width, +0x60 output_height, +0x64 out_color_components, +0x68 output_components, +0x6C rec_outbuf_height, +0x74 colormap,
// +0xC8 progressive_mode, +0xC9 arith_code, +0xFC restart_interval, +0x120 sample_range_limit, +0x17C unread_marker, +0x180 master, +0x184 main, +0x188 coef, +0x18C post, +0x190 inputctl, +0x194 marker, +0x198 entropy,
// +0x19C idct, +0x1A0 upsample, +0x1A4 cconvert, +0x1A8 cquantize. jpeg_source_mgr: +0 next_input_byte, +4 bytes_in_buffer, +0x0C fill_input_buffer, +0x10 skip_input_data, +0x14 resync_to_restart.
// marker (0xA8): +0 reset_marker_reader, +4 read_markers, +8 read_restart_marker, +0x0C saw_SOI, +0x0D saw_SOF, +0x10 next_restart_num, +0x14 discarded_bytes, +0x18 process_COM, +0x1C process_APPn[16], +0x5C length_limit_COM,
// +0x60 length_limit_APPn[16], +0xA0. master (0x1C): +0 prepare_for_output_pass, +4 finish_output_pass, +8 is_dummy_pass, +0x0C pass_number, +0x10 using_merged_upsample, +0x14 quantizer_1pass, +0x18 quantizer_2pass.
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

constexpr int32 JERR_BAD_LENGTH = 0x0B;
constexpr int32 JERR_DHT_INDEX = 0x1E;
constexpr int32 JERR_NO_SOI = 0x35;
constexpr int32 JERR_BAD_STATE = 0x14;
constexpr int32 JERR_ARITH_NOTIMPL = 0x01;
constexpr int32 JERR_MODE_CHANGE = 0x2E;
constexpr int32 JERR_NOT_COMPILED = 0x30;
constexpr int32 JTRC_DHT = 0x50;
constexpr int32 JTRC_DRI = 0x52;
constexpr int32 JTRC_HUFFBITS = 0x56;
constexpr int32 JTRC_SKIP_VARIABLE = 0x5B;
constexpr int32 JTRC_RST = 0x62;
constexpr int32 JWRN_EXTRANEOUS_DATA = 0x74;

// TRACEMS*: err->msg_code = code; err->msg_parm.i[k] = parm[k]; (*err->emit_message)(cinfo, level).
inline void TraceMs(void* cinfo, int32 code, int32 level, std::initializer_list<int32> parms) {
    At<int32>(At<void*>(cinfo, 0x00), 0x14) = code;
    uint32 k = 0;
    for (const int32 parm : parms) {
        At<int32>(At<void*>(cinfo, 0x00), 0x18 + 4 * k) = parm;
        ++k;
    }
    At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x00), 0x04)(cinfo, level);
}

// Variables locales de jdmarker (INPUT_VARS / INPUT_BYTE / INPUT_SYNC): next_input_byte y bytes_in_buffer viven en registros y solo se escriben en la fuente con Sync(). Si fill_input_buffer devuelve falso
// la funcion sale con FALSE SIN sincronizar (como el EXE).
struct JpegInput {
    void* cinfo;
    uint8* src;
    const uint8* next;
    uint32 bytes;

    explicit JpegInput(void* c)
        : cinfo(c)
        , src(At<uint8*>(c, 0x18)) {
        next = At<const uint8*>(src, 0x00);
        bytes = At<uint32>(src, 0x04);
    }

    bool Fill() {
        if (!At<uint8(__cdecl*)(void*)>(src, 0x0C)(cinfo)) {
            return false;
        }
        next = At<const uint8*>(src, 0x00);
        bytes = At<uint32>(src, 0x04);
        return true;
    }

    bool Byte(uint32& value) {
        if (bytes == 0 && !Fill()) {
            return false;
        }
        value = *next;
        --bytes;
        ++next;
        return true;
    }

    void Sync() {
        At<const uint8*>(src, 0x00) = next;
        At<uint32>(src, 0x04) = bytes;
    }
};

// 0x5CB5B0 (cdecl, un argumento, 781 bytes; el listado decia FxEmitterBP_c): get_dht(cinfo) de jdmarker -> boolean. length = 2 bytes - 2; mientras length > 16 (con signo): index, bits[1..16] (bits[0] = 0), TRACEMS1(0x50, index) nivel 1,
// length -= 17, dos TRACEMS8(0x56) nivel 2 (bits[1..8], bits[9..16]), count > 256 o count > length (con signo) -> ERREXIT(8) y SIGUE; huffval[count]; length -= count; index & 0x10 -> AC (cinfo+0xB0) con index - 0x10, si no DC (+0xA0);
// indice fuera de 0..3 -> ERREXIT1(0x1E, index) y SIGUE; tabla nula -> jpeg_alloc_huff_table (0x5C6BD0); copia 17 bytes de bits y 256 de huffval (el EXE copia los 256 aunque count sea menor: los sobrantes son de la pila).
// Al final length != 0 -> ERREXIT(0x0B) y SIGUE; escribe next/bytes en la fuente.
bool ExeGetDht(void* cinfo) {
    JpegInput in(cinfo);
    uint32 hi;
    uint32 lo;
    if (!in.Byte(hi) || !in.Byte(lo)) {
        return false;
    }
    int32 length = static_cast<int32>(hi << 8) + static_cast<int32>(lo) - 2;
    while (length > 16) {
        uint32 index;
        if (!in.Byte(index)) {
            return false;
        }
        TraceMs(cinfo, JTRC_DHT, 1, {static_cast<int32>(index)});
        uint8 bits[17];
        bits[0] = 0;
        int32 count = 0;
        for (int32 i = 1; i <= 16; ++i) {
            uint32 v;
            if (!in.Byte(v)) {
                return false;
            }
            bits[i] = static_cast<uint8>(v);
            count += static_cast<int32>(v);
        }
        length -= 17;
        TraceMs(cinfo, JTRC_HUFFBITS, 2, {bits[1], bits[2], bits[3], bits[4], bits[5], bits[6], bits[7], bits[8]});
        TraceMs(cinfo, JTRC_HUFFBITS, 2, {bits[9], bits[10], bits[11], bits[12], bits[13], bits[14], bits[15], bits[16]});
        if (count > 256 || count > length) {
            ErrExit(cinfo, JERR_BAD_HUFF_TABLE);
        }
        // El EXE usa 256 bytes de pila (y desborda si count > 256 tras el error); aqui el tope es el maximo posible (16 * 255).
        uint8 huffval[4096];
        for (int32 i = 0; i < count; ++i) {
            uint32 v;
            if (!in.Byte(v)) {
                return false;
            }
            huffval[i] = static_cast<uint8>(v);
        }
        length -= count;
        int32 tbl = static_cast<int32>(index);
        uint8** slot;
        if (index & 0x10) {
            tbl -= 0x10;
            slot = reinterpret_cast<uint8**>(static_cast<uint8*>(cinfo) + 0xB0 + 4 * static_cast<int64>(tbl));
        } else {
            slot = reinterpret_cast<uint8**>(static_cast<uint8*>(cinfo) + 0xA0 + 4 * static_cast<int64>(tbl));
        }
        if (tbl < 0 || tbl >= 4) {
            ErrExit1(cinfo, JERR_DHT_INDEX, tbl);
        }
        if (*slot == nullptr) {
            *slot = reinterpret_cast<uint8*(__cdecl*)(void*)>(0x5C6BD0)(cinfo); // jpeg_alloc_huff_table
        }
        uint8* const htbl = *slot;
        std::memcpy(htbl, bits, 17);
        std::memcpy(htbl + 0x11, huffval, 256);
    }
    if (length != 0) {
        ErrExit(cinfo, JERR_BAD_LENGTH);
    }
    in.Sync();
    return true;
}

// 0x5CBB20 (cdecl, un argumento, 223 bytes; el listado decia FxEmitterBP_c): get_dri(cinfo) de jdmarker -> boolean. length (2 bytes, SIN restar 2) != 4 -> ERREXIT(0x0B) y SIGUE; tmp = 2 bytes; TRACEMS1(0x52, tmp) nivel 1;
// cinfo->restart_interval (+0xFC) = tmp; escribe next/bytes en la fuente.
bool ExeGetDri(void* cinfo) {
    JpegInput in(cinfo);
    uint32 hi;
    uint32 lo;
    if (!in.Byte(hi) || !in.Byte(lo)) {
        return false;
    }
    if (hi * 256 + lo != 4) {
        ErrExit(cinfo, JERR_BAD_LENGTH);
    }
    uint32 tmpHi;
    uint32 tmpLo;
    if (!in.Byte(tmpHi) || !in.Byte(tmpLo)) {
        return false;
    }
    const uint32 tmp = tmpHi * 256 + tmpLo;
    TraceMs(cinfo, JTRC_DRI, 1, {static_cast<int32>(tmp)});
    At<uint32>(cinfo, 0xFC) = tmp;
    in.Sync();
    return true;
}

// 0x5CC050 (cdecl, un argumento, 157 bytes; el listado decia FxEmitterBP_c): skip_variable(cinfo) de jdmarker -> boolean (process_COM y process_APPn por defecto). length = 2 bytes - 2 (con signo);
// TRACEMS2(0x5B, unread_marker, length) nivel 1; consume el byte bajo de la longitud y sincroniza la fuente; length > 0 -> src->skip_input_data(cinfo, length) (+0x10).
bool ExeSkipVariable(void* cinfo) {
    JpegInput in(cinfo);
    uint32 hi;
    uint32 lo;
    if (!in.Byte(hi) || !in.Byte(lo)) {
        return false;
    }
    const int32 length = static_cast<int32>(hi << 8) + static_cast<int32>(lo) - 2;
    TraceMs(cinfo, JTRC_SKIP_VARIABLE, 1, {At<int32>(cinfo, 0x17C), length});
    in.Sync();
    if (length > 0) {
        At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x18), 0x10)(cinfo, length);
    }
    return true;
}

// 0x5CC0F0 (cdecl, un argumento, 256 bytes; el listado decia FxEmitterBP_c): next_marker(cinfo) de jdmarker -> boolean. Descarta bytes hasta 0xFF (marker->discarded_bytes, +0x14, ++ con sincronizacion por byte), salta los 0xFF
// de relleno; un 0x00 detras suma 2 a discarded_bytes, sincroniza y reinicia. Con un codigo distinto de cero: discarded_bytes != 0 -> WARNMS2(0x74, discarded_bytes, c) con nivel -1 y se pone a cero; unread_marker (+0x17C) = c;
// sincroniza la fuente. Falso de fill_input_buffer -> FALSE sin sincronizar.
bool ExeNextMarker(void* cinfo) {
    JpegInput in(cinfo);
    for (;;) {
        uint32 c;
        if (!in.Byte(c)) {
            return false;
        }
        while (c != 0xFF) {
            At<int32>(At<void*>(cinfo, 0x194), 0x14) += 1;
            in.Sync();
            if (!in.Byte(c)) {
                return false;
            }
        }
        do {
            if (!in.Byte(c)) {
                return false;
            }
        } while (c == 0xFF);
        if (c != 0) {
            if (At<int32>(At<void*>(cinfo, 0x194), 0x14) != 0) {
                TraceMs(cinfo, JWRN_EXTRANEOUS_DATA, -1, {At<int32>(At<void*>(cinfo, 0x194), 0x14), static_cast<int32>(c)});
                At<int32>(At<void*>(cinfo, 0x194), 0x14) = 0;
            }
            At<int32>(cinfo, 0x17C) = static_cast<int32>(c);
            in.Sync();
            return true;
        }
        At<int32>(At<void*>(cinfo, 0x194), 0x14) += 2;
        in.Sync();
    }
}

// 0x5CC1F0 (cdecl, un argumento, 153 bytes; el listado decia FxEmitterBP_c): first_marker(cinfo) de jdmarker -> boolean. Lee dos bytes (c, c2); c != 0xFF o c2 != 0xD8 -> ERREXIT2(0x35, c, c2) y SIGUE; unread_marker (+0x17C) = c2;
// escribe next/bytes en la fuente.
bool ExeFirstMarker(void* cinfo) {
    JpegInput in(cinfo);
    uint32 c;
    uint32 c2;
    if (!in.Byte(c) || !in.Byte(c2)) {
        return false;
    }
    if (c != 0xFF || c2 != 0xD8) {
        ErrExit2(cinfo, JERR_NO_SOI, static_cast<int32>(c), static_cast<int32>(c2));
    }
    At<int32>(cinfo, 0x17C) = static_cast<int32>(c2);
    in.Sync();
    return true;
}

// 0x5CC610 (cdecl, un argumento, 136 bytes; el listado decia FxEmitterBP_c): read_restart_marker(cinfo) de jdmarker -> boolean. Sin marcador pendiente llama a next_marker (falso -> 0). marker = cinfo+0x194, next_restart_num (+0x10).
// unread_marker == 0xD0 + next_restart_num -> TRACEMS1(0x62, next_restart_num) nivel 3 y unread_marker = 0; si no src->resync_to_restart(cinfo, next_restart_num) (+0x14; falso -> 0). Luego next_restart_num = (n + 1) & 7; devuelve 1.
bool ExeReadRestartMarker(void* cinfo) {
    if (At<int32>(cinfo, 0x17C) == 0) {
        if (!ExeNextMarker(cinfo)) {
            return false;
        }
    }
    const int32 nextRestart = At<int32>(At<void*>(cinfo, 0x194), 0x10);
    if (At<int32>(cinfo, 0x17C) == nextRestart + 0xD0) {
        TraceMs(cinfo, JTRC_RST, 3, {At<int32>(At<void*>(cinfo, 0x194), 0x10)});
        At<int32>(cinfo, 0x17C) = 0;
    } else if (!At<uint8(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x18), 0x14)(cinfo, nextRestart)) {
        return false;
    }
    uint8* const marker = At<uint8*>(cinfo, 0x194);
    At<int32>(marker, 0x10) = (At<int32>(marker, 0x10) + 1) & 7;
    return true;
}

// 0x5CC7D0 (cdecl, un argumento, 132 bytes; el listado decia FxEmitterBP_c): jinit_marker_reader(cinfo) de jdmarker. alloc_small(pool 0 = permanente, 0xA8) -> cinfo+0x194; ranuras: +0 reset_marker_reader 0x5CC7A0,
// +4 read_markers 0x5CC290, +8 read_restart_marker 0x5CC610, process_COM (+0x18) = 0x5CC050; length_limit_COM (+0x5C) = 0; por vuelta (16): process_APPn[i] (+0x1C + 4i) = 0x5CC050 y length_limit_APPn[i] (+0x60 + 4i) = 0;
// despues process_APPn[0] (+0x1C) y [14] (+0x54) = 0x5CBF00 (get_interesting_appn). Por ultimo cinfo+0xC4 (comp_info), +0x7C (input_scan_number) y +0x17C (unread_marker) = 0, marker +0x0C y +0x0D (bytes) = 0, +0x14 = 0 y +0xA0 = 0.
void ExeJinitMarkerReader(void* cinfo) {
    uint8* const marker = static_cast<uint8*>(AllocSmall(cinfo, 0, 0xA8));
    At<void*>(cinfo, 0x194) = marker;
    At<uint32>(marker, 0x00) = 0x5CC7A0;
    At<uint32>(marker, 0x04) = 0x5CC290;
    At<uint32>(marker, 0x08) = 0x5CC610;
    At<uint32>(marker, 0x18) = 0x5CC050;
    At<uint32>(marker, 0x5C) = 0;
    for (uint32 i = 0; i < 16; ++i) {
        At<uint32>(marker, 0x1C + 4 * i) = 0x5CC050;
        At<uint32>(marker, 0x60 + 4 * i) = 0;
    }
    At<uint32>(marker, 0x1C) = 0x5CBF00;
    At<uint32>(marker, 0x54) = 0x5CBF00;
    uint8* const marker2 = At<uint8*>(cinfo, 0x194);
    At<uint32>(cinfo, 0xC4) = 0;
    At<uint32>(cinfo, 0x7C) = 0;
    At<uint32>(cinfo, 0x17C) = 0;
    At<uint8>(marker2, 0x0C) = 0;
    At<uint8>(marker2, 0x0D) = 0;
    At<uint32>(marker2, 0x14) = 0;
    At<uint32>(marker2, 0xA0) = 0;
}

// 0x5CC8C0 (cdecl, un argumento, 121 bytes; el listado decia FxEmitterBP_c): jpeg_calc_output_dimensions(cinfo) de jdmaster (version sin escalado DCT). global_state (+0x14) != 0xCA (DSTATE_READY) -> ERREXIT1(0x14, global_state) y SIGUE.
// output_width (+0x5C) = image_width, output_height (+0x60) = image_height; out_color_components (+0x64) segun out_color_space (+0x2C) - 1 (SIN signo) con la tabla del EXE 0x5CC93C: 0 -> 1, 1 -> 4, 2 -> 3, 3 -> 4, 4 -> 4, mayor que 4 ->
// num_components (+0x24); rec_outbuf_height (+0x6C) = 1; output_components (+0x68) = quantize_colors (+0x4A) ? 1 : out_color_components.
void ExeJpegCalcOutputDimensions(void* cinfo) {
    if (At<int32>(cinfo, 0x14) != 0xCA) {
        ErrExit1(cinfo, JERR_BAD_STATE, At<int32>(cinfo, 0x14));
    }
    const int32 width = At<int32>(cinfo, 0x1C);
    const uint32 space = static_cast<uint32>(At<int32>(cinfo, 0x2C)) - 1;
    const int32 height = At<int32>(cinfo, 0x20);
    At<int32>(cinfo, 0x5C) = width;
    At<int32>(cinfo, 0x60) = height;
    switch (space) {
    case 0:
        At<int32>(cinfo, 0x64) = 1;
        break;
    case 2:
        At<int32>(cinfo, 0x64) = 3;
        break;
    case 1:
    case 3:
    case 4:
        At<int32>(cinfo, 0x64) = 4;
        break;
    default:
        At<int32>(cinfo, 0x64) = At<int32>(cinfo, 0x24);
        break;
    }
    At<int32>(cinfo, 0x6C) = 1;
    int32 components = 1;
    if (!At<uint8>(cinfo, 0x4A)) {
        components = At<int32>(cinfo, 0x64);
    }
    At<int32>(cinfo, 0x68) = components;
}

// 0x5CC950 (EAX = cinfo, 119 bytes, saltada como hook por ABI): prepare_range_limit_table de jdmaster. Reproducida SIN hook para master_selection. alloc_small(pool 1, 0x580); tabla = bloque + 0x100 -> cinfo+0x120;
// bloque[0..0xFF] = 0; tabla[i] = i (0..255); desde tabla + 0x80: 384 bytes a 0xFF, 384 bytes a 0 (tabla + 0x200) y 128 bytes copiados de tabla (a tabla + 0x380).
void PrepareRangeLimitTable(void* cinfo) {
    uint8* const block = static_cast<uint8*>(AllocSmall(cinfo, 1, 0x580));
    uint8* const table = block + 0x100;
    At<void*>(cinfo, 0x120) = table;
    std::memset(block, 0, 0x100);
    for (int32 i = 0; i <= 0xFF; ++i) {
        table[i] = static_cast<uint8>(i);
    }
    uint8* const post = table + 0x80;
    std::memset(post + 0x80, 0xFF, 0x180);
    std::memset(post + 0x200, 0, 0x180);
    std::memcpy(post + 0x380, At<uint8*>(cinfo, 0x120), 0x80);
}

// 0x5CC9D0 (ESI = cinfo, 373 bytes, saltada como hook por ABI): master_selection de jdmaster. Reproducida SIN hook para jinit_master_decompress. master = cinfo+0x180.
// jpeg_calc_output_dimensions, prepare_range_limit_table, master +0x0C = 0, +0x10 = 0 (sin fusion de muestreo), +0x14 = 0, +0x18 = 0. Sin quantize_colors o sin buffered_image: enable_1pass/external/2pass (+0x58/+0x59/+0x5A) = 0.
// Con quantize_colors: raw_data_out -> ERREXIT(0x2F); out_color_components != 3 -> 1pass = 1, external = 0, 2pass = 0, colormap = 0; si no colormap != 0 -> external = 1; si no two_pass_quantize -> 2pass = 1; si no 1pass = 1;
// luego 1pass -> ERREXIT(0x30) y (2pass o external) -> ERREXIT(0x30) (todos SIGUEN). Sin raw_data_out: using_merged_upsample -> ERREXIT(0x30), si no jinit_color_deconverter (0x5C9350) y jinit_upsampler (0x5CD350);
// jinit_d_post_controller (0x5CCDF0, cinfo, enable_2pass_quant). jinit_inverse_dct (0x5C9710); arith_code (+0xC9) -> ERREXIT(1); si no progressive_mode (+0xC8) -> ERREXIT(0x30); si no jinit_huff_decoder.
// use_c_buffer = inputctl->has_multiple_scans (+0x10) o buffered_image; jinit_d_coef_controller (0x5C8E20, cinfo, use_c_buffer); sin raw_data_out jinit_d_main_controller (0x5CAE30, cinfo, 0);
// mem->realize_virt_arrays (+0x18); inputctl->start_input_pass (+8).
void MasterSelection(void* cinfo) {
    uint8* const master = At<uint8*>(cinfo, 0x180);
    ExeJpegCalcOutputDimensions(cinfo);
    PrepareRangeLimitTable(cinfo);
    At<uint32>(master, 0x0C) = 0;
    At<uint8>(master, 0x10) = 0;
    At<uint32>(master, 0x14) = 0;
    At<uint32>(master, 0x18) = 0;
    const uint8 quantize = At<uint8>(cinfo, 0x4A);
    if (!quantize || !At<uint8>(cinfo, 0x40)) {
        At<uint8>(cinfo, 0x58) = 0;
        At<uint8>(cinfo, 0x59) = 0;
        At<uint8>(cinfo, 0x5A) = 0;
    }
    if (quantize) {
        if (At<uint8>(cinfo, 0x41)) {
            ErrExit(cinfo, JERR_NOTIMPL);
        }
        if (At<int32>(cinfo, 0x64) != 3) {
            At<uint8>(cinfo, 0x59) = 0;
            At<uint8>(cinfo, 0x5A) = 0;
            At<uint32>(cinfo, 0x74) = 0;
            At<uint8>(cinfo, 0x58) = 1;
        } else if (At<uint32>(cinfo, 0x74) != 0) {
            At<uint8>(cinfo, 0x59) = 1;
        } else if (At<uint8>(cinfo, 0x50)) {
            At<uint8>(cinfo, 0x5A) = 1;
        } else {
            At<uint8>(cinfo, 0x58) = 1;
        }
        if (At<uint8>(cinfo, 0x58)) {
            ErrExit(cinfo, JERR_NOT_COMPILED);
        }
        if (At<uint8>(cinfo, 0x5A) || At<uint8>(cinfo, 0x59)) {
            ErrExit(cinfo, JERR_NOT_COMPILED);
        }
    }
    if (!At<uint8>(cinfo, 0x41)) {
        if (At<uint8>(master, 0x10)) {
            ErrExit(cinfo, JERR_NOT_COMPILED);
        } else {
            reinterpret_cast<void(__cdecl*)(void*)>(0x5C9350)(cinfo); // jinit_color_deconverter
            reinterpret_cast<void(__cdecl*)(void*)>(0x5CD350)(cinfo); // jinit_upsampler
        }
        reinterpret_cast<void(__cdecl*)(void*, uint32)>(0x5CCDF0)(cinfo, At<uint8>(cinfo, 0x5A)); // jinit_d_post_controller
    }
    reinterpret_cast<void(__cdecl*)(void*)>(0x5C9710)(cinfo); // jinit_inverse_dct
    if (At<uint8>(cinfo, 0xC9)) {
        ErrExit(cinfo, JERR_ARITH_NOTIMPL);
    } else if (At<uint8>(cinfo, 0xC8)) {
        ErrExit(cinfo, JERR_NOT_COMPILED);
    } else {
        ExeJinitHuffDecoder(cinfo);
    }
    const uint8 useCBuffer = (At<uint8>(At<void*>(cinfo, 0x190), 0x10) || At<uint8>(cinfo, 0x40)) ? 1 : 0;
    reinterpret_cast<void(__cdecl*)(void*, uint32)>(0x5C8E20)(cinfo, useCBuffer); // jinit_d_coef_controller
    if (!At<uint8>(cinfo, 0x41)) {
        ExeJinitDMainController(cinfo, 0);
    }
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x04), 0x18)(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x190), 0x08)(cinfo);
}

// 0x5CCB50 (cdecl, un argumento, 318 bytes; el listado decia FxEmitterBP_c): prepare_for_output_pass(cinfo) de jdmaster. master = cinfo+0x180. is_dummy_pass (+8) -> ERREXIT(0x30) y salta a la parte de progreso. Si no: con
// quantize_colors y sin colormap: two_pass_quantize y enable_2pass_quant -> cquantize (+0x1A8) = master->quantizer_2pass (+0x18) e is_dummy_pass = 1; si no enable_1pass_quant -> cquantize = master->quantizer_1pass (+0x14);
// si no ERREXIT(0x2E) y SIGUE. Luego idct->start_pass (+0x19C), coef->start_output_pass (+0x188 ranura 2); sin raw_data_out: cconvert->start_pass (+0x1A4) si no using_merged_upsample, upsample->start_pass (+0x1A0), con
// quantize_colors cquantize->start_pass(cinfo, is_dummy_pass), post->start_pass(cinfo, is_dummy_pass ? 3 : 0) (+0x18C), main->start_pass(cinfo, 0) (+0x184). Progreso (cinfo+8): completed_passes (+0x0C) = pass_number (master +0x0C);
// total_passes (+0x10) = pass_number + (is_dummy_pass ? 2 : 1); con buffered_image y sin eoi_reached (inputctl +0x11) suma (enable_2pass_quant ? 2 : 1).
void ExePrepareForOutputPass(void* cinfo) {
    uint8* const master = At<uint8*>(cinfo, 0x180);
    if (At<uint8>(master, 0x08)) {
        ErrExit(cinfo, JERR_NOT_COMPILED);
    } else {
        if (At<uint8>(cinfo, 0x4A) && At<uint32>(cinfo, 0x74) == 0) {
            if (At<uint8>(cinfo, 0x50) && At<uint8>(cinfo, 0x5A)) {
                At<uint32>(cinfo, 0x1A8) = At<uint32>(master, 0x18);
                At<uint8>(master, 0x08) = 1;
            } else if (At<uint8>(cinfo, 0x58)) {
                At<uint32>(cinfo, 0x1A8) = At<uint32>(master, 0x14);
            } else {
                ErrExit(cinfo, JERR_MODE_CHANGE);
            }
        }
        At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x19C), 0x00)(cinfo);
        At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x188), 0x08)(cinfo);
        if (!At<uint8>(cinfo, 0x41)) {
            if (!At<uint8>(master, 0x10)) {
                At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x1A4), 0x00)(cinfo);
            }
            At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x1A0), 0x00)(cinfo);
            if (At<uint8>(cinfo, 0x4A)) {
                At<void(__cdecl*)(void*, uint32)>(At<void*>(cinfo, 0x1A8), 0x00)(cinfo, At<uint8>(master, 0x08));
            }
            At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x18C), 0x00)(cinfo, At<uint8>(master, 0x08) ? 3 : 0);
            At<void(__cdecl*)(void*, int32)>(At<void*>(cinfo, 0x184), 0x00)(cinfo, 0);
        }
    }
    if (void* const progress = At<void*>(cinfo, 0x08)) {
        At<int32>(progress, 0x0C) = At<int32>(master, 0x0C);
        At<int32>(At<void*>(cinfo, 0x08), 0x10) = At<int32>(master, 0x0C) + (At<uint8>(master, 0x08) ? 2 : 1);
        if (At<uint8>(cinfo, 0x40) && !At<uint8>(At<void*>(cinfo, 0x190), 0x11)) {
            void* const progress2 = At<void*>(cinfo, 0x08);
            At<int32>(progress2, 0x10) = At<int32>(progress2, 0x10) + (At<uint8>(cinfo, 0x5A) ? 2 : 1);
        }
    }
}

// 0x5CCCC0 (cdecl, un argumento, 48 bytes; el listado decia FxEmitterBP_c): jinit_master_decompress(cinfo) de jdmaster. alloc_small(pool 1, 0x1C) -> cinfo+0x180; ranura 0 = prepare_for_output_pass 0x5CCB50,
// ranura 1 = finish_output_pass 0x5CCC90 (no enganchada), is_dummy_pass (+8) = 0; llama a master_selection (0x5CC9D0, con ESI = cinfo; reproducida arriba sin hook).
void ExeJinitMasterDecompress(void* cinfo) {
    uint8* const master = static_cast<uint8*>(AllocSmall(cinfo, 1, 0x1C));
    At<void*>(cinfo, 0x180) = master;
    At<uint32>(master, 0x00) = 0x5CCB50;
    At<uint32>(master, 0x04) = 0x5CCC90;
    At<uint8>(master, 0x08) = 0;
    MasterSelection(cinfo);
}
} // namespace

/**
 * Registra las rutinas internas de descompresion de la libjpeg del EXE (lotes 233A y 234A, sin validar).
 */
void LibJpegDecompress::InjectHooks() {
    RH_ScopedClass(LibJpegDecompress);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(ExeJpegMakeDDerivedTbl, "jpeg_make_d_derived_tbl", 0x5C9790);         // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegFillBitBuffer, "jpeg_fill_bit_buffer", 0x5C9A60);              // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegHuffDecode, "jpeg_huff_decode", 0x5C9B80);                     // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitHuffDecoder, "jinit_huff_decoder", 0x5CA260);                 // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeStartInputPass, "start_input_pass", 0x5CA6E0);                     // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeConsumeMarkers, "consume_markers", 0x5CA720);                      // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeResetInputController, "reset_input_controller", 0x5CA7E0);         // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitInputController, "jinit_input_controller", 0x5CA840);         // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeMakeFunnyPointers, "make_funny_pointers", 0x5CA940);               // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeSetWraparoundPointers, "set_wraparound_pointers", 0x5CAA90);       // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeSetBottomPointers, "set_bottom_pointers", 0x5CAB70);               // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitDMainController, "jinit_d_main_controller", 0x5CAE30);        // lote 233A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeGetDht, "get_dht", 0x5CB5B0);                                      // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeGetDri, "get_dri", 0x5CBB20);                                      // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeSkipVariable, "skip_variable", 0x5CC050);                          // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeNextMarker, "next_marker", 0x5CC0F0);                              // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeFirstMarker, "first_marker", 0x5CC1F0);                            // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeReadRestartMarker, "read_restart_marker", 0x5CC610);               // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitMarkerReader, "jinit_marker_reader", 0x5CC7D0);               // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegCalcOutputDimensions, "jpeg_calc_output_dimensions", 0x5CC8C0); // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExePrepareForOutputPass, "prepare_for_output_pass", 0x5CCB50);        // lote 234A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitMasterDecompress, "jinit_master_decompress", 0x5CCCC0);       // lote 234A (sin validar)
}
