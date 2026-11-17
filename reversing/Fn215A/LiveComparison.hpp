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
#include "X87Math.h"

// Banco par a par Fn215A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 22 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedGlobalInstall / RH_ScopedInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40).
// Colaboradores: los que tienen hook RH en la DLL (apagado durante el banco) llevan el MISMO falso en la direccion del EXE y en el cuerpo de la DLL (F): cuerpo del destructor 0x4CF100, ConvertBoneTag2BoneName 0x4D56F0,
// GetFrameNodeName 0x72FB30, GetUppercaseKey 0x53CF30, GetFrameHierarchyId 0x732A20 y FxMemoryPool_c::GetMem 0x4A9CA0; operator delete 0x8214BD (la DLL lo llama por la direccion del EXE) lleva el falso solo ahi;
// _stricmp 0x8229B6 lleva el falso en la direccion del EXE y en el _stricmp del CRT estatico de la DLL (D). Los constructores del interp (0x4A8990, 0x4A8440, 0x4A87D0) corren reales con su hook apagado.
// Se observan: los bytes cambiados de las cuatro arenas, los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de arena), las dos vtables que escriben los constructores
// de FxInfo*_c (normalizadas: la del EXE contra la esperada y la de la DLL contra "esta en el ASI"), la variable global 0xB5F87C y el estado x87 (siempre vacio).
// EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Diagnostico de arranque: si el juego muere con una violacion de acceso ANTES de que empiece el banco, escribe la direccion y la pila en fn215a-crash.txt (junto al EXE).
namespace F215ACrash {
inline LONG WINAPI Handler(EXCEPTION_POINTERS* p) {
    if (p->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION) return EXCEPTION_CONTINUE_SEARCH;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH);
    std::string path = std::string(exe); path = path.substr(0, path.find_last_of("\\/") + 1) + "fn215a-crash.txt";
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

namespace F215ALive {
using namespace F215AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F215AFix::Arena(WIN);
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

// ================================================================================================ falsos de los colaboradores
inline u32 gNameRet{};               // lo que devuelven los falsos de ConvertBoneTag2BoneName (0x4D56F0) y GetFrameNodeName (0x72FB30)
inline u32 gCmpRet{};                // lo que devuelve el falso de _stricmp (0x8229B6)
inline u32 gKeyRet{};                // lo que devuelve el falso de GetUppercaseKey (0x53CF30)
inline u32 gIdxRet{};                // lo que devuelve el falso de GetFrameHierarchyId (0x732A20)
inline u32 gMemRet{};                // lo que devuelve el falso de FxMemoryPool_c::GetMem (0x4A9CA0)
inline DWORD gBenchThread{};

inline void DumpTag(Ev& e, unsigned i, uintptr_t v) { e.v[i] = Tag(v); }

// ConvertBoneTag2BoneName (0x4D56F0, cdecl (tag de 32 bits)): evento [direccion, tag]; devuelve gNameRet
inline u32 __cdecl FkBoneName(u32 tag) {
    Ev& e = PushEv(0x4D56F0, 1);
    e.v[0] = tag;
    return gNameRet;
}
// GetFrameNodeName (0x72FB30, cdecl (frame)): evento [direccion, frame (etiqueta)]; devuelve gNameRet
inline u32 __cdecl FkNodeName(void* frame) {
    Ev& e = PushEv(0x72FB30, 1);
    DumpTag(e, 0, uintptr_t(frame));
    return gNameRet;
}
// _stricmp (0x8229B6, cdecl (a, b)): evento [direccion, a, b] (punteros crudos: no se desreferencian); devuelve gCmpRet
inline u32 __cdecl FkStricmp(const char* a, const char* b) {
    Ev& e = PushEv(0x8229B6, 2);
    DumpTag(e, 0, uintptr_t(a)); DumpTag(e, 1, uintptr_t(b));
    return gCmpRet;
}
// CKeyGen::GetUppercaseKey (0x53CF30, cdecl (cadena)): evento [direccion, cadena]; devuelve gKeyRet
inline u32 __cdecl FkUpperKey(const char* s) {
    Ev& e = PushEv(0x53CF30, 1);
    DumpTag(e, 0, uintptr_t(s));
    return gKeyRet;
}
// CVisibilityPlugins::GetFrameHierarchyId (0x732A20, cdecl (frame)): evento [direccion, frame]; devuelve gIdxRet
inline u32 __cdecl FkHierId(void* frame) {
    Ev& e = PushEv(0x732A20, 1);
    DumpTag(e, 0, uintptr_t(frame));
    return gIdxRet;
}
// FxMemoryPool_c::GetMem (0x4A9CA0, thiscall (tamano, alineacion), `ret 8`): evento [direccion, ECX, tamano, alineacion]; devuelve gMemRet
inline u32 __fastcall FkGetMem(void* self, void*, u32 size, u32 align) {
    Ev& e = PushEv(0x4A9CA0, 3);
    DumpTag(e, 0, uintptr_t(self)); e.v[1] = size; e.v[2] = align;
    return gMemRet;
}
// Funcion thiscall sin argumentos que no toca memoria (cuerpo del destructor 0x4CF100): evento [direccion, ECX]
template<u32 ADDR> inline void __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(self));
}
// operator delete (0x8214BD): cdecl, un puntero CRUDO (no se desreferencia); evento [direccion, puntero (etiqueta)].
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 1);
    DumpTag(e, 0, uintptr_t(p));
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

// Memoria global del juego que el banco toca (se registra en Update): gpAnimBlendFrameData (0xB5F87C, un dword)
inline constexpr uintptr_t kFrameGlobalAddr = 0xB5F87C;

// Puntero de un triple (modo, desplazamiento, crudo): 0 nulo, 1 crudo, 2 dentro de la arena `ar` (el desplazamiento puede ser negativo)
template<size_t W> inline u32 PtrOf(u32 mode, u32 off, u32 raw, const Arena<W>& ar) {
    return mode == 0 ? 0u : (mode == 1 ? raw : u32(ar.Obj(size_t(i32(off)))));
}

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call;
    const auto& a = c.a;
    arenaA.Pattern(a[2], c.sd[0]); arenaB.Pattern(a[2], c.sd[1]); arenaC.Pattern(a[2], c.sd[2]); arenaD.Pattern(a[2], c.sd[3]);
    gNameRet = gCmpRet = gKeyRet = gIdxRet = gMemRet = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    const Kind k = kKind[c.fn];
    switch (k) {
    case kDel:
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 0;
        break;
    case kNameSkin: case kNameNon: case kKeySkin: case kKeyNon: case kFill: {
        arenaA.Put32(o + 0x14, a[4]);
        arenaA.Put32(o + 0x10, PtrOf(a[5], a[6], a[7], arenaD));
        gNameRet = PtrOf(a[8], a[9], a[10], arenaC);
        gCmpRet = a[14]; gKeyRet = a[15]; gIdxRet = a[14];
        u32 data = PtrOf(a[11], a[12], a[13], arenaB);
        if ((k == kKeySkin || k == kKeyNon) && a[16]) data = a[15];
        if (k != kFill) *reinterpret_cast<u32*>(kFrameGlobalAddr) = a[17];
        call.ecx = c.ecx;
        call.args[0] = thisA; call.args[1] = data; call.argCount = 2; call.popBytes = 8;   // cdecl
        break;
    }
    case kSwitch:
        for (u32 i = 0; i < a[6] && i < 8; ++i) arenaA.Put8(o + a[7 + i] * 0x14 + 3, a[4]);
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2; call.popBytes = 0;
        break;
    case kAddBrain:
        for (u32 i = 0; i < 70; ++i) arenaA.Put16(o + i * 0x14, a[16 + i]);
        for (u32 i = 0; i < 6; ++i) call.args[i] = a[4 + i];
        call.argCount = 6; call.popBytes = 0;
        break;
    case kAddStr: {
        for (u32 i = 0; i < 70; ++i) arenaA.Put16(o + i * 0x14, a[16 + i]);
        u32 str;
        if (a[5] == 3) {
            const u32 at = o + a[6];
            for (u32 j = 0; j < a[7]; ++j) arenaA.Put8(at + j, StrChar(c.seed, j));
            arenaA.Put8(at + a[7], 0);
            str = u32(arenaA.Obj(at));
        } else {
            for (u32 j = 0; j < a[7]; ++j) arenaB.Put8(a[6] + j, StrChar(c.seed, j));
            arenaB.Put8(a[6] + a[7], 0);
            str = u32(arenaB.Obj(a[6]));
        }
        call.args[0] = a[4]; call.args[1] = str; call.args[2] = a[8]; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kFx:
        gMemRet = PtrOf(a[4], a[5], a[6], arenaD);
        call.argCount = 0; call.popBytes = 0;
        break;
    }
    return call;
}

enum RetKind { rNone, rTag };
inline RetKind RetOf(Kind k) { return (k == kDel || k == kFx) ? rTag : rNone; }

// Las vtables que escriben los constructores de FxInfo*: el EXE deja las del EXE (en +0 y +8) y la DLL las suyas (en el ASI); se normalizan a 1 si son las esperadas y se sacan de los cambios
inline void NormalizeVtables(Env& env, unsigned fn, bool candidate, const C& c, json& obs) {
    const u32 o = c.a[0];
    const unsigned fi = fn - 9;
    const u32 v0 = arenaA.Get32(o), v8 = arenaA.Get32(o + 8);
    const auto norm = [&](u32 v, u32 exeVt) -> u32 {
        if (candidate) return ModuleOf(v) == env.asi ? 1u : v;
        return v == exeVt ? 1u : v;
    };
    obs["vt"] = json::array({norm(v0, kFxVt[fi]), norm(v8, kFxInterpVt[fi])});
    memcpy(arenaA.m.data() + kGuard + o, arenaA.r.data() + kGuard + o, 4);
    memcpy(arenaA.m.data() + kGuard + o + 8, arenaA.r.data() + kGuard + o + 8, 4);
}

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
        if (kKind[fn] == kFx) NormalizeVtables(env, fn, candidate, c, obs);
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        obs["threw"] = gThrew;
        const u32 eax = sides[s].reg.eax;
        switch (gThrew ? rNone : RetOf(kKind[fn])) {
        case rTag: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN215A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN215A_CASES", buffer, sizeof(buffer)) > 0) {
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
    // Cada direccion debe saltar a SU cuerpo: ningun par de funciones probadas comparte cuerpo en la DLL (RH no admite dos hooks hacia la misma funcion)
    bool distinct = true;
    for (unsigned f = 0; f < kFunctions; ++f)
        for (unsigned g = f + 1; g < kFunctions; ++g)
            if (env.route[f] && env.route[g] && Body(env.route[f]->Body()) == Body(env.route[g]->Body())) distinct = false;
    ready &= distinct;
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"distinct_bodies", distinct}, {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
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
// Direccion de un miembro no virtual (4 bytes en x86), resuelta hasta su cuerpo
template<class F> inline uintptr_t MemberBody(F f) { return Body(std::bit_cast<uintptr_t>(f)); }

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn215a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn215a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn215a"); }
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
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete 0x8214BD): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH cuya llamada, en la DLL, va a una funcion propia (direccion dada) y no a la del EXE: el falso va en la direccion del EXE y en esa funcion (_stricmp del CRT estatico de la DLL)
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador CON hook RH que corre REAL (hook apagado en ambos lados: el EXE ejecuta sus bytes y la DLL su cuerpo)
    auto Real = [&](uintptr_t exe) {
        const Pair real = MakeCollab(env, exe, hooksFound);
        if (!real.hooked || !real.dll) ready = false;
        return real;
    };
    const uintptr_t stricmpBody = std::bit_cast<uintptr_t>(static_cast<int(__cdecl*)(const char*, const char*)>(&_stricmp));
    env.stubs[0] = {F(0x4CF100, &FkThis<0x4CF100>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[1] = {F(0x4D56F0, &FkBoneName), D(0x8229B6, stricmpBody, &FkStricmp)};
    env.stubs[2] = {F(0x72FB30, &FkNodeName), D(0x8229B6, stricmpBody, &FkStricmp)};
    env.stubs[3] = {F(0x4D56F0, &FkBoneName), F(0x53CF30, &FkUpperKey)};
    env.stubs[4] = {F(0x72FB30, &FkNodeName), F(0x53CF30, &FkUpperKey)};
    env.stubs[5] = {F(0x732A20, &FkHierId)};
    // constructores de FxInfo*: GetMem falso; los constructores del interp (0x4A8990 de 32, 0x4A8440 de Float, 0x4A87D0 de U255) corren REALES con su hook apagado
    const uintptr_t interpCtor[13] = {0x4A8990, 0x4A8990, 0x4A8990, 0x4A8440, 0x4A87D0, 0x4A8440, 0x4A8440, 0x4A8440, 0x4A8990, 0x4A8990, 0x4A8990, 0x4A8990, 0x4A8990};
    for (unsigned f = 9; f < kFunctions; ++f) {
        env.stubs[f] = {F(0x4A9CA0, &FkGetMem)};
        env.reals[f] = {Real(interpCtor[f - 9])};
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en el cuerpo de la DLL, o solo en la direccion del EXE si la DLL lo llama por ella)";
    dllReal[0] = "toda la funcion salvo el cuerpo del destructor 0x4CF100 (falso identico) y operator delete (0x8214BD, falso solo en la direccion del EXE: la DLL lo llama por ella)";
    for (unsigned f : {1u, 2u}) dllReal[f] = "toda la funcion salvo el nombre del hueso/frame (falso identico) y _stricmp (falso en 0x8229B6 del EXE y en el _stricmp del CRT estatico de la DLL)";
    for (unsigned f : {6u, 7u, 8u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f = 9; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo FxMemoryPool_c::GetMem (falso identico); el constructor del interp corre real (hook apagado; el EXE ejecuta sus bytes y la DLL su constructor)";
    // Memoria global del juego tocada: gpAnimBlendFrameData (0xB5F87C); se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kFrameGlobalAddr, 4);
    for (unsigned f = 1; f <= 4; ++f) regionsOf[f] = {0};
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"dll_fakes", json::array()}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    Run(env, ready, registered, bodies, collaborators);
}
}
