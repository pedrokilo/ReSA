/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3D9RasterStandards
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <cstring>
#include <d3d9.h>

#include "D3D9RasterStandards.h"
#include "D3D9RasterDriver.h"
#include "D3D9RasterFormat.h"
#include "D3DResourceSystem.h"
#include "rw/rwplcore.h"
#include "rw/rwcore.h"

// Reconstruccion en C++ puro de los 12 manejadores de raster que _rwD3D9DeviceSystemStandards (rwcore.cpp, 0x7F72E0) guardaba como direcciones del EXE sin funcion de Ghidra ni enganche (huecos de codigo vivo
// 0x4CA4E0, 0x4CAE40/60/80, 0x4CB4C0/E0, 0x4CB520, 0x4CBCB0, 0x4CBD40/50, 0x4CD4D0 y 0x4CD820) y de los cuatro ayudantes usercall que por ellos seguian vivos (0x4CAD90, 0x4CAEA0, 0x4CAAE0, 0x4CA5A0).
// Los manejadores son cdecl (salida, entrada/salida, valor): el motor los llama por puntero de la tabla `stdFunc`, que ahora guarda estas funciones, y cada direccion queda ademas enganchada para cualquier llamador
// del EXE. Los ayudantes usercall (registros eax/edx/ecx/esi/ebx) solo los llamaban estos manejadores: pasan a ser funciones C++ con parametros y no llevan enganche (su ABI no es cdecl y no queda llamador en el EXE).
// Banco fuera del juego: reversing/FnD3DStd (EXE original contra este archivo, con falsos de IDirect3DDevice9 y trazas de las llamadas).

// Funciones de rwcore.cpp y de otros archivos que estas rutinas llaman (todas en C++; antes eran direcciones del EXE).
RwBool _rwD3D9PixelToRGB(RwUInt8* color, const RwUInt32* pixel, RwUInt32 format);   // 0x7FF070 (rwcore.cpp)
RwBool _rwD3D9RGBToPixel(RwUInt32* pixel, const RwUInt8* color, RwUInt32 format);   // 0x7FEE20 (rwcore.cpp)
RwBool _rwD3D9SetRenderTarget(RwUInt32 index, void* surface);                       // 0x7F5F20 (rwcore.cpp)
RwBool _rwD3D9RestoreDevice();                                                      // 0x7F8300 (rwcore.cpp)
RwBool _rwD3D9RWSetRenderState(RwInt32 state, void* value);                         // 0x7FE420 (rwcore.cpp)
RwBool _rwD3D9RenderStateVertexAlphaEnable(RwBool enable);                          // 0x7FE0A0 (rwcore.cpp)
RwBool _rwD3D9Im2DRenderPrimitive(RwInt32 primType, void* vertices, RwInt32 numVertices); // 0x7FBB00 (rwcore.cpp)
RwBool D3D9RasterLockCpp(RwUInt8** pixelsOut, RwRaster* raster, RwInt32 lockMode);  // 0x4C9F90 (rwcore.cpp)
RwBool D3D9RasterUnlockCpp(void* unused, RwRaster* raster);                         // 0x4CA290 (rwcore.cpp)

namespace {
// Estado del controlador de rasters D3D9 (direcciones del EXE):
//   0xB4E9E0 desplazamiento de la extension D3D9 en el raster    0xB4E9E4 raster destino actual (rwSTANDARDSETRASTERCONTEXT)
//   0xC97C28 IDirect3DDevice9*    0xC97C30 superficie de render por defecto (back buffer)    0xC97C2C superficie de profundidad por defecto
//   0xC9BFF4 y 0xC9BF44 banderas de capacidades del dispositivo (filtros de StretchRect; cubos con mipmaps)
auto& RasterExtOffset       = StaticRef<RwInt32>(0xB4E9E0);
auto& TargetRasterVar       = StaticRef<RwRaster*>(0xB4E9E4);
auto& D3D9Device            = StaticRef<void*>(0xC97C28);
auto& RenderSurface         = StaticRef<void*>(0xC97C30);
auto& DefaultDepthSurface   = StaticRef<void*>(0xC97C2C);
auto& StretchCapsFlags      = StaticRef<uint32>(0xC9BFF4);
auto& CubeMipCapsFlags      = StaticRef<uint32>(0xC9BF44);

// Extension D3D9 del raster, en raster + 0xB4E9E0 (la misma disposicion que D3D9RasterExtLayout de rwcore.cpp).
struct RasterExt {
    void*  texture;        // +0x00 textura, textura de cubo o superficie (camaras)
    void*  palette;        // +0x04
    uint8  alpha;          // +0x08
    uint8  cubeFace;       // +0x09 nibble bajo = cubo, alto = cara
    uint8  mipFlags;       // +0x0A nibble bajo = generacion automatica de mipmaps, alto = comprimida
    uint8  lockedMipLevel; // +0x0B
    void*  lockedSurface;  // +0x0C
    int32  lockedPitch;    // +0x10
    void*  lockedBits;     // +0x14
    uint32 d3dFormat;      // +0x18
    void*  swapChain;      // +0x1C
    void*  window;         // +0x20
};
static_assert(sizeof(RasterExt) == 0x24 && offsetof(RasterExt, d3dFormat) == 0x18 && offsetof(RasterExt, swapChain) == 0x1C);

// Vertice de Im2D transformado (RwIm2DVertex, 0x1C bytes).
struct QuadVertex {
    float  x, y, z, rhw;
    uint32 color;
    float  u, v;
};
static_assert(sizeof(QuadVertex) == 0x1C);

// Cabeceras de la textura nativa D3D9 en el flujo: 0x48 bytes (plataforma 9, filtro/direccionamiento, nombre y mascara) y 0x10 bytes (formato RW, formato D3D, ancho, alto, profundidad, niveles, tipo y banderas).
struct NativeTextureHeader {
    uint32 platformId;          // 9
    uint32 filterAddressing;    // ((v << 4 | u) << 8) | filtro
    char   name[32];
    char   mask[32];
};
static_assert(sizeof(NativeTextureHeader) == 0x48);
struct NativeRasterHeader {
    uint32 rasterFormat; // cFormat << 8
    uint32 d3dFormat;
    uint16 width;
    uint16 height;
    uint8  depth;
    uint8  numLevels;
    uint8  type;
    uint8  flags;        // 1 = alfa, 2 = cubo, 4 = mipmaps automaticos, 8 = comprimida
};
static_assert(sizeof(NativeRasterHeader) == 0x10);

constexpr size_t kSlotRelease              = 2;   // IUnknown::Release (+8)
constexpr size_t kSlotGetLevelCount        = 13;  // IDirect3DBaseTexture9::GetLevelCount (+0x34)
constexpr size_t kSlotGetLevelDesc         = 17;  // IDirect3DTexture9/IDirect3DCubeTexture9::GetLevelDesc (+0x44)
constexpr size_t kSlotGetSurfaceLevel      = 18;  // IDirect3DTexture9::GetSurfaceLevel / IDirect3DCubeTexture9::GetCubeMapSurface (+0x48)
constexpr size_t kSlotCreateCubeTexture    = 25;  // IDirect3DDevice9::CreateCubeTexture (+0x64)
constexpr size_t kSlotUpdateSurface        = 30;  // IDirect3DDevice9::UpdateSurface (+0x78)
constexpr size_t kSlotGetRenderTargetData  = 32;  // IDirect3DDevice9::GetRenderTargetData (+0x80)
constexpr size_t kSlotStretchRect          = 34;  // IDirect3DDevice9::StretchRect (+0x88)
constexpr size_t kSlotColorFill            = 35;  // IDirect3DDevice9::ColorFill (+0x8C)
constexpr size_t kSlotClear                = 43;  // IDirect3DDevice9::Clear (+0xAC)
constexpr size_t kSlotSetViewport          = 47;  // IDirect3DDevice9::SetViewport (+0xBC)

// Llamada COM stdcall por la tabla virtual (this primero); la tabla se lee en el momento de la llamada.
template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}
inline void ReleaseCom(void* object) { ComCall<kSlotRelease, ULONG>(object); }

inline RasterExt& ExtOf(RwRaster* raster) {
    return *reinterpret_cast<RasterExt*>(reinterpret_cast<uint8*>(raster) + static_cast<uint32>(RasterExtOffset));
}
// Raster superior: cadena de padres hasta uno que sea su propio padre.
inline RwRaster* RootOf(RwRaster* raster) {
    while (raster->parent != raster) {
        raster = raster->parent;
    }
    return raster;
}
inline bool IsCameraOrTarget(const RwRaster* raster) { return raster->cType == 2 || raster->cType == 5; }

// Superficie del nivel 0 (o de la cara activa) de la textura de un raster superior que no es camara. Un fallo de COM deja la superficie sin tocar (como el EXE).
inline void* TextureSurface(RasterExt& ext) {
    void* surface = nullptr;
    if (ext.cubeFace & 0x0F) {
        ComCall<kSlotGetSurfaceLevel, int32>(ext.texture, static_cast<uint32>(ext.cubeFace >> 4), 0u, &surface);
    } else {
        ComCall<kSlotGetSurfaceLevel, int32>(ext.texture, 0u, &surface);
    }
    return surface;
}
// Superficie de un raster superior: camara (tipo 2) = la de la extension si tiene cadena de intercambio, si no la de render por defecto; el resto = nivel 0 de su textura.
inline void* RootSurface(RwRaster* root) {
    RasterExt& ext = ExtOf(root);
    if (root->cType == 2) {
        return ext.swapChain ? ext.texture : RenderSurface;
    }
    return TextureSurface(ext);
}
} // namespace

// 0x4CBD40 (hueco 0x4CBD40; cdecl, (salida, entrada, sin uso), 16 bytes; rwSTANDARDTEXTURESETRASTER): escribe en *salida el valor de entrada y devuelve 1.
RwBool D3D9RasterStandards::SetTextureRaster(void* out, void* in, RwInt32 /*unused*/) {
    *static_cast<void**>(out) = in;
    return TRUE;
}

// 0x4CB520 (hueco 0x4CB520; cdecl, (sin uso, raster, sin uso), 16 bytes; rwSTANDARDSETRASTERCONTEXT): el raster pasa a ser el destino actual de las operaciones 2D (0xB4E9E4); devuelve 1.
RwBool D3D9RasterStandards::SetRasterContext(void* /*unused*/, RwRaster* raster, RwInt32 /*unused2*/) {
    TargetRasterVar = raster;
    return TRUE;
}

// 0x4CBD50 (hueco 0x4CBD50; cdecl, (subraster, raster padre, sin uso), 45 bytes; rwSTANDARDRASTERSUBRASTER): copia paso (+0x18), profundidad (+0x14), tipo (+0x20) y formato (+0x23) del padre al subraster y
// pone sus pixeles (+4) a NULL; devuelve 1.
RwBool D3D9RasterStandards::RasterSubRaster(RwRaster* sub, RwRaster* parent, RwInt32 /*unused*/) {
    sub->stride   = parent->stride;
    sub->depth    = parent->depth;
    sub->cType    = parent->cType;
    sub->cFormat  = parent->cFormat;
    sub->cpPixels = nullptr;
    return TRUE;
}

// 0x4CA4E0 (hueco 0x4CA4E0; cdecl, (RwUInt8** paleta, raster, modo de bloqueo), 88 bytes; rwSTANDARDRASTERLOCKPALETTE): solo rasters de tipo 0, 4 y 5 (cType & 7; otros devuelven 0). La paleta de la extension (+4)
// pasa a raster->palette (+8) y a *paleta; privateFlags (+0x22) gana 0x08 (y 0x10 tambien si el modo lleva el bit 0, escritura). Devuelve 1.
RwBool D3D9RasterStandards::RasterLockPalette(RwUInt8** paletteOut, RwRaster* raster, RwInt32 lockMode) {
    const uint32 type = raster->cType & 7u;
    if (type != 0 && type != 4 && type != 5) {
        return FALSE;
    }
    RwUInt8* const palette = static_cast<RwUInt8*>(ExtOf(raster).palette);
    raster->palette = palette;
    raster->privateFlags |= (lockMode & 1) ? 0x18 : 0x08;
    *paletteOut = palette;
    return TRUE;
}

// 0x4CBCB0 (hueco 0x4CBCB0; cdecl, (RwInt32* niveles, raster, sin uso), 144 bytes; rwSTANDARDRASTERGETMIPLEVELS): con el raster superior de tipo 0, 4 o 5 devuelve en *niveles el GetLevelCount de su textura
// (si no hay textura, el numero de bits del lado mayor); con otro tipo, 1. Devuelve 1.
RwBool D3D9RasterStandards::RasterGetMipLevels(RwInt32* levelsOut, RwRaster* raster, RwInt32 /*unused*/) {
    RwRaster* const root = RootOf(raster);
    if (root->cType != 4 && root->cType != 5 && root->cType != 0) {
        *levelsOut = 1;
        return TRUE;
    }
    void* const texture = ExtOf(root).texture;
    if (texture) {
        *levelsOut = ComCall<kSlotGetLevelCount, int32>(texture);
        return TRUE;
    }
    uint32 side = root->width > root->height ? static_cast<uint32>(root->width) : static_cast<uint32>(root->height);
    RwInt32 bits = 0;
    while (side != 0) {
        side >>= 1;
        ++bits;
    }
    *levelsOut = bits;
    return TRUE;
}

// 0x4CAAE0 (ayudante usercall; eax = raster origen, pila = (posicion, escalado); sin enganche): copia entre superficies D3D con el dispositivo. Obtiene la superficie del origen y la del destino actual (cada una
// de su raster superior), calcula el rectangulo del origen (con su desplazamiento) y el del destino (posicion mas el desplazamiento del destino; con `escalado` la posicion lleva ancho y alto propios) y llama a
// UpdateSurface (destino que no es camara ni textura de camara, o origen que no lo es; el punto de destino que usa es el desplazamiento del origen, como el EXE), GetRenderTargetData (destino normal con origen camara/
// textura de camara) o StretchRect (los dos camara/textura de camara; filtro punto o lineal si el dispositivo lo admite segun `escalado`). Libera las superficies que no son de camara y devuelve (HRESULT >= 0).
RwBool D3D9RasterStandards::CopyBetweenSurfaces(RwRaster* source, const RwInt32* position, bool scaled) {
    RwRaster* const sourceRoot = RootOf(source);
    void* const sourceSurface = RootSurface(sourceRoot);
    RwRaster* const target = TargetRasterVar;
    RwRaster* const targetRoot = RootOf(target);
    void* const targetSurface = RootSurface(targetRoot);

    const int32 sourceX = source->nOffsetX;
    const int32 sourceY = source->nOffsetY;
    const RECT sourceRect = { sourceX, sourceY, sourceX + source->width, source->height + sourceY };
    RECT targetRect;
    targetRect.left = target->nOffsetX + position[0];
    targetRect.top  = target->nOffsetY + position[1];
    if (scaled) {
        targetRect.right  = position[2] + targetRect.left;
        targetRect.bottom = position[3] + targetRect.top;
    } else {
        targetRect.right  = source->width + targetRect.left;
        targetRect.bottom = source->height + targetRect.top;
    }
    POINT point = { sourceX, sourceY };

    int32 result;
    if (!IsCameraOrTarget(targetRoot) || !IsCameraOrTarget(sourceRoot)) {
        if (!IsCameraOrTarget(targetRoot) && IsCameraOrTarget(sourceRoot)) {
            result = ComCall<kSlotGetRenderTargetData, int32>(D3D9Device, sourceSurface, targetSurface);
        } else {
            result = ComCall<kSlotUpdateSurface, int32>(D3D9Device, sourceSurface, &sourceRect, targetSurface, &point);
        }
    } else {
        uint32 filter = 0;
        if (scaled) {
            if (StretchCapsFlags & 0x2000200u) {
                filter = 2;
            }
        } else if (StretchCapsFlags & 0x1000100u) {
            filter = 1;
        }
        result = ComCall<kSlotStretchRect, int32>(D3D9Device, sourceSurface, &sourceRect, targetSurface, &targetRect, filter);
    }
    if (targetRoot->cType != 2) {
        ReleaseCom(targetSurface);
    }
    if (sourceRoot->cType != 2) {
        ReleaseCom(sourceSurface);
    }
    return result >= 0;
}

// 0x4CA5A0 (ayudante usercall; esi = raster origen, ebx = escalado, pila = (posicion, mezcla); sin enganche): dibuja el raster origen como un quad Im2D transformado sobre el destino actual (camara, textura de
// camara). Pone el destino con RwD3D9SetRenderTarget (si falla o es la camara, la superficie de render por defecto), sin z-buffer, el viewport del destino y, si _rwD3D9RestoreDevice lo permite, guarda 8
// estados de render y 2 de D3D, enlaza el raster superior como textura 0, activa mezcla (si `mezcla` y la extension tiene alfa) y dibuja una tira de 4 vertices con las coordenadas de textura del origen dentro
// de su raster superior; despues restaura los estados. Si el destino no era camara y no se pudo poner como destino de render, copia la region del back buffer a la textura del origen con StretchRect.
void D3D9RasterStandards::RenderTexturedQuad(RwRaster* source, bool scaled, const RwInt32* position, bool useBlend) {
    RwRaster* const root = RootOf(source->parent);
    RasterExt& ext = ExtOf(root);
    RwRaster* target = TargetRasterVar;
    RwBool targetOk = FALSE;
    if (target->cType == 2 || !(targetOk = RwD3D9SetRenderTarget(0, target))) {
        _rwD3D9SetRenderTarget(0, RenderSurface);
    }
    _rwD3D9SetDepthStencilSurface(nullptr);

    target = TargetRasterVar;
    D3DVIEWPORT9 viewport;
    viewport.X      = static_cast<DWORD>(static_cast<int32>(target->nOffsetX));
    viewport.Y      = static_cast<DWORD>(static_cast<int32>(target->nOffsetY));
    viewport.Width  = static_cast<DWORD>(target->width);
    viewport.Height = static_cast<DWORD>(target->height);
    viewport.MinZ   = 0.0f;
    viewport.MaxZ   = 1.0f;
    ComCall<kSlotSetViewport, int32>(D3D9Device, &viewport);

    if (_rwD3D9RestoreDevice()) {
        QuadVertex quad[4];
        const float x0 = static_cast<float>(position[0]);
        const float y0 = static_cast<float>(position[1]);
        float x1, y1;
        if (scaled) {
            x1 = static_cast<float>(position[0] + position[2]);
            y1 = static_cast<float>(position[1] + position[3]);
        } else {
            x1 = static_cast<float>(position[0] + source->width);
            y1 = static_cast<float>(position[1] + source->height);
        }
        const float u0 = static_cast<float>(static_cast<int32>(source->nOffsetX)) / static_cast<float>(root->width);
        const float v0 = static_cast<float>(static_cast<int32>(source->nOffsetY)) / static_cast<float>(root->height);
        const float u1 = static_cast<float>(static_cast<int32>(source->nOffsetX) + source->width) / static_cast<float>(root->width);
        const float v1 = static_cast<float>(static_cast<int32>(source->nOffsetY) + source->height) / static_cast<float>(root->height);
        quad[0] = { x0, y0, 0.0f, 1.0f, 0xFFFFFFFFu, u0, v0 };
        quad[1] = { x0, y1, 0.0f, 1.0f, 0xFFFFFFFFu, u0, v1 };
        quad[2] = { x1, y0, 0.0f, 1.0f, 0xFFFFFFFFu, u1, v0 };
        quad[3] = { x1, y1, 0.0f, 1.0f, 0xFFFFFFFFu, u1, v1 };

        // Estados de render que se guardan (orden del EXE): 0xA, 0xB, 0xC, 0xE, 9, 3, 4 y 1 (textura 0), y los de D3D 7 y 0x16.
        RwUInt32 saved[8];
        static constexpr RwInt32 kSavedStates[8] = { 0xA, 0xB, 0xC, 0xE, 9, 3, 4, 1 };
        for (int i = 0; i < 8; ++i) {
            _rwD3D9RWGetRenderState(kSavedStates[i], &saved[i]);
        }
        RwUInt32 savedD3D7 = 0, savedD3D16 = 0;
        RwD3D9GetRenderState(7, &savedD3D7);
        RwD3D9GetRenderState(0x16, &savedD3D16);

        _rwD3D9RWSetRasterStage(root, 0);
        if (useBlend && ext.alpha) {
            _rwD3D9RenderStateVertexAlphaEnable(1);
            _rwD3D9RenderStateSrcBlend(5);
            _rwD3D9RenderStateDestBlend(6);
        } else {
            _rwD3D9RenderStateVertexAlphaEnable(0);
        }
        _rwD3D9RWSetRenderState(0xE, nullptr);
        _rwD3D9RWSetRenderState(9, reinterpret_cast<void*>(static_cast<uintptr_t>(scaled ? 2 : 1)));
        _rwD3D9RWSetRenderState(2, reinterpret_cast<void*>(static_cast<uintptr_t>(3)));
        RwD3D9SetRenderState(7, 0);
        RwD3D9SetRenderState(0x16, 1);
        _rwD3D9Im2DRenderPrimitive(4, quad, 4);
        RwD3D9SetRenderState(7, savedD3D7);
        RwD3D9SetRenderState(0x16, savedD3D16);
        for (int i = 0; i < 5; ++i) {
            _rwD3D9RWSetRenderState(kSavedStates[i], reinterpret_cast<void*>(static_cast<uintptr_t>(saved[i])));
        }
        _rwD3D9RWSetRasterStage(reinterpret_cast<RwRaster*>(static_cast<uintptr_t>(saved[7])), 0);
        _rwD3D9RWSetRenderState(3, reinterpret_cast<void*>(static_cast<uintptr_t>(saved[5])));
        _rwD3D9RWSetRenderState(4, reinterpret_cast<void*>(static_cast<uintptr_t>(saved[6])));
    }

    if (TargetRasterVar->cType == 2) {
        return;
    }
    if (targetOk) {
        _rwD3D9SetRenderTarget(0, RenderSurface);
        _rwD3D9SetDepthStencilSurface(DefaultDepthSurface);
        return;
    }
    // Sin destino de render: copia la region del back buffer a la textura del origen.
    void* const surface = TextureSurface(ext);
    target = TargetRasterVar;
    const int32 targetX = target->nOffsetX;
    const int32 targetY = target->nOffsetY;
    const RECT backBufferRect = { targetX, targetY, target->width + targetX, target->height + targetY };
    const int32 sourceX = source->nOffsetX;
    const int32 sourceY = source->nOffsetY;
    const RECT sourceRect = { sourceX, sourceY, source->width + sourceX, source->height + sourceY };
    uint32 filter = 0;
    if (scaled) {
        if (StretchCapsFlags & 0x2000200u) {
            filter = 2;
        }
    } else if (StretchCapsFlags & 0x1000100u) {
        filter = 1;
    }
    ComCall<kSlotStretchRect, int32>(D3D9Device, RenderSurface, &backBufferRect, surface, &sourceRect, filter);
    ReleaseCom(surface);
}

// 0x4CAD90 (ayudante usercall; eax = origen, edx = posicion, ecx = escalado, pila = mezcla; sin enganche): elige la operacion segun los tipos del destino actual y del origen (cType). Sin origen devuelve 0.
// Destino normal o textura (0 y 4): origen 0, 2, 4 o 5 -> CopyBetweenSurfaces (su resultado); origen 1 -> 0; origen 3 o mayor que 5 -> 1. Destino camara o textura de camara (2 y 5): origen 2 ->
// CopyBetweenSurfaces; 0, 4 y 5 -> RenderTexturedQuad (devuelve 1); 1 -> 0; 3 o mayor que 5 -> 1. Destino 1, 3 o mayor que 5: 0.
RwBool D3D9RasterStandards::RasterBlit(RwRaster* source, const RwInt32* position, bool scaled, bool useBlend) {
    if (source == nullptr) {
        return FALSE;
    }
    const uint32 targetType = TargetRasterVar->cType;
    const uint32 sourceType = source->cType;
    switch (targetType) {
    case 0:
    case 4:
        if (sourceType > 5) {
            return TRUE;
        }
        switch (sourceType) {
        case 0:
        case 2:
        case 4:
        case 5:
            return CopyBetweenSurfaces(source, position, scaled) ? TRUE : FALSE;
        case 3:
            return TRUE;
        default:
            return FALSE;
        }
    case 2:
    case 5:
        if (sourceType > 5) {
            return TRUE;
        }
        switch (sourceType) {
        case 0:
        case 4:
        case 5:
            RenderTexturedQuad(source, scaled, position, useBlend);
            return TRUE;
        case 2:
            return CopyBetweenSurfaces(source, position, scaled) ? TRUE : FALSE;
        case 3:
            return TRUE;
        default:
            return FALSE;
        }
    default:
        return FALSE;
    }
}

// 0x4CAE40 (hueco 0x4CAE40; cdecl, (raster, posicion {x, y}, sin uso), 24 bytes; rwSTANDARDRASTERRENDER): RasterBlit sin escalado y con mezcla.
RwBool D3D9RasterStandards::RasterRender(RwRaster* raster, RwInt32* position, RwInt32 /*unused*/) {
    return RasterBlit(raster, position, false, true);
}

// 0x4CAE80 (hueco 0x4CAE80; cdecl, (raster, rectangulo {x, y, ancho, alto}, sin uso), 24 bytes; rwSTANDARDRASTERRENDERSCALED): RasterBlit con escalado y con mezcla.
RwBool D3D9RasterStandards::RasterRenderScaled(RwRaster* raster, RwInt32* rect, RwInt32 /*unused*/) {
    return RasterBlit(raster, rect, true, true);
}

// 0x4CAE60 (hueco 0x4CAE60; cdecl, (raster, posicion {x, y}, sin uso), 24 bytes; rwSTANDARDRASTERRENDERFAST): RasterBlit sin escalado y sin mezcla.
RwBool D3D9RasterStandards::RasterRenderFast(RwRaster* raster, RwInt32* position, RwInt32 /*unused*/) {
    return RasterBlit(raster, position, false, false);
}

// 0x4CAEA0 (ayudante usercall; esi = rectangulo {x, y, ancho, alto}, pila = color; sin enganche): rellena un rectangulo del destino actual. Segun su tipo (cType): 2 (camara): convierte el color a 32 bits si
// la profundidad no lo es, pone la superficie de render y Clear con D3DCLEAR_TARGET; 1 (z-buffer): pone su superficie de profundidad, Clear con D3DCLEAR_ZBUFFER (z 1.0) y restaura la de por defecto; 5 (textura de camara)
// y 0 y 4 (textura) con la textura en el pool por defecto: ColorFill sobre la superficie del nivel 0 (el color de las textura se convierte siempre); 0 y 4 con otro pool: bloquea el raster y escribe el color en cada
// pixel (32 o 16 bits, o el indice de paleta mas cercano con 8; otras profundidades no escriben) y lo desbloquea; 3 y mayores que 5: devuelve 0. Devuelve 1 si rellena (0 si no se pudo o la profundidad no se admite).
RwBool D3D9RasterStandards::RasterFill(const RwInt32* rect, uint32 color) {
    RwRaster* const target = TargetRasterVar;
    switch (target->cType) {
    case 2: {
        uint32 pixel = color;
        if (target->depth != 0x20) {
            uint8 rgba[4];
            _rwD3D9PixelToRGB(rgba, &color, static_cast<uint32>(target->cFormat) << 8);
            _rwD3D9RGBToPixel(&pixel, rgba, 0x500);
        }
        _rwD3D9SetRenderTarget(0, RenderSurface);
        _rwD3D9SetDepthStencilSurface(DefaultDepthSurface);
        const int32 left = target->nOffsetX + rect[0];
        const int32 top  = target->nOffsetY + rect[1];
        const RECT clearRect = { left, top, rect[2] + left, rect[3] + top };
        ComCall<kSlotClear, int32>(D3D9Device, 1u, &clearRect, 1u, pixel, 1.0f, 0u);
        return TRUE;
    }
    case 1: {
        RwRaster* const root = RootOf(target);
        _rwD3D9SetDepthStencilSurface(ExtOf(root).texture);
        const int32 left = target->nOffsetX + rect[0];
        const int32 top  = target->nOffsetY + rect[1];
        const RECT clearRect = { left, top, rect[2] + left, rect[3] + top };
        ComCall<kSlotClear, int32>(D3D9Device, 1u, &clearRect, 2u, 0u, 1.0f, 0u);
        _rwD3D9SetDepthStencilSurface(DefaultDepthSurface);
        return TRUE;
    }
    case 5: {
        uint32 pixel = color;
        if (target->depth != 0x20) {
            uint8 rgba[4];
            _rwD3D9PixelToRGB(rgba, &color, static_cast<uint32>(target->cFormat) << 8);
            _rwD3D9RGBToPixel(&pixel, rgba, 0x500);
        }
        void* const surface = TextureSurface(ExtOf(RootOf(TargetRasterVar)));
        const int32 left = TargetRasterVar->nOffsetX + rect[0];
        const int32 top  = TargetRasterVar->nOffsetY + rect[1];
        const RECT fillRect = { left, top, rect[2] + left, rect[3] + top };
        ComCall<kSlotColorFill, int32>(D3D9Device, surface, &fillRect, pixel);
        ReleaseCom(surface);
        return TRUE;
    }
    case 0:
    case 4: {
        RasterExt& parentExt = ExtOf(target->parent);
        D3DSURFACE_DESC desc;
        ComCall<kSlotGetLevelDesc, int32>(parentExt.texture, 0u, &desc);
        if (desc.Pool == D3DPOOL_DEFAULT) {
            uint8 rgba[4];
            uint32 pixel;
            _rwD3D9PixelToRGB(rgba, &color, static_cast<uint32>(TargetRasterVar->cFormat) << 8);
            _rwD3D9RGBToPixel(&pixel, rgba, 0x500);
            void* surface = nullptr;
            RasterExt& ext = ExtOf(target->parent);
            if (ext.cubeFace & 0x0F) {
                ComCall<kSlotGetSurfaceLevel, int32>(ext.texture, static_cast<uint32>(ext.cubeFace >> 4), 0u, &surface);
            } else {
                ComCall<kSlotGetSurfaceLevel, int32>(ext.texture, 0u, &surface);
            }
            const int32 left = TargetRasterVar->nOffsetX + rect[0];
            const int32 top  = TargetRasterVar->nOffsetY + rect[1];
            const RECT fillRect = { left, top, rect[2] + left, rect[3] + top };
            ComCall<kSlotColorFill, int32>(D3D9Device, surface, &fillRect, pixel);
            ReleaseCom(surface);
            return TRUE;
        }
        RwUInt8* pixels = nullptr;
        if (!D3D9RasterLockCpp(&pixels, TargetRasterVar, 1)) {
            return FALSE;
        }
        RwRaster* const locked = TargetRasterVar;
        bool filled = false;
        if (locked->depth == 0x20) {
            for (RwInt32 row = 0; row < rect[3]; ++row) {
                uint32* line = reinterpret_cast<uint32*>(pixels + static_cast<int32>(rect[1] + row) * locked->stride) + rect[0];
                for (RwInt32 column = 0; column < rect[2]; ++column) {
                    *line++ = color;
                }
            }
            filled = true;
        } else if (locked->depth == 0x10) {
            for (RwInt32 row = 0; row < rect[3]; ++row) {
                uint16* line = reinterpret_cast<uint16*>(pixels + static_cast<int32>(rect[1] + row) * locked->stride) + rect[0];
                for (RwInt32 column = 0; column < rect[2]; ++column) {
                    *line++ = static_cast<uint16>(color);
                }
            }
            filled = true;
        } else if (locked->depth == 8) {
            // Indice de la paleta mas cercano al color (distancia cuadratica por bytes; se detiene en una coincidencia exacta; entre iguales gana el primero).
            const uint8* palette = static_cast<const uint8*>(ExtOf(RootOf(locked)).palette);
            const int32 c0 = color & 0xFF, c1 = (color >> 8) & 0xFF, c2 = (color >> 16) & 0xFF, c3 = (color >> 24) & 0xFF;
            uint32 bestDistance = 0xFFFFFFFFu;
            uint8  bestIndex = 0;
            for (uint32 index = 0; index < 0x100; ++index) {
                const uint8* entry = palette + index * 4;
                const int32 d0 = static_cast<int32>(entry[0]) - c0;
                const int32 d1 = static_cast<int32>(entry[1]) - c1;
                const int32 d2 = static_cast<int32>(entry[2]) - c2;
                const int32 d3 = static_cast<int32>(entry[3]) - c3;
                const uint32 distance = static_cast<uint32>(d3 * d3 + d2 * d2 + d1 * d1 + d0 * d0);
                if (distance < bestDistance) {
                    bestIndex = static_cast<uint8>(index);
                    bestDistance = distance;
                    if (distance == 0) {
                        break;
                    }
                }
            }
            for (RwInt32 row = 0; row < rect[3]; ++row) {
                uint8* line = pixels + static_cast<int32>(rect[1] + row) * TargetRasterVar->stride + rect[0];
                for (RwInt32 column = 0; column < rect[2]; ++column) {
                    *line++ = bestIndex;
                }
            }
            filled = true;
        }
        D3D9RasterUnlockCpp(pixels, TargetRasterVar);
        return filled ? TRUE : FALSE;
    }
    default:
        return FALSE;
    }
}

// 0x4CB4C0 (hueco 0x4CB4C0; cdecl, (sin uso, rectangulo {x, y, ancho, alto}, color), 32 bytes; rwSTANDARDRASTERCLEARRECT): RasterFill del rectangulo; devuelve su resultado.
RwBool D3D9RasterStandards::RasterClearRect(void* /*unused*/, RwInt32* rect, uint32 color) {
    return RasterFill(rect, color);
}

// 0x4CB4E0 (hueco 0x4CB4E0; cdecl, (sin uso, sin uso, color), 64 bytes; rwSTANDARDRASTERCLEAR): RasterFill del destino actual entero ({0, 0, ancho, alto}); devuelve su resultado.
RwBool D3D9RasterStandards::RasterClear(void* /*unused*/, void* /*unused2*/, uint32 color) {
    const RwInt32 rect[4] = { 0, 0, TargetRasterVar->width, TargetRasterVar->height };
    return RasterFill(rect, color);
}

// 0x4CD4D0 (hueco 0x4CD4D0; cdecl, (flujo, textura, tamano sin uso), 834 bytes; rwSTANDARDNATIVETEXTUREWRITE): escribe una textura nativa D3D9. Cabecera de chunk 1 (version 0x36003, libreria 0xFFFF) con el tamano
// que da 0x4CD360 menos 0xC; cabecera de 0x48 bytes (plataforma 9, filtro y direccionamiento reordenados, nombre y mascara) y de 0x10 (formato, formato D3D, ancho, alto, profundidad, niveles, tipo y banderas
// alfa/cubo/mipmaps automaticos/comprimida); la paleta (0x80 con PAL4, 0x400 con PAL8, bloqueada para lectura) y, por cada cara (6 con cubo, que deja la cara de la extension en 6) y cada nivel (solo el primero con
// mipmaps automaticos), bloquea el raster, escribe el tamano de los datos (paso por alto, o por alto/4 si es comprimida) en 4 bytes y los datos en trozos de 0x400. Un fallo de escritura devuelve 0 sin desbloquear.
RwBool D3D9RasterStandards::NativeTextureWrite(RwStream* stream, RwTexture* texture, RwInt32 /*size*/) {
    uint32 size;
    D3D9RasterFormat::GetRasterMemorySize(&size, &texture->raster);
    if (!_rwStreamWriteVersionedChunkHeader(stream, 1, static_cast<RwInt32>(size - 0xC), 0x36003, 0xFFFF)) {
        return FALSE;
    }
    const uint32 filterAddressing = texture->filterAddressing;
    NativeTextureHeader header;
    header.platformId = 9;
    header.filterAddressing = ((((filterAddressing >> 12) & 0xF) << 4 | ((filterAddressing >> 8) & 0xF)) << 8) | (filterAddressing & 0xFF);
    std::memcpy(header.name, texture->name, sizeof header.name);
    std::memcpy(header.mask, texture->mask, sizeof header.mask);
    if (!RwStreamWrite(stream, &header, sizeof header)) {
        return FALSE;
    }
    RwRaster* const raster = texture->raster;
    RasterExt& ext = ExtOf(raster);
    NativeRasterHeader rasterHeader;
    rasterHeader.rasterFormat = static_cast<uint32>(raster->cFormat) << 8;
    rasterHeader.d3dFormat    = ext.d3dFormat;
    rasterHeader.width        = static_cast<uint16>(raster->width);
    rasterHeader.height       = static_cast<uint16>(raster->height);
    rasterHeader.depth        = static_cast<uint8>(raster->depth);
    rasterHeader.numLevels    = static_cast<uint8>(RwRasterGetNumLevels(raster));
    rasterHeader.type         = raster->cType;
    rasterHeader.flags        = 0;
    if (ext.alpha) {
        rasterHeader.flags = 1;
    }
    if (ext.cubeFace & 0x0F) {
        rasterHeader.flags |= 2;
    }
    if (ext.mipFlags & 0x0F) {
        rasterHeader.flags |= 4;
    }
    if (ext.mipFlags & 0xF0) {
        rasterHeader.flags |= 8;
    }
    if (!RwStreamWrite(stream, &rasterHeader, sizeof rasterHeader)) {
        return FALSE;
    }
    const uint32 rasterFormat = rasterHeader.rasterFormat;
    if (rasterFormat & 0x4000) {
        RwStream* const result = RwStreamWrite(stream, RwRasterLockPalette(raster, 2), 0x80);
        if (!result) {
            return FALSE;
        }
        RwRasterUnlockPalette(raster);
    } else if (rasterFormat & 0x2000) {
        RwStream* const result = RwStreamWrite(stream, RwRasterLockPalette(raster, 2), 0x400);
        if (!result) {
            return FALSE;
        }
        RwRasterUnlockPalette(raster);
    }
    ext.cubeFace &= 0x0F;
    uint32 faces = (ext.cubeFace & 0x0F) ? 6u : 1u;
    do {
        const uint32 levels = rasterHeader.numLevels;
        for (uint32 level = 0; level < levels; ++level) {
            RwUInt8* pixels = RwRasterLock(raster, static_cast<RwUInt8>(level), 2);
            if (!pixels) {
                return FALSE;
            }
            D3DSURFACE_DESC desc;
            ComCall<kSlotGetLevelDesc, int32>(ext.texture, level, &desc);
            uint32 dataSize;
            if (ext.mipFlags & 0xF0) {
                dataSize = (desc.Height >> 2) * static_cast<uint32>(ext.lockedPitch);
            } else {
                dataSize = static_cast<uint32>(ext.lockedPitch) * desc.Height;
            }
            if (!RwStreamWrite(stream, &dataSize, 4)) {
                return FALSE;
            }
            for (uint32 chunks = dataSize >> 10; chunks != 0; --chunks) {
                if (!RwStreamWrite(stream, pixels, 0x400)) {
                    return FALSE;
                }
                dataSize -= 0x400;
                pixels += 0x400;
            }
            if (dataSize != 0 && !RwStreamWrite(stream, pixels, dataSize)) {
                return FALSE;
            }
            RwRasterUnlock(raster);
            if (ext.mipFlags & 0x0F) {
                break;
            }
        }
        const uint8 cube = ext.cubeFace & 0x0F;
        if (cube) {
            ext.cubeFace = static_cast<uint8>(((ext.cubeFace & 0xF0) + 0x10) & 0xF0) | cube;
        }
    } while (--faces != 0);
    return TRUE;
}

// 0x4CD820 (hueco 0x4CD820; cdecl, (flujo, RwTexture** salida, tamano sin uso), 1.464 bytes; rwSTANDARDNATIVETEXTUREREAD): lee una textura nativa D3D9 (lo inverso de NativeTextureWrite). Busca el chunk 1 (version
// entre 0x34000 y 0x36003), lee las cabeceras (plataforma 9) y crea el raster segun las banderas: comprimida (8) -> RwRasterCreate con 0x80 y textura o cubo de D3D (CreateTexture 0x730510 o
// CreateCubeTexture; el formato debe pasar 0x4CBDE0, los cubos sin mipmaps si el dispositivo no los admite), alfa/cubo/mipmaps automaticos en la extension; solo cubo (2) -> RwRasterCreate con 0x80 y cubo;
// otro con bits fuera de 0x9000 -> RwRasterCreate directo; si no 0x4CD050 (raster a partir del formato D3D). Comprueba que formato, formato D3D, ancho y alto coincidan con los del raster creado, lee la
// paleta (0x80 con PAL4, 0x400 con PAL8) y, por cada cara y nivel, bloquea el raster, lee el tamano (4 bytes) y los datos y desbloquea; crea la textura, restaura filtro/direccionamiento (los 16 bits bajos) y
// nombre y mascara y la devuelve en *salida. Cualquier fallo destruye el raster (con su paleta o nivel bloqueado liberados) y devuelve 0.
RwBool D3D9RasterStandards::NativeTextureRead(RwStream* stream, RwTexture** textureOut, RwInt32 /*size*/) {
    RwUInt32 length, version;
    if (!RwStreamFindChunk(stream, 1, &length, &version)) {
        return FALSE;
    }
    if (version < 0x34000 || version > 0x36003) {
        return FALSE;
    }
    NativeTextureHeader header;
    if (RwStreamRead(stream, &header, sizeof header) != sizeof header || header.platformId != 9) {
        return FALSE;
    }
    NativeRasterHeader rasterHeader;
    if (RwStreamRead(stream, &rasterHeader, sizeof rasterHeader) != sizeof rasterHeader) {
        return FALSE;
    }
    const uint32 rasterFormat = rasterHeader.rasterFormat;
    const uint32 d3dFormat    = rasterHeader.d3dFormat;
    const uint8  flags        = rasterHeader.flags;
    const RwInt32 width = rasterHeader.width, height = rasterHeader.height;
    const RwInt32 depth = rasterHeader.depth;
    const RwInt32 type  = rasterHeader.type;

    // Creacion del raster y de su recurso D3D segun las banderas.
    RwRaster* raster;
    RasterExt* ext;
    if ((flags & 8) || (flags & 2)) {
        if (flags & 8) {
            if (!RwD3D9RasterDriver::CheckTextureFormatCpp(d3dFormat)) {
                return FALSE;
            }
        }
        raster = RwRasterCreate(width, height, depth, type | 0x80 | static_cast<RwInt32>(rasterFormat));
        if (!raster) {
            return FALSE;
        }
        ext = &ExtOf(raster);
        const uint32 levels = (raster->cFormat & 0x80) ? rasterHeader.numLevels : 1u;
        if (flags & 2) {
            if (!(CubeMipCapsFlags & 0x30000) && levels > 1) {
                RwRasterDestroy(raster);
                return FALSE;
            }
            ext->cubeFace = (ext->cubeFace & 0xF1) | 1;
            if ((rasterFormat & 0x9000) == 0x9000 && RwD3D9RasterDriver::CheckCubeFormatUsage400Cpp(d3dFormat)) {
                ext->mipFlags = (ext->mipFlags & 0xF1) | 1;
            }
            const uint32 usage = (ext->mipFlags & 0x0F) ? 0x400u : 0u;
            if (ComCall<kSlotCreateCubeTexture, int32>(D3D9Device, static_cast<uint32>(raster->width), levels, usage, d3dFormat, 1u, &ext->texture, static_cast<void*>(nullptr)) < 0) {
                RwRasterDestroy(raster);
                return FALSE;
            }
        } else {
            if ((rasterFormat & 0x9000) == 0x9000 && RwD3D9RasterDriver::CheckTextureFormatUsage400Cpp(d3dFormat)) {
                ext->mipFlags = (ext->mipFlags & 0xF1) | 1;
            }
            if (D3DResourceSystem::CreateTexture(raster->width, raster->height, static_cast<int32>(levels), d3dFormat, &ext->texture) < 0) {
                RwRasterDestroy(raster);
                return FALSE;
            }
        }
        raster->cFlags &= 0x7F;
        ext->alpha = flags & 1;
        if (flags & 8) {
            if (flags & 2) {
                ext->cubeFace = (ext->cubeFace & 0xF1) | 1;
            }
            ext->mipFlags = (ext->mipFlags & 0x0F) | 0x10;
            ext->d3dFormat = d3dFormat;
        }
    } else if (rasterFormat & 0xFFFF6FFFu) {
        raster = RwRasterCreate(width, height, depth, type | static_cast<RwInt32>(rasterFormat));
        if (!raster) {
            return FALSE;
        }
        ext = &ExtOf(raster);
        ext->mipFlags &= 0x0F;
    } else {
        raster = D3D9RasterFormat::CreateRasterFromD3DFormat(width, height, static_cast<RwInt32>(d3dFormat), static_cast<RwUInt32>(type) | (rasterFormat & 0x9000));
        if (!raster) {
            return FALSE;
        }
        ext = &ExtOf(raster);
    }

    // Comprobaciones contra el raster creado.
    if ((flags & 4) && !(ext->mipFlags & 0x0F)) {
        RwRasterDestroy(raster);
        return FALSE;
    }
    if (rasterFormat != (static_cast<uint32>(raster->cFormat) << 8) || d3dFormat != ext->d3dFormat || width != raster->width || height != raster->height) {
        RwRasterDestroy(raster);
        return FALSE;
    }
    if (rasterFormat & 0x4000) {
        if (RwStreamRead(stream, RwRasterLockPalette(raster, 1), 0x80) != 0x80) {
            RwRasterUnlockPalette(raster);
            RwRasterDestroy(raster);
            return FALSE;
        }
        RwRasterUnlockPalette(raster);
    } else if (rasterFormat & 0x2000) {
        if (RwStreamRead(stream, RwRasterLockPalette(raster, 1), 0x400) != 0x400) {
            RwRasterUnlockPalette(raster);
            RwRasterDestroy(raster);
            return FALSE;
        }
        RwRasterUnlockPalette(raster);
    }

    // Datos de los niveles de cada cara.
    const uint8 savedFormatBit = raster->cFormat & 0x10;
    raster->cFormat &= static_cast<uint8>(~0x10);
    uint32 faces = (ext->cubeFace & 0x0F) ? 6u : 1u;
    const uint32 levelsToRead = (ext->mipFlags & 0x0F) ? 1u : static_cast<uint32>(rasterHeader.numLevels);
    ext->cubeFace &= 0x0F;
    do {
        for (uint32 level = 0; level < levelsToRead; ++level) {
            RwUInt8* pixels = RwRasterLock(raster, static_cast<RwUInt8>(level), 1);
            uint32 dataSize;
            if (RwStreamRead(stream, &dataSize, 4) != 4 || RwStreamRead(stream, pixels, dataSize) != dataSize) {
                RwRasterUnlock(raster);
                RwRasterDestroy(raster);
                return FALSE;
            }
            RwRasterUnlock(raster);
        }
        const uint8 cube = ext->cubeFace & 0x0F;
        if (cube) {
            ext->cubeFace = static_cast<uint8>(((ext->cubeFace & 0xF0) + 0x10) & 0xF0) | cube;
        }
    } while (--faces != 0);
    raster->cFormat |= savedFormatBit;

    RwTexture* const texture = RwTextureCreate(raster);
    if (!texture) {
        RwRasterDestroy(raster);
        return FALSE;
    }
    texture->filterAddressing = (texture->filterAddressing & 0xFFFF0000u) | (header.filterAddressing & 0xFFFFu);
    RwTextureSetName(texture, header.name);
    RwTextureSetMaskName(texture, header.mask);
    *textureOut = texture;
    return TRUE;
}

// Registra los manejadores de raster de la tabla estandar (huecos de codigo vivo del EXE).
void D3D9RasterStandards::InjectHooks() {
    RH_ScopedClass(D3D9RasterStandards);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SetTextureRaster, 0x4CBD40);      // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(SetRasterContext, 0x4CB520);      // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(RasterSubRaster, 0x4CBD50);       // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(RasterClearRect, 0x4CB4C0);       // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(RasterClear, 0x4CB4E0);           // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(RasterRender, 0x4CAE40);          // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(RasterRenderScaled, 0x4CAE80);    // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(RasterRenderFast, 0x4CAE60);      // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(RasterLockPalette, 0x4CA4E0);     // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(NativeTextureRead, 0x4CD820);     // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(NativeTextureWrite, 0x4CD4D0);    // codigo vivo D3DSTD (sin validar)
    RH_ScopedInstall(RasterGetMipLevels, 0x4CBCB0);    // codigo vivo D3DSTD (sin validar)
}
