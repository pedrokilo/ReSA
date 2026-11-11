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

// Banco par a par Fn161A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 12 funciones del EXE (constructor y liberacion del pool de IPL, GetGTA3ImgSize, los dos constructores y el cuerpo del destructor de CDummyObject, el destructor de CMatrixLinkList, el constructor de CPlaceable, las dos
// CreateInstance de CAtomicModelInfo y los cuerpos de CAtomicModelInfo / CDamageAtomicModelInfo; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall / RH_ScopedGlobalInstall) y V (RH_ScopedVMTOverloadedInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de
// 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de constructor y destructor de CDummy (0x532540, 0x532560), de CBaseModelInfo (0x4C4A60) y
// de CMatrix (0x59ACD0), CEntity::SetModelIndexNoCreate (virtual, 0x533700), AttachToRwObject, DetachFromRwObject, CIplStore::IncludeEntity, CBaseModelInfo::AddRef / RemoveRef y RpAtomicSetFrame (con hook); operator new[] y
// delete[] (0x821195, 0x8213AE), RpAtomicClone (0x749E60) y RwFrameCreate (0x7F0410) (sin hook: la DLL los llama por la direccion del EXE, un solo parche) y GetFileSize (casilla 0x858074 de la IAT del EXE y casillas de la IAT del ASI,
// solo alrededor de la llamada). Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus
// argumentos), el valor devuelto (this o el objeto nuevo como etiqueta de arena; EAX completo en GetGTA3ImgSize) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F161ALive {
using namespace F161AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F161AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos (no es this)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this] (la vtable de la base)
inline std::array<u32, 2> gNewArr{}; // lo que devuelve operator new[] falso, en orden
inline unsigned gNewArrN{};
inline u32 gClone{}, gFrame{};       // lo que devuelven RpAtomicClone y RwFrameCreate falsos
inline u32 gGfsRet{};                // lo que devuelve GetFileSize falso
inline DWORD gBenchThread{};
inline FARPROC gRealGetFileSize{};

// Cuerpo de constructor / destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator new[] del EXE (0x821195; cdecl, un tamano): en el hilo del banco evento [direccion, tamano] y devuelve el siguiente de gNewArr; en otros hilos llama al operator new real (0x82119A, al que salta 0x821195).
inline void* __cdecl FkNewArr(u32 size) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<void*(__cdecl*)(u32)>(0x82119A)(size);
    Ev& e = PushEv(kNewArr, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gNewArr[gNewArrN < 2 ? gNewArrN++ : 1]));
}
// operator delete[] del EXE (0x8213AE; cdecl, un puntero): en el hilo del banco evento [direccion, puntero (etiqueta)] y no libera nada; en otros hilos llama al operator delete real (0x8214BD, al que salta 0x8213AE).
inline void __cdecl FkFree(void* p) {
    if (GetCurrentThreadId() != gBenchThread) { reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(p); return; }
    Ev& e = PushEv(kFreeArr, 1);
    e.v[0] = Tag(p);
}
// GetFileSize (stdcall, manejador y puntero a la palabra alta): en el hilo del banco evento [casilla de la IAT, manejador, puntero (crudo)] y devuelve gGfsRet; en otros hilos llama a la real.
inline DWORD WINAPI FkGetFileSize(HANDLE h, LPDWORD high) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<DWORD(WINAPI*)(HANDLE, LPDWORD)>(gRealGetFileSize)(h, high);
    Ev& e = PushEv(kGfsSlot, 2);
    e.v[0] = u32(uintptr_t(h));
    e.v[1] = u32(uintptr_t(high));
    return gGfsRet;
}
// CBaseModelInfo::AddRef / RemoveRef (thiscall, sin argumentos): evento [direccion, ECX (etiqueta)].
template<u32 ADDR> inline void __fastcall FkThis(void* self, void*) { Ev& e = PushEv(ADDR, 1); e.v[0] = Tag(self); }
// RpAtomicClone (0x749E60; cdecl, un atomic): evento [direccion, atomic (etiqueta)]; devuelve gClone.
inline void* __cdecl FkAtomicClone(void* atomic) { Ev& e = PushEv(kAtomicClone, 1); e.v[0] = Tag(atomic); return reinterpret_cast<void*>(uintptr_t(gClone)); }
// RwFrameCreate (0x7F0410; cdecl, sin argumentos): evento [direccion]; devuelve gFrame.
inline void* __cdecl FkFrameCreate() { PushEv(kFrameCreate, 0); return reinterpret_cast<void*>(uintptr_t(gFrame)); }
// RpAtomicSetFrame (0x74BF20; cdecl, atomic y frame): evento [direccion, atomic (etiqueta), frame (etiqueta)]; devuelve gRet.
inline void* __cdecl FkSetFrame(void* atomic, void* frame) { Ev& e = PushEv(kSetFrame, 2); e.v[0] = Tag(atomic); e.v[1] = Tag(frame); return reinterpret_cast<void*>(uintptr_t(gRet)); }
// CEntity::SetModelIndexNoCreate (0x533700; thiscall, un indice en la pila, `ret 4`): evento [direccion, ECX (etiqueta), indice].
inline void __fastcall FkSetModelIdx(void* self, void*, u32 index) { Ev& e = PushEv(kSetModelIdx, 2); e.v[0] = Tag(self); e.v[1] = index; }
// CEntity::AttachToRwObject (0x533ED0; thiscall, RwObject y bandera en la pila, `ret 8`): evento [direccion, ECX (etiqueta), objeto (etiqueta), bandera (dword completo)].
inline void __fastcall FkAttach(void* self, void*, void* rw, u32 flag) { Ev& e = PushEv(kAttachRw, 3); e.v[0] = Tag(self); e.v[1] = Tag(rw); e.v[2] = flag; }
// CEntity::DetachFromRwObject (0x533FB0; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)].
inline void __fastcall FkDetach(void* self, void*) { Ev& e = PushEv(kDetachRw, 1); e.v[0] = Tag(self); }
// CIplStore::IncludeEntity (0x404C90; cdecl, indice de IPL y entidad): evento [direccion, indice, entidad (etiqueta)].
inline void __cdecl FkInclude(i32 index, void* entity) { Ev& e = PushEv(kIncludeEnt, 2); e.v[0] = u32(index); e.v[1] = Tag(entity); }
template<size_t... I> inline std::array<void*, kFunctions> MakeBodyFakes(std::index_sequence<I...>) { return {reinterpret_cast<void*>(&FkBody<kBodyExe[I]>)...}; }
inline const std::array<void*, kFunctions> kBodyFakes = MakeBodyFakes(std::make_index_sequence<kFunctions>{});

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
};

inline void PutRaw32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline void PutRaw16(uintptr_t at, u16 v) { memcpy(reinterpret_cast<void*>(at), &v, 2); }
inline void PutRaw8(uintptr_t at, u8 v) { *reinterpret_cast<u8*>(at) = v; }

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gNewArr = {}; gNewArrN = 0; gClone = 0; gFrame = 0; gGfsRet = 0;
    const u32 self = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kIplCtor: {   // thiscall con capacidad y nombre en la pila (`ret 8`: lo limpia el callee)
        const u32 p1 = a[5] == 1 ? 0u : (a[5] == 4 ? a[10] : u32(arenaB.Obj(a[7])));
        u32 p2;
        switch (a[5]) {
        case 2: p2 = self + a[9]; break;
        case 3: p2 = u32(arenaA.Obj(a[11])); break;
        case 5: p2 = a[10]; break;
        case 6: p2 = 0; break;
        default: p2 = u32(arenaC.Obj(a[8])); break;
        }
        gNewArr = {p1, p2};
        call.ecx = self;
        call.args = {a[3], a[4], 0, 0, 0, 0, 0, 0}; call.argCount = 2; call.popBytes = 0;
        break;
    }
    case kIplFlush: {
        const u32 stor = (a[8] & 1) ? u32(arenaB.Obj(a[9])) : a[3];
        const u32 sts = (a[8] & 2) ? u32(arenaC.Obj(a[10])) : a[4];
        arenaA.Put32(a[0] + 0x0, stor);
        arenaA.Put32(a[0] + 0x4, sts);
        arenaA.Put32(a[0] + 0x8, a[5]);
        arenaA.Put32(a[0] + 0xC, a[6]);
        arenaA.Put8(a[0] + 0x10, u8(a[7]));
        call.ecx = self;
        break;
    }
    case kImgSize:
        gGfsRet = a[3];
        break;
    case kCtorBody: case kEvBody: case kVtOnly: case kPlCtor: case kMll:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    case kDummyObj: {   // thiscall con el objeto en la pila (`ret 4`)
        arenaA.Put32(a[0], a[3]);
        const u32 obj = a[16] == 0 ? u32(arenaB.Obj(a[6])) : (a[16] == 1 ? self : self + a[17]);
        const u32 rw = a[18] ? u32(arenaC.Obj(a[19])) : a[8];
        PutRaw16(obj + 0x22, u16(a[7]));
        PutRaw32(obj + 0x18, rw);
        PutRaw8(obj + 0x2E, u8(a[9]));
        PutRaw8(obj + 0x2F, u8(a[10]));
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        call.args[0] = obj; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case kCreate: case kCreateM: {   // `ret` (sin argumentos) o `ret 4` (matriz)
        const u32 atomic = a[3] == 0 ? 0u : (a[3] == 1 ? u32(arenaB.Obj(a[4])) : a[5]);
        arenaA.Put32(a[0] + 0x1C, atomic);
        gClone = a[6] == 0 ? u32(arenaB.Obj(a[7])) : a[10];
        const u32 frame = u32(arenaC.Obj(a[8]));
        gFrame = frame; gRet = a[9];
        call.ecx = self;
        if (kKind[c.fn] == kCreateM) {
            u32 matrix;
            switch (a[16]) {
            case 1: matrix = frame + 0x10; break;
            case 2: matrix = frame + 0x10 + 4 * a[18]; break;
            case 3: matrix = frame + 0x10 - 4 * a[18]; break;
            case 4: matrix = u32(arenaA.Obj(a[17])); break;
            case 5: matrix = frame + 0x10 + a[18]; break;
            default: matrix = u32(arenaB.Obj(a[17])); break;
            }
            call.args[0] = matrix; call.argCount = 1; call.popBytes = 0;
        }
        break;
    }
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    if (kKind[fn] == kImgSize) input["handle"] = BitsOf(reinterpret_cast<const void*>(uintptr_t(kHandleSlot)));                             // gStreamFileHandles[0] tal cual esta (no se modifica: lo lee el hilo de streaming)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kImgSize: obs["ret"] = eax; break;                                                            // valor completo de EAX
        case kIplCtor: case kCtorBody: case kDummyObj: case kPlCtor: case kCreate: case kCreateM: obs["ret"] = Tag(uintptr_t(eax)); break;   // this / el objeto nuevo como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN161A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN161A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Colaborador virtual (hook V): el cuerpo de la DLL es la entrada de la vtabla del hook (un puntero a miembro virtual apuntaria a una ranura, no a la funcion)
inline Pair MakeCollabVirtual(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t body = 0;
    if (FindHook(exe, cat, name, &body) && body) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, body));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", true}});
        if (env.collab.back()->Registered()) { p.dll = Body(body); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn161a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn161a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn161a"); }
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
    json hooksFound = json::array(), iatInfo = json::object();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador virtual con hook V (la DLL lo llama de forma directa, calificada, al cuerpo de la clase)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabVirtual(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador sin hook RH: el falso va solo en la direccion del EXE (la DLL lo llama por esa direccion)
    auto X = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    const auto& K = kBodyFakes;
    env.stubs[0] = {X(kNewArr, &FkNewArr)};
    env.stubs[1] = {X(kFreeArr, &FkFree)};
    env.stubs[3] = {F(0x532540, K[3])};
    env.stubs[4] = {F(0x532540, K[4]), FV(kSetModelIdx, &FkSetModelIdx), F(kAttachRw, &FkAttach), F(kDetachRw, &FkDetach), F(kIncludeEnt, &FkInclude)};
    env.stubs[5] = {F(0x532560, K[5])};
    env.stubs[6] = {F(0x59ACD0, K[6])};
    for (unsigned f : {8u, 9u})
        env.stubs[f] = {F(kAddRef, &FkThis<kAddRef>), X(kAtomicClone, &FkAtomicClone), X(kFrameCreate, &FkFrameCreate), F(kSetFrame, &FkSetFrame), F(kRemoveRef, &FkThis<kRemoveRef>)};
    env.stubs[11] = {F(0x4C4A60, K[11])};
    // GetGTA3ImgSize: GetFileSize por la IAT (casilla del EXE 0x858074 y casillas del ASI que apuntan a la GetFileSize real)
    gRealGetFileSize = nullptr;
    {
        const HMODULE mods[2]{GetModuleHandleA("kernel32.dll"), GetModuleHandleA("kernelbase.dll")};
        uintptr_t cand[2]{};
        for (int m = 0; m < 2; ++m) cand[m] = mods[m] ? uintptr_t(GetProcAddress(mods[m], "GetFileSize")) : 0;
        auto isReal = [&](uintptr_t v) { return v && (v == cand[0] || v == cand[1]); };
        auto* exeSlot = reinterpret_cast<uintptr_t*>(kGfsSlot);
        const bool exeOk = isReal(*exeSlot);
        if (exeOk) gRealGetFileSize = reinterpret_cast<FARPROC>(*exeSlot);
        std::vector<uintptr_t*> asiSlots;
        auto* base = reinterpret_cast<uint8*>(env.asi);
        auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
        const auto& dirImp = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
        if (dirImp.VirtualAddress)
            for (auto* d = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + dirImp.VirtualAddress); d->Name; ++d)
                for (auto* s = reinterpret_cast<uintptr_t*>(base + d->FirstThunk); *s; ++s)
                    if (isReal(*s)) asiSlots.push_back(s);
        if (!exeOk || asiSlots.empty() || asiSlots.size() > 3) ready = false;
        env.iat[2].push_back({exeSlot, reinterpret_cast<void*>(&FkGetFileSize)});
        for (auto* s : asiSlots) env.iat[2].push_back({s, reinterpret_cast<void*>(&FkGetFileSize)});
        iatInfo = {{"exe_slot_ok", exeOk}, {"asi_slots", asiSlots.size()}};
    }
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {7u, 10u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[0] = "toda la funcion salvo operator new[] (0x821195, sin hook: la DLL lo llama por esa direccion, un solo parche)";
    dllReal[1] = "toda la funcion salvo operator delete[] (0x8213AE, sin hook: la DLL lo llama por esa direccion, un solo parche)";
    dllReal[2] = "toda la funcion (GetFileSize es un falso en la casilla de la IAT del EXE y en las casillas de la IAT del ASI, solo alrededor de la llamada)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        for (const auto& r : env.reals[f]) {
            env.watches.emplace_back(r.exe, 16);
            real.push_back({{"exe", r.exe}, {"dll", r.dll != 0}, {"hooked", r.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", f == 2 ? iatInfo : json(nullptr)}};
    }
    // Sin memoria global del juego: todo ocurre en las tres arenas propias (GetGTA3ImgSize lee gStreamFileHandles[0] sin modificarlo)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
