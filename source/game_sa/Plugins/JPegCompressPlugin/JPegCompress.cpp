/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: JPegPlugin
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "jpeglib.h"

static auto& g_ScreenshotFileBuf    = StaticRef<std::array<uint8, 204'800>>(0xBD0B78);
static auto& g_JpegDecodingToRaster = StaticRef<CRGBA*>(0xBD0160);

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
    RH_ScopedGlobalInstall(JPegDecompressToRaster, 0x5D05F0);
    RH_ScopedGlobalInstall(JPegDecompressToVramFromBuffer, 0x5D0320);
}

// 0x5D0470
/**
 * Captura la cámara a RwImage y alimenta libjpeg línea por línea en formato RGBA.
 * El destino se inyecta mediante jpeg_destination_mgr para reutilizar la misma
 * compresión tanto con archivo como con otros buffers.
 */
void JPegCompressScreen(RwCamera* camera, jpeg_destination_mgr& dst) {
    jpeg_error_mgr       jerr{};
    jpeg_compress_struct cinfo{
        .err = jpeg_std_error(&jerr)
    };
    jerr.error_exit = JPegErrorExit;

    const auto image = RsGrabScreen(camera);
    jpeg_CreateCompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));

    // It was JCS_RGB originally, but libjpeg errors with 'bogus colorspace'.
    cinfo.in_color_space = JCS_EXT_RGBA;
    jpeg_set_defaults(&cinfo);

    cinfo.image_width      = RwRasterGetWidth(image);
    cinfo.image_height     = RwRasterGetHeight(image);
    cinfo.dest             = &dst;
    cinfo.dct_method       = JDCT_FLOAT;
    cinfo.input_components = 4;
    jpeg_start_compress(&cinfo, true);

    for (auto i = 0u; i < cinfo.image_height; i++) {
        const auto* line = &RwImageGetPixels(image)[sizeof(RwRGBA) * i * cinfo.image_width];
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
 */
void JPegCompressScreenToFile(RwCamera* camera, const char* path) {
    static FILESTREAM g_ScreenshotFileHandle{};

    if (g_ScreenshotFileHandle = CFileMgr::OpenFile(path, "wb")) {
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
            // NOTSA: signed -> unsigned
            const auto totalProcessed = (uintptr)c->dest->next_output_byte - (uintptr)g_ScreenshotFileBuf.data();
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
        if (static_cast<size_t>(c->dest->next_output_byte + TotalProcessed - g_ScreenshotFileBuf.data()) < BufferSize) {
            std::memcpy(
                *Buffer + TotalProcessed,
                g_ScreenshotFileBuf.data(),
                g_ScreenshotFileBuf.size()
            );
            TotalProcessed += g_ScreenshotFileBuf.size();
        }
    };

    bufferSizeInOut = TotalProcessed;
}

// 0x5D05F0 -- PS2 leftover
// Decodifica scanlines JPEG y las convierte al formato de píxel del RwRaster bloqueado.
void JPegDecompressToRaster(RwRaster* raster, jpeg_source_mgr& src) {
    static auto& g_JpegScan = StaticRef<uint8*>(0xBD0170);

    jpeg_error_mgr         jerr{};
    jpeg_decompress_struct cinfo{
        .err = jpeg_std_error(&jerr)
    };
    jerr.error_exit = JPegErrorExit;

    jpeg_CreateDecompress(&cinfo, JPEG_LIB_VERSION, sizeof(cinfo));
    cinfo.src = &src;

    if (jpeg_read_header(&cinfo, true) == 1 && jpeg_start_decompress(&cinfo)) {
        cinfo.dct_method       = JDCT_FLOAT;
        g_JpegDecodingToRaster = reinterpret_cast<CRGBA*>(RwRasterLock(raster, 0, rwRASTERLOCKWRITE));

        bool success{ true };
        while (cinfo.output_scanline < cinfo.output_height) {
            auto* scanlines = g_JpegScan;
            if (jpeg_read_scanlines(&cinfo, &scanlines, 1) != 1) {
                success = false;
                break;
            }

            for (auto i = 0; i < 2'048; i += 4) {
                RwRGBA color{ g_JpegScan[i + 256], g_JpegScan[i + 257], g_JpegScan[i + 258], 255 };
                *g_JpegDecodingToRaster++ = RwRGBAToPixel(&color, RwRasterGetFormat(raster));
            }
        }

        if (success) {
            jpeg_finish_decompress(&cinfo);
        } else {
            NOTSA_LOG_ERR("Couldn't decode JPEG image.");
        }
        jpeg_destroy_decompress(&cinfo);
    }
    RwRasterUnlock(raster);
}

// 0x5D0320 -- PS2 leftover
// Adapta el buffer de galería del frontend como jpeg_source_mgr y reutiliza el
// decodificador común hacia RwRaster.
void JPegDecompressToVramFromBuffer(RwRaster* raster, bool enable) {
    static auto& g_ScreenshotFilePointer = StaticRef<size_t>(0xBD0B70);

    if (!enable) {
        return;
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
}
