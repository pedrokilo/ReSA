// RwD3D9RasterStreamReadDDS (0x81F360): lectura de un raster desde un flujo con una textura DDS (lote S30A33A). Cuerpo en C++ puro que sustituye a la envoltura por direccion de rwcore.cpp.
// Desensamblado: reversing/FnS30A33A/dis_dds_n.txt (0x81F360..0x820A08). Las funciones del EXE que aun no tienen cuerpo C++ (0x4CBEE0, comprobacion de formato de textura de cubo) se llaman por direccion.
#include "StdInc.h"

#include "rwcore.h"
#include "RenderWare/D3D9RasterFormat.h"
#include "RenderWare/D3D9RasterDriver.h"

// 0x7FED70 (_rwD3D9CheckValidTextureSize, definida en rwcore.cpp; cdecl, (ancho*, alto*, formato)): ajusta el tamano de la textura a las capacidades del dispositivo.
void _rwD3D9CheckValidTextureSize(RwInt32* width, RwInt32* height, RwUInt32 format);

namespace {
// Llamada COM stdcall por la tabla virtual (this primero); la tabla se lee en el momento de la llamada (copia de la de rwcore.cpp).
template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}
constexpr size_t kSlotDdsDeviceCreateTexture     = 23; // IDirect3DDevice9::CreateTexture (+0x5C)
constexpr size_t kSlotDdsDeviceCreateCubeTexture = 25; // IDirect3DDevice9::CreateCubeTexture (+0x64)
constexpr size_t kSlotDdsTextureGetLevelDesc     = 17; // IDirect3DTexture9 / IDirect3DCubeTexture9::GetLevelDesc (+0x44)

constexpr uint32 kDdsMagic   = 0x20534444; // 'DDS '
constexpr uint32 kDdsFourCcDxt1 = 0x31545844;
constexpr uint32 kDdsFourCcDxt2 = 0x32545844;
constexpr uint32 kDdsFourCcDxt3 = 0x33545844;
constexpr uint32 kDdsFourCcDxt4 = 0x34545844;
constexpr uint32 kDdsFourCcDxt5 = 0x35545844;

// Formato de pixel del encabezado DDS (DDS_PIXELFORMAT, 0x20 bytes).
struct DdsReadPixelFormat {
    uint32 size;
    uint32 flags;       // 4 = FourCC, 0x40 = RGB, 0x41 = RGB con alfa; otros: luminancia, alfa...
    uint32 fourCC;
    uint32 rgbBitCount;
    uint32 rMask;
    uint32 gMask;
    uint32 bMask;
    uint32 aMask;
};

// Encabezado DDS (DDS_HEADER, 0x7C bytes). En el EXE vive en [esp+0x24..] tras el prologo.
struct DdsReadHeader {
    uint32             size;
    uint32             flags;             // & 0x1007 obligatorio; 0x20000 = hay contador de mipmaps; 0x80000 = tamano lineal
    uint32             height;
    uint32             width;
    uint32             pitchOrLinearSize;
    uint32             depth;
    uint32             mipMapCount;
    uint32             reserved1[11];
    DdsReadPixelFormat pf;
    uint32             caps;              // & 8 = complejo
    uint32             caps2;             // & 0xFE00 = mapa de cubo y caras
    uint32             caps3;
    uint32             caps4;
    uint32             reserved2;
};
static_assert(sizeof(DdsReadHeader) == 0x7C && offsetof(DdsReadHeader, pf) == 0x48 && offsetof(DdsReadHeader, caps2) == 0x6C);

// Extension D3D9 del raster (plugin 0x40C, 0x24 bytes) en raster + [0xB4E9E0].
struct DdsReadRasterExt {
    void*  texture;        // +0x00
    void*  palette;        // +0x04
    uint8  alpha;          // +0x08
    uint8  cubeFace;       // +0x09: nibble bajo = cubo, alto = cara
    uint8  mipFlags;       // +0x0A: nibble bajo = generacion automatica, alto = comprimida
    uint8  lockedMipLevel; // +0x0B
    void*  lockedSurface;  // +0x0C
    int32  lockedPitch;    // +0x10
    void*  lockedBits;     // +0x14
    uint32 d3dFormat;      // +0x18
    void*  swapChain;      // +0x1C
    void*  window;         // +0x20
};
static_assert(sizeof(DdsReadRasterExt) == 0x24 && offsetof(DdsReadRasterExt, d3dFormat) == 0x18);

DdsReadRasterExt& DdsExt(RwRaster* raster) {
    return *reinterpret_cast<DdsReadRasterExt*>(reinterpret_cast<uint8*>(raster) + StaticRef<RwInt32>(0xB4E9E0));
}

// Tabla de formatos por D3DFORMAT (0xB4E7E0, 4 bytes por entrada, 0x80 entradas): +0 alfa, +1 profundidad en bits, +3 bits de formato RW.
// EXE: el original indexa sin comprobar el limite; aqui un indice fuera de la tabla da ceros (en el EXE leia fuera de la tabla).
constexpr uint32 kDdsFormatTableEntries = 0x80;
uint8 DdsFormatTableByte(uint32 format, uint32 offset) {
    if (format >= kDdsFormatTableEntries) {
        return 0;
    }
    return *reinterpret_cast<const uint8*>(0xB4E7E0u + format * 4u + offset);
}

// Comprobaciones de formato (cdecl, un argumento de pila, resultado 0/1): 0x4CBDE0 = textura normal (D3D9RasterDriver.cpp);
// 0x4CBEE0 = textura de cubo (RwD3D9RasterDriver::CheckCubeTextureFormatCpp, codigo vivo CAM).
bool DdsCheckTextureFormat(uint32 format) {
    return RwD3D9RasterDriver::CheckTextureFormatCpp(format) != 0;
}
bool DdsCheckCubeTextureFormat(uint32 format) {
    return RwD3D9RasterDriver::CheckCubeTextureFormatCpp(format) != 0;
}

// Estado compartido de la lectura (en el EXE son los locales de pila de la funcion).
struct DdsReadContext {
    RwStream*    stream;
    DdsReadHeader hdr;
    uint32       mipFlag; // (flags >> 2) & 0x8000: bandera de mipmaps del raster
    bool         isCube;  // (caps & 8) y (caps2 & 0xFE00)
};

// Niveles que se piden a D3D: 1 si no hay contador de mipmaps (flags & 0x20000); con cubo ademas hace falta (0xC9BF44 & 0x30000), si no 1.
uint32 DdsLevelsForTexture(const DdsReadContext& c) {
    if (c.isCube) {
        if ((StaticRef<uint32>(0xC9BF44) & 0x30000u) == 0 || (c.hdr.flags & 0x20000u) == 0) {
            return 1;
        }
        return c.hdr.mipMapCount;
    }
    return (c.hdr.flags & 0x20000u) ? c.hdr.mipMapCount : 1u;
}

RwRaster* DdsReadBody(DdsReadContext& c, RwRaster* raster);
RwRaster* DdsReadFallback(DdsReadContext& c);

// Bucle de conversion fila a fila de la rama de imagen: la fila de origen y = origen + y * pasoOrigen, el destino es la fila y de la imagen (pixeles + stride * y); `conv(origen, destino)` escribe 4 bytes RGBA.
template<typename Conv>
void DdsConvertImage(const uint8* source, uint32 sourceStride, uint32 bytesPerPixel, RwImage* image, uint32 width, uint32 height, Conv conv) {
    for (uint32 y = 0; y < height; ++y) {
        const uint8* src = source + y * sourceStride;
        uint8* dst = image->cpPixels + static_cast<uint32>(image->stride) * y;
        for (uint32 x = 0; x < width; ++x) {
            conv(src, dst);
            src += bytesPerPixel;
            dst += 4;
        }
    }
}

// 0x81FD6B..0x81FEE0 (DXT1: busca bloques con transparencia): bloquea cada nivel para leer (modo 2) y revisa los bloques con color0 <= color1 (modo de 3 colores) buscando algun indice 3 en las filas/columnas
// visibles del bloque. EXE: usa siempre el ancho y alto del raster superior (nivel 0) para todos los niveles, de modo que en los niveles inferiores lee mas bloques de los que hay; se mantiene tal cual.
bool DdsDxt1HasAlpha(RwRaster* raster) {
    const RwInt32 numLevels = RwRasterGetNumLevels(raster);
    for (RwInt32 level = 0; level < numLevels; ++level) {
        const uint8* const pixels = RwRasterLock(raster, static_cast<RwUInt8>(level), 2);
        const RwInt32 width = raster->width;
        const RwInt32 height = raster->height;
        const RwInt32 blocksPerRow = (width < 4) ? 1 : ((width + ((width >> 31) & 3)) >> 2);
        const RwInt32 columns = (width < 4) ? width : 4;
        const RwInt32 rows = (height < 4) ? height : 4;
        const RwInt32 blockRows = (height < 4) ? 1 : ((height + ((height >> 31) & 3)) >> 2);
        uint32 count = static_cast<uint32>(blocksPerRow * blockRows);
        const uint8* block = pixels;
        while (count != 0) {
            const uint16 color0 = *reinterpret_cast<const uint16*>(block);
            const uint16 color1 = *reinterpret_cast<const uint16*>(block + 2);
            if (!(color0 > color1)) {
                for (RwInt32 row = 0; row < rows; ++row) {
                    uint32 bits = block[4 + row];
                    for (RwInt32 column = 0; column < columns; ++column) {
                        if ((bits & 3u) == 3u) {
                            RwRasterUnlock(raster);
                            return true;
                        }
                        bits >>= 2;
                    }
                }
            }
            block += 8;
            --count;
        }
        RwRasterUnlock(raster);
    }
    return false;
}

// 0x81FD50..0x81FFCF: ultimos ajustes del raster segun el formato (alfa, profundidad y formato RW). Devuelve el raster.
RwRaster* DdsFinishRaster(DdsReadContext& c, RwRaster* raster) {
    DdsReadRasterExt& ext = DdsExt(raster);
    if (c.hdr.pf.flags == 4) {
        const uint32 format = ext.d3dFormat;
        if (format == kDdsFourCcDxt1) {
            const bool hasAlpha = DdsDxt1HasAlpha(raster);
            ext.alpha = hasAlpha ? 1 : 0;
            if (!hasAlpha) {
                raster->cFormat = static_cast<RwUInt8>((raster->cFormat & 0xF2u) | 2u);
                return raster;
            }
            raster->depth = 0x10;
            raster->cFormat = static_cast<RwUInt8>((raster->cFormat & 0xF1u) | 1u);
            return raster;
        }
        if (format == kDdsFourCcDxt2 || format == kDdsFourCcDxt3 || format == kDdsFourCcDxt4 || format == kDdsFourCcDxt5) {
            ext.alpha = 1;
            raster->depth = 0x10;
            raster->cFormat = static_cast<RwUInt8>((raster->cFormat & 0xF3u) | 3u);
            return raster;
        }
        // Resto de formatos D3D numericos: alfa, profundidad y formato RW de la tabla 0xB4E7E0.
        ext.alpha = DdsFormatTableByte(format, 0);
        raster->depth = DdsFormatTableByte(format, 1);
        const uint32 low = raster->cFormat & 0xF0u;
        raster->cFormat = static_cast<RwUInt8>(DdsFormatTableByte(ext.d3dFormat, 3) | low);
        return raster;
    }
    if ((c.hdr.pf.flags & 1u) == 0 && c.hdr.pf.aMask == 0) {
        ext.alpha = 0;
        return raster;
    }
    ext.alpha = 1;
    return raster;
}

// 0x81F543..0x81FD4C: lee los datos de todas las caras y niveles al raster ya creado y termina con DdsFinishRaster. Cada cara del cubo se elige de caps2 (+X..-Z en el nibble alto del byte +9 de la extension);
// los niveles que el dispositivo no tiene se saltan en el flujo. Si la lectura falla devuelve NULL sin desbloquear ni destruir (como el EXE).
RwRaster* DdsReadBody(DdsReadContext& c, RwRaster* raster) {
    DdsReadRasterExt& ext = DdsExt(raster);
    const RwInt32 numLevels = RwRasterGetNumLevels(raster);
    const RwInt32 levelsToRead = c.mipFlag ? static_cast<RwInt32>(c.hdr.mipMapCount) : 1;
    uint32 caps2 = c.hdr.caps2;
    for (;;) {
        if (c.isCube) {
            if (caps2 & 0x600u) {
                ext.cubeFace = static_cast<uint8>(ext.cubeFace & 0x0Fu);
                caps2 &= ~0x600u;
            } else if (caps2 & 0xA00u) {
                ext.cubeFace = static_cast<uint8>((ext.cubeFace & 0x0Fu) | 0x10u);
                caps2 &= ~0xA00u;
            } else if (caps2 & 0x1200u) {
                ext.cubeFace = static_cast<uint8>((ext.cubeFace & 0x0Fu) | 0x20u);
                caps2 &= ~0x1200u;
            } else if (caps2 & 0x2200u) {
                ext.cubeFace = static_cast<uint8>((ext.cubeFace & 0x0Fu) | 0x30u);
                caps2 &= ~0x2200u;
            } else if (caps2 & 0x4200u) {
                ext.cubeFace = static_cast<uint8>((ext.cubeFace & 0x0Fu) | 0x40u);
                caps2 &= ~0x4200u;
            } else if (caps2 & 0x8200u) {
                ext.cubeFace = static_cast<uint8>((ext.cubeFace & 0x0Fu) | 0x50u);
                caps2 = 0;
            } else {
                break; // ninguna cara mas: 0x81FD50
            }
        }
        for (RwInt32 level = 0; level < levelsToRead; ++level) {
            if (level >= numLevels) {
                // Nivel que no existe en el raster: se salta en el flujo el tamano que ocupa (ancho y alto del nivel, minimo 1).
                const uint32 shift = static_cast<uint32>(level) & 0x1Fu; // sar con cl enmascarado a 5 bits
                RwInt32 w = raster->width >> shift;
                RwInt32 h = raster->height >> shift;
                if (w == 0) {
                    w = 1;
                }
                if (h == 0) {
                    h = 1;
                }
                uint32 skip;
                if ((ext.mipFlags & 0xF0u) == 0) {
                    skip = static_cast<uint32>((raster->depth >> 3) * h * w);
                } else {
                    const uint32 blockShift = ((ext.d3dFormat != kDdsFourCcDxt1) ? 1u : 0u) + 3u;
                    uint32 blocksW = static_cast<uint32>(w) >> 2;
                    uint32 blocksH = static_cast<uint32>(h) >> 2;
                    if (blocksW == 0) {
                        blocksW = 1;
                    }
                    if (blocksH == 0) {
                        blocksH = 1;
                    }
                    skip = (blocksH * blocksW) << blockShift;
                }
                RwStreamSkip(c.stream, skip);
                continue;
            }
            RwUInt8* const pixels = RwRasterLock(raster, static_cast<RwUInt8>(level), 1);
            D3DSURFACE_DESC desc;
            ComCall<kSlotDdsTextureGetLevelDesc, int32>(ext.texture, static_cast<uint32>(level), &desc);
            uint32 size;
            const uint32 fourCC = c.hdr.pf.fourCC;
            if (c.hdr.pf.flags == 4 && (fourCC == kDdsFourCcDxt1 || fourCC == kDdsFourCcDxt2 || fourCC == kDdsFourCcDxt3 || fourCC == kDdsFourCcDxt4 || fourCC == kDdsFourCcDxt5)) {
                size = (desc.Height >> 2) * static_cast<uint32>(ext.lockedPitch);
            } else {
                size = static_cast<uint32>(ext.lockedPitch) * desc.Height;
            }
            if (RwStreamRead(c.stream, pixels, size) != size) {
                return nullptr; // EXE: sin RwRasterUnlock ni RwRasterDestroy (el raster queda bloqueado y perdido)
            }
            RwRasterUnlock(raster);
        }
        if (!(c.isCube && caps2 != 0)) {
            break;
        }
    }
    return DdsFinishRaster(c, raster);
}

// Crea el raster con el formato D3D dado (no cubo): ajusta el tamano al dispositivo (CheckValidTextureSize con mipmap | 4); si cambia pasa a la rama de imagen (0x81FFD8); si no, CreateRasterFromD3DFormat (0x4CD050) y lee.
RwRaster* DdsCreateStandard(DdsReadContext& c, uint32 d3dFormat) {
    RwInt32 width = static_cast<RwInt32>(c.hdr.width);
    RwInt32 height = static_cast<RwInt32>(c.hdr.height);
    _rwD3D9CheckValidTextureSize(&width, &height, c.mipFlag | 4u);
    if (static_cast<uint32>(width) != c.hdr.width || static_cast<uint32>(height) != c.hdr.height) {
        return DdsReadFallback(c);
    }
    RwRaster* const raster = D3D9RasterFormat::CreateRasterFromD3DFormat(static_cast<RwInt32>(c.hdr.width), static_cast<RwInt32>(c.hdr.height), static_cast<RwInt32>(d3dFormat), c.mipFlag | 4u);
    if (!raster) {
        return nullptr;
    }
    return DdsReadBody(c, raster);
}

// Rama de cubo con formato RGB (0x81F5E2 y 0x81F7A4): formatBits = bits de formato RW (0x600/0x200/0x100/0x300). Si el tamano no es valido para el dispositivo devuelve NULL (no hay rama de imagen para cubos).
// EXE: no escribe el D3DFORMAT en la extension (+0x18); solo marca el nibble bajo de +9 y crea la textura de cubo con el formato dado.
RwRaster* DdsCreateCube(DdsReadContext& c, uint32 d3dFormat, uint32 formatBits) {
    const uint32 flags = formatBits | c.mipFlag;
    RwInt32 width = static_cast<RwInt32>(c.hdr.width);
    RwInt32 height = static_cast<RwInt32>(c.hdr.height);
    _rwD3D9CheckValidTextureSize(&width, &height, flags | 4u);
    if (static_cast<uint32>(width) != c.hdr.width) {
        return nullptr;
    }
    if (static_cast<uint32>(height) != c.hdr.height) {
        return nullptr;
    }
    RwRaster* const raster = RwRasterCreate(static_cast<RwInt32>(c.hdr.width), static_cast<RwInt32>(c.hdr.height), 0, static_cast<RwInt32>(flags | 0x84u));
    if (!raster) {
        return nullptr;
    }
    raster->cFlags = static_cast<RwUInt8>(raster->cFlags & 0x7Fu);
    DdsReadRasterExt& ext = DdsExt(raster);
    ext.cubeFace = static_cast<uint8>((ext.cubeFace & 0xF1u) | 1u);
    const uint32 levels = DdsLevelsForTexture(c);
    const int32 hr = ComCall<kSlotDdsDeviceCreateCubeTexture, int32>(StaticRef<void*>(0xC97C28), static_cast<uint32>(raster->width), levels, 0u, d3dFormat, 1u, &ext.texture, static_cast<void*>(nullptr));
    if (hr < 0) {
        return nullptr; // EXE: el raster queda sin destruir
    }
    return DdsReadBody(c, raster);
}

// Rama de imagen (0x81FFD0): el formato no lo admite el dispositivo o el tamano no es valido. Lee el bloque de datos del flujo completo, lo convierte a una RwImage de 32 bits (descomprimiendo DXT o convirtiendo a mano)
// y con ella crea el raster (RwRasterCreate + RwRasterSetFromImage); si la imagen no tiene el tamano del raster la remuestrea (RwImageCreateResample).
RwRaster* DdsReadFallback(DdsReadContext& c) {
    const DdsReadHeader& hdr = c.hdr;
    const uint32 width = hdr.width;
    const uint32 height = hdr.height;
    uint32 pitch = hdr.pitchOrLinearSize; // [esp+0x34]; el EXE lo reescribe al calcularlo
    uint32 rowStride = 0;                 // [esp+0x18]: bytes por fila del origen
    bool rowStrideKnown = false;
    uint32 readSize;                      // esi
    uint32 formatBits = 0;                // [esp+0x10]: bits de formato RW del raster nuevo (0 si ninguna rama los fija)

    if (hdr.pf.flags == 4) {
        readSize = pitch;
        if (pitch == 0) {
            const uint32 fourCC = hdr.pf.fourCC;
            if (fourCC >= kDdsFourCcDxt1 && fourCC <= kDdsFourCcDxt5) {
                uint32 shift;
                if (fourCC == kDdsFourCcDxt1) {
                    shift = 3;
                } else if (fourCC == kDdsFourCcDxt2 || fourCC == kDdsFourCcDxt3 || fourCC == kDdsFourCcDxt4 || fourCC == kDdsFourCcDxt5) {
                    shift = 4;
                } else {
                    shift = static_cast<uint8>(c.mipFlag); // inalcanzable; el EXE usaba [esp+0x20] (mipFlag), cuyo byte bajo es 0
                }
                uint32 blocksW = width >> 2;
                if (blocksW == 0) {
                    blocksW = 1;
                }
                uint32 blocksH = height >> 2;
                if (blocksH == 0) {
                    blocksH = 1;
                }
                readSize = (blocksH * blocksW) << shift;
            } else {
                if (fourCC >= kDdsFormatTableEntries) {
                    return nullptr; // EXE: indexaba la tabla 0xB4E7E0 fuera de limite (acceso a memoria arbitraria)
                }
                rowStride = (static_cast<uint32>(DdsFormatTableByte(fourCC, 1)) >> 3) * width;
                rowStrideKnown = true;
                readSize = rowStride * height;
            }
        }
    } else if (hdr.flags & 0x80000u) {
        // Tamano lineal dado: el total es dwPitchOrLinearSize; el paso por fila se deduce del ancho y la profundidad.
        readSize = pitch;
        const uint32 bits = hdr.pf.rgbBitCount;
        if (bits == 8) {
            rowStride = width;
        } else if (bits == 0x10) {
            rowStride = width + width;
        } else if (bits == 0x18) {
            rowStride = width * 3;
        } else if (bits == 0x20) {
            rowStride = width * 4;
        } else if (bits == 0x40) {
            rowStride = width << 3;
        } else if (hdr.pf.rMask == 0xFF0000) {
            rowStride = width * 4;
        } else {
            rowStride = width + width;
        }
        rowStrideKnown = true;
    } else {
        if (pitch == 0) {
            const uint32 bits = hdr.pf.rgbBitCount;
            if (bits == 8) {
                pitch = width;
            } else if (bits == 0x10) {
                pitch = width + width;
            } else if (bits == 0x18) {
                pitch = width * 3;
            } else if (bits == 0x20) {
                pitch = width * 4;
            } else if (bits == 0x40) {
                pitch = width << 3;
            } else if (hdr.pf.rMask == 0xFF0000) {
                pitch = width * 4;
            } else {
                pitch = width + width;
            }
        }
        rowStride = pitch;
        rowStrideKnown = true;
        readSize = pitch * height;
    }
    // EXE: con FourCC y dwPitchOrLinearSize != 0 el paso por fila ([esp+0x18]) queda sin asignar y la conversion de formatos numericos lo lee como basura; aqui se usa el paso natural de la tabla.
    if (!rowStrideKnown) {
        rowStride = (static_cast<uint32>(DdsFormatTableByte(hdr.pf.fourCC, 1)) >> 3) * width;
    }

    uint8* const buffer = static_cast<uint8*>(RwEngineInstance->memoryFuncs.rwmalloc(readSize, 0x10018));
    if (!buffer) {
        return nullptr; // EXE: no comprobaba el resultado de RwMalloc
    }
    if (RwStreamRead(c.stream, buffer, readSize) != readSize) {
        RwEngineInstance->memoryFuncs.rwfree(buffer);
        return nullptr;
    }
    RwImage* image = RwImageCreate(static_cast<RwInt32>(width), static_cast<RwInt32>(height), 0x20);
    if (!image) {
        RwEngineInstance->memoryFuncs.rwfree(buffer);
        return nullptr;
    }
    RwImageAllocatePixels(image);

    if (hdr.pf.flags == 4) {
        const uint32 fourCC = hdr.pf.fourCC;
        uint32* const dst = reinterpret_cast<uint32*>(image->cpPixels);
        if (fourCC == kDdsFourCcDxt2 || fourCC == kDdsFourCcDxt3) {
            formatBits = (DdsCheckTextureFormat(0x1A) ? 0x200u : 0u) + 0x100u;
            D3D9RasterFormat::DecompressDxt3(static_cast<int32>(width), static_cast<int32>(height), dst, buffer);
        } else if (fourCC == kDdsFourCcDxt1) {
            const int32 result = D3D9RasterFormat::DecompressDxt1(static_cast<int32>(width), static_cast<int32>(height), dst, buffer);
            formatBits = (result != 0) ? 0x100u : 0x200u; // neg/sbb/xor al,al/add 0x200
        } else if (fourCC == 0x24) {
            // D3DFMT_A16B16G16R16: se toma el byte alto de cada canal.
            if (DdsCheckTextureFormat(0x15)) {
                formatBits = 0x500;
            } else {
                formatBits = (DdsCheckTextureFormat(0x1A) ? 0x200u : 0u) + 0x100u;
            }
            DdsConvertImage(buffer, rowStride, 8, image, width, height, [](const uint8* s, uint8* d) {
                d[0] = s[1];
                d[1] = s[3];
                d[2] = s[5];
                d[3] = s[7];
            });
        } else if (fourCC == kDdsFourCcDxt4 || fourCC == kDdsFourCcDxt5) {
            formatBits = (DdsCheckTextureFormat(0x1A) ? 0x200u : 0u) + 0x100u;
            D3D9RasterFormat::DecompressDxt5(static_cast<int32>(width), static_cast<int32>(height), dst, buffer);
        }
    } else if (hdr.pf.flags == 0x40) {
        if (hdr.pf.rgbBitCount == 0x18) {
            formatBits = (DdsCheckTextureFormat(0x16) ? 0x400u : 0u) + 0x200u;
            DdsConvertImage(buffer, rowStride, 3, image, width, height, [](const uint8* s, uint8* d) {
                d[0] = s[2];
                d[1] = s[1];
                d[2] = s[0];
                d[3] = 0xFF;
            });
        } else if (hdr.pf.rMask == 0xFF0000) {
            formatBits = (DdsCheckTextureFormat(0x16) ? 0x400u : 0u) + 0x200u;
            DdsConvertImage(buffer, rowStride, 4, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = *reinterpret_cast<const uint32*>(s);
                d[3] = 0xFF;
                d[2] = static_cast<uint8>(v);
                d[0] = static_cast<uint8>(v >> 16);
                d[1] = static_cast<uint8>(v >> 8);
            });
        } else if (hdr.pf.rMask == 0xF800) {
            formatBits = 0x200;
            DdsConvertImage(buffer, rowStride, 2, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = *reinterpret_cast<const uint16*>(s);
                d[3] = 0xFF;
                d[0] = static_cast<uint8>((v >> 8) & 0xF8u);
                d[1] = static_cast<uint8>((v >> 3) & 0xFCu);
                d[2] = static_cast<uint8>(v << 3);
            });
        } else if (hdr.pf.rMask == 0xE0) {
            formatBits = 0x200;
            DdsConvertImage(buffer, rowStride, 1, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = s[0];
                d[3] = 0xFF;
                d[0] = static_cast<uint8>(v & 0xE0u);
                d[1] = static_cast<uint8>((v & 0xFCu) << 3);
                d[2] = static_cast<uint8>(v << 6);
            });
        }
    } else if (hdr.pf.flags == 0x41) {
        const uint32 rMask = hdr.pf.rMask;
        // Formato RW elegido segun lo que admite el dispositivo: 8888 (0x500), si no 4444 (0x300) o 1555 (0x100).
        const auto bestWithAlpha = []() -> uint32 {
            if (DdsCheckTextureFormat(0x15)) {
                return 0x500;
            }
            return (DdsCheckTextureFormat(0x1A) ? 0x200u : 0u) + 0x100u;
        };
        if (rMask == 0xFF0000) {
            formatBits = bestWithAlpha();
            DdsConvertImage(buffer, rowStride, 4, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = *reinterpret_cast<const uint32*>(s);
                d[2] = static_cast<uint8>(v);
                d[0] = static_cast<uint8>(v >> 16);
                d[1] = static_cast<uint8>(v >> 8);
                d[3] = static_cast<uint8>(v >> 24);
            });
        } else if (rMask == 0x7C00) {
            formatBits = 0x100;
            DdsConvertImage(buffer, rowStride, 2, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = *reinterpret_cast<const uint16*>(s);
                d[3] = (v & 0x8000u) ? 0xFF : 0x00;
                d[0] = static_cast<uint8>((v >> 7) & 0xF8u);
                d[1] = static_cast<uint8>((v >> 2) & 0xF8u);
                d[2] = static_cast<uint8>(v << 3);
            });
        } else if (rMask == 0xF00) {
            formatBits = (DdsCheckTextureFormat(0x1A) ? 0x200u : 0u) + 0x100u;
            DdsConvertImage(buffer, rowStride, 2, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = *reinterpret_cast<const uint16*>(s);
                d[3] = static_cast<uint8>((v >> 8) & 0xF0u);
                d[0] = static_cast<uint8>((v >> 4) & 0xF0u);
                d[1] = static_cast<uint8>(v & 0xF0u);
                d[2] = static_cast<uint8>(v << 4);
            });
        } else if (rMask == 0x3FF) {
            // EXE: la mascara roja 0x3FF (A2B10G10R10) se descodifica con los 8 bits altos de cada campo en el orden de A2R10G10B10 (d[0] = campo alto); se mantiene.
            formatBits = bestWithAlpha();
            DdsConvertImage(buffer, rowStride, 4, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = *reinterpret_cast<const uint32*>(s);
                d[0] = static_cast<uint8>(v >> 22);
                d[1] = static_cast<uint8>(v >> 12);
                d[2] = static_cast<uint8>(v >> 2);
                d[3] = static_cast<uint8>((v >> 30) * 0x55u);
            });
        } else if (rMask == 0xE0) {
            formatBits = 0x300;
            DdsConvertImage(buffer, rowStride, 2, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = *reinterpret_cast<const uint16*>(s);
                d[0] = static_cast<uint8>(v & 0xE0u);
                d[1] = static_cast<uint8>((v & 0xFCu) << 3);
                d[2] = static_cast<uint8>(v << 6);
                d[3] = static_cast<uint8>((v >> 8) & 0xFFu);
            });
        } else if (hdr.pf.bMask == 0x3FF) {
            formatBits = bestWithAlpha();
            DdsConvertImage(buffer, rowStride, 4, image, width, height, [](const uint8* s, uint8* d) {
                const uint32 v = *reinterpret_cast<const uint32*>(s);
                d[0] = static_cast<uint8>(v >> 2);
                d[1] = static_cast<uint8>(v >> 12);
                d[2] = static_cast<uint8>(v >> 22);
                d[3] = static_cast<uint8>((v >> 30) * 0x55u);
            });
        }
    }

    RwEngineInstance->memoryFuncs.rwfree(buffer);
    // Con mipmaps pide ademas la generacion automatica (0x1000 sobre el 0x8000).
    uint32 extraFlags = c.mipFlag;
    if (extraFlags != 0) {
        extraFlags |= 0x1000u;
    }
    const uint32 rasterFlags = (formatBits | extraFlags) | 4u;
    RwRaster* const raster = RwRasterCreate(static_cast<RwInt32>(width), static_cast<RwInt32>(height), 0, static_cast<RwInt32>(rasterFlags));
    if (!raster) {
        RwImageDestroy(image);
        return nullptr;
    }
    if (image->width != raster->width || image->height != raster->height) {
        RwImage* const resampled = RwImageCreateResample(image, raster->width, raster->height);
        RwImageDestroy(image);
        image = resampled;
        if (!image) {
            RwRasterDestroy(raster); // EXE: no comprobaba el resultado (RwRasterSetFromImage con NULL)
            return nullptr;
        }
    }
    RwRasterSetFromImage(raster, image);
    RwImageDestroy(image);
    return raster;
}
} // namespace

// 0x81F360 (cdecl, (flujo) -> RwRaster*; `ret`, 5,1 KB hasta 0x820A08): lee un archivo DDS del flujo. Cabecera 'DDS ' (4 bytes) y DDS_HEADER (0x7C); (flags & 0x1007) obligatorio; mipFlag = (flags >> 2) & 0x8000.
// Es mapa de cubo si (caps & 8) y (caps2 & 0xFE00). Segun pf.flags: 4 (FourCC; DXT1..DXT5 o D3DFORMAT numerico) -> textura normal o de cubo con ese formato si el dispositivo la admite (0x4CBDE0 / 0x4CBEE0) y si no rama de imagen;
// 0x40 y 0x41 (RGB y RGBA) -> formato D3D por las mascaras; otros (luminancia / alfa de 8 y 16 bits) -> L16, A8L8, L8, A8. Con formato admitido crea el raster (RwRasterCreate o 0x4CD050), lee las caras y niveles y ajusta
// alfa, profundidad y formato RW. Con formato no admitido (o tamano distinto del valido) lee el bloque, lo convierte a RwImage de 32 bits (DecompressDxt1/3/5 o conversion propia) y crea el raster con RwRasterSetFromImage.
// Devuelve NULL ante cualquier fallo.
RwRaster* RwD3D9RasterStreamReadDDS(RwStream* stream) {
    DdsReadContext c;
    c.stream = stream;
    uint32 magic = 0;
    if (RwStreamRead(stream, &magic, 4) != 4) {
        return nullptr;
    }
    if (magic != kDdsMagic) {
        return nullptr;
    }
    if (RwStreamRead(stream, &c.hdr, sizeof(c.hdr)) != sizeof(c.hdr)) {
        return nullptr;
    }
    const DdsReadHeader& hdr = c.hdr;
    if ((hdr.flags & 0x1007u) == 0) {
        return nullptr;
    }
    c.mipFlag = (hdr.flags >> 2) & 0x8000u;
    c.isCube = (hdr.caps & 8u) != 0 && (hdr.caps2 & 0xFE00u) != 0;

    if (hdr.pf.flags == 4) {
        const uint32 fourCC = hdr.pf.fourCC;
        bool unsupported;
        if (c.isCube) {
            if (!DdsCheckCubeTextureFormat(fourCC)) {
                return nullptr;
            }
            unsupported = false;
        } else {
            unsupported = !DdsCheckTextureFormat(fourCC);
        }
        if (unsupported) {
            return DdsReadFallback(c);
        }
        RwRaster* const raster = RwRasterCreate(static_cast<RwInt32>(hdr.width), static_cast<RwInt32>(hdr.height), 0, static_cast<RwInt32>(c.mipFlag | 0x84u));
        if (!raster) {
            return nullptr;
        }
        raster->cFlags = static_cast<RwUInt8>(raster->cFlags & 0x7Fu);
        DdsReadRasterExt& ext = DdsExt(raster);
        ext.d3dFormat = fourCC;
        // Formatos DXT1..DXT5 (comparacion con signo en el EXE): nibble alto de +0xA = 1 (comprimida).
        if (static_cast<int32>(fourCC) >= static_cast<int32>(kDdsFourCcDxt1) && static_cast<int32>(fourCC) <= static_cast<int32>(kDdsFourCcDxt5)) {
            ext.mipFlags = static_cast<uint8>((ext.mipFlags & 0x0Fu) | 0x10u);
        } else {
            ext.mipFlags = static_cast<uint8>(ext.mipFlags & 0x0Fu);
        }
        const uint32 levels = DdsLevelsForTexture(c);
        int32 hr;
        if (c.isCube) {
            ext.cubeFace = static_cast<uint8>((ext.cubeFace & 0xF1u) | 1u);
            hr = ComCall<kSlotDdsDeviceCreateCubeTexture, int32>(StaticRef<void*>(0xC97C28), static_cast<uint32>(raster->width), levels, 0u, ext.d3dFormat, 1u, &ext.texture, static_cast<void*>(nullptr));
        } else {
            hr = ComCall<kSlotDdsDeviceCreateTexture, int32>(StaticRef<void*>(0xC97C28), static_cast<uint32>(raster->width), static_cast<uint32>(raster->height), levels, 0u, ext.d3dFormat, 1u, &ext.texture, static_cast<void*>(nullptr));
        }
        if (hr < 0) {
            return nullptr; // EXE: el raster queda sin destruir
        }
        return DdsReadBody(c, raster);
    }

    if (hdr.pf.flags == 0x40) {
        const uint32 bits = hdr.pf.rgbBitCount;
        const uint32 rMask = hdr.pf.rMask;
        if (c.isCube) {
            if (bits == 0x18) {
                if (!DdsCheckCubeTextureFormat(0x14)) {
                    return nullptr;
                }
                return DdsCreateCube(c, 0x14, 0x600);
            }
            if (rMask == 0xFF0000) {
                if (!DdsCheckCubeTextureFormat(0x16)) {
                    return nullptr;
                }
                return DdsCreateCube(c, 0x16, 0x600);
            }
            if (rMask == 0xF800) {
                if (!DdsCheckCubeTextureFormat(0x17)) {
                    return nullptr;
                }
                return DdsCreateCube(c, 0x17, 0x200);
            }
            return nullptr;
        }
        uint32 format;
        if (bits == 0x18) {
            // EXE: aqui (y solo aqui en la rama no cubo) comprueba el formato R8G8B8 con 0x4CBEE0 (comprobacion de cubo) en vez de 0x4CBDE0.
            if (!DdsCheckCubeTextureFormat(0x14)) {
                return DdsReadFallback(c);
            }
            format = 0x14;
        } else if (rMask == 0xFF0000) {
            if (!DdsCheckTextureFormat(0x16)) {
                return DdsReadFallback(c);
            }
            format = 0x16;
        } else if (rMask == 0xF800) {
            format = 0x17; // sin comprobacion en el EXE
        } else if (rMask == 0xE0) {
            if (!DdsCheckTextureFormat(0x1B)) {
                return DdsReadFallback(c);
            }
            format = 0x1B;
        } else {
            return nullptr;
        }
        return DdsCreateStandard(c, format);
    }

    if (hdr.pf.flags == 0x41) {
        const uint32 rMask = hdr.pf.rMask;
        if (c.isCube) {
            if (rMask == 0xFF0000) {
                // EXE: con alfa y mascara 0xFF0000 usa X8R8G8B8 (0x16), no A8R8G8B8.
                if (!DdsCheckCubeTextureFormat(0x16)) {
                    return nullptr;
                }
                return DdsCreateCube(c, 0x16, 0x600);
            }
            if (rMask == 0x7C00) {
                if (!DdsCheckCubeTextureFormat(0x19)) {
                    return nullptr;
                }
                return DdsCreateCube(c, 0x19, 0x100);
            }
            if (rMask == 0xF00) {
                if (!DdsCheckCubeTextureFormat(0x1A)) {
                    return nullptr;
                }
                return DdsCreateCube(c, 0x1A, 0x300);
            }
            return nullptr;
        }
        uint32 format;
        if (rMask == 0xFF0000) {
            if (!DdsCheckTextureFormat(0x15)) {
                return DdsReadFallback(c);
            }
            format = 0x15;
        } else if (rMask == 0x7C00) {
            format = 0x19; // sin comprobacion en el EXE
        } else if (rMask == 0xF00) {
            if (!DdsCheckTextureFormat(0x1A)) {
                return DdsReadFallback(c);
            }
            format = 0x1A;
        } else if (rMask == 0x3FF) {
            if (!DdsCheckTextureFormat(0x23)) {
                return DdsReadFallback(c);
            }
            format = 0x23;
        } else if (hdr.pf.bMask == 0x3FF) {
            if (!DdsCheckTextureFormat(0x1F)) {
                return DdsReadFallback(c);
            }
            format = 0x1F;
        } else if (rMask == 0xE0) {
            if (!DdsCheckTextureFormat(0x1D)) {
                return DdsReadFallback(c);
            }
            format = 0x1D;
        } else {
            return nullptr;
        }
        return DdsCreateStandard(c, format);
    }

    // Resto de pf.flags: luminancia / alfa. Sin cubos y sin rama de imagen si el dispositivo no admite el formato (devuelve NULL).
    if (c.isCube) {
        return nullptr;
    }
    const uint32 bits = hdr.pf.rgbBitCount;
    uint32 format;
    if (bits == 0x10) {
        if (hdr.pf.rMask == 0xFFFF) {
            if (!DdsCheckTextureFormat(0x51)) {
                return nullptr;
            }
            format = 0x51; // D3DFMT_L16
        } else if (hdr.pf.rMask == 0xFF) {
            if (hdr.pf.aMask != 0xFF00) {
                return nullptr;
            }
            if (!DdsCheckTextureFormat(0x33)) {
                return nullptr;
            }
            format = 0x33; // D3DFMT_A8L8
        } else {
            return nullptr;
        }
    } else if (bits == 8) {
        if (hdr.pf.rMask == 0xFF) {
            if (!DdsCheckTextureFormat(0x32)) {
                return nullptr;
            }
            format = 0x32; // D3DFMT_L8
        } else if (hdr.pf.aMask == 0xFF) {
            if (!DdsCheckTextureFormat(0x1C)) {
                return nullptr;
            }
            format = 0x1C; // D3DFMT_A8
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
    return DdsCreateStandard(c, format);
}

// Lote S30A33A (sin validar): gancho de RwD3D9RasterStreamReadDDS.
void RwD3D9InjectHooksDDS() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(RwD3D9RasterStreamReadDDS, 0x81F360);
}
