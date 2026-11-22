// Parte 6 del codigo vivo (cierre de huecos, rama vivo-p6b): callbacks de los nodos AllInOne de D3D9 de RenderWare (atomos y sectores del mundo) que el EXE apuntaba desde sus definiciones estaticas
// de .data (0x8D6338 atomos, 0x8D63B8 sectores, 0x8DED08 skin) y que Ghidra no habia separado como funciones (huecos 0x756DE1, 0x7573EF, 0x75E3E6 y la pieza 0x758270 del hueco 0x75823E).
// Cada funcion esta enganchada con RH_ScopedGlobalInstall (sin validar) en RpWorldInjectHooksP6B, al final del archivo. Las definiciones de nodo (RxNodeDefinition) siguen en la .data del EXE (datos-exe)
// y sus punteros siguen apuntando a estas direcciones, que ahora saltan al C++.
#include "StdInc.h"

#include <cstring>
#include <d3d9.h>

#include "rpworld.h"
#include "rpskin.h"

// Funciones del EXE que aun no estan reconstruidas o no tienen cabecera propia; se llaman por direccion.
void _rwD3D9SetClippingForObject(void* object, RwInt32 type);   // 0x756D90 (rpworld.cpp)
RwBool _rwD3D9RenderStateVertexAlphaEnable(RwBool enable);      // 0x7FE0A0 (rwcore.cpp)
RwInt32 _rwD3D9RenderStateGetVertexAlphaEnable();               // 0x7FE190 (rwcore.cpp)
void RxD3D9ResEntryHeaderDestroyCpp(RwResEntry* entry);         // 0x4C9990 (rwcore.cpp)
void D3D9FindMinVertAndNumVerticesCpp(const RwUInt16* indices, RwUInt32 numIndices, RwUInt32* numVertices, RwUInt32* minVert); // 0x756710 (rpworld.cpp)
void D3D9SortTrianglesCpp(RwUInt16* indices, RwUInt32 numIndices);                                                                // 0x756770 (rpworld.cpp)
RwUInt32 D3D9TriStripToTriListCpp(RwUInt16* dst, const RwUInt16* src, RwUInt32 numIndices, RwUInt32 offset);                      // 0x756830 (rpworld.cpp)
RwResEntry* D3D9InstanceObject(void* object, void* owner, RwUInt8 type, RwResEntry** ownerRef, RpMeshHeader* meshHeader, RxD3D9AllInOneInstanceCallBack instanceCallback, RwUInt32 nativeInstance); // 0x756960

namespace {
// 0x756960 (instanciado generico de un objeto con callback de instanciado): ahora C++ (D3D9InstanceObject, mas abajo).
inline RwResEntry* InstanceObject(void* object, void* owner, RwUInt8 type, RwResEntry** ownerRef, RpMeshHeader* meshHeader, RxD3D9AllInOneInstanceCallBack cb, RwUInt32 nativeInstance) {
    return D3D9InstanceObject(object, owner, type, ownerRef, meshHeader, cb, nativeInstance);
}
using ExeTextureHasAlphaFn = RwInt32(__cdecl*)(RwTexture*);
inline RwInt32 TextureHasAlpha(RwTexture* texture) { return reinterpret_cast<ExeTextureHasAlphaFn>(0x4C9EA0)(texture); }

constexpr size_t kSlotDrawPrimitive        = 81;  // IDirect3DDevice9::DrawPrimitive (+0x144)
constexpr size_t kSlotDrawIndexedPrimitive = 82;  // IDirect3DDevice9::DrawIndexedPrimitive (+0x148)
constexpr size_t kSlotSetVertexDeclaration = 87;  // IDirect3DDevice9::SetVertexDeclaration (+0x15C)
constexpr size_t kSlotSetVertexShader      = 92;  // IDirect3DDevice9::SetVertexShader (+0x170)
constexpr size_t kSlotSetIndices           = 104; // IDirect3DDevice9::SetIndices (+0x1A0)
constexpr size_t kSlotSetPixelShader       = 107; // IDirect3DDevice9::SetPixelShader (+0x1AC)

template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}
inline void* Device() { return *reinterpret_cast<void**>(0xC97C28); } // _RwD3DDevice

auto& D3D9LastVertexShader      = StaticRef<void*>(0x8E2448);
auto& D3D9LastPixelShader       = StaticRef<void*>(0x8E244C);
auto& D3D9LastFVF               = StaticRef<RwUInt32>(0x8E2440);
auto& D3D9LastVertexDeclaration = StaticRef<uintptr_t>(0x8E2444);
auto& D3D9LastIndexBuffer       = StaticRef<void*>(0x8E2450);
auto& D3D9AmbientRed            = StaticRef<float>(0x8E2418);
auto& D3D9AmbientGreen          = StaticRef<float>(0x8E241C);
auto& D3D9AmbientBlue           = StaticRef<float>(0x8E2420);
auto& D3D9AmbientAlpha          = StaticRef<float>(0x8E2424);

constexpr uintptr_t kD3D9FlushRenderStates = 0x7FC200; // _rwD3D9RenderStateFlushCache (vacia los estados pendientes antes de dibujar)
inline void FlushRenderStates() { reinterpret_cast<void(__cdecl*)()>(kD3D9FlushRenderStates)(); }

inline RwUInt32 GetRs(RwUInt32 state) {
    RwUInt32 value = 0;
    RwD3D9GetRenderState(state, &value);
    return value;
}
inline void SetRs(RwUInt32 state, RwUInt32 value) { RwD3D9SetRenderState(state, value); }
inline void Tss(RwUInt32 stage, RwUInt32 type, RwUInt32 value) { RwD3D9SetTextureStageState(stage, type, value); }

// Datos privados del nodo AllInOne (RxPipelineNode::privateData, +0x14): callbacks de instanciado, reinstanciado, iluminacion y render. Ver D3D9AllInOnePrivateCallbacks (rpworld.cpp) para los dos primeros.
struct AllInOneNodeData {
    RxD3D9AllInOneInstanceCallBack   instance;
    RxD3D9AllInOneReinstanceCallBack reinstance;
    RxD3D9AllInOneLightingCallBack   lighting;
    RxD3D9AllInOneRenderCallBack     render;
};
static_assert(sizeof(AllInOneNodeData) == 0x10);

inline AllInOneNodeData* NodeData(RxPipelineNode* node) { return static_cast<AllInOneNodeData*>(node->privateData); }

// Mueve una entrada de recursos al principio de la lista de usadas (RwResourcesUseResEntry del SDK, expandido en el EXE): solo si ya esta enlazada (link.next != NULL).
inline void ResourcesUseResEntry(RwResEntry* entry) {
    if (entry->link.next == nullptr) {
        return;
    }
    rwLinkListRemoveLLLink(&entry->link);
    rwResources* const res = reinterpret_cast<rwResources*>(reinterpret_cast<RwUInt8*>(RwEngineInstance) + StaticRef<RwInt32>(0xC9BC58));
    rwLinkListAddLLLink(res->usedEntries, &entry->link);
}

inline RwUInt32 FloatBits(float value) {
    RwUInt32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

inline RpWorld* CurrentWorld() { return static_cast<RpWorld*>(RwEngineInstance->curWorld); }

// D3DVERTEXELEMENT9 (8 bytes).
struct VertexElement {
    RwUInt16 stream;
    RwUInt16 offset;
    RwUInt8  type;
    RwUInt8  method;
    RwUInt8  usage;
    RwUInt8  usageIndex;
};
static_assert(sizeof(VertexElement) == 8);

constexpr uintptr_t kD3D9DeclTypesAddress = 0xC9BFEC; // D3DCAPS9::DeclTypes (bit 0x80 = DEC3N)
inline bool SupportsDec3N() { return (*reinterpret_cast<const volatile RwUInt8*>(kD3D9DeclTypesAddress) & 0x80) != 0; }
} // namespace

static_assert(offsetof(RpAtomic, repEntry) == 0x14 && offsetof(RpAtomic, geometry) == 0x18 && offsetof(RpAtomic, interpolator) == 0x4C && offsetof(RpAtomic, llWorldSectorsInAtomic) == 0x64);
static_assert(offsetof(RpGeometry, flags) == 0x08 && offsetof(RpGeometry, numVertices) == 0x14 && offsetof(RpGeometry, numMorphTargets) == 0x18 && offsetof(RpGeometry, mesh) == 0x54 && offsetof(RpGeometry, repEntry) == 0x58);
static_assert(offsetof(RpWorldSector, repEntry) == 0x34 && offsetof(RpWorldSector, lightsInWorldSector) == 0x40 && offsetof(RpWorldSector, mesh) == 0x78 && offsetof(RpWorldSector, numVertices) == 0x82);
static_assert(offsetof(RpWorld, flags) == 0x08 && offsetof(RpWorld, directionalLightList) == 0x3C);
static_assert(offsetof(RpLight, radius) == 0x14 && offsetof(RpLight, color) == 0x18 && offsetof(RpLight, inWorld) == 0x34 && offsetof(RpLight, lightFrame) == 0x3C);
static_assert(offsetof(RpTie, worldSector) == 0x14 && offsetof(RpTie, lWorldSectorInAtomic) == 0x0C && offsetof(RpLightTie, light) == 0x08);
static_assert(offsetof(RwResEntry, link) == 0 && sizeof(RwResEntry) == 0x18);
static_assert(offsetof(RxD3D9ResEntryHeader, indexBuffer) == 0x08 && offsetof(RxD3D9ResEntryHeader, primType) == 0x0C && offsetof(RxD3D9ResEntryHeader, vertexStream) == 0x10 && offsetof(RxD3D9ResEntryHeader, useOffsets) == 0x30);
static_assert(sizeof(RxD3D9InstanceData) == 0x24 && offsetof(RxD3D9InstanceData, vertexShader) == 0x10 && offsetof(RxD3D9InstanceData, baseIndex) == 0x14 && offsetof(RxD3D9InstanceData, numPrimitives) == 0x20);

// 0x756960 (codigo vivo D3DSTD, sin validar; hueco 0x756958; 1.080 bytes; cdecl, 7 argumentos (objeto, propietario, tipo, &entrada, cabecera de mallas, callback de instanciado, instancia nativa); nombre inferido: el instanciado
// generico de los nodos AllInOne de D3D9). Crea la entrada de recursos (RwResEntry + RxD3D9ResEntryHeader de 0x40 bytes + un RxD3D9InstanceData de 0x24 por malla): con `instancia nativa` por RwMalloc (pista 0x3050D; *entrada
// = ella; notificacion de destruccion 0x4C9990), si no por RwResourcesAllocateResEntry. La pone a cero, copia serie y numero de mallas y, si la cabecera es indexada (flags & 0x100 = 0), suma los indices de todas las mallas:
// en tira de triangulos (byte de la tabla del plugin 0xC9B8C0 + 8 + tipo = 4) con mas indices que 3 veces los triangulos del objeto (sector 0xFF: numTriangles de +0x84; atomo 1: los de su geometria) los recorta a ese
// maximo y pasa a lista de triangulos (primType = 4 y las tiras se convierten con D3D9TriStripToTriListCpp); crea el bufer de indices (RwD3D9IndexBufferCreate) y lo bloquea (Lock(0, 0, &puntero, 0)); sin recorte
// el tipo primitivo sale de la tabla 0x874FEC (1 -> LINELIST ... 6 -> POINTLIST). Por malla: minimo de vertice y numero de vertices (rango de indices; sin indices, un bloque correlativo), material, indices copiados
// (restando el minimo de 16 bits, o tal cual con minimo 0; ordenados por triangulos si el tipo es lista de triangulos), startIndex/numIndex y numPrimitives segun el tipo (/2, -1, /3, -2). Desbloquea, llama al callback
// (objeto, cabecera, FALSE) y si falla libera la entrada (RwFree o RwResourcesFreeResEntry) y devuelve NULL; si no, la entrada.
RwResEntry* D3D9InstanceObject(void* object, void* owner, RwUInt8 type, RwResEntry** ownerRef, RpMeshHeader* meshHeader, RxD3D9AllInOneInstanceCallBack instanceCallback, RwUInt32 nativeInstance) {
    constexpr RwUInt32 kResEntryHint = 0x3050D;
    constexpr size_t kSlotIndexBufferLock = 11;   // IDirect3DIndexBuffer9::Lock (+0x2C)
    constexpr size_t kSlotIndexBufferUnlock = 12; // IDirect3DIndexBuffer9::Unlock (+0x30)
    // Tabla 0x874FEC: tipo de primitiva de RW (3 = lista de triangulos...) a D3DPRIMITIVETYPE.
    static constexpr RwUInt32 kD3DPrimitiveTypes[8] = { 0, 2, 3, 4, 5, 6, 1, 0x40404040 };

    const RwUInt32 numMeshes = meshHeader->numMeshes;
    const RwUInt32 entrySize = numMeshes * sizeof(RxD3D9InstanceData) + sizeof(RxD3D9ResEntryHeader);
    bool useTriangleList = false;
    RwResEntry* resEntry;
    if (nativeInstance != 0) {
        resEntry = static_cast<RwResEntry*>(RwMalloc(entrySize + sizeof(RwResEntry), kResEntryHint));
        *ownerRef = resEntry;
        resEntry->link.next = nullptr;
        resEntry->link.prev = nullptr;
        resEntry->size = static_cast<RwInt32>(entrySize);
        resEntry->owner = nullptr;
        resEntry->ownerRef = nullptr;
        resEntry->destroyNotify = RxD3D9ResEntryHeaderDestroyCpp;
    } else {
        resEntry = RwResourcesAllocateResEntry(owner, ownerRef, static_cast<RwInt32>(entrySize), RxD3D9ResEntryHeaderDestroyCpp);
    }
    RxD3D9ResEntryHeader* const header = reinterpret_cast<RxD3D9ResEntryHeader*>(resEntry + 1);
    std::memset(header, 0, entrySize);
    header->serialNumber = meshHeader->serialNum;
    header->numMeshes = numMeshes;
    RwUInt16* indexLock = nullptr;
    const RpMesh* const meshes = reinterpret_cast<const RpMesh*>(meshHeader + 1);
    const RwUInt8 primitiveKind = *(reinterpret_cast<const RwUInt8*>(RwEngineInstance) + StaticRef<RwUInt32>(0xC9B8C0) + 8 + (meshHeader->flags & 0xFFu));
    bool keepPrimType = false;
    if (!(meshHeader->flags & 0x100)) {
        for (RwUInt32 i = 0; i < numMeshes; ++i) {
            header->totalNumIndex += meshes[i].numIndices;
        }
        if (header->totalNumIndex != 0) {
            if (primitiveKind == 4) {
                RwUInt32 maxTriangles = 0x55555555;
                if (type == 0xFF) {
                    maxTriangles = static_cast<RpWorldSector*>(object)->numTriangles;
                } else if (type == 1) {
                    maxTriangles = static_cast<RwUInt32>(static_cast<RpAtomic*>(object)->geometry->numTriangles);
                }
                maxTriangles *= 3;
                if (header->totalNumIndex > maxTriangles) {
                    useTriangleList = true;
                    header->totalNumIndex = maxTriangles;
                }
            }
            if (RwD3D9IndexBufferCreate(header->totalNumIndex, reinterpret_cast<void**>(&header->indexBuffer))) {
                ComCall<kSlotIndexBufferLock, int32>(header->indexBuffer, 0u, 0u, reinterpret_cast<void**>(&indexLock), 0u);
            }
            if (useTriangleList) {
                header->primType = D3DPT_TRIANGLELIST;
                keepPrimType = true;
            }
        }
    }
    if (!keepPrimType) {
        header->primType = static_cast<D3DPRIMITIVETYPE>(primitiveKind < 8 ? kD3DPrimitiveTypes[primitiveKind] : 0);
    }

    RxD3D9InstanceData* instance = reinterpret_cast<RxD3D9InstanceData*>(header + 1);
    const RpMesh* mesh = meshes;
    RwUInt32 nextVertex = 0;
    RwUInt32 nextIndex = 0;
    for (RwUInt32 remaining = numMeshes; remaining != 0; --remaining, ++instance, ++mesh) {
        RwUInt32 count = mesh->numIndices;
        if (meshHeader->flags & 0x100) {
            instance->numVertices = count;
            instance->minVert = nextVertex;
            nextVertex += count;
        } else {
            D3D9FindMinVertAndNumVerticesCpp(mesh->indices, count, &instance->numVertices, &instance->minVert);
        }
        instance->material = mesh->material;
        instance->vertexShader = nullptr;
        instance->vertexAlpha = 0;
        if (indexLock == nullptr) {
            instance->startIndex = 0;
            instance->numIndex = 0;
        } else {
            instance->startIndex = nextIndex;
            if (useTriangleList) {
                count = D3D9TriStripToTriListCpp(indexLock, mesh->indices, count, instance->minVert);
            } else if (instance->minVert != 0) {
                for (RwUInt32 i = 0; i < count; ++i) {
                    indexLock[i] = static_cast<RwUInt16>(mesh->indices[i] - static_cast<RwUInt16>(instance->minVert));
                }
            } else {
                std::memcpy(indexLock, mesh->indices, count * sizeof(RwUInt16));
            }
            instance->numIndex = count;
            if (header->primType == D3DPT_TRIANGLELIST) {
                D3D9SortTrianglesCpp(indexLock, count);
            }
            indexLock += count;
            nextIndex += count;
        }
        switch (header->primType) {
        case D3DPT_LINELIST:
            instance->numPrimitives = count >> 1;
            break;
        case D3DPT_LINESTRIP:
            instance->numPrimitives = count - 1;
            break;
        case D3DPT_TRIANGLELIST:
            instance->numPrimitives = count / 3;
            break;
        case D3DPT_TRIANGLESTRIP:
        case D3DPT_TRIANGLEFAN:
            instance->numPrimitives = count - 2;
            break;
        default:
            instance->numPrimitives = 0;
            break;
        }
        instance->baseIndex = 0;
    }
    if (indexLock != nullptr) {
        ComCall<kSlotIndexBufferUnlock, int32>(header->indexBuffer);
    }
    if (instanceCallback != nullptr && !instanceCallback(object, header, FALSE)) {
        if (nativeInstance != 0) {
            RwFree(resEntry);
        } else {
            RwResourcesFreeResEntry(resEntry);
        }
        return nullptr;
    }
    return resEntry;
}

// 0x756DF0 (lote P6B, sin validar; nombre inferido: _rxD3D9DefaultRenderCallBack del SDK; cdecl, 4 argumentos (entrada de recursos, objeto, tipo, banderas), 1414 bytes; apuntada desde los datos del nodo de skin
// por 0x7CB249): callback de render por defecto de los pipelines AllInOne de D3D9. Cabecera = entrada + 0x18, mallas = cabecera + 0x40.
// Quita el pixel shader activo (0x8E244C -> NULL, SetPixelShader(NULL)) y aplica el recorte del objeto (_rwD3D9SetClippingForObject: el EXE lo lleva en linea). Bufer de indices distinto del ultimo (0x8E2450):
// SetIndices. _rwD3D9SetStreams y, si la declaracion de vertices difiere de la ultima (0x8E2444): la guarda, FVF 0x8E2440 = 0xFFFFFFFF y SetVertexDeclaration.
// Con la iluminacion (estado 0x89) apagada y sin la bandera 8: "modo plano". Guarda los estados 0x1a y 9, alfa por vertice 0, TEXTUREFACTOR (0x3c) 0xFF000000, estado 0x1a a 0 y 9 a 1 y la etapa 0 como
// (1,3) (2,2) (3,3) (4,3) (5,2) (6,3), sin textura; por malla (do-while sobre las mallas de la cabecera): con banderas & 0x84 y una textura con alfa (_rwD3D9TextureHasAlpha) pone la textura y, si no estaba, ALPHAOP
// (etapa 0, tipo 4) = 4; si no, quita lo anterior (ALPHAOP = 3 y textura NULL); vertex shader de la malla si difiere del ultimo (0x8E2448); dibuja con DrawIndexedPrimitive (si hay indices) o
// DrawPrimitive tras vaciar los estados (0x7FC200). Al final restaura los estados 0x1a y 9.
// Con la iluminacion encendida (o bandera 8): por malla alfa por vertice segun el alfa del material y el alfa de la malla (solo cambia el estado si hace falta), con iluminacion RwD3D9SetSurfaceProperties
// y, si no, con la bandera 0x40 y un color distinto de 0xFFFFFFFF el TEXTUREFACTOR en ARGB; textura (solo con banderas & 0x84); mascara de etapas (1 textura, 2 factor de color) comparada con la anterior
// (inicial 0x80000000) para reconfigurar las etapas 0 y 1; vertex shader si difiere; dibuja. Al final, si la mascara quedo en 3, desactiva la etapa 1 (COLOROP y ALPHAOP = 1).
void D3D9DefaultRenderCallBack(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags) {
    if (D3D9LastPixelShader != nullptr) {
        D3D9LastPixelShader = nullptr;
        ComCall<kSlotSetPixelShader, long>(Device(), static_cast<void*>(nullptr));
    }
    _rwD3D9SetClippingForObject(object, type);

    RxD3D9ResEntryHeader* const header = reinterpret_cast<RxD3D9ResEntryHeader*>(reinterpret_cast<RwUInt8*>(repEntry) + 0x18);
    RxD3D9InstanceData* const instances = reinterpret_cast<RxD3D9InstanceData*>(reinterpret_cast<RwUInt8*>(header) + 0x40);

    if (header->indexBuffer != nullptr && D3D9LastIndexBuffer != header->indexBuffer) {
        D3D9LastIndexBuffer = header->indexBuffer;
        ComCall<kSlotSetIndices, long>(Device(), static_cast<void*>(header->indexBuffer));
    }
    _rwD3D9SetStreams(header->vertexStream, header->useOffsets);
    if (D3D9LastVertexDeclaration != reinterpret_cast<uintptr_t>(header->vertexDeclaration)) {
        D3D9LastVertexDeclaration = reinterpret_cast<uintptr_t>(header->vertexDeclaration);
        D3D9LastFVF = 0xFFFFFFFFu;
        ComCall<kSlotSetVertexDeclaration, long>(Device(), static_cast<void*>(header->vertexDeclaration));
    }

    const RwUInt32 lighting = GetRs(0x89);
    const RwUInt32 numMeshes = header->numMeshes;

    const auto drawMesh = [&](RxD3D9InstanceData* instance) {
        if (D3D9LastVertexShader != static_cast<void*>(instance->vertexShader)) {
            D3D9LastVertexShader = instance->vertexShader;
            ComCall<kSlotSetVertexShader, long>(Device(), static_cast<void*>(instance->vertexShader));
        }
        FlushRenderStates();
        if (header->indexBuffer != nullptr) {
            ComCall<kSlotDrawIndexedPrimitive, long>(Device(), static_cast<RwUInt32>(header->primType), static_cast<RwInt32>(instance->baseIndex), 0u, instance->numVertices, instance->startIndex, instance->numPrimitives);
        } else {
            ComCall<kSlotDrawPrimitive, long>(Device(), static_cast<RwUInt32>(header->primType), instance->baseIndex, instance->numPrimitives);
        }
    };

    if (lighting == 0 && (flags & 8) == 0) {
        // Modo plano (sin iluminacion).
        const RwUInt32 savedRs1A = GetRs(0x1A);
        const RwUInt32 savedRs09 = GetRs(9);
        _rwD3D9RenderStateVertexAlphaEnable(FALSE);
        SetRs(0x3C, 0xFF000000u);
        SetRs(0x1A, 0);
        SetRs(9, 1);
        Tss(0, 1, 3);
        Tss(0, 2, 2);
        Tss(0, 3, 3);
        Tss(0, 4, 3);
        Tss(0, 5, 2);
        Tss(0, 6, 3);
        RwD3D9SetTexture(nullptr, 0);
        if (numMeshes != 0) {
            const RwUInt32 textured = flags & 0x84;
            bool alphaModulate = false;
            RxD3D9InstanceData* instance = instances;
            RwUInt32 remaining = numMeshes;
            do {
                RpMaterial* const material = instance->material;
                if (textured != 0 && material->texture != nullptr && TextureHasAlpha(material->texture) != 0) {
                    RwD3D9SetTexture(material->texture, 0);
                    if (!alphaModulate) {
                        Tss(0, 4, 4);
                        alphaModulate = true;
                    }
                } else if (alphaModulate) {
                    alphaModulate = false;
                    Tss(0, 4, 3);
                    RwD3D9SetTexture(nullptr, 0);
                }
                drawMesh(instance);
                ++instance;
            } while (--remaining != 0);
        }
        SetRs(0x1A, savedRs1A);
        SetRs(9, savedRs09);
        return;
    }

    // Modo con iluminacion.
    RwUInt32 vertexAlphaState = static_cast<RwUInt32>(_rwD3D9RenderStateGetVertexAlphaEnable());
    RwUInt32 previousMask = 0x80000000u;
    if (numMeshes == 0) {
        return;
    }
    RxD3D9InstanceData* instance = instances;
    RwUInt32 remaining = numMeshes;
    do {
        RpMaterial* const material = instance->material;
        RwUInt32 mask = 0;
        if (material->color.alpha == 0xFF && instance->vertexAlpha == 0) {
            if (vertexAlphaState != 0) {
                vertexAlphaState = 0;
                _rwD3D9RenderStateVertexAlphaEnable(FALSE);
            }
        } else if (vertexAlphaState == 0) {
            vertexAlphaState = 1;
            _rwD3D9RenderStateVertexAlphaEnable(TRUE);
        }
        if (lighting != 0) {
            RwD3D9SetSurfaceProperties(&material->surfaceProps, &material->color, flags);
        } else if ((flags & 0x40) != 0 && *reinterpret_cast<const RwUInt32*>(&material->color) != 0xFFFFFFFFu) {
            const RwUInt32 argb = (static_cast<RwUInt32>(material->color.alpha) << 24) | (static_cast<RwUInt32>(material->color.red) << 16) |
                                  (static_cast<RwUInt32>(material->color.green) << 8) | static_cast<RwUInt32>(material->color.blue);
            SetRs(0x3C, argb);
            mask = 2;
        }
        if (material->texture != nullptr && (flags & 0x84) != 0) {
            RwD3D9SetTexture(material->texture, 0);
            mask |= 1;
        } else {
            RwD3D9SetTexture(nullptr, 0);
        }
        if (mask != previousMask) {
            if ((mask & 1) != 0) {
                if ((previousMask & 1) == 0) {
                    Tss(0, 1, 4);
                    Tss(0, 2, 2);
                    Tss(0, 3, 0);
                    Tss(0, 4, 4);
                    Tss(0, 5, 2);
                    Tss(0, 6, 0);
                }
                if ((mask & 2) != 0) {
                    Tss(1, 1, 4);
                    Tss(1, 2, 1);
                    Tss(1, 3, 3);
                    Tss(1, 4, 4);
                    Tss(1, 5, 1);
                    Tss(1, 6, 3);
                    previousMask = mask;
                    goto maskDone;
                }
            } else if ((mask & 2) != 0) {
                Tss(0, 1, 4);
                Tss(0, 2, 0);
                Tss(0, 3, 3);
                Tss(0, 4, 4);
                Tss(0, 5, 0);
                Tss(0, 6, 3);
            } else {
                Tss(0, 1, 3);
                Tss(0, 3, 0);
                Tss(0, 4, 3);
                Tss(0, 6, 0);
            }
            if (previousMask == 3) {
                Tss(1, 1, 1);
                Tss(1, 4, 1);
            }
            previousMask = mask;
        }
    maskDone:
        drawMesh(instance);
        ++instance;
    } while (--remaining != 0);
    if (previousMask == 3) {
        Tss(1, 1, 1);
        Tss(1, 4, 1);
    }
}

// 0x757400 (lote P6B, sin validar; nombre inferido: _rwD3D9AtomicDefaultLightingCallback del SDK; cdecl, (atomo), 491 bytes; la apuntan los datos de los nodos de atomo y de skin): configura las luces D3D9 de un atomo.
// Sin la bandera de geometria 0x20 (iluminada) o sin mundo actual: _rwD3D9LightsEnable(0, 1). Si no: ambiente acumulado 0x8E2418..0x8E2424 = (0, 0, 0, 1.0f) y recorre las luces direccionales del mundo
// (nodo - 0x34) con la bandera 1 (iluminan atomos): las direccionales (subtipo 1) solo con normales en la geometria (bandera 0x10) llaman a _rwD3D9LightDirectionalEnable; las demas (ambiente) suman su color (+0x18..+0x20)
// al ambiente acumulado; ambas marcan "usadas" (las direccionales sin normales no). Estado de render 0x8B = usadas ? -1 : 0. Con normales: lightFrame del motor ++ y por cada sector del atomo (lista +0x64) las
// luces locales de su lista (+0x40) aun no marcadas en este cuadro y con la bandera 1: se marcan, y si la distancia al cuadrado entre la esfera del mundo del atomo y la posicion de la luz (matriz LTM +0x30) es
// menor que (radio de la luz + radio de la esfera)^2 (comparacion ordenada: NaN no entra) se llama a _rwD3D9LightLocalEnable y su resultado se acumula con OR en "usadas". Termina con _rwD3D9LightsEnable(usadas, 1).
void D3D9AtomicDefaultLightingCallBack(void* object) {
    RpAtomic* const atomic = static_cast<RpAtomic*>(object);
    const RwUInt32 geometryFlags = atomic->geometry->flags;
    RpWorld* const world = CurrentWorld();
    if ((geometryFlags & 0x20) == 0 || world == nullptr) {
        _rwD3D9LightsEnable(0, 1);
        return;
    }
    D3D9AmbientRed = 0.0f;
    D3D9AmbientGreen = 0.0f;
    D3D9AmbientBlue = 0.0f;
    D3D9AmbientAlpha = 1.0f;
    RwUInt32 used = 0;
    RwLLLink* const directionalEnd = rwLinkListGetTerminator(&world->directionalLightList);
    for (RwLLLink* link = world->directionalLightList.link.next; link != directionalEnd; link = link->next) {
        RpLight* const light = reinterpret_cast<RpLight*>(reinterpret_cast<RwUInt8*>(link) - offsetof(RpLight, inWorld));
        if (light == nullptr || (light->object.object.flags & 1) == 0) {
            continue;
        }
        if (light->object.object.subType == 1) {
            if ((geometryFlags & 0x10) == 0) {
                continue;
            }
            _rwD3D9LightDirectionalEnable(light);
        } else {
            D3D9AmbientRed = D3D9AmbientRed + light->color.red;
            D3D9AmbientGreen = D3D9AmbientGreen + light->color.green;
            D3D9AmbientBlue = D3D9AmbientBlue + light->color.blue;
        }
        used = 1;
    }
    SetRs(0x8B, used != 0 ? 0xFFFFFFFFu : 0u);
    if ((geometryFlags & 0x10) != 0) {
        RwEngineInstance->lightFrame++;
        RwLLLink* const sectorsEnd = rwLinkListGetTerminator(&atomic->llWorldSectorsInAtomic);
        for (RwLLLink* sectorLink = atomic->llWorldSectorsInAtomic.link.next; sectorLink != sectorsEnd; sectorLink = sectorLink->next) {
            RpWorldSector* const sector = reinterpret_cast<RpTie*>(reinterpret_cast<RwUInt8*>(sectorLink) - offsetof(RpTie, lWorldSectorInAtomic))->worldSector;
            RwLLLink* const lightsEnd = rwLinkListGetTerminator(&sector->lightsInWorldSector);
            for (RwLLLink* lightLink = sector->lightsInWorldSector.link.next; lightLink != lightsEnd; lightLink = lightLink->next) {
                RpLight* const light = reinterpret_cast<RpLightTie*>(lightLink)->light;
                if (light == nullptr || light->lightFrame == RwEngineInstance->lightFrame || (light->object.object.flags & 1) == 0) {
                    continue;
                }
                light->lightFrame = RwEngineInstance->lightFrame;
                RwMatrix* const ltm = RwFrameGetLTM(static_cast<RwFrame*>(light->object.object.parent));
                const RwSphere* const sphere = RpAtomicGetWorldBoundingSphere(atomic);
                const float dx = sphere->center.x - ltm->pos.x;
                const float dy = sphere->center.y - ltm->pos.y;
                const float dz = sphere->center.z - ltm->pos.z;
                const float radius = light->radius + sphere->radius;
                const float distanceSq = dy * dy + dx * dx + dz * dz;
                if (radius * radius > distanceSq) {
                    used |= static_cast<RwUInt32>(_rwD3D9LightLocalEnable(light));
                }
            }
        }
    }
    _rwD3D9LightsEnable(used, 1);
}

// 0x7575F0 (lote P6B, sin validar; nombre inferido: nodeD3D9AtomicAllInOne body del SDK; cdecl, (nodo, parametros del nodo), 663 bytes; la apunta nodeBody de la definicion 0x8D6338): cuerpo del nodo AllInOne de
// D3D9 para atomos. Devuelve TRUE (1) salvo si falla el instanciado (FALSE). Atomo = primer dato de los parametros, datos privados = nodo + 0x14.
// Sin vertices en la geometria (<= 0) o sin mallas: termina. Si la geometria no es nativa (bandera 0x1000000): la entrada es la de la geometria (una sola posicion de morph) o la del atomo; si existe y su numero de
// serie no es el de la cabecera de mallas se libera; si no existe se instancia con 0x756960 (tipo 1; con la bandera 0x2000000 como "instancia nativa"), poniendo lockedSinceLastInst a 0 (fallo: devuelve FALSE);
// si existe y es valida y hay datos bloqueados (o mas de un morph) se reinstancia con el callback de reinstanciado (fallo: libera la entrada y devuelve FALSE) y se baja la bandera 1 de la interpolacion del atomo;
// despues se pasa la entrada al principio de la lista de usadas. Sin la bandera 0x2000000: callback de iluminacion (+8), matriz LTM del marco del atomo -> RwD3D9SetTransformWorld, y si la iluminacion (estado 0x89)
// esta encendida y la matriz no es identidad/ortonormal (bits 0x20001 del campo +0xC) se normalizan normales (estado 0x8F = 1) cuando alguna de las normas al cuadrado de las tres filas queda fuera de [0.9, 1.1]
// (comparadas como enteros con los bits de los floats, sin signo en el signo: jg/jl). Por ultimo el callback de render (+0xC) con (entrada, atomo, 1, banderas de la geometria).
RwBool D3D9AtomicAllInOneNode(RxPipelineNode* self, const RxPipelineNodeParam* params) {
    RpAtomic* const atomic = *reinterpret_cast<RpAtomic* const*>(params);
    AllInOneNodeData* const data = NodeData(self);
    RpGeometry* const geometry = atomic->geometry;
    if (geometry->numVertices <= 0) {
        return TRUE;
    }
    RpMeshHeader* const meshHeader = geometry->mesh;
    if (meshHeader->numMeshes == 0) {
        return TRUE;
    }
    const RwUInt32 geometryFlags = geometry->flags;
    RwResEntry* entry;
    if ((geometryFlags & 0x1000000) != 0) {
        entry = geometry->repEntry;
    } else {
        const bool singleMorph = geometry->numMorphTargets == 1;
        entry = singleMorph ? geometry->repEntry : atomic->repEntry;
        bool needCreate = true;
        if (entry != nullptr) {
            const RxD3D9ResEntryHeader* const entryHeader = reinterpret_cast<const RxD3D9ResEntryHeader*>(reinterpret_cast<const RwUInt8*>(entry) + 0x18);
            if (entryHeader->serialNumber == meshHeader->serialNum) {
                needCreate = false;
            } else {
                RwResourcesFreeResEntry(entry);
            }
        }
        if (needCreate) {
            void* const owner = singleMorph ? static_cast<void*>(geometry) : static_cast<void*>(atomic);
            RwResEntry** const ownerRef = singleMorph ? &geometry->repEntry : &atomic->repEntry;
            entry = InstanceObject(atomic, owner, 1, ownerRef, meshHeader, data->instance, (geometryFlags >> 25) & 1);
            if (entry == nullptr) {
                return FALSE;
            }
            geometry->lockedSinceLastInst = 0;
        } else {
            if (geometry->lockedSinceLastInst != 0 || geometry->numMorphTargets != 1) {
                if (data->reinstance != nullptr) {
                    if (data->reinstance(atomic, entry, data->instance) == 0) {
                        RwResourcesFreeResEntry(entry);
                        return FALSE;
                    }
                }
                *reinterpret_cast<RwUInt32*>(&atomic->interpolator) &= ~1u;
                geometry->lockedSinceLastInst = 0;
            }
            ResourcesUseResEntry(entry);
        }
    }
    if ((geometryFlags & 0x2000000) != 0) {
        return TRUE;
    }
    if (data->lighting != nullptr) {
        data->lighting(atomic);
    }
    RwMatrix* const ltm = RwFrameGetLTM(static_cast<RwFrame*>(atomic->object.object.parent));
    RwD3D9SetTransformWorld(ltm);
    const RwUInt32 lighting = GetRs(0x89);
    RwUInt32 normalize = 0;
    if (lighting != 0 && (ltm->flags & 0x20001) == 0) {
        const float* const m = reinterpret_cast<const float*>(ltm);
        const RwUInt32 upper = FloatBits(1.1f); // 0x3F8CCCCD
        const RwUInt32 lower = FloatBits(0.9f); // 0x3F666666
        for (int row = 0; row < 3; ++row) {
            const float x = m[row * 4 + 0];
            const float y = m[row * 4 + 1];
            const float z = m[row * 4 + 2];
            const float lengthSq = (x * x + y * y) + z * z;
            const RwInt32 bits = static_cast<RwInt32>(FloatBits(lengthSq));
            if (bits > static_cast<RwInt32>(upper) || bits < static_cast<RwInt32>(lower)) {
                normalize = 1;
                break;
            }
        }
    }
    SetRs(0x8F, normalize);
    if (data->render != nullptr) {
        data->render(entry, atomic, 1, geometryFlags);
    }
    return TRUE;
}

// 0x757890 (lote P6B; nombre inferido: pipelineNodeInit del nodo AllInOne de atomos; cdecl, (nodo), 40 bytes): rellena los datos privados: instanciado 0x7578C0 (D3D9AtomicDefaultInstanceCallback),
// reinstanciado 0x758270, iluminacion 0x757400 y render +0xC = 0x756DF0 (D3D9DefaultRenderCallBack); devuelve TRUE. (Antes omitia el render y no devolvia nada: la tuberia de atomos por defecto
// no dibujaba, p. ej. los marcadores 3D; arreglo del 22/11/2026 comparando los bytes de 0x757890: mov [eax],0x7578C0 / [eax+4],0x758270 / [eax+8],0x757400 / [eax+0xC],0x756DF0 / mov eax,1.)
RwBool D3D9AtomicAllInOneNodeInit(RxPipelineNode* node) {
    AllInOneNodeData* const data = NodeData(node);
    data->instance = D3D9AtomicDefaultInstanceCallback;
    data->reinstance = D3D9AtomicDefaultReinstanceCallback;
    data->lighting = D3D9AtomicDefaultLightingCallBack;
    data->render = D3D9DefaultRenderCallBack;
    return TRUE;
}

// 0x758270 (lote P6B, sin validar; nombre inferido: callback de reinstanciado por defecto del nodo AllInOne de atomos; cdecl, (atomo, entrada de recursos, callback de instanciado), 107 bytes; la apunta 0x757890; el
// listado de huecos la daba por citada en el C++ y no lo estaba): con mas de un morph target: si la interpolacion tiene la bandera 1 o hay datos bloqueados, llama al instanciado por defecto 0x7578C0 con
// la cabecera de la entrada y reinstanciar = 1 poniendo temporalmente lockedSinceLastInst |= 6, y devuelve siempre TRUE. Con un solo morph: sin callback devuelve TRUE; si no, devuelve su resultado (TRUE si distinto de 0, si no 0).
RwBool D3D9AtomicDefaultReinstanceCallback(void* object, RwResEntry* repEntry, RxD3D9AllInOneInstanceCallBack instanceCallback) {
    RpAtomic* const atomic = static_cast<RpAtomic*>(object);
    RpGeometry* const geometry = atomic->geometry;
    RxD3D9ResEntryHeader* const header = reinterpret_cast<RxD3D9ResEntryHeader*>(reinterpret_cast<RwUInt8*>(repEntry) + 0x18);
    if (geometry->numMorphTargets == 1) {
        if (instanceCallback == nullptr) {
            return TRUE;
        }
        return instanceCallback(atomic, header, TRUE) != 0 ? TRUE : FALSE;
    }
    if ((*reinterpret_cast<const RwUInt8*>(&atomic->interpolator) & 1) != 0 || geometry->lockedSinceLastInst != 0) {
        const RwUInt16 saved = geometry->lockedSinceLastInst;
        geometry->lockedSinceLastInst = static_cast<RwUInt16>(saved | 6);
        D3D9AtomicDefaultInstanceCallback(atomic, header, TRUE);
        geometry->lockedSinceLastInst = saved;
    }
    return TRUE;
}

// 0x75E3F0 (lote P6B, sin validar; nombre inferido: nodeD3D9WorldSectorAllInOne body del SDK; cdecl, (nodo, parametros), 332 bytes; la apunta nodeBody de la definicion 0x8D63B8): cuerpo del nodo AllInOne de
// D3D9 para sectores del mundo. Sector = primer dato de los parametros. Sin vertices (word +0x82 == 0) o sin mallas termina con TRUE. Si el mundo actual no es nativo (bandera 0x1000000): si la entrada del
// sector existe y su numero de serie no es el de la cabecera de mallas se libera; si no existe (o se libero) se instancia con 0x756960 (tipo 0xFF, la propia entrada del sector como referencia, con la bandera
// 0x2000000 del mundo como "instancia nativa"; fallo: devuelve FALSE); si es valida pasa al principio de la lista de usadas. Sin la bandera 0x2000000 del mundo: callback de iluminacion (+8), matriz mundo NULL
// (RwD3D9SetTransformWorld(NULL)), estado 0x8F = 0 y callback de render (+0xC) con (entrada, sector, 0xFF, banderas del mundo). Devuelve TRUE.
RwBool D3D9WorldSectorAllInOneNode(RxPipelineNode* self, const RxPipelineNodeParam* params) {
    RpWorldSector* const sector = *reinterpret_cast<RpWorldSector* const*>(params);
    AllInOneNodeData* const data = NodeData(self);
    if (sector->numVertices == 0) {
        return TRUE;
    }
    RpMeshHeader* const meshHeader = sector->mesh;
    if (meshHeader->numMeshes == 0) {
        return TRUE;
    }
    const RwUInt32 worldFlags = CurrentWorld()->flags;
    RwResEntry* entry = sector->repEntry;
    if ((worldFlags & 0x1000000) == 0) {
        bool needCreate = true;
        if (entry != nullptr) {
            const RxD3D9ResEntryHeader* const entryHeader = reinterpret_cast<const RxD3D9ResEntryHeader*>(reinterpret_cast<const RwUInt8*>(entry) + 0x18);
            if (entryHeader->serialNumber == meshHeader->serialNum) {
                needCreate = false;
            } else {
                RwResourcesFreeResEntry(entry);
            }
        }
        if (needCreate) {
            entry = InstanceObject(sector, sector, 0xFF, &sector->repEntry, meshHeader, data->instance, (worldFlags >> 25) & 1);
            if (entry == nullptr) {
                return FALSE;
            }
        } else {
            ResourcesUseResEntry(entry);
        }
    }
    if ((worldFlags & 0x2000000) != 0) {
        return TRUE;
    }
    if (data->lighting != nullptr) {
        data->lighting(sector);
    }
    RwD3D9SetTransformWorld(nullptr);
    SetRs(0x8F, 0);
    if (data->render != nullptr) {
        data->render(entry, sector, 0xFF, worldFlags);
    }
    return TRUE;
}

// 0x75E540 (lote P6B; nombre inferido: pipelineNodeInit del nodo AllInOne de sectores; cdecl, (nodo), 40 bytes): datos privados: instanciado 0x75E5E0, reinstanciado NULL, iluminacion 0x75E570,
// render +0xC = 0x756DF0 (bytes 0x75E540: mov [eax+0xC],0x756DF0) y devuelve TRUE (arreglo del 22/11/2026: antes omitia el render).
RwBool D3D9WorldSectorAllInOneNodeInit(RxPipelineNode* node) {
    AllInOneNodeData* const data = NodeData(node);
    data->instance = D3D9WorldSectorDefaultInstanceCallback;
    data->reinstance = nullptr;
    data->lighting = D3D9WorldSectorDefaultLightingCallBack;
    data->render = D3D9DefaultRenderCallBack;
    return TRUE;
}

// 0x75E570 (lote P6B, sin validar; nombre inferido: _rwD3D9WorldSectorDefaultLightingCallback del SDK; cdecl, (sector), 109 bytes): sin la bandera 0x20 del mundo actual (iluminado): _rwD3D9LightsEnable(0, -1). Si no:
// usadas = _rwD3D9LightsGlobalEnable(2), lightFrame del motor ++ y para cada luz de la lista del sector (+0x40) con la bandera 2 se acumula con OR _rwD3D9LightLocalEnable(luz); termina con
// _rwD3D9LightsEnable(usadas, -1).
void D3D9WorldSectorDefaultLightingCallBack(void* object) {
    RpWorldSector* const sector = static_cast<RpWorldSector*>(object);
    if ((CurrentWorld()->flags & 0x20) == 0) {
        _rwD3D9LightsEnable(0, static_cast<RwUInt32>(-1));
        return;
    }
    RwUInt32 used = static_cast<RwUInt32>(_rwD3D9LightsGlobalEnable(static_cast<RpLightFlag>(2)));
    RwEngineInstance->lightFrame++;
    RwLLLink* const end = rwLinkListGetTerminator(&sector->lightsInWorldSector);
    for (RwLLLink* link = sector->lightsInWorldSector.link.next; link != end; link = link->next) {
        RpLight* const light = reinterpret_cast<RpLightTie*>(link)->light;
        if (light != nullptr && (light->object.object.flags & 2) != 0) {
            used |= static_cast<RwUInt32>(_rwD3D9LightLocalEnable(light));
        }
    }
    _rwD3D9LightsEnable(used, static_cast<RwUInt32>(-1));
}

// 0x75E5E0 (lote P6B, sin validar; nombre inferido: _rxD3D9DefaultInstanceCallBack de sectores del SDK; cdecl, (sector, cabecera de la entrada, reinstanciar), 1031 bytes; la apunta 0x75E540): instancia los
// vertices de un sector en un unico flujo gestionado. Con las banderas del mundo actual: posicion FLOAT3; normales (0x10: FLOAT3 o DEC3N si DeclTypes & 0x80); color (8: D3DCOLOR); texcoords (0x84: FLOAT2, y una
// segunda con 0x80) y tangentes (uso del sector & 0x10000: FLOAT3 o DEC3N). totalNumVertex = numVertices; flujo 0 con el stride acumulado y los geometryFlags (2 posicion, 4 normales, 8 color, 0x10 y 0x20 texcoords).
// Crea la declaracion (0x7FAA30, resultado ignorado) y el bufer gestionado (RwD3D9CreateVertexBuffer: FALSE si falla); useOffsets = 0 y baseIndex de cada malla = offset / stride + minVert (do-while sobre las mallas).
// Bloquea el bufer (Lock vtable +0x2C, offset y tamano de la entrada, flags 0) y lo rellena con _rpD3D9VertexDeclarationInstV3d / V3dComp / InstColor (guarda vertexAlpha por malla; sin color los pone a 0) / V2d / Tangent;
// desbloquea (vtable +0x30) y devuelve TRUE.
RwBool D3D9WorldSectorDefaultInstanceCallback(void* object, RxD3D9ResEntryHeader* header, RwBool reinstance) {
    RpWorldSector* const sector = static_cast<RpWorldSector*>(object);
    const RwUInt32 worldFlags = CurrentWorld()->flags;
    const RwUInt32 usage = static_cast<RwUInt32>(RpD3D9WorldSectorGetUsageFlags(sector));
    header->totalNumVertex = sector->numVertices;
    RxD3D9VertexStream* const stream = &header->vertexStream[0];

    VertexElement elements[16];
    std::memset(elements, 0, sizeof(elements));
    elements[0] = VertexElement{ 0, 0, 2, 0, 0, 0 };
    RwUInt32 elementCount = 1;
    RwUInt32 offset = 0xC;
    stream->stride = 0xC;
    stream->geometryFlags = 2;

    const bool normals = (worldFlags & 0x10) != 0;
    const bool prelit = (worldFlags & 8) != 0;
    const bool textured = (worldFlags & 0x84) != 0;
    const bool secondTexCoords = (worldFlags & 0x80) != 0;
    const bool tangents = (usage & 0x10000) != 0;

    if (normals) {
        if (SupportsDec3N()) {
            elements[elementCount] = VertexElement{ 0, static_cast<RwUInt16>(offset), 0xE, 0, 3, 0 };
            offset = 0x10;
        } else {
            elements[elementCount] = VertexElement{ 0, static_cast<RwUInt16>(offset), 2, 0, 3, 0 };
            offset = 0x18;
        }
        stream->stride = offset;
        ++elementCount;
        stream->geometryFlags = 6;
    }
    if (prelit) {
        elements[elementCount] = VertexElement{ 0, static_cast<RwUInt16>(offset), 4, 0, 0xA, 0 };
        stream->stride += 4;
        ++elementCount;
        offset += 4;
        stream->geometryFlags |= 8;
    }
    if (textured) {
        stream->stride += 8;
        elements[elementCount] = VertexElement{ 0, static_cast<RwUInt16>(offset), 1, 0, 5, 0 };
        ++elementCount;
        offset += 8;
        stream->geometryFlags |= 0x10;
    }
    if (secondTexCoords) {
        elements[elementCount] = VertexElement{ 0, static_cast<RwUInt16>(offset), 1, 0, 5, 1 };
        stream->stride += 8;
        ++elementCount;
        offset += 8;
        stream->geometryFlags |= 0x20;
    }
    if (tangents) {
        elements[elementCount].stream = 0;
        elements[elementCount].offset = static_cast<RwUInt16>(offset);
        if (SupportsDec3N()) {
            elements[elementCount].type = 0xE;
            stream->stride += 4;
        } else {
            elements[elementCount].type = 2;
            stream->stride += 0xC;
        }
        elements[elementCount].method = 0;
        elements[elementCount].usage = 6;
        elements[elementCount].usageIndex = 0;
        ++elementCount;
    }
    elements[elementCount] = VertexElement{ 0xFF, 0, 0x11, 0, 0, 0 };
    RwD3D9CreateVertexDeclaration(elements, reinterpret_cast<void**>(&header->vertexDeclaration));

    const RwUInt32 stride = stream->stride;
    const RwUInt32 size = header->totalNumVertex * stride;
    stream->managed = 1;
    if (!RwD3D9CreateVertexBuffer(stride, size, &stream->vertexBuffer, &stream->offset)) {
        return FALSE;
    }
    const RwUInt32 numMeshes = header->numMeshes;
    header->useOffsets = 0;
    RxD3D9InstanceData* const instances = reinterpret_cast<RxD3D9InstanceData*>(header + 1);
    {
        RxD3D9InstanceData* instance = instances;
        RwUInt32 remaining = numMeshes;
        do {
            instance->baseIndex = stream->offset / stream->stride + instance->minVert;
            ++instance;
        } while (--remaining != 0);
    }
    RwUInt8* locked = nullptr;
    static_cast<IDirect3DVertexBuffer9*>(stream->vertexBuffer)->Lock(stream->offset, size, reinterpret_cast<void**>(&locked), 0);

    RwUInt32 elementIndex = 0;
    RwUInt32 written = _rpD3D9VertexDeclarationInstV3d(elements[0].type, locked, sector->vertices, static_cast<RwInt32>(header->totalNumVertex), stream->stride);
    elementIndex = 1;
    if (normals) {
        written += _rpD3D9VertexDeclarationInstV3dComp(elements[1].type, locked + written, sector->normals, static_cast<RwInt32>(header->totalNumVertex), stream->stride);
        elementIndex = 2;
    }
    if (prelit) {
        RxD3D9InstanceData* instance = instances;
        RwUInt32 remaining = numMeshes;
        const RwUInt32 meshStride = stream->stride;
        do {
            instance->vertexAlpha = _rpD3D9VertexDeclarationInstColor(locked + instance->minVert * meshStride + written, sector->preLitLum + instance->minVert, static_cast<RwInt32>(instance->numVertices), meshStride);
            ++instance;
        } while (--remaining != 0);
        ++elementIndex;
        written += 4;
    } else {
        RxD3D9InstanceData* instance = instances;
        RwUInt32 remaining = numMeshes;
        do {
            instance->vertexAlpha = 0;
            ++instance;
        } while (--remaining != 0);
    }
    if (textured) {
        written += _rpD3D9VertexDeclarationInstV2d(elements[elementIndex].type, locked + written, reinterpret_cast<const RwV2d*>(sector->texCoords[0]), static_cast<RwInt32>(header->totalNumVertex), stream->stride);
        ++elementIndex;
    }
    if (secondTexCoords) {
        written += _rpD3D9VertexDeclarationInstV2d(elements[elementIndex].type, locked + written, reinterpret_cast<const RwV2d*>(sector->texCoords[1]), static_cast<RwInt32>(header->totalNumVertex), stream->stride);
        ++elementIndex;
    }
    if (tangents) {
        const RwTexCoords* const texCoords = secondTexCoords ? sector->texCoords[1] : sector->texCoords[0];
        _rpD3D9VertexDeclarationInstTangent(elements[elementIndex].type, locked + written, sector->vertices, texCoords, header, stream->stride);
    }
    static_cast<IDirect3DVertexBuffer9*>(stream->vertexBuffer)->Unlock();
    return TRUE;
}

// Lote P6B (sin validar): ganchos de los callbacks de los nodos AllInOne de D3D9 reconstruidos en este archivo.
void RpWorldInjectHooksP6B() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpWorld");

    RH_ScopedGlobalInstall(D3D9InstanceObject, 0x756960);                         // codigo vivo D3DSTD (sin validar)
    RH_ScopedGlobalInstall(D3D9DefaultRenderCallBack, 0x756DF0);                  // lote P6B (sin validar)
    RH_ScopedGlobalInstall(D3D9AtomicDefaultLightingCallBack, 0x757400);          // lote P6B (sin validar)
    RH_ScopedGlobalInstall(D3D9AtomicAllInOneNode, 0x7575F0);                     // lote P6B (sin validar)
    RH_ScopedGlobalInstall(D3D9AtomicAllInOneNodeInit, 0x757890);                 // lote P6B (sin validar)
    RH_ScopedGlobalInstall(D3D9AtomicDefaultReinstanceCallback, 0x758270);        // lote P6B (sin validar)
    RH_ScopedGlobalInstall(D3D9WorldSectorAllInOneNode, 0x75E3F0);                // lote P6B (sin validar)
    RH_ScopedGlobalInstall(D3D9WorldSectorAllInOneNodeInit, 0x75E540);            // lote P6B (sin validar)
    RH_ScopedGlobalInstall(D3D9WorldSectorDefaultLightingCallBack, 0x75E570);     // lote P6B (sin validar)
    RH_ScopedGlobalInstall(D3D9WorldSectorDefaultInstanceCallback, 0x75E5E0);     // lote P6B (sin validar)
}
