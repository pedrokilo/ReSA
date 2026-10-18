// ---- rwplcore.cpp
namespace {
// 0x8019B0: RwFreeListCreateAndPreallocateSpace real (el simbolo publico 0x801B70 es un JMP a esta direccion); sin reconstruir.
RwFreeList* FreeListCreateAndPreallocateSpaceExe(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint) {
    return ((RwFreeList*(__cdecl *)(RwInt32, RwInt32, RwInt32, RwInt32, RwFreeList*, RwUInt32))0x8019B0)(entrySize, entriesPerBlock, alignment, numBlocksToPreallocate, inPlaceSpaceForFreeListStruct, hint);
}
} // namespace
RwInt32 _rwPluginRegistryAddPlugin(RwPluginRegistry* reg, RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x8084A0)(reg, size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 _rwPluginRegistryAddPluginStream(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x8088E0)(reg, pluginID, readCB, writeCB, getSizeCB);
}

// 0x801980: RwFreeListCreate de la compilacion sin RWDEBUG (4 argumentos de pila; la version con fileCreate/lineCreate es solo de depuracion):
// reserva con un bloque preasignado y sin espacio propio para la estructura.
RwFreeList* RwFreeListCreate(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwUInt32 hint) {
    return FreeListCreateAndPreallocateSpaceExe(entrySize, entriesPerBlock, alignment, 1, nullptr, hint);
}

// 0x801B70: salto directo a 0x8019B0 con los mismos seis argumentos.
RwFreeList* RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize, RwInt32 entriesPerBlock, RwInt32 alignment, RwInt32 numBlocksToPreallocate, RwFreeList* inPlaceSpaceForFreeListStruct, RwUInt32 hint) {
    return FreeListCreateAndPreallocateSpaceExe(entrySize, entriesPerBlock, alignment, numBlocksToPreallocate, inPlaceSpaceForFreeListStruct, hint);
}

// ---- rwcore.cpp
namespace {
auto& TextureModuleGlobalsOffset  = StaticRef<RwInt32>(0xC97B4C);
auto& RasterModuleGlobalsOffset   = StaticRef<RwInt32>(0xC980D8);
auto& TextureTKList               = StaticRef<RwPluginRegistry>(0x8E23CC);
auto& TexDictTKList               = StaticRef<RwPluginRegistry>(0x8E23E4);
auto& RasterTKList                = StaticRef<RwPluginRegistry>(0x8E2518);
auto& CurrentDepthStencilSurface  = StaticRef<void*>(0xC9808C);
auto& D3D9Device                  = StaticRef<void*>(0xC97C28);

struct TextureGlobalsLayout {   // solo los campos que usan estas funciones (batextur.c de RW 3.6)
    uint8 pad0[0x1C];
    RwBool mipmapping;          // +0x1C
    RwBool autoMipmapping;      // +0x20
    uint8 pad1[0x08];
    RwTextureCallBackMipmapGeneration mipmapGenerationCallBack; // +0x2C
};
static_assert(offsetof(TextureGlobalsLayout, mipmapping) == 0x1C && offsetof(TextureGlobalsLayout, autoMipmapping) == 0x20
           && offsetof(TextureGlobalsLayout, mipmapGenerationCallBack) == 0x2C);

struct RasterGlobalsLayout {    // pila de contextos de RwRasterPushContext/PopContext (bardevice/baraster.c)
    RwRaster* rasterStack[10];  // +0x00
    RwInt32   rasterSP;         // +0x28
};
static_assert(offsetof(RasterGlobalsLayout, rasterSP) == 0x28);

struct D3D9DynamicVertexBuffer { // nodo de la lista 0xC97B8C (0x14 bytes)
    uint32 size;                 // +0x00
    uint32 inUse;                // +0x04 (RwD3D9DynamicVertexBufferDestroy lo pone a 0)
    void*  vertexBuffer;         // +0x08 IDirect3DVertexBuffer9*
    uint32 used;                 // +0x0C (RwD3D9DynamicVertexBufferDestroy lo pone a 0)
    D3D9DynamicVertexBuffer* next; // +0x10
};
static_assert(sizeof(D3D9DynamicVertexBuffer) == 0x14 && offsetof(D3D9DynamicVertexBuffer, next) == 0x10);
auto& DynamicVertexBufferList = StaticRef<D3D9DynamicVertexBuffer*>(0xC97B8C);

TextureGlobalsLayout& TextureGlobals() {
    return *reinterpret_cast<TextureGlobalsLayout*>(reinterpret_cast<uint8*>(RwEngineInstance) + TextureModuleGlobalsOffset);
}
RasterGlobalsLayout& RasterGlobals() {
    return *reinterpret_cast<RasterGlobalsLayout*>(reinterpret_cast<uint8*>(RwEngineInstance) + RasterModuleGlobalsOffset);
}

// Llamada COM stdcall por la tabla virtual (this primero); la tabla se lee en el momento de la llamada.
template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}
constexpr size_t kSlotRelease = 2;                      // IUnknown::Release
constexpr size_t kSlotVertexBufferUnlock = 12;          // IDirect3DVertexBuffer9::Unlock (+0x30)
constexpr size_t kSlotSetDepthStencilSurface = 39;      // IDirect3DDevice9::SetDepthStencilSurface (+0x9C)
constexpr size_t kSlotCreatePixelShader = 106;          // IDirect3DDevice9::CreatePixelShader (+0x1A8)
} // namespace
// 0x7F3550: RwTextureGlobals::mipmapping
RwBool RwTextureGetMipmapping() {
    return TextureGlobals().mipmapping;
}

// 0x7F3580: RwTextureGlobals::autoMipmapping
RwBool RwTextureGetAutoMipmapping() {
    return TextureGlobals().autoMipmapping;
}

// 0x7F3BB0: _rwPluginRegistryAddPlugin(&textureTKList, ...)
RwInt32 RwTextureRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&TextureTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x7F3C10: _rwPluginRegistryAddPlugin(&texDictTKList, ...)
RwInt32 RwTexDictionaryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&TexDictTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x7F3CD0: llama al callback de generacion de mipmaps del motor (sin comprobar que exista) y devuelve TRUE si no devuelve NULL.
RwBool RwTextureRasterGenerateMipmaps(RwRaster* raster, RwImage* image) {
    return TextureGlobals().mipmapGenerationCallBack(raster, image) != nullptr;
}

// 0x7F5AE0: marca como libre (inUse = used = 0) el primer nodo de la lista cuyo bufer es `vertexBuffer`; no libera nada ni toca el resto.
void RwD3D9DynamicVertexBufferDestroy(void* vertexBuffer) {
    for (auto* node = DynamicVertexBufferList; node != nullptr; node = node->next) {
        if (node->vertexBuffer == vertexBuffer) {
            node->inUse = 0;
            node->used = 0;
            return;
        }
    }
}

// 0x7F5C90: IDirect3DVertexBuffer9::Unlock; TRUE si el HRESULT no es negativo.
RwBool RwD3D9DynamicVertexBufferUnlock(void* vertexBuffer) {
    const HRESULT result = ComCall<kSlotVertexBufferUnlock, HRESULT>(vertexBuffer);
    return result >= 0;
}

// 0x7F5EF0: si la superficie de profundidad actual cambia, la anota ANTES de llamar a _RwD3DDevice->SetDepthStencilSurface y no la
// vuelve a escribir despues (aunque la llamada la modifique).
void _rwD3D9SetDepthStencilSurface(void* surface) {
    if (CurrentDepthStencilSurface == surface) {
        return;
    }
    void* const device = D3D9Device;
    CurrentDepthStencilSurface = surface;
    ComCall<kSlotSetDepthStencilSurface, HRESULT>(device, surface);
}

// 0x7F5F60: salto directo a 0x4C9AB0 (registro del plugin de raster D3D9, sin reconstruir; cuerpo del EXE)
RwBool _rwDeviceRegisterPlugin() {
    return ((RwBool(__cdecl *)(void))0x4C9AB0)();
}

// 0x7FAC90: IDirect3DVertexShader9::Release (sin comprobar NULL)
void RwD3D9DeleteVertexShader(void* shader) {
    ComCall<kSlotRelease, ULONG>(shader);
}

// 0x7FACC0: _RwD3DDevice->CreatePixelShader(function, shader); TRUE si el HRESULT no es negativo.
RwBool RwD3D9CreatePixelShader(const RwUInt32* function, void** shader) {
    const HRESULT result = ComCall<kSlotCreatePixelShader, HRESULT>(D3D9Device, function, shader);
    return result >= 0;
}

// 0x7FACF0: IDirect3DPixelShader9::Release (sin comprobar NULL)
void RwD3D9DeletePixelShader(void* shader) {
    ComCall<kSlotRelease, ULONG>(shader);
}

// 0x7FAE60: cima de la pila de contextos de rasters (rasterStack[rasterSP], sin comprobar el rango de rasterSP).
RwRaster* RwRasterGetCurrentContext() {
    auto& globals = RasterGlobals();
    return *(globals.rasterStack + globals.rasterSP);
}

// 0x7FB0B0: _rwPluginRegistryAddPlugin(&rasterTKList, ...)
RwInt32 RwRasterRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&RasterTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x7FB0E0: stdFunc[rwSTANDARDRASTERLOCKPALETTE](&paleta, raster, lockMode). El EXE reutiliza como variable de salida la ranura del
// argumento `lockMode`: si la funcion estandar devuelve TRUE sin escribir la salida, el resultado es el propio `lockMode`.
RwUInt8* RwRasterLockPalette(RwRaster* raster, RwInt32 lockMode) {
    RwUInt8* palette = reinterpret_cast<RwUInt8*>(static_cast<uintptr_t>(static_cast<uint32>(lockMode)));
    if (!RwEngineInstance->stdFunc[rwSTANDARDRASTERLOCKPALETTE](&palette, raster, lockMode)) {
        return nullptr;
    }
    return palette;
}

// 0x803740: stdFunc[rwSTANDARDRGBTOPIXEL](&pixel, rgbIn, rasterFormat). El EXE reutiliza como salida la ranura del argumento
// `rasterFormat`: si la funcion estandar no escribe, el resultado es el propio `rasterFormat`.
RwUInt32 RwRGBAToPixel(RwRGBA* rgbIn, RwInt32 rasterFormat) {
    RwUInt32 pixel = static_cast<RwUInt32>(rasterFormat);
    RwEngineInstance->stdFunc[rwSTANDARDRGBTOPIXEL](&pixel, rgbIn, rasterFormat);
    return pixel;
}

// 0x804550: _rwPluginRegistryAddPluginStream(&textureTKList, ...)
RwInt32 RwTextureRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return _rwPluginRegistryAddPluginStream(&TextureTKList, pluginID, readCB, writeCB, getSizeCB);
}

// 0x804F40: si el objeto tiene marco (object.parent), lo saca de la lista de objetos del marco (sin borrar `parent`).
void _rwObjectHasFrameReleaseFrame(void *object) {
    auto* const hasFrame = static_cast<RwObjectHasFrame*>(object);
    if (hasFrame->object.parent == nullptr) {
        return;
    }
    rwLinkListRemoveLLLink(&hasFrame->lFrame);
}

