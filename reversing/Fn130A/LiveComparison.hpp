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

// Banco par a par Fn130A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Diez funciones del EXE de formas variadas (ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall: enlace de
// la entrada del EXE hacia la DLL). El banco invoca la direccion del EXE con objetos sinteticos en arenas propias (A, B, C con guardas de 0x40) y, donde hace falta, memoria global real del juego
// (se guarda al empezar y se restaura tras cada lado). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair); los colaboradores
// de la CRT del EXE (free 0x8213AE, operator delete 0x8214BD) llevan el falso solo en la direccion del EXE (la DLL los llama por esa direccion).
// Se observan: los bytes cambiados de las tres arenas y de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto (cuando el EXE lo define) y el estado x87
// (debe quedar vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

uintptr_t Lote130ADummyDestructorBody();                // 0x532560, Dummy.cpp
uintptr_t Lote130ATheZonesLockedOrUnlockedAddress();    // 0x572130, TheZones.cpp (funcion propia de la DLL, sin hook)

namespace F130ALive {
using namespace F130AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x63A040, 0x4DFB60, 0x5B3680, 0x536E90, 0x583AF0, 0x405A60, 0x632E00, 0x49CD50, 0x40FAF0, 0x532770, 0x6912C0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F130AFix::Arena(WIN);
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

// Regiones de memoria global del juego que el banco toca (indices fijos; ver regionsOf)
enum : unsigned { kRegString, kRegBlips, kRegDrawing, kRegRevealed, kRegSeq, kRegTags, kRegPool };
inline constexpr uintptr_t kGString = 0xB71670, kBlipsBase = 0xBA86E8, kDrawingAt = 0xBA67A1, kRevealedAt = 0xBA372C, kSeqBase = 0xC17898, kTagBase = 0xA9A8B8, kPoolPtrAt = 0xB744A0;

// ================================================================================================ falsos de los colaboradores
inline u32 gDtorWrite{};             // dword que el falso del cuerpo del destructor deja en [this]
inline u32 gRet{};                   // EAX que devuelven los falsos
inline DWORD gBenchThread{};

// Lee un dword solo si el puntero cae dentro de una arena registrada
inline u32 SafeBits(const void* p) {
    for (const auto& t : tagTable) if (uintptr_t(p) >= t.base && uintptr_t(p) - t.base + 4 <= t.size) return BitsOf(p);
    return 0xDEAD0000u;
}
inline u32 StrLen(const char* s) { return s ? u32(strlen(s)) : 0u; }
// LoadingScreen (0x53DED0, cdecl, tres punteros): evento [direccion, hash y longitud de msg1, msg2 == gString, hash y longitud de msg2, msg3 en crudo]
inline void __cdecl FkLoadingScreen(const char* m1, const char* m2, const char* m3) {
    Ev& e = PushEv(0x53DED0, 6);
    e.v[0] = FnvStr(m1); e.v[1] = StrLen(m1); e.v[2] = uintptr_t(m2) == kGString ? 1u : 0u; e.v[3] = FnvStr(m2); e.v[4] = StrLen(m2); e.v[5] = u32(uintptr_t(m3));
}
// CTheZones::GetCurrentZoneLockedOrUnlocked (0x572130, cdecl, dos floats en la pila): evento [direccion, bits de x, bits de y]; devuelve EAX del caso
inline u32 __cdecl FkZone(u32 xBits, u32 yBits) {
    Ev& e = PushEv(0x572130, 2);
    e.v[0] = xBits; e.v[1] = yBits;
    return gRet;
}
// free / operator delete[] (0x8213AE) y operator delete (0x8214BD) de la CRT del EXE (cdecl, un puntero): evento [direccion, puntero (etiqueta de arena o crudo), dword de [puntero] si cae en una arena]
inline void __cdecl FkFree(void* p) { Ev& e = PushEv(0x8213AE, 2); e.v[0] = Tag(p); e.v[1] = SafeBits(p); }
inline void __cdecl FkDelete(void* p) { Ev& e = PushEv(0x8214BD, 2); e.v[0] = Tag(p); e.v[1] = SafeBits(p); }
// CVisibilityPlugins::SetUserValue (0x732380, cdecl, atomic y word): evento [direccion, atomic en crudo, word bajo del valor]
inline void __cdecl FkSetUser(void* atomic, u32 value) { Ev& e = PushEv(0x732380, 2); e.v[0] = u32(uintptr_t(atomic)); e.v[1] = value & 0xFFFFu; }
// Cuerpo del destructor de CDummy (0x532560, thiscall sin argumentos): evento [direccion, ECX (etiqueta), dword de [ECX]]; deja gDtorWrite en [ECX]; devuelve gRet
inline u32 __fastcall FkDummyBody(void* self, void*) {
    Ev& e = PushEv(0x532560, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}

// Ranura 0 de la vtable de la tarea guardada de CTaskSimpleAffectSecondaryBehaviour (destructor con borrado: thiscall, un argumento de pila, `ret 4`): evento [etiqueta de ranura, ECX (etiqueta), banderas]
inline u32 __fastcall FkTaskSlot0(void* self, void*, u32 flags) {
    Ev& e = PushEv(0xFAB1, 2);
    e.v[0] = Tag(self); e.v[1] = flags;
    return gRet;
}
// CTask::Destructor (0x61A3A0, thiscall sin argumentos): evento [direccion, ECX (etiqueta), dword de [ECX] al entrar (la vtable que dejo el destructor)]
inline u32 __fastcall FkTaskBase(void* self, void*) {
    Ev& e = PushEv(0x61A3A0, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
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

inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    switch (c.fn) {
    case 0: {   // CopyFrom
        auto& src = a[4] ? arenaA : arenaB;
        src.Put32(a[3], a[5]);
        call.ecx = u32(arenaA.Obj(a[0]));
        call.args[0] = u32(src.Obj(a[3])); call.argCount = 1; call.popBytes = 0;
        break;
    }
    case 1:
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case 2: {   // LoadingScreenLoadingFile
        u8* const dst = arenaC.m.data() + kGuard + a[0];
        memcpy(dst, c.s.data(), c.s.size()); dst[c.s.size()] = 0;
        if (a[3] == 3) memset(reinterpret_cast<void*>(kGString), 0x5A, 352);
        else FillKind(reinterpret_cast<u8*>(kGString), 352, a[3], a[15]);
        call.args[0] = u32(arenaC.Obj(a[0])); call.argCount = 1; call.popBytes = 4;
        break;
    }
    case 3: {   // CompressVector
        auto& src = a[4] ? arenaA : arenaB;
        src.Put32(a[3], a[5]); src.Put32(a[3] + 4, a[6]); src.Put32(a[3] + 8, a[7]);
        call.args[0] = u32(arenaA.Obj(a[0])); call.args[1] = u32(src.Obj(a[3])); call.argCount = 2; call.popBytes = 8;
        break;
    }
    case 4: {   // HasThisBlipBeenRevealed
        FillKind(reinterpret_cast<u8*>(kBlipsBase), 7040, a[2], a[15]);
        reinterpret_cast<u8*>(kDrawingAt)[0] = u8(a[3]);
        reinterpret_cast<u8*>(0xBA8715 + a[0] * 40)[0] = u8(a[4]);
        PutGlobal32(kRevealedAt, a[5]);
        PutGlobal32(0xBA86F8 + a[0] * 40, a[6]); PutGlobal32(0xBA86FC + a[0] * 40, a[7]);
        gRet = a[8];
        call.args[0] = a[0]; call.argCount = 1; call.popBytes = 4;
        break;
    }
    case 5: {   // CIplDefPool::ScalarDeletingDestructor
        arenaA.Put32(a[0] + 0, a[3]); arenaA.Put32(a[0] + 4, a[4]); arenaA.Put32(a[0] + 8, a[5]); arenaA.Put32(a[0] + 0xC, a[6]); arenaA.Put32(a[0] + 0x10, a[8] | (a[9] << 8));
        call.ecx = u32(arenaA.Obj(a[0]));
        call.args[0] = a[7]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    case 6: {   // GetAvailableSlot
        FillKind(reinterpret_cast<u8*>(kSeqBase), 0x1058, a[2], a[15]);
        for (u32 i = 0; i < 64; ++i) {
            reinterpret_cast<u8*>(0xC17898 + i)[0] = c.s[i];
            const bool nonNull = i < 32 ? (a[4] >> i) & 1u : (a[5] >> (i - 32)) & 1u;
            PutGlobal32(0xC17900 + i * 0x40, nonNull ? (Rand(a[15], i) | 1u) : 0u);
        }
        call.args[0] = a[0]; call.argCount = 1; call.popBytes = 4;
        break;
    }
    case 7: {   // ResetAlpha
        arenaA.Put32(a[0] + 0x18, a[3]);
        const u32 self = u32(arenaA.Obj(a[0]));
        FillKind(reinterpret_cast<u8*>(kTagBase), 0x4C0, a[2], a[15]);
        PutGlobal32(0xA9AD70, a[4]);
        for (u32 i = 0; i < 150; ++i) {
            const bool isEntity = (a[5 + i / 32] >> (i % 32)) & 1u;
            u32 other = Rand(a[15], 200 + i) | 1u;
            if (other == self) other += 2;
            PutGlobal32(0xA9A8C0 + i * 8, isEntity ? self : other);
            reinterpret_cast<u8*>(0xA9A8C0 + i * 8 + 4)[0] = c.s[i];
        }
        call.args[0] = self; call.argCount = 1; call.popBytes = 4;
        break;
    }
    case 8:
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    case 9: {   // CDummy::ScalarDeletingDestructor
        const u32 self = u32(arenaA.Obj(a[0]));
        arenaA.Put32(a[0], a[6]);
        arenaB.Put32(a[10] + 0, self - a[3]); arenaB.Put32(a[10] + 4, u32(arenaC.Obj(a[4]))); arenaB.Put32(a[10] + 0xC, a[5]);
        PutGlobal32(kPoolPtrAt, u32(arenaB.Obj(a[10])));
        gDtorWrite = a[8]; gRet = a[9];
        call.ecx = self;
        call.args[0] = a[7]; call.argCount = 1; call.popBytes = 0;
        break;
    }
    default: {  // CTaskSimpleAffectSecondaryBehaviour::Destructor: la tarea guardada es un objeto de B con una vtable de C cuya ranura 0 es un falso
        const u32 task = u32(arenaB.Obj(a[10]));
        arenaA.Put32(a[0], a[3]);
        arenaA.Put32(a[0] + 0x10, a[4] ? task : 0u);
        arenaB.Put32(a[10], u32(arenaC.Obj(a[11])));
        arenaC.Put32(a[11], u32(reinterpret_cast<uintptr_t>(&FkTaskSlot0)));
        gRet = 0x00C0FFEEu;
        call.ecx = u32(arenaA.Obj(a[0])); call.argCount = 0; call.popBytes = 0;
        break;
    }
    }
    return call;
}

// Observa las regiones globales de la funcion: [[region, desplazamiento, byte nuevo]] respecto a la imagen tras preparar el caso
inline json GlobalChanges(const std::vector<std::vector<u8>>& before) {
    json out = json::array();
    const auto& idx = regionsOf[curFn];
    for (size_t k = 0; k < idx.size(); ++k) {
        const auto& r = regions[idx[k]];
        for (size_t i = 0; i < r.bytes; ++i)
            if (r.Ptr()[i] != before[k][i]) out.push_back(json::array({u32(idx[k]), u32(i), r.Ptr()[i]}));
    }
    return out;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const json input = json::parse(JsonC(c));
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        gRet = 0; gDtorWrite = 0;
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        std::vector<std::vector<u8>> before;
        for (const auto i : regionsOf[fn]) before.push_back(ReversingProbe::Bytes(regions[i].Ptr(), regions[i].bytes));
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        obs["gchanges"] = GlobalChanges(before);
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (fn) {
        case 4: obs["ret"] = eax & 0xFFu; break;
        case 5: case 8: case 9: obs["ret"] = Tag(uintptr_t(eax)); break;
        case 6: obs["ret"] = eax; break;
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN130A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN130A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn130a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn130a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn130a"); }
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
    // Colaborador SIN hook RH: la DLL tiene su propia funcion (direccion dada); el falso va en la direccion del EXE y en esa funcion. Se exige que de verdad no tenga hook.
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador de la CRT del EXE: sin hook y sin funcion propia de la DLL (dll = 0: la DLL llama a la direccion del EXE); el falso va solo en la direccion del EXE
    auto X = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    env.stubs[2] = {F(0x53DED0, &FkLoadingScreen)};
    env.reals[2] = {Pair{0x821490, 0, false}, Pair{0x821BB5, 0, false}};
    env.reals[3] = {Pair{0x821B40, 0, false}};
    env.stubs[4] = {U(0x572130, Lote130ATheZonesLockedOrUnlockedAddress(), &FkZone)};
    env.stubs[5] = {X(0x8213AE, &FkFree), X(0x8214BD, &FkDelete)};
    env.stubs[7] = {F(0x732380, &FkSetUser)};
    env.stubs[9] = {F(0x532560, &FkDummyBody)};   // desde el lote 141A el cuerpo de ~CDummy (0x532560) tiene hook RH
    env.stubs[10] = {F(0x61A3A0, &FkTaskBase)};
    const char* dllReal[kFunctions] = {
        "toda la funcion (sin colaboradores)",
        "toda la funcion (sin colaboradores)",
        "toda la funcion salvo LoadingScreen (0x53DED0, con hook, falso identico); strrchr y sprintf se ejecutan reales (CRT del EXE 0x821490 / 0x821BB5 frente a la CRT de la DLL) y GetFilename es codigo de la DLL",
        "toda la funcion; _ftol2 real (CRT del EXE 0x821B40 frente a la de la DLL)",
        "toda la funcion salvo CTheZones::GetCurrentZoneLockedOrUnlocked (0x572130; funcion propia de la DLL sin hook, falso identico)",
        "toda la funcion salvo free / delete[] (0x8213AE) y operator delete (0x8214BD) de la CRT del EXE (falsos identicos en la direccion del EXE; la DLL los llama por esa direccion)",
        "toda la funcion (sin colaboradores)",
        "toda la funcion salvo CVisibilityPlugins::SetUserValue (0x732380, con hook, falso identico) y CTagManager::FindTagDesc (funcion de la DLL real; el EXE hace la busqueda en linea)",
        "toda la funcion (sin colaboradores)",
        "toda la funcion salvo el cuerpo del destructor de CDummy (0x532560, funcion propia sin hook, falso identico); el borrado del pool va en linea",
        "toda la funcion salvo CTask::Destructor (0x61A3A0, con hook, falso identico); la ranura 0 de la vtable de la tarea guardada es un falso de la arena (vtable propia)"};
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
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}};
    }
    // Memoria global del juego que toca el banco (se guarda ahora y se restaura tras cada lado)
    regions.clear();
    regions.emplace_back(kGString, 352);
    regions.emplace_back(kBlipsBase, 7040);
    regions.emplace_back(kDrawingAt, 1);
    regions.emplace_back(kRevealedAt, 4);
    regions.emplace_back(kSeqBase, 0x1058);
    regions.emplace_back(kTagBase, 0x4C0);
    regions.emplace_back(kPoolPtrAt, 4);
    regionsOf[2] = {kRegString};
    regionsOf[4] = {kRegBlips, kRegDrawing, kRegRevealed};
    regionsOf[6] = {kRegSeq};
    regionsOf[7] = {kRegTags};
    regionsOf[9] = {kRegPool};
    Run(env, ready, registered, bodies, collaborators);
}
}
