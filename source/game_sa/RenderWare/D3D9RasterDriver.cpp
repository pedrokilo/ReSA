/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RwD3D9RasterDriver
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
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
    for (uint32 stage = 0; stage < 8; ++stage) {
        if (_rwD3D9RWGetRasterStage(stage) == raster) {
            _rwD3D9RWSetRasterStage(nullptr, stage);
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
} // namespace

// Entradas publicas de la lista de rasters para rwcore.cpp (que las llamaba por la direccion del EXE).
void D3D9RasterListInitExe() { RasterListInit(); }                           // 0x4CC150
void D3D9RasterListShutdownExe() { RasterListShutdown(); }                   // 0x4CC170
void D3D9RasterListReleaseResourcesExe() { RasterListReleaseResources(); }   // 0x4CB640

void  RwD3D9RasterDriver::RasterListPushCpp(RwRaster* raster) { ::RasterListPush(raster); }
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
