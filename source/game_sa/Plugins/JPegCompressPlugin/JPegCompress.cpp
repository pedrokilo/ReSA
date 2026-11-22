/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ExeLibJpeg
 * Reconstrucción: COMPLETA — 15/15 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 15/15 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: JPegPlugin
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "LibJpegExports.h"
#include "DatosExe/DataBssE4.h"
#include "jpeglib.h"
#include "LibJpegCallback.h"

static auto& g_ScreenshotFileBuf    = StaticRef<std::array<uint8, 204'800>>(0xBD0B78);
static auto& g_JpegDecodingToRaster = DatosExe::Bss::bBD0160;

// NOTSA. Convierte los fallos de libjpeg en un error visible de depuración en lugar
// de permitir que el juego termine sin contexto.
static void JPegErrorExit(j_common_ptr cinfo) {
    static char message[JMSG_LENGTH_MAX]{};

    const auto* einfo = cinfo->err;
    einfo->format_message(cinfo, message);
    NOTSA_UNREACHABLE("libjpeg error!\nCode:\n{}\nMessage:\n{}", einfo->msg_code, message);
}

/**
 * Registra las rutas de captura JPEG y las funciones heredadas de galería.
 */
void JPegPlugin::InjectHooks() {
    RH_ScopedNamespace(JPegPlugin);
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(JPegCompressScreen, 0x5D0470);
    RH_ScopedGlobalInstall(JPegCompressScreenToFile, 0x5D0820);
    RH_ScopedGlobalInstall(JPegCompressScreenToBuffer, 0x5D0740);
    RH_ScopedGlobalInstall(JPegScanlineToRaster, 0x5D0570); // lote 132A (banco Fn132A)
    RH_ScopedGlobalInstall(JPegDecompressToRaster, 0x5D05F0);
    RH_ScopedGlobalInstall(JPegDecompressToVramFromBuffer, 0x5D07A0); // Como el original: 0x5D0320 es su callback fill_input_buffer
}

// 0x5D0470
/**
 * Captura la cámara a RwImage y alimenta libjpeg línea por línea en formato RGBA.
 * El destino se inyecta mediante jpeg_destination_mgr para reutilizar la misma
 * compresión tanto con archivo como con otros buffers.
 *
 * @addr 0x5D0470
 */
void JPegCompressScreen(RwCamera* camera, jpeg_destination_mgr& dst) {
    // Como el original: primero se captura la pantalla y despues se prepara libjpeg.
    const auto image = RsGrabScreen(camera);

    jpeg_error_mgr       jerr{};
    jpeg_compress_struct cinfo{};
    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = JPegErrorExit;

    jpeg_CreateCompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));

    // It was JCS_RGB originally, but libjpeg errors with 'bogus colorspace'.
    cinfo.in_color_space = JCS_EXT_RGBA;
    jpeg_set_defaults(&cinfo);

    cinfo.image_width      = RwImageGetWidth(image);
    cinfo.image_height     = RwImageGetHeight(image);
    cinfo.dest             = &dst;
    cinfo.dct_method       = JDCT_FLOAT;
    cinfo.input_components = 4;
    jpeg_start_compress(&cinfo, true);

    // Como el original: el bucle se controla con next_scanline (no con el contador de filas).
    for (auto row = 0u; cinfo.next_scanline < cinfo.image_height; row++) {
        const auto* line = &RwImageGetPixels(image)[sizeof(RwRGBA) * row * cinfo.image_width];
        if (jpeg_write_scanlines(&cinfo, const_cast<uint8**>(&line), 1) != 1) {
            break;
        }
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    RwImageDestroy(image);
}

// 0x5D0820
/**
 * Implementa un jpeg_destination_mgr respaldado por el buffer global y vuelca al
 * archivo cada vez que libjpeg agota el espacio, incluyendo el remanente final.
 *
 * @addr 0x5D0820
 */
void JPegCompressScreenToFile(RwCamera* camera, const char* path) {
    // Como el original: el handle vive en 0xBD0B74 y se acepta solo si es positivo (jle).
    static auto& g_ScreenshotFileHandle = StaticRef<FILESTREAM>(0xBD0B74);

    g_ScreenshotFileHandle = CFileMgr::OpenFile(path, "wb");
    if ((int32)(intptr_t)g_ScreenshotFileHandle > 0) {
        jpeg_destination_mgr dst{};

        // InitDestination (0x5D03E0)
        dst.init_destination = [](j_compress_ptr c) {
            c->dest->next_output_byte = g_ScreenshotFileBuf.data();
            c->dest->free_in_buffer   = g_ScreenshotFileBuf.size();
        };

        // HDEmptyOutputBuffer (0x5D0400)
        dst.empty_output_buffer = [](j_compress_ptr c) -> boolean {
            CFileMgr::Write(g_ScreenshotFileHandle, g_ScreenshotFileBuf.data(), g_ScreenshotFileBuf.size());
            c->dest->next_output_byte = g_ScreenshotFileBuf.data();
            c->dest->free_in_buffer   = g_ScreenshotFileBuf.size();
            return true;
        };

        // HDTermDestination (0x5D0440)
        dst.term_destination = [](j_compress_ptr c) {
            // Como el original: resta con signo (test eax, eax; jle)
            const auto totalProcessed = (int32)((uintptr)c->dest->next_output_byte - (uintptr)g_ScreenshotFileBuf.data());
            if (totalProcessed > 0) {
                CFileMgr::Write(g_ScreenshotFileHandle, g_ScreenshotFileBuf.data(), totalProcessed);
            }
        };

        JPegCompressScreen(camera, dst);
        CFileMgr::CloseFile(g_ScreenshotFileHandle);
    }
}

// 0x5D0740 -- unused
void JPegCompressScreenToBuffer(RwCamera* cam, uint8** buffer, size_t& bufferSizeInOut) {
    static auto& Buffer         = StaticRef<uint8**>(0xC02B78);
    static auto& BufferSize     = StaticRef<size_t>(0xC02B7C);
    static auto& TotalProcessed = StaticRef<size_t>(0xC02B80);

    Buffer         = buffer;
    BufferSize     = bufferSizeInOut;
    TotalProcessed = 0;

    jpeg_destination_mgr dst{};

    // 0x5D0260
    dst.init_destination = [](j_compress_ptr c) {
        c->dest->next_output_byte = g_ScreenshotFileBuf.data();
        c->dest->free_in_buffer   = g_ScreenshotFileBuf.size();
    };

    // 0x5D0280
    dst.empty_output_buffer = [](j_compress_ptr c) -> boolean {
        if (TotalProcessed + g_ScreenshotFileBuf.size() < BufferSize) {
            std::memcpy(
                *Buffer + TotalProcessed,
                g_ScreenshotFileBuf.data(),
                g_ScreenshotFileBuf.size()
            );
            TotalProcessed += g_ScreenshotFileBuf.size();
        }
        c->dest->next_output_byte = g_ScreenshotFileBuf.data();
        c->dest->free_in_buffer   = g_ScreenshotFileBuf.size();

        return true;
    };

    // 0x5D02D0
    dst.term_destination = [](j_compress_ptr c) {
        // Como el original: copia solo el remanente pero suma el tamano completo del buffer.
        const auto remaining = static_cast<size_t>(c->dest->next_output_byte - g_ScreenshotFileBuf.data());
        if (TotalProcessed + remaining < BufferSize) {
            std::memcpy(
                *Buffer + TotalProcessed,
                g_ScreenshotFileBuf.data(),
                remaining
            );
            TotalProcessed += g_ScreenshotFileBuf.size();
        }
    };

    JPegCompressScreen(cam, dst); // Como el original: comprime con este destino
    bufferSizeInOut = TotalProcessed;
}

// 0x5D0570 (lote 132A, banco Fn132A; antes un bucle en linea de JPegDecompressToRaster): convierte los pixeles `first`..`last - 1` (comparacion SIN signo) de la scanline decodificada (el buffer del EXE en
// 0xBD0170, 4 bytes por pixel; se toman R, G y B por ese orden) al formato del raster y los escribe consecutivamente en el cursor global g_JpegDecodingToRaster (0xBD0160), que avanza 4 bytes por pixel.
// cdecl, cuatro argumentos de pila (raster, el numero de scanline -no se usa-, primer y ultimo pixel). Como el original: el formato es el byte cFormat (+0x23) << 8 (sin la mascara de rwRASTERFORMATMASK),
// el pixel se pide a RwRGBAToPixel (0x803740) con alfa 255 y se escribe el dword tal cual (el cursor se relee tras la llamada, como el EXE: nada lo toca).
void JPegScanlineToRaster(RwRaster* raster, int32 /*scanline*/, uint32 first, uint32 last) {
    auto* const scan = reinterpret_cast<uint8*>(0xBD0170);
    for (uint32 i = first; i < last; i++) {
        const uint8 r = scan[i * 4 + 0];
        const uint8 g = scan[i * 4 + 1];
        const uint8 b = scan[i * 4 + 2];
        RwRGBA color{ r, g, b, 255 };
        const RwInt32  format = static_cast<RwInt32>(static_cast<uint32>(raster->cFormat) << 8);
        const RwUInt32 pixel  = RwRGBAToPixel(&color, format);
        *reinterpret_cast<RwUInt32*>(g_JpegDecodingToRaster) = pixel;
        g_JpegDecodingToRaster++;
    }
}

// 0x5D05F0 -- PS2 leftover
// Decodifica scanlines JPEG y las convierte al formato de píxel del RwRaster bloqueado.
bool JPegDecompressToRaster(RwRaster* raster, jpeg_source_mgr& src) {
    // Como el original: 0xBD0170 es el propio buffer de la scanline (no un puntero).
    auto* const scan = reinterpret_cast<uint8*>(0xBD0170);

    jpeg_error_mgr         jerr{};
    jpeg_decompress_struct cinfo{};
    cinfo.err = jpeg_std_error(&jerr);
    jerr.error_exit = JPegErrorExit;

    jpeg_CreateDecompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));
    cinfo.src = &src;

    if (jpeg_read_header(&cinfo, true) != 1) {
        jpeg_destroy_decompress(&cinfo);
        return false;
    }
    cinfo.dct_method = JDCT_FLOAT; // Como el original: antes de jpeg_start_decompress
    if (!jpeg_start_decompress(&cinfo)) {
        jpeg_destroy_decompress(&cinfo);
        return false;
    }

    bool success{ false };
    g_JpegDecodingToRaster = reinterpret_cast<CRGBA*>(RwRasterLock(raster, 0, rwRASTERLOCKWRITE));
    while (cinfo.output_scanline < cinfo.output_height) {
        auto* scanlines = scan;
        if (jpeg_read_scanlines(&cinfo, &scanlines, 1) != 1) {
            success = false;
            break;
        }
        success = true;

        JPegScanlineToRaster(raster, static_cast<int32>(cinfo.output_scanline), 0x40, 0x240);
    }
    RwRasterUnlock(raster);
    g_JpegDecodingToRaster = nullptr;

    if (success) {
        jpeg_finish_decompress(&cinfo);
    }
    jpeg_destroy_decompress(&cinfo);
    return success;
}

// 0x5D07A0 -- PS2 leftover
// Adapta el buffer de galería del frontend como jpeg_source_mgr y reutiliza el
// decodificador común hacia RwRaster.
bool JPegDecompressToVramFromBuffer(RwRaster* raster, bool enable) {
    static auto& g_ScreenshotFilePointer = StaticRef<size_t>(0xBD0B70);

    if (!enable) {
        return false;
    }

    std::memcpy(g_ScreenshotFileBuf.data(), FrontEndMenuManager.m_GalleryImgBuffer, g_ScreenshotFileBuf.size());
    g_ScreenshotFilePointer = 0;

    jpeg_source_mgr src{};
    src.init_source = src.term_source = [](j_decompress_ptr) { /* nop */ };
    src.skip_input_data               = [](j_decompress_ptr, long) { /* nop */ };
    src.resync_to_restart = jpeg_resync_to_restart;

    // 0x5D0320
    src.fill_input_buffer = [](j_decompress_ptr d) -> boolean {
        std::memcpy(
            g_ScreenshotFileBuf.data(),
            &FrontEndMenuManager.m_GalleryImgBuffer[g_ScreenshotFilePointer],
            g_ScreenshotFileBuf.size()
        );

        d->src->next_input_byte = g_ScreenshotFileBuf.data();
        d->src->bytes_in_buffer = g_ScreenshotFileBuf.size();
        g_ScreenshotFilePointer += g_ScreenshotFileBuf.size();
        return true;
    };

    src.next_input_byte = g_ScreenshotFileBuf.data();
    JPegDecompressToRaster(raster, src);
    return true; // Como el original: devuelve 1 aunque falle la decodificacion
}

// ---------------------------------------------------------------------------------------------------------------------
// Lote 232A (sin validar): libjpeg 6b enlazada dentro del EXE (0x5C6BB0..0x5C9790), la que usan las capturas y la galeria.
// El listado atribuia estas direcciones a FxEmitterBP_c; el desensamblado (jcparam.c, jcprepct.c, jcsample.c, jdapistd.c,
// jdatadst.c, jdcoefct.c, jdcolor.c, jddctmgr.c) y sus llamadores dicen otra cosa. Esta es la libjpeg 6b del juego (booleanos de
// un byte, RGB de 4 bytes), NO la libjpeg-turbo de conan (su jpeglib.h aqui es version 80 y la disposicion de las estructuras
// difiere), por eso las estructuras se describen a mano con sus desplazamientos (comprobados con static_assert) y todo vive en
// el espacio de nombres ExeLibJpeg.
// Los punteros a funciones que se guardan en los controladores son direcciones del EXE (callbacks no reconstruidos y los que
// ya estan enganchados, que saltan al hook). Las rutinas internas que el compilador del EXE compilo con registros (add_huff_table
// 0x5C7010 EAX+EBX, std_huff_tables 0x5C70D0 ESI, output_pass_setup 0x5C8620 ESI) no se pueden enganchar en C++ puro:
// jpeg_set_defaults y jpeg_start_decompress llevan su cuerpo en C++ como rutinas internas propias (no enganchadas).
// ---------------------------------------------------------------------------------------------------------------------
#include "X87Math.h"

namespace ExeLibJpeg {

// Codigos de error de libjpeg 6b (jerror.h) usados por este lote.
namespace msg {
constexpr int32 BAD_BUFFER_MODE      = 0x04;
constexpr int32 BAD_HUFF_TABLE       = 0x08;
constexpr int32 BAD_IN_COLORSPACE    = 0x09;
constexpr int32 BAD_J_COLORSPACE     = 0x0A;
constexpr int32 BAD_STATE            = 0x14;
constexpr int32 CCIR601_NOTIMPL      = 0x19;
constexpr int32 COMPONENT_COUNT      = 0x1A;
constexpr int32 CONVERSION_NOTIMPL   = 0x1B;
constexpr int32 DQT_INDEX            = 0x1F;
constexpr int32 FILE_WRITE           = 0x25;
constexpr int32 FRACT_SAMPLE_NOTIMPL = 0x26;
constexpr int32 NOT_COMPILED         = 0x30;
constexpr int32 WRN_TOO_MUCH_DATA    = 0x7B;
} // namespace msg

constexpr int32 kPoolPermanent = 0;
constexpr int32 kPoolImage     = 1;

// Estados (global_state).
constexpr int32 CSTATE_START     = 0x64; // 100
constexpr int32 DSTATE_READY     = 0xCA; // 202
constexpr int32 DSTATE_PRELOAD   = 0xCB; // 203
constexpr int32 DSTATE_PRESCAN   = 0xCC; // 204
constexpr int32 DSTATE_SCANNING  = 0xCD; // 205
constexpr int32 DSTATE_RAW_OK    = 0xCE; // 206
constexpr int32 DSTATE_BUFIMAGE  = 0xCF; // 207

// J_COLOR_SPACE.
constexpr int32 kCsUNKNOWN = 0, kCsGRAYSCALE = 1, kCsRGB = 2, kCsYCbCr = 3, kCsCMYK = 4, kCsYCCK = 5;

struct ErrMgr {
    void (__cdecl* error_exit)(void* cinfo);                 // 0x00
    void (__cdecl* emit_message)(void* cinfo, int32 level);  // 0x04
    void (__cdecl* output_message)(void* cinfo);             // 0x08
    void (__cdecl* format_message)(void* cinfo, char* buf);  // 0x0C
    void (__cdecl* reset_error_mgr)(void* cinfo);            // 0x10
    int32 msg_code;                                          // 0x14
    int32 msg_parm_i[8];                                     // 0x18 (union con char s[80])
};
static_assert(offsetof(ErrMgr, msg_code) == 0x14 && offsetof(ErrMgr, msg_parm_i) == 0x18);

struct MemMgr {
    void*   (__cdecl* alloc_small)(void* cinfo, int32 pool, uint32 size);                          // 0x00
    void*   (__cdecl* alloc_large)(void* cinfo, int32 pool, uint32 size);                          // 0x04
    uint8** (__cdecl* alloc_sarray)(void* cinfo, int32 pool, uint32 samplesPerRow, uint32 rows);   // 0x08
};

struct ProgressMgr {
    void (__cdecl* progress_monitor)(void* cinfo); // 0x00
    int32 pass_counter;                            // 0x04
    int32 pass_limit;                              // 0x08
};

struct ComponentInfo { // 0x54 bytes
    int32  component_id;       // 0x00
    int32  component_index;    // 0x04
    int32  h_samp_factor;      // 0x08
    int32  v_samp_factor;      // 0x0C
    int32  quant_tbl_no;       // 0x10
    int32  dc_tbl_no;          // 0x14
    int32  ac_tbl_no;          // 0x18
    uint32 width_in_blocks;    // 0x1C
    uint32 height_in_blocks;   // 0x20
    int32  DCT_scaled_size;    // 0x24
    uint32 downsampled_width;  // 0x28
    uint32 downsampled_height; // 0x2C
    uint8  component_needed;   // 0x30
    uint8  pad31[3];
    int32  MCU_width;          // 0x34
    int32  MCU_height;         // 0x38
    int32  MCU_blocks;         // 0x3C
    int32  MCU_sample_width;   // 0x40
    int32  last_col_width;     // 0x44
    int32  last_row_height;    // 0x48
    void*  quant_table;        // 0x4C
    void*  dct_table;          // 0x50
};
static_assert(sizeof(ComponentInfo) == 0x54 && offsetof(ComponentInfo, component_needed) == 0x30 && offsetof(ComponentInfo, dct_table) == 0x50);

#pragma pack(push, 1)
struct QuantTbl { uint16 quantval[64]; uint8 sent_table; };                  // sent_table en 0x80
struct HuffTbl  { uint8 bits[17]; uint8 huffval[256]; uint8 sent_table; };   // huffval en 0x11, sent_table en 0x111
#pragma pack(pop)
static_assert(offsetof(QuantTbl, sent_table) == 0x80 && offsetof(HuffTbl, huffval) == 0x11 && offsetof(HuffTbl, sent_table) == 0x111);

// Controladores de compresion.
struct PrepCtl { // 0x20 bytes
    uintptr_t start_pass;        // 0x00
    uintptr_t pre_process_data;  // 0x04
    uint8**   color_buf[4];      // 0x08
    int32     rows_to_go;        // 0x18
    int32     next_buf_row;      // 0x1C
};
struct Downsampler { // 0x1C bytes
    uintptr_t start_pass;        // 0x00
    uintptr_t downsample;        // 0x04
    uint8     need_context_rows; // 0x08
    uint8     pad09[3];
    uintptr_t methods[4];        // 0x0C
};
static_assert(sizeof(PrepCtl) == 0x20 && sizeof(Downsampler) == 0x1C && offsetof(Downsampler, methods) == 0x0C);

struct CompressInfo {
    ErrMgr*      err;                 // 0x00
    MemMgr*      mem;                 // 0x04
    ProgressMgr* progress;            // 0x08
    void*        client_data;         // 0x0C
    uint8        is_decompressor;     // 0x10
    uint8        pad11[3];
    int32        global_state;        // 0x14
    void*        dest;                // 0x18
    uint32       image_width;         // 0x1C
    uint32       image_height;        // 0x20
    int32        input_components;    // 0x24
    int32        in_color_space;      // 0x28
    uint8        pad2C[4];
    double       input_gamma;         // 0x30
    int32        data_precision;      // 0x38
    int32        num_components;      // 0x3C
    int32        jpeg_color_space;    // 0x40
    ComponentInfo* comp_info;         // 0x44
    QuantTbl*    quant_tbl_ptrs[4];   // 0x48
    HuffTbl*     dc_huff_tbl_ptrs[4]; // 0x58
    HuffTbl*     ac_huff_tbl_ptrs[4]; // 0x68
    uint8        arith_dc_L[16];      // 0x78
    uint8        arith_dc_U[16];      // 0x88
    uint8        arith_ac_K[16];      // 0x98
    int32        num_scans;           // 0xA8
    const void*  scan_info;           // 0xAC
    uint8        raw_data_in;         // 0xB0
    uint8        arith_code;          // 0xB1
    uint8        optimize_coding;     // 0xB2
    uint8        CCIR601_sampling;    // 0xB3
    int32        smoothing_factor;    // 0xB4
    int32        dct_method;          // 0xB8
    uint32       restart_interval;    // 0xBC
    int32        restart_in_rows;     // 0xC0
    uint8        write_JFIF_header;   // 0xC4
    uint8        JFIF_major_version;  // 0xC5
    uint8        JFIF_minor_version;  // 0xC6
    uint8        density_unit;        // 0xC7
    uint16       X_density;           // 0xC8
    uint16       Y_density;           // 0xCA
    uint8        write_Adobe_marker;  // 0xCC
    uint8        padCD[0xD8 - 0xCD];
    int32        max_h_samp_factor;   // 0xD8
    int32        max_v_samp_factor;   // 0xDC
    uint8        padE0[0x144 - 0xE0];
    PrepCtl*     prep;                // 0x144
    void*        coef;                // 0x148
    void*        marker;              // 0x14C
    void*        cconvert;            // 0x150
    Downsampler* downsample;          // 0x154
};
static_assert(offsetof(CompressInfo, global_state) == 0x14 && offsetof(CompressInfo, data_precision) == 0x38);
static_assert(offsetof(CompressInfo, comp_info) == 0x44 && offsetof(CompressInfo, quant_tbl_ptrs) == 0x48);
static_assert(offsetof(CompressInfo, dc_huff_tbl_ptrs) == 0x58 && offsetof(CompressInfo, ac_huff_tbl_ptrs) == 0x68);
static_assert(offsetof(CompressInfo, arith_dc_L) == 0x78 && offsetof(CompressInfo, num_scans) == 0xA8);
static_assert(offsetof(CompressInfo, raw_data_in) == 0xB0 && offsetof(CompressInfo, CCIR601_sampling) == 0xB3);
static_assert(offsetof(CompressInfo, write_JFIF_header) == 0xC4 && offsetof(CompressInfo, X_density) == 0xC8);
static_assert(offsetof(CompressInfo, write_Adobe_marker) == 0xCC && offsetof(CompressInfo, max_h_samp_factor) == 0xD8);
static_assert(offsetof(CompressInfo, prep) == 0x144 && offsetof(CompressInfo, downsample) == 0x154);

// Descompresion.
struct MasterD {
    uintptr_t prepare_for_output_pass; // 0x00 (puntero a funcion con cinfo)
    uintptr_t finish_output_pass;      // 0x04
    uint8     is_dummy_pass;           // 0x08
};
struct MainD { uintptr_t start_pass; uintptr_t process_data; }; // process_data(cinfo, buf, row_ctr*, rows_avail)
struct InputCtl {
    uintptr_t consume_input, reset_input_controller, start_input_pass, finish_input_pass; // 0x00..0x0C
    uint8     has_multiple_scans;                                                        // 0x10
};
struct CoefCtlD { // 0x48 bytes
    uintptr_t start_input_pass;      // 0x00
    uintptr_t consume_data;          // 0x04
    uintptr_t start_output_pass;     // 0x08
    uintptr_t decompress_data;       // 0x0C
    void*     coef_arrays;           // 0x10
    uint32    MCU_ctr;               // 0x14
    int32     MCU_vert_offset;       // 0x18
    int32     MCU_rows_per_iMCU_row; // 0x1C
    void*     MCU_buffer[10];        // 0x20
};
struct CconvertCtl { // 0x18 bytes
    uintptr_t start_pass;     // 0x00
    uintptr_t color_convert;  // 0x04
    int32*    Cr_r_tab;       // 0x08
    int32*    Cb_b_tab;       // 0x0C
    int32*    Cr_g_tab;       // 0x10 (INT32, 4 bytes en el EXE)
    int32*    Cb_g_tab;       // 0x14
};
struct IdctCtl { // 0x24 bytes
    uintptr_t start_pass;              // 0x00
    uintptr_t inverse_DCT_method[4];   // 0x04
    int32     cur_method[4];           // 0x14
};
struct DestMgr { // jdatadst.c my_destination_mgr
    uint8*    next_output_byte;    // 0x00
    uint32    free_in_buffer;      // 0x04
    uintptr_t init_destination;    // 0x08
    uintptr_t empty_output_buffer; // 0x0C
    uintptr_t term_destination;    // 0x10
    uint8*    outfile;             // 0x14 (FILE* del CRT del EXE)
    uint8*    buffer;              // 0x18
};
static_assert(sizeof(CoefCtlD) == 0x48 && sizeof(CconvertCtl) == 0x18 && sizeof(IdctCtl) == 0x24);
static_assert(offsetof(InputCtl, has_multiple_scans) == 0x10 && offsetof(DestMgr, outfile) == 0x14 && offsetof(DestMgr, buffer) == 0x18);

struct DecompressInfo {
    ErrMgr*      err;                  // 0x00
    MemMgr*      mem;                  // 0x04
    ProgressMgr* progress;             // 0x08
    void*        client_data;          // 0x0C
    uint8        is_decompressor;      // 0x10
    uint8        pad11[3];
    int32        global_state;         // 0x14
    void*        src;                  // 0x18
    uint32       image_width;          // 0x1C
    uint32       image_height;         // 0x20
    int32        num_components;       // 0x24
    int32        jpeg_color_space;     // 0x28
    int32        out_color_space;      // 0x2C
    uint8        pad30[0x40 - 0x30];
    uint8        buffered_image;       // 0x40
    uint8        raw_data_out;         // 0x41
    uint8        pad42[0x4A - 0x42];
    uint8        quantize_colors;      // 0x4A
    uint8        pad4B[0x5C - 0x4B];
    uint32       output_width;         // 0x5C
    uint32       output_height;        // 0x60
    int32        out_color_components; // 0x64
    int32        output_components;    // 0x68
    uint8        pad6C[0x78 - 0x6C];
    uint32       output_scanline;      // 0x78
    int32        input_scan_number;    // 0x7C
    uint8        pad80[0x84 - 0x80];
    int32        output_scan_number;   // 0x84
    uint8        pad88[0xC4 - 0x88];
    ComponentInfo* comp_info;          // 0xC4
    uint8        padC8[0x180 - 0xC8];
    MasterD*     master;               // 0x180
    MainD*       main;                 // 0x184
    CoefCtlD*    coef;                 // 0x188
    void*        post;                 // 0x18C
    InputCtl*    inputctl;             // 0x190
    void*        marker;               // 0x194
    void*        entropy;              // 0x198
    IdctCtl*     idct;                 // 0x19C
    void*        upsample;             // 0x1A0
    CconvertCtl* cconvert;             // 0x1A4
};
static_assert(offsetof(DecompressInfo, global_state) == 0x14 && offsetof(DecompressInfo, jpeg_color_space) == 0x28);
static_assert(offsetof(DecompressInfo, buffered_image) == 0x40 && offsetof(DecompressInfo, quantize_colors) == 0x4A);
static_assert(offsetof(DecompressInfo, output_width) == 0x5C && offsetof(DecompressInfo, output_scanline) == 0x78);
static_assert(offsetof(DecompressInfo, output_scan_number) == 0x84 && offsetof(DecompressInfo, comp_info) == 0xC4);
static_assert(offsetof(DecompressInfo, master) == 0x180 && offsetof(DecompressInfo, inputctl) == 0x190);
static_assert(offsetof(DecompressInfo, idct) == 0x19C && offsetof(DecompressInfo, cconvert) == 0x1A4);

// ERREXIT / ERREXIT1 / ERREXIT2 (jerror.h): msg_code y parametros, luego error_exit. error_exit puede no volver, pero si vuelve el
// EXE sigue ejecutando el resto de la funcion, igual que aqui.
template <class T> inline void ErrExit(T* cinfo, int32 code) {
    cinfo->err->msg_code = code;
    cinfo->err->error_exit(cinfo);
}
template <class T> inline void ErrExit1(T* cinfo, int32 code, int32 p1) {
    cinfo->err->msg_code = code;
    cinfo->err->msg_parm_i[0] = p1;
    cinfo->err->error_exit(cinfo);
}
template <class T> inline void ErrExit2(T* cinfo, int32 code, int32 p1, int32 p2) {
    cinfo->err->msg_code = code;
    cinfo->err->msg_parm_i[0] = p1;
    cinfo->err->msg_parm_i[1] = p2;
    cinfo->err->error_exit(cinfo);
}

// Funciones del EXE llamadas por direccion fija (no se reconstruyen en este lote).
inline QuantTbl* ExeAllocQuantTable(CompressInfo* cinfo) { return reinterpret_cast<QuantTbl*>(LibJpegCompress::JpegAllocQuantTable(reinterpret_cast<ExeJpegCompress*>(cinfo))); } // jpeg_alloc_quant_table
inline HuffTbl*  ExeAllocHuffTable(CompressInfo* cinfo)  { return reinterpret_cast<HuffTbl*>(LibJpegCompress::JpegAllocHuffTable(reinterpret_cast<ExeJpegCompress*>(cinfo))); }  // jpeg_alloc_huff_table
inline void      ExeInitMasterDecompress(DecompressInfo* cinfo) { LibJpegExportJinitMasterDecompress(cinfo); }    // jinit_master_decompress
inline uint32    ExeFwrite(const void* buf, uint32 size, uint32 count, void* file) { return reinterpret_cast<uint32(__cdecl*)(const void*, uint32, uint32, void*)>(uintptr_t{0x823674})(buf, size, count, file); } // fwrite del CRT del EXE
inline int32     ExeFflush(void* file) { return reinterpret_cast<int32(__cdecl*)(void*)>(uintptr_t{0x823E86})(file); } // fflush del CRT del EXE

// 0x5C6BF0 (jcparam.c)
// cdecl (cinfo, which_tbl, basic_table, scale_factor, force_baseline). Tabla escalada por 'scale_factor' porciento con redondeo,
// limitada a [1, 32767] (y 255 si force_baseline). La division es con signo de 32 bits, como el EXE.
static void __cdecl jpeg_add_quant_table(CompressInfo* cinfo, int32 which_tbl, const uint32* basic_table, int32 scale_factor, uint8 force_baseline) {
    if (cinfo->global_state != CSTATE_START) {
        ErrExit1(cinfo, msg::BAD_STATE, cinfo->global_state);
    }
    if (which_tbl < 0 || which_tbl >= 4) {
        ErrExit1(cinfo, msg::DQT_INDEX, which_tbl);
    }
    QuantTbl** qtblptr = cinfo->quant_tbl_ptrs + which_tbl;
    if (*qtblptr == nullptr) {
        *qtblptr = ExeAllocQuantTable(cinfo);
    }
    for (int32 i = 0; i < 64; i++) {
        int32 temp = static_cast<int32>(basic_table[i] * static_cast<uint32>(scale_factor) + 50u) / 100;
        if (temp <= 0) {
            temp = 1;
        } else if (temp > 0x7FFF) {
            temp = 0x7FFF;
        }
        if (force_baseline && temp > 0xFF) {
            temp = 0xFF;
        }
        (*qtblptr)->quantval[i] = static_cast<uint16>(temp);
    }
    (*qtblptr)->sent_table = 0;
}

// Cuerpo de 0x5C7010 (add_huff_table, jcparam.c). En el EXE recibe bits en EAX y htblptr en EBX (+ cinfo y val en la pila):
// convencion con dos registros que C++ no puede leer, por eso NO se engancha; solo lo usa std_huff_tables (interna).
static void add_huff_table(CompressInfo* cinfo, HuffTbl** htblptr, const uint8* bits, const uint8* val) {
    if (*htblptr == nullptr) {
        *htblptr = ExeAllocHuffTable(cinfo);
    }
    std::memcpy((*htblptr)->bits, bits, 17);
    int32 nsymbols = 0;
    for (int32 len = 1; len <= 16; len++) {
        nsymbols += bits[len];
    }
    if (nsymbols < 1 || nsymbols > 256) {
        ErrExit(cinfo, msg::BAD_HUFF_TABLE);
    }
    std::memcpy((*htblptr)->huffval, val, static_cast<size_t>(nsymbols));
    (*htblptr)->sent_table = 0;
}

// Cuerpo de 0x5C70D0 (std_huff_tables, jcparam.c). En el EXE recibe cinfo en ESI: NO se engancha (ver arriba).
// Orden del EXE: DC luminancia, AC luminancia, DC crominancia, AC crominancia; tablas de bits/valores en el .rdata del EXE.
static void std_huff_tables(CompressInfo* cinfo) {
    add_huff_table(cinfo, &cinfo->dc_huff_tbl_ptrs[0], reinterpret_cast<const uint8*>(0x86B6E4), reinterpret_cast<const uint8*>(0x86B6F8));
    add_huff_table(cinfo, &cinfo->ac_huff_tbl_ptrs[0], reinterpret_cast<const uint8*>(0x86B62C), reinterpret_cast<const uint8*>(0x86B640));
    add_huff_table(cinfo, &cinfo->dc_huff_tbl_ptrs[1], reinterpret_cast<const uint8*>(0x86B60C), reinterpret_cast<const uint8*>(0x86B620));
    add_huff_table(cinfo, &cinfo->ac_huff_tbl_ptrs[1], reinterpret_cast<const uint8*>(0x86B550), reinterpret_cast<const uint8*>(0x86B568));
}

static void SetComp(CompressInfo* cinfo, int32 index, int32 id, int32 hsamp, int32 vsamp, int32 quant, int32 dctbl, int32 actbl) {
    ComponentInfo* compptr = &cinfo->comp_info[index];
    compptr->component_id  = id;
    compptr->h_samp_factor = hsamp;
    compptr->v_samp_factor = vsamp;
    compptr->quant_tbl_no  = quant;
    compptr->dc_tbl_no     = dctbl;
    compptr->ac_tbl_no     = actbl;
}

// 0x5C7130 (jcparam.c)
// cdecl (cinfo, colorspace). Tabla de saltos en 0x5C73FC (0 desconocido, 1 gris, 2 RGB, 3 YCbCr, 4 CMYK, 5 YCCK).
static void __cdecl jpeg_set_colorspace(CompressInfo* cinfo, int32 colorspace) {
    if (cinfo->global_state != CSTATE_START) {
        ErrExit1(cinfo, msg::BAD_STATE, cinfo->global_state);
    }
    cinfo->jpeg_color_space   = colorspace;
    cinfo->write_JFIF_header  = 0;
    cinfo->write_Adobe_marker = 0;
    switch (static_cast<uint32>(colorspace)) {
    case kCsGRAYSCALE:
        cinfo->write_JFIF_header = 1;
        cinfo->num_components = 1;
        SetComp(cinfo, 0, 1, 1, 1, 0, 0, 0);
        break;
    case kCsRGB:
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 3;
        SetComp(cinfo, 0, 0x52, 1, 1, 0, 0, 0); // 'R'
        SetComp(cinfo, 1, 0x47, 1, 1, 0, 0, 0); // 'G'
        SetComp(cinfo, 2, 0x42, 1, 1, 0, 0, 0); // 'B'
        break;
    case kCsYCbCr:
        cinfo->write_JFIF_header = 1;
        cinfo->num_components = 3;
        SetComp(cinfo, 0, 1, 2, 2, 0, 0, 0);
        SetComp(cinfo, 1, 2, 1, 1, 1, 1, 1);
        SetComp(cinfo, 2, 3, 1, 1, 1, 1, 1);
        break;
    case kCsCMYK:
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 4;
        SetComp(cinfo, 0, 0x43, 1, 1, 0, 0, 0); // 'C'
        SetComp(cinfo, 1, 0x4D, 1, 1, 0, 0, 0); // 'M'
        SetComp(cinfo, 2, 0x59, 1, 1, 0, 0, 0); // 'Y'
        SetComp(cinfo, 3, 0x4B, 1, 1, 0, 0, 0); // 'K'
        break;
    case kCsYCCK:
        cinfo->write_Adobe_marker = 1;
        cinfo->num_components = 4;
        SetComp(cinfo, 0, 1, 2, 2, 0, 0, 0);
        SetComp(cinfo, 1, 2, 1, 1, 1, 1, 1);
        SetComp(cinfo, 2, 3, 1, 1, 1, 1, 1);
        SetComp(cinfo, 3, 4, 2, 2, 0, 0, 0);
        break;
    case kCsUNKNOWN:
        cinfo->num_components = cinfo->input_components;
        if (cinfo->num_components < 1 || cinfo->num_components > 4) {
            ErrExit2(cinfo, msg::COMPONENT_COUNT, cinfo->num_components, 4);
        }
        for (int32 ci = 0; ci < cinfo->num_components; ci++) {
            SetComp(cinfo, ci, ci, 1, 1, 0, 0, 0);
        }
        break;
    default:
        ErrExit(cinfo, msg::BAD_J_COLORSPACE);
        break;
    }
}

// 0x5C7420 (jcparam.c)
// cdecl (cinfo). Tabla de saltos en 0x5C7570: gris -> gris; RGB y YCbCr -> YCbCr; CMYK -> CMYK; YCCK -> YCCK; desconocido -> desconocido.
// El EXE inlina jpeg_set_colorspace en los casos gris y YCCK (mismo efecto).
static void __cdecl jpeg_default_colorspace(CompressInfo* cinfo) {
    switch (static_cast<uint32>(cinfo->in_color_space)) {
    case kCsUNKNOWN:   jpeg_set_colorspace(cinfo, kCsUNKNOWN); break;
    case kCsGRAYSCALE: jpeg_set_colorspace(cinfo, kCsGRAYSCALE); break;
    case kCsRGB:
    case kCsYCbCr:     jpeg_set_colorspace(cinfo, kCsYCbCr); break;
    case kCsCMYK:      jpeg_set_colorspace(cinfo, kCsCMYK); break;
    case kCsYCCK:      jpeg_set_colorspace(cinfo, kCsYCCK); break;
    default:            ErrExit(cinfo, msg::BAD_IN_COLORSPACE); break;
    }
}

// 0x5C7590 (jcparam.c)
// cdecl (cinfo). jpeg_set_quality(75) esta inlinado: dos jpeg_add_quant_table con escala 50 (200 - 2*75) y force_baseline.
// Tablas de cuantizacion luminancia 0x86B450 y crominancia 0x86B350 (unsigned int[64]).
static void __cdecl jpeg_set_defaults(CompressInfo* cinfo) {
    if (cinfo->global_state != CSTATE_START) {
        ErrExit1(cinfo, msg::BAD_STATE, cinfo->global_state);
    }
    if (cinfo->comp_info == nullptr) {
        cinfo->comp_info = static_cast<ComponentInfo*>(cinfo->mem->alloc_small(cinfo, kPoolPermanent, 4 * sizeof(ComponentInfo)));
    }
    cinfo->data_precision = 8;
    jpeg_add_quant_table(cinfo, 0, reinterpret_cast<const uint32*>(0x86B450), 50, 1);
    jpeg_add_quant_table(cinfo, 1, reinterpret_cast<const uint32*>(0x86B350), 50, 1);
    std_huff_tables(cinfo);
    for (int32 i = 0; i < 16; i++) {
        cinfo->arith_dc_L[i] = 0;
        cinfo->arith_dc_U[i] = 1;
        cinfo->arith_ac_K[i] = 5;
    }
    cinfo->num_scans = 0;
    cinfo->scan_info = nullptr;
    cinfo->raw_data_in = 0;
    cinfo->arith_code = 0;
    cinfo->optimize_coding = 0;
    if (cinfo->data_precision > 8) {
        cinfo->optimize_coding = 1;
    }
    cinfo->CCIR601_sampling = 0;
    cinfo->smoothing_factor = 0;
    cinfo->dct_method = 0;
    cinfo->restart_interval = 0;
    cinfo->restart_in_rows = 0;
    cinfo->JFIF_major_version = 1;
    cinfo->JFIF_minor_version = 1;
    cinfo->density_unit = 0;
    cinfo->X_density = 1;
    cinfo->Y_density = 1;
    jpeg_default_colorspace(cinfo);
}

// 0x5C7920 (jcprepct.c)
// cdecl (cinfo, need_full_buffer). Sin filas de contexto (no compilado). start_pass 0x5C76A0, pre_process_data 0x5C7710 (EXE).
static void __cdecl jinit_c_prep_controller(CompressInfo* cinfo, uint8 need_full_buffer) {
    if (need_full_buffer) {
        ErrExit(cinfo, msg::BAD_BUFFER_MODE);
    }
    PrepCtl* prep = static_cast<PrepCtl*>(cinfo->mem->alloc_small(cinfo, kPoolImage, sizeof(PrepCtl)));
    cinfo->prep = prep;
    prep->start_pass = LIBJPEG_CB(0x5C76A0, "jcprepct", "start_pass_prep");
    if (cinfo->downsample->need_context_rows) {
        ErrExit(cinfo, msg::NOT_COMPILED);
        return;
    }
    prep->pre_process_data = LIBJPEG_CB(0x5C7710, "jcprepct", "pre_process_data");
    ComponentInfo* compptr = cinfo->comp_info;
    for (int32 ci = 0; ci < cinfo->num_components; ci++, compptr++) {
        const int32 samplesPerRow = static_cast<int32>(compptr->width_in_blocks * cinfo->max_h_samp_factor * 8) / compptr->h_samp_factor;
        prep->color_buf[ci] = cinfo->mem->alloc_sarray(cinfo, kPoolImage, static_cast<uint32>(samplesPerRow), static_cast<uint32>(cinfo->max_v_samp_factor));
    }
}

// 0x5C79F0 (jcsample.c)
// cdecl (image_data, num_rows, input_cols, output_cols). Repite el ultimo pixel de cada fila en las columnas de relleno.
static void __cdecl expand_right_edge(uint8** image_data, int32 num_rows, int32 input_cols, int32 output_cols) {
    const int32 numcols = output_cols - input_cols;
    if (numcols > 0) {
        for (int32 row = 0; row < num_rows; row++) {
            uint8* ptr = image_data[row] + input_cols;
            std::memset(ptr, ptr[-1], static_cast<size_t>(numcols));
        }
    }
}

// 0x5C7DF0 (jcsample.c)
// cdecl (cinfo). Metodos del EXE: fullsize 0x5C7C10, h2v1 0x5C7C60, h2v2 0x5C7D10, entero 0x5C7AE0; start_pass 0x5C79E0 y
// downsample 0x5C7A50. Sin suavizado de entrada (smoothing_factor no se mira, como el EXE).
static void __cdecl jinit_downsampler(CompressInfo* cinfo) {
    Downsampler* downsample = static_cast<Downsampler*>(cinfo->mem->alloc_small(cinfo, kPoolImage, sizeof(Downsampler)));
    cinfo->downsample = downsample;
    downsample->start_pass = LIBJPEG_CB(0x5C79E0, "jcsample", "start_pass_downsample");
    downsample->downsample = LIBJPEG_CB(0x5C7A50, "jcsample", "sep_downsample");
    downsample->need_context_rows = 0;
    if (cinfo->CCIR601_sampling) {
        ErrExit(cinfo, msg::CCIR601_NOTIMPL);
    }
    ComponentInfo* compptr = cinfo->comp_info;
    for (int32 ci = 0; ci < cinfo->num_components; ci++, compptr++) {
        const int32 h = compptr->h_samp_factor;
        const int32 v = compptr->v_samp_factor;
        if (h == cinfo->max_h_samp_factor && v == cinfo->max_v_samp_factor) {
            downsample->methods[ci] = LIBJPEG_CB(0x5C7C10, "jcsample", "fullsize_downsample");
        } else if (h * 2 == cinfo->max_h_samp_factor && v == cinfo->max_v_samp_factor) {
            downsample->methods[ci] = LIBJPEG_CB(0x5C7C60, "jcsample", "h2v1_downsample");
        } else if (h * 2 == cinfo->max_h_samp_factor && v * 2 == cinfo->max_v_samp_factor) {
            downsample->methods[ci] = LIBJPEG_CB(0x5C7D10, "jcsample", "h2v2_downsample");
        } else if ((cinfo->max_h_samp_factor % h) == 0 && (cinfo->max_v_samp_factor % v) == 0) {
            downsample->methods[ci] = LIBJPEG_CB(0x5C7AE0, "jcsample", "int_downsample");
        } else {
            ErrExit(cinfo, msg::FRACT_SAMPLE_NOTIMPL);
        }
    }
}

// Cuerpo de 0x5C8620 (output_pass_setup, jdapistd.c). En el EXE recibe cinfo en ESI: NO se engancha; lo usa jpeg_start_decompress.
static uint8 output_pass_setup(DecompressInfo* cinfo) {
    if (cinfo->global_state != DSTATE_PRESCAN) {
        reinterpret_cast<void(__cdecl*)(void*)>(cinfo->master->prepare_for_output_pass)(cinfo);
        cinfo->output_scanline = 0;
        cinfo->global_state = DSTATE_PRESCAN;
    }
    while (cinfo->master->is_dummy_pass) {
        ErrExit(cinfo, msg::NOT_COMPILED);
    }
    cinfo->global_state = cinfo->raw_data_out ? DSTATE_RAW_OK : DSTATE_SCANNING;
    return 1;
}

// 0x5C8690 (jdapistd.c)
// cdecl (cinfo, scanlines, max_lines) -> filas leidas.
static uint32 __cdecl jpeg_read_scanlines(DecompressInfo* cinfo, uint8** scanlines, uint32 max_lines) {
    if (cinfo->global_state != DSTATE_SCANNING) {
        ErrExit1(cinfo, msg::BAD_STATE, cinfo->global_state);
    }
    if (cinfo->output_scanline >= cinfo->output_height) {
        cinfo->err->msg_code = msg::WRN_TOO_MUCH_DATA;
        cinfo->err->emit_message(cinfo, -1);
        return 0;
    }
    if (cinfo->progress != nullptr) {
        cinfo->progress->pass_counter = static_cast<int32>(cinfo->output_scanline);
        cinfo->progress->pass_limit   = static_cast<int32>(cinfo->output_height);
        cinfo->progress->progress_monitor(cinfo);
    }
    uint32 row_ctr = 0;
    reinterpret_cast<void(__cdecl*)(void*, uint8**, uint32*, uint32)>(cinfo->main->process_data)(cinfo, scanlines, &row_ctr, max_lines);
    cinfo->output_scanline += row_ctr;
    return row_ctr;
}

// 0x5C87E0 (jdapistd.c)
// cdecl (cinfo) -> boolean. Sin soporte de varios escaneos (D_MULTISCAN_FILES_SUPPORTED no compilado): NOT_COMPILED.
// jinit_master_decompress (0x5CCCC0) se llama por direccion fija; output_pass_setup es la rutina interna de arriba.
static uint8 __cdecl jpeg_start_decompress(DecompressInfo* cinfo) {
    if (cinfo->global_state == DSTATE_READY) {
        ExeInitMasterDecompress(cinfo);
        if (cinfo->buffered_image) {
            cinfo->global_state = DSTATE_BUFIMAGE;
            return 1;
        }
        cinfo->global_state = DSTATE_PRELOAD;
    }
    if (cinfo->global_state == DSTATE_PRELOAD) {
        if (cinfo->inputctl->has_multiple_scans) {
            ErrExit(cinfo, msg::NOT_COMPILED);
        }
        cinfo->output_scan_number = cinfo->input_scan_number;
    } else if (cinfo->global_state != DSTATE_PRESCAN) {
        ErrExit1(cinfo, msg::BAD_STATE, cinfo->global_state);
    }
    return output_pass_setup(cinfo);
}

// 0x5C88F0 (jdatadst.c)
// cdecl (cinfo): vuelca lo que quede del buffer de 4096 bytes con fwrite, fflush y comprueba el indicador de error del FILE
// (bit 0x20 de _flag, en +0x0C del FILE del CRT del EXE, que es lo que expande el macro ferror).
static void __cdecl term_destination(CompressInfo* cinfo) {
    DestMgr* dest = static_cast<DestMgr*>(cinfo->dest);
    const uint32 datacount = 0x1000 - dest->free_in_buffer;
    if (datacount != 0) {
        if (ExeFwrite(dest->buffer, 1, datacount, dest->outfile) != datacount) {
            ErrExit(cinfo, msg::FILE_WRITE);
        }
    }
    ExeFflush(dest->outfile);
    if (dest->outfile[0x0C] & 0x20) {
        ErrExit(cinfo, msg::FILE_WRITE);
    }
}

// 0x5C8E20 (jdcoefct.c)
// cdecl (cinfo, need_full_buffer). Sin buffer de coeficientes completo (NOT_COMPILED). Callbacks del EXE: start_input_pass 0x5C8B40,
// start_output_pass 0x5C8B60, decompress_data 0x5C8B70, consume_data 0x5C8E10 (dummy, devuelve 0).
static void __cdecl jinit_d_coef_controller(DecompressInfo* cinfo, uint8 need_full_buffer) {
    CoefCtlD* coef = static_cast<CoefCtlD*>(cinfo->mem->alloc_small(cinfo, kPoolImage, sizeof(CoefCtlD)));
    cinfo->coef = coef;
    coef->start_input_pass = LIBJPEG_CB(0x5C8B40, "jdcoefct", "start_input_pass");
    coef->start_output_pass = LIBJPEG_CB(0x5C8B60, "jdcoefct", "start_output_pass");
    if (need_full_buffer) {
        ErrExit(cinfo, msg::NOT_COMPILED);
        return;
    }
    uint8* buffer = static_cast<uint8*>(cinfo->mem->alloc_large(cinfo, kPoolImage, 10 * 128));
    for (int32 i = 0; i < 10; i++) {
        coef->MCU_buffer[i] = buffer + i * 128;
    }
    coef->consume_data = LIBJPEG_CB(0x5C8E10, "jdcoefct", "dummy_consume_data");
    coef->decompress_data = LIBJPEG_CB(0x5C8B70, "jdcoefct", "decompress_onepass");
    coef->coef_arrays = nullptr;
}

// Cuerpo de 0x5C8EE0 (build_ycc_rgb_table, jdcolor.c): tablas de conversion YCbCr -> RGB con SCALEBITS = 16 (FIX(1.40200) = 91881,
// FIX(1.77200) = 116130, FIX(0.71414) = 46802, FIX(0.34414) = 22554, ONE_HALF = 0x8000), x = i - 128 para i = 0..255.
static void build_ycc_rgb_table(DecompressInfo* cinfo) {
    CconvertCtl* cconvert = cinfo->cconvert;
    cconvert->Cr_r_tab = static_cast<int32*>(cinfo->mem->alloc_small(cinfo, kPoolImage, 256 * sizeof(int32)));
    cconvert->Cb_b_tab = static_cast<int32*>(cinfo->mem->alloc_small(cinfo, kPoolImage, 256 * sizeof(int32)));
    cconvert->Cr_g_tab = static_cast<int32*>(cinfo->mem->alloc_small(cinfo, kPoolImage, 256 * sizeof(int32)));
    cconvert->Cb_g_tab = static_cast<int32*>(cinfo->mem->alloc_small(cinfo, kPoolImage, 256 * sizeof(int32)));
    for (int32 i = 0; i < 256; i++) {
        const int32 x = i - 128;
        cconvert->Cr_r_tab[i] = (91881 * x + 0x8000) >> 16;
        cconvert->Cb_b_tab[i] = (116130 * x + 0x8000) >> 16;
        cconvert->Cr_g_tab[i] = -46802 * x;
        cconvert->Cb_g_tab[i] = -22554 * x + 0x8000;
    }
}

// 0x5C8EE0: el EXE recibe cinfo en EAX (resto de registros no se leen; EBX, ESI, EDI y EBP se conservan). EntryEaxCpp tiene que ser
// lo primero que se ejecuta (lo vigila contrib/audit_register_residue.py, objeto JPegCompress).
#pragma runtime_checks("", off)
// 0x5C8EE0
__declspec(safebuffers) static void build_ycc_rgb_table_EaxEntry() {
    build_ycc_rgb_table(reinterpret_cast<DecompressInfo*>(EntryEaxCpp()));
}
#pragma runtime_checks("", restore)

// 0x5C90D0 (jdcolor.c)
// cdecl (cinfo, input_buf, input_row, output_buf, num_rows): copia plana de num_components planos a pixeles entrelazados.
static void __cdecl null_convert(DecompressInfo* cinfo, uint8*** input_buf, uint32 input_row, uint8** output_buf, int32 num_rows) {
    const int32  nc = cinfo->num_components;
    const uint32 num_cols = cinfo->output_width;
    while (--num_rows >= 0) {
        for (int32 ci = 0; ci < nc; ci++) {
            const uint8* inptr = input_buf[ci][input_row];
            uint8* outptr = output_buf[0] + ci;
            for (uint32 col = 0; col < num_cols; col++) {
                *outptr = *inptr++;
                outptr += nc;
            }
        }
        input_row++;
        output_buf++;
    }
}

// 0x5C9350 (jdcolor.c)
// cdecl (cinfo). Convertidores del EXE: gris 0x5C9150, gris->RGB 0x5C9180, YCC->RGB 0x5C8FB0, YCCK->CMYK 0x5C91E0, null 0x5C90D0;
// start_pass 0x5C9340. RGB sale en 4 componentes (out_color_components = 4), como en el EXE.
static void __cdecl jinit_color_deconverter(DecompressInfo* cinfo) {
    CconvertCtl* cconvert = static_cast<CconvertCtl*>(cinfo->mem->alloc_small(cinfo, kPoolImage, sizeof(CconvertCtl)));
    cinfo->cconvert = cconvert;
    cconvert->start_pass = LIBJPEG_CB(0x5C9340, "jdcolor", "start_pass_dcolor");
    switch (static_cast<uint32>(cinfo->jpeg_color_space - 1)) {
    case 0: // gris
        if (cinfo->num_components != 1) {
            ErrExit(cinfo, msg::BAD_J_COLORSPACE);
        }
        break;
    case 1: // RGB
    case 2: // YCbCr
        if (cinfo->num_components != 3) {
            ErrExit(cinfo, msg::BAD_J_COLORSPACE);
        }
        break;
    case 3: // CMYK
    case 4: // YCCK
        if (cinfo->num_components != 4) {
            ErrExit(cinfo, msg::BAD_J_COLORSPACE);
        }
        break;
    default:
        if (cinfo->num_components < 1) {
            ErrExit(cinfo, msg::BAD_J_COLORSPACE);
        }
        break;
    }
    switch (cinfo->out_color_space) {
    case kCsGRAYSCALE:
        cinfo->out_color_components = 1;
        if (cinfo->jpeg_color_space == kCsGRAYSCALE || cinfo->jpeg_color_space == kCsYCbCr) {
            cconvert->color_convert = LIBJPEG_CB(0x5C9150, "jdcolor", "grayscale_convert");
            for (int32 ci = 1; ci < cinfo->num_components; ci++) {
                cinfo->comp_info[ci].component_needed = 0;
            }
        } else {
            ErrExit(cinfo, msg::CONVERSION_NOTIMPL);
        }
        break;
    case kCsRGB:
        cinfo->out_color_components = 4;
        if (cinfo->jpeg_color_space == kCsYCbCr) {
            cconvert->color_convert = LIBJPEG_CB(0x5C8FB0, "jdcolor", "ycc_rgb_convert");
            build_ycc_rgb_table(cinfo);
        } else if (cinfo->jpeg_color_space == kCsGRAYSCALE) {
            cconvert->color_convert = LIBJPEG_CB(0x5C9180, "jdcolor", "gray_rgb_convert");
        } else {
            ErrExit(cinfo, msg::CONVERSION_NOTIMPL);
        }
        break;
    case kCsCMYK:
        cinfo->out_color_components = 4;
        if (cinfo->jpeg_color_space == kCsYCCK) {
            cconvert->color_convert = LIBJPEG_CB(0x5C91E0, "jdcolor", "ycck_cmyk_convert");
            build_ycc_rgb_table(cinfo);
        } else if (cinfo->jpeg_color_space == kCsCMYK) {
            cconvert->color_convert = LIBJPEG_CB(static_cast<uintptr_t>(reinterpret_cast<uintptr_t>(&null_convert)), "jdcolor", "null_convert") /* 0x5C90D0 */;
        } else {
            ErrExit(cinfo, msg::CONVERSION_NOTIMPL);
        }
        break;
    default:
        if (cinfo->out_color_space == cinfo->jpeg_color_space) {
            cinfo->out_color_components = cinfo->num_components;
            cconvert->color_convert = LIBJPEG_CB(static_cast<uintptr_t>(reinterpret_cast<uintptr_t>(&null_convert)), "jdcolor", "null_convert") /* 0x5C90D0 */;
        } else {
            ErrExit(cinfo, msg::CONVERSION_NOTIMPL);
        }
        break;
    }
    if (cinfo->quantize_colors) {
        cinfo->output_components = 1;
    } else {
        cinfo->output_components = cinfo->out_color_components;
    }
}

// 0x5C9710 (jddctmgr.c)
// cdecl (cinfo). Por componente: tabla de multiplicadores de 256 bytes a cero y cur_method = -1. start_pass 0x5C9510 (EXE).
static void __cdecl jinit_inverse_dct(DecompressInfo* cinfo) {
    IdctCtl* idct = static_cast<IdctCtl*>(cinfo->mem->alloc_small(cinfo, kPoolImage, sizeof(IdctCtl)));
    cinfo->idct = idct;
    idct->start_pass = LIBJPEG_CB(0x5C9510, "jddctmgr", "start_pass");
    ComponentInfo* compptr = cinfo->comp_info;
    for (int32 ci = 0; ci < cinfo->num_components; ci++, compptr++) {
        compptr->dct_table = cinfo->mem->alloc_small(cinfo, kPoolImage, 256);
        std::memset(compptr->dct_table, 0, 256);
        idct->cur_method[ci] = -1;
    }
}

// Registro de los hooks del lote 232A, sin validar.
static void InjectHooks232A() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("ExeLibJpeg");

    RH_ScopedGlobalInstall(jpeg_add_quant_table, 0x5C6BF0);
    RH_ScopedGlobalInstall(jpeg_set_colorspace, 0x5C7130);
    RH_ScopedGlobalInstall(jpeg_default_colorspace, 0x5C7420);
    RH_ScopedGlobalInstall(jpeg_set_defaults, 0x5C7590);
    RH_ScopedGlobalInstall(jinit_c_prep_controller, 0x5C7920);
    RH_ScopedGlobalInstall(expand_right_edge, 0x5C79F0);
    RH_ScopedGlobalInstall(jinit_downsampler, 0x5C7DF0);
    RH_ScopedGlobalInstall(jpeg_read_scanlines, 0x5C8690);
    RH_ScopedGlobalInstall(jpeg_start_decompress, 0x5C87E0);
    RH_ScopedGlobalInstall(term_destination, 0x5C88F0);
    RH_ScopedGlobalInstall(jinit_d_coef_controller, 0x5C8E20);
    RH_ScopedNamedGlobalInstall(build_ycc_rgb_table_EaxEntry, "build_ycc_rgb_table", 0x5C8EE0); // argumento en EAX
    RH_ScopedGlobalInstall(null_convert, 0x5C90D0);
    RH_ScopedGlobalInstall(jinit_color_deconverter, 0x5C9350);
    RH_ScopedGlobalInstall(jinit_inverse_dct, 0x5C9710);
}

// Entradas publicas para LibJpegCompressEncode.cpp y LibJpegDecompress.cpp (ver LibJpegExports.h): las implementaciones de este archivo son `static`.
void ExportJinitCPrepController(void* cinfo, uint32 needFull) { jinit_c_prep_controller(static_cast<CompressInfo*>(cinfo), static_cast<uint8>(needFull)); }          // 0x5C7920
void ExportJinitDownsampler(void* cinfo) { jinit_downsampler(static_cast<CompressInfo*>(cinfo)); }                                                                  // 0x5C7DF0
void ExportJinitDCoefController(void* cinfo, uint32 needFull) { jinit_d_coef_controller(static_cast<DecompressInfo*>(cinfo), static_cast<uint8>(needFull)); }        // 0x5C8E20
void ExportJinitColorDeconverter(void* cinfo) { jinit_color_deconverter(static_cast<DecompressInfo*>(cinfo)); }                                                    // 0x5C9350
void ExportJinitInverseDct(void* cinfo) { jinit_inverse_dct(static_cast<DecompressInfo*>(cinfo)); }                                                                // 0x5C9710

} // namespace ExeLibJpeg

using ExeLibJpeg::InjectHooks232A;

void ExeLibJpegInjectHooks232A() {
    InjectHooks232A();
}

// Entradas publicas para LibJpegCompressEncode.cpp y LibJpegDecompress.cpp (ver LibJpegExports.h).
void LibJpegExportJinitCPrepController(void* cinfo, uint32 needFull) { ExeLibJpeg::ExportJinitCPrepController(cinfo, needFull); }
void LibJpegExportJinitDownsampler(void* cinfo) { ExeLibJpeg::ExportJinitDownsampler(cinfo); }
void LibJpegExportJinitDCoefController(void* cinfo, uint32 needFull) { ExeLibJpeg::ExportJinitDCoefController(cinfo, needFull); }
void LibJpegExportJinitColorDeconverter(void* cinfo) { ExeLibJpeg::ExportJinitColorDeconverter(cinfo); }
void LibJpegExportJinitInverseDct(void* cinfo) { ExeLibJpeg::ExportJinitInverseDct(cinfo); }
