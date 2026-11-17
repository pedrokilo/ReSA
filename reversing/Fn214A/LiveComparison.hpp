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

// Banco par a par Fn214A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 7 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias
// (A, B, C y D con guardas de 0x40). Colaboradores: falsos en 0x45B730 (EXE thiscall y DLL cdecl: ReplayStoreCarUpdate), 0x45BA90, 0x571A00, 0x571B70 (mismo falso en la direccion del EXE y en el cuerpo de la DLL) y 0x409C90;
// 0x45C4B0 es REAL en la funcion 1. Memoria global tocada y restaurada: marca 0xA9B0B0, puntero al pool de vehiculos 0xB74494, tabla de conversion de CReplay 0x97F67C.., SuppressedVehicleModels 0xA44940..0xA44A00.
// Se observan: los bytes cambiados de las cuatro arenas, los cambios de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como etiqueta de arena) y el estado x87 (siempre vacio).
// EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

// Cuerpo de la DLL del guardado de paquetes de coche (Replay.cpp, fuera del espacio anonimo): el banco lo sustituye por un falso.
void ReplayStoreCarUpdate(uint8* p, CVehicle* veh, int32 poolIdx);
// Cuerpo de 0x45C4B0 con el ESI del llamador explicito (PlayerPedData.cpp): lo que llama CPlayerInfo::operator= de la DLL.
void PlayerPedDataAssignExe(CPlayerPedData* self, const CPlayerPedData* rhsPtr, uint32 esi);

namespace F214ALive {
using namespace F214AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F214AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); }
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
inline DWORD gBenchThread{};
inline void Put32At(u8* p, u32 v) { memcpy(p, &v, 4); }
// Mutaciones de los falsos (comprueban el ORDEN: lo que la funcion escribe antes de la llamada se pierde, lo que escribe despues se conserva)
inline bool gMutStore{};                        // 0x45B730: el falso reescribe +0 y +0x34..+0x4B del paquete
inline std::array<u32, 7> gMutStoreVals{};
inline bool gMutExtract{};                      // 0x45BA90: el falso reescribe +0x5A4, +0x5A8, +0x5AC y +0x5C0 del tren
inline std::array<u32, 4> gMutExtractVals{};

// 0x45B730 (guardado del paquete de coche): EXE = thiscall (ECX = paquete, dos argumentos de pila, `ret 8`); DLL = ReplayStoreCarUpdate (cdecl, tres argumentos). Evento [direccion, paquete, vehiculo, indice, byte +0 y dwords +0x34, +0x40, +0x44 al ENTRAR]
inline void CarUpdateRecord(u8* p, void* veh, u32 idx) {
    Ev& e = PushEv(0x45B730, 7);
    e.v[0] = Tag(p); e.v[1] = Tag(veh); e.v[2] = idx; e.v[3] = p[0]; e.v[4] = BitsOf(p + 0x34); e.v[5] = BitsOf(p + 0x40); e.v[6] = BitsOf(p + 0x44);
    if (gMutStore) { p[0] = u8(gMutStoreVals[0]); for (u32 i = 0; i < 6; ++i) Put32At(p + 0x34 + 4 * i, gMutStoreVals[1 + i]); }
}
inline void __fastcall FkCarUpdateExe(u8* p, void* /*edx*/, void* veh, u32 idx) { CarUpdateRecord(p, veh, idx); }
inline void __cdecl FkCarUpdateDll(u8* p, void* veh, u32 idx) { CarUpdateRecord(p, veh, idx); }
// 0x45BA90 (lectura del paquete de coche, thiscall, ECX = paquete, vehiculo y float en la pila, `ret 8`; la DLL llama al metodo con la misma convencion): evento [direccion, paquete, vehiculo, bits del float, dwords +0x5A4, +0x5A8, +0x5AC y byte +0x5C0 al ENTRAR]
inline void __fastcall FkExtractVehicle(u8* p, void* /*edx*/, u8* veh, u32 interpBits) {
    Ev& e = PushEv(0x45BA90, 7);
    e.v[0] = Tag(p); e.v[1] = Tag(veh); e.v[2] = interpBits; e.v[3] = BitsOf(veh + 0x5A4); e.v[4] = BitsOf(veh + 0x5A8); e.v[5] = BitsOf(veh + 0x5AC); e.v[6] = veh[0x5C0];
    if (gMutExtract) { Put32At(veh + 0x5A4, gMutExtractVals[0]); Put32At(veh + 0x5A8, gMutExtractVals[1]); Put32At(veh + 0x5AC, gMutExtractVals[2]); veh[0x5C0] = u8(gMutExtractVals[3]); }
}
// 0x571A00 (CEntity::CleanUpOldReference, thiscall, ECX = entidad vieja, un argumento de pila `ret 4`) y 0x571B70 (RegisterReference, ECX = entidad nueva): evento [direccion, entidad, direccion de la referencia, valor de la referencia al llamar]
inline void __fastcall FkCleanUpRef(void* self, void* /*edx*/, u32* ref) { Ev& e = PushEv(0x571A00, 3); e.v[0] = Tag(self); e.v[1] = Tag(ref); e.v[2] = Tag(uintptr_t(*ref)); }
inline void __fastcall FkRegisterRef(void* self, void* /*edx*/, u32* ref) { Ev& e = PushEv(0x571B70, 3); e.v[0] = Tag(self); e.v[1] = Tag(ref); e.v[2] = Tag(uintptr_t(*ref)); }
// 0x409C90 (CStreaming::SetMissionDoesntRequireModel, cdecl, un argumento): evento [direccion, argumento]
inline void __cdecl FkSetMissionDoesntRequireModel(i32 id) { Ev& e = PushEv(0x409C90, 1); e.v[0] = u32(id); }

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
    u32 esi{0x2468ACE0u};
};
inline void Put32G(u32 addr, u32 v) { memcpy(reinterpret_cast<void*>(addr), &v, 4); }
inline u32 PoolRelative(u32 base, u32 k, u32 rem) { return base + k * 0xA18u + rem; }   // objeto del pool con resto (aritmetica de 32 bits)

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    const Kind kind = kKind[c.fn];
    arenaA.Pattern(a[2], a[92]); arenaB.Pattern(a[2], a[93]); arenaC.Pattern(a[2], a[94]); arenaD.Pattern(a[2], a[95]);
    gMutStore = gMutExtract = false;
    *reinterpret_cast<u8*>(kMarker) = 0x5A;   // marca que la prueba negativa (mutate.py) lee; el juego real nunca la pone
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kind) {
    case kPedCopy: case kInfoCopy: {
        const u32 span = kSpan[c.fn];
        arenaB.Pattern(0, a[93]);   // el origen en B es siempre aleatorio (los demas rellenos harian invisible la copia)
        u32 src{};
        if (a[4] == 0) src = u32(arenaB.Obj(a[5]));
        else { FillWords(arenaA.m.data() + kGuard + a[5], span, a[6]); src = u32(arenaA.Obj(a[5])); }
        call.args[0] = src; call.argCount = 1; call.popBytes = 0;
        if (kind == kPedCopy) call.esi = thisA - 4u;   // el EXE usa el ESI de su unico llamador (el CPlayerInfo = this - 4); la DLL lo calcula asi
        break;
    }
    case kStoreTrain: {
        arenaB.Pattern(0, a[93]); arenaC.Pattern(0, a[94]);
        const u32 base = a[13] == 1 ? u32(arenaC.Obj(a[14])) : a[14];
        for (u32 w = 0; w < 2; ++w) {
            const u32 b = 7 + 3 * w;
            const u32 v = a[b] == 0 ? 0u : (a[b] == 1 ? PoolRelative(base, a[b + 1], a[b + 2]) : a[b + 1]);
            arenaB.Put32(a[5] + 0x5D0 + 4 * w, v);
        }
        arenaD.Put32(a[15], base); arenaD.Put32(a[15] + 4, u32(arenaC.Obj(0x800)));
        Put32G(kPoolPtr, u32(arenaD.Obj(a[15])));
        gMutStore = a[16] != 0;
        gMutStoreVals = {a[17], a[18], a[19], a[20], a[21], a[22], a[23]};
        call.args[0] = u32(arenaB.Obj(a[5])); call.args[1] = a[6]; call.argCount = 2; call.popBytes = 0;   // `ret 8`
        break;
    }
    case kExtractTrain: {
        arenaB.Pattern(0, a[93]); arenaC.Pattern(0, a[94]);
        arenaA.Put32(o + 0x34, a[23]); arenaA.Put32(o + 0x38, a[24]); arenaA.Put32(o + 0x3C, a[25]); arenaA.Put8(o + 0x48, a[26]);
        arenaA.Put32(o + 0x40, a[4]); arenaA.Put32(o + 0x44, a[6]);
        for (u32 w = 0; w < 2; ++w) {
            const u32 b = 7 + 2 * w;
            const u32 v = a[b] == 0 ? 0u : (a[b] == 1 ? u32(arenaC.Obj(a[b + 1])) : a[b + 1]);
            arenaB.Put32(a[5] + 0x5D0 + 4 * w, v);
        }
        const u32 base = a[12] == 1 ? u32(arenaC.Obj(a[13])) : a[13];
        arenaD.Put32(a[14], base); arenaD.Put32(a[14] + 4, u32(arenaC.Obj(a[15])));
        Put32G(kPoolPtr, u32(arenaD.Obj(a[14])));
        for (u32 i = 0; i < kConvEntries; ++i) Put32G(kConvTable + 4 + 4 * i, TableSlot(a[16], a[17], i));
        gMutExtract = a[18] != 0;
        gMutExtractVals = {a[19], a[20], a[21], a[22]};
        call.args[0] = u32(arenaB.Obj(a[5])); call.args[1] = a[11]; call.argCount = 2; call.popBytes = 0;   // `ret 8`
        break;
    }
    case kSuppress:
        for (u32 i = 0; i < 48; ++i) Put32G(kSuppressed + 4 * i, a[8 + i]);
        call.args[0] = a[56]; call.argCount = 1; call.popBytes = 4;   // cdecl
        break;
    case kSetAnim:
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;    // cdecl
        break;
    case kQuat:
        for (u32 i = 0; i < 4; ++i) arenaA.Put32(o + 4 * i, a[4 + i]);
        call.popBytes = 0;
        break;
    }
    return call;
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
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet, call.esi);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kPedCopy: case kInfoCopy:
            obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        default: obs["ret"] = nullptr; break;          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN214A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN214A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Colaborador con hook V (RH_ScopedVMTInstall): la ruta usa el cuerpo de la DLL de la ranura (la funcion que la DLL llama de forma no virtual) y el hook se apaga.
inline Pair MakeCollabV(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody)) {
        if (vbody) env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        else env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", vbody != 0}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn214a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn214a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn214a"); }
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
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0 && ModuleOf(env.route[f]->Body()) == env.asi;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}}) : json(nullptr);
        collaborators[kFn[f]] = {{"found", found}, {"body", found ? env.route[f]->Body() : 0}, {"body_module", found ? ModuleOf(env.route[f]->Body()) : 0}, {"asi", env.asi}, {"cat", cat}, {"name", name}};
    }
    json hooksFound = json::array();
    if (!ready) { Run(env, false, registered, bodies, collaborators); return; }
    // Colaborador con hook RH en la EXE (apagado durante el banco por la ruta que crea MakeCollab). `dllBody` = el cuerpo de la DLL que la funcion probada llama de verdad (0 = el cuerpo del hook, que es la misma
    // funcion); `fakeDll` = falso para el cuerpo de la DLL cuando su convencion no es la de la direccion del EXE.
    auto Collab = [&](uintptr_t exe, uintptr_t dllBody, void* fakeExe, void* fakeDll = nullptr) {
        Pair p = MakeCollab(env, exe, hooksFound);
        if (!p.hooked || !p.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        if (dllBody) p.dll = dllBody;
        return StubSpec{p, fakeExe, fakeDll};
    };
    // 2: guardado del paquete de tren. La DLL llama a ReplayStoreCarUpdate (Replay.cpp, fuera del espacio anonimo, sin insertar en linea) y no al hook de 0x45B730.
    env.stubs[2] = {Collab(0x45B730, Body(reinterpret_cast<uintptr_t>(&ReplayStoreCarUpdate)), reinterpret_cast<void*>(&FkCarUpdateExe), reinterpret_cast<void*>(&FkCarUpdateDll))};
    // 3: lectura del paquete de tren. La DLL llama al metodo tReplayVehicleBlock::ExtractVehicleUpdateData (no al hook de 0x45BA90, que comprueba el vehiculo nulo) y a CEntity::CleanUpOldReference / RegisterReference.
    {
        auto vehMember = &tReplayVehicleBlock::ExtractVehicleUpdateData; uintptr_t vehAddr{}; static_assert(sizeof(vehMember) == sizeof(uintptr_t)); memcpy(&vehAddr, &vehMember, sizeof(vehAddr));
        env.stubs[3] = {Collab(0x45BA90, Body(vehAddr), reinterpret_cast<void*>(&FkExtractVehicle)), Collab(0x571A00, 0, reinterpret_cast<void*>(&FkCleanUpRef)), Collab(0x571B70, 0, reinterpret_cast<void*>(&FkRegisterRef))};
    }
    // 5: SetMissionDoesntRequireAnim salta a 0x409C90 (SetMissionDoesntRequireModel; la DLL llama al cuerpo del hook)
    env.stubs[5] = {Collab(0x409C90, 0, reinterpret_cast<void*>(&FkSetMissionDoesntRequireModel))};
    // 1: CPlayerInfo::operator= llama a 0x45C4B0 (CPlayerPedData::operator=, hook de la funcion 0): colaborador REAL en ambos lados (la EXE corre sus bytes y la DLL PlayerPedDataAssignExe; el hook sigue apagado)
    env.reals[1] = {Pair{0x45C4B0, Body(reinterpret_cast<uintptr_t>(&PlayerPedDataAssignExe)), true}};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[1] = "toda la funcion, con el operator= de CPlayerPedData (0x45C4B0) REAL: la EXE corre sus bytes (hook apagado) y la DLL el cuerpo de su operator=";
    dllReal[2] = "toda la funcion salvo ReplayStoreCarUpdate (0x45B730; el cuerpo de la DLL que llama; falsos de la EXE (thiscall) y de la DLL (cdecl) con el mismo registro)";
    dllReal[3] = "toda la funcion salvo tReplayVehicleBlock::ExtractVehicleUpdateData (0x45BA90), CEntity::CleanUpOldReference (0x571A00) y RegisterReference (0x571B70), con hook y falsos identicos; FindPoolIndexForVehicle (0x45C460) REAL";
    dllReal[4] = "toda la funcion (memoria global 0xA44940..0xA44A00 en una region restaurada)";
    dllReal[5] = "toda la funcion salvo SetMissionDoesntRequireModel (0x409C90), con hook (falso identico)";
    dllReal[6] = "toda la funcion y el helper x87 AnimQuaternionNormaliseCpp (X87Math.cpp), sin colaboradores";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& s : env.reals[f]) {
            env.watches.emplace_back(s.exe, 16);
            real.push_back({{"exe", s.exe}, {"dll", s.dll != 0}, {"hooked", s.hooked}, {"dll_addr", s.dll}, {"dll_module", ModuleOf(s.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada (se guarda al empezar y se restaura tras cada lado)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    auto R = [&](uintptr_t at, size_t n) { regions.emplace_back(at, n); return regions.size() - 1; };
    const size_t rMarker = R(kMarker, 1), rPool = R(kPoolPtr, 4), rConv = R(kConvTable, kConvTableBytes), rSupp = R(kSuppressed, kSuppressedBytes);
    for (unsigned f = 0; f < kFunctions; ++f) regionsOf[f] = {rMarker};
    regionsOf[2] = {rMarker, rPool};
    regionsOf[3] = {rMarker, rPool, rConv};
    regionsOf[4] = {rMarker, rSupp};
    Run(env, ready, registered, bodies, collaborators);
}
}
