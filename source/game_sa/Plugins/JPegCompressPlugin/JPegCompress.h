#pragma once

#include "RenderWare.h"
#include "MenuManager.h"

struct jpeg_destination_mgr;
struct jpeg_source_mgr;

namespace JPegPlugin {
void InjectHooks();
}

/**
 * Comprime una captura de cámara usando el destino libjpeg suministrado.
 *
 * @addr  0x5D0470
 */
void JPegCompressScreen(RwCamera* camera, struct jpeg_destination_mgr& dst);

/**
 * Captura la cámara, la comprime como JPEG y escribe el stream al archivo indicado.
 *
 * @addr  0x5D0820
 */
void JPegCompressScreenToFile(RwCamera* camera, const char* path);

/**
 * Variante de compresión destinada a un buffer proporcionado por el llamador.
 *
 * @addr  0x5D0740
 */
void JPegCompressScreenToBuffer(RwCamera* cam, uint8** buffer, size_t& bufferSizeInOut);

/**
 * Decodifica una fuente JPEG sobre un RwRaster; ruta heredada de la galería de PS2.
 *
 * @addr  0x5D05F0
 */
bool JPegDecompressToRaster(RwRaster* raster, struct jpeg_source_mgr& src);

/**
 * Usa el buffer de galería como fuente JPEG y vuelca el resultado al raster indicado.
 *
 * @addr  0x5D05F0
 */
bool JPegDecompressToVramFromBuffer(RwRaster* raster, bool enable);
