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
#include "X87Math.h"
#include "Core.hpp"
#include "AnimBlendAssociation.h"
#include "TaskSimpleDuck.h"

#ifndef PEDCTOR_CALL_INDEX
#define PEDCTOR_CALL_INDEX 0   // indice, entre las llamadas directas del envoltorio CPed::Constructor, de la que va al constructor de C++ (se fija tras desensamblar la DLL)
#endif

// Banco par a par Fn187A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 19 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): cuerpos de destructor (0x6093B0, 0x690BB0,
// 0x6926F0 y, desde este, 0x692030), operator delete (0x5E4760 de CPed, 0x61A5B0 de CTask), CTask::operator new 0x61A5A0, constructores de eventos (0x4AF670, 0x4AFB50, 0x4AFCF0, 0x4B0700), de CPed (0x5E8030) y de
// CTaskSimpleDuckWhileShotsWhizzing (0x692680), metodos de CPlayerPedData/CPed/CPedGroups/CPedGroup/CAEPedSpeechAudioEntity/CAnimManager/CAnimBlendAssociation/CStats y los virtuales CTaskSimpleDuck::MakeAbortable (0x692100)
// y ProcessPed (0x694390). El EXE lleva en linea el CEventPool::New (0x4B5570 sobre el pool 0xB744AC; falso solo en esa direccion del EXE) y la DLL llama a CEvent::operator new (0x4B5620; falso solo en su cuerpo de la DLL):
// ambos anotan el mismo evento. Corre real, en el EXE y en la DLL, el constructor base de CTask (0x61A390, sin hook; en la DLL va en linea). Memoria global tocada (restaurada tras cada lado): el puntero del pool de eventos
// 0xB744AC, la hora 0xB7CB84, los datos de los jugadores 0xB7CD9C, el byte +0x2D0 de los grupos 0xC09BF0 y el indice de bloque de "playidles" 0xC0BC10.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como
// etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue.

namespace F187ALive {
using namespace F187AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F187AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u16 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    u32 Get32(size_t off) const { u32 v; memcpy(&v, m.data() + kGuard + off, 4); return v; }
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
inline u32 gNew{};                   // memoria que devuelve el falso de operator new / del pool (0 = sin memoria)
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gEvType{};                // lo que devuelve el GetEventType falso
inline u32 gGroup{}, gIdles{}, gFatBits{};   // AddGroup, indice del bloque "playidles", bits de la salud (CStats::GetFatAndMuscleModifier)
inline std::array<u32, 2> gPoison{}; // dwords que el falso del constructor de CPed deja en +0 y +4
inline uintptr_t gDllDefaultCb{}, gDllDeleteCb{};   // direcciones en la DLL de CDefaultAnimCallback::DefaultAnimCB y CTaskSimpleDuck::DeleteDuckAnimCB
inline DWORD gBenchThread{};
inline constexpr u32 kEvGetType = 0xE7A1;

// Metodo de otro objeto con 0, 1, 2 y 3 dwords de pila (thiscall, el callee limpia): evento [direccion del EXE, ECX (etiqueta), argumentos crudos]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkT0(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkT1(void* self, void*, u32 a1) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = a1;
    return gRet;
}
// idem con el argumento tambien como etiqueta (un puntero a un objeto de una arena)
template<u32 ADDR> inline u32 __fastcall FkT1P(void* self, void*, u32 a1) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(a1));
    return gRet;
}
// CTaskSimpleDuck::MakeAbortable (0x692100; ped, prioridad, evento): evento [direccion, ECX, ped (etiqueta), prioridad, evento (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkMakeAbortable(void* self, void*, u32 ped, u32 priority, u32 event) {
    Ev& e = PushEv(0x692100, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ped)); e.v[2] = priority; e.v[3] = Tag(uintptr_t(event));
    return gRet;
}
// CTaskSimpleDuck::ProcessPed (0x694390; ped): evento [direccion, ECX, ped (etiqueta)]; devuelve gNew.
inline u32 __fastcall FkProcessPed(void* self, void*, u32 ped) {
    Ev& e = PushEv(0x694390, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ped));
    return gNew;
}
// CAnimBlendAssociation::SetFinishCallback / SetDeleteCallback (0x4CEBE0 / 0x4CEBC0; devolucion de llamada y datos): evento [direccion, ECX, devolucion de llamada (0x4D1390 o 0x692550 si es la del EXE o la de la DLL), datos (etiqueta)]
inline u32 NormCb(uintptr_t cb, uintptr_t exe, uintptr_t dll) { return cb == exe || cb == dll ? u32(exe) : u32(cb); }
inline void __fastcall FkSetFinishCb(void* self, void*, uintptr_t cb, u32 data) {
    Ev& e = PushEv(0x4CEBE0, 3);
    e.v[0] = Tag(self); e.v[1] = NormCb(cb, 0x4D1390, gDllDefaultCb); e.v[2] = Tag(uintptr_t(data));
}
inline void __fastcall FkSetDeleteCb(void* self, void*, uintptr_t cb, u32 data) {
    Ev& e = PushEv(0x4CEBC0, 3);
    e.v[0] = Tag(self); e.v[1] = NormCb(cb, 0x692550, gDllDeleteCb); e.v[2] = Tag(uintptr_t(data));
}
// GetEventType virtual del evento de MakeAbortable (ranura 1; sin argumentos de pila): evento [marca, ECX (etiqueta)]; devuelve gEvType.
inline u32 __fastcall FkEvType(void* self, void*) {
    Ev& e = PushEv(kEvGetType, 1);
    e.v[0] = Tag(self);
    return gEvType;
}
// CAnimManager::BlendAnimation (0x4D4610; cdecl: clump, grupo, animacion, float como dword): evento [direccion, clump, grupo, animacion, bits del float]; devuelve gRet.
inline u32 __cdecl FkBlend(u32 clump, u32 group, u32 id, u32 bits) {
    Ev& e = PushEv(0x4D4610, 4);
    e.v[0] = Tag(uintptr_t(clump)); e.v[1] = group; e.v[2] = id; e.v[3] = bits;
    return gRet;
}
// CPedGroups::AddGroup (0x5FB800; cdecl sin argumentos): evento [direccion]; devuelve gGroup.
inline u32 __cdecl FkAddGroup() {
    PushEv(0x5FB800, 0);
    return gGroup;
}
// CAnimManager::GetAnimationBlockIndex(const char*) (0x4D3990; cdecl): evento [direccion, hash de la cadena, longitud]; devuelve gIdles.
inline u32 __cdecl FkAnimIndex(const char* name) {
    Ev& e = PushEv(0x4D3990, 2);
    e.v[0] = FnvStr(name); e.v[1] = name ? u32(strlen(name)) : 0u;
    return gIdles;
}
// CStats::GetFatAndMuscleModifier(int) (0x559AF0; cdecl; devuelve un float en ST0): evento [direccion, argumento]; devuelve los bits de gFatBits como float (fld de 32 bits: sin cambiar los bits).
inline float __cdecl FkFat(int id) {
    Ev& e = PushEv(0x559AF0, 1);
    e.v[0] = u32(id);
    float f; memcpy(&f, &gFatBits, 4);
    return f;
}
// CPed::CPed (0x5E8030; un dword de pila: tipo): evento [direccion, ECX, tipo]; deja gPoison en +0 y +4; devuelve gRet.
inline u32 __fastcall FkPedCtor(void* self, void*, u32 type) {
    Ev& e = PushEv(0x5E8030, 2);
    e.v[0] = Tag(self); e.v[1] = type;
    memcpy(self, &gPoison[0], 4); memcpy(static_cast<u8*>(self) + 4, &gPoison[1], 4);
    return gRet;
}
// Constructor de un evento con un dword de pila (0x4AF670, 0x4AFB50, 0x4B0700, 0x692680): evento [direccion, ECX (etiqueta), argumento]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor1(void* self, void*, u32 a1) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = a1;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CEventKnockOffBike (0x4AFCF0; 11 dwords de pila): evento [direccion, ECX, vehiculo, &moveSpeed (etiqueta), &collisionImpactVelocity (etiqueta), danio, a6, tipo, direccion, tiempo, ped, conductor, forzar]
inline u32 __fastcall FkKnockCtor(void* self, void*, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10) {
    Ev& e = PushEv(0x4AFCF0, 12);
    e.v[0] = Tag(self); e.v[1] = a0; e.v[2] = Tag(uintptr_t(a1)); e.v[3] = Tag(uintptr_t(a2)); e.v[4] = a3; e.v[5] = a4; e.v[6] = a5; e.v[7] = a6; e.v[8] = a7; e.v[9] = a8; e.v[10] = a9; e.v[11] = a10;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Cuerpo de destructor sin argumentos de pila (thiscall): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
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
// CEventPool::New en el EXE (0x4B5570; ECX = pool, sin argumentos de pila): evento [direccion, ECX (etiqueta)]; devuelve gNew.
inline u32 __fastcall FkPoolNewExe(void* pool, void*) {
    Ev& e = PushEv(0x4B5570, 1);
    e.v[0] = Tag(pool);
    return gNew;
}
// CEvent::operator new en la DLL (cuerpo; cdecl con el tamano): la DLL no pasa el pool: se anota el valor actual del puntero global 0xB744AC, que es lo que el EXE pone en ECX; mismo evento; devuelve gNew.
inline u32 __cdecl FkPoolNewDll(u32 /*size*/) {
    Ev& e = PushEv(0x4B5570, 1);
    e.v[0] = Tag(uintptr_t(BitsOf(reinterpret_cast<const void*>(uintptr_t(kEventPoolGlobal)))));
    return gNew;
}
inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }

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

// indices de las regiones globales (ver Update)
inline constexpr size_t kRegPool = 0, kRegTimer = 1, kRegPlayers = 2, kRegGroups = 3, kRegIdles = 4;

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[kSeed]); arenaB.Pattern(a[2], a[kSeed + 1]); arenaC.Pattern(a[2], a[kSeed + 2]); arenaD.Pattern(a[2], a[kSeed + 3]);
    gRet = 0; gNew = 0; gDtorWrite = 0; gEvType = 0; gGroup = 0; gIdles = 0; gFatBits = 0; gPoison = {};
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    const auto setPool = [&](u32 mode, u32 val) { PutRaw32(kEventPoolGlobal, mode ? u32(arenaD.Obj(val)) : val); };
    switch (kKind[c.fn]) {
    case kConst:
    case kVoid:
        arenaA.Put32(o, a[3]);
        break;
    case kEvClone1:
        arenaA.Put32(o + kFieldOff[c.fn], a[3]);
        setPool(a[4], a[5]);
        gNew = a[6] == 0 ? 0u : u32(arenaB.Obj(a[7]));
        gRet = a[8]; gDtorWrite = a[9];
        break;
    case kEvKnock:
        arenaA.Put32(o + 0x24, a[3]); arenaA.Put32(o + 0x28, a[4]); arenaA.Put32(o + 0x2C, a[5]); arenaA.Put32(o + 0x30, a[6]); arenaA.Put32(o + 0x38, a[7]); arenaA.Put32(o + 0x34, a[8]);
        for (u32 i = 0; i < 3; ++i) { arenaA.Put32(o + 0xC + 4 * i, a[9 + i]); arenaA.Put32(o + 0x18 + 4 * i, a[12 + i]); }
        setPool(a[15], a[16]);
        gNew = a[17] == 0 ? 0u : u32(arenaB.Obj(a[18]));
        gRet = a[19]; gDtorWrite = a[20];
        break;
    case kEvInAir:
        setPool(a[3], a[4]);
        gNew = a[5] == 0 ? 0u : u32(arenaB.Obj(a[6]));
        break;
    case kSdd:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kPlayerCtor:
        FillKind(regions[kRegPlayers].Ptr(), regions[kRegPlayers].bytes, a[2], a[kGSeed]);
        FillKind(regions[kRegGroups].Ptr(), regions[kRegGroups].bytes, a[2], a[kGSeed + 1]);
        memset(regions[kRegIdles].Ptr(), 0, 4);
        arenaA.Put32(o + 0x47C, u32(arenaC.Obj(a[9])));
        arenaA.Put32(o + 0x71C, a[5]);
        gRet = a[10]; gGroup = a[6]; gIdles = a[7]; gFatBits = a[8]; gPoison = {a[11], a[12]};
        call.args[0] = a[3]; call.args[1] = a[4]; call.argCount = 2;
        break;
    case kDuckCtor:
        PutRaw32(kTimerGlobal, a[4]);
        call.args[0] = a[3]; call.argCount = 1;
        break;
    case kDuckBody:
        arenaA.Put32(o, a[3]);
        gDtorWrite = a[5];
        break;
    case kMakeAbort: {
        const u32 ped = u32(arenaB.Obj(a[3]));
        const u32 event = a[5] ? u32(arenaC.Obj(a[6])) : 0u;
        arenaB.Put32(a[3] + 0x46C, a[11]);
        arenaA.Put32(o + 0x10, a[8]); arenaA.Put32(o + 0x14, a[9]);
        if (a[5]) {
            arenaC.Put32(a[6], u32(arenaC.Obj(kVtOff)));
            arenaC.Put32(kVtOff + 4, u32(reinterpret_cast<uintptr_t>(&FkEvType)));
        }
        gRet = a[10]; gEvType = a[7];
        call.args[0] = ped; call.args[1] = a[4]; call.args[2] = event; call.argCount = 3;
        break;
    }
    case kTaskClone:
        arenaA.Put32(o + 0xC, a[3]);
        gNew = a[5] == 0 ? 0u : u32(arenaB.Obj(a[6]));
        gRet = a[7]; gDtorWrite = a[8];
        break;
    case kProcessPed: {
        const u32 ped = u32(arenaB.Obj(a[3]));
        const u32 clump = a[4] ? u32(arenaD.Obj(a[8])) : 0u;
        if (a[4]) arenaD.Put8(a[8], 2);
        arenaB.Put32(a[3] + 0x18, clump);
        arenaA.Put32(o + 0x10, a[5]);
        gRet = a[6]; gNew = a[7];   // BlendAnimation devuelve gRet; el falso de CTaskSimpleDuck::ProcessPed devuelve gNew
        call.args[0] = ped; call.argCount = 1;
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
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kEvClone1: case kEvKnock: case kEvInAir: case kSdd: case kPlayerCtor: case kDuckCtor: case kTaskClone: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                                                                             // valor completo de EAX
        case kMakeAbort: case kProcessPed: obs["ret"] = eax & 0xFFu; break;                                                                               // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                                             // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN187A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN187A_CASES", buffer, sizeof(buffer)) > 0) {
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
// Igual que MakeCollab para un colaborador con hook VIRTUAL (ranura de vtable): el cuerpo de la DLL es la entrada de la vtabla
inline Pair MakeCollabVirtual(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name; uintptr_t vbody = 0;
    if (FindHook(exe, cat, name, &vbody) && vbody) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, vbody));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}, {"virtual", true}});
        if (env.collab.back()->Registered()) { p.dll = Body(env.collab.back()->Body()); p.hooked = true; }
    }
    return p;
}
// Destino de la primera llamada directa (E8) del cuerpo de un envoltorio de la DLL, contando solo las que caen dentro del ASI y no son llamadas a ayudantes de depuracion cortos: el constructor `CPed::CPed` que el
// envoltorio `CPed::Constructor` llama y que el constructor de C++ de CPlayerPed tambien llama (la DLL no pasa por la direccion del EXE ni por el envoltorio). `skip` = cuantas llamadas del ASI se ignoran antes.
inline std::vector<uintptr_t> CallTargets(uintptr_t body, size_t scan = 160) {
    std::vector<uintptr_t> out;
    const auto* p = reinterpret_cast<const uint8*>(body);
    for (size_t i = 0; i + 5 <= scan; ++i) {
        if (p[i] == 0xE8) out.push_back(Resolve(body + i + 5 + *reinterpret_cast<const int32*>(p + i + 1)));
        if (p[i] == 0xC3 || p[i] == 0xC2) break;
    }
    return out;
}

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn187a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn187a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn187a"); }
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
    gDllDefaultCb = std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB);
    gDllDeleteCb = std::bit_cast<uintptr_t>(&CTaskSimpleDuck::DeleteDuckAnimCB);
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Igual que F para un colaborador cuyo hook RH es VIRTUAL (ranura de vtable)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabVirtual(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL con hook RH en la DLL: se crea su ruta (hook apagado); devuelve su par
    auto Route = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return it->second;
    };
    // Pool de eventos: el EXE lleva en linea CEventPool::New (0x4B5570, falso solo en la direccion del EXE) y la DLL llama a CEvent::operator new (0x4B5620, falso solo en su cuerpo de la DLL)
    const Pair poolExe = Route(0x4B5570), newDll = Route(0x4B5620);
    if (!poolExe.hooked || !newDll.hooked || !newDll.dll || ModuleOf(newDll.dll) != env.asi) ready = false;
    const StubSpec poolStubExe = S(Pair{0x4B5570, 0, false}, reinterpret_cast<void*>(&FkPoolNewExe));
    const StubSpec poolStubDll = S(Pair{0, newDll.dll, false}, reinterpret_cast<void*>(&FkPoolNewDll));
    // CPed::CPed: el envoltorio CPed::Constructor (0x5E8030) llama al constructor de C++; el constructor de CPlayerPed lo llama directamente. Se parchea ese destino. (Ver README: como se identifica.)
    Pair pedCtor = Route(0x5E8030);
    const auto pedTargets = CallTargets(pedCtor.dll);
    json pedTargetsJson = json::array(); for (const auto t : pedTargets) pedTargetsJson.push_back(t);
    constexpr size_t kPedCtorCallIndex = PEDCTOR_CALL_INDEX;
    if (pedTargets.size() <= kPedCtorCallIndex || ModuleOf(pedTargets[kPedCtorCallIndex]) != env.asi) ready = false;
    else pedCtor.dll = pedTargets[kPedCtorCallIndex];
    hooksFound.push_back({{"exe", 0x5E8030}, {"ped_ctor_call_targets", pedTargetsJson}, {"ped_ctor_dll", pedCtor.dll}});
    // CPedGroups::AddGroup (0x5FB800): el hook de la DLL es la envoltura AddGroupExe; el constructor de CPlayerPed llama a CPedGroups::AddGroup (otra funcion): el falso va en la direccion del EXE y en esa funcion
    Pair addGroup = Route(0x5FB800);
    addGroup.dll = Body(std::bit_cast<uintptr_t>(&CPedGroups::AddGroup));
    if (!addGroup.hooked || ModuleOf(addGroup.dll) != env.asi) ready = false;
    // CStats::GetFatAndMuscleModifier (0x559AF0): el constructor de CPlayerPed llama a la funcion de la clase; el falso va en la direccion del EXE y en esa funcion
    Pair fatMod = Route(0x559AF0);
    fatMod.dll = Body(std::bit_cast<uintptr_t>(static_cast<float(*)(eStatModAbilities)>(&CStats::GetFatAndMuscleModifier)));
    if (!fatMod.hooked || ModuleOf(fatMod.dll) != env.asi) ready = false;
    // 0..2, 10, 12: sin colaboradores. 3, 4, 6 (Clone de eventos con un dato), 5 (KnockOffBike), 7 (InAir): pool + constructor
    env.stubs[3] = {poolStubExe, F(0x4AF670, &FkCtor1<0x4AF670>)};
    env.stubs[4] = {poolStubExe, F(0x4AFB50, &FkCtor1<0x4AFB50>)};
    env.stubs[5] = {poolStubExe, F(0x4AFCF0, &FkKnockCtor)};
    env.stubs[6] = {poolStubExe, F(0x4B0700, &FkCtor1<0x4B0700>)};
    env.stubs[7] = {poolStubExe};
    for (unsigned f : {3u, 4u, 5u, 6u, 7u}) env.dllStubs[f] = {poolStubDll};
    env.stubs[8]  = {F(0x6093B0, &FkBody<0x6093B0>), F(0x5E4760, &FkDelete<0x5E4760>)};
    env.stubs[9]  = {S(pedCtor, reinterpret_cast<void*>(&FkPedCtor)), F(0x571880, &FkT0<0x571880>), FV(0x5E4880, &FkT1<0x5E4880>), F(0x60CD20, &FkT1<0x60CD20>), F(0x571A00, &FkT1P<0x571A00>), F(0x5E4500, &FkT1<0x5E4500>),
                     F(0x4D3990, &FkAnimIndex), S(addGroup, reinterpret_cast<void*>(&FkAddGroup)), F(0x5FBB70, &FkT1<0x5FBB70>), F(0x5FB9C0, &FkT1P<0x5FB9C0>), F(0x5FC7E0, &FkT0<0x5FC7E0>), S(fatMod, reinterpret_cast<void*>(&FkFat)), F(0x4E68D0, &FkT1P<0x4E68D0>)};
    env.stubs[13] = {F(0x692030, &FkBody<0x692030>)};
    env.stubs[14] = {FV(0x692100, &FkMakeAbortable), F(0x4CEBE0, &FkSetFinishCb)};
    env.stubs[15] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x692680, &FkCtor1<0x692680>)};
    env.stubs[16] = {F(0x690BB0, &FkBody<0x690BB0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[17] = {F(0x6926F0, &FkBody<0x6926F0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[18] = {F(0x4D4610, &FkBlend), F(0x4CEBC0, &FkSetDeleteCb), FV(0x694390, &FkProcessPed)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 1u, 2u, 10u, 12u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f : {3u, 4u, 5u, 6u}) dllReal[f] = "toda la funcion salvo el constructor del evento (con hook, falso identico en el EXE y en la DLL) y CEvent::operator new (0x4B5620), que sustituye al CEventPool::New en linea del EXE (falso en la direccion 0x4B5570 del EXE y en el cuerpo de operator new de la DLL, mismo evento)";
    dllReal[7] = "toda la funcion (construye en linea) salvo CEvent::operator new (0x4B5620), que sustituye al CEventPool::New en linea del EXE (falso en la direccion 0x4B5570 del EXE y en el cuerpo de operator new de la DLL, mismo evento)";
    dllReal[9] = "toda la funcion salvo los colaboradores falseados; el constructor base de CPed se falsea en la direccion del EXE (0x5E8030) y en el destino de la llamada de la DLL (no en el envoltorio)";
    dllReal[11] = "toda la funcion (el constructor base de CTask 0x61A390 corre real en el EXE y en linea en la DLL)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array(), dllOnly = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& s : env.dllStubs[f]) dllOnly.push_back({{"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        for (const auto& p : env.reals[f]) {
            env.watches.emplace_back(p.exe, 16);
            real.push_back({{"exe", p.exe}, {"dll", p.dll != 0}, {"hooked", p.hooked}, {"dll_addr", p.dll}, {"dll_module", ModuleOf(p.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", dllOnly}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: se guarda al empezar y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kEventPoolGlobal, 4);                 // kRegPool
    regions.emplace_back(kTimerGlobal, 4);                     // kRegTimer
    regions.emplace_back(kPlayersBase, kPlayersRegion);        // kRegPlayers
    regions.emplace_back(kGroupsRegion, kGroupsBytes);         // kRegGroups
    regions.emplace_back(kPlayIdlesGlobal, 4);                 // kRegIdles
    for (unsigned f : {3u, 4u, 5u, 6u, 7u}) regionsOf[f] = {kRegPool};
    regionsOf[9] = {kRegPlayers, kRegGroups, kRegIdles};
    regionsOf[11] = {kRegTimer};
    Run(env, ready, registered, bodies, collaborators);
}
}
