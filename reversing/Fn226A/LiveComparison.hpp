#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <new>
#include <set>
#include <map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn226A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 17 funciones del EXE (ver Fixtures.hpp: nucleo de RpAtomic y RpClump de RenderWare, RpAnisotPluginAttach y CGamma::SetGammaNoFade), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedGlobalInstall). El banco
// invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40).
// Colaboradores: todos tienen hook RH en la DLL (apagado durante el banco) y llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (los que la DLL llama por la direccion del EXE solo usan la del
// EXE: el parche del cuerpo es inocuo). RpAtomicCreate (0x749C50) y _rpAtomicResyncInterpolatedSphere (0x7491F0), funciones probadas, son colaboradores falsos de otras funciones. Las funciones de RwEngineInstance
// (+0x144 RwFreeListAlloc y +0x148 RwFreeListFree) se llaman por puntero: [0xC97B24] apunta a una de dos tablas falsas (E1 o E2). Los objetos COM del dispositivo llevan vtables falsas en las arenas; los callbacks de
// RpClumpRender / RpClumpForAllCameras / RpClumpForAllLights son funciones falsas.
// Los punteros a funcion que guarda RpAtomicCreate (+0x10 sync, +0x48 dibujo) valen en el EXE la direccion de la funcion del EXE y en la DLL la de su cuerpo (que es lo que ejecuta el hook): el banco sustituye
// la direccion del cuerpo de la DLL por la del EXE antes de observar (fn == 12).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), las regiones globales vigiladas (gchanges y, la grande, su CRC), los
// eventos de los falsos (con sus argumentos), el valor devuelto (EAX completo) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).


// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn226a-crash.txt (junto al EXE).
namespace F226ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn226a-crash.txt";
    char text[2048]{};
    const auto* c = p->ContextRecord;
    int n = wsprintfA(text, "av eip=%08X addr=%08X rw=%u eax=%08X ecx=%08X edx=%08X esi=%08X edi=%08X ebp=%08X esp=%08X\r\nstack:", unsigned(c->Eip), unsigned(p->ExceptionRecord->ExceptionInformation[1]), unsigned(p->ExceptionRecord->ExceptionInformation[0]),
        unsigned(c->Eax), unsigned(c->Ecx), unsigned(c->Edx), unsigned(c->Esi), unsigned(c->Edi), unsigned(c->Ebp), unsigned(c->Esp));
    const unsigned* sp = reinterpret_cast<const unsigned*>(c->Esp);
    for (int i = 0; i < 48 && !IsBadReadPtr(sp + i, 4); ++i) n += wsprintfA(text + n, " %08X", sp[i]);
    HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(c->Eip), &m);
    n += wsprintfA(text + n, "\r\nmodule=%08X\r\n", unsigned(reinterpret_cast<uintptr_t>(m)));
    HANDLE f = CreateFileA(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f != INVALID_HANDLE_VALUE) { DWORD w{}; WriteFile(f, text, DWORD(n), &w, nullptr); CloseHandle(f); }
    return EXCEPTION_CONTINUE_SEARCH;
}
inline const bool installed = [] { AddVectoredExceptionHandler(1, Handler); return true; }();
}

namespace F226ALive {
using namespace F226AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F226AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>& ArenaAt(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }

// ================================================================================================ falsos de los colaboradores
alignas(16) inline u32 gEngine1[0x60]{}, gEngine2[0x60]{};      // RwEngineInstance falsas (E1 y E2): ranuras +0x144 y +0x148 = funciones falsas, el resto palabras del caso
inline u32 gNode{};                                              // lo que devuelven los falsos de RwFreeListAlloc y de RpAtomicCreate
inline u32 gDev1{}, gDev2{}, gDevCalls{}, gRampSeed{};           // dispositivos de la 1a y la 2a llamada a 0x7F9D50 y semilla de lo que escribe GetGammaRamp
inline u32 gReg[3]{};                                            // lo que devuelven los registros de plugins (anisotropia: 3; clumps: 2)
inline u32 gReadRet[2]{}, gReadVal[2]{}, gWriteRet[2]{}, gReadCalls{}, gWriteCalls{};
inline u32 gDirty{}, gLtmRet{}, gSqrtBits{}, gXor[3]{};
inline u32 gGwMask{}, gGwFixed{}, gGwCalls{}, gGwBits{};         // RpAtomicGetWorld: por mascara de llamada (bits) o fijo
inline u32 gCbMask{}, gCbCount{};                                // callbacks: resultado de la llamada n = (mascara >> (n % 32)) & 1
inline u32 gPokeId{}, gPokeIdx{}, gPokeKind{}, gPokeA{}, gPokeB{}, gPokeVal{}, gPokeCount{};
inline DWORD gBenchThread{};

inline void DumpTag(Ev& e, unsigned i, uintptr_t v) { e.v[i] = Tag(v); }
// Efecto lateral del caso: tras anotar el evento `id` (la llamada gPokeIdx de ese id) escribe 4 bytes en el destino (region global mas desplazamiento, arena, puntero-argumento mas desplazamiento o
// enlace del nodo actual mas desplazamiento)
inline void PokeIf(u32 id, uintptr_t ptrArg, uintptr_t ptr2 = 0) {
    if (!gPokeId || id != gPokeId) return;
    if (gPokeCount++ != gPokeIdx) return;
    uintptr_t at = 0;
    switch (gPokeKind) {
    case 0: at = uintptr_t(kGlobal[gPokeA]) + gPokeB; break;
    case 1: at = ArenaAt(gPokeA).Obj(gPokeB); break;
    case 2: at = ptrArg + gPokeB; break;
    default: at = ptr2 + gPokeB; break;
    }
    memcpy(reinterpret_cast<void*>(at), &gPokeVal, 4);
}

// --- funciones falsas (cdecl salvo los metodos COM, stdcall)
inline u32 __cdecl FkDevReset() { const u32 n = gDevCalls++; const u32 d = n == 1 ? gDev2 : gDev1; PushEv(0x7F9D50, 0); PokeIf(0x7F9D50, 0); return d; }
template<int VT> inline u32 __stdcall FkGetRamp(void* self, u32 swap, u32 ramp) {
    const u32 id = 0xC016 + 0x100 * (VT - 1);
    Ev& e = PushEv(id, 3); DumpTag(e, 0, uintptr_t(self)); e.v[1] = swap; e.v[2] = ramp;
    if (gRampSeed) FillWords(reinterpret_cast<u8*>(uintptr_t(ramp)), 0x600, gRampSeed);
    PokeIf(id, uintptr_t(self));
    return 0;
}
template<int VT> inline u32 __stdcall FkSetRamp(void* self, u32 swap, u32 flags, u32 ramp) {
    const u32 id = 0xC015 + 0x100 * (VT - 1);
    Ev& e = PushEv(id, 5); DumpTag(e, 0, uintptr_t(self)); e.v[1] = swap; e.v[2] = flags; e.v[3] = ramp; e.v[4] = Crc32(reinterpret_cast<const void*>(uintptr_t(ramp)), 0x600);
    PokeIf(id, uintptr_t(self));
    return 0;
}
template<int VT, int S> inline u32 __stdcall FkComWrong(void* self) { Ev& e = PushEv(u32(0xD000 + VT * 0x100 + S), 1); DumpTag(e, 0, uintptr_t(self)); return 0; }
template<int VT, size_t... S> inline void FillVtable(u32* out, std::index_sequence<S...>) {
    ((out[S] = u32(uintptr_t(S == 21 ? reinterpret_cast<void*>(&FkSetRamp<VT>) : (S == 22 ? reinterpret_cast<void*>(&FkGetRamp<VT>) : reinterpret_cast<void*>(&FkComWrong<VT, int(S)>))))), ...);
}
inline u32 __cdecl FkRegEngine(u32 a0, u32 a1, u32 a2, u32 a3) { Ev& e = PushEv(0x7F2BB0, 4); e.v[0] = a0; e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; PokeIf(0x7F2BB0, 0); return gReg[0]; }
inline u32 __cdecl FkRegTexture(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) { Ev& e = PushEv(0x7F3BB0, 5); e.v[0] = a0; e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4; PokeIf(0x7F3BB0, 0); return gReg[1]; }
inline u32 __cdecl FkRegStream(u32 a0, u32 a1, u32 a2, u32 a3) { Ev& e = PushEv(0x804550, 4); e.v[0] = a0; e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; PokeIf(0x804550, 0); return gReg[2]; }
inline u32 __cdecl FkAnisOffset(u32 off) { Ev& e = PushEv(0x7FDE60, 1); e.v[0] = off; PokeIf(0x7FDE60, 0); return 0; }
inline u32 __cdecl FkCamReg(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) { Ev& e = PushEv(0x7EE450, 5); e.v[0] = a0; e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4; PokeIf(0x7EE450, 0); return gReg[0]; }
inline u32 __cdecl FkLightReg(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) { Ev& e = PushEv(0x751D60, 5); e.v[0] = a0; e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4; PokeIf(0x751D60, 0); return gReg[1]; }
inline u32 __cdecl FkStreamRead(u32 stream, u32 ptr, u32 size) {
    const u32 n = gReadCalls++ & 1;
    Ev& e = PushEv(0x7ED540, 3); DumpTag(e, 0, stream); e.v[1] = ptr; e.v[2] = size;
    memcpy(reinterpret_cast<void*>(uintptr_t(ptr)), &gReadVal[n], 4);
    PokeIf(0x7ED540, 0);
    return gReadRet[n];
}
inline u32 __cdecl FkStreamWrite(u32 stream, u32 ptr, u32 size) {
    const u32 n = gWriteCalls++ & 1;
    u32 v; memcpy(&v, reinterpret_cast<const void*>(uintptr_t(ptr)), 4);
    Ev& e = PushEv(0x7ED460, 4); DumpTag(e, 0, stream); DumpTag(e, 1, ptr); e.v[2] = size; e.v[3] = v;
    PokeIf(0x7ED460, ptr);
    return gWriteRet[n];
}
inline void __cdecl FkResync(u32 atom) { Ev& e = PushEv(0x7491F0, 1); DumpTag(e, 0, atom); PokeIf(0x7491F0, atom); }
inline u32 __cdecl FkFrameDirty(u32 frame) { Ev& e = PushEv(0x7F0340, 1); DumpTag(e, 0, frame); PokeIf(0x7F0340, 0); return gDirty; }
inline u32 __cdecl FkGetLTM(u32 frame) { Ev& e = PushEv(0x7F0990, 1); DumpTag(e, 0, frame); PokeIf(0x7F0990, 0); return gLtmRet; }
inline u32 __cdecl FkTransform(u32 out, u32 in, u32 m) {
    u32 v[3]; memcpy(v, reinterpret_cast<const void*>(uintptr_t(in)), 12);
    Ev& e = PushEv(0x7EDD60, 6); DumpTag(e, 0, out); DumpTag(e, 1, in); DumpTag(e, 2, m); e.v[3] = v[0]; e.v[4] = v[1]; e.v[5] = v[2];
    for (int i = 0; i < 3; ++i) { const u32 w = v[i] ^ gXor[i]; memcpy(reinterpret_cast<void*>(uintptr_t(out) + 4 * i), &w, 4); }
    PokeIf(0x7EDD60, 0);
    return out;
}
inline float __cdecl FkSqrt(u32 bits) { Ev& e = PushEv(0x7EDB30, 1); e.v[0] = bits; PokeIf(0x7EDB30, 0); return std::bit_cast<float>(gSqrtBits); }
inline u32 __cdecl FkDeInit(u32 table, u32 obj) { Ev& e = PushEv(0x808740, 2); e.v[0] = table; DumpTag(e, 1, obj); PokeIf(0x808740, obj); return 0; }
inline u32 __cdecl FkResFree(u32 entry) { Ev& e = PushEv(0x807DE0, 1); DumpTag(e, 0, entry); PokeIf(0x807DE0, entry); return 0; }
inline u32 __cdecl FkGeomDestroy(u32 geom) { Ev& e = PushEv(0x74CCC0, 1); DumpTag(e, 0, geom); PokeIf(0x74CCC0, geom); return 1; }
inline u32 __cdecl FkGeomAddRef(u32 geom) { Ev& e = PushEv(0x74CCB0, 1); DumpTag(e, 0, geom); PokeIf(0x74CCB0, geom); return geom; }
inline u32 __cdecl FkGetWorld(u32 atom) {
    const u32 n = gGwCalls++;
    Ev& e = PushEv(0x751050, 1); DumpTag(e, 0, atom);
    PokeIf(0x751050, atom);
    return gGwMask ? ((gGwBits >> (n % 32)) & 1) : gGwFixed;
}
inline u32 __cdecl FkFrameUpdate(u32 frame) { Ev& e = PushEv(0x7F0910, 1); DumpTag(e, 0, frame); PokeIf(0x7F0910, frame); return 0; }
inline u32 __cdecl FkReleaseFrame(u32 obj) { Ev& e = PushEv(0x804F40, 1); DumpTag(e, 0, obj); PokeIf(0x804F40, obj); return 0; }
inline u32 __cdecl FkLightDestroy(u32 light) { Ev& e = PushEv(0x7520D0, 1); DumpTag(e, 0, light); PokeIf(0x7520D0, light); return 1; }
inline u32 __cdecl FkCameraDestroy(u32 cam) { Ev& e = PushEv(0x7EE4B0, 1); DumpTag(e, 0, cam); PokeIf(0x7EE4B0, cam); return 1; }
inline u32 __cdecl FkFrameDestroyHierarchy(u32 frame) { Ev& e = PushEv(0x7F08A0, 1); DumpTag(e, 0, frame); PokeIf(0x7F08A0, frame); return 1; }
inline u32 __cdecl FkSetFrame(u32 atom, u32 frame) { Ev& e = PushEv(0x804EF0, 2); DumpTag(e, 0, atom); e.v[1] = frame; PokeIf(0x804EF0, atom); return atom; }
inline u32 __cdecl FkInitObject(u32 table, u32 obj) { Ev& e = PushEv(0x8086E0, 2); e.v[0] = table; DumpTag(e, 1, obj); PokeIf(0x8086E0, obj); return obj; }
inline u32 __cdecl FkCopyObject(u32 table, u32 dst, u32 src) { Ev& e = PushEv(0x808770, 3); e.v[0] = table; DumpTag(e, 1, dst); DumpTag(e, 2, src); PokeIf(0x808770, dst); return dst; }
inline u32 __cdecl FkCreate() { PushEv(0x749C50, 0); PokeIf(0x749C50, 0); return gNode; }
// RwEngineInstance+0x144 (RwFreeListAlloc (lista, pista)) y +0x148 (RwFreeListFree (lista, nodo)) de las tablas E1 (N = 1) y E2 (N = 2)
template<int N> inline u32 __cdecl FkListAlloc(u32 fl, u32 hint) { const u32 id = 0xE144 + 0x100 * (N - 1); Ev& e = PushEv(id, 2); e.v[0] = fl; e.v[1] = hint; PokeIf(id, 0); return gNode; }
template<int N> inline u32 __cdecl FkListFree(u32 fl, u32 node) { const u32 id = 0xE148 + 0x100 * (N - 1); Ev& e = PushEv(id, 2); e.v[0] = fl; DumpTag(e, 1, node); PokeIf(id, node); return 0; }
// Callbacks: de dibujo de RpClumpRender (un argumento, resultado por la mascara) y de RpClumpForAllCameras / Lights (objeto y datos)
template<int K> inline u32 __cdecl FkRender(u32 atom) { const u32 id = 0xCB10 + K; Ev& e = PushEv(id, 1); DumpTag(e, 0, atom); const u32 r = (gCbMask >> (gCbCount++ % 32)) & 1; PokeIf(id, atom); return r; }
template<int K> inline u32 __cdecl FkForAll(u32 obj, u32 data) {
    const u32 id = 0xCB20 + K;
    Ev& e = PushEv(id, 2); DumpTag(e, 0, obj); DumpTag(e, 1, data);
    const u32 r = (gCbMask >> (gCbCount++ % 32)) & 1;
    const u32 reg = (curFn == 10) ? 0xC924D8u : 0xC924DCu;
    PokeIf(id, obj, uintptr_t(obj) + *reinterpret_cast<const u32*>(reg) + 4);
    return r;
}
template<size_t... S> inline void FillEngine(u32* out, u32 seed, int n, std::index_sequence<S...>) {
    ((out[S] = Rand(seed, u32(S))), ...);
    out[0x144 / 4] = u32(uintptr_t(n == 1 ? reinterpret_cast<void*>(&FkListAlloc<1>) : reinterpret_cast<void*>(&FkListAlloc<2>)));
    out[0x148 / 4] = u32(uintptr_t(n == 1 ? reinterpret_cast<void*>(&FkListFree<1>) : reinterpret_cast<void*>(&FkListFree<2>)));
}
inline void* FakeFn(u32 k) {
    if (curFn == 9) { switch (k) { case 0: return reinterpret_cast<void*>(&FkRender<0>); case 1: return reinterpret_cast<void*>(&FkRender<1>); case 2: return reinterpret_cast<void*>(&FkRender<2>); default: return reinterpret_cast<void*>(&FkRender<3>); } }
    switch (k) { case 0: return reinterpret_cast<void*>(&FkForAll<0>); case 1: return reinterpret_cast<void*>(&FkForAll<1>); case 2: return reinterpret_cast<void*>(&FkForAll<2>); default: return reinterpret_cast<void*>(&FkForAll<3>); }
}
// Especificacion de valor del caso -> valor real
inline u32 Res(u32 v) {
    if (IsPS(v)) return u32(ArenaAt((v >> 28) - 0xAu).Obj(v & 0x0FFFFFFFu));
    if (v == kEngine1) return u32(uintptr_t(gEngine1));
    if (v == kEngine2) return u32(uintptr_t(gEngine2));
    if (IsFS(v)) return u32(uintptr_t(FakeFn(v & 0xFFu)));
    return v;
}

// Direcciones de los cuerpos de la DLL de las dos funciones cuyo puntero guarda RpAtomicCreate (se canonizan a la direccion del EXE antes de observar)
inline uintptr_t gSyncBody{}, gDrawBody{}, gAsi{};
// true si la cadena de saltos E9 que empieza en v (hasta 8) pasa por `body` (el puntero que guarda el codigo C++ puede ser el del cuerpo o el de un enlace de salto hacia el)
inline bool ChainHits(uintptr_t v, uintptr_t body) {
    uintptr_t p = v;
    for (int k = 0; k < 8; ++k) {
        if (p == body) return true;
        if (*reinterpret_cast<const uint8*>(p) != 0xE9) return false;
        p += 5 + *reinterpret_cast<const int32*>(p + 1);
    }
    return p == body;
}
inline u32 CanonPtr(u32 v) {
    if (!v || !gAsi || ModuleOf(v) != gAsi) return v;
    if ((gSyncBody && ChainHits(v, gSyncBody)) || ChainHits(v, 0x749D20)) return 0x749D20;
    if ((gDrawBody && ChainHits(v, gDrawBody)) || ChainHits(v, 0x7491C0)) return 0x7491C0;   // con el hook apagado el enlace de salto de la DLL va a la funcion del EXE
    return v;
}

// ================================================================================================ cierre de un lado
inline json CloseSide(Env& env, Side& side, json obs) {
    auto fp = NormalizeFp(side.fp);
    obs["globals_hash"] = RegionsHash();
    json out = CommonRow(side, fp); out["observed"] = obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"] = env.WatchesIntact() && RegionsIntact();
    out["floating_state_restored"] = FloatingStateRestored();
    return out;
}

// Descripcion de la llamada de un caso
struct Call {
    std::array<u32, 8> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

inline u8* At(uintptr_t addr) { return reinterpret_cast<u8*>(addr); }
inline void PutVtable(unsigned arena, u32 off, int vt) {
    u32 tab[24];
    if (vt == 1) FillVtable<1>(tab, std::make_index_sequence<24>{}); else FillVtable<2>(tab, std::make_index_sequence<24>{});
    memcpy(At(ArenaAt(arena).Obj(off)), tab, sizeof(tab));
}
// Prepara las arenas y las regiones globales del caso; devuelve la llamada. Las escrituras son las del oraculo (fixture_oracle.py).
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gNode = 0; gDev1 = gDev2 = gDevCalls = gRampSeed = 0;
    for (auto& v : gReg) v = 0;
    for (auto& v : gReadRet) v = 0;
    for (auto& v : gReadVal) v = 0;
    for (auto& v : gWriteRet) v = 0;
    gReadCalls = gWriteCalls = 0;
    gDirty = gLtmRet = gSqrtBits = 0;
    for (auto& v : gXor) v = 0;
    gGwMask = gGwFixed = gGwCalls = gGwBits = 0;
    gCbMask = gCbCount = 0;
    FillEngine(gEngine1, a[47], 1, std::make_index_sequence<0x60>{});
    FillEngine(gEngine2, a[47] + 1, 2, std::make_index_sequence<0x60>{});
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    // regiones globales vigiladas de la funcion
    for (const u32 gi : GlobalsOf(c.fn)) {
        u8* p = reinterpret_cast<u8*>(kGlobal[gi]);
        if (gi == 0) { const u32 v[4]{a[48], a[59], a[60], a[61]}; memcpy(p, v, 16); }
        else if (gi == 2) FillWords(p, 0xC00, a[62]);
        else { const u32 v = gi == 10 ? Res(a[48 + 10]) : a[48 + gi]; memcpy(p, &v, 4); }
    }
    auto& A = arenaA; auto& B = arenaB; auto& Cc = arenaC; auto& D = arenaD;
    const auto Devs = [&](u32 v) -> u32 { return v == 0 ? 0u : (v == 1 ? u32(A.Obj(o)) : u32(D.Obj(a[13]))); };
    switch (kKind[c.fn]) {
    case kGamma:
        PutVtable(2, a[12], 1); PutVtable(2, a[14], 2);
        A.Put32(o, u32(Cc.Obj(a[12]))); D.Put32(a[13], u32(Cc.Obj(a[14])));
        gDev1 = Devs(a[10]); gDev2 = Devs(a[11]); gRampSeed = a[15];
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kAnisot:
        gReg[0] = a[4]; gReg[1] = a[5]; gReg[2] = a[6];
        break;
    case kRead:
        gReadRet[0] = a[9]; gReadRet[1] = a[10]; gReadVal[0] = a[11]; gReadVal[1] = a[12];
        call.args = {a[4], a[5], a[6], a[7], a[8]}; call.argCount = 5; call.popBytes = 20;
        break;
    case kWrite:
        A.Put32(o + 0x6C, u32(B.Obj(a[8])));
        gWriteRet[0] = a[10]; gWriteRet[1] = a[11];
        call.args = {a[4], a[5], thisA, a[6], a[7]}; call.argCount = 5; call.popBytes = 20;
        break;
    case kResync:
    case kWorldBS: {
        A.Put32(o + 0x18, a[4] ? u32(B.Obj(a[12])) : 0u);
        A.Put32(o + 0x4C, a[8]); A.Put16(o + 0x50, a[6] & 0xFFFF); A.Put16(o + 0x52, a[7] & 0xFFFF);
        A.Put32(o + 0x58, a[9]); A.Put32(o + 0x5C, a[10]); A.Put8(o + 3, a[11] & 0xFF);
        for (u32 j = 0; j < 4; ++j) A.Put32(o + 0x1C + 4 * j, a[64 + j]);
        B.Put32(a[12] + 0x18, a[5]); B.Put32(a[12] + 0x5C, u32(D.Obj(a[13])));
        for (u32 i = 0; i < 15; ++i) for (u32 j = 0; j < 4; ++j) D.Put32(a[13] + 0x1C * i - 0x70 + 4 + 4 * j, a[68 + 4 * i + j]);
        if (kKind[c.fn] == kWorldBS) {
            A.Put32(o + 4, Res(a[31]));
            const u32 m = a[16];
            static constexpr u32 offs[9]{0, 4, 8, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28};
            for (u32 j = 0; j < 9; ++j) Cc.Put32(m + offs[j], a[18 + j]);
            Cc.Put32(m + 0xC, a[17]);
            gLtmRet = u32(Cc.Obj(m)); gDirty = a[15]; gSqrtBits = a[27]; gXor[0] = a[28]; gXor[1] = a[29]; gXor[2] = a[30];
        }
        call.args[0] = thisA; call.argCount = 1; call.popBytes = 4;
        break;
    }
    case kClumpFree: {
        const u32 L_a = a[4], L_l = a[5], L_c = a[6], bB = a[7], sB = a[8];
        A.Put32(o + 4, Res(a[14]));
        const u32 headA = thisA + 8;
        const auto AtomLink = [&](u32 i) { return u32(B.Obj(bB + sB * i + 0x40)); };
        A.Put32(o + 8, L_a ? AtomLink(0) : headA); A.Put32(o + 0xC, L_a ? AtomLink(L_a - 1) : headA);
        for (u32 i = 0; i < L_a; ++i) {
            B.Put32(bB + sB * i + 0x40, i + 1 < L_a ? AtomLink(i + 1) : headA);
            B.Put32(bB + sB * i + 0x44, i ? AtomLink(i - 1) : headA);
            const u32 p = 64 + 4 * i;
            B.Put32(bB + sB * i + 0x14, a[p]); B.Put32(bB + sB * i + 0x18, a[p + 1]); B.Put32(bB + sB * i + 4, Res(a[p + 2]));
        }
        struct LS { u32 cnt, base, sep, arena, headoff; };
        const LS lists[2]{{L_l, a[9], a[10], 2, 0x10}, {L_c, a[11], a[12], 3, 0x18}};
        for (const auto& l : lists) {
            const u32 head = thisA + l.headoff;
            auto& mm = ArenaAt(l.arena);
            const auto Node = [&](u32 i) { return u32(mm.Obj(l.base + l.sep * i)); };
            A.Put32(o + l.headoff, l.cnt ? Node(0) : head); A.Put32(o + l.headoff + 4, l.cnt ? Node(l.cnt - 1) : head);
            for (u32 i = 0; i < l.cnt; ++i) {
                mm.Put32(l.base + l.sep * i, i + 1 < l.cnt ? Node(i + 1) : head);
                mm.Put32(l.base + l.sep * i + 4, i ? Node(i - 1) : head);
            }
        }
        gGwMask = 1; gGwBits = a[13];
        call.args[0] = thisA; call.args[1] = c.edx; call.argCount = 2; call.popBytes = 8;
        break;
    }
    case kAtomFree:
        A.Put32(o + 0x14, a[4]); A.Put32(o + 0x18, a[5]); A.Put32(o + 4, Res(a[6]));
        gGwFixed = a[7];
        call.args[0] = thisA; call.args[1] = c.edx; call.argCount = c.fn == 7 ? 2 : 1; call.popBytes = 4 * call.argCount;
        break;
    case kClumpAttach:
        gReg[0] = a[4]; gReg[1] = a[5];
        break;
    case kRender: {
        const u32 cnt = a[4], bB = a[5], sB = a[6];
        const u32 head = thisA + 8;
        const auto Link = [&](u32 i) { return u32(B.Obj(bB + sB * i + 0x40)); };
        A.Put32(o + 8, cnt ? Link(0) : head); A.Put32(o + 0xC, cnt ? Link(cnt - 1) : head);
        for (u32 i = 0; i < cnt; ++i) {
            B.Put32(bB + sB * i + 0x40, i + 1 < cnt ? Link(i + 1) : head);
            B.Put32(bB + sB * i + 0x44, i ? Link(i - 1) : head);
            const u32 p = 64 + 4 * i;
            B.Put8(bB + sB * i + 2, a[p] & 0xFF); B.Put32(bB + sB * i + 4, Res(a[p + 1])); B.Put32(bB + sB * i + 0x48, Res(a[p + 2]));
        }
        gCbMask = a[7]; gLtmRet = a[8];
        call.args[0] = thisA; call.argCount = 1; call.popBytes = 4;
        break;
    }
    case kForAll: {
        const u32 cnt = a[4], base = a[5], sep = a[6];
        const bool cams = c.fn == 10;
        const u32 headoff = cams ? 0x18 : 0x10;
        const u32 head = thisA + headoff;
        const auto Node = [&](u32 i) { return u32(Cc.Obj(base + sep * i)); };
        A.Put32(o + headoff, cnt ? Node(0) : head); A.Put32(o + headoff + 4, cnt ? Node(cnt - 1) : head);
        for (u32 i = 0; i < cnt; ++i) {
            Cc.Put32(base + sep * i, i + 1 < cnt ? Node(i + 1) : head);
            Cc.Put32(base + sep * i + 4, i ? Node(i - 1) : head);
        }
        gCbMask = a[7];
        call.args[0] = thisA; call.args[1] = Res(a[8]); call.args[2] = Res(a[9]); call.argCount = 3; call.popBytes = 12;
        break;
    }
    case kCreate:
        gNode = a[4] ? thisA : 0u;
        break;
    case kSync:
        A.Put32(o + 0x4C, a[4]); A.Put8(o + 3, a[5] & 0xFF);
        call.args[0] = thisA; call.argCount = 1; call.popBytes = 4;
        break;
    case kSetGeom: {
        const u32 G1 = u32(B.Obj(a[8])), G2 = u32(B.Obj(a[9]));
        const u32 specs[4]{0u, G1, G2, a[13]};
        const u32 cur = specs[a[4]];
        const u32 arg = a[5] == 4 ? cur : (a[5] == 3 ? a[14] : specs[a[5]]);
        A.Put32(o + 0x18, cur); A.Put32(o + 4, Res(a[7]));
        for (u32 j = 0; j < 4; ++j) A.Put32(o + 0x1C + 4 * j, a[64 + j]);
        B.Put32(a[8] + 0x5C, u32(D.Obj(a[10]))); B.Put32(a[9] + 0x5C, u32(D.Obj(a[11])));
        for (u32 j = 0; j < 4; ++j) { D.Put32(a[10] + 4 + 4 * j, a[68 + j]); D.Put32(a[11] + 4 + 4 * j, a[72 + j]); }
        gGwFixed = a[12];
        call.args[0] = thisA; call.args[1] = arg; call.args[2] = a[6]; call.argCount = 3; call.popBytes = 12;
        break;
    }
    case kClone: {
        const u32 gc = a[6] ? a[13] : 0u;
        const u32 sg = a[5] == 0 ? 0u : (a[5] == 1 ? u32(Cc.Obj(a[10])) : gc);
        A.Put32(o + 0x18, sg); B.Put32(a[4] + 0x18, gc);
        Cc.Put32(a[10] + 0x5C, u32(D.Obj(a[11])));
        for (u32 j = 0; j < 4; ++j) D.Put32(a[11] + 4 + 4 * j, a[64 + j]);
        gNode = a[7] ? 0u : u32(B.Obj(a[4]));
        gGwFixed = a[9];
        call.args[0] = thisA; call.argCount = 1; call.popBytes = 4;
        break;
    }
    }
    gPokeId = a[40]; gPokeIdx = a[41]; gPokeKind = a[42]; gPokeA = a[43]; gPokeB = a[44]; gPokeVal = a[45] ? Res(a[46]) : a[46]; gPokeCount = 0;
    return call;
}

enum RetKind { rNone, rEax };
inline RetKind RetOf(unsigned fn) { return (fn == 0 || fn == 4 || fn == 6 || fn == 7) ? rNone : rEax; }

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0; gThrew = false;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        if (fn == 12) {   // punteros a funcion que guarda RpAtomicCreate: la DLL guarda la direccion de su cuerpo, el EXE la suya; se compara la direccion del EXE (la del hook)
            for (const u32 off : {0x10u, 0x48u}) arenaA.Put32(c.a[0] + off, CanonPtr(arenaA.Get32(c.a[0] + off)));
        }
        json obs;
        if (fn == 9) {   // punteros a las funciones falsas de dibujo guardados en los atomos (por el caso o por un efecto lateral): se comparan como 0xF00000kk
            for (u32 i = 0; i < 4; ++i) {
                const size_t off = kGuard + c.a[5] + c.a[6] * i + 0x48;
                for (auto* mem : {&arenaB.m, &arenaB.r}) {
                    u32 v; memcpy(&v, mem->data() + off, 4);
                    for (u32 k = 0; k < 4; ++k) if (v == u32(uintptr_t(FakeFn(k)))) { const u32 w = FS(k); memcpy(mem->data() + off, &w, 4); }
                }
            }
        }
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["big"] = RegionBigCrc();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        if (!gThrew && RetOf(fn) == rEax) obs["ret"] = eax; else obs["ret"] = nullptr;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN226A_CASES (solo para el humo y las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN226A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c);
            Write({{"event", "function_summary"}, {"function", kFn[f]}, {"cases", nCases}, {"failures", totals.failures - before}, {"state_restored", env.WatchesIntact() && RegionsIntact()}});
        }
    }
    RestoreAllRegions();
    const bool entriesIntact = env.WatchesIntact() && RegionsIntact();
    bool stateRestored = entriesIntact;
    for (auto& r : env.collab) if (r) stateRestored &= r->Restore();
    for (auto& r : env.route) if (r) stateRestored &= r->Restore();
    RestoreFloatingState(); stateRestored &= FloatingStateRestored();
    Write({{"event", "global_state_final"}, {"floating_state", HexBytes(savedFP.data(), 108)}, {"mxcsr", ReadMXCSR()}, {"state_restored", stateRestored}, {"watched_intact", entriesIntact}});
    Write({{"event", ready && totals.passed && stateRestored ? "complete" : "error"}, {"pairs", totals.pairs}, {"state_restored", stateRestored}, {"floating_state_restored", FloatingStateRestored()}, {"entries_intact", entriesIntact}});
    RestoreFloatingState(); savedFPActive = false;
    // El banco ya escribio todos los resultados: el juego sale aqui. Con la partida cargada el siguiente fotograma tras una corrida de ~25 min pierde el dispositivo D3D (asercion de RenderWare ajena al lote)
    // antes de que el analizador termine; la salida inmediata deja el registro acabado en `complete` (o `error`).
    log.flush();
    ExitProcess(ready && totals.passed && stateRestored ? 0u : 1u);
}

// Busca el hook registrado para una direccion del EXE (categoria y nombre). Hooks Simple (direccion del EXE) y Virtual (direccion de la funcion del EXE que ocupa la ranura): de un Virtual devuelve ademas
// el cuerpo de la DLL (la entrada de su vtabla), que HookRoute usa como candidato. Un Virtual lleva dentro un Simple sobre la misma direccion (Virtual::Switch).
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name, uintptr_t* virtualBody = nullptr) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items()) {
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            } else if (virtualBody && h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Virtual
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
                *virtualBody = reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h.get())->GetHookOurAddress());
            }
        }
    });
    return found;
}
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}


inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn226a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn226a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn226a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    gAsi = env.asi;
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
    // Ruta RH de cada funcion probada: se construye ANTES que nada; el constructor deja el hook apagado.
    for (unsigned f = 0; f < kFunctions; ++f) {
        std::string cat, name; uintptr_t vbody = 0;
        const bool found = FindHook(kEntries[f], cat, name, &vbody);
        if (found) {
            if (vbody) env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], vbody));
            else env.route[f].reset(new HookRoute(cat.c_str(), name.c_str(), kEntries[f], HookRoute::RegisteredTarget{}));
        }
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) {
        for (unsigned f = 0; f < kFunctions; ++f) collaborators[kFn[f]] = {{"own_hook", ownHooks[kFn[f]]}, {"body", env.route[f] ? env.route[f]->Body() : 0}, {"body_module", env.route[f] ? ModuleOf(env.route[f]->Body()) : 0}};
        Run(env, false, registered, bodies, collaborators);
        return;
    }
    gSyncBody = Body(env.route[13]->Body());
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Un colaborador con hook RH que es tambien una funcion probada reutiliza su ruta (el hook ya esta apagado); si no, se crea una ruta de colaborador
    auto Collab = [&](uintptr_t exe) -> Pair {
        auto it = collabCache.find(exe);
        if (it != collabCache.end()) return it->second;
        Pair p{exe, 0, false};
        for (unsigned f = 0; f < kFunctions; ++f)
            if (kEntries[f] == exe && env.route[f]) { p.dll = Body(env.route[f]->Body()); p.hooked = true; hooksFound.push_back({{"exe", exe}, {"tested_function", kFn[f]}}); }
        if (!p.hooked) p = MakeCollab(env, exe, hooksFound);
        return collabCache.emplace(exe, p).first->second;
    };
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en la direccion del EXE y en el cuerpo de la DLL (los que la DLL llama por la direccion del EXE usan solo la del EXE; el parche
    // en el cuerpo de la DLL no se usa y es inocuo)
    auto F = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(p, reinterpret_cast<void*>(fake));
    };
    {   // RpAtomicDefaultRenderCallBack (0x7491C0): solo su cuerpo en la DLL, para canonizar el puntero que guarda RpAtomicCreate
        const Pair p = Collab(0x7491C0);
        if (!p.hooked || !p.dll) ready = false;
        gDrawBody = p.dll;
    }
    env.stubs[0] = {F(0x7F9D50, &FkDevReset)};
    env.stubs[1] = {F(0x7F2BB0, &FkRegEngine), F(0x7F3BB0, &FkRegTexture), F(0x804550, &FkRegStream), F(0x7FDE60, &FkAnisOffset)};
    env.stubs[2] = {F(0x7ED540, &FkStreamRead)};
    env.stubs[3] = {F(0x7ED460, &FkStreamWrite)};
    env.stubs[5] = {F(0x7491F0, &FkResync), F(0x7F0340, &FkFrameDirty), F(0x7F0990, &FkGetLTM), F(0x7EDD60, &FkTransform), F(0x7EDB30, &FkSqrt)};
    const auto AtomDestroyStubs = [&]() -> std::vector<StubSpec> {
        return {F(0x808740, &FkDeInit), F(0x807DE0, &FkResFree), F(0x74CCC0, &FkGeomDestroy), F(0x751050, &FkGetWorld), F(0x7F0910, &FkFrameUpdate), F(0x804F40, &FkReleaseFrame)};
    };
    env.stubs[6] = AtomDestroyStubs(); env.stubs[6].push_back(F(0x7520D0, &FkLightDestroy)); env.stubs[6].push_back(F(0x7EE4B0, &FkCameraDestroy)); env.stubs[6].push_back(F(0x7F08A0, &FkFrameDestroyHierarchy));
    env.stubs[7] = AtomDestroyStubs();
    env.stubs[15] = AtomDestroyStubs();
    env.stubs[8] = {F(0x7EE450, &FkCamReg), F(0x751D60, &FkLightReg)};
    env.stubs[9] = {F(0x7F0990, &FkGetLTM)};
    env.stubs[12] = {F(0x804EF0, &FkSetFrame), F(0x8086E0, &FkInitObject)};
    env.stubs[13] = {F(0x7491F0, &FkResync)};
    env.stubs[14] = {F(0x74CCB0, &FkGeomAddRef), F(0x74CCC0, &FkGeomDestroy), F(0x751050, &FkGetWorld), F(0x7F0910, &FkFrameUpdate)};
    env.stubs[16] = {F(0x749C50, &FkCreate), F(0x804EF0, &FkSetFrame), F(0x74CCB0, &FkGeomAddRef), F(0x74CCC0, &FkGeomDestroy), F(0x751050, &FkGetWorld), F(0x7F0910, &FkFrameUpdate), F(0x808770, &FkCopyObject)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en el cuerpo de la DLL); RwEngineInstance falsa (dos tablas) y objetos COM con vtables falsas";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    collaborators["canon"] = {{"sync_body", gSyncBody}, {"draw_body", gDrawBody}, {"draw_exe_hooked", true}};
    // Regiones globales del juego que el banco fija y vigila: una por region; cada funcion declara las suyas
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    for (unsigned gi = 0; gi < kRegions; ++gi) regions.emplace_back(kGlobal[gi], kGSize[gi]);
    for (unsigned f = 0; f < kFunctions; ++f) for (const u32 gi : GlobalsOf(f)) regionsOf[f].push_back(gi);
    Run(env, ready, registered, bodies, collaborators);
}
}
