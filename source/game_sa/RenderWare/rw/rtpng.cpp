#include "StdInc.h"

#include <csetjmp>

#include "rtpng.h"

// Lote LIB4A: RtPNGImageWrite (0x7CF600, 821 bytes) y RtPNGImageRead (0x7CF9B0, 1039 bytes) de RtPNG (Criterion/RenderWare) y sus cinco callbacks de libpng
// (0x7CF940, 0x7CF960, 0x7CF980, 0x7CF9A0, 0x7CFDE0), reconstruidos en C++ desde el desensamblado. Sin validar: usan el sistema de archivos y la memoria de RenderWare y no se
// pueden ejecutar fuera del juego (ver reversing/FnLIB4A/README.md). Desde el lote E1 (P4) llaman DIRECTAMENTE a libpng 1.0.12 enlazada (libs/), sin direcciones del EXE: las 48 funciones de libpng/zlib del EXE que quedaban vivas (0x7D26E0-0x7E77E0) ya no tienen llamador.
//
// setjmp: el EXE original llamaba a _setjmp3 de su CRT (0x825F7C) sobre png_ptr->jmpbuf (primer campo de png_struct, 64 bytes) y libpng hacia longjmp con el CRT del EXE. Con
// png_error enganchado a la biblioteca el longjmp lo hace el CRT de la DLL, asi que el setjmp tiene que ser tambien el de la DLL. Por eso este codigo solo se activa junto con
// GTASA_WITH_ZLIB_LIBPNG; sin la opcion las dos funciones siguen llamando al EXE. Las variables que se leen tras el longjmp son `volatile` (el EXE las guardaba en la pila).

#ifdef GTASA_WITH_ZLIB_LIBPNG
// Lote E1 (P4): RtPNG llama a libpng 1.0.12 ENLAZADA (libs/libpng-1.0.12 + zlib-1.1.3, GTASA_WITH_ZLIB_LIBPNG), ya no a las direcciones de la libpng del EXE. Los prototipos son
// los de png.h con punteros opacos (el mismo recurso que libpngread.cpp/libpngwrite.cpp: no se incluye png.h para no chocar con los nombres de este archivo).
extern "C" {
    void* png_create_write_struct_2(const char* ver, void* errorPtr, void* errorFn, void* warnFn, void* memPtr, void* mallocFn, void* freeFn);
    void* png_create_read_struct_2(const char* ver, void* errorPtr, void* errorFn, void* warnFn, void* memPtr, void* mallocFn, void* freeFn);
    void* png_create_info_struct(void* png);
    void png_set_write_fn(void* png, void* ioPtr, void* writeFn, void* flushFn);
    void png_set_read_fn(void* png, void* ioPtr, void* readFn);
    void* png_get_io_ptr(void* png);
    void png_set_IHDR(void* png, void* info, uint32 width, uint32 height, int32 bitDepth, int32 colorType, int32 interlace, int32 compression, int32 filter);
    void png_set_PLTE(void* png, void* info, void* palette, int32 numPalette);
    void png_set_tRNS(void* png, void* info, void* trans, int32 numTrans, void* transValues);
    void png_write_info(void* png, void* info);
    void png_set_packing(void* png);
    void png_write_image(void* png, void* rows);
    void png_write_end(void* png, void* info);
    void png_destroy_write_struct(void** png, void** info);
    void png_destroy_read_struct(void** png, void** info, void** endInfo);
    void png_error(void* png, const char* message);
    void png_read_info(void* png, void* info);
    uint32 png_get_IHDR(void* png, void* info, uint32* width, uint32* height, int32* bitDepth, int32* colorType, int32* interlace, int32* compression, int32* filter);
    void png_set_strip_16(void* png);
    void png_set_gray_1_2_4_to_8(void* png);
    void png_set_gray_to_rgb(void* png);
    void png_set_filler(void* png, uint32 filler, int32 loc);
    uint32 png_get_valid(void* png, void* info, uint32 flag);
    uint32 png_get_PLTE(void* png, void* info, uint8** palette, int32* numPalette);
    uint32 png_get_tRNS(void* png, void* info, uint8** trans, int32* numTrans, void** transValues);
    void png_read_image(void* png, void* rows);
    void png_read_end(void* png, void* info);
}

namespace {
using Png = void*;

constexpr const char* kPngVersion = "1.0.12";  // el EXE pasa 0x8DEDF4 (la cadena "1.0.12")
constexpr RwUInt32 kPngHint = 0x3010D;         // pista de memoria que pasa el EXE a RwMalloc
constexpr RwInt32 kRtPNGPluginID = 0x10D;      // RWERROR de RtPNGImageRead
constexpr uint32 PNG_INFO_PLTE = 0x08;
constexpr uint32 PNG_INFO_tRNS = 0x10;

// Callbacks de libpng (el EXE los pasa como punteros; sus direcciones originales tambien estan enganchadas a estos cuerpos)
void* __cdecl RtPngMalloc(Png /*png*/, uint32 size) { return RwMalloc(size, kPngHint); }                // 0x7CF940
void __cdecl RtPngFree(Png /*png*/, void* ptr) { RwFree(ptr); }                                         // 0x7CF960
void __cdecl RtPngWriteData(Png png, void* data, uint32 length) {                                       // 0x7CF980
    auto* const stream = static_cast<RwStream*>(png_get_io_ptr(png));
    if (stream) RwStreamWrite(stream, data, length);
    else png_error(png, nullptr);
}
void __cdecl RtPngFlush(Png /*png*/) {}                                                                 // 0x7CF9A0
void __cdecl RtPngReadData(Png png, void* data, uint32 length) {                                        // 0x7CFDE0
    auto* const stream = static_cast<RwStream*>(png_get_io_ptr(png));
    if (stream) RwStreamRead(stream, data, length);
    else png_error(png, nullptr);
}

// RWERROR((codigo)) del SDK: RwError { pluginID, errorCode = _rwerror(codigo) } + RwErrorSet (0x808820); _rwerror (0x8088D0) se llama por direccion.
void RaiseRtPNGError(RwInt32 code) {
    RwError error;
    error.pluginID  = kRtPNGPluginID;
    error.errorCode = _rwerror(code); // 0x8088D0
    RwErrorSet(&error);
}

// jmp_buf de png_struct: primer campo (64 bytes)
inline jmp_buf& PngJmpBuf(Png png) { return *reinterpret_cast<jmp_buf*>(png); }
} // namespace

// 0x7CF600 (821 bytes, sin validar): escribe `image` como PNG en el archivo `imageName`; devuelve la imagen (o NULL si falla). Profundidad 4/8: paleta (PLTE + tRNS con el alfa de
// cada entrada); 32: RGBA de 8 bits; otra: IHDR con profundidad 0 y tipo 0. Sin nombre o sin imagen devuelve `image` sin hacer nada. Los fallos tras abrir el flujo no liberan la
// paleta ni tRNS (el EXE tampoco).
RwImage* RtPNGImageWrite(RwImage* image, const RwChar* imageName) {
    if (!imageName || !image)
        return image;

    RwStream* volatile stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMWRITE, imageName);
    if (!stream)
        return nullptr;

    Png volatile png = png_create_write_struct_2(kPngVersion, nullptr, nullptr, nullptr, nullptr, reinterpret_cast<void*>(RtPngMalloc), reinterpret_cast<void*>(RtPngFree));
    if (!png) {
        RwStreamClose(stream, nullptr);
        return nullptr;
    }

    void* volatile info = png_create_info_struct(png);
    if (!info) {
        RwStreamClose(stream, nullptr);
        png_destroy_write_struct(const_cast<Png*>(&png), nullptr);
        return nullptr;
    }

    if (setjmp(PngJmpBuf(png))) {
        RwStreamClose(stream, nullptr);
        png_destroy_write_struct(const_cast<Png*>(&png), nullptr);
        return nullptr;
    }

    png_set_write_fn(png, stream, reinterpret_cast<void*>(RtPngWriteData), reinterpret_cast<void*>(RtPngFlush));

    const RwInt32 width = image->width;
    const RwInt32 height = image->height;
    const RwInt32 depth = image->depth;
    int32 bitDepth = 0;
    int32 colorType = 0;
    RwRGBA* palette = nullptr;
    switch (depth) {
    case 4:
        bitDepth = 4;
        colorType = 3;
        palette = image->palette;
        break;
    case 8:
        bitDepth = 8;
        colorType = 3;
        palette = image->palette;
        break;
    case 32:
        bitDepth = 8;
        colorType = 6;
        break;
    default:
        break;
    }
    png_set_IHDR(png, info, static_cast<uint32>(width), static_cast<uint32>(height), bitDepth, colorType, 0, 0, 0);

    uint8* pngPalette = nullptr;
    uint8* pngTrans = nullptr;
    if (colorType == 3) {
        const uint32 entries = 1u << depth;
        pngPalette = static_cast<uint8*>(RwMalloc(entries * 3, kPngHint));
        pngTrans = static_cast<uint8*>(RwMalloc(entries, kPngHint));
        for (uint32 i = 0; i < entries; i++) {
            pngPalette[i * 3 + 0] = palette[i].red;
            pngPalette[i * 3 + 1] = palette[i].green;
            pngPalette[i * 3 + 2] = palette[i].blue;
            pngTrans[i] = palette[i].alpha;
        }
        png_set_PLTE(png, info, pngPalette, static_cast<int32>(entries));
        png_set_tRNS(png, info, pngTrans, static_cast<int32>(entries), nullptr);
    }

    png_write_info(png, info);
    png_set_packing(png);

    auto** const rows = static_cast<uint8**>(RwMalloc(static_cast<RwUInt32>(height) << 2, kPngHint));
    if (!rows) {
        png_destroy_write_struct(const_cast<Png*>(&png), nullptr);
        RwStreamClose(stream, nullptr);
        return nullptr;
    }
    uint8* pixels = image->cpPixels;
    for (RwInt32 y = 0; y < height; y++) {
        rows[y] = pixels;
        pixels += image->stride;
    }

    png_write_image(png, rows);
    png_write_end(png, info);

    if (pngPalette) {
        RwFree(pngPalette);
        RwFree(pngTrans);
    }
    RwFree(rows);
    png_destroy_write_struct(const_cast<Png*>(&png), const_cast<void**>(&info));
    RwStreamClose(stream, nullptr);
    return image;
}

// 0x7CF9B0 (1039 bytes, sin validar): lee el PNG `imageName` en una RwImage nueva (profundidad 4/8 con paleta, o 32 bits). Gris -> 8 (con png_set_gray_1_2_4_to_8 si tiene menos de 8
// bits), gris+alfa -> 32 (png_set_gray_to_rgb), RGB -> 32 (png_set_filler 0xFF detras), RGBA -> 32, paleta -> 8 (o 4 con png_set_packing si tiene menos de 8 bits); otros tipos de color:
// profundidad 0. Los fallos de RwImageCreate/RwImageAllocatePixels no destruyen png_struct (el EXE tampoco). Sin nombre: RWERROR(0x80000016) y NULL.
RwImage* RtPNGImageRead(const RwChar* imageName) {
    if (!imageName) {
        RaiseRtPNGError(0x80000016);
        return nullptr;
    }

    RwStream* volatile stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, imageName);
    if (!stream)
        return nullptr;

    void* volatile rows = nullptr;
    Png volatile png = png_create_read_struct_2(kPngVersion, nullptr, nullptr, nullptr, nullptr, reinterpret_cast<void*>(RtPngMalloc), reinterpret_cast<void*>(RtPngFree));
    if (!png) {
        RwStreamClose(stream, nullptr);
        return nullptr;
    }

    void* volatile info = png_create_info_struct(png);
    if (!info) {
        RwStreamClose(stream, nullptr);
        png_destroy_read_struct(const_cast<Png*>(&png), nullptr, nullptr);
        return nullptr;
    }

    if (setjmp(PngJmpBuf(png))) {
        if (rows) {
            RwFree(rows);
            rows = nullptr;
        }
        RwStreamClose(stream, nullptr);
        png_destroy_read_struct(const_cast<Png*>(&png), const_cast<void**>(&info), nullptr);
        png = nullptr;
        info = nullptr;
        return nullptr;
    }

    png_set_read_fn(png, stream, reinterpret_cast<void*>(RtPngReadData));
    png_read_info(png, info);

    uint32 width = 0, height = 0;
    int32 bitDepth = 0, colorType = 0, interlace = 0;
    png_get_IHDR(png, info, &width, &height, &bitDepth, &colorType, &interlace, nullptr, nullptr);
    png_set_strip_16(png);

    int32 imageDepth = 0;
    switch (colorType) {
    case 0: // gris
        imageDepth = 8;
        if (bitDepth < 8)
            png_set_gray_1_2_4_to_8(png);
        break;
    case 2: // RGB
        imageDepth = 32;
        png_set_filler(png, 0xFF, 1 /* PNG_FILLER_AFTER */);
        break;
    case 3: // paleta
        imageDepth = 8;
        if (bitDepth < 8) {
            png_set_packing(png);
            imageDepth = 4;
        }
        break;
    case 4: // gris + alfa
        imageDepth = 32;
        png_set_gray_to_rgb(png);
        break;
    case 6: // RGBA
        imageDepth = 32;
        break;
    default: // 1, 5 y mayores que 6
        break;
    }

    RwImage* const image = RwImageCreate(static_cast<RwInt32>(width), static_cast<RwInt32>(height), imageDepth);
    if (!image) {
        RwStreamClose(stream, nullptr);
        return nullptr;
    }
    if (!RwImageAllocatePixels(image)) {
        RwImageDestroy(image);
        RwStreamClose(stream, nullptr);
        return nullptr;
    }

    uint8* pixels = image->cpPixels;
    const RwInt32 stride = image->stride;
    RwRGBA* const palette = image->palette;

    auto** const rowPointers = static_cast<uint8**>(RwMalloc(height << 2, kPngHint));
    rows = rowPointers;
    if (!rowPointers) {
        RwImageFreePixels(image);
        RwImageDestroy(image);
        RwStreamClose(stream, nullptr);
        return nullptr;
    }
    for (uint32 y = 0; y < height; y++) {
        rowPointers[y] = pixels;
        pixels += stride;
    }

    if ((imageDepth == 4 || imageDepth == 8) && palette) {
        const int32 entries = 1 << imageDepth;
        for (int32 i = 0; i < entries; i++) {
            palette[i].red = static_cast<RwUInt8>(i);
            palette[i].green = static_cast<RwUInt8>(i);
            palette[i].blue = static_cast<RwUInt8>(i);
            palette[i].alpha = 0xFF;
        }
        if (png_get_valid(png, info, PNG_INFO_PLTE)) {
            uint8* pngPalette = nullptr;
            int32 numPalette = 0;
            png_get_PLTE(png, info, &pngPalette, &numPalette);
            if (entries < numPalette)
                numPalette = entries;
            for (int32 i = 0; i < numPalette; i++) {
                palette[i].red = pngPalette[i * 3 + 0];
                palette[i].green = pngPalette[i * 3 + 1];
                palette[i].blue = pngPalette[i * 3 + 2];
            }
        }
        if (png_get_valid(png, info, PNG_INFO_tRNS)) {
            uint8* trans = nullptr;
            int32 numTrans = 0;
            void* transValues = nullptr;
            png_get_tRNS(png, info, &trans, &numTrans, &transValues);
            for (int32 i = 0; i < numTrans; i++)
                palette[i].alpha = trans[i];
        }
    }

    png_read_image(png, rowPointers);
    png_read_end(png, info);
    png_destroy_read_struct(const_cast<Png*>(&png), const_cast<void**>(&info), nullptr);
    png = nullptr;
    info = nullptr;
    RwFree(rowPointers);
    rows = nullptr;
    RwStreamClose(stream, nullptr);
    return image;
}

void RtPNGInjectHooks() {
    RH_ScopedNamespaceName("RtPNG");
    RH_ScopedCategory("Plugins");

    RH_ScopedGlobalInstall(RtPNGImageWrite, 0x7CF600);
    RH_ScopedGlobalInstall(RtPNGImageRead, 0x7CF9B0);
    RH_ScopedGlobalInstall(RtPngMalloc, 0x7CF940);
    RH_ScopedGlobalInstall(RtPngFree, 0x7CF960);
    RH_ScopedGlobalInstall(RtPngWriteData, 0x7CF980);
    RH_ScopedGlobalInstall(RtPngFlush, 0x7CF9A0);
    RH_ScopedGlobalInstall(RtPngReadData, 0x7CFDE0);
}
#else
// Sin GTASA_WITH_ZLIB_LIBPNG el EXE sigue ejecutando su RtPNG y su libpng.
RwImage* RtPNGImageWrite(RwImage* image, const RwChar* imageName) {
    return ((RwImage*(__cdecl*)(RwImage*, const RwChar*))0x7CF600)(image, imageName);
}

RwImage* RtPNGImageRead(const RwChar* imageName) {
    return ((RwImage*(__cdecl*)(const RwChar*))0x7CF9B0)(imageName);
}

void RtPNGInjectHooks() {}
#endif
