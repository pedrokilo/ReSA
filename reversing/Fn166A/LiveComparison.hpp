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

// Banco par a par Fn166A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp. 24 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL.
// Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). Colaboradores falseados con el mismo falso en la direccion del EXE y en el cuerpo de la DLL.
#include "SeekEntity/TaskComplexSeekEntityStandard.h"
#include "TaskSimpleRunNamedAnim.h"
#include "TaskSimpleAnim.h"

extern void* TxdParentConstructor(void*, RwInt32, RwInt32);
extern void* TxdParentDestructor(void*, RwInt32, RwInt32);
extern void* TxdParentCopyConstructor(void*, const void*, RwInt32, RwInt32);

namespace F166ALive {
using namespace F166AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F166AFix::Arena(WIN);
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
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gNew1{}, gNew2{}, gTaskNew{}; inline unsigned gNewN{};
inline u32 gAssoc{};                 // objeto que devuelve BlendAnimation
inline u32 gIsPlayer{}, gIsAlive{}, gActiveTask{}, gTaskType{}, gEvType{}, gPrioThis{}, gPrioRef{};
inline DWORD gBenchThread{};
inline std::array<void*, 16> vtTask{}, vtThis{}, vtRef{};     // vtables falsas (estaticas)
inline uintptr_t gDllCb[5]{};        // direcciones de la DLL de los callbacks: [0] FinishRunAnimMovePedCB, [1] FinishRunAnimCB, [2..4] TxdParent ctor/dtor/copia

template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
inline void* __cdecl FkTaskNew(u32 size) { Ev& e = PushEv(kTaskNew, 1); e.v[0] = size; return reinterpret_cast<void*>(uintptr_t(gTaskNew)); }
inline u32 __fastcall FkFightCtor(void* self, void*, u32 entity, u32 cmd, u32 idle) {
    Ev& e = PushEv(kFightCtor, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(entity)); e.v[2] = cmd; e.v[3] = idle;
    return gRet;
}
inline void __fastcall FkRegRef(void* self, void*, void* ref) { Ev& e = PushEv(kRegRef, 2); e.v[0] = Tag(self); e.v[1] = Tag(ref); }
// ampliacion del vector (0x7304B0; thiscall, capacidad nueva en la pila): evento [direccion, ECX, capacidad]; escribe la capacidad en [ECX]
inline void __fastcall FkResize(void* self, void*, u32 newCap) {
    Ev& e = PushEv(kResize, 2);
    e.v[0] = Tag(self); e.v[1] = newCap;
    memcpy(self, &newCap, 4);
}
// operator new del EXE (0x82119A): en el hilo del banco evento [direccion, tamano]; primera llamada gNew1, segunda gNew2; en otros hilos llama a malloc del CRT (0x824257)
inline void* __cdecl FkNewScalar(u32 size) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<void*(__cdecl*)(u32)>(0x824257)(size);
    Ev& e = PushEv(kNewScalar, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gNewN++ == 0 ? gNew1 : gNew2));
}
inline u32 __fastcall FkDirCtor(void* self, void*, u32 capacity) { Ev& e = PushEv(kDirCtor, 2); e.v[0] = Tag(self); e.v[1] = capacity; return gRet; }
inline u32 NormCb(u32 cb) {
    static const u32 exe[5]{0x61AAA0, 0x61A8A0, 0x731600, 0x731620, 0x731630};
    if (cb < 0x1000000u) return cb;   // direcciones del EXE (o valores pequenos): tal cual
    const u32 resolved = u32(Body(uintptr_t(cb)));
    for (int i = 0; i < 5; ++i) if (gDllCb[i] && (cb == u32(gDllCb[i]) || resolved == u32(gDllCb[i]))) return exe[i];
    return cb;
}
inline u32 __cdecl FkRegPlugin(i32 size, u32 id, u32 ctor, u32 dtor, u32 copy) {
    Ev& e = PushEv(kRegPlugin, 5);
    e.v[0] = u32(size); e.v[1] = id; e.v[2] = NormCb(ctor); e.v[3] = NormCb(dtor); e.v[4] = NormCb(copy);
    return gRet;
}
inline u32 __cdecl FkTexDestroy(void* p) { Ev& e = PushEv(kTexDestroy, 1); e.v[0] = Tag(p); return gRet; }
inline u32 __fastcall FkGetActive(void* self, void*) { Ev& e = PushEv(kGetActive, 1); e.v[0] = Tag(self); return gActiveTask; }
inline u32 __fastcall FkIsPlayer(void* self, void*) { Ev& e = PushEv(kIsPlayer, 1); e.v[0] = Tag(self); return gIsPlayer; }
inline u32 __fastcall FkIsAlive(void* self, void*) { Ev& e = PushEv(kIsAlive, 1); e.v[0] = Tag(self); return gIsAlive; }
inline u32 __fastcall FkTaskType(void* self, void*) { Ev& e = PushEv(kEvTaskType, 1); e.v[0] = Tag(self); return gTaskType; }
inline u32 __fastcall FkEvType(void* self, void*) { Ev& e = PushEv(kEvType, 1); e.v[0] = Tag(self); return gEvType; }
inline u32 __fastcall FkPrioRef(void* self, void*) { Ev& e = PushEv(kEvPrioRef, 1); e.v[0] = Tag(self); return gPrioRef; }
inline u32 __fastcall FkPrioThis(void* self, void*) { Ev& e = PushEv(kEvPrioThis, 1); e.v[0] = Tag(self); return gPrioThis; }
inline u32 __cdecl FkBlend(u32 clump, u32 hier, u32 flags, u32 delta) {
    Ev& e = PushEv(kBlend, 4);
    e.v[0] = Tag(uintptr_t(clump)); e.v[1] = hier; e.v[2] = flags; e.v[3] = delta;
    return gAssoc;
}
template<u32 ADDR> inline void __fastcall FkSetCb(void* self, void*, u32 cb, void* data) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self); e.v[1] = NormCb(cb); e.v[2] = Tag(data);
}
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
inline u32 gGlobalBefore{};
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
    gRet = 0; gDtorWrite = 0; gNew1 = gNew2 = gTaskNew = 0; gNewN = 0; gAssoc = 0; gIsPlayer = gIsAlive = gActiveTask = gTaskType = gEvType = gPrioThis = gPrioRef = 0;
    const u32 self = u32(arenaA.Obj(a[0]));
    const u32 o = a[0];
    switch (kKind[c.fn]) {
    case kSdd: case kIkCtor:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.ecx = self;
        if (kKind[c.fn] == kSdd) { call.args[0] = a[6]; call.argCount = 1; }
        break;
    case kClone:
        arenaA.Put32(o + 0x10, a[3]);
        arenaA.Put32(o + 0x18, a[11] ? u32(arenaB.Obj(a[10])) : a[4]);
        arenaA.Put32(o + 0x24, a[5]);
        gTaskNew = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : a[9]);
        gRet = a[8] == 0 ? gTaskNew : (a[8] == 1 ? a[9] : 0u);
        call.ecx = self;
        break;
    case kConst: case kFloat:
        call.ecx = self;
        break;
    case kSetDist:
        arenaA.Put32(o + 0x18, a[3]); arenaA.Put32(o + 0x28, a[5]); arenaA.Put32(o + 0x2C, a[6]); arenaA.Put32(o + 0x30, a[7]);
        call.ecx = self;
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kAnyMeans: {
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        const u32 entity = a[6] == 0 ? 0u : (a[6] == 1 ? u32(arenaB.Obj(a[7])) : a[8]);
        call.ecx = self;
        call.args = {entity, a[9], a[10], 0, 0, 0, 0, 0}; call.argCount = 3;
        break;
    }
    case kPush: {
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 4, a[4]);
        const u32 store = a[6] == 0 ? u32(arenaB.Obj(a[7])) : (a[6] == 1 ? u32(arenaC.Obj(a[7])) : self + (a[6] == 2 ? 4u : a[6] == 3 ? 8u : a[6] == 4 ? 0u : 0xCu));
        arenaA.Put32(o + 8, store - 4u * a[4]);
        call.ecx = self;
        call.args[0] = a[5]; call.argCount = 1;
        break;
    }
    case kScratch:
        gNew1 = a[3]; gNew2 = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : a[6]); gRet = a[7];
        break;
    case kPlugin:
        gRet = a[3];
        memcpy(reinterpret_cast<void*>(uintptr_t(kPluginGlobal)), &a[4], 4);
        break;
    case kFxCtor:
        arenaA.Put32(o, a[3]);
        call.ecx = self;
        break;
    case kFxDtor:
        arenaA.Put32(o, a[3]);
        for (unsigned k = 0; k < 4; ++k) arenaA.Put32(o + 0xC + 4 * k, a[4 + k] == 0 ? 0u : (a[4 + k] == 1 ? u32(arenaB.Obj(a[16 + k])) : a[20 + k]));
        call.ecx = self;
        break;
    case kAffects: {
        const u32 ped = u32(arenaB.Obj(a[8]));
        arenaB.Put32(a[8] + 0x47C, u32(arenaC.Obj(a[9])));
        if (a[3]) { arenaC.Put32(a[4], u32(uintptr_t(vtTask.data()))); gActiveTask = u32(arenaC.Obj(a[4])); }
        gIsPlayer = a[5]; gIsAlive = a[6]; gTaskType = a[7];
        call.ecx = self;
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kTakes: {
        arenaA.Put32(o, u32(uintptr_t(vtThis.data()))); arenaA.Put32(o + 0xC, a[3]);
        arenaB.Put32(a[7], u32(uintptr_t(vtRef.data())));
        gEvType = a[4]; gPrioThis = a[5]; gPrioRef = a[6];
        call.ecx = self;
        call.args[0] = u32(arenaB.Obj(a[7])); call.argCount = 1;
        break;
    }
    case kStartAnim: {
        const u32 ped = u32(arenaB.Obj(a[16]));
        arenaA.Put32(o + 0xC, a[3]); arenaA.Put32(o + 0x38, a[4]); arenaA.Put32(o + 0x3C, a[5]); arenaA.Put32(o + 0x40, a[6]); arenaA.Put32(o + 0x5C, a[7]);
        arenaA.Put32(o + 0x44, a[10]); arenaA.Put32(o + 0x48, a[11]); arenaA.Put32(o + 0x4C, a[19]); arenaA.Put32(o + 0x8, a[18]); arenaA.Put32(o + 0x60, a[20]);
        arenaB.Put32(a[16] + 0x18, a[8]);
        arenaC.Put32(a[17] + 0x2C, a[9]);
        gAssoc = u32(arenaC.Obj(a[17]));
        call.ecx = self;
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kMatAdd: {
        arenaA.Put32(o + 0x28, a[10]); arenaA.Put32(o + 0x2C, a[11]);
        for (unsigned k = 0; k < 3; ++k) arenaA.Put32(o + 0x30 + 4 * k, a[7 + k]);
        arenaA.Put32(o + 0x3C, a[20]); arenaA.Put32(o + 0x40, a[21]);
        for (unsigned k = 0; k < 3; ++k) arenaB.Put32(a[6] + 4 * k, a[3 + k]);
        static const i32 delta[6]{0, 0, 4, -4, 8, -8};
        const u32 vec = a[22] == 0 ? u32(arenaB.Obj(a[6])) : self + 0x30u + u32(delta[a[22]]);
        call.ecx = self;
        call.args[0] = vec; call.argCount = 1;
        break;
    }
    case kEntryAdd: {
        arenaA.Put32(o, a[3]);
        const u32 text = a[9] == 0 ? u32(arenaC.Obj(a[8])) : self + a[10];
        auto putTextByte = [&](u32 off, u8 v) { if (a[9] == 0) arenaC.Put8(a[8] + off, v); else arenaA.Put8(a[10] + o + off, v); };
        for (u32 i = 0; i < a[16]; ++i) putTextByte(i, u8(a[20] + i % 5));
        putTextByte(a[16], 0);
        for (u32 i = 0; i < a[17]; ++i) putTextByte(0x18 + i, u8(a[20] + 7 + i % 5));
        putTextByte(0x18 + a[17], 0);
        const u32 vec = a[11] == 0 ? u32(arenaB.Obj(a[4])) : self + a[18];
        for (unsigned k = 0; k < 3; ++k) { if (a[11] == 0) arenaB.Put32(a[4] + 4 * k, a[5 + k]); else arenaA.Put32(a[18] + o + 4 * k, a[5 + k]); }
        call.ecx = self;
        call.args[0] = vec; call.args[1] = text; call.argCount = 2;
        break;
    }
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data()))});
    if (kKind[fn] == kSetDist || kKind[fn] == kStartAnim) input["now"] = BitsOf(reinterpret_cast<const void*>(uintptr_t(kTimeGlobal)));   // CTimer::m_snTimeInMilliseconds tal cual esta
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
        if (kKind[fn] == kPlugin) {
            const u8* now = reinterpret_cast<const u8*>(uintptr_t(kPluginGlobal));
            for (unsigned i = 0; i < 4; ++i) if (now[i] != u8(c.a[4] >> (8 * i))) obs["gchanges"].push_back(json::array({kPluginGlobal + i, now[i]}));
        }
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kConst: obs["ret"] = eax; break;
        case kScratch: case kPlugin: case kAffects: case kTakes: case kEntryAdd: obs["ret"] = eax & 0xFFu; break;
        case kClone: case kSdd: case kIkCtor: case kAnyMeans: case kFxCtor: obs["ret"] = Tag(uintptr_t(eax)); break;
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN166A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN166A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn166a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn166a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn166a"); }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return; done = true;
    Env env;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = uintptr_t(GetModuleHandleA(nullptr));
    bool ready = true; json registered = json::object(); json bodies = json::object(); json collaborators = json::object(); json ownHooks = json::object();
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
    std::map<uintptr_t, Pair> collabCache;
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    auto H = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        Pair p = it->second;
        p.dll = Body(dllAddress); p.hooked = true;
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto X = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // vtables falsas (estaticas): tarea (ranura 4 = GetTaskType), evento propio (ranura 2 = GetEventPriority), evento ajeno (ranuras 1 y 2)
    vtTask.fill(nullptr); vtThis.fill(nullptr); vtRef.fill(nullptr);
    vtTask[4] = reinterpret_cast<void*>(&FkTaskType);
    vtThis[2] = reinterpret_cast<void*>(&FkPrioThis);
    vtRef[1] = reinterpret_cast<void*>(&FkEvType); vtRef[2] = reinterpret_cast<void*>(&FkPrioRef);
    gDllCb[0] = reinterpret_cast<uintptr_t>(&CTaskSimpleRunNamedAnim::FinishRunAnimMovePedCB);
    gDllCb[1] = reinterpret_cast<uintptr_t>(&CTaskSimpleAnim::FinishRunAnimCB);
    gDllCb[2] = reinterpret_cast<uintptr_t>(&TxdParentConstructor);
    gDllCb[3] = reinterpret_cast<uintptr_t>(&TxdParentDestructor);
    gDllCb[4] = reinterpret_cast<uintptr_t>(&TxdParentCopyConstructor);
    for (auto& cb : gDllCb) cb = Body(cb);
    const auto& K = kBodyFakes;
    auto Del = [&] { return F(kTaskDelete, &FkDelete<0x61A5B0>); };
    env.stubs[0] = {F(kTaskNew, &FkTaskNew), F(kFightCtor, &FkFightCtor)};
    env.stubs[1] = {F(0x61C530, K[1]), Del()};
    env.stubs[2] = {F(0x61DCA0, K[2]), Del()};
    env.stubs[3] = {F(0x61A390, K[3])};
    env.stubs[6] = {F(0x61A3B0, K[6]), F(kRegRef, &FkRegRef)};
    env.stubs[7] = {X(kResize, &FkResize)};
    env.stubs[8] = {X(kNewScalar, &FkNewScalar), F(kDirCtor, &FkDirCtor)};
    env.stubs[9] = {F(kRegPlugin, &FkRegPlugin)};
    env.stubs[11] = {X(kTexDestroy, &FkTexDestroy)};
    env.stubs[16] = {F(kGetActive, &FkGetActive), F(kIsPlayer, &FkIsPlayer), F(kIsAlive, &FkIsAlive)};
    env.stubs[18] = {F(kBlend, &FkBlend), F(kSetFinish, &FkSetCb<0x4CEBE0>), F(kSetDelete, &FkSetCb<0x4CEBC0>)};
    env.stubs[19] = {F(0x48E090, K[19]), Del()};
    env.stubs[20] = {H(0x46AD70, reinterpret_cast<uintptr_t>(&CTaskComplexSeekEntityStandard::DestructorBodyExe), K[20]), Del()};
    env.stubs[23] = {F(0x46AF10, K[23]), Del(), F(0x61A3A0, &FkBody<0x61A3A0>)};   // 0x46AF10 del EXE es `jmp 0x61A3A0` (CTask::Destructor, con hook): su ruta tambien se apaga (el falso de 0x61A3A0 no llega a ejecutarse)
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({env.stubs[f].empty() ? "toda la funcion (sin colaboradores)" : "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)"})},
                                 {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", nullptr}};
    }
    regions.clear();
    regions.emplace_back(kPluginGlobal, 4);
    for (auto& v : regionsOf) v.clear();
    regionsOf[9] = {0};
    Run(env, ready, registered, bodies, collaborators);
}
}
