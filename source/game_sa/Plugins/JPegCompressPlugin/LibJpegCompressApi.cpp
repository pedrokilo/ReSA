/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: LibJpegCompressApi
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "LibJpegCompressApi.h"
#include "LibJpegExe.h"

// Lote 235A (sin validar): jcapimin.c de la libjpeg 6b del EXE sobre el jpeg_compress_struct del EXE (0x168 bytes). Desplazamientos usados: +0x00 err, +0x04 mem, +0x08 progress
// ({+0x00 progress_monitor, +0x04 pass_counter, +0x08 pass_limit}), +0x0C client_data, +0x10 is_decompressor (byte), +0x14 global_state, +0x18 dest ({+0x10 term_destination}),
// +0x20 image_height, +0x30 input_gamma (double), +0x44 (puesto a cero), +0x48..+0x54 quant_tbl_ptrs[4], +0x58 dc_huff_tbl_ptrs[4], +0x68 ac_huff_tbl_ptrs[4], +0xD0 next_scanline,
// +0xE0 total_iMCU_rows, +0x13C master ({+0x00 prepare_for_pass, +0x08 finish_pass, +0x0D is_last_pass}), +0x148 coef ({+0x04 compress_data}), +0x14C marker ({+0x0C write_file_trailer}),
// +0x160 marker_list. JQUANT_TBL: +0x80 sent_table; JHUFF_TBL: +0x111 sent_table.
namespace {
using namespace LibJpegExe;

constexpr int32 CSTATE_START = 0x64;
constexpr int32 CSTATE_SCANNING = 0x65;
constexpr int32 CSTATE_RAW_OK = 0x66;
constexpr int32 CSTATE_WRCOEFS = 0x67;
constexpr int32 EXE_JPEG_LIB_VERSION = 0x3E;
constexpr int32 EXE_COMPRESS_STRUCT_SIZE = 0x168;

// 0x5CF960 (198 B): jpeg_CreateCompress(cinfo, version, structsize) de jcapimin. cinfo->mem = 0; version != 62 -> ERREXIT2(0x0C, 62, version) y SIGUE; structsize != 0x168 ->
// ERREXIT2(0x15, 0x168, structsize) y SIGUE. Conserva err (+0x00) y client_data (+0x0C), pone a cero los 0x168 bytes, restaura ambos, is_decompressor (byte +0x10) = 0, jinit_memory_mgr
// (0x5CF6D0) y pone a cero progress, dest, +0x44, +0x48..+0x54, dc/ac_huff_tbl_ptrs (+0x58..+0x74) y marker_list (+0x160); input_gamma = 1.0 (+0x30 = 0, +0x34 = 0x3FF00000) y
// global_state = CSTATE_START (0x64).
void ExeJpegCreateCompress(void* cinfo, int32 version, int32 structSize) {
    At<uint32>(cinfo, 0x04) = 0;
    if (version != EXE_JPEG_LIB_VERSION) {
        ErrExit2(cinfo, JERR_BAD_LIB_VERSION, EXE_JPEG_LIB_VERSION, version);
    }
    if (structSize != EXE_COMPRESS_STRUCT_SIZE) {
        ErrExit2(cinfo, JERR_BAD_STRUCT_SIZE, EXE_COMPRESS_STRUCT_SIZE, structSize);
    }
    void* const err = At<void*>(cinfo, 0x00);
    void* const clientData = At<void*>(cinfo, 0x0C);
    for (uint32 i = 0; i < EXE_COMPRESS_STRUCT_SIZE / 4; ++i) {
        At<uint32>(cinfo, i * 4) = 0;
    }
    At<void*>(cinfo, 0x00) = err;
    At<void*>(cinfo, 0x0C) = clientData;
    At<uint8>(cinfo, 0x10) = 0;
    reinterpret_cast<void(__cdecl*)(void*)>(0x5CF6D0)(cinfo); // jinit_memory_mgr
    At<uint32>(cinfo, 0x08) = 0;
    At<uint32>(cinfo, 0x18) = 0;
    At<uint32>(cinfo, 0x44) = 0;
    At<uint32>(cinfo, 0x48) = 0;
    At<uint32>(cinfo, 0x4C) = 0;
    At<uint32>(cinfo, 0x50) = 0;
    At<uint32>(cinfo, 0x54) = 0;
    for (uint32 i = 0; i < 4; ++i) {
        At<uint32>(cinfo, 0x58 + 4 * i) = 0;
        At<uint32>(cinfo, 0x68 + 4 * i) = 0;
    }
    At<uint32>(cinfo, 0x160) = 0;
    At<uint32>(cinfo, 0x30) = 0;
    At<uint32>(cinfo, 0x34) = 0x3FF00000;
    At<int32>(cinfo, 0x14) = CSTATE_START;
}

// 0x5CFA50 (102 B): jpeg_suppress_tables(cinfo, suppress BYTE) de jcapimin. Para cada tabla de cuantificacion no nula (+0x48, +0x4C, +0x50, +0x54) escribe sent_table (+0x80) = suppress y,
// para i = 0..3, sent_table (+0x111) = suppress en la tabla Huffman DC (+0x58 + 4i) y despues en la AC (+0x68 + 4i) si no son nulas.
void ExeJpegSuppressTables(void* cinfo, int32 suppress) {
    const uint8 value = static_cast<uint8>(suppress);
    for (uint32 i = 0; i < 4; ++i) {
        uint8* const qtbl = At<uint8*>(cinfo, 0x48 + 4 * i);
        if (qtbl != nullptr) {
            At<uint8>(qtbl, 0x80) = value;
        }
    }
    for (uint32 i = 0; i < 4; ++i) {
        uint8* const dctbl = At<uint8*>(cinfo, 0x58 + 4 * i);
        if (dctbl != nullptr) {
            At<uint8>(dctbl, 0x111) = value;
        }
        uint8* const actbl = At<uint8*>(cinfo, 0x68 + 4 * i);
        if (actbl != nullptr) {
            At<uint8>(actbl, 0x111) = value;
        }
    }
}

// 0x5CFAC0 (260 B): jpeg_finish_compress(cinfo) de jcapimin. Con global_state 0x65/0x66: next_scanline < image_height (sin signo) -> ERREXIT(0x43) y SIGUE; master->finish_pass (+0x08).
// Si no es 0x67: ERREXIT1(0x14, global_state) y SIGUE. Mientras master (releido) no tenga is_last_pass (+0x0D): prepare_for_pass (+0x00); por cada iMCU row (total_iMCU_rows
// +0xE0 releido, sin signo; no entra si es 0): con progress (+0x08) escribe pass_counter (+4) y pass_limit (+8) y llama a progress_monitor (+0); coef->compress_data(cinfo, 0)
// (+0x04, solo AL) y si es 0 ERREXIT(0x18) y SIGUE; despues finish_pass. Por ultimo marker->write_file_trailer (+0x0C), dest->term_destination (+0x10) y jpeg_abort (0x5C6B50).
void ExeJpegFinishCompress(void* cinfo) {
    const int32 state = At<int32>(cinfo, 0x14);
    if (state == CSTATE_SCANNING || state == CSTATE_RAW_OK) {
        if (At<uint32>(cinfo, 0xD0) < At<uint32>(cinfo, 0x20)) {
            ErrExit(cinfo, JERR_TOO_LITTLE_DATA);
        }
        At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x13C), 0x08)(cinfo);
    } else if (state != CSTATE_WRCOEFS) {
        ErrExit1(cinfo, JERR_BAD_STATE, At<int32>(cinfo, 0x14));
    }
    void* master = At<void*>(cinfo, 0x13C);
    if (At<uint8>(master, 0x0D) == 0) {
        do {
            At<void(__cdecl*)(void*)>(master, 0x00)(cinfo);
            uint32 row = 0;
            if (At<uint32>(cinfo, 0xE0) != 0) {
                do {
                    if (At<void*>(cinfo, 0x08) != nullptr) {
                        At<uint32>(At<void*>(cinfo, 0x08), 0x04) = row;
                        At<uint32>(At<void*>(cinfo, 0x08), 0x08) = At<uint32>(cinfo, 0xE0);
                        At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x08), 0x00)(cinfo);
                    }
                    const uint8 ok = At<uint8(__cdecl*)(void*, void*)>(At<void*>(cinfo, 0x148), 0x04)(cinfo, nullptr);
                    if (ok == 0) {
                        At<int32>(At<void*>(cinfo, 0x00), 0x14) = JERR_CANT_SUSPEND;
                        At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x00), 0x00)(cinfo);
                    }
                    ++row;
                } while (row < At<uint32>(cinfo, 0xE0));
            }
            At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x13C), 0x08)(cinfo);
            master = At<void*>(cinfo, 0x13C);
        } while (At<uint8>(master, 0x0D) == 0);
    }
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x14C), 0x0C)(cinfo);
    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x18), 0x10)(cinfo);
    reinterpret_cast<void(__cdecl*)(void*)>(0x5C6B50)(cinfo); // jpeg_abort
}
} // namespace

/**
 * Registra las rutinas de jcapimin de la libjpeg del EXE (lote 235A, sin validar).
 */
void LibJpegCompressApi::InjectHooks() {
    RH_ScopedClass(LibJpegCompressApi);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(ExeJpegCreateCompress, "jpeg_CreateCompress", 0x5CF960);     // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegSuppressTables, "jpeg_suppress_tables", 0x5CFA50);    // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegFinishCompress, "jpeg_finish_compress", 0x5CFAC0);    // lote 235A (sin validar)
}
