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
#include <xmmintrin.h>

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
    RwLLLink texDictList;       // +0x00 (cabeza de la lista de todos los diccionarios; lote R25A)
    RwFreeList* textureFreeList; // +0x08 (lista libre de RwTexture; lote R7A)
    RwFreeList* texDictFreeList; // +0x0C (lista libre de RwTexDictionary; lote R25A)
    RwTexDictionary* currentTexDict; // +0x10 (diccionario actual; lote R25A)
    RwTextureCallBackRead readCallBack; // +0x14 (lote R24A)
    RwTextureCallBackFind findCallBack; // +0x18 (lote R24A)
    RwBool mipmapping;          // +0x1C
    RwBool autoMipmapping;      // +0x20
    uint8 pad1[0x08];
    RwTextureCallBackMipmapGeneration mipmapGenerationCallBack; // +0x2C
    RwTextureCallBackMipmapName mipmapNameCallBack;             // +0x30 (lote R7A)
};
static_assert(offsetof(TextureGlobalsLayout, mipmapping) == 0x1C && offsetof(TextureGlobalsLayout, autoMipmapping) == 0x20
           && offsetof(TextureGlobalsLayout, mipmapGenerationCallBack) == 0x2C);
static_assert(offsetof(TextureGlobalsLayout, textureFreeList) == 0x08 && offsetof(TextureGlobalsLayout, mipmapNameCallBack) == 0x30);
static_assert(offsetof(TextureGlobalsLayout, readCallBack) == 0x14 && offsetof(TextureGlobalsLayout, findCallBack) == 0x18);
static_assert(offsetof(TextureGlobalsLayout, texDictFreeList) == 0x0C && offsetof(TextureGlobalsLayout, currentTexDict) == 0x10);

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

// Lote R25A (sin validar): ayudantes de las texturas (batextur.c). CopyStringChecked se define mas abajo (lote R7A). RwRaiseError es el RWERROR del SDK con el pluginID literal 1 del EXE:
// RwError { 1, _rwerror(codigo [, argumentos]) } y RwErrorSet (0x808820).
namespace {
void CopyStringChecked(char* destination, const char* source, uint32 size);
template<typename... Args>
void RwRaiseError(RwInt32 code, Args... args) {
    RwError error;
    error.pluginID  = 1;
    error.errorCode = _rwerror(code, args...);
    RwErrorSet(&error);
}
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

// 0x804FE0 (_rxPipelineOpen): reconstruida al final del archivo (lote R31A, RwCoreInjectHooksR31A).

// 0x804F60 (lote R30A, sin validar; 88 bytes; era una envoltura; cdecl, sin argumentos; cierre del modulo de tuberias Rx): si la bandera de modulo abierto (0xC9A6AC) es 0 solo devuelve 1. Si no: RwFreeListDestroy
// de la lista libre de tuberias ([RwEngineInstance + [0xC9BC60]], la misma de RxPipelineCreate), pone esa ranura a NULL, RxHeapDestroy (0x809F30) del monton global (0xC9A6B0), pone el monton y la bandera a 0
// y devuelve 1.
RwBool _rxPipelineClose() {
    if (StaticRef<RwInt32>(0xC9A6AC) != 0) {
        RwFreeList** const slot = reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9BC60));
        RwFreeListDestroy(*slot);
        RwFreeList** const slotAfter = reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9BC60));
        *slotAfter = nullptr;
        RxHeapDestroy(StaticRef<RxHeap*>(0xC9A6B0));
        StaticRef<RxHeap*>(0xC9A6B0) = nullptr;
        StaticRef<RwInt32>(0xC9A6AC) = 0;
    }
    return TRUE;
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

// 0x805820 (_rxPipelineDestroy): reconstruida al final del archivo (lote R31A, RwCoreInjectHooksR31A).

// 0x8052F0: `mov eax, [0xC9A6B0] / ret`. El montón global de Rx es una variable del EXE (se lee por dirección, sin copiarla).
RxHeap* RxHeapGetGlobalHeap() {
    return StaticRef<RxHeap*>(0xC9A6B0);
}

// 0x805710 (lote R25A, sin validar; 159 bytes; era una envoltura; cdecl, (tuberia, datos, reiniciar monton)): con `heapReset` distinto de 0 y el monton global (0xC9A6B0) sucio (+0x18) llama a _rxHeapReset (0x809EC0).
// Anota el contexto de ejecucion en las variables del EXE (resultado 0xC9BC48 = 1, tuberia 0xC9BC40, paquete de parametros {datos 0xC9BC50, monton 0xC9BC54}), pone embeddedPacketState (+0x10) a 0 y llama al cuerpo
// del primer nodo (nodes[0].nodeDef->nodeMethods.nodeBody, +4 de la definicion) con (nodo, &paquete); si devuelve 0 el resultado queda a 0. Si embeddedPacketState > 1 (con signo) lo pone a 2 y llama a
// _rxPacketDestroy (0x80F070) con embeddedPacket (+0x14). Limpia tuberia y parametros (0) y devuelve la tuberia si el resultado (releido) es distinto de 0, si no NULL.
static_assert(offsetof(RxNodeDefinition, nodeMethods) == 4 && offsetof(RxNodeMethods, nodeBody) == 0 && offsetof(RxPipeline, embeddedPacketState) == 0x10 && offsetof(RxPipeline, embeddedPacket) == 0x14
           && offsetof(RxPipeline, nodes) == 8 && offsetof(RxHeap, dirty) == 0x18);
RxPipeline* RxPipelineExecute(RxPipeline* pipeline, void* data, RwBool heapReset) {
    static auto& pipelineResult = StaticRef<RwInt32>(0xC9BC48);
    static auto& currentPipeline = StaticRef<RxPipeline*>(0xC9BC40);
    static auto& paramData = StaticRef<void*>(0xC9BC50);
    static auto& paramHeap = StaticRef<RxHeap*>(0xC9BC54);
    if (heapReset != 0) {
        RxHeap* const heap = StaticRef<RxHeap*>(0xC9A6B0);
        if (heap->dirty != 0) {
            _rxHeapReset(heap);
        }
    }
    RxHeap* const heap = StaticRef<RxHeap*>(0xC9A6B0);
    pipelineResult = 1;
    currentPipeline = pipeline;
    paramData = data;
    paramHeap = heap;
    RxPipelineNode* const node = pipeline->nodes;
    pipeline->embeddedPacketState = static_cast<rxEmbeddedPacketState>(0);
    const RwBool bodyResult = node->nodeDef->nodeMethods.nodeBody(node, reinterpret_cast<const RxPipelineNodeParam*>(&paramData));
    if (bodyResult == 0) {
        pipelineResult = 0;
    }
    if (static_cast<RwInt32>(pipeline->embeddedPacketState) > 1) {
        pipeline->embeddedPacketState = static_cast<rxEmbeddedPacketState>(2);
        _rxPacketDestroy(pipeline->embeddedPacket);
    }
    const RwInt32 result = pipelineResult;
    currentPipeline = nullptr;
    paramData = nullptr;
    paramHeap = nullptr;
    return result != 0 ? pipeline : nullptr;
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

// 0x8059C0 (RxPipelineNodeCloneDefinition): reconstruida al final del archivo (lote R31A, RwCoreInjectHooksR31A).

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

// 0x806990 (RxPipelineLock) y 0x805F40 (RxLockedPipeUnlock): reconstruidas al final del archivo (lote R31A, RwCoreInjectHooksR31A).

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

// Primera parte comun de 0x7F5840 (Release) y 0x7F5D20 (_rwD3D9VertexBufferManagerClose, lote R25A): indice actual a 0 y, por cada uno de los cuatro buferes dinamicos activos: posicion y tamano a 0 y, si tenia
// bufer, busca su nodo en la lista (0xC97B8C) y lo marca libre (inUse = 0, owner = NULL) y deja la ranura a NULL.
static void DynamicVertexBufferResetActive() {
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
}

// 0x7F5840 (lote R7A, sin validar; nombre inferido): primera parte = DynamicVertexBufferResetActive (indice a 0 y cuatro buferes activos liberados). Con DevCaps & 0x10000 (pool DEFAULT) recorre despues
// TODOS los nodos con bufer: Release, vertexBuffer = NULL y, si tiene owner, escribe NULL en la variable del propietario.
void _rwD3D9DynamicVertexBufferRelease() {
    DynamicVertexBufferResetActive();
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

// 0x7F5D20 (lote R25A, sin validar; 456 bytes; cdecl sin argumentos, sin valor de retorno; nombre inferido: cierre del gestor de buferes de vertices, `_rwD3D9VertexBufferManagerClose` del SDK): primera
// parte de 0x7F5840 (DynamicVertexBufferResetActive). Despues recorre TODA la lista de buferes dinamicos (0xC97B8C): por nodo, si tiene bufer lo libera (Release), pone vertexBuffer = NULL y, si hay owner,
// escribe NULL en la variable del propietario; devuelve el nodo a la lista libre (0xC97B90, memoryFree +0x148) y avanza la cabeza de la lista al siguiente; al terminar destruye esa lista libre
// (RwFreeListDestroy 0x801B80) y la pone a NULL. Despues, por cada grupo de la lista (0xC97B7C): devuelve a la lista libre de bloques (0xC97B84) todos sus tramos libres, libera cada bufer de sus entradas
// (Release) y las devuelve a la lista de entradas (0xC97B88) y devuelve el grupo a la de grupos (0xC97B80), avanzando la cabeza de grupos; el EXE vuelve a leer la cabeza (0xC97B7C) entre pasos (equivale
// al grupo actual). Termina destruyendo las listas libres de entradas, bloques y grupos (este orden; cada una solo si no es NULL, y la pone a NULL).
void _rwD3D9VertexBufferManagerClose() {
    DynamicVertexBufferResetActive();
    for (D3D9DynamicVertexBuffer* node = DynamicVertexBufferList; node != nullptr; node = DynamicVertexBufferList) {
        D3D9DynamicVertexBuffer* const next = node->next;
        if (node->vertexBuffer != nullptr) {
            ComCall<kSlotRelease, ULONG>(node->vertexBuffer);
            node->vertexBuffer = nullptr;
            if (node->owner != nullptr) {
                *node->owner = nullptr;
            }
        }
        RwFreeListFree(DynamicVertexBufferFreeList, node);
        DynamicVertexBufferList = next;
    }
    if (DynamicVertexBufferFreeList != nullptr) {
        RwFreeListDestroy(DynamicVertexBufferFreeList);
        DynamicVertexBufferFreeList = nullptr;
    }
    for (VertexBufferGroup* group = VertexBufferGroups; group != nullptr; group = VertexBufferGroups) {
        VertexBufferGroup* const nextGroup = group->next;
        while (group->freeBlocks != nullptr) {
            VertexBufferFreeBlock* const block = group->freeBlocks;
            VertexBufferFreeBlock* const nextBlock = block->next;
            RwFreeListFree(VertexBufferBlockFreeList, block);
            group->freeBlocks = nextBlock;
        }
        while (group->vertexBuffers != nullptr) {
            VertexBufferEntry* const entry = group->vertexBuffers;
            VertexBufferEntry* const nextEntry = entry->next;
            if (entry->vertexBuffer != nullptr) {
                ComCall<kSlotRelease, ULONG>(entry->vertexBuffer);
            }
            RwFreeListFree(VertexBufferEntryFreeList, entry);
            group->vertexBuffers = nextEntry;
        }
        RwFreeListFree(VertexBufferGroupFreeList, group);
        VertexBufferGroups = nextGroup;
    }
    if (VertexBufferEntryFreeList != nullptr) {
        RwFreeListDestroy(VertexBufferEntryFreeList);
        VertexBufferEntryFreeList = nullptr;
    }
    if (VertexBufferBlockFreeList != nullptr) {
        RwFreeListDestroy(VertexBufferBlockFreeList);
        VertexBufferBlockFreeList = nullptr;
    }
    if (VertexBufferGroupFreeList != nullptr) {
        RwFreeListDestroy(VertexBufferGroupFreeList);
        VertexBufferGroupFreeList = nullptr;
    }
}

// 0x7F5CB0 (lote R25A, sin validar; 103 bytes; cdecl sin argumentos, devuelve TRUE; nombre inferido: `_rwD3D9VertexBufferManagerOpen` del SDK): primero _rwD3D9VertexBufferManagerClose (0x7F5D20) y despues crea
// con RwFreeListCreate (pista 0x40411, alineacion 4) las cuatro listas libres: grupos (0xC97B80; 0x10 bytes, 0x10 por bloque), tramos libres (0xC97B84; 0x14 bytes, 0x64 por bloque), entradas (0xC97B88; 8 bytes,
// 0x64) y nodos dinamicos (0xC97B90; 0x14 bytes, 0x2A). Sin comprobar NULL. Llama a _rwD3D9DynamicVertexBufferManagerOpen (0x7F5940) y IGNORA su resultado: siempre TRUE.
RwBool _rwD3D9VertexBufferManagerOpen() {
    _rwD3D9VertexBufferManagerClose();
    VertexBufferGroupFreeList   = RwFreeListCreate(0x10, 0x10, 4, 0x40411);
    VertexBufferBlockFreeList   = RwFreeListCreate(0x14, 0x64, 4, 0x40411);
    VertexBufferEntryFreeList   = RwFreeListCreate(8, 0x64, 4, 0x40411);
    DynamicVertexBufferFreeList = RwFreeListCreate(0x14, 0x2A, 4, 0x40411);
    _rwD3D9DynamicVertexBufferManagerOpen();
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

// 0x7FB230 (RwRasterCreate) y 0x7FB020 (RwRasterDestroy): reconstruidas al final del archivo (lote R28A, RwCoreInjectHooksR28A).

RwRaster* RwRasterGetOffset(RwRaster* raster, RwInt16* xOffset, RwInt16* yOffset) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwInt16*, RwInt16*))0x7FAEA0)(raster, xOffset, yOffset);
}

// 0x7FB160 (RwRasterGetNumLevels): reconstruida al final del archivo (lote R28A, RwCoreInjectHooksR28A).

RwRaster* RwRasterSubRaster(RwRaster* subRaster, RwRaster* raster, RwRect* rect) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwRaster*, RwRect*))0x7FB1D0)(subRaster, raster, rect);
}

// 0x7FAF50 (RwRasterRenderFast): reconstruida al final del archivo (lote R28A, RwCoreInjectHooksR28A).

RwRaster* RwRasterRender(RwRaster* raster, RwInt32 x, RwInt32 y) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwInt32, RwInt32))0x7FAFB0)(raster, x, y);
}

RwRaster* RwRasterRenderScaled(RwRaster* raster, RwRect* rect) {
    return ((RwRaster*(__cdecl *)(RwRaster*, RwRect*))0x7FAE80)(raster, rect);
}

// 0x7FB060 y 0x7FB110 (RwRasterPushContext y RwRasterPopContext): reconstruidas al final del archivo (lote R28A, RwCoreInjectHooksR28A).

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

// 0x7FB1A0 (RwRasterShowRaster), 0x7FB2D0 (RwRasterLock) y 0x7FAEC0 (RwRasterUnlock): reconstruidas al final del archivo (lote R28A, RwCoreInjectHooksR28A).
//! NOTE: RwRasterLock is responsible for calculating the `stride` in the `raster`

// 0x7FB0E0: stdFunc[rwSTANDARDRASTERLOCKPALETTE](&paleta, raster, lockMode). El EXE reutiliza como variable de salida la ranura del
// argumento `lockMode`: si la funcion estandar devuelve TRUE sin escribir la salida, el resultado es el propio `lockMode`.
RwUInt8* RwRasterLockPalette(RwRaster* raster, RwInt32 lockMode) {
    RwUInt8* palette = reinterpret_cast<RwUInt8*>(static_cast<uintptr_t>(static_cast<uint32>(lockMode)));
    if (!RwEngineInstance->stdFunc[rwSTANDARDRASTERLOCKPALETTE](&palette, raster, lockMode)) {
        return nullptr;
    }
    return palette;
}

// 0x7FAFF0 (RwRasterUnlockPalette): reconstruida al final del archivo (lote R28A, RwCoreInjectHooksR28A).

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

// Lote R29A (sin validar): imagenes de RwCore (baimage.c). Modulo de imagenes: registro de plugins 0x8E2670; lista libre = [RwEngineInstance + [0xC9A65C]] (pista 0x30018).
namespace {
auto& ImageTKListR29A = StaticRef<RwPluginRegistry>(0x8E2670);
RwFreeList* ImageFreeListR29A() {
    return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9A65C));
}
} // namespace

// Lote R30A (sin validar): resto del modulo de imagenes (baimage.c, 0x802AF0..0x803FE0). El bloque del modulo = RwEngineInstance + [0xC9A65C]; lo que usan estas funciones: lista libre +0x00, ruta de busqueda
// (cadena de rutas separadas por ';') +0x04, tabla de gamma +0x0C (256 bytes), tabla de gamma inversa +0x10C (256 bytes), gamma +0x20C, bufer temporal +0x210 y su tamano +0x214, lista de formatos +0x21C.
// Nodo de formato (0x34 bytes): extension +0x00 (0x14 bytes), extension alternativa +0x14 (0x14 bytes), lectura +0x28, escritura +0x2C, siguiente +0x30.
namespace {
struct ImageFormatNodeR30A {
    char                 extension[0x14];  // +0x00
    char                 alternate[0x14];  // +0x14
    RwImageCallBackRead  read;             // +0x28
    RwImageCallBackWrite write;            // +0x2C
    ImageFormatNodeR30A* next;             // +0x30
};
static_assert(sizeof(ImageFormatNodeR30A) == 0x34 && offsetof(ImageFormatNodeR30A, read) == 0x28 && offsetof(ImageFormatNodeR30A, next) == 0x30);

struct ImageModuleGlobalsR30A {
    RwFreeList* freeList;            // +0x000
    char*       searchPath;          // +0x004
    uint8       unknown008[4];       // +0x008
    uint8       gammaTable[256];     // +0x00C
    uint8       gammaInverse[256];   // +0x10C
    RwReal      gamma;               // +0x20C
    void*       scratchBuffer;       // +0x210
    RwInt32     scratchSize;         // +0x214
    uint8       unknown218[4];       // +0x218
    ImageFormatNodeR30A* formats;    // +0x21C
};
static_assert(offsetof(ImageModuleGlobalsR30A, searchPath) == 0x04 && offsetof(ImageModuleGlobalsR30A, gammaTable) == 0x0C && offsetof(ImageModuleGlobalsR30A, gammaInverse) == 0x10C
           && offsetof(ImageModuleGlobalsR30A, gamma) == 0x20C && offsetof(ImageModuleGlobalsR30A, scratchBuffer) == 0x210 && offsetof(ImageModuleGlobalsR30A, scratchSize) == 0x214
           && offsetof(ImageModuleGlobalsR30A, formats) == 0x21C);
ImageModuleGlobalsR30A* ImageModuleR30A() {
    return reinterpret_cast<ImageModuleGlobalsR30A*>(reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9A65C));
}
// Devolucion de llamada de las busquedas por ruta: (ruta completa en el bufer temporal, datos). Devuelve 0 para parar la busqueda. Las del EXE (0x8032D0 y 0x803340) se llaman por direccion fija.
using ImagePathCallbackR30A = void*(__cdecl*)(char* path, void* data);
constexpr uint32 kImageOpenCallbackAddress = 0x8032D0;       // abre el archivo: (ruta, {lectura, resultado}); si existe llama a lectura(ruta) y guarda la imagen
constexpr uint32 kImageExtensionCallbackAddress = 0x803340;  // prueba cada extension registrada: (ruta, &extension encontrada)
} // namespace

// 0x803210 (lote R30A, sin validar; 179 bytes; cdecl, (tamano); nombre inferido): garantiza un bufer temporal de al menos `tamano` bytes en el modulo de imagenes (+0x210 puntero, +0x214 capacidad).
// Si tamano (con signo) <= capacidad devuelve el bufer actual. Si no: RwRealloc (RwEngineInstance + 0x13C) del bufer, o RwMalloc (+0x134) si no hay, con la pista 0x1040018; sin memoria emite
// RwError { 1, _rwerror(0x80000013, tamano) } y devuelve NULL; si no guarda puntero y capacidad y devuelve el bufer.
void* _rwImageScratchBufferReserve(RwInt32 size) {
    if (size > ImageModuleR30A()->scratchSize) {
        void* const current = ImageModuleR30A()->scratchBuffer;
        void* const grown = (current != nullptr) ? RwRealloc(current, static_cast<size_t>(size), 0x1040018) : RwMalloc(static_cast<size_t>(size), 0x1040018);
        if (grown == nullptr) {
            RwRaiseError(static_cast<RwInt32>(0x80000013), size);
            return nullptr;
        }
        ImageModuleR30A()->scratchBuffer = grown;
        ImageModuleR30A()->scratchSize = size;
    }
    return ImageModuleR30A()->scratchBuffer;
}

// 0x8030C0 (lote R30A, sin validar; 333 bytes; cdecl, (nombre, tamano extra, devolucion, datos); nombre inferido): recorre las rutas de busqueda del modulo (+0x04, separadas por ';') probando `ruta + nombre`.
// Con un nombre absoluto (0x80E9B0: empieza por '\' o por letra y ':') o sin rutas (NULL o cadena vacia) usa solo el nombre. Para cada ruta pide un bufer de strlen(nombre) + longitud de la ruta + extra
// (0x803210; NULL -> devuelve 0), copia la ruta (sin anadir separador), le concatena el nombre con rwstrcpy y llama a devolucion(bufer, datos): 0 -> para y devuelve el nombre; distinto de 0 -> sigue con la
// ruta siguiente. Sin rutas pide strlen(nombre) + extra, copia el nombre y llama a devolucion una vez (resultado ignorado). Devuelve el nombre. rwstrchr se llama dos veces seguidas como en el EXE.
const RwChar* _rwImageForAllPaths(const RwChar* name, RwInt32 extra, ImagePathCallbackR30A callback, void* data) {
    const char* paths = ImageModuleR30A()->searchPath;
    const bool absolute = reinterpret_cast<RwInt32(__cdecl*)(const RwChar*)>(0x80E9B0)(name) != 0;
    if (!absolute && paths != nullptr && *paths != '\0') {
        while (*paths != '\0') {
            (void)rwstrchr(paths, ';');
            const char* const separator = rwstrchr(paths, ';');
            RwInt32 prefixLength;
            const char* next;
            if (separator != nullptr) {
                prefixLength = static_cast<RwInt32>(separator - paths);
                next = separator + 1;
            } else {
                prefixLength = static_cast<RwInt32>(rwstrlen(paths));
                next = nullptr;
            }
            const RwInt32 total = static_cast<RwInt32>(rwstrlen(name)) + prefixLength + extra;
            char* const buffer = static_cast<char*>(_rwImageScratchBufferReserve(total));
            if (buffer == nullptr) {
                return nullptr;
            }
            memcpy(buffer, paths, static_cast<size_t>(prefixLength));
            rwstrcpy(buffer + prefixLength, name);
            if (callback(buffer, data) == nullptr) {
                return name;
            }
            paths = next;
            if (paths == nullptr) {
                return name;
            }
        }
        return name;
    }
    const RwInt32 total = static_cast<RwInt32>(rwstrlen(name)) + extra;
    char* const buffer = static_cast<char*>(_rwImageScratchBufferReserve(total));
    if (buffer == nullptr) {
        return nullptr;
    }
    rwstrcpy(buffer, name);
    callback(buffer, data);
    return name;
}

// 0x803310 (lote R30A, sin validar; 39 bytes; cdecl, (nombre); nombre inferido): busca la extension registrada con la que existe el archivo `nombre + extension` en alguna de las rutas: llama a
// _rwImageForAllPaths(nombre, 0x14, 0x803340, &resultado) con resultado = NULL y lo devuelve (NULL si ninguna extension existe; es el puntero a la cadena del nodo del formato).
const RwChar* _rwImageFindFormatExtension(const RwChar* name) {
    const RwChar* found = nullptr;
    _rwImageForAllPaths(name, 0x14, reinterpret_cast<ImagePathCallbackR30A>(kImageExtensionCallbackAddress), &found);
    return found;
}

// 0x803790 (lote R30A, sin validar; 162 bytes; cdecl, (destino, origen); nombre inferido): copia paleta y pixeles entre imagenes de la misma profundidad. Si ambas tienen paleta (+0x18) y la profundidad del
// origen es <= 8 (con signo) copia (1 << profundidad) * 4 bytes de paleta. Despues copia alto (destino, con signo; <= 0 nada) filas de ((profundidad del destino + 7) >> 3) * ancho bytes; avanza el destino
// con SU stride (+0x10) y el origen con el stride del origen. Devuelve TRUE.
RwBool _rwImageCopySameDepth(RwImage* destination, const RwImage* source) {
    if (destination->palette != nullptr && source->palette != nullptr && source->depth <= 8) {
        memcpy(destination->palette, source->palette, static_cast<size_t>((1 << (source->depth & 0x1F)) << 2));
    }
    const RwInt32 rowBytes = ((destination->depth + 7) >> 3) * destination->width;
    const RwUInt8* sourceRow = source->cpPixels;
    RwUInt8* destinationRow = destination->cpPixels;
    for (RwInt32 row = 0; row < destination->height; ++row) {
        memcpy(destinationRow, sourceRow, static_cast<size_t>(static_cast<uint32>(rowBytes)));
        destinationRow += destination->stride;
        sourceRow += source->stride;
    }
    return TRUE;
}

// 0x803840 (lote R30A, sin validar; 271 bytes; cdecl, (destino, origen); nombre inferido): conversion de profundidad (clave = profundidad origen << 8 | profundidad destino). 0x404, 0x808 y 0x2020: no hace nada
// (TRUE). 0x408 (4 -> 8): copia `ancho` bytes por fila. 0x420 y 0x820 (4 u 8 -> 32): cada pixel del destino = paleta del origen[indice] (dword); las filas con ancho <= 0 solo avanzan. Alto con signo (<= 0 no hace
// nada); el destino avanza con su stride y el origen con el suyo. Otra clave: RwError { 1, _rwerror(0x80000009) } y FALSE. No toca las banderas.
RwBool _rwImageCopyConvertDepth(RwImage* destination, const RwImage* source) {
    const RwInt32 key = (source->depth << 8) | destination->depth;
    const RwInt32 width = destination->width;
    const RwUInt8* sourceRow = source->cpPixels;
    RwUInt8* destinationRow = destination->cpPixels;
    const RwRGBA* palette = source->palette;
    switch (key) {
    case 0x404:
    case 0x808:
    case 0x2020:
        return TRUE;
    case 0x408:
        for (RwInt32 row = 0; row < destination->height; ++row) {
            memcpy(destinationRow, sourceRow, static_cast<size_t>(static_cast<uint32>(width)));
            sourceRow += source->stride;
            destinationRow += destination->stride;
        }
        return TRUE;
    case 0x420:
    case 0x820:
        for (RwInt32 row = 0; row < destination->height; ++row) {
            for (RwInt32 x = 0; x < width; ++x) {
                reinterpret_cast<uint32*>(destinationRow)[x] = reinterpret_cast<const uint32*>(palette)[sourceRow[x]];
            }
            sourceRow += source->stride;
            destinationRow += destination->stride;
        }
        return TRUE;
    default:
        RwRaiseError(static_cast<RwInt32>(0x80000009));
        return FALSE;
    }
}

// 0x8026E0 (lote R29A, sin validar; 87 bytes; era una envoltura; cdecl, (ancho, alto, profundidad); `RwImageCreate` del SDK): RwFreeListAlloc (RwEngineInstance + 0x144) de la lista de imagenes con la pista 0x30018;
// sin memoria devuelve NULL. Escribe ancho (+4), alto (+8), profundidad (+0xC) y pone a cero cpPixels (+0x14), palette (+0x18) y flags (+0) (el stride +0x10 no se toca) y llama a
// _rwPluginRegistryInitObject(0x8E2670, imagen) (0x8086E0). Devuelve la imagen.
RwImage* RwImageCreate(RwInt32 width, RwInt32 height, RwInt32 depth) {
    RwImage* const image = static_cast<RwImage*>(RwFreeListAlloc(ImageFreeListR29A(), 0x30018));
    if (image == nullptr) {
        return nullptr;
    }
    image->width = width;
    image->height = height;
    image->depth = depth;
    image->cpPixels = nullptr;
    image->palette = nullptr;
    image->flags = 0;
    _rwPluginRegistryInitObject(&ImageTKListR29A, image);
    return image;
}

// 0x802740 (lote R29A, sin validar; 86 bytes; era una envoltura; cdecl, (imagen); `RwImageDestroy` del SDK): si flags (+0) & 1 (pixeles propios) libera cpPixels (+0x14) con RwFree ([RwEngineInstance + 0x138]), pone
// cpPixels y palette (+0x18) a NULL y quita el bit 0 de flags. Despues _rwPluginRegistryDeInitObject(0x8E2670, imagen) (0x808740) y RwFreeListFree (+0x148) a la lista de imagenes. Devuelve TRUE.
RwBool RwImageDestroy(RwImage* image) {
    if (image->flags & 1) {
        RwFree(image->cpPixels);
        image->cpPixels = nullptr;
        image->palette = nullptr;
        image->flags &= ~1;
    }
    _rwPluginRegistryDeInitObject(&ImageTKListR29A, image);
    RwFreeListFree(ImageFreeListR29A(), image);
    return TRUE;
}

// 0x8027A0 (lote R29A, sin validar; 188 bytes; era una envoltura; cdecl, (imagen); `RwImageAllocatePixels` del SDK): stride (+0x10) = (((profundidad + 7) >> 3) * ancho + 3) & ~3 (el EXE enmascara solo AL; equivale). Con
// profundidad 4 u 8 anade una paleta de (1 << profundidad) * 4 bytes tras los pixeles. RwMalloc ([RwEngineInstance + 0x134]) de alto * stride + paleta con la pista 0x30018 en cpPixels (+0x14). Sin memoria: RwError
// { pluginID 1, _rwerror(0x80000013, tamano) } + RwErrorSet y devuelve NULL (cpPixels queda NULL; el stride ya esta escrito). Si no: palette = pixeles + alto * stride solo con paleta (si no NULL) y flags |= 1. Devuelve la imagen.
RwImage* RwImageAllocatePixels(RwImage* image) {
    const RwInt32 depth = image->depth;
    bool hasPalette = false;
    RwInt32 paletteBytes = 0;
    if (depth == 4 || depth == 8) {
        hasPalette = true;
        paletteBytes = (1 << (depth & 0x1F)) << 2;
    }
    const RwInt32 stride = (((depth + 7) >> 3) * image->width + 3) & ~3;
    image->stride = stride;
    const RwInt32 pixelBytes = image->height * stride;
    const RwInt32 totalBytes = pixelBytes + paletteBytes;
    image->cpPixels = static_cast<RwUInt8*>(RwMalloc(static_cast<size_t>(totalBytes), 0x30018));
    if (image->cpPixels == nullptr) {
        RwRaiseError(static_cast<RwInt32>(0x80000013), totalBytes);
        return nullptr;
    }
    image->palette = hasPalette ? reinterpret_cast<RwRGBA*>(image->cpPixels + pixelBytes) : nullptr;
    image->flags |= 1;
    return image;
}

// 0x802860 (lote R29A, sin validar; 42 bytes; era una envoltura; cdecl, (imagen); `RwImageFreePixels` del SDK): RwFree (+0x138) de cpPixels (+0x14) SIN mirar el bit 0 de flags, pone cpPixels y palette (+0x18) a NULL y
// quita el bit 0 de flags. Devuelve la imagen.
RwImage* RwImageFreePixels(RwImage* image) {
    RwFree(image->cpPixels);
    image->cpPixels = nullptr;
    image->palette = nullptr;
    image->flags &= ~1;
    return image;
}

// 0x802AF0 (lote R30A, sin validar; 936 bytes; era una envoltura; cdecl, (imagen, mascara); `RwImageApplyMask` del SDK): copia el alfa de la mascara al alfa (byte 3) de cada pixel de la imagen. Con otras
// dimensiones: RwError { 1, _rwerror(0x8000000A) } y NULL. Profundidad de la imagen distinta de 4, 8 y 32: RwError { 1, _rwerror(0x80000009) } y NULL. Con 4 u 8 bits primero la pasa a 32: crea una imagen
// temporal (lista libre, pista 0x30018, InitObject 0x8E2670; sin memoria devuelve NULL SIN error) de la misma profundidad y la reserva (RwImageAllocatePixels; si falla la destruye sin mas y devuelve NULL),
// copia imagen -> temporal con el copiador de misma profundidad (0x803790; bit 1 de las banderas se hereda de la imagen), libera los pixeles de la imagen (si banderas & 1), la pasa a profundidad 32 y la
// reserva (el resultado se ignora), convierte temporal -> imagen (0x803840; el bit 1 pasa de la temporal a la imagen) y destruye la temporal (RwFree de sus pixeles, DeInitObject, RwFreeListFree).
// El EXE tiene dos ramas muertas en este camino (profundidades iguales con el 32 de la imagen; resultado 0 de los copiadores, que acabaria dereferenciando NULL): aqui no se reproducen.
// Despues, por fila (alto de la imagen, con signo): mascara de 4 u 8 bits -> alfa del pixel = alfa (+3) de la entrada de la paleta de la mascara indicada por el pixel; mascara de 32 bits -> alfa del pixel =
// alfa del pixel de la mascara; otra profundidad de mascara no toca nada; avanza el stride de la mascara y el de la imagen. Devuelve la imagen.
RwImage* RwImageApplyMask(RwImage* image, const RwImage* mask) {
    const RwInt32 width = image->width;
    if (width != mask->width || image->height != mask->height) {
        RwRaiseError(static_cast<RwInt32>(0x8000000A));
        return nullptr;
    }
    const RwInt32 height = image->height;
    const RwInt32 depth = image->depth;
    if (depth == 4 || depth == 8) {
        RwImage* const temporary = static_cast<RwImage*>(RwFreeListAlloc(ImageFreeListR29A(), 0x30018));
        if (temporary == nullptr) {
            return nullptr;
        }
        temporary->width = width;
        temporary->height = height;
        temporary->depth = depth;
        temporary->cpPixels = nullptr;
        temporary->palette = nullptr;
        temporary->flags = 0;
        _rwPluginRegistryInitObject(&ImageTKListR29A, temporary);
        if (RwImageAllocatePixels(temporary) == nullptr) {
            if (temporary->flags & 1) {
                RwFree(temporary->cpPixels);
                temporary->cpPixels = nullptr;
                temporary->palette = nullptr;
                temporary->flags &= ~1;
            }
            _rwPluginRegistryDeInitObject(&ImageTKListR29A, temporary);
            RwFreeListFree(ImageFreeListR29A(), temporary);
            return nullptr;
        }
        _rwImageCopySameDepth(temporary, image);
        temporary->flags = (temporary->flags & ~2) | (image->flags & 2);
        if (image->flags & 1) {
            RwFree(image->cpPixels);
            image->cpPixels = nullptr;
            image->palette = nullptr;
            image->flags &= ~1;
        }
        image->depth = 32;
        RwImageAllocatePixels(image);
        _rwImageCopyConvertDepth(image, temporary);
        image->flags = (image->flags & ~2) | (temporary->flags & 2);
        RwFree(temporary->cpPixels);
        temporary->cpPixels = nullptr;
        temporary->palette = nullptr;
        temporary->flags &= ~1;
        _rwPluginRegistryDeInitObject(&ImageTKListR29A, temporary);
        RwFreeListFree(ImageFreeListR29A(), temporary);
    } else if (depth != 32) {
        RwRaiseError(static_cast<RwInt32>(0x80000009));
        return nullptr;
    }
    RwUInt8* imageRow = image->cpPixels;
    const RwUInt8* maskRow = mask->cpPixels;
    const RwRGBA* const maskPalette = mask->palette;
    for (RwInt32 row = 0; row < image->height; ++row) {
        const RwInt32 maskDepth = mask->depth;
        if (maskDepth == 4 || maskDepth == 8) {
            for (RwInt32 x = 0; x < image->width; ++x) {
                imageRow[x * 4 + 3] = maskPalette[maskRow[x]].alpha;
            }
        } else if (maskDepth == 32) {
            for (RwInt32 x = 0; x < image->width; ++x) {
                imageRow[x * 4 + 3] = maskRow[x * 4 + 3];
            }
        }
        maskRow += mask->stride;
        imageRow += image->stride;
    }
    return image;
}

// 0x802A20 (lote R29A, sin validar; 198 bytes; era una envoltura; cdecl, (imagen); `RwImageMakeMask` del SDK): pone el alfa (byte 3) de cada color a max(R, G, B). Profundidad 4 u 8: recorre las (1 << profundidad)
// entradas de la paleta (+0x18); profundidad 32: recorre alto filas (con signo; <= 0 no hace nada) de ancho pixeles de cpPixels (+0x14) avanzando stride (+0x10) por fila; otra profundidad no toca nada. Devuelve la imagen.
RwImage* RwImageMakeMask(RwImage* image) {
    const auto setAlphaToMax = [](RwUInt8* color) {
        RwInt32 maximum = color[0];
        if (static_cast<RwInt32>(color[1]) > maximum) {
            maximum = color[1];
        }
        if (static_cast<RwInt32>(color[2]) > maximum) {
            maximum = color[2];
        }
        color[3] = static_cast<RwUInt8>(maximum);
    };
    const RwInt32 depth = image->depth;
    if (depth == 4 || depth == 8) {
        RwUInt8* color = reinterpret_cast<RwUInt8*>(image->palette);
        for (RwInt32 remaining = 1 << (depth & 0x1F); remaining > 0; --remaining, color += 4) {
            setAlphaToMax(color);
        }
    } else if (depth == 32) {
        RwUInt8* row = image->cpPixels;
        for (RwInt32 y = 0; y < image->height; ++y, row += image->stride) {
            RwUInt8* color = row;
            for (RwInt32 x = 0; x < image->width; ++x, color += 4) {
                setAlphaToMax(color);
            }
        }
    }
    return image;
}

// 0x8035C0 (lote R30A, sin validar; 375 bytes; era una envoltura; cdecl, (nombre, nombre de la mascara); `RwImageReadMaskedImage` del SDK): RwImageRead(nombre) (0x802FD0; NULL -> NULL). Sin nombre de mascara
// (NULL o cadena vacia) devuelve la imagen. Si no lee la mascara con RwImageRead: si falla destruye la imagen (flags & 1 libera pixeles, DeInitObject, RwFreeListFree: es RwImageDestroy) y devuelve NULL;
// RwImageMakeMask(mascara) (0x802A20) y, si da distinto de 0, RwImageApplyMask(imagen, mascara) (0x802AF0): con exito destruye la mascara y devuelve la imagen; si alguna falla destruye la imagen y despues la
// mascara y devuelve NULL.
RwImage* RwImageReadMaskedImage(const RwChar* imageName, const RwChar* maskname) {
    RwImage* const image = RwImageRead(imageName);
    if (image == nullptr) {
        return nullptr;
    }
    if (maskname == nullptr || maskname[0] == '\0') {
        return image;
    }
    RwImage* const mask = RwImageRead(maskname);
    if (mask == nullptr) {
        RwImageDestroy(image);
        return nullptr;
    }
    if (RwImageMakeMask(mask) != nullptr && RwImageApplyMask(image, mask) != nullptr) {
        RwImageDestroy(mask);
        return image;
    }
    RwImageDestroy(image);
    RwImageDestroy(mask);
    return nullptr;
}

// 0x802FD0 (lote R30A, sin validar; 240 bytes; era una envoltura; cdecl, (nombre); `RwImageRead` del SDK): se queda con la parte tras la ultima ':' , '/' y '\' del nombre (rwstrrchr, cada una sobre el resultado
// de la anterior) y busca en ella la ultima '.' (sin punto -> NULL). Recorre la lista de formatos del modulo (+0x21C): el nodo coincide si rwstrcmp(extension, punto) o rwstrcmp(extension alternativa +0x14, punto)
// dan 0; sin funcion de lectura (+0x28) -> NULL. Con una coincidencia llama a _rwImageForAllPaths(nombre COMPLETO, 5, 0x8032D0, &{lectura, resultado}) y devuelve el resultado (la imagen leida por el primer
// camino donde el archivo existe y la lectura da una imagen, o NULL). Sin coincidencia -> NULL.
RwImage* RwImageRead(const RwChar* imageName) {
    const RwChar* start = imageName;
    if (const RwChar* const colon = rwstrrchr(imageName, ':')) {
        start = colon;
    }
    if (const RwChar* const slash = rwstrrchr(start, '/')) {
        start = slash;
    }
    if (const RwChar* const backslash = rwstrrchr(start, '\\')) {
        start = backslash;
    }
    const RwChar* const dot = rwstrrchr(start, '.');
    if (dot == nullptr) {
        return nullptr;
    }
    for (ImageFormatNodeR30A* format = ImageModuleR30A()->formats; format != nullptr; format = format->next) {
        if (rwstrcmp(format->extension, dot) == 0 || rwstrcmp(format->alternate, dot) == 0) {
            if (format->read == nullptr) {
                return nullptr;
            }
            struct {
                RwImageCallBackRead read;
                RwImage*            result;
            } context = { format->read, nullptr };
            _rwImageForAllPaths(imageName, 5, reinterpret_cast<ImagePathCallbackR30A>(kImageOpenCallbackAddress), &context);
            return context.result;
        }
    }
    return nullptr;
}

const RwChar* RwImageSetPath(const RwChar* path) {
    return ((const RwChar*(__cdecl *)(const RwChar*))0x802EA0)(path);
}

// 0x803FE0 (lote R30A, sin validar; 325 bytes; era una envoltura; cdecl, (gamma float); `RwImageSetGamma` del SDK): guarda gamma en el modulo de imagenes (+0x20C) y rellena las dos tablas de 256 bytes:
// tabla (+0x0C)[0] = tabla inversa (+0x10C)[0] = 0 y, para i = 1..255, t = float(i * (1 / 255)) (0x859A3C; fild + fmul y guardado como float), tabla[i] = byte de truncar(float(pow(t, float(1.0f / gamma)) * 255.0f
// + 0.5f)) e inversa[i] = lo mismo con pow(t, gamma) (pow del CRT del EXE 0x8220F0, cdecl con dos double; 255.0f en 0x859AAC y 0.5f en 0x858B8C; la x87 a 24 bits, el truncado con fistp en modo truncar). El
// calculo esta en ImageGammaTablesCpp (X87Math.cpp). Devuelve TRUE.
RwBool RwImageSetGamma(RwReal gammaValue) {
    ImageModuleGlobalsR30A* const module = ImageModuleR30A();
    module->gamma = gammaValue;
    ImageGammaTablesCpp(gammaValue, module->gammaTable, module->gammaInverse, reinterpret_cast<double(__cdecl*)(double, double)>(0x8220F0));
    return TRUE;
}

// 0x803E30 (lote R30A, sin validar; 395 bytes; era una envoltura; cdecl, (imagen); `RwImageGammaCorrect` del SDK): aplica la tabla de gamma del modulo (+0x0C) a R, G y B (el alfa no cambia). Profundidad 4 u 8:
// recorre (1 << profundidad) entradas de la paleta (+0x18; NULL -> RwError { 1, _rwerror(0x80000016) } y NULL). Profundidad 32: pixeles (+0x14; NULL -> el mismo error y NULL), alto filas (con signo; <= 0 no hace
// nada) de ancho pixeles (el contador se compara solo con 0: un ancho negativo daria vueltas como en el EXE), avanzando stride (+0x10) por fila. Otra profundidad: RwError { 1, _rwerror(0x80000008) } y NULL.
// Si acaba pone el bit 1 de las banderas (+0) y devuelve la imagen.
RwImage* RwImageGammaCorrect(RwImage* image) {
    const RwInt32 depth = image->depth;
    if (depth == 4 || depth == 8) {
        RwUInt8* color = reinterpret_cast<RwUInt8*>(image->palette);
        const RwInt32 entries = 1 << (depth & 0x1F);
        if (color == nullptr) {
            RwRaiseError(static_cast<RwInt32>(0x80000016));
            return nullptr;
        }
        const uint8* const table = ImageModuleR30A()->gammaTable;
        for (RwInt32 remaining = entries; remaining != 0; --remaining, color += 4) {
            color[0] = table[color[0]];
            color[1] = table[color[1]];
            color[2] = table[color[2]];
        }
    } else if (depth == 32) {
        RwUInt8* row = image->cpPixels;
        const RwUInt32 width = static_cast<RwUInt32>(image->width);
        const RwInt32 height = image->height;
        if (row == nullptr) {
            RwRaiseError(static_cast<RwInt32>(0x80000016));
            return nullptr;
        }
        if (height > 0) {
            const uint8* const table = ImageModuleR30A()->gammaTable;
            for (RwInt32 remainingRows = height; remainingRows != 0; --remainingRows) {
                RwUInt8* color = row;
                for (RwUInt32 remaining = width; remaining != 0; --remaining, color += 4) {
                    color[0] = table[color[0]];
                    color[1] = table[color[1]];
                    color[2] = table[color[2]];
                }
                row += image->stride;
            }
        }
    } else {
        RwRaiseError(static_cast<RwInt32>(0x80000008));
        return nullptr;
    }
    image->flags |= 2;
    return image;
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

// 0x7F3530 (lote R24A, sin validar; 26 bytes; era una envoltura; cdecl, (bool)): RwTextureGlobals::mipmapping (+0x1C) = enable (dword). Devuelve TRUE.
RwBool RwTextureSetMipmapping(RwBool enable) {
    TextureGlobals().mipmapping = enable;
    return TRUE;
}

// 0x7F3550: RwTextureGlobals::mipmapping
RwBool RwTextureGetMipmapping() {
    return TextureGlobals().mipmapping;
}

// 0x7F3560 (lote R24A, sin validar; 26 bytes; era una envoltura; cdecl, (bool)): RwTextureGlobals::autoMipmapping (+0x20) = enable (dword). Devuelve TRUE.
RwBool RwTextureSetAutoMipmapping(RwBool enable) {
    TextureGlobals().autoMipmapping = enable;
    return TRUE;
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

// 0x7F3500 (lote R24A, sin validar; 26 bytes; era una envoltura; cdecl, (callback)): RwTextureGlobals::readCallBack (+0x14) = callback (sin sustituir nulos). Devuelve TRUE.
RwBool RwTextureSetReadCallBack(RwTextureCallBackRead callBack) {
    TextureGlobals().readCallBack = callBack;
    return TRUE;
}

// 0x7F34F0 (lote R24A, sin validar; 16 bytes; era una envoltura; cdecl, sin argumentos): devuelve RwTextureGlobals::findCallBack (+0x18).
RwTextureCallBackFind RwTextureGetFindCallBack() {
    return TextureGlobals().findCallBack;
}

// 0x7F34D0 (lote R24A, sin validar; 26 bytes; era una envoltura; cdecl, (callback)): RwTextureGlobals::findCallBack (+0x18) = callback (sin sustituir nulos). Devuelve TRUE.
RwBool RwTextureSetFindCallBack(RwTextureCallBackFind callBack) {
    TextureGlobals().findCallBack = callBack;
    return TRUE;
}

// 0x7F38A0 (lote R25A, sin validar; 110 bytes; era una envoltura; cdecl, (textura, nombre)): copia el nombre en texture->name (+0x10, 32 bytes) con rwstrncpy y, si rwstrlen(nombre) >= 32, emite el error
// {1, _rwerror(0x8000001E, nombre, 32, 31, (char)nombre[31])} y cierra la cadena (name[31] = 0). Devuelve la textura. Es CopyStringChecked (lote R7A).
RwTexture* RwTextureSetName(RwTexture* texture, const RwChar* name) {
    CopyStringChecked(texture->name, name, 0x20);
    return texture;
}

// 0x7F3910 (lote R25A, sin validar; 110 bytes; era una envoltura; cdecl, (textura, mascara)): igual que RwTextureSetName pero sobre texture->mask (+0x30; cierre en +0x4F).
RwTexture* RwTextureSetMaskName(RwTexture* texture, const RwChar* maskName) {
    CopyStringChecked(texture->mask, maskName, 0x20);
    return texture;
}

void RwTexDictionarySetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7F3E80)(blockSize, numBlocksToPrealloc);
}

// 0x7F3600 (lote R25A, sin validar; 152 bytes; era una envoltura; cdecl, sin argumentos): reserva el diccionario en la lista libre de los globales de textura (+0x0C, pista 0x30016); sin memoria -> NULL. Pone
// object.type = 6 y subType/flags/privateFlags/parent = 0, lo enlaza a la CABEZA de la lista de diccionarios (globales +0x00: lInInstance.next = cabeza.next, .prev = &cabeza, cabeza.next->prev = &enlace,
// cabeza.next = &enlace), deja vacia su lista de texturas (+0x08: prev = next = su propia direccion; el prev primero) y llama a _rwPluginRegistryInitObject(registro 0x8E23E4, diccionario) ignorando su resultado.
RwTexDictionary* RwTexDictionaryCreate() {
    TextureGlobalsLayout& globals = TextureGlobals();
    auto* const dict = static_cast<RwTexDictionary*>(RwFreeListAlloc(globals.texDictFreeList, 0x30016));
    if (dict == nullptr) {
        return nullptr;
    }
    dict->object.type = 6;
    dict->object.subType = 0;
    dict->object.flags = 0;
    dict->object.privateFlags = 0;
    dict->object.parent = nullptr;
    TextureGlobalsLayout& list = TextureGlobals();
    RwLLLink* const link = &dict->lInInstance;
    link->next = list.texDictList.next;
    link->prev = &list.texDictList;
    TextureGlobals().texDictList.next->prev = link;
    TextureGlobals().texDictList.next = link;
    dict->texturesInDict.link.prev = &dict->texturesInDict.link;
    dict->texturesInDict.link.next = &dict->texturesInDict.link;
    _rwPluginRegistryInitObject(&TexDictTKList, dict);
    return dict;
}

// 0x7F36A0 (lote R25A, sin validar; 133 bytes; era una envoltura; cdecl, (diccionario); devuelve TRUE siempre): si es el diccionario actual (globales +0x10) lo deja a NULL. Recorre su lista de texturas
// (leyendo el siguiente enlace ANTES) llamando a RwTextureDestroy (0x7F3820; el EXE le pasa ademas un 0 que ignora) y para en cuanto devuelve 0. Despues _rwPluginRegistryDeInitObject(registro 0x8E23E4,
// diccionario), lo saca de la lista global (prev->next = next; next->prev = prev) y lo libera en la lista libre de diccionarios (+0x0C). Las texturas con mas referencias quedan en su lista.
RwBool RwTexDictionaryDestroy(RwTexDictionary* dict) {
    if (TextureGlobals().currentTexDict == dict) {
        TextureGlobals().currentTexDict = nullptr;
    }
    RwLLLink* const end = &dict->texturesInDict.link;
    RwLLLink* link = end->next;
    if (link != end) {
        do {
            RwLLLink* const next = link->next;
            RwTexture* const texture = reinterpret_cast<RwTexture*>(reinterpret_cast<RwUInt8*>(link) - offsetof(RwTexture, lInDictionary));
            if (!RwTextureDestroy(texture)) {
                break;
            }
            link = next;
        } while (link != end);
    }
    _rwPluginRegistryDeInitObject(&TexDictTKList, dict);
    dict->lInInstance.prev->next = dict->lInInstance.next;
    dict->lInInstance.next->prev = dict->lInInstance.prev;
    RwFreeListFree(TextureGlobals().texDictFreeList, dict);
    return TRUE;
}

// 0x7F37C0 (lote R25A, sin validar; 90 bytes; era una envoltura; cdecl, (raster)): reserva la textura en la lista libre de los globales (+0x08, pista 0x30006); sin memoria -> NULL. filterAddressing = 0 (dword)
// y despues el byte +0x51 = 0x11 y el byte +0x50 = 1; raster, dict = NULL, name[0] = mask[0] = 0, refCount = 1 y _rwPluginRegistryInitObject(registro 0x8E23CC, textura) ignorando su resultado. Devuelve la textura.
RwTexture* RwTextureCreate(RwRaster* raster) {
    auto* const texture = static_cast<RwTexture*>(RwFreeListAlloc(TextureGlobals().textureFreeList, 0x30006));
    if (texture == nullptr) {
        return nullptr;
    }
    texture->filterAddressing = 0;
    texture->raster = raster;
    reinterpret_cast<RwUInt8*>(texture)[0x51] = 0x11;
    texture->dict = nullptr;
    texture->name[0] = 0;
    texture->mask[0] = 0;
    texture->refCount = 1;
    reinterpret_cast<RwUInt8*>(texture)[0x50] = 1;
    _rwPluginRegistryInitObject(&TextureTKList, texture);
    return texture;
}

// 0x7F3AC0 (lote R25A, sin validar; 231 bytes; era una envoltura; cdecl, (nombre, mascara)): llama a la devolucion de busqueda (globales +0x18) con (nombre); si da una textura, refCount += 1 y la devuelve.
// Si no, a la de lectura (+0x14) con (nombre, mascara); NULL -> error {1, _rwerror(0x16, nombre, mascara)} (con mascara NULL el tercer argumento es la cadena fija 0x8E240C, "(null)") y devuelve NULL.
// Con textura leida: si hay diccionario actual (+0x10), la saca de su diccionario anterior (si tiene) y la inserta a la cabeza de la lista del actual (como RwTexDictionaryAddTexture inlinado). Devuelve la textura.
RwTexture* RwTextureRead(const char* name, const char* maskName) {
    RwTexture* texture = TextureGlobals().findCallBack(name);
    if (texture != nullptr) {
        texture->refCount = texture->refCount + 1;
        return texture;
    }
    texture = TextureGlobals().readCallBack(name, maskName);
    if (texture == nullptr) {
        if (maskName != nullptr) {
            RwRaiseError(0x16, name, maskName);
        } else {
            RwRaiseError(0x16, name, reinterpret_cast<const char*>(0x8E240C));
        }
        return nullptr;
    }
    RwTexDictionary* const current = TextureGlobals().currentTexDict;
    if (current != nullptr) {
        if (texture->dict != nullptr) {
            texture->lInDictionary.prev->next = texture->lInDictionary.next;
            texture->lInDictionary.next->prev = texture->lInDictionary.prev;
        }
        texture->dict = current;
        RwLLLink* const first = current->texturesInDict.link.next;
        RwLLLink* const head = &current->texturesInDict.link;
        RwLLLink* const link = &texture->lInDictionary;
        texture->lInDictionary.prev = head;
        link->next = first;
        head->next->prev = link;
        head->next = link;
    }
    return texture;
}

// 0x7F3820 (lote R25A, sin validar; 121 bytes; era una envoltura; cdecl, (textura); devuelve TRUE siempre): refCount -= 1; si sigue > 0 (con signo) no hace mas. Si no: restaura refCount al valor anterior,
// _rwPluginRegistryDeInitObject(registro 0x8E23CC, textura), si tiene diccionario la saca de su lista (prev->next = next; next->prev = prev; sin tocar `dict`), si tiene raster lo destruye con RwRasterDestroy
// (0x7FB020) y lo pone a NULL, refCount -= 1 (queda en 0) y libera la textura en la lista libre (globales +0x08).
RwBool RwTextureDestroy(RwTexture* texture) {
    const RwInt32 remaining = texture->refCount - 1;
    texture->refCount = remaining;
    if (remaining > 0) {
        return TRUE;
    }
    texture->refCount = remaining + 1;
    _rwPluginRegistryDeInitObject(&TextureTKList, texture);
    if (texture->dict != nullptr) {
        texture->lInDictionary.prev->next = texture->lInDictionary.next;
        texture->lInDictionary.next->prev = texture->lInDictionary.prev;
    }
    if (texture->raster != nullptr) {
        RwRasterDestroy(texture->raster);
        texture->raster = nullptr;
    }
    texture->refCount = texture->refCount - 1;
    RwFreeListFree(TextureGlobals().textureFreeList, texture);
    return TRUE;
}

// 0x7F35D0 (lote R24A, sin validar; 47 bytes; era una envoltura; cdecl, (textura, raster)): raster nulo -> texture->raster (+0) = NULL y devuelve la textura sin llamar a nada. Si no llama a la funcion estandar
// rwSTANDARDTEXTURESETRASTER ([motor + 0x68], cdecl, (textura, raster, 0)): resultado distinto de cero -> devuelve la textura; cero -> NULL (neg/sbb/and).
RwTexture* RwTextureSetRaster(RwTexture* texture, RwRaster* raster) {
    if (raster == nullptr) {
        texture->raster = nullptr;
        return texture;
    }
    const RwBool result = RwEngineInstance->stdFunc[rwSTANDARDTEXTURESETRASTER](texture, raster, 0);
    return result != 0 ? texture : nullptr;
}

// 0x7F3980 (lote R25A, sin validar; 58 bytes; era una envoltura; cdecl, (diccionario, textura)): si la textura ya tenia diccionario (`dict` != NULL) la saca de su lista (prev->next = next; next->prev = prev);
// despues dict = diccionario y la inserta a la CABEZA de la lista de texturas del diccionario (lInDictionary.prev = &cabeza; next = cabeza.next; cabeza.next->prev = &enlace; cabeza.next = &enlace). Devuelve la textura.
RwTexture* RwTexDictionaryAddTexture(RwTexDictionary* dict, RwTexture* texture) {
    if (texture->dict != nullptr) {
        texture->lInDictionary.prev->next = texture->lInDictionary.next;
        texture->lInDictionary.next->prev = texture->lInDictionary.prev;
    }
    texture->dict = dict;
    RwLLLink* const head = &dict->texturesInDict.link;
    RwLLLink* const link = &texture->lInDictionary;
    RwLLLink* const first = head->next;
    texture->lInDictionary.prev = head;
    link->next = first;
    head->next->prev = link;
    head->next = link;
    return texture;
}

// 0x7F39C0 (lote R25A, sin validar; 36 bytes; era una envoltura; cdecl, (textura)): si tiene diccionario: dict = NULL y la saca de su lista (prev->next = next; next->prev = prev). Devuelve la textura.
RwTexture* RwTexDictionaryRemoveTexture(RwTexture* texture) {
    if (texture->dict != nullptr) {
        RwLLLink* const prev = texture->lInDictionary.prev;
        RwLLLink* const next = texture->lInDictionary.next;
        texture->dict = nullptr;
        prev->next = next;
        texture->lInDictionary.next->prev = texture->lInDictionary.prev;
    }
    return texture;
}

// 0x7F39F0 (lote R25A, sin validar; 115 bytes; era una envoltura; cdecl, (diccionario, nombre)): recorre la lista de texturas (de la cabeza a la cola) y devuelve la primera cuyo nombre (+0x10) coincide con
// `name` sin distinguir mayusculas: compara byte a byte parando al primer 0 de cualquiera de los dos; antes de comparar pasa a mayuscula (resta 0x20) los bytes con signo entre 'a' y 'z' (los >= 0x80 no
// cambian); al terminar compara los bytes sin convertir. Sin coincidencia -> NULL.
RwTexture* RwTexDictionaryFindNamedTexture(RwTexDictionary* dict, const RwChar* name) {
    RwLLLink* const end = &dict->texturesInDict.link;
    for (RwLLLink* link = end->next; link != end; link = link->next) {
        RwTexture* const texture = reinterpret_cast<RwTexture*>(reinterpret_cast<RwUInt8*>(link) - offsetof(RwTexture, lInDictionary));
        const signed char* a = reinterpret_cast<const signed char*>(texture->name);
        const signed char* b = reinterpret_cast<const signed char*>(name);
        bool differs = false;
        while (*a != 0) {
            signed char ca = *a;
            signed char cb = *b;
            if (cb == 0) {
                break;
            }
            if (ca >= 0x61 && ca <= 0x7A) {
                ca = static_cast<signed char>(ca + static_cast<signed char>(0xE0));
            }
            if (cb >= 0x61 && cb <= 0x7A) {
                cb = static_cast<signed char>(cb + static_cast<signed char>(0xE0));
            }
            if (ca != cb) {
                differs = true;
                break;
            }
            ++a;
            ++b;
        }
        if (!differs && *a == *b) {
            return texture;
        }
    }
    return nullptr;
}

// 0x7F3A90 (lote R25A, sin validar; 16 bytes; era una envoltura; cdecl, sin argumentos): devuelve el diccionario actual (globales de textura +0x10).
RwTexDictionary* RwTexDictionaryGetCurrent() {
    return TextureGlobals().currentTexDict;
}

// 0x7F3A70 (lote R25A, sin validar; 21 bytes; era una envoltura; cdecl, (diccionario)): guarda el diccionario como actual (+0x10; acepta NULL) y lo devuelve (EAX conserva el argumento).
RwTexDictionary* RwTexDictionarySetCurrent(RwTexDictionary* dict) {
    TextureGlobals().currentTexDict = dict;
    return dict;
}

// 0x7F3730 (lote R25A, sin validar; 64 bytes; era una envoltura; cdecl, (diccionario, callback, datos)): recorre la lista de texturas llamando a callback(textura, datos) con el siguiente enlace leido ANTES; para
// si devuelve 0. Devuelve siempre el diccionario. (Lista vacia: sale sin leer callback ni datos.)
const RwTexDictionary* RwTexDictionaryForAllTextures(const RwTexDictionary* dict, RwTextureCallBack fpCallBack, void* data) {
    const RwLLLink* const end = &dict->texturesInDict.link;
    RwLLLink* link = end->next;
    if (link == end) {
        return dict;
    }
    do {
        RwLLLink* const next = link->next;
        RwTexture* const texture = reinterpret_cast<RwTexture*>(reinterpret_cast<RwUInt8*>(link) - offsetof(RwTexture, lInDictionary));
        if (fpCallBack(texture, data) == nullptr) {
            break;
        }
        link = next;
    } while (link != end);
    return dict;
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

// 0x7EFAC0 (lote R24A, sin validar; 71 bytes; era una envoltura; cdecl, (tuberia)): tuberia de transformacion del modulo Im3D (+0x00) = la tuberia dada o, si es nula, la de por defecto (+0x1C). Devuelve la que queda.
RxPipeline* RwIm3DSetTransformPipeline(RxPipeline* pipeline) {
    uint8* const module = R23AIm3DModule();
    if (pipeline != nullptr) {
        R23AIm3DField<RxPipeline*>(module, kIm3DTransformPipeline) = pipeline;
        return R23AIm3DField<RxPipeline*>(R23AIm3DModule(), kIm3DTransformPipeline);
    }
    R23AIm3DField<RxPipeline*>(module, kIm3DTransformPipeline) = R23AIm3DField<RxPipeline*>(module, 0x1C);
    return R23AIm3DField<RxPipeline*>(R23AIm3DModule(), kIm3DTransformPipeline);
}

// 0x7EFB10 (lote R24A, sin validar; 644 bytes; era una envoltura; cdecl, (tuberia, tipo)): tabla de saltos de tipo - 1 (sin signo, 0..5; son SEIS tipos): tipos 1..6 -> ranuras +0x10, +0x14, +0x04, +0x0C, +0x08 y +0x18
// del modulo Im3D, con la ranura por defecto a +0x1C mas arriba (+0x2C, +0x30, +0x20, +0x28, +0x24, +0x34). Con tuberia: la guarda en la ranura y la devuelve. Sin tuberia (nula): la ranura = su valor por defecto
// (o NULL si este es nulo) y devuelve el contenido de la ranura. Tipo fuera de 1..6: RwError { 1, _rwerror(0x25, tipo) } y NULL (en ambas ramas).
RxPipeline* RwIm3DSetRenderPipeline(RxPipeline* pipeline, RwPrimitiveType primType) {
    constexpr uint32 slots[6] = { 0x10, 0x14, 0x04, 0x0C, 0x08, 0x18 };
    constexpr uint32 defaults[6] = { 0x2C, 0x30, 0x20, 0x28, 0x24, 0x34 };
    const uint32 index = static_cast<uint32>(primType) - 1u;
    if (index > 5) {
        R23ARaiseError(0x25, static_cast<RwInt32>(primType));
        return nullptr;
    }
    uint8* const module = R23AIm3DModule();
    if (pipeline != nullptr) {
        R23AIm3DField<RxPipeline*>(module, slots[index]) = pipeline;
        return pipeline;
    }
    RxPipeline* const fallback = R23AIm3DField<RxPipeline*>(module, defaults[index]);
    R23AIm3DField<RxPipeline*>(module, slots[index]) = fallback;
    return R23AIm3DField<RxPipeline*>(R23AIm3DModule(), slots[index]);
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

// 0x7F9C30 (RwD3D9DeviceSupportsDXTTexture): reconstruida al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

void* _rwD3D9DeviceCachesReset(); // 0x7F9D50 (lote R27A; definida al final del archivo)

// 0x7F9D50 (lote R27A): el EXE usa esta direccion como "dame el dispositivo" (los ocho llamadores 0x53D91E, 0x7452B5, 0x7471A5... leen EAX como IDirect3DDevice9*), pero su cuerpo reinicia las caches de estado del
// controlador D3D9 y devuelve el dispositivo; el cuerpo C++ (_rwD3D9DeviceCachesReset) esta al final del archivo y esta funcion lo llama, igual que el EXE.
void* RwD3D9GetCurrentD3DDevice() { // IDirect3DDevice9
    return _rwD3D9DeviceCachesReset();
}

// 0x7F84E0, 0x7F84F0 y 0x7F8580: reconstruidas al final del archivo (lote R26A, RwD3D9InjectHooksR26A).

void RwD3D9EngineSetMultiThreadSafe(RwBool enable) {
    ((void(__cdecl *)(RwBool))0x7F8620)(enable);
}

void RwD3D9EngineSetSoftwareVertexProcessing(RwBool enable) {
    ((void(__cdecl *)(RwBool))0x7F8630)(enable);
}

void* RwD3D9GetCurrentD3DRenderTarget(RwUInt32 index) {
    return ((void*(__cdecl *)(RwUInt32))0x7F9E80)(index);
}

// 0x7F9E90: reconstruida al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

// 0x7F8640, 0x7F8A90 y 0x7F8D70: reconstruidas al final del archivo (lote R26A, RwD3D9InjectHooksR26A).

// 0x7FA030: reconstruida al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

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

// 0x7FA320 y 0x7FA360 (_rwD3D9DrawIndexedPrimitive y _rwD3D9DrawPrimitive): reconstruidas al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

void _rwD3D9SetVertexShaderConstant(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount) {
   ((void(__cdecl *)(RwUInt32, const void*, RwUInt32))0x7FACA0)(registerAddress, constantData, constantCount);
}

void _rwD3D9SetPixelShaderConstant(RwUInt32 registerAddress, const void* constantData, RwUInt32 constantCount) {
   ((void(__cdecl *)(RwUInt32, const void*, RwUInt32))0x7FAD00)(registerAddress, constantData, constantCount);
}

void _rwD3D9SetFVF(RwUInt32 fvf) {
   ((void(__cdecl *)(RwUInt32))0x7F9F30)(fvf);
}

// 0x7F9F70, 0x7F9FB0 y 0x7F9FF0 (declaracion de vertices, vertex shader y pixel shader): reconstruidas al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

// 0x7FC2D0, 0x7FC320 y 0x7FC340 (RwD3D9SetRenderState, RwD3D9GetRenderState y RwD3D9SetTextureStageState): reconstruidas al final del archivo (lote R28A, RwD3D9InjectHooksR28A).

void RwD3D9GetTextureStageState(RwUInt32 stage, RwUInt32 type, void* value) {
   ((void(__cdecl *)(RwUInt32, RwUInt32, void*))0x7FC3A0)(stage, type, value);
}

// 0x7FC3C0 (RwD3D9SetSamplerState): reconstruida al final del archivo (lote R28A, RwD3D9InjectHooksR28A).

void RwD3D9GetSamplerState(RwUInt32 stage, RwUInt32 type, void* value) {
   ((void(__cdecl *)(RwUInt32, RwUInt32, void*))0x7FC400)(stage, type, value);
}

// 0x7F9D30 (RwD3D9SetStencilClear): reconstruida al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

RwUInt32 RwD3D9GetStencilClear() {
   return ((RwUInt32(__cdecl *)(void))0x7F9D40)();
}

// 0x7FDE70 (RwD3D9SetTexture): reconstruida al final del archivo (lote R28A, RwD3D9InjectHooksR28A).

// 0x7FA390 y 0x7FA4F0 (RwD3D9SetTransform y RwD3D9GetTransform): reconstruidas al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

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

// 0x7FA520 (RwD3D9SetTransformWorld): reconstruida al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

// 0x7FC4D0 (RwD3D9SetSurfaceProperties): reconstruida al final del archivo (lote R28A, RwD3D9InjectHooksR28A).

// 0x7FA660 (RwD3D9SetLight): reconstruida al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

void RwD3D9GetLight(RwInt32 index, void* light) {
   ((void(__cdecl *)(RwInt32, void*))0x7FA820)(index, light);
}

// 0x7FA860 (RwD3D9EnableLight): reconstruida al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

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

// 0x7FAC10 (RwD3D9DeleteVertexDeclaration): reconstruida al final del archivo (lote R28A, RwD3D9InjectHooksR28A).

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

// Lote R25A (sin validar): estado del controlador D3D9 de RenderWare. Direcciones del EXE: 0xC98090[4] superficies de render actuales (indice = RwD3D9SetRenderTarget); cache del ultimo estado enviado al dispositivo:
// 0x8E2440 FVF, 0x8E2444 declaracion de vertices, 0x8E2448 vertex shader, 0x8E244C pixel shader, 0x8E2450 buffer de indices (todos 0xFFFFFFFF = desconocido) y 0xC97BD8[4] flujos de vertices (0x10 bytes cada uno:
// buffer, desplazamiento, paso); 0xC98080 lista libre de matrices (0x40 bytes) y 0xC97C70[0x104] punteros a matrices del cache de transformaciones (0xC97C68 contador); 0xC98070 matriz reservada de esa lista;
// 0xC97C20 IDirect3D9*, 0xC97C24 adaptador, 0x8E2428 tipo de dispositivo, 0xC9BEF0 formato del adaptador, 0xC9BEF8 bandera de pantalla completa; 0x8E242C/0x8E2430/0x8E2434/0x8E2438 niveles de multimuestreo
// (maximo, elegido, maximo no enmascarable, elegido no enmascarable).
namespace {
constexpr size_t kSlotSetRenderTarget = 37;                  // IDirect3DDevice9::SetRenderTarget (+0x94)
constexpr size_t kSlotCreateVertexShader = 91;               // IDirect3DDevice9::CreateVertexShader (+0x16C)
constexpr size_t kSlotCheckDeviceMultiSampleType = 11;       // IDirect3D9::CheckDeviceMultiSampleType (+0x2C)
struct D3D9LastStream {                                      // elemento de 0xC97BD8 (0x10 bytes; el cuarto dword no se toca)
    uint32 vertexBuffer;                                     // +0x00
    uint32 offset;                                           // +0x04
    uint32 stride;                                           // +0x08
    uint32 unused;                                           // +0x0C
};
static_assert(sizeof(D3D9LastStream) == 0x10);
void* (&D3D9CurrentRenderTargets)[4] = *reinterpret_cast<void*(*)[4]>(0xC98090);
D3D9LastStream (&D3D9LastStreams)[4] = *reinterpret_cast<D3D9LastStream(*)[4]>(0xC97BD8);
void* (&D3D9TransformCache)[0x104]   = *reinterpret_cast<void*(*)[0x104]>(0xC97C70);
auto& D3D9TransformCacheCount       = StaticRef<uint32>(0xC97C68);
auto& D3D9TransformCacheFreeList    = StaticRef<RwFreeList*>(0xC98080);
auto& D3D9TransformCacheMatrix      = StaticRef<void*>(0xC98070);
auto& D3D9DirectObject              = StaticRef<void*>(0xC97C20);
auto& D3D9AdapterIndex              = StaticRef<uint32>(0xC97C24);
auto& D3D9DeviceType                = StaticRef<uint32>(0x8E2428);
auto& D3D9AdapterFormat             = StaticRef<uint32>(0xC9BEF0);
auto& D3D9FullScreenFlag            = StaticRef<uint32>(0xC9BEF8);
auto& D3D9MaxMultisamplingLevels    = StaticRef<uint32>(0x8E242C);
auto& D3D9SelectedMultisamplingLevels = StaticRef<uint32>(0x8E2430);
auto& D3D9MaxMultisamplingLevelsNonMask = StaticRef<uint32>(0x8E2434);
auto& D3D9SelectedMultisamplingLevelsNonMask = StaticRef<uint32>(0x8E2438);
constexpr float kD3D9IdentityMatrix[16] = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 }; // 0x8847A0 (64 bytes)
} // namespace

// 0x7F5F20 (lote R25A, sin validar; 59 bytes; era una envoltura de WindowedMode.cpp; cdecl, (indice, superficie); devuelve TRUE o FALSE): si la superficie del indice ya es la actual (0xC98090 + indice * 4) devuelve
// TRUE sin llamar. Si no, la anota ANTES de llamar a _RwD3DDevice->SetRenderTarget(indice, superficie) (+0x94) y devuelve TRUE si el HRESULT no es negativo.
RwBool _rwD3D9SetRenderTarget(RwUInt32 index, void* surface) {
    if (D3D9CurrentRenderTargets[index] == surface) {
        return TRUE;
    }
    void* const device = D3D9Device;
    D3D9CurrentRenderTargets[index] = surface;
    const HRESULT result = ComCall<kSlotSetRenderTarget, HRESULT>(device, index, surface);
    return result >= 0;
}

// 0x7FAC60 (lote R25A, sin validar; 48 bytes; cdecl, (funcion, vertex shader*); `RwD3D9CreateVertexShader` del SDK): _RwD3DDevice->CreateVertexShader(funcion, shader) (+0x16C). Si el HRESULT no es negativo
// pone el ultimo vertex shader enviado (0x8E2448) a 0xFFFFFFFF. Devuelve TRUE si el HRESULT no es negativo.
RwBool RwD3D9CreateVertexShader(const RwUInt32* function, void** shader) {
    const HRESULT result = ComCall<kSlotCreateVertexShader, HRESULT>(D3D9Device, function, shader);
    if (result >= 0) {
        StaticRef<uint32>(0x8E2448) = 0xFFFFFFFFu;
    }
    return result >= 0;
}

// 0x7F6B00 (lote R25A, sin validar; 64 bytes; cdecl sin argumentos; nombre inferido): borra la cache del ultimo estado enviado al dispositivo: FVF, declaracion, vertex shader, pixel shader y buffer de indices
// (0x8E2440..0x8E2450) = 0xFFFFFFFF y los cuatro flujos de vertices (0xC97BD8, 0x10 bytes) con buffer = 0xFFFFFFFF, desplazamiento = 0 y paso = 0 (el cuarto dword no se toca).
void _rwD3D9StateCacheReset() {
    StaticRef<uint32>(0x8E2440) = 0xFFFFFFFFu;
    StaticRef<uint32>(0x8E2444) = 0xFFFFFFFFu;
    StaticRef<uint32>(0x8E2448) = 0xFFFFFFFFu;
    StaticRef<uint32>(0x8E244C) = 0xFFFFFFFFu;
    StaticRef<uint32>(0x8E2450) = 0xFFFFFFFFu;
    for (int i = 0; i < 4; ++i) {
        D3D9LastStreams[i].vertexBuffer = 0xFFFFFFFFu;
        D3D9LastStreams[i].offset = 0;
        D3D9LastStreams[i].stride = 0;
    }
}

// 0x7F6B40 (lote R25A, sin validar; 169 bytes; cdecl sin argumentos; nombre inferido): reinicia el cache de matrices de transformacion. Contador (0xC97C68) = 0. Con lista libre (0xC98080): devuelve cada puntero no
// nulo de la tabla de 0x104 entradas (0xC97C70) a la lista (memoryFree +0x148, releyendo la lista) y lo pone a NULL, y llama a RwFreeListPurge (0x801E00). Sin lista: la crea con RwFreeListCreate(0x40, 0xF, 0x10,
// 0x30411) y pone a cero la tabla (0x410 bytes). Despues reserva una matriz de la lista (pista 0x30411; relee 0xC98080), la guarda en 0xC98070 y le copia 64 bytes: la identidad de 0x8847A0.
void _rwD3D9TransformCacheReset() {
    D3D9TransformCacheCount = 0;
    if (D3D9TransformCacheFreeList != nullptr) {
        for (int i = 0; i < 0x104; ++i) {
            if (D3D9TransformCache[i] != nullptr) {
                RwFreeListFree(D3D9TransformCacheFreeList, D3D9TransformCache[i]);
                D3D9TransformCache[i] = nullptr;
            }
        }
        RwFreeListPurge(D3D9TransformCacheFreeList);
    } else {
        D3D9TransformCacheFreeList = RwFreeListCreate(0x40, 0xF, 0x10, 0x30411);
        memset(D3D9TransformCache, 0, sizeof(D3D9TransformCache));
    }
    void* const matrix = RwFreeListAlloc(D3D9TransformCacheFreeList, 0x30411);
    D3D9TransformCacheMatrix = matrix;
    memcpy(matrix, kD3D9IdentityMatrix, sizeof(kD3D9IdentityMatrix));
}

// 0x7F6BF0 (lote R25A, sin validar; 180 bytes; cdecl sin argumentos; `_rwD3D9FindMSLevels` del SDK): pone a 1 los cuatro niveles de multimuestreo (0x8E242C..0x8E2438). windowed = (~[0xC9BEF8]) & 1. Para nivel
// = 2..16 (unsigned) llama a IDirect3D9::CheckDeviceMultiSampleType (+0x2C) con (adaptador, tipo de dispositivo, formato del adaptador, windowed, nivel, NULL) y si el HRESULT no es negativo guarda el nivel como
// maximo (0x8E242C). Despues la misma llamada con el tipo 1 (NONMASKABLE) y &calidad (la variable local de nivel, que valia 0x11): si no es negativo, maximo no enmascarable (0x8E2434) = calidad + 1 y, si es
// mayor que el maximo (0x8E242C), tambien lo sustituye (unsigned).
void _rwD3D9FindMSLevels() {
    const uint32 windowed = (~D3D9FullScreenFlag) & 1u;
    D3D9MaxMultisamplingLevels = 1;
    D3D9SelectedMultisamplingLevels = 1;
    D3D9MaxMultisamplingLevelsNonMask = 1;
    D3D9SelectedMultisamplingLevelsNonMask = 1;
    uint32 level = 2;
    do {
        const HRESULT result = ComCall<kSlotCheckDeviceMultiSampleType, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, D3D9AdapterFormat, windowed, level, static_cast<uint32*>(nullptr));
        if (result >= 0) {
            D3D9MaxMultisamplingLevels = level;
        }
        ++level;
    } while (level <= 0x10);
    const HRESULT result = ComCall<kSlotCheckDeviceMultiSampleType, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, D3D9AdapterFormat, windowed, 1u, &level);
    if (result >= 0) {
        const uint32 nonMaskable = level + 1;
        D3D9MaxMultisamplingLevelsNonMask = nonMaskable;
        if (nonMaskable > D3D9MaxMultisamplingLevels) {
            D3D9MaxMultisamplingLevels = nonMaskable;
        }
    }
}

// 0x7FA090 (_rwD3D9SetStreams): reconstruida al final del archivo (lote R27A, RwD3D9InjectHooksR27A).

// 0x7FAD20 (mov eax, 0C9BF00h; ret): devuelve la direccion fija del D3DCAPS9 de RenderWare (_RwD3D9DeviceCaps). Lote R1A (sin validar).
const void* RwD3D9GetCaps() { // D3DCAPS9*
   return reinterpret_cast<const void*>(0xC9BF00);
}

// 0x7FAD30 (RwD3D9CameraIsSphereFullyInsideFrustum): reconstruida al final del archivo (lote R28A, RwD3D9InjectHooksR28A).

// 0x7FAD90 (lote R25A, sin validar; 130 bytes; era una envoltura; cdecl, (camara, caja); `RwD3D9CameraIsBBoxFullyInsideFrustum` del SDK): para cada uno de los 6 planos del frustum (camara + 0x94, 0x14
// bytes: normal, distancia, closestX/Y/Z en +0x10/+0x11/+0x12) toma la esquina de la caja mas cercana al plano (closest* = 0 -> inf, 1 -> sup; caja + 0xC/+0x10/+0x14 - closest * 12 por eje) y calcula
// d = (ny * y + x * nx) + nz * z (en este orden; x87 a 24 bits en el EXE, float aqui); si d > distancia (comparacion ordenada: con NaN no se cumple) devuelve FALSE. Con los seis planos superados, TRUE.
static_assert(sizeof(RwFrustumPlane) == 0x14 && offsetof(RwFrustumPlane, closestX) == 0x10 && offsetof(RwFrustumPlane, plane) + offsetof(RwPlane, distance) == 0x0C && offsetof(RwBBox, inf) == 0x0C);
RwBool RwD3D9CameraIsBBoxFullyInsideFrustum(const void* camera, const void* boundingBox) {
    const RwFrustumPlane* plane = static_cast<const RwCamera*>(camera)->frustumPlanes;
    const RwUInt8* const box = static_cast<const RwUInt8*>(boundingBox);
    for (int i = 0; i < 6; ++i, ++plane) {
        const float x = *reinterpret_cast<const float*>(box + 0x0C - plane->closestX * 12);
        const float y = *reinterpret_cast<const float*>(box + 0x10 - plane->closestY * 12);
        const float z = *reinterpret_cast<const float*>(box + 0x14 - plane->closestZ * 12);
        const float partial = plane->plane.normal.y * y;
        const float distance = (partial + x * plane->plane.normal.x) + plane->plane.normal.z * z;
        if (distance > plane->plane.distance) {
            return FALSE;
        }
    }
    return TRUE;
}

void _rwD3D9RasterConvertToNonPalettized(RwRaster* raster) {
   ((void(__cdecl *)(RwRaster*))0x4CD250)(raster);
}

// 0x7F5F60: salto directo a 0x4C9AB0 (registro del plugin de raster D3D9, sin reconstruir; cuerpo del EXE)
RwBool _rwDeviceRegisterPlugin() {
    return ((RwBool(__cdecl *)(void))0x4C9AB0)();
}

// 0x7FAE20 (_rwD3D9DeviceSetRestoreCallback): reconstruida al final del archivo (lote R28A, RwD3D9InjectHooksR28A).

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

// 0x803950 (lote R30A, sin validar; 468 bytes; era una llamada por direccion fija; cdecl, (destino, origen); `RwImageCopy` del SDK): copia los pixeles de origen en destino (mismo tamano; el EXE no lo comprueba).
// Misma profundidad: es el cuerpo de 0x803790 (paleta si ambas la tienen y profundidad <= 8, y filas de ((profundidad + 7) >> 3) * ancho bytes con el stride de cada una). Distinta: es el de 0x803840 (clave =
// profundidad origen << 8 | profundidad destino: 0x404/0x808/0x2020 nada, 0x408 filas de `ancho` bytes, 0x420/0x820 expansion con la paleta del origen). Otra clave: RwError { 1, _rwerror(0x80000009) }; el EXE
// sigue con destino = NULL y dereferencia NULL (cuelgue): aqui devuelve NULL sin tocar nada. Al acabar: banderas del destino = (banderas & ~2) | (banderas del origen & 2). Devuelve el destino.
RwImage* RwImageCopy(RwImage* destImage, const RwImage* sourceImage) {
    if (destImage->depth == sourceImage->depth) {
        _rwImageCopySameDepth(destImage, sourceImage);
    } else if (_rwImageCopyConvertDepth(destImage, sourceImage) == FALSE) {
        return nullptr;
    }
    destImage->flags = (destImage->flags & ~2) | (sourceImage->flags & 2);
    return destImage;
}

// 0x804250 (lote R30A, sin validar; 51 bytes; era una llamada por direccion fija; cdecl, (imagen, raster); `RwImageSetFromRaster` del SDK): stdFunc[rwSTANDARDIMAGEGETRASTER] (RwEngineInstance + 0x60) con
// (imagen, raster, 0); si da 0 devuelve NULL. Si privateFlags del raster (+0x22) & 1 pone el bit 1 de las banderas de la imagen. Devuelve la imagen.
RwImage* RwImageSetFromRaster(RwImage* image, RwRaster* raster) {
    if (!RwEngineInstance->stdFunc[rwSTANDARDIMAGEGETRASTER](image, raster, 0)) {
        return nullptr;
    }
    if (raster->privateFlags & 1) {
        image->flags |= 2;
    }
    return image;
}

// 0x804290 (lote R30A, sin validar; 48 bytes; era una llamada por direccion fija; cdecl, (raster, imagen); `RwRasterSetFromImage` del SDK): stdFunc[rwSTANDARDRASTERSETIMAGE] (+0x64) con (raster, imagen, 0); si da
// 0 devuelve NULL. Si el bit 1 de las banderas de la imagen esta puesto pone el bit 0 de privateFlags del raster (+0x22). Devuelve el raster.
RwRaster* RwRasterSetFromImage(RwRaster* raster, RwImage* image) {
    if (!RwEngineInstance->stdFunc[rwSTANDARDRASTERSETIMAGE](raster, image, 0)) {
        return nullptr;
    }
    if (image->flags & 2) {
        raster->privateFlags |= 1;
    }
    return raster;
}


RwRaster* RwRasterRead(const RwChar* filename) {
    return ((RwRaster*(__cdecl *)(const RwChar*))0x8043F0)(filename);
}

RwRaster* RwRasterReadMaskedRaster(const RwChar* filename, const RwChar* maskname) {
    return ((RwRaster*(__cdecl *)(const RwChar*, const RwChar*))0x8044E0)(filename, maskname);
}

// 0x8042C0 (lote R30A, sin validar; 107 bytes; era una llamada por direccion fija; cdecl, (imagen, tipo de raster, &ancho, &alto, &profundidad, &formato); `RwImageFindRasterFormat` del SDK): stdFunc[
// rwSTANDARDIMAGEFINDRASTERFORMAT] (+0x6C) con (&raster temporal de 0x34 bytes sin inicializar en la pila, imagen, tipo); si da 0 devuelve NULL. Si no escribe ancho (+0xC), alto (+0x10) y profundidad (+0x14)
// del temporal y formato = (cFormat (+0x23) << 8) | cType (+0x20) y devuelve la imagen.
RwImage* RwImageFindRasterFormat(RwImage* ipImage, RwInt32 nRasterType, RwInt32* npWidth, RwInt32* npHeight, RwInt32* npDepth, RwInt32* npFormat) {
    static_assert(sizeof(RwRaster) == 0x34 && offsetof(RwRaster, cType) == 0x20 && offsetof(RwRaster, cFormat) == 0x23 && offsetof(RwRaster, privateFlags) == 0x22);
    RwRaster raster;
    if (!RwEngineInstance->stdFunc[rwSTANDARDIMAGEFINDRASTERFORMAT](&raster, ipImage, nRasterType)) {
        return nullptr;
    }
    *npFormat = (static_cast<RwInt32>(raster.cFormat) << 8) | static_cast<RwInt32>(raster.cType);
    *npWidth = raster.width;
    *npHeight = raster.height;
    *npDepth = raster.depth;
    return ipImage;
}

// Lote R24A (sin validar): RwFrame (bafframe.c). Datos del EXE: lista libre de marcos = [RwEngineInstance + [0xC9796C]] (pista 0x3000E); registro de plugins de RwFrame 0x8E2270; lista de marcos sucios del motor
// (RwEngineInstance + 0xBC, RwGlobals::dirtyFrameList, circular con cabeza propia). Los bits de privateFlags (+3): 1 = jerarquia con LTM sucia, 2 = jerarquia con objetos sucios, 4 / 8 = subarbol con LTM / objetos sucios.
void rwFrameInitFields(RwFrame* frame); // 0x7F0360 (lote R11A; definida mas abajo)
namespace {
constexpr RwUInt32 kFrameMemHint = 0x3000E;
RwPluginRegistry* FrameRegistry() {
    return reinterpret_cast<RwPluginRegistry*>(0x8E2270);
}
RwFreeList* FrameFreeList() {
    return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9796C));
}
// Macro de RenderWare que el compilador del EXE expande en linea (identica a la funcion independiente 0x7F0910): si la raiz del marco no esta sucia (privateFlags & 3 == 0) la inserta a la CABEZA de la lista de marcos
// sucios; despues raiz.privateFlags |= 3 (con los flags leidos al principio) y marco.privateFlags |= 0xC (leido DESPUES de escribir la raiz: si el marco es la raiz suma ambos).
void FrameMarkDirty(RwFrame* frame) {
    RwFrame* const root = frame->root;
    const RwUInt8 rootFlags = root->object.privateFlags;
    if ((rootFlags & 3) == 0) {
        RwLLLink* const head = &RwEngineInstance->dirtyFrameList.link;
        root->inDirtyListLink.next = head->next;
        root->inDirtyListLink.prev = head;
        head->next->prev = &root->inDirtyListLink;
        head->next = &root->inDirtyListLink;
    }
    root->object.privateFlags = static_cast<RwUInt8>(rootFlags | 3);
    frame->object.privateFlags = static_cast<RwUInt8>(frame->object.privateFlags | 0x0C);
}
// Saca el marco de la lista de marcos sucios: prev->next = next; next->prev = prev (el llamador ya comprobo privateFlags & 3).
void FrameUnlinkDirty(RwFrame* frame) {
    frame->inDirtyListLink.prev->next = frame->inDirtyListLink.next;
    frame->inDirtyListLink.next->prev = frame->inDirtyListLink.prev;
}
// Saca el marco de la lista de hijos de su padre (sin comprobar que tenga padre): si es el primer hijo, el padre pasa a apuntar a su hermano; si no, busca el hermano anterior y lo enlaza con el siguiente.
void FrameUnlinkFromParent(RwFrame* child) {
    RwFrame* const parent = static_cast<RwFrame*>(child->object.parent);
    RwFrame* current = parent->child;
    if (current == child) {
        parent->child = child->next;
        return;
    }
    RwFrame* following = current->next;
    while (following != child) {
        current = following;
        following = current->next;
    }
    current->next = child->next;
}
}

// 0x7F0210 (lote R24A, sin validar; 49 bytes; cdecl, (marco, raiz); nombre inferido: pone la raiz en todo un subarbol): root (+0xA0) del marco = raiz y lo mismo, recursivamente, para cada hijo (+0x98 / hermanos +0x9C).
void _rwFrameSetRootRec(RwFrame* frame, RwFrame* root) {
    RwFrame* child = frame->child;
    frame->root = root;
    while (child != nullptr) {
        _rwFrameSetRootRec(child, root);
        child = child->next;
    }
}

// 0x7F0830 (lote R24A, sin validar; 107 bytes; cdecl, (marco); nombre inferido: destruccion recursiva de un marco y sus hijos; el EXE no devuelve nada util): marco nulo -> nada. Destruye cada hijo (se lee el
// siguiente hermano ANTES), _rwPluginRegistryDeInitObject(registro 0x8E2270, marco), si privateFlags & 3 lo saca de la lista de sucios y lo libera en la lista libre de marcos (RwFreeListFree).
void _rwFrameDestroyRec(RwFrame* frame) {
    if (frame == nullptr) {
        return;
    }
    RwFrame* child = frame->child;
    while (child != nullptr) {
        RwFrame* const nextSibling = child->next;
        _rwFrameDestroyRec(child);
        child = nextSibling;
    }
    _rwPluginRegistryDeInitObject(FrameRegistry(), frame);
    if ((frame->object.privateFlags & 3) != 0) {
        FrameUnlinkDirty(frame);
    }
    RwFreeListFree(FrameFreeList(), frame);
}

// 0x7F0050 (lote R24A, sin validar; 334 bytes; cdecl, (marco, raiz del clon); nombre inferido: clonado recursivo de una jerarquia, enlazando cada original con su clon por su campo root): reserva el clon en la
// lista libre (pista 0x3000E; sin memoria -> NULL); raiz nula -> el propio clon. Copia type, subType, flags y privateFlags del original (byte a byte), parent = NULL, lista de objetos vacia, SOLO la matriz de modelado
// (+0x10, 0x40 bytes; ni inDirtyListLink ni la LTM), child = next = NULL y root = raiz del clon. Despues original.root (+0xA0) = clon y clona cada hijo del original (raiz del clon pasada); si falla alguno destruye
// con _rwFrameDestroyRec los hijos ya clonados, _rwPluginRegistryDeInitObject, saca el clon de la lista de sucios si privateFlags & 3 (con los flags copiados) y lo libera: NULL. Cada hijo clonado se pone a la
// CABEZA de la lista de hijos del clon (orden inverso) y su parent = clon. Al final _rwPluginRegistryInitObject(registro, clon) y _rwPluginRegistryCopyObject(registro, clon, original).
RwFrame* _rwFrameCloneRec(RwFrame* frame, RwFrame* cloneRoot) {
    RwFrame* const clone = static_cast<RwFrame*>(RwFreeListAlloc(FrameFreeList(), kFrameMemHint));
    if (clone == nullptr) {
        return nullptr;
    }
    if (cloneRoot == nullptr) {
        cloneRoot = clone;
    }
    clone->object.type = frame->object.type;
    clone->object.subType = frame->object.subType;
    clone->object.flags = frame->object.flags;
    clone->object.privateFlags = frame->object.privateFlags;
    clone->objectList.link.prev = &clone->objectList.link;
    clone->objectList.link.next = &clone->objectList.link;
    clone->object.parent = nullptr;
    memcpy(&clone->modelling, &frame->modelling, sizeof(RwMatrix));
    clone->child = nullptr;
    clone->next = nullptr;
    clone->root = cloneRoot;
    RwFrame* child = frame->child;
    frame->root = clone;
    while (child != nullptr) {
        RwFrame* const childClone = _rwFrameCloneRec(child, cloneRoot);
        if (childClone == nullptr) {
            RwFrame* done = clone->child;
            while (done != nullptr) {
                RwFrame* const nextDone = done->next;
                _rwFrameDestroyRec(done);
                done = nextDone;
            }
            _rwPluginRegistryDeInitObject(FrameRegistry(), clone);
            if ((clone->object.privateFlags & 3) != 0) {
                FrameUnlinkDirty(clone);
            }
            RwFreeListFree(FrameFreeList(), clone);
            return nullptr;
        }
        childClone->next = clone->child;
        clone->child = childClone;
        childClone->object.parent = clone;
        child = child->next;
    }
    _rwPluginRegistryInitObject(FrameRegistry(), clone);
    _rwPluginRegistryCopyObject(FrameRegistry(), clone, frame);
    return clone;
}

// 0x7F1200 (lote R24A, sin validar; 63 bytes; era una envoltura; cdecl, (marco, callback, datos)): recorre la lista de objetos del marco (cabeza +0x90; el objeto es el enlace - 8; el siguiente enlace se lee ANTES
// de llamar) con callback(objeto, datos) (cdecl); se detiene si devuelve NULL o al llegar a la cabeza. Devuelve el marco.
RwFrame* RwFrameForAllObjects(RwFrame* frame, RwObjectCallBack callBack, void* data) {
    RwLLLink* const end = &frame->objectList.link;
    RwLLLink* link = end->next;
    while (link != end) {
        RwLLLink* const following = link->next;
        if (callBack(reinterpret_cast<RwObject*>(reinterpret_cast<uint8*>(link) - 8), data) == nullptr) {
            break;
        }
        if (following == end) {
            break;
        }
        link = following;
    }
    return frame;
}

// 0x7F0E30 (lote R24A, sin validar; 146 bytes; era una envoltura; cdecl, (marco, vector, combinacion)): RwMatrixTranslate(&marco->modelling, vector, combinacion) y despues el marcado de sucio (FrameMarkDirty). Devuelve el marco.
RwFrame* RwFrameTranslate(RwFrame* frame, const RwV3d* v, RwOpCombineType combine) {
    RwMatrixTranslate(&frame->modelling, v, combine);
    FrameMarkDirty(frame);
    return frame;
}

// 0x7F1010 (lote R24A, sin validar; 151 bytes; era una envoltura; cdecl, (marco, eje, angulo float, combinacion)): RwMatrixRotate(&marco->modelling, eje, angulo, combinacion) (el angulo se pasa como dword de la pila,
// sin x87) y el marcado de sucio. Devuelve el marco.
RwFrame* RwFrameRotate(RwFrame* frame, const RwV3d* axis, RwReal angle, RwOpCombineType combine) {
    RwMatrixRotate(&frame->modelling, axis, angle, combine);
    FrameMarkDirty(frame);
    return frame;
}

RwFrame* RwFrameScale(RwFrame* frame, const RwV3d* v, RwOpCombineType combine) {
    return ((RwFrame*(__cdecl *)(RwFrame*, const RwV3d*, RwOpCombineType))0x7F0ED0)(frame, v, combine);
}

// 0x7F0F70 (lote R24A, sin validar; 146 bytes; era una envoltura; cdecl, (marco, matriz, combinacion)): RwMatrixTransform(&marco->modelling, matriz, combinacion) y el marcado de sucio. Devuelve el marco.
RwFrame* RwFrameTransform(RwFrame* frame, const RwMatrix* m, RwOpCombineType combine) {
    RwMatrixTransform(&frame->modelling, m, combine);
    FrameMarkDirty(frame);
    return frame;
}

// 0x7F1170 (lote R24A, sin validar; 137 bytes; era una envoltura; cdecl, (marco)): RwMatrixOrthoNormalize(&marco->modelling, &marco->modelling) (0x7F1920, que salta a 0x7F1930) y el marcado de sucio. Devuelve el marco.
RwFrame* RwFrameOrthoNormalize(RwFrame* frame) {
    RwMatrixOrthoNormalize(&frame->modelling, &frame->modelling);
    FrameMarkDirty(frame);
    return frame;
}

// 0x7F10B0 (lote R24A, sin validar; 180 bytes; era una envoltura; cdecl, (marco)): matriz de modelado a la identidad: diagonal = 1.0f (0x3F800000) y el resto de los doce floats a 0 (SIN tocar los rellenos), y flags
// (+0x1C) |= 0x20003 (se conservan los demas bits). Despues el marcado de sucio. Devuelve el marco.
RwFrame* RwFrameSetIdentity(RwFrame* frame) {
    RwMatrix& m = frame->modelling;
    m.at.z = 1.0f;
    m.up.y = 1.0f;
    m.right.x = 1.0f;
    m.up.x = 0.0f;
    m.right.z = 0.0f;
    m.right.y = 0.0f;
    m.at.y = 0.0f;
    m.at.x = 0.0f;
    m.up.z = 0.0f;
    m.pos.z = 0.0f;
    m.pos.y = 0.0f;
    m.pos.x = 0.0f;
    m.flags |= 0x20003;
    FrameMarkDirty(frame);
    return frame;
}

RwFrame* RwFrameCloneHierarchy(RwFrame* root) {
    return ((RwFrame*(__cdecl *)(RwFrame*))0x7F0250)(root);
}

// 0x7F08A0 (lote R24A, sin validar; 112 bytes; era una envoltura; cdecl, (marco)): igual que _rwFrameDestroyRec pero SOLO para el marco dado (el EXE inlina el cuerpo): marco nulo -> TRUE sin hacer nada; si no destruye
// cada hijo con _rwFrameDestroyRec (siguiente hermano leido antes), _rwPluginRegistryDeInitObject, saca de la lista de sucios si privateFlags & 3 y libera. Devuelve siempre TRUE.
RwBool RwFrameDestroyHierarchy(RwFrame* frame) {
    if (frame == nullptr) {
        return TRUE;
    }
    RwFrame* child = frame->child;
    while (child != nullptr) {
        RwFrame* const nextSibling = child->next;
        _rwFrameDestroyRec(child);
        child = nextSibling;
    }
    _rwPluginRegistryDeInitObject(FrameRegistry(), frame);
    if ((frame->object.privateFlags & 3) != 0) {
        FrameUnlinkDirty(frame);
    }
    RwFreeListFree(FrameFreeList(), frame);
    return TRUE;
}

// 0x7F0DC0 (lote R24A, sin validar; 56 bytes; era una envoltura; cdecl, (marco, callback, datos)): recorre los hijos (+0x98, hermanos +0x9C; el siguiente se lee ANTES de llamar) con callback(hijo, datos) (cdecl); para si
// devuelve NULL o no quedan mas. Devuelve el marco.
RwFrame* RwFrameForAllChildren(RwFrame* frame, RwFrameCallBack callBack, void* data) {
    RwFrame* child = frame->child;
    while (child != nullptr) {
        RwFrame* const following = child->next;
        if (callBack(child, data) == nullptr) {
            break;
        }
        child = following;
    }
    return frame;
}

// 0x7F0CD0 (lote R24A, sin validar; 232 bytes; era una envoltura; cdecl, (hijo)): sin comprobar que tenga padre: lo saca de la lista de hijos del padre (FrameUnlinkFromParent), root = el propio marco, parent = NULL, next =
// NULL, pone la raiz de cada uno de sus hijos a el (_rwFrameSetRootRec) y lo marca sucio. Devuelve el hijo.
RwFrame* RwFrameRemoveChild(RwFrame* child) {
    FrameUnlinkFromParent(child);
    RwFrame* grandChild = child->child;
    child->root = child;
    child->object.parent = nullptr;
    child->next = nullptr;
    while (grandChild != nullptr) {
        _rwFrameSetRootRec(grandChild, child);
        grandChild = grandChild->next;
    }
    FrameMarkDirty(child);
    return child;
}

// 0x7F0B00 (lote R24A, sin validar; 461 bytes; era una envoltura; cdecl, (padre, hijo)): si el hijo tiene padre lo desengancha igual que RwFrameRemoveChild (hijo.root = hijo, parent = next = NULL, subarbol con raiz
// el hijo y marcado sucio). Despues lo pone a la cabeza de los hijos del padre (next = padre.child, padre.child = hijo, parent = padre), root = padre.root y pone esa raiz en cada hijo del hijo; si el hijo tiene
// privateFlags & 3 lo saca de la lista de sucios y borra esos dos bits; por ultimo marca sucio el hijo (su raiz nueva). Devuelve el padre.
RwFrame* RwFrameAddChild(RwFrame* parent, RwFrame* child) {
    if (child->object.parent != nullptr) {
        FrameUnlinkFromParent(child);
        RwFrame* oldGrandChild = child->child;
        child->object.parent = nullptr;
        child->next = nullptr;
        child->root = child;
        while (oldGrandChild != nullptr) {
            _rwFrameSetRootRec(oldGrandChild, child);
            oldGrandChild = oldGrandChild->next;
        }
        FrameMarkDirty(child);
    }
    child->next = parent->child;
    parent->child = child;
    RwFrame* grandChild = child->child;
    child->object.parent = parent;
    RwFrame* const newRoot = parent->root;
    child->root = newRoot;
    while (grandChild != nullptr) {
        _rwFrameSetRootRec(grandChild, newRoot);
        grandChild = grandChild->next;
    }
    if ((child->object.privateFlags & 3) != 0) {
        FrameUnlinkDirty(child);
        child->object.privateFlags = static_cast<RwUInt8>(child->object.privateFlags & 0xFC);
    }
    FrameMarkDirty(child);
    return parent;
}

// 0x7F09C0 (lote R24A, sin validar; 313 bytes; era una envoltura; cdecl, (padre, hijo)): como RwFrameAddChild pero SIN sacar al hijo de la lista de sucios ni marcar sucio el hijo al final: si el hijo tiene padre lo
// desengancha (igual que RwFrameRemoveChild, con marcado sucio de el); luego lo pone a la cabeza de los hijos del padre (next = padre.child, padre.child = hijo, parent = padre), root = padre.root y esa raiz en
// cada hijo del hijo. Devuelve el padre.
RwFrame* RwFrameAddChildNoUpdate(RwFrame* parent, RwFrame* child) {
    if (child->object.parent != nullptr) {
        FrameUnlinkFromParent(child);
        RwFrame* oldGrandChild = child->child;
        child->object.parent = nullptr;
        child->next = nullptr;
        child->root = child;
        while (oldGrandChild != nullptr) {
            _rwFrameSetRootRec(oldGrandChild, child);
            oldGrandChild = oldGrandChild->next;
        }
        FrameMarkDirty(child);
    }
    child->next = parent->child;
    parent->child = child;
    child->object.parent = parent;
    RwFrame* const newRoot = parent->root;
    child->root = newRoot;
    for (RwFrame* grandChild = child->child; grandChild != nullptr; grandChild = grandChild->next) {
        _rwFrameSetRootRec(grandChild, newRoot);
    }
    return parent;
}

RwFrame* RwFrameGetRoot(const RwFrame* frame) {
    return ((RwFrame*(__cdecl *)(const RwFrame*))0x7F09B0)(frame);
}

// 0x7F0990 (lote R24A, sin validar; 31 bytes; era una envoltura; cdecl, (marco)): si la raiz del marco tiene el bit 0 de privateFlags (LTM de la jerarquia sucia) sincroniza con _rwFrameSyncHierarchyLTM(raiz) (0x8097D0).
// Devuelve &marco->ltm (+0x50).
RwMatrix* RwFrameGetLTM(RwFrame* frame) {
    RwFrame* const root = frame->root;
    if ((root->object.privateFlags & 1) != 0) {
        _rwFrameSyncHierarchyLTM(root);
    }
    return &frame->ltm;
}

// 0x7F0910 (lote R25A, sin validar; 125 bytes; era una envoltura; cdecl, (marco)): marcado de sucio de RenderWare (FrameMarkDirty, ya usado por RwFrameTranslate y compania): si la raiz del marco no tiene los bits
// 0..1 de privateFlags la inserta a la cabeza de la lista de marcos sucios (engine + 0xBC); raiz.privateFlags |= 3 (con el valor leido ANTES de insertar) y marco.privateFlags |= 0xC (releido). Devuelve el marco.
RwFrame* RwFrameUpdateObjects(RwFrame* frame) {
    FrameMarkDirty(frame);
    return frame;
}

void RwFrameSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x7EFED0)(blockSize, numBlocksToPrealloc);
}

// 0x7F0410 (lote R24A, sin validar; 51 bytes; era una envoltura; cdecl, sin argumentos): reserva el marco en la lista libre (pista 0x3000E; sin memoria -> NULL) e inicializa los campos con rwFrameInitFields (0x7F0360).
RwFrame* RwFrameCreate() {
    auto* const frame = static_cast<RwFrame*>(RwFreeListAlloc(FrameFreeList(), kFrameMemHint));
    if (frame == nullptr) {
        return nullptr;
    }
    rwFrameInitFields(frame);
    return frame;
}

// 0x7F05A0 (lote R24A, sin validar; 330 bytes; era una envoltura; cdecl, (marco); sin comprobar nulos): _rwPluginRegistryDeInitObject(registro 0x8E2270, marco). Si tiene padre: lo saca de la lista de hijos del padre,
// parent = next = NULL, root = el propio marco, pone la raiz de sus hijos al marco (_rwFrameSetRootRec) y lo marca sucio (FrameMarkDirty). Despues, si privateFlags & 3, lo saca de la lista de sucios; pone parent = NULL en
// cada hijo (para que no queden apuntando al marco liberado) y libera el marco en la lista libre. Devuelve TRUE.
RwBool RwFrameDestroy(RwFrame* frame) {
    _rwPluginRegistryDeInitObject(FrameRegistry(), frame);
    if (frame->object.parent != nullptr) {
        FrameUnlinkFromParent(frame);
        RwFrame* child = frame->child;
        frame->object.parent = nullptr;
        frame->next = nullptr;
        frame->root = frame;
        while (child != nullptr) {
            _rwFrameSetRootRec(child, frame);
            child = child->next;
        }
        FrameMarkDirty(frame);
    }
    if ((frame->object.privateFlags & 3) != 0) {
        FrameUnlinkDirty(frame);
    }
    for (RwFrame* child = frame->child; child != nullptr; child = child->next) {
        child->object.parent = nullptr;
    }
    RwFreeListFree(FrameFreeList(), frame);
    return TRUE;
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

// 0x7F1260 (lote R24A, sin validar; 39 bytes; era una envoltura; cdecl, (tamano, id, constructor, destructor, copia)): _rwPluginRegistryAddPlugin(registro de RwFrame 0x8E2270, ...) con los mismos argumentos; devuelve su resultado.
RwInt32 RwFrameRegisterPlugin(RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor constructCB, RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return _rwPluginRegistryAddPlugin(FrameRegistry(), size, pluginID, constructCB, destructCB, copyCB);
}

RwInt32 RwFrameGetPluginOffset(RwUInt32 pluginID) {
    return ((RwInt32(__cdecl *)(RwUInt32))0x7F1290)(pluginID);
}

RwBool RwFrameValidatePlugins(const RwFrame* frame) {
    return ((RwBool(__cdecl *)(const RwFrame*))0x7F12B0)(frame);
}

// 0x7EFFB0 (lote R24A, sin validar; 153 bytes; era una envoltura; cdecl, (raiz)): clona la jerarquia con _rwFrameCloneRec(raiz, NULL); sin clon -> NULL. Si hay: clon.privateFlags &= 0xFC y lo marca sucio (FrameMarkDirty:
// con los flags de la raiz leidos DESPUES de borrar los bits, por lo que siempre se inserta en la lista de sucios). Devuelve el clon.
RwFrame* _rwFrameCloneAndLinkClones(RwFrame* root) {
    RwFrame* const clone = _rwFrameCloneRec(root, nullptr);
    if (clone == nullptr) {
        return nullptr;
    }
    clone->object.privateFlags = static_cast<RwUInt8>(clone->object.privateFlags & 0xFC);
    FrameMarkDirty(clone);
    return clone;
}

// 0x7F01A0 (lote R24A, sin validar; 98 bytes; era una envoltura; cdecl, (marco)): con padre, root = padre.root; sin padre, root = el propio marco; despues pone esa raiz en cada hijo (_rwFrameSetRootRec; el siguiente
// hermano se lee despues de la llamada). Devuelve el marco.
RwFrame* _rwFramePurgeClone(RwFrame* root) {
    RwFrame* newRoot = root;
    if (root->object.parent != nullptr) {
        newRoot = static_cast<RwFrame*>(root->object.parent)->root;
    }
    RwFrame* child = root->child;
    root->root = newRoot;
    while (child != nullptr) {
        _rwFrameSetRootRec(child, newRoot);
        child = child->next;
    }
    return root;
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

// 0x8045A0 (lote R30A, sin validar; 54 bytes; era una envoltura; cdecl, (textura); `RwTextureStreamGetSize` del SDK): tamano en el flujo = (_rwStringStreamGetSize(nombre +0x10) + 0x1C) +
// _rwStringStreamGetSize(mascara +0x30) + 0xC + _rwPluginRegistryGetSize(registro 0x8E23CC, textura) + 0xC, sumado en ese orden.
RwUInt32 RwTextureStreamGetSize(const RwTexture* texture) {
    RwUInt32 size = _rwStringStreamGetSize(texture->name) + 0x1C;
    size = size + _rwStringStreamGetSize(texture->mask) + 0xC;
    return static_cast<RwUInt32>(_rwPluginRegistryGetSize(&TextureTKList, texture)) + size + 0xC;
}


// 0x8046E0 (lote R30A, sin validar; 511 bytes; era una envoltura; cdecl, (flujo); `RwTextureStreamRead` del SDK): RwStreamFindChunk(flujo, 1, &longitud, &version) (0x7ED2D0; FALSE -> NULL sin error); version
// < 0x34000 o > 0x36003 -> RwError { 1, _rwerror(0x80000004) } y NULL. Lee `longitud` bytes a un dword puesto a 0 (RwStreamRead 0x7EC9D0; otra cantidad -> NULL). Si el nibble alto del byte 1 (direccionamiento V) es 0
// copia el U (bits 8..11) a los bits 12..15. Guarda mipmapping (0x7F3550) y automipmapping (0x7F3580) y, segun el filtro (byte bajo 3..6 = con mipmaps) pone mipmapping 1 y automipmapping = (byte 2 & 1) ? 0 : 1, o
// ambos a 0. Lee nombre y mascara con _rwStringStreamFindAndRead (0x80A510; bufer de 0x80 bytes cada uno; NULL -> restaura y NULL), RwTextureRead(nombre, mascara) (0x7F3AC0; NULL -> _rwPluginRegistrySkipDataChunks
// (0x808C10) y restaura). Con refCount (+0x54) == 1 pone filterAddressing (+0x50) = dword & 0xFFFF. Restaura mipmapping (0x7F3530) y automipmapping (0x7F3560) y devuelve la textura si
// _rwPluginRegistryReadDataChunks (0x808980) da distinto de NULL (si no NULL).
RwTexture* RwTextureStreamRead(RwStream* stream) {
    RwUInt32 length = 0;
    RwUInt32 version = 0;
    if (!RwStreamFindChunk(stream, 1, &length, &version)) {
        return nullptr;
    }
    if (version < 0x34000 || version > 0x36003) {
        RwRaiseError(static_cast<RwInt32>(0x80000004));
        return nullptr;
    }
    RwUInt32 flags = 0;
    if (RwStreamRead(stream, &flags, length) != length) {
        return nullptr;
    }
    const RwUInt32 filter = flags & 0xFF;
    if ((flags & 0xF000) == 0) {
        flags |= (flags & 0xF00) << 4;
    }
    const RwUInt32 mipFlags = (flags >> 16) & 0xFF;
    const RwBool savedMipmapping = RwTextureGetMipmapping();
    const RwBool savedAutoMipmapping = RwTextureGetAutoMipmapping();
    if (filter == 3 || filter == 4 || filter == 5 || filter == 6) {
        RwTextureSetMipmapping(1);
        RwTextureSetAutoMipmapping((mipFlags & 1) ? 0 : 1);
    } else {
        RwTextureSetMipmapping(0);
        RwTextureSetAutoMipmapping(0);
    }
    RwChar name[0x80];
    RwChar mask[0x80];
    if (_rwStringStreamFindAndRead(name, stream) == nullptr) {
        RwTextureSetMipmapping(savedMipmapping);
        RwTextureSetAutoMipmapping(savedAutoMipmapping);
        return nullptr;
    }
    if (_rwStringStreamFindAndRead(mask, stream) == nullptr) {
        RwTextureSetMipmapping(savedMipmapping);
        RwTextureSetAutoMipmapping(savedAutoMipmapping);
        return nullptr;
    }
    RwTexture* const texture = RwTextureRead(name, mask);
    if (texture == nullptr) {
        _rwPluginRegistrySkipDataChunks(&TextureTKList, stream);
        RwTextureSetMipmapping(savedMipmapping);
        RwTextureSetAutoMipmapping(savedAutoMipmapping);
        return nullptr;
    }
    if (texture->refCount == 1) {
        texture->filterAddressing = flags & 0xFFFF;
    }
    RwTextureSetMipmapping(savedMipmapping);
    RwTextureSetAutoMipmapping(savedAutoMipmapping);
    return _rwPluginRegistryReadDataChunks(&TextureTKList, stream, texture) != nullptr ? texture : nullptr;
}

// 0x8045E0 (lote R30A, sin validar; 250 bytes; era una envoltura; cdecl, (textura, flujo); `RwTextureStreamWrite` del SDK): calcula el tamano como 0x8045A0 y escribe, con _rwStreamWriteVersionedChunkHeader
// (0x7ED270) y version 0x36003 / build 0xFFFF, la cabecera tipo 6 con ese tamano y la de tipo 1 (struct) de 4 bytes (si alguna da NULL devuelve NULL). Escribe un dword (RwStreamWrite 0x7ECB30) = ((raster != NULL
// y su cFormat (+0x23) sin el bit 0x10 (mipmaps automaticos)) ? 1 : 0) << 16 | (filterAddressing (+0x50) & 0xFFFF); despues el nombre y la mascara con _rwStringStreamWrite (0x80A4A0) y por ultimo
// _rwPluginRegistryWriteDataChunks (0x808B40) del registro de texturas. Cualquier resultado 0 -> NULL; si todo va bien devuelve la textura.
const RwTexture* RwTextureStreamWrite(const RwTexture* texture, RwStream* stream) {
    const RwInt32 size = static_cast<RwInt32>(RwTextureStreamGetSize(texture));
    if (_rwStreamWriteVersionedChunkHeader(stream, 6, size, 0x36003, 0xFFFF) == nullptr) {
        return nullptr;
    }
    if (_rwStreamWriteVersionedChunkHeader(stream, 1, 4, 0x36003, 0xFFFF) == nullptr) {
        return nullptr;
    }
    const RwRaster* const raster = texture->raster;
    const RwUInt32 hasMipmaps = (raster != nullptr && (raster->cFormat & 0x10) == 0) ? 1 : 0;
    RwUInt32 flags = ((hasMipmaps & 0xFF) << 16) | (texture->filterAddressing & 0xFFFF);
    if (RwStreamWrite(stream, &flags, 4) == nullptr) {
        return nullptr;
    }
    if (_rwStringStreamWrite(texture->name, stream) == nullptr) {
        return nullptr;
    }
    if (_rwStringStreamWrite(texture->mask, stream) == nullptr) {
        return nullptr;
    }
    return _rwPluginRegistryWriteDataChunks(&TextureTKList, stream, texture) != nullptr ? texture : nullptr;
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

// RwBBoxCalculate (0x808F60) vive ahora en rwplcore.cpp (RwPlCore, lote R25A).

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

// 0x804EF0 (lote R25A, sin validar; 76 bytes; era una envoltura; cdecl, (objeto, marco), sin retorno): si el objeto ya tiene marco (object.parent) saca su enlace lFrame (+8) de la lista de objetos de ese marco
// (prev->next = next; next->prev = prev). object.parent = marco. Si el marco no es NULL inserta lFrame a la CABEZA de su lista de objetos (+0x90: lFrame.prev = &cabeza; lFrame.next = cabeza.next;
// cabeza.next->prev = &lFrame; cabeza.next = &lFrame) y llama a RwFrameUpdateObjects (0x7F0910).
static_assert(offsetof(RwObjectHasFrame, lFrame) == 8 && offsetof(RwFrame, objectList) == 0x90);
void _rwObjectHasFrameSetFrame(void *object, RwFrame *frame) {
    auto* const owner = static_cast<RwObjectHasFrame*>(object);
    if (owner->object.parent != nullptr) {
        owner->lFrame.prev->next = owner->lFrame.next;
        owner->lFrame.next->prev = owner->lFrame.prev;
    }
    owner->object.parent = frame;
    if (frame != nullptr) {
        RwLLLink* const head = &frame->objectList.link;
        RwLLLink* const link = &owner->lFrame;
        RwLLLink* const first = head->next;
        owner->lFrame.prev = head;
        link->next = first;
        head->next->prev = link;
        head->next = link;
        RwFrameUpdateObjects(frame);
    }
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

constexpr uint32 kTextureReadDefaultMaskSource = 0xC97904; // cadena de 32 bytes que el EXE copia como mascara cuando no hay nombre de mascara
// 0x803310 (lote R30A): _rwImageFindFormatExtension devuelve la extension (cadena) que se anade al nombre de una imagen segun el formato de archivo registrado, o NULL. 0x803950: RwImageCopy(destino, origen).
const char* ImageFileSuffix(const char* name) {
    return _rwImageFindFormatExtension(name);
}
RwImage* RwImageCopyByAddress(RwImage* destination, const RwImage* source) {
    return RwImageCopy(destination, source);
}
} // namespace

// 0x7F44C0 (lote R25A, sin validar; 464 bytes; era una llamada por direccion fija; cdecl, (paleta, imagen a conservar, imagenes, niveles, bits); nombre inferido): paleta comun de las imagenes de los niveles
// de un raster paletizado (la paleta es un bufer de 1 << bits entradas RGBA que pone el llamador). Si la imagen 0 tiene paleta y todas las demas (hasta niveles - 1) tienen otra con las mismas 1 << bits
// entradas (comparadas como dwords), copia la de la imagen 0 al bufer ((1 << imagen0.depth) * 4 bytes) y devuelve TRUE sin tocar nada mas. Un nivel con paleta nula o distinta deja el contador en el centinela
// 0x40 (el EXE lo conserva asi si es nulo y le suma 1 si es distinta), de modo que no coincide con `niveles`. Si no: RwPalQuantInit (FALSE si falla; el resto queda sin liberar), anade las imagenes (peso 1.0f),
// RwPalQuantResolvePalette(paleta, 1 << bits) y por nivel crea una imagen de `bits` de profundidad del mismo tamano (FALSE si falla, sin liberar el cuantizador), RwImageAllocatePixels (resultado ignorado),
// RwPalQuantMatchImage(pixeles, stride, depth, FALSE, cuantizador, imagen original), su `palette` = el bufer del llamador (no una copia), images[nivel] = la nueva y destruye la original salvo que sea la
// `imagen a conservar` (arg 2 del EXE, el llamador pasa 0). Termina con RwPalQuantTerm y TRUE.
RwBool _rwTextureMakePalette(void* palette, const RwImage* keepImage, RwImage** images, RwInt32 numLevels, RwInt32 bits) {
    RwImage* const first = images[0];
    const uint32* const firstPalette = reinterpret_cast<const uint32*>(first->palette);
    if (firstPalette != nullptr) {
        RwInt32 level = 1;
        if (numLevels > 1) {
            do {
                const uint32* const other = reinterpret_cast<const uint32*>(images[level]->palette);
                if (other == nullptr) {
                    level = 0x40;
                    break;
                }
                const RwInt32 count = 1 << bits;
                for (RwInt32 i = 0; i < count; ++i) {
                    if (firstPalette[i] != other[i]) {
                        level = 0x40;
                        break;
                    }
                }
                level = level + 1;
            } while (level < numLevels);
        }
        if (level == numLevels) {
            memcpy(palette, firstPalette, static_cast<size_t>(1 << first->depth) << 2);
            return TRUE;
        }
    }

    RwPalQuant quantizer;
    if (!RwPalQuantInit(&quantizer)) {
        return FALSE;
    }
    for (RwInt32 i = 0; i < numLevels; ++i) {
        RwPalQuantAddImage(&quantizer, images[i], 1.0f);
    }
    RwPalQuantResolvePalette(static_cast<RwRGBA*>(palette), 1 << bits, &quantizer);
    for (RwInt32 i = 0; i < numLevels; ++i) {
        RwImage* const source = images[i];
        RwImage* const indexed = RwImageCreate(source->width, source->height, bits);
        if (indexed == nullptr) {
            return FALSE;
        }
        RwImageAllocatePixels(indexed);
        RwPalQuantMatchImage(indexed->cpPixels, indexed->stride, indexed->depth, FALSE, &quantizer, source);
        indexed->palette = static_cast<RwRGBA*>(palette);
        images[i] = indexed;
        if (source != keepImage) {
            RwImageDestroy(source);
        }
    }
    RwPalQuantTerm(&quantizer);
    return TRUE;
}

// 0x7F4690 (lote R25A, sin validar; 1092 bytes; era una llamada por direccion fija; cdecl, (nombre, mascara, tipo de raster, &ancho, &alto, &profundidad, &formato); nombre inferido): lee la imagen del
// disco para una textura. Copia nombre y mascara a dos buferes de 256 bytes con CopyStringChecked (la mascara solo si existe y no esta vacia; si no queda "") y a cada uno le anade con rwstrcat la cadena de
// 0x803310(original) si no es NULL. RwImageReadMaskedImage(nombre, mascara) (NULL -> NULL). Si el ancho o el alto pedidos (entrada) son 0 llama a RwImageFindRasterFormat(imagen, tipo, ...) que los rellena;
// si falla destruye la imagen, emite el error {1, _rwerror(0x80000009)} y devuelve NULL. Si la imagen ya mide ancho x alto la devuelve tal cual. Si no: si su profundidad no es 32, la copia a una imagen de 32
// bits (RwImageCreate + RwImageAllocatePixels + RwImageCopy 0x803950; los fallos destruyen lo creado y la imagen y dan NULL) y destruye la original; despues crea una de ancho x alto x 32, RwImageResample
// (destino, origen) y destruye la anterior (mismos fallos). Con la profundidad ORIGINAL 4 u 8 la reduce de nuevo: RwPalQuantInit (si falla devuelve la de 32 bits), AddImage (1.0f), ResolvePalette (16 o 256),
// RwImageCreate(ancho, alto, 4 u 8) (si falla devuelve la de 32 bits sin liberar el cuantizador), RwImageAllocatePixels, RwPalQuantMatchImage(pixeles, stride, depth, FALSE, cuantizador, imagen de 32),
// copia 16 o 256 dwords de la paleta a la imagen nueva, destruye la de 32 bits y RwPalQuantTerm. Devuelve la imagen resultante.
RwImage* _rwTextureReadImage(const char* name, const char* maskName, uint32 rasterType, RwInt32* width, RwInt32* height, RwInt32* depth, RwInt32* format) {
    char nameBuffer[0x100];
    char maskBuffer[0x100];

    CopyStringChecked(nameBuffer, name, 0x100);
    if (const char* const suffix = ImageFileSuffix(name)) {
        rwstrcat(nameBuffer, suffix);
    }
    maskBuffer[0] = 0;
    if (maskName != nullptr && maskName[0] != 0) {
        CopyStringChecked(maskBuffer, maskName, 0x100);
        if (const char* const suffix = ImageFileSuffix(maskName)) {
            rwstrcat(maskBuffer, suffix);
        }
    }

    RwImage* image = RwImageReadMaskedImage(nameBuffer, maskBuffer);
    if (image == nullptr) {
        return nullptr;
    }
    if (*width == 0 || *height == 0) {
        if (RwImageFindRasterFormat(image, static_cast<RwInt32>(rasterType), width, height, depth, format) == nullptr) {
            RwImageDestroy(image);
            RwRaiseError(static_cast<RwInt32>(0x80000009));
            return nullptr;
        }
    }
    if (image->width == *width && image->height == *height) {
        return image;
    }

    const RwInt32 originalDepth = image->depth;
    if (originalDepth != 0x20) {
        RwImage* const converted = RwImageCreate(image->width, image->height, 0x20);
        if (converted == nullptr) {
            RwImageDestroy(image);
            return nullptr;
        }
        if (RwImageAllocatePixels(converted) == nullptr) {
            RwImageDestroy(converted);
            RwImageDestroy(image);
            return nullptr;
        }
        RwImageCopyByAddress(converted, image);
        RwImageDestroy(image);
        image = converted;
    }

    RwImage* const resized = RwImageCreate(*width, *height, 0x20);
    if (resized == nullptr) {
        RwImageDestroy(image);
        return nullptr;
    }
    if (RwImageAllocatePixels(resized) == nullptr) {
        RwImageDestroy(resized);
        RwImageDestroy(image);
        return nullptr;
    }
    RwImageResample(resized, image);
    RwImageDestroy(image);
    image = resized;

    if (originalDepth == 4 || originalDepth == 8) {
        const RwInt32 colors = originalDepth == 4 ? 0x10 : 0x100;
        RwPalQuant quantizer;
        RwRGBA palette[0x100];
        if (RwPalQuantInit(&quantizer)) {
            RwPalQuantAddImage(&quantizer, image, 1.0f);
            RwPalQuantResolvePalette(palette, colors, &quantizer);
            RwImage* const indexed = RwImageCreate(image->width, image->height, originalDepth);
            if (indexed != nullptr) {
                RwImageAllocatePixels(indexed);
                RwPalQuantMatchImage(indexed->cpPixels, indexed->stride, indexed->depth, FALSE, &quantizer, image);
                memcpy(indexed->palette, palette, static_cast<size_t>(colors) * sizeof(RwRGBA));
                RwImageDestroy(image);
                image = indexed;
                RwPalQuantTerm(&quantizer);
            }
        }
    }
    return image;
}

// 0x7F4080 (lote R25A, sin validar; 112 bytes; cdecl, (nombre, mascara, nivel de mipmap, formato sin usar); nombre inferido): devolucion por defecto de RwTextureSetMipmapNameCallBack. Con un nivel entre 1 y 15
// forma el sufijo "m" + digito hexadecimal minuscula ("0123456789abcdef" de 0x8846D8, indexada por el nivel) y lo concatena con rwstrcat al nombre y, si la mascara existe y no esta vacia, tambien a la mascara.
// Con nivel 0 o >= 16 no toca nada. Devuelve TRUE.
RwBool _rwTextureDefaultMipmapNameCallBack(RwChar* name, RwChar* maskName, RwUInt8 mipLevel, RwInt32 format) {
    static constexpr char kMipLevelDigits[17] = "0123456789abcdef";
    char suffix[3];
    suffix[0] = 'm';
    suffix[1] = (mipLevel != 0 && mipLevel < 0x10) ? kMipLevelDigits[mipLevel] : 0;
    suffix[2] = 0;
    if (suffix[1] != 0) {
        rwstrcat(name, suffix);
        if (maskName != nullptr && maskName[0] != 0) {
            rwstrcat(maskName, suffix);
        }
    }
    return TRUE;
}

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
    images[0] = _rwTextureReadImage(nameBuffer, maskBuffer, format, &width, &height, &depth, &rasterFlags);
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
                images[level] = _rwTextureReadImage(nameBuffer, maskBuffer, format, &width, &height, &depth, &rasterFlags);
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
            _rwTextureMakePalette(palette, nullptr, images, numLevels, (raster->cFormat & 0x40) != 0 ? 4 : 8);
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
// 0x800810 (lote R29A; definida al final del archivo): D3DFORMAT -> funcion de conversion RGBA -> pixel del EXE (0 si no hay).
using RwD3D9RGBAToPixelFn = RwUInt32(__cdecl*)(const void*);
RwD3D9RGBAToPixelFn _rwD3D9GetRGBAToPixelConverter(RwUInt32 d3dFormat);
namespace {
// Funciones de conversion RGBA -> pixel (cdecl, UN argumento de pila: puntero a 4 bytes R, G, B, A; devuelven el pixel en EAX); lote R29A: 0x800930..0x800C20, reconstruidas al final del archivo.
using ImageToPixelFn = RwD3D9RGBAToPixelFn;
// Tabla de saltos del EXE (0x800810; la copia de 0x801860 es identica): D3DFORMAT -> funcion; sin entrada devuelve NULL (el EXE haria call 0).
ImageToPixelFn ImageToPixelConverter(uint32 d3dFormat) {
    return _rwD3D9GetRGBAToPixelConverter(d3dFormat);
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
// Almacen temporal de Rx (0x805890 por el extremo alto, 0x8058F0 por el bajo): reconstruidos al final del archivo (lote R31A).
void* RxScratchAllocHigh(RwUInt32 size);
void* RxScratchAllocLow(RwUInt32 size);
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

// Lote R24A (sin validar): tuberias Im3D (0x7EFAC0, 0x7EFB10), RwFrame (0x7EFFB0..0x7F1260) y callbacks/modos de RwTexture (0x7F34D0..0x7F35D0).
void RwCoreInjectHooksR24A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwIm3DSetTransformPipeline, 0x7EFAC0);
    RH_ScopedGlobalInstall(RwIm3DSetRenderPipeline, 0x7EFB10);
    RH_ScopedGlobalInstall(_rwFrameCloneAndLinkClones, 0x7EFFB0);
    RH_ScopedGlobalInstall(_rwFrameCloneRec, 0x7F0050);
    RH_ScopedGlobalInstall(_rwFramePurgeClone, 0x7F01A0);
    RH_ScopedGlobalInstall(_rwFrameSetRootRec, 0x7F0210);
    RH_ScopedGlobalInstall(RwFrameCreate, 0x7F0410);
    RH_ScopedGlobalInstall(RwFrameDestroy, 0x7F05A0);
    RH_ScopedGlobalInstall(_rwFrameDestroyRec, 0x7F0830);
    RH_ScopedGlobalInstall(RwFrameDestroyHierarchy, 0x7F08A0);
    RH_ScopedGlobalInstall(RwFrameGetLTM, 0x7F0990);
    RH_ScopedGlobalInstall(RwFrameAddChildNoUpdate, 0x7F09C0);
    RH_ScopedGlobalInstall(RwFrameAddChild, 0x7F0B00);
    RH_ScopedGlobalInstall(RwFrameRemoveChild, 0x7F0CD0);
    RH_ScopedGlobalInstall(RwFrameForAllChildren, 0x7F0DC0);
    RH_ScopedGlobalInstall(RwFrameTranslate, 0x7F0E30);
    RH_ScopedGlobalInstall(RwFrameTransform, 0x7F0F70);
    RH_ScopedGlobalInstall(RwFrameRotate, 0x7F1010);
    RH_ScopedGlobalInstall(RwFrameSetIdentity, 0x7F10B0);
    RH_ScopedGlobalInstall(RwFrameOrthoNormalize, 0x7F1170);
    RH_ScopedGlobalInstall(RwFrameForAllObjects, 0x7F1200);
    RH_ScopedGlobalInstall(RwFrameRegisterPlugin, 0x7F1260);
    RH_ScopedGlobalInstall(RwTextureSetFindCallBack, 0x7F34D0);
    RH_ScopedGlobalInstall(RwTextureGetFindCallBack, 0x7F34F0);
    RH_ScopedGlobalInstall(RwTextureSetReadCallBack, 0x7F3500);
    RH_ScopedGlobalInstall(RwTextureSetMipmapping, 0x7F3530);
    RH_ScopedGlobalInstall(RwTextureSetAutoMipmapping, 0x7F3560);
    RH_ScopedGlobalInstall(RwTextureSetRaster, 0x7F35D0);
}

// Lote R25A (sin validar): diccionarios y texturas (0x7F3600..0x7F3AC0), lectura de imagenes y devolucion de nombres de mipmap (0x7F4080, 0x7F44C0, 0x7F4690), RwFrameUpdateObjects (0x7F0910),
// _rwObjectHasFrameSetFrame (0x804EF0) y RxPipelineExecute (0x805710).
void RwCoreInjectHooksR25A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwFrameUpdateObjects, 0x7F0910);
    RH_ScopedGlobalInstall(RwTexDictionaryCreate, 0x7F3600);
    RH_ScopedGlobalInstall(RwTexDictionaryDestroy, 0x7F36A0);
    RH_ScopedGlobalInstall(RwTexDictionaryForAllTextures, 0x7F3730);
    RH_ScopedGlobalInstall(RwTextureCreate, 0x7F37C0);
    RH_ScopedGlobalInstall(RwTextureDestroy, 0x7F3820);
    RH_ScopedGlobalInstall(RwTextureSetName, 0x7F38A0);
    RH_ScopedGlobalInstall(RwTextureSetMaskName, 0x7F3910);
    RH_ScopedGlobalInstall(RwTexDictionaryAddTexture, 0x7F3980);
    RH_ScopedGlobalInstall(RwTexDictionaryRemoveTexture, 0x7F39C0);
    RH_ScopedGlobalInstall(RwTexDictionaryFindNamedTexture, 0x7F39F0);
    RH_ScopedGlobalInstall(RwTexDictionarySetCurrent, 0x7F3A70);
    RH_ScopedGlobalInstall(RwTexDictionaryGetCurrent, 0x7F3A90);
    RH_ScopedGlobalInstall(RwTextureRead, 0x7F3AC0);
    RH_ScopedGlobalInstall(_rwTextureDefaultMipmapNameCallBack, 0x7F4080);
    RH_ScopedGlobalInstall(_rwTextureMakePalette, 0x7F44C0);
    RH_ScopedGlobalInstall(_rwTextureReadImage, 0x7F4690);
    RH_ScopedGlobalInstall(_rwObjectHasFrameSetFrame, 0x804EF0);
    RH_ScopedGlobalInstall(RxPipelineExecute, 0x805710);
}

// Lote R25A (sin validar): controlador D3D9 de RenderWare: gestor de buferes de vertices (0x7F5CB0, 0x7F5D20), render target (0x7F5F20), cache de estado y de matrices (0x7F6B00, 0x7F6B40),
// multimuestreo (0x7F6BF0), CreateVertexShader (0x7FAC60) y caja dentro del frustum (0x7FAD90).
void RwD3D9InjectHooksR25A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(_rwD3D9VertexBufferManagerOpen, 0x7F5CB0);
    RH_ScopedGlobalInstall(_rwD3D9VertexBufferManagerClose, 0x7F5D20);
    RH_ScopedGlobalInstall(_rwD3D9SetRenderTarget, 0x7F5F20);
    RH_ScopedGlobalInstall(_rwD3D9StateCacheReset, 0x7F6B00);
    RH_ScopedGlobalInstall(_rwD3D9TransformCacheReset, 0x7F6B40);
    RH_ScopedGlobalInstall(_rwD3D9FindMSLevels, 0x7F6BF0);
    RH_ScopedGlobalInstall(RwD3D9CreateVertexShader, 0x7FAC60);
    RH_ScopedGlobalInstall(RwD3D9CameraIsBBoxFullyInsideFrustum, 0x7FAD90);
}

// --- Lote R26A (sin validar): controlador D3D9 de RenderWare: parametros de presentacion, parada del sistema, tabla de estandares, modos de video, liberacion y restauracion del dispositivo, multimuestreo,
// cambio de modo y camara con ventana (0x7F6CB0..0x7F8D70; el listado las atribuia a RwCore por vecindad). 0x7F6781 (cola de una rutina, enganchada por WindowedMode.cpp) y 0x7F7730
// (_rwD3D9CameraClear, enganchada por WindowedMode.cpp con la correccion NOTSA del suelo reflectante) no se rehacen aqui. ---
// Estado global del EXE que usan (direcciones):
//   0xC9C040 D3DPRESENT_PARAMETERS (0x38 bytes)    0xC9BEE4 modo actual (ancho, alto, frecuencia, formato; 16 bytes) y 0xC9BEF4 su profundidad, 0xC9BEF8 su bandera, 0xC9BEFC profundidad del Z-buffer
//   0xC97C48 tabla de modos de video (0x14 bytes: D3DDISPLAYMODE + bandera) y 0xC97C40 su numero, 0xC97C18 modo actual, 0xC9BEE0 modos del adaptador, 0xC97C38 bandera "modos listos", 0xC97BD4 formato base del adaptador
//   0xC97C1C ventana principal    0xC97C28 dispositivo    0xC97C2C / 0xC97C30 superficies de profundidad y de color actuales    0xC97C54 "dentro de escena"    0xC980B0 callback de restauracion
//   0xC98088 / 0xC98084 cache de luces (0x6C bytes cada una: D3DLIGHT9 + bandera de activa) y su numero    0xC97C58 / 0xC97C60 tabla de registros de 12 bytes (objeto COM, sin uso, bloque)    0xC980A4 bandera de estado de coma flotante
namespace {
constexpr size_t kSlotD3DGetAdapterModeCount        = 6;   // IDirect3D9::GetAdapterModeCount (+0x18)
constexpr size_t kSlotD3DEnumAdapterModes           = 7;   // +0x1C
constexpr size_t kSlotD3DGetAdapterDisplayMode      = 8;   // +0x20
constexpr size_t kSlotD3DCheckDeviceType            = 9;   // +0x24
constexpr size_t kSlotD3DCheckDeviceFormat          = 10;  // +0x28
constexpr size_t kSlotD3DCheckDepthStencilMatch     = 12;  // +0x30
constexpr size_t kSlotDeviceTestCooperativeLevel    = 3;   // IDirect3DDevice9::TestCooperativeLevel (+0x0C)
constexpr size_t kSlotDeviceCreateAdditionalSwapChain = 13; // +0x34
constexpr size_t kSlotDeviceReset                   = 16;  // +0x40
constexpr size_t kSlotDeviceGetRenderTarget         = 38;  // +0x98
constexpr size_t kSlotDeviceGetDepthStencilSurface  = 40;  // +0xA0
constexpr size_t kSlotDeviceBeginScene              = 41;  // +0xA4
constexpr size_t kSlotDeviceSetLight                = 51;  // +0xCC
constexpr size_t kSlotDeviceLightEnable             = 53;  // +0xD4
constexpr size_t kSlotDeviceSetVertexDeclaration    = 87;  // +0x15C
constexpr size_t kSlotDeviceSetVertexShader         = 92;  // +0x170
constexpr size_t kSlotDeviceSetStreamSource         = 100; // +0x190
constexpr size_t kSlotDeviceSetIndices              = 104; // +0x1A0
constexpr size_t kSlotDeviceSetPixelShader          = 107; // +0x1AC
constexpr size_t kSlotSwapChainGetBackBuffer        = 5;   // IDirect3DSwapChain9::GetBackBuffer (+0x14)

struct D3D9PresentLayout {                 // D3DPRESENT_PARAMETERS (0xC9C040)
    uint32 backBufferWidth;                // +0x00
    uint32 backBufferHeight;               // +0x04
    uint32 backBufferFormat;               // +0x08
    uint32 backBufferCount;                // +0x0C
    uint32 multiSampleType;                // +0x10
    uint32 multiSampleQuality;             // +0x14
    uint32 swapEffect;                     // +0x18
    void*  deviceWindow;                   // +0x1C
    int32  windowed;                       // +0x20
    int32  enableAutoDepthStencil;         // +0x24
    uint32 autoDepthStencilFormat;         // +0x28
    uint32 flags;                          // +0x2C
    uint32 fullScreenRefreshRateInHz;      // +0x30
    uint32 presentationInterval;           // +0x34
};
static_assert(sizeof(D3D9PresentLayout) == 0x38 && offsetof(D3D9PresentLayout, windowed) == 0x20 && offsetof(D3D9PresentLayout, enableAutoDepthStencil) == 0x24
           && offsetof(D3D9PresentLayout, fullScreenRefreshRateInHz) == 0x30 && offsetof(D3D9PresentLayout, presentationInterval) == 0x34 && offsetof(D3D9PresentLayout, deviceWindow) == 0x1C);

struct D3D9DisplayModeLayout {             // D3DDISPLAYMODE
    uint32 width;                          // +0x00
    uint32 height;                         // +0x04
    uint32 refreshRate;                    // +0x08
    uint32 format;                         // +0x0C
};
struct D3D9VideoModeLayout {               // _rxD3D9DisplayMode (0x14 bytes)
    D3D9DisplayModeLayout mode;            // +0x00
    int32 flags;                           // +0x10
};
static_assert(sizeof(D3D9DisplayModeLayout) == 0x10 && sizeof(D3D9VideoModeLayout) == 0x14);

struct D3D9LightEntry {                    // RxD3D9Light: D3DLIGHT9 (0x68) + bandera
    uint8 light[0x68];
    int32 enabled;                         // +0x68
};
static_assert(sizeof(D3D9LightEntry) == 0x6C);

struct D3D9ExtraRecord {                   // registro de 12 bytes de 0xC97C60
    void*  object;                         // +0x00 (objeto COM)
    uint32 unused;                         // +0x04
    void*  block;                          // +0x08 (memoria liberada con RwFree)
};
static_assert(sizeof(D3D9ExtraRecord) == 0xC);

auto& D3D9Present                  = StaticRef<D3D9PresentLayout>(0xC9C040);
auto& D3D9MainWindow               = StaticRef<void*>(0xC97C1C);
auto& D3D9DepthStencilSurface      = StaticRef<void*>(0xC97C2C);
auto& D3D9InsideScene              = StaticRef<int32>(0xC97C54);
auto& D3D9RestoreCallback          = StaticRef<void(__cdecl*)()>(0xC980B0);
auto& D3D9ModesReady               = StaticRef<int32>(0xC97C38);
auto& D3D9VideoModes               = StaticRef<D3D9VideoModeLayout*>(0xC97C48);
auto& D3D9VideoModeCount           = StaticRef<int32>(0xC97C40);
auto& D3D9CurrentVideoMode         = StaticRef<int32>(0xC97C18);
auto& D3D9AdapterModeCount         = StaticRef<int32>(0xC9BEE0);
auto& D3D9BaseAdapterFormat        = StaticRef<uint32>(0xC97BD4);
auto& D3D9RefreshRateLimit         = StaticRef<uint32>(0x8E243C);
auto& D3D9CurrentModeDepth         = StaticRef<uint32>(0xC9BEF4);
auto& D3D9ZBufferDepthValue        = StaticRef<int32>(0xC9BEFC);
auto& D3D9HasStencilBuffer         = StaticRef<int32>(0xC97C3C);
auto& D3D9LightCount               = StaticRef<int32>(0xC98084);
auto& D3D9LightCache               = StaticRef<D3D9LightEntry*>(0xC98088);
auto& D3D9ExtraRecordCount         = StaticRef<uint32>(0xC97C58);
auto& D3D9ExtraRecordUnused        = StaticRef<uint32>(0xC97C5C);
auto& D3D9ExtraRecords             = StaticRef<D3D9ExtraRecord*>(0xC97C60);
auto& D3D9FpuStateFlag             = StaticRef<uint32>(0xC980A4);
D3D9DisplayModeLayout& D3D9CurrentMode = *reinterpret_cast<D3D9DisplayModeLayout*>(0xC9BEE4);

// Profundidad asociada a un D3DFORMAT (tabla de selectores de 15 bytes + interruptor, repetida en 0x7F6CB0, 0x7F8300 y 0x7F8640): A8R8G8B8, X8R8G8B8 y A2R10G10B10 -> 32; R5G6B5, X1R5G5B5 y A1R5G5B5 -> 16; el resto -> 0.
uint32 D3D9FindDepth(uint32 format) {
    static constexpr uint8 kSelector[15] = { 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0 };
    const uint32 index = format - 0x15u;
    if (index > 0xEu) {
        return 0;
    }
    switch (kSelector[index]) {
    case 0:  return 32;
    case 1:  return 16;
    default: return 0;
    }
}

// Restaura la palabra de control x87 del EXE (0x85742B: fldcw [0xCAF004]).
void D3D9RestoreFpuControlWord() {
    ((void(__cdecl *)(void))0x85742B)();
}

// Comprobacion de un formato de profundidad: CheckDeviceFormat(usage 2 = DEPTHSTENCIL, tipo 1 = SURFACE) y CheckDepthStencilMatch con el formato del back buffer actual; ambas deben dar exactamente D3D_OK (0).
bool D3D9DepthFormatSupported(uint32 adapterFormat, uint32 depthFormat) {
    if (ComCall<kSlotD3DCheckDeviceFormat, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, adapterFormat, 2u, 1u, depthFormat) != 0) {
        return false;
    }
    return ComCall<kSlotD3DCheckDepthStencilMatch, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, adapterFormat, D3D9Present.backBufferFormat, depthFormat) == 0;
}

// Cadena comun tras un Reset correcto del dispositivo (0x7F8300, 0x7F8640, 0x7F8A90): GetRenderTarget(0) y GetDepthStencilSurface se anotan en 0xC97C30 / 0xC97C2C y se liberan (solo se queda el puntero), y
// despues, mientras cada paso lo permita: lista de rasters (0x4CC970), buferes dinamicos (0x7F58D0), 0x7FD100, luces (SetLight + LightEnable por entrada; un HRESULT negativo del ultimo LightEnable corta),
// 0x7FB480 y 0x80E020. Al final llama siempre al callback de restauracion (0xC980B0). Devuelve TRUE solo si todo se ejecuto y 0x80E020 dio distinto de cero (0x7F8300 y la rama de reserva ignoran el resultado).
bool D3D9RestoreAfterReset() {
    ComCall<kSlotDeviceGetRenderTarget, HRESULT>(D3D9Device, 0u, &D3D9RenderSurface);
    ComCall<kSlotRelease, ULONG>(D3D9RenderSurface);
    ComCall<kSlotDeviceGetDepthStencilSurface, HRESULT>(D3D9Device, &D3D9DepthStencilSurface);
    ComCall<kSlotRelease, ULONG>(D3D9DepthStencilSurface);
    bool ok = false;
    if (((RwBool(__cdecl *)(void))0x4CC970)() != 0 && _rwD3D9DynamicVertexBufferRestore() != 0) {
        ((void(__cdecl *)(void))0x7FD100)();
        bool lightsOk = true;
        if (D3D9LightCount > 0) {
            int32 index = 0;
            uint32 offset = 0;
            HRESULT last;
            do {
                ComCall<kSlotDeviceSetLight, HRESULT>(D3D9Device, static_cast<uint32>(index), static_cast<void*>(reinterpret_cast<uint8*>(D3D9LightCache) + offset));
                last = ComCall<kSlotDeviceLightEnable, HRESULT>(D3D9Device, static_cast<uint32>(index), *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(D3D9LightCache) + offset + 0x68));
                ++index;
                offset += 0x6C;
            } while (index < D3D9LightCount);
            lightsOk = last >= 0;
        }
        if (lightsOk && ((RwBool(__cdecl *)(void))0x7FB480)() != 0 && ((RwBool(__cdecl *)(void))0x80E020)() != 0) {
            ok = true;
        }
    }
    if (D3D9RestoreCallback != nullptr) {
        D3D9RestoreCallback();
    }
    return ok;
}

// Elige el nivel de multimuestreo mas alto que admite el dispositivo, bajando de uno en uno desde el elegido (0x7F84F0 y 0x7F8A90 comparten el bucle).
void D3D9SelectMultisamplingLevel() {
    const uint32 windowed = (~D3D9FullScreenFlag) & 1u;
    while (true) {
        const HRESULT result = ComCall<kSlotCheckDeviceMultiSampleType, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, D3D9AdapterFormat, windowed, D3D9SelectedMultisamplingLevels, static_cast<uint32*>(nullptr));
        if (result >= 0) {
            return;
        }
        const uint32 lowered = D3D9SelectedMultisamplingLevels - 1;
        D3D9SelectedMultisamplingLevels = lowered;
        if (lowered < 2) {
            return;
        }
    }
}
} // namespace

// 0x7F84E0 (lote R26A, sin validar; 6 bytes; cdecl sin argumentos): devuelve el maximo de multimuestreo (0x8E242C) calculado por _rwD3D9FindMSLevels.
RwUInt32 RwD3D9EngineGetMaxMultiSamplingLevels() {
    return D3D9MaxMultisamplingLevels;
}

// 0x7F84F0 (lote R26A, sin validar; 135 bytes; era una envoltura; cdecl, (niveles)): recorta el pedido al maximo (0x8E242C, sin signo), lo guarda como nivel elegido (0x8E2430) y, si es < 2, deja nivel elegido y no
// enmascarable (0x8E2438) en 1 y sale. Si no, baja el nivel de uno en uno (minimo 2) hasta que CheckDeviceMultiSampleType (+0x2C, calidad NULL, windowed = ~[0xC9BEF8] & 1, formato 0xC9BEF0) da un HRESULT no
// negativo y fija el no enmascarable = (maximo no enmascarable 0x8E2434 * pedido recortado) / maximo (division sin signo; el EXE usa imul + div con edx = 0).
void RwD3D9EngineSetMultiSamplingLevels(RwUInt32 numLevels) {
    const uint32 maximum = D3D9MaxMultisamplingLevels;
    uint32 level = numLevels;
    if (level > maximum) {
        level = maximum;
    }
    D3D9SelectedMultisamplingLevels = level;
    if (level < 2) {
        D3D9SelectedMultisamplingLevels = 1;
        D3D9SelectedMultisamplingLevelsNonMask = 1;
        return;
    }
    D3D9SelectMultisamplingLevel();
    D3D9SelectedMultisamplingLevelsNonMask = (D3D9MaxMultisamplingLevelsNonMask * level) / D3D9MaxMultisamplingLevels;
}

// 0x7F8580 (lote R26A, sin validar; 147 bytes; era una envoltura; cdecl, (frecuencia)): guarda la frecuencia pedida (0x8E243C). Si no es 0, para cada uno de los tres formatos de 0x884788 (R5G6B5, X8R8G8B8, A2R10G10B10)
// recorre los modos del adaptador (GetAdapterModeCount +0x18 y EnumAdapterModes +0x1C, del ultimo al primero) y se queda con la frecuencia exacta si aparece (y deja de mirar ese formato) o, si no, con la mayor
// menor que la pedida (sin signo). Si el resultado difiere de la pedida la sustituye (puede quedar en 0). El modo del bucle vive fuera de el, como en la pila del EXE.
void RwD3D9EngineSetRefreshRate(RwUInt32 refreshRate) {
    D3D9RefreshRateLimit = refreshRate;
    if (refreshRate == 0) {
        return;
    }
    const uint32 (&formats)[3] = *reinterpret_cast<const uint32(*)[3]>(0x884788);
    uint32 best = 0;
    D3D9DisplayModeLayout mode = {};
    for (int format = 0; format < 3; ++format) {
        uint32 remaining = ComCall<kSlotD3DGetAdapterModeCount, uint32>(D3D9DirectObject, D3D9AdapterIndex, formats[format]);
        if (remaining == 0) {
            continue;
        }
        do {
            --remaining;
            ComCall<kSlotD3DEnumAdapterModes, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, formats[format], remaining, &mode);
            const uint32 requested = D3D9RefreshRateLimit;
            if (mode.refreshRate == requested) {
                best = requested;
                break;
            }
            if (mode.refreshRate < requested && best < mode.refreshRate) {
                best = mode.refreshRate;
            }
        } while (remaining != 0);
    }
    if (D3D9RefreshRateLimit != best) {
        D3D9RefreshRateLimit = best;
    }
}

// 0x7F6CB0 (lote R26A, sin validar; 1075 bytes; cdecl, (modo, banderas, formato del adaptador); `D3D9SetPresentParameters` del SDK): rellena D3DPRESENT_PARAMETERS (0xC9C040). Con el bit 0 de `banderas` (solo el byte
// bajo; pantalla completa): adaptador = formato del modo, Windowed 0, BackBufferCount 1, frecuencia = min(modo.frecuencia, 0x8E243C) sin signo, intervalo 0x80000000 (IMMEDIATE), ancho y alto del modo y SwapEffect 2 (FLIP). Si no:
// Windowed 1, un back buffer, frecuencia 0, intervalo IMMEDIATE, GetWindowRect de la ventana (0xC97C1C) y ancho/alto = 1 si el rectangulo mide 0, si no 0, SwapEffect 3 (COPY). Formato del back buffer: A8R8G8B8 (0x15) si el
// adaptador es X8R8G8B8 (0x16) y CheckDeviceType (+0x24) lo admite (resultado no negativo), si no el del adaptador. EnableAutoDepthStencil 1. Profundidad (0xC9BEFC) = D3D9FindDepth; con 32: D24S8 (0x4B, con stencil), D24X4S4 (0x4F,
// con stencil), D32 (0x47), D24X8 (0x4D) y, si ninguno, 16; con 16: D15S1 (0x49, con stencil) o D16 (0x50). Multimuestreo: tipo y calidad a 0 y, si el nivel elegido (0x8E2430) o el no enmascarable (0x8E2438) pasan de 1: primero con el
// nivel elegido (calidad > 0 -> tipo = nivel, calidad - 1); si no, con tipo 1 (NONMASKABLE): calidad = min(no enmascarable - 2, calidad - 1), tipo 1 y SwapEffect 1. Con tipo != 0, SwapEffect 1 (DISCARD). No tiene el modo de dialogo
// de NOTSA que trae la copia de WindowedMode.cpp (esa no esta enganchada). El rectangulo de GetWindowRect se inicializa a 0 aqui (el EXE deja basura de pila si falla la llamada).
void _rwD3D9SetPresentParameters(const void* modePointer, RwUInt32 flags, RwUInt32 adapterFormatIn) {
    const D3D9DisplayModeLayout* const mode = static_cast<const D3D9DisplayModeLayout*>(modePointer);
    uint32 adapterFormat = adapterFormatIn;
    D3D9PresentLayout& present = D3D9Present;
    if ((flags & 1u) != 0) {
        adapterFormat = mode->format;
        const uint32 limit = D3D9RefreshRateLimit;
        present.windowed = 0;
        present.backBufferCount = 1;
        const uint32 rate = mode->refreshRate;
        present.fullScreenRefreshRateInHz = rate;
        if (rate >= limit) {
            present.fullScreenRefreshRateInHz = limit;
        }
        present.presentationInterval = 0x80000000u;
        present.backBufferWidth = mode->width;
        present.backBufferHeight = mode->height;
        present.swapEffect = 2;
    } else {
        RECT rect = {};
        present.windowed = 1;
        present.backBufferCount = 1;
        present.fullScreenRefreshRateInHz = 0;
        present.presentationInterval = 0x80000000u;
        GetWindowRect(static_cast<HWND>(D3D9MainWindow), &rect);
        present.backBufferWidth = (rect.right - rect.left == 0) ? 1u : 0u;
        present.backBufferHeight = (rect.bottom - rect.top == 0) ? 1u : 0u;
        present.swapEffect = 3;
    }
    uint32 backBufferFormat = adapterFormat;
    if (adapterFormat == 0x16) {
        const HRESULT result = ComCall<kSlotD3DCheckDeviceType, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, adapterFormat, 0x15u, static_cast<uint32>(present.windowed));
        backBufferFormat = (result < 0) ? 0x16u : 0x15u;
    }
    present.backBufferFormat = backBufferFormat;
    present.enableAutoDepthStencil = 1;
    D3D9ZBufferDepthValue = static_cast<int32>(D3D9FindDepth(present.backBufferFormat));
    if (D3D9ZBufferDepthValue == 32) {
        if (D3D9DepthFormatSupported(adapterFormat, 0x4B)) {
            present.autoDepthStencilFormat = 0x4B;
            D3D9HasStencilBuffer = 1;
        } else if (D3D9DepthFormatSupported(adapterFormat, 0x4F)) {
            present.autoDepthStencilFormat = 0x4F;
            D3D9HasStencilBuffer = 1;
        } else if (D3D9DepthFormatSupported(adapterFormat, 0x47)) {
            present.autoDepthStencilFormat = 0x47;
            D3D9HasStencilBuffer = 0;
        } else if (D3D9DepthFormatSupported(adapterFormat, 0x4D)) {
            present.autoDepthStencilFormat = 0x4D;
            D3D9HasStencilBuffer = 0;
        } else {
            D3D9ZBufferDepthValue = 16;
        }
    }
    if (D3D9ZBufferDepthValue == 16) {
        if (D3D9DepthFormatSupported(adapterFormat, 0x49)) {
            present.autoDepthStencilFormat = 0x49;
            D3D9HasStencilBuffer = 1;
        } else {
            present.autoDepthStencilFormat = 0x50;
            D3D9HasStencilBuffer = 0;
        }
    }
    present.multiSampleType = 0;
    present.multiSampleQuality = 0;
    if (D3D9SelectedMultisamplingLevels <= 1 && D3D9SelectedMultisamplingLevelsNonMask <= 1) {
        return;
    }
    uint32 quality = 0;
    HRESULT result = ComCall<kSlotCheckDeviceMultiSampleType, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, present.backBufferFormat, static_cast<uint32>(present.windowed), D3D9SelectedMultisamplingLevels, &quality);
    if (result >= 0 && quality > 0) {
        present.multiSampleType = D3D9SelectedMultisamplingLevels;
        present.multiSampleQuality = quality - 1;
    } else {
        quality = 0;
        result = ComCall<kSlotCheckDeviceMultiSampleType, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, present.backBufferFormat, static_cast<uint32>(present.windowed), 1u, &quality);
        if (result >= 0 && quality > 0) {
            uint32 nonMaskQuality = D3D9SelectedMultisamplingLevelsNonMask - 2;
            const uint32 maxQuality = quality - 1;
            if (nonMaskQuality > maxQuality) {
                nonMaskQuality = maxQuality;
            }
            present.multiSampleType = 1;
            present.swapEffect = 1;
            present.multiSampleQuality = nonMaskQuality;
            return;
        }
    }
    if (present.multiSampleType != 0) {
        present.swapEffect = 1;
    }
}

// 0x7F7100 (lote R26A, sin validar; 472 bytes; cdecl con tres argumentos que ignora; `D3D9DeviceSystemStop` del SDK; nombre inferido): parada del sistema del dispositivo D3D9. Pone 0xC97C38 a 0 y libera la tabla de modos (RwFree;
// 0xC97C48 y 0xC97C40 a 0). Restaura la palabra de control x87 (0x85742B) y, si 0xC980A4 != 0, limpia el bit 15 (FZ) de MXCSR. Devuelve a la lista libre de matrices (0xC98080) los 0x104 punteros de la tabla 0xC97C70 (releyendo
// la lista), la destruye (RwFreeListDestroy) y pone a 0 la lista y el numero de luces; libera la cache de luces (0xC98088). Desengancha del dispositivo: texturas 0..7, indices, flujos 0..3 (0, 0, 0), pixel shader, declaracion y
// vertex shader. Para cada registro de 12 bytes de 0xC97C60 hace Release del objeto hasta que devuelve 0 y libera el bloque (+8); libera la tabla y pone a 0 0xC97C58 y 0xC97C5C. Llama a 0x4CC170 (lista de rasters), al
// gestor de buferes de vertices (cierre), a 0x7FB5F0 y libera el dispositivo (Release; 0xC97C28 = NULL). Devuelve TRUE.
RwBool _rwD3D9DeviceSystemStop(void* /*out*/, void* /*inOut*/, RwInt32 /*in*/) {
    D3D9ModesReady = 0;
    if (D3D9VideoModes != nullptr) {
        RwFree(D3D9VideoModes);
        D3D9VideoModes = nullptr;
        D3D9VideoModeCount = 0;
    }
    D3D9RestoreFpuControlWord();
    if (D3D9FpuStateFlag != 0) {
        _mm_setcsr(_mm_getcsr() & 0xFFFF7FFFu);
    }
    for (int i = 0; i < 0x104; ++i) {
        if (D3D9TransformCache[i] != nullptr) {
            RwFreeListFree(D3D9TransformCacheFreeList, D3D9TransformCache[i]);
            D3D9TransformCache[i] = nullptr;
        }
    }
    RwFreeListDestroy(D3D9TransformCacheFreeList);
    D3D9LightEntry* const lights = D3D9LightCache;
    D3D9TransformCacheFreeList = nullptr;
    D3D9LightCount = 0;
    if (lights != nullptr) {
        RwFree(lights);
        D3D9LightCache = nullptr;
    }
    for (uint32 stage = 0; stage < 8; ++stage) {
        ComCall<kSlotDeviceSetTexture, HRESULT>(D3D9Device, stage, static_cast<void*>(nullptr));
    }
    ComCall<kSlotDeviceSetIndices, HRESULT>(D3D9Device, static_cast<void*>(nullptr));
    for (uint32 stream = 0; stream < 4; ++stream) {
        ComCall<kSlotDeviceSetStreamSource, HRESULT>(D3D9Device, stream, static_cast<void*>(nullptr), 0u, 0u);
    }
    ComCall<kSlotDeviceSetPixelShader, HRESULT>(D3D9Device, static_cast<void*>(nullptr));
    ComCall<kSlotDeviceSetVertexDeclaration, HRESULT>(D3D9Device, static_cast<void*>(nullptr));
    ComCall<kSlotDeviceSetVertexShader, HRESULT>(D3D9Device, static_cast<void*>(nullptr));
    if (D3D9ExtraRecordCount > 0) {
        uint32 offset = 0;
        uint32 index = 0;
        do {
            if (*reinterpret_cast<void**>(reinterpret_cast<uint8*>(D3D9ExtraRecords) + offset) != nullptr) {
                ULONG remaining;
                do {
                    remaining = ComCall<kSlotRelease, ULONG>(*reinterpret_cast<void**>(reinterpret_cast<uint8*>(D3D9ExtraRecords) + offset));
                } while (remaining > 0);
            }
            void* const block = *reinterpret_cast<void**>(reinterpret_cast<uint8*>(D3D9ExtraRecords) + offset + 8);
            if (block != nullptr) {
                RwFree(block);
            }
            ++index;
            offset += 0xC;
        } while (index < D3D9ExtraRecordCount);
    }
    if (D3D9ExtraRecords != nullptr) {
        RwFree(D3D9ExtraRecords);
        D3D9ExtraRecords = nullptr;
    }
    D3D9ExtraRecordCount = 0;
    D3D9ExtraRecordUnused = 0;
    ((void(__cdecl *)(void))0x4CC170)();
    _rwD3D9VertexBufferManagerClose();
    ((void(__cdecl *)(void))0x7FB5F0)();
    ComCall<kSlotRelease, ULONG>(D3D9Device);
    D3D9Device = nullptr;
    return TRUE;
}

// 0x7F72E0 (lote R26A, sin validar; 579 bytes; cdecl, (salida, inOut sin uso, numero de funciones); `D3D9DeviceSystemStandards` del SDK; nombre inferido): rellena la tabla de funciones estandar del dispositivo. Con numero > 0
// pone en las `numero` primeras entradas la funcion nula 0x7F7530 (xor eax, eax; ret) y, con numero > 1, escribe despues (del ultimo par al primero) tabla[indice] = funcion para 27 pares fijos, sin comprobar el limite. Las
// direcciones son del EXE (incluye 0x7F8F20 y 0x7F7730, enganchadas por WindowedMode.cpp, y 0x4Cxxxx del controlador de rasters). Devuelve TRUE.
RwBool _rwD3D9DeviceSystemStandards(void* out, void* /*inOut*/, RwInt32 numStandards) {
    struct StandardEntry { uint32 index; uint32 function; };
    static constexpr StandardEntry kStandards[27] = {
        { 0x01, 0x7F8F20 }, { 0x0A, 0x7F98D0 }, { 0x15, 0x7F7730 }, { 0x14, 0x7F99B0 }, { 0x02, 0x7FEE20 }, { 0x03, 0x7FF070 }, { 0x07, 0x8001E0 },
        { 0x06, 0x7FF270 }, { 0x05, 0x4CBB00 }, { 0x04, 0x4CCE60 }, { 0x09, 0x7FFF00 }, { 0x08, 0x4CBD40 }, { 0x0F, 0x4C9F90 }, { 0x10, 0x4CA290 },
        { 0x17, 0x4CA4E0 }, { 0x18, 0x4CA540 }, { 0x0E, 0x4CB4E0 }, { 0x0D, 0x4CB4C0 }, { 0x11, 0x4CAE40 }, { 0x12, 0x4CAE80 }, { 0x13, 0x4CAE60 },
        { 0x0B, 0x4CB520 }, { 0x0C, 0x4CBD50 }, { 0x19, 0x4CD360 }, { 0x1B, 0x4CD4D0 }, { 0x1A, 0x4CD820 }, { 0x1C, 0x4CBCB0 },
    };
    uint32* const standards = static_cast<uint32*>(out);
    if (numStandards > 0) {
        for (RwInt32 i = 0; i < numStandards; ++i) {
            standards[i] = 0x7F7530;
        }
    }
    for (int k = 26; k >= 0; --k) {
        if (numStandards > 1) {
            standards[kStandards[k].index] = kStandards[k].function;
        }
    }
    return TRUE;
}

// 0x7F7540 (lote R26A, sin validar; 459 bytes; cdecl sin argumentos; el EXE deja un valor en EAX que los tres llamadores (0x7F60E2, 0x7F6110, 0x7F65D5) no leen; nombre inferido): enumera los modos de video. Reserva (0xC97C48 ya
// existe: RwRealloc, si no RwMalloc; pista 0x1040411) (modos del adaptador 0xC9BEE0 + 1) * 0x14 bytes, guarda el modo actual del escritorio (GetAdapterDisplayMode +0x20) en la entrada 0 y, segun su formato, pone su bandera a 0 y
// el numero de modos a 1 (formatos 0x15..0x19 y 0x23) o el numero a 0 (0x1A..0x22 y fuera de rango). Para cada formato de 0x884788 (R5G6B5, X8R8G8B8, A2R10G10B10) y cada modo del adaptador
// (EnumAdapterModes +0x1C) lo escribe en la entrada [numero]; si ya hay una igual (ancho, alto y formato, entradas 1..numero-1; la 0 no se mira) conserva la mayor frecuencia (sin signo) y no cuenta, y si no pone la bandera
// a 1 y suma uno. Al final, si el numero (con signo) es menor que modos del adaptador + 1, reduce la tabla con RwRealloc.
void _rwD3D9EnumerateDisplayModes() {
    const uint32 allocBytes = static_cast<uint32>(D3D9AdapterModeCount) * 20u + 20u;
    D3D9VideoModeLayout* newModes;
    if (D3D9VideoModes != nullptr) {
        newModes = static_cast<D3D9VideoModeLayout*>(RwRealloc(D3D9VideoModes, allocBytes, 0x1040411));
    } else {
        newModes = static_cast<D3D9VideoModeLayout*>(RwMalloc(allocBytes, 0x1040411));
    }
    D3D9VideoModes = newModes;
    ComCall<kSlotD3DGetAdapterDisplayMode, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, static_cast<void*>(&D3D9VideoModes[0].mode));
    static constexpr uint8 kSelector[15] = { 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0 };
    const uint32 desktopIndex = D3D9VideoModes[0].mode.format - 0x15u;
    if (desktopIndex <= 0xEu && kSelector[desktopIndex] == 0) {
        D3D9VideoModes[0].flags = 0;
        D3D9VideoModeCount = 1;
    } else {
        D3D9VideoModeCount = 0;
    }
    const uint32 (&formats)[3] = *reinterpret_cast<const uint32(*)[3]>(0x884788);
    for (int format = 0; format < 3; ++format) {
        const int32 count = static_cast<int32>(ComCall<kSlotD3DGetAdapterModeCount, uint32>(D3D9DirectObject, D3D9AdapterIndex, formats[format]));
        if (count <= 0) {
            continue;
        }
        int32 mode = 0;
        do {
            int32 total = D3D9VideoModeCount;
            D3D9VideoModeLayout* const base = D3D9VideoModes;
            ComCall<kSlotD3DEnumAdapterModes, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, formats[format], static_cast<uint32>(mode), static_cast<void*>(&base[total].mode));
            total = D3D9VideoModeCount;
            D3D9VideoModeLayout* const modes = D3D9VideoModes;
            int32 found = -1;
            if (total > 1) {
                for (int32 candidate = 1; candidate < total; ++candidate) {
                    if (modes[total].mode.width == modes[candidate].mode.width && modes[total].mode.height == modes[candidate].mode.height && modes[total].mode.format == modes[candidate].mode.format) {
                        found = candidate;
                        break;
                    }
                }
            }
            if (found < 0) {
                modes[total].flags = 1;
                D3D9VideoModeCount = D3D9VideoModeCount + 1;
            } else if (modes[found].mode.refreshRate < modes[total].mode.refreshRate) {
                modes[found].mode.refreshRate = modes[total].mode.refreshRate;
            }
            ++mode;
        } while (mode < count);
    }
    const int32 finalCount = D3D9VideoModeCount;
    if (finalCount < D3D9AdapterModeCount + 1) {
        D3D9VideoModes = static_cast<D3D9VideoModeLayout*>(RwRealloc(D3D9VideoModes, static_cast<uint32>(finalCount) * 20u, 0x1040411));
    }
}

// 0x7F7F70 (lote R26A, sin validar; 488 bytes; era una envoltura en WindowedMode.cpp; cdecl sin argumentos; `D3D9DeviceReleaseVideoMemory` del SDK): prepara el dispositivo para un Reset. Reinicia el cache de estado (0x7F6B00) y el de
// matrices (0x7F6B40; ambos inlinados en el EXE, aqui llamadas directas), desengancha texturas 0..7, indices, flujos 0..3, pixel shader, declaracion y vertex shader, devuelve la superficie de color a la anotada (0xC97C30 si difiere de
// 0xC98090[0], anotandola ANTES de SetRenderTarget(0)), pone a NULL los objetivos 1..3 que no lo estaban, devuelve la superficie de profundidad (0xC97C2C si difiere de 0xC9808C, anotada antes de SetDepthStencilSurface) y llama
// a 0x7FB5F0, 0x80DFB0, _rwD3D9DynamicVertexBufferRelease (0x7F5840), 0x4CB640 (liberar los recursos de la lista de rasters) y RwFreeListPurgeAllFreeLists (0x801F90). Devuelve TRUE.
RwBool _rwD3D9DeviceReleaseVideoMemory() {
    _rwD3D9StateCacheReset();
    _rwD3D9TransformCacheReset();
    for (uint32 stage = 0; stage < 8; ++stage) {
        ComCall<kSlotDeviceSetTexture, HRESULT>(D3D9Device, stage, static_cast<void*>(nullptr));
    }
    ComCall<kSlotDeviceSetIndices, HRESULT>(D3D9Device, static_cast<void*>(nullptr));
    for (uint32 stream = 0; stream < 4; ++stream) {
        ComCall<kSlotDeviceSetStreamSource, HRESULT>(D3D9Device, stream, static_cast<void*>(nullptr), 0u, 0u);
    }
    ComCall<kSlotDeviceSetPixelShader, HRESULT>(D3D9Device, static_cast<void*>(nullptr));
    ComCall<kSlotDeviceSetVertexDeclaration, HRESULT>(D3D9Device, static_cast<void*>(nullptr));
    ComCall<kSlotDeviceSetVertexShader, HRESULT>(D3D9Device, static_cast<void*>(nullptr));
    void* const renderSurface = D3D9RenderSurface;
    if (D3D9CurrentRenderTargets[0] != renderSurface) {
        void* const device = D3D9Device;
        D3D9CurrentRenderTargets[0] = renderSurface;
        ComCall<kSlotSetRenderTarget, HRESULT>(device, 0u, renderSurface);
    }
    for (uint32 target = 1; target < 4; ++target) {
        if (D3D9CurrentRenderTargets[target] != nullptr) {
            void* const device = D3D9Device;
            D3D9CurrentRenderTargets[target] = nullptr;
            ComCall<kSlotSetRenderTarget, HRESULT>(device, target, static_cast<void*>(nullptr));
        }
    }
    void* const depthSurface = D3D9DepthStencilSurface;
    if (CurrentDepthStencilSurface != depthSurface) {
        void* const device = D3D9Device;
        CurrentDepthStencilSurface = depthSurface;
        ComCall<kSlotSetDepthStencilSurface, HRESULT>(device, depthSurface);
    }
    ((void(__cdecl *)(void))0x7FB5F0)();
    ((void(__cdecl *)(void))0x80DFB0)();
    _rwD3D9DynamicVertexBufferRelease();
    ((void(__cdecl *)(void))0x4CB640)();
    RwFreeListPurgeAllFreeLists();
    return TRUE;
}

// 0x7F8300 (lote R26A, sin validar; 443 bytes; cdecl sin argumentos; `D3D9RestoreDevice` del SDK; nombre inferido): comprueba y restaura el dispositivo. TestCooperativeLevel (+0x0C): si da DEVICENOTRESET (0x88760869) libera la memoria
// de video (0x7F7F70) y hace Reset(&0xC9C040) (+0x40); si da INVALIDCALL (0x8876086C) y la ventana esta en modo ventana (Windowed != 0) toma el modo del escritorio (GetAdapterDisplayMode), pone su formato en 0xC9BEF0 y en el back
// buffer, la profundidad (D3D9FindDepth) en 0xC9BEF4 y repite el Reset; sin ventana vuelve con 0xC97C54 = 0 y FALSE. Un HRESULT negativo en cualquier punto deja 0xC97C54 = 0 y devuelve FALSE. Tras un Reset correcto ejecuta
// D3D9RestoreAfterReset (el resultado no se mira). Despues: si ya estaba dentro de escena (0xC97C54 != 0) devuelve ese valor; si no, BeginScene (+0xA4): negativo devuelve 0xC97C54 (0) sin tocarlo, si no 0xC97C54 = 1 y TRUE.
RwBool _rwD3D9RestoreDevice() {
    HRESULT result = ComCall<kSlotDeviceTestCooperativeLevel, HRESULT>(D3D9Device);
    if (result == static_cast<HRESULT>(0x88760869)) {
        _rwD3D9DeviceReleaseVideoMemory();
        result = ComCall<kSlotDeviceReset, HRESULT>(D3D9Device, &D3D9Present);
        if (result == static_cast<HRESULT>(0x8876086C)) {
            if (D3D9Present.windowed == 0) {
                D3D9InsideScene = 0;
                return FALSE;
            }
            D3D9DisplayModeLayout desktop = {};
            ComCall<kSlotD3DGetAdapterDisplayMode, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, &desktop);
            D3D9AdapterFormat = desktop.format;
            D3D9Present.backBufferFormat = desktop.format;
            D3D9CurrentModeDepth = D3D9FindDepth(desktop.format);
            result = ComCall<kSlotDeviceReset, HRESULT>(D3D9Device, &D3D9Present);
        }
        if (result < 0) {
            D3D9InsideScene = 0;
            return FALSE;
        }
        D3D9RestoreAfterReset();
    }
    if (result < 0) {
        D3D9InsideScene = 0;
        return FALSE;
    }
    if (D3D9InsideScene != 0) {
        return D3D9InsideScene;
    }
    if (ComCall<kSlotDeviceBeginScene, HRESULT>(D3D9Device) < 0) {
        return D3D9InsideScene;
    }
    D3D9InsideScene = 1;
    return TRUE;
}

namespace {
// Ajusta la ventana principal al tamano pedido (0x7F8640, ramas de pantalla completa y ventana): rectangulo (0, 0, ancho, alto) -> AdjustWindowRectEx(estilo, menu?, estilo extendido) y SetWindowPos(NULL, izquierda, ABAJO, ancho,
// alto, banderas). El EXE pasa el borde inferior como Y; con SWP_NOMOVE (todas las banderas de las dos ramas lo llevan) se ignora.
void D3D9ResizeMainWindow(int32 width, int32 height, uint32 swpFlags) {
    HWND const window = static_cast<HWND>(D3D9MainWindow);
    RECT rect = { 0, 0, width, height };
    const LONG exStyle = GetWindowLongA(window, GWL_EXSTYLE);
    const BOOL hasMenu = GetMenu(window) != nullptr;
    const LONG style = GetWindowLongA(window, GWL_STYLE);
    AdjustWindowRectEx(&rect, static_cast<DWORD>(style), hasMenu, static_cast<DWORD>(exStyle));
    SetWindowPos(window, nullptr, rect.left, rect.bottom, rect.right - rect.left, rect.bottom - rect.top, swpFlags);
}
} // namespace

// 0x7F8640 (lote R26A, sin validar; 1068 bytes; era una envoltura; cdecl, (indice de modo); `RwD3D9ChangeVideoMode` del SDK): devuelve TRUE sin hacer nada si 0xC97C38 es 0, el indice (con signo) esta fuera de [0, 0xC97C40) o ya es
// el actual (0xC97C18). Si no: copia D3DPRESENT_PARAMETERS (0x38 bytes), libera la memoria de video (0x7F7F70), TestCooperativeLevel (resultado ignorado), _rwD3D9SetPresentParameters(modo, bandera del modo, 0xC97BD4) y Reset. Con
// Reset correcto ejecuta D3D9RestoreAfterReset; si todo salio bien guarda el modo (0xC97C18, 0xC9BEE4..0xC9BEF0, bandera en 0xC9BEF8, profundidad en 0xC9BEF4); en pantalla completa ajusta la ventana a (ancho, alto) del modo y,
// en ventana, pone el estilo 0x10CF0000, la ajusta al tamano del back buffer (SetWindowPos con 0x26) y lee el area cliente en 0xC9BEE4/0xC9BEE8; devuelve TRUE. Si el Reset fallo o la restauracion no fue completa: (libera la memoria de video si
// el Reset habia ido bien), restaura la copia, repite el Reset (negativo -> FALSE) y la restauracion (resultado ignorado) y devuelve TRUE sin cambiar de modo.
RwBool RwD3D9ChangeVideoMode(RwInt32 modeIndex) {
    if (D3D9ModesReady == 0 || modeIndex < 0 || modeIndex >= D3D9VideoModeCount || D3D9CurrentVideoMode == modeIndex) {
        return TRUE;
    }
    const D3D9PresentLayout saved = D3D9Present;
    _rwD3D9DeviceReleaseVideoMemory();
    ComCall<kSlotDeviceTestCooperativeLevel, HRESULT>(D3D9Device);
    {
        const D3D9VideoModeLayout* const selected = &D3D9VideoModes[modeIndex];
        _rwD3D9SetPresentParameters(selected, static_cast<RwUInt32>(selected->flags), D3D9BaseAdapterFormat);
    }
    HRESULT result = ComCall<kSlotDeviceReset, HRESULT>(D3D9Device, &D3D9Present);
    if (result >= 0) {
        if (D3D9RestoreAfterReset()) {
            const D3D9VideoModeLayout& chosen = D3D9VideoModes[modeIndex];
            D3D9CurrentVideoMode = modeIndex;
            D3D9CurrentMode.width = chosen.mode.width;
            D3D9CurrentMode.height = chosen.mode.height;
            D3D9CurrentMode.refreshRate = chosen.mode.refreshRate;
            D3D9CurrentMode.format = chosen.mode.format;
            D3D9FullScreenFlag = static_cast<uint32>(chosen.flags);
            D3D9CurrentModeDepth = D3D9FindDepth(chosen.mode.format);
            if (D3D9Present.windowed == 0) {
                D3D9ResizeMainWindow(static_cast<int32>(chosen.mode.width), static_cast<int32>(chosen.mode.height), 6u);
                return TRUE;
            }
            SetWindowLongA(static_cast<HWND>(D3D9MainWindow), GWL_STYLE, 0x10CF0000);
            D3D9ResizeMainWindow(static_cast<int32>(D3D9Present.backBufferWidth), static_cast<int32>(D3D9Present.backBufferHeight), 0x26u);
            RECT client = {};
            GetClientRect(static_cast<HWND>(D3D9MainWindow), &client);
            D3D9CurrentMode.width = static_cast<uint32>(client.right);
            D3D9CurrentMode.height = static_cast<uint32>(client.bottom);
            return TRUE;
        }
        _rwD3D9DeviceReleaseVideoMemory();
    }
    D3D9Present = saved;
    result = ComCall<kSlotDeviceReset, HRESULT>(D3D9Device, &D3D9Present);
    if (result < 0) {
        return FALSE;
    }
    D3D9RestoreAfterReset();
    return TRUE;
}

// 0x7F8A90 (lote R26A, sin validar; 731 bytes; era una envoltura; cdecl, (niveles); `RwD3D9ChangeMultiSamplingLevels` del SDK): TRUE sin hacer nada si 0xC97C38 es 0 o el nivel pedido ya es el elegido (0x8E2430). Si no: guarda
// el nivel elegido y el no enmascarable (0x8E2438), copia D3DPRESENT_PARAMETERS, recorta el pedido al maximo (sin signo), y calcula nivel elegido y no enmascarable como RwD3D9EngineSetMultiSamplingLevels (< 2 -> ambos 1). Libera la
// memoria de video (0x7F7F70), TestCooperativeLevel (ignorado), _rwD3D9SetPresentParameters(0xC9BEE4, 0xC9BEF8, 0xC97BD4) y Reset. Con Reset correcto y D3D9RestoreAfterReset completa devuelve TRUE. Si no (liberando antes la memoria
// de video cuando el Reset habia ido bien): restaura la copia de parametros y los dos niveles guardados, repite el Reset (negativo -> FALSE) y la restauracion (resultado ignorado) y devuelve TRUE.
RwBool RwD3D9ChangeMultiSamplingLevels(RwUInt32 numLevels) {
    if (D3D9ModesReady == 0) {
        return TRUE;
    }
    const uint32 previousLevel = D3D9SelectedMultisamplingLevels;
    if (previousLevel == numLevels) {
        return TRUE;
    }
    const uint32 previousNonMask = D3D9SelectedMultisamplingLevelsNonMask;
    const D3D9PresentLayout saved = D3D9Present;
    uint32 level = numLevels;
    const uint32 maximum = D3D9MaxMultisamplingLevels;
    if (level > maximum) {
        level = maximum;
    }
    D3D9SelectedMultisamplingLevels = level;
    if (level < 2) {
        D3D9SelectedMultisamplingLevels = 1;
        D3D9SelectedMultisamplingLevelsNonMask = 1;
    } else {
        D3D9SelectMultisamplingLevel();
        D3D9SelectedMultisamplingLevelsNonMask = (D3D9MaxMultisamplingLevelsNonMask * level) / D3D9MaxMultisamplingLevels;
    }
    _rwD3D9DeviceReleaseVideoMemory();
    ComCall<kSlotDeviceTestCooperativeLevel, HRESULT>(D3D9Device);
    _rwD3D9SetPresentParameters(&D3D9CurrentMode, D3D9FullScreenFlag, D3D9BaseAdapterFormat);
    HRESULT result = ComCall<kSlotDeviceReset, HRESULT>(D3D9Device, &D3D9Present);
    if (result >= 0) {
        if (D3D9RestoreAfterReset()) {
            return TRUE;
        }
        _rwD3D9DeviceReleaseVideoMemory();
    }
    D3D9Present = saved;
    D3D9SelectedMultisamplingLevels = previousLevel;
    D3D9SelectedMultisamplingLevelsNonMask = previousNonMask;
    result = ComCall<kSlotDeviceReset, HRESULT>(D3D9Device, &D3D9Present);
    if (result < 0) {
        return FALSE;
    }
    D3D9RestoreAfterReset();
    return TRUE;
}

// 0x7F8D70 (lote R26A, sin validar; 424 bytes; era una envoltura; cdecl, (camara, ventana); `RwD3D9CameraAttachWindow` del SDK): FALSE si el dispositivo no esta en modo ventana (Windowed == 0) o la ventana es la principal
// (0xC97C1C). GetWindowRect de la ventana; destruye (RwRasterDestroy 0x7FB020, anulando antes el campo) el frameBuffer (+0x60) y el zBuffer (+0x64) de la camara si existen. Con una copia de D3DPRESENT_PARAMETERS (ventana = la nueva,
// ancho y alto = 1 si el rectangulo mide 0 y si no 0, sin Z automatico) crea una cadena de intercambio (+0x34); un HRESULT distinto de 0 devuelve FALSE. Toma su back buffer (+0x14 de la cadena; 0, 0), GetDesc (+0x30) y crea un
// raster de camara (RwRasterCreate(ancho, alto, 0, 0x82)) con ancho, alto, profundidad (tabla 0xB4E7E0 + formato * 4 + 1) y formato RW (byte +3 de la entrada), cuya extension guarda superficie, alfa (byte +0 de la entrada), formato D3D,
// ventana y cadena, y lo deja como frameBuffer; despues un raster de Z-buffer (ancho, alto, 0, 1) como zBuffer (si se crean). Libera el back buffer (la cadena queda en la extension). Devuelve TRUE. El rectangulo se inicializa a 0 aqui.
static_assert(offsetof(RwCamera, frameBuffer) == 0x60 && offsetof(RwCamera, zBuffer) == 0x64 && offsetof(RwRaster, depth) == 0x14 && offsetof(RwRaster, cFormat) == 0x23);
RwBool RwD3D9CameraAttachWindow(void* cameraPointer, void* windowPointer) {
    if (D3D9Present.windowed == 0) {
        return FALSE;
    }
    if (D3D9MainWindow == windowPointer) {
        return FALSE;
    }
    RECT rect = {};
    GetWindowRect(static_cast<HWND>(windowPointer), &rect);
    RwCamera* const camera = static_cast<RwCamera*>(cameraPointer);
    if (camera->frameBuffer != nullptr) {
        RwRaster* const raster = camera->frameBuffer;
        camera->frameBuffer = nullptr;
        RwRasterDestroy(raster);
    }
    if (camera->zBuffer != nullptr) {
        RwRaster* const raster = camera->zBuffer;
        camera->zBuffer = nullptr;
        RwRasterDestroy(raster);
    }
    D3D9PresentLayout parameters = D3D9Present;
    parameters.deviceWindow = windowPointer;
    parameters.backBufferWidth = (rect.right - rect.left == 0) ? 1u : 0u;
    parameters.backBufferHeight = (rect.bottom - rect.top == 0) ? 1u : 0u;
    parameters.enableAutoDepthStencil = 0;
    void* swapChain = nullptr;
    if (ComCall<kSlotDeviceCreateAdditionalSwapChain, HRESULT>(D3D9Device, &parameters, &swapChain) != 0) {
        return FALSE;
    }
    void* backBuffer = nullptr;
    ComCall<kSlotSwapChainGetBackBuffer, HRESULT>(swapChain, 0u, 0u, &backBuffer);
    uint32 description[8] = {};
    ComCall<kSlotSurfaceGetDesc, HRESULT>(backBuffer, description);
    const D3D9FormatInfoLayout* const formatTable = reinterpret_cast<const D3D9FormatInfoLayout*>(0xB4E7E0);
    RwRaster* const frameBuffer = RwRasterCreate(static_cast<RwInt32>(description[6]), static_cast<RwInt32>(description[7]), 0, 0x82);
    if (frameBuffer != nullptr) {
        frameBuffer->width = static_cast<RwInt32>(description[6]);
        frameBuffer->height = static_cast<RwInt32>(description[7]);
        frameBuffer->depth = formatTable[description[0]].depth;
        frameBuffer->cFormat = static_cast<RwUInt8>(formatTable[description[0]].rwFormat >> 8);
        D3D9RasterExtLayout& extension = D3D9RasterExt(frameBuffer);
        extension.texture = backBuffer;
        extension.alpha = formatTable[description[0]].alpha;
        extension.d3dFormat = description[0];
        extension.window = windowPointer;
        extension.swapChain = swapChain;
        camera->frameBuffer = frameBuffer;
    }
    RwRaster* const zBuffer = RwRasterCreate(static_cast<RwInt32>(description[6]), static_cast<RwInt32>(description[7]), 0, 1);
    if (zBuffer != nullptr) {
        camera->zBuffer = zBuffer;
    }
    ComCall<kSlotRelease, ULONG>(backBuffer);
    return TRUE;
}

// Lote R26A (sin validar): controlador D3D9 de RenderWare: multimuestreo (0x7F84E0, 0x7F84F0, 0x7F8A90), frecuencia (0x7F8580), parametros de presentacion (0x7F6CB0), parada del sistema (0x7F7100), estandares (0x7F72E0),
// modos de video (0x7F7540), liberacion y restauracion del dispositivo (0x7F7F70, 0x7F8300), cambio de modo (0x7F8640) y camara con ventana (0x7F8D70).
void RwD3D9InjectHooksR26A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(_rwD3D9SetPresentParameters, 0x7F6CB0);
    RH_ScopedGlobalInstall(_rwD3D9DeviceSystemStop, 0x7F7100);
    RH_ScopedGlobalInstall(_rwD3D9DeviceSystemStandards, 0x7F72E0);
    RH_ScopedGlobalInstall(_rwD3D9EnumerateDisplayModes, 0x7F7540);
    RH_ScopedGlobalInstall(_rwD3D9DeviceReleaseVideoMemory, 0x7F7F70);
    RH_ScopedGlobalInstall(_rwD3D9RestoreDevice, 0x7F8300);
    RH_ScopedGlobalInstall(RwD3D9EngineGetMaxMultiSamplingLevels, 0x7F84E0);
    RH_ScopedGlobalInstall(RwD3D9EngineSetMultiSamplingLevels, 0x7F84F0);
    RH_ScopedGlobalInstall(RwD3D9EngineSetRefreshRate, 0x7F8580);
    RH_ScopedGlobalInstall(RwD3D9ChangeVideoMode, 0x7F8640);
    RH_ScopedGlobalInstall(RwD3D9ChangeMultiSamplingLevels, 0x7F8A90);
    RH_ScopedGlobalInstall(RwD3D9CameraAttachWindow, 0x7F8D70);
}

// Lote R27A (sin validar): controlador D3D9 de RenderWare (SDK de RwD3D9, 0x7F9C30..0x7FA860): soporte DXT, borrado de caches de estado, render target, declaracion de vertices, shaders, flujos, indices, dibujo, transformaciones y luces.
// 0x7F8F20 (_rwD3D9CameraBeginUpdate) NO se rehace: ya esta enganchada por WindowedMode.cpp (RH_ScopedGlobalInstall bajo NOTSA_WINDOWED_MODE).
namespace {
constexpr size_t kSlotDeviceSetTransformR27A         = 44;  // IDirect3DDevice9::SetTransform (+0xB0)
constexpr size_t kSlotDeviceDrawPrimitiveR27A        = 81;  // IDirect3DDevice9::DrawPrimitive (+0x144)
constexpr size_t kSlotDeviceDrawIndexedPrimitiveR27A = 82;  // IDirect3DDevice9::DrawIndexedPrimitive (+0x148)
void* const kD3D9CacheInvalid = reinterpret_cast<void*>(static_cast<uintptr_t>(0xFFFFFFFFu));
auto& D3D9LastFvf                 = StaticRef<void*>(0x8E2440);  // ultimo FVF enviado
auto& D3D9LastVertexDeclaration   = StaticRef<void*>(0x8E2444);
auto& D3D9LastVertexShader        = StaticRef<void*>(0x8E2448);
auto& D3D9LastPixelShader         = StaticRef<void*>(0x8E244C);
auto& D3D9LastIndexBuffer         = StaticRef<void*>(0x8E2450);
constexpr uintptr_t kD3D9IdentityMatrixAddress = 0x8847A0; // 64 bytes del EXE
uint32 (&D3D9WorldMatrixStaging)[16] = *reinterpret_cast<uint32(*)[16]>(0x8E24D0); // matriz D3D del mundo (las columnas w, +0xC/+0x1C/+0x2C/+0x3C, no se tocan)
static_assert(sizeof(RxD3D9VertexStream) == 0x10);

bool D3D9Matrices16Equal(const uint32* a, const uint32* b) {
    for (int i = 0; i < 16; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}
} // namespace

// 0x7F9C30 (lote R27A, sin validar; 242 bytes; era una envoltura; cdecl sin argumentos; `RwD3D9DeviceSupportsDXTTexture` del SDK): IDirect3D9::CheckDeviceFormat (+0x28) con (adaptador 0xC97C24, tipo de dispositivo
// 0x8E2428, formato del adaptador 0xC9BEF0, uso 0, D3DRTYPE_TEXTURE = 3, formato) para DXT1 ('DXT1' = 0x31545844) a DXT5 en ese orden; TRUE al primero cuyo HRESULT no es negativo, FALSE si fallan los cinco.
RwBool RwD3D9DeviceSupportsDXTTexture() {
    for (uint32 format = 0x31545844u; format <= 0x35545844u; format += 0x01000000u) {
        const HRESULT result = ComCall<kSlotD3DCheckDeviceFormat, HRESULT>(D3D9DirectObject, D3D9AdapterIndex, D3D9DeviceType, D3D9AdapterFormat, 0u, 3u, format);
        if (result >= 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// 0x7F9D30 (lote R27A, sin validar; 10 bytes; era una envoltura; cdecl, (valor)): 0xC97C44 = valor (el que devuelve RwD3D9GetStencilClear, 0x7F9D40).
void RwD3D9SetStencilClear(RwUInt32 stencilClear) {
    StaticRef<RwUInt32>(0xC97C44) = stencilClear;
}

// 0x7F9D50 (lote R27A, sin validar; 300 bytes; era una envoltura con el nombre RwD3D9GetCurrentD3DDevice, pero el cuerpo no es un getter; nombre inferido; cdecl sin argumentos): reinicia las caches del controlador y devuelve
// el dispositivo (0xC97C28). Superficies de render target (0xC98090..0xC9809C) y de profundidad (0xC9808C) = 0; ultimo FVF, declaracion, vertex shader, pixel shader e indices (0x8E2440..0x8E2450) = 0xFFFFFFFF; los cuatro flujos
// (0xC97BD8) = {0xFFFFFFFF, 0, 0} (= _rwD3D9StateCacheReset, 0x7F6B00, que el EXE inlina); cache de matrices igual que _rwD3D9TransformCacheReset (0x7F6B40, inlinada: contador a 0, devuelve los punteros de la tabla a la
// lista libre o crea la lista, reserva la matriz de mundo con la identidad); cache de luces: contador (0xC98084) = 0 y, si hay tabla (0xC98088), RwFree y la pone a NULL.
// Los ocho llamadores del EXE (0x53D91E, 0x7452B5, 0x7471A5, 0x747248, 0x74727A, 0x747DFE, 0x747E31, 0x748089) usan el valor devuelto como IDirect3DDevice9*.
void* _rwD3D9DeviceCachesReset() {
    for (int i = 0; i < 4; ++i) {
        D3D9CurrentRenderTargets[i] = nullptr;
    }
    CurrentDepthStencilSurface = nullptr;
    _rwD3D9StateCacheReset();
    _rwD3D9TransformCacheReset();
    D3D9LightEntry* const lights = D3D9LightCache;
    D3D9LightCount = 0;
    if (lights != nullptr) {
        RwFree(lights);
        D3D9LightCache = nullptr;
    }
    return D3D9Device;
}

// 0x7F9E90 (lote R27A, sin validar; 148 bytes; era una envoltura; cdecl, (indice, raster); `RwD3D9SetRenderTarget` del SDK): superficie = NULL; con raster, toma la extension D3D9 del raster PADRE (raster->parent + 0xB4E9E0) y
// la superficie del nivel 0 de su textura (GetSurfaceLevel, o GetCubeMapSurface con la cara del nibble alto si el nibble bajo del byte +9 no es cero; el HRESULT no se mira). Si la superficie es la ya anotada para ese indice
// (0xC98090 + indice * 4) el resultado es TRUE; si no la anota ANTES de llamar a _RwD3DDevice->SetRenderTarget(indice, superficie) (+0x94) y el resultado es HRESULT >= 0. Despues, con superficie, Release (+8; la referencia
// que dio Get*). Sin raster compara y envia NULL.
RwBool RwD3D9SetRenderTarget(RwUInt32 index, RwRaster* raster) {
    void* surface = nullptr;
    if (raster != nullptr) {
        const D3D9RasterExtLayout& extension = D3D9RasterExt(raster->parent);
        D3D9GetTextureSurface(extension.texture, extension.cubeFace, 0u, &surface);
    }
    void** const renderTargets = &D3D9CurrentRenderTargets[0];
    RwBool result;
    if (renderTargets[index] == surface) {
        result = TRUE;
    } else {
        void* const device = D3D9Device;
        renderTargets[index] = surface;
        result = ComCall<kSlotSetRenderTarget, HRESULT>(device, index, surface) >= 0;
    }
    if (surface != nullptr) {
        ComCall<kSlotRelease, ULONG>(surface);
    }
    return result;
}

// 0x7F9F70 (lote R27A, sin validar; 56 bytes; era una envoltura de DNSetVertexDeclaration; cdecl, (declaracion); `_rwD3D9SetVertexDeclaration` del SDK): si es la ultima enviada (0x8E2444) no hace nada. Si no llama a
// _RwD3DDevice->SetVertexDeclaration (+0x15C); la cache (0x8E2444) queda en 0xFFFFFFFF y, si el HRESULT no es negativo, pasa a ser la declaracion; el ultimo FVF (0x8E2440) queda siempre en 0xFFFFFFFF.
void _rwD3D9SetVertexDeclaration(void* vertexDeclaration) {
    if (D3D9LastVertexDeclaration == vertexDeclaration) {
        return;
    }
    const HRESULT result = ComCall<kSlotDeviceSetVertexDeclaration, HRESULT>(D3D9Device, vertexDeclaration);
    D3D9LastVertexDeclaration = kD3D9CacheInvalid;
    if (result >= 0) {
        D3D9LastVertexDeclaration = vertexDeclaration;
    }
    D3D9LastFvf = kD3D9CacheInvalid;
}

// 0x7F9FB0 (lote R27A, sin validar; 51 bytes; era una envoltura; cdecl, (shader); `_rwD3D9SetVertexShader` del SDK): como la declaracion pero con SetVertexShader (+0x170) y la cache 0x8E2448 (a 0xFFFFFFFF tras la llamada,
// y al shader si el HRESULT no es negativo); no toca el FVF.
void _rwD3D9SetVertexShader(void* shader) {
    if (D3D9LastVertexShader == shader) {
        return;
    }
    const HRESULT result = ComCall<kSlotDeviceSetVertexShader, HRESULT>(D3D9Device, shader);
    D3D9LastVertexShader = kD3D9CacheInvalid;
    if (result >= 0) {
        D3D9LastVertexShader = shader;
    }
}

// 0x7F9FF0 (lote R27A, sin validar; 51 bytes; era una envoltura; cdecl, (shader); `_rwD3D9SetPixelShader` del SDK): igual que el vertex shader con SetPixelShader (+0x1AC) y la cache 0x8E244C.
void _rwD3D9SetPixelShader(void* shader) {
    if (D3D9LastPixelShader == shader) {
        return;
    }
    const HRESULT result = ComCall<kSlotDeviceSetPixelShader, HRESULT>(D3D9Device, shader);
    D3D9LastPixelShader = kD3D9CacheInvalid;
    if (result >= 0) {
        D3D9LastPixelShader = shader;
    }
}

// 0x7FA030 (lote R27A, sin validar; 90 bytes; era una envoltura; cdecl, (flujo, buffer, desplazamiento, paso); `RwD3D9SetStreamSource` del SDK): cache de flujos en 0xC97BD8 + flujo * 0x10 {buffer, desplazamiento, paso}.
// Si los tres coinciden no hace nada; si no los anota y llama a _RwD3DDevice->SetStreamSource(flujo, buffer, desplazamiento, paso) (+0x190). El indice (sin signo) no se acota.
void RwD3D9SetStreamSource(RwUInt32 streamNumber, void* streamData, RwUInt32 offset, RwUInt32 stride) {
    D3D9LastStream& last = (&D3D9LastStreams[0])[streamNumber];
    const uint32 buffer = static_cast<uint32>(reinterpret_cast<uintptr_t>(streamData));
    if (last.vertexBuffer == buffer && last.offset == offset && last.stride == stride) {
        return;
    }
    last.vertexBuffer = buffer;
    last.offset = offset;
    last.stride = stride;
    ComCall<kSlotDeviceSetStreamSource, HRESULT>(D3D9Device, streamNumber, streamData, offset, stride);
}

// 0x7FA090 (lote R27A, sin validar; 298 bytes; era una envoltura de plugin::CallAndReturn; cdecl, (flujos, con desplazamientos); `_rwD3D9SetStreams` del SDK): recorre los 2 primeros flujos (elementos de 0x10 bytes:
// buffer +0, desplazamiento +4, paso +8) contra la cache 0xC97BD8. Con buffer: si cambia el buffer, el desplazamiento (sin desplazamientos se compara contra 0) o el paso, anota los tres (desplazamiento 0 si no se piden) y llama a
// SetStreamSource(i, buffer, desplazamiento, paso) (+0x190). Sin buffer: si la cache tenia buffer la pone a {0, 0, 0} y llama SetStreamSource(i, NULL, 0, 0).
void _rwD3D9SetStreams(const RxD3D9VertexStream* streams, RwBool useOffsets) {
    for (uint32 i = 0; i < 2; ++i) {
        D3D9LastStream& last = D3D9LastStreams[i];
        const uint32 buffer = static_cast<uint32>(reinterpret_cast<uintptr_t>(streams[i].vertexBuffer));
        if (buffer != 0) {
            const uint32 offset = useOffsets != 0 ? streams[i].offset : 0u;
            if (last.vertexBuffer != buffer || last.offset != offset || last.stride != streams[i].stride) {
                last.vertexBuffer = buffer;
                last.offset = offset;
                last.stride = streams[i].stride;
                ComCall<kSlotDeviceSetStreamSource, HRESULT>(D3D9Device, i, streams[i].vertexBuffer, offset, streams[i].stride);
            }
        } else if (last.vertexBuffer != 0) {
            last.vertexBuffer = 0;
            last.offset = 0;
            last.stride = 0;
            ComCall<kSlotDeviceSetStreamSource, HRESULT>(D3D9Device, i, static_cast<void*>(nullptr), 0u, 0u);
        }
    }
}

// 0x7FA1C0 (lote R27A, sin validar; 36 bytes; era una envoltura de DNSetIndices; cdecl, (buffer de indices); `_rwD3D9SetIndices` del SDK): si es el ultimo enviado (0x8E2450) no hace nada; si no lo anota ANTES de llamar a
// _RwD3DDevice->SetIndices (+0x1A0) (el HRESULT no se mira).
void _rwD3D9SetIndices(void* indexBuffer) {
    if (D3D9LastIndexBuffer == indexBuffer) {
        return;
    }
    void* const device = D3D9Device;
    D3D9LastIndexBuffer = indexBuffer;
    ComCall<kSlotDeviceSetIndices, HRESULT>(device, indexBuffer);
}

// 0x7FA320 (lote R27A, sin validar; 50 bytes; era una envoltura; cdecl, 6 argumentos; `_rwD3D9DrawIndexedPrimitive` del SDK): vuelca los estados pendientes (_rwD3D9RenderStateFlushCache, 0x7FC200; directa a C++) y llama a
// _RwD3DDevice->DrawIndexedPrimitive(tipo, indice base, indice minimo, numero de vertices, indice inicial, primitivas) (+0x148). El HRESULT no se lee.
void _rwD3D9DrawIndexedPrimitive(RwUInt32 primitiveType, RwInt32 baseVertexIndex, RwUInt32 minIndex, RwUInt32 numVertices, RwUInt32 startIndex, RwUInt32 primitiveCount) {
    _rwD3D9RenderStateFlushCache();
    ComCall<kSlotDeviceDrawIndexedPrimitiveR27A, HRESULT>(D3D9Device, primitiveType, baseVertexIndex, minIndex, numVertices, startIndex, primitiveCount);
}

// 0x7FA360 (lote R27A, sin validar; 35 bytes; no tenia envoltura; cdecl, (tipo, vertice inicial, primitivas); `_rwD3D9DrawPrimitive` del SDK): vuelca los estados pendientes (0x7FC200, directa a C++) y llama a
// _RwD3DDevice->DrawPrimitive(tipo, inicio, primitivas) (+0x144). El HRESULT no se lee.
void _rwD3D9DrawPrimitive(RwUInt32 primitiveType, RwUInt32 startVertex, RwUInt32 primitiveCount) {
    _rwD3D9RenderStateFlushCache();
    ComCall<kSlotDeviceDrawPrimitiveR27A, HRESULT>(D3D9Device, primitiveType, startVertex, primitiveCount);
}

// 0x7FA390 (lote R27A, sin validar; 344 bytes; era una envoltura; cdecl, (estado, matriz*); `RwD3D9SetTransform` del SDK): cache de matrices 0xC97C70[estado] (puntero a 16 dwords; el indice no se acota). Con matriz: si hay entrada
// y es igual (dword a dword) devuelve TRUE sin llamar; si no la crea (RwFreeListAlloc de 0xC98080, pista 0x30411) si faltaba, copia 64 bytes y llama a SetTransform(estado, matriz ORIGINAL) (+0xB0); con estado 0x100 (mundo) pone
// el contador 0xC97C68 a 0; devuelve HRESULT >= 0. Sin matriz: lo mismo con la identidad del EXE (0x8847A0; el puntero que recibe el dispositivo es esa direccion), con estado 0x100 deja 0xC97C68 en 1, y una entrada
// ya igual a la identidad devuelve TRUE.
RwBool RwD3D9SetTransform(RwUInt32 state, const void* matrix) {
    const uint32* const identity = reinterpret_cast<const uint32*>(kD3D9IdentityMatrixAddress);
    const uint32* const source = matrix != nullptr ? static_cast<const uint32*>(matrix) : identity;
    uint32* const existing = static_cast<uint32*>(D3D9TransformCache[state]);
    if (existing != nullptr && D3D9Matrices16Equal(existing, source)) {
        return TRUE;
    }
    if (existing == nullptr) {
        D3D9TransformCache[state] = RwFreeListAlloc(D3D9TransformCacheFreeList, 0x30411);
    }
    memcpy(D3D9TransformCache[state], source, 0x40);
    const HRESULT result = ComCall<kSlotDeviceSetTransformR27A, HRESULT>(D3D9Device, state, static_cast<const void*>(source));
    if (state == 0x100) {
        D3D9TransformCacheCount = matrix != nullptr ? 0u : 1u;
    }
    return result >= 0;
}

// 0x7FA4F0 (lote R27A, sin validar; 42 bytes; era una envoltura; cdecl, (estado, salida*); `RwD3D9GetTransform` del SDK): con salida distinta de NULL copia 64 bytes de la entrada 0xC97C70[estado] o, si es NULL, de la
// identidad (0x8847A0). Sin salida no hace nada.
void RwD3D9GetTransform(RwUInt32 state, void* matrix) {
    if (matrix == nullptr) {
        return;
    }
    const void* source = D3D9TransformCache[state];
    if (source == nullptr) {
        source = reinterpret_cast<const void*>(kD3D9IdentityMatrixAddress);
    }
    memcpy(matrix, source, 0x40);
}

// 0x7FA520 (lote R27A, sin validar; 320 bytes; era una envoltura; cdecl, (RwMatrix*); `RwD3D9SetTransformWorld` del SDK): con matriz que NO tenga rwMATRIXINTERNALIDENTITY (0x20000 en +0xC) copia a la matriz D3D de trabajo
// 0x8E24D0 las filas right (+0), up (+0x10), at (+0x20) y pos (+0x30), tres dwords cada una (las columnas w de la matriz de trabajo no se tocan). Si el contador 0xC97C68 es 0 y la cache de mundo (0xC98070, 16 dwords)
// es igual a la de trabajo devuelve TRUE; si no copia 64 bytes a la cache, llama a SetTransform(0x100, 0x8E24D0) (+0xB0), pone 0xC97C68 = 0 y devuelve HRESULT >= 0. Sin matriz o con la bandera de identidad:
// si 0xC97C68 != 0 devuelve TRUE; si no copia la identidad (0x8847A0) a la cache, SetTransform(0x100, 0x8847A0), 0xC97C68 = 1 y HRESULT >= 0.
RwBool RwD3D9SetTransformWorld(const RwMatrix* matrix) {
    if (matrix != nullptr && (static_cast<uint32>(matrix->flags) & 0x20000u) == 0) {
        const uint32* const m = reinterpret_cast<const uint32*>(matrix);
        for (int row = 0; row < 4; ++row) {
            for (int column = 0; column < 3; ++column) {
                D3D9WorldMatrixStaging[row * 4 + column] = m[row * 4 + column];
            }
        }
        uint32* const cache = static_cast<uint32*>(D3D9TransformCacheMatrix);
        if (D3D9TransformCacheCount == 0 && D3D9Matrices16Equal(cache, D3D9WorldMatrixStaging)) {
            return TRUE;
        }
        memcpy(cache, D3D9WorldMatrixStaging, 0x40);
        const HRESULT result = ComCall<kSlotDeviceSetTransformR27A, HRESULT>(D3D9Device, 0x100u, static_cast<const void*>(D3D9WorldMatrixStaging));
        D3D9TransformCacheCount = 0;
        return result >= 0;
    }
    if (D3D9TransformCacheCount != 0) {
        return TRUE;
    }
    memcpy(D3D9TransformCacheMatrix, reinterpret_cast<const void*>(kD3D9IdentityMatrixAddress), 0x40);
    const HRESULT result = ComCall<kSlotDeviceSetTransformR27A, HRESULT>(D3D9Device, 0x100u, reinterpret_cast<const void*>(kD3D9IdentityMatrixAddress));
    D3D9TransformCacheCount = 1;
    return result >= 0;
}

// 0x7FA660 (lote R27A, sin validar; 236 bytes; era una envoltura; cdecl, (indice, luz*); `RwD3D9SetLight` del SDK): cache de luces 0xC98088 de 0x6C bytes (D3DLIGHT9 de 0x68 + bandera de activa en +0x68), contador 0xC98084.
// Con indice >= contador (con signo): contador = indice + 1, tabla = RwRealloc(tabla, (indice + 1) * 0x6C, 0x1030411) o RwMalloc si no habia, y la bandera de las entradas nuevas (contador anterior .. nuevo) = 0 (releyendo
// tabla y contador en cada vuelta). Con indice < contador: si la entrada es igual a la luz (_rwD3D9LightsEqual, 0x7FA8B0; cdecl (luz, entrada), no cero = igual) devuelve TRUE sin llamar. Despues copia 0x68 bytes a la entrada y llama a
// SetLight(indice, luz ORIGINAL) (+0xCC); devuelve HRESULT >= 0.
RwBool _rwD3D9LightsEqual(const void* light, const void* entry); // 0x7FA8B0 (lote R28A; definida al final del archivo)
RwBool RwD3D9SetLight(RwInt32 index, const void* light) {
    const RwInt32 previousCount = D3D9LightCount;
    if (index >= previousCount) {
        D3D9LightCount = index + 1;
        const uint32 size = static_cast<uint32>(index + 1) * 0x6Cu;
        if (D3D9LightCache != nullptr) {
            D3D9LightCache = static_cast<D3D9LightEntry*>(RwRealloc(D3D9LightCache, size, 0x1030411));
        } else {
            D3D9LightCache = static_cast<D3D9LightEntry*>(RwMalloc(size, 0x1030411));
        }
        if (previousCount < D3D9LightCount) {
            RwInt32 i = previousCount;
            do {
                reinterpret_cast<D3D9LightEntry*>(reinterpret_cast<uint8*>(D3D9LightCache) + static_cast<uint32>(i) * 0x6Cu)->enabled = 0;
                ++i;
            } while (i < D3D9LightCount);
        }
    } else {
        auto* const existing = reinterpret_cast<D3D9LightEntry*>(reinterpret_cast<uint8*>(D3D9LightCache) + static_cast<uint32>(index) * 0x6Cu);
        if (_rwD3D9LightsEqual(light, existing) != 0) {
            return TRUE;
        }
    }
    auto* const entry = reinterpret_cast<D3D9LightEntry*>(reinterpret_cast<uint8*>(D3D9LightCache) + static_cast<uint32>(index) * 0x6Cu);
    memcpy(entry, light, 0x68);
    return ComCall<kSlotDeviceSetLight, HRESULT>(D3D9Device, static_cast<uint32>(index), light) >= 0;
}

// 0x7FA860 (lote R27A, sin validar; 78 bytes; era una envoltura; cdecl, (indice, activar); `RwD3D9EnableLight` del SDK): con indice >= contador de luces (0xC98084, con signo) devuelve TRUE sin hacer nada. Si la bandera
// de la entrada (+0x68, dword) ya vale `activar` devuelve TRUE; si no la anota ANTES de llamar a LightEnable(indice, activar) (+0xD4) y devuelve HRESULT >= 0.
RwBool RwD3D9EnableLight(RwInt32 index, RwBool enable) {
    if (index >= D3D9LightCount) {
        return TRUE;
    }
    auto* const entry = reinterpret_cast<D3D9LightEntry*>(reinterpret_cast<uint8*>(D3D9LightCache) + static_cast<uint32>(index) * 0x6Cu);
    if (entry->enabled == enable) {
        return TRUE;
    }
    entry->enabled = enable;
    return ComCall<kSlotDeviceLightEnable, HRESULT>(D3D9Device, static_cast<uint32>(index), enable) >= 0;
}

// Lote R27A (sin validar): ganchos de las funciones del controlador D3D9 de 0x7F9C30 a 0x7FA860.
void RwD3D9InjectHooksR27A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(RwD3D9DeviceSupportsDXTTexture, 0x7F9C30);
    RH_ScopedGlobalInstall(RwD3D9SetStencilClear, 0x7F9D30);
    RH_ScopedGlobalInstall(_rwD3D9DeviceCachesReset, 0x7F9D50);
    RH_ScopedGlobalInstall(RwD3D9SetRenderTarget, 0x7F9E90);
    RH_ScopedGlobalInstall(_rwD3D9SetVertexDeclaration, 0x7F9F70);
    RH_ScopedGlobalInstall(_rwD3D9SetVertexShader, 0x7F9FB0);
    RH_ScopedGlobalInstall(_rwD3D9SetPixelShader, 0x7F9FF0);
    RH_ScopedGlobalInstall(RwD3D9SetStreamSource, 0x7FA030);
    RH_ScopedGlobalInstall(_rwD3D9SetStreams, 0x7FA090);
    RH_ScopedGlobalInstall(_rwD3D9SetIndices, 0x7FA1C0);
    RH_ScopedGlobalInstall(_rwD3D9DrawIndexedPrimitive, 0x7FA320);
    RH_ScopedGlobalInstall(_rwD3D9DrawPrimitive, 0x7FA360);
    RH_ScopedGlobalInstall(RwD3D9SetTransform, 0x7FA390);
    RH_ScopedGlobalInstall(RwD3D9GetTransform, 0x7FA4F0);
    RH_ScopedGlobalInstall(RwD3D9SetTransformWorld, 0x7FA520);
    RH_ScopedGlobalInstall(RwD3D9SetLight, 0x7FA660);
    RH_ScopedGlobalInstall(RwD3D9EnableLight, 0x7FA860);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R28A (sin validar): 0x7FA8B0..0x7FFFF0 atribuido por vecindad a RwCore con confianza mixta. La clase real es doble: las diez funciones de rasters (0x7FAEC0..0x7FB2D0) son de RwCore (baraster.c: llaman a la tabla
// de funciones estandar del dispositivo, RwEngineInstance->stdFunc) y el resto son del controlador D3D9 de RenderWare (RwD3D9): comparacion de luces, frustum, borrado de declaraciones, buferes y declaracion de Im2D,
// cache de estados de render / etapa / muestreo, material de superficie, reinicio de los estados, RwD3D9SetTexture y busqueda del formato de raster de una imagen.
// 0x7FAC10/0x7FAD30/0x7FAE20/0x7FB160/0x7FB480/0x7FC3C0/0x7FD100 y las demas marcadas "env" en el listado eran envolturas o llamadas por direccion fija: sustituidas por el cuerpo.
// Estado del EXE que usan (direcciones):
//   0xC991D0 + 8 * id  estados de render pedidos {valor, bandera "en lista"}   0xC98E88 + 4 * id  valor actual enviado   0xC98B40 lista de pendientes y 0xC9A5EC su contador
//   0xC980F8 + 8 * (etapa * 33 + tipo)  estados de etapa de textura pedidos {valor, bandera}   0xC99860 actuales   0xC99C80 lista {etapa, tipo} y 0xC9A5F0 su contador
//   0xC98938 + 4 * (etapa * 14 + tipo)  estados de muestreo actuales   0xC98AF8 material actual (17 dwords)   0xC9A508 + 0x18 * etapa  cache de la etapa (textura, direccion U, V, filtro, color de borde, anisotropia)
//   0xC980E0..0xC980F4  Im2D: bufer dinamico creado (E0), desplazamiento (E4), bufer de vertices (E8), desplazamiento de indices (EC), bufer de indices (F0), declaracion (F4)
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
constexpr size_t kSlotDeviceGetAvailableTextureMem = 4;   // IDirect3DDevice9::GetAvailableTextureMem (+0x10)
constexpr size_t kSlotDeviceCreateIndexBufferR28A  = 27;  // IDirect3DDevice9::CreateIndexBuffer (+0x6C)

struct D3D9RenderStateSlot { uint32 value; uint32 pending; };            // 0xC991D0 + 8 * id
struct D3D9StageStateSlot  { uint32 value; uint32 pending; };            // 0xC980F8 + 8 * (etapa * 33 + tipo)
struct D3D9TextureStageCache {                                           // 0xC9A508 + 0x18 * etapa
    uint32 texture;       // +0x00 raster actual (RwD3D9SetTexture / _rwD3D9RWSetRasterStage)
    uint32 addressU;      // +0x04
    uint32 addressV;      // +0x08
    uint32 filter;        // +0x0C
    uint32 borderColor;   // +0x10
    uint32 anisotropy;    // +0x14
};
static_assert(sizeof(D3D9RenderStateSlot) == 8 && sizeof(D3D9TextureStageCache) == 0x18);

D3D9RenderStateSlot* const   D3D9RenderStateWanted      = reinterpret_cast<D3D9RenderStateSlot*>(0xC991D0);
uint32* const                D3D9RenderStateCurrent     = reinterpret_cast<uint32*>(0xC98E88);
D3D9StageStateSlot* const    D3D9StageStateWanted       = reinterpret_cast<D3D9StageStateSlot*>(0xC980F8);
uint32* const                D3D9StageStateCurrent      = reinterpret_cast<uint32*>(0xC99860);
uint32* const                D3D9SamplerStateCurrent    = reinterpret_cast<uint32*>(0xC98938);
uint32* const                D3D9StageStatePendingList  = reinterpret_cast<uint32*>(0xC99C80);
D3D9TextureStageCache* const D3D9TextureStageCaches     = reinterpret_cast<D3D9TextureStageCache*>(0xC9A508);
uint32* const                D3D9MaterialStaging        = reinterpret_cast<uint32*>(0x8E2538);   // D3DMATERIAL9 de trabajo (17 dwords)
auto& D3D9RenderStatePendingCount   = StaticRef<uint32>(0xC9A5EC);
auto& D3D9StageStatePendingCount    = StaticRef<uint32>(0xC9A5F0);
auto& D3D9RasterCaps                = StaticRef<uint32>(0xC9BF24);   // D3DCAPS9::RasterCaps (+0x24)
auto& D3D9TextureFilterCaps         = StaticRef<uint32>(0xC9BF40);   // D3DCAPS9::TextureFilterCaps (+0x40)
auto& D3D9ImageDisplayDepth         = StaticRef<int32>(0xC9BEF4);    // _RwD3D9AdapterInformation.displayDepth
// Tablas del EXE (dwords): 0x88493C direccion RW -> D3DTADDRESS, 0x884950 filtro RW -> (min/mag, mip) de 8 bytes, 0x88490C mezcla RW -> D3DBLEND, 0x8848F0 sombreado, 0x8848FC niebla, 0x884990 cull,
// 0x8849A0 operacion de stencil, 0x8849C4 funcion de comparacion.
const uint32* const kTableAddressMode = reinterpret_cast<const uint32*>(0x88493C);
const uint32* const kTableFilterMode  = reinterpret_cast<const uint32*>(0x884950);
const uint32* const kTableBlend       = reinterpret_cast<const uint32*>(0x88490C);
const uint32* const kTableShadeMode   = reinterpret_cast<const uint32*>(0x8848F0);
const uint32* const kTableFogMode     = reinterpret_cast<const uint32*>(0x8848FC);
const uint32* const kTableCullMode    = reinterpret_cast<const uint32*>(0x884990);
const uint32* const kTableStencilOp   = reinterpret_cast<const uint32*>(0x8849A0);
const uint32* const kTableCompare     = reinterpret_cast<const uint32*>(0x8849C4);

RwFreeList*& RasterFreeListR28A() { // RwRasterGlobals + 0x60
    return *reinterpret_cast<RwFreeList**>(reinterpret_cast<uint8*>(&RasterGlobals()) + 0x60);
}

int32 D3D9CheckTextureFormatByAddress(int32 format) { // 0x4CBDE0 (cdecl, formato): 0/1
    return reinterpret_cast<int32(__cdecl*)(int32)>(0x4CBDE0)(format);
}
} // namespace

// 0x7FA8B0 (lote R28A, sin validar; 244 bytes; el listado la daba como "A, exacta"; nombre inferido, `_rwD3D9LightsEqual` de RenderWare; cdecl, (luz, entrada) -> 1 si iguales / 0 si no): compara dos D3DLIGHT9 dword a dword
// (sin x87: un NaN o un -0 distinto cuenta como diferencia). Siempre el tipo (+0) y los tres primeros componentes del color difuso (+4, +8, +0xC). Despues, segun el tipo: 1 (punto) posicion (+0x34, +0x38, +0x3C) y rango
// (+0x4C); 2 (foco) posicion, direccion (+0x40, +0x44, +0x48), rango, theta (+0x60) y phi (+0x64); 3 (direccional) direccion; otro tipo: nada mas.
RwBool _rwD3D9LightsEqual(const void* light, const void* entry) {
    const uint32* const a = static_cast<const uint32*>(light);
    const uint32* const b = static_cast<const uint32*>(entry);
    if (a[0] != b[0] || a[1] != b[1] || a[2] != b[2] || a[3] != b[3]) {
        return FALSE;
    }
    static constexpr uint32 kPointOffsets[]       = { 0x34, 0x38, 0x3C, 0x4C };
    static constexpr uint32 kSpotOffsets[]        = { 0x34, 0x38, 0x3C, 0x40, 0x44, 0x48, 0x4C, 0x60, 0x64 };
    static constexpr uint32 kDirectionalOffsets[] = { 0x40, 0x44, 0x48 };
    const uint32* offsets = nullptr;
    size_t count = 0;
    switch (a[0]) {
    case 1: offsets = kPointOffsets;       count = std::size(kPointOffsets);       break;
    case 2: offsets = kSpotOffsets;        count = std::size(kSpotOffsets);        break;
    case 3: offsets = kDirectionalOffsets; count = std::size(kDirectionalOffsets); break;
    default: break;
    }
    for (size_t i = 0; i < count; ++i) {
        if (a[offsets[i] / 4] != b[offsets[i] / 4]) {
            return FALSE;
        }
    }
    return TRUE;
}

// 0x7FAC10 (lote R28A, sin validar; 76 bytes; era una envoltura declarada sin argumento; cdecl, (declaracion); `RwD3D9DeleteVertexDeclaration` del SDK): busca la declaracion en la cache de declaraciones (0xC97C60, entradas
// de 12 bytes, 0xC97C58 entradas; la comparacion es por puntero) y si la encuentra llama a IDirect3DVertexDeclaration9::Release (+8) con el puntero guardado en la entrada; si la cuenta de referencias resultante es 0 (comparacion
// sin signo, "ja" en el EXE) anula la entrada (declaracion = NULL, releyendo la direccion de la tabla) para que RwD3D9CreateVertexDeclaration la vuelva a crear. Sin entrada igual no hace nada.
void RwD3D9DeleteVertexDeclaration(void* vertexdeclaration) {
    const uint32 count = D3D9VertexDeclarationCount;
    if (count == 0) {
        return;
    }
    D3D9VertexDeclarationCacheEntry* const entries = D3D9VertexDeclarationEntries;
    uint32 i = 0;
    while (entries[i].declaration != vertexdeclaration) {
        if (++i >= count) {
            return;
        }
    }
    void* const stored = entries[i].declaration;
    if (ComCall<kSlotRelease, ULONG>(stored) == 0) {
        D3D9VertexDeclarationEntries[i].declaration = nullptr;
    }
}

// 0x7FAD30 (lote R28A, sin validar; 82 bytes; era una envoltura; cdecl, (camara, esfera) -> RwBool; `RwD3D9CameraIsSphereFullyInsideFrustum` del SDK): para cada uno de los 6 planos del frustum de la camara (+0x94, 0x14 bytes:
// normal, distancia) calcula d = ((ny * cy + nx * cx) + nz * cz) - distancia (x87 a 24 bits en el EXE: cada producto y suma se redondea a float; aqui float en el mismo orden) y, con la esfera {cx, cy, cz, radio}, devuelve
// FALSE si d > -radio (comparacion ordenada: con NaN no se cumple y sigue). Con los seis planos superados, TRUE. El EXE guarda -radio en el hueco del argumento de la camara (sin efecto).
RwBool RwD3D9CameraIsSphereFullyInsideFrustum(const void* camera, const void* sphere) {
    const RwFrustumPlane* plane = static_cast<const RwCamera*>(camera)->frustumPlanes;
    const float* const center = static_cast<const float*>(sphere);
    const float negativeRadius = -center[3];
    for (int i = 0; i < 6; ++i, ++plane) {
        const float yTerm = plane->plane.normal.y * center[1];
        const float xTerm = plane->plane.normal.x * center[0];
        const float sumXY = yTerm + xTerm;
        const float zTerm = plane->plane.normal.z * center[2];
        const float sum = sumXY + zTerm;
        const float distance = sum - plane->plane.distance;
        if (distance > negativeRadius) {
            return FALSE;
        }
    }
    return TRUE;
}

// 0x7FAE20 (lote R28A, sin validar; 10 bytes; era una envoltura; cdecl, (funcion)): guarda en 0xC980B0 la funcion de restauracion del dispositivo (la que devuelve _rwD3D9DeviceGetRestoreCallback, 0x7FAE30).
void _rwD3D9DeviceSetRestoreCallback(rwD3D9DeviceRestoreCallBack callback) {
    StaticRef<rwD3D9DeviceRestoreCallBack>(0xC980B0) = callback;
}

// 0x7FAEC0 (lote R28A, sin validar; 28 bytes; era una envoltura; cdecl, (raster); `RwRasterUnlock` del SDK): stdFunc[rwSTANDARDRASTERUNLOCK] (RwEngineInstance + 0x88) con (NULL, raster, 0); devuelve el raster.
RwRaster* RwRasterUnlock(RwRaster* raster) {
    RwEngineInstance->stdFunc[rwSTANDARDRASTERUNLOCK](nullptr, raster, 0);
    return raster;
}

// 0x7FAF50 (lote R28A, sin validar; 53 bytes; era una envoltura; cdecl, (raster, x, y); `RwRasterRenderFast` del SDK): stdFunc[rwSTANDARDRASTERRENDERFAST] (+0x94) con (&{x, y}, raster, 0); devuelve el raster
// (el resultado de la funcion estandar se ignora). El EXE reserva 16 bytes de local y solo escribe x e y.
RwRaster* RwRasterRenderFast(RwRaster* raster, RwInt32 x, RwInt32 y) {
    RwInt32 position[2] = { x, y };
    RwEngineInstance->stdFunc[rwSTANDARDRASTERRENDERFAST](position, raster, 0);
    return raster;
}

// 0x7FAFF0 (lote R28A, sin validar; 36 bytes; era una envoltura; cdecl, (raster); `RwRasterUnlockPalette` del SDK): stdFunc[rwSTANDARDRASTERUNLOCKPALETTE] (+0xA8) con (NULL, raster, 0) y despues quita las banderas
// 0x18 de privateFlags (+0x22 &= 0xE7); devuelve el raster.
RwRaster* RwRasterUnlockPalette(RwRaster* raster) {
    RwEngineInstance->stdFunc[rwSTANDARDRASTERUNLOCKPALETTE](nullptr, raster, 0);
    raster->privateFlags = static_cast<RwUInt8>(raster->privateFlags & 0xE7u);
    return raster;
}

// 0x7FB020 (lote R28A, sin validar; 62 bytes; era una envoltura; cdecl, (raster); `RwRasterDestroy` del SDK): _rwPluginRegistryDeInitObject(&rasterTKList = 0x8E2518, raster) (0x808740), stdFunc[rwSTANDARDRASTERDESTROY]
// (+0x5C) con (NULL, raster, 0) y RwFreeListFree de la lista de rasters (RwRasterGlobals + 0x60; RwEngineInstance + 0x148). Devuelve TRUE siempre.
RwBool RwRasterDestroy(RwRaster* raster) {
    _rwPluginRegistryDeInitObject(&RasterTKList, raster);
    RwEngineInstance->stdFunc[rwSTANDARDRASTERDESTROY](nullptr, raster, 0);
    RwFreeListFree(RasterFreeListR28A(), raster);
    return TRUE;
}

// 0x7FB060 (lote R28A, sin validar; 74 bytes; era una envoltura; cdecl, (raster); `RwRasterPushContext` del SDK): stdFunc[rwSTANDARDSETRASTERCONTEXT] (+0x74) con (NULL, raster, 0); si devuelve FALSE el resultado es NULL.
// Si no, incrementa el puntero de pila (RwRasterGlobals + 0x28), guarda el raster en rasterStack[puntero nuevo] (sin acotar) y lo devuelve.
RwRaster* RwRasterPushContext(RwRaster* raster) {
    if (!RwEngineInstance->stdFunc[rwSTANDARDSETRASTERCONTEXT](nullptr, raster, 0)) {
        return nullptr;
    }
    RasterGlobals().rasterSP = RasterGlobals().rasterSP + 1;
    *(RasterGlobals().rasterStack + RasterGlobals().rasterSP) = raster;
    return raster;
}

// 0x7FB110 (lote R28A, sin validar; 74 bytes; era una envoltura; cdecl sin argumentos; `RwRasterPopContext` del SDK): decrementa el puntero de pila ANTES de llamar, guarda el raster que estaba en la cima (rasterStack[puntero
// viejo]) y llama a stdFunc[rwSTANDARDSETRASTERCONTEXT] con (NULL, rasterStack[puntero nuevo], 0). Devuelve el raster sacado si la funcion estandar devolvio TRUE y NULL si no (el puntero ya esta decrementado).
RwRaster* RwRasterPopContext() {
    const RwInt32 oldPointer = RasterGlobals().rasterSP;
    RasterGlobals().rasterSP = oldPointer - 1;
    RwRaster* const popped = *(RasterGlobals().rasterStack + oldPointer);
    RwRaster* const current = *(RasterGlobals().rasterStack + RasterGlobals().rasterSP);
    return RwEngineInstance->stdFunc[rwSTANDARDSETRASTERCONTEXT](nullptr, current, 0) ? popped : nullptr;
}

// 0x7FB160 (lote R28A, sin validar; 54 bytes; era una envoltura; cdecl, (raster); `RwRasterGetNumLevels` del SDK): sin el bit 7 de cFormat (+0x23) devuelve 1. Con el bit llama a stdFunc[rwSTANDARDRASTERGETMIPLEVELS]
// (+0xB8) con (&salida, raster, 0), donde la salida es el hueco del argumento (inicialmente el propio puntero del raster, que es el valor que devuelve si la funcion estandar da TRUE sin escribir); FALSE -> -1.
RwInt32 RwRasterGetNumLevels(RwRaster* raster) {
    if ((raster->cFormat & 0x80u) == 0) {
        return 1;
    }
    RwInt32 levels = static_cast<RwInt32>(reinterpret_cast<uintptr_t>(raster));
    if (RwEngineInstance->stdFunc[rwSTANDARDRASTERGETMIPLEVELS](&levels, raster, 0)) {
        return levels;
    }
    return -1;
}

// 0x7FB1A0 (lote R28A, sin validar; 47 bytes; era una envoltura; cdecl, (raster, dispositivo, banderas); `RwRasterShowRaster` del SDK): lee stdFunc[rwSTANDARDRASTERSHOWRASTER] (+0x98), llama a _rwResourcesPurge (0x807E50) y
// llama a la funcion con (raster, dispositivo, banderas) (la salida es el propio raster). Devuelve el raster si dio TRUE y NULL si no.
RwRaster* RwRasterShowRaster(RwRaster* raster, void* dev, RwUInt32 flags) {
    const RwStandardFunc show = RwEngineInstance->stdFunc[rwSTANDARDRASTERSHOWRASTER];
    _rwResourcesPurge();
    return show(raster, dev, static_cast<RwInt32>(flags)) ? raster : nullptr;
}

// 0x7FB230 (lote R28A, sin validar; 154 bytes; era una envoltura; cdecl, (ancho, alto, profundidad, banderas); `RwRasterCreate` del SDK): RwFreeListAlloc (RwEngineInstance + 0x144) de la lista de rasters (RwRasterGlobals + 0x60)
// con la pista 0x30407; sin memoria devuelve NULL. Inicializa ancho (+0xC), alto (+0x10), profundidad (+0x14), privateFlags (+0x22), cFlags (+0x21), nOffsetX/Y (+0x1C/+0x1E) a 0, parent (+0) = el propio raster y
// cpPixels/palette (+4, +8) a NULL (el resto de la memoria no se toca) y llama a stdFunc[rwSTANDARDRASTERCREATE] (+0x58) con (NULL, raster, banderas). Si da FALSE libera el raster (RwFreeListFree) y devuelve NULL; si
// da TRUE llama a _rwPluginRegistryInitObject(&rasterTKList, raster) (0x8086E0) y devuelve el raster.
RwRaster* RwRasterCreate(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags) {
    RwRaster* const raster = static_cast<RwRaster*>(RwFreeListAlloc(RasterFreeListR28A(), 0x30407));
    if (raster == nullptr) {
        return nullptr;
    }
    const RwStandardFunc create = RwEngineInstance->stdFunc[rwSTANDARDRASTERCREATE];
    raster->width = width;
    raster->privateFlags = 0;
    raster->height = height;
    raster->depth = depth;
    raster->cFlags = 0;
    raster->nOffsetX = 0;
    raster->nOffsetY = 0;
    raster->parent = raster;
    raster->cpPixels = nullptr;
    raster->palette = nullptr;
    if (!create(nullptr, raster, flags)) {
        RwFreeListFree(RasterFreeListR28A(), raster);
        return nullptr;
    }
    _rwPluginRegistryInitObject(&RasterTKList, raster);
    return raster;
}

// 0x7FB2D0 (lote R28A, sin validar; 54 bytes; era una envoltura; cdecl, (raster, nivel, modo de bloqueo); `RwRasterLock` del SDK; calcula el stride del raster): llama a stdFunc[rwSTANDARDRASTERLOCK] (+0x84) con
// (&salida, raster, (nivel & 0xFF) * 256 + modo), donde la salida es el hueco del argumento `nivel` (inicialmente el propio nivel, que se devuelve si la funcion estandar da TRUE sin escribir). Devuelve la salida si dio TRUE
// y NULL si no.
RwUInt8* RwRasterLock(RwRaster* raster, RwUInt8 level, RwInt32 lockMode) {
    RwUInt8* pixels = reinterpret_cast<RwUInt8*>(static_cast<uintptr_t>(level));
    const RwInt32 request = static_cast<RwInt32>((static_cast<uint32>(level) & 0xFFu) << 8) + lockMode;
    return RwEngineInstance->stdFunc[rwSTANDARDRASTERLOCK](&pixels, raster, request) ? pixels : nullptr;
}

// 0x7FC2D0 (lote R28A, sin validar; 67 bytes; era una envoltura; cdecl, (estado, valor); `RwD3D9SetRenderState` del SDK): si el valor pedido (0xC991D0 + 8 * estado) ya es `valor` no hace nada; si no lo guarda y, si la bandera
// "en lista" (+4) era 0, la pone a 1 y anade el estado a la lista de pendientes (0xC98B40[contador++], contador 0xC9A5EC). No llama al dispositivo (lo hace _rwD3D9RenderStateFlushCache, 0x7FC200). Estado sin acotar.
// Las secuencias identicas que el EXE incrusta en 0x7FC4D0, 0x7FCAC0 y 0x7FD100 son este mismo cuerpo.
void RwD3D9SetRenderState(RwUInt32 state, RwUInt32 value) {
    D3D9RenderStateSlot& slot = D3D9RenderStateWanted[state];
    if (slot.value == value) {
        return;
    }
    slot.value = value;
    if (slot.pending == 0) {
        const uint32 count = D3D9RenderStatePendingCount;
        slot.pending = 1;
        reinterpret_cast<uint32*>(0xC98B40)[count] = state;
        D3D9RenderStatePendingCount = count + 1;
    }
}

// 0x7FC320 (lote R28A, sin validar; 18 bytes; era una envoltura; cdecl, (estado, salida*)): *salida = valor pedido del estado (0xC991D0 + 8 * estado), sin acotar.
void RwD3D9GetRenderState(RwUInt32 state, void* value) {
    *static_cast<uint32*>(value) = D3D9RenderStateWanted[state].value;
}

// 0x7FC340 (lote R28A, sin validar; 90 bytes; era una envoltura; cdecl, (etapa, tipo, valor); `RwD3D9SetTextureStageState` del SDK): indice = etapa * 33 + tipo; si el valor pedido (0xC980F8 + 8 * indice) ya es `valor` no hace
// nada; si no lo guarda y, si la bandera (+4) era 0, la pone a 1 y anade el par (etapa, tipo) a la lista de pendientes (0xC99C80, 8 bytes por par; contador 0xC9A5F0). Sin acotar.
void RwD3D9SetTextureStageState(RwUInt32 stage, RwUInt32 type, RwUInt32 value) {
    D3D9StageStateSlot& slot = D3D9StageStateWanted[stage + (type + (stage << 5))];
    if (slot.value == value) {
        return;
    }
    slot.value = value;
    if (slot.pending == 0) {
        slot.pending = 1;
        const uint32 count = D3D9StageStatePendingCount;
        D3D9StageStatePendingList[count * 2] = stage;
        D3D9StageStatePendingList[count * 2 + 1] = type;
        D3D9StageStatePendingCount = count + 1;
    }
}

// 0x7FC3C0 (lote R28A, sin validar; 63 bytes; era una envoltura; cdecl, (etapa, tipo, valor); `RwD3D9SetSamplerState` del SDK): indice = tipo + 14 * etapa; si el valor actual (0xC98938 + 4 * indice) ya es `valor` no hace
// nada; si no lo guarda ANTES de llamar a _RwD3DDevice->SetSamplerState(etapa, tipo, valor) (+0x114). El HRESULT no se mira. Sin acotar.
void RwD3D9SetSamplerState(RwUInt32 stage, RwUInt32 type, RwUInt32 value) {
    uint32& current = D3D9SamplerStateCurrent[type + stage * 14u];
    if (current == value) {
        return;
    }
    current = value;
    ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, stage, type, value);
}

// 0x7FDE70 (lote R28A, sin validar; 556 bytes; era una envoltura; cdecl, (textura, etapa); `RwD3D9SetTexture` del SDK) -> TRUE siempre. Con textura, con la cache de la etapa (0xC9A508 + 0x18 * etapa; etapa sin acotar):
//  * direccion U (filterAddressing >> 8 & 0xF) y V (>> 0xC & 0xF): si difieren de la cache (+4 / +8) la anota, guarda en el estado de muestreo (0xC98938 + 4 * (1 o 2 + 14 * etapa)) la traduccion por la tabla 0x88493C y
//    llama SIEMPRE a SetSamplerState(etapa, 1 o 2, traduccion) (+0x114), sin comparar contra el estado de muestreo como hace 0x7FC3C0.
//  * filtro = filterAddressing & 0xFF. Con el desplazamiento de RpAnisot (0xC9A5E8, con signo) > 0: anisotropia = byte de la textura en ese desplazamiento; si difiere de la cache (+0x14) la anota, escribe 0xC98960 (estado
//    de muestreo MAXANISOTROPY de la ETAPA 0, siempre) y llama SetSamplerState(0, 0xA, anisotropia) (siempre la etapa 0, con independencia de `etapa`); anisotropia > 1 (con signo) fuerza el filtro 7.
//  * si el filtro difiere de la cache (+0xC): la anota; con la tabla de pares 0x884950: min/mag = [filtro * 2], mip = [filtro * 2 + 1]; guarda y envia SetSamplerState(etapa, 6, min/mag), (etapa, 5, min/mag) y
//    (etapa, 7, mip) (siempre los tres, sin comparar).
//  * termina con _rwD3D9RWSetRasterStage(textura->raster, etapa) (0x7FDCD0).
// Sin textura: con la etapa 0 y 0xC9A4EC != 0: lo pone a 0 y, si 0xC9A4E8 es 0, deja 0xC992A8 = 0 (SIEMPRE, y en lista 0x1B con la bandera 0xC992AC si no estaba) y, si 0xC99248 != 0, lo pone a 0 y encola 0x0F (bandera
// 0xC9924C). Despues, para cualquier etapa: si la cache +0 es distinta de 0 la pone a 0 y llama a SetTexture(etapa, NULL) (+0x104).
RwBool RwD3D9SetTexture(RwTexture* texture, RwUInt32 stage) {
    D3D9TextureStageCache& cache = D3D9TextureStageCaches[stage];
    if (texture != nullptr) {
        const uint32 addressU = (texture->filterAddressing >> 8) & 0xFu;
        if (addressU != cache.addressU) {
            cache.addressU = addressU;
            const uint32 translated = kTableAddressMode[addressU];
            D3D9SamplerStateCurrent[1 + stage * 14u] = translated;
            ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, stage, kD3DSAMPAddressU, translated);
        }
        const uint32 addressV = (texture->filterAddressing >> 12) & 0xFu;
        if (addressV != cache.addressV) {
            cache.addressV = addressV;
            const uint32 translated = kTableAddressMode[addressV];
            D3D9SamplerStateCurrent[2 + stage * 14u] = translated;
            ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, stage, kD3DSAMPAddressV, translated);
        }
        uint32 filter = texture->filterAddressing & 0xFFu;
        const int32 anisotropyOffset = StaticRef<int32>(0xC9A5E8);
        if (anisotropyOffset > 0) {
            const uint32 anisotropy = *(reinterpret_cast<const uint8*>(texture) + anisotropyOffset);
            if (cache.anisotropy != anisotropy) {
                cache.anisotropy = anisotropy;
                D3D9SamplerStateCurrent[kD3DSAMPMaxAnisotropy] = anisotropy;
                ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, 0u, kD3DSAMPMaxAnisotropy, anisotropy);
            }
            if (static_cast<int32>(anisotropy) > 1) {
                filter = 7;
            }
        }
        if (cache.filter != filter) {
            const uint32 minMag = kTableFilterMode[filter * 2];
            cache.filter = filter;
            const uint32 mip = kTableFilterMode[filter * 2 + 1];
            D3D9SamplerStateCurrent[kD3DSAMPMinFilter + stage * 14u] = minMag;
            ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, stage, kD3DSAMPMinFilter, minMag);
            D3D9SamplerStateCurrent[kD3DSAMPMagFilter + stage * 14u] = minMag;
            ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, stage, kD3DSAMPMagFilter, minMag);
            D3D9SamplerStateCurrent[kD3DSAMPMipFilter + stage * 14u] = mip;
            ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, stage, kD3DSAMPMipFilter, mip);
        }
        _rwD3D9RWSetRasterStage(texture->raster, stage);
        return TRUE;
    }
    if (stage == 0 && StaticRef<uint32>(0xC9A4EC) != 0) {
        const uint32 vertexAlpha = StaticRef<uint32>(0xC9A4E8);
        StaticRef<uint32>(0xC9A4EC) = 0;
        if (vertexAlpha == 0) {
            const uint32 alphaBlendPending = StaticRef<uint32>(0xC992AC);
            StaticRef<uint32>(0xC992A8) = 0;
            if (alphaBlendPending == 0) {
                D3D9QueueRenderState(StaticRef<uint32>(0xC992AC), kD3DRSAlphaBlendEnable);
            }
            if (StaticRef<uint32>(0xC99248) != 0) {
                StaticRef<uint32>(0xC99248) = 0;
                D3D9QueueRenderState(StaticRef<uint32>(0xC9924C), kD3DRSAlphaTestEnable);
            }
        }
    }
    if (cache.texture != 0) {
        cache.texture = 0;
        ComCall<kSlotDeviceSetTexture, int32>(D3D9Device, stage, static_cast<void*>(nullptr));
    }
    return TRUE;
}

// 0x7FC4D0 (lote R28A, sin validar; 1512 bytes; era una envoltura; cdecl, (propiedades de superficie, color, banderas); `RwD3D9SetSurfaceProperties` del SDK): construye el D3DMATERIAL9 de trabajo (0x8E2538: difuso +0, ambiente
// +0x10, especular +0x20 (no se toca), emisivo +0x30) y lo envia. Coeficientes: propiedades = {ambiente +0, especular +4, difuso +8} (float); luz ambiente global 0x8E2418/1C/20; k = 0x859A3C (1/255 = 0x3B808081).
// Cache de salida rapida (0xC9A5FC = difuso, 0xC9A600 = ambiente, 0xC9A5F8 = color RGBA como dword, 0xC9A5F4 = banderas & 0x48, 0xC9A5D8/DC/E0 = luz ambiente), todo comparado como DWORDS (un NaN o un -0 distinto cuenta como
// cambio): si todo coincide devuelve TRUE sin tocar nada; si no actualiza la cache. Con banderas 0x40 (modular el color del material) y color != 0xFFFFFFFF: s = difuso * k; difuso rgb = (float)canal * s, alfa = (float)a * k;
// t = ambiente * k; con 0x08 (prelit): AMBIENT (0x8B) = ARGB (a << 24 | r << 16 | g << 8 | b), COLORVERTEX (0x8D) = 1, AMBIENTMATERIALSOURCE (0x93) = 1, EMISSIVEMATERIALSOURCE (0x94) = 0, ambiente del material = 0 y emisivo
// = ((float)canal * luz) * t; sin 0x08: AMBIENT = 0xFFFFFFFF, 0x8D = 0, 0x93 = 0, 0x94 = 0, ambiente del material = ((float)canal * luz) * t y emisivo = 0. Sin modular: difuso rgb = difuso (copia por la x87: NaN senalizador
// silenciado), alfa = 1.0f, AMBIENT = 0xFFFFFFFF, 0x93 = 0; con 0x08: 0x8D = 1 y 0x94 = 1 (el emisivo no se toca); sin 0x08: 0x8D = 0, 0x94 = 0 y emisivo = 0; ambiente del material = luz * ambiente o, si el dword del ambiente
// es 0x3F800000 (1.0f), la luz copiada tal cual. Los estados pasan por RwD3D9SetRenderState (el EXE incrusta el mismo cuerpo). Al final copia los 17 dwords a la cache de material (0xC98AF8) y llama a
// SetMaterial (+0xC4) con el material de trabajo; devuelve HRESULT >= 0. x87 a 24 bits: cada producto se redondea a float, igual que en C++.
RwBool RwD3D9SetSurfaceProperties(const RwSurfaceProperties* surfaceProps, const RwRGBA* color, RwUInt32 flags) {
    const uint32* const properties = reinterpret_cast<const uint32*>(surfaceProps);
    const uint32 colorDword = *reinterpret_cast<const uint32*>(color);
    const uint32 maskedFlags = flags & 0x48u;
    const uint32 lightBits[3] = { StaticRef<uint32>(0x8E2418), StaticRef<uint32>(0x8E241C), StaticRef<uint32>(0x8E2420) };
    if (StaticRef<uint32>(0xC9A5FC) == properties[2] && StaticRef<uint32>(0xC9A600) == properties[0] && StaticRef<uint32>(0xC9A5F8) == colorDword
        && StaticRef<uint32>(0xC9A5F4) == maskedFlags && StaticRef<uint32>(0xC9A5D8) == lightBits[0] && StaticRef<uint32>(0xC9A5DC) == lightBits[1] && StaticRef<uint32>(0xC9A5E0) == lightBits[2]) {
        return TRUE;
    }
    StaticRef<uint32>(0xC9A5FC) = properties[2];
    StaticRef<uint32>(0xC9A600) = properties[0];
    StaticRef<uint32>(0xC9A5F8) = colorDword;
    StaticRef<uint32>(0xC9A5D8) = StaticRef<uint32>(0x8E2418);
    StaticRef<uint32>(0xC9A5DC) = StaticRef<uint32>(0x8E241C);
    StaticRef<uint32>(0xC9A5E0) = StaticRef<uint32>(0x8E2420);
    StaticRef<uint32>(0xC9A5F4) = maskedFlags;
    float* const material = reinterpret_cast<float*>(D3D9MaterialStaging);
    const float* const light = reinterpret_cast<const float*>(0x8E2418);
    const RwUInt8* const channel = reinterpret_cast<const RwUInt8*>(color);
    constexpr uint32 kD3DRSAmbient = 0x8B, kD3DRSColorVertex = 0x8D, kD3DRSAmbientMaterialSource = 0x93, kD3DRSEmissiveMaterialSource = 0x94;
    if ((maskedFlags & 0x40u) != 0 && colorDword != 0xFFFFFFFFu) {
        const float k = StaticRef<float>(0x859A3C);
        const float scale = surfaceProps->diffuse * k;
        material[0] = static_cast<float>(channel[0]) * scale;
        material[1] = static_cast<float>(channel[1]) * scale;
        material[2] = static_cast<float>(channel[2]) * scale;
        material[3] = static_cast<float>(channel[3]) * k;
        const float ambientScale = surfaceProps->ambient * k;
        if ((maskedFlags & 0x08u) != 0) {
            const uint32 argb = (static_cast<uint32>(channel[3]) << 24) | (static_cast<uint32>(channel[0]) << 16) | (static_cast<uint32>(channel[1]) << 8) | static_cast<uint32>(channel[2]);
            RwD3D9SetRenderState(kD3DRSAmbient, argb);
            RwD3D9SetRenderState(kD3DRSColorVertex, 1);
            RwD3D9SetRenderState(kD3DRSAmbientMaterialSource, 1);
            RwD3D9SetRenderState(kD3DRSEmissiveMaterialSource, 0);
            D3D9MaterialStaging[4] = 0;
            D3D9MaterialStaging[5] = 0;
            D3D9MaterialStaging[6] = 0;
            material[12] = (static_cast<float>(channel[0]) * light[0]) * ambientScale;
            material[13] = (static_cast<float>(channel[1]) * light[1]) * ambientScale;
            material[14] = (static_cast<float>(channel[2]) * light[2]) * ambientScale;
        } else {
            RwD3D9SetRenderState(kD3DRSAmbient, 0xFFFFFFFFu);
            RwD3D9SetRenderState(kD3DRSColorVertex, 0);
            RwD3D9SetRenderState(kD3DRSAmbientMaterialSource, 0);
            RwD3D9SetRenderState(kD3DRSEmissiveMaterialSource, 0);
            material[4] = (static_cast<float>(channel[0]) * light[0]) * ambientScale;
            material[5] = (static_cast<float>(channel[1]) * light[1]) * ambientScale;
            D3D9MaterialStaging[12] = 0;
            D3D9MaterialStaging[13] = 0;
            material[6] = (static_cast<float>(channel[2]) * light[2]) * ambientScale;
            D3D9MaterialStaging[14] = 0;
        }
    } else {
        const uint32 diffuseBits = X87FloatCopyBitsD3D9(properties[2]);
        D3D9MaterialStaging[0] = diffuseBits;
        D3D9MaterialStaging[1] = diffuseBits;
        D3D9MaterialStaging[2] = diffuseBits;
        D3D9MaterialStaging[3] = 0x3F800000u;
        RwD3D9SetRenderState(kD3DRSAmbient, 0xFFFFFFFFu);
        RwD3D9SetRenderState(kD3DRSAmbientMaterialSource, 0);
        if ((maskedFlags & 0x08u) != 0) {
            RwD3D9SetRenderState(kD3DRSColorVertex, 1);
            RwD3D9SetRenderState(kD3DRSEmissiveMaterialSource, 1);
        } else {
            RwD3D9SetRenderState(kD3DRSColorVertex, 0);
            RwD3D9SetRenderState(kD3DRSEmissiveMaterialSource, 0);
            D3D9MaterialStaging[12] = 0;
            D3D9MaterialStaging[13] = 0;
            D3D9MaterialStaging[14] = 0;
        }
        if (properties[0] != 0x3F800000u) {
            material[4] = light[0] * surfaceProps->ambient;
            material[5] = light[1] * surfaceProps->ambient;
            material[6] = light[2] * surfaceProps->ambient;
        } else {
            D3D9MaterialStaging[4] = lightBits[0];
            D3D9MaterialStaging[5] = lightBits[1];
            D3D9MaterialStaging[6] = lightBits[2];
        }
    }
    for (int i = 0; i < 17; ++i) {
        D3D9CurrentMaterial.dwords[i] = D3D9MaterialStaging[i];
    }
    return ComCall<kSlotDeviceSetMaterial, int32>(D3D9Device, static_cast<const void*>(D3D9MaterialStaging)) >= 0;
}

// Fases comunes de _rwD3D9RenderStateOpen y _rwD3D9RenderStateReset (lote R28A): el EXE las incrusta en las dos funciones, con las mismas escrituras.
namespace {
// Pone a -1 los valores actuales de los estados de render (0xD2 dwords), a 0 los de 0x22, 0x8B, 0x3A y 0x3B (0xC98F10, 0xC990B4, 0xC98F70, 0xC98F74) y copia los valores a los pedidos con la bandera a 0.
void D3D9ClearRenderStateCaches() {
    for (uint32 i = 0; i < 0xD2; ++i) {
        D3D9RenderStateCurrent[i] = 0xFFFFFFFFu;
    }
    D3D9RenderStateCurrent[0x22] = 0;
    D3D9RenderStateCurrent[0x8B] = 0;
    D3D9RenderStateCurrent[0x3A] = 0;
    D3D9RenderStateCurrent[0x3B] = 0;
    for (uint32 i = 0; i < 0xD2; ++i) {
        D3D9RenderStateWanted[i].pending = 0;
        D3D9RenderStateWanted[i].value = D3D9RenderStateCurrent[i];
    }
}
// Estados de etapa actuales a -1 (0x108), contador de render a 0, estados de etapa pedidos {-1, 0}, estados de muestreo actuales a -1 (0x70), material actual a 0 (17 dwords) y las cinco cache del material de superficie a 0.
void D3D9ClearStageAndMaterialCaches() {
    for (uint32 i = 0; i < 0x108; ++i) {
        D3D9StageStateCurrent[i] = 0xFFFFFFFFu;
    }
    D3D9RenderStatePendingCount = 0;
    for (uint32 i = 0; i < 0x108; ++i) {
        D3D9StageStateWanted[i].value = 0xFFFFFFFFu;
        D3D9StageStateWanted[i].pending = 0;
    }
    for (uint32 i = 0; i < 0x70; ++i) {
        D3D9SamplerStateCurrent[i] = 0xFFFFFFFFu;
    }
    for (uint32 i = 0; i < 17; ++i) {
        D3D9CurrentMaterial.dwords[i] = 0;
    }
    StaticRef<uint32>(0xC9A5FC) = 0;
    StaticRef<uint32>(0xC9A600) = 0;
    StaticRef<uint32>(0xC9A5D8) = 0;
    StaticRef<uint32>(0xC9A5DC) = 0;
    StaticRef<uint32>(0xC9A5E0) = 0;
}
bool D3D9FogUsesTable() { // RasterCaps: FOGTABLE (0x100) y WFOG (0x100000)
    return (D3D9RasterCaps & 0x100u) != 0 && (D3D9RasterCaps & 0x100000u) != 0;
}
} // namespace

// 0x7FCAC0 (lote R28A, sin validar; 1595 bytes; no tenia envoltura; nombre inferido, `_rwD3D9RenderStateOpen` de RenderWare; cdecl sin argumentos, `jmp 0x7FC200` al final): inicializa las caches de estados y los valores
// por defecto de RenderWare. Pone las caches a su valor inicial (render actuales -1 salvo 0x22, 0x8B, 0x3A, 0x3B a 0; pedidos = actuales sin bandera; etapa y muestreo a -1; material a 0; 0xC9A5F0, 0xC9A5F4 y 0xC9A5F8 a 0;
// 0xC9A4F4 = 0, 0xC9A4F8 = 1, 0xC9A4FC = 1.0f, 0xC9A500 = 0) y encola, por RwD3D9SetRenderState y en este orden: 0x1C = 0; niebla (RasterCaps & 0x100 y & 0x100000: 0x23 = 3 y 0x8C = 0; si no: 0x23 = 0 y 0x8C = 3);
// 0x22 = 0, 9 = 2 (0xC9A504 = 2), 0x17 = 4, 0x0E = 1, 7 = 1 (0xC9A4C0 = 1, 0xC9A4C4 = 1), 0x34 = 0, 0x35 = 1, 0x36 = 1, 0x37 = 1, 0x38 = 8, 0x39 = 0, 0x3A = -1, 0x3B = -1 (0xC9A4D8 = 8, 0xC9A4E0 = 0xC9A4E4 = -1,
// 0xC9A4C8 = 0, 0xC9A4CC = 0xC9A4D0 = 0xC9A4D4 = 1, 0xC9A4DC = 0). Despues, para las 8 etapas: SetTexture(etapa, NULL) y la cache de la etapa (+0 = 0); con TextureFilterCaps & 0x200: muestreo 6 = 2 (directo si cambia),
// luego 5 = 2 por 0x7FC3C0 y filtro de la etapa (+0xC) = 2; si no: 6 = 1, 5 = 1 y filtro = 1; muestreo 7 = 0, 1 = 1, 2 = 1 (direcciones U y V de la etapa = 1), 4 = 0xFF000000 (borde = 0xFF000000), 0xA = 1 (anisotropia = 1);
// con etapa != 0: estados de etapa (etapa, 1) = 1 y (etapa, 4) = 1. Despues 0x13 = 5, 0x14 = 6 (0xC9A5C8 = 5, 0xC9A5CC = 6), 0x19 = 5 (0xC9A5D0 = 5), 0x18 = 0 (0xC9A5D4 = 1), 0x1B = 0, 0x0F = 0, 0x89 = 0 (0xC9A4E8 = 0, 0xC9A4EC = 0),
// 0x16 = 2 (0xC9A4F0 = 2); estados de etapa (0,1) = 3, (0,3) = 0, (0,4) = 3, (0,6) = 0; 0x91 = 0, 0x92 = 0, 0x93 = 0, 0x1A = RasterCaps & 1, 0x1D = 0, 0x8E = 0, 0x8B = -1, 0x8F = 0 y vuelca los estados pendientes
// (_rwD3D9RenderStateFlushCache, 0x7FC200; en el EXE es un `jmp`). Sin x87.
void _rwD3D9RenderStateOpen() {
    D3D9ClearRenderStateCaches();
    D3D9ClearStageAndMaterialCaches();
    StaticRef<uint32>(0xC9A5F0) = 0;
    StaticRef<uint32>(0xC9A5F4) = 0;
    StaticRef<uint32>(0xC9A5F8) = 0;
    StaticRef<uint32>(0xC9A4F4) = 0;
    StaticRef<uint32>(0xC9A4F8) = 1;
    StaticRef<uint32>(0xC9A4FC) = 0x3F800000u;
    StaticRef<uint32>(0xC9A500) = 0;
    RwD3D9SetRenderState(0x1C, 0);
    if (D3D9FogUsesTable()) {
        RwD3D9SetRenderState(0x23, 3);
        RwD3D9SetRenderState(0x8C, 0);
    } else {
        RwD3D9SetRenderState(0x23, 0);
        RwD3D9SetRenderState(0x8C, 3);
    }
    RwD3D9SetRenderState(0x22, 0);
    RwD3D9SetRenderState(9, 2);
    StaticRef<uint32>(0xC9A504) = 2;
    RwD3D9SetRenderState(0x17, 4);
    RwD3D9SetRenderState(0x0E, 1);
    RwD3D9SetRenderState(7, 1);
    StaticRef<uint32>(0xC9A4C0) = 1;
    StaticRef<uint32>(0xC9A4C4) = 1;
    RwD3D9SetRenderState(0x34, 0);
    RwD3D9SetRenderState(0x35, 1);
    RwD3D9SetRenderState(0x36, 1);
    RwD3D9SetRenderState(0x37, 1);
    RwD3D9SetRenderState(0x38, 8);
    RwD3D9SetRenderState(0x39, 0);
    RwD3D9SetRenderState(0x3A, 0xFFFFFFFFu);
    RwD3D9SetRenderState(0x3B, 0xFFFFFFFFu);
    StaticRef<uint32>(0xC9A4D8) = 8;
    StaticRef<uint32>(0xC9A4E0) = 0xFFFFFFFFu;
    StaticRef<uint32>(0xC9A4E4) = 0xFFFFFFFFu;
    StaticRef<uint32>(0xC9A4C8) = 0;
    StaticRef<uint32>(0xC9A4CC) = 1;
    StaticRef<uint32>(0xC9A4D0) = 1;
    StaticRef<uint32>(0xC9A4D4) = 1;
    StaticRef<uint32>(0xC9A4DC) = 0;
    for (uint32 stage = 0; stage < 8; ++stage) {
        D3D9TextureStageCache& cache = D3D9TextureStageCaches[stage];
        ComCall<kSlotDeviceSetTexture, int32>(D3D9Device, stage, static_cast<void*>(nullptr));
        cache.texture = 0;
        if ((D3D9TextureFilterCaps & 0x200u) != 0) {
            uint32& minFilterCache = D3D9SamplerStateCurrent[kD3DSAMPMinFilter + stage * 14u];
            if (minFilterCache != 2) {
                minFilterCache = 2;
                ComCall<kSlotDeviceSetSamplerState, int32>(D3D9Device, stage, kD3DSAMPMinFilter, 2u);
            }
            RwD3D9SetSamplerState(stage, kD3DSAMPMagFilter, 2);
            cache.filter = 2;
        } else {
            RwD3D9SetSamplerState(stage, kD3DSAMPMinFilter, 1);
            RwD3D9SetSamplerState(stage, kD3DSAMPMagFilter, 1);
            cache.filter = 1;
        }
        RwD3D9SetSamplerState(stage, kD3DSAMPMipFilter, 0);
        RwD3D9SetSamplerState(stage, kD3DSAMPAddressU, 1);
        RwD3D9SetSamplerState(stage, kD3DSAMPAddressV, 1);
        cache.addressU = 1;
        cache.addressV = 1;
        RwD3D9SetSamplerState(stage, kD3DSAMPBorderColor, 0xFF000000u);
        cache.borderColor = 0xFF000000u;
        cache.anisotropy = 1;
        RwD3D9SetSamplerState(stage, kD3DSAMPMaxAnisotropy, 1);
        if (stage != 0) {
            RwD3D9SetTextureStageState(stage, 1, 1);
            RwD3D9SetTextureStageState(stage, 4, 1);
        }
    }
    RwD3D9SetRenderState(0x13, 5);
    RwD3D9SetRenderState(0x14, 6);
    StaticRef<uint32>(0xC9A5C8) = 5;
    StaticRef<uint32>(0xC9A5CC) = 6;
    RwD3D9SetRenderState(0x19, 5);
    StaticRef<uint32>(0xC9A5D0) = 5;
    StaticRef<uint32>(0xC9A5D4) = 1;
    RwD3D9SetRenderState(0x18, 0);
    RwD3D9SetRenderState(0x1B, 0);
    RwD3D9SetRenderState(0x0F, 0);
    StaticRef<uint32>(0xC9A4E8) = 0;
    StaticRef<uint32>(0xC9A4EC) = 0;
    RwD3D9SetRenderState(0x89, 0);
    RwD3D9SetRenderState(0x16, 2);
    StaticRef<uint32>(0xC9A4F0) = 2;
    RwD3D9SetTextureStageState(0, 1, 3);
    RwD3D9SetTextureStageState(0, 3, 0);
    RwD3D9SetTextureStageState(0, 4, 3);
    RwD3D9SetTextureStageState(0, 6, 0);
    RwD3D9SetRenderState(0x91, 0);
    RwD3D9SetRenderState(0x92, 0);
    RwD3D9SetRenderState(0x93, 0);
    RwD3D9SetRenderState(0x1A, D3D9RasterCaps & 1u);
    RwD3D9SetRenderState(0x1D, 0);
    RwD3D9SetRenderState(0x8E, 0);
    RwD3D9SetRenderState(0x8B, 0xFFFFFFFFu);
    RwD3D9SetRenderState(0x8F, 0);
    _rwD3D9RenderStateFlushCache();
}

// 0x7FD100 (lote R28A, sin validar; 1806 bytes; era una llamada por direccion fija desde RwD3D9Close y el modo ventana; nombre `_rwD3D9RenderStateReset` como en WindowedMode.cpp; cdecl sin argumentos, `jmp 0x7FC200`
// al final): como _rwD3D9RenderStateOpen pero rehace la cache a partir de los valores de RenderWare guardados (0xC9A4xx..0xC9A5Dx), traducidos por las tablas del EXE, y tras una perdida del dispositivo. Guarda antes de
// borrar los pedidos de 0x24 (niebla, inicio), 0x25 (fin) y 0x18 (referencia alfa). Despues del mismo borrado de caches (mas 0xC9A5F0, 0xC9A5F4, 0xC9A5F8 = 0) encola por RwD3D9SetRenderState y en este orden: 0x26 = [0xC9A4FC];
// 0x1C = [0xC9A4F4]; niebla (con RasterCaps & 0x100 y & 0x100000: 0x23 = tabla 0x8848FC[[0xC9A4F8]] y 0x8C = 0; si no 0x23 = 0 y 0x8C = la tabla); 0x24 y 0x25 = lo guardado; 0x22 = [0xC9A500]; 9 = 0x8848F0[[0xC9A504]];
// 0x0E = [0xC9A4C0]; con [0xC9A4C4] != 0: 0x17 = 4 y 7 = 1; si no: 0x17 = 8 y 7 = ([0xC9A4C0] != 0); 0x34 = [0xC9A4C8]; 0x35, 0x36 y 0x37 = 0x8849A0[[0xC9A4CC]], [[0xC9A4D0]] y [[0xC9A4D4]]; 0x38 = 0x8849C4[[0xC9A4D8]],
// 0x39 = [0xC9A4DC], 0x3A = [0xC9A4E0], 0x3B = [0xC9A4E4]. Para las 8 etapas: SetTexture(etapa, NULL), cache +0 = 0 y, por RwD3D9SetSamplerState, 6 y 5 = 0x884950[cache.filtro * 2], 7 = 0x884950[cache.filtro * 2 + 1],
// 1 y 2 = 0x88493C[cache.U] y [cache.V], 4 = borde, 0xA = anisotropia; con etapa != 0, estados de etapa (etapa, 1) = 1 y (etapa, 4) = 1. Despues 0x13 = 0x88490C[[0xC9A5C8]], 0x14 = 0x88490C[[0xC9A5CC]], 0x19 = 0x8849C4[[0xC9A5D0]],
// 0x18 = lo guardado; con [0xC9A4E8] != 0: 0x1B = 1 y 0x0F = [0xC9A5D4]; si no 0x1B = 0 y 0x0F = 0; [0xC9A4EC] = 0; 0x89 = 0; 0x16 = 0x884990[[0xC9A4F0]]; estados de etapa (0,1) = 3, (0,3) = 0, (0,4) = 3, (0,6) = 0; 0x91, 0x92, 0x93 = 0;
// 0x1A = RasterCaps & 1; 0x1D = 0; 0x8E = 0; 0x8B = -1; 0x8F = 0 y vuelca los pendientes (_rwD3D9RenderStateFlushCache). Sin x87.
void _rwD3D9RenderStateReset() {
    const uint32 savedFogStart = D3D9RenderStateWanted[0x24].value;
    const uint32 savedFogEnd   = D3D9RenderStateWanted[0x25].value;
    const uint32 savedAlphaRef = D3D9RenderStateWanted[0x18].value;
    D3D9ClearRenderStateCaches();
    D3D9ClearStageAndMaterialCaches();
    StaticRef<uint32>(0xC9A5F0) = 0;
    StaticRef<uint32>(0xC9A5F4) = 0;
    StaticRef<uint32>(0xC9A5F8) = 0;
    RwD3D9SetRenderState(0x26, StaticRef<uint32>(0xC9A4FC));
    RwD3D9SetRenderState(0x1C, StaticRef<uint32>(0xC9A4F4));
    if (D3D9FogUsesTable()) {
        RwD3D9SetRenderState(0x23, kTableFogMode[StaticRef<uint32>(0xC9A4F8)]);
        RwD3D9SetRenderState(0x8C, 0);
    } else {
        RwD3D9SetRenderState(0x23, 0);
        RwD3D9SetRenderState(0x8C, kTableFogMode[StaticRef<uint32>(0xC9A4F8)]);
    }
    RwD3D9SetRenderState(0x24, savedFogStart);
    RwD3D9SetRenderState(0x25, savedFogEnd);
    RwD3D9SetRenderState(0x22, StaticRef<uint32>(0xC9A500));
    RwD3D9SetRenderState(9, kTableShadeMode[StaticRef<uint32>(0xC9A504)]);
    const uint32 zWrite = StaticRef<uint32>(0xC9A4C0);
    RwD3D9SetRenderState(0x0E, zWrite);
    if (StaticRef<uint32>(0xC9A4C4) != 0) {
        RwD3D9SetRenderState(0x17, 4);
        RwD3D9SetRenderState(7, 1);
    } else {
        RwD3D9SetRenderState(0x17, 8);
        RwD3D9SetRenderState(7, zWrite != 0 ? 1u : 0u);
    }
    RwD3D9SetRenderState(0x34, StaticRef<uint32>(0xC9A4C8));
    RwD3D9SetRenderState(0x35, kTableStencilOp[StaticRef<uint32>(0xC9A4CC)]);
    RwD3D9SetRenderState(0x36, kTableStencilOp[StaticRef<uint32>(0xC9A4D0)]);
    RwD3D9SetRenderState(0x37, kTableStencilOp[StaticRef<uint32>(0xC9A4D4)]);
    RwD3D9SetRenderState(0x38, kTableCompare[StaticRef<uint32>(0xC9A4D8)]);
    RwD3D9SetRenderState(0x39, StaticRef<uint32>(0xC9A4DC));
    RwD3D9SetRenderState(0x3A, StaticRef<uint32>(0xC9A4E0));
    RwD3D9SetRenderState(0x3B, StaticRef<uint32>(0xC9A4E4));
    for (uint32 stage = 0; stage < 8; ++stage) {
        D3D9TextureStageCache& cache = D3D9TextureStageCaches[stage];
        ComCall<kSlotDeviceSetTexture, int32>(D3D9Device, stage, static_cast<void*>(nullptr));
        cache.texture = 0;
        RwD3D9SetSamplerState(stage, kD3DSAMPMinFilter, kTableFilterMode[cache.filter * 2]);
        RwD3D9SetSamplerState(stage, kD3DSAMPMagFilter, kTableFilterMode[cache.filter * 2]);
        RwD3D9SetSamplerState(stage, kD3DSAMPMipFilter, kTableFilterMode[cache.filter * 2 + 1]);
        RwD3D9SetSamplerState(stage, kD3DSAMPAddressU, kTableAddressMode[cache.addressU]);
        RwD3D9SetSamplerState(stage, kD3DSAMPAddressV, kTableAddressMode[cache.addressV]);
        RwD3D9SetSamplerState(stage, kD3DSAMPBorderColor, cache.borderColor);
        RwD3D9SetSamplerState(stage, kD3DSAMPMaxAnisotropy, cache.anisotropy);
        if (stage != 0) {
            RwD3D9SetTextureStageState(stage, 1, 1);
            RwD3D9SetTextureStageState(stage, 4, 1);
        }
    }
    RwD3D9SetRenderState(0x13, kTableBlend[StaticRef<uint32>(0xC9A5C8)]);
    RwD3D9SetRenderState(0x14, kTableBlend[StaticRef<uint32>(0xC9A5CC)]);
    RwD3D9SetRenderState(0x19, kTableCompare[StaticRef<uint32>(0xC9A5D0)]);
    RwD3D9SetRenderState(0x18, savedAlphaRef);
    if (StaticRef<uint32>(0xC9A4E8) != 0) {
        RwD3D9SetRenderState(0x1B, 1);
        RwD3D9SetRenderState(0x0F, StaticRef<uint32>(0xC9A5D4));
    } else {
        RwD3D9SetRenderState(0x1B, 0);
        RwD3D9SetRenderState(0x0F, 0);
    }
    StaticRef<uint32>(0xC9A4EC) = 0;
    RwD3D9SetRenderState(0x89, 0);
    RwD3D9SetRenderState(0x16, kTableCullMode[StaticRef<uint32>(0xC9A4F0)]);
    RwD3D9SetTextureStageState(0, 1, 3);
    RwD3D9SetTextureStageState(0, 3, 0);
    RwD3D9SetTextureStageState(0, 4, 3);
    RwD3D9SetTextureStageState(0, 6, 0);
    RwD3D9SetRenderState(0x91, 0);
    RwD3D9SetRenderState(0x92, 0);
    RwD3D9SetRenderState(0x93, 0);
    RwD3D9SetRenderState(0x1A, D3D9RasterCaps & 1u);
    RwD3D9SetRenderState(0x1D, 0);
    RwD3D9SetRenderState(0x8E, 0);
    RwD3D9SetRenderState(0x8B, 0xFFFFFFFFu);
    RwD3D9SetRenderState(0x8F, 0);
    _rwD3D9RenderStateFlushCache();
}

// 0x7FB480 (lote R28A, sin validar; 368 bytes; era llamada por direccion fija desde _rwD3D9Im2DRenderOpen de WindowedMode.cpp y los controladores; `_rwD3D9Im2DRenderOpen` de RenderWare; cdecl sin argumentos -> RwBool): 0xC980E0 = 0.
// Sin HWTRANSFORMANDLIGHT (DevCaps 0xC9BF1C & 0x10000) y con mas de 8 MB de memoria de texturas (GetAvailableTextureMem, ranura 4, sin signo): CreateVertexBuffer(0x40000, uso 0x228, FVF 0, pool 0, &0xC980E8, NULL) (ranura 26);
// si HRESULT == 0: 0xC980E0 = 1 y 0xC980E4 = 0. Despues 0xC980EC = 0 y CreateIndexBuffer(0x4E20, uso 0x228, D3DFMT_INDEX16 = 0x65, pool 0, &0xC980F0, NULL) (ranura 27). Si da 0: crea la declaracion de Im2D con
// RwD3D9CreateVertexDeclaration (0x7FAA30) y {posicion float4 en 0 (POSITIONT), color D3DCOLOR en 0x10, texcoord float2 en 0x14, fin} en 0xC980F4 (el resultado se ignora) y devuelve TRUE. Si falla: libera la declaracion
// (RwD3D9DeleteVertexDeclaration, 0x7FAC10; 0xC980F4 = 0), el bufer de vertices (solo si 0xC980E0 != 0 y 0xC980E8 != 0; Release ranura 2; 0xC980E8 = 0) y el de indices (0xC980F0 = 0) y devuelve FALSE.
// Tamano de la declaracion en pila: 4 elementos de 8 bytes; el EXE no la inicializa antes.
RwBool _rwD3D9Im2DRenderOpen() {
    StaticRef<uint32>(0xC980E0) = 0;
    if ((D3D9DeviceDevCaps & 0x10000u) == 0) {
        void* const device = D3D9Device;
        const uint32 availableMemory = ComCall<kSlotDeviceGetAvailableTextureMem, uint32>(device);
        if (availableMemory > 0x800000u) {
            const int32 result = ComCall<kSlotDeviceCreateVertexBuffer, int32>(D3D9Device, 0x40000u, 0x228u, 0u, 0u, reinterpret_cast<void**>(0xC980E8), static_cast<void*>(nullptr));
            if (result == 0) {
                StaticRef<uint32>(0xC980E0) = 1;
                StaticRef<uint32>(0xC980E4) = 0;
            }
        }
    }
    StaticRef<uint32>(0xC980EC) = 0;
    const int32 indexResult = ComCall<kSlotDeviceCreateIndexBufferR28A, int32>(D3D9Device, 0x4E20u, 0x228u, 0x65u, 0u, reinterpret_cast<void**>(0xC980F0), static_cast<void*>(nullptr));
    if (indexResult == 0) {
        struct VertexElement { uint16 stream; uint16 offset; uint8 type; uint8 method; uint8 usage; uint8 usageIndex; };
        static_assert(sizeof(VertexElement) == 8);
        VertexElement elements[4];
        elements[0] = { 0, 0x00, 3, 0, 9, 0 };      // D3DDECLTYPE_FLOAT4, D3DDECLUSAGE_POSITIONT
        elements[1] = { 0, 0x10, 4, 0, 10, 0 };     // D3DDECLTYPE_D3DCOLOR, D3DDECLUSAGE_COLOR
        elements[2] = { 0, 0x14, 1, 0, 5, 0 };      // D3DDECLTYPE_FLOAT2, D3DDECLUSAGE_TEXCOORD
        elements[3] = { 0xFF, 0, 0x11, 0, 0, 0 };   // D3DDECL_END
        RwD3D9CreateVertexDeclaration(elements, reinterpret_cast<void**>(0xC980F4));
        return TRUE;
    }
    if (StaticRef<void*>(0xC980F4) != nullptr) {
        RwD3D9DeleteVertexDeclaration(StaticRef<void*>(0xC980F4));
        StaticRef<void*>(0xC980F4) = nullptr;
    }
    if (StaticRef<uint32>(0xC980E0) != 0) {
        void* const vertexBuffer = StaticRef<void*>(0xC980E8);
        if (vertexBuffer != nullptr) {
            ComCall<kSlotRelease, ULONG>(vertexBuffer);
            StaticRef<void*>(0xC980E8) = nullptr;
        }
    }
    void* const indexBuffer = StaticRef<void*>(0xC980F0);
    if (indexBuffer != nullptr) {
        ComCall<kSlotRelease, ULONG>(indexBuffer);
        StaticRef<void*>(0xC980F0) = nullptr;
    }
    return FALSE;
}

// 0x7FB5F0 (lote R28A, sin validar; 88 bytes; era llamada por direccion fija desde _rwD3D9DeviceReleaseVideoMemory y _rwD3D9DeviceSystemStop; `_rwD3D9Im2DRenderClose` de RenderWare; cdecl sin argumentos): libera la declaracion de
// Im2D (0xC980F4 si no es NULL: RwD3D9DeleteVertexDeclaration y la anula), el bufer de vertices (0xC980E8, solo si 0xC980E0 != 0 y no es NULL: Release y 0xC980E8 = 0) y el de indices (0xC980F0 si no es NULL: Release y a 0).
void _rwD3D9Im2DRenderClose() {
    if (StaticRef<void*>(0xC980F4) != nullptr) {
        RwD3D9DeleteVertexDeclaration(StaticRef<void*>(0xC980F4));
        StaticRef<void*>(0xC980F4) = nullptr;
    }
    if (StaticRef<uint32>(0xC980E0) != 0) {
        void* const vertexBuffer = StaticRef<void*>(0xC980E8);
        if (vertexBuffer != nullptr) {
            ComCall<kSlotRelease, ULONG>(vertexBuffer);
            StaticRef<void*>(0xC980E8) = nullptr;
        }
    }
    void* const indexBuffer = StaticRef<void*>(0xC980F0);
    if (indexBuffer != nullptr) {
        ComCall<kSlotRelease, ULONG>(indexBuffer);
        StaticRef<void*>(0xC980F0) = nullptr;
    }
}

// 0x7FB810 (lote R28A, sin validar; 206 bytes; era llamada por direccion fija desde las funciones de dibujo de Im2D (0x7FB7C6, 0x7FBAB0, 0x7FBCC6, 0x7FBF3D, 0x7FC170); nombre inferido; cdecl sin argumentos, `jmp 0x7FC200`): prepara el
// dispositivo para dibujar Im2D: _rwD3D9SetVertexDeclaration(0xC980F4), vertex shader = NULL, pixel shader = NULL, estados de render 0x88 = 0 y 0x89 = 0 y la textura de la etapa 0 (_rwD3D9RWGetRasterStage(0), 0x7FDE50): sin
// raster, estados de etapa (0,1) = 3, (0,3) = 0, (0,4) = 3, (0,6) = 0; con raster (0,1) = 4, (0,2) = 2, (0,3) = 0, (0,4) = 4, (0,5) = 2, (0,6) = 0. Despues RwD3D9SetStreamSource(0, 0xC980E8, 0, 0x1C) y vuelca los estados
// pendientes (_rwD3D9RenderStateFlushCache).
void _rwD3D9Im2DRenderBegin() {
    _rwD3D9SetVertexDeclaration(StaticRef<void*>(0xC980F4));
    _rwD3D9SetVertexShader(nullptr);
    _rwD3D9SetPixelShader(nullptr);
    RwD3D9SetRenderState(0x88, 0);
    RwD3D9SetRenderState(0x89, 0);
    if (_rwD3D9RWGetRasterStage(0) == nullptr) {
        RwD3D9SetTextureStageState(0, 1, 3);
        RwD3D9SetTextureStageState(0, 3, 0);
        RwD3D9SetTextureStageState(0, 4, 3);
        RwD3D9SetTextureStageState(0, 6, 0);
    } else {
        RwD3D9SetTextureStageState(0, 1, 4);
        RwD3D9SetTextureStageState(0, 2, 2);
        RwD3D9SetTextureStageState(0, 3, 0);
        RwD3D9SetTextureStageState(0, 4, 4);
        RwD3D9SetTextureStageState(0, 5, 2);
        RwD3D9SetTextureStageState(0, 6, 0);
    }
    RwD3D9SetStreamSource(0, StaticRef<void*>(0xC980E8), 0, 0x1C);
    _rwD3D9RenderStateFlushCache();
}

// 0x7FFEE0 (lote R28A, sin validar; 24 bytes; era un puntero de funcion de la tabla de _rwD3D9RasterToImageGeneric (D3DFMT_L16 = 0x51); nombre inferido; cdecl, (destino, valor)): destino[0..2] = (valor >> 8) & 0xFF (gris de 16
// bits: el byte alto) y destino[3] = 0xFF. Sin x87.
void _rwD3D9PixelConvertL16(RwUInt32* destination, RwUInt32 value) {
    RwUInt8* const bytes = reinterpret_cast<RwUInt8*>(destination);
    const RwUInt8 gray = static_cast<RwUInt8>(value >> 8);
    bytes[0] = gray;
    bytes[1] = gray;
    bytes[2] = gray;
    bytes[3] = 0xFF;
}

// 0x7FFFF0 (lote R28A, sin validar; 491 bytes; no tenia envoltura; nombre inferido, la llama la funcion estandar RwImageFindRasterFormat del controlador (0x7FFF00, 0x800253, 0x800D6D); cdecl, (imagen) -> formato de raster):
// elige el formato de raster (rwRASTERFORMAT*) de una imagen. Profundidad 4 u 8 (paleta de 16 o 256 entradas RGBA): si todas las entradas tienen alfa 0xFF devuelve 0x4600 (PAL4 | 888) / 0x2600 (PAL8 | 888); si alguna no, recorre los
// pixeles (un byte por pixel, filas separadas por stride) y si alguno usa una entrada con alfa != 0xFF devuelve 0x4500 / 0x2500 (con 8888); si no, 0x4600 / 0x2600. Otra profundidad: recorre los alfas (byte +3 de pixeles de 4 bytes;
// filas por stride): un alfa < 0xFF y > 0x0F corta con "alfa completo", uno <= 0x0F marca "alfa de un bit"; las lecturas del alfa de la fila usan ancho/alto con signo. Sin alfa: con profundidad de pantalla (0xC9BEF4, con signo) > 16
// devuelve 0x600 (888) si la textura X8R8G8B8 (0x16) esta soportada (0x4CBDE0) y 0x200 (565) si no; con 16 o menos, 0x200. Con alfa de un bit: con profundidad > 16: 0x500 si A8R8G8B8 (0x15) esta soportado y 0x100 (1555) si no; si no,
// 0x100. Con alfa completo: con profundidad > 16 y A8R8G8B8 soportado, 0x500; si no, 0x300 (4444) si A4R4G4B4 (0x1A) esta soportado y 0x100 si no. Sin x87.
RwInt32 _rwD3D9ImageFindRasterFormat(const RwImage* image) {
    const RwInt32 depth = image->depth;
    if (depth == 4 || depth == 8) {
        const RwUInt8* const palette = reinterpret_cast<const RwUInt8*>(image->palette);
        const RwInt32 entries = depth == 4 ? 16 : 256;
        bool opaquePalette = true;
        for (RwInt32 i = 0; i < entries; ++i) {
            if (palette[i * 4 + 3] != 0xFF) {
                opaquePalette = false;
                break;
            }
        }
        const RwInt32 opaqueFormat = depth == 4 ? 0x4600 : 0x2600;
        if (opaquePalette) {
            return opaqueFormat;
        }
        const RwUInt8* row = image->cpPixels;
        for (RwInt32 y = 0; y < image->height; ++y) {
            const RwUInt8* pixel = row;
            for (RwInt32 x = 0; x < image->width; ++x, ++pixel) {
                if (palette[static_cast<uint32>(*pixel) * 4 + 3] != 0xFF) {
                    return depth == 4 ? 0x4500 : 0x2500;
                }
            }
            row += image->stride;
        }
        return opaqueFormat;
    }
    bool oneBitAlpha = false;
    const RwUInt8* row = image->cpPixels;
    for (RwInt32 y = 0; y < image->height; ++y) {
        const RwUInt8* alpha = row + 3;
        for (RwInt32 x = 0; x < image->width; ++x, alpha += 4) {
            if (*alpha < 0xFF) {
                if (*alpha > 0x0F) {
                    if (D3D9ImageDisplayDepth > 16 && D3D9CheckTextureFormatByAddress(0x15) != 0) {
                        return 0x500;
                    }
                    return D3D9CheckTextureFormatByAddress(0x1A) != 0 ? 0x300 : 0x100;
                }
                oneBitAlpha = true;
            }
        }
        row += image->stride;
    }
    if (oneBitAlpha) {
        if (D3D9ImageDisplayDepth > 16) {
            return D3D9CheckTextureFormatByAddress(0x15) != 0 ? 0x500 : 0x100;
        }
        return 0x100;
    }
    if (D3D9ImageDisplayDepth > 16) {
        return D3D9CheckTextureFormatByAddress(0x16) != 0 ? 0x600 : 0x200;
    }
    return 0x200;
}

// Lote R28A (sin validar): ganchos de las funciones de rasters de RwCore (baraster.c, 0x7FAEC0..0x7FB2D0).
void RwCoreInjectHooksR28A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwRasterUnlock, 0x7FAEC0);
    RH_ScopedGlobalInstall(RwRasterRenderFast, 0x7FAF50);
    RH_ScopedGlobalInstall(RwRasterUnlockPalette, 0x7FAFF0);
    RH_ScopedGlobalInstall(RwRasterDestroy, 0x7FB020);
    RH_ScopedGlobalInstall(RwRasterPushContext, 0x7FB060);
    RH_ScopedGlobalInstall(RwRasterPopContext, 0x7FB110);
    RH_ScopedGlobalInstall(RwRasterGetNumLevels, 0x7FB160);
    RH_ScopedGlobalInstall(RwRasterShowRaster, 0x7FB1A0);
    RH_ScopedGlobalInstall(RwRasterCreate, 0x7FB230);
    RH_ScopedGlobalInstall(RwRasterLock, 0x7FB2D0);
}

// Lote R28A (sin validar): ganchos de las funciones del controlador D3D9 de 0x7FA8B0 a 0x7FFFF0.
void RwD3D9InjectHooksR28A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(_rwD3D9LightsEqual, 0x7FA8B0);
    RH_ScopedGlobalInstall(RwD3D9DeleteVertexDeclaration, 0x7FAC10);
    RH_ScopedGlobalInstall(RwD3D9CameraIsSphereFullyInsideFrustum, 0x7FAD30);
    RH_ScopedGlobalInstall(_rwD3D9DeviceSetRestoreCallback, 0x7FAE20);
    RH_ScopedGlobalInstall(_rwD3D9Im2DRenderOpen, 0x7FB480);
    RH_ScopedGlobalInstall(_rwD3D9Im2DRenderClose, 0x7FB5F0);
    RH_ScopedGlobalInstall(_rwD3D9Im2DRenderBegin, 0x7FB810);
    RH_ScopedGlobalInstall(RwD3D9SetRenderState, 0x7FC2D0);
    RH_ScopedGlobalInstall(RwD3D9GetRenderState, 0x7FC320);
    RH_ScopedGlobalInstall(RwD3D9SetTextureStageState, 0x7FC340);
    RH_ScopedGlobalInstall(RwD3D9SetSamplerState, 0x7FC3C0);
    RH_ScopedGlobalInstall(RwD3D9SetSurfaceProperties, 0x7FC4D0);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateOpen, 0x7FCAC0);
    RH_ScopedGlobalInstall(_rwD3D9RenderStateReset, 0x7FD100);
    RH_ScopedGlobalInstall(RwD3D9SetTexture, 0x7FDE70);
    RH_ScopedGlobalInstall(_rwD3D9PixelConvertL16, 0x7FFEE0);
    RH_ScopedGlobalInstall(_rwD3D9ImageFindRasterFormat, 0x7FFFF0);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R29A (sin validar): conversion de imagenes a rasters del controlador D3D9 de RenderWare (0x8001E0..0x800C80) y conversores RGBA -> pixel (0x800810..0x800C20).
// El listado las daba por RwCore (rwcore.cpp). 0x8001E0 es la funcion estandar rwSTANDARDRASTERSETIMAGE (indice 7 de la tabla que rellena 0x7F72E0), llamada desde RwRasterSetFromImage y desde 0x4CD250.
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
// Luminancia entera del EXE: (30 * R + 59 * G + 11 * B) / 100 (division con signo; el EXE la hace con la constante magica 0x51EB851F).
RwInt32 LumaR29A(const uint8* pixel) {
    return (static_cast<RwInt32>(pixel[1]) * 59 + static_cast<RwInt32>(pixel[2]) * 11 + static_cast<RwInt32>(pixel[0]) * 30) / 100;
}
} // namespace

// 0x800930 (lote R29A, sin validar; 39 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_X1R5G5B5 = 0x18): ((R & 0xF8) << 5 | (G & 0xF8)) << 2 | B >> 3.
RwUInt32 _rwD3D9RGBAToPixelX1R5G5B5(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return ((((static_cast<uint32>(p[0]) & 0xF8u) << 5) | (static_cast<uint32>(p[1]) & 0xF8u)) << 2) | (static_cast<uint32>(p[2]) >> 3);
}

// 0x800960 (lote R29A, sin validar; 39 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_R5G6B5 = 0x17): ((R & 0xF8) << 5 | (G & 0xFC)) << 3 | B >> 3.
RwUInt32 _rwD3D9RGBAToPixelR5G6B5(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return ((((static_cast<uint32>(p[0]) & 0xF8u) << 5) | (static_cast<uint32>(p[1]) & 0xFCu)) << 3) | (static_cast<uint32>(p[2]) >> 3);
}

// 0x800990 (lote R29A, sin validar; 52 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_A1R5G5B5 = 0x19): (((A & 0x80) * 2 | (R & 0xF8)) << 5 | (G & 0xF8)) << 2 | B >> 3.
RwUInt32 _rwD3D9RGBAToPixelA1R5G5B5(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    uint32 value = ((static_cast<uint32>(p[3]) & 0x80u) * 2u) | (static_cast<uint32>(p[0]) & 0xF8u);
    value = (value << 5) | (static_cast<uint32>(p[1]) & 0xF8u);
    return (value << 2) | (static_cast<uint32>(p[2]) >> 3);
}

// 0x8009D0 (lote R29A, sin validar; 50 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_A4R4G4B4 = 0x1A): (((A & 0xF0) << 4 | (R & 0xF0)) << 4 | (G & 0xF0)) | B >> 4.
RwUInt32 _rwD3D9RGBAToPixelA4R4G4B4(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    uint32 value = ((static_cast<uint32>(p[3]) & 0xF0u) << 4) | (static_cast<uint32>(p[0]) & 0xF0u);
    value = (value << 4) | (static_cast<uint32>(p[1]) & 0xF0u);
    return value | (static_cast<uint32>(p[2]) >> 4);
}

// 0x800A10 (lote R29A, sin validar; 43 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_A8R3G3B2 = 0x1D): (((G & 0xE0) | B >> 3) >> 3) | A << 8 | (R & 0xE0).
RwUInt32 _rwD3D9RGBAToPixelA8R3G3B2(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    const uint32 low = (((static_cast<uint32>(p[1]) & 0xE0u) | (static_cast<uint32>(p[2]) >> 3)) >> 3);
    return low | (static_cast<uint32>(p[3]) << 8) | (static_cast<uint32>(p[0]) & 0xE0u);
}

// 0x800A40 (lote R29A, sin validar; 39 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_X4R4G4B4 = 0x1E): ((0xF00 | (R & 0xF0)) << 4 | (G & 0xF0)) | B >> 4 (alfa fijo 0xF).
RwUInt32 _rwD3D9RGBAToPixelX4R4G4B4(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    uint32 value = (static_cast<uint32>(p[0]) & 0xF0u) | 0xF00u;
    value = (value << 4) | (static_cast<uint32>(p[1]) & 0xF0u);
    return value | (static_cast<uint32>(p[2]) >> 4);
}

// 0x800A70 (lote R29A, sin validar; 39 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_L6V5U5 = 0x3D): (((B & 0xFC) << 6 | (G & 0xF8)) << 2) | R >> 3.
RwUInt32 _rwD3D9RGBAToPixelL6V5U5(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    const uint32 value = ((static_cast<uint32>(p[2]) & 0xFCu) << 6) | (static_cast<uint32>(p[1]) & 0xF8u);
    return (value << 2) | (static_cast<uint32>(p[0]) >> 3);
}

// 0x800AA0 (lote R29A, sin validar; 34 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_X8R8G8B8 = 0x16): 0xFF000000 | R << 16 | G << 8 | B.
RwUInt32 _rwD3D9RGBAToPixelX8R8G8B8(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return ((((0xFFFFFF00u | p[0]) << 8) | p[1]) << 8) | p[2];
}

// 0x800AD0 (lote R29A, sin validar; 34 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_X8B8G8R8 = 0x21 y D3DFMT_X8L8V8U8 = 0x3E): 0xFF000000 | B << 16 | G << 8 | R.
RwUInt32 _rwD3D9RGBAToPixelX8B8G8R8(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return ((((0xFFFFFF00u | p[2]) << 8) | p[1]) << 8) | p[0];
}

// 0x800B00 (lote R29A, sin validar; 34 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_A8R8G8B8 = 0x15): A << 24 | R << 16 | G << 8 | B.
RwUInt32 _rwD3D9RGBAToPixelA8R8G8B8(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return ((((((static_cast<uint32>(p[3]) << 8) | p[0]) << 8) | p[1]) << 8)) | p[2];
}

// 0x800B30 (lote R29A, sin validar; 34 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_A8B8G8R8 = 0x20 y D3DFMT_Q8W8V8U8 = 0x3F): A << 24 | B << 16 | G << 8 | R.
RwUInt32 _rwD3D9RGBAToPixelA8B8G8R8(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return ((((((static_cast<uint32>(p[3]) << 8) | p[2]) << 8) | p[1]) << 8)) | p[0];
}

// 0x800B60 (lote R29A, sin validar; 14 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_V8U8 = 0x3C y D3DFMT_CxV8U8 = 0x75): R | G << 8.
RwUInt32 _rwD3D9RGBAToPixelV8U8(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return (static_cast<uint32>(p[1]) << 8) | p[0];
}

// 0x800B70 (lote R29A, sin validar; 86 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_L8 = 0x32): luminancia * A / 255 (division sin signo; luminancia = (30 * R + 59 * G + 11 * B) / 100).
RwUInt32 _rwD3D9RGBAToPixelL8(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return (static_cast<uint32>(LumaR29A(p)) * static_cast<uint32>(p[3])) / 255u;
}

// 0x800BD0 (lote R29A, sin validar; 75 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_A8L8 = 0x33): A << 8 | luminancia (luminancia = (30 * R + 59 * G + 11 * B) / 100, sin acotar).
RwUInt32 _rwD3D9RGBAToPixelA8L8(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    return static_cast<uint32>(LumaR29A(p)) | (static_cast<uint32>(p[3]) << 8);
}

// 0x800C20 (lote R29A, sin validar; 81 bytes; nombre inferido; cdecl, (puntero a R, G, B, A); D3DFMT_L16 = 0x51): ((30 * R + 59 * G + 11 * B) * A / 25500) * 255 (todo con signo; el EXE usa la constante 0x291EE171).
RwUInt32 _rwD3D9RGBAToPixelL16(const void* rgba) {
    const uint8* p = static_cast<const uint8*>(rgba);
    const RwInt32 sum = static_cast<RwInt32>(p[1]) * 59 + static_cast<RwInt32>(p[2]) * 11 + static_cast<RwInt32>(p[0]) * 30;
    return static_cast<RwUInt32>((sum * static_cast<RwInt32>(p[3]) / 25500) * 255);
}

// 0x800810 (lote R29A, sin validar; 119 bytes; nombre inferido; cdecl, (D3DFORMAT); tabla de saltos con indices por formato - 0x15 y 14 destinos): devuelve la funcion de conversion RGBA -> pixel de ese formato o NULL
// (formato fuera de los 18 listados; el EXE devuelve 0 y los llamadores harian call 0). Formatos: 0x15 A8R8G8B8, 0x16 X8R8G8B8, 0x17 R5G6B5, 0x18 X1R5G5B5, 0x19 A1R5G5B5, 0x1A A4R4G4B4, 0x1D A8R3G3B2, 0x1E X4R4G4B4,
// 0x20 A8B8G8R8, 0x21 X8B8G8R8, 0x32 L8, 0x33 A8L8, 0x3C V8U8, 0x3D L6V5U5, 0x3E X8L8V8U8 (= X8B8G8R8), 0x3F Q8W8V8U8 (= A8B8G8R8), 0x51 L16, 0x75 CxV8U8 (= V8U8).
RwD3D9RGBAToPixelFn _rwD3D9GetRGBAToPixelConverter(RwUInt32 d3dFormat) {
    switch (d3dFormat) {
    case 0x15: return _rwD3D9RGBAToPixelA8R8G8B8;
    case 0x16: return _rwD3D9RGBAToPixelX8R8G8B8;
    case 0x17: return _rwD3D9RGBAToPixelR5G6B5;
    case 0x18: return _rwD3D9RGBAToPixelX1R5G5B5;
    case 0x19: return _rwD3D9RGBAToPixelA1R5G5B5;
    case 0x1A: return _rwD3D9RGBAToPixelA4R4G4B4;
    case 0x1D: return _rwD3D9RGBAToPixelA8R3G3B2;
    case 0x1E: return _rwD3D9RGBAToPixelX4R4G4B4;
    case 0x20: return _rwD3D9RGBAToPixelA8B8G8R8;
    case 0x21: return _rwD3D9RGBAToPixelX8B8G8R8;
    case 0x32: return _rwD3D9RGBAToPixelL8;
    case 0x33: return _rwD3D9RGBAToPixelA8L8;
    case 0x3C: return _rwD3D9RGBAToPixelV8U8;
    case 0x3D: return _rwD3D9RGBAToPixelL6V5U5;
    case 0x3E: return _rwD3D9RGBAToPixelX8B8G8R8;
    case 0x3F: return _rwD3D9RGBAToPixelA8B8G8R8;
    case 0x51: return _rwD3D9RGBAToPixelL16;
    case 0x75: return _rwD3D9RGBAToPixelV8U8;
    default:   return nullptr;
    }
}

// 0x800C80 (lote R29A, sin validar; 1805 bytes; nombre inferido; cdecl, (raster, imagen), sin valor de retorno; la llaman 0x8001E0 y ella misma): copia una imagen a un raster de 4 u 8 bits o de luminancia ya bloqueado.
// Extension D3D9 del raster = raster + [0xB4E9E0]. Raster con (cFormat & 0xF) == 4 (luminancia 8 bits): segun el D3DFORMAT de la extension (0x1B R3G3B2, 0x34 A4L4, 0x1C A8, otro = L8) convierte cada pixel (imagen de
// 4/8 bits: antes convierte la paleta a una tabla de 256 bytes; otra profundidad: pixeles de 4 bytes R, G, B, A) y lo escribe fila a fila (alto y ancho del RASTER, con signo). Si no: imagen de 4/8 bits -> copia de bytes (de golpe
// si los strides de imagen y raster coinciden con el ancho del raster, si no fila a fila) y, con extension+0xB == 0 y paleta de raster, copia la paleta (4 << profundidad bytes). Imagen de otra profundidad: la cuantifica a una imagen
// temporal de la profundidad del raster (RwImageCreate + RwImageAllocatePixels + RwPalQuant*; si RwImageCreate o RwPalQuantInit fallan sale sin mas, y en el segundo caso sin destruir la imagen), si la extension tiene alfa y la
// imagen temporal es 888 (formato & 0xF00 == 0x600) deja el raster en (cFormat & 0xFE) | 6 y alfa 0, y se llama a si misma con la temporal, que destruye.
void _rwD3D9RasterSetFromImageIndexed(RwRaster* raster, RwImage* image) {
    D3D9RasterExtLayout& ext = D3D9RasterExt(raster);
    if ((raster->cFormat & 0xF) == 4) {
        const uint32 d3dFormat = ext.d3dFormat;
        const auto convert = [d3dFormat](const uint8* p) -> uint8 {
            if (d3dFormat == 0x1B) {
                uint8 value = static_cast<uint8>(p[1] & 0xE7);
                value = static_cast<uint8>(value | static_cast<uint8>(p[2] >> 3));
                value = static_cast<uint8>(value >> 3);
                return static_cast<uint8>(value | static_cast<uint8>(p[0] & 0xE0));
            }
            if (d3dFormat == 0x34) {
                return static_cast<uint8>(static_cast<uint8>(LumaR29A(p) >> 4) | static_cast<uint8>(p[3] & 0xF0));
            }
            if (d3dFormat == 0x1C) {
                return p[3];
            }
            return static_cast<uint8>((static_cast<uint32>(LumaR29A(p)) * static_cast<uint32>(p[3])) / 255u);
        };
        const bool paletted = image->depth == 4 || image->depth == 8;
        uint8 table[256] = {}; // el EXE deja sin inicializar las entradas que la paleta no cubre
        if (paletted) {
            const uint8* palette = reinterpret_cast<const uint8*>(image->palette);
            const RwInt32 count = 1 << (image->depth & 0x1F);
            for (RwInt32 i = 0; i < count; ++i) {
                table[i] = convert(palette + i * 4);
            }
        }
        for (RwInt32 y = 0; y < raster->height; ++y) {
            const uint8* source = image->cpPixels + image->stride * y;
            uint8* destination = raster->cpPixels + raster->stride * y;
            for (RwInt32 x = 0; x < raster->width; ++x, ++destination) {
                if (paletted) {
                    *destination = table[source[x]];
                } else {
                    *destination = convert(source);
                    source += 4;
                }
            }
        }
        return;
    }

    if (image->depth == 4 || image->depth == 8) {
        if (image->stride == raster->stride && raster->width == raster->stride) {
            memcpy(raster->cpPixels, image->cpPixels, static_cast<size_t>(static_cast<uint32>(raster->height * raster->width)));
        } else {
            for (RwInt32 y = 0; y < raster->height; ++y) {
                memcpy(raster->cpPixels + raster->stride * y, image->cpPixels + image->stride * y, static_cast<size_t>(static_cast<uint32>(raster->width)));
            }
        }
        if (ext.lockedMipLevel == 0 && raster->palette != nullptr) {
            memcpy(raster->palette, image->palette, static_cast<size_t>((1u << (image->depth & 0x1F)) << 2));
        }
        return;
    }

    RwImage* const temporary = RwImageCreate(image->width, image->height, raster->depth);
    if (temporary == nullptr) {
        return;
    }
    RwImageAllocatePixels(temporary);
    static_assert(sizeof(RwPalQuant) == 0x4008);
    RwPalQuant quant;
    if (!RwPalQuantInit(&quant)) {
        return;
    }
    RwPalQuantAddImage(&quant, image, 1.0f);
    RwPalQuantResolvePalette(temporary->palette, 1 << (raster->depth & 0x1F), &quant);
    RwPalQuantMatchImage(temporary->cpPixels, temporary->stride, temporary->depth, FALSE, &quant, image);
    RwPalQuantTerm(&quant);
    if (ext.alpha != 0 && (_rwD3D9ImageFindRasterFormat(temporary) & 0xF00) == 0x600) {
        raster->cFormat = static_cast<RwUInt8>((raster->cFormat & 0xFE) | 6);
        ext.alpha = 0;
    }
    _rwD3D9RasterSetFromImageIndexed(raster, temporary);
    RwImageDestroy(temporary);
}

// 0x8001E0 (lote R29A, sin validar; 1420 bytes; nombre inferido: funcion estandar rwSTANDARDRASTERSETIMAGE; cdecl, (raster, imagen, 0 sin uso); devuelve TRUE salvo en los fallos indicados): copia una imagen a un raster del
// controlador D3D9 (RwRasterSetFromImage). Extension = raster + [0xB4E9E0]. 1) extension+0xA con nibble alto (comprimido): RwError { 1, _rwerror(2, 0x8E2638) } y devuelve 0. 2) alfa de la extension (+8): raster tipo 0 ->
// tabla 0x85C675 + 8 * ((_rwD3D9ImageFindRasterFormat(imagen) >> 8) & 0xF); otro tipo -> D3DFORMAT 0x29 = (formato & 0xF00) == 0x500, si no el byte de alfa de 0xB4E7E0 + 4 * D3DFORMAT; con alfa y raster hijo tambien el
// alfa de la extension del padre = 1. 3) si el ancho o el alto del raster difieren de los de la imagen, la reescala con RwImageCreateResample (0x80CD10). 4) bloquea el raster (RwRasterLock nivel 0, modo 5) salvo que
// privateFlags & 4 (ya bloqueado); 0 si falla (sin liberar la imagen reescalada). 5) con paleta (cFormat << 8) & 0x6000 bloquea la paleta (RwRasterLockPalette modo 5) salvo privateFlags & 0x10. 6) segun la profundidad del
// raster: 4/8 -> _rwD3D9RasterSetFromImageIndexed; 16 -> imagen de 32 bits: conversor de 0x800810 por D3DFORMAT (el EXE inlinea los cuatro de 0x17..0x1A con la misma formula), imagen de 4/8 bits: paleta de 16/256 words
// convertida y pixeles por indice; 24/32 -> _rwD3D9ImageToRaster (0x801390); 64/128 -> nada; otra -> RwError { 1, _rwerror(0x8000000C) }. 7) desbloquea paleta (si se bloqueo aqui) y raster (si se bloqueo aqui), destruye la
// imagen reescalada y devuelve TRUE. El EXE deja sin inicializar la bandera "paleta ya bloqueada" cuando la bloquea el: aqui vale 0 (intencion del original).
RwBool _rwD3D9RasterSetFromImage(RwRaster* raster, RwImage* image, RwInt32 /*sin uso*/) {
    D3D9RasterExtLayout& ext = D3D9RasterExt(raster);
    if (ext.mipFlags & 0xF0) {
        RwRaiseError(2, reinterpret_cast<const char*>(0x8E2638));
        return FALSE;
    }
    const uint32 rasterFormat = static_cast<uint32>(raster->cFormat) << 8;
    if (raster->cType == 0) {
        const RwInt32 imageFormat = _rwD3D9ImageFindRasterFormat(image);
        ext.alpha = *reinterpret_cast<const uint8*>(0x85C675 + static_cast<uintptr_t>((imageFormat >> 8) & 0xF) * 8);
    } else if (ext.d3dFormat == 0x29) {
        ext.alpha = (rasterFormat & 0xF00) == 0x500 ? 1 : 0;
    } else {
        ext.alpha = PixelFormatAlphaByte(ext.d3dFormat);
    }
    if (ext.alpha != 0) {
        RwRaster* const parent = raster->parent;
        if (raster != parent) {
            D3D9RasterExt(parent).alpha = 1;
        }
    }
    RwImage* current = image;
    if (raster->width != image->width || raster->height != image->height) {
        current = RwImageCreateResample(current, raster->width, raster->height);
    }
    bool rasterWasLocked = false;
    if (raster->privateFlags & 4) {
        rasterWasLocked = true;
    } else if (RwRasterLock(raster, 0, 5) == nullptr) {
        return FALSE;
    }
    const uint32 paletteFlags = rasterFormat & 0x6000;
    bool paletteWasLocked = false;
    if (paletteFlags != 0) {
        if (raster->privateFlags & 0x10) {
            paletteWasLocked = true;
        } else if (RwRasterLockPalette(raster, 5) == nullptr) {
            return FALSE;
        }
    }

    switch (raster->depth) {
    case 4:
    case 8:
        _rwD3D9RasterSetFromImageIndexed(raster, current);
        break;
    case 16:
        if (current->depth == 4 || current->depth == 8) {
            const RwD3D9RGBAToPixelFn convert = _rwD3D9GetRGBAToPixelConverter(ext.d3dFormat);
            uint16 palette[256] = {};
            const RwInt32 byteCount = current->depth == 4 ? 0x40 : 0x400;
            for (RwInt32 offset = 0, i = 0; offset < byteCount; offset += 4, ++i) {
                palette[i] = static_cast<uint16>(convert(reinterpret_cast<const uint8*>(current->palette) + offset));
            }
            for (RwInt32 y = 0; y < raster->height; ++y) {
                const uint8* source = current->cpPixels + current->stride * y;
                uint8* destination = raster->cpPixels + raster->stride * y;
                for (RwInt32 x = 0; x < raster->width; ++x, ++source, destination += 2) {
                    *reinterpret_cast<uint16*>(destination) = palette[*source];
                }
            }
        } else if (current->depth == 32) {
            const RwD3D9RGBAToPixelFn convert = _rwD3D9GetRGBAToPixelConverter(ext.d3dFormat);
            for (RwInt32 y = 0; y < raster->height; ++y) {
                const uint8* source = current->cpPixels + current->stride * y;
                uint8* destination = raster->cpPixels + raster->stride * y;
                for (RwInt32 x = 0; x < raster->width; ++x, source += 4, destination += 2) {
                    *reinterpret_cast<uint16*>(destination) = static_cast<uint16>(convert(source));
                }
            }
        }
        break;
    case 24:
    case 32:
        _rwD3D9ImageToRaster(raster, current);
        break;
    case 64:
    case 128:
        break;
    default:
        RwRaiseError(static_cast<RwInt32>(0x8000000C));
        break;
    }

    if (paletteFlags != 0 && !paletteWasLocked) {
        RwRasterUnlockPalette(raster);
    }
    if (!rasterWasLocked) {
        RwRasterUnlock(raster);
    }
    if (current != image) {
        RwImageDestroy(current);
    }
    return TRUE;
}

// Lote R29A (sin validar): ganchos de las imagenes de RwCore (baimage.c, 0x8026E0..0x802A20).
void RwCoreInjectHooksR29A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwImageCreate, 0x8026E0);
    RH_ScopedGlobalInstall(RwImageDestroy, 0x802740);
    RH_ScopedGlobalInstall(RwImageAllocatePixels, 0x8027A0);
    RH_ScopedGlobalInstall(RwImageFreePixels, 0x802860);
    RH_ScopedGlobalInstall(RwImageMakeMask, 0x802A20);
}

// Lote R29A (sin validar): ganchos del controlador D3D9: imagen -> raster y conversores RGBA -> pixel (0x8001E0..0x800C80).
void RwD3D9InjectHooksR29A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwD3D9");

    RH_ScopedGlobalInstall(_rwD3D9RasterSetFromImage, 0x8001E0);
    RH_ScopedGlobalInstall(_rwD3D9GetRGBAToPixelConverter, 0x800810);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelX1R5G5B5, 0x800930);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelR5G6B5, 0x800960);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelA1R5G5B5, 0x800990);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelA4R4G4B4, 0x8009D0);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelA8R3G3B2, 0x800A10);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelX4R4G4B4, 0x800A40);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelL6V5U5, 0x800A70);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelX8R8G8B8, 0x800AA0);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelX8B8G8R8, 0x800AD0);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelA8R8G8B8, 0x800B00);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelA8B8G8R8, 0x800B30);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelV8U8, 0x800B60);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelL8, 0x800B70);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelA8L8, 0x800BD0);
    RH_ScopedGlobalInstall(_rwD3D9RGBAToPixelL16, 0x800C20);
    RH_ScopedGlobalInstall(_rwD3D9RasterSetFromImageIndexed, 0x800C80);
}

// Lote R30A (sin validar): ganchos del resto de las imagenes de RwCore (baimage.c, 0x802AF0..0x803FE0), RwImageSetFromRaster y compania (0x804250..0x8042C0), los flujos de textura (0x8045A0..0x8046E0)
// y el cierre del modulo de tuberias Rx (0x804F60).
void RwCoreInjectHooksR30A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(RwImageApplyMask, 0x802AF0);
    RH_ScopedGlobalInstall(RwImageRead, 0x802FD0);
    RH_ScopedGlobalInstall(_rwImageForAllPaths, 0x8030C0);
    RH_ScopedGlobalInstall(_rwImageScratchBufferReserve, 0x803210);
    RH_ScopedGlobalInstall(_rwImageFindFormatExtension, 0x803310);
    RH_ScopedGlobalInstall(RwImageReadMaskedImage, 0x8035C0);
    RH_ScopedGlobalInstall(_rwImageCopySameDepth, 0x803790);
    RH_ScopedGlobalInstall(_rwImageCopyConvertDepth, 0x803840);
    RH_ScopedGlobalInstall(RwImageCopy, 0x803950);
    RH_ScopedGlobalInstall(RwImageGammaCorrect, 0x803E30);
    RH_ScopedGlobalInstall(RwImageSetGamma, 0x803FE0);
    RH_ScopedGlobalInstall(RwImageSetFromRaster, 0x804250);
    RH_ScopedGlobalInstall(RwRasterSetFromImage, 0x804290);
    RH_ScopedGlobalInstall(RwImageFindRasterFormat, 0x8042C0);
    RH_ScopedGlobalInstall(RwTextureStreamGetSize, 0x8045A0);
    RH_ScopedGlobalInstall(RwTextureStreamWrite, 0x8045E0);
    RH_ScopedGlobalInstall(RwTextureStreamRead, 0x8046E0);
    RH_ScopedGlobalInstall(_rxPipelineClose, 0x804F60);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R31A (sin validar): tuberias de Rx (rxpipeline.c, 0x804FE0..0x806990): apertura del modulo, destruccion de nodos y tuberias, almacen temporal, bloqueo (RxPipelineLock) y desbloqueo
// (RxLockedPipeUnlock) con su ordenacion topologica, clonado de definiciones de nodo y recolocacion del "superbloque" de cada tuberia.
//
// Datos del modulo de tuberias (en el EXE; se leen siempre por direccion fija, sin copias):
//   0xC9BC60 desplazamiento del modulo dentro de RwEngineInstance; datos: +0x00 lista libre de tuberias, +0x04 vector de estados de render por defecto (RxRenderStateVector),
//            +0x30/+0x34 a cero al abrir, +0x38 "M" (nodos maximos por tuberia: 0x8E269C)
//   0xC9A6AC bandera de modulo abierto, 0xC9A6B0 monton global (RxHeap), 0xC9A688 espacio de la lista libre (0x34 bytes por entrada), 0xC9A6B4/0xC9A6B8 extremos alto/bajo del almacen temporal
// Superbloque de una tuberia bloqueada (tamano minimo 180 * M): nodos (0x28 bytes cada uno, M reservados), M bloques de 0x80 bytes con las salidas de cada nodo, y M elementos de 12 bytes
// (RxPipelineNodeTopSortData) a partir de nodes + 168 * M; al desbloquear las salidas se empaquetan justo detras de los nodos.
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
uint8* RxModuleData() {
    return reinterpret_cast<uint8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9BC60);
}
RwUInt32 RxMaxNodes() {
    return *reinterpret_cast<RwUInt32*>(RxModuleData() + 0x38);
}
RwFreeList*& RxPipelineFreeListSlot() {
    return *reinterpret_cast<RwFreeList**>(RxModuleData());
}
template<typename T>
void RxShiftPointer(T*& pointer, uintptr_t delta) {
    if (pointer != nullptr) {
        pointer = reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(pointer) + delta);
    }
}
// Copia de cadena hasta el NUL incluido (bucle de bytes del EXE).
void RxCopyString(char* destination, const char* source) {
    char c;
    do {
        c = *source++;
        *destination++ = c;
    } while (c != 0);
}
// Tamano de una cadena en la reserva: (longitud + 1) redondeado a 4.
RwUInt32 RxStringSlot(const char* text) {
    return (static_cast<RwUInt32>(strlen(text)) + 4u) & ~3u;
}
constexpr RwInt32 kRxErrNullPointer = static_cast<RwInt32>(0x80000016);
constexpr RwInt32 kRxErrOutOfMemory = static_cast<RwInt32>(0x80000013);
static_assert(offsetof(RxNodeDefinition, nodeMethods) == 4 && offsetof(RxNodeMethods, pipelineNodeTerm) == 0x10 && offsetof(RxNodeMethods, nodeTerm) == 8);
static_assert(offsetof(RxNodeDefinition, io) == 0x20 && offsetof(RxIoSpec, outputs) == 0x10 && offsetof(RxNodeDefinition, InputPipesCnt) == 0x3C && offsetof(RxNodeDefinition, editable) == 0x38);
static_assert(offsetof(RxPipelineNode, initializationData) == 0x20 && offsetof(RxPipelineNode, initializationDataSize) == 0x24 && sizeof(RxPipelineNodeTopSortData) == 0xC);
static_assert(offsetof(RxPipeline, superBlock) == 0x20 && offsetof(RxPipeline, superBlockSize) == 0x24 && offsetof(RxPipeline, entryPoint) == 0x28 && offsetof(RxPipeline, embeddedPacket) == 0x14);
} // namespace

// Contexto de la ordenacion topologica: la tuberia y cuantos nodos llevan ya su sitio definitivo (dos dwords consecutivos en la pila de RxLockedPipeUnlock).
struct RxPipelineTopSortContext {
    RxPipeline* pipeline;
    RwUInt32    numSorted;
};

// 0x804FE0 (lote R31A, sin validar; 210 bytes; era una envoltura; cdecl, sin argumentos; llamador: 0x807C49): abre el modulo de tuberias. Si la bandera 0xC9A6AC ya es distinta de 0 devuelve 0.
// Si no: crea el monton global (RxHeapCreate(0x8E2698), 0x809F90) en 0xC9A6B0 (NULL: devuelve 0), crea la lista libre de tuberias (RwFreeListCreateAndPreallocateSpace 0x801B70: entrada 0x34,
// 0x8E26A0 por bloque, alineacion 4, 0x8E26A4 bloques, espacio 0xC9A688, pista 0x40409) en la ranura del modulo (sin memoria: RxHeapDestroy y monton a NULL, devuelve 0). Con exito: M (+0x38) =
// 0x8E269C, vector de estados de render por defecto (0x80EA40) sobre modulo + 4, +0x34 y +0x30 a 0, bandera a 1 y devuelve 1.
RwBool _rxPipelineOpen() {
    if (StaticRef<RwInt32>(0xC9A6AC) != 0) {
        return FALSE;
    }
    RxHeap* const heap = RxHeapCreate(StaticRef<RwUInt32>(0x8E2698));
    StaticRef<RxHeap*>(0xC9A6B0) = heap;
    if (heap == nullptr) {
        return FALSE;
    }
    RwFreeList* const freeList = RwFreeListCreateAndPreallocateSpace(0x34, StaticRef<RwInt32>(0x8E26A0), 4, StaticRef<RwInt32>(0x8E26A4), reinterpret_cast<RwFreeList*>(0xC9A688), 0x40409);
    RxPipelineFreeListSlot() = freeList;
    if (RxPipelineFreeListSlot() == nullptr) {
        RxHeapDestroy(StaticRef<RxHeap*>(0xC9A6B0));
        StaticRef<RxHeap*>(0xC9A6B0) = nullptr;
        return FALSE;
    }
    *reinterpret_cast<RwUInt32*>(RxModuleData() + 0x38) = StaticRef<RwUInt32>(0x8E269C);
    RxRenderStateVectorSetDefaultRenderStateVector(reinterpret_cast<RxRenderStateVector*>(RxModuleData() + 4));
    *reinterpret_cast<RwUInt32*>(RxModuleData() + 0x34) = 0;
    *reinterpret_cast<RwUInt32*>(RxModuleData() + 0x30) = 0;
    StaticRef<RwInt32>(0xC9A6AC) = 1;
    return TRUE;
}

// 0x8050C0 (lote R31A, sin validar; 546 bytes; cdecl (nodo, tuberia); nombre inferido: destruccion de un nodo de tuberia; llamadores: 0x805839 (_rxPipelineDestroy) y 0x806D2A, 0x806D70, 0x807024,
// 0x80705D): devuelve el nodo. Tuberia desbloqueada: pipelineNodeTerm (+0x14 de la definicion) si existe; InputPipesCnt (+0x3C) -= 1 y, si llega a 0, nodeTerm (+0x0C) y, si la definicion es
// editable (+0x38), RwFree y nodeDef = NULL; libera los datos de inicializacion (+0x20, y pone +0x20/+0x24 a 0), pone el nodo a cero (0x28 bytes) y numNodes -= 1. Tuberia bloqueada: libera los datos
// de inicializacion; si InputPipesCnt == 0 y la definicion es editable la libera (nodeDef = NULL); si el nodo no es el ultimo (indice = (nodo - nodes) / 40, con signo) corre los bloques de 0x80 bytes de
// salidas (a partir de nodes + 40 * M + 128 * indice), copia 12 bytes por elemento DESDE EL ELEMENTO 0 (nodes + 168 * M; el EXE no suma el indice), corre los nodos y resta 0x80 a sus salidas y 0xC a
// su topSortData, y renumera las salidas de todos los nodos (indice -> 0xFFFFFFFF, mayores -> menos 1; el comparador es sin signo: un 0xFFFFFFFF pasa a 0xFFFFFFFE). Al final numNodes -= 1.
RxPipelineNode* _rxPipelineNodeDestroy(RxPipelineNode* node, RxPipeline* pipeline) {
    if (pipeline->locked == 0) {
        {
            const RxPipelineNodeTermFn term = node->nodeDef->nodeMethods.pipelineNodeTerm;
            if (term != nullptr) {
                term(node);
            }
        }
        node->nodeDef->InputPipesCnt = node->nodeDef->InputPipesCnt - 1;
        if (node->nodeDef->InputPipesCnt == 0) {
            RxNodeDefinition* definition = node->nodeDef;
            const RxNodeTermFn nodeTerm = definition->nodeMethods.nodeTerm;
            if (nodeTerm != nullptr) {
                nodeTerm(definition);
            }
            definition = node->nodeDef;
            if (definition->editable != 0) {
                RwFree(definition);
                node->nodeDef = nullptr;
            }
        }
        if (node->initializationData != nullptr) {
            RwFree(node->initializationData);
            node->initializationData = nullptr;
            node->initializationDataSize = 0;
        }
        memset(node, 0, sizeof(RxPipelineNode));
        pipeline->numNodes = pipeline->numNodes - 1;
        return node;
    }

    if (node->initializationData != nullptr) {
        RwFree(node->initializationData);
        node->initializationData = nullptr;
        node->initializationDataSize = 0;
    }
    {
        RxNodeDefinition* const definition = node->nodeDef;
        if (definition->InputPipesCnt == 0 && definition->editable != 0) {
            RwFree(definition);
            node->nodeDef = nullptr;
        }
    }
    const RwUInt32 index = static_cast<RwUInt32>(static_cast<RwInt32>(reinterpret_cast<uint8*>(node) - reinterpret_cast<uint8*>(pipeline->nodes)) / static_cast<RwInt32>(sizeof(RxPipelineNode)));
    if (index < pipeline->numNodes - 1) {
        // Bloques de 0x80 bytes con las salidas de cada nodo.
        {
            uint8* destination = reinterpret_cast<uint8*>(pipeline->nodes) + index * 0x80 + RxMaxNodes() * 0x28;
            uint8* source = destination + 0x80;
            RwUInt32 current = index;
            do {
                memcpy(destination, source, 0x80);
                destination = source;
                source += 0x80;
                ++current;
            } while (current < pipeline->numNodes - 1);
        }
        // Elementos de 12 bytes (a partir del elemento 0, como el EXE).
        {
            uint8* destination = reinterpret_cast<uint8*>(pipeline->nodes) + RxMaxNodes() * 0xA8;
            uint8* source = destination + 0xC;
            RwUInt32 current = index;
            do {
                memcpy(destination, source, 0xC);
                destination = source;
                source += 0xC;
                ++current;
            } while (current < pipeline->numNodes - 1);
        }
        // Nodos.
        {
            RwUInt32 current = index;
            RwUInt32 offset = index * sizeof(RxPipelineNode);
            do {
                uint8* const base = reinterpret_cast<uint8*>(pipeline->nodes);
                memcpy(base + offset, base + offset + sizeof(RxPipelineNode), sizeof(RxPipelineNode));
                RxPipelineNode* const moved = reinterpret_cast<RxPipelineNode*>(base + offset);
                moved->outputs = reinterpret_cast<RwUInt32*>(reinterpret_cast<uint8*>(moved->outputs) - 0x80);
                moved->topSortData = reinterpret_cast<RxPipelineNodeTopSortData*>(reinterpret_cast<uint8*>(moved->topSortData) - 0xC);
                ++current;
                offset += sizeof(RxPipelineNode);
            } while (current < pipeline->numNodes - 1);
        }
        // Renumeracion de las salidas.
        if (pipeline->numNodes - 1 != 0) {
            RwUInt32 nodeIndex = 0;
            do {
                if (pipeline->nodes[nodeIndex].numOutputs != 0) {
                    RwUInt32 outputIndex = 0;
                    do {
                        RwUInt32* const slot = &pipeline->nodes[nodeIndex].outputs[outputIndex];
                        const RwUInt32 value = *slot;
                        if (value >= index) {
                            if (value == index) {
                                *slot = 0xFFFFFFFFu;
                            } else {
                                *slot = value - 1;
                            }
                        }
                        ++outputIndex;
                    } while (outputIndex < pipeline->nodes[nodeIndex].numOutputs);
                }
                ++nodeIndex;
            } while (nodeIndex < pipeline->numNodes - 1);
        }
    }
    pipeline->numNodes = pipeline->numNodes - 1;
    return node;
}

// 0x805820 (lote R31A, sin validar; 111 bytes; era una envoltura; cdecl (tuberia); llamadores: 0x5D5FAA, 0x74EB31, 0x7C86FD y otros): tuberia NULL: nada. Si no: destruye uno a uno (0x8050C0,
// con el numero de nodos leido una vez al principio) los numNodes nodos desde nodes, pone nodes a NULL, libera el superbloque (+0x20; superBlock y superBlockSize a 0) y devuelve la tuberia a la
// lista libre del modulo (RwFreeListFree).
void _rxPipelineDestroy(RxPipeline* Pipeline) {
    if (Pipeline == nullptr) {
        return;
    }
    const RwUInt32 numNodes = Pipeline->numNodes;
    RxPipelineNode* node = Pipeline->nodes;
    if (numNodes != 0) {
        RwUInt32 remaining = numNodes;
        do {
            _rxPipelineNodeDestroy(node, Pipeline);
            node = reinterpret_cast<RxPipelineNode*>(reinterpret_cast<uint8*>(node) + sizeof(RxPipelineNode));
            --remaining;
        } while (remaining != 0);
    }
    void* const superBlock = Pipeline->superBlock;
    Pipeline->nodes = nullptr;
    if (superBlock != nullptr) {
        RwFree(superBlock);
        Pipeline->superBlock = nullptr;
        Pipeline->superBlockSize = 0;
    }
    RwFreeListFree(RxPipelineFreeListSlot(), Pipeline);
}

// 0x805890 (lote R31A, sin validar; 84 bytes; cdecl (tamano); nombre inferido: reserva del almacen temporal de Rx por el extremo ALTO; llamadores: 0x80F464, 0x80F47D, 0x80F9B4, 0x80FCB6):
// tamano = (size + 3) & ~3; alto (0xC9A6B4) -= tamano; si el nuevo alto es >= bajo (0xC9A6B8) devuelve el alto. Si no: restituye el alto, error 0x80000013 con el tamano y devuelve NULL.
void* RxScratchAllocHigh(RwUInt32 size) {
    const RwUInt32 rounded = (size + 3u) & ~3u;
    const RwUInt32 low = StaticRef<RwUInt32>(0xC9A6B8);
    const RwUInt32 high = StaticRef<RwUInt32>(0xC9A6B4) - rounded;
    StaticRef<RwUInt32>(0xC9A6B4) = high;
    if (high >= low) {
        return reinterpret_cast<void*>(static_cast<uintptr_t>(high));
    }
    StaticRef<RwUInt32>(0xC9A6B4) = high + rounded;
    RwRaiseError(kRxErrOutOfMemory, rounded);
    return nullptr;
}

// 0x8058F0 (lote R31A, sin validar; 90 bytes; cdecl (tamano); nombre inferido: reserva del almacen temporal de Rx por el extremo BAJO; llamadores: 0x80FCBF..0x80FFB2): tamano = (size + 3) & ~3;
// bajo (0xC9A6B8) += tamano; si el nuevo bajo es <= alto (0xC9A6B4) devuelve el bajo anterior. Si no: restituye el bajo, error 0x80000013 con el tamano y devuelve NULL.
void* RxScratchAllocLow(RwUInt32 size) {
    const RwUInt32 rounded = (size + 3u) & ~3u;
    const RwUInt32 high = StaticRef<RwUInt32>(0xC9A6B4);
    const RwUInt32 low = StaticRef<RwUInt32>(0xC9A6B8) + rounded;
    StaticRef<RwUInt32>(0xC9A6B8) = low;
    if (low <= high) {
        return reinterpret_cast<void*>(static_cast<uintptr_t>(low - rounded));
    }
    StaticRef<RwUInt32>(0xC9A6B8) = low - rounded;
    RwRaiseError(kRxErrOutOfMemory, rounded);
    return nullptr;
}

// 0x805950 (lote R31A, sin validar; 108 bytes; cdecl (tuberia); nombre inferido: numero de clusters distintos de la tuberia; llamador: 0x80F15B): devuelve cuantas definiciones de cluster distintas hay
// entre las "clustersOfInterest" de las definiciones de todos los nodos. Se cuentan ordenando por la direccion de la definicion: en cada pasada se busca la menor direccion (sin signo) mayor que la
// de la pasada anterior (0 al principio); sin ninguna se devuelve la cuenta. El numero de nodos se lee una vez.
RwUInt32 _rxPipelineCountClusters(RxPipeline* pipeline) {
    const RwUInt32 numNodes = pipeline->numNodes;
    RwUInt32 count = 0;
    uintptr_t threshold = 0;
    for (;;) {
        uintptr_t minimum = 0xFFFFFFFFu;
        if (numNodes == 0) {
            return count;
        }
        const RxPipelineNode* node = pipeline->nodes;
        for (RwUInt32 remaining = numNodes; remaining != 0; --remaining) {
            const RxNodeDefinition* const definition = node->nodeDef;
            const RxClusterRef* reference = definition->io.clustersOfInterest;
            for (RwUInt32 left = definition->io.numClustersOfInterest; left != 0; --left) {
                const uintptr_t address = reinterpret_cast<uintptr_t>(reference->clusterDef);
                if (address > threshold && address < minimum) {
                    minimum = address;
                }
                ++reference;
            }
            node = reinterpret_cast<const RxPipelineNode*>(reinterpret_cast<const uint8*>(node) + sizeof(RxPipelineNode));
        }
        if (minimum == 0xFFFFFFFFu) {
            return count;
        }
        ++count;
        threshold = minimum;
    }
}

// 0x805A20 (lote R31A, sin validar; 747 bytes; cdecl (nodo, cluster a anadir o NULL); nombre inferido: clonado de la definicion de un nodo con un cluster de mas; llamadores: 0x8059F3 y 0x805A06
// (RxPipelineNodeCloneDefinition), 0x805DF0 (RxPipelineNodeRequestCluster), 0x805E6F (ReplaceCluster), 0x8073F3): crea con RwMalloc (pista 0x30409) un bloque con la copia editable de nodo->nodeDef
// (cabecera de 0x40 bytes, nombre, lista de clusters de interes (12 bytes, uno mas si se anade: forcePresent 0, reservado 0), requisitos de entrada (el nuevo = 0) y salidas (nombre, estados de
// cluster por salida; el del cluster nuevo = allOtherClusters)); tamanos redondeados a 4. Sin memoria: error 0x80000013 con el tamano total y devuelve NULL. Con exito: InputPipesCnt = 0, editable = 1,
// copia pipelineNodePrivateDataSize; si la definicion anterior no tenia tuberias y era editable la libera (RwFree); nodo->nodeDef = la copia, que devuelve.
RxNodeDefinition* _rxNodeDefinitionClone(RxPipelineNode* node, RxClusterDefinition* clusterToAdd) {
    RxNodeDefinition* const definition = node->nodeDef;
    const RwUInt32 numClusters = definition->io.numClustersOfInterest;
    const RwUInt32 newNumClusters = (clusterToAdd != nullptr) ? numClusters + 1 : numClusters;
    const RwUInt32 numOutputs = definition->io.numOutputs;
    const RwUInt32 clusterListBytes = (newNumClusters * 4u + 3u) & ~3u;
    const RwUInt32 clusterRefsBytes = (newNumClusters * 12u + 3u) & ~3u;
    const RwUInt32 outputsBytes = (numOutputs * 12u + 3u) & ~3u;

    RwUInt32 totalSize = outputsBytes + clusterListBytes + RxStringSlot(definition->name) + clusterRefsBytes + 0x40;
    for (RwUInt32 i = 0; i < numOutputs; ++i) {
        totalSize += RxStringSlot(definition->io.outputs[i].name) + clusterListBytes;
    }

    uint8* const block = static_cast<uint8*>(RwMalloc(totalSize, 0x30409));
    if (block == nullptr) {
        RwRaiseError(kRxErrOutOfMemory, totalSize);
        return nullptr;
    }
    RxNodeDefinition* const clone = reinterpret_cast<RxNodeDefinition*>(block);
    uint8* cursor = block + 0x40;

    clone->name = reinterpret_cast<RwChar*>(cursor);
    cursor += RxStringSlot(definition->name);
    RxCopyString(clone->name, definition->name);
    clone->nodeMethods = definition->nodeMethods;
    clone->io.numClustersOfInterest = newNumClusters;
    clone->io.clustersOfInterest = reinterpret_cast<RxClusterRef*>(cursor);
    cursor += clusterRefsBytes;

    for (RwUInt32 i = 0; i < numClusters; ++i) {
        clone->io.clustersOfInterest[i].forcePresent = definition->io.clustersOfInterest[i].forcePresent;
        clone->io.clustersOfInterest[i].clusterDef = definition->io.clustersOfInterest[i].clusterDef;
        clone->io.clustersOfInterest[i].reserved = definition->io.clustersOfInterest[i].reserved;
    }
    if (clusterToAdd != nullptr) {
        clone->io.clustersOfInterest[numClusters].forcePresent = rxCLALLOWABSENT;
        clone->io.clustersOfInterest[numClusters].clusterDef = clusterToAdd;
        clone->io.clustersOfInterest[numClusters].reserved = 0;
    }

    clone->io.inputRequirements = reinterpret_cast<RxClusterValidityReq*>(cursor);
    cursor += clusterListBytes;
    for (RwUInt32 i = 0; i < numClusters; ++i) {
        clone->io.inputRequirements[i] = definition->io.inputRequirements[i];
    }
    if (clusterToAdd != nullptr) {
        clone->io.inputRequirements[newNumClusters - 1] = static_cast<RxClusterValidityReq>(0);
    }

    clone->io.outputs = reinterpret_cast<RxOutputSpec*>(cursor);
    clone->io.numOutputs = numOutputs;
    cursor += outputsBytes;
    for (RwUInt32 i = 0; i < numOutputs; ++i) {
        const RxOutputSpec& oldOutput = definition->io.outputs[i];
        RxOutputSpec& newOutput = clone->io.outputs[i];
        newOutput.name = reinterpret_cast<RwChar*>(cursor);
        const RwUInt32 nameSlot = RxStringSlot(oldOutput.name);
        RxCopyString(newOutput.name, oldOutput.name);
        newOutput.outputClusters = reinterpret_cast<RxClusterValid*>(cursor + nameSlot);
        cursor += nameSlot + clusterListBytes;
        for (RwUInt32 k = 0; k < numClusters; ++k) {
            newOutput.outputClusters[k] = oldOutput.outputClusters[k];
        }
        if (clusterToAdd != nullptr) {
            newOutput.outputClusters[newNumClusters - 1] = oldOutput.allOtherClusters;
        }
        newOutput.allOtherClusters = oldOutput.allOtherClusters;
    }

    clone->pipelineNodePrivateDataSize = definition->pipelineNodePrivateDataSize;
    clone->editable = static_cast<RxNodeDefEditable>(1);
    clone->InputPipesCnt = 0;
    if (definition->InputPipesCnt == 0 && definition->editable != 0) {
        RwFree(definition);
    }
    node->nodeDef = clone;
    return clone;
}

// 0x8059C0 (lote R31A, sin validar; 82 bytes; era una envoltura; cdecl (nodo, cluster a anadir o NULL)): nodo NULL: devuelve NULL. Con cluster y ya presente entre los de interes de la definicion
// (comparacion por direccion de la definicion): devuelve NULL. En cualquier otro caso devuelve _rxNodeDefinitionClone (0x805A20) del nodo con ese cluster.
RxNodeDefinition* RxPipelineNodeCloneDefinition(RxPipelineNode* node, RxClusterDefinition* cluster2add) {
    if (node == nullptr) {
        return nullptr;
    }
    if (cluster2add != nullptr) {
        const RxIoSpec& io = node->nodeDef->io;
        for (RwUInt32 i = 0; i < io.numClustersOfInterest; ++i) {
            if (io.clustersOfInterest[i].clusterDef == cluster2add) {
                return nullptr;
            }
        }
    }
    return _rxNodeDefinitionClone(node, cluster2add);
}

// 0x8064D0 (lote R31A, sin validar; 271 bytes; cdecl (tuberia, tamano nuevo); nombre inferido: cambio de tamano del superbloque; llamadores: 0x806087, 0x8062F9, 0x8069C9): RwRealloc del superbloque
// (+0x20) con pista 0x1040409; NULL: error 0x80000013 con el tamano y devuelve 0. Si no: superBlock y nodes = el bloque nuevo, superBlockSize = tamano, y suma la diferencia de direcciones (nuevo -
// anterior) a embeddedPacket (+0x14) y a inputRequirements (+0x1C) si no son NULL y, en cada nodo, a +0x08, +0x0C, +0x10, +0x14, +0x18 y +0x1C si no son NULL. Devuelve 1.
RwBool _rxPipelineResizeSuperBlock(RxPipeline* pipeline, RwUInt32 newSize) {
    void* const oldBlock = pipeline->superBlock;
    uint8* const block = static_cast<uint8*>(RwRealloc(oldBlock, newSize, 0x1040409));
    if (block == nullptr) {
        RwRaiseError(kRxErrOutOfMemory, newSize);
        return FALSE;
    }
    const RwUInt32 numNodes = pipeline->numNodes;
    pipeline->superBlock = block;
    pipeline->nodes = reinterpret_cast<RxPipelineNode*>(block);
    const uintptr_t delta = reinterpret_cast<uintptr_t>(block) - reinterpret_cast<uintptr_t>(oldBlock);
    pipeline->superBlockSize = newSize;
    RxShiftPointer(pipeline->embeddedPacket, delta);
    RxShiftPointer(pipeline->inputRequirements, delta);
    for (RwUInt32 i = 0; i < numNodes; ++i) {
        RxPipelineNode* const node = &pipeline->nodes[i];
        RxShiftPointer(node->outputs, delta);
        RxShiftPointer(node->slotClusterRefs, delta);
        RxShiftPointer(node->slotsContinue, delta);
        RxShiftPointer(node->privateData, delta);
        RxShiftPointer(node->inputToClusterSlot, delta);
        RxShiftPointer(node->topSortData, delta);
    }
    return TRUE;
}

// 0x8065E0 (lote R31A, sin validar; 462 bytes; cdecl (destino, origen); nombre inferido: reconstruye la disposicion "bloqueada" del superbloque del destino a partir del origen; llamadores:
// 0x806431 (desbloqueo fallido), 0x8069DE (RxPipelineLock) y 0x806B01 (RxPipelineClone), los dos primeros con destino == origen). Si destino != origen: copia los nodos de origen a destino del
// ultimo al primero (0x28 bytes), pone a cero slotClusterRefs, slotsContinue, privateData e inputToClusterSlot y duplica los datos de inicializacion con RwMalloc (pista 0x30409; sin memoria: error
// 0x80000013 con su tamano y devuelve 0); al acabar numNodes del destino = el del origen. Despues, comun: pone las salidas de cada nodo del destino en su bloque de 0x80 bytes (nodes + 40 * M + 128 *
// indice) y, si el origen tenia salidas, copia 32 dwords hacia delante con rep movs DESPUES de escribir el puntero (con destino == origen la copia es de un bloque a si mismo); por ultimo reparte
// los elementos de 12 bytes (nodes + 168 * M, puestos a cero) como topSortData de cada nodo. Devuelve 1.
RwBool _rxPipelineBuildLockedLayout(RxPipeline* destination, RxPipeline* source) {
    if (destination != source) {
        RwInt32 index = static_cast<RwInt32>(source->numNodes) - 1;
        if (index >= 0) {
            RwUInt32 offset = static_cast<RwUInt32>(index) * sizeof(RxPipelineNode);
            do {
                RxPipelineNode* const sourceNode = reinterpret_cast<RxPipelineNode*>(reinterpret_cast<uint8*>(source->nodes) + offset);
                RxPipelineNode* const destinationNode = reinterpret_cast<RxPipelineNode*>(reinterpret_cast<uint8*>(destination->nodes) + offset);
                memcpy(destinationNode, sourceNode, sizeof(RxPipelineNode));
                destinationNode->slotClusterRefs = nullptr;
                destinationNode->slotsContinue = nullptr;
                destinationNode->privateData = nullptr;
                destinationNode->inputToClusterSlot = nullptr;
                const RwUInt32 dataSize = destinationNode->initializationDataSize;
                if (dataSize != 0) {
                    destinationNode->initializationData = RwMalloc(dataSize, 0x30409);
                    if (destinationNode->initializationData == nullptr) {
                        RwRaiseError(kRxErrOutOfMemory, destination->nodes[index].initializationDataSize);
                        return FALSE;
                    }
                    const uint8* const sourceData = static_cast<const uint8*>(source->nodes[index].initializationData);
                    uint8* const destinationData = static_cast<uint8*>(destinationNode->initializationData);
                    __movsd(reinterpret_cast<unsigned long*>(destinationData), reinterpret_cast<const unsigned long*>(sourceData), dataSize >> 2);
                    __movsb(destinationData + (dataSize >> 2) * 4, sourceData + (dataSize >> 2) * 4, dataSize & 3);
                }
                --index;
                offset -= sizeof(RxPipelineNode);
            } while (index >= 0);
        }
        destination->numNodes = source->numNodes;
    }

    uint8* const blocks = reinterpret_cast<uint8*>(destination->nodes) + RxMaxNodes() * sizeof(RxPipelineNode);
    {
        RwInt32 index = static_cast<RwInt32>(source->numNodes) - 1;
        if (index >= 0) {
            uint8* block = blocks + static_cast<RwUInt32>(index) * 0x80;
            RwUInt32 offset = static_cast<RwUInt32>(index) * sizeof(RxPipelineNode);
            RwUInt32 remaining = static_cast<RwUInt32>(index) + 1;
            do {
                RxPipelineNode* const destinationNode = reinterpret_cast<RxPipelineNode*>(reinterpret_cast<uint8*>(destination->nodes) + offset);
                destinationNode->outputs = reinterpret_cast<RwUInt32*>(block);
                RwUInt32* const sourceOutputs = reinterpret_cast<RxPipelineNode*>(reinterpret_cast<uint8*>(source->nodes) + offset)->outputs;
                if (sourceOutputs != nullptr) {
                    __movsd(reinterpret_cast<unsigned long*>(destinationNode->outputs), reinterpret_cast<const unsigned long*>(sourceOutputs), 0x20);
                }
                block -= 0x80;
                offset -= sizeof(RxPipelineNode);
                --remaining;
            } while (remaining != 0);
        }
    }

    RwUInt32* element = reinterpret_cast<RwUInt32*>(blocks + RxMaxNodes() * 0x80);
    if (source->numNodes != 0) {
        RwUInt32 index = 0;
        RwUInt32 offset = 0;
        do {
            element[0] = 0;
            element[1] = 0;
            element[2] = 0;
            reinterpret_cast<RxPipelineNode*>(reinterpret_cast<uint8*>(destination->nodes) + offset)->topSortData = reinterpret_cast<RxPipelineNodeTopSortData*>(element);
            ++index;
            element += 3;
            offset += sizeof(RxPipelineNode);
        } while (index < source->numNodes);
    }
    return TRUE;
}

// 0x8067B0 (lote R31A, sin validar; 474 bytes; cdecl (contexto, indice); nombre inferido: paso de la ordenacion topologica de RxLockedPipeUnlock; llamadores: 0x806154 y ella misma): el contexto
// es {tuberia, nodos ya ordenados}. Si el nodo `indice` no esta ya en la posicion `numSorted`: intercambia los 32 dwords de los bloques de salidas de ambos, cruza sus punteros de salidas, intercambia
// el contenido de sus topSortData y cruza esos punteros, intercambia los dos nodos enteros (40 bytes) y cambia en las salidas de todos los nodos la referencia a una posicion por la otra. Despues el nodo
// de la posicion numSorted cuenta como ordenado (numSorted += 1) y por cada salida distinta de 0xFFFFFFFF suma uno a numInsVisited del destino; si ya iguala a numIns recorre ese destino de la misma manera.
void _rxPipelineTopSortVisit(RxPipelineTopSortContext* context, RwUInt32 index) {
    const RwUInt32 sorted = context->numSorted;
    if (sorted != index) {
        RxPipeline* const pipeline = context->pipeline;
        RwUInt32* const outputsOfSorted = pipeline->nodes[sorted].outputs;
        RwUInt32* const outputsOfIndex = pipeline->nodes[index].outputs;
        for (RwUInt32 i = 0; i < 0x20; ++i) {
            const RwUInt32 previous = outputsOfSorted[i];
            outputsOfSorted[i] = outputsOfIndex[i];
            outputsOfIndex[i] = previous;
        }
        pipeline->nodes[sorted].outputs = outputsOfIndex;
        pipeline->nodes[index].outputs = outputsOfSorted;

        RxPipelineNodeTopSortData* const dataOfSorted = pipeline->nodes[sorted].topSortData;
        RxPipelineNodeTopSortData* const dataOfIndex = pipeline->nodes[index].topSortData;
        const RxPipelineNodeTopSortData saved = *dataOfSorted;
        *dataOfSorted = *dataOfIndex;
        *dataOfIndex = saved;
        pipeline->nodes[sorted].topSortData = dataOfIndex;
        pipeline->nodes[index].topSortData = dataOfSorted;

        RxPipelineNode swapped;
        memcpy(&swapped, &pipeline->nodes[sorted], sizeof(RxPipelineNode));
        memcpy(&pipeline->nodes[sorted], &pipeline->nodes[index], sizeof(RxPipelineNode));
        memcpy(&pipeline->nodes[index], &swapped, sizeof(RxPipelineNode));

        if (context->pipeline->numNodes != 0) {
            RwUInt32 nodeIndex = 0;
            do {
                RxPipelineNode* const node = &context->pipeline->nodes[nodeIndex];
                if (node->numOutputs != 0) {
                    RwUInt32 outputIndex = 0;
                    do {
                        RwUInt32* const slot = &node->outputs[outputIndex];
                        const RwUInt32 value = *slot;
                        if (value == sorted) {
                            *slot = index;
                        } else if (value == index) {
                            *slot = sorted;
                        }
                        ++outputIndex;
                    } while (outputIndex < node->numOutputs);
                }
                ++nodeIndex;
            } while (nodeIndex < context->pipeline->numNodes);
        }
    }

    RxPipeline* const pipeline = context->pipeline;
    RxPipelineNode* const current = &pipeline->nodes[context->numSorted];
    context->numSorted = context->numSorted + 1;
    if (current->numOutputs != 0) {
        RwUInt32 outputIndex = 0;
        do {
            const RwUInt32 target = current->outputs[outputIndex];
            if (target != 0xFFFFFFFFu) {
                RxPipelineNode* const targetNode = &context->pipeline->nodes[target];
                targetNode->topSortData->numInsVisited = targetNode->topSortData->numInsVisited + 1;
                if (targetNode->topSortData->numIns == targetNode->topSortData->numInsVisited) {
                    _rxPipelineTopSortVisit(context, target);
                }
            }
            ++outputIndex;
        } while (outputIndex < current->numOutputs);
    }
}

// 0x806990 (lote R31A, sin validar; 291 bytes; era una envoltura; cdecl (tuberia); llamadores: 0x5D676A, 0x74EAE5, 0x7CB34D, 0x806AF3, 0x80737C y otros): ya bloqueada: la devuelve sin mas. Si no:
// tamano = 180 * M. Con nodes != NULL: si el superbloque es menor lo redimensiona (0x8064D0; fallo: devuelve NULL) y reconstruye la disposicion bloqueada (0x8065E0 con destino == origen; fallo:
// NULL). Sin nodes: RwMalloc (pista 0x1030409) del superbloque (superBlock = resultado antes de comprobarlo; NULL: error 0x80000013 con el tamano y devuelve NULL), superBlockSize = tamano, nodes =
// superbloque. Marca locked = 1 y, con nodes != NULL, por cada nodo: pipelineNodeTerm (+0x14 de la definicion leida ANTES de la llamada) si existe; InputPipesCnt -= 1 y, si llega a 0, nodeTerm
// (la funcion de la definicion anterior, con la definicion releida) si existe; slotClusterRefs = NULL. Devuelve la tuberia.
RxLockedPipe* RxPipelineLock(RxPipeline* pipeline) {
    if (pipeline->locked != 0) {
        return pipeline;
    }
    const RwUInt32 requiredSize = RxMaxNodes() * 180u;
    if (pipeline->nodes != nullptr) {
        if (requiredSize > pipeline->superBlockSize) {
            if (!_rxPipelineResizeSuperBlock(pipeline, requiredSize)) {
                return nullptr;
            }
        }
        if (!_rxPipelineBuildLockedLayout(pipeline, pipeline)) {
            return nullptr;
        }
    } else {
        void* const block = RwMalloc(requiredSize, 0x1030409);
        pipeline->superBlock = block;
        if (block == nullptr) {
            RwRaiseError(kRxErrOutOfMemory, requiredSize);
            return nullptr;
        }
        pipeline->superBlockSize = requiredSize;
        pipeline->nodes = static_cast<RxPipelineNode*>(block);
    }
    pipeline->locked = TRUE;
    if (pipeline->nodes != nullptr && pipeline->numNodes != 0) {
        RwUInt32 index = 0;
        do {
            RxPipelineNode* const node = &pipeline->nodes[index];
            const RxNodeDefinition* const firstDefinition = node->nodeDef;
            {
                const RxPipelineNodeTermFn term = firstDefinition->nodeMethods.pipelineNodeTerm;
                if (term != nullptr) {
                    term(node);
                }
            }
            pipeline->nodes[index].nodeDef->InputPipesCnt = pipeline->nodes[index].nodeDef->InputPipesCnt - 1;
            RxNodeDefinition* const secondDefinition = pipeline->nodes[index].nodeDef;
            if (secondDefinition->InputPipesCnt == 0) {
                const RxNodeTermFn nodeTerm = firstDefinition->nodeMethods.nodeTerm;
                if (nodeTerm != nullptr) {
                    nodeTerm(secondDefinition);
                }
            }
            pipeline->nodes[index].slotClusterRefs = nullptr;
            ++index;
        } while (index < pipeline->numNodes);
    }
    return pipeline;
}

// 0x805F40 (lote R31A, sin validar; 1412 bytes; era una envoltura; cdecl (tuberia bloqueada); llamadores: 0x5D6793, 0x74EB04, 0x7CB369, 0x806B0E, 0x80752F, 0x80A0D4 y otros): desbloquea una tuberia
// de Rx. Errores (RwErrorSet con pluginID 1; devuelven NULL): tuberia NULL 0x80000016, no bloqueada 0x34, entryPoint fuera de rango o nodo de entrada sin definicion 0x24; sin nodos: locked = 0 y
// devuelve la tuberia. Calcula el tamano necesario S = max(180 * M, ...) con el numero de clusters distintos (_rxPipelineCountClusters; el EXE lo hace en linea), lo hace crecer si hace falta (0x8064D0),
// fija el almacen temporal (bajo 0, alto superbloque + S), pone a cero numIns/numInsVisited, cuenta las entradas de cada nodo (numIns += 1 por cada salida distinta de 0xFFFFFFFF), exige numIns == 0
// en el nodo de entrada (0x24) y != 0 en los demas (0x22), ordena topologicamente desde el nodo de entrada (0x8067B0) y exige numIns == numInsVisited en todos (0x1C; ciclo). Despues entryPoint = 0;
// mueve los elementos topSortData al final del superbloque, empaqueta las salidas de cada nodo justo detras de los nodos, fija el almacen temporal (alto = nodes + 168 * M - 12, bajo = bloques +
// 4 * salidas) y llama a 0x80F0D0 (asignacion de clusters y ranuras, EXE; distinto de 0: devuelve NULL sin mas). Ajusta el superbloque al tamano usado (bajo - superBlock; fallo: reconstruye la
// disposicion bloqueada y devuelve NULL), pone topSortData = NULL en los nodos y, del ultimo al primero, nodeInit (solo la primera vez que se usa la definicion) y pipelineNodeInit; si alguna falla
// deshace con pipelineNodeTerm/nodeTerm lo ya iniciado, reconstruye la disposicion bloqueada (0x8065E0) y devuelve NULL. Del ultimo al primero pipelineNodeConfig(nodo, tuberia) (un fallo termina
// todos los nodos igual). Con exito locked = 0 y devuelve la tuberia.
RxPipeline* RxLockedPipeUnlock(RxLockedPipe* pipeline) {
    RxPipeline* const locked = pipeline;
    if (locked == nullptr) {
        RwRaiseError(kRxErrNullPointer);
        return nullptr;
    }
    if (locked->locked == 0) {
        RwRaiseError(0x34);
        return nullptr;
    }
    const RwUInt32 numNodes = locked->numNodes;
    if (numNodes == 0) {
        locked->locked = FALSE;
        return locked;
    }
    if (locked->entryPoint >= numNodes || locked->nodes[locked->entryPoint].nodeDef == nullptr) {
        RwRaiseError(0x24);
        return nullptr;
    }

    // Tamano necesario del superbloque.
    const RwUInt32 numClusters = _rxPipelineCountClusters(locked);
    const RwUInt32 maxNodes = RxMaxNodes();
    const RwUInt32 minimumSize = maxNodes * 180u;
    RwUInt32 outputsBytes = numNodes * 0x28u;
    outputsBytes += (locked->nodes[0].numOutputs * numNodes) * 4u;
    RwUInt32 total = maxNodes * 12u + (numClusters * 52u + 0x14u) * numNodes + outputsBytes;
    RwUInt32 clusterBytes = ((numNodes + 5u) * numClusters + (numClusters + 1u) * numNodes) * 4u;
    {
        const RxPipelineNode* node = locked->nodes;
        for (RwUInt32 remaining = numNodes; remaining != 0; --remaining) {
            const RxNodeDefinition* const definition = node->nodeDef;
            if (definition->pipelineNodePrivateDataSize != 0) {
                clusterBytes += definition->pipelineNodePrivateDataSize;
            }
            clusterBytes += definition->io.numClustersOfInterest * 4u;
            node = reinterpret_cast<const RxPipelineNode*>(reinterpret_cast<const uint8*>(node) + sizeof(RxPipelineNode));
        }
    }
    total += clusterBytes + numClusters * 28u + 0x14u;
    const RwUInt32 superBlockNeeded = (total > minimumSize) ? total : minimumSize;
    if (superBlockNeeded > locked->superBlockSize) {
        if (!_rxPipelineResizeSuperBlock(locked, superBlockNeeded)) {
            return nullptr;
        }
    }

    // Almacen temporal y contadores de entradas.
    StaticRef<RwUInt32>(0xC9A6B8) = 0;
    StaticRef<RwUInt32>(0xC9A6B4) = static_cast<RwUInt32>(reinterpret_cast<uintptr_t>(locked->superBlock)) + superBlockNeeded;
    RxPipelineTopSortContext context;
    context.pipeline = locked;
    context.numSorted = 0;
    for (RwUInt32 i = 0; i < locked->numNodes; ++i) {
        RxPipelineNode* const node = &locked->nodes[i];
        if (node->nodeDef != nullptr) {
            node->topSortData->numInsVisited = 0;
            node->topSortData->numIns = 0;
        }
    }
    for (RwUInt32 i = 0; i < locked->numNodes; ++i) {
        RxPipelineNode* const node = &locked->nodes[i];
        if (node->nodeDef != nullptr && node->numOutputs != 0) {
            const RwUInt32* output = node->outputs;
            for (RwUInt32 left = node->numOutputs; left != 0; --left) {
                const RwUInt32 target = *output;
                if (target != 0xFFFFFFFFu) {
                    locked->nodes[target].topSortData->numIns = locked->nodes[target].topSortData->numIns + 1;
                }
                ++output;
            }
        }
    }
    const RwUInt32 entryPoint = locked->entryPoint;
    if (locked->nodes[entryPoint].topSortData->numIns != 0) {
        RwRaiseError(0x24);
        return nullptr;
    }
    for (RwUInt32 i = 0; i < locked->numNodes; ++i) {
        if (i != entryPoint && locked->nodes[i].topSortData->numIns == 0) {
            RwRaiseError(0x22);
            return nullptr;
        }
    }

    // Ordenacion topologica.
    _rxPipelineTopSortVisit(&context, entryPoint);
    for (RwUInt32 i = 0; i < locked->numNodes; ++i) {
        if (locked->nodes[i].topSortData->numIns != locked->nodes[i].topSortData->numInsVisited) {
            RwRaiseError(0x1C);
            return nullptr;
        }
    }
    locked->entryPoint = 0;

    // Los topSortData pasan al final del superbloque; las salidas se empaquetan detras de los nodos.
    uint8* const blocks = reinterpret_cast<uint8*>(locked->nodes) + RxMaxNodes() * 0x28u;
    uint8* const elementBase = blocks + RxMaxNodes() * 0x80u;
    const RwUInt32 nodeCount = locked->numNodes;
    uint8* source = elementBase + nodeCount * 12u - 0xC;
    uint8* destination = reinterpret_cast<uint8*>(locked->superBlock) + superBlockNeeded - 0xC;
    for (RwInt32 index = static_cast<RwInt32>(nodeCount) - 1; index >= 0; --index) {
        memcpy(destination, source, 0xC);
        locked->nodes[index].topSortData = reinterpret_cast<RxPipelineNodeTopSortData*>(destination);
        source -= 0xC;
        destination -= 0xC;
    }
    uint8* const scratchHigh = source;

    uint8* packed = reinterpret_cast<uint8*>(locked->nodes) + locked->numNodes * sizeof(RxPipelineNode);
    uint8* block = blocks;
    RwUInt32 totalOutputs = 0;
    for (RwUInt32 i = 0; i < locked->numNodes; ++i) {
        RxPipelineNode* const node = &locked->nodes[i];
        const RwUInt32 count = node->numOutputs;
        if (count == 0) {
            node->outputs = nullptr;
        } else {
            __movsd(reinterpret_cast<unsigned long*>(packed), reinterpret_cast<const unsigned long*>(block), count);
            locked->nodes[i].outputs = reinterpret_cast<RwUInt32*>(packed);
        }
        block += 0x80;
        totalOutputs += locked->nodes[i].numOutputs;
        packed += locked->nodes[i].numOutputs * 4u;
    }
    StaticRef<RwUInt32>(0xC9A6B4) = static_cast<RwUInt32>(reinterpret_cast<uintptr_t>(scratchHigh));
    StaticRef<RwUInt32>(0xC9A6B8) = static_cast<RwUInt32>(reinterpret_cast<uintptr_t>(block)) + totalOutputs * 4u;
    if (reinterpret_cast<RwInt32(__cdecl*)(RxPipeline*)>(0x80F0D0)(locked) != 0) {
        return nullptr;
    }
    const RwUInt32 usedSize = StaticRef<RwUInt32>(0xC9A6B8) - static_cast<RwUInt32>(reinterpret_cast<uintptr_t>(locked->superBlock));
    if (!_rxPipelineResizeSuperBlock(locked, usedSize)) {
        _rxPipelineBuildLockedLayout(locked, locked);
        return nullptr;
    }
    for (RwUInt32 i = 0; i < locked->numNodes; ++i) {
        locked->nodes[i].topSortData = nullptr;
    }

    // Termina los nodos desde `first` hasta el ultimo (pipelineNodeTerm, InputPipesCnt -= 1 y nodeTerm si llega a 0).
    const auto terminateFrom = [locked](RwUInt32 first) {
        for (RwUInt32 i = first; i < locked->numNodes; ++i) {
            RxPipelineNode* const node = &locked->nodes[i];
            RxNodeDefinition* const definition = node->nodeDef;
            if (definition->nodeMethods.pipelineNodeTerm != nullptr) {
                definition->nodeMethods.pipelineNodeTerm(node);
            }
            definition->InputPipesCnt = definition->InputPipesCnt - 1;
            if (definition->InputPipesCnt == 0 && definition->nodeMethods.nodeTerm != nullptr) {
                definition->nodeMethods.nodeTerm(definition);
            }
        }
    };

    // nodeInit y pipelineNodeInit, del ultimo nodo al primero.
    for (RwInt32 index = static_cast<RwInt32>(locked->numNodes) - 1; index >= 0; --index) {
        RxPipelineNode* const node = &locked->nodes[index];
        RxNodeDefinition* const definition = node->nodeDef;
        const RwInt32 previousCount = definition->InputPipesCnt;
        definition->InputPipesCnt = previousCount + 1;
        if (previousCount == 0 && definition->nodeMethods.nodeInit != nullptr) {
            if (!definition->nodeMethods.nodeInit(definition)) {
                terminateFrom(static_cast<RwUInt32>(index) + 1);
                _rxPipelineBuildLockedLayout(locked, locked);
                return nullptr;
            }
        }
        if (definition->nodeMethods.pipelineNodeInit != nullptr) {
            if (!definition->nodeMethods.pipelineNodeInit(node)) {
                definition->InputPipesCnt = definition->InputPipesCnt - 1;
                if (definition->InputPipesCnt == 0 && definition->nodeMethods.nodeTerm != nullptr) {
                    definition->nodeMethods.nodeTerm(definition);
                }
                terminateFrom(static_cast<RwUInt32>(index) + 1);
                _rxPipelineBuildLockedLayout(locked, locked);
                return nullptr;
            }
        }
    }

    // pipelineNodeConfig, del ultimo nodo al primero.
    for (RwInt32 index = static_cast<RwInt32>(locked->numNodes) - 1; index >= 0; --index) {
        RxPipelineNode* const node = &locked->nodes[index];
        const RxPipelineNodeConfigFn config = node->nodeDef->nodeMethods.pipelineNodeConfig;
        if (config != nullptr) {
            if (!config(node, locked)) {
                terminateFrom(0);
                _rxPipelineBuildLockedLayout(locked, locked);
                return nullptr;
            }
        }
    }
    locked->locked = FALSE;
    return locked;
}

// Lote R31A (sin validar): ganchos de las tuberias de Rx (0x804FE0..0x806990).
void RwCoreInjectHooksR31A() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RwCore");

    RH_ScopedGlobalInstall(_rxPipelineOpen, 0x804FE0);
    RH_ScopedGlobalInstall(_rxPipelineNodeDestroy, 0x8050C0);
    RH_ScopedGlobalInstall(_rxPipelineDestroy, 0x805820);
    RH_ScopedGlobalInstall(RxScratchAllocHigh, 0x805890);
    RH_ScopedGlobalInstall(RxScratchAllocLow, 0x8058F0);
    RH_ScopedGlobalInstall(_rxPipelineCountClusters, 0x805950);
    RH_ScopedGlobalInstall(RxPipelineNodeCloneDefinition, 0x8059C0);
    RH_ScopedGlobalInstall(_rxNodeDefinitionClone, 0x805A20);
    RH_ScopedGlobalInstall(RxLockedPipeUnlock, 0x805F40);
    RH_ScopedGlobalInstall(_rxPipelineResizeSuperBlock, 0x8064D0);
    RH_ScopedGlobalInstall(_rxPipelineBuildLockedLayout, 0x8065E0);
    RH_ScopedGlobalInstall(_rxPipelineTopSortVisit, 0x8067B0);
    RH_ScopedGlobalInstall(RxPipelineLock, 0x806990);
}
