#pragma once

/**
 * Manejadores de raster de la tabla de funciones estandar del dispositivo D3D9 de RenderWare (rwSTANDARD*, RwEngineInstance->stdFunc; la rellena _rwD3D9DeviceSystemStandards 0x7F72E0)
 * y sus ayudantes con ABI usercall, en el EXE entre 0x4CA4E0 y 0x4CD820. Todos los manejadores son cdecl (salida, entrada/salida, valor). Los llama el motor por puntero de la tabla, que ahora
 * guarda estas funciones C++; los cuatro ayudantes usercall (0x4CAD90, 0x4CAEA0, 0x4CAAE0 y 0x4CA5A0) solo los llamaban estos manejadores y pasan a ser funciones C++ normales (sin enganche:
 * su ABI usa registros, y el EXE ya no tiene ningun llamador). El listado no los atribuia a ninguna clase del juego.
 */
class D3D9RasterStandards {
public:
    static void InjectHooks();

    // Manejadores de la tabla (indice rwSTANDARD* entre parentesis).
    static RwBool SetTextureRaster(void* out, void* in, RwInt32 unused);                      // 0x4CBD40 (rwSTANDARDTEXTURESETRASTER, 8)
    static RwBool SetRasterContext(void* unused, RwRaster* raster, RwInt32 unused2);          // 0x4CB520 (rwSTANDARDSETRASTERCONTEXT, 0xB)
    static RwBool RasterSubRaster(RwRaster* sub, RwRaster* parent, RwInt32 unused);           // 0x4CBD50 (rwSTANDARDRASTERSUBRASTER, 0xC)
    static RwBool RasterClearRect(void* unused, RwInt32* rect, uint32 color);                 // 0x4CB4C0 (rwSTANDARDRASTERCLEARRECT, 0xD)
    static RwBool RasterClear(void* unused, void* unused2, uint32 color);                     // 0x4CB4E0 (rwSTANDARDRASTERCLEAR, 0xE)
    static RwBool RasterRender(RwRaster* raster, RwInt32* position, RwInt32 unused);          // 0x4CAE40 (rwSTANDARDRASTERRENDER, 0x11)
    static RwBool RasterRenderScaled(RwRaster* raster, RwInt32* rect, RwInt32 unused);        // 0x4CAE80 (rwSTANDARDRASTERRENDERSCALED, 0x12)
    static RwBool RasterRenderFast(RwRaster* raster, RwInt32* position, RwInt32 unused);      // 0x4CAE60 (rwSTANDARDRASTERRENDERFAST, 0x13)
    static RwBool RasterLockPalette(RwUInt8** paletteOut, RwRaster* raster, RwInt32 lockMode); // 0x4CA4E0 (rwSTANDARDRASTERLOCKPALETTE, 0x17)
    static RwBool NativeTextureRead(RwStream* stream, RwTexture** textureOut, RwInt32 size);  // 0x4CD820 (rwSTANDARDNATIVETEXTUREREAD, 0x1A)
    static RwBool NativeTextureWrite(RwStream* stream, RwTexture* texture, RwInt32 size);     // 0x4CD4D0 (rwSTANDARDNATIVETEXTUREWRITE, 0x1B)
    static RwBool RasterGetMipLevels(RwInt32* levelsOut, RwRaster* raster, RwInt32 unused);   // 0x4CBCB0 (rwSTANDARDRASTERGETMIPLEVELS, 0x1C)

    // Ayudantes usercall del EXE (sin enganche), ahora funciones normales.
    static RwBool RasterBlit(RwRaster* source, const RwInt32* position, bool scaled, bool useBlend); // 0x4CAD90 (eax = origen, edx = posicion, ecx = escalado, pila = mezcla)
    static RwBool RasterFill(const RwInt32* rect, uint32 color);                                     // 0x4CAEA0 (esi = rectangulo, pila = color)
    static RwBool CopyBetweenSurfaces(RwRaster* source, const RwInt32* position, bool scaled);       // 0x4CAAE0 (eax = origen, pila = posicion y escalado)
    static void   RenderTexturedQuad(RwRaster* source, bool scaled, const RwInt32* position, bool useBlend); // 0x4CA5A0 (esi = origen, ebx = escalado, pila = posicion y mezcla)
};
