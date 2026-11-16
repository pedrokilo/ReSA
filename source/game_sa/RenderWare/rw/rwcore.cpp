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
#include "X87Math.h"

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
    uint8 pad00[0x08];
    RwFreeList* textureFreeList; // +0x08 (lista libre de RwTexture; lote R7A)
    uint8 pad0[0x10];
    RwBool mipmapping;          // +0x1C
    RwBool autoMipmapping;      // +0x20
    uint8 pad1[0x08];
    RwTextureCallBackMipmapGeneration mipmapGenerationCallBack; // +0x2C
    RwTextureCallBackMipmapName mipmapNameCallBack;             // +0x30 (lote R7A)
};
static_assert(offsetof(TextureGlobalsLayout, mipmapping) == 0x1C && offsetof(TextureGlobalsLayout, autoMipmapping) == 0x20
           && offsetof(TextureGlobalsLayout, mipmapGenerationCallBack) == 0x2C);
static_assert(offsetof(TextureGlobalsLayout, textureFreeList) == 0x08 && offsetof(TextureGlobalsLayout, mipmapNameCallBack) == 0x30);

struct RasterGlobalsLayout {    // pila de contextos de RwRasterPushContext/PopContext (bardevice/baraster.c)
    RwRaster* rasterStack[10];  // +0x00
    RwInt32   rasterSP;         // +0x28
};
static_assert(offsetof(RasterGlobalsLayout, rasterSP) == 0x28);

struct D3D9DynamicVertexBuffer { // nodo de la lista 0xC97B8C (0x14 bytes)
    uint32 size;                 // +0x00
    uint32 inUse;                // +0x04 (RwD3D9DynamicVertexBufferDestroy lo pone a 0)
    void*  vertexBuffer;         // +0x08 IDirect3DVertexBuffer9*
    void** owner;                // +0x0C direccion de la variable del llamador que guarda el bufer (RwD3D9DynamicVertexBufferDestroy la pone a 0)
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
constexpr size_t kSlotVertexBufferLock = 11;            // IDirect3DVertexBuffer9::Lock (+0x2C; lote R11A)
constexpr size_t kSlotVertexBufferUnlock = 12;          // IDirect3DVertexBuffer9::Unlock (+0x30)
constexpr size_t kSlotSetDepthStencilSurface = 39;      // IDirect3DDevice9::SetDepthStencilSurface (+0x9C)
constexpr size_t kSlotCreatePixelShader = 106;          // IDirect3DDevice9::CreatePixelShader (+0x1A8)
constexpr size_t kSlotSetRenderState = 57;              // IDirect3DDevice9::SetRenderState (+0xE4; lote R10A)
constexpr size_t kSlotSetTextureStageState = 67;        // IDirect3DDevice9::SetTextureStageState (+0x10C; lote R10A)
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

// 0x8057B0 (lote R11A, sin validar; 102 bytes): reserva un RxPipeline (0x34 bytes) de la lista libre del modulo de tuberias ([RwEngineInstance + [0xC9BC60]], pista 0x30409) y lo pone a cero.
// Sin memoria: RwError { pluginID = 1, errorCode = _rwerror(0x80000013, 0x34) } con RwErrorSet y devuelve NULL.
RxPipeline* RxPipelineCreate() {
    auto* const freeList = *reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9BC60));
    void* const pipeline = RwFreeListAlloc(freeList, 0x30409);
    if (pipeline == nullptr) {
        RwError error;
        error.pluginID  = 1;
        error.errorCode = _rwerror(static_cast<RwInt32>(0x80000013), 0x34);
        RwErrorSet(&error);
        return nullptr;
    }
    memset(pipeline, 0, 0x34);
    return static_cast<RxPipeline*>(pipeline);
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

// --- Lote R7A (sin validar): gestor de buferes de vertices D3D9 de RenderWare (0x7F5500..0x7F5A00; el listado dejaba 0x7F5800..0x7F5940 en RpSkin/D3DX9 con confianza baja) ---
// Estado del EXE (direcciones): 0xC97B7C lista de grupos por paso (stride); 0xC97B80 / 0xC97B84 / 0xC97B88 / 0xC97B90 listas libres de grupos, bloques libres, entradas de bufer y nodos dinamicos;
// 0x8E2414 tamano por defecto de un bufer gestionado (RwD3D9VertexBufferManagerChangeDefaultSize); 0xC97B8C lista de buferes dinamicos; 0xC97B94 indice del bufer dinamico actual;
// 0xC97B98[4] posiciones, 0xC97BA8[4] tamanos y 0xC97BB8[4] buferes dinamicos activos; 0xC9BF1C = D3DCAPS9.DevCaps (bit 0x10000 = HWTRANSFORMANDLIGHT: buferes dinamicos en pool DEFAULT).
namespace {
struct VertexBufferFreeBlock {  // 0x14 bytes (lista libre 0xC97B84): tramo libre de un bufer gestionado, lista doble por grupo
    uint32 offset;              // +0x00
    uint32 size;                // +0x04
    void*  vertexBuffer;        // +0x08
    VertexBufferFreeBlock* next; // +0x0C
    VertexBufferFreeBlock* prev; // +0x10
};
struct VertexBufferEntry {      // 8 bytes (lista libre 0xC97B88): un bufer D3D creado para el grupo
    void* vertexBuffer;         // +0x00
    VertexBufferEntry* next;    // +0x04
};
struct VertexBufferGroup {      // 0x10 bytes (lista libre 0xC97B80): un paso (stride)
    uint32 stride;              // +0x00
    VertexBufferFreeBlock* freeBlocks; // +0x04
    VertexBufferEntry* vertexBuffers;  // +0x08
    VertexBufferGroup* next;    // +0x0C
};
static_assert(sizeof(VertexBufferFreeBlock) == 0x14 && sizeof(VertexBufferEntry) == 8 && sizeof(VertexBufferGroup) == 0x10);

auto& VertexBufferGroups           = StaticRef<VertexBufferGroup*>(0xC97B7C);
auto& VertexBufferGroupFreeList    = StaticRef<RwFreeList*>(0xC97B80);
auto& VertexBufferBlockFreeList    = StaticRef<RwFreeList*>(0xC97B84);
auto& VertexBufferEntryFreeList    = StaticRef<RwFreeList*>(0xC97B88);
auto& DynamicVertexBufferFreeList  = StaticRef<RwFreeList*>(0xC97B90);
auto& VertexBufferDefaultSize      = StaticRef<uint32>(0x8E2414);
auto& DynamicVertexBufferCurrent   = StaticRef<uint32>(0xC97B94);
uint32 (&DynamicVertexBufferPosition)[4] = *reinterpret_cast<uint32(*)[4]>(0xC97B98);
uint32 (&DynamicVertexBufferSize)[4]     = *reinterpret_cast<uint32(*)[4]>(0xC97BA8);
void*  (&DynamicVertexBufferActive)[4]   = *reinterpret_cast<void*(*)[4]>(0xC97BB8);
auto& D3D9DeviceDevCaps            = StaticRef<uint32>(0xC9BF1C);

constexpr size_t kSlotDeviceCreateVertexBuffer = 26;    // IDirect3DDevice9::CreateVertexBuffer (+0x68)
constexpr uint32 kDevCapsHardwareTransformAndLight = 0x10000;
// RwEngineInstance + 0x144 / +0x148: memoryAlloc / memoryFree (RwFreeListAlloc / RwFreeListFree).
static_assert(offsetof(RwGlobals, memoryAlloc) == 0x144 && offsetof(RwGlobals, memoryFree) == 0x148);
} // namespace

// 0x7F5500 (lote R7A, sin validar): (stride, size, void** vertexBuffer, uint32* offset). Busca el grupo del paso (lo crea con la lista libre 0xC97B80, pista 0x30411, y lo pone al principio de la lista
// 0xC97B7C) y en el un tramo libre con size >= el pedido (el primero de la lista). Si no hay: crea un bloque (lista 0xC97B84) de (defaultSize + stride - 1) / stride * stride bytes (el pedido si es >= ese
// valor), con CreateVertexBuffer(longitud, D3DUSAGE_WRITEONLY 8, FVF 0, D3DPOOL_MANAGED 1); si el HRESULT es distinto de 0 libera el bloque y devuelve FALSE; si no lo pone al principio de los libres
// y registra el bufer en una entrada (0xC97B88). Entrega bufer y offset del tramo, avanza su offset y reduce su tamano; si queda a 0 lo saca de la lista y lo libera. Devuelve TRUE.
RwBool RwD3D9CreateVertexBuffer(RwUInt32 stride, RwUInt32 size, void** vertexBuffer, RwUInt32* offset) {
    VertexBufferGroup* group = VertexBufferGroups;
    while (group != nullptr && stride != group->stride) {
        group = group->next;
    }
    if (group == nullptr) {
        group = static_cast<VertexBufferGroup*>(RwFreeListAlloc(VertexBufferGroupFreeList, 0x30411));
        group->stride        = stride;
        group->freeBlocks    = nullptr;
        group->vertexBuffers = nullptr;
        group->next          = VertexBufferGroups;
        VertexBufferGroups   = group;
    }
    VertexBufferFreeBlock* block = group->freeBlocks;
    while (block != nullptr && block->size < size) {
        block = block->next;
    }
    if (block == nullptr) {
        block = static_cast<VertexBufferFreeBlock*>(RwFreeListAlloc(VertexBufferBlockFreeList, 0x30411));
        block->offset = 0;
        const uint32 rounded = (VertexBufferDefaultSize + stride - 1) / stride * stride;
        block->size = rounded;
        if (!(size < rounded)) {
            block->size = size;
        }
        if (ComCall<kSlotDeviceCreateVertexBuffer, int32>(D3D9Device, block->size, 8u, 0u, 1u, &block->vertexBuffer, static_cast<void*>(nullptr)) != 0) {
            RwFreeListFree(VertexBufferBlockFreeList, block);
            return FALSE;
        }
        block->prev = nullptr;
        block->next = group->freeBlocks;
        if (group->freeBlocks != nullptr) {
            group->freeBlocks->prev = block;
        }
        group->freeBlocks = block;
        VertexBufferEntry* const entry = static_cast<VertexBufferEntry*>(RwFreeListAlloc(VertexBufferEntryFreeList, 0x30411));
        entry->vertexBuffer = block->vertexBuffer;
        entry->next         = group->vertexBuffers;
        group->vertexBuffers = entry;
    }
    *vertexBuffer = block->vertexBuffer;
    *offset = block->offset;
    block->offset += size;
    block->size   -= size;
    if (!(block->size > 0)) {
        if (block->prev != nullptr) {
            block->prev->next = block->next;
        }
        if (block->next != nullptr) {
            block->next->prev = block->prev;
        }
        if (group->freeBlocks == block) {
            group->freeBlocks = block->next;
        }
        RwFreeListFree(VertexBufferBlockFreeList, block);
    }
    return TRUE;
}

// 0x7F56A0 (lote R7A, sin validar): (stride, size, vertexBuffer, offset). Devuelve un tramo al grupo de su paso (si no hay grupo no hace nada). Busca en los tramos libres del mismo bufer uno contiguo (el termina
// donde empieza el devuelto, o el devuelto termina donde empieza el): si no hay crea un tramo nuevo (0xC97B84, pista 0x30411) al principio de la lista. Si hay lo ensancha (por detras: size +=; por delante:
// offset = el devuelto y size +=) y despues repite desde el principio de la lista: mientras haya otro tramo del mismo bufer contiguo al resultante, saca el resultante de la lista, lo libera y fusiona sus
// limites en el vecino. No libera los buferes D3D. Los nodos de la lista se tratan como en el EXE (sin comprobar solapes).
void RwD3D9DestroyVertexBuffer(RwUInt32 stride, RwUInt32 size, void* vertexBuffer, RwUInt32 offset) {
    VertexBufferGroup* group = VertexBufferGroups;
    while (group != nullptr && stride != group->stride) {
        group = group->next;
    }
    if (group == nullptr) {
        return;
    }
    VertexBufferFreeBlock* target = group->freeBlocks;
    while (target != nullptr) {
        if (target->vertexBuffer == vertexBuffer && (target->offset + target->size == offset || size + offset == target->offset)) {
            break;
        }
        target = target->next;
    }
    if (target == nullptr) {
        VertexBufferFreeBlock* const block = static_cast<VertexBufferFreeBlock*>(RwFreeListAlloc(VertexBufferBlockFreeList, 0x30411));
        block->offset       = offset;
        block->size         = size;
        block->vertexBuffer = vertexBuffer;
        block->prev         = nullptr;
        block->next         = group->freeBlocks;
        if (group->freeBlocks != nullptr) {
            group->freeBlocks->prev = block;
        }
        group->freeBlocks = block;
        return;
    }
    uint32 mergedOffset = offset;
    uint32 mergedSize   = size;
    for (;;) {
        uint32 newSize = target->size;
        const uint32 targetOffset = target->offset;
        bool grow = false;
        if (targetOffset + newSize == mergedOffset) {
            grow = true;
        } else if (mergedSize + mergedOffset == targetOffset) {
            target->offset = mergedOffset;
            grow = true;
        }
        if (grow) {
            newSize += mergedSize;
            target->size = newSize;
        }
        mergedOffset = target->offset;
        mergedSize   = target->size;
        VertexBufferFreeBlock* const merged = target;
        VertexBufferFreeBlock* neighbour = group->freeBlocks;
        while (neighbour != nullptr) {
            if (neighbour->vertexBuffer == vertexBuffer && (neighbour->offset + neighbour->size == mergedOffset || mergedSize + mergedOffset == neighbour->offset)) {
                break;
            }
            neighbour = neighbour->next;
        }
        if (neighbour == nullptr) {
            return;
        }
        if (merged->prev != nullptr) {
            merged->prev->next = merged->next;
        }
        if (merged->next != nullptr) {
            merged->next->prev = merged->prev;
        }
        if (group->freeBlocks == merged) {
            group->freeBlocks = merged->next;
        }
        RwFreeListFree(VertexBufferBlockFreeList, merged);
        target = neighbour;
    }
}

// 0x7F5A00 (lote R7A, sin validar): (size, void** vertexBuffer). Recorre la lista de buferes dinamicos (0xC97B8C): un nodo libre (inUse == 0) del mismo tamano se reutiliza (inUse = 1, entrega su bufer,
// owner = vertexBuffer, TRUE); si no, el candidato es el libre MAS GRANDE. Con candidato: Release de su bufer (el EXE desreferencia aunque sea NULL) y a NULL; sin candidato: nodo nuevo (lista libre
// 0xC97B90, pista 0x40411) al principio de la lista. Despues CreateVertexBuffer(size, D3DUSAGE_WRITEONLY|DYNAMIC 0x208, FVF 0, pool 0 si DevCaps & 0x10000 o 2 si no); un HRESULT distinto de 0 devuelve
// FALSE (el nodo queda tal cual); si no guarda size, inUse = 1, el bufer y owner = vertexBuffer y devuelve TRUE.
RwBool RwD3D9DynamicVertexBufferCreate(RwUInt32 size, void** vertexBuffer) {
    D3D9DynamicVertexBuffer* candidate = nullptr;
    for (D3D9DynamicVertexBuffer* node = DynamicVertexBufferList; node != nullptr; node = node->next) {
        if (node->inUse != 0) {
            continue;
        }
        if (node->size == size) {
            node->inUse = 1;
            *vertexBuffer = node->vertexBuffer;
            node->owner = vertexBuffer;
            return TRUE;
        }
        if (candidate == nullptr || candidate->size < node->size) {
            candidate = node;
        }
    }
    D3D9DynamicVertexBuffer* node = candidate;
    if (node != nullptr) {
        ComCall<kSlotRelease, ULONG>(node->vertexBuffer);
        node->vertexBuffer = nullptr;
    } else {
        node = static_cast<D3D9DynamicVertexBuffer*>(RwFreeListAlloc(DynamicVertexBufferFreeList, 0x40411));
        node->next = DynamicVertexBufferList;
        DynamicVertexBufferList = node;
    }
    const uint32 pool = (~D3D9DeviceDevCaps >> 15) & 2u;
    if (ComCall<kSlotDeviceCreateVertexBuffer, int32>(D3D9Device, size, 0x208u, 0u, pool, vertexBuffer, static_cast<void*>(nullptr)) != 0) {
        return FALSE;
    }
    node->size         = size;
    node->inUse        = 1;
    node->owner        = vertexBuffer;
    node->vertexBuffer = *vertexBuffer;
    return TRUE;
}

// 0x7F5AE0: marca como libre (inUse = used = 0) el primer nodo de la lista cuyo bufer es `vertexBuffer`; no libera nada ni toca el resto.
void RwD3D9DynamicVertexBufferDestroy(void* vertexBuffer) {
    for (auto* node = DynamicVertexBufferList; node != nullptr; node = node->next) {
        if (node->vertexBuffer == vertexBuffer) {
            node->inUse = 0;
            node->owner = nullptr;
            return;
        }
    }
}

// 0x7F5B10 (lote R11A, sin validar; era una envoltura): (stride, numVertex, void** vertexBuffer, void** vertexData, uint32* baseIndex). Reserva stride * numVertex bytes en uno de los cuatro buferes dinamicos
// activos. Con DevCaps & 0x10000 busca primero en el actual (posicion alineada hacia arriba al paso) y si no cabe en los otros tres por orden (el primero que cabe pasa a ser el actual); si ninguno cabe avanza el
// actual modulo 4 y empieza en 0. Sin ese bit empieza siempre en 0. Si lo pedido supera el tamano del bufer actual libera su nodo (inUse = 0, owner = 0), fija el tamano y lo recrea con 0x7F5A00. Bloquea con
// D3DLOCK_DISCARD | NOSYSLOCK (0x2800) si el desplazamiento es 0 o NOOVERWRITE | NOSYSLOCK (0x1800) si no; baseIndex = desplazamiento / stride (0 con descarte). TRUE si el HRESULT del Lock no es negativo.
RwBool RwD3D9DynamicVertexBufferLock(RwUInt32 vertexSize, RwUInt32 numVertex, void** vertexBufferOut, void** vertexDataOut, RwUInt32* baseIndexOut) {
    const uint32 stride = vertexSize;
    const uint32 bytes  = vertexSize * numVertex;
    uint32 current      = DynamicVertexBufferCurrent;
    uint32 offset = 0;
    uint32 end    = 0;
    bool   reuse  = false;
    if ((D3D9DeviceDevCaps & kDevCapsHardwareTransformAndLight) != 0) {
        offset = (DynamicVertexBufferPosition[current] + stride - 1) / stride * stride;
        end    = offset + bytes;
        if (end <= DynamicVertexBufferSize[current]) {
            reuse = true;
        } else {
            uint32 other = 0;
            for (; other < 4; ++other) {
                if (other == current) {
                    continue;
                }
                offset = (DynamicVertexBufferPosition[other] + stride - 1) / stride * stride;
                end    = offset + bytes;
                if (end <= DynamicVertexBufferSize[other]) {
                    break;
                }
            }
            if (other < 4) {
                current = other;
                DynamicVertexBufferCurrent = current;
                reuse = true;
            } else {
                ++current;
                DynamicVertexBufferCurrent = current;
                if (!(current < 4)) {
                    current = 0;
                    DynamicVertexBufferCurrent = current;
                }
            }
        }
    }
    if (!reuse) {
        offset = 0;
        end    = bytes;
    }
    if (bytes > DynamicVertexBufferSize[current]) {
        for (auto* node = DynamicVertexBufferList; node != nullptr; node = node->next) {
            if (node->vertexBuffer == DynamicVertexBufferActive[current]) {
                node->inUse = 0;
                node->owner = nullptr;
                current = DynamicVertexBufferCurrent;
                break;
            }
        }
        DynamicVertexBufferSize[current] = bytes;
        RwD3D9DynamicVertexBufferCreate(bytes, &DynamicVertexBufferActive[current]);
        current = DynamicVertexBufferCurrent;
    }
    void* const vb = DynamicVertexBufferActive[current];
    int32 result;
    if (offset == 0) {
        result = ComCall<kSlotVertexBufferLock, int32>(vb, 0u, bytes, static_cast<void*>(vertexDataOut), 0x2800u);
        DynamicVertexBufferPosition[DynamicVertexBufferCurrent] = bytes;
        *baseIndexOut = 0;
    } else {
        result = ComCall<kSlotVertexBufferLock, int32>(vb, offset, bytes, static_cast<void*>(vertexDataOut), 0x1800u);
        DynamicVertexBufferPosition[DynamicVertexBufferCurrent] = end;
        *baseIndexOut = offset / stride;
    }
    *vertexBufferOut = DynamicVertexBufferActive[DynamicVertexBufferCurrent];
    return result >= 0;
}

// 0x7F5C90: IDirect3DVertexBuffer9::Unlock; TRUE si el HRESULT no es negativo.
RwBool RwD3D9DynamicVertexBufferUnlock(void* vertexBuffer) {
    const HRESULT result = ComCall<kSlotVertexBufferUnlock, HRESULT>(vertexBuffer);
    return result >= 0;
}

// 0x7F5800 (lote R7A, sin validar; nombre inferido): pone a 0 las cuatro posiciones de los buferes dinamicos activos (0xC97B98..0xC97BA4) y avanza el indice actual (0xC97B94) modulo 4.
void _rwD3D9DynamicVertexBufferManagerForceDiscard() {
    DynamicVertexBufferPosition[0] = 0;
    DynamicVertexBufferPosition[1] = 0;
    DynamicVertexBufferPosition[2] = 0;
    DynamicVertexBufferPosition[3] = 0;
    const uint32 next = DynamicVertexBufferCurrent + 1;
    DynamicVertexBufferCurrent = next;
    if (!(next < 4)) {
        DynamicVertexBufferCurrent = 0;
    }
}

// 0x7F5840 (lote R7A, sin validar; nombre inferido): indice actual a 0 y, por cada uno de los cuatro buferes dinamicos activos: posicion y tamano a 0 y, si tenia bufer, busca su nodo en la lista (0xC97B8C)
// y lo marca libre (inUse = 0, owner = NULL) y deja la ranura a NULL. Con DevCaps & 0x10000 (pool DEFAULT) recorre despues TODOS los nodos con bufer: Release, vertexBuffer = NULL y, si tiene owner, escribe
// NULL en la variable del propietario.
void _rwD3D9DynamicVertexBufferRelease() {
    DynamicVertexBufferCurrent = 0;
    for (int i = 0; i < 4; ++i) {
        void* const vertexBuffer = DynamicVertexBufferActive[i];
        DynamicVertexBufferPosition[i] = 0;
        DynamicVertexBufferSize[i]     = 0;
        if (vertexBuffer != nullptr) {
            for (D3D9DynamicVertexBuffer* node = DynamicVertexBufferList; node != nullptr; node = node->next) {
                if (node->vertexBuffer == vertexBuffer) {
                    node->inUse = 0;
                    node->owner = nullptr;
                    break;
                }
            }
            DynamicVertexBufferActive[i] = nullptr;
        }
    }
    if ((D3D9DeviceDevCaps & kDevCapsHardwareTransformAndLight) != 0 && DynamicVertexBufferList != nullptr) {
        D3D9DynamicVertexBuffer* node = DynamicVertexBufferList;
        while (node != nullptr) {
            D3D9DynamicVertexBuffer* const next = node->next;
            if (node->vertexBuffer != nullptr) {
                ComCall<kSlotRelease, ULONG>(node->vertexBuffer);
                node->vertexBuffer = nullptr;
                if (node->owner != nullptr) {
                    *node->owner = nullptr;
                }
            }
            node = next;
        }
    }
}

// 0x7F5940 (lote R7A, sin validar; nombre inferido; la llama 0x7F5D0C y 0x7F58D0 por salto): indice actual a 0. Con DevCaps & 0x10000: para los cuatro buferes activos posicion = 0, tamano = 0x40000 y
// RwD3D9DynamicVertexBufferCreate(0x40000, &activo[i]); FALSE en cuanto uno falla; TRUE. Sin ese bit: solo el primero (los otros tres: posicion, tamano y bufer a 0) y TRUE.
RwBool _rwD3D9DynamicVertexBufferManagerOpen() {
    DynamicVertexBufferCurrent = 0;
    if ((D3D9DeviceDevCaps & kDevCapsHardwareTransformAndLight) != 0) {
        for (int i = 0; i < 4; ++i) {
            DynamicVertexBufferPosition[i] = 0;
            DynamicVertexBufferSize[i]     = 0x40000;
            if (!RwD3D9DynamicVertexBufferCreate(0x40000, &DynamicVertexBufferActive[i])) {
                return FALSE;
            }
        }
        return TRUE;
    }
    DynamicVertexBufferPosition[0] = 0;
    DynamicVertexBufferSize[0]     = 0x40000;
    if (!RwD3D9DynamicVertexBufferCreate(0x40000, &DynamicVertexBufferActive[0])) {
        return FALSE;
    }
    for (int i = 1; i < 4; ++i) {
        DynamicVertexBufferPosition[i] = 0;
        DynamicVertexBufferActive[i]   = nullptr;
        DynamicVertexBufferSize[i]     = 0;
    }
    return TRUE;
}

// 0x7F58D0 (lote R7A, sin validar; nombre del EXE por WindowedMode.cpp): tras recuperar el dispositivo. Con DevCaps & 0x10000 recorre la lista de buferes dinamicos y recrea (CreateVertexBuffer(size,
// 0x208, FVF 0, pool DEFAULT)) los que no tienen bufer, copiando el nuevo a la variable del propietario si existe; un HRESULT negativo devuelve FALSE (el nodo anterior ya recreado se conserva). Despues
// salta a 0x7F5940 y devuelve su resultado (tambien cuando no hay lista ni bit).
RwBool _rwD3D9DynamicVertexBufferRestore() {
    int32 result = 0;
    if ((D3D9DeviceDevCaps & kDevCapsHardwareTransformAndLight) != 0) {
        D3D9DynamicVertexBuffer* node = DynamicVertexBufferList;
        while (node != nullptr) {
            D3D9DynamicVertexBuffer* const next = node->next;
            if (node->vertexBuffer == nullptr) {
                result = ComCall<kSlotDeviceCreateVertexBuffer, int32>(D3D9Device, node->size, 0x208u, 0u, 0u, &node->vertexBuffer, static_cast<void*>(nullptr));
                if (node->owner != nullptr) {
                    *node->owner = node->vertexBuffer;
                }
            }
            if (result < 0) {
                return FALSE;
            }
            node = next;
        }
    }
    return _rwD3D9DynamicVertexBufferManagerOpen();
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

// Lote R23A (sin validar): Im3D (rwim3d.c). El modulo cuelga de RwEngineInstance + [0xC9C078]: +0x00 tuberia de transformacion; +0x04/+0x08/+0x0C/+0x10/+0x14 tuberias de render de los tipos de primitiva
// 3 (lista de triangulos), 5 (abanico), 4 (tira), 1 (lista de lineas) y 2 (polilinea); +0x38 estado de la transformacion en curso (15 dwords hasta +0x74): numero de vertices (word), vertices (+0x3C),
// stride 0x24 (+0x40), banderas (+0x44), matriz (+0x48); +0x44.. es ademas el paquete de la ejecucion de render: tuberia (+0x64), tipo (+0x68), indices (+0x6C), numero de indices (+0x70).
namespace {
constexpr uint32 kIm3DTransformPipeline = 0x00;
constexpr uint32 kIm3DNumVerts = 0x38;
constexpr uint32 kIm3DVerts = 0x3C;
constexpr uint32 kIm3DStride = 0x40;
constexpr uint32 kIm3DFlags = 0x44;
constexpr uint32 kIm3DLtm = 0x48;
constexpr uint32 kIm3DRenderPacket = 0x44;  // paquete de render = modulo + 0x44 (campos +0x20 tuberia, +0x24 tipo, +0x28 indices, +0x2C numero)
uint8* R23AIm3DModule() {
    return reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9C078);
}
template<typename T>
T& R23AIm3DField(uint8* module, uint32 offset) {
    return *reinterpret_cast<T*>(module + offset);
}
// Tuberia de render del tipo de primitiva (tabla de saltos del EXE: tipos 1..5 -> +0x10, +0x14, +0x04, +0x0C, +0x08).
uint32 R23AIm3DRenderPipelineOffset(uint32 primTypeMinusOne) {
    constexpr uint32 offsets[5] = { 0x10, 0x14, 0x04, 0x0C, 0x08 };
    return offsets[primTypeMinusOne];
}
// RwError { pluginID = 1, errorCode = _rwerror(codigo [, argumento]) } + RwErrorSet.
void R23ARaiseError(RwInt32 code) {
    RwError error;
    error.pluginID  = 1;
    error.errorCode = _rwerror(code);
    RwErrorSet(&error);
}
void R23ARaiseError(RwInt32 code, RwInt32 argument) {
    RwError error;
    error.pluginID  = 1;
    error.errorCode = _rwerror(code, argument);
    RwErrorSet(&error);
}
}

// 0x7EF450 (lote R23A, sin validar; 196 bytes; era una envoltura; cdecl, (vertices, numero, matriz, banderas)): mas de 0x10000 vertices -> RwError { 1, _rwerror(0x32) } y NULL. Si no guarda en el modulo
// (en este orden) numero (word, +0x38), vertices (+0x3C), stride 0x24 (+0x40), matriz (+0x48) y banderas | 0x18 (+0x44), y ejecuta la tuberia de transformacion ([modulo + 0]) con RxPipelineExecute(tuberia,
// modulo + 0x38, 1). Si falla devuelve NULL; si no, los vertices.
void* RwIm3DTransform(RwIm3DVertex* pVerts, RwUInt32 numVerts, RwMatrix* ltm, RwUInt32 flags) {
    if (numVerts > 0x10000) {
        R23ARaiseError(0x32);
        return nullptr;
    }
    uint8* const module = R23AIm3DModule();
    R23AIm3DField<uint16>(module, kIm3DNumVerts) = static_cast<uint16>(numVerts);
    R23AIm3DField<RwIm3DVertex*>(module, kIm3DVerts) = pVerts;
    R23AIm3DField<uint32>(module, kIm3DStride) = 0x24;
    R23AIm3DField<RwMatrix*>(module, kIm3DLtm) = ltm;
    R23AIm3DField<uint32>(module, kIm3DFlags) = flags | 0x18;
    if (RxPipelineExecute(R23AIm3DField<RxPipeline*>(module, kIm3DTransformPipeline), module + kIm3DNumVerts, TRUE) == nullptr) {
        return nullptr;
    }
    return pVerts;
}

// 0x7EF520 (lote R23A, sin validar; 47 bytes; era una envoltura): sin vertices en curso (+0x3C == NULL) devuelve FALSE; si no pone a cero los 15 dwords del estado (+0x38..+0x73) y devuelve TRUE.
RwBool RwIm3DEnd() {
    uint8* const module = R23AIm3DModule();
    if (R23AIm3DField<void*>(module, kIm3DVerts) == nullptr) {
        return FALSE;
    }
    memset(module + kIm3DNumVerts, 0, 15 * sizeof(uint32));
    return TRUE;
}

RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2) {
    return ((RwBool(__cdecl *)(RwInt32, RwInt32))0x7EF900)(vert1, vert2);
}

RwBool RwIm3DRenderTriangle(RwInt32 vert1, RwInt32 vert2, RwInt32 vert3) {
    return ((RwBool(__cdecl *)(RwInt32, RwInt32, RwInt32))0x7EF810)(vert1, vert2, vert3);
}

// 0x7EF550 (lote R23A, sin validar; 318 bytes; era una envoltura; cdecl, (tipo, indices, numero)): sin vertices en curso -> RwError { 1, _rwerror(0x23) } y FALSE. Rellena el paquete (modulo + 0x44): tuberia = 0 (+0x64),
// tipo (+0x68), indices (+0x6C), numero (+0x70) y elige la tuberia por tipo (tabla de saltos de tipo - 1, sin signo): 1 (lineas) +0x10 con numero -= numero % 2 (resto con signo); 2 +0x14; 3 (triangulos) +0x04 con
// numero -= numero % 3 (idiv con signo); 4 +0x0C; 5 +0x08. Cualquier otro tipo: RwError { 1, _rwerror(0x25, tipo) } y SIGUE con la tuberia a NULL. Ejecuta RxPipelineExecute(tuberia, paquete, 0): NULL -> FALSE, si no TRUE.
RwBool RwIm3DRenderIndexedPrimitive(RwPrimitiveType primType, RwImVertexIndex* indices, RwInt32 numIndices) {
    uint8* const module = R23AIm3DModule();
    if (R23AIm3DField<void*>(module, kIm3DVerts) == nullptr) {
        R23ARaiseError(0x23);
        return FALSE;
    }
    uint8* const packet = module + kIm3DRenderPacket;
    R23AIm3DField<RwImVertexIndex*>(packet, 0x28) = indices;
    R23AIm3DField<RxPipeline*>(packet, 0x20) = nullptr;
    R23AIm3DField<RwInt32>(packet, 0x24) = static_cast<RwInt32>(primType);
    R23AIm3DField<RwInt32>(packet, 0x2C) = numIndices;
    const uint32 index = static_cast<uint32>(primType) - 1u;
    if (index > 4) {
        R23ARaiseError(0x25, static_cast<RwInt32>(primType));
    } else {
        R23AIm3DField<RxPipeline*>(packet, 0x20) = R23AIm3DField<RxPipeline*>(module, R23AIm3DRenderPipelineOffset(index));
        if (index == 0) {
            R23AIm3DField<RwInt32>(packet, 0x2C) = numIndices - numIndices % 2;
        } else if (index == 2) {
            R23AIm3DField<RwInt32>(packet, 0x2C) = numIndices - numIndices % 3;
        }
    }
    return RxPipelineExecute(R23AIm3DField<RxPipeline*>(packet, 0x20), packet, FALSE) != nullptr ? TRUE : FALSE;
}

// 0x7EF6B0 (lote R23A, sin validar; 319 bytes; era una envoltura; cdecl, (tipo)): llama a RxHeapGetGlobalHeap (0x8052F0, resultado ignorado) y, sin vertices en curso, RwError { 1, _rwerror(0x23) } y FALSE. Rellena el
// paquete: tuberia = 0 (+0x64), tipo (+0x68), indices = NULL (+0x6C), numero = el numero de vertices del estado (word +0x38, sin signo; +0x70) y elige la tuberia como RwIm3DRenderIndexedPrimitive (sin ajustar
// el numero); tipo invalido: RwError { 1, _rwerror(0x25, tipo) } y sigue con la tuberia a NULL. Ejecuta RxPipelineExecute(tuberia, paquete, 0): NULL -> FALSE, si no TRUE.
RwBool RwIm3DRenderPrimitive(RwPrimitiveType primType) {
    const RwBool hasVertices = R23AIm3DField<void*>(R23AIm3DModule(), kIm3DVerts) != nullptr;
    RxHeapGetGlobalHeap();
    if (!hasVertices) {
        R23ARaiseError(0x23);
        return FALSE;
    }
    uint8* const module = R23AIm3DModule();
    R23AIm3DField<RxPipeline*>(module, 0x64) = nullptr;
    uint8* const packet = module + kIm3DRenderPacket;
    R23AIm3DField<RwInt32>(packet, 0x24) = static_cast<RwInt32>(primType);
    R23AIm3DField<void*>(packet, 0x28) = nullptr;
    R23AIm3DField<uint32>(packet, 0x2C) = R23AIm3DField<uint16>(module, kIm3DNumVerts);
    const uint32 index = static_cast<uint32>(primType) - 1u;
    if (index > 4) {
        R23ARaiseError(0x25, static_cast<RwInt32>(primType));
    } else {
        R23AIm3DField<RxPipeline*>(packet, 0x20) = R23AIm3DField<RxPipeline*>(module, R23AIm3DRenderPipelineOffset(index));
    }
    return RxPipelineExecute(R23AIm3DField<RxPipeline*>(packet, 0x20), packet, FALSE) != nullptr ? TRUE : FALSE;
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

// 0x7FC200 (lote R10A, sin validar; sustituye la envoltura; cdecl sin argumentos, `ret`, 195 bytes; el listado la atribuia a RpMatFX por vecindad): vuelca al dispositivo los estados pendientes de la cache de estados del controlador D3D9.
// Dos listas de pendientes: estados de render (contador 0xC9A5EC, ids en 0xC98B40) y estados de etapa de textura (contador 0xC9A5F0, pares (etapa, tipo) de 8 bytes en 0xC99C80). Estados de render: valor pedido en 0xC991D0 + 8 * id (con la
// bandera de pendiente en +4) y valor actual en 0xC98E88 + 4 * id; estados de etapa: indice = etapa * 33 + tipo, valor pedido en 0xC980F8 + 8 * indice (bandera en +4) y actual en 0xC99860 + 4 * indice. Para cada pendiente pone la bandera a 0 y,
// si el valor pedido difiere del actual, lo copia y llama a IDirect3DDevice9::SetRenderState (+0xE4) o SetTextureStageState (+0x10C). Al terminar cada lista deja su contador a 0. La lista de render se recorre releyendo el contador en cada vuelta
// (como el EXE); la de etapas es un do-while.
void _rwD3D9RenderStateFlushCache() {
    auto& renderStateCount = StaticRef<RwUInt32>(0xC9A5EC);
    auto& stageStateCount  = StaticRef<RwUInt32>(0xC9A5F0);
    const RwUInt32* const dirtyRenderStates = reinterpret_cast<const RwUInt32*>(0xC98B40);
    RwUInt32* const wantedRenderStates      = reinterpret_cast<RwUInt32*>(0xC991D0); // {valor, pendiente} por id
    RwUInt32* const currentRenderStates     = reinterpret_cast<RwUInt32*>(0xC98E88);
    const RwUInt32* const dirtyStageStates  = reinterpret_cast<const RwUInt32*>(0xC99C80); // {etapa, tipo}
    RwUInt32* const wantedStageStates       = reinterpret_cast<RwUInt32*>(0xC980F8);     // {valor, pendiente} por indice
    RwUInt32* const currentStageStates      = reinterpret_cast<RwUInt32*>(0xC99860);

    if (renderStateCount != 0) {
        for (RwUInt32 i = 0; i < renderStateCount; ++i) {
            const RwUInt32 state  = dirtyRenderStates[i];
            const RwUInt32 wanted = wantedRenderStates[state * 2];
            const RwUInt32 actual = currentRenderStates[state];
            wantedRenderStates[state * 2 + 1] = 0;
            if (actual != wanted) {
                currentRenderStates[state] = wanted;
                ComCall<kSlotSetRenderState, int32>(D3D9Device, state, wanted);
            }
        }
        renderStateCount = 0;
    }
    if (stageStateCount != 0) {
        RwUInt32 i = 0;
        do {
            const RwUInt32 stage  = dirtyStageStates[i * 2];
            const RwUInt32 type   = dirtyStageStates[i * 2 + 1];
            const RwUInt32 index  = stage + (type + (stage << 5));
            const RwUInt32 wanted = wantedStageStates[index * 2];
            const RwUInt32 actual = currentStageStates[index];
            wantedStageStates[index * 2 + 1] = 0;
            if (actual != wanted) {
                currentStageStates[index] = wanted;
                ComCall<kSlotSetTextureStageState, int32>(D3D9Device, stage, type, wanted);
            }
            ++i;
        } while (i < stageStateCount);
        stageStateCount = 0;
    }
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

// 0x7FC430 (lote R11A, sin validar; 97 bytes): D3DMATERIAL9 (17 dwords). Si es igual (dword a dword) a la copia en cache (0xC98AF8) devuelve TRUE sin tocar el dispositivo; si no, copia el material a la cache,
// pone a 0 el dword de 0xC9A5F4 y llama a IDirect3DDevice9::SetMaterial (ranura 49, +0xC4) con el puntero ORIGINAL; TRUE si el HRESULT no es negativo.
namespace {
struct D3D9MaterialCache { uint32 dwords[17]; };
auto& D3D9CurrentMaterial   = StaticRef<D3D9MaterialCache>(0xC98AF8);
auto& D3D9MaterialStateFlag = StaticRef<uint32>(0xC9A5F4);
constexpr size_t kSlotDeviceSetMaterial = 49; // IDirect3DDevice9::SetMaterial (+0xC4)
}
RwBool RwD3D9SetMaterial(const void* material) {
    const auto* const source = static_cast<const uint32*>(material);
    bool same = true;
    for (int i = 0; i < 17; ++i) {
        if (D3D9CurrentMaterial.dwords[i] != source[i]) {
            same = false;
            break;
        }
    }
    if (same) {
        return TRUE;
    }
    for (int i = 0; i < 17; ++i) {
        D3D9CurrentMaterial.dwords[i] = source[i];
    }
    D3D9MaterialStateFlag = 0;
    return ComCall<kSlotDeviceSetMaterial, int32>(D3D9Device, material) >= 0;
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

// 0x7FAA30 (lote R11A, sin validar; 480 bytes): cache de declaraciones de vertices D3D9. `elements` es un array de D3DVERTEXELEMENT9 (8 bytes) terminado por D3DDECL_END (byte de tipo en +4 == 0x11, que cuenta).
// La cache (0xC97C58 = numero de entradas, 0xC97C5C = capacidad, 0xC97C60 = array de entradas de 12 bytes {declaracion, numElementos, copia de los elementos}) se recorre en orden; la primera entrada con el
// mismo numero de elementos y los mismos bytes se reutiliza: con declaracion hace AddRef (ranura 1) y sin ella (queda a NULL tras una perdida del dispositivo) CreateVertexDeclaration (ranura 86, +0x158) sobre su copia
// (resultado ignorado); *vertexdeclaration = entrada.declaracion y TRUE si no es NULL. Si no hay entrada: crece la cache si count >= capacity (capacidad += 16; RwMalloc o RwRealloc, pista 0x1040411, sin comprobar NULL),
// CreateVertexDeclaration(device, elements, vertexdeclaration) (HRESULT negativo: *vertexdeclaration = NULL y FALSE) y anade la entrada (copia de los elementos con RwMalloc, pista 0x40411).
namespace {
struct D3D9VertexDeclarationCacheEntry {
    void*  declaration;  // +0x00 IDirect3DVertexDeclaration9*
    uint32 numElements;  // +0x04 (con el elemento final)
    void*  elements;     // +0x08 copia de los D3DVERTEXELEMENT9
};
static_assert(sizeof(D3D9VertexDeclarationCacheEntry) == 12);
auto& D3D9VertexDeclarationCount    = StaticRef<uint32>(0xC97C58);
auto& D3D9VertexDeclarationCapacity = StaticRef<uint32>(0xC97C5C);
auto& D3D9VertexDeclarationEntries  = StaticRef<D3D9VertexDeclarationCacheEntry*>(0xC97C60);
constexpr size_t kSlotAddRef = 1;                         // IUnknown::AddRef
constexpr size_t kSlotDeviceCreateVertexDeclaration = 86; // IDirect3DDevice9::CreateVertexDeclaration (+0x158)
}
RwBool RwD3D9CreateVertexDeclaration(const void* elements, void** vertexdeclaration) {
    const auto* const bytes = static_cast<const uint8*>(elements);
    uint32 numElements = 0;
    for (const uint8* type = bytes + 4; *type != 0x11; type += 8) {
        ++numElements;
    }
    ++numElements;
    for (uint32 i = 0; i < D3D9VertexDeclarationCount; ++i) {
        D3D9VertexDeclarationCacheEntry* const entry = &D3D9VertexDeclarationEntries[i];
        if (entry->numElements != numElements) {
            continue;
        }
        if (memcmp(entry->elements, elements, numElements * 8) != 0) {
            continue;
        }
        if (entry->declaration != nullptr) {
            ComCall<kSlotAddRef, ULONG>(entry->declaration);
        } else {
            ComCall<kSlotDeviceCreateVertexDeclaration, int32>(D3D9Device, entry->elements, &entry->declaration);
        }
        void* const declaration = D3D9VertexDeclarationEntries[i].declaration;
        *vertexdeclaration = declaration;
        return declaration != nullptr;
    }
    if (!(D3D9VertexDeclarationCount < D3D9VertexDeclarationCapacity)) {
        const uint32 newCapacity = D3D9VertexDeclarationCapacity + 0x10;
        D3D9VertexDeclarationCapacity = newCapacity;
        if (D3D9VertexDeclarationEntries == nullptr) {
            D3D9VertexDeclarationEntries = static_cast<D3D9VertexDeclarationCacheEntry*>(RwMalloc(newCapacity * 12, 0x1040411));
        } else {
            D3D9VertexDeclarationEntries = static_cast<D3D9VertexDeclarationCacheEntry*>(RwRealloc(D3D9VertexDeclarationEntries, newCapacity * 12, 0x1040411));
        }
    }
    if (ComCall<kSlotDeviceCreateVertexDeclaration, int32>(D3D9Device, elements, vertexdeclaration) < 0) {
        *vertexdeclaration = nullptr;
        return FALSE;
    }
    const uint32 index = D3D9VertexDeclarationCount;
    D3D9VertexDeclarationEntries[index].declaration = *vertexdeclaration;
    D3D9VertexDeclarationEntries[index].numElements = numElements;
    const uint32 byteCount = numElements * 8;
    void* const copy = RwMalloc(byteCount, 0x40411);
    D3D9VertexDeclarationEntries[index].elements = copy;
    memcpy(copy, elements, byteCount);
    D3D9VertexDeclarationCount = D3D9VertexDeclarationCount + 1;
    return *vertexdeclaration != nullptr;
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

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R11A (sin validar; segundo encargo con ese nombre: resto de 0x7F5B10-0x7FF8C0 atribuido por vecindad a RpMatFX con confianza baja): son del controlador D3D9 de RenderWare (RwD3D9), no de
// RpMatFX. Manejadores de estados de render (0x7FE0A0-0x7FED00), despachador de RwRenderStateSet (0x7FE420), tamano valido de textura (0x7FED70), conversiones de pixel RGBA (0x7FEE20, 0x7FF070),
// raster -> imagen (0x7FF270, 0x7FF450, 0x7FF8C0) y RwIm2DRenderPrimitive (0x7FBB00). Estado: igual que R6A (valor actual 0xC9A4xx, copia D3D 0xC99xxx, bandera "en lista" y lista 0xC98B40 / 0xC9A5EC).
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
constexpr uint32 kD3DRSZEnable = 0x07, kD3DRSZWriteEnable = 0x0E, kD3DRSShadeMode = 0x09, kD3DRSCullMode = 0x16, kD3DRSZFunc = 0x17, kD3DRSAlphaRef = 0x18, kD3DRSAlphaFunc = 0x19;
constexpr uint32 kD3DRSFogEnable = 0x1C, kD3DRSFogColor = 0x22, kD3DRSFogTableMode = 0x23, kD3DRSFogStart = 0x24, kD3DRSFogEnd = 0x25, kD3DRSFogDensity = 0x26, kD3DRSFogVertexMode = 0x8C;
constexpr uint32 kD3DSAMPAddressU = 1, kD3DSAMPAddressV = 2, kD3DSAMPBorderColor = 4;

// RWERROR((codigo [, argumento])) con pluginID 1 (literal del EXE): RwError { 1, _rwerror(codigo [, argumento]) } + RwErrorSet.
void D3D9RaiseError(RwInt32 code) {
    RwError error;
    error.pluginID  = 1;
    error.errorCode = _rwerror(code);
    RwErrorSet(&error);
}
void D3D9RaiseError(RwInt32 code, const char* argument) {
    RwError error;
    error.pluginID  = 1;
    error.errorCode = _rwerror(code, argument);
    RwErrorSet(&error);
}
} // namespace

// 0x7FE0A0 (lote R11A, sin validar; manejador del estado VERTEXALPHAENABLE; cdecl, un argumento (RwBool), `ret`, 235 bytes): actual en 0xC9A4E8. Con un valor distinto de 0: si ya estaba activo no hace nada; si no lo activa
// y, solo con 0xC9A4EC (raster con alfa) a 0, deja 0xC992A8 = 1 (D3DRS_ALPHABLENDENABLE 0x1B a la lista con su bandera 0xC992AC) y copia 0xC9A5D4 a 0xC99248 si difieren (solo entonces D3DRS_ALPHATESTENABLE 0x0F con la
// bandera 0xC9924C). Con 0: lo mismo en sentido contrario (0xC992A8 = 0 y 0xC99248 a 0 si no lo era). Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateVertexAlphaEnable(RwBool enable) {
    auto& current = StaticRef<uint32>(0xC9A4E8);
    if (enable != 0) {
        if (current != 0) {
            return TRUE;
        }
        const uint32 rasterAlpha = StaticRef<uint32>(0xC9A4EC);
        current = 1;
        if (rasterAlpha != 0) {
            return TRUE;
        }
        StaticRef<uint32>(0xC992A8) = 1;
        D3D9QueueRenderState(StaticRef<uint32>(0xC992AC), kD3DRSAlphaBlendEnable);
        const uint32 alphaTest = StaticRef<uint32>(0xC9A5D4);
        if (StaticRef<uint32>(0xC99248) == alphaTest) {
            return TRUE;
        }
        StaticRef<uint32>(0xC99248) = alphaTest;
    } else {
        if (current == 0) {
            return TRUE;
        }
        const uint32 rasterAlpha = StaticRef<uint32>(0xC9A4EC);
        current = 0;
        if (rasterAlpha != 0) {
            return TRUE;
        }
        StaticRef<uint32>(0xC992A8) = 0;
        D3D9QueueRenderState(StaticRef<uint32>(0xC992AC), kD3DRSAlphaBlendEnable);
        if (StaticRef<uint32>(0xC99248) == 0) {
            return TRUE;
        }
        StaticRef<uint32>(0xC99248) = 0;
    }
    D3D9QueueRenderState(StaticRef<uint32>(0xC9924C), kD3DRSAlphaTestEnable);
    return TRUE;
}

// 0x7FE190 (lote R11A, sin validar; 6 bytes: mov eax, [0xC9A4E8]; ret; nombre inferido): estado actual de VERTEXALPHAENABLE.
RwInt32 _rwD3D9RenderStateGetVertexAlphaEnable() {
    return StaticRef<RwInt32>(0xC9A4E8);
}

// 0x7FE1A0 (lote R11A, sin validar; manejador de STENCILENABLE; cdecl, un argumento, `ret`, 72 bytes): si difiere del actual (0xC9A4C8): copia D3D (0xC99370) = valor; con la bandera 0xC99374 a 0 la pone a 1 y anade D3DRS_STENCILENABLE
// (0x34) a la lista; guarda el valor. Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateStencilEnable(RwInt32 enable) {
    auto& current = StaticRef<RwInt32>(0xC9A4C8);
    if (enable != current) {
        StaticRef<uint32>(0xC99370) = static_cast<uint32>(enable);
        D3D9QueueRenderState(StaticRef<uint32>(0xC99374), 0x34);
        current = enable;
    }
    return TRUE;
}

// 0x7FE1F0 (lote R11A, sin validar; STENCILFAIL; 78 bytes): igual con actual 0xC9A4CC, copia 0xC99378 = tabla 0x8849A0[valor], bandera 0xC9937C y D3DRS_STENCILFAIL (0x35). Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateStencilFail(RwInt32 operation) {
    auto& current = StaticRef<RwInt32>(0xC9A4CC);
    if (operation != current) {
        StaticRef<uint32>(0xC99378) = reinterpret_cast<const uint32*>(0x8849A0)[operation];
        D3D9QueueRenderState(StaticRef<uint32>(0xC9937C), 0x35);
        current = operation;
    }
    return TRUE;
}

// 0x7FE240 (lote R11A, sin validar; STENCILZFAIL; 78 bytes): actual 0xC9A4D0, copia 0xC99380 = tabla 0x8849A0[valor], bandera 0xC99384, D3DRS_STENCILZFAIL (0x36).
RwBool _rwD3D9RenderStateStencilZFail(RwInt32 operation) {
    auto& current = StaticRef<RwInt32>(0xC9A4D0);
    if (operation != current) {
        StaticRef<uint32>(0xC99380) = reinterpret_cast<const uint32*>(0x8849A0)[operation];
        D3D9QueueRenderState(StaticRef<uint32>(0xC99384), 0x36);
        current = operation;
    }
    return TRUE;
}

// 0x7FE290 (lote R11A, sin validar; STENCILPASS; 78 bytes): actual 0xC9A4D4, copia 0xC99388 = tabla 0x8849A0[valor], bandera 0xC9938C, D3DRS_STENCILPASS (0x37).
RwBool _rwD3D9RenderStateStencilPass(RwInt32 operation) {
    auto& current = StaticRef<RwInt32>(0xC9A4D4);
    if (operation != current) {
        StaticRef<uint32>(0xC99388) = reinterpret_cast<const uint32*>(0x8849A0)[operation];
        D3D9QueueRenderState(StaticRef<uint32>(0xC9938C), 0x37);
        current = operation;
    }
    return TRUE;
}

// 0x7FE2E0 (lote R11A, sin validar; STENCILFUNCTION; 78 bytes): actual 0xC9A4D8, copia 0xC99390 = tabla 0x8849C4[valor], bandera 0xC99394, D3DRS_STENCILFUNC (0x38).
RwBool _rwD3D9RenderStateStencilFunction(RwInt32 function) {
    auto& current = StaticRef<RwInt32>(0xC9A4D8);
    if (function != current) {
        StaticRef<uint32>(0xC99390) = reinterpret_cast<const uint32*>(0x8849C4)[function];
        D3D9QueueRenderState(StaticRef<uint32>(0xC99394), 0x38);
        current = function;
    }
    return TRUE;
}

// 0x7FE330 (lote R11A, sin validar; STENCILFUNCTIONREF; 72 bytes): actual 0xC9A4DC, copia 0xC99398 = valor, bandera 0xC9939C, D3DRS_STENCILREF (0x39).
RwBool _rwD3D9RenderStateStencilFunctionRef(RwInt32 value) {
    auto& current = StaticRef<RwInt32>(0xC9A4DC);
    if (value != current) {
        StaticRef<uint32>(0xC99398) = static_cast<uint32>(value);
        D3D9QueueRenderState(StaticRef<uint32>(0xC9939C), 0x39);
        current = value;
    }
    return TRUE;
}

// 0x7FE380 (lote R11A, sin validar; STENCILFUNCTIONMASK; 72 bytes): actual 0xC9A4E0, copia 0xC993A0 = valor, bandera 0xC993A4, D3DRS_STENCILMASK (0x3A).
RwBool _rwD3D9RenderStateStencilFunctionMask(RwInt32 value) {
    auto& current = StaticRef<RwInt32>(0xC9A4E0);
    if (value != current) {
        StaticRef<uint32>(0xC993A0) = static_cast<uint32>(value);
        D3D9QueueRenderState(StaticRef<uint32>(0xC993A4), 0x3A);
        current = value;
    }
    return TRUE;
}

// 0x7FE3D0 (lote R11A, sin validar; STENCILFUNCTIONWRITEMASK; 72 bytes): actual 0xC9A4E4, copia 0xC993A8 = valor, bandera 0xC993AC, D3DRS_STENCILWRITEMASK (0x3B).
RwBool _rwD3D9RenderStateStencilFunctionWriteMask(RwInt32 value) {
    auto& current = StaticRef<RwInt32>(0xC9A4E4);
    if (value != current) {
        StaticRef<uint32>(0xC993A8) = static_cast<uint32>(value);
        D3D9QueueRenderState(StaticRef<uint32>(0xC993AC), 0x3B);
        current = value;
    }
    return TRUE;
}

// 0x7FEA70 (lote R11A, sin validar; ZWRITEENABLE; cdecl, un argumento, `ret`, 244 bytes): actual 0xC9A4C0. Con un valor distinto de 0 y el actual a 0: si el ZTEST (0xC9A4C4) esta a 0 deja 0xC99208 = 1 (D3DRS_ZENABLE 0x07 a
// la lista con la bandera 0xC9920C); copia 0xC99240 = 1 (D3DRS_ZWRITEENABLE 0x0E con la bandera 0xC99244); actual = 1. Con 0 y el actual distinto de 0: igual con 0 (y ZENABLE solo si el ZTEST esta a 0). Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateZWriteEnable(RwBool enable) {
    auto& current = StaticRef<uint32>(0xC9A4C0);
    if (enable != 0) {
        if (current != 0) {
            return TRUE;
        }
        if (StaticRef<uint32>(0xC9A4C4) == 0) {
            StaticRef<uint32>(0xC99208) = 1;
            D3D9QueueRenderState(StaticRef<uint32>(0xC9920C), kD3DRSZEnable);
        }
        StaticRef<uint32>(0xC99240) = 1;
        D3D9QueueRenderState(StaticRef<uint32>(0xC99244), kD3DRSZWriteEnable);
        current = 1;
    } else {
        if (current == 0) {
            return TRUE;
        }
        if (StaticRef<uint32>(0xC9A4C4) == 0) {
            StaticRef<uint32>(0xC99208) = 0;
            D3D9QueueRenderState(StaticRef<uint32>(0xC9920C), kD3DRSZEnable);
        }
        StaticRef<uint32>(0xC99240) = 0;
        D3D9QueueRenderState(StaticRef<uint32>(0xC99244), kD3DRSZWriteEnable);
        current = 0;
    }
    return TRUE;
}

// 0x7FEB70 (lote R11A, sin validar; ZTESTENABLE; 253 bytes): actual 0xC9A4C4. Con un valor distinto de 0 y el actual a 0: si el ZWRITE (0xC9A4C0) esta a 0 deja 0xC99208 = 1 (D3DRS_ZENABLE, bandera 0xC9920C);
// 0xC99288 = 4 (D3DCMP_LESSEQUAL; D3DRS_ZFUNC 0x17, bandera 0xC9928C); actual = 1. Con 0: igual con 0 y ZFUNC = 8 (D3DCMP_ALWAYS). Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateZTestEnable(RwBool enable) {
    auto& current = StaticRef<uint32>(0xC9A4C4);
    if (enable != 0) {
        if (current != 0) {
            return TRUE;
        }
        if (StaticRef<uint32>(0xC9A4C0) == 0) {
            StaticRef<uint32>(0xC99208) = 1;
            D3D9QueueRenderState(StaticRef<uint32>(0xC9920C), kD3DRSZEnable);
        }
        StaticRef<uint32>(0xC99288) = 4;
        D3D9QueueRenderState(StaticRef<uint32>(0xC9928C), kD3DRSZFunc);
        current = 1;
    } else {
        if (current == 0) {
            return TRUE;
        }
        if (StaticRef<uint32>(0xC9A4C0) == 0) {
            StaticRef<uint32>(0xC99208) = 0;
            D3D9QueueRenderState(StaticRef<uint32>(0xC9920C), kD3DRSZEnable);
        }
        StaticRef<uint32>(0xC99288) = 8;
        D3D9QueueRenderState(StaticRef<uint32>(0xC9928C), kD3DRSZFunc);
        current = 0;
    }
    return TRUE;
}

// 0x7FEC70 (lote R11A, sin validar; SHADEMODE; 78 bytes): actual 0xC9A504; si difiere: copia 0xC99218 = tabla 0x8848F0[valor], bandera 0xC9921C, D3DRS_SHADEMODE (0x09). Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateShadeMode(RwInt32 mode) {
    auto& current = StaticRef<RwInt32>(0xC9A504);
    if (mode != current) {
        StaticRef<uint32>(0xC99218) = reinterpret_cast<const uint32*>(0x8848F0)[mode];
        D3D9QueueRenderState(StaticRef<uint32>(0xC9921C), kD3DRSShadeMode);
        current = mode;
    }
    return TRUE;
}

// 0x7FECC0 (lote R11A, sin validar; BORDERCOLOR; 50 bytes): si difiere del actual (0xC9A518): lo guarda, copia 0xC98948 = valor y SetSamplerState(0, D3DSAMP_BORDERCOLOR 4, valor) del dispositivo. Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateBorderColor(RwUInt32 color) {
    auto& current = StaticRef<RwUInt32>(0xC9A518);
    if (color != current) {
        current = color;
        StaticRef<uint32>(0xC98948) = color;
        ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, 0u, kD3DSAMPBorderColor, color);
    }
    return TRUE;
}

// 0x7FED00 (lote R11A, sin validar; CULLMODE; 78 bytes): actual 0xC9A4F0; si difiere: copia 0xC99280 = tabla 0x884990[valor], bandera 0xC99284, D3DRS_CULLMODE (0x16). Devuelve siempre TRUE.
RwBool _rwD3D9RenderStateCullMode(RwInt32 mode) {
    auto& current = StaticRef<RwInt32>(0xC9A4F0);
    if (mode != current) {
        StaticRef<uint32>(0xC99280) = reinterpret_cast<const uint32*>(0x884990)[mode];
        D3D9QueueRenderState(StaticRef<uint32>(0xC99284), kD3DRSCullMode);
        current = mode;
    }
    return TRUE;
}

namespace {
// Bloques de TEXTUREADDRESS: direccion U (0xC9A50C, copia 0xC9893C, SetSamplerState(0, ADDRESSU)) y V (0xC9A510, copia 0xC98940, SetSamplerState(0, ADDRESSV)); tabla 0x88493C (RwTextureAddressMode -> D3DTADDRESS).
void D3D9SetTextureAddressU(uint32 mode) {
    if (mode != StaticRef<uint32>(0xC9A50C)) {
        const uint32 d3dMode = reinterpret_cast<const uint32*>(0x88493C)[mode];
        StaticRef<uint32>(0xC9A50C) = mode;
        StaticRef<uint32>(0xC9893C) = d3dMode;
        ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, 0u, kD3DSAMPAddressU, d3dMode);
    }
}
void D3D9SetTextureAddressV(uint32 mode) {
    if (mode != StaticRef<uint32>(0xC9A510)) {
        StaticRef<uint32>(0xC9A510) = mode;
        const uint32 d3dMode = reinterpret_cast<const uint32*>(0x88493C)[mode];
        StaticRef<uint32>(0xC98940) = d3dMode;
        ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, 0u, kD3DSAMPAddressV, d3dMode);
    }
}
} // namespace

// 0x7FE420 (lote R11A, sin validar; RwRenderStateSet del controlador D3D9; cdecl, (RwRenderState, void* valor), `ret`, 1493 bytes): reparte por la tabla 0x7FE9F8 (estado - 1 > 29 devuelve 0; 18 y 19 tambien):
//  1 raster de la etapa 0; 2 direccion U y V; 3 U; 4 V; 5 devuelve (valor != 0) sin guardar nada; 6 ZTEST (0x7FEB70); 7 SHADEMODE (0x7FEC70); 8 ZWRITE (0x7FEA70); 9 filtro (0x7FDB80); 10 SRCBLEND; 11 DESTBLEND;
//  12 VERTEXALPHA (0x7FE0A0); 13 BORDERCOLOR (0x7FECC0); 14 FOGENABLE, 15 FOGCOLOR, 16 FOGTYPE, 17 FOGDENSITY (el valor es un puntero a float) en linea; 20 CULLMODE (0x7FED00); 21-28 los ocho del stencil; 29 funcion
//  del test de alfa en linea; 30 referencia de alfa (RwD3D9SetRenderState(D3DRS_ALPHAREF 0x18, valor)). Los manejadores devuelven TRUE (1); FOGENABLE con capacidades sin niebla (RasterCaps & 0x180) no cambia nada y devuelve 1.
RwBool _rwD3D9RWSetRenderState(RwInt32 state, void* value) {
    const uint32 raw = static_cast<uint32>(reinterpret_cast<uintptr_t>(value));
    const uint32 index = static_cast<uint32>(state) - 1u;
    if (index > 0x1Du) {
        return FALSE;
    }
    switch (index) {
    case 0:
        return _rwD3D9RWSetRasterStage(static_cast<RwRaster*>(value), 0);
    case 1:
        D3D9SetTextureAddressU(raw);
        D3D9SetTextureAddressV(raw);
        return TRUE;
    case 2:
        D3D9SetTextureAddressU(raw);
        return TRUE;
    case 3:
        D3D9SetTextureAddressV(raw);
        return TRUE;
    case 4:
        return raw != 0 ? TRUE : FALSE;
    case 5:
        return _rwD3D9RenderStateZTestEnable(static_cast<RwBool>(raw));
    case 6:
        return _rwD3D9RenderStateShadeMode(static_cast<RwInt32>(raw));
    case 7:
        return _rwD3D9RenderStateZWriteEnable(static_cast<RwBool>(raw));
    case 8:
        return _rwD3D9RenderStateTextureFilter(static_cast<RwInt32>(raw));
    case 9:
        return _rwD3D9RenderStateSrcBlend(static_cast<RwInt32>(raw));
    case 10:
        return _rwD3D9RenderStateDestBlend(static_cast<RwInt32>(raw));
    case 11:
        return _rwD3D9RenderStateVertexAlphaEnable(static_cast<RwBool>(raw));
    case 12:
        return _rwD3D9RenderStateBorderColor(raw);
    case 13: { // FOGENABLE
        auto& current = StaticRef<uint32>(0xC9A4F4);
        if (raw != 0) {
            if (current == 0 && (StaticRef<uint32>(0xC9BF24) & 0x180u) != 0) {
                StaticRef<uint32>(0xC992B0) = 1;
                D3D9QueueRenderState(StaticRef<uint32>(0xC992B4), kD3DRSFogEnable);
                current = 1;
            }
        } else if (current != 0) {
            StaticRef<uint32>(0xC992B0) = 0;
            D3D9QueueRenderState(StaticRef<uint32>(0xC992B4), kD3DRSFogEnable);
            current = 0;
        }
        return TRUE;
    }
    case 14: { // FOGCOLOR
        auto& current = StaticRef<uint32>(0xC9A500);
        if (raw != current) {
            StaticRef<uint32>(0xC992E0) = raw;
            D3D9QueueRenderState(StaticRef<uint32>(0xC992E4), kD3DRSFogColor);
            current = raw;
        }
        return TRUE;
    }
    case 15: { // FOGTYPE
        auto& current = StaticRef<uint32>(0xC9A4F8);
        if (raw != current) {
            const uint32 caps = StaticRef<uint32>(0xC9BF24);
            if ((caps & 0x100u) != 0 && (caps & 0x100000u) != 0) {
                StaticRef<uint32>(0xC992E8) = reinterpret_cast<const uint32*>(0x8848FC)[raw];
                D3D9QueueRenderState(StaticRef<uint32>(0xC992EC), kD3DRSFogTableMode);
                if (StaticRef<uint32>(0xC99630) != 0) {
                    StaticRef<uint32>(0xC99630) = 0;
                    D3D9QueueRenderState(StaticRef<uint32>(0xC99634), kD3DRSFogVertexMode);
                }
            } else {
                if (StaticRef<uint32>(0xC992E8) != 0) {
                    StaticRef<uint32>(0xC992E8) = 0;
                    D3D9QueueRenderState(StaticRef<uint32>(0xC992EC), kD3DRSFogTableMode);
                }
                StaticRef<uint32>(0xC99630) = reinterpret_cast<const uint32*>(0x8848FC)[raw];
                D3D9QueueRenderState(StaticRef<uint32>(0xC99634), kD3DRSFogVertexMode);
            }
            current = raw;
        }
        if (raw == 1) { // niebla lineal: FOGEND = plano lejano y FOGSTART = plano cercano de la camara actual (comparados como enteros)
            const uint8* const camera = StaticRef<const uint8*>(0xC9BCC0);
            const uint32 farPlane  = *reinterpret_cast<const uint32*>(camera + 0x88);
            const uint32 nearPlane = *reinterpret_cast<const uint32*>(camera + 0x84);
            if (StaticRef<uint32>(0xC992F0) != farPlane) {
                StaticRef<uint32>(0xC992F0) = farPlane;
                D3D9QueueRenderState(StaticRef<uint32>(0xC992F4), kD3DRSFogEnd);
            }
            if (StaticRef<uint32>(0xC992F8) != nearPlane) {
                StaticRef<uint32>(0xC992F8) = nearPlane;
                D3D9QueueRenderState(StaticRef<uint32>(0xC992FC), kD3DRSFogStart);
            }
        }
        return TRUE;
    }
    case 16: { // FOGDENSITY: *valor es un float; se copian los bits tal cual y se compara como float (un NaN siempre cuenta como distinto)
        const uint32 bits = *static_cast<const uint32*>(value);
        const uint32 currentBits = StaticRef<uint32>(0xC9A4FC);
        float newDensity;
        float currentDensity;
        memcpy(&newDensity, &bits, sizeof(float));
        memcpy(&currentDensity, &currentBits, sizeof(float));
        if (!(newDensity == currentDensity)) {
            StaticRef<uint32>(0xC99300) = bits;
            D3D9QueueRenderState(StaticRef<uint32>(0xC99304), kD3DRSFogDensity);
            StaticRef<uint32>(0xC9A4FC) = bits;
        }
        return TRUE;
    }
    case 17:
    case 18:
        return FALSE;
    case 19:
        return _rwD3D9RenderStateCullMode(static_cast<RwInt32>(raw));
    case 20:
        return _rwD3D9RenderStateStencilEnable(static_cast<RwInt32>(raw));
    case 21:
        return _rwD3D9RenderStateStencilFail(static_cast<RwInt32>(raw));
    case 22:
        return _rwD3D9RenderStateStencilZFail(static_cast<RwInt32>(raw));
    case 23:
        return _rwD3D9RenderStateStencilPass(static_cast<RwInt32>(raw));
    case 24:
        return _rwD3D9RenderStateStencilFunction(static_cast<RwInt32>(raw));
    case 25:
        return _rwD3D9RenderStateStencilFunctionRef(static_cast<RwInt32>(raw));
    case 26:
        return _rwD3D9RenderStateStencilFunctionMask(static_cast<RwInt32>(raw));
    case 27:
        return _rwD3D9RenderStateStencilFunctionWriteMask(static_cast<RwInt32>(raw));
    case 28: { // ALPHATESTFUNCTION
        auto& current = StaticRef<uint32>(0xC9A5D0);
        if (raw != current) {
            const uint32 alphaBlend = StaticRef<uint32>(0xC992A8);
            const uint32 testEnable = raw != 8 ? 1u : 0u;
            current = raw;
            StaticRef<uint32>(0xC9A5D4) = testEnable;
            if (alphaBlend != 0) {
                RwD3D9SetRenderState(kD3DRSAlphaTestEnable, testEnable);
            }
            StaticRef<uint32>(0xC99298) = reinterpret_cast<const uint32*>(0x8849C4)[raw];
            D3D9QueueRenderState(StaticRef<uint32>(0xC9929C), kD3DRSAlphaFunc);
        }
        return TRUE;
    }
    default: // 29: ALPHATESTFUNCTIONREF
        RwD3D9SetRenderState(kD3DRSAlphaRef, raw);
        return TRUE;
    }
}

// 0x7FED70 (lote R11A, sin validar; cdecl, (RwInt32* ancho, RwInt32* alto, RwUInt32 formato), `ret` sin valor de retorno definido, 176 bytes): ajusta el tamano de una textura a las capacidades del dispositivo.
// Limita a MaxTextureWidth (0xC9BF58) y MaxTextureHeight (0xC9BF5C) (comparacion con signo). TextureCaps = 0xC9BF3C. Potencia de dos (cada lado pasa a 1 << floor(log2(lado)); un lado a 0 da 0x80000000 porque el
// desplazamiento 0xFF se reduce a 31): con (formato & 7) != 0 y el bit 0x8000 (mipmap) si TextureCaps & 2 (POW2); si no, con TextureCaps & 2 y sin TextureCaps & 0x100 (NONPOW2CONDITIONAL).
// Despues, con TextureCaps & 0x20 (SQUAREONLY), los dos lados valen el menor (comparacion con signo).
void _rwD3D9CheckValidTextureSize(RwInt32* width, RwInt32* height, RwUInt32 format) {
    const auto floorLog2 = [](RwInt32 value) {
        RwInt32 log = -1;
        while (value != 0) {
            ++log;
            value >>= 1;
        }
        return log;
    };
    if (*width > StaticRef<RwInt32>(0xC9BF58)) {
        *width = StaticRef<RwInt32>(0xC9BF58);
    }
    if (*height > StaticRef<RwInt32>(0xC9BF5C)) {
        *height = StaticRef<RwInt32>(0xC9BF5C);
    }
    const uint32 caps = StaticRef<uint32>(0xC9BF3C);
    bool roundToPowerOfTwo;
    if ((format & 7u) != 0 && (format & 0x8000u) != 0) {
        roundToPowerOfTwo = (caps & 2u) != 0;
    } else {
        roundToPowerOfTwo = (caps & 2u) != 0 && (caps & 0x100u) == 0;
    }
    if (roundToPowerOfTwo) {
        *width  = static_cast<RwInt32>(1u << (static_cast<uint32>(floorLog2(*width)) & 31u));
        *height = static_cast<RwInt32>(1u << (static_cast<uint32>(floorLog2(*height)) & 31u));
    }
    if ((caps & 0x20u) != 0) {
        const RwInt32 w = *width;
        const RwInt32 h = *height;
        if (w < h) {
            *height = w;
        } else {
            *width = h;
        }
    }
}

// 0x7FEE20 (lote R11A, sin validar; cdecl, (RwUInt32* pixel, const RwRGBA* color, RwUInt32 formato), `ret`, 587 bytes): empaqueta un RwRGBA (bytes R, G, B, A) en el formato de raster (formato & 0xF00):
// 0x100 1555, 0x200 565, 0x300 4444, 0x400 luminancia (59 G + 11 B + 30 R) / 100 con signo, por A / 255 (multiplicacion por 0x80808081 >> 39), 0x500 y 0 argb de 32 bits, 0x600 xrgb con alfa 0xFF, 0xA00 555.
// Otro formato: error 0x8000000D (RwError { 1, _rwerror(...) } + RwErrorSet) y devuelve 0. Si no, escribe *pixel y devuelve 1.
RwBool _rwD3D9RGBToPixel(RwUInt32* pixel, const RwUInt8* color, RwUInt32 format) {
    const uint32 r = color[0], g = color[1], b = color[2], a = color[3];
    uint32 value;
    switch (format & 0xF00u) {
    case 0x100:
        value = ((((a & 0x80u) * 2u) | (r & 0xF8u)) << 5 | (g & 0xF8u)) << 2 | (b >> 3);
        break;
    case 0x200:
        value = (((r & 0xF8u) << 5) | (g & 0xFCu)) << 3 | (b >> 3);
        break;
    case 0x300:
        value = ((((a & 0xF0u) << 4) | (r & 0xF0u)) << 4) | ((g & 0xF0u) | (b >> 4));
        break;
    case 0x400: {
        const int32 sum = static_cast<int32>(g * 59u + b * 11u + r * 30u);
        const int32 luminance = sum / 100;
        const uint32 product = static_cast<uint32>(luminance * static_cast<int32>(a));
        value = static_cast<uint32>((static_cast<uint64>(product) * 0x80808081ull) >> 39);
        break;
    }
    case 0:
    case 0x500:
        value = (a << 24) | (r << 16) | (g << 8) | b;
        break;
    case 0x600:
        value = 0xFF000000u | (r << 16) | (g << 8) | b;
        break;
    case 0xA00:
        value = (((r & 0xF8u) << 5) | (g & 0xF8u)) << 2 | (b >> 3);
        break;
    default:
        D3D9RaiseError(static_cast<RwInt32>(0x8000000D));
        return FALSE;
    }
    *pixel = value;
    return TRUE;
}

// 0x7FF070 (lote R11A, sin validar; cdecl, (RwRGBA* color, const RwUInt32* pixel, RwUInt32 formato), `ret`, 501 bytes): desempaqueta un pixel. Con formato & 0x2000 (paleta de 8 bits) copia el dword tal cual y devuelve 1.
// Si no (formato & 0xF00): 0x100 1555 (alfa 0xFF con el bit 15), 0x200 565 (alfa 0xFF), 0x300 4444 (alfa 0xFF si los cuatro bits son 1; si no el nibble alto del byte alto), 0x400 gris (R = G = B = byte bajo, alfa 0xFF),
// 0x500 argb, 0x600 xrgb (alfa 0xFF), 0xA00 555 (alfa 0xFF); canales de 5/6/4 bits SIN replicar los bits bajos. Otro formato: error 0x8000000D y devuelve 0. Los bytes se escriben en orden R, G, B, A.
RwBool _rwD3D9PixelToRGB(RwUInt8* color, const RwUInt32* pixel, RwUInt32 format) {
    if ((format & 0x2000u) != 0) {
        *reinterpret_cast<RwUInt32*>(color) = *pixel;
        return TRUE;
    }
    const uint32 v = *pixel;
    switch (format & 0xF00u) {
    case 0x400:
        color[3] = 0xFF;
        color[0] = static_cast<RwUInt8>(v);
        color[1] = static_cast<RwUInt8>(v);
        color[2] = static_cast<RwUInt8>(v);
        return TRUE;
    case 0x100:
        color[3] = (v & 0x8000u) != 0 ? 0xFF : 0;
        color[0] = static_cast<RwUInt8>((v >> 7) & 0xF8u);
        color[2] = static_cast<RwUInt8>(v << 3);
        color[1] = static_cast<RwUInt8>((v >> 2) & 0xF8u);
        return TRUE;
    case 0x200:
        color[3] = 0xFF;
        color[0] = static_cast<RwUInt8>((v >> 8) & 0xF8u);
        color[2] = static_cast<RwUInt8>(v << 3);
        color[1] = static_cast<RwUInt8>((v >> 3) & 0xFCu);
        return TRUE;
    case 0x300:
        if ((v & 0xF000u) == 0xF000u) {
            color[3] = 0xFF;
        } else {
            color[3] = static_cast<RwUInt8>((v >> 8) & 0xF0u);
        }
        color[0] = static_cast<RwUInt8>((v >> 4) & 0xF0u);
        color[2] = static_cast<RwUInt8>(v << 4);
        color[1] = static_cast<RwUInt8>(v & 0xF0u);
        return TRUE;
    case 0x500:
        color[2] = static_cast<RwUInt8>(v);
        color[0] = static_cast<RwUInt8>(v >> 16);
        color[3] = static_cast<RwUInt8>(v >> 24);
        color[1] = static_cast<RwUInt8>(v >> 8);
        return TRUE;
    case 0x600:
        color[3] = 0xFF;
        color[2] = static_cast<RwUInt8>(v);
        color[0] = static_cast<RwUInt8>(v >> 16);
        color[1] = static_cast<RwUInt8>(v >> 8);
        return TRUE;
    case 0xA00:
        color[3] = 0xFF;
        color[0] = static_cast<RwUInt8>((v >> 7) & 0xF8u);
        color[2] = static_cast<RwUInt8>(v << 3);
        color[1] = static_cast<RwUInt8>((v >> 2) & 0xF8u);
        return TRUE;
    default:
        D3D9RaiseError(static_cast<RwInt32>(0x8000000D));
        return FALSE;
    }
}

// 0x7FF8C0 (lote R11A, sin validar; nombre inferido; cdecl, (RwImage* imagen, RwRaster* raster), `ret` sin valor definido, 564 bytes): conversion generica raster -> imagen de 32 bits por una funcion por pixel
// segun el D3DFORMAT de la extension del raster padre (ext + 0x18 menos 0x15 como indice de la tabla 0x7FFB40; formatos sin entrada dejan el puntero a 0, que el EXE llamaria igualmente: aqui se sale sin escribir).
// Las funciones por pixel son cdecl (RwUInt32* destino, RwUInt32 valor): D3DFMT 0x15 -> 0x7FFDF0, 0x16 -> 0x7FFD90, 0x17 -> 0x7FFC10, 0x18 -> 0x7FFBE0, 0x19 -> 0x7FFC70, 0x1A -> 0x7FFCB0, 0x1B -> 0x7FFD30,
// 0x1C -> 0x7FFE70, 0x1D -> 0x7FFD00, 0x1E -> 0x7FFD60, 0x20 y 0x3F -> 0x7FFE20, 0x21 y 0x3E -> 0x7FFDB0, 0x32 -> 0x7FFE50, 0x33 -> 0x7FFEC0, 0x34 -> 0x7FFE90, 0x3C y 0x75 -> 0x7FFDD0, 0x3D -> 0x7FFC40,
// 0x51 -> 0x7FFEE0. Profundidad del raster (menos 4, maximo 0x1C): 4 y 8 leen un byte por pixel, 16 un word, 32 un dword; otra profundidad (24 incluida): error 0x8000000C (RwError { 1, _rwerror(...) }). Recorre las
// filas de 0 a alto - 1 (si el alto es <= 0 no escribe nada; el ancho se recorre con do-while).
void _rwD3D9RasterToImageGeneric(RwImage* image, RwRaster* raster) {
    using PixelConvert = void(__cdecl*)(RwUInt32*, RwUInt32);
    uintptr_t convert = 0;
    switch (D3D9RasterExt(raster->parent).d3dFormat) {
    case 0x15: convert = 0x7FFDF0; break;
    case 0x16: convert = 0x7FFD90; break;
    case 0x17: convert = 0x7FFC10; break;
    case 0x18: convert = 0x7FFBE0; break;
    case 0x19: convert = 0x7FFC70; break;
    case 0x1A: convert = 0x7FFCB0; break;
    case 0x1B: convert = 0x7FFD30; break;
    case 0x1C: convert = 0x7FFE70; break;
    case 0x1D: convert = 0x7FFD00; break;
    case 0x1E: convert = 0x7FFD60; break;
    case 0x20: convert = 0x7FFE20; break;
    case 0x21: convert = 0x7FFDB0; break;
    case 0x32: convert = 0x7FFE50; break;
    case 0x33: convert = 0x7FFEC0; break;
    case 0x34: convert = 0x7FFE90; break;
    case 0x3C: convert = 0x7FFDD0; break;
    case 0x3D: convert = 0x7FFC40; break;
    case 0x3E: convert = 0x7FFDB0; break;
    case 0x3F: convert = 0x7FFE20; break;
    case 0x51: convert = 0x7FFEE0; break;
    case 0x75: convert = 0x7FFDD0; break;
    default: break;
    }
    const RwInt32 depthIndex = raster->depth - 4;
    uint32 unit;
    if (depthIndex == 0 || depthIndex == 4) {
        unit = 1;
    } else if (depthIndex == 12) {
        unit = 2;
    } else if (depthIndex == 28) {
        unit = 4;
    } else {
        D3D9RaiseError(static_cast<RwInt32>(0x8000000C));
        return;
    }
    if (convert == 0) {
        return; // el EXE hace `call 0` aqui
    }
    for (RwInt32 y = 0; y < raster->height; ++y) {
        const RwUInt8* source = raster->cpPixels + static_cast<RwInt32>(raster->stride * y);
        RwUInt8* destination  = image->cpPixels + static_cast<RwInt32>(image->stride * y);
        uint32 count = static_cast<uint32>(raster->width);
        do {
            RwUInt32 pixel;
            if (unit == 1) {
                pixel = *source;
            } else if (unit == 2) {
                pixel = *reinterpret_cast<const uint16*>(source);
            } else {
                pixel = *reinterpret_cast<const uint32*>(source);
            }
            reinterpret_cast<PixelConvert>(convert)(reinterpret_cast<RwUInt32*>(destination), pixel);
            source += unit;
            destination += 4;
            --count;
        } while (count != 0);
    }
}

// 0x7FF450 (lote R11A, sin validar; nombre inferido; cdecl, (RwImage* imagen de 32 bits, RwRaster* raster), `ret` sin valor definido, 1087 bytes): raster -> imagen de 32 bits (R, G, B, A en ese orden de bytes).
// Sin formato de pixel en cFormat & 0xF: 0x7FF8C0. Si no, por la profundidad del raster (menos 4; fuera de 0..0x1C = error 0x8000000C; solo 4, 8, 16, 24 y 32 valen, otras caen en el mismo error): 4 y 8 con paleta: dword
// paleta[indice] por pixel; sin paleta: gris 0xFF000000 | indice * 0x010101. 16: segun el formato (0x100 1555, 0x200 565, 0x300 4444, 0xA00 555; otro: no escribe). 24 y 32: segun el D3DFORMAT de la extension del
// raster padre: 0x16 (xrgb, alfa 0xFF), 0x15 (argb) y 0x23 (a2r10g10b10: canales >> 22, >> 12, >> 2 truncados y alfa = (v >> 30) * 0x55); otro formato: 0x7FF8C0. Alto <= 0 no escribe nada; el ancho se recorre con do-while.
void _rwD3D9RasterToImage32(RwImage* image, RwRaster* raster) {
    const uint32 format = static_cast<uint32>(raster->cFormat & 0x0Fu) << 8;
    if (format == 0) {
        _rwD3D9RasterToImageGeneric(image, raster);
        return;
    }
    const uint32 depthIndex = static_cast<uint32>(raster->depth - 4);
    if (depthIndex > 0x1Cu) {
        D3D9RaiseError(static_cast<RwInt32>(0x8000000C));
        return;
    }
    const auto forEachPixel = [&](auto&& convertPixel, uint32 sourceSize) {
        for (RwInt32 y = 0; y < raster->height; ++y) {
            const RwUInt8* source = raster->cpPixels + static_cast<RwInt32>(raster->stride * y);
            RwUInt8* destination  = image->cpPixels + static_cast<RwInt32>(image->stride * y);
            uint32 count = static_cast<uint32>(raster->width);
            do {
                convertPixel(source, destination);
                source += sourceSize;
                destination += 4;
                --count;
            } while (count != 0);
        }
    };
    if (depthIndex == 0 || depthIndex == 4) { // 4 y 8 bits
        const uint32* const palette = reinterpret_cast<const uint32*>(raster->palette);
        if (palette != nullptr) {
            forEachPixel([palette](const RwUInt8* source, RwUInt8* destination) {
                *reinterpret_cast<uint32*>(destination) = palette[*source];
            }, 1);
        } else {
            forEachPixel([](const RwUInt8* source, RwUInt8* destination) {
                const uint32 index = *source;
                *reinterpret_cast<uint32*>(destination) = 0xFF000000u | (index << 16) | (index << 8) | index;
            }, 1);
        }
        return;
    }
    if (depthIndex == 12) { // 16 bits
        switch (format) {
        case 0x200:
            forEachPixel([](const RwUInt8* source, RwUInt8* destination) {
                const uint32 v = *reinterpret_cast<const uint16*>(source);
                destination[3] = 0xFF;
                destination[0] = static_cast<RwUInt8>((v >> 8) & 0xF8u);
                destination[1] = static_cast<RwUInt8>((v >> 3) & 0xFCu);
                destination[2] = static_cast<RwUInt8>(v << 3);
            }, 2);
            break;
        case 0x100:
            forEachPixel([](const RwUInt8* source, RwUInt8* destination) {
                const uint32 v = *reinterpret_cast<const uint16*>(source);
                destination[0] = static_cast<RwUInt8>((v >> 7) & 0xF8u);
                destination[1] = static_cast<RwUInt8>((v >> 2) & 0xF8u);
                destination[2] = static_cast<RwUInt8>(v << 3);
                destination[3] = (v & 0x8000u) != 0 ? 0xFF : 0;
            }, 2);
            break;
        case 0x300:
            forEachPixel([](const RwUInt8* source, RwUInt8* destination) {
                const uint32 v = *reinterpret_cast<const uint16*>(source);
                destination[0] = static_cast<RwUInt8>((v >> 4) & 0xF0u);
                destination[1] = static_cast<RwUInt8>(v & 0xF0u);
                destination[2] = static_cast<RwUInt8>(v << 4);
                destination[3] = (v & 0xF000u) == 0xF000u ? 0xFF : static_cast<RwUInt8>((v >> 8) & 0xF0u);
            }, 2);
            break;
        case 0xA00:
            forEachPixel([](const RwUInt8* source, RwUInt8* destination) {
                const uint32 v = *reinterpret_cast<const uint16*>(source);
                destination[3] = 0xFF;
                destination[0] = static_cast<RwUInt8>((v >> 7) & 0xF8u);
                destination[1] = static_cast<RwUInt8>((v >> 2) & 0xF8u);
                destination[2] = static_cast<RwUInt8>(v << 3);
            }, 2);
            break;
        default:
            break;
        }
        return;
    }
    if (depthIndex == 20 || depthIndex == 28) { // 24 y 32 bits
        const uint32 d3dFormat = D3D9RasterExt(raster->parent).d3dFormat;
        if (d3dFormat == 0x16) {
            forEachPixel([](const RwUInt8* source, RwUInt8* destination) {
                const uint32 v = *reinterpret_cast<const uint32*>(source);
                destination[3] = 0xFF;
                destination[2] = static_cast<RwUInt8>(v);
                destination[0] = static_cast<RwUInt8>(v >> 16);
                destination[1] = static_cast<RwUInt8>(v >> 8);
            }, 4);
            return;
        }
        if (d3dFormat == 0x15) {
            forEachPixel([](const RwUInt8* source, RwUInt8* destination) {
                const uint32 v = *reinterpret_cast<const uint32*>(source);
                destination[2] = static_cast<RwUInt8>(v);
                destination[0] = static_cast<RwUInt8>(v >> 16);
                destination[1] = static_cast<RwUInt8>(v >> 8);
                destination[3] = static_cast<RwUInt8>(v >> 24);
            }, 4);
            return;
        }
        if (d3dFormat == 0x23) {
            forEachPixel([](const RwUInt8* source, RwUInt8* destination) {
                const uint32 v = *reinterpret_cast<const uint32*>(source);
                destination[0] = static_cast<RwUInt8>(v >> 22);
                destination[1] = static_cast<RwUInt8>(v >> 12);
                destination[2] = static_cast<RwUInt8>(v >> 2);
                destination[3] = static_cast<RwUInt8>(static_cast<int8>(v >> 30) * 0x55);
            }, 4);
            return;
        }
        _rwD3D9RasterToImageGeneric(image, raster);
        return;
    }
    D3D9RaiseError(static_cast<RwInt32>(0x8000000C));
}

// 0x7FF270 (lote R11A, sin validar; nombre inferido: RwImageSetFromRaster del controlador; cdecl, (RwImage*, RwRaster*) (el llamador de rwcore pasa un tercer argumento que no se lee), `ret`, 468 bytes): copia el
// contenido de un raster a una imagen. Con raster comprimido (extension +0xA & 0xF0): error 2 con la cadena 0x8E2600 y devuelve 0. Si el raster no esta bloqueado (privateFlags & 2 == 0): RwRasterLock(raster, 0, 2)
// (marca X); con cFormat & 0x60 y sin privateFlags & 8: RwRasterLockPalette(raster, 2) (marca Y). Con la imagen de 32 bits: 0x7FF450; con 8 bits y un raster de 8 bits con formato de paleta (cFormat & 0xF): copia
// de pixeles (de una vez si los tres pasos coinciden, fila a fila si no) y la paleta de 256 dwords (o, sin paleta, una escala de grises con alfa 0xFF); otro caso: error 2 con la cadena 0x8E25B8 (raster no
// paletizado a 8 bits) o 0x8E257C (profundidad de imagen distinta de 8 y 32). Al final desbloquea la paleta (si Y) y los pixeles (si X). Devuelve 1.
RwBool _rwD3D9RasterToImage(RwImage* image, RwRaster* raster) {
    const auto& ext = D3D9RasterExt(raster);
    if ((ext.mipFlags & 0xF0u) != 0) {
        D3D9RaiseError(2, reinterpret_cast<const char*>(0x8E2600));
        return FALSE;
    }
    bool lockedPixels = false;
    bool lockedPalette = false;
    if ((raster->privateFlags & 2u) == 0) {
        RwRasterLock(raster, 0, 2);
        lockedPixels = true;
    }
    if ((raster->cFormat & 0x60u) != 0 && (raster->privateFlags & 8u) == 0) {
        RwRasterLockPalette(raster, 2);
        lockedPalette = true;
    }
    if (image->depth == 0x20) {
        _rwD3D9RasterToImage32(image, raster);
    } else if (image->depth != 8) {
        D3D9RaiseError(2, reinterpret_cast<const char*>(0x8E257C));
    } else if (raster->depth != 8 || (raster->cFormat & 0x0Fu) == 0) {
        D3D9RaiseError(2, reinterpret_cast<const char*>(0x8E25B8));
    } else {
        if (image->stride == raster->stride && raster->width == raster->stride) {
            memcpy(image->cpPixels, raster->cpPixels, static_cast<size_t>(static_cast<uint32>(raster->height * raster->width)));
        } else {
            for (RwInt32 y = 0; y < raster->height; ++y) {
                memcpy(image->cpPixels + static_cast<RwInt32>(image->stride * y), raster->cpPixels + static_cast<RwInt32>(raster->stride * y), static_cast<size_t>(static_cast<uint32>(raster->width)));
            }
        }
        if (raster->palette != nullptr) {
            memcpy(image->palette, raster->palette, 0x100 * sizeof(uint32));
        } else {
            auto* const palette = reinterpret_cast<RwUInt8*>(image->palette);
            for (int i = 0; i < 0x100; ++i) {
                palette[i * 4]     = static_cast<RwUInt8>(i);
                palette[i * 4 + 1] = static_cast<RwUInt8>(i);
                palette[i * 4 + 2] = static_cast<RwUInt8>(i);
                palette[i * 4 + 3] = 0xFF;
            }
        }
    }
    if (lockedPalette) {
        RwRasterUnlockPalette(raster);
    }
    if (lockedPixels) {
        RwRasterUnlock(raster);
    }
    return TRUE;
}

// 0x7FBB00 (lote R11A, sin validar; nombre inferido: RwIm2DRenderPrimitive del controlador D3D9; cdecl, (RwPrimitiveType tipo, RwIm2DVertex* vertices, RwInt32 numero), `ret`, 534 bytes): copia `numero` vertices de
// 0x1C bytes (x, y, z, rhw, color, u, v) a un bufer dinamico y los dibuja. Con 0xC980E0 distinto de 0 usa el bufer propio (0xC980E8; posicion en vertices 0xC980E4; si posicion + numero > 0x2492 la pone a 0 y bloquea
// con descarte 0x2800, si no con NOOVERWRITE 0x1800); sin el: RwD3D9DynamicVertexBufferLock(0x1C, numero, 0xC980E8, &datos, 0xC980E4). Sin datos devuelve 0. Si el raster del marco de la camara (camara actual 0xC9BCC0 + 0x60, o el
// contexto de raster 0x7FAE60) tiene desplazamiento (nOffsetX, nOffsetY) distinto de 0, suma el desplazamiento a x e y de cada vertice; si no copia. Desbloquea, llama a 0x7FB810 (estados del dibujo 2D) y DrawPrimitive(tipo D3D de
// la tabla 0x884888, inicio 0xC980E4, primitivas) por la ranura 0x144; 0xC980E4 += numero; TRUE si el HRESULT no es negativo. Primitivas: tipo 1 numero / 2, 2 numero - 1, 3 numero / 3, 4 y 5 numero - 2, otro 0.
RwBool _rwD3D9Im2DRenderPrimitive(RwInt32 primType, void* vertices, RwInt32 numVertices) {
    constexpr size_t kSlotDeviceDrawPrimitive = 81; // IDirect3DDevice9::DrawPrimitive (+0x144)
    auto& bufferInUse  = StaticRef<uint32>(0xC980E0);
    auto& position     = StaticRef<uint32>(0xC980E4);
    auto& vertexBuffer = StaticRef<void*>(0xC980E8);
    const uint8* const camera = StaticRef<const uint8*>(0xC9BCC0);
    const RwRaster* const frameBuffer = camera != nullptr ? *reinterpret_cast<RwRaster* const*>(camera + 0x60) : RwRasterGetCurrentContext();
    RwInt32 primitives;
    switch (primType) {
    case 1: primitives = numVertices / 2; break;
    case 2: primitives = numVertices - 1; break;
    case 3: primitives = numVertices / 3; break;
    case 4:
    case 5: primitives = numVertices - 2; break;
    default: primitives = 0; break;
    }
    const uint32 byteCount = static_cast<uint32>(numVertices) * 28u;
    uint8* data = nullptr;
    if (bufferInUse != 0) {
        const uint32 start = position;
        if (start + static_cast<uint32>(numVertices) > 0x2492u) {
            position = 0;
            ComCall<kSlotVertexBufferLock, int32>(vertexBuffer, 0u, byteCount, static_cast<void*>(&data), 0x2800u);
        } else {
            ComCall<kSlotVertexBufferLock, int32>(vertexBuffer, start * 28u, byteCount, static_cast<void*>(&data), 0x1800u);
        }
    } else {
        RwD3D9DynamicVertexBufferLock(0x1C, static_cast<RwUInt32>(numVertices), &vertexBuffer, reinterpret_cast<void**>(&data), &position);
    }
    if (data == nullptr) {
        return FALSE;
    }
    const int16 offsetX = frameBuffer->nOffsetX;
    const int16 offsetY = frameBuffer->nOffsetY;
    if (offsetX == 0 && offsetY == 0) {
        memcpy(data, vertices, static_cast<size_t>(byteCount));
    } else {
        const float shiftX = static_cast<float>(static_cast<int32>(offsetX));
        const float shiftY = static_cast<float>(static_cast<int32>(offsetY));
        const uint32* source = static_cast<const uint32*>(vertices);
        uint32* destination  = reinterpret_cast<uint32*>(data);
        for (RwInt32 i = 0; i < numVertices; ++i) {
            float x, y;
            memcpy(&x, source, sizeof(float));
            memcpy(&y, source + 1, sizeof(float));
            x = shiftX + x;
            y = shiftY + y;
            memcpy(destination, &x, sizeof(float));
            memcpy(destination + 1, &y, sizeof(float));
            destination[2] = source[2];
            destination[3] = source[3];
            destination[4] = source[4];
            destination[5] = source[5];
            destination[6] = source[6];
            source += 7;
            destination += 7;
        }
    }
    ComCall<kSlotVertexBufferUnlock, int32>(vertexBuffer);
    reinterpret_cast<void(__cdecl*)()>(0x7FB810)();
    const uint32 d3dPrimitive = reinterpret_cast<const uint32*>(0x884888)[primType];
    const int32 result = ComCall<kSlotDeviceDrawPrimitive, int32>(D3D9Device, d3dPrimitive, static_cast<uint32>(position), static_cast<uint32>(primitives));
    position += static_cast<uint32>(numVertices);
    return result >= 0;
}

// Lote R11A (sin validar; segundo encargo con ese nombre): controlador D3D9 de RenderWare (RwD3D9).
void RwD3D9InjectHooksR11A2() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(_rwD3D9Im2DRenderPrimitive, 0x7FBB00);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateVertexAlphaEnable, 0x7FE0A0);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateGetVertexAlphaEnable, 0x7FE190);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateStencilEnable, 0x7FE1A0);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateStencilFail, 0x7FE1F0);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateStencilZFail, 0x7FE240);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateStencilPass, 0x7FE290);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateStencilFunction, 0x7FE2E0);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateStencilFunctionRef, 0x7FE330);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateStencilFunctionMask, 0x7FE380);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateStencilFunctionWriteMask, 0x7FE3D0);
    RH_ScopedGlobalInstall(_rwD3D9RWSetRenderState, 0x7FE420);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateZWriteEnable, 0x7FEA70);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateZTestEnable, 0x7FEB70);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateShadeMode, 0x7FEC70);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateBorderColor, 0x7FECC0);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateCullMode, 0x7FED00);
    RH_ScopedGlobalInstall(_rwD3D9CheckValidTextureSize, 0x7FED70);
    RH_ScopedGlobalInstall(_rwD3D9RGBToPixel, 0x7FEE20);
    RH_ScopedGlobalInstall(_rwD3D9PixelToRGB, 0x7FF070);
    RH_ScopedGlobalInstall(_rwD3D9RasterToImage, 0x7FF270);
    RH_ScopedGlobalInstall(_rwD3D9RasterToImage32, 0x7FF450);
    RH_ScopedGlobalInstall(_rwD3D9RasterToImageGeneric, 0x7FF8C0);
}

// Estados de render y etapas de textura del controlador D3D9 (lote R6A, sin validar).
void RwD3D9InjectHooksR6A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(_rwD3D9RenderStateFlushCache, 0x7FC200); // lote R10A (sin validar)
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

// Lote R23A (sin validar): RwCamera (bacamera.c). Cuerpos numericos en X87Math.cpp (CameraZScaleCpp, CameraRecipViewWindowCpp, CameraFrustumTestSphereCpp, CameraFrustumPerspectiveCpp,
// CameraFrustumParallel*Cpp, CameraParallelViewMatrixCpp). Datos del EXE: RwEngineInstance + [0xC97960] = modulo de camaras (+0 lista libre; contador en 0xC97964); registro de plugins 0x8E222C.
static_assert(offsetof(RwGlobals, dOpenDevice) + offsetof(RwDevice, zBufferNear) == 0x18 && offsetof(RwGlobals, dOpenDevice) + offsetof(RwDevice, zBufferFar) == 0x1C);
static_assert(offsetof(RwCamera, recipViewWindow) == offsetof(RwCamera, viewWindow) + 8 && offsetof(RwCamera, viewWindow) == 0x68 && offsetof(RwCamera, nearPlane) == 0x80 && offsetof(RwCamera, farPlane) == 0x84);
static_assert(offsetof(RwCamera, zScale) == 0x8C && offsetof(RwCamera, zShift) == 0x90 && offsetof(RwCamera, frustumPlanes) == 0x94 && offsetof(RwCamera, frustumCorners) == 0x124);
static_assert(offsetof(RwCamera, viewMatrix) == 0x20 && offsetof(RwCamera, projectionType) == 0x14 && offsetof(RwCamera, viewOffset) == 0x78 && sizeof(RwFrustumPlane) == 0x14);
static_assert(offsetof(RwFrame, ltm) == 0x50);

// 0x7EE200 (lote R23A, sin validar; 150 bytes; cdecl, (camara); nombre inferido): recalcula zScale (+0x8C) y zShift (+0x90) con los z de pantalla del dispositivo (RwEngineInstance + 0x18 y +0x1C), la proyeccion
// (+0x14; 2 = paralela) y near/far (ver CameraZScaleCpp). Deja la camara en EAX.
RwCamera* _rwCameraUpdateZScale(RwCamera* camera) {
    CameraZScaleCpp(RwEngineInstance->dOpenDevice.zBufferNear, RwEngineInstance->dOpenDevice.zBufferFar, static_cast<int>(camera->projectionType),
                    camera->nearPlane, camera->farPlane, &camera->zScale, &camera->zShift);
    return camera;
}

// 0x7EE1D0 y 0x7EE2A0 (lote R23A, sin validar; 44 bytes cada una; eran envolturas; cdecl, (camara, valor)): guardan el plano (+0x80 near, +0x84 far), llaman a 0x7EE200 y, si la camara tiene marco (+4),
// RwFrameUpdateObjects (0x7F0910). Devuelven la camara.
RwCamera* RwCameraSetNearClipPlane(RwCamera* camera, RwReal nearClip) {
    camera->nearPlane = nearClip;
    _rwCameraUpdateZScale(camera);
    if (auto* const parent = static_cast<RwFrame*>(camera->object.object.parent)) {
        RwFrameUpdateObjects(parent);
    }
    return camera;
}

RwCamera* RwCameraSetFarClipPlane(RwCamera* camera, RwReal farClip) {
    camera->farPlane = farClip;
    _rwCameraUpdateZScale(camera);
    if (auto* const parent = static_cast<RwFrame*>(camera->object.object.parent)) {
        RwFrameUpdateObjects(parent);
    }
    return camera;
}

// 0x7EE410 (lote R23A, sin validar; 64 bytes; era una envoltura; cdecl, (camara, ventana)): copia los dos dwords de la ventana a camera->viewWindow (+0x68) como bits y calcula recipViewWindow (+0x70, +0x74) = 1.0 / ventana
// (fld1; fdiv; fstp dword). Si la camara tiene marco (+4), RwFrameUpdateObjects (0x7F0910). Devuelve la camara.
RwCamera* RwCameraSetViewWindow(RwCamera* camera, const RwV2d* viewWindow) {
    const auto* const source = reinterpret_cast<const uint32*>(viewWindow);
    auto* const target = reinterpret_cast<uint32*>(&camera->viewWindow);
    target[0] = source[0];
    target[1] = source[1];
    CameraRecipViewWindowCpp(&camera->viewWindow.x);
    if (auto* const parent = static_cast<RwFrame*>(camera->object.object.parent)) {
        RwFrameUpdateObjects(parent);
    }
    return camera;
}

// 0x7EE3A0 (lote R23A, sin validar; 102 bytes; era una envoltura; cdecl, (camara, proyeccion)): proyeccion <= 0 o > 2 (con signo) -> RwError { 1, _rwerror(0x80000003, "Invalid projection type specified" (0x8E224C)) } y
// NULL. Si no guarda la proyeccion (+0x14), RwFrameUpdateObjects si hay marco y despues 0x7EE200. Devuelve la camara.
RwCamera* RwCameraSetProjection(RwCamera* camera, RwCameraProjection projection) {
    if (projection <= 0 || projection > 2) {
        RwError error;
        error.pluginID  = 1;
        error.errorCode = _rwerror(static_cast<RwInt32>(0x80000003), reinterpret_cast<const char*>(0x8E224C));
        RwErrorSet(&error);
        return nullptr;
    }
    camera->projectionType = projection;
    if (auto* const parent = static_cast<RwFrame*>(camera->object.object.parent)) {
        RwFrameUpdateObjects(parent);
    }
    _rwCameraUpdateZScale(camera);
    return camera;
}

// 0x7EE740 (lote R23A, sin validar; 1682 bytes; cdecl, (camara); nombre inferido; llamador: la sincronizacion de la camara 0x7EE5A0 en perspectiva): esquinas (+0x124) y los 6 planos (+0x94) del frustum a partir de la matriz
// ltm del marco (frame + 0x50), la ventana, el desplazamiento y near/far (ver CameraFrustumPerspectiveCpp; normaliza con _rwInvSqrt 0x7EDB90). El EXE deja basura en EAX.
void _rwCameraBuildFrustumPerspective(RwCamera* camera) {
    const auto* const frame = static_cast<const RwFrame*>(camera->object.object.parent);
    CameraFrustumPerspectiveCpp(reinterpret_cast<float*>(camera), reinterpret_cast<const float*>(&frame->ltm), &_rwInvSqrt);
}

// 0x7EEDE0 (lote R23A, sin validar; 1040 bytes; cdecl, (camara); nombre inferido; llamador: 0x7EE5A0 en paralela, despues de 0x7EF1F0): esquinas del frustum paralelo en espacio de camara, RwV3dTransformPoints (0x7EDD90)
// de las 8 esquinas sobre si mismas con la ltm del marco y los 6 planos (ver CameraFrustumParallelCornersCpp y CameraFrustumParallelPlanesCpp). El EXE deja basura en EAX.
void _rwCameraBuildFrustumParallel(RwCamera* camera) {
    auto* const cam = reinterpret_cast<float*>(camera);
    const auto* const frame = static_cast<const RwFrame*>(camera->object.object.parent);
    CameraFrustumParallelCornersCpp(cam);
    RwV3dTransformPoints(camera->frustumCorners, camera->frustumCorners, 8, &frame->ltm);
    CameraFrustumParallelPlanesCpp(cam, reinterpret_cast<const float*>(&frame->ltm), &_rwInvSqrt);
}

// 0x7EF1F0 (lote R23A, sin validar; 334 bytes; cdecl, (camara); nombre inferido; llamador: 0x7EE5A0 en paralela): matriz de vista de la camara paralela (+0x20) a partir de la ltm del marco, la inversa de la ventana (+0x70, +0x74)
// y el desplazamiento (ver CameraParallelViewMatrixCpp); termina con RwMatrixOptimize (0x7F17E0, matriz de vista, tolerancia NULL). Devuelve la camara.
RwCamera* _rwCameraBuildViewMatrixParallel(RwCamera* camera) {
    const auto* const frame = static_cast<const RwFrame*>(camera->object.object.parent);
    CameraParallelViewMatrixCpp(reinterpret_cast<float*>(camera), reinterpret_cast<const float*>(&frame->ltm));
    RwMatrixOptimize(&camera->viewMatrix, nullptr);
    return camera;
}

// 0x7EE4B0 (lote R23A, sin validar; 54 bytes; era una envoltura; cdecl, (camara)): _rwPluginRegistryDeInitObject(registro 0x8E222C, camara), _rwObjectHasFrameReleaseFrame (0x804F40) y libera la camara en la lista libre
// del modulo (RwFreeListFree, +0x148 del motor). Devuelve TRUE.
RwBool RwCameraDestroy(RwCamera* camera) {
    _rwPluginRegistryDeInitObject(reinterpret_cast<RwPluginRegistry*>(0x8E222C), camera);
    _rwObjectHasFrameReleaseFrame(camera);
    auto* const freeList = *reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC97960));
    RwFreeListFree(freeList, camera);
    return TRUE;
}

// 0x7EE4F0 (lote R23A, sin validar; 171 bytes; era una envoltura; cdecl, sin argumentos): reserva la camara en la lista libre del modulo (pista 0x30005; sin memoria -> NULL). Rellena: tipo 4 (rwCAMERA), subtipo, banderas y
// banderas privadas 0, parent NULL, sync = 0x7EE5A0, beginUpdate = 0x7EF370, endUpdate = 0x7EF340, ventana y su inversa (1, 1), desplazamiento (0, 0), near 0x3D4CCCCD (0.05f), far 10.0f, niebla 5.0f, raster y
// zbuffer NULL, proyeccion 1 (perspectiva); llama a 0x7EE200, pone viewMatrix.flags (+0x2C) a 0 e inicializa los plugins (0x8086E0 con el registro 0x8E222C). Devuelve la camara.
RwCamera* RwCameraCreate() {
    auto* const freeList = *reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC97960));
    auto* const camera = static_cast<RwCamera*>(RwFreeListAlloc(freeList, 0x30005));
    if (camera == nullptr) {
        return nullptr;
    }
    camera->object.object.type = 4;
    camera->object.object.subType = 0;
    camera->object.object.flags = 0;
    camera->object.object.privateFlags = 0;
    camera->object.object.parent = nullptr;
    camera->object.sync = reinterpret_cast<RwObjectHasFrameSyncFunction>(0x7EE5A0);
    camera->beginUpdate = reinterpret_cast<RwCameraBeginUpdateFunc>(0x7EF370);
    camera->endUpdate = reinterpret_cast<RwCameraEndUpdateFunc>(0x7EF340);
    camera->viewWindow.y = 1.0f;
    camera->viewWindow.x = 1.0f;
    camera->recipViewWindow.y = 1.0f;
    camera->recipViewWindow.x = 1.0f;
    camera->viewOffset.y = 0.0f;
    camera->viewOffset.x = 0.0f;
    camera->nearPlane = std::bit_cast<float>(0x3D4CCCCDu);
    camera->farPlane = std::bit_cast<float>(0x41200000u);
    camera->fogPlane = std::bit_cast<float>(0x40A00000u);
    camera->frameBuffer = nullptr;
    camera->zBuffer = nullptr;
    camera->projectionType = rwPERSPECTIVE;
    _rwCameraUpdateZScale(camera);
    camera->viewMatrix.flags = 0;
    _rwPluginRegistryInitObject(reinterpret_cast<RwPluginRegistry*>(0x8E222C), camera);
    return camera;
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

// 0x7EE2D0 (lote R23A, sin validar; 104 bytes; era una envoltura; cdecl, (camara, esfera)): por cada uno de los 6 planos (+0x94, de 0x14 bytes) d = ((n.y*c.y + n.x*c.x) + n.z*c.z) - distancia; d > radio -> 0 (fuera,
// sin mirar el resto); d > -radio -> resultado 1 (frontera); si nunca, 2 (dentro). Comparaciones ordenadas de la x87 (ver CameraFrustumTestSphereCpp).
RwFrustumTestResult RwCameraFrustumTestSphere(const RwCamera* camera, const RwSphere* sphere) {
    return static_cast<RwFrustumTestResult>(CameraFrustumTestSphereCpp(reinterpret_cast<const float*>(&camera->frustumPlanes[0]), reinterpret_cast<const float*>(sphere)));
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

// --- Lote R7A (sin validar): lectura de texturas con mipmaps del controlador D3D9 (0x7F4AE0; el listado la atribuia a RpSkin con confianza baja: es el cuerpo de RwTextureRead con mipmapping activo,
// la llama 0x7F40F0 cuando textureGlobals+0x1C es distinto de 0) ---
namespace {
// Copia con comprobacion de longitud como la macro de cadenas de RW en el EXE: rwstrncpy(destino, origen, size) (RwEngineInstance + 0xFC) y, si rwstrlen(origen) (+0x120) >= size, emite
// RwError { pluginID = 1, errorCode = _rwerror(0x8000001E, origen, size, size - 1, (char)origen[size - 1]) } con RwErrorSet (0x808820) y cierra la cadena (destino[size - 1] = 0).
// _rwerror (0x8088D0) se llama por direccion para conservar la secuencia de colaboradores.
static_assert(offsetof(RwGlobals, stringFuncs) + offsetof(RwStringFunctions, vecStrncpy) == 0xFC && offsetof(RwGlobals, stringFuncs) + offsetof(RwStringFunctions, vecStrlen) == 0x120);
void CopyStringChecked(char* destination, const char* source, uint32 size) {
    rwstrncpy(destination, source, size);
    if (static_cast<uint32>(rwstrlen(source)) >= size) {
        RwError error;
        error.pluginID  = 1;
        error.errorCode = reinterpret_cast<RwInt32(__cdecl*)(RwInt32, ...)>(0x8088D0)(static_cast<RwInt32>(0x8000001E), source, static_cast<RwInt32>(size), static_cast<RwInt32>(size - 1), static_cast<RwInt32>(static_cast<signed char>(source[size - 1])));
        RwErrorSet(&error);
        destination[size - 1] = 0;
    }
}

// 0x7F4690 (sin reconstruir): lee la imagen (nombre, mascara, banderas de raster) y devuelve la imagen; escribe ancho, alto, profundidad y formato (cType | cFormat << 8) del raster que le corresponde.
// Los dos primeros parametros de salida se leen como entrada (0 la primera vez; las dimensiones del nivel de mipmap despues).
using TextureImageReadFn = RwImage*(__cdecl*)(const char*, const char*, uint32, RwInt32*, RwInt32*, RwInt32*, RwInt32*);
// 0x7F44C0 (sin reconstruir): genera la paleta (1024 bytes) comun de las imagenes de los niveles; los dos ultimos parametros son 4 (formato de 8 bits) u 8 (de 4 bits).
using TextureImagePaletteFn = void(__cdecl*)(void*, int, RwImage**, int, int);
constexpr uint32 kTextureReadDefaultMaskSource = 0xC97904; // cadena de 32 bytes que el EXE copia como mascara cuando no hay nombre de mascara
} // namespace

// 0x7F4AE0 (lote R7A, sin validar; nombre inferido): (name, maskName). Copia el nombre (y la mascara, si existe y no esta vacia) a dos buferes locales de 256 bytes con CopyStringChecked. Formato = 4;
// con mipmapping 0x8004 y con generacion automatica 0x9004. Si hay devolucion de nombres de mipmap (textureGlobals+0x30) la llama (nombre, mascara, 0, formato). Lee la imagen del nivel 0 con 0x7F4690
// (NULL si falla) y crea el raster con RwRasterCreate (ancho, alto, profundidad, banderas; si falla destruye la imagen y devuelve NULL). Sin el bit 0x8000 de las banderas: RwImageGammaCorrect y
// RwRasterSetFromImage; con 0x1000 (automatico): solo RwRasterSetFromImage; en ambos casos un fallo destruye el raster y la imagen y devuelve NULL, y si no se destruye la imagen. Con mipmaps de archivo:
// por nivel 1..RwRasterGetNumLevels-1 repite las copias de nombre y mascara y la devolucion, hace RwRasterLock(raster, nivel, 5) solo para leer ancho/alto/profundidad y cType | cFormat << 8 del raster
// (el bloqueo cambia sus dimensiones) y RwRasterUnlock, y lee la imagen de ese nivel con 0x7F4690 (un fallo destruye las imagenes anteriores (de la ultima a la 0) y el raster y devuelve NULL).
// Con formato paletizado (cFormat & 0x60) llama a 0x7F44C0(paleta, 0, imagenes, niveles, 4 si cFormat & 0x40 u 8) y corrige el gamma solo de la imagen 0; si no, corrige el gamma de todas. Despues, por
// nivel: RwRasterLock(5) y, si devuelve algo, RwRasterSetFromImage (un fallo destruye las imagenes restantes, empezando por la actual, y el raster, y devuelve NULL) y RwRasterUnlock; destruye la imagen.
// Finalmente reserva la textura (lista libre textureGlobals+8, pista 0x30006; NULL destruye el raster), la deja con raster, dict = NULL, filtro 1 / direccionamiento 0x11, refCount 1, inicializa sus plugins
// (_rwPluginRegistryInitObject sobre el registro 0x8E23CC), copia el nombre (32 bytes) y la mascara (o la cadena de 0xC97904 si maskName es NULL) con CopyStringChecked.
RwTexture* _rwTextureReadMipmapped(const RwChar* name, const RwChar* maskName) {
    char nameBuffer[256];
    char maskBuffer[256];
    RwImage* images[144];

    CopyStringChecked(nameBuffer, name, 0x100);
    maskBuffer[0] = 0;
    if (maskName != nullptr && maskName[0] != 0) {
        CopyStringChecked(maskBuffer, maskName, 0x100);
    }
    TextureGlobalsLayout& globals = TextureGlobals();
    uint32 format = 4;
    if (globals.mipmapping != 0) {
        format = 0x8004;
        if (globals.autoMipmapping != 0) {
            format = 0x9004;
        }
    }
    if (globals.mipmapNameCallBack != nullptr) {
        globals.mipmapNameCallBack(nameBuffer, maskBuffer, 0, static_cast<RwInt32>(format));
    }

    RwInt32 width = 0;
    RwInt32 height = 0;
    RwInt32 depth = 0;
    RwInt32 rasterFlags = 0;
    images[0] = reinterpret_cast<TextureImageReadFn>(0x7F4690)(nameBuffer, maskBuffer, format, &width, &height, &depth, &rasterFlags);
    if (images[0] == nullptr) {
        return nullptr;
    }
    RwRaster* const raster = RwRasterCreate(width, height, depth, rasterFlags);
    if (raster == nullptr) {
        RwImageDestroy(images[0]);
        return nullptr;
    }

    RwInt32 numLevels = 0;
    bool singleImage = true;
    RwRaster* setResult = nullptr;
    if ((rasterFlags & 0x8000) == 0) {
        RwImageGammaCorrect(images[0]);
        setResult = RwRasterSetFromImage(raster, images[0]);
    } else if ((rasterFlags & 0x1000) != 0) {
        setResult = RwRasterSetFromImage(raster, images[0]);
    } else {
        singleImage = false;
        numLevels = RwRasterGetNumLevels(raster);
        if (numLevels > 1) {
            RwInt32 level = 1;
            do {
                CopyStringChecked(nameBuffer, name, 0x100);
                maskBuffer[0] = 0;
                if (maskName != nullptr && maskName[0] != 0) {
                    CopyStringChecked(maskBuffer, maskName, 0x100);
                }
                if (globals.mipmapNameCallBack != nullptr) {
                    globals.mipmapNameCallBack(nameBuffer, maskBuffer, static_cast<RwUInt8>(level), static_cast<RwInt32>(format));
                }
                RwRasterLock(raster, static_cast<RwUInt8>(level), 5);
                width       = raster->width;
                height      = raster->height;
                depth       = raster->depth;
                rasterFlags = static_cast<RwInt32>((static_cast<uint32>(raster->cFormat) << 8) | raster->cType);
                RwRasterUnlock(raster);
                images[level] = reinterpret_cast<TextureImageReadFn>(0x7F4690)(nameBuffer, maskBuffer, format, &width, &height, &depth, &rasterFlags);
                if (images[level] == nullptr) {
                    for (RwInt32 i = level - 1; i >= 0; --i) {
                        RwImageDestroy(images[i]);
                    }
                    RwRasterDestroy(raster);
                    return nullptr;
                }
                ++level;
            } while (level < numLevels);
        }
        if ((raster->cFormat & 0x60) != 0) {
            RwUInt8 palette[0x400];
            reinterpret_cast<TextureImagePaletteFn>(0x7F44C0)(palette, 0, images, numLevels, (raster->cFormat & 0x40) != 0 ? 4 : 8);
            RwImageGammaCorrect(images[0]);
        } else {
            for (RwInt32 i = 0; i < numLevels; ++i) {
                RwImageGammaCorrect(images[i]);
            }
        }
        for (RwInt32 i = 0; i < numLevels; ++i) {
            if (RwRasterLock(raster, static_cast<RwUInt8>(i), 5) != nullptr) {
                if (RwRasterSetFromImage(raster, images[i]) == nullptr) {
                    for (; i < numLevels; ++i) {
                        RwImageDestroy(images[i]);
                    }
                    RwRasterDestroy(raster);
                    return nullptr;
                }
                RwRasterUnlock(raster);
            }
            RwImageDestroy(images[i]);
        }
    }
    if (singleImage) {
        if (setResult == nullptr) {
            RwRasterDestroy(raster);
            RwImageDestroy(images[0]);
            return nullptr;
        }
        RwImageDestroy(images[0]);
    }

    RwTexture* const texture = static_cast<RwTexture*>(RwFreeListAlloc(globals.textureFreeList, 0x30006));
    if (texture == nullptr) {
        RwRasterDestroy(raster);
        return nullptr;
    }
    texture->filterAddressing = 0;
    reinterpret_cast<RwUInt8*>(texture)[0x51] = 0x11;
    texture->dict = nullptr;
    texture->name[0] = 0;
    texture->mask[0] = 0;
    texture->raster = raster;
    texture->refCount = 1;
    reinterpret_cast<RwUInt8*>(texture)[0x50] = 1;
    _rwPluginRegistryInitObject(&TextureTKList, texture);
    CopyStringChecked(texture->name, name, 0x20);
    if (maskName != nullptr) {
        CopyStringChecked(texture->mask, maskName, 0x20);
    } else {
        CopyStringChecked(texture->mask, reinterpret_cast<const char*>(kTextureReadDefaultMaskSource), 0x20);
    }
    return texture;
}

// 0x7F0360 (lote R11A, sin validar; 167 bytes; el listado no le daba clase fiable): inicializacion de los campos de un RwFrame (la llaman RwFrameCreate 0x7F0410 y _rwFrameInit 0x7F0450). object.type/subType/
// flags/privateFlags = 0 y parent = NULL; las dos matrices (modelling +0x10 y ltm +0x50) a la identidad con las banderas 0x20003 (ORTHONORMAL | INTERNALIDENTITY; los paddings no se tocan); objectList (+0x90)
// vacia (next = prev = su propia direccion), child = next = NULL y root = el propio marco; por ultimo _rwPluginRegistryInitObject(registro de plugins de RwFrame 0x8E2270, frame). inDirtyListLink (+8) no se toca.
// Los literales de 1.0f son la constante 0x3F800000 (sin aritmetica x87).
static_assert(offsetof(RwFrame, modelling) == 0x10 && offsetof(RwFrame, ltm) == 0x50 && offsetof(RwFrame, objectList) == 0x90 && offsetof(RwFrame, child) == 0x98 && offsetof(RwFrame, root) == 0xA0);
void rwFrameInitFields(RwFrame* frame) {
    static auto& FrameTKList = StaticRef<RwPluginRegistry>(0x8E2270);
    frame->object.type = 0;
    frame->objectList.link.next = &frame->objectList.link;
    frame->object.subType = 0;
    frame->objectList.link.prev = &frame->objectList.link;
    frame->object.flags = 0;
    frame->object.privateFlags = 0;
    frame->object.parent = nullptr;
    frame->modelling.at.z = 1.0f;
    frame->modelling.up.y = 1.0f;
    frame->modelling.right.x = 1.0f;
    frame->modelling.up.x = 0.0f;
    frame->modelling.right.z = 0.0f;
    frame->modelling.right.y = 0.0f;
    frame->modelling.at.y = 0.0f;
    frame->modelling.at.x = 0.0f;
    frame->modelling.up.z = 0.0f;
    frame->modelling.pos.z = 0.0f;
    frame->modelling.pos.y = 0.0f;
    frame->modelling.pos.x = 0.0f;
    frame->modelling.flags = 0x20003;
    frame->ltm.at.z = 1.0f;
    frame->ltm.up.y = 1.0f;
    frame->ltm.right.x = 1.0f;
    frame->ltm.up.x = 0.0f;
    frame->ltm.right.z = 0.0f;
    frame->ltm.right.y = 0.0f;
    frame->ltm.at.y = 0.0f;
    frame->ltm.at.x = 0.0f;
    frame->ltm.up.z = 0.0f;
    frame->ltm.pos.z = 0.0f;
    frame->ltm.pos.y = 0.0f;
    frame->ltm.pos.x = 0.0f;
    frame->ltm.flags = 0x20003;
    frame->child = nullptr;
    frame->next = nullptr;
    frame->root = frame;
    _rwPluginRegistryInitObject(&FrameTKList, frame);
}

// Lote R11A (sin validar): declaraciones de vertices D3D9, material D3D9, RxPipelineCreate y campos iniciales de RwFrame.
void RwCoreInjectHooksR11A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwD3D9CreateVertexDeclaration, 0x7FAA30);
    RH_ScopedGlobalInstall(RwD3D9SetMaterial, 0x7FC430);
    RH_ScopedGlobalInstall(RxPipelineCreate, 0x8057B0);
    RH_ScopedGlobalInstall(rwFrameInitFields, 0x7F0360);
}

// Gestor de buferes de vertices D3D9 y lectura de texturas con mipmaps (lote R7A, sin validar).
void RwCoreInjectHooksR7A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(_rwTextureReadMipmapped, 0x7F4AE0);
    RH_ScopedGlobalInstall(RwD3D9CreateVertexBuffer, 0x7F5500);
    RH_ScopedGlobalInstall(RwD3D9DestroyVertexBuffer, 0x7F56A0);
    RH_ScopedGlobalInstall(_rwD3D9DynamicVertexBufferManagerForceDiscard, 0x7F5800);
    RH_ScopedGlobalInstall(_rwD3D9DynamicVertexBufferRelease, 0x7F5840);
    RH_ScopedGlobalInstall(_rwD3D9DynamicVertexBufferRestore, 0x7F58D0);
    RH_ScopedGlobalInstall(_rwD3D9DynamicVertexBufferManagerOpen, 0x7F5940);
    RH_ScopedGlobalInstall(RwD3D9DynamicVertexBufferCreate, 0x7F5A00);
    RH_ScopedGlobalInstall(RwD3D9DynamicVertexBufferLock, 0x7F5B10); // lote R11A (sin validar)
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R8A (sin validar): ordenacion de registros por una clave de 32 bits (0x8103D0). La llama 0x80FC30 (RwCore, creacion de la lista de grupos de la tuberia de Rx) con
// (registros, cantidad, 0xC, 0, 0, 0xFFFFFFFF); el listado la daba por RpMatFX.
// ---------------------------------------------------------------------------------------------------------------------------------------------
void _rwSortRecordsByKey(void* base, RwUInt32 count, RwUInt32 stride, RwUInt32 keyOffset, RwUInt32 keyMin, RwUInt32 keyMax);

void RwCoreInjectHooksR8A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(_rwSortRecordsByKey, 0x8103D0);
}

// 0x8103D0 (lote R8A, sin validar; cdecl, 6 argumentos (registros, cantidad, tamano del registro, desplazamiento de la clave, clave minima, clave maxima), `ret`, 598 bytes; llamador: 0x80FF92): ordena de
// menor a mayor (sin signo) los registros por la clave de 32 bits en registro + desplazamiento. Sin efecto si los registros son NULL, si desplazamiento + 4 > tamano o si minima >= maxima. Con mas de 5 registros
// hace primero una particion por bits ("radix exchange"): la mascara empieza en el bit mas alto de la clave maxima y se divide por 2 en cada nivel; cada particion intercambia (en dwords; los bytes sobrantes del
// registro no se intercambian) el primer registro con el bit a 1 por el ultimo con el bit a 0, y solo se subdividen las mitades de 6 o mas registros (una pila de trozos en el marco). Despues (para
// cualquier cantidad > 1) pone el menor de los primeros min(5, cantidad) en la cabeza (a igualdad de clave, el de menor indice: busca de mayor a menor con `<` estricto) y termina con una insercion directa
// que no comprueba el limite inferior (la cabeza es el minimo).
void _rwSortRecordsByKey(void* basePointer, RwUInt32 count, RwUInt32 stride, RwUInt32 keyOffset, RwUInt32 keyMin, RwUInt32 keyMax) {
    const RwUInt32 base = reinterpret_cast<RwUInt32>(basePointer);
    if (base == 0 || keyOffset + 4 > stride || keyMin >= keyMax) {
        return;
    }
    const auto keyAt = [keyOffset](RwUInt32 record) { return *reinterpret_cast<const RwUInt32*>(record + keyOffset); };
    // Intercambio de los registros por dwords (stride >= 4: stride / 4 dwords).
    const auto swapRecords = [stride](RwUInt32 a, RwUInt32 b) {
        if (stride < 4) {
            return;
        }
        for (RwUInt32 n = stride >> 2; n != 0; --n, a += 4, b += 4) {
            const RwUInt32 va = *reinterpret_cast<const RwUInt32*>(a);
            const RwUInt32 vb = *reinterpret_cast<const RwUInt32*>(b);
            *reinterpret_cast<RwUInt32*>(a) = vb;
            *reinterpret_cast<RwUInt32*>(b) = va;
        }
    };

    if (count > 5) {
        RwInt32 bit = -1;
        if (keyMax != 0) {
            bit = 0;
            for (RwUInt32 t = keyMax >> 1; t != 0; t >>= 1) {
                ++bit;
            }
        }
        struct Piece {
            RwUInt32 first;
            RwUInt32 last;
            RwUInt32 mask;
        };
        Piece pieces[40];
        RwInt32 depth = 0;
        pieces[depth++] = { base, base + (count - 1) * stride, 1u << bit };
        do {
            --depth;
            const RwUInt32 first = pieces[depth].first;
            RwUInt32 last = pieces[depth].last;
            RwUInt32 mask = pieces[depth].mask;
            for (;;) {
                const RwUInt32 pieceLast = last;
                RwUInt32 lo = first;
                RwUInt32 hi = last;
                for (;;) {
                    if ((keyAt(lo) & mask) == 0) {
                        lo += stride;
                        if (lo > hi) {
                            break;
                        }
                        while ((keyAt(lo) & mask) == 0) {
                            lo += stride;
                            if (lo > hi) {
                                goto partitioned;
                            }
                        }
                    }
                    if ((keyAt(hi) & mask) != 0) {
                        hi -= stride;
                        if (lo > hi) {
                            break;
                        }
                        while ((keyAt(hi) & mask) != 0) {
                            hi -= stride;
                            if (lo > hi) {
                                goto partitioned;
                            }
                        }
                    }
                    swapRecords(lo, hi);
                    lo += stride;
                    hi -= stride;
                    if (lo > hi) {
                        break;
                    }
                }
            partitioned:
                mask >>= 1;
                if (mask == 0) {
                    break;
                }
                hi += stride;
                if (pieceLast >= hi + 5 * stride) {
                    pieces[depth++] = { hi, pieceLast, mask };
                }
                last = lo - stride;
                if (last < first + 5 * stride) {
                    break;
                }
            }
        } while (depth != 0);
    }

    if (count <= 1) {
        return;
    }
    const RwUInt32 lastIndex = count - 1;
    const RwUInt32 span = lastIndex < 4 ? lastIndex : 4;
    RwUInt32 bestIndex = span;
    RwUInt32 bestKey = keyAt(base + span * stride);
    for (RwUInt32 i = span - 1;; --i) {
        const RwUInt32 key = keyAt(base + i * stride);
        if (key < bestKey) {
            bestKey = key;
            bestIndex = i;
        }
        if (i == 0) {
            break;
        }
    }
    if (bestIndex != 0) {
        swapRecords(base, base + bestIndex * stride);
    }
    RwUInt32 previous = base;
    for (RwUInt32 remaining = lastIndex; remaining != 0; --remaining, previous += stride) {
        const RwUInt32 current = previous + stride;
        const RwUInt32 currentKey = keyAt(current);
        if (keyAt(previous) > currentKey) {
            RwUInt32 slot = previous;
            do {
                swapRecords(slot, slot + stride);
                slot -= stride;
            } while (keyAt(slot) > currentKey);
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R14A (sin validar): 0x801390, imagen -> raster del controlador D3D9 (RwD3D9). El listado la daba por RwPlCore; la llama 0x8006F8 (rama de imagen a raster del controlador) con (raster, imagen).
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
// Funciones de conversion RGBA -> pixel del EXE (cdecl, UN argumento de pila: puntero a 4 bytes R, G, B, A; devuelven el pixel en EAX), sin reconstruir; se llaman por direccion fija.
using ImageToPixelFn = uint32(__cdecl*)(const void*);
// Tabla de saltos del EXE (0x801860: indices por D3DFORMAT - 0x15; 0x801820: destinos; la segunda copia 0x801904 / 0x8018C4 es identica): D3DFORMAT -> funcion; sin entrada 0 (el EXE haria call 0).
ImageToPixelFn ImageToPixelConverter(uint32 d3dFormat) {
    uintptr_t address = 0;
    switch (d3dFormat) {
    case 0x15: address = 0x800B00; break;
    case 0x16: address = 0x800AA0; break;
    case 0x17: address = 0x800960; break;
    case 0x18: address = 0x800930; break;
    case 0x19: address = 0x800990; break;
    case 0x1A: address = 0x8009D0; break;
    case 0x1D: address = 0x800A10; break;
    case 0x1E: address = 0x800A40; break;
    case 0x20: address = 0x800B30; break;
    case 0x21: address = 0x800AD0; break;
    case 0x32: address = 0x800B70; break;
    case 0x33: address = 0x800BD0; break;
    case 0x3C: address = 0x800B60; break;
    case 0x3D: address = 0x800A70; break;
    case 0x3E: address = 0x800AD0; break;
    case 0x3F: address = 0x800B30; break;
    case 0x51: address = 0x800C20; break;
    case 0x75: address = 0x800B60; break;
    default: break;
    }
    return reinterpret_cast<ImageToPixelFn>(address);
}
// Byte de alfa de la tabla _rwD3D9PixelFormatInfo (0xB4E7E0, 4 bytes por D3DFORMAT): 0xFF = sin alfa.
uint8 PixelFormatAlphaByte(uint32 d3dFormat) {
    return *reinterpret_cast<const uint8*>(0xB4E7E0 + static_cast<uintptr_t>(d3dFormat) * 4);
}
} // namespace

// 0x801390 (lote R14A, sin validar; 1167 bytes; nombre inferido, simetrica de _rwD3D9RasterToImage32; cdecl (RwRaster* raster, RwImage* imagen), sin valor de retorno): copia los pixeles de la imagen al raster
// ya bloqueado (raster->cpPixels y stride; imagen: cpPixels, stride y paleta de RGBA en bytes R, G, B, A). Extension D3D9 del raster = raster + [0xB4E9E0]; su byte +8 es "tiene alfa".
// Profundidad de imagen 32: cFormat & 0xF = 0x6 (xrgb): pixel = 0xFF000000 | R << 16 | G << 8 | B y alfa = 0; 0x5 (argb): igual con el alfa de la imagen (byte 3) y termina como "con alfa"; otro formato:
// funcion de conversion por el D3DFORMAT de la extension (tabla de saltos; 18 formatos con funcion, el resto hace call 0), un pixel por llamada con el puntero a los 4 bytes de la imagen, escrito como DWORD.
// Profundidad 4 u 8 (1 << profundidad entradas): paleta local de dwords convertida con el mismo criterio (6: alfa FF; 5: con alfa; otro: por la funcion de conversion), luego cada pixel indexa esa paleta (indices de UN
// byte por pixel tambien con 4 bits). Otra profundidad: solo el cierre "con alfa". Cierre: byte de alfa de la tabla de formatos (0xB4E7E0 + d3dFormat * 4; 0xFF en las ramas xrgb): >= 0xFF -> alfa = 0 y sale;
// si no alfa = 1 y, si el raster tiene padre distinto de el, tambien el alfa de la extension del padre = 1. Los bucles son con signo (alto y ancho <= 0 no copian nada).
void _rwD3D9ImageToRaster(RwRaster* raster, RwImage* image) {
    D3D9RasterExtLayout& ext = D3D9RasterExt(raster);
    const auto markAlpha = [&]() {
        ext.alpha = 1;
        RwRaster* const parent = raster->parent;
        if (parent != raster) {
            D3D9RasterExt(parent).alpha = 1;
        }
    };
    // Recorre las filas de la imagen y del raster; convertPixel(origen, destino) por pixel; avanza 4 bytes en los dos.
    const auto convertRows = [&](auto&& convertPixel) {
        for (RwInt32 y = 0; y < raster->height; y++) {
            const RwUInt8* source = image->cpPixels + static_cast<RwInt32>(image->stride * y);
            RwUInt8* destination = raster->cpPixels + static_cast<RwInt32>(raster->stride * y);
            if (raster->width > 0) {
                RwInt32 x = 0;
                do {
                    convertPixel(source, destination);
                    source += 4;
                    destination += 4;
                    x++;
                } while (x < raster->width);
            }
        }
    };
    const auto packXrgb = [](const RwUInt8* rgba) -> uint32 {
        return 0xFF000000u | (static_cast<uint32>(rgba[0]) << 16) | (static_cast<uint32>(rgba[1]) << 8) | rgba[2];
    };
    const auto packArgb = [](const RwUInt8* rgba) -> uint32 {
        return (static_cast<uint32>(rgba[3]) << 24) | (static_cast<uint32>(rgba[0]) << 16) | (static_cast<uint32>(rgba[1]) << 8) | rgba[2];
    };
    const uint32 rasterFormat = static_cast<uint32>(raster->cFormat & 0x0Fu) << 8;
    const RwInt32 depth = image->depth;

    if (depth == 4 || depth == 8) {
        const uint32 count = 1u << depth;
        uint32 palette[256];
        uint8 alphaInfo;
        const RwUInt8* const imagePalette = reinterpret_cast<const RwUInt8*>(image->palette);
        if (rasterFormat == 0x600) {
            alphaInfo = 0xFF;
            for (uint32 i = 0; i < count; i++) {
                palette[i] = packXrgb(imagePalette + i * 4);
            }
        } else if (rasterFormat == 0x500) {
            alphaInfo = 0;
            for (uint32 i = 0; i < count; i++) {
                palette[i] = packArgb(imagePalette + i * 4);
            }
        } else {
            const ImageToPixelFn convert = ImageToPixelConverter(ext.d3dFormat);
            alphaInfo = PixelFormatAlphaByte(ext.d3dFormat);
            if (convert == nullptr) {
                return; // el EXE hace call 0
            }
            for (uint32 i = 0; i < count; i++) {
                palette[i] = convert(imagePalette + i * 4);
            }
        }
        for (RwInt32 y = 0; y < raster->height; y++) {
            const RwUInt8* source = image->cpPixels + static_cast<RwInt32>(image->stride * y);
            RwUInt8* destination = raster->cpPixels + static_cast<RwInt32>(raster->stride * y);
            if (raster->width > 0) {
                RwInt32 x = 0;
                do {
                    *reinterpret_cast<uint32*>(destination) = palette[*source];
                    source++;
                    destination += 4;
                    x++;
                } while (x < raster->width);
            }
        }
        if (alphaInfo >= 0xFF) {
            ext.alpha = 0;
            return;
        }
        markAlpha();
        return;
    }
    if (depth != 32) {
        markAlpha();
        return;
    }
    if (rasterFormat == 0x600) {
        convertRows([&](const RwUInt8* source, RwUInt8* destination) {
            *reinterpret_cast<uint32*>(destination) = packXrgb(source);
        });
        ext.alpha = 0;
        return;
    }
    if (rasterFormat == 0x500) {
        convertRows([&](const RwUInt8* source, RwUInt8* destination) {
            *reinterpret_cast<uint32*>(destination) = packArgb(source);
        });
        markAlpha();
        return;
    }
    const ImageToPixelFn convert = ImageToPixelConverter(ext.d3dFormat);
    const uint8 alphaInfo = PixelFormatAlphaByte(ext.d3dFormat);
    if (convert == nullptr) {
        return; // el EXE hace call 0
    }
    convertRows([&](const RwUInt8* source, RwUInt8* destination) {
        *reinterpret_cast<uint32*>(destination) = convert(source);
    });
    if (alphaInfo >= 0xFF) {
        ext.alpha = 0;
        return;
    }
    markAlpha();
}

void RwD3D9InjectHooksR14A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(_rwD3D9ImageToRaster, 0x801390);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R15A (sin validar): tuberias de Rx. 0x80A1A0 crea las tuberias por defecto de Im3D; 0x80F970, 0x80FBA0 y 0x80FC30 son las tres fases finales del desbloqueo de una tuberia
// (RxLockedPipeUnlock, 0x805F40: las llaman 0x80F38A, 0x80F39C y 0x80F3AE, en este orden, mientras la anterior devuelva 0). Los datos temporales de cada nodo (RxPipelineNode::topSortData, +0x1C)
// cuelgan de una matriz de "requisitos" {+0 cuenta, +0xC elementos de 0x24 bytes}; los grupos de elementos (0x10 bytes) salen del extremo alto del almacen temporal de Rx (0x805890) y los
// datos definitivos del extremo bajo (0x8058F0). Los dos almacenes son el EXE (por direccion fija).
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
struct RxUnlockGroup;
struct RxUnlockInfo {
    RwUInt32 field0;
    RwUInt32 field4;
    RwUInt32 slotMask; // +8: mascara de ranuras de cluster ya usadas
};
struct RxUnlockItem { // 0x24 bytes: un cluster pedido por un nodo (en la tuberia final: RxPipelineRequiresCluster = {clusterDef, tipo, ranura})
    RxClusterDefinition* clusterDef;  // +0x00
    RwUInt32             type;        // +0x04 (1 = obligatorio, 2 = opcional; 0 = no lo quiere)
    RwUInt32             field8;      // +0x08
    RxUnlockGroup*       group;       // +0x0C
    RxUnlockItem*        nextInGroup; // +0x10
    RxUnlockInfo*        info;        // +0x14
    RwUInt32             outputsMask; // +0x18: salidas a las que el cluster persiste
    RwUInt32             slot;        // +0x1C: ranura de cluster asignada
    RwUInt32             field20;     // +0x20
};
struct RxUnlockGroup { // 0x10 bytes
    RxUnlockItem*  firstItem;  // +0x00
    RxUnlockGroup* chained;    // +0x04: grupos fundidos con este (cadena)
    RxUnlockGroup* nextInList; // +0x08: lista global de grupos
    RxUnlockGroup* parent;     // +0x0C: grupo en el que se fundio este (union-find)
};
struct RxUnlockItemArray { // matriz de requisitos de un nodo
    RwUInt32      count; // +0x00
    RwUInt32      field4;
    RwUInt32      field8;
    RxUnlockItem* items; // +0x0C
};
struct RxUnlockClusterRecord { // 8 bytes: un cluster distinto de la tuberia y sus atributos por defecto
    RxClusterDefinition* clusterDef;
    RwUInt32             attributes;
};
static_assert(sizeof(RxUnlockItem) == 0x24 && offsetof(RxUnlockItem, group) == 0xC && offsetof(RxUnlockItem, info) == 0x14 && offsetof(RxUnlockItem, slot) == 0x1C);
static_assert(sizeof(RxUnlockGroup) == 0x10 && offsetof(RxUnlockItemArray, items) == 0xC && offsetof(RxUnlockInfo, slotMask) == 8);
static_assert(sizeof(RxPipelineNode) == 0x28 && offsetof(RxPipelineNode, topSortData) == 0x1C && offsetof(RxPipelineNode, inputToClusterSlot) == 0x18);
static_assert(offsetof(RxPipelineNodeTopSortData, req) == 8 && offsetof(RxPipeline, inputRequirements) == 0x1C && offsetof(RxPipeline, pluginId) == 0x2C);
static_assert(offsetof(RxNodeDefinition, io) == 0x20 && offsetof(RxNodeDefinition, pipelineNodePrivateDataSize) == 0x34 && offsetof(RxClusterDefinition, defaultAttributes) == 8);

void* RxScratchAllocHigh(RwUInt32 size) { return reinterpret_cast<void*(__cdecl*)(RwUInt32)>(0x805890)(size); }
void* RxScratchAllocLow(RwUInt32 size) { return reinterpret_cast<void*(__cdecl*)(RwUInt32)>(0x8058F0)(size); }
RxUnlockItemArray* UnlockRequirements(const RxPipelineNode* node) { return reinterpret_cast<RxUnlockItemArray*>(node->topSortData->req); }

// Hay un grupo anterior a `group` (desde `head`) con el mismo cluster en su primer elemento.
bool UnlockGroupIsDuplicate(const RxUnlockGroup* head, const RxUnlockGroup* group) {
    for (const RxUnlockGroup* other = head; other != group; other = other->nextInList) {
        if (other->firstItem->clusterDef == group->firstItem->clusterDef) {
            return true;
        }
    }
    return false;
}
} // namespace

RwInt32 _rxPipelineUnlockLinkItems(RxPipeline* pipeline, RxUnlockGroup** groupList);
RwInt32 _rxPipelineUnlockAssignSlots(RxPipeline* pipeline, RxUnlockGroup** groupList);
RwInt32 _rxPipelineUnlockBuildExecutionData(RxPipeline* pipeline, RxUnlockGroup* groups);
RwInt32 _rwIm3DDefaultPipelinesCreate(RxPipeline** pipelines);

// 0x80F970 (lote R15A, sin validar; 560 bytes; cdecl, (tuberia, puntero a la lista de grupos); devuelve 0 o 0x20; nombre inferido): fase 1 del desbloqueo. Por cada nodo (do-while: el contador de
// nodos se decrementa al final) da un grupo (0x805890(0x10), enlazado al frente de la lista global; sin memoria: error {1, _rwerror(0x20)} y devuelve 0x20) a cada requisito sin grupo y lo pone al
// frente del grupo. Despues, por cada salida (!= -1) y por cada requisito del nodo destino con tipo != 0, busca en el nodo actual el requisito con el mismo cluster; si existe y la salida esta en su
// mascara (1 << salida, con la cuenta modulo 32): si el del destino no tiene grupo entra en el grupo del encontrado; si tiene, funde las raices (union-find): la del encontrado cuelga de la del destino,
// se enlaza al final de la cadena de la primera y se quita de la lista global.
RwInt32 _rxPipelineUnlockLinkItems(RxPipeline* pipeline, RxUnlockGroup** groupList) {
    RwUInt32 nodesLeft = pipeline->numNodes;
    RxPipelineNode* node = pipeline->nodes;
    do {
        if (UnlockRequirements(node)->count > 0) {
            RwUInt32 i = 0;
            do {
                RxUnlockItem* const item = &UnlockRequirements(node)->items[i];
                if (item->group == nullptr) {
                    RxUnlockGroup* const group = static_cast<RxUnlockGroup*>(RxScratchAllocHigh(0x10));
                    if (group != nullptr) {
                        group->chained = nullptr;
                        group->firstItem = nullptr;
                        group->parent = nullptr;
                        group->nextInList = *groupList;
                        *groupList = group;
                    }
                    item->group = group;
                    if (group == nullptr) {
                        RwError error;
                        error.pluginID = 1;
                        error.errorCode = _rwerror(0x20);
                        RwErrorSet(&error);
                        return 0x20;
                    }
                    item->nextInGroup = group->firstItem;
                    group->firstItem = item;
                }
                ++i;
            } while (i < UnlockRequirements(node)->count);
        }
        if (node->numOutputs > 0) {
            RwUInt32 output = 0;
            do {
                const RwUInt32 targetIndex = node->outputs[output];
                if (targetIndex != 0xFFFFFFFFu) {
                    RxPipelineNode* const target = &pipeline->nodes[targetIndex];
                    if (UnlockRequirements(target)->count > 0) {
                        RwUInt32 j = 0;
                        do {
                            RxUnlockItem* const targetItem = &UnlockRequirements(target)->items[j];
                            if (targetItem->type != 0) {
                                const RwUInt32 ownCount = UnlockRequirements(node)->count;
                                RxUnlockItem* match = nullptr;
                                if (ownCount != 0) {
                                    RxUnlockItem* const ownItems = UnlockRequirements(node)->items;
                                    for (RwUInt32 k = 0; k < ownCount; ++k) {
                                        if (ownItems[k].clusterDef == targetItem->clusterDef) {
                                            match = &ownItems[k];
                                            break;
                                        }
                                    }
                                }
                                if (match != nullptr && ((1u << (output & 31u)) & match->outputsMask) != 0) {
                                    if (targetItem->group == nullptr) {
                                        RxUnlockGroup* const matchGroup = match->group;
                                        targetItem->nextInGroup = matchGroup->firstItem;
                                        matchGroup->firstItem = targetItem;
                                        targetItem->group = match->group;
                                    } else {
                                        RxUnlockGroup* rootTarget = targetItem->group;
                                        while (rootTarget->parent != nullptr) {
                                            rootTarget = rootTarget->parent;
                                        }
                                        RxUnlockGroup* rootMatch = match->group;
                                        while (rootMatch->parent != nullptr) {
                                            rootMatch = rootMatch->parent;
                                        }
                                        if (rootTarget != rootMatch) {
                                            RxUnlockGroup* tail = rootTarget;
                                            while (tail->chained != nullptr) {
                                                tail = tail->chained;
                                            }
                                            tail->chained = rootMatch;
                                            rootMatch->parent = rootTarget;
                                            RxUnlockGroup** link = groupList;
                                            while (*link != rootMatch) {
                                                link = &(*link)->nextInList;
                                            }
                                            *link = rootMatch->nextInList;
                                        }
                                    }
                                }
                            }
                            ++j;
                        } while (j < UnlockRequirements(target)->count);
                    }
                }
                ++output;
            } while (output < node->numOutputs);
        }
        ++node;
        --nodesLeft;
    } while (nodesLeft != 0);
    return 0;
}

// 0x80FBA0 (lote R15A, sin validar; 137 bytes; cdecl, (tuberia, puntero a la lista de grupos); devuelve 0; nombre inferido): fase 2, asigna ranuras de cluster. Sin grupos: packetNumClusterSlots = 0.
// Por cada grupo de la lista: O de las mascaras de ranuras (info +8) de los elementos de todos los grupos de su cadena (+4); ranura = numero de bits bajos consecutivos a 1 de ese O (solo se mira el
// bit 0 en cada paso); packetNumClusterSlots = max(ranura + 1); cada elemento de la cadena recibe la ranura (+0x1C) y marca su mascara con 1 << ranura (modulo 32).
RwInt32 _rxPipelineUnlockAssignSlots(RxPipeline* pipeline, RxUnlockGroup** groupList) {
    RxUnlockGroup* head = *groupList;
    if (head == nullptr) {
        pipeline->packetNumClusterSlots = 0;
        return 0;
    }
    RwUInt32 numSlots = 0;
    do {
        RwUInt32 used = 0;
        for (RxUnlockGroup* group = head; group != nullptr; group = group->chained) {
            for (RxUnlockItem* item = group->firstItem; item != nullptr; item = item->nextInGroup) {
                used |= item->info->slotMask;
            }
        }
        RwUInt32 slot = 0;
        while ((used & 1u) != 0) {
            used >>= 1;
            ++slot;
        }
        if (slot >= numSlots) {
            numSlots = slot + 1;
        }
        for (RxUnlockGroup* group = head; group != nullptr; group = group->chained) {
            const RwUInt32 bit = 1u << (slot & 31u);
            for (RxUnlockItem* item = group->firstItem; item != nullptr; item = item->nextInGroup) {
                RxUnlockInfo* const info = item->info;
                item->slot = slot;
                info->slotMask |= bit;
            }
        }
        head = head->nextInList;
    } while (head != nullptr);
    pipeline->packetNumClusterSlots = numSlots;
    return 0;
}

// 0x80FC30 (lote R15A, sin validar; 884 bytes; cdecl, (tuberia, lista de grupos POR VALOR); devuelve 0; nombre inferido): fase 3, datos definitivos. numInputs = requisitos del nodo 0 con tipo 1 o 2.
// Cuenta los clusters distintos (primer elemento de cada grupo; sin duplicados anteriores). Pone a cero todo el almacen temporal libre (de 0x8058F0(0) a 0x805890(0), en ese orden de llamadas) y reserva
// el paquete embebido (28 * ranuras + 0x14; numClusters = ranuras de 16 bits, pipeline en +4, estado 0, puntero en +0x14). Crea la matriz de registros {cluster, atributos por defecto (+8)} de 8 bytes
// (uno por cluster distinto, reservados seguidos). Por cada nodo: slotClusterRefs (ranuras * 4, solo si > 0), slotsContinue (ranuras * 4 + 4), inputToClusterSlot (clusters de interes * 4, si > 0) y
// privateData (tamano redondeado a 4, si != 0); rellena slotClusterRefs[ranura] con el registro del cluster y slotsContinue[ranura + 1] con la mascara de salidas, slotsContinue[0] = AND de todas;
// inputToClusterSlot[i] = primera ranura cuyo cluster coincide con clustersOfInterest[i] (0xFFFFFFFF si ninguna). Por ultimo inputRequirements (12 bytes por requisito 1 o 2 del nodo 0: cluster,
// tipo, ranura), numInputRequirements y ordenacion por cluster con 0x8103D0.
RwInt32 _rxPipelineUnlockBuildExecutionData(RxPipeline* pipeline, RxUnlockGroup* groups) {
    RwUInt32 numInputs = 0;
    {
        const RxUnlockItemArray* const req = UnlockRequirements(&pipeline->nodes[0]);
        const RwUInt32 count = req->count;
        for (RwUInt32 i = 0; i < count; ++i) {
            const RwUInt32 type = req->items[i].type;
            if (type == 1 || type == 2) {
                ++numInputs;
            }
        }
    }
    RwUInt32 numUnique = 0;
    for (const RxUnlockGroup* group = groups; group != nullptr; group = group->nextInList) {
        if (!UnlockGroupIsDuplicate(groups, group)) {
            ++numUnique;
        }
    }
    void* const scratchTop = RxScratchAllocHigh(0);
    void* const scratchBottom = RxScratchAllocLow(0);
    memset(scratchBottom, 0, reinterpret_cast<uintptr_t>(scratchTop) - reinterpret_cast<uintptr_t>(scratchBottom));
    RxPacket* const packet = static_cast<RxPacket*>(RxScratchAllocLow(pipeline->packetNumClusterSlots * 28u + 0x14u));
    pipeline->embeddedPacket = packet;
    packet->numClusters = static_cast<RwUInt16>(pipeline->packetNumClusterSlots);
    packet->pipeline = pipeline;
    pipeline->embeddedPacketState = rxPKST_PACKETLESS;
    RxUnlockClusterRecord* records = nullptr;
    for (RxUnlockGroup* group = groups; group != nullptr; group = group->nextInList) {
        if (UnlockGroupIsDuplicate(groups, group)) {
            continue;
        }
        RxClusterDefinition* const clusterDef = group->firstItem->clusterDef;
        RxUnlockClusterRecord* const record = static_cast<RxUnlockClusterRecord*>(RxScratchAllocLow(8));
        if (records == nullptr) {
            records = record;
        }
        record->clusterDef = clusterDef;
        record->attributes = clusterDef->defaultAttributes;
    }
    for (RwUInt32 nodeIndex = 0; nodeIndex < pipeline->numNodes; ++nodeIndex) {
        RxPipelineNode* const node = &pipeline->nodes[nodeIndex];
        if (pipeline->packetNumClusterSlots > 0) {
            node->slotClusterRefs = static_cast<RxPipelineCluster**>(RxScratchAllocLow(pipeline->packetNumClusterSlots * 4u));
        }
        node->slotsContinue = static_cast<RwUInt32*>(RxScratchAllocLow(pipeline->packetNumClusterSlots * 4u + 4u));
        if (node->nodeDef->io.numClustersOfInterest > 0) {
            node->inputToClusterSlot = static_cast<RwUInt32*>(RxScratchAllocLow(node->nodeDef->io.numClustersOfInterest * 4u));
        }
        if (node->nodeDef->pipelineNodePrivateDataSize != 0) {
            node->privateData = RxScratchAllocLow((node->nodeDef->pipelineNodePrivateDataSize + 3u) & ~3u);
        }
        RwUInt32 continueMask = 0xFFFFFFFFu;
        RxUnlockItemArray* req = UnlockRequirements(node);
        RwUInt32 count = req->count;
        if (count > 0) {
            RwUInt32 i = 0;
            do {
                const RxUnlockItem* const item = &req->items[i];
                RxUnlockClusterRecord* record = nullptr;
                for (RwUInt32 k = 0; k < numUnique; ++k) {
                    if (records[k].clusterDef == item->clusterDef) {
                        record = &records[k];
                        break;
                    }
                }
                node->slotClusterRefs[item->slot] = reinterpret_cast<RxPipelineCluster*>(record);
                node->slotsContinue[item->slot + 1] = item->outputsMask;
                continueMask &= item->outputsMask;
                ++i;
                req = UnlockRequirements(node);
                count = req->count;
            } while (i < count);
        }
        node->slotsContinue[0] = continueMask;
        if (node->nodeDef->io.numClustersOfInterest > 0) {
            RwUInt32 cluster = 0;
            do {
                node->inputToClusterSlot[cluster] = 0xFFFFFFFFu;
                const RwUInt32 numSlots = pipeline->packetNumClusterSlots;
                RxPipelineCluster** ref = node->slotClusterRefs;
                for (RwUInt32 slot = 0; slot < numSlots; ++slot, ++ref) {
                    if (*ref != nullptr && reinterpret_cast<const RxUnlockClusterRecord*>(*ref)->clusterDef == node->nodeDef->io.clustersOfInterest[cluster].clusterDef) {
                        node->inputToClusterSlot[cluster] = slot;
                        break;
                    }
                }
                ++cluster;
            } while (cluster < node->nodeDef->io.numClustersOfInterest);
        }
    }
    if (numInputs > 0) {
        pipeline->inputRequirements = static_cast<RxPipelineRequiresCluster*>(RxScratchAllocLow(numInputs * 12u));
    }
    RwUInt32 numRequirements = 0;
    {
        const RxUnlockItemArray* req = UnlockRequirements(&pipeline->nodes[0]);
        RwUInt32 count = req->count;
        for (RwUInt32 i = 0; i < count;) {
            const RxUnlockItem* const item = &req->items[i];
            if (item->type == 1 || item->type == 2) {
                RxPipelineRequiresCluster* const out = &pipeline->inputRequirements[numRequirements];
                out->clusterDef = item->clusterDef;
                out->rqdOrOpt = static_cast<RxClusterValidityReq>(item->type);
                out->slotIndex = item->slot;
                ++numRequirements;
            }
            ++i;
            req = UnlockRequirements(&pipeline->nodes[0]);
            count = req->count;
        }
    }
    pipeline->numInputRequirements = numRequirements;
    _rwSortRecordsByKey(pipeline->inputRequirements, numRequirements, 0xC, 0, 0, 0xFFFFFFFFu);
    return 0;
}

// 0x80A1A0 (lote R15A, sin validar; 152 bytes; cdecl, (tabla de 5 tuberias); nombre inferido: creacion de las tuberias por defecto de Im3D; llamador: 0x7EFE7A, el constructor del plugin de motor 0x40A): crea
// una tuberia (0x8057B0) con pluginId (+0x2C) = 1, la bloquea (0x806990), le anade el nodo que devuelve 0x80E9A0 (0x806BE0 con indice NULL) y la desbloquea (0x805F40). Con exito escribe la tuberia
// en las 5 ranuras de la tabla, la asigna a los tipos de primitiva 3, 5, 4, 1 y 2 con RwIm3DSetRenderPipeline (0x7EFB10) y TERMINA con salto (cola) a 0x80E020(tabla), cuyo resultado devuelve. Fallo
// (bloqueo o desbloqueo): destruye con 0x805820 la tuberia creada (en el fallo del bloqueo) o NULL (en el del desbloqueo; la tuberia se pierde, como en el EXE) y devuelve 0; sin tuberia devuelve 0.
RwInt32 _rwIm3DDefaultPipelinesCreate(RxPipeline** pipelines) {
    RxPipeline* toDestroy = RxPipelineCreate();
    if (toDestroy == nullptr) {
        return 0;
    }
    RxPipeline* const created = toDestroy;
    created->pluginId = 1;
    RxLockedPipe* locked = RxPipelineLock(created);
    if (locked != nullptr) {
        RxNodeDefinition* const nodeDefinition = reinterpret_cast<RxNodeDefinition*(__cdecl*)()>(0x80E9A0)();
        locked = RxLockedPipeAddFragment(locked, nullptr, nodeDefinition, nullptr);
        RxPipeline* const unlocked = RxLockedPipeUnlock(locked);
        if (unlocked != nullptr) {
            pipelines[0] = unlocked;
            pipelines[1] = unlocked;
            pipelines[2] = unlocked;
            pipelines[3] = unlocked;
            pipelines[4] = unlocked;
            reinterpret_cast<void(__cdecl*)(RxPipeline*, RwInt32)>(0x7EFB10)(unlocked, 3);
            reinterpret_cast<void(__cdecl*)(RxPipeline*, RwInt32)>(0x7EFB10)(unlocked, 5);
            reinterpret_cast<void(__cdecl*)(RxPipeline*, RwInt32)>(0x7EFB10)(unlocked, 4);
            reinterpret_cast<void(__cdecl*)(RxPipeline*, RwInt32)>(0x7EFB10)(unlocked, 1);
            reinterpret_cast<void(__cdecl*)(RxPipeline*, RwInt32)>(0x7EFB10)(unlocked, 2);
            return reinterpret_cast<RwInt32(__cdecl*)(RxPipeline**)>(0x80E020)(pipelines);
        }
        toDestroy = unlocked;
    }
    _rxPipelineDestroy(toDestroy);
    return 0;
}

void RwCoreInjectHooksR15A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(_rwIm3DDefaultPipelinesCreate, 0x80A1A0);
    RH_ScopedGlobalInstall(_rxPipelineUnlockLinkItems, 0x80F970);
    RH_ScopedGlobalInstall(_rxPipelineUnlockAssignSlots, 0x80FBA0);
    RH_ScopedGlobalInstall(_rxPipelineUnlockBuildExecutionData, 0x80FC30);
}

// Lote R23A (sin validar): Im3D (0x7EF450..0x7EF6B0) y RwCamera (0x7EE1D0..0x7EF1F0).
void RwCoreInjectHooksR23A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwCameraSetNearClipPlane, 0x7EE1D0);
    RH_ScopedGlobalInstall(_rwCameraUpdateZScale, 0x7EE200);
    RH_ScopedGlobalInstall(RwCameraSetFarClipPlane, 0x7EE2A0);
    RH_ScopedGlobalInstall(RwCameraFrustumTestSphere, 0x7EE2D0);
    RH_ScopedGlobalInstall(RwCameraSetProjection, 0x7EE3A0);
    RH_ScopedGlobalInstall(RwCameraSetViewWindow, 0x7EE410);
    RH_ScopedGlobalInstall(RwCameraDestroy, 0x7EE4B0);
    RH_ScopedGlobalInstall(RwCameraCreate, 0x7EE4F0);
    RH_ScopedGlobalInstall(_rwCameraBuildFrustumPerspective, 0x7EE740);
    RH_ScopedGlobalInstall(_rwCameraBuildFrustumParallel, 0x7EEDE0);
    RH_ScopedGlobalInstall(_rwCameraBuildViewMatrixParallel, 0x7EF1F0);
    RH_ScopedGlobalInstall(RwIm3DTransform, 0x7EF450);
    RH_ScopedGlobalInstall(RwIm3DEnd, 0x7EF520);
    RH_ScopedGlobalInstall(RwIm3DRenderIndexedPrimitive, 0x7EF550);
    RH_ScopedGlobalInstall(RwIm3DRenderPrimitive, 0x7EF6B0);
}
