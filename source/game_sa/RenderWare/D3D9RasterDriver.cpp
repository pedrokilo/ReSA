/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RwD3D9RasterDriver
 * Reconstrucción: COMPLETA — 15/15 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/15 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "D3D9RasterDriver.h"
#include "D3DResourceSystem.h"
#include "rw/rwplcore.h"
#include "rw/rwcore.h"

void _rwD3D9CheckValidTextureSize(RwInt32* width, RwInt32* height, RwUInt32 format); // 0x7FED70 (rwcore.cpp)

// Estado del controlador de rasters D3D9 que leen o escriben estas funciones (direcciones del EXE):
//   0xB4E9E0 desplazamiento de la extension D3D9 dentro del raster      0xB4E9FC cabeza de la lista de rasters con recursos D3D
//   0xB4EA00 RwFreeList de los nodos de esa lista (8 bytes)             0xB4E9F8 RwFreeList de las paletas (0x404 bytes)
//   0xB4E9E8 numero de indices libres de paleta (pila)                  0xB4E9EC capacidad de esa pila (en uint16)
//   0xB4E9F0 siguiente indice de paleta nuevo                           0xB4E9F4 datos de la pila de indices libres (uint16*)
//   0xC97B24 RwEngineInstance (funciones de memoria +0x134 malloc, +0x138 free, +0x13C realloc, +0x144 RwFreeListAlloc, +0x148 RwFreeListFree)
//   0xC97C20 IDirect3D9* ; 0xC97C24 adaptador ; 0x8E2428 tipo de dispositivo ; 0xC9BEF0 formato del adaptador ; 0xC9C068 formato de profundidad
//   0xC9BF0C banderas de capacidades (bit 0x40000000: bump mapping) ; 0xC97C2C superficie de profundidad por defecto
// Extension D3D9 del raster (raster + [0xB4E9E0]): +0x00 textura/superficie COM, +0x04 bloque de paleta (+0x400 = su indice), +0x18 D3DFORMAT, +0x1C objeto COM del tipo 2.
namespace {
struct RasterListNode {
    RwRaster*       raster; // +0x00
    RasterListNode* next;   // +0x04
};
static_assert(sizeof(RasterListNode) == 8);

auto& RasterExtOffset      = StaticRef<int32>(0xB4E9E0);
auto& RasterListHead       = StaticRef<RasterListNode*>(0xB4E9FC);
auto& RasterListFreeList   = StaticRef<RwFreeList*>(0xB4EA00);
auto& PaletteFreeList      = StaticRef<RwFreeList*>(0xB4E9F8);
auto& FreePaletteCount     = StaticRef<uint32>(0xB4E9E8);
auto& FreePaletteCapacity  = StaticRef<uint32>(0xB4E9EC);
auto& NextPaletteIndex     = StaticRef<uint32>(0xB4E9F0);
auto& FreePaletteIndices   = StaticRef<uint16*>(0xB4E9F4);
auto& DefaultDepthSurface  = StaticRef<void*>(0xC97C2C);
auto& Direct3DObject       = StaticRef<void*>(0xC97C20);
auto& AdapterIndex         = StaticRef<uint32>(0xC97C24);
auto& DeviceType           = StaticRef<uint32>(0x8E2428);
auto& AdapterFormat        = StaticRef<uint32>(0xC9BEF0);
auto& DepthStencilFormat   = StaticRef<uint32>(0xC9C068);
auto& DeviceCapsFlags      = StaticRef<uint32>(0xC9BF0C);

constexpr uint32 kMemoryHint = 0x30411;
constexpr uint32 kPaletteIndexHint = 0x1030411;
constexpr size_t kSlotRelease = 2;                 // IUnknown::Release (+8)
constexpr size_t kSlotCheckDeviceFormat = 10;      // IDirect3D9::CheckDeviceFormat (+0x28)
constexpr size_t kSlotCheckDepthStencilMatch = 12; // IDirect3D9::CheckDepthStencilMatch (+0x30)

// Llamada COM stdcall por la tabla virtual (this primero); la tabla se lee en el momento de la llamada.
template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}

// Funciones de memoria del motor, leidas de RwEngineInstance (0xC97B24) en cada llamada (cdecl).
template<size_t Offset, typename R, typename... Args>
R EngineCall(Args... args) {
    const uintptr_t engine = StaticRef<uintptr_t>(0xC97B24);
    return (*reinterpret_cast<R(__cdecl**)(Args...)>(engine + Offset))(args...);
}
constexpr size_t kEngineMalloc = 0x134, kEngineFree = 0x138, kEngineRealloc = 0x13C, kEngineFreeListAlloc = 0x144, kEngineFreeListFree = 0x148;

inline uint8* RasterExt(RwRaster* raster, int32 offset) {
    return reinterpret_cast<uint8*>(static_cast<uintptr_t>(reinterpret_cast<uintptr_t>(raster) + static_cast<uint32>(offset)));
}
template<typename T>
inline T& ExtField(uint8* ext, size_t offset) {
    return *reinterpret_cast<T*>(ext + offset);
}
inline void ReleaseCom(void* object) {
    ComCall<kSlotRelease, ULONG>(object);
}
// Formatos de 0x46 a 0x50: rango con signo del EXE (`cmp 46h; jl`, `cmp 50h; jg`).
inline bool IsFormatInReleaseRange(int32 format) {
    return format >= 0x46 && format <= 0x50;
}

// 0x4CB560 (ESI = raster; sin hook: argumento en registro): quita de la lista el nodo del raster y lo devuelve a la lista libre.
void RemoveRasterFromList(RwRaster* raster) {
    RasterListNode* node = RasterListHead;
    RasterListNode* previous = nullptr;
    RasterListNode* const head = node;
    if (!node) {
        return;
    }
    while (node->raster != raster) {
        previous = node;
        node = node->next;
        if (!node) {
            return;
        }
    }
    if (previous) {
        previous->next = node->next;
    } else if (head == node) {
        RasterListHead = node->next;
    }
    EngineCall<kEngineFreeListFree, void>(RasterListFreeList, node);
}

// 0x4CB730 (ESI = indice; sin hook: argumento en registro): devuelve un indice de paleta a la pila de libres (uint16), ampliandola de 0x200 en 0x200 cuando esta llena.
void PushFreePaletteIndex(uint32 index) {
    uint32 count = FreePaletteCount;
    uint32 capacity = FreePaletteCapacity;
    if (count < capacity) {
        FreePaletteIndices[count] = static_cast<uint16>(index);
        FreePaletteCount = count + 1;
        return;
    }
    capacity += 0x200;
    FreePaletteCapacity = capacity;
    uint16* data = FreePaletteIndices;
    if (!data) {
        data = EngineCall<kEngineMalloc, uint16*>(capacity * 2, kPaletteIndexHint);
    } else {
        data = EngineCall<kEngineRealloc, uint16*>(data, capacity * 2, kPaletteIndexHint);
    }
    count = FreePaletteCount;
    data[count] = static_cast<uint16>(index);
    FreePaletteIndices = data;
    FreePaletteCount = count + 1;
}

// 0x4CB530 (cdecl, un argumento): alta de un raster al principio de la lista (nodo de la lista libre 0xB4EA00).
__declspec(noinline) void __cdecl RasterListPush(RwRaster* raster) {
    auto* const node = EngineCall<kEngineFreeListAlloc, RasterListNode*>(RasterListFreeList, kMemoryHint);
    node->raster = raster;
    node->next = RasterListHead;
    RasterListHead = node;
}

// 0x4CB5E0 (cdecl, sin argumentos): vacia la lista devolviendo cada nodo a la lista libre y destruye esa lista.
__declspec(noinline) void __cdecl RasterListDestroy() {
    RasterListNode* node = RasterListHead;
    if (node) {
        RasterListNode* next;
        do {
            next = node->next;
            EngineCall<kEngineFreeListFree, void>(RasterListFreeList, node);
            node = next;
            RasterListHead = node;
        } while (next);
    }
    if (RasterListFreeList) {
        RwFreeListDestroy(RasterListFreeList);
        RasterListFreeList = nullptr;
    }
}

// 0x4CB640 (cdecl, sin argumentos): recorre la lista y suelta los recursos D3D de los rasters raiz (sin tocar la lista), segun su tipo:
// 0 y 4: textura (solo formatos 0x46..0x50) y puesta a cero; 5: textura y puesta a cero; 1: superficie distinta de la de profundidad por defecto y puesta a cero; 2: objeto en +0x1C (sin poner a cero).
__declspec(noinline) void __cdecl RasterListReleaseResources() {
    RasterListNode* node = RasterListHead;
    if (!node) {
        return;
    }
    do {
        RwRaster* const raster = node->raster;
        RasterListNode* const next = node->next;
        uint8* const ext = RasterExt(raster, RasterExtOffset);
        if (raster->parent == raster && raster->cType <= 5) {
            switch (raster->cType) {
            case 0:
            case 4: {
                void* const resource = ExtField<void*>(ext, 0);
                if (resource && IsFormatInReleaseRange(ExtField<int32>(ext, 0x18))) {
                    ReleaseCom(resource);
                    ExtField<void*>(ext, 0) = nullptr;
                }
                break;
            }
            case 5: {
                void* const resource = ExtField<void*>(ext, 0);
                if (resource) {
                    ReleaseCom(resource);
                    ExtField<void*>(ext, 0) = nullptr;
                }
                break;
            }
            case 1: {
                void* const resource = ExtField<void*>(ext, 0);
                if (resource && resource != DefaultDepthSurface) {
                    ReleaseCom(resource);
                    ExtField<void*>(ext, 0) = nullptr;
                }
                break;
            }
            case 2: {
                void* const object = ExtField<void*>(ext, 0x1C);
                if (object) {
                    ReleaseCom(object);
                }
                break;
            }
            default:
                break;
            }
        }
        node = next;
    } while (node);
}

// 0x4CBB00 (cdecl, tres argumentos de la tabla de dispositivo (dispositivo sin uso, raster, indicadores sin uso); el EXE lee el raster en [esp+0Ch]): destruccion del raster D3D9. Retira el raster de las etapas de textura (0x7FDE50 / 0x7FDCD0) y, si es raiz, libera segun su tipo
// (0: paleta + textura; 1: superficie de profundidad; 2: solo lista; 4: como 0 pero la textura vuelve a la cache de D3DResourceSystem; 5: lista + textura; 3 y > 5: devuelve 0).
// Con la bandera 0x80 de cFlags no libera nada. Devuelve 1 salvo en los tipos 3 y > 5.
__declspec(noinline) int32 __cdecl RasterDestroy(void* /*device*/, RwRaster* raster, int32 /*flags*/) {
    uint8* const ext = RasterExt(raster, RasterExtOffset);
    using GetStageFn = RwRaster*(__cdecl*)(uint32);
    using SetStageFn = void(__cdecl*)(RwRaster*, uint32);
    for (uint32 stage = 0; stage < 8; ++stage) {
        if (reinterpret_cast<GetStageFn>(0x7FDE50)(stage) == raster) {
            reinterpret_cast<SetStageFn>(0x7FDCD0)(nullptr, stage);
        }
    }
    if (raster->parent != raster) {
        if (raster->cType == 2) {
            RemoveRasterFromList(raster);
        }
        return 1;
    }
    // El tipo se lee UNA vez (el EXE salta por la tabla 0x4CBC94 y los tipos 0 y 4 son dos ramas distintas): las llamadas siguientes no lo vuelven a mirar.
    const uint8 type = raster->cType;
    if (type > 5) {
        return 0;
    }
    switch (type) {
    case 0:
    case 4: {
        if (static_cast<int8>(raster->cFlags) < 0) {
            return 1;
        }
        if (ExtField<void*>(ext, 4)) {
            RemoveRasterFromList(raster);
            const uint32 paletteIndex = *reinterpret_cast<uint32*>(ExtField<uint8*>(ext, 4) + 0x400);
            PushFreePaletteIndex(paletteIndex);
            EngineCall<kEngineFreeListFree, void>(PaletteFreeList, ExtField<void*>(ext, 4));
            ExtField<void*>(ext, 4) = nullptr;
        } else if (IsFormatInReleaseRange(ExtField<int32>(ext, 0x18))) {
            RemoveRasterFromList(raster);
        }
        void* const texture = ExtField<void*>(ext, 0);
        if (!texture) {
            return 1;
        }
        if (type == 0) {
            ReleaseCom(texture);
        } else {
            D3DResourceSystem::DestroyTexture(texture);
        }
        return 1;
    }
    case 1: {
        if (static_cast<int8>(raster->cFlags) < 0) {
            return 1;
        }
        RemoveRasterFromList(raster);
        void* const surface = ExtField<void*>(ext, 0);
        if (surface == DefaultDepthSurface || !surface) {
            return 1;
        }
        ReleaseCom(surface);
        return 1;
    }
    case 2:
        RemoveRasterFromList(raster);
        return 1;
    case 5: {
        if (static_cast<int8>(raster->cFlags) < 0) {
            return 1;
        }
        RemoveRasterFromList(raster);
        void* const texture = ExtField<void*>(ext, 0);
        if (!texture) {
            return 1;
        }
        ReleaseCom(texture);
        return 1;
    }
    default: // 3
        return 0;
    }
}

// Consultas a IDirect3D9 (CheckDeviceFormat = ranura 10, CheckDepthStencilMatch = ranura 12); el objeto, el adaptador y el tipo de dispositivo se releen de las globales en cada llamada. El formato del adaptador
// (0xC9BEF0) NO: las rutinas que hacen dos consultas lo leen una sola vez (esi) y lo reutilizan en la segunda; la profundidad (0xC9C068) se lee justo antes de la segunda consulta.
inline int32 CheckDeviceFormat(uint32 adapterFormat, uint32 usage, uint32 resourceType, uint32 checkFormat) {
    void* const d3d = Direct3DObject;
    return ComCall<kSlotCheckDeviceFormat, int32>(d3d, AdapterIndex, DeviceType, adapterFormat, usage, resourceType, checkFormat);
}
inline int32 CheckDepthStencilMatch(uint32 adapterFormat, uint32 renderTargetFormat, uint32 depthFormat) {
    void* const d3d = Direct3DObject;
    return ComCall<kSlotCheckDepthStencilMatch, int32>(d3d, AdapterIndex, DeviceType, adapterFormat, renderTargetFormat, depthFormat);
}

// 0x4CBD80 (cdecl, formato): superficie D3DUSAGE_DEPTHSTENCIL (2, tipo 1) valida y compatible con el formato del adaptador como destino de render y profundidad.
__declspec(noinline) int32 __cdecl CheckDepthStencilSurfaceFormat(uint32 format) {
    const uint32 adapterFormat = AdapterFormat;
    int32 hr = CheckDeviceFormat(adapterFormat, 2, 1, format);
    if (hr >= 0) {
        hr = CheckDepthStencilMatch(adapterFormat, adapterFormat, format);
    }
    return hr >= 0;
}

// 0x4CBDE0 (cdecl, formato): textura (tipo 3) con uso 0.
__declspec(noinline) int32 __cdecl CheckTextureFormat(uint32 format) {
    return CheckDeviceFormat(AdapterFormat, 0, 3, format) >= 0;
}

// 0x4CBE20 (cdecl, formato): textura como destino de render (uso 1) y su pareja con el formato de profundidad 0xC9C068.
__declspec(noinline) int32 __cdecl CheckRenderTargetTextureFormat(uint32 format) {
    const uint32 adapterFormat = AdapterFormat;
    int32 hr = CheckDeviceFormat(adapterFormat, 1, 3, format);
    if (hr >= 0) {
        hr = CheckDepthStencilMatch(adapterFormat, format, DepthStencilFormat);
    }
    return hr >= 0;
}

// 0x4CBE80 (cdecl, formato): textura con uso D3DUSAGE_DEPTHSTENCIL (2), compatible con el formato del adaptador.
__declspec(noinline) int32 __cdecl CheckDepthStencilTextureFormat(uint32 format) {
    const uint32 adapterFormat = AdapterFormat;
    int32 hr = CheckDeviceFormat(adapterFormat, 2, 3, format);
    if (hr >= 0) {
        hr = CheckDepthStencilMatch(adapterFormat, adapterFormat, format);
    }
    return hr >= 0;
}

// 0x4CBF20 (cdecl, formato): textura de cubo (tipo 5) como destino de render (uso 1) y su pareja con el formato de profundidad 0xC9C068.
__declspec(noinline) int32 __cdecl CheckRenderTargetCubeFormat(uint32 format) {
    const uint32 adapterFormat = AdapterFormat;
    int32 hr = CheckDeviceFormat(adapterFormat, 1, 5, format);
    if (hr >= 0) {
        hr = CheckDepthStencilMatch(adapterFormat, format, DepthStencilFormat);
    }
    return hr >= 0;
}

// Variantes con la bandera 0x40000000 de 0xC9BF0C (sin ella devuelven 0 sin consultar): 1 solo si CheckDeviceFormat devuelve exactamente 0 (S_OK).
inline int32 CheckBumpFormat(uint32 usage, uint32 resourceType, uint32 format) {
    if (!(DeviceCapsFlags & 0x40000000u)) {
        return 0;
    }
    return CheckDeviceFormat(AdapterFormat, usage, resourceType, format) == 0;
}
// 0x4CBF80: textura (tipo 3), uso 0x400.
__declspec(noinline) int32 __cdecl CheckTextureFormatUsage400(uint32 format) { return CheckBumpFormat(0x400, 3, format); }
// 0x4CBFD0: textura (tipo 3), uso 0x401.
__declspec(noinline) int32 __cdecl CheckTextureFormatUsage401(uint32 format) { return CheckBumpFormat(0x401, 3, format); }
// 0x4CC020: textura de cubo (tipo 5), uso 0x400.
__declspec(noinline) int32 __cdecl CheckCubeFormatUsage400(uint32 format) { return CheckBumpFormat(0x400, 5, format); }

// 0x4CC150 (cdecl, sin argumentos): crea la lista libre de nodos (8 bytes, 0x7F por bloque, alineacion 4) y la deja en 0xB4EA00 (sin comprobar el resultado).
__declspec(noinline) void __cdecl RasterListInit() {
    RasterListFreeList = RwFreeListCreate(8, 0x7F, 4, kMemoryHint);
}

// 0x4CC170 (cdecl, sin argumentos): vacia la lista (0x4CB5E0), pone a cero los contadores de la pila de indices y libera sus datos y la lista libre de paletas.
__declspec(noinline) void __cdecl RasterListShutdown() {
    RasterListDestroy();
    uint16* const data = FreePaletteIndices;
    FreePaletteCount = 0;
    FreePaletteCapacity = 0;
    NextPaletteIndex = 0;
    if (data) {
        EngineCall<kEngineFree, void>(data);
        FreePaletteIndices = nullptr;
    }
    RwFreeList* const palettes = PaletteFreeList;
    if (palettes) {
        RwFreeListDestroy(palettes);
        PaletteFreeList = nullptr;
    }
}

namespace RasterCreation { // espacio propio: el archivo puede compilarse junto a D3D9RasterFormat.cpp (unity) y ambos tienen nombres iguales en su espacio anonimo
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
// Lote S2 (codigo vivo, parte P3): creacion de rasters D3D9. Las cinco rutinas 0x4CB7C0, 0x4CB9C0, 0x4CC5C0, 0x4CCD70 y 0x4CCE60 del EXE son usercall (raster en EDI/ESI y banderas en EAX); aqui
// reciben todo por parametros y 0x4CB730 (PushFreePaletteIndex, arriba) pierde su ultimo llamador del EXE. Solo 0x4CCE60 tiene hook (cdecl; es la entrada `rasterCreate` de la tabla de dispositivo,
// indice 4); el resto son ayudantes locales: el EXE ya no los llama (su unico llamador era 0x4CCE60) y quedan descartados.
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
auto& AdapterDisplayDepth  = StaticRef<int32>(0xC9BEF4);  // _RwD3D9AdapterInformation.displayDepth
auto& ZBufferDepth         = StaticRef<int32>(0xC9BEFC);  // _RwD3D9ZBufferDepth
auto& BackBufferFormat     = StaticRef<int32>(0xC9C048);  // Present.BackBufferFormat
auto& MultiSampleType      = StaticRef<uint32>(0xC9C050);
auto& MultiSampleQuality   = StaticRef<uint32>(0xC9C054);
auto& TextureCapsLowByte   = StaticRef<uint8>(0xC9BF3C);  // byte bajo de D3DCAPS9.TextureCaps (bit 7 = ALPHAPALETTE)
auto& DeviceWindow         = StaticRef<HWND>(0xC97C1C);
auto& D3DDevice            = StaticRef<void*>(0xC97C28);

constexpr int32 kD3DERR_OUTOFVIDEOMEMORY = static_cast<int32>(0x8876017C);
constexpr size_t kSlotEvictManagedResources = 5;       // IDirect3DDevice9::EvictManagedResources (+0x14)
constexpr size_t kSlotCreateTexture = 23;               // IDirect3DDevice9::CreateTexture (+0x5C)
constexpr size_t kSlotCreateCubeTexture = 25;           // IDirect3DDevice9::CreateCubeTexture (+0x64)
constexpr size_t kSlotCreateDepthStencilSurface = 29;   // IDirect3DDevice9::CreateDepthStencilSurface (+0x74)

// Tabla de formatos RW (0x85C670), 8 bytes por (formato de pixel >> 8): formato D3D (dword), profundidad, alpha. Tabla _rwD3D9PixelFormatInfo (0xB4E7E0), 4 bytes por D3DFORMAT: alpha, profundidad, formato RW (16 bits).
struct RwFormatEntry {
    uint32 d3dFormat;
    uint8  depth;
    uint8  alpha;
    uint8  pad[2];
};
static_assert(sizeof(RwFormatEntry) == 8);
struct PixelFormatEntry {
    uint8  alpha;
    uint8  depth;
    uint16 rwFormat;
};
static_assert(sizeof(PixelFormatEntry) == 4);
inline const RwFormatEntry& RwFormatInfo(uint32 formatIndex) { return reinterpret_cast<const RwFormatEntry*>(0x85C670)[formatIndex]; }
inline const PixelFormatEntry& PixelFormatInfo(int32 d3dFormat) { return reinterpret_cast<const PixelFormatEntry*>(0xB4E7E0)[d3dFormat]; }

void* EngineFreeListAllocHint(RwFreeList* list) { return EngineCall<kEngineFreeListAlloc, void*>(list, kMemoryHint); }

// 0x4CB7C0 (usercall: stack = raster, EDI = extension; `ret`, 512 bytes; sin hook): crea la textura de un raster de tipo 0 (y 4). Raster sin paleta: formato D3D de la tabla de formatos RW. Con paleta de 8 bits (0x500 o 0x600; otro formato con paleta da 0):
// bloque de paleta de la lista libre 0xB4E9F8 (se crea al primer uso, 0x404 bytes) con su indice (de la pila de libres o el siguiente) en +0x400, formato D3DFMT_P8 (0x29). Ajusta el tamano al dispositivo (0x7FED70), recoge el alfa de la tabla y crea
// segun el caso: cubo (CreateCubeTexture, administrada), textura de profundidad para los formatos 0x46..0x50 (CreateTexture con uso 2 en la piscina por defecto; ademas alta en la lista de rasters) o textura normal por D3DResourceSystem::CreateTexture (0x730510).
// Si falla y el formato era P8 con paleta, devuelve el indice y el bloque; con exito y paleta, alta en la lista de rasters. Devuelve 1 si va bien.
int32 RasterCreateTexture(RwRaster* raster, uint8* ext) {
    const uint32 format = static_cast<uint32>(raster->cFormat) << 8;
    if ((format & 0x6000u) == 0) {
        ExtField<int32>(ext, 0x18) = static_cast<int32>(RwFormatInfo((format >> 8) & 0xFu).d3dFormat);
    } else {
        const uint32 palettedFormat = format & 0xF00u;
        if (palettedFormat != 0x600u && palettedFormat != 0x500u) {
            return 0;
        }
        if (!PaletteFreeList) {
            PaletteFreeList = RwFreeListCreate(0x404, 0x40, 4, kMemoryHint);
        }
        uint8* const palette = static_cast<uint8*>(EngineFreeListAllocHint(PaletteFreeList));
        ExtField<uint8*>(ext, 4) = palette;
        if (palette) {
            uint32 paletteIndex;
            if (FreePaletteCount != 0) {
                --FreePaletteCount;
                paletteIndex = FreePaletteIndices[FreePaletteCount];
            } else {
                paletteIndex = NextPaletteIndex++;
            }
            *reinterpret_cast<uint32*>(palette + 0x400) = paletteIndex;
        }
        ExtField<int32>(ext, 0x18) = 0x29;
    }
    ExtField<uint8>(ext, 8) = RwFormatInfo((format >> 8) & 0xFu).alpha;
    _rwD3D9CheckValidTextureSize(&raster->width, &raster->height, static_cast<uint32>(raster->cType) | format);

    const uint32 levels = (~(format >> 15)) & 1u;
    int32 hr;
    if (ExtField<uint8>(ext, 9) & 0xFu) {
        const uint32 usage = (ExtField<uint8>(ext, 0xA) & 0xFu) ? 0x400u : 0u;
        hr = ComCall<kSlotCreateCubeTexture, int32>(D3DDevice, static_cast<uint32>(raster->width), levels, usage, static_cast<uint32>(ExtField<int32>(ext, 0x18)), 1u, static_cast<void*>(ext), static_cast<void*>(nullptr));
    } else if (IsFormatInReleaseRange(ExtField<int32>(ext, 0x18))) {
        hr = ComCall<kSlotCreateTexture, int32>(D3DDevice, static_cast<uint32>(raster->width), static_cast<uint32>(raster->height), 1u, 2u, static_cast<uint32>(ExtField<int32>(ext, 0x18)), 0u,
                                                static_cast<void*>(ext), static_cast<void*>(nullptr));
        if (hr >= 0) {
            RasterListPush(raster);
        }
    } else {
        hr = D3DResourceSystem::CreateTexture(raster->width, raster->height, static_cast<int32>(levels), static_cast<uint32>(ExtField<int32>(ext, 0x18)), reinterpret_cast<void**>(ext));
    }
    if (hr < 0) {
        if (ExtField<int32>(ext, 0x18) == 0x29 && ExtField<uint8*>(ext, 4)) {
            PushFreePaletteIndex(*reinterpret_cast<uint32*>(ExtField<uint8*>(ext, 4) + 0x400));
            EngineCall<kEngineFreeListFree, void>(PaletteFreeList, ExtField<void*>(ext, 4));
            ExtField<void*>(ext, 4) = nullptr;
        }
        return 0;
    }
    if (ExtField<uint8*>(ext, 4)) {
        auto* const node = static_cast<RasterListNode*>(EngineFreeListAllocHint(RasterListFreeList));
        node->raster = raster;
        node->next = RasterListHead;
        RasterListHead = node;
    }
    return 1;
}

// 0x4CB9C0 (usercall: stack = raster, ESI = extension; `ret`, 310 bytes; sin hook): crea la textura (o cubo) de un raster de tipo 5 como destino de render. Sin paleta (bits 0x6000 de la clase, si no devuelve 0): formato D3D y alfa de la tabla, tamano valido
// (0x7FED70) y EvictManagedResources. Primer intento con el formato del raster (uso 1 = destino de render, piscina por defecto; con generacion de mipmaps +0x400); OUTOFVIDEOMEMORY (0x8876017C) devuelve 0; si falla por otra razon reintenta con el
// formato del buffer trasero (0xC9C048, sin el uso 1), y con exito deja cFormat de la tabla de formatos de pixel y la profundidad de pantalla. Devuelve 1 si va bien.
int32 RasterCreateRenderTargetTexture(RwRaster* raster, uint8* ext) {
    const uint32 format = static_cast<uint32>(raster->cFormat) << 8;
    if (format & 0x6000u) {
        return 0;
    }
    const RwFormatEntry& entry = RwFormatInfo((format >> 8) & 0xFu);
    ExtField<int32>(ext, 0x18) = static_cast<int32>(entry.d3dFormat);
    ExtField<uint8>(ext, 8) = entry.alpha;
    _rwD3D9CheckValidTextureSize(&raster->width, &raster->height, static_cast<uint32>(raster->cType) | format);
    ComCall<kSlotEvictManagedResources, int32>(D3DDevice);

    const uint32 levels = (~(format >> 15)) & 1u;
    const auto create = [&](uint32 d3dFormat, uint32 usage) -> int32 {
        if (ExtField<uint8>(ext, 9) & 0xFu) {
            return ComCall<kSlotCreateCubeTexture, int32>(D3DDevice, static_cast<uint32>(raster->width), levels, usage, d3dFormat, 0u, static_cast<void*>(ext), static_cast<void*>(nullptr));
        }
        return ComCall<kSlotCreateTexture, int32>(D3DDevice, static_cast<uint32>(raster->width), static_cast<uint32>(raster->height), levels, usage, d3dFormat, 0u,
                                                  static_cast<void*>(ext), static_cast<void*>(nullptr));
    };
    const uint32 mipUsage = (ExtField<uint8>(ext, 0xA) & 0xFu) ? 0x400u : 0u;
    int32 hr = create(static_cast<uint32>(ExtField<int32>(ext, 0x18)), mipUsage | 1u);
    if (hr == kD3DERR_OUTOFVIDEOMEMORY) {
        return 0;
    }
    if (hr < 0) {
        hr = create(static_cast<uint32>(BackBufferFormat), mipUsage);
        if (hr < 0) {
            return 0;
        }
        raster->cFormat = static_cast<uint8>(PixelFormatInfo(BackBufferFormat).rwFormat >> 8);
        raster->depth = AdapterDisplayDepth;
    }
    return 1;
}

// 0x4CC5C0 (usercall: EAX = banderas, EDI = raster; `ret`, 907 bytes; sin hook): fija en el raster el tipo (banderas & 7), las banderas (& 0xF8), la profundidad y el formato de pixel (cFormat) segun el tipo y las consultas de formato al dispositivo. Devuelve 1 si se acepta
// y 0 si el dispositivo no admite el formato pedido. Tipo 0 y 4: sin formato, paleta de 8 bits (D3DFMT_P8 0x29; formato 0x500 con alfa de paleta, 0x600 sin), 32 bits (0x15, 0x500), 16 bits (0x1A, 0x300; si no, 0x100); con formato, el de la tabla
// (los 0x46..0x50 como textura de profundidad); con 0x9000 y sin paleta, si admite el uso 0x400 marca la generacion de mipmaps en la extension. Tipo 5: como los anteriores sin paleta y con destino de render. Tipo 1: profundidad 16 (0x700) o 32 (0x900) segun 0xC9BEFC.
// Tipo 2: formato del buffer trasero. Tipo 3 y mayores de 5: 0.
int32 RasterCheckFormat(RwRaster* raster, uint32 flags) {
    const uint32 type = flags & 7u;
    raster->cType = static_cast<uint8>(type);
    raster->cFlags = static_cast<uint8>(flags & 0xF8u);
    uint32 format = flags & 0xFF00u;
    uint8* const ext = RasterExt(raster, RasterExtOffset);
    const auto setFormat = [&] { raster->cFormat = static_cast<uint8>(format >> 8); };
    const auto markAutoMipmaps = [&] {
        uint8& mipFlags = ExtField<uint8>(ext, 0xA);
        mipFlags = static_cast<uint8>((mipFlags & 0xF1u) | 1u);
    };
    switch (type) {
    case 0:
    case 4: {
        const uint32 requested = format & 0xF00u;
        if (requested == 0) {
            if (format & 0x2000u) {
                if (!CheckTextureFormat(0x29)) {
                    return 0;
                }
                format |= (static_cast<int8>(TextureCapsLowByte) < 0) ? 0x500u : 0x600u;
                raster->depth = 8;
            } else if (AdapterDisplayDepth > 0x10 && CheckTextureFormat(0x15)) {
                format |= 0x500u;
                raster->depth = 0x20;
            } else if (CheckTextureFormat(0x1A)) {
                format |= 0x300u;
                raster->depth = 0x10;
            } else {
                format |= 0x100u;
                raster->depth = 0x10;
            }
        } else {
            if (format & 0x4000u) {
                return 0;
            }
            if (format & 0x2000u) {
                if (!CheckTextureFormat(0x29)) {
                    return 0;
                }
                if (requested != 0x600u && (requested != 0x500u || static_cast<int8>(TextureCapsLowByte) >= 0)) {
                    return 0;
                }
                raster->depth = 8;
            } else {
                const RwFormatEntry& entry = RwFormatInfo((format >> 8) & 0xFu);
                const bool supported = IsFormatInReleaseRange(static_cast<int32>(entry.d3dFormat)) ? CheckDepthStencilTextureFormat(entry.d3dFormat) : CheckTextureFormat(entry.d3dFormat);
                if (!supported) {
                    return 0;
                }
                raster->depth = entry.depth;
            }
        }
        if (!(format & 0x2000u) && (flags & 0x9000u) == 0x9000u && CheckTextureFormatUsage400(RwFormatInfo((format >> 8) & 0xFu).d3dFormat)) {
            markAutoMipmaps();
        }
        setFormat();
        return 1;
    }
    case 5: {
        if ((format & 0xF00u) == 0) {
            if (AdapterDisplayDepth == 0x10) {
                format |= 0x200u;
                raster->depth = 0x10;
            } else if (CheckRenderTargetTextureFormat(0x16)) {
                format |= 0x600u;
                raster->depth = 0x20;
            } else {
                format |= 0x200u;
                raster->depth = 0x10;
            }
        } else {
            const RwFormatEntry& entry = RwFormatInfo((format >> 8) & 0xFu);
            if (!CheckRenderTargetTextureFormat(entry.d3dFormat)) {
                return 0;
            }
            raster->depth = entry.depth;
        }
        if ((flags & 0x9000u) == 0x9000u && CheckTextureFormatUsage401(RwFormatInfo((format >> 8) & 0xFu).d3dFormat)) {
            markAutoMipmaps();
        }
        setFormat();
        return 1;
    }
    case 1: {
        const uint32 requested = format & 0xF00u;
        if (requested == 0) {
            if (ZBufferDepth == 0x10) {
                format = 0x700;
                raster->depth = 0x10;
            } else if (ZBufferDepth == 0x20) {
                format = 0x900;
                raster->depth = 0x20;
            }
        } else if (requested == 0x700u && ZBufferDepth == 0x10) {
            raster->depth = 0x10;
        } else if (requested == 0x900u && ZBufferDepth == 0x20) {
            raster->depth = 0x20;
        } else {
            return 0;
        }
        setFormat();
        return 1;
    }
    case 2: {
        const uint32 backBufferRwFormat = PixelFormatInfo(BackBufferFormat).rwFormat;
        if ((format & 0xF00u) == 0) {
            format = backBufferRwFormat;
        } else if (backBufferRwFormat != format) {
            return 0;
        }
        raster->depth = AdapterDisplayDepth;
        setFormat();
        return 1;
    }
    default:
        return 0;
    }
}

// 0x4CCD70 (usercall: ESI = extension, EDI = raster; `ret`, 236 bytes; sin hook): crea la superficie de profundidad de un raster de tipo 1. Formato segun 0xC9BEFC: 16 bits, 0x49 si CheckDepthStencilSurfaceFormat lo admite y si no 0x50; 32 bits, el primero admitido de
// 0x4B, 0x4F y, si ninguno, 0x47 o 0x4D; otra profundidad deja el formato a 0. Si la ventana (GetClientRect de 0xC97C1C) mide lo que el raster, usa la superficie de profundidad por defecto (0xC97C2C); si no, EvictManagedResources y
// CreateDepthStencilSurface con el muestreo de 0xC9C050/0xC9C054 (devuelve 1 si el resultado no es negativo).
int32 RasterCreateDepthSurface(RwRaster* raster, uint8* ext) {
    const auto pick = [](uint32 preferred, uint32 fallback) { return CheckDepthStencilSurfaceFormat(preferred) ? preferred : fallback; };
    if (ZBufferDepth == 0x10) {
        ExtField<int32>(ext, 0x18) = static_cast<int32>(pick(0x49, 0x50));
    } else if (ZBufferDepth == 0x20) {
        if (CheckDepthStencilSurfaceFormat(0x4B)) {
            ExtField<int32>(ext, 0x18) = 0x4B;
        } else if (CheckDepthStencilSurfaceFormat(0x4F)) {
            ExtField<int32>(ext, 0x18) = 0x4F;
        } else {
            ExtField<int32>(ext, 0x18) = static_cast<int32>(pick(0x47, 0x4D));
        }
    }
    ExtField<uint8>(ext, 8) = 0;
    RECT rect{};
    GetClientRect(DeviceWindow, &rect);
    if (rect.right == raster->width && rect.bottom == raster->height) {
        ExtField<void*>(ext, 0) = DefaultDepthSurface;
        return 1;
    }
    ComCall<kSlotEvictManagedResources, int32>(D3DDevice);
    const int32 hr = ComCall<kSlotCreateDepthStencilSurface, int32>(D3DDevice, static_cast<uint32>(raster->width), static_cast<uint32>(raster->height), static_cast<uint32>(ExtField<int32>(ext, 0x18)),
                                                                    MultiSampleType, MultiSampleQuality, 0u, static_cast<void*>(ext), static_cast<void*>(nullptr));
    return hr != kD3DERR_OUTOFVIDEOMEMORY && hr >= 0;
}

// 0x4CCE60 (cdecl, (dispositivo sin uso, raster, banderas), `ret`, 457 bytes; entrada `rasterCreate` de la tabla de dispositivo, indice 4): crea el recurso D3D de un raster. Limpia los campos del raster (+4, +8) y de la extension, fija tipo y formato (0x4CC5C0; 0 si falla) y,
// segun el tipo, crea la textura (tipo 0 y 4, 0x4CB7C0), el cubo de destino de render (5, 0x4CB9C0, con alta en la lista), la superficie de profundidad (1, 0x4CCD70, con alta en la lista) o enlaza el buffer trasero (2: sin la bandera 0x80 exige una ventana al menos tan grande,
// formato y profundidad de 0xC9C048, se marca 0x80, sin pixeles y alta en la lista); el tipo 3 y los mayores de 5 no hacen nada. Sin ancho o sin alto: bandera 0x80, profundidad 16 si faltaba y, en el tipo 2, alta en la lista. Con la bandera 0x80 ya puesta no crea nada. Devuelve 1 si va bien.
__declspec(noinline) int32 __cdecl RasterCreate(void* /*device*/, RwRaster* raster, uint32 flags) {
    uint8* const ext = RasterExt(raster, RasterExtOffset);
    raster->cpPixels = nullptr;
    raster->palette = nullptr;
    raster->cType = static_cast<uint8>(flags & 7u);
    raster->cFlags = static_cast<uint8>(flags & 0xF8u);
    ExtField<void*>(ext, 0) = nullptr;
    ExtField<void*>(ext, 4) = nullptr;
    ExtField<uint8>(ext, 8) = 0;
    ExtField<uint8>(ext, 0xA) = 0;
    ExtField<uint8>(ext, 9) = 0;
    ExtField<uint8>(ext, 0xB) = 0xFF;
    ExtField<void*>(ext, 0xC) = nullptr;
    ExtField<int32>(ext, 0x18) = 0;
    if (!RasterCheckFormat(raster, flags)) {
        return 0;
    }
    if (raster->width == 0 || raster->height == 0) {
        const bool noDepth = raster->depth == 0;
        raster->cFlags = 0x80;
        raster->stride = 0;
        if (noDepth) {
            raster->depth = 0x10;
        }
        if (raster->cType == 2) {
            RasterListPush(raster);
        }
        return 1;
    }
    switch (raster->cType) {
    case 0:
    case 4:
        if (static_cast<int8>(raster->cFlags) < 0) {
            return 1;
        }
        return RasterCreateTexture(raster, ext) != 0;
    case 5:
        if (static_cast<int8>(raster->cFlags) < 0) {
            return 1;
        }
        if (!RasterCreateRenderTargetTexture(raster, ext)) {
            return 0;
        }
        RasterListPush(raster);
        return 1;
    case 1:
        if (static_cast<int8>(raster->cFlags) < 0) {
            return 1;
        }
        if (!RasterCreateDepthSurface(raster, ext)) {
            return 0;
        }
        RasterListPush(raster);
        return 1;
    case 2: {
        RECT rect{};
        GetClientRect(DeviceWindow, &rect);
        if (!(raster->cFlags & 0x80u)) {
            if (rect.right < raster->width || rect.bottom < raster->height) {
                return 0;
            }
        }
        switch (BackBufferFormat) {
        case 0x15:
        case 0x16:
        case 0x23:
            raster->depth = 0x20;
            break;
        case 0x17:
        case 0x18:
        case 0x19:
            raster->depth = 0x10;
            break;
        default:
            raster->depth = 0;
            break;
        }
        raster->cFlags = 0x80;
        raster->stride = 0;
        raster->cpPixels = nullptr;
        raster->originalWidth = raster->width;
        raster->originalHeight = raster->height;
        ExtField<int32>(ext, 0x18) = BackBufferFormat;
        RasterListPush(raster);
        return 1;
    }
    default:
        return 1;
    }
}
} // namespace RasterCreation
} // namespace

void  RwD3D9RasterDriver::RasterListPushCpp(RwRaster* raster) { ::RasterListPush(raster); }
int32 RwD3D9RasterDriver::RasterCreateCpp(void* device, RwRaster* raster, uint32 flags) { return RasterCreation::RasterCreate(device, raster, flags); }
int32 RwD3D9RasterDriver::RasterDestroyCpp(void* device, RwRaster* raster, int32 flags) { return ::RasterDestroy(device, raster, flags); }
int32 RwD3D9RasterDriver::CheckDepthStencilSurfaceFormatCpp(uint32 format) { return ::CheckDepthStencilSurfaceFormat(format); }
int32 RwD3D9RasterDriver::CheckTextureFormatCpp(uint32 format) { return ::CheckTextureFormat(format); }
int32 RwD3D9RasterDriver::CheckRenderTargetTextureFormatCpp(uint32 format) { return ::CheckRenderTargetTextureFormat(format); }
int32 RwD3D9RasterDriver::CheckDepthStencilTextureFormatCpp(uint32 format) { return ::CheckDepthStencilTextureFormat(format); }
int32 RwD3D9RasterDriver::CheckRenderTargetCubeFormatCpp(uint32 format) { return ::CheckRenderTargetCubeFormat(format); }
int32 RwD3D9RasterDriver::CheckTextureFormatUsage400Cpp(uint32 format) { return ::CheckTextureFormatUsage400(format); }

/**
 * Registra las rutinas del controlador de rasters D3D9 (lote 224A, sin validar).
 */
void RwD3D9RasterDriver::InjectHooks() {
    RH_ScopedClass(RwD3D9RasterDriver);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(RasterListPush, "RasterListPush", 0x4CB530);                                           // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(RasterListDestroy, "RasterListDestroy", 0x4CB5E0);                                     // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(RasterListReleaseResources, "RasterListReleaseResources", 0x4CB640);                   // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(RasterCreation::RasterCreate, "RasterCreate", 0x4CCE60);                                               // lote S2 (sin validar)
    RH_ScopedNamedGlobalInstall(RasterDestroy, "RasterDestroy", 0x4CBB00);                                             // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(CheckDepthStencilSurfaceFormat, "CheckDepthStencilSurfaceFormat", 0x4CBD80);           // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(CheckTextureFormat, "CheckTextureFormat", 0x4CBDE0);                                   // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(CheckRenderTargetTextureFormat, "CheckRenderTargetTextureFormat", 0x4CBE20);           // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(CheckDepthStencilTextureFormat, "CheckDepthStencilTextureFormat", 0x4CBE80);           // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(CheckRenderTargetCubeFormat, "CheckRenderTargetCubeFormat", 0x4CBF20);                 // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(CheckTextureFormatUsage400, "CheckTextureFormatUsage400", 0x4CBF80);                   // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(CheckTextureFormatUsage401, "CheckTextureFormatUsage401", 0x4CBFD0);                   // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(CheckCubeFormatUsage400, "CheckCubeFormatUsage400", 0x4CC020);                         // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(RasterListInit, "RasterListInit", 0x4CC150);                                           // lote 224A (sin validar)
    RH_ScopedNamedGlobalInstall(RasterListShutdown, "RasterListShutdown", 0x4CC170);                                   // lote 224A (sin validar)
}
