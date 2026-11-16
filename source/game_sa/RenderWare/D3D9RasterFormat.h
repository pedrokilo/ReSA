#pragma once

/**
 * Funciones del controlador D3D9 de rasters de RenderWare (0x4CC1D0..0x4CD360, en el EXE entre los gestores de recursos de video).
 * El listado las atribuia a CWeaponModelInfo; no pertenecen a ninguna clase del juego. Lote 225A (sin validar).
 */
class D3D9RasterFormat {
public:
    static void InjectHooks();

    static uint32    FindCorrectRasterFormat(uint32 type, uint32 flags);                                 // 0x4CC1D0
    static RwBool    SetRasterFormat(RwRaster* raster, uint32 flags);                                    // 0x4CC580
    static RwBool    VideoMemoryRasterListRestore();                                                     // 0x4CC970
    static RwRaster* CreateRasterFromD3DFormat(RwInt32 width, RwInt32 height, RwInt32 d3dFormat, RwUInt32 flags); // 0x4CD050
    static void      ConvertRasterToDisplayFormat(RwRaster* raster);                                     // 0x4CD250
    static RwBool    GetRasterMemorySize(uint32* outSize, RwRaster** inRaster);                          // 0x4CD360

    // Lote R10A (sin validar): descompresores de bloques DXT (BC1, BC2/DXT3 y BC3/DXT5) a pixeles de 32 bits (R en el byte bajo, A en el alto) que usa la lectura de texturas nativas D3D9 (0x82xxxx). El listado las atribuia a RpMatFX.
    static int32     DecompressDxt1(int32 width, int32 height, uint32* dst, const uint8* src);                   // 0x81E720
    static void      DecompressDxt3(int32 width, int32 height, uint32* dst, const uint8* src);                   // 0x81EA70
    static void      DecompressDxt5(int32 width, int32 height, uint32* dst, const uint8* src);                   // 0x81EDC0
    static void      DecodeDxt5AlphaBlock(uint32* dst, const uint8* src, int32 stridePixels, uint32 keepMask);   // 0x81F0C0
};
