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
#include <intrin.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Fn196A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 25 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall / RH_ScopedOverloadedInstall) y V (RH_ScopedVMTInstall).
// El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores (el mismo falso en la direccion del EXE y en el cuerpo de la DLL):
// ~CVehicle 0x6E2B40, CEvent::operator new 0x4B5620 y delete 0x4B5630, ~CEventAcquaintancePed 0x4AF890, constructor de CEventAcquaintancePed 0x4AF820, constructor 0x4AC450 y cuerpo 0x4AC480 de CEventEditableResponse,
// CMatrix::Rotate 0x59B460, CPad::UpdatePads 0x541DD0 (entrada del EXE = salto permanente HookInstall a la DLL: Pair::permanent) y el cuerpo 0x5FF3A0 (otra funcion probada del lote, `Rt`); solo en la direccion
// del EXE (la DLL los llama por ella): RwEngineGetNumVideoModes 0x7F2CC0. IUnknown::Release de DirectInput: objeto COM sintetico con vtable propia. Memoria global del juego tocada (restaurada tras cada lado): almacen de
// vehiculos 0xB74494, estaticos de CGangWars 0x96AB44..0x96ABD4 y contadores de zonas 0xBA1DE8 / 0xBA3794, fabricante de CPU 0x8D6238, marca 0xA9B0B0, paso del reloj 0xB7CB5C y psGlobal 0xC17054.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue /
// candidate_residue (informativos).

namespace F196ALive {
using namespace F196AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F196AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
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
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de operator new (0 = sin memoria)
inline u32 gSwap{};                  // psGlobal nuevo que deja el falso de Release (0 = no lo cambia)
inline DWORD gBenchThread{};

// operator delete (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator new (cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// Cuerpo de destructor / constructor sin argumentos de pila (thiscall): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Funcion thiscall sin argumentos que no toca memoria (cuerpo de ~CVehicle sobre un puntero que no se desreferencia): evento [direccion, ECX (etiqueta)]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return gRet;
}
// Constructor con un argumento de pila (thiscall, `ret 4`): evento [direccion, ECX (etiqueta), argumento (crudo)]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor1(void* self, void*, u32 a) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = a;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// RwEngineGetNumVideoModes (0x7F2CC0; cdecl, sin argumentos): evento [direccion]; devuelve gRet.
inline u32 __cdecl FkNumModes() {
    PushEv(0x7F2CC0, 0);
    return gRet;
}
// CPad::UpdatePads (0x541DD0; cdecl, sin argumentos): evento [direccion].
inline void __cdecl FkUpdatePads() {
    PushEv(0x541DD0, 0);
}
// CMatrix::Rotate(CVector) (0x59B460; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), x, y, z (bits)]; devuelve gRet.
inline u32 __fastcall FkRotate(void* self, void*, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(0x59B460, 4);
    e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z;
    return gRet;
}
// IUnknown de DirectInput (stdcall): Release = ranura 2 (evento [0xD1, objeto (etiqueta)]; si gSwap, cambia psGlobal); las otras ranuras anotan una llamada inesperada [0xD0 + k, objeto]
inline std::array<uintptr_t, 3> comVt{};
inline u32 __stdcall FkRelease(void* self) {
    Ev& e = PushEv(0xD1, 1);
    e.v[0] = Tag(self);
    if (gSwap) *reinterpret_cast<u32*>(kPsGlobal) = gSwap;
    return gRet;
}
inline u32 __stdcall FkQueryInterface(void* self, void*, void*) { Ev& e = PushEv(0xD0, 1); e.v[0] = Tag(self); return 0; }
inline u32 __stdcall FkAddRef(void* self) { Ev& e = PushEv(0xD2, 1); e.v[0] = Tag(self); return 0; }
inline void InitComVt() { comVt = {uintptr_t(&FkQueryInterface), uintptr_t(&FkAddRef), uintptr_t(&FkRelease)}; }

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

// Funciones cuya prueba negativa se condiciona a la marca 0x5A en 0xA9B0B0 (el juego real tiene ahi 0 o 1: ms_bCreateDamagedVersion): las que no tienen `this` propio que desalinear
inline constexpr bool UsesMarker(Kind k) { return k == kCpuExt || k == kCpuVend || k == kVid || k == kNop || k == kThunk || k == kTime || k == kInShut; }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[60]); arenaB.Pattern(a[2], a[61]); arenaC.Pattern(a[2], a[62]); arenaD.Pattern(a[2], a[63]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gSwap = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kConst:
    case kThunk:
    case kCpuExt:
    case kNop:
        break;
    case kVDmg:
        for (u32 i = 0; i < 6; ++i) call.args[i] = a[4 + i];
        call.argCount = 6;   // `ret 18h`: limpia el callee
        break;
    case kVDel:
    case kVSdd: {
        const u32 base = a[6], idx = a[5];
        const u32 ptr = base + idx * kVehSize;                  // no se desreferencia
        const u32 map = u32(arenaD.Obj(a[7])) - idx;            // mapa + indice = el byte de estado en D
        arenaB.Put32(a[4] + 0, base); arenaB.Put32(a[4] + 4, map); arenaB.Put32(a[4] + 8, a[9]); arenaB.Put32(a[4] + 0xC, a[8]);
        arenaD.m[kGuard + a[7]] = u8(a[10]);
        *reinterpret_cast<u32*>(kVehPool) = u32(arenaB.Obj(a[4]));
        if (kKind[c.fn] == kVDel) {
            call.args[0] = ptr; call.args[1] = a[11];
            call.argCount = c.fn == 2 ? 1 : 2; call.popBytes = 4 * call.argCount;
        } else {
            call.ecx = ptr;
            call.args[0] = a[11]; call.argCount = 1;            // `ret 4`: limpia el callee
        }
        break;
    }
    case kPiCon:
        arenaB.Put16(a[4] + 0x144, a[5]); arenaB.Put16(a[4] + 0x130, a[6]); arenaB.Put16(a[4] + 0x156, a[7]);
        call.args[0] = u32(arenaB.Obj(a[4])); call.argCount = 1;   // `ret 4`
        break;
    case kPiExt:
        arenaA.Put32(o + 0x10, a[5]); arenaA.Put16(o + 4, a[6]); arenaA.Put16(o + 0x26, a[7]);
        call.args[0] = u32(arenaB.Obj(a[4])); call.argCount = 1;   // `ret 4`
        break;
    case kGwCon:
        FillKind(reinterpret_cast<u8*>(kGwBase), kGwBytes, a[2], a[63]);
        *reinterpret_cast<u32*>(0x96AB8C) = a[10]; *reinterpret_cast<u32*>(0x96AB88) = a[11];
        *reinterpret_cast<u16*>(kZoneCount1) = u16(a[6]); *reinterpret_cast<u16*>(kZoneCount2) = u16(a[9]);
        break;
    case kGwExt:
        FillKind(reinterpret_cast<u8*>(kGwBase), kGwBytes, a[2], a[63]);
        arenaA.Put32(o + 0xC, a[10]); arenaA.Put32(o + 0x10, a[11]);
        break;
    case kCpuVend:
        FillKind(reinterpret_cast<u8*>(kCpuVendor), 12, a[2], a[63]);
        break;
    case kVid:
        gRet = a[4];
        break;
    case kPadDrunk:
        call.args[0] = a[4]; call.argCount = 1;   // `ret 4`
        break;
    case kPadSub:
        arenaA.Put16(o + 0x10A, a[4]); arenaA.Put16(o + 0x12, a[5]); arenaA.Put16(o + 0x42, a[6]);
        break;
    case kRotPtr:
        arenaB.Put32(a[7], a[4]); arenaB.Put32(a[7] + 4, a[5]); arenaB.Put32(a[7] + 8, a[6]);
        gRet = a[8];
        call.args[0] = u32(arenaB.Obj(a[7])); call.argCount = 1;   // `ret 4`
        break;
    case kTime:
        *reinterpret_cast<u32*>(kTimeStep) = a[5];
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;   // cdecl
        break;
    case kInShut: {
        const u32 mode = a[4];
        arenaB.Put32(a[5] + 0x18, mode == 0 ? 0u : u32(arenaC.Obj(a[6])));
        arenaC.Put32(a[6], u32(uintptr_t(comVt.data())));
        arenaD.Put32(a[7] + 0x18, a[9]);
        *reinterpret_cast<u32*>(kPsGlobal) = u32(arenaB.Obj(a[5]));
        gSwap = mode == 2 ? u32(arenaD.Obj(a[7])) : 0u;
        gRet = a[8];
        break;
    }
    case kBody:
    case kCtor:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kCloneSc:
        arenaA.Put32(o + 0x14, a[8]);
        gNew = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[6]; gDtorWrite = a[7];
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    }
    if (UsesMarker(kKind[c.fn])) *reinterpret_cast<u8*>(kFlag) = 0x5A;   // marca que la prueba negativa (mutate.py) lee para condicionar sus mutaciones a algo que el juego real nunca produce
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n, const json& cpuid) {
    const C c = MakeC(fn, n);
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    input["cpuid"] = cpuid;   // CPUID medida por el banco con __cpuid (maxima extendida, EDX de 0x80000001, EBX/EDX/ECX del fabricante): el oraculo no puede ejecutar CPUID
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
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kVSdd: case kCtor: case kCloneSc: case kSdd: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: case kCpuExt: case kVid: obs["ret"] = eax; break;                              // valor completo de EAX
        case kPadSub: obs["ret"] = eax & 0xFFu; break;                                              // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                        // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN196A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN196A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
// CPUID de la maquina, medida con el intrinseco del compilador (independiente de las funciones probadas): [hoja maxima extendida, EDX de 0x80000001 (0 si no hay), EBX, EDX, ECX de la hoja 0]
inline json MeasureCpuid() {
    int r[4]{};
    __cpuid(r, int(0x80000000));
    const u32 maxExt = u32(r[0]);
    u32 extEdx = 0;
    if (maxExt >= 0x80000001u) { __cpuid(r, int(0x80000001)); extEdx = u32(r[3]); }
    __cpuid(r, 0);
    return json::array({maxExt, extEdx, u32(r[1]), u32(r[3]), u32(r[2])});
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    const json cpuid = MeasureCpuid();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}, {"cpuid", cpuid}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            const unsigned before = totals.failures;
            for (unsigned c = 0; c < nCases; ++c) CaseRun(env, totals, f, c, cpuid);
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
    if (!std::filesystem::exists(dir / "fn196a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn196a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn196a"); }
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (RwEngineGetNumVideoModes 0x7F2CC0): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // CPad::UpdatePads (0x541DD0): la entrada del EXE es un salto PERMANENTE (HookInstall, sin hook reversible) a la funcion de la DLL, que llama el cuerpo del hook del lote: el falso va en la entrada del EXE
    // y en esa funcion de la DLL (el destino del salto).
    auto Perm = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;   // se espera que NO sea un hook reversible
        Pair p{exe, Resolve(exe), true, true};
        if (!p.dll || p.dll == exe || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    InitComVt();
    env.stubs[4] = {F(0x6E2B40, &FkThis<0x6E2B40>)};
    env.stubs[11] = {ByExeAddress(0x7F2CC0, &FkNumModes)};
    env.stubs[15] = {Perm(0x541DD0, &FkUpdatePads)};
    env.stubs[16] = {F(0x59B460, &FkRotate)};
    env.stubs[19] = {F(0x4AF890, &FkBody<0x4AF890>)};
    env.stubs[21] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AF820, &FkCtor1<0x4AF820>)};
    env.stubs[22] = {Rt(19, &FkBody<0x5FF3A0>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[23] = {F(0x4AC450, &FkBody<0x4AC450>)};
    env.stubs[24] = {F(0x4AC480, &FkBody<0x4AC480>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 1u, 2u, 3u, 5u, 6u, 7u, 8u, 9u, 10u, 12u, 13u, 14u, 17u, 18u, 20u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[18] = "toda la funcion; IUnknown::Release es un objeto COM sintetico con vtable propia (la misma en ambos lados)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}, {"permanent", s.pair.permanent}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada; se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kVehPool, 4); regionsOf[2] = regionsOf[3] = regionsOf[4] = {regions.size() - 1};
    regions.emplace_back(kGwBase, kGwBytes); regions.emplace_back(kZoneCount1, 2); regions.emplace_back(kZoneCount2, 2);
    regionsOf[7] = regionsOf[8] = {regions.size() - 3, regions.size() - 2, regions.size() - 1};
    regions.emplace_back(kCpuVendor, 12); regionsOf[10] = {regions.size() - 1};
    regions.emplace_back(kFlag, 1); const size_t flagRegion = regions.size() - 1;
    regions.emplace_back(kTimeStep, 4); regionsOf[17] = {regions.size() - 1};
    regions.emplace_back(kPsGlobal, 4); regionsOf[18] = {regions.size() - 1};
    for (unsigned f : {9u, 10u, 11u, 14u, 15u, 17u, 18u}) regionsOf[f].push_back(flagRegion);
    Run(env, ready, registered, bodies, collaborators);
}
}
