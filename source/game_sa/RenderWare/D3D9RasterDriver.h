#pragma once

/**
 * Rutinas del controlador de rasters D3D9 de RenderWare dentro del EXE (zona 0x4CB530..0x4CC170): lista de rasters con recursos D3D
 * vivos, destruccion de rasters y consultas de formatos a IDirect3D9. El listado las atribuia a CWeaponModelInfo por cercania de
 * direcciones; no pertenecen a ninguna clase del juego.
 */
class RwD3D9RasterDriver {
public:
    static void InjectHooks();

    // Cuerpos reconstruidos en un espacio de nombres anonimo de D3D9RasterDriver.cpp, expuestos para que el resto del C++ no llame al EXE por direccion.
    static void  RasterListPushCpp(RwRaster* raster);                              // 0x4CB530
    static int32 RasterCreateCpp(void* device, RwRaster* raster, uint32 flags);   // 0x4CCE60
    static int32 RasterDestroyCpp(void* device, RwRaster* raster, int32 flags);   // 0x4CBB00
    static int32 CheckDepthStencilSurfaceFormatCpp(uint32 format);                // 0x4CBD80
    static int32 CheckTextureFormatCpp(uint32 format);                            // 0x4CBDE0
    static int32 CheckCubeTextureFormatCpp(uint32 format);                        // 0x4CBEE0
    static int32 CheckRenderTargetTextureFormatCpp(uint32 format);                // 0x4CBE20
    static int32 CheckDepthStencilTextureFormatCpp(uint32 format);                // 0x4CBE80
    static int32 CheckRenderTargetCubeFormatCpp(uint32 format);                   // 0x4CBF20
    static int32 CheckTextureFormatUsage400Cpp(uint32 format);                    // 0x4CBF80
    static int32 CheckCubeFormatUsage400Cpp(uint32 format);                       // 0x4CC020
};
