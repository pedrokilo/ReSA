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

// Banco par a par Fn178A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 23 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos
// sinteticos en cuatro arenas propias (A de 0x5000 bytes, B, C y D de 0x1000, guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair):
// CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00, SetPanelDamage 0x6B1480 y SetBumperDamage 0x6B1350, CDummy 0x532540 / 0x532560 y operator delete 0x5326F0, CCopPed::Destructor 0x5DE0D0, CPed
// 0x5E8030 (constructor), 0x5E8620 (destructor), 0x5E4760 (operator delete) y 0x5E4880 / 0x5E8CD0 (V), CAEPedSpeechAudioEntity::Initialise 0x4E68D0, CWeapon::Update 0x73DB40, CPed::IsPedInControl 0x5E3960,
// CEventAcquaintancePed 0x4AF890, CEvent operator new 0x4B5620 y delete 0x4B5630, constructor de CEventAcquaintancePedHateBadlyLit 0x5FF250, GetPlaneNumGuns 0x6D3F30, FxSystem_c::Kill 0x4AA3F0, DestroyFxSystem 0x4A9810,
// CAEVehicleAudioEntity::Terminate 0x4FB8C0, CAutomobile::Destructor 0x6A61E0, CRideAnimData::Destructor 0x6D0B30 (todos con hook) y los cuerpos del lote (Rt: 0x59CBA0, 0x5DE300, 0x5DE3B0, 0x5FF2C0, 0x6C9160);
// solo por direccion del EXE (la DLL los llama por ella): delete[] del CRT 0x8213AE y operator delete del CRT 0x8214BD; CVehicle::operator delete 0x6E2D90 (sin hook: falso en la direccion del EXE y en la funcion
// de la DLL). No hay memoria global tocada: ninguna funcion del lote escribe fuera de los objetos.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos y, donde importa el
// orden, el dword de [this] al entrar), el valor devuelto (punteros como etiqueta de arena; EAX completo en GetEventType; solo AL en CanBeInterruptedBySameEvent) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la
// llamada quedan en original_residue / candidate_residue (informativos).

namespace F178ALive {
using namespace F178AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F178AFix::Arena(WIN);
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
    gStatusScr = {}; gReplayK = 0; gNum = {}; gNumK = 0; gKillKind = {}; gKillVal = {}; gKillK = 0; gTableField = 0; gAltTable = 0;
    gPed = 0; gPcFlags = gPcState = gPcType = gPcSlot = gU1State = gU1Slot = gU2Type = gU2Slot = gUMask = gUpdK = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kVtCtor:
    case kVtDtor: {
        const u32 veh = a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : a[6]);
        if (kKind[c.fn] == kVtCtor) { call.args[0] = veh; call.argCount = 1; }
        else arenaA.Put32(o + 0x14, veh);
        break;
    }
    case kImCtor:
    case kImDtor:
    case kWsBase:
    case kConstType:
    case kConstBool:
        break;
    case kReplay: {
        const u32 car = u32(arenaB.Obj(a[7]));
        arenaB.Put32(a[7] + 0x5B4, a[5]);
        for (u32 k = 0; k < 7; ++k) gStatusScr[k] = a[8 + k];
        call.args[0] = a[4]; call.args[1] = car; call.args[2] = a[6]; call.argCount = 3; call.popBytes = 12;
        call.ecx = c.ecx;
        break;
    }
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kDummyCtor:
    case kBodyJmp:
    case kQuadBody:
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kEmerCtor:
        gRet = a[6]; gDtorWrite = a[7]; gRet2 = a[8];
        call.args[0] = a[4]; call.args[1] = a[5]; call.argCount = 2;
        break;
    case kEmerPc:
        gPed = thisA;
        gPcFlags = a[4]; gPcState = a[5]; gPcType = a[6]; gPcSlot = a[7];
        gU1State = a[8]; gUMask = a[9]; gU1Slot = a[10]; gU2Type = a[11]; gU2Slot = a[12];
        break;
    case kHblClone:
        arenaA.Put32(o + 0x14, a[4]); arenaA.Put32(o + 0x18, a[5]);
        gNew = a[6] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        gRet = a[8]; gDtorWrite = a[9];
        break;
    case kPlaneBody: {
        const u32 table = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        arenaA.Put32(o + 0x9D4, table);
        for (u32 i = 0; i < 8; ++i) { arenaB.Put32(a[5] + 4 * i, a[6 + i]); arenaC.Put32(a[14] + 4 * i, a[15 + i]); }
        gAltTable = u32(arenaC.Obj(a[14]));
        for (u32 i = 0; i < 4; ++i) { arenaA.Put32(o + 0x9E8 + 4 * i, a[33 + i]); gKillKind[i] = a[38 + i]; gKillVal[i] = a[42 + i]; }
        arenaA.Put32(o + 0x9F8, a[37]);
        for (u32 k = 0; k < 10; ++k) gNum[k] = a[23 + k];
        gTableField = arenaA.Obj(o + 0x9D4);
        gDtorWrite = a[46]; gRet = a[47];
        break;
    }
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
        case kVtCtor: case kImCtor: case kSdd: case kDummyCtor: case kEmerCtor: case kHblClone: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConstType: obs["ret"] = eax; break;                                                                                          // valor completo de EAX
        case kConstBool: obs["ret"] = eax & 0xFFu; break;                                                                                  // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                              // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN178A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN178A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn178a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn178a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn178a"); }
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
    env.stubs[0] = {F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[1] = {F(0x571A00, &FkRef<0x571A00>)};
    env.stubs[4] = {F(0x6B1480, &FkPanel<0x6B1480>), F(0x6B1350, &FkPanel<0x6B1350>)};
    env.stubs[6] = {Rt(5, &FkBody<0x59CBA0>), ByExeAddress(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[7] = {F(0x5DE0D0, &FkBody<0x5DE0D0>), F(0x5E4760, &FkDelete<0x5E4760>)};
    env.stubs[8] = {F(0x532540, &FkBody<0x532540>)};
    env.stubs[9] = {F(0x532560, &FkBody<0x532560>)};
    env.stubs[10] = {Rt(9, &FkBody<0x5DE300>), F(0x5326F0, &FkDelete<0x5326F0>)};
    env.stubs[11] = {F(0x5E8030, &FkCtor1<0x5E8030>), FV(0x5E4880, &FkSetModel), F(0x4E68D0, &FkInit)};
    env.stubs[12] = {F(0x5E8620, &FkBody<0x5E8620>)};
    env.stubs[13] = {Rt(12, &FkBody<0x5DE3B0>), F(0x5E4760, &FkDelete<0x5E4760>)};
    env.stubs[14] = {FV(0x5E8CD0, &FkPedControl), F(0x73DB40, &FkWeaponUpdate), F(0x5E3960, &FkThis<0x5E3960>)};
    env.stubs[15] = {F(0x4AF890, &FkBody<0x4AF890>)};
    env.stubs[17] = {F(0x4B5620, &FkNew<0x4B5620>), F(0x5FF250, &FkCtor3)};
    env.stubs[19] = {Rt(15, &FkBody<0x5FF2C0>), F(0x4B5630, &FkDelete<0x4B5630>)};
    env.stubs[20] = {F(0x6D3F30, &FkNumGuns), F(0x4AA3F0, &FkKill), F(0x4A9810, &FkDestroyFx), ByExeAddress(0x8213AE, &FkDeleteArr), F(0x4FB8C0, &FkThis<0x4FB8C0>), F(0x6A61E0, &FkBody<0x6A61E0>)};
    env.stubs[21] = {Rt(20, &FkBody<0x6C9160>), Ext(0x6E2D90, reinterpret_cast<uintptr_t>(static_cast<void (*)(void*)>(&CVehicle::operator delete)), &FkDelete<0x6E2D90>)};
    env.stubs[22] = {F(0x6D0B30, &FkThis<0x6D0B30>), F(0x6A61E0, &FkBody<0x6A61E0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {2u, 3u, 5u, 16u, 18u}) dllReal[f] = "toda la funcion (sin colaboradores; InteriorManager_c corre los Interior_c y las listas reales: los del EXE en el original, los de la DLL en el candidato)";
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
