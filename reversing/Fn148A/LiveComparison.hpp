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

// Banco par a par Fn148A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 20 funciones diminutas del EXE (ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedInstall / RH_ScopedNamedGlobalInstall) o
// V (RH_ScopedVMTInstall: ranura de la vtable del EXE; el Virtual lleva dentro un Simple sobre la entrada del EXE, que es lo que el banco enciende). El banco invoca la direccion del EXE con objetos
// sinteticos en tres arenas propias (A, B, C con guardas de 0x40). Falsos de los colaboradores: el mismo falso en el EXE y en el cuerpo de la DLL (MakeCollab / Pair): CAESound::Destructor (0x4EF660), SetTask
// (0x5F7540), ReportAllTasksFinished (0x5F7730), RemoveMember (0x5F80D0), ~CTaskComplexCarDrive (0x63CA40), cuerpos de destructor (0x63CCC0, 0x63CE70, 0x5F7FD0), ~CTaskComplex (0x61A3D0) y CTask::operator
// delete (0x61A5B0), todos con hook (apagado durante el banco); el cuerpo 0x678D50 tiene hook desde el lote 152A (falso en su cuerpo, F) y 0x678EE0 no lo tiene: falso en la direccion del EXE y en la funcion propia de la DLL (Lote148ABody_*); operator delete[]
// 0x8213AE (la DLL lo llama por la direccion del EXE: falso solo ahi).
// Se observan: los bytes cambiados de las tres arenas (y, en CLinkList::Insert, los dwords cambiados con los punteros a arena como etiquetas), los eventos de los falsos (con sus argumentos), el valor devuelto
// (EAX completo, etiqueta de this, o AL) y el estado x87 (vacio) y el contrato de ABI. EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

uintptr_t Lote148ABody_CTaskSimpleLand();    // 0x678EE0 (sin hook), TaskSimpleLand.cpp

namespace F148ALive {
using namespace F148AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x4A9F20, 0x4A9F40, 0x4A9F50, 0x4A9F60, 0x4A9F70, 0x5F8580, 0x5F8750, 0x63CB50, 0x643260, 0x643280, 0x5F7FB0, 0x5FB990, 0x5FE960,
                                                  0x5FE9C0, 0x4161D0, 0x4161F0, 0x678CB0, 0x678CC0, 0x67D340, 0x67D360};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F148AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    // Cambios por dwords alineados (CLinkList::Insert): [desplazamiento respecto a la ventana, dword nuevo; un puntero a una arena se expresa como etiqueta]
    json WChanges() const {
        json out = json::array();
        for (size_t i = 0; i + 4 <= N; i += 4) {
            u32 x, y; memcpy(&x, m.data() + i, 4); memcpy(&y, r.data() + i, 4);
            if (x != y) out.push_back(json::array({int64(i) - int64(kGuard), Tag(uintptr_t(x))}));
        }
        return out;
    }
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
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline DWORD gBenchThread{};
inline const u8* gLinksField{};      // CLinkList::Shutdown: direccion de [this + 0x30] (el falso de delete[] lee su valor al entrar)
inline u32 gRmAt{}, gRmSlot{}, gRmVal{}, gRmCalls{};   // CPedGroupMembership::Destructor: el falso de RemoveMember escribe gRmVal en el hueco gRmSlot tras su llamada numero gRmAt

// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator delete[] (cdecl, un argumento CRUDO que puede no apuntar a memoria valida): evento [direccion del EXE, argumento (etiqueta o crudo), dword de [this + 0x30] de la lista al entrar (aun el puntero viejo)].
inline void __cdecl FkDeleteArray(void* p) {
    Ev& e = PushEv(0x8213AE, 2);
    e.v[0] = Tag(p); e.v[1] = gLinksField ? BitsOf(gLinksField) : 0u;
}
// CAESound::Destructor (0x4EF660, thiscall sin pila) sobre m_tempSound (this + 8): evento [direccion, ECX (etiqueta), dword de [ECX - 8] al entrar (la vtable que dejo el cuerpo)]; devuelve gRet.
inline u32 __fastcall FkSoundDtor(void* self, void*) {
    Ev& e = PushEv(0x4EF660, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(static_cast<u8*>(self) - 8);
    return gRet;
}
// CPedGroupIntelligence::SetTask (0x5F7540, thiscall, 5 argumentos de pila, `ret 14h`): evento [direccion, ECX, ped, tarea, tabla de pares, ranura (dword), forzar (byte bajo)]; devuelve gRet. No desreferencia nada.
inline u32 __fastcall FkSetTask(void* self, void*, void* ped, void* task, void* pairs, u32 slot, u32 force) {
    Ev& e = PushEv(0x5F7540, 6);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = Tag(task); e.v[3] = Tag(pairs); e.v[4] = slot; e.v[5] = force & 0xFFu;
    return gRet;
}
// CPedGroupIntelligence::ReportAllTasksFinished (0x5F7730, thiscall, un argumento de pila, `ret 4`): evento [direccion, ECX, tabla de pares]; devuelve gRet.
inline u32 __fastcall FkReportAll(void* self, void*, void* pairs) {
    Ev& e = PushEv(0x5F7730, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(pairs);
    return gRet;
}
// CPedGroupMembership::RemoveMember (0x5F80D0, thiscall, un argumento de pila, `ret 4`): evento [direccion, ECX, hueco]; pone a cero el hueco (this + 4 + 4*hueco, como el real) y, en la llamada numero gRmAt,
// escribe ademas gRmVal en el hueco gRmSlot (prueba si quien llama relee cada hueco); devuelve gRet.
inline u32 __fastcall FkRemoveMember(void* self, void*, u32 idx) {
    Ev& e = PushEv(0x5F80D0, 2);
    e.v[0] = Tag(self); e.v[1] = idx;
    auto* const b = static_cast<u8*>(self);
    const u32 zero = 0;
    memcpy(b + 4 + 4 * (idx & 7), &zero, 4);
    if (gRmCalls == gRmAt) memcpy(b + 4 + 4 * (gRmSlot & 7), &gRmVal, 4);
    ++gRmCalls;
    return gRet;
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
    std::array<u32, 4> args{};
    u32 argCount{}, popBytes{}, ecx{};
};

// Prepara arenas del caso; devuelve la llamada
inline void PutAt(uintptr_t addr, u32 v) { memcpy(reinterpret_cast<void*>(addr), &v, 4); }
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[16]); arenaB.Pattern(a[2], a[17]); arenaC.Pattern(a[2], a[18]);
    auto thisCall = [&](u32 argCount) {
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = argCount; call.popBytes = 0;   // `ret n` del callee: lo limpia el EXE o la DLL
    };
    if (IsSdd(c.fn)) {                // thiscall con un argumento (`ret 4`): banderas
        arenaA.Put32(a[0], a[3]);
        gDtorWrite = a[5]; gRet = a[6];
        thisCall(1); call.args[0] = a[4];
        return call;
    }
    switch (c.fn) {
    case kIdxFxPrimCtor: case kIdxFxPrimDtor:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 4, a[4]); arenaA.Put32(a[0] + 8, a[5]); arenaA.Put32(a[0] + 0xC, a[6]);
        thisCall(0);
        break;
    case kIdxFxPrimEnable:
        arenaA.Put32(a[0] + 0xC, a[3]);
        thisCall(1); call.args[0] = a[4];
        break;
    case kIdxAudioDtor:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4];
        thisCall(0);
        break;
    case kIdxFxSphereCtor:
        arenaA.Put32(a[0] + 0x10, a[3]); arenaA.Put32(a[0] + 0xC, a[4]);
        thisCall(0);
        break;
    case kIdxSetDefaultTask: {
        const u32 ped = a[3] == 0 ? a[4] : u32(arenaC.Obj(a[5]));
        const u32 task = a[6] == 0 ? a[7] : u32(arenaB.Obj(a[8]));
        gRet = a[9];
        thisCall(2); call.args[0] = ped; call.args[1] = task;
        break;
    }
    case kIdxReportBarDefault:
        gRet = a[4];
        thisCall(0);
        break;
    case kIdxWanderDtor: case kIdxInAirLandDtor:
        arenaA.Put32(a[0], a[3]);
        gDtorWrite = a[4]; gRet = a[5];
        thisCall(0);
        break;
    case kIdxMembershipDtor:
        for (u32 i = 0; i < 8; ++i) arenaA.Put32(a[0] + 4 + 4 * i, ((a[3] >> i) & 1) ? a[8 + i] : 0u);
        gRmAt = a[4]; gRmSlot = a[5]; gRmVal = a[6]; gRmCalls = 0; gRet = a[7];
        thisCall(0);
        break;
    case kIdxUserList:
        arenaA.Put32(a[0] + 0x20, a[3]);
        for (u32 k = 0; k < 4; ++k) { arenaA.Put32(a[0] + 4 * k, a[4 + k]); arenaA.Put32(a[0] + 0x10 + 4 * k, a[8 + k]); }
        thisCall(1); call.args[0] = a[12];
        break;
    case kIdxIsPedTypeGang:           // cdecl: lo limpia el llamador
        call.argCount = 1; call.args[0] = a[3]; call.popBytes = 4;
        break;
    case kIdxLinkShutdown:
        arenaA.Put32(a[0] + 0x30, a[3]);
        gLinksField = arenaA.m.data() + kGuard + a[0] + 0x30;
        thisCall(0);
        break;
    case kIdxLinkInsert: {
        const u32 free = a[3], used = a[4];
        const uintptr_t list = arenaA.Obj(a[0]);
        const uintptr_t usedHead = list, usedTail = list + 0xC, freeHead = list + 0x18, freeTail = list + 0x24;
        auto node = [&](u32 j) { return arenaB.Obj(a[5] + 0xC * j); };
        for (u32 j = 0; j < 5; ++j) PutAt(node(j), a[8 + j]);                                  // data
        PutAt(usedHead + 8, used ? u32(node(0)) : u32(usedTail));
        PutAt(usedTail + 4, used ? u32(node(used - 1)) : u32(usedHead));
        for (u32 j = 0; j < used; ++j) {
            PutAt(node(j) + 4, j ? u32(node(j - 1)) : u32(usedHead));
            PutAt(node(j) + 8, j + 1 < used ? u32(node(j + 1)) : u32(usedTail));
        }
        PutAt(freeHead + 8, free ? u32(node(used)) : u32(freeTail));
        PutAt(freeTail + 4, free ? u32(node(used + free - 1)) : u32(freeHead));
        for (u32 j = 0; j < free; ++j) {
            PutAt(node(used + j) + 4, j ? u32(node(used + j - 1)) : u32(freeHead));
            PutAt(node(used + j) + 8, j + 1 < free ? u32(node(used + j + 1)) : u32(freeTail));
        }
        arenaC.Put32(a[6], a[7]);
        thisCall(1); call.args[0] = u32(arenaC.Obj(a[6]));
        break;
    }
    case kIdxInAirType:
        arenaA.Put32(a[0], a[3]);
        thisCall(0);
        break;
    default:
        arenaA.Put32(a[0], a[3]);
        thisCall(0);
        break;
    }
    return call;
}

// valor devuelto observado: 0 = this o puntero (etiqueta), 1 = AL, 2 = EAX completo, 3 = void
inline int RetKind(unsigned fn) {
    switch (fn) {
    case kIdxMissionSdd: case kIdxDriveToPointSdd: case kIdxChatSdd: case kIdxSimpleInAirSdd: case kIdxSimpleLandSdd: case kIdxFxPrimCtor: case kIdxFxSphereCtor: case kIdxLinkInsert: return 0;
    case kIdxUserList: return 1;
    case kIdxIsPedTypeGang: case kIdxInAirType: return 2;
    default: return 3;
    }
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        gRet = 0; gDtorWrite = 0; gLinksField = nullptr; gRmAt = gRmSlot = gRmVal = gRmCalls = 0;
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["wchanges"] = fn == kIdxLinkInsert ? json({{"A", arenaA.WChanges()}, {"B", arenaB.WChanges()}, {"C", arenaC.WChanges()}}) : json(nullptr);
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (RetKind(fn)) {
        case 0: obs["ret"] = Tag(uintptr_t(eax)); break;
        case 1: obs["ret"] = eax & 0xFFu; break;
        case 2: obs["ret"] = eax; break;
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN148A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN148A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn148a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn148a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn148a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator delete 0x8214BD y delete[] 0x8213AE, MemoryMgr.cpp): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador CON hook RH cuya llamada en la funcion probada es a una funcion propia de la DLL sin hook (direccion dada): el falso va en la direccion del EXE y en esa funcion. Se exige que no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    const StubSpec ctaskDelete = F(0x61A5B0, &FkDelete<0x61A5B0>);
    env.stubs[kIdxAudioDtor] = {F(0x4EF660, &FkSoundDtor)};
    env.stubs[kIdxSetDefaultTask] = {F(0x5F7540, &FkSetTask)};
    env.stubs[kIdxReportBarDefault] = {F(0x5F7730, &FkReportAll)};
    env.stubs[kIdxWanderDtor] = {F(0x63CA40, &FkBody<0x63CA40>)};
    env.stubs[kIdxMissionSdd] = {F(0x63CCC0, &FkBody<0x63CCC0>), ctaskDelete};
    env.stubs[kIdxDriveToPointSdd] = {F(0x63CE70, &FkBody<0x63CE70>), ctaskDelete};
    env.stubs[kIdxChatSdd] = {F(0x5F7FD0, &FkBody<0x5F7FD0>), ctaskDelete};
    env.stubs[kIdxMembershipDtor] = {F(0x5F80D0, &FkRemoveMember)};
    env.stubs[kIdxLinkShutdown] = {ByExeAddress(0x8213AE, &FkDeleteArray)};
    env.stubs[kIdxInAirLandDtor] = {F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[kIdxSimpleInAirSdd] = {F(0x678D50, &FkBody<0x678D50>), ctaskDelete};
    env.stubs[kIdxSimpleLandSdd] = {F(0x678EE0, &FkBody<0x678EE0>), ctaskDelete};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[kIdxAudioDtor] = "toda la funcion salvo CAESound::Destructor (0x4EF660, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxSetDefaultTask] = "toda la funcion salvo CPedGroupIntelligence::SetTask (0x5F7540, con hook, falso identico)";
    dllReal[kIdxReportBarDefault] = "toda la funcion salvo CPedGroupIntelligence::ReportAllTasksFinished (0x5F7730, con hook, falso identico)";
    dllReal[kIdxWanderDtor] = "toda la funcion salvo CTaskComplexCarDrive::Destructor (0x63CA40, con hook, falso identico)";
    dllReal[kIdxMembershipDtor] = "toda la funcion salvo CPedGroupMembership::RemoveMember(int) (0x5F80D0, con hook, falso identico)";
    dllReal[kIdxLinkShutdown] = "toda la funcion salvo operator delete[] (0x8213AE: la DLL lo llama por la direccion del EXE, falso)";
    dllReal[kIdxInAirLandDtor] = "toda la funcion salvo ~CTaskComplex (0x61A3D0, con hook, falso identico)";
    const char* const sddHooked = "toda la funcion salvo el cuerpo del destructor (con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    const char* const sddPlain = "toda la funcion salvo el cuerpo del destructor (sin hook: falso en la direccion del EXE y en la funcion propia de la DLL) y CTask::operator delete (0x61A5B0, con hook, falso identico)";
    for (const unsigned f : {kIdxMissionSdd, kIdxDriveToPointSdd, kIdxChatSdd}) dllReal[f] = sddHooked;
    for (const unsigned f : {kIdxSimpleInAirSdd, kIdxSimpleLandSdd}) dllReal[f] = sddHooked;   // 0x678D50 (lote 152A) y 0x678EE0 (lote 159A, banco Fn159A) tienen hook: falso en el cuerpo del hook
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()},
                                 {"iat", nullptr}};
    }
    regions.clear();                                              // sin memoria global propia
    Run(env, ready, registered, bodies, collaborators);
}
}
