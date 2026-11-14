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

// Banco par a par Fn188A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 13 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias
// (A, B, C y D de 0x1000 bytes, guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): ~CEventAttractor (cuerpo 0x4AF400) y ~CEventAcquaintancePed
// (cuerpo 0x4AF890), CEvent::operator new 0x4B5620 y operator delete 0x4B5630, constructor de CEventAttractor 0x4AF350 y de CEventAcquaintancePed 0x4AF820 (todos con hook) y los cuerpos del propio lote (Rt: 0x5FEF70,
// 0x5FF030, 0x5FF0F0, 0x5FF1B0). No hay memoria global tocada: ninguna funcion del lote escribe fuera de los objetos.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos y el dword de [this] al entrar),
// el valor devuelto (punteros como etiqueta de arena; EAX completo en GetEventType) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F188ALive {
using namespace F188AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F188AFix::Arena(WIN);
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
inline Arena<kWinA> arenaA;
inline Arena<kWin> arenaB, arenaC, arenaD;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de constructor
inline u32 gRet2{};                  // EAX que devuelven los demas falsos del constructor de CEmergencyPed
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de CEvent::operator new (0 = sin memoria)
inline std::array<u32, 7> gStatusScr{}; inline u32 gReplayK{};                   // CReplay: XOR que cada llamada de los falsos deja en el estado de paneles
inline std::array<u32, 10> gNum{}; inline u32 gNumK{};                           // CPlane: lo que devuelve GetPlaneNumGuns en su llamada k
inline std::array<u32, 4> gKillKind{}, gKillVal{}; inline u32 gKillK{}; inline uintptr_t gTableField{}; inline u32 gAltTable{};   // CPlane: escritura del falso de Kill
inline uintptr_t gPed{}; inline u32 gPcFlags{}, gPcState{}, gPcType{}, gPcSlot{}, gU1State{}, gU1Slot{}, gU2Type{}, gU2Slot{}, gUMask{}, gUpdK{};   // CEmergencyPed::ProcessControl
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator delete[] del CRT (0x8213AE; cdecl, un argumento que puede ser cualquier puntero de la tabla): evento [direccion, argumento (etiqueta)]. No libera nada.
inline void __cdecl FkDeleteArr(void* p) {
    Ev& e = PushEv(0x8213AE, 1);
    e.v[0] = Tag(p);
}
// CEvent::operator new (0x4B5620; cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// Cuerpo del destructor / constructor sin argumentos de pila (thiscall): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Funcion thiscall sin argumentos que no toca memoria: evento [direccion, ECX (etiqueta)]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return gRet;
}
// Constructor con un argumento de pila (thiscall, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta), dword que tenia [ECX]]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor1(void* self, void*, u32 a) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self);
    e.v[1] = Tag(uintptr_t(a));
    e.v[2] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CEventAcquaintancePedHateBadlyLit (0x5FF250; thiscall, tres de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), persona (etiqueta), tiempo, punto (etiqueta), dword que tenia [ECX]]; deja
// gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtor3(void* self, void*, u32 ped, u32 time, void* point) {
    Ev& e = PushEv(0x5FF250, 5);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ped)); e.v[2] = time; e.v[3] = Tag(point); e.v[4] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CEventAttractor (0x4AF350; thiscall, tres de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), efecto (etiqueta), entidad (etiqueta), tercer argumento (dword completo), dword que tenia [ECX]]; deja
// gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtorAttr(void* self, void*, u32 effect, u32 entity, u32 avoid) {
    Ev& e = PushEv(0x4AF350, 5);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(effect)); e.v[2] = Tag(uintptr_t(entity)); e.v[3] = avoid; e.v[4] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEntity::RegisterReference (0x571B70) / CleanUpOldReference (0x571A00) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), referencia (etiqueta), dword de [referencia - 14h] (la vtable del evento)]. No escribe nada.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
    e.v[2] = BitsOf(static_cast<u8*>(ref) - 0x14);
}
// CAutomobile::SetPanelDamage (0x6B1480) / SetBumperDamage (0x6B1350) (thiscall, panel y bandera de pila, `ret 8`): evento [direccion, ECX (etiqueta), panel, bandera, estado de paneles al entrar]; despues escribe en el
// estado (+5B4h) el XOR gStatusScr[k] de su llamada k.
template<u32 ADDR> inline void __fastcall FkPanel(void* self, void*, u32 panel, u32 flag) {
    u32* status = reinterpret_cast<u32*>(static_cast<u8*>(self) + 0x5B4);
    Ev& e = PushEv(ADDR, 4);
    e.v[0] = Tag(self); e.v[1] = panel; e.v[2] = flag; e.v[3] = *status;
    if (gReplayK < gStatusScr.size()) *status ^= gStatusScr[gReplayK];
    ++gReplayK;
}
// CPed::SetModelIndex (0x5E4880; thiscall, un dword de pila, `ret 4`): evento [direccion, ECX (etiqueta), modelo (etiqueta), dword de [ECX]]; devuelve gRet2.
inline u32 __fastcall FkSetModel(void* self, void*, u32 model) {
    Ev& e = PushEv(0x5E4880, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(model)); e.v[2] = BitsOf(self);
    return gRet2;
}
// CAEPedSpeechAudioEntity::Initialise (0x4E68D0; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), ped (etiqueta)]; devuelve gRet2.
inline u32 __fastcall FkInit(void* self, void*, void* ped) {
    Ev& e = PushEv(0x4E68D0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ped);
    return gRet2;
}
// CPed::ProcessControl (0x5E8CD0, V; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]; deja banderas (+1Ch), estado (+530h), tipo (+598h) y slot (+718h, byte) del caso.
inline void __fastcall FkPedControl(void* self, void*) {
    Ev& e = PushEv(0x5E8CD0, 1);
    e.v[0] = Tag(self);
    u8* p = static_cast<u8*>(self);
    memcpy(p + 0x1C, &gPcFlags, 4); memcpy(p + 0x530, &gPcState, 4); memcpy(p + 0x598, &gPcType, 4); p[0x718] = u8(gPcSlot);
}
// CWeapon::Update (0x73DB40; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), propietario (etiqueta)]; tras la 1a llamada cambia el estado y/o el slot del ped, tras la 2a el tipo y/o el slot.
inline void __fastcall FkWeaponUpdate(void* self, void*, void* owner) {
    Ev& e = PushEv(0x73DB40, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(owner);
    u8* p = reinterpret_cast<u8*>(gPed);
    if (gUpdK == 0) { if (gUMask & 1) memcpy(p + 0x530, &gU1State, 4); if (gUMask & 2) p[0x718] = u8(gU1Slot); }
    else if (gUpdK == 1) { if (gUMask & 4) memcpy(p + 0x598, &gU2Type, 4); if (gUMask & 8) p[0x718] = u8(gU2Slot); }
    ++gUpdK;
}
// CVehicle::GetPlaneNumGuns (0x6D3F30; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]; devuelve gNum[k] de su llamada k (la ultima se repite).
inline u32 __fastcall FkNumGuns(void* self, void*) {
    Ev& e = PushEv(0x6D3F30, 1);
    e.v[0] = Tag(self);
    const u32 k = gNumK < gNum.size() ? gNumK : u32(gNum.size() - 1);
    ++gNumK;
    return gNum[k];
}
// FxSystem_c::Kill (0x4AA3F0; thiscall, sin argumentos): evento [direccion, ECX (etiqueta)]; en sus cuatro primeras llamadas puede escribir una entrada de la tabla de fogonazos vigente o cambiar el puntero +9D4h.
inline void __fastcall FkKill(void* self, void*) {
    Ev& e = PushEv(0x4AA3F0, 1);
    e.v[0] = Tag(self);
    const u32 k = gKillK++;
    if (k < 4) {
        const u32 what = gKillKind[k] & 0xFFu, slot = (gKillKind[k] >> 8) & 7u;
        u32 table{};
        memcpy(&table, reinterpret_cast<void*>(gTableField), 4);
        if (what == 1 && table) memcpy(reinterpret_cast<void*>(uintptr_t(table) + 4 * slot), &gKillVal[k], 4);
        else if (what == 2) memcpy(reinterpret_cast<void*>(gTableField), &gAltTable, 4);
    }
}
// FxManager_c::DestroyFxSystem (0x4A9810; thiscall con g_fxMan, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), sistema (etiqueta)].
inline void __fastcall FkDestroyFx(void* self, void*, void* fx) {
    Ev& e = PushEv(0x4A9810, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(fx);
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

// Prepara arenas del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[60]); arenaB.Pattern(a[2], a[61]); arenaC.Pattern(a[2], a[62]); arenaD.Pattern(a[2], a[63]);
    gRet = 0; gRet2 = 0; gDtorWrite = 0; gNew = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kConstType:
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kBodyJmp:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kEvClone:
        arenaA.Put32(o + 0x14, a[4]); arenaA.Put32(o + 0x18, a[5]);
        gNew = a[6] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        gRet = a[8]; gDtorWrite = a[9];
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
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = json::array();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kSdd: case kEvClone: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConstType: obs["ret"] = eax; break;                            // valor completo de EAX
        default: obs["ret"] = nullptr; break;                                // void
        }
                rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN188A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN188A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn188a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn188a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn188a"); }
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
    // Colaborador con hook V (la DLL lo llama de forma no virtual: el cuerpo es la funcion de la ranura)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabV(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (delete[] 0x8213AE, delete 0x8214BD): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH cuya funcion en la DLL es otra funcion propia (direccion dada): el falso va en la direccion del EXE y en esa funcion. Se exige que la direccion del EXE no tenga hook.
    auto Ext = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    env.stubs[0] = {F(0x4AF400, &FkBody<0x4AF400>)};
    env.stubs[2] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AF350, &FkCtorAttr)};
    env.stubs[3] = {Rt(0, &FkBody<0x5FEF70>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[4] = {F(0x4AF890, &FkBody<0x4AF890>)};
    env.stubs[5] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AF820, &FkCtor1<0x4AF820>)};
    env.stubs[6] = {Rt(4, &FkBody<0x5FF030>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[7] = {F(0x4AF890, &FkBody<0x4AF890>)};
    env.stubs[8] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AF820, &FkCtor1<0x4AF820>)};
    env.stubs[9] = {Rt(7, &FkBody<0x5FF0F0>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[10] = {F(0x4AF890, &FkBody<0x4AF890>)};
    env.stubs[11] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x4AF820, &FkCtor1<0x4AF820>)};
    env.stubs[12] = {Rt(10, &FkBody<0x5FF1B0>), F(0x4B5630, &FkDelete<0x4B5630>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {1u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Ninguna funcion del lote toca memoria global del juego (regions queda vacia)
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    Run(env, ready, registered, bodies, collaborators);
}
}
