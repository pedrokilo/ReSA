/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RwCore
 * Reconstrucción: COMPLETA — 29/29 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 29/29 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rwcore.h"

// Estado global del motor que leen o escriben las funciones reconstruidas del grupo g25d en este archivo (direcciones del EXE):
//   RwEngineInstance (0xC97B24, ver RenderWare.h)       puntero a RwGlobals; los modulos cuelgan de el con su `globalsOffset`
//   textureModule.globalsOffset  0xC97B4C                RwTextureGlobals: mipmapping +0x1C, autoMipmapping +0x20, callback de generacion +0x2C
//   rasterModule.globalsOffset   0xC980D8                RwRasterGlobals: pila de contextos +0x00 (10 entradas), puntero de pila +0x28
//   lista de buferes de vertices dinamicos D3D9 0xC97B8C (cabeza)
//   superficie de profundidad actual 0xC9808C ; dispositivo D3D9 (_RwD3DDevice) 0xC97C28
//   registros de plugins: textureTKList 0x8E23CC, texDictTKList 0x8E23E4, rasterTKList 0x8E2518
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

// --- Lote 223A (sin validar): plugin de raster del controlador D3D9 de RenderWare (0x4C9990..0x4CA5A0; el listado las atribuia a CWeaponModelInfo) ---
// La extension D3D9 del raster (plugin 0x40C, 0x24 bytes) cuelga en raster + _RwD3D9RasterExtOffset (0xB4E9E0); es la misma `_rwD3D9RasterExt` de WindowedMode.cpp. En las camaras, `texture` guarda la superficie.
// Los bytes +9 y +0xA llevan dos campos de cuatro bits cada uno (cube/face y automipmapgen/compressed, el primero en el nibble bajo).
namespace {
struct D3D9RasterExtLayout {
    void*  texture;        // +0x00
    void*  palette;        // +0x04
    uint8  alpha;          // +0x08
    uint8  cubeFace;       // +0x09: nibble bajo = cube, alto = face
    uint8  mipFlags;       // +0x0A: nibble bajo = automipmapgen, alto = compressed
    uint8  lockedMipLevel; // +0x0B
    void*  lockedSurface;  // +0x0C
    int32  lockedPitch;    // +0x10 (D3DLOCKED_RECT.Pitch)
    void*  lockedBits;     // +0x14 (D3DLOCKED_RECT.pBits)
    uint32 d3dFormat;      // +0x18
    void*  swapChain;      // +0x1C
    void*  window;         // +0x20
};
static_assert(sizeof(D3D9RasterExtLayout) == 0x24 && offsetof(D3D9RasterExtLayout, alpha) == 0x08 && offsetof(D3D9RasterExtLayout, lockedSurface) == 0x0C
           && offsetof(D3D9RasterExtLayout, lockedPitch) == 0x10 && offsetof(D3D9RasterExtLayout, lockedBits) == 0x14 && offsetof(D3D9RasterExtLayout, swapChain) == 0x1C);

struct D3D9FormatInfoLayout { // elemento de la tabla 0xB4E7E0 (indexada por D3DFORMAT, 4 bytes por entrada)
    uint8  alpha;             // +0
    uint8  depth;             // +1
    uint16 rwFormat;          // +2
};
static_assert(sizeof(D3D9FormatInfoLayout) == 4);

auto& D3D9RasterExtOffset = StaticRef<RwInt32>(0xB4E9E0);
auto& D3D9RenderSurface   = StaticRef<void*>(0xC97C30); // _RwD3D9RenderSurface

D3D9RasterExtLayout& D3D9RasterExt(const RwRaster* raster) {
    return *reinterpret_cast<D3D9RasterExtLayout*>(reinterpret_cast<uint8*>(const_cast<RwRaster*>(raster)) + D3D9RasterExtOffset);
}

// Ranuras de las tablas virtuales de D3D9 que usa este grupo (desplazamiento / 4).
constexpr size_t kSlotSurfaceGetDesc               = 12;  // IDirect3DSurface9::GetDesc (+0x30)
constexpr size_t kSlotSurfaceLockRect              = 13;  // IDirect3DSurface9::LockRect (+0x34)
constexpr size_t kSlotSurfaceUnlockRect            = 14;  // IDirect3DSurface9::UnlockRect (+0x38)
constexpr size_t kSlotTextureGetSurfaceLevel       = 18;  // IDirect3DTexture9::GetSurfaceLevel y IDirect3DCubeTexture9::GetCubeMapSurface (+0x48)
constexpr size_t kSlotDeviceUpdateSurface          = 30;  // IDirect3DDevice9::UpdateSurface (+0x78)
constexpr size_t kSlotDeviceGetRenderTargetData    = 32;  // IDirect3DDevice9::GetRenderTargetData (+0x80)
constexpr size_t kSlotDeviceCreateOffscreenSurface = 36;  // IDirect3DDevice9::CreateOffscreenPlainSurface (+0x90)
constexpr size_t kSlotDeviceSetPaletteEntries      = 71;  // IDirect3DDevice9::SetPaletteEntries (+0x11C)

struct D3D9Rect  { int32 left, top, right, bottom; };
struct D3D9Point { int32 x, y; };

// IDirect3DTexture9::GetSurfaceLevel o, si el nibble bajo del byte +9 de la extension (cubo) no es cero, IDirect3DCubeTexture9::GetCubeMapSurface con la cara del nibble alto.
int32 D3D9GetTextureSurface(void* texture, uint8 cubeFace, uint32 level, void** surface) {
    if ((cubeFace & 0x0Fu) != 0) {
        return ComCall<kSlotTextureGetSurfaceLevel, int32>(texture, static_cast<uint32>(cubeFace >> 4), level, surface);
    }
    return ComCall<kSlotTextureGetSurfaceLevel, int32>(texture, level, surface);
}

// 0x4C9990 (lote 223A, sin validar; el listado decia CWeaponModelInfo): destruye los recursos D3D9 de una cabecera de instancia de malla del pipeline D3D9 de RenderWare (RxD3D9ResEntryHeader; cdecl, un argumento de pila,
// `ret`, 202 bytes; llamadores: 0x763A80 y 0x7641C0, ambos dentro del bloque de instanciacion). Como el original, en este orden: (1) si el objeto asociado (+0xC) no es nulo y su primer byte no es 0xFF, y el byte +0x36 de
// alguno de los dos flujos (+0x36 y +0x46: `managed`) es cero, quita la geometria dinamica con 0x74E9E0 (con el propio objeto si su tipo es 8; con [objeto + 0x18] si es 1; con otro tipo no hace nada); (2) si el bufer de
// indices (+0x20) no es nulo, lo destruye con 0x730D30 y lo pone a cero; (3) si la declaracion de vertices (+0x4C) no es nula, la destruye con 0x7FAC10 y la pone a cero; (4) para los dos flujos de 0x10 bytes desde +0x28
// (bufer, desplazamiento, paso, banderas de 16 bits, `managed`, `libre`): con bufer no nulo, si `managed` (+0xE) es distinto de cero llama a 0x7F56A0(paso, [header + 0x54] * paso, bufer, desplazamiento); si no, y si el
// desplazamiento y `libre` (+0xF) son cero, llama a 0x7F5AE0(bufer); y pone el bufer a cero. Despues (con o sin bufer) ceros en desplazamiento, paso, banderas y los dos bytes.
__declspec(noinline) void __cdecl RxD3D9ResEntryHeaderDestroyExe(uint8* header) {
    uint8* const owner = *reinterpret_cast<uint8**>(header + 0x0C);
    if (owner != nullptr) {
        const uint8 ownerType = *owner;
        if (ownerType != 0xFF) {
            for (uint32 i = 0; i < 2; ++i) {
                if (header[0x36 + 0x10 * i] == 0) {
                    if (ownerType == 8) {
                        reinterpret_cast<void(__cdecl*)(void*)>(0x74E9E0)(owner);
                    } else if (ownerType == 1) {
                        reinterpret_cast<void(__cdecl*)(void*)>(0x74E9E0)(*reinterpret_cast<void**>(owner + 0x18));
                    }
                    break;
                }
            }
        }
    }
    if (void* const indexBuffer = *reinterpret_cast<void**>(header + 0x20)) {
        reinterpret_cast<void(__cdecl*)(void*)>(0x730D30)(indexBuffer);
        *reinterpret_cast<void**>(header + 0x20) = nullptr;
    }
    if (void* const declaration = *reinterpret_cast<void**>(header + 0x4C)) {
        reinterpret_cast<void(__cdecl*)(void*)>(0x7FAC10)(declaration);
        *reinterpret_cast<void**>(header + 0x4C) = nullptr;
    }
    for (uint32 i = 0; i < 2; ++i) {
        uint8* const stream = header + 0x28 + 0x10 * i;
        if (void* const vertexBuffer = *reinterpret_cast<void**>(stream)) {
            if (stream[0x0E] != 0) {
                const uint32 offset = *reinterpret_cast<uint32*>(stream + 4);
                const uint32 stride = *reinterpret_cast<uint32*>(stream + 8);
                reinterpret_cast<void(__cdecl*)(uint32, uint32, void*, uint32)>(0x7F56A0)(stride, *reinterpret_cast<uint32*>(header + 0x54) * stride, vertexBuffer, offset);
            } else if (*reinterpret_cast<uint32*>(stream + 4) == 0 && stream[0x0F] == 0) {
                reinterpret_cast<void(__cdecl*)(void*)>(0x7F5AE0)(vertexBuffer);
            }
            *reinterpret_cast<void**>(stream) = nullptr;
        }
        *reinterpret_cast<uint32*>(stream + 4) = 0;
        *reinterpret_cast<uint32*>(stream + 8) = 0;
        *reinterpret_cast<uint16*>(stream + 0x0C) = 0;
        stream[0x0E] = 0;
        stream[0x0F] = 0;
    }
}

// 0x4C9AB0 (lote 223A, sin validar; el listado decia CWeaponModelInfo): registro del plugin de raster D3D9 (_rwD3D9RasterPluginAttach; cdecl, sin argumentos, `ret`, 967 bytes; se llega por el salto 0x7F5F60). Como el
// original: desplazamiento = RwRasterRegisterPlugin(0x24, 0x40C, constructor 0x4C9A60, destructor 0x4C9A80, sin copia) (0x7FB0B0, con hook), guardado SIEMPRE en 0xB4E9E0; si es negativo devuelve 0. Si no: pone a
// cero 0x80 dwords desde 0xB4E7E0 (la tabla de formatos de pixel por D3DFORMAT, 4 bytes por entrada: alfa, profundidad y formato RW de 16 bits) y escribe las entradas de los formatos conocidos; devuelve 1. Las
// 138 escrituras (bytes y palabras, en el orden del EXE) salen del desensamblado con bl = 0, al = 1, cl = 0x20, dl = 0x10, di = 0x700 y si = 0x900 como en el original.
__declspec(noinline) RwBool __cdecl D3D9RasterPluginAttachExe() {
    const RwInt32 offset = RwRasterRegisterPlugin(0x24, 0x40C, reinterpret_cast<RwPluginObjectConstructor>(0x4C9A60), reinterpret_cast<RwPluginObjectDestructor>(0x4C9A80), nullptr);
    D3D9RasterExtOffset = offset;
    if (offset < 0) {
        return 0;
    }
    auto* const fmt = reinterpret_cast<D3D9FormatInfoLayout*>(0xB4E7E0);
    std::memset(fmt, 0, 0x80 * 4);
    fmt[20].alpha = 0;
    fmt[20].depth = 24;
    fmt[20].rwFormat = 0x0;
    fmt[21].alpha = 1;
    fmt[21].depth = 32;
    fmt[21].rwFormat = 0x500;
    fmt[22].alpha = 0;
    fmt[22].depth = 32;
    fmt[22].rwFormat = 0x600;
    fmt[23].alpha = 0;
    fmt[23].depth = 16;
    fmt[23].rwFormat = 0x200;
    fmt[24].alpha = 0;
    fmt[24].depth = 16;
    fmt[24].rwFormat = 0xA00;
    fmt[25].alpha = 1;
    fmt[25].depth = 16;
    fmt[25].rwFormat = 0x100;
    fmt[26].alpha = 1;
    fmt[26].depth = 16;
    fmt[26].rwFormat = 0x300;
    fmt[27].alpha = 0;
    fmt[27].depth = 8;
    fmt[27].rwFormat = 0x0;
    fmt[28].alpha = 1;
    fmt[28].depth = 8;
    fmt[28].rwFormat = 0x0;
    fmt[29].alpha = 1;
    fmt[29].depth = 16;
    fmt[29].rwFormat = 0x0;
    fmt[30].alpha = 0;
    fmt[30].depth = 16;
    fmt[30].rwFormat = 0x0;
    fmt[31].alpha = 1;
    fmt[31].depth = 32;
    fmt[31].rwFormat = 0x0;
    fmt[32].alpha = 1;
    fmt[32].depth = 32;
    fmt[32].rwFormat = 0x0;
    fmt[33].alpha = 0;
    fmt[33].depth = 32;
    fmt[33].rwFormat = 0x0;
    fmt[34].alpha = 0;
    fmt[34].depth = 32;
    fmt[34].rwFormat = 0x0;
    fmt[35].alpha = 1;
    fmt[35].depth = 32;
    fmt[35].rwFormat = 0x0;
    fmt[36].alpha = 1;
    fmt[36].depth = 64;
    fmt[36].rwFormat = 0x0;
    fmt[40].alpha = 1;
    fmt[40].depth = 16;
    fmt[40].rwFormat = 0x0;
    fmt[41].alpha = 0;
    fmt[41].depth = 8;
    fmt[41].rwFormat = 0x2000;
    fmt[50].alpha = 0;
    fmt[50].depth = 8;
    fmt[50].rwFormat = 0x400;
    fmt[51].alpha = 1;
    fmt[51].depth = 16;
    fmt[51].rwFormat = 0x0;
    fmt[52].alpha = 1;
    fmt[52].depth = 8;
    fmt[52].rwFormat = 0x0;
    fmt[60].alpha = 0;
    fmt[60].depth = 16;
    fmt[60].rwFormat = 0x0;
    fmt[61].alpha = 0;
    fmt[61].depth = 16;
    fmt[61].rwFormat = 0x0;
    fmt[70].rwFormat = 0x700;
    fmt[73].rwFormat = 0x700;
    fmt[80].rwFormat = 0x700;
    fmt[71].rwFormat = 0x900;
    fmt[75].rwFormat = 0x900;
    fmt[77].rwFormat = 0x900;
    fmt[79].rwFormat = 0x900;
    fmt[82].rwFormat = 0x900;
    fmt[83].rwFormat = 0x900;
    fmt[62].alpha = 0;
    fmt[62].rwFormat = 0x0;
    fmt[63].alpha = 0;
    fmt[63].rwFormat = 0x0;
    fmt[64].alpha = 0;
    fmt[64].rwFormat = 0x0;
    fmt[67].rwFormat = 0x0;
    fmt[70].alpha = 0;
    fmt[71].alpha = 0;
    fmt[73].alpha = 0;
    fmt[75].alpha = 0;
    fmt[77].alpha = 0;
    fmt[79].alpha = 0;
    fmt[80].alpha = 0;
    fmt[82].alpha = 0;
    fmt[83].alpha = 0;
    fmt[81].alpha = 0;
    fmt[81].rwFormat = 0x0;
    fmt[110].alpha = 0;
    fmt[110].rwFormat = 0x0;
    fmt[111].alpha = 0;
    fmt[111].rwFormat = 0x0;
    fmt[112].alpha = 0;
    fmt[112].rwFormat = 0x0;
    fmt[113].rwFormat = 0x0;
    fmt[114].alpha = 0;
    fmt[114].rwFormat = 0x0;
    fmt[115].alpha = 0;
    fmt[115].rwFormat = 0x0;
    fmt[116].rwFormat = 0x0;
    fmt[117].alpha = 0;
    fmt[117].rwFormat = 0x0;
    fmt[62].depth = 32;
    fmt[63].depth = 32;
    fmt[64].depth = 32;
    fmt[67].alpha = 1;
    fmt[67].depth = 32;
    fmt[70].depth = 16;
    fmt[71].depth = 32;
    fmt[73].depth = 16;
    fmt[75].depth = 32;
    fmt[77].depth = 32;
    fmt[79].depth = 32;
    fmt[80].depth = 16;
    fmt[82].depth = 32;
    fmt[83].depth = 32;
    fmt[81].depth = 16;
    fmt[110].depth = 64;
    fmt[111].depth = 16;
    fmt[112].depth = 32;
    fmt[113].alpha = 1;
    fmt[113].depth = 64;
    fmt[114].depth = 32;
    fmt[115].depth = 64;
    fmt[116].alpha = 1;
    fmt[116].depth = 128;
    fmt[117].depth = 16;
    return 1;
}

// 0x4C9EA0 (lote 223A, sin validar; el listado decia CWeaponModelInfo): la textura tiene alfa (_rwD3D9TextureHasAlpha; cdecl, un argumento de pila (RwTexture*), `ret`, 25 bytes; llamadores: 0x813B09, 0x813B1A,
// 0x81447C y 0x814605). Como el original: raster = *(RwRaster**)textura (el primer campo); si es nulo devuelve 0; si no, el byte de alfa (+8) de la extension del raster (movzx).
__declspec(noinline) RwInt32 __cdecl D3D9TextureHasAlphaExe(const RwTexture* texture) {
    const RwRaster* const raster = *reinterpret_cast<const RwRaster* const*>(texture);
    if (raster == nullptr) {
        return 0;
    }
    return D3D9RasterExt(raster).alpha;
}

// 0x4C9F70 (lote 223A, sin validar; el listado decia CWeaponModelInfo): campo `cube` de la extension del raster (_rwD3D9RasterIsCubeRaster; cdecl, un argumento de pila (RwRaster*), `ret`, 19 bytes; llamador: 0x814291):
// el nibble bajo del byte +9 (`movzx; and eax,0Fh`).
__declspec(noinline) RwInt32 __cdecl D3D9RasterIsCubeRasterExe(const RwRaster* raster) {
    return D3D9RasterExt(raster).cubeFace & 0x0F;
}

// 0x4C9F90 (lote 223A, sin validar; el listado decia CWeaponModelInfo): bloqueo de un raster D3D9 (_rwD3D9RasterLock; cdecl, tres argumentos de pila (RwUInt8** pixeles, RwRaster*, modo), `ret`, 744 bytes; el modo lleva en
// el byte 1 el nivel de mip y en los bits 0 y 1 escritura y lectura). Como el original: si el raster ya tiene pixeles (+4) devuelve 0; raster superior = cadena de padres hasta uno que sea su propio padre; banderas de
// LockRect = 0x800 con escritura (si no, 0x8810); tipo (cType & 7): 0 y 4 (normal, textura): superficie del nivel (cubo: cara del nibble alto) de la textura del raster superior directamente en la extension (+0xC) y
// LockRect sobre ella (rectangulo propio si el raster es un sub-raster, calculado con los desplazamientos de 16 bits +0x1C/+0x1E); 2 y 5 (camara, textura de camara): superficie de origen (camara: la de la extension
// del superior si tiene cadena de intercambio, si no la 0xC97C30; textura de camara: nivel de la textura), GetDesc, CreateOffscreenPlainSurface (pool SYSTEMMEM) del mismo tamano y formato en la extension, con
// lectura GetRenderTargetData y LockRect; la de origen se libera en el tipo 5 (con o sin exito) y si el LockRect falla se libera la de sistema; 1, 3 y mayores de 5 devuelven 0. Un fallo de COM (HRESULT < 0)
// devuelve 0 sin tocar el raster. En el exito: bits 1 (lectura) y 2 (escritura) de privateFlags, pixeles (+4) = bits del bloqueo, tamano y paso originales (+0x28, +0x2C) y el nuevo (ancho y alto desplazados por el
// nivel de mip, minimo 1; el paso del bloqueo en +0x18), nivel bloqueado (+0xB de la extension) y *pixeles = pixeles; devuelve 1.
__declspec(noinline) RwBool __cdecl D3D9RasterLockExe(RwUInt8** pixelsOut, RwRaster* raster, RwInt32 lockMode) {
    if (raster->cpPixels != nullptr) {
        return 0;
    }
    D3D9RasterExtLayout& ext = D3D9RasterExt(raster);
    RwRaster* top = raster->parent;
    while (top->parent != top) {
        top = top->parent;
    }
    const uint8  mip       = static_cast<uint8>(lockMode >> 8);
    const bool   write     = (lockMode & 1) != 0;
    const uint32 lockFlags = write ? 0x800u : 0x8810u;
    const uint32 typeIndex = raster->cType & 7u;
    if (typeIndex > 5) {
        return 0;
    }
    D3D9RasterExtLayout& topExt = D3D9RasterExt(top);

    const auto LockSurface = [&](void* surface) -> int32 {
        if (top == raster) {
            return ComCall<kSlotSurfaceLockRect, int32>(surface, &ext.lockedPitch, static_cast<const void*>(nullptr), lockFlags);
        }
        D3D9Rect rect;
        rect.left   = raster->nOffsetX;
        rect.top    = raster->nOffsetY;
        rect.right  = raster->width + rect.left;
        rect.bottom = raster->height + rect.top;
        return ComCall<kSlotSurfaceLockRect, int32>(surface, &ext.lockedPitch, static_cast<const void*>(&rect), lockFlags);
    };

    if (typeIndex == 0 || typeIndex == 4) {
        if (D3D9GetTextureSurface(topExt.texture, topExt.cubeFace, mip, &ext.lockedSurface) < 0) {
            return 0;
        }
        if (LockSurface(ext.lockedSurface) < 0) {
            return 0;
        }
        if ((lockMode & 2) != 0) {
            raster->privateFlags |= 2;
        }
        if (write) {
            raster->privateFlags |= 4;
        }
    } else {
        void* source;
        if (typeIndex == 5) {
            source = nullptr;
            if (D3D9GetTextureSurface(topExt.texture, topExt.cubeFace, mip, &source) < 0) {
                return 0;
            }
        } else {
            source = (topExt.swapChain != nullptr) ? topExt.texture : D3D9RenderSurface;
        }
        uint32 desc[8]; // D3DSURFACE_DESC: Format, Type, Usage, Pool, MultiSampleType, MultiSampleQuality, Width, Height
        ComCall<kSlotSurfaceGetDesc, int32>(source, desc);
        int32 result = ComCall<kSlotDeviceCreateOffscreenSurface, int32>(D3D9Device, desc[6], desc[7], desc[0], 2u, &ext.lockedSurface, static_cast<void*>(nullptr));
        if (result >= 0) {
            const bool read = (lockMode & 2) != 0;
            if (read) {
                ComCall<kSlotDeviceGetRenderTargetData, int32>(D3D9Device, source, ext.lockedSurface);
            }
            result = LockSurface(ext.lockedSurface);
            if (result >= 0) {
                if (read) {
                    raster->privateFlags |= 2;
                }
                if (write) {
                    raster->privateFlags |= 4;
                }
            } else {
                ComCall<kSlotRelease, ULONG>(ext.lockedSurface);
            }
        }
        if ((raster->cType & 7u) == 5) {
            ComCall<kSlotRelease, ULONG>(source);
        }
        if (result < 0) {
            return 0;
        }
    }

    raster->cpPixels       = static_cast<RwUInt8*>(ext.lockedBits);
    const RwInt32 height   = raster->height;
    raster->originalWidth  = raster->width;
    const RwInt32 width    = raster->width >> (mip & 31u);
    raster->originalHeight = height;
    const RwInt32 newHeight = height >> (mip & 31u);
    raster->width  = width;
    raster->height = newHeight;
    if (width == 0) {
        raster->width = 1;
    }
    if (newHeight == 0) {
        raster->height = 1;
    }
    raster->stride = ext.lockedPitch;
    ext.lockedMipLevel = mip;
    *pixelsOut = raster->cpPixels;
    return 1;
}

// 0x4CA290 (lote 223A, sin validar; el listado decia CWeaponModelInfo): desbloqueo de un raster D3D9 (_rwD3D9RasterUnlock; cdecl, dos argumentos de pila (el primero no se lee; el raster), `ret`, 559 bytes). Como el
// original: sin pixeles (+4) devuelve 0; tipo (cType & 7) 0 y 4: UnlockRect y Release de la superficie bloqueada (+0xC de la extension), tamano y paso originales de vuelta (+0xC, +0x10 desde +0x28, +0x2C; paso 0 y pixeles
// 0), y si estaba bloqueado para escribir (bit 2 de privateFlags), el formato tiene mipmap automatico (bit 0x10 de cFormat), el nivel bloqueado es 0 y automipmapgen es 0, se marca el nivel como 0xFF y se generan los
// mipmaps del raster superior (0x7F3CD0, con hook; sin imagen) -- los bits 1 y 2 de privateFlags se apagan en todos los casos; tipos 2 y 5: tamano, paso y pixeles igual, UnlockRect y, sin bit de escritura,
// Release; con escritura, el superior copia su superficie con UpdateSurface (rectangulo y punto propios si es un sub-raster) a la de la textura (tipo 5: nivel/cara bloqueados) o a la superficie de la camara
// (ext. del superior con cadena de intercambio, si no 0xC97C30); un HRESULT negativo del GetSurface se salta la copia y la liberacion de la de destino; despues Release de la de sistema, bits de privateFlags
// apagados y, solo en el tipo 5 con mipmap automatico y nivel 0, nivel = 0xFF y mipmaps del superior. Los tipos 1, 3 y mayores de 5 devuelven 0. Devuelve 1 en los demas casos.
__declspec(noinline) RwBool __cdecl D3D9RasterUnlockExe(void* /*unused*/, RwRaster* raster) {
    if (raster->cpPixels == nullptr) {
        return 0;
    }
    D3D9RasterExtLayout& ext = D3D9RasterExt(raster);
    const uint32 typeIndex = raster->cType & 7u;
    if (typeIndex > 5) {
        return 0;
    }
    if (typeIndex == 0 || typeIndex == 4) {
        ComCall<kSlotSurfaceUnlockRect, int32>(ext.lockedSurface);
        ComCall<kSlotRelease, ULONG>(ext.lockedSurface);
        raster->width  = raster->originalWidth;
        raster->height = raster->originalHeight;
        const uint8 privateFlags = raster->privateFlags;
        raster->stride   = 0;
        raster->cpPixels = nullptr;
        if ((privateFlags & 4) != 0 && (raster->cFormat & 0x10) != 0 && ext.lockedMipLevel == 0 && (ext.mipFlags & 0x0Fu) == 0) {
            raster->privateFlags = privateFlags & 0xF9;
            ext.lockedMipLevel   = 0xFF;
            RwRaster* top = raster->parent;
            while (top->parent != top) {
                top = top->parent;
            }
            RwTextureRasterGenerateMipmaps(top, nullptr);
            return 1;
        }
        raster->privateFlags = privateFlags & 0xF9;
        return 1;
    }

    // tipos 2 y 5
    raster->width    = raster->originalWidth;
    raster->height   = raster->originalHeight;
    raster->stride   = 0;
    raster->cpPixels = nullptr;
    ComCall<kSlotSurfaceUnlockRect, int32>(ext.lockedSurface);
    if ((raster->privateFlags & 4) == 0) {
        ComCall<kSlotRelease, ULONG>(ext.lockedSurface);
        raster->privateFlags &= 0xF9;
        return 1;
    }
    RwRaster* top = raster->parent;
    while (top->parent != top) {
        top = top->parent;
    }
    D3D9RasterExtLayout& topExt = D3D9RasterExt(top);
    void* destination = nullptr;
    int32 result      = 0;
    if ((raster->cType & 7u) == 5) {
        result = D3D9GetTextureSurface(topExt.texture, topExt.cubeFace, ext.lockedMipLevel, &destination);
    } else {
        destination = (topExt.swapChain != nullptr) ? topExt.texture : D3D9RenderSurface;
    }
    if (result >= 0) {
        if (top == raster) {
            ComCall<kSlotDeviceUpdateSurface, int32>(D3D9Device, ext.lockedSurface, static_cast<const void*>(nullptr), destination, static_cast<const void*>(nullptr));
        } else {
            D3D9Rect rect;
            D3D9Point point;
            rect.left    = raster->nOffsetX;
            rect.top     = raster->nOffsetY;
            rect.right   = raster->width + rect.left;
            rect.bottom  = raster->height + rect.top;
            point.x      = rect.left;
            point.y      = rect.top;
            ComCall<kSlotDeviceUpdateSurface, int32>(D3D9Device, ext.lockedSurface, static_cast<const void*>(&rect), destination, static_cast<const void*>(&point));
        }
        if ((raster->cType & 7u) == 5) {
            ComCall<kSlotRelease, ULONG>(destination);
        }
    }
    ComCall<kSlotRelease, ULONG>(ext.lockedSurface);
    raster->privateFlags &= 0xF9;
    if ((raster->cType & 7u) == 5 && (raster->cFormat & 0x10) != 0 && ext.lockedMipLevel == 0) {
        ext.lockedMipLevel = 0xFF;
        RwTextureRasterGenerateMipmaps(top, nullptr);
    }
    return 1;
}

// 0x4CA540 (lote 223A, sin validar; el listado decia CWeaponModelInfo): desbloqueo de la paleta de un raster D3D9 (_rwD3D9RasterPaletteUnlock o equivalente; cdecl, dos argumentos de pila (el primero no se lee; el
// raster), `ret`, 85 bytes). Como el original: solo vale para el tipo 0 y los tipos 4 y 5 (cType & 7; si no, devuelve 0); si el bit 4 de privateFlags esta puesto, SetPaletteEntries del dispositivo (0xC97C28, ranura 0x11C)
// con el numero de paleta ([paleta + 0x400]) y los 256 PALETTEENTRY de la paleta de la extension (+4); apaga los bits 3 y 4 de privateFlags, pone la paleta del raster (+8) a cero y devuelve 1.
__declspec(noinline) RwBool __cdecl D3D9RasterPaletteUnlockExe(void* /*unused*/, RwRaster* raster) {
    const uint32 typeIndex = raster->cType & 7u;
    if (typeIndex != 0 && !(typeIndex > 3 && typeIndex <= 5)) {
        return 0;
    }
    if ((raster->privateFlags & 0x10) != 0) {
        uint8* const palette = static_cast<uint8*>(D3D9RasterExt(raster).palette);
        ComCall<kSlotDeviceSetPaletteEntries, int32>(D3D9Device, *reinterpret_cast<uint32*>(palette + 0x400), static_cast<const void*>(palette));
    }
    raster->privateFlags &= 0xE7;
    raster->palette = nullptr;
    return 1;
}
} // namespace

// Biblioteca RenderWare / controlador D3D9 (reconstruidas en C++, banco Dependencies25D); registro propio para no compartir lista con otros grupos.
void RwCoreInjectHooks25d() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwTextureGetMipmapping, 0x7F3550);
    RH_ScopedGlobalInstall(RwTextureGetAutoMipmapping, 0x7F3580);
    RH_ScopedGlobalInstall(RwTextureRegisterPlugin, 0x7F3BB0);
    RH_ScopedGlobalInstall(RwTexDictionaryRegisterPlugin, 0x7F3C10);
    RH_ScopedGlobalInstall(RwTextureRasterGenerateMipmaps, 0x7F3CD0);
    RH_ScopedGlobalInstall(RwD3D9DynamicVertexBufferDestroy, 0x7F5AE0);
    RH_ScopedGlobalInstall(RwD3D9DynamicVertexBufferUnlock, 0x7F5C90);
    RH_ScopedGlobalInstall(_rwD3D9SetDepthStencilSurface, 0x7F5EF0);
    RH_ScopedGlobalInstall(_rwDeviceRegisterPlugin, 0x7F5F60);
    RH_ScopedNamedGlobalInstall(RxD3D9ResEntryHeaderDestroyExe, "RxD3D9ResEntryHeaderDestroy", 0x4C9990); // lote 223A (sin validar)
    RH_ScopedNamedGlobalInstall(D3D9RasterPluginAttachExe, "_rwD3D9RasterPluginAttach", 0x4C9AB0); // lote 223A (sin validar)
    RH_ScopedNamedGlobalInstall(D3D9TextureHasAlphaExe, "_rwD3D9TextureHasAlpha", 0x4C9EA0); // lote 223A (sin validar)
    RH_ScopedNamedGlobalInstall(D3D9RasterIsCubeRasterExe, "_rwD3D9RasterIsCubeRaster", 0x4C9F70); // lote 223A (sin validar)
    RH_ScopedNamedGlobalInstall(D3D9RasterLockExe, "_rwD3D9RasterLock", 0x4C9F90); // lote 223A (sin validar)
    RH_ScopedNamedGlobalInstall(D3D9RasterUnlockExe, "_rwD3D9RasterUnlock", 0x4CA290); // lote 223A (sin validar)
    RH_ScopedNamedGlobalInstall(D3D9RasterPaletteUnlockExe, "_rwD3D9RasterPaletteUnlock", 0x4CA540); // lote 223A (sin validar)
    RH_ScopedGlobalInstall(RwD3D9DeleteVertexShader, 0x7FAC90);
    RH_ScopedGlobalInstall(RwD3D9CreatePixelShader, 0x7FACC0);
    RH_ScopedGlobalInstall(RwD3D9DeletePixelShader, 0x7FACF0);
    RH_ScopedGlobalInstall(RwRasterGetCurrentContext, 0x7FAE60);
    RH_ScopedGlobalInstall(RwRasterRegisterPlugin, 0x7FB0B0);
    RH_ScopedGlobalInstall(RwRasterLockPalette, 0x7FB0E0);
    RH_ScopedGlobalInstall(RwRGBAToPixel, 0x803740);
    RH_ScopedGlobalInstall(RwTextureRegisterPluginStream, 0x804550);
    RH_ScopedGlobalInstall(_rwObjectHasFrameReleaseFrame, 0x804F40);
}

void RwCoreInjectHooks() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwCameraBeginUpdate, 0x7EE190);
    RH_ScopedGlobalInstall(RwCameraEndUpdate, 0x7EE180);
    RH_ScopedGlobalInstall(RwCameraClear, 0x7EE340);
    RH_ScopedGlobalInstall(RwCameraShowRaster, 0x7EE370);

    // Funciones de biblioteca (RenderWare) de <= 40 bytes reconstruidas en C++ (lote 25, grupo g25e).
    RH_ScopedGlobalInstall(RxHeapGetGlobalHeap, 0x8052F0);
    RH_ScopedGlobalInstall(_rwFrameListDeinitialize, 0x807720);
    RH_ScopedGlobalInstall(RwD3D9RasterIsCompressed, 0x820C90);
}

RxHeap* RxHeapCreate(RwUInt32 size) {
    return ((RxHeap*(__cdecl *)(RwUInt32))0x809F90)(size);
}

void RxHeapDestroy(RxHeap* heap) {
    ((void(__cdecl *)(RxHeap*))0x809F30)(heap);
}

RwBool _rxHeapReset(RxHeap* heap) {
    return ((RwBool(__cdecl *)(RxHeap*))0x809EC0)(heap);
}

void* RxHeapAlloc(RxHeap* heap, RwUInt32 size) {
    return ((void*(__cdecl *)(RxHeap*, RwUInt32))0x809AA0)(heap, size);
}

void RxHeapFree(RxHeap* heap, void* block) {
    ((void(__cdecl *)(RxHeap*, void*))0x8098B0)(heap, block);
}

void* RxHeapRealloc(RxHeap* heap, void* block, RwUInt32 newSize, RwBool allowCopy) {
    return ((void*(__cdecl *)(RxHeap*, void*, RwUInt32, RwBool))0x809D10)(heap, block, newSize, allowCopy);
}

void RxPipelineSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x804FC0)(blockSize, numBlocksToPrealloc);
}

RwBool _rxPipelineOpen() {
    return ((RwBool(__cdecl *)(void))0x804FE0)();
}

RwBool _rxPipelineClose() {
    return ((RwBool(__cdecl *)(void))0x804F60)();
}

RxPipeline* RxPipelineCreate() {
    return ((RxPipeline*(__cdecl *)(void))0x8057B0)();
}

void _rxPipelineDestroy(RxPipeline* Pipeline) {
    ((void(__cdecl *)(RxPipeline*))0x805820)(Pipeline);
}

// 0x8052F0: `mov eax, [0xC9A6B0] / ret`. El montón global de Rx es una variable del EXE (se lee por dirección, sin copiarla).
RxHeap* RxHeapGetGlobalHeap() {
    return StaticRef<RxHeap*>(0xC9A6B0);
}

RxPipeline* RxPipelineExecute(RxPipeline* pipeline, void* data, RwBool heapReset) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, void*, RwBool))0x805710)(pipeline, data, heapReset);
}

RxPacket* RxPacketCreate(RxPipelineNode* node) {
    return ((RxPacket*(__cdecl *)(RxPipelineNode*))0x805300)(node);
}

RxCluster* RxClusterSetStride(RxCluster* cluster, RwInt32 stride) {
    return ((RxCluster*(__cdecl *)(RxCluster*, RwInt32))0x805330)(cluster, stride);
}

RxCluster* RxClusterSetExternalData(RxCluster* cluster, void* data, RwInt32 stride, RwInt32 numElements) {
    return ((RxCluster*(__cdecl *)(RxCluster*, void*, RwInt32, RwInt32))0x805340)(cluster, data, stride, numElements);
}

RxCluster* RxClusterSetData(RxCluster* cluster, void* data, RwInt32 stride, RwInt32 numElements) {
    return ((RxCluster*(__cdecl *)(RxCluster*, void*, RwInt32, RwInt32))0x8053A0)(cluster, data, stride, numElements);
}

void _rxPacketDestroy(RxPacket* Packet) {
    ((void(__cdecl *)(RxPacket*))0x80F070)(Packet);
}

RxCluster* RxClusterInitializeData(RxCluster* cluster, RwUInt32 numElements, RwUInt16 stride) {
    return ((RxCluster*(__cdecl *)(RxCluster*, RwUInt32, RwUInt16))0x805400)(cluster, numElements, stride);
}

RxCluster* RxClusterResizeData(RxCluster* CurrentCluster, RwUInt32 NumElements) {
    return ((RxCluster*(__cdecl *)(RxCluster*, RwUInt32))0x805470)(CurrentCluster, NumElements);
}

RxCluster* RxClusterDestroyData(RxCluster* CurrentCluster) {
    return ((RxCluster*(__cdecl *)(RxCluster*))0x8054C0)(CurrentCluster);
}

RxCluster* RxClusterLockWrite(RxPacket* packet, RwUInt32 clusterIndex, RxPipelineNode* node) {
    return ((RxCluster*(__cdecl *)(RxPacket*, RwUInt32, RxPipelineNode*))0x8054F0)(packet, clusterIndex, node);
}

void RxClusterUnlock(RxCluster* cluster) {
    ((void(__cdecl *)(RxCluster*))0x8055C0)(cluster);
}

RwUInt32 RxPipelineNodeSendConfigMsg(RxPipelineNode* dest, RwUInt32 msg, RwUInt32 intparam, void* ptrparam) {
    return ((RwUInt32(__cdecl *)(RxPipelineNode*, RwUInt32, RwUInt32, void*))0x8055D0)(dest, msg, intparam, ptrparam);
}

RxPipelineNode* RxPipelineNodeForAllConnectedOutputs(RxPipelineNode* node, RxPipeline* pipeline, RxPipelineNodeOutputCallBack callbackfn, void* callbackdata) {
    return ((RxPipelineNode*(__cdecl *)(RxPipelineNode*, RxPipeline*, RxPipelineNodeOutputCallBack, void*))0x8055F0)(node, pipeline, callbackfn, callbackdata);
}

RxPipelineCluster* RxPipelineNodeGetPipelineCluster(RxPipelineNode* node, RwUInt32 clustersOfInterestIndex) {
    return ((RxPipelineCluster*(__cdecl *)(RxPipelineNode*, RwUInt32))0x805680)(node, clustersOfInterestIndex);
}

RwUInt32 RxPipelineClusterGetCreationAttributes(RxPipelineCluster* cluster) {
    return ((RwUInt32(__cdecl *)(RxPipelineCluster*))0x8056B0)(cluster);
}

RxPipelineCluster* RxPipelineClusterSetCreationAttributes(RxPipelineCluster* cluster, RwUInt32 creationAttributes) {
    return ((RxPipelineCluster*(__cdecl *)(RxPipelineCluster*, RwUInt32))0x8056C0)(cluster, creationAttributes);
}

RwUInt32 RxClusterGetAttributes(RxCluster* cluster) {
    return ((RwUInt32(__cdecl *)(RxCluster*))0x8056E0)(cluster);
}

RxCluster* RxClusterSetAttributes(RxCluster* cluster, RwUInt32 attributes) {
    return ((RxCluster*(__cdecl *)(RxCluster*, RwUInt32))0x8056F0)(cluster, attributes);
}

RxNodeOutput RxPipelineNodeFindOutputByName(RxPipelineNode* node, const RwChar* outputname) {
    return ((RxNodeOutput(__cdecl *)(RxPipelineNode*, const RwChar*))0x805D10)(node, outputname);
}

RxNodeOutput RxPipelineNodeFindOutputByIndex(RxPipelineNode* node, RwUInt32 outputindex) {
    return ((RxNodeOutput(__cdecl *)(RxPipelineNode*, RwUInt32))0x805D70)(node, outputindex);
}

RxNodeInput RxPipelineNodeFindInput(RxPipelineNode* node) {
    return ((RxNodeInput(__cdecl *)(RxPipelineNode*))0x805DA0)(node);
}

RxNodeDefinition* RxPipelineNodeCloneDefinition(RxPipelineNode* node, RxClusterDefinition* cluster2add) {
    return ((RxNodeDefinition*(__cdecl *)(RxPipelineNode*, RxClusterDefinition*))0x8059C0)(node, cluster2add);
}

RxPipeline* RxPipelineNodeRequestCluster(RxPipeline* pipeline, RxPipelineNode* node, RxClusterDefinition* clusterDef) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, RxPipelineNode*, RxClusterDefinition*))0x805DB0)(pipeline, node, clusterDef);
}

RxPipeline* RxPipelineNodeReplaceCluster(RxPipeline* pipeline, RxPipelineNode* node, RxClusterDefinition* oldClusterDef, RxClusterDefinition* newClusterDef) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, RxPipelineNode*, RxClusterDefinition*, RxClusterDefinition*))0x805E20)(pipeline, node, oldClusterDef, newClusterDef);
}

void* RxPipelineNodeGetInitData(RxPipelineNode* node) {
    return ((void*(__cdecl *)(RxPipelineNode*))0x805F30)(node);
}

void* RxPipelineNodeCreateInitData(RxPipelineNode* node, RwUInt32 size) {
    return ((void*(__cdecl *)(RxPipelineNode*, RwUInt32))0x805EA0)(node, size);
}

RxPipeline* RxPipelineClone(RxPipeline* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*))0x806AC0)(pipeline);
}

RxPipelineNode* RxPipelineFindNodeByName(RxPipeline* pipeline, const RwChar* name, RxPipelineNode* start, RwInt32* nodeIndex) {
    return ((RxPipelineNode*(__cdecl *)(RxPipeline*, const RwChar*, RxPipelineNode*, RwInt32*))0x806B30)(pipeline, name, start, nodeIndex);
}

RxPipelineNode* RxPipelineFindNodeByIndex(RxPipeline* pipeline, RwUInt32 nodeindex) {
    return ((RxPipelineNode*(__cdecl *)(RxPipeline*, RwUInt32))0x806BC0)(pipeline, nodeindex);
}

RxLockedPipe* RxPipelineLock(RxPipeline* pipeline) {
    return ((RxLockedPipe*(__cdecl *)(RxPipeline*))0x806990)(pipeline);
}

RxPipeline* RxLockedPipeUnlock(RxLockedPipe* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*))0x805F40)(pipeline);
}

RxPipeline* RxLockedPipeReplaceNode(RxLockedPipe* pipeline, RxPipelineNode* node, RxNodeDefinition* nodeDef) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxPipelineNode*, RxNodeDefinition*))0x806F20)(pipeline, node, nodeDef);
}

RxPipeline* RxLockedPipeDeleteNode(RxLockedPipe* pipeline, RxPipelineNode* node) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxPipelineNode*))0x807040)(pipeline, node);
}

RxPipeline* RxLockedPipeSetEntryPoint(RxLockedPipe* pipeline, RxNodeInput in) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxNodeInput))0x807070)(pipeline, in);
}

RxLockedPipe* RxLockedPipeAddFragment(RxLockedPipe* pipeline, RwUInt32* firstIndex, RxNodeDefinition* nodeDef0, RxNodeDefinition* __va_end__) {
    return plugin::CallAndReturn<RxPipeline*, 0x806BE0, RxLockedPipe*, RwUInt32*, RxNodeDefinition*, RxNodeDefinition*>(pipeline, firstIndex, nodeDef0, __va_end__);
}

RxPipelineNode* RxLockedPipeGetEntryPoint(RxLockedPipe* pipeline) {
    return ((RxPipelineNode*(__cdecl *)(RxLockedPipe*))0x8070D0)(pipeline);
}

RxPipeline* RxLockedPipeAddPath(RxLockedPipe* pipeline, RxNodeOutput out, RxNodeInput in) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxNodeOutput, RxNodeInput))0x807100)(pipeline, out, in);
}

RxPipeline* RxLockedPipeDeletePath(RxLockedPipe* pipeline, RxNodeOutput out, RxNodeInput in) {
    return ((RxPipeline*(__cdecl *)(RxLockedPipe*, RxNodeOutput, RxNodeInput))0x807170)(pipeline, out, in);
}

RxPipeline* RxPipelineInsertDebugNode(RxPipeline* pipeline, RxPipelineNode* before, RxPipelineNode* after, RxNodeDefinition* debugNode) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, RxPipelineNode*, RxPipelineNode*, RxNodeDefinition*))0x8071B0)(pipeline, before, after, debugNode);
}

void RwD3D9VertexBufferManagerChangeDefaultSize(RwUInt32 defaultSize) {
    ((void(__cdecl *)(RwUInt32))0x7F57F0)(defaultSize);
}

RwBool RwD3D9CreateVertexBuffer(RwUInt32 stride, RwUInt32 size, void* vertexBuffer, RwUInt32* offset) {
    return ((RwBool(__cdecl *)(RwUInt32, RwUInt32, void*, RwUInt32*))0x7F5500)(stride, size, vertexBuffer, offset);
}

void RwD3D9DestroyVertexBuffer(RwUInt32 stride, RwUInt32 size, void* vertexBuffer, RwUInt32 offset) {
    ((void(__cdecl *)(RwUInt32, RwUInt32, void*, RwUInt32))0x7F56A0)(stride, size, vertexBuffer, offset);
}

RwBool RwD3D9DynamicVertexBufferCreate(RwUInt32 size, void* vertexBuffer) {
    return ((RwBool(__cdecl *)(RwUInt32, void*))0x7F5A00)(size, vertexBuffer);
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

RwBool RwD3D9DynamicVertexBufferLock(RwUInt32 vertexSize, RwUInt32 numVertex, void* vertexBufferOut, void* vertexDataOut, RwUInt32* baseIndexOut) {
    return ((RwBool(__cdecl *)(RwUInt32, RwUInt32, void*, void*, RwUInt32*))0x7F5B10)(vertexSize, numVertex, vertexBufferOut, vertexDataOut, baseIndexOut);
}

// 0x7F5C90: IDirect3DVertexBuffer9::Unlock; TRUE si el HRESULT no es negativo.
RwBool RwD3D9DynamicVertexBufferUnlock(void* vertexBuffer) {
    const HRESULT result = ComCall<kSlotVertexBufferUnlock, HRESULT>(vertexBuffer);
    return result >= 0;
}

void RwRasterSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7FB350)(blockSize, numBlocksToPrealloc);
}

RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags) {
    return ((RwRaster*(__cdecl *)(RwInt32, RwInt32, RwInt32, RwInt32))0x7FB230)(width, height, depth, flags);
}

RwBool RwRasterDestroy(RwRaster* raster) {
    return ((RwBool(__cdecl *)(RwRaster*))0x7FB020)(raster);
}

RwRaster* RwRasterGetOffset(RwRaster* raster, RwInt16* xOffset, RwInt16* yOffset) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwInt16*, RwInt16*))0x7FAEA0)(raster, xOffset, yOffset);
}

RwInt32 RwRasterGetNumLevels(RwRaster* raster) {
    return ((RwInt32(__cdecl *)(RwRaster*))0x7FB160)(raster);
}

RwRaster* RwRasterSubRaster(RwRaster* subRaster, RwRaster* raster, RwRect* rect) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwRaster*, RwRect*))0x7FB1D0)(subRaster, raster, rect);
}

RwRaster* RwRasterRenderFast(RwRaster* raster, RwInt32 x, RwInt32 y) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwInt32, RwInt32))0x7FAF50)(raster, x, y);
}

RwRaster* RwRasterRender(RwRaster* raster, RwInt32 x, RwInt32 y) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwInt32, RwInt32))0x7FAFB0)(raster, x, y);
}

RwRaster* RwRasterRenderScaled(RwRaster* raster, RwRect* rect) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwRect*))0x7FAE80)(raster, rect);
}

RwRaster* RwRasterPushContext(RwRaster* raster) {
    return ((RwRaster*(__cdecl *)(RwRaster*))0x7FB060)(raster);
}

RwRaster* RwRasterPopContext() {
    return ((RwRaster*(__cdecl *)(void))0x7FB110)();
}

// 0x7FAE60: cima de la pila de contextos de rasters (rasterStack[rasterSP], sin comprobar el rango de rasterSP).
RwRaster* RwRasterGetCurrentContext() {
    auto& globals = RasterGlobals();
    return *(globals.rasterStack + globals.rasterSP);
}

RwBool RwRasterClear(RwInt32 pixelValue) {
    return ((RwBool(__cdecl *)(RwInt32))0x7FAEE0)(pixelValue);
}

RwBool RwRasterClearRect(RwRect* rpRect, RwInt32 pixelValue) {
    return ((RwBool(__cdecl *)(RwRect*, RwInt32))0x7FAF90)(rpRect, pixelValue);
}

RwRaster* RwRasterShowRaster(RwRaster* raster, void* dev, RwUInt32 flags) {
    return ((RwRaster*(__cdecl *)(RwRaster*, void*, RwUInt32))0x7FB1A0)(raster, dev, flags);
}

//! NOTE: This function is responsible for calculating the `stride` in the `raster`
RwUInt8* RwRasterLock(RwRaster* raster, RwUInt8 level, RwInt32 lockMode) {
    return ((RwUInt8*(__cdecl *)(RwRaster*, RwUInt8, RwInt32))0x7FB2D0)(raster, level, lockMode);
}

RwRaster* RwRasterUnlock(RwRaster* raster) {
    return ((RwRaster*(__cdecl *)(RwRaster*))0x7FAEC0)(raster);
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

RwRaster* RwRasterUnlockPalette(RwRaster* raster) {
    return ((RwRaster*(__cdecl *)(RwRaster*))0x7FAFF0)(raster);
}

// 0x7FB0B0: _rwPluginRegistryAddPlugin(&rasterTKList, ...)
RwInt32 RwRasterRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&RasterTKList, size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwRasterGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7FAE40)(pluginID);
}

RxRenderStateVector* RxRenderStateVectorSetDefaultRenderStateVector(RxRenderStateVector* rsvp) {
    return ((RxRenderStateVector*(__cdecl *)(RxRenderStateVector*))0x80EA40)(rsvp);
}

RwImage* RwImageCreate(RwInt32 width, RwInt32 height, RwInt32 depth) {
    return ((RwImage*(__cdecl *)(RwInt32, RwInt32, RwInt32))0x8026E0)(width, height, depth);
}

RwBool RwImageDestroy(RwImage* image) {
    return ((RwBool(__cdecl *)(RwImage*))0x802740)(image);
}

RwImage* RwImageAllocatePixels(RwImage* image) {
    return ((RwImage*(__cdecl *)(RwImage*))0x8027A0)(image);
}

RwImage* RwImageFreePixels(RwImage* image) {
    return ((RwImage*(__cdecl *)(RwImage*))0x802860)(image);
}

RwImage* RwImageApplyMask(RwImage* image, const RwImage* mask) {
    return ((RwImage*(__cdecl *)(RwImage*, const RwImage*))0x802AF0)(image, mask);
}

RwImage* RwImageMakeMask(RwImage* image) {
    return ((RwImage*(__cdecl *)(RwImage*))0x802A20)(image);
}

RwImage* RwImageReadMaskedImage(const RwChar* imageName, const RwChar* maskname) {
    return ((RwImage*(__cdecl *)(const RwChar*, const RwChar*))0x8035C0)(imageName, maskname);
}

RwImage* RwImageRead(const RwChar* imageName) {
    return ((RwImage*(__cdecl *)(const RwChar*))0x802FD0)(imageName);
}

const RwChar* RwImageSetPath(const RwChar* path) {
    return ((const RwChar*(__cdecl *)(const RwChar*))0x802EA0)(path);
}

RwBool RwImageSetGamma(RwReal gammaValue) {
    return ((RwBool(__cdecl *)(RwReal))0x803FE0)(gammaValue);
}

RwImage* RwImageGammaCorrect(RwImage* image) {
    return ((RwImage*(__cdecl *)(RwImage*))0x803E30)(image);
}

RwBool RwPalQuantInit(RwPalQuant* pq) {
    return ((RwBool(__cdecl *)(RwPalQuant*))0x80C470)(pq);
}

void RwPalQuantTerm(RwPalQuant* pq) {
    ((void(__cdecl *)(RwPalQuant*))0x80C520)(pq);
}

void RwPalQuantAddImage(RwPalQuant* pq, RwImage* img, RwReal weight) {
    ((void(__cdecl *)(RwPalQuant*, RwImage*, RwReal))0x80AA80)(pq, img, weight);
}

RwInt32 RwPalQuantResolvePalette(RwRGBA* palette, RwInt32 maxcols, RwPalQuant* pq) {
    return ((RwInt32(__cdecl *)(RwRGBA*, RwInt32, RwPalQuant*))0x80AF60)(palette, maxcols, pq);
}

void RwPalQuantMatchImage(RwUInt8* dstpixels, RwInt32 dststride, RwInt32 dstdepth, RwBool dstPacked, RwPalQuant* pq, RwImage* img) {
    ((void(__cdecl *)(RwUInt8*, RwInt32, RwInt32, RwBool, RwPalQuant*, RwImage*))0x80BF20)(dstpixels, dststride, dstdepth, dstPacked, pq, img);
}

RwBool RwTextureSetMipmapping(RwBool enable) {
    return ((RwBool(__cdecl *)(RwBool))0x7F3530)(enable);
}

// 0x7F3550: RwTextureGlobals::mipmapping
RwBool RwTextureGetMipmapping() {
    return TextureGlobals().mipmapping;
}

RwBool RwTextureSetAutoMipmapping(RwBool enable) {
    return ((RwBool(__cdecl *)(RwBool))0x7F3560)(enable);
}

// 0x7F3580: RwTextureGlobals::autoMipmapping
RwBool RwTextureGetAutoMipmapping() {
    return TextureGlobals().autoMipmapping;
}

RwBool RwTextureSetMipmapGenerationCallBack(RwTextureCallBackMipmapGeneration callback) {
    return ((RwBool(__cdecl *)(RwTextureCallBackMipmapGeneration))0x7F3C70)(callback);
}

RwTextureCallBackMipmapGeneration RwTextureGetMipmapGenerationCallBack() {
    return ((RwTextureCallBackMipmapGeneration(__cdecl *)(void))0x7F3C90)();
}

RwBool RwTextureSetMipmapNameCallBack(RwTextureCallBackMipmapName callback) {
    return ((RwBool(__cdecl *)(RwTextureCallBackMipmapName))0x7F3CA0)(callback);
}

RwTextureCallBackMipmapName RwTextureGetMipmapNameCallBack() {
    return ((RwTextureCallBackMipmapName(__cdecl *)(void))0x7F3CC0)();
}

RwBool RwTextureGenerateMipmapName(RwChar* name, RwChar* maskName, RwUInt8 mipLevel, RwInt32 format) {
    return ((RwBool(__cdecl *)(RwChar*, RwChar*, RwUInt8, RwInt32))0x7F3AA0)(name, maskName, mipLevel, format);
}

// 0x7F3CD0: llama al callback de generacion de mipmaps del motor (sin comprobar que exista) y devuelve TRUE si no devuelve NULL.
RwBool RwTextureRasterGenerateMipmaps(RwRaster* raster, RwImage* image) {
    return TextureGlobals().mipmapGenerationCallBack(raster, image) != nullptr;
}

RwBool _rwTextureSetAutoMipMapState(RwBool enable) {
    return ((RwBool(__cdecl *)(RwBool))0x7F3590)(enable);
}

RwBool _rwTextureGetAutoMipMapState() {
    return ((RwBool(__cdecl *)(void))0x7F35C0)();
}

RwTextureCallBackRead RwTextureGetReadCallBack() {
    return ((RwTextureCallBackRead(__cdecl *)(void))0x7F3520)();
}

RwBool RwTextureSetReadCallBack(RwTextureCallBackRead callBack) {
    return ((RwBool(__cdecl *)(RwTextureCallBackRead))0x7F3500)(callBack);
}

RwTextureCallBackFind RwTextureGetFindCallBack() {
    return ((RwTextureCallBackFind(__cdecl *)(void))0x7F34F0)();
}

RwBool RwTextureSetFindCallBack(RwTextureCallBackFind callBack) {
    return ((RwBool(__cdecl *)(RwTextureCallBackFind))0x7F34D0)(callBack);
}

RwTexture* RwTextureSetName(RwTexture* texture, const RwChar* name) {
    return ((RwTexture*(__cdecl *)(RwTexture*, const RwChar*))0x7F38A0)(texture, name);
}

RwTexture* RwTextureSetMaskName(RwTexture* texture, const RwChar* maskName) {
    return ((RwTexture*(__cdecl *)(RwTexture*, const RwChar*))0x7F3910)(texture, maskName);
}

void RwTexDictionarySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7F3E80)(blockSize, numBlocksToPrealloc);
}

RwTexDictionary* RwTexDictionaryCreate() {
    return ((RwTexDictionary*(__cdecl *)(void))0x7F3600)();
}

RwBool RwTexDictionaryDestroy(RwTexDictionary* dict) {
    return ((RwBool(__cdecl *)(RwTexDictionary*))0x7F36A0)(dict);
}

RwTexture* RwTextureCreate(RwRaster* raster) {
    return ((RwTexture*(__cdecl *)(RwRaster*))0x7F37C0)(raster);
}

RwTexture* RwTextureRead(const char* name, const char* maskName) {
    return plugin::CallAndReturn<RwTexture*, 0x7F3AC0, const char*, const char*>(name, maskName);
}

RwBool RwTextureDestroy(RwTexture* texture) {
    return ((RwBool(__cdecl *)(RwTexture*))0x7F3820)(texture);
}

RwTexture* RwTextureSetRaster(RwTexture* texture, RwRaster* raster) {
    return ((RwTexture*(__cdecl *)(RwTexture*, RwRaster*))0x7F35D0)(texture, raster);
}

RwTexture* RwTexDictionaryAddTexture(RwTexDictionary* dict, RwTexture* texture) {
    return ((RwTexture*(__cdecl *)(RwTexDictionary*, RwTexture*))0x7F3980)(dict, texture);
}

RwTexture* RwTexDictionaryRemoveTexture(RwTexture* texture) {
    return ((RwTexture*(__cdecl *)(RwTexture*))0x7F39C0)(texture);
}

RwTexture* RwTexDictionaryFindNamedTexture(RwTexDictionary* dict, const RwChar* name) {
    return ((RwTexture*(__cdecl *)(RwTexDictionary*, const RwChar*))0x7F39F0)(dict, name);
}

RwTexDictionary* RwTexDictionaryGetCurrent() {
    return ((RwTexDictionary*(__cdecl *)(void))0x7F3A90)();
}

RwTexDictionary* RwTexDictionarySetCurrent(RwTexDictionary* dict) {
    return ((RwTexDictionary*(__cdecl *)(RwTexDictionary*))0x7F3A70)(dict);
}

const RwTexDictionary* RwTexDictionaryForAllTextures(const RwTexDictionary* dict, RwTextureCallBack fpCallBack, void* data) {
    return ((const RwTexDictionary*(__cdecl *)(const RwTexDictionary*, RwTextureCallBack, void*))0x7F3730)(dict, fpCallBack, data);
}

RwBool RwTexDictionaryForAllTexDictionaries(RwTexDictionaryCallBack fpCallBack, void* data) {
    return ((RwBool(__cdecl *)(RwTexDictionaryCallBack, void*))0x7F3770)(fpCallBack, data);
}

// 0x7F3BB0: _rwPluginRegistryAddPlugin(&textureTKList, ...)
RwInt32 RwTextureRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&TextureTKList, size, pluginID, constructCB, destructCB, copyCB);
}

// 0x7F3C10: _rwPluginRegistryAddPlugin(&texDictTKList, ...)
RwInt32 RwTexDictionaryRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(&TexDictTKList, size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwTextureGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7F3BE0)(pluginID);
}

RwInt32 RwTexDictionaryGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7F3C40)(pluginID);
}

RwBool RwTextureValidatePlugins(const RwTexture* texture) {
    return ((RwBool(__cdecl *)(const RwTexture*))0x7F3C00)(texture);
}

RwBool RwTexDictionaryValidatePlugins(const RwTexDictionary* dict) {
    return ((RwBool(__cdecl *)(const RwTexDictionary*))0x7F3C60)(dict);
}

rwIm3DPool* _rwIm3DGetPool() {
    return ((rwIm3DPool*(__cdecl *)(void))0x7EFDD0)();
}

void* RwIm3DTransform(RwIm3DVertex* pVerts, RwUInt32 numVerts, RwMatrix* ltm, RwUInt32 flags) {
    return ((void*(__cdecl *)(RwIm3DVertex*, RwUInt32, RwMatrix*, RwUInt32))0x7EF450)(pVerts, numVerts, ltm, flags);
}

RwBool RwIm3DEnd() {
    return ((RwBool(__cdecl *)(void))0x7EF520)();
}

RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2) {
    return ((RwBool(__cdecl *)(RwInt32, RwInt32))0x7EF900)(vert1, vert2);
}

RwBool RwIm3DRenderTriangle(RwInt32 vert1, RwInt32 vert2, RwInt32 vert3) {
    return ((RwBool(__cdecl *)(RwInt32, RwInt32, RwInt32))0x7EF810)(vert1, vert2, vert3);
}

RwBool RwIm3DRenderIndexedPrimitive(RwPrimitiveType primType, RwImVertexIndex* indices, RwInt32 numIndices) {
    return ((RwBool(__cdecl *)(RwPrimitiveType, RwImVertexIndex*, RwInt32))0x7EF550)(primType, indices, numIndices);
}

RwBool RwIm3DRenderPrimitive(RwPrimitiveType primType) {
    return ((RwBool(__cdecl *)(RwPrimitiveType))0x7EF6B0)(primType);
}

RxPipeline* RwIm3DGetTransformPipeline() {
    return ((RxPipeline*(__cdecl *)(void))0x7EF9D0)();
}

RxPipeline* RwIm3DGetRenderPipeline(RwPrimitiveType primType) {
    return ((RxPipeline*(__cdecl *)(RwPrimitiveType))0x7EF9E0)(primType);
}

RxPipeline* RwIm3DSetTransformPipeline(RxPipeline* pipeline) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*))0x7EFAC0)(pipeline);
}

RxPipeline* RwIm3DSetRenderPipeline(RxPipeline* pipeline, RwPrimitiveType primType) {
    return ((RxPipeline*(__cdecl *)(RxPipeline*, RwPrimitiveType))0x7EFB10)(pipeline, primType);
}

RwRaster* RwD3D9RasterStreamReadDDS(RwStream* stream) {
    return ((RwRaster*(__cdecl *)(RwStream*))0x81F360)(stream);
}

RwTexture* RwD3D9DDSTextureRead(const RwChar* name, const RwChar* maskname) {
    return ((RwTexture*(__cdecl *)(const RwChar*, const RwChar*))0x820A10)(name, maskname);
}

// 0x820C90: la extensión D3D9 del raster está en raster + _RwD3D9RasterExtOffset (0xB4E9E0); su byte +0xA guarda en el nibble alto el formato comprimido.
// Devuelve 1 si ese nibble no es cero (neg al / sbb eax,eax / neg eax) y 0 si lo es. Aritmética de 32 bits con envoltura, como el EXE.
RwBool RwD3D9RasterIsCompressed(const RwRaster* raster) {
    const uint32 address = static_cast<uint32>(reinterpret_cast<uintptr_t>(raster)) + static_cast<uint32>(StaticRef<RwInt32>(0xB4E9E0)) + 0xAu;
    return (*reinterpret_cast<const uint8*>(static_cast<uintptr_t>(address)) & 0xF0u) != 0;
}

RwBool RwD3D9DeviceSupportsDXTTexture() {
    return ((RwBool(__cdecl *)(void))0x7F9C30)();
}

void* RwD3D9GetCurrentD3DDevice() { // IDirect3DDevice9
    return ((void*(__cdecl *)(void))0x7F9D50)();
}

RwUInt32 RwD3D9EngineGetMaxMultiSamplingLevels() {
    return ((RwUInt32(__cdecl *)(void))0x7F84E0)();
}

void RwD3D9EngineSetMultiSamplingLevels(RwUInt32 numLevels) {
    ((void(__cdecl *)(RwUInt32))0x7F84F0)(numLevels);
}

void RwD3D9EngineSetRefreshRate(RwUInt32 refreshRate) {
    ((void(__cdecl *)(RwUInt32))0x7F8580)(refreshRate);
}

void RwD3D9EngineSetMultiThreadSafe(RwBool enable) {
    ((void(__cdecl *)(RwBool))0x7F8620)(enable);
}

void RwD3D9EngineSetSoftwareVertexProcessing(RwBool enable) {
    ((void(__cdecl *)(RwBool))0x7F8630)(enable);
}

void* RwD3D9GetCurrentD3DRenderTarget(RwUInt32 index) {
    return ((void*(__cdecl *)(RwUInt32))0x7F9E80)(index);
}

RwBool RwD3D9SetRenderTarget(RwUInt32 index, RwRaster* raster) {
    return ((RwBool(__cdecl *)(RwUInt32, RwRaster*))0x7F9E90)(index, raster);
}

RwBool RwD3D9ChangeVideoMode(RwInt32 modeIndex) {
    return ((RwBool(__cdecl *)(RwInt32))0x7F8640)(modeIndex);
}

RwBool RwD3D9ChangeMultiSamplingLevels(RwUInt32 numLevels) {
    return ((RwBool(__cdecl *)(RwUInt32))0x7F8A90)(numLevels);
}

RwBool RwD3D9CameraAttachWindow(void* camera, void* hwnd) {
    return ((RwBool(__cdecl *)(void*, void*))0x7F8D70)(camera, hwnd);
}

void RwD3D9SetStreamSource(RwUInt32 streamNumber, void* streamData, RwUInt32 offset, RwUInt32 stride) {
    ((void(__cdecl *)(RwUInt32, void*, RwUInt32, RwUInt32))0x7FA030)(streamNumber, streamData, offset, stride);
}

void _rwD3D9RenderStateFlushCache() {
    ((void(__cdecl *)(void))0x7FC200)();
}

void _rwD3D9DrawIndexedPrimitiveUP(RwUInt32 primitiveType, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 primitiveCount, const void* indexData, const void* vertexStreamZeroData, RwUInt32 VertexStreamZeroStride) {
   ((void(__cdecl *)(RwUInt32, RwUInt32, RwUInt32, RwUInt32, const void*, const void*, RwUInt32))0x7FA1F0)(primitiveType, minIndex, numVertices, primitiveCount, indexData, vertexStreamZeroData, VertexStreamZeroStride);
}

void _rwD3D9DrawPrimitiveUP(RwUInt32 primitiveType, RwUInt32 primitiveCount, const void* vertexStreamZeroData, RwUInt32 VertexStreamZeroStride) {
   ((void(__cdecl *)(RwUInt32, RwUInt32, const void*, RwUInt32))0x7FA290)(primitiveType, primitiveCount, vertexStreamZeroData, VertexStreamZeroStride);
}

void _rwD3D9DrawIndexedPrimitive(RwUInt32 primitiveType, RwInt32 baseVertexIndex, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 primitiveCount) {
   ((void(__cdecl *)(RwUInt32, RwInt32, RwUInt32, RwUInt32, RwUInt32, RwUInt32))0x7FA320)(primitiveType, baseVertexIndex, minIndex, numVertices, startIndex, primitiveCount);
}

void _rwD3D9SetVertexShaderConstant(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount) {
   ((void(__cdecl *)(RwUInt32, const void*, RwUInt32))0x7FACA0)(registerAddress, constantData, constantCount);
}

void _rwD3D9SetPixelShaderConstant(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount) {
   ((void(__cdecl *)(RwUInt32, const void*, RwUInt32))0x7FAD00)(registerAddress, constantData, constantCount);
}

void _rwD3D9SetFVF(RwUInt32 fvf) {
   ((void(__cdecl *)(RwUInt32))0x7F9F30)(fvf);
}

void _rwD3D9SetVertexShader(void *shader) {
   ((void(__cdecl *)(void *))0x7F9FB0)(shader);
}

void _rwD3D9SetPixelShader(void *shader) {
   ((void(__cdecl *)(void *))0x7F9FF0)(shader);
}

void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value) {
   ((void(__cdecl *)(RwUInt32, RwUInt32))0x7FC2D0)(state, value);
}

void RwD3D9GetRenderState(RwUInt32 state, void* value) {
   ((void(__cdecl *)(RwUInt32, void*))0x7FC320)(state, value);
}

void RwD3D9SetTextureStageState(RwUInt32 stage, RwUInt32 type, RwUInt32 value) {
   ((void(__cdecl *)(RwUInt32, RwUInt32, RwUInt32))0x7FC340)(stage, type, value);
}

void RwD3D9GetTextureStageState(RwUInt32 stage, RwUInt32 type, void* value) {
   ((void(__cdecl *)(RwUInt32, RwUInt32, void*))0x7FC3A0)(stage, type, value);
}

void RwD3D9SetSamplerState(RwUInt32 stage, RwUInt32 type, RwUInt32 value) {
   ((void(__cdecl *)(RwUInt32, RwUInt32, RwUInt32))0x7FC3C0)(stage, type, value);
}

void RwD3D9GetSamplerState(RwUInt32 stage, RwUInt32 type, void* value) {
   ((void(__cdecl *)(RwUInt32, RwUInt32, void*))0x7FC400)(stage, type, value);
}

void RwD3D9SetStencilClear(RwUInt32 stencilClear) {
   ((void(__cdecl *)(RwUInt32))0x7F9D30)(stencilClear);
}

RwUInt32 RwD3D9GetStencilClear() {
   return ((RwUInt32(__cdecl *)(void))0x7F9D40)();
}

RwBool RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage) {
   return ((RwBool(__cdecl *)(RwTexture*, RwUInt32))0x7FDE70)(texture, stage);
}

RwBool RwD3D9SetTransform(RwUInt32 state, const void* matrix) {
   return ((RwBool(__cdecl *)(RwUInt32, const void*))0x7FA390)(state, matrix);
}

void RwD3D9GetTransform(RwUInt32 state, void* matrix) {
   ((void(__cdecl *)(RwUInt32, void*))0x7FA4F0)(state, matrix);
}

RwBool RwD3D9SetMaterial(const void* material) {
   return ((RwBool(__cdecl *)(const void*))0x7FC430)(material);
}

RwBool RwD3D9SetClipPlane(RwUInt32 index, const RwV4d* plane) {
   return ((RwBool(__cdecl *)(RwUInt32, const RwV4d*))0x7FC4A0)(index, plane);
}

RwBool RwD3D9SetTransformWorld(const RwMatrix* matrix) {
   return ((RwBool(__cdecl *)(const RwMatrix*))0x7FA520)(matrix);
}

RwBool RwD3D9SetSurfaceProperties(const RwSurfaceProperties* surfaceProps, const RwRGBA* color, RwUInt32 flags) {
   return ((RwBool(__cdecl *)(const RwSurfaceProperties*, const RwRGBA*, RwUInt32))0x7FC4D0)(surfaceProps, color, flags);
}

RwBool RwD3D9SetLight(RwInt32 index, const void* light) {
   return ((RwBool(__cdecl *)(RwInt32, const void*))0x7FA660)(index, light);
}

void RwD3D9GetLight(RwInt32 index, void* light) {
   ((void(__cdecl *)(RwInt32, void*))0x7FA820)(index, light);
}

RwBool RwD3D9EnableLight(RwInt32 index, RwBool enable) {
   return ((RwBool(__cdecl *)(RwInt32, RwBool))0x7FA860)(index, enable);
}

// Registro de los hooks del grupo g25b en rwcore.cpp (propio para no compartir la lista de RwCoreInjectHooks con otros grupos).
void RwCoreInjectHooks25b() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwD3D9IndexBufferCreate, 0x4C9970);
}

// 0x4C9970
// Indice de 16 bits (D3DFMT_INDEX16 = 0x65) creado por el sistema de recursos D3D; true si el resultado con signo es >= 0 (setge del EXE).
RwBool RwD3D9IndexBufferCreate(RwUInt32 numIndices, void** indexBuffer) {
    constexpr uint32 D3DFMT_INDEX16_VALUE = 0x65;
    return D3DResourceSystem::CreateIndexBuffer(numIndices, D3DFMT_INDEX16_VALUE, indexBuffer) >= 0;
}

RwBool RwD3D9CreateVertexDeclaration(const void* elements, void* vertexdeclaration) {
   return ((RwBool(__cdecl *)(const void*, void*))0x7FAA30)(elements, vertexdeclaration);
}

void RwD3D9DeleteVertexDeclaration() {
   ((void(__cdecl *)(void))0x7FAC10)();
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

void _rwD3D9SetStreams(const RxD3D9VertexStream* streams, RwBool useOffsets) {
    plugin::CallAndReturn<void, 0x7FA090>(streams, useOffsets);
}

// 0x7FAD20 (mov eax, 0C9BF00h; ret): devuelve la direccion fija del D3DCAPS9 de RenderWare (_RwD3D9DeviceCaps). Lote R1A (sin validar).
const void* RwD3D9GetCaps() { // D3DCAPS9*
   return reinterpret_cast<const void*>(0xC9BF00);
}

RwBool RwD3D9CameraIsSphereFullyInsideFrustum(const void* camera, const void* sphere) {
   return ((RwBool(__cdecl *)(const void*, const void*))0x7FAD30)(camera, sphere);
}

RwBool RwD3D9CameraIsBBoxFullyInsideFrustum(const void* camera, const void* boundingBox) {
   return ((RwBool(__cdecl *)(const void*, const void*))0x7FAD90)(camera, boundingBox);
}

void _rwD3D9RasterConvertToNonPalettized(RwRaster* raster) {
   ((void(__cdecl *)(RwRaster*))0x4CD250)(raster);
}

// 0x7F5F60: salto directo a 0x4C9AB0 (registro del plugin de raster D3D9, sin reconstruir; cuerpo del EXE)
RwBool _rwDeviceRegisterPlugin() {
    return ((RwBool(__cdecl *)(void))0x4C9AB0)();
}

void _rwD3D9DeviceSetRestoreCallback(rwD3D9DeviceRestoreCallBack callback) {
   ((void(__cdecl *)(rwD3D9DeviceRestoreCallBack))0x7FAE20)(callback);
}

// 0x7FAE30 (mov eax, [0C980B0h]; ret): devuelve el puntero de la funcion de restauracion guardado por _rwD3D9DeviceSetRestoreCallback. Lote R1A (sin validar).
rwD3D9DeviceRestoreCallBack _rwD3D9DeviceGetRestoreCallback() {
   return StaticRef<rwD3D9DeviceRestoreCallBack>(0xC980B0);
}

// Funciones del controlador D3D9 de RenderWare de tamano trivial (lote R1A, sin validar).
void RwD3D9InjectHooksR1A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(RwD3D9GetCaps, 0x7FAD20);
    RH_ScopedGlobalInstall(_rwD3D9DeviceGetRestoreCallback, 0x7FAE30);
}

// --- Lote R6A (sin validar): estados de render y etapas de textura del controlador D3D9 de RenderWare (0x7FD810..0x7FDE60; el listado las atribuia por vecindad a RpAnisot con confianza baja) ---
// Estado global del EXE que usan (direcciones):
//   0xC9A4C0..0xC9A518, 0xC9A5C8..0xC9A5D0, 0xC99290  valores actuales de los estados de render de RW (uno por RwRenderState; ver _rwD3D9RWGetRenderState)
//   0xC9A508  etapas de textura: entradas de 0x18 bytes con el raster actual en el primer dword    0xC9A50C / 0xC9A510  direccion de textura U / V actuales
//   0xC9A514  filtro de textura actual    0xC9A51C  anisotropia maxima actual    0xC9A5E8  desplazamiento de la extension de RpAnisot en la textura (_rwD3D9SetAnisotropyTextureOffset)
//   0xC98938 + 4 * tipo  copia local de los estados de muestreo de la etapa 0 (U = 0xC9893C, V = 0xC98940, MAG = 0xC9894C, MIN = 0xC98950, MIP = 0xC98954, MAXANISOTROPY = 0xC98960)
//   0xC98B40  lista de estados D3D pendientes de enviar (dwords con el D3DRENDERSTATETYPE) y 0xC9A5EC su contador; cada estado lleva una bandera propia de "ya en la lista"
//   0x88490C  tabla RwBlendFunction -> D3DBLEND (4 bytes por entrada)  0x884950  tabla RwTextureFilterMode -> (D3DTEXF min/mag, D3DTEXF mip) (8 bytes por entrada)
namespace {
// Ranuras de IDirect3DDevice9 (desplazamiento / 4) que usan estas funciones.
constexpr size_t kSlotDeviceSetTexture               = 65;  // +0x104
constexpr size_t kSlotDeviceSetSamplerState          = 69;  // +0x114
constexpr size_t kSlotDeviceSetCurrentTexturePalette = 73;  // +0x124

constexpr uintptr_t kD3D9RasterStages = 0xC9A508;           // RwRaster* por etapa, 0x18 bytes por etapa
constexpr uint32 kD3DRSAlphaTestEnable  = 0x0F;
constexpr uint32 kD3DRSSrcBlend         = 0x13;
constexpr uint32 kD3DRSDestBlend        = 0x14;
constexpr uint32 kD3DRSAlphaBlendEnable = 0x1B;
constexpr uint32 kD3DSAMPMagFilter = 5, kD3DSAMPMinFilter = 6, kD3DSAMPMipFilter = 7, kD3DSAMPMaxAnisotropy = 0xA;

// Si la bandera del estado aun es 0: bandera = 1, lista[contador] = estado y contador + 1 (lista 0xC98B40, contador 0xC9A5EC).
void D3D9QueueRenderState(uint32& flag, uint32 state) {
    if (flag == 0) {
        const uint32 count = StaticRef<uint32>(0xC9A5EC);
        flag = 1;
        reinterpret_cast<uint32*>(0xC98B40)[count] = state;
        StaticRef<uint32>(0xC9A5EC) = count + 1;
    }
}

// Copia de un float por la x87 (fld/fstp de 32 bits): un NaN senalizador se vuelve silencioso (bit 22) y el resto de patrones se conserva.
uint32 X87FloatCopyBitsD3D9(uint32 bits) {
    if ((bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0) {
        bits |= 0x00400000u;
    }
    return bits;
}
} // namespace

// 0x7FD810 (lote R6A, sin validar; el listado decia RpAnisot): lectura de un estado de render del controlador D3D9 (cdecl, (RwRenderState, RwUInt32* valor), `ret`, 517 bytes). Para los estados 1..30 (resta uno y mira
// 0..0x1D sin signo; cualquier otro devuelve 0 sin escribir) salta por la tabla 0x7FDA18: escribe en *valor el dword del estado y devuelve 1. Sin soporte (devuelve 0 sin escribir): 5 (perspectiva), 18 y 19.
// El 2 (direccion de textura) escribe la direccion solo si la U (0xC9A50C) y la V (0xC9A510) coinciden y devuelve 1; si difieren escribe 0 y devuelve 0. El 17 (densidad de niebla) pasa por la x87 (fld/fstp de
// 32 bits: un NaN senalizador sale silencioso).
RwBool _rwD3D9RWGetRenderState(RwInt32 state, void* value) {
    static constexpr uintptr_t sources[30] = {
        0xC9A508, // 1  TEXTURERASTER (raster de la etapa 0)
        0,        // 2  TEXTUREADDRESS (caso aparte)
        0xC9A50C, // 3  TEXTUREADDRESSU
        0xC9A510, // 4  TEXTUREADDRESSV
        0,        // 5  sin soporte
        0xC9A4C4, // 6
        0xC9A504, // 7
        0xC9A4C0, // 8
        0xC9A514, // 9  TEXTUREFILTER
        0xC9A5C8, // 10 SRCBLEND
        0xC9A5CC, // 11 DESTBLEND
        0xC9A4E8, // 12
        0xC9A518, // 13
        0xC9A4F4, // 14
        0xC9A500, // 15
        0xC9A4F8, // 16
        0xC9A4FC, // 17 (float)
        0,        // 18 sin soporte
        0,        // 19 sin soporte
        0xC9A4F0, // 20
        0xC9A4C8, // 21
        0xC9A4CC, // 22
        0xC9A4D0, // 23
        0xC9A4D4, // 24
        0xC9A4D8, // 25
        0xC9A4DC, // 26
        0xC9A4E0, // 27
        0xC9A4E4, // 28
        0xC9A5D0, // 29
        0xC99290, // 30
    };
    const uint32 index = static_cast<uint32>(state) - 1u;
    if (index > 0x1Du) {
        return FALSE;
    }
    uint32* const out = static_cast<uint32*>(value);
    if (index == 1) {
        const uint32 addressU = StaticRef<uint32>(0xC9A50C);
        if (addressU == StaticRef<uint32>(0xC9A510)) {
            *out = addressU;
            return TRUE;
        }
        *out = 0;
        return FALSE;
    }
    const uintptr_t source = sources[index];
    if (source == 0) {
        return FALSE;
    }
    const uint32 bits = *reinterpret_cast<const uint32*>(source);
    *out = (index == 16) ? X87FloatCopyBitsD3D9(bits) : bits;
    return TRUE;
}

// 0x7FDB80 (lote R6A, sin validar; manejador del estado TEXTUREFILTER del controlador D3D9; cdecl, un argumento (RwTextureFilterMode), `ret`, 176 bytes): si la anisotropia maxima actual (0xC9A51C) es mayor que 1
// la deja en 1 (0xC9A51C y la copia 0xC98960) y llama a SetSamplerState(0, MAXANISOTROPY, 1). Si el filtro pedido difiere del actual (0xC9A514): lo guarda, lee de la tabla 0x884950 (indice * 8) el par (min/mag, mip),
// guarda y envia primero MINFILTER (copia 0xC98950) y luego MAGFILTER (0xC9894C) con el primero, y MIPFILTER (0xC98954) solo si el segundo difiere de la copia. Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateTextureFilter(RwInt32 filter) {
    auto& maxAnisotropy = StaticRef<RwInt32>(0xC9A51C);
    if (maxAnisotropy > 1) {
        maxAnisotropy = 1;
        StaticRef<uint32>(0xC98960) = 1;
        ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, 0u, kD3DSAMPMaxAnisotropy, 1u);
    }
    auto& currentFilter = StaticRef<RwInt32>(0xC9A514);
    if (filter != currentFilter) {
        const uint32* const filterTable = reinterpret_cast<const uint32*>(0x884950);
        const uint32 mipFilter = filterTable[filter * 2 + 1];
        const uint32 minMagFilter = filterTable[filter * 2];
        currentFilter = filter;
        StaticRef<uint32>(0xC98950) = minMagFilter;
        ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, 0u, kD3DSAMPMinFilter, minMagFilter);
        StaticRef<uint32>(0xC9894C) = minMagFilter;
        ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, 0u, kD3DSAMPMagFilter, minMagFilter);
        if (StaticRef<uint32>(0xC98954) != mipFilter) {
            StaticRef<uint32>(0xC98954) = mipFilter;
            ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, 0u, kD3DSAMPMipFilter, mipFilter);
        }
    }
    return TRUE;
}

// 0x7FDC30 (lote R6A, sin validar; manejador del estado SRCBLEND; cdecl, un argumento (RwBlendFunction), `ret`, 78 bytes): si difiere del actual (0xC9A5C8): copia D3D (0xC99268) = tabla 0x88490C[modo]; si la bandera
// de "en lista" (0xC9926C) es 0 la pone a 1 y anade D3DRS_SRCBLEND (0x13) a la lista de estados pendientes (0xC98B40, contador 0xC9A5EC); guarda el modo. Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateSrcBlend(RwInt32 blend) {
    auto& currentBlend = StaticRef<RwInt32>(0xC9A5C8);
    if (blend != currentBlend) {
        StaticRef<uint32>(0xC99268) = reinterpret_cast<const uint32*>(0x88490C)[blend];
        D3D9QueueRenderState(StaticRef<uint32>(0xC9926C), kD3DRSSrcBlend);
        currentBlend = blend;
    }
    return TRUE;
}

// 0x7FDC80 (lote R6A, sin validar; manejador del estado DESTBLEND; igual que el anterior con 0xC9A5CC, copia 0xC99270, bandera 0xC99274 y D3DRS_DESTBLEND 0x14). Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateDestBlend(RwInt32 blend) {
    auto& currentBlend = StaticRef<RwInt32>(0xC9A5CC);
    if (blend != currentBlend) {
        StaticRef<uint32>(0xC99270) = reinterpret_cast<const uint32*>(0x88490C)[blend];
        D3D9QueueRenderState(StaticRef<uint32>(0xC99274), kD3DRSDestBlend);
        currentBlend = blend;
    }
    return TRUE;
}

// 0x7FDCD0 (lote R6A, sin validar; cdecl, (RwRaster*, etapa), `ret`, 375 bytes): pone un raster en una etapa de textura. Con la etapa 0: si el raster no es nulo y su extension D3D9 (byte +8) dice que tiene alfa:
// si el indicador 0xC9A4EC era 0 lo pone a 1 y, si el estado 0xC9A4E8 es 0, deja 0xC992A8 = 1 (D3DRS_ALPHABLENDENABLE 0x1B a la lista con su bandera 0xC992AC si no estaba) y copia 0xC9A5D4 a 0xC99248 si difieren
// (solo entonces D3DRS_ALPHATESTENABLE 0x0F a la lista con la bandera 0xC9924C). Si no hay raster o no tiene alfa y 0xC9A4EC era distinto de 0: lo pone a 0 y, con 0xC9A4E8 a 0, deja 0xC992A8 = 0 (misma lista) y,
// si 0xC99248 no era 0, lo pone a 0 y encola ALPHATESTENABLE igual. Despues, para cualquier etapa: si el raster difiere del guardado (0xC9A508 + etapa * 0x18) lo guarda y llama a SetTexture(etapa, textura de la
// extension) o a SetTexture(etapa, NULL) sin raster y, con paleta (extension +4), a SetCurrentTexturePalette(paleta + 0x400). Devuelve siempre TRUE. Sin comprobar el rango de la etapa.
RwBool _rwD3D9RWSetRasterStage(RwRaster* raster, RwUInt32 stage) {
    D3D9RasterExtLayout* const ext = raster != nullptr ? &D3D9RasterExt(raster) : nullptr;
    if (stage == 0) {
        bool queueAlphaTest = false;
        if (raster != nullptr && ext->alpha != 0) {
            if (StaticRef<uint32>(0xC9A4EC) == 0) {
                const uint32 vertexAlpha = StaticRef<uint32>(0xC9A4E8);
                StaticRef<uint32>(0xC9A4EC) = 1;
                if (vertexAlpha == 0) {
                    StaticRef<uint32>(0xC992A8) = 1;
                    D3D9QueueRenderState(StaticRef<uint32>(0xC992AC), kD3DRSAlphaBlendEnable);
                    const uint32 alphaTest = StaticRef<uint32>(0xC9A5D4);
                    if (StaticRef<uint32>(0xC99248) != alphaTest) {
                        StaticRef<uint32>(0xC99248) = alphaTest;
                        queueAlphaTest = true;
                    }
                }
            }
        } else if (StaticRef<uint32>(0xC9A4EC) != 0) {
            const uint32 vertexAlpha = StaticRef<uint32>(0xC9A4E8);
            StaticRef<uint32>(0xC9A4EC) = 0;
            if (vertexAlpha == 0) {
                StaticRef<uint32>(0xC992A8) = 0;
                D3D9QueueRenderState(StaticRef<uint32>(0xC992AC), kD3DRSAlphaBlendEnable);
                if (StaticRef<uint32>(0xC99248) != 0) {
                    StaticRef<uint32>(0xC99248) = 0;
                    queueAlphaTest = true;
                }
            }
        }
        if (queueAlphaTest) {
            D3D9QueueRenderState(StaticRef<uint32>(0xC9924C), kD3DRSAlphaTestEnable);
        }
    }
    RwRaster*& stageRaster = *reinterpret_cast<RwRaster**>(kD3D9RasterStages + stage * 0x18);
    if (raster == stageRaster) {
        return TRUE;
    }
    stageRaster = raster;
    if (raster == nullptr) {
        ComCall<kSlotDeviceSetTexture, int32>(D3D9Device, stage, static_cast<void*>(nullptr));
        return TRUE;
    }
    ComCall<kSlotDeviceSetTexture, int32>(D3D9Device, stage, ext->texture);
    if (ext->palette != nullptr) {
        ComCall<kSlotDeviceSetCurrentTexturePalette, int32>(D3D9Device, *reinterpret_cast<uint32*>(static_cast<uint8*>(ext->palette) + 0x400));
    }
    return TRUE;
}

// 0x7FDE50 (lote R6A, sin validar; cdecl, un argumento, `ret`, 15 bytes): raster guardado de la etapa (0xC9A508 + etapa * 0x18). Sin comprobar el rango.
RwRaster* _rwD3D9RWGetRasterStage(RwUInt32 stage) {
    return *reinterpret_cast<RwRaster**>(kD3D9RasterStages + stage * 0x18);
}

// 0x7FDE60 (lote R6A, sin validar; cdecl, un argumento, `ret`, 10 bytes): guarda en 0xC9A5E8 el desplazamiento de la extension de RpAnisot dentro de la textura (la llama RpAnisotPluginAttach con el
// desplazamiento de la textura). Nombre inferido.
void _rwD3D9SetAnisotropyTextureOffset(RwInt32 offset) {
    StaticRef<RwInt32>(0xC9A5E8) = offset;
}

// Estados de render y etapas de textura del controlador D3D9 (lote R6A, sin validar).
void RwD3D9InjectHooksR6A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(_rwD3D9RWGetRenderState, 0x7FD810);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateTextureFilter, 0x7FDB80);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateSrcBlend, 0x7FDC30);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateDestBlend, 0x7FDC80);
    RH_ScopedGlobalInstall(_rwD3D9RWSetRasterStage, 0x7FDCD0);
    RH_ScopedGlobalInstall(_rwD3D9RWGetRasterStage, 0x7FDE50);
    RH_ScopedGlobalInstall(_rwD3D9SetAnisotropyTextureOffset, 0x7FDE60);
}

RwImage* RwImageResample(RwImage* dstImage, const RwImage* srcImage) {
    return ((RwImage*(__cdecl *)(RwImage*, const RwImage*))0x80C600)(dstImage, srcImage);
}

RwImage* RwImageCreateResample(const RwImage* srcImage, RwInt32 width, RwInt32 height) {
    return ((RwImage*(__cdecl *)(const RwImage*, RwInt32, RwInt32))0x80CD10)(srcImage, width, height);
}

RwImage* RwImageSetFromRaster(RwImage* image, RwRaster* raster) {
    return ((RwImage*(__cdecl *)(RwImage*, RwRaster*))0x804250)(image, raster);
}

RwRaster* RwRasterSetFromImage(RwRaster* raster, RwImage* image) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwImage*))0x804290)(raster, image);
}

RwRaster* RwRasterRead(const RwChar* filename) {
    return ((RwRaster*(__cdecl *)(const RwChar*))0x8043F0)(filename);
}

RwRaster* RwRasterReadMaskedRaster(const RwChar* filename, const RwChar* maskname) {
    return ((RwRaster*(__cdecl *)(const RwChar*, const RwChar*))0x8044E0)(filename, maskname);
}

RwImage* RwImageFindRasterFormat(RwImage* ipImage, RwInt32 nRasterType, RwInt32* npWidth, RwInt32* npHeight, RwInt32* npDepth, RwInt32* npFormat) {
    return ((RwImage*(__cdecl *)(RwImage*, RwInt32, RwInt32*, RwInt32*, RwInt32*, RwInt32*))0x8042C0)(ipImage, nRasterType, npWidth, npHeight, npDepth, npFormat);
}

RwFrame* RwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, void* data) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwObjectCallBack, void*))0x7F1200)(frame, callBack, data);
}

RwFrame* RwFrameTranslate(RwFrame* frame, const RwV3d* v, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwV3d*, RwOpCombineType))0x7F0E30)(frame, v, combine);
}

RwFrame* RwFrameRotate(RwFrame* frame, const RwV3d* axis, RwReal angle, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwV3d*, RwReal, RwOpCombineType))0x7F1010)(frame, axis, angle, combine);
}

RwFrame* RwFrameScale(RwFrame* frame, const RwV3d* v, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwV3d*, RwOpCombineType))0x7F0ED0)(frame, v, combine);
}

RwFrame* RwFrameTransform(RwFrame* frame, const RwMatrix* m, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwMatrix*, RwOpCombineType))0x7F0F70)(frame, m, combine);
}

RwFrame* RwFrameOrthoNormalize(RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F1170)(frame);
}

RwFrame* RwFrameSetIdentity(RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F10B0)(frame);
}

RwFrame* RwFrameCloneHierarchy(RwFrame* root) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F0250)(root);
}

RwBool RwFrameDestroyHierarchy(RwFrame* frame) {
    return ((RwBool(__cdecl *)(RwFrame*))0x7F08A0)(frame);
}

RwFrame* RwFrameForAllChildren(RwFrame* frame, RwFrameCallBack callBack, void* data) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwFrameCallBack, void*))0x7F0DC0)(frame, callBack, data);
}

RwFrame* RwFrameRemoveChild(RwFrame* child) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F0CD0)(child);
}

RwFrame* RwFrameAddChild(RwFrame* parent, RwFrame* child) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwFrame*))0x7F0B00)(parent, child);
}

RwFrame* RwFrameAddChildNoUpdate(RwFrame* parent, RwFrame* child) {
    return ((RwFrame*(__cdecl *)(RwFrame*, RwFrame*))0x7F09C0)(parent, child);
}

RwFrame* RwFrameGetRoot(const RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(const RwFrame*))0x7F09B0)(frame);
}

RwMatrix* RwFrameGetLTM(RwFrame* frame) {
    return ((RwMatrix*(__cdecl *)(RwFrame*))0x7F0990)(frame);
}

RwFrame* RwFrameUpdateObjects(RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F0910)(frame);
}

void RwFrameSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7EFED0)(blockSize, numBlocksToPrealloc);
}

RwFrame* RwFrameCreate() {
    return ((RwFrame*(__cdecl *)(void))0x7F0410)();
}

RwBool RwFrameDestroy(RwFrame* frame) {
    assert(frame);
    return ((RwBool(__cdecl *)(RwFrame*))0x7F05A0)(frame);
}

RwBool _rwFrameInit(RwFrame* frame) {
    return ((RwBool(__cdecl *)(RwFrame*))0x7F0450)(frame);
}

RwBool _rwFrameDeInit(RwFrame* frame) {
    return ((RwBool(__cdecl *)(RwFrame*))0x7F06F0)(frame);
}

// 0x7F0340: bits 0 y 1 del indicador privado del marco RAIZ (frame->root, +0xA0; privateFlags en +3). Lee la raiz sin comprobar nulos.
static_assert(offsetof(RwFrame, root) == 0xA0 && offsetof(RwObject, privateFlags) == 3);
RwBool RwFrameDirty(const RwFrame* frame) {
    return frame->root->object.privateFlags & 3;
}

RwInt32 RwFrameCount(RwFrame* frame) {
    return ((RwInt32(__cdecl *)(RwFrame*))0x7F0E00)(frame);
}

RwInt32 RwFrameRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return ((RwInt32(__cdecl *)(RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x7F1260)(size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwFrameGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7F1290)(pluginID);
}

RwBool RwFrameValidatePlugins(const RwFrame* frame) {
    return ((RwBool(__cdecl *)(const RwFrame*))0x7F12B0)(frame);
}

RwFrame* _rwFrameCloneAndLinkClones(RwFrame* root) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7EFFB0)(root);
}

RwFrame* _rwFramePurgeClone(RwFrame* root) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F01A0)(root);
}

RwBool _rwFrameSyncDirty() {
    return ((RwBool(__cdecl *)(void))0x809550)();
}

void _rwFrameSyncHierarchyLTM(RwFrame* frame) {
    ((void(__cdecl *)(RwFrame*))0x8097D0)(frame);
}

// 0x804550: _rwPluginRegistryAddPluginStream(&textureTKList, ...)
RwInt32 RwTextureRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return _rwPluginRegistryAddPluginStream(&TextureTKList, pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RwTextureSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x804580)(pluginID, alwaysCB);
}

RwUInt32 RwTextureStreamGetSize(const RwTexture* texture) {
    return ((RwUInt32(__cdecl *)(const RwTexture*))0x8045A0)(texture);
}

RwTexture* RwTextureStreamRead(RwStream* stream) {
    return ((RwTexture*(__cdecl *)(RwStream*))0x8046E0)(stream);
}

const RwTexture* RwTextureStreamWrite(const RwTexture* texture, RwStream* stream) {
    return ((const RwTexture*(__cdecl *)(const RwTexture*, RwStream*))0x8045E0)(texture, stream);
}

RwInt32 RwTexDictionaryRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x8048E0)(pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RwTexDictionarySetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x804910)(pluginID, alwaysCB);
}

RwUInt32 RwTexDictionaryStreamGetSize(const RwTexDictionary* texDict) {
    return ((RwUInt32(__cdecl *)(const RwTexDictionary*))0x804930)(texDict);
}

RwTexDictionary* RwTexDictionaryStreamRead(RwStream* stream) {
    return ((RwTexDictionary*(__cdecl *)(RwStream*))0x804C30)(stream);
}

const RwTexDictionary* RwTexDictionaryStreamWrite(const RwTexDictionary* texDict, RwStream* stream) {
    return ((const RwTexDictionary*(__cdecl *)(const RwTexDictionary*, RwStream*))0x8049F0)(texDict, stream);
}

RwTextureChunkInfo* _rwTextureChunkInfoRead(RwStream* stream, RwTextureChunkInfo* textureChunkInfo, RwInt32* bytesRead) {
    return ((RwTextureChunkInfo*(__cdecl *)(RwStream*, RwTextureChunkInfo*, RwInt32*))0x804E60)(stream, textureChunkInfo, bytesRead);
}

void RwFrameListSetAutoUpdate(RwBool flag) {
    ((void(__cdecl *)(RwBool))0x807570)(flag);
}

RwInt32 RwFrameRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x807580)(pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RwFrameSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x8075B0)(pluginID, alwaysCB);
}

rwFrameList* _rwFrameListInitialize(rwFrameList* frameList, RwFrame* frame) {
    return ((rwFrameList*(__cdecl *)(rwFrameList*, RwFrame*))0x8075D0)(frameList, frame);
}

RwBool _rwFrameListFindFrame(const rwFrameList* frameList, const RwFrame* frame, RwInt32* npIndex) {
    return ((RwBool(__cdecl *)(const rwFrameList*, const RwFrame*, RwInt32*))0x8076E0)(frameList, frame, npIndex);
}

// 0x807720: si numFrames (+4) no es 0 libera el vector `frames` (+0) con la función rwfree del motor ([RwEngineInstance + 0x138], cdecl, un argumento)
// y devuelve la propia lista. El EXE comprueba numFrames, no el puntero.
rwFrameList* _rwFrameListDeinitialize(rwFrameList* frameList) {
    if (frameList->numFrames) {
        RwFree(frameList->frames);
    }
    return frameList;
}

RwUInt32 _rwFrameListStreamGetSize(const rwFrameList* frameList) {
    return ((RwUInt32(__cdecl *)(const rwFrameList*))0x807750)(frameList);
}

rwFrameList* _rwFrameListStreamRead(RwStream* stream, rwFrameList* fl) {
    return ((rwFrameList*(__cdecl *)(RwStream*, rwFrameList*))0x807970)(stream, fl);
}

const rwFrameList* _rwFrameListStreamWrite(const rwFrameList* frameList, RwStream* stream) {
    return ((const rwFrameList*(__cdecl *)(const rwFrameList*, RwStream*))0x8077A0)(frameList, stream);
}

RwBBox* RwBBoxCalculate(RwBBox* boundBox, const RwV3d* verts, RwInt32 numVerts) {
    return ((RwBBox*(__cdecl *)(RwBBox*, const RwV3d*, RwInt32))0x808F60)(boundBox, verts, numVerts);
}

RwBBox* RwBBoxInitialize(RwBBox* boundBox, const RwV3d* vertex) {
    return ((RwBBox*(__cdecl *)(RwBBox*, const RwV3d*))0x809020)(boundBox, vertex);
}

RwBBox* RwBBoxAddPoint(RwBBox* boundBox, const RwV3d* vertex) {
    return ((RwBBox*(__cdecl *)(RwBBox*, const RwV3d*))0x809060)(boundBox, vertex);
}

RwBool RwBBoxContainsPoint(const RwBBox* boundBox, const RwV3d* vertex) {
    return ((RwBool(__cdecl *)(const RwBBox*, const RwV3d*))0x8090E0)(boundBox, vertex);
}

RwCamera* RwCameraBeginUpdate(RwCamera* camera) {
    ZoneScoped;
    return camera->beginUpdate(camera);
}

RwCamera* RwCameraEndUpdate(RwCamera* camera) {
    ZoneScoped;
    return camera->endUpdate(camera);
}

RwCamera* RwCameraClear(RwCamera* camera, RwRGBA* colour, RwInt32 clearMode) {
    ZoneScoped;
    return RwEngineInstance->stdFunc[rwSTANDARDCAMERACLEAR](camera, colour, clearMode) ? camera : nullptr;
}

RwCamera* RwCameraShowRaster(RwCamera* camera, void* pDev, RwUInt32 flags) {
    ZoneScoped;
    return RwRasterShowRaster(RwCameraGetRaster(camera), pDev, flags) != 0 ? camera : nullptr;
}

void RwCameraSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7EE0F0)(blockSize, numBlocksToPrealloc);
}

RwBool RwCameraDestroy(RwCamera* camera) {
    return ((RwBool(__cdecl *)(RwCamera*))0x7EE4B0)(camera);
}

RwCamera* RwCameraCreate() {
    return ((RwCamera*(__cdecl *)(void))0x7EE4F0)();
}

RwCamera* RwCameraClone(RwCamera* camera) {
    return ((RwCamera*(__cdecl *)(RwCamera*))0x7EF3B0)(camera);
}

// Direcciones del EXE usadas por las funciones del lote 25 (g25c) de este archivo.
namespace {
constexpr uintptr_t kCameraPluginRegistry = 0x8E222C; // RwCamera: lista de plugins
}

void RwCoreFrameCameraInjectHooks() {
    RH_ScopedNamespaceName("RwCore");
    RH_ScopedCategory("Renderware");

    RH_ScopedGlobalInstall(RwFrameDirty, 0x7F0340);
    RH_ScopedGlobalInstall(RwCameraSetViewOffset, 0x7EE1A0);
    RH_ScopedGlobalInstall(RwCameraRegisterPlugin, 0x7EE450);
}

// 0x7EE1A0: copia los dos dwords del desplazamiento a camera->viewOffset (+0x78/+0x7C) en este orden (lee x, escribe x, lee y, escribe y: importa si se
// solapan), sin pasar por ninguna operacion de coma flotante (copia de bits), y, si la camara tiene marco padre (object.parent, +4), llama a
// RwFrameUpdateObjects (0x7F0910) con el. Devuelve la camara.
static_assert(offsetof(RwCamera, viewOffset) == 0x78 && offsetof(RwCamera, object) + offsetof(RwObjectHasFrame, object) + offsetof(RwObject, parent) == 4);
RwCamera* RwCameraSetViewOffset(RwCamera* camera, const RwV2d* offset) {
    const auto* const source = reinterpret_cast<const uint32*>(offset);
    auto* const target = reinterpret_cast<uint32*>(&camera->viewOffset);
    target[0] = source[0];
    target[1] = source[1];
    if (auto* const parent = static_cast<RwFrame*>(camera->object.object.parent)) {
        RwFrameUpdateObjects(parent);
    }
    return camera;
}

RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* viewWindow) {
    return ((RwCamera*(__cdecl *)(RwCamera*, const RwV2d*))0x7EE410)(camera, viewWindow);
}

RwCamera* RwCameraSetProjection(RwCamera* camera, RwCameraProjection projection) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RwCameraProjection))0x7EE3A0)(camera, projection);
}

RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal nearClip) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RwReal))0x7EE1D0)(camera, nearClip);
}

RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal farClip) {
    return ((RwCamera*(__cdecl *)(RwCamera*, RwReal))0x7EE2A0)(camera, farClip);
}

// 0x7EE450: registra el plugin en la lista de RwCamera (0x8E222C) con _rwPluginRegistryAddPlugin (0x8084A0).
RwInt32 RwCameraRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(reinterpret_cast<RwPluginRegistry*>(kCameraPluginRegistry), size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwCameraGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7EE480)(pluginID);
}

RwBool RwCameraValidatePlugins(const RwCamera* camera) {
    return ((RwBool(__cdecl *)(const RwCamera*))0x7EE4A0)(camera);
}

RwFrustumTestResult RwCameraFrustumTestSphere(const RwCamera* camera, const RwSphere* sphere) {
    return ((RwFrustumTestResult(__cdecl *)(const RwCamera*, const RwSphere*))0x7EE2D0)(camera, sphere);
}

RwInt32 RwCameraRegisterPluginStream(RwUInt32 pluginID, RwPluginDataChunkReadCallBack readCB, RwPluginDataChunkWriteCallBack writeCB, RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x808C90)(pluginID, readCB, writeCB, getSizeCB);
}

RwInt32 RwCameraSetStreamAlwaysCallBack(RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    return ((RwInt32(__cdecl *)(RwUInt32, RwPluginDataChunkAlwaysCallBack))0x808CC0)(pluginID, alwaysCB);
}

RwUInt32 RwCameraStreamGetSize(const RwCamera* camera) {
    return ((RwUInt32(__cdecl *)(const RwCamera*))0x808CE0)(camera);
}

RwCamera* RwCameraStreamRead(RwStream* stream) {
    return ((RwCamera*(__cdecl *)(RwStream*))0x808DE0)(stream);
}

const RwCamera* RwCameraStreamWrite(const RwCamera* camera, RwStream* stream) {
    return ((const RwCamera*(__cdecl *)(const RwCamera*, RwStream*))0x808D00)(camera, stream);
}

RwCameraChunkInfo* RwCameraChunkInfoRead(RwStream* stream, RwCameraChunkInfo* cameraChunkInfo, RwInt32* bytesRead) {
    return ((RwCameraChunkInfo*(__cdecl *)(RwStream*, RwCameraChunkInfo*, RwInt32*))0x808EF0)(stream, cameraChunkInfo, bytesRead);
}

void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame) {
    plugin::Call<0x804EF0, void*, RwFrame*>(object, frame);
}

// 0x804F40: si el objeto tiene marco (object.parent), lo saca de la lista de objetos del marco (sin borrar `parent`).
void _rwObjectHasFrameReleaseFrame(void *object) {
    auto* const hasFrame = static_cast<RwObjectHasFrame*>(object);
    if (hasFrame->object.parent == nullptr) {
        return;
    }
    rwLinkListRemoveLLLink(&hasFrame->lFrame);
}

// 0x803740: stdFunc[rwSTANDARDRGBTOPIXEL](&pixel, rgbIn, rasterFormat). El EXE reutiliza como salida la ranura del argumento
// `rasterFormat`: si la funcion estandar no escribe, el resultado es el propio `rasterFormat`.
RwUInt32 RwRGBAToPixel(RwRGBA* rgbIn, RwInt32 rasterFormat) {
    RwUInt32 pixel = static_cast<RwUInt32>(rasterFormat);
    RwEngineInstance->stdFunc[rwSTANDARDRGBTOPIXEL](&pixel, rgbIn, rasterFormat);
    return pixel;
}
