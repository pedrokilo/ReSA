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

// Banco par a par Fn224A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 14 funciones del EXE (ver Fixtures.hpp: controlador de rasters D3D9 de RenderWare), cada una con 16384 pares EXE/DLL. Hooks S (RH_ScopedNamedGlobalInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F): RwFreeListCreate 0x801980, RwFreeListDestroy 0x801B80,
// D3DResourceSystem::DestroyTexture 0x730B70 y RasterListDestroy 0x4CB5E0 (funcion probada, la usa RasterListShutdown); _rwD3D9RWGetRasterStage 0x7FDE50 y _rwD3D9RWSetRasterStage 0x7FDCD0, que la DLL llama por
// la direccion del EXE, llevan el falso solo en esa direccion (FE). Las funciones de RwEngineInstance (+0x134 malloc, +0x138 free, +0x13C realloc, +0x144 RwFreeListAlloc, +0x148 RwFreeListFree) se llaman por
// puntero: [0xC97B24] apunta a una estructura falsa (los demas huecos anotan que se llamaron por error). Los objetos COM (IDirect3D9, recursos) llevan vtables falsas en las arenas.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), las globales vigiladas (gchanges), los eventos de los falsos (con sus
// argumentos), el valor devuelto (EAX completo) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).


// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn224a-crash.txt (junto al EXE).
namespace F224ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn224a-crash.txt";
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

namespace F224ALive {
using namespace F224AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F224AFix::Arena(WIN);
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
// Especificacion de puntero del caso -> valor real
inline u32 Res(u32 v) { return IsPS(v) ? u32(ArenaAt((v >> 28) - 0xAu).Obj(v & 0x0FFFFFFFu)) : v; }

// ================================================================================================ falsos de los colaboradores
inline u32 gNode{}, gMalloc{}, gRealloc{}, gCreate{};   // lo que devuelven los falsos de RwFreeListAlloc, malloc, realloc y RwFreeListCreate
inline u32 gHr1{}, gHr2{};                               // HRESULT de CheckDeviceFormat y CheckDepthStencilMatch
inline u32 gStage[8]{};                                  // lo que devuelve el falso de _rwD3D9RWGetRasterStage
inline u32 gPokeId{}, gPokeIdx{}, gPokeKind{}, gPokeA{}, gPokeB{}, gPokeVal{}, gPokeCount{};
inline DWORD gBenchThread{};
alignas(16) inline u32 gEngine[0x60]{};                  // RwEngineInstance falsa (ranuras hasta +0x17F)

inline void DumpTag(Ev& e, unsigned i, uintptr_t v) { e.v[i] = Tag(v); }
// Efecto lateral del caso: tras anotar el evento `id` (la llamada gPokeIdx de ese id) escribe 4 bytes en el destino (global, arena o puntero-argumento mas desplazamiento)
inline void PokeIf(u32 id, uintptr_t ptrArg) {
    if (!gPokeId || id != gPokeId) return;
    if (gPokeCount++ != gPokeIdx) return;
    const uintptr_t at = gPokeKind == 0 ? uintptr_t(kGlobal[gPokeA]) : (gPokeKind == 1 ? ArenaAt(gPokeA).Obj(gPokeB) : ptrArg + gPokeB);
    memcpy(reinterpret_cast<void*>(at), &gPokeVal, 4);
}

// RwEngineInstance+0x144 (RwFreeListAlloc, cdecl (lista, pista)): evento [0xE144, lista, pista]; devuelve gNode.
inline u32 __cdecl FkListAlloc(u32 fl, u32 hint) { Ev& e = PushEv(0xE144, 2); DumpTag(e, 0, fl); e.v[1] = hint; PokeIf(0xE144, 0); return gNode; }
// RwEngineInstance+0x148 (RwFreeListFree, cdecl (lista, nodo)): evento [0xE148, lista, nodo].
inline u32 __cdecl FkListFree(u32 fl, u32 node) { Ev& e = PushEv(0xE148, 2); DumpTag(e, 0, fl); DumpTag(e, 1, node); PokeIf(0xE148, node); return 0; }
// RwEngineInstance+0x134 (malloc (tamano, pista)): [0xE134, tamano, pista]; devuelve gMalloc.
inline u32 __cdecl FkMalloc(u32 size, u32 hint) { Ev& e = PushEv(0xE134, 2); e.v[0] = size; e.v[1] = hint; PokeIf(0xE134, 0); return gMalloc; }
// RwEngineInstance+0x138 (free (puntero)): [0xE138, puntero].
inline u32 __cdecl FkFree(u32 p) { Ev& e = PushEv(0xE138, 1); DumpTag(e, 0, p); PokeIf(0xE138, 0); return 0; }
// RwEngineInstance+0x13C (realloc (puntero, tamano, pista)): [0xE13C, puntero, tamano, pista]; devuelve gRealloc.
inline u32 __cdecl FkRealloc(u32 p, u32 size, u32 hint) { Ev& e = PushEv(0xE13C, 3); DumpTag(e, 0, p); e.v[1] = size; e.v[2] = hint; PokeIf(0xE13C, 0); return gRealloc; }
template<u32 S> inline u32 __cdecl FkEngineWrong() { PushEv(0xEF00 + S, 0); return 0; }
// RwFreeListCreate (0x801980, cdecl (entrada, por bloque, alineacion, pista)): [0x801980, 4 argumentos]; devuelve gCreate.
inline u32 __cdecl FkFLCreate(u32 a0, u32 a1, u32 a2, u32 a3) { Ev& e = PushEv(0x801980, 4); e.v[0] = a0; e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; PokeIf(0x801980, 0); return gCreate; }
// RwFreeListDestroy (0x801B80, cdecl (lista)): [0x801B80, lista]; devuelve 1.
inline u32 __cdecl FkFLDestroy(u32 fl) { Ev& e = PushEv(0x801B80, 1); DumpTag(e, 0, fl); PokeIf(0x801B80, 0); return 1; }
// _rwD3D9RWGetRasterStage (0x7FDE50, cdecl (etapa)): [0x7FDE50, etapa]; devuelve gStage[etapa].
inline u32 __cdecl FkGetStage(u32 stage) { Ev& e = PushEv(0x7FDE50, 1); e.v[0] = stage; PokeIf(0x7FDE50, 0); return gStage[stage & 7]; }
// _rwD3D9RWSetRasterStage (0x7FDCD0, cdecl (raster, etapa)): [0x7FDCD0, raster, etapa].
inline void __cdecl FkSetStage(u32 raster, u32 stage) { Ev& e = PushEv(0x7FDCD0, 2); DumpTag(e, 0, raster); e.v[1] = stage; PokeIf(0x7FDCD0, 0); }
// D3DResourceSystem::DestroyTexture (0x730B70, cdecl (textura)): [0x730B70, textura].
inline void __cdecl FkDestroyTexture(u32 tex) { Ev& e = PushEv(0x730B70, 1); DumpTag(e, 0, tex); PokeIf(0x730B70, 0); }
// RasterListDestroy (0x4CB5E0, cdecl sin argumentos): [0x4CB5E0].
inline void __cdecl FkRasterListDestroy() { PushEv(0x4CB5E0, 0); PokeIf(0x4CB5E0, 0); }
// COM (stdcall): Release (ranura 2), CheckDeviceFormat (ranura 10) y CheckDepthStencilMatch (ranura 12) de las dos vtables falsas (VT = 1 o 2); las demas ranuras anotan que se llamaron por error.
template<int VT> inline u32 __stdcall FkRelease(void* self) { const u32 id = 0xC008 + 0x100 * (VT - 1); Ev& e = PushEv(id, 1); DumpTag(e, 0, uintptr_t(self)); PokeIf(id, uintptr_t(self)); return 1; }
template<int VT> inline u32 __stdcall FkCheckFmt(void* self, u32 adapter, u32 dev, u32 afmt, u32 usage, u32 rtype, u32 fmt) {
    const u32 id = 0xC00A + 0x100 * (VT - 1);
    Ev& e = PushEv(id, 7);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = adapter; e.v[2] = dev; e.v[3] = afmt; e.v[4] = usage; e.v[5] = rtype; e.v[6] = fmt;
    PokeIf(id, uintptr_t(self));
    return gHr1;
}
template<int VT> inline u32 __stdcall FkMatch(void* self, u32 adapter, u32 dev, u32 afmt, u32 rt, u32 depth) {
    const u32 id = 0xC00C + 0x100 * (VT - 1);
    Ev& e = PushEv(id, 6);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = adapter; e.v[2] = dev; e.v[3] = afmt; e.v[4] = rt; e.v[5] = depth;
    PokeIf(id, uintptr_t(self));
    return gHr2;
}
template<int VT, int S> inline u32 __stdcall FkComWrong(void* self) { Ev& e = PushEv(u32(0xD000 + VT * 0x100 + S), 1); DumpTag(e, 0, uintptr_t(self)); return 0; }
template<int VT, size_t... S> inline void FillVtable(u32* out, std::index_sequence<S...>) {
    ((out[S] = u32(uintptr_t(S == 2 ? reinterpret_cast<void*>(&FkRelease<VT>) : (S == 10 ? reinterpret_cast<void*>(&FkCheckFmt<VT>) : (S == 12 ? reinterpret_cast<void*>(&FkMatch<VT>) : reinterpret_cast<void*>(&FkComWrong<VT, int(S)>)))))), ...);
}
template<size_t... S> inline void FillEngine(u32* out, std::index_sequence<S...>) {
    ((out[S] = u32(uintptr_t(reinterpret_cast<void*>(&FkEngineWrong<u32(S * 4)>)))), ...);
    out[0x134 / 4] = u32(uintptr_t(&FkMalloc)); out[0x138 / 4] = u32(uintptr_t(&FkFree)); out[0x13C / 4] = u32(uintptr_t(&FkRealloc));
    out[0x144 / 4] = u32(uintptr_t(&FkListAlloc)); out[0x148 / 4] = u32(uintptr_t(&FkListFree));
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
// Prepara las arenas y las globales del caso; devuelve la llamada. El orden de las escrituras es el del oraculo (fixture_oracle.py).
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gNode = gMalloc = gRealloc = gCreate = 0; gHr1 = gHr2 = 0;
    for (auto& s : gStage) s = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    // globales vigiladas de la funcion
    for (const u32 gi : GlobalsOf(c.fn)) *reinterpret_cast<u32*>(kGlobal[gi]) = gi == 9 ? u32(uintptr_t(gEngine)) : Res(a[48 + gi]);
    auto& A = arenaA; auto& B = arenaB; auto& Cc = arenaC; auto& D = arenaD;
    switch (kKind[c.fn]) {
    case kPush:
        gNode = thisA;
        call.args[0] = Res(a[4]); call.argCount = 1; call.popBytes = 4;
        break;
    case kLDestroy: {
        const u32 L = a[4], base = a[5], sep = a[6];
        for (u32 i = 0; i < L; ++i) B.Put32(base + sep * i + 4, i + 1 < L ? u32(B.Obj(base + sep * (i + 1))) : 0u);
        *reinterpret_cast<u32*>(kGlobal[7]) = (L && a[7] == 0) ? u32(B.Obj(base)) : 0u;
        break;
    }
    case kRelease: {
        const u32 L = a[4], base = a[5], sep = a[6], rbase = a[7], rsep = a[8], E = a[48 + 1];
        PutVtable(2, a[10], 1); PutVtable(2, a[11], 2);
        for (u32 i = 0; i < 6; ++i) {
            D.Put32(a[12] + 0x20 * i, u32(Cc.Obj(a[64 + 8 * i + 6] ? a[11] : a[10])));
            D.Put32(a[12] + 0x20 * i + 0x10, u32(Cc.Obj(a[64 + 8 * i + 7] ? a[11] : a[10])));
        }
        if (IsPS(a[48 + 12]) && (a[48 + 12] >> 28) == 0xD) D.Put32(a[48 + 12] & 0x0FFFFFFFu, u32(Cc.Obj(a[10])));
        for (u32 i = 0; i < L; ++i) {
            B.Put32(base + sep * i, u32(A.Obj(rbase + rsep * i)));
            B.Put32(base + sep * i + 4, i + 1 < L ? u32(B.Obj(base + sep * (i + 1))) : 0u);
        }
        *reinterpret_cast<u32*>(kGlobal[7]) = L ? u32(B.Obj(base)) : 0u;
        for (u32 i = 0; i < L; ++i) {
            const u32 p = 64 + 8 * i, r = rbase + rsep * i, self = u32(A.Obj(r));
            A.Put32(r, a[p] == 0 ? self : (a[p] == 1 ? a[13] : self + 4));
            A.Put8(r + 0x20, a[p + 1]); A.Put8(r + 0x21, a[p + 2]);
            const u32 e = r + E;
            A.Put32(e, a[p + 3] == 0 ? 0u : (a[p + 3] == 1 ? u32(D.Obj(a[12] + 0x20 * i)) : Res(a[48 + 12])));
            A.Put32(e + 0x18, a[p + 4]);
            A.Put32(e + 0x1C, a[p + 5] == 0 ? 0u : u32(D.Obj(a[12] + 0x20 * i + 0x10)));
        }
        break;
    }
    case kRDestroy: {
        const u32 E = a[48 + 1], self = thisA;
        PutVtable(2, a[19], 1);
        D.Put32(a[20], u32(Cc.Obj(a[19])));
        if (IsPS(a[48 + 12]) && (a[48 + 12] >> 28) == 0xD) D.Put32(a[48 + 12] & 0x0FFFFFFFu, u32(Cc.Obj(a[19])));
        Cc.Put32(a[10] + 0x400, a[11]);
        Cc.Put32(a[25] + 0x400, a[26]);
        A.Put32(o, a[5] == 0 ? self : (a[5] == 1 ? a[27] : self + 4));
        A.Put8(o + 0x20, a[6]); A.Put8(o + 0x21, a[7]);
        const u32 e = o + E;
        A.Put32(e, a[8] == 0 ? 0u : (a[8] == 1 ? u32(D.Obj(a[20])) : Res(a[48 + 12])));
        A.Put32(e + 4, a[9] ? u32(Cc.Obj(a[10])) : 0u);
        A.Put32(e + 0x18, a[12]);
        const u32 L = a[14], base = a[15], sep = a[16];
        for (u32 i = 0; i < L; ++i) {
            B.Put32(base + sep * i, i == a[17] ? self : NoPS(Rand(a[18], i)));
            B.Put32(base + sep * i + 4, i + 1 < L ? u32(B.Obj(base + sep * (i + 1))) : 0u);
        }
        *reinterpret_cast<u32*>(kGlobal[7]) = L ? u32(B.Obj(base)) : 0u;
        for (u32 s = 0; s < 8; ++s) gStage[s] = ((a[13] >> s) & 1) ? self : a[24];
        gMalloc = u32(D.Obj(a[22])); gRealloc = u32(D.Obj(a[23]));
        call.args[0] = a[4]; call.args[1] = self; call.args[2] = a[28]; call.argCount = 3; call.popBytes = 12;
        break;
    }
    case kCheck:
    case kBump:
        PutVtable(2, a[7], 1); PutVtable(2, a[8], 2);
        A.Put32(o, u32(Cc.Obj(a[7])));
        D.Put32(a[9], u32(Cc.Obj(a[8])));
        gHr1 = a[5]; gHr2 = a[6];
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kInit:
        gCreate = Res(a[4]);
        break;
    case kShut:
        break;
    }
    gPokeId = a[40]; gPokeIdx = a[41]; gPokeKind = a[42]; gPokeA = a[43]; gPokeB = a[44]; gPokeVal = a[45] ? Res(a[46]) : a[46]; gPokeCount = 0;
    return call;
}

enum RetKind { rNone, rEax };
inline RetKind RetOf(unsigned fn) { return fn >= 3 && fn <= 11 ? rEax : rNone; }

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
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        if (!gThrew && RetOf(fn) == rEax) obs["ret"] = eax; else obs["ret"] = nullptr;
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN224A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN224A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn224a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn224a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn224a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
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
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador con hook RH que la DLL llama POR LA DIRECCION DEL EXE (plugin::Call o puntero a funcion): el falso va solo en la direccion del EXE (el hook queda apagado)
    auto FE = [&](uintptr_t exe, auto* fake) {
        const Pair p = Collab(exe);
        if (!p.hooked) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[1] = {F(0x801B80, &FkFLDestroy)};                                                                       // RwFreeListDestroy
    env.stubs[3] = {FE(0x7FDE50, &FkGetStage), FE(0x7FDCD0, &FkSetStage), F(0x730B70, &FkDestroyTexture)};          // etapas de textura y D3DResourceSystem::DestroyTexture
    env.stubs[12] = {F(0x801980, &FkFLCreate)};                                                                       // RwFreeListCreate
    env.stubs[13] = {F(0x4CB5E0, &FkRasterListDestroy), F(0x801B80, &FkFLDestroy)};                                   // RasterListDestroy (funcion probada) y RwFreeListDestroy
    // RwEngineInstance falsa: se aplica en Setup (global 9); aqui solo se rellena su tabla
    FillEngine(gEngine, std::make_index_sequence<0x60>{});
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella); RwEngineInstance falsa y objetos COM con vtables falsas";
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
    // Globales del juego que el banco fija y vigila: una region de 4 bytes por global; cada funcion declara las suyas (ascendente por direccion)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    for (unsigned gi = 0; gi < 16; ++gi) regions.emplace_back(kGlobal[gi], 4);
    for (unsigned f = 0; f < kFunctions; ++f) for (const u32 gi : GlobalsOf(f)) regionsOf[f].push_back(gi);
    Run(env, ready, registered, bodies, collaborators);
}
}
