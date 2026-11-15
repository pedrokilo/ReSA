#include "StdInc.h"

#include "LibJpegDecompressUpsample.h"
#include "LibJpegExe.h"

// Lote 235A (sin validar): jdpostct.c (jinit_post_controller) y jdsample.c (sep_upsample, jinit_upsampler) de la libjpeg 6b del EXE.
// Desplazamientos usados del jpeg_decompress_struct: +0x00 err, +0x04 mem ({+0x00 alloc_small, +0x08 alloc_sarray}), +0x24 num_components, +0x48 do_fancy_upsampling (byte),
// +0x4A quantize_colors (byte), +0x5C output_width, +0x64 out_color_components, +0xC4 comp_info (0x54 bytes cada uno), +0x10A CCIR601_sampling (byte), +0x110 max_h_samp_factor,
// +0x114 max_v_samp_factor, +0x118 min_DCT_scaled_size, +0x18C post, +0x1A0 upsample, +0x1A4 cconvert ({+0x04 color_convert}).
// jpeg_component_info: +0x08 h_samp_factor, +0x0C v_samp_factor, +0x24 DCT_scaled_size, +0x28 downsampled_width, +0x30 component_needed (byte).
// my_post_controller (0x1C): +0x00 start_pass, +0x08 whole_image, +0x0C buffer, +0x10 strip_height.
// my_upsampler (0x4C): +0x00 start_pass, +0x04 upsample, +0x08 need_context_rows (byte), +0x0C color_buf[4], +0x1C methods[4], +0x2C next_row_out, +0x30 rows_to_go,
// +0x34 rowgroup_height[4], +0x44 h_expand[4], +0x48 v_expand[4].
namespace {
using namespace LibJpegExe;

// Las rutinas de metodo de sobremuestreo (0x5CCF60 fullsize, 0x5CCF70 noop, 0x5CCF80 int, 0x5CD080 h2v1, 0x5CD0E0 h2v2, 0x5CD160 h2v1 fancy, 0x5CD220 h2v2 fancy) y start_pass
// (0x5CCE60 sobremuestreo, 0x5CCD70 post) solo se guardan por direccion.

inline void* AllocSmallViaMem(void* cinfo, int32 pool, uint32 size) {
    return At<void*(__cdecl*)(void*, int32, uint32)>(At<void*>(cinfo, 0x04), 0x00)(cinfo, pool, size);
}

inline void** AllocSarrayViaMem(void* cinfo, void* mem, int32 pool, uint32 samplesPerRow, uint32 numRows) {
    return At<void**(__cdecl*)(void*, int32, uint32, uint32)>(mem, 0x08)(cinfo, pool, samplesPerRow, numRows);
}

// 0x5CCDF0 (110 B): jinit_post_controller(cinfo, need_full_buffer BYTE) de jdpostct. alloc_small(pool 1, 0x1C) -> cinfo+0x18C; start_pass = 0x5CCD70; whole_image y buffer = 0.
// Solo si quantize_colors (+0x4A): strip_height (+0x10) = max_v_samp_factor y, si need_full_buffer, ERREXIT(4) y VUELVE (no reserva); si no, buffer = alloc_sarray(pool 1,
// output_width * out_color_components, strip_height) (mem+8).
void ExeJinitPostController(void* cinfo, int32 needFullBuffer) {
    uint8* const post = static_cast<uint8*>(AllocSmallViaMem(cinfo, 1, 0x1C));
    At<uint8*>(cinfo, 0x18C) = post;
    At<uint32>(post, 0x00) = 0x5CCD70;
    At<uint32>(post, 0x08) = 0;
    At<uint32>(post, 0x0C) = 0;
    if (At<uint8>(cinfo, 0x4A) == 0) {
        return;
    }
    At<int32>(post, 0x10) = At<int32>(cinfo, 0x114);
    if (static_cast<uint8>(needFullBuffer) != 0) {
        ErrExit(cinfo, JERR_BAD_BUFFER_MODE);
        return;
    }
    const int32 width = At<int32>(cinfo, 0x64) * At<int32>(cinfo, 0x5C);
    At<void**>(post, 0x0C) = AllocSarrayViaMem(cinfo, At<void*>(cinfo, 0x04), 1, static_cast<uint32>(width), At<uint32>(post, 0x10));
}

// 0x5CCE80 (222 B): sep_upsample(cinfo, input_buf, in_row_group_ctr*, in_row_groups_avail, output_buf, out_row_ctr*, out_rows_avail) de jdsample. Con next_row_out >= max_v_samp_factor (con signo)
// sobremuestrea un grupo de filas: por componente (num_components releido en cada vuelta) llama a methods[ci](cinfo, compptr, input_buf[ci] + *in_row_group_ctr * rowgroup_height[ci],
// &color_buf[ci]) y pone next_row_out = 0. num_rows = min(max_v - next_row_out, rows_to_go, out_rows_avail - *out_row_ctr) (comparaciones SIN signo); cconvert->color_convert(cinfo,
// color_buf, next_row_out, output_buf + *out_row_ctr, num_rows); *out_row_ctr += num_rows; rows_to_go -= num_rows; next_row_out += num_rows; y si next_row_out >= max_v (releido,
// con signo) incrementa *in_row_group_ctr.
void ExeSepUpsample(void* cinfo, uint8*** inputBuf, uint32* inRowGroupCtr, uint32 inRowGroupsAvail, void* outputBuf, uint32* outRowCtr, uint32 outRowsAvail) {
    uint8* const up = At<uint8*>(cinfo, 0x1A0);
    if (!(At<int32>(up, 0x2C) < At<int32>(cinfo, 0x114))) {
        uint8* compptr = At<uint8*>(cinfo, 0xC4);
        int32 ci = 0;
        if (At<int32>(cinfo, 0x24) > 0) {
            do {
                const uint32 rowgroupHeight = At<uint32>(up, 0x34 + 4 * static_cast<uint32>(ci));
                uint8** const inputData = inputBuf[ci] + static_cast<int32>(rowgroupHeight * (*inRowGroupCtr));
                At<void(__cdecl*)(void*, void*, uint8**, void*)>(up, 0x1C + 4 * static_cast<uint32>(ci))(cinfo, compptr, inputData, up + 0x0C + 4 * static_cast<uint32>(ci));
                ++ci;
                compptr += 0x54;
            } while (ci < At<int32>(cinfo, 0x24));
        }
        At<uint32>(up, 0x2C) = 0;
    }
    uint32 numRows = static_cast<uint32>(At<int32>(cinfo, 0x114) - At<int32>(up, 0x2C));
    if (numRows > At<uint32>(up, 0x30)) {
        numRows = At<uint32>(up, 0x30);
    }
    const uint32 outRowsLeft = outRowsAvail - *outRowCtr;
    if (numRows > outRowsLeft) {
        numRows = outRowsLeft;
    }
    uint8* const cconvert = At<uint8*>(cinfo, 0x1A4);
    At<void(__cdecl*)(void*, void*, uint32, void*, uint32)>(cconvert, 0x04)(cinfo, up + 0x0C, At<uint32>(up, 0x2C), static_cast<uint8*>(outputBuf) + 4 * (*outRowCtr), numRows);
    *outRowCtr += numRows;
    At<uint32>(up, 0x30) -= numRows;
    At<uint32>(up, 0x2C) += numRows;
    if (!(At<int32>(up, 0x2C) < At<int32>(cinfo, 0x114))) {
        ++(*inRowGroupCtr);
    }
}

// 0x5CD350 (477 B): jinit_upsampler(cinfo) de jdsample. alloc_small(pool 1, 0x4C) -> cinfo+0x1A0; start_pass = 0x5CCE60, upsample = 0x5CCE80, need_context_rows = 0.
// CCIR601_sampling (+0x10A) -> ERREXIT(0x19) y SIGUE. do_fancy = do_fancy_upsampling (+0x48) y min_DCT_scaled_size > 1 (con signo). Por componente: h_in_group = h * DCT_scaled_size /
// min_DCT_scaled_size y v_in_group (idiv con signo), rowgroup_height[ci] = v_in_group; sin component_needed -> methods = 0x5CCF70 (noop) sin buffer; h y v iguales al maximo -> 0x5CCF60
// (fullsize) sin buffer; h * 2 == max_h con v == max_v -> h2v1 (0x5CD160 fancy si do_fancy y downsampled_width > 2 SIN signo, si no 0x5CD080); h * 2 == max_h y v * 2 == max_v -> h2v2
// (0x5CD220 fancy, con need_context_rows = 1, o 0x5CD0E0); si no, si max_h % h == 0 y max_v % v == 0 -> 0x5CCF80 (int) con h_expand / v_expand (bytes); si no ERREXIT(0x26) y SIGUE.
// En los cuatro ultimos casos color_buf[ci] = alloc_sarray(pool 1, jround_up(output_width, max_h) (0x5CF8A0), max_v_samp_factor).
void ExeJinitUpsampler(void* cinfo) {
    uint8* const up = static_cast<uint8*>(AllocSmallViaMem(cinfo, 1, 0x4C));
    At<uint8*>(cinfo, 0x1A0) = up;
    At<uint32>(up, 0x00) = 0x5CCE60;
    At<uint32>(up, 0x04) = 0x5CCE80;
    At<uint8>(up, 0x08) = 0;
    if (At<uint8>(cinfo, 0x10A) != 0) {
        ErrExit(cinfo, JERR_CCIR601_NOTIMPL);
    }
    uint8 doFancy;
    if (At<uint8>(cinfo, 0x48) != 0 && At<int32>(cinfo, 0x118) > 1) {
        doFancy = 1;
    } else {
        doFancy = 0;
    }
    uint8* compptr = At<uint8*>(cinfo, 0xC4);
    int32 ci = 0;
    if (At<int32>(cinfo, 0x24) <= 0) {
        return;
    }
    do {
        const int32 dctScaledSize = At<int32>(compptr, 0x24);
        const int32 minDctScaledSize = At<int32>(cinfo, 0x118);
        const int32 hInGroup = (At<int32>(compptr, 0x08) * dctScaledSize) / minDctScaledSize;
        const int32 vInGroup = (At<int32>(compptr, 0x0C) * dctScaledSize) / minDctScaledSize;
        const int32 maxV = At<int32>(cinfo, 0x114);
        const int32 maxH = At<int32>(cinfo, 0x110);
        const uint32 idx = static_cast<uint32>(ci);
        At<int32>(up, 0x34 + 4 * idx) = vInGroup;
        bool allocBuffer = true;
        if (At<uint8>(compptr, 0x30) == 0) {
            At<uint32>(up, 0x1C + 4 * idx) = 0x5CCF70;
            allocBuffer = false;
        } else if (hInGroup == maxH && vInGroup == maxV) {
            At<uint32>(up, 0x1C + 4 * idx) = 0x5CCF60;
            allocBuffer = false;
        } else if (hInGroup * 2 == maxH && vInGroup == maxV) {
            if (doFancy != 0 && At<uint32>(compptr, 0x28) > 2) {
                At<uint32>(up, 0x1C + 4 * idx) = 0x5CD160;
            } else {
                At<uint32>(up, 0x1C + 4 * idx) = 0x5CD080;
            }
        } else if (hInGroup * 2 == maxH && vInGroup * 2 == maxV) {
            if (doFancy != 0 && At<uint32>(compptr, 0x28) > 2) {
                At<uint32>(up, 0x1C + 4 * idx) = 0x5CD220;
                At<uint8>(up, 0x08) = 1;
            } else {
                At<uint32>(up, 0x1C + 4 * idx) = 0x5CD0E0;
            }
        } else {
            const int32 hExpand = maxH / hInGroup;
            if (maxH % hInGroup == 0 && maxV % vInGroup == 0) {
                const int32 vExpand = maxV / vInGroup;
                At<uint32>(up, 0x1C + 4 * idx) = 0x5CCF80;
                At<uint8>(up, 0x44 + idx) = static_cast<uint8>(hExpand);
                At<uint8>(up, 0x48 + idx) = static_cast<uint8>(vExpand);
            } else {
                ErrExit(cinfo, JERR_FRACT_SAMPLE_NOTIMPL);
            }
        }
        if (allocBuffer) {
            void* const mem = At<void*>(cinfo, 0x04);
            const int32 width = reinterpret_cast<int32(__cdecl*)(int32, int32)>(0x5CF8A0)(At<int32>(cinfo, 0x5C), At<int32>(cinfo, 0x110)); // jround_up
            At<void**>(up, 0x0C + 4 * idx) = AllocSarrayViaMem(cinfo, mem, 1, static_cast<uint32>(width), static_cast<uint32>(At<int32>(cinfo, 0x114)));
        }
        ++ci;
        compptr += 0x54;
    } while (ci < At<int32>(cinfo, 0x24));
}
} // namespace

/**
 * Registra las rutinas de jdpostct y jdsample de la libjpeg del EXE (lote 235A, sin validar).
 */
void LibJpegDecompressUpsample::InjectHooks() {
    RH_ScopedClass(LibJpegDecompressUpsample);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(ExeJinitPostController, "jinit_post_controller", 0x5CCDF0); // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeSepUpsample, "sep_upsample", 0x5CCE80);                  // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitUpsampler, "jinit_upsampler", 0x5CD350);            // lote 235A (sin validar)
}
