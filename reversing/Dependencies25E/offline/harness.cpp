// Arnes aislado (sin juego): ejecuta los bytes REALES de las 16 funciones del EXE, colocados en sus direcciones originales, frente a la reconstruccion C++ tal cual
// esta en los fuentes del parche (cpp_bodies.inc), con las mismas fixtures del banco (Fixtures.hpp). Las rutinas que las funciones enlazan (_onexit, _lock, _unlock,
// _getptd, _nh_malloc, EnterCriticalSection/LeaveCriticalSection por la tabla de importacion) son dobles trazados en sus direcciones del EXE.
// Salida: una linea JSON por (funcion, caso) con base, resultado del EXE y de C++; check_offline.py las compara entre si y con el oraculo de Python.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>
#include "../Fixtures.hpp"
#include "exebytes.inc"
using namespace Dependencies25EFix;
typedef std::uint8_t uint8; typedef std::uint32_t uint32; typedef std::int32_t int32; typedef std::int32_t RwInt32; typedef std::uint32_t RwUInt32; typedef int RwBool; typedef char RwChar; typedef std::uint8_t RwUInt8;

// ---- Sustitutos minimos de los tipos del juego (misma disposicion que rwplcore.h / rwcore.h)
static std::uintptr_t gDelta;   // bloque reubicado del EXE: direccion real = direccion del EXE + gDelta
template<class T> T& StaticRef(std::uintptr_t a) { return *reinterpret_cast<T*>(a); }   // `a` ya lleva gDelta (cpp_bodies.inc lo suma)
struct RxHeap; struct RwRaster; struct RwFrame;
struct rwFrameList { RwFrame** frames; RwInt32 numFrames; };
struct RwSList { RwUInt8* listElements; RwInt32 numElementsFilled; RwInt32 numElementsAlloced; RwInt32 entrySize; };
struct RwPluginRegEntry; struct RwPluginRegistry;
struct RwPluginRegistry { RwInt32 sizeOfStruct, origSizeOfStruct, maxSizeOfStruct, staticAlloc; RwPluginRegEntry* firstRegEntry; RwPluginRegEntry* lastRegEntry; };
struct RwPluginRegEntry { RwInt32 offset, size; RwUInt32 pluginID; void *readCB, *writeCB, *getSizeCB, *alwaysCB, *rightsCB, *constructCB, *destructCB, *copyCB, *errStrCB; RwPluginRegEntry *nextRegEntry, *prevRegEntry; RwPluginRegistry* parentRegistry; };
typedef size_t (*vecStrlenFunc)(const RwChar*);
struct RwStringFunctions { void *a0, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9, *a10, *a11; vecStrlenFunc vecStrlen; void *a13, *a14, *a15, *a16; };
struct RwMemoryFunctions { void* (*rwmalloc)(size_t, RwUInt32); void (*rwfree)(void*); void* (*rwrealloc)(void*, size_t, RwUInt32); void* (*rwcalloc)(size_t, size_t, RwUInt32); };
struct RwGlobals { uint8 pad[0xF0]; RwStringFunctions stringFuncs; RwMemoryFunctions memoryFuncs; };
static_assert(offsetof(RwGlobals, stringFuncs) + offsetof(RwStringFunctions, vecStrlen) == 0x120, "rwstrlen debe estar en engine+0x120");
static_assert(offsetof(RwGlobals, memoryFuncs) + offsetof(RwMemoryFunctions, rwfree) == 0x138, "rwfree debe estar en engine+0x138");
static_assert(offsetof(RwPluginRegistry, firstRegEntry) == 0x10 && offsetof(RwPluginRegEntry, pluginID) == 8 && offsetof(RwPluginRegEntry, nextRegEntry) == 0x30, "diseno RwPluginRegEntry");
static_assert(offsetof(RwSList, entrySize) == 0xC && offsetof(rwFrameList, numFrames) == 4, "diseno RwSList / rwFrameList");
#define RwEngineInstance (StaticRef<RwGlobals*>(0xC97B24 + gDelta))
#define RWSRCGLOBAL(variable) (((RwGlobals *)RwEngineInstance)->variable)
#define RwFree(_p) ((RWSRCGLOBAL(memoryFuncs).rwfree)((_p)))

// ---- Cuerpos C++ tal cual estan en el parche
#include "cpp_bodies.inc"

// ---- Dobles trazados de las rutinas que enlazan las funciones
static std::vector<uint32> T;
static uint32 gOnexit, gStrlen, gPtd, gNhMalloc;
static uint32 __cdecl HOnexit(void* f) { T.push_back(uint32(std::uintptr_t(f))); return gOnexit; }
static void __cdecl HLock(int n) { T.push_back(1); T.push_back(uint32(n)); }
static void __cdecl HUnlock(int n) { T.push_back(2); T.push_back(uint32(n)); }
static uint32 __cdecl HGetPtd() { return gPtd; }
static uint32 __cdecl HNhMalloc(uint32 size, int flag) { T.push_back(size); T.push_back(uint32(flag)); return gNhMalloc; }
static void __stdcall HEnter(void* cs) { T.push_back(3); T.push_back(uint32(std::uintptr_t(cs))); }
static void __stdcall HLeave(void* cs) { T.push_back(4); T.push_back(uint32(std::uintptr_t(cs))); }
static void __cdecl HFree(void* p) { T.push_back(uint32(std::uintptr_t(p))); }
static size_t __cdecl HStrlen(const char* s) { T.push_back(uint32(std::uintptr_t(s))); return gStrlen; }

static void Jump(std::uintptr_t at, const void* target) { uint8* p = reinterpret_cast<uint8*>(at + gDelta); p[0] = 0xE9; *reinterpret_cast<int32*>(p + 1) = int32(std::uintptr_t(target) - (at + gDelta + 5)); }
static void Setup() {
    // Bloque reubicado de 9 MB (0x400000..0xD00000 del EXE): codigo, importaciones y datos.
    uint8* block = static_cast<uint8*>(VirtualAlloc(nullptr, 0x900000, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
    if (!block) { std::fprintf(stderr, "no puedo reservar el bloque\n"); std::exit(2); }
    gDelta = std::uintptr_t(block) - 0x400000;
    for (const auto& f : kExeFunctions) {
        uint8* code = reinterpret_cast<uint8*>(f.addr + gDelta);
        std::memcpy(code, f.bytes, f.len);
        for (unsigned i = 0; i < f.nreloc; ++i) *reinterpret_cast<std::uintptr_t*>(code + f.reloc[i]) += gDelta;   // palabras absolutas del EXE
    }
    Jump(0x821CE6, &HOnexit); Jump(0x82ADBE, &HLock); Jump(0x82AD12, &HUnlock); Jump(0x827B3D, &HGetPtd); Jump(0x82422B, &HNhMalloc);
    StaticRef<void*>(0x8580EC + gDelta) = reinterpret_cast<void*>(&HEnter); StaticRef<void*>(0x8580E8 + gDelta) = reinterpret_cast<void*>(&HLeave);
    static RwGlobals engine{};
    engine.memoryFuncs.rwfree = &HFree; engine.stringFuncs.vecStrlen = &HStrlen;
    StaticRef<RwGlobals*>(0xC97B24 + gDelta) = &engine;
}

// ---- Arenas y salida
template<size_t Interior> struct Arena {
    alignas(16) uint8 mem[Interior + 2 * kGuard];
    uint8* In() { return mem + kGuard; }
    void Reset() { std::memset(mem, kGuardByte, sizeof(mem)); }
};
static std::string Hex(const uint8* p, size_t n) { static const char d[] = "0123456789abcdef"; std::string s; for (size_t i = 0; i < n; ++i) { s += d[p[i] >> 4]; s += d[p[i] & 15]; } return s; }
static std::string Trace() { std::string s = "["; for (size_t i = 0; i < T.size(); ++i) { if (i) s += ","; s += std::to_string(T[i]); } return s + "]"; }
struct Obs { uint32 eax; std::string arena; std::string trace; };
// run(side) -> eax. setup() reconstruye el estado antes de cada lado.
static void Emit(const char* fn, unsigned c, uint32 base, uint8* arenaMem, size_t arenaBytes, const std::function<void()>& setup, const std::function<uint32(bool)>& run) {
    Obs o[2];
    for (int side = 0; side < 2; ++side) { T.clear(); setup(); o[side].eax = run(side == 1); o[side].arena = arenaMem ? Hex(arenaMem, arenaBytes) : std::string(); o[side].trace = Trace(); }
    std::printf("{\"f\":\"%s\",\"c\":%u,\"delta\":%u,\"base\":%u,\"exe\":{\"eax\":%u,\"arena\":\"%s\",\"trace\":%s},\"cpp\":{\"eax\":%u,\"arena\":\"%s\",\"trace\":%s}}\n", fn, c, uint32(gDelta), base,
                o[0].eax, o[0].arena.c_str(), o[0].trace.c_str(), o[1].eax, o[1].arena.c_str(), o[1].trace.c_str());
}
#define EXE(type, addr) (reinterpret_cast<type>(std::uintptr_t(addr) + gDelta))
static uint32 U(const void* p) { return uint32(std::uintptr_t(p)); }

int main() {
    std::setvbuf(stdout, nullptr, _IOLBF, 65536);
    Setup();
    static Arena<kFrameListInterior> frameArena; static Arena<kPluginInterior> pluginArena; static Arena<kSListInterior> slistArena; static Arena<kStringCloseInterior> closeArena;
    static Arena<kStringInterior> stringArena; static Arena<kRasterInterior> rasterArena; static Arena<kAtexitInterior> atexitArena;
    for (unsigned c = 0; c < kCases; ++c) {
        // RxHeapGetGlobalHeap
        Emit("RwCore::RxHeapGetGlobalHeap", c, 0, nullptr, 0, [&] { StaticRef<uint32>(0xC9A6B0 + gDelta) = RxHeapValue(c); },
             [&](bool cpp) { return U(cpp ? RxHeapGetGlobalHeap() : EXE(RxHeap* (__cdecl*)(), 0x8052F0)()); });
        // _rwFrameListDeinitialize
        auto frameSetup = [&] { frameArena.Reset(); BuildFrameList(frameArena.In(), c); };
        Emit("RwCore::_rwFrameListDeinitialize", c, U(frameArena.In()), frameArena.mem, sizeof(frameArena.mem), frameSetup, [&](bool cpp) {
            auto* list = reinterpret_cast<rwFrameList*>(frameArena.In() + kFrameListObject);
            return U(cpp ? _rwFrameListDeinitialize(list) : EXE(rwFrameList* (__cdecl*)(rwFrameList*), 0x807720)(list)); });
        // RwD3D9RasterIsCompressed
        const auto rf = Raster(c);
        Emit("RwCore::RwD3D9RasterIsCompressed", c, U(rasterArena.In()), rasterArena.mem, sizeof(rasterArena.mem), [&] { rasterArena.Reset(); BuildRaster(rasterArena.In(), c); StaticRef<uint32>(0xB4E9E0 + gDelta) = rf.delta; },
             [&](bool cpp) { const RwRaster* raster = reinterpret_cast<const RwRaster*>(std::uintptr_t(U(rasterArena.In() + kRasterFlag) - 0xAu - rf.delta));
                             return uint32(cpp ? RwD3D9RasterIsCompressed(raster) : EXE(RwBool (__cdecl*)(const RwRaster*), 0x820C90)(raster)); });
        // _rwPluginRegistryGetPluginOffset
        const auto pf = Plugin(c);
        Emit("RwPlCore::_rwPluginRegistryGetPluginOffset", c, U(pluginArena.In()), pluginArena.mem, sizeof(pluginArena.mem), [&] { pluginArena.Reset(); BuildPlugin(pluginArena.In(), c); },
             [&](bool cpp) { auto* reg = reinterpret_cast<const RwPluginRegistry*>(pluginArena.In() + kPluginRegistry);
                             return uint32(cpp ? _rwPluginRegistryGetPluginOffset(reg, pf.query) : EXE(RwInt32 (__cdecl*)(const RwPluginRegistry*, RwUInt32), 0x808470)(reg, pf.query)); });
        // _rwSList*
        const auto sf = SList(c);
        auto slistSetup = [&] { slistArena.Reset(); BuildSList(slistArena.In(), c); };
        auto* list = reinterpret_cast<RwSList*>(slistArena.In() + kSListObject);
        Emit("RwPlCore::_rwSListDestroyEndEntries", c, U(slistArena.In()), slistArena.mem, sizeof(slistArena.mem), slistSetup, [&](bool cpp) {
            if (cpp) _rwSListDestroyEndEntries(list, RwInt32(sf.arg)); else EXE(void (__cdecl*)(RwSList*, RwInt32), 0x809400)(list, RwInt32(sf.arg)); return 0u; });
        Emit("RwPlCore::_rwSListGetNumEntries", c, U(slistArena.In()), slistArena.mem, sizeof(slistArena.mem), slistSetup, [&](bool cpp) {
            return uint32(cpp ? _rwSListGetNumEntries(list) : EXE(RwInt32 (__cdecl*)(const RwSList*), 0x8094B0)(list)); });
        Emit("RwPlCore::_rwSListGetEntry", c, U(slistArena.In()), slistArena.mem, sizeof(slistArena.mem), slistSetup, [&](bool cpp) {
            return U(cpp ? _rwSListGetEntry(list, RwInt32(sf.arg)) : EXE(void* (__cdecl*)(RwSList*, RwInt32), 0x809510)(list, RwInt32(sf.arg))); });
        Emit("RwPlCore::_rwSListGetBegin", c, U(slistArena.In()), slistArena.mem, sizeof(slistArena.mem), slistSetup, [&](bool cpp) {
            return U(cpp ? _rwSListGetBegin(list) : EXE(void* (__cdecl*)(RwSList*), 0x809530)(list)); });
        Emit("RwPlCore::_rwSListGetEnd", c, U(slistArena.In()), slistArena.mem, sizeof(slistArena.mem), slistSetup, [&](bool cpp) {
            return U(cpp ? _rwSListGetEnd(list) : EXE(void* (__cdecl*)(RwSList*), 0x809540)(list)); });
        // _rwStringClose
        Emit("RwPlCore::_rwStringClose", c, U(closeArena.In()), closeArena.mem, sizeof(closeArena.mem), [&] { closeArena.Reset(); BuildStringClose(closeArena.In(), c); },
             [&](bool cpp) { if (cpp) _rwStringClose(); else EXE(void (__cdecl*)(), 0x80A440)(); return 0u; });
        // _rwStringStreamGetSize
        const auto tf = String(c);
        Emit("RwPlCore::_rwStringStreamGetSize", c, U(stringArena.In()), stringArena.mem, sizeof(stringArena.mem), [&] { stringArena.Reset(); BuildString(stringArena.In(), c); gStrlen = tf.length; },
             [&](bool cpp) { const RwChar* s = tf.null ? nullptr : reinterpret_cast<const RwChar*>(stringArena.In() + kStringObject);
                             return uint32(cpp ? _rwStringStreamGetSize(s) : EXE(RwUInt32 (__cdecl*)(const RwChar*), 0x80A470)(s)); });
        // CrtAtexit
        const auto af = Atexit(c);
        Emit("CRT::CrtAtexit", c, U(atexitArena.In()), atexitArena.mem, sizeof(atexitArena.mem), [&] { atexitArena.Reset(); BuildAtexit(atexitArena.In(), c); gOnexit = af.result; },
             [&](bool cpp) { auto fn = reinterpret_cast<void(__cdecl*)()>(std::uintptr_t(af.func));
                             return uint32(cpp ? CrtAtexit(fn) : EXE(int (__cdecl*)(void(__cdecl*)()), 0x821D1E)(fn)); });
        // CrtLockFile2 / CrtUnlockFile2 (los dobles registran _lock/_unlock y Enter/Leave con la direccion de la seccion)
        const auto lf = Lock(c);
        void* stream = reinterpret_cast<void*>(std::uintptr_t(lf.real ? lf.stream : 0x500000u));
        Emit("CRT::CrtLockFile2", c, 0, nullptr, 0, [] {}, [&](bool cpp) { if (cpp) CrtLockFile2(int(lf.index), stream); else EXE(void (__cdecl*)(int, void*), 0x823FD1)(int(lf.index), stream); return 0u; });
        Emit("CRT::CrtUnlockFile2", c, 0, nullptr, 0, [] {}, [&](bool cpp) { if (cpp) CrtUnlockFile2(int(lf.index), stream); else EXE(void (__cdecl*)(int, void*), 0x824023)(int(lf.index), stream); return 0u; });
        // CrtMalloc: el doble de _nh_malloc devuelve un valor propio del caso y registra (size, _newmode)
        const auto mf = Malloc(c);
        Emit("CRT::CrtMalloc", c, 0, nullptr, 0, [&] { StaticRef<uint32>(0xC9ADCC + gDelta) = mf.newmode; gNhMalloc = Word(c, 77); },
             [&](bool cpp) { return U(cpp ? CrtMalloc(mf.size) : EXE(void* (__cdecl*)(uint32), 0x824257)(mf.size)); });
        // CrtErrno: el doble de _getptd devuelve un bloque propio del caso
        Emit("CRT::CrtErrno", c, 0, nullptr, 0, [&] { gPtd = Word(c, 78); }, [&](bool cpp) { return U(cpp ? CrtErrno() : EXE(int* (__cdecl*)(), 0x82B181)()); });
    }
    return 0;
}
