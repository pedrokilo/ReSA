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

// Banco par a par Fn153A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 24 funciones del EXE (constantes virtuales de CEventStuckInAir, CTaskGangHasslePed y CColStore; constructor y destructor de CRideAnimData; constructor de CAETwinLoopSoundEntity; CTrailer::IsBaggageTrailer; CVector::Sum;
// CColDisk::Set; volumen del motor de CAEVehicleAudioEntity; liberacion del pool de eventos; dos IsValid; seis destructores con borrado; tres cuerpos de destructor; ver Fixtures.hpp), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40). Falsos de los
// colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (con hook propio, `H`: el hook se apaga y el falso va en la envoltura de la DLL; sin hook, `U`:
// funcion propia de la DLL; con hook y funcion propia, `F`: el cuerpo del hook; funcion probada del lote, `Rt`), CTask::operator delete 0x61A5B0 (con hook), operator delete del CRT 0x8214BD (sin hook, la DLL lo llama
// por esa direccion), CMemoryMgr::FreeAlign 0x72F4F0, CAnimManager::RemoveAnimBlockRef 0x4D3FD0, CEntity::CleanUpOldReference 0x571A00, ReleasePedConversation 0x4E52A0, ~CTaskComplex 0x61A3D0,
// ~CTaskComplexPartner 0x683A40, CPed::IsAlive 0x5E0170 y el GetLifeTime virtual (vtable falsa en la arena C). La liberacion del pool de eventos es EN LINEA: el banco apunta 0xB744AC a un pool falso de la arena B.
// Se observan: los bytes cambiados de las tres arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto
// (this o la salida como etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Funciones propias de la DLL (envolturas) que exportan los .cpp de cada clase para que el banco las sustituya.
uintptr_t Lote153ABody_CTaskComplexChat();                  // 0x682D40 (con hook)
uintptr_t Lote153ABody_CTaskComplexUseWaterCannon();        // 0x6597B0 (sin hook)
uintptr_t Lote153ABody_CTaskComplexStareAtPed();            // 0x660310 (con hook)
uintptr_t Lote153ABody_CTaskSimpleDoHandSignal();           // 0x6608B0 (con hook)

namespace F153ALive {
using namespace F153AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F153AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
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
inline u32 gLife{};                  // valor que devuelve el GetLifeTime falso
inline DWORD gBenchThread{};

// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Igual (para 0x4AC480), pero conserva ECX: el EXE de 0x4B5670 sigue usando ECX = this tras esa llamada (`mov [ecx],85AB38h; ret`). Envoltura sin marco que guarda y restaura ECX alrededor del falso.
inline u32 __fastcall FkBodyForEcx(void* self, void*) { return FkBody<0x4AC480>(self, nullptr); }
__declspec(naked) inline void FkBodyKeepEcx() {
    __asm {
        push ecx
        xor edx, edx
        call FkBodyForEcx
        pop ecx
        ret
    }
}
// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CMemoryMgr::FreeAlign (0x72F4F0; cdecl, un puntero): evento [direccion, puntero (etiqueta)]. No lee ni libera nada.
inline void __cdecl FkFreeAlign(void* p) { Ev& e = PushEv(0x72F4F0, 1); e.v[0] = Tag(p); }
// CAnimManager::RemoveAnimBlockRef (0x4D3FD0; cdecl, un id): evento [direccion, id]; devuelve gRet.
inline u32 __cdecl FkRemoveRef(u32 id) { Ev& e = PushEv(0x4D3FD0, 1); e.v[0] = id; return gRet; }
// CEntity::CleanUpOldReference (0x571A00; thiscall, un puntero en la pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No toca la referencia.
inline void __fastcall FkCleanUp(void* self, void*, void* ref) { Ev& e = PushEv(0x571A00, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
// CAEPedSpeechAudioEntity::ReleasePedConversation (0x4E52A0; cdecl, sin argumentos): evento [direccion].
inline void __cdecl FkReleaseConv() { PushEv(0x4E52A0, 0); }
// CPed::IsAlive (0x5E0170; thiscall, sin argumentos de pila, `ret`): evento [direccion, ECX (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkIsAlive(void* self, void*) { Ev& e = PushEv(0x5E0170, 1); e.v[0] = Tag(self); return gRet; }
// GetLifeTime falso (ranura 3 de la vtable falsa de la arena C): evento [identificador, ECX (etiqueta)]; devuelve gLife.
inline u32 __fastcall FkLife(void* self, void*) { Ev& e = PushEv(kEvLife, 1); e.v[0] = Tag(self); return gLife; }
inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
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

// Pool falso de eventos (arena B, a[pool]): +0 almacenamiento = obj - d, +4 estados (la celda de estados[indice] cae en la arena C), +8 relleno, +0xC cursor; 0xB744AC apunta a el.
inline void PutPool(u32 obj, u32 d, u32 cursor, u32 cellC, u32 poolB, u32 fill) {
    const i32 index = i32(d) / 68;
    arenaB.Put32(poolB + 0x0, obj - d);
    arenaB.Put32(poolB + 0x4, u32(arenaC.Obj(cellC)) - u32(index));
    arenaB.Put32(poolB + 0x8, fill);
    arenaB.Put32(poolB + 0xC, cursor);
    PutGlobal32(kEventPoolPtr, u32(arenaB.Obj(poolB)));
}

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gLife = 0;
    const u32 self = u32(arenaA.Obj(a[0]));
    switch (kKind[c.fn]) {
    case kConst: case kConstAL: case kNoop: case kRideCtor: case kTwinCtor:
        arenaA.Put32(a[0], a[3]);
        call.ecx = self;
        break;
    case kTrailer:
        arenaA.Put32(a[0] + 0x9EC, a[3]);
        call.ecx = self;
        break;
    case kSum: {   // thiscall(out) con dos punteros en la pila (`ret 8`)
        for (unsigned i = 0; i < 3; ++i) { arenaA.Put32(a[0] + 4 * i, a[3 + i]); arenaC.Put32(a[16] + 4 * i, a[6 + i]); }
        const u32 left = self, right = (a[9] == 6 || a[9] == 7) ? left : u32(arenaC.Obj(a[16]));
        const u32 outs[8]{u32(arenaB.Obj(a[10])), left, right, left + 4, left + 8, right + 4, u32(arenaB.Obj(a[10])), left};
        call.ecx = outs[a[9]];
        call.args[0] = left; call.args[1] = right; call.argCount = 2; call.popBytes = 0;
        break;
    }
    case kDiskSet: {   // thiscall con siete dwords en la pila (`ret 1Ch`)
        const u32 start = a[8] == 0 ? u32(arenaB.Obj(a[10])) : self + kAlias[a[8] - 1];
        const u32 end = a[9] == 0 ? u32(arenaC.Obj(a[11])) : self + kAlias[a[9] - 1];
        call.ecx = self;
        call.args = {a[3], start, end, a[4], a[5], a[6], a[7], 0}; call.argCount = 7; call.popBytes = 0;
        break;
    }
    case kEngine: {
        const i32 st = int16(a[3] & 0xFFFF);
        arenaA.Put32(a[0] + 0xD8, a[6]);
        arenaA.Put32(a[0] + 0xE8 + 8 * st, a[7] ? u32(arenaB.Obj(a[8])) : 0u);
        call.ecx = self;
        call.args = {a[3], a[4], a[5], 0, 0, 0, 0, 0}; call.argCount = 3; call.popBytes = 0;
        break;
    }
    case kPoolDelete:   // cdecl(objeto): el caller limpia
        PutPool(self, a[3], a[4], a[5], a[6], a[7]);
        call.args[0] = self; call.argCount = 1; call.popBytes = 4;
        break;
    case kIsValid:      // this con vtable falsa (ranura 3 = GetLifeTime falso) y un ped opcional
        arenaA.Put32(a[0] + 0, u32(arenaC.Obj(a[9])));
        arenaA.Put32(a[0] + 4, a[4]);
        arenaA.Put32(a[0] + 8, a[5]);
        arenaC.Put32(a[9] + 12, u32(uintptr_t(&FkLife)));
        gLife = a[3]; gRet = a[8];
        call.ecx = self;
        call.args[0] = a[6] ? u32(arenaB.Obj(a[7])) : 0u; call.argCount = 1; call.popBytes = 0;
        break;
    case kSdd: case kSddPool:   // thiscall con banderas en la pila (`ret 4`: lo limpia el callee)
        arenaA.Put32(a[0], a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        call.args[0] = a[6]; call.argCount = 1; call.popBytes = 0;
        if (kKind[c.fn] == kSddPool) PutPool(self, a[7], a[8], a[9], a[10], a[11]);
        break;
    case kAnimBody: {
        const u32 outs[4]{0u, 0u, self, self + 4};
        const u32 next = a[7] == 1 ? u32(arenaC.Obj(a[8])) : outs[a[7]], prev = a[9] == 1 ? u32(arenaC.Obj(a[10])) : outs[a[9]];
        const u32 hier = a[16] ? self : u32(arenaB.Obj(a[17]));
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 4, next);
        arenaA.Put32(a[0] + 8, prev);
        arenaA.Put32(a[0] + 0x10, a[5] ? u32(arenaB.Obj(a[6])) : 0u);
        arenaA.Put32(a[0] + 0x14, hier);
        arenaA.Put32(a[0] + 0x2C, a[11]);
        (a[16] ? arenaA : arenaB).Put32((a[16] ? a[0] : a[17]) + 0xC, a[18]);
        gRet = a[4];
        call.ecx = self;
        break;
    }
    case kSwatBody:
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0x14, a[6]);
        arenaA.Put32(a[0] + 0x18, a[7] ? u32(arenaB.Obj(a[8])) : 0u);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    case kPartnerBody:
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0x74, a[6]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});   // direcciones reales de las arenas (para el oraculo)
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
        case kConst: obs["ret"] = eax; break;                                                              // valor completo de EAX
        case kConstAL: case kTrailer: case kIsValid: obs["ret"] = eax & 0xFFu; break;                      // bool: solo AL
        case kRideCtor: case kTwinCtor: case kSdd: case kSddPool: obs["ret"] = Tag(uintptr_t(eax)); break; // this como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN153A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN153A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn153a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn153a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn153a"); }
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
    // Colaborador CON hook RH cuya llamada, en la funcion probada, va a una funcion propia de la DLL distinta de la envoltura del hook (direccion dada): el hook se apaga (ruta) y el falso va en la
    // direccion del EXE y en esa funcion.
    auto H = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        Pair p = it->second;
        p.dll = Body(dllAddress); p.hooked = true;
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete del CRT): un solo parche, en el EXE.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    const auto& K = kBodyFakes;
    env.stubs[15] = {Rt(21, K[15]), E(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[16] = {H(0x682D40, Lote153ABody_CTaskComplexChat(), K[16]), Del()};
    env.stubs[17] = {U(0x6597B0, Lote153ABody_CTaskComplexUseWaterCannon(), K[17]), Del()};
    env.stubs[18] = {H(0x660310, Lote153ABody_CTaskComplexStareAtPed(), K[18]), Del()};
    env.stubs[19] = {H(0x6608B0, Lote153ABody_CTaskSimpleDoHandSignal(), K[19]), Del()};
    env.stubs[20] = {F(0x4AC480, &FkBodyKeepEcx)};
    env.stubs[13] = {F(0x5E0170, &FkIsAlive)};
    env.stubs[14] = {F(0x5E0170, &FkIsAlive)};
    env.stubs[21] = {F(0x72F4F0, &FkFreeAlign), F(0x4D3FD0, &FkRemoveRef)};
    env.stubs[22] = {F(0x571A00, &FkCleanUp), F(0x61A3D0, K[22])};
    env.stubs[23] = {F(0x4E52A0, &FkReleaseConv), F(0x683A40, K[23])};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 10u, 12u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[9] = "toda la funcion (la aritmetica x87 vive en Vector.cpp, compilado con /arch:IA32)";
    dllReal[11] = "toda la funcion (la suma x87 vive en X87Math.cpp: FloatSumStoreCpp)";
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    // Memoria global del juego: solo el puntero del pool de eventos (0xB744AC), que apuntan al pool falso de la arena B en las funciones 12 y 20; el resto ocurre en las tres arenas propias
    regions.clear();
    regions.emplace_back(kEventPoolPtr, 4);
    for (auto& v : regionsOf) v.clear();
    regionsOf[12] = {0};
    regionsOf[20] = {0};
    Run(env, ready, registered, bodies, collaborators);
}
}
