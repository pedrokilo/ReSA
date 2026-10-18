#pragma once
// Entorno minimo para compilar sin cambios el texto de las 20 funciones (cand.inc): tipos de RenderWare con la misma disposicion que rpworld.h (comprobada con static_assert),
// StaticRef y los envoltorios de colaboradores que la DLL llama (llaman a la direccion del EXE, igual que en rwplcore.cpp / rwcore.cpp / D3DResourceSystem.cpp).
#include <cstddef>
#include <cstdint>
#include <d3d9.h>

using uint32 = std::uint32_t;
using int32 = std::int32_t;
using uintptr = std::uintptr_t;
using RwInt8 = signed char;
using RwInt32 = int;
using RwUInt32 = unsigned int;
using RwUInt16 = unsigned short;
using RwBool = int;

template<typename T> T& StaticRef(uintptr addr) { return *reinterpret_cast<T*>(addr); }

struct RwLLLink { RwLLLink* next; RwLLLink* prev; };
struct RwLinkList { RwLLLink link; };
struct RpMaterial; struct RwTexture; struct RpWorld; struct RpWorldSector; struct RwStream;
struct RpClump { unsigned char object[8]; RwLinkList atomicList; };
struct RpAtomic { unsigned char pad[0x40]; RwLLLink inClumpLink; };
struct RpMaterialList { RpMaterial** materials; RwInt32 numMaterials; RwInt32 space; };
struct RpTriangle { RwUInt16 vertIndex[3]; RwUInt16 matIndex; };
struct RpGeometry { unsigned char pad[0x20]; RpMaterialList matList; };
struct RwPluginRegistry { unsigned char pad[0x30]; };
enum RpD3D9GeometryUsageFlag : int { rpD3D9GeometryUsageFlagDummy = 0 };
enum RpD3D9WorldSectorUsageFlag : int { rpD3D9WorldSectorUsageFlagDummy = 0 };
static_assert(offsetof(RpClump, atomicList) == 8 && offsetof(RpAtomic, inClumpLink) == 0x40 && offsetof(RpGeometry, matList) == 0x20 && offsetof(RpTriangle, matIndex) == 6);
static_assert(sizeof(RpMaterialList) == 12);

using RwPluginDataChunkAlwaysCallBack = int(*)(void*, int, int);
using RwPluginDataChunkRightsCallBack = int(*)(void*, int, int, unsigned);
using RwPluginDataChunkReadCallBack = RwStream*(*)(RwStream*, int, void*, int, int);
using RwPluginDataChunkWriteCallBack = RwStream*(*)(RwStream*, int, const void*, int, int);
using RwPluginDataChunkGetSizeCallBack = int(*)(const void*, int, int);
using RwPluginObjectConstructor = void*(*)(void*, int, int);
using RwPluginObjectDestructor = void*(*)(void*, int, int);
using RwPluginObjectCopy = void*(*)(void*, const void*, int, int);

// Envoltorios de la DLL (cada uno llama a la direccion del EXE; el banco los parchea a la vez que la direccion del EXE).
__declspec(noinline) inline RwInt32 _rwPluginRegistryAddPlgnStrmlwysCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkAlwaysCallBack cb) {
    return ((RwInt32(__cdecl*)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkAlwaysCallBack))0x808920)(reg, pluginID, cb);
}
__declspec(noinline) inline RwInt32 _rwPluginRegistryAddPlgnStrmRightsCB(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkRightsCallBack cb) {
    return ((RwInt32(__cdecl*)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkRightsCallBack))0x808950)(reg, pluginID, cb);
}
__declspec(noinline) inline RwInt32 _rwPluginRegistryGetPluginOffset(const RwPluginRegistry* reg, RwUInt32 pluginID) {
    return ((RwInt32(__cdecl*)(const RwPluginRegistry*, RwUInt32))0x808470)(reg, pluginID);
}
__declspec(noinline) inline RwInt32 _rwPluginRegistryAddPlugin(RwPluginRegistry* reg, RwInt32 size, RwUInt32 pluginID, RwPluginObjectConstructor c, RwPluginObjectDestructor d, RwPluginObjectCopy cp) {
    return ((RwInt32(__cdecl*)(RwPluginRegistry*, RwInt32, RwUInt32, RwPluginObjectConstructor, RwPluginObjectDestructor, RwPluginObjectCopy))0x8084A0)(reg, size, pluginID, c, d, cp);
}
__declspec(noinline) inline RwInt32 _rwPluginRegistryAddPluginStream(RwPluginRegistry* reg, RwUInt32 pluginID, RwPluginDataChunkReadCallBack r, RwPluginDataChunkWriteCallBack w, RwPluginDataChunkGetSizeCallBack g) {
    return ((RwInt32(__cdecl*)(RwPluginRegistry*, RwUInt32, RwPluginDataChunkReadCallBack, RwPluginDataChunkWriteCallBack, RwPluginDataChunkGetSizeCallBack))0x8088E0)(reg, pluginID, r, w, g);
}
__declspec(noinline) inline const void* RwD3D9GetCaps() { return ((const void*(__cdecl*)())0x7FAD20)(); }
struct D3DResourceSystem {
    __declspec(noinline) static int32 CreateIndexBuffer(uint32 numIndices, uint32 format, void** ppIndexBuffer) {
        return ((int32(__cdecl*)(uint32, uint32, void**))0x7306A0)(numIndices, format, ppIndexBuffer);
    }
};
