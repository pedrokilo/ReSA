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
};
