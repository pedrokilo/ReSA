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

// Banco par a par Fn144A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 25 funciones diminutas del EXE (ver Fixtures.hpp para la lista y el esquema de familias), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall / RH_ScopedGlobalInstall) o
// V (RH_ScopedVMTInstall: ranura de la vtable del EXE; el Virtual lleva dentro un Simple sobre la entrada del EXE, que es lo que el banco enciende). El banco invoca la direccion del EXE con objetos
// sinteticos en arenas propias (A, B, C con guardas de 0x40) y, para SetCutsceneOffset, con la region global [0xBC4020, 0xBC4050) (se guarda y se restaura). Falsos de los colaboradores: el mismo falso en el
// EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor con hook (0x4DFB20, 0x4E0940, 0x4AA260, 0x506C50), SwitchOn (0x6F32C0), GetLeader (0x5F69A0), CAESound::Destructor (0x4EF660),
// RpAnimBlendClumpGetAssociation (0x4D68B0), CDecision::From (0x6006B0) y StopScanner (0x4E6DC0), todos con hook (apagado durante el banco); operator delete 0x8214BD y operator delete[] 0x8213AE (la DLL los
// llama por la direccion del EXE: falso solo en esa direccion). 0x4A8DB0 (constructor de ListItem_c, sin hook) corre real en el EXE y va en linea en la DLL.
// Se observan: los bytes cambiados de las tres arenas y de la region global, los eventos de los falsos (con sus argumentos), el valor devuelto (EAX completo, etiqueta de this, o AL), el estado x87 (vacio)
// y el contrato de ABI. EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F144ALive {
using namespace F144AFix;
inline constexpr uintptr_t kEntries[kFunctions] = {0x69FFA0, 0x69FFD0, 0x4D8670, 0x4D8690, 0x47C0E0, 0x47E050, 0x47E070, 0x4A9110, 0x4A9340, 0x4A94E0, 0x4A9C10, 0x4A9C20, 0x4B21F0,
                                                  0x4B2200, 0x4B2390, 0x4B23A0, 0x4E3350, 0x4E34C0, 0x630F50, 0x630FF0, 0x5BA020, 0x6E3E30, 0x506DC0, 0x507400, 0x5074B0};

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F144AFix::Arena(WIN);
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
inline u32 gDtorWrite{};             // dword que el falso del cuerpo del destructor deja en [this]
inline u32 gStampWord{};             // palabra que el falso de SwitchOn deja en +0x1A
inline DWORD gBenchThread{};
inline std::array<u8, kRegionBytes> gRegionRef{};   // imagen de la region global tras prepararla (referencia de los cambios)

// Cuerpo del destructor con borrado (thiscall, sin argumentos de pila): evento [direccion del EXE del cuerpo, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p); e.v[1] = BitsOf(p);
}
// operator delete[] (cdecl, un argumento CRUDO que puede no apuntar a memoria valida): evento [direccion del EXE, argumento (etiqueta o crudo)]. No lo desreferencia ni libera nada.
template<u32 ADDR> inline void __cdecl FkDeleteRaw(void* p) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(p);
}
// CCarGenerator::SwitchOn (0x6F32C0, thiscall sin pila): evento [direccion, ECX (etiqueta), dword previo de [ECX + 0x18]]; deja gStampWord en la palabra +0x1A (como el real deja 0xFFFF) para que se vea si
// quien llama escribe el contador despues.
inline u32 __fastcall FkSwitchOn(void* self, void*) {
    Ev& e = PushEv(0x6F32C0, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(static_cast<u8*>(self) + 0x18);
    const u16 w = u16(gStampWord);
    memcpy(static_cast<u8*>(self) + 0x1A, &w, 2);
    return gRet;
}
// CPedGroupMembership::GetLeader (0x5F69A0, thiscall sin pila): evento [direccion, ECX (etiqueta o crudo)]; devuelve gRet. No desreferencia ECX.
inline u32 __fastcall FkGetLeader(void* self, void*) {
    Ev& e = PushEv(0x5F69A0, 1);
    e.v[0] = Tag(self);
    return gRet;
}
// CAESound::Destructor (0x4EF660, thiscall sin pila) sobre m_tempSound (this + 8): evento [direccion, ECX (etiqueta), dword de [ECX - 8] al entrar (la vtable que dejo el cuerpo)]; devuelve gRet.
inline u32 __fastcall FkSoundDtor(void* self, void*) {
    Ev& e = PushEv(0x4EF660, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(static_cast<u8*>(self) - 8);
    return gRet;
}
// RpAnimBlendClumpGetAssociation (0x4D68B0, cdecl, clump y id de animacion): evento [direccion, clump (etiqueta o crudo), id]; devuelve gRet (puntero). No desreferencia el clump.
inline u32 __cdecl FkGetAssoc(void* clump, u32 id) {
    Ev& e = PushEv(0x4D68B0, 2);
    e.v[0] = Tag(clump); e.v[1] = id;
    return gRet;
}
// CDecision::From (0x6006B0, thiscall con un argumento de pila, `ret 4`): evento [direccion, ECX (etiqueta), origen (etiqueta o crudo)]; devuelve gRet. No desreferencia nada.
inline u32 __fastcall FkDecisionFrom(void* self, void*, void* src) {
    Ev& e = PushEv(0x6006B0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(src);
    return gRet;
}
// CAEPoliceScannerAudioEntity::StopScanner (0x4E6DC0, cdecl, un argumento): evento [direccion, (byte bajo del argumento != 0)]; el EXE empuja el dword completo y la DLL un bool; StopScanner solo lee el byte.
inline void __cdecl FkStopScanner(u32 a) {
    Ev& e = PushEv(0x4E6DC0, 1);
    e.v[0] = (a & 0xFFu) != 0 ? 1u : 0u;
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
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[12]); arenaB.Pattern(a[2], a[13]); arenaC.Pattern(a[2], a[14]);
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
    case kIdxKeyDtor: case kIdxDataDtor:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 4, a[4]);
        thisCall(0);
        break;
    case kIdxGenCount:
        arenaA.Put32(a[0] + 0x14, a[6]); arenaA.Put32(a[0] + 0x18, a[3]);
        gStampWord = a[5];
        thisCall(1); call.args[0] = a[4];
        break;
    case kIdxSurfB:
        arenaA.Put32(a[0] + 0x20, a[3]);
        thisCall(0);
        break;
    case kIdxCutOffset: {            // cdecl: un puntero; la region global se rellena con palabras de la semilla a[15]
        for (u32 k = 0; k < kRegionBytes / 4; ++k) {
            const u32 w = Rand(a[15], k);
            memcpy(reinterpret_cast<void*>(uintptr_t(kRegionAt) + 4 * k), &w, 4);
        }
        memcpy(gRegionRef.data(), reinterpret_cast<const void*>(uintptr_t(kRegionAt)), kRegionBytes);
        arenaA.Put32(a[0], a[4]); arenaA.Put32(a[0] + 4, a[5]); arenaA.Put32(a[0] + 8, a[6]);
        static constexpr int kDelta[8] = {0, 0, 4, -4, 8, -8, 12, -12};
        const u32 src = a[3] == 0 ? u32(arenaA.Obj(a[0])) : u32(int32(kOffsetAt) + kDelta[a[3]]);
        call.argCount = 1; call.args[0] = src; call.popBytes = 4;
        break;
    }
    case kIdxParticleCtor: case kIdxEmitterCtor:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 4, a[4]);
        thisCall(0);
        break;
    case kIdxWaterCtor: case kIdxPoolCtor: case kIdxPoolDtor:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 4, a[4]); arenaA.Put32(a[0] + 8, a[5]);
        thisCall(0);
        break;
    case kIdxAreaType: case kIdxAreaLife: case kIdxTiredType:
        arenaA.Put32(a[0], a[3]);
        thisCall(0);
        break;
    case kIdxLeaderPrio:
        arenaA.Put32(a[0], a[3]);
        thisCall(1); call.args[0] = a[4];
        break;
    case kIdxLeaderGroup: {
        const u32 ped = a[3] == 0 ? 0u : (a[3] == 1 ? a[4] : u32(arenaC.Obj(a[5])));
        arenaA.Put32(a[0] + 0xC, ped);
        const u32 group = a[6] == 0 ? u32(arenaB.Obj(a[7])) : a[8];
        switch (a[9]) {
        case 0: gRet = ped; break;
        case 1: gRet = u32(arenaC.Obj(a[10])); break;
        case 2: gRet = 0; break;
        default: gRet = a[11]; break;
        }
        thisCall(1); call.args[0] = group;
        break;
    }
    case kIdxSlotReset:
        arenaA.Put32(a[0] + 0x14, a[3]); arenaA.Put32(a[0] + 0x18, a[4]);
        thisCall(0);
        break;
    case kIdxSpeechDtor:
        arenaA.Put32(a[0], a[3]);
        gRet = a[4];
        thisCall(0);
        break;
    case kIdxTiredAbort: {           // [task + 0x18] = m_TiredAnim; [ped + 0x18] = clump
        const u32 anim = a[3] == 0 ? 0u : (a[3] == 1 ? u32(arenaC.Obj(a[4])) : a[5]);
        arenaA.Put32(a[0] + 0x18, anim);
        arenaB.Put32(a[6] + 0x18, a[7]);
        switch (a[8]) {
        case 0: gRet = a[3] == 1 ? anim : 0u; break;
        case 1: gRet = u32(arenaC.Obj(a[10])); break;
        case 2: gRet = 0; break;
        default: gRet = a[9]; break;
        }
        thisCall(3); call.args[0] = u32(arenaB.Obj(a[6])); call.args[1] = a[11]; call.args[2] = ~a[11];
        break;
    }
    case kIdxDecisionFrom: {
        gRet = 0;
        const u32 src = a[3] == 0 ? u32(arenaB.Obj(a[4])) : a[5];
        thisCall(1); call.args[0] = src;
        break;
    }
    case kIdxStopScanner:
        thisCall(1); call.args[0] = a[3]; call.ecx = c.ecx;
        break;
    case kIdxExplosionCtor:
        arenaA.Put32(a[0], a[3]); arenaA.Put32(a[0] + 0x10, a[4]); arenaA.Put32(a[0] + 0x7C, a[5]);
        thisCall(0);
        break;
    default:
        arenaA.Put32(a[0], a[3]);
        thisCall(0);
        break;
    }
    return call;
}

// valor devuelto observado: 0 = this (etiqueta), 1 = AL, 2 = EAX completo, 3 = void
inline int RetKind(unsigned fn) {
    switch (fn) {
    case kIdxBankSdd: case kIdxMp3Sdd: case kIdxFxSystemSdd: case kIdxWeaponSdd: case kIdxParticleCtor: case kIdxEmitterCtor: case kIdxPoolCtor: case kIdxWaterCtor: case kIdxExplosionCtor: return 0;
    case kIdxSurfB: case kIdxLeaderPrio: case kIdxLeaderGroup: case kIdxTiredAbort: return 1;
    case kIdxAreaType: case kIdxAreaLife: case kIdxTiredType: return 2;
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
        gRet = 0; gDtorWrite = 0; gStampWord = 0;
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}};
        json gch = json::array();
        if (fn == kIdxCutOffset) {
            const auto* now = reinterpret_cast<const u8*>(uintptr_t(kRegionAt));
            for (u32 i = 0; i < kRegionBytes; ++i) if (now[i] != gRegionRef[i]) gch.push_back(json::array({i, now[i]}));
        }
        obs["gchanges"] = gch;
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
// Casos por funcion: kCases salvo que la variable de entorno FN144A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN144A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn144a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn144a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn144a"); }
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
    env.stubs[kIdxKeyDtor] = {ByExeAddress(0x8213AE, &FkDeleteRaw<0x8213AE>)};
    env.stubs[kIdxDataDtor] = {ByExeAddress(0x8213AE, &FkDeleteRaw<0x8213AE>)};
    env.stubs[kIdxBankSdd] = {F(0x4DFB20, &FkBody<0x4DFB20>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kIdxMp3Sdd] = {F(0x4E0940, &FkBody<0x4E0940>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kIdxFxSystemSdd] = {F(0x4AA260, &FkBody<0x4AA260>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kIdxWeaponSdd] = {F(0x506C50, &FkBody<0x506C50>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[kIdxGenCount] = {F(0x6F32C0, &FkSwitchOn)};
    env.stubs[kIdxLeaderGroup] = {F(0x5F69A0, &FkGetLeader)};
    env.stubs[kIdxSpeechDtor] = {F(0x4EF660, &FkSoundDtor)};
    env.stubs[kIdxTiredAbort] = {F(0x4D68B0, &FkGetAssoc)};
    env.stubs[kIdxDecisionFrom] = {F(0x6006B0, &FkDecisionFrom)};
    env.stubs[kIdxStopScanner] = {F(0x4E6DC0, &FkStopScanner)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion (sin colaboradores)";
    const char* const sddHooked = "toda la funcion salvo el cuerpo del destructor (con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL) y operator delete (0x8214BD: la DLL lo llama por la direccion del EXE, falso)";
    for (const unsigned f : {kIdxBankSdd, kIdxMp3Sdd, kIdxFxSystemSdd, kIdxWeaponSdd}) dllReal[f] = sddHooked;
    dllReal[kIdxKeyDtor] = dllReal[kIdxDataDtor] = "toda la funcion salvo operator delete[] (0x8213AE: la DLL lo llama por la direccion del EXE, falso)";
    dllReal[kIdxGenCount] = "toda la funcion salvo CCarGenerator::SwitchOn (0x6F32C0, con hook, falso identico en la direccion del EXE y en el cuerpo de la DLL)";
    dllReal[kIdxLeaderGroup] = "toda la funcion salvo CPedGroupMembership::GetLeader (0x5F69A0, con hook, falso identico)";
    dllReal[kIdxSpeechDtor] = "toda la funcion salvo CAESound::Destructor (0x4EF660, con hook, falso identico)";
    dllReal[kIdxTiredAbort] = "toda la funcion salvo RpAnimBlendClumpGetAssociation (0x4D68B0, con hook, falso identico)";
    dllReal[kIdxDecisionFrom] = "toda la funcion salvo CDecision::From (0x6006B0, con hook, falso identico)";
    dllReal[kIdxStopScanner] = "toda la funcion salvo CAEPoliceScannerAudioEntity::StopScanner (0x4E6DC0, con hook, falso identico)";
    dllReal[kIdxParticleCtor] = dllReal[kIdxEmitterCtor] = dllReal[kIdxWaterCtor] = "toda la funcion (el constructor de ListItem_c, 0x4A8DB0, sin hook, va en linea en la DLL: sus escrituras se observan en los bytes del objeto)";
    dllReal[kIdxCutOffset] = "toda la funcion (escribe en la region global 0xBC4034, que se guarda y se restaura)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        if (f == kIdxParticleCtor || f == kIdxEmitterCtor || f == kIdxWaterCtor) env.watches.emplace_back(0x4A8DB0, 16);   // el EXE sigue llamando a este constructor de ListItem_c (sin hook): debe seguir intacto
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
    regions.clear();
    regions.emplace_back(kRegionAt, kRegionBytes);                 // SetCutsceneOffset: ms_cutsceneAssociations / ms_cutsceneOffset
    regionsOf[kIdxCutOffset] = {0};
    Run(env, ready, registered, bodies, collaborators);
}
}
