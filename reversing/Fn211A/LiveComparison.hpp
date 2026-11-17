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

// Banco par a par Fn211A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 25 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion
// del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair).
// Colaboradores REALES (hook apagado: el EXE corre sus bytes y la DLL su cuerpo): CMatrix::Attach 0x59BD10, CColSphere::Set 0x40FD10, CEntity::GetColModel 0x535300 (SetUpWheelColModel), CVector::FloatTimesVector 0x40FE90
// (BurstTyre), CDamageManager::GetWheelStatus 0x6C21B0 (BurstTyre; en la DLL es una funcion en linea del CDamageManager) y el constructor y destructor del evento de peligro 0x4B2600 / 0x4B26A0 (DoHeliDustEffect; la DLL crea el CEventDanger con su propio constructor de C++, que solo llama a los falsos de RegisterReference y
// CleanUpOldReference). Falsos con hook en la DLL (apagado): ver Update. Falsos solo en la direccion del EXE (la DLL llama a esa direccion): operator new 0x82119A y operator delete 0x8214BD del CRT del EXE.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F211ALive {
using namespace F211AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F211AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = uint8(v); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;

// ================================================================================================ guion de los falsos de un caso
struct Script {
    u32 ret{}, dtorWrite{};
    std::array<u32, 2> rnd{}, water{}, level{}, create{};
    std::array<u32, 4> status{};
    u32 probeRet{}, probeZ{}, probeSurf{}, sandRet{}, group{}, caps{};
    std::array<u32, 2> newRet{};
    u32 mark1Idx{}, mark1Val{}, mark2Idx{}, mark2Val{};
    unsigned nRnd{}, nWater{}, nStatus{}, nCreate{}, nNew{};
};
inline Script gS;
inline uintptr_t newTrampoline{};
inline DWORD gBenchThread{};

// ================================================================================================ falsos de los colaboradores
// Cuerpo sin argumentos de pila (thiscall): evento [direccion, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja gS.dtorWrite en [ECX]; devuelve gS.ret.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gS.dtorWrite, 4);
    return gS.ret;
}
// Funcion thiscall sin argumentos que no toca memoria: evento [direccion, ECX (etiqueta)]; devuelve 0.
template<u32 ADDR> inline u32 __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return 0;
}
// --- CAutomobile::BurstTyre
inline u32 __fastcall FkSetWheel(void* self, void*, u32 wheel, u32 status) { Ev& e = PushEv(0x6C21A0, 3); e.v[0] = Tag(self); e.v[1] = wheel; e.v[2] = status; return 0; }
inline void __cdecl FkIncStat(u32 stat, u32 bits) { Ev& e = PushEv(0x55C180, 2); e.v[0] = stat; e.v[1] = bits; }
inline u32 __fastcall FkAudio(void* self, void*, u32 id, u32 bits) { Ev& e = PushEv(0x4F6420, 3); e.v[0] = Tag(self); e.v[1] = id; e.v[2] = bits; return 0; }
inline void __cdecl FkSwitchReal(void* vehicle) { Ev& e = PushEv(0x423FC0, 1); e.v[0] = Tag(vehicle); }
inline float __cdecl FkRand(u32 lo, u32 hi) {
    Ev& e = PushEv(0x41BD90, 2); e.v[0] = lo; e.v[1] = hi;
    const u32 bits = gS.rnd[gS.nRnd < 2 ? gS.nRnd : 1]; ++gS.nRnd;
    return std::bit_cast<float>(bits);
}
inline u32 __fastcall FkMove(void* self, void*, u32 x, u32 y, u32 z) { Ev& e = PushEv(0x5429F0, 4); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z; return 0; }
inline u32 __fastcall FkTurn(void* self, void*, u32 x, u32 y, u32 z, u32 px, u32 py, u32 pz) {
    Ev& e = PushEv(0x542A50, 7); e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z; e.v[4] = px; e.v[5] = py; e.v[6] = pz; return 0;
}
// --- CAutomobile::DoNitroEffect y DoHeliDustEffect
// operator*(CMatrix, CVector) (0x59C890; cdecl, destino, matriz, vector): evento [direccion, matriz (etiqueta), x, y, z]; copia el vector al destino y lo devuelve.
inline u32 __cdecl FkMatVec(u32* out, void* matrix, const u32* vec) {
    Ev& e = PushEv(0x59C890, 4); e.v[0] = Tag(matrix); e.v[1] = vec[0]; e.v[2] = vec[1]; e.v[3] = vec[2];
    out[0] = vec[0]; out[1] = vec[1]; out[2] = vec[2];
    return u32(uintptr_t(out));
}
// CWaterLevel::GetWaterLevel (0x6EB690; cdecl, x, y, z, &nivel, bool, nulo): evento [direccion, x, y, z, &nivel (etiqueta), b1, b2]; devuelve el valor del guion y solo con exito escribe el nivel.
inline u32 __cdecl FkWater(u32 x, u32 y, u32 z, u32* level, u32 b1, u32 b2) {
    Ev& e = PushEv(0x6EB690, 6); e.v[0] = x; e.v[1] = y; e.v[2] = z; e.v[3] = Tag(level); e.v[4] = b1; e.v[5] = b2;
    const unsigned k = gS.nWater < 2 ? gS.nWater : 1; ++gS.nWater;
    const u32 ret = gS.water[k] & 0xFFu;
    if (ret) *level = gS.level[k];
    return ret;
}
// FxManager_c::CreateFxSystem (0x4A9BE0; thiscall, nombre, &posicion, matriz, bool; `ret 0x10`)
inline u32 __fastcall FkCreateFx(void* self, void*, const char* name, const u32* pos, void* matrix, u32 flag) {
    Ev& e = PushEv(0x4A9BE0, 7); e.v[0] = Tag(self); e.v[1] = FnvStr(name); e.v[2] = pos[0]; e.v[3] = pos[1]; e.v[4] = pos[2]; e.v[5] = Tag(matrix); e.v[6] = flag;
    const unsigned k = gS.nCreate < 2 ? gS.nCreate : 1; ++gS.nCreate;
    return gS.create[k];
}
inline u32 __fastcall FkSetLocal(void* self, void*, u32 b) { Ev& e = PushEv(0x4AA910, 2); e.v[0] = Tag(self); e.v[1] = b; return 0; }
inline u32 __fastcall FkConstTime(void* self, void*, u32 b, u32 bits) { Ev& e = PushEv(0x4AA6C0, 3); e.v[0] = Tag(self); e.v[1] = b; e.v[2] = bits; return 0; }
inline u32 __fastcall FkPlayStatus(void* self, void*) {
    Ev& e = PushEv(0x4AA900, 1); e.v[0] = Tag(self);
    const unsigned k = gS.nStatus & 3u; ++gS.nStatus;
    return gS.status[k];
}
inline u32 __fastcall FkSetOffset(void* self, void*, const u32* v) { Ev& e = PushEv(0x4AA660, 4); e.v[0] = Tag(self); e.v[1] = v[0]; e.v[2] = v[1]; e.v[3] = v[2]; return 0; }
inline u32 __fastcall FkEnablePrim(void* self, void*, u32 index, u32 b) { Ev& e = PushEv(0x4AA610, 3); e.v[0] = Tag(self); e.v[1] = index; e.v[2] = b; return 0; }
// CWorld::ProcessVerticalLine (0x5674E0; cdecl, 11 argumentos): evento [direccion, origen x, y, z, distancia, &punto, &entidad, siete banderas]; escribe z (+8) y el tipo de superficie B (+0x23) del punto de impacto.
inline u32 __cdecl FkProbe(const u32* origin, u32 dist, u8* colPoint, void** entity, u32 b0, u32 b1, u32 b2, u32 b3, u32 b4, u32 b5, u32 b6) {
    Ev& e = PushEv(0x5674E0, 13);
    e.v[0] = origin[0]; e.v[1] = origin[1]; e.v[2] = origin[2]; e.v[3] = dist; e.v[4] = Tag(colPoint); e.v[5] = Tag(entity);
    e.v[6] = b0; e.v[7] = b1; e.v[8] = b2; e.v[9] = b3; e.v[10] = b4; e.v[11] = b5; e.v[12] = b6;
    memcpy(colPoint + 8, &gS.probeZ, 4);
    colPoint[0x23] = uint8(gS.probeSurf);
    return gS.probeRet;
}
inline u32 __fastcall FkIsSand(void* self, void*, u32 surface) { Ev& e = PushEv(0x55E6F0, 2); e.v[0] = Tag(self); e.v[1] = surface; return gS.sandRet; }
inline u32 __cdecl FkGlobalGroup() { PushEv(0x4ABA50, 0); return gS.group; }
// CEventGroup::Add (0x4AB420; thiscall, evento, bool; `ret 8`): evento [direccion, grupo (etiqueta), evento (etiqueta de pila), bool, campos del evento (+4 dword, +8 byte, +0xC byte, +0xE word, +0x10 word, +0x14 puntero, +0x18 dword)]
inline u32 __fastcall FkGroupAdd(void* self, void*, const u8* ev, u32 flag) {
    Ev& e = PushEv(0x4AB420, 10);
    e.v[0] = Tag(self); e.v[1] = Tag(ev); e.v[2] = flag;
    u32 d4{}, d14{}, d18{}; u16 w0e{}, w10{};
    memcpy(&d4, ev + 4, 4); memcpy(&w0e, ev + 0xE, 2); memcpy(&w10, ev + 0x10, 2); memcpy(&d14, ev + 0x14, 4); memcpy(&d18, ev + 0x18, 4);
    e.v[3] = d4; e.v[4] = ev[8]; e.v[5] = ev[0xC]; e.v[6] = w0e; e.v[7] = w10; e.v[8] = Tag(uintptr_t(d14)); e.v[9] = d18;
    return 0;
}
inline u32 __fastcall FkRegisterRef(void* self, void*, void* field) { Ev& e = PushEv(0x571B70, 2); e.v[0] = Tag(self); e.v[1] = Tag(field); return 0; }
inline u32 __fastcall FkCleanUpRef(void* self, void*, void* field) { Ev& e = PushEv(0x571A00, 2); e.v[0] = Tag(self); e.v[1] = Tag(field); return 0; }
// --- CAutomobile::SetModelIndex: CVehicle::SetModelIndex (0x6D6A40; thiscall, indice) y CClumpModelInfo::FillFrameArray (0x4C5440; cdecl, clump, tabla); cada falso deja una marca en la tabla de nodos
inline u32 __fastcall FkSetIndex(void* self, void*, u32 index) {
    Ev& e = PushEv(0x6D6A40, 2); e.v[0] = Tag(self); e.v[1] = index;
    memcpy(static_cast<u8*>(self) + 0x648 + 4 * gS.mark1Idx, &gS.mark1Val, 4);
    return 0;
}
inline u32 __cdecl FkFillFrames(void* clump, u32* table) {
    Ev& e = PushEv(0x4C5440, 2); e.v[0] = Tag(clump); e.v[1] = Tag(table);
    table[gS.mark2Idx] = gS.mark2Val;
    return 0;
}
// --- CAutomobile destructor con borrado: CVehicle::operator delete (0x6E2D90; cdecl, puntero)
inline void __cdecl FkVehicleDelete(void* p) { Ev& e = PushEv(0x6E2D90, 1); e.v[0] = Tag(p); }
// --- IsEnvironmentMappingSupported: RwD3D9GetCaps (0x7FAD20; cdecl, sin argumentos)
inline u32 __cdecl FkGetCaps() { PushEv(0x7FAD20, 0); return gS.caps; }
// --- CPool: operator new (0x82119A; cdecl, tamano) y operator delete (0x8214BD; cdecl, puntero). Los demas hilos del juego pasan por el CRT real (trampolin / free del EXE).
inline void* __cdecl FkNew(u32 size) {
    if (GetCurrentThreadId() != gBenchThread) return reinterpret_cast<void*(__cdecl*)(u32)>(newTrampoline)(size);
    Ev& e = PushEv(0x82119A, 1); e.v[0] = size;
    const unsigned k = gS.nNew < 2 ? gS.nNew : 1; ++gS.nNew;
    return reinterpret_cast<void*>(uintptr_t(gS.newRet[k]));
}
inline void __cdecl FkDelete(void* p) {
    if (GetCurrentThreadId() != gBenchThread) { reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    Ev& e = PushEv(0x8214BD, 1); e.v[0] = Tag(p);
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

// Puntero de un caso: 0 nulo, 1 dentro de una arena (desplazamiento), 2 crudo (el desplazamiento tal cual, no es una direccion de arena)
template<class A> inline u32 PtrKind(u32 kind, A& arena, u32 off) { return kind == 0 ? 0u : (kind == 1 ? u32(arena.Obj(off)) : off); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gS = Script{};
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kSetUp: {
        arenaA.Put16(o + 0x22, a[4]); arenaA.Put8(o + 0x36, a[5]);
        const u32 slot[6] = {0x65C, 0x664, 0x650, 0x658, 0x660, 0x654};   // LF, LB, RF, RB, LM, RM
        for (u32 k = 0; k < 6; ++k) {
            const u32 frame = a[7] + k * 0x60;
            const bool present = k < 4 || (k == 4 ? (a[6] & 1) : (a[6] & 2));
            arenaA.Put32(o + slot[k], present ? u32(arenaB.Obj(frame)) : 0u);
            arenaB.Put32(frame + 0x40, a[8 + 3 * k]); arenaB.Put32(frame + 0x44, a[9 + 3 * k]); arenaB.Put32(frame + 0x48, a[10 + 3 * k]);
        }
        arenaD.Put32(a[28] + 0x14, u32(arenaD.Obj(a[29]))); arenaD.Put32(a[28] + 0x40, a[26]); arenaD.Put32(a[28] + 0x44, a[27]);
        *reinterpret_cast<u32*>(kModelTable + 4 * a[4]) = u32(arenaD.Obj(a[28]));
        arenaC.Put32(a[30] + 0x2C, u32(arenaC.Obj(a[31]))); arenaC.Put32(a[31] + 8, u32(arenaC.Obj(a[32])));
        call.args[0] = u32(arenaC.Obj(a[30])); call.argCount = 1;
        break;
    }
    case kBurst: {
        arenaA.Put16(o + 0x22, a[4]); arenaA.Put8(o + 0x42B, a[5]); arenaA.Put32(o + 0x40, a[6]); arenaA.Put8(o + 0x36, a[9]);
        arenaA.Put32(o + 0x8C, a[13]); arenaA.Put32(o + 0x90, a[14]); arenaA.Put8(o + 0xB8, 0xA5);
        arenaA.Put32(o + 0x14, u32(arenaB.Obj(a[15])));
        for (u32 i = 0; i < 3; ++i) { arenaB.Put32(a[15] + 4 * i, a[16 + i]); arenaB.Put32(a[15] + 0x10 + 4 * i, a[19 + i]); }
        {   // GetWheelStatus (0x6C21B0) corre real en el EXE y en linea en la DLL: el estado de la rueda sale de la memoria del CDamageManager (+0x5A0, bytes desde +5)
            const u32 comp = a[7] & 0xFFu;
            const u32 wheel = comp == 13 ? 0u : (comp == 14 ? 2u : (comp == 15 ? 1u : (comp == 16 ? 3u : comp)));
            arenaA.Put8(o + 0x5A5 + wheel, a[10]);
        }
        gS.rnd = {a[11], a[12]};
        call.args[0] = a[7]; call.args[1] = a[8]; call.argCount = 2;
        break;
    }
    case kNitro: {
        arenaA.Put16(o + 0x22, a[4]); arenaA.Put32(o + 0x40, a[5]);
        arenaA.Put32(o + 0x384, u32(arenaC.Obj(a[6]))); arenaC.Put32(a[6] + 0xCC, a[7]);
        arenaA.Put32(o + 0x18, a[8] == 1 ? u32(arenaB.Obj(a[9])) : 0u);
        if (a[8] == 1) arenaB.Put32(a[9] + 4, a[10]);
        arenaA.Put32(o + 0x978, PtrKind(a[11], arenaD, a[12])); arenaA.Put32(o + 0x97C, PtrKind(a[13], arenaD, a[14]));
        arenaA.Put32(o + 0x14, a[15] == 1 ? u32(arenaB.Obj((a[9] + 0x40) & 0xFFC)) : (a[10] | 1u));
        arenaD.Put32(a[16] + 0x5C, u32(arenaD.Obj(a[17])));
        arenaD.Put32(a[17] + 0x48, a[18]); arenaD.Put32(a[17] + 0x4C, a[19]); arenaD.Put32(a[17] + 0x50, a[20]);
        *reinterpret_cast<u32*>(kModelTable + 4 * a[4]) = u32(arenaD.Obj(a[16]));
        gS.water = {a[22], a[23]}; gS.level = {a[24], a[25]};
        gS.status = {a[26], a[27], a[28], a[29]};
        gS.create = {PtrKind(a[30], arenaD, a[31]), PtrKind(a[30], arenaD, a[31] + 0x40)};
        call.args[0] = a[21]; call.argCount = 1;
        break;
    }
    case kRideAnim:
    case kFlagDestroy:
        break;
    case kSetModel:
        arenaA.Put32(o + 0x18, a[5]);
        for (u32 i = 0; i < 25; ++i) arenaA.Put32(o + 0x648 + 4 * i, 0xA5A5A500u + i);
        gS.mark1Idx = a[6]; gS.mark1Val = a[7]; gS.mark2Idx = a[8]; gS.mark2Val = a[9];
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kHeli: {
        arenaA.Put32(o + 0x594, a[4]); arenaA.Put32(o + 0x84C, a[5]); arenaA.Put16(o + 0x22, a[11]);
        arenaA.Put32(o + 0x580, PtrKind(a[12], arenaD, a[33])); arenaA.Put32(o + 0x984, a[13]);
        if (a[6] == 1) {
            arenaA.Put32(o + 0x14, u32(arenaB.Obj(a[7])));
            arenaB.Put32(a[7] + 0x30, a[8]); arenaB.Put32(a[7] + 0x34, a[9]); arenaB.Put32(a[7] + 0x38, a[10]);
        } else {
            arenaA.Put32(o + 0x14, 0);
            arenaA.Put32(o + 4, a[8]); arenaA.Put32(o + 8, a[9]); arenaA.Put32(o + 0xC, a[10]);
        }
        *reinterpret_cast<u32*>(kCamPos) = a[16]; *reinterpret_cast<u32*>(kCamPos + 4) = a[17]; *reinterpret_cast<u32*>(kCamPos + 8) = a[18];
        if (a[14] == 1) {
            *reinterpret_cast<u32*>(kCamMatrix) = u32(arenaC.Obj(a[15]));
            arenaC.Put32(a[15] + 0x30, a[19]); arenaC.Put32(a[15] + 0x34, a[20]); arenaC.Put32(a[15] + 0x38, a[21]);
        } else {
            *reinterpret_cast<u32*>(kCamMatrix) = 0;
        }
        *reinterpret_cast<u32*>(kFrameCounter) = a[22];
        gS.probeRet = a[23]; gS.probeZ = a[24]; gS.probeSurf = a[25]; gS.sandRet = a[26];
        gS.water = {a[27], a[27]}; gS.level = {a[28], a[28]};
        gS.create = {PtrKind(a[31], arenaD, a[33] + 0x40), PtrKind(a[31], arenaD, a[33] + 0x40)};
        gS.group = PtrKind(a[32], arenaD, a[34]);
        call.args[0] = a[29]; call.args[1] = a[30]; call.argCount = 2;
        break;
    }
    case kSdd:
        gS.dtorWrite = a[5];
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kEnvSupp:
        arenaD.Put32(a[8] + 0x38, a[4]); arenaD.Put32(a[8] + 0x90, a[5]); arenaD.Put32(a[8] + 0x94, a[6]); arenaD.Put32(a[8] + 0x98, a[7]);
        gS.caps = u32(arenaD.Obj(a[8]));
        call.ecx = 0;
        break;
    case kFromBuf:
        for (u32 i = 0; i < 5; ++i) arenaB.Put32(a[10] + 4 * i, a[4 + i]);
        arenaB.Put32(a[10] + 0x14, a[9]);
        call.args[0] = u32(arenaB.Obj(a[10])); call.args[1] = thisA; call.argCount = 2; call.popBytes = 8;
        break;
    case kToBuf:
        for (u32 i = 0; i < 5; ++i) arenaB.Put8(a[10] + i, a[4 + i]);
        arenaB.Put32(a[10] + 8, a[9]);
        call.args[0] = u32(arenaB.Obj(a[10])); call.args[1] = thisA; call.argCount = 2; call.popBytes = 8;
        break;
    case kPoolCtor:
        gS.newRet = {PtrKind(a[5], arenaC, a[6]), u32(arenaD.Obj(a[7]))};
        call.args[0] = a[4]; call.args[1] = a[8]; call.argCount = 2;
        break;
    case kPoolDtor:
    case kPoolSdd:
        arenaA.Put32(o + 0, PtrKind(a[4], arenaC, a[5])); arenaA.Put32(o + 4, PtrKind(a[6], arenaD, a[7]));
        arenaA.Put32(o + 8, a[8]); arenaA.Put32(o + 0xC, a[9]); arenaA.Put8(o + 0x10, a[10]);
        if (kKind[c.fn] == kPoolSdd) { call.args[0] = a[11]; call.argCount = 1; }
        break;
    case kPoolNew: {
        arenaA.Put32(o + 0, u32(arenaC.Obj(a[6]))); arenaA.Put32(o + 4, u32(arenaD.Obj(a[7])));
        arenaA.Put32(o + 8, a[4]); arenaA.Put32(o + 0xC, a[5]);
        for (u32 i = 0; i < a[4]; ++i) {
            const u32 low = Rand(a[37], 0x500 + i) & 0x7Fu;
            bool freeFlag = false;
            switch (a[9]) {
            case 0: freeFlag = Rand(a[37], 0x900 + i) % 100 < a[8]; break;
            case 1: freeFlag = true; break;
            case 2: freeFlag = false; break;
            default: freeFlag = i == a[10]; break;
            }
            arenaD.Put8(a[7] + i, low | (freeFlag ? 0x80u : 0u));
        }
        break;
    }
    case kPoolDel: {
        const u32 idx = a[6];
        const u32 objects = u32(arenaC.Obj(a[8]));
        arenaA.Put32(o + 0, objects); arenaA.Put32(o + 4, u32(arenaD.Obj(a[9])));
        arenaA.Put32(o + 8, a[4]); arenaA.Put32(o + 0xC, a[5]);
        uint8* state = arenaD.m.data() + kGuard + a[9] + i32(idx);
        *state = u8((*state & 0x7Fu) | (a[10] ? 0x80u : 0u));
        call.args[0] = objects + idx * 12u + a[7]; call.argCount = 1;
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
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kSetUp: case kBurst: obs["ret"] = eax & 0xFFu; break;                                                          // bool (AL)
        case kRideAnim: case kEnvSupp: obs["ret"] = eax; break;                                                             // EAX completo
        case kSdd: case kFromBuf: case kToBuf: case kPoolCtor: case kPoolSdd: case kPoolNew: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                                               // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN211A_CASES (solo para las rondas de la prueba negativa y las pruebas rapidas, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN211A_CASES", buffer, sizeof(buffer)) > 0) {
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
        // Trampolin de operator new (0x82119A): los cinco primeros bytes (`push esi; mov esi,[esp+8]`) y salto a 0x82119F; lo usan los hilos ajenos al banco mientras el falso esta instalado
        {
            auto* mem = static_cast<uint8*>(VirtualAlloc(nullptr, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
            memcpy(mem, reinterpret_cast<void*>(0x82119A), 5);
            mem[5] = 0xE9; const int32 rel = int32(0x82119F - (uintptr_t(mem) + 10)); memcpy(mem + 6, &rel, 4);
            newTrampoline = uintptr_t(mem);
        }
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
    if (!std::filesystem::exists(dir / "fn211a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn211a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn211a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator new 0x82119A y operator delete 0x8214BD): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL con hook (apagado): el EXE corre sus bytes y la DLL su cuerpo
    auto Real = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    env.reals[0] = {Real(0x59BD10), Real(0x40FD10), Real(0x535300)};
    env.stubs[1] = {F(0x6C21A0, &FkSetWheel), F(0x55C180, &FkIncStat), F(0x4F6420, &FkAudio), F(0x423FC0, &FkSwitchReal), F(0x41BD90, &FkRand), F(0x5429F0, &FkMove), F(0x542A50, &FkTurn)};
    env.reals[1] = {Real(0x40FE90), Real(0x6C21B0)};
    env.stubs[2] = {F(0x59C890, &FkMatVec), F(0x6EB690, &FkWater), F(0x4A9BE0, &FkCreateFx), F(0x4AA910, &FkSetLocal), F(0x4AA2F0, &FkThis<0x4AA2F0>), F(0x4AA390, &FkThis<0x4AA390>), F(0x4AA6C0, &FkConstTime), F(0x4AA900, &FkPlayStatus)};
    env.stubs[5] = {FV(0x6D6A40, &FkSetIndex), F(0x4C5440, &FkFillFrames)};
    env.stubs[6] = {F(0x5674E0, &FkProbe), F(0x55E6F0, &FkIsSand), F(0x6EB690, &FkWater), F(0x4ABA50, &FkGlobalGroup), F(0x4AB420, &FkGroupAdd), F(0x571B70, &FkRegisterRef), F(0x571A00, &FkCleanUpRef),
                    F(0x4A9BE0, &FkCreateFx), F(0x4AA910, &FkSetLocal), F(0x4AA2F0, &FkThis<0x4AA2F0>), F(0x4AA6C0, &FkConstTime), F(0x4AA660, &FkSetOffset), F(0x4AA610, &FkEnablePrim), F(0x4AA3F0, &FkThis<0x4AA3F0>)};
    env.reals[6] = {Real(0x4B2600), Real(0x4B26A0)};
    env.stubs[7] = {F(0x6A61E0, &FkBody<0x6A61E0>), F(0x6E2D90, &FkVehicleDelete)};
    env.stubs[8] = {F(0x7FAD20, &FkGetCaps)};
    for (unsigned f : {11u, 12u, 13u}) env.stubs[f] = {ByExeAddress(0x82119A, &FkNew)};
    for (unsigned f : {14u, 15u, 16u, 17u, 18u, 19u, 20u}) env.stubs[f] = {ByExeAddress(0x8214BD, &FkDelete)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {3u, 4u, 9u, 10u, 21u, 22u, 23u, 24u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[0] = "toda la funcion; CMatrix::Attach, CColSphere::Set y CEntity::GetColModel corren reales (hook apagado)";
    dllReal[1] = "toda la funcion salvo los falsos; CVector::FloatTimesVector (0x40FE90) y CDamageManager::GetWheelStatus (0x6C21B0; la DLL lo lleva en linea) corren reales (hook apagado)";
    dllReal[6] = "toda la funcion salvo los falsos; el constructor y el destructor del evento de peligro corren reales (EXE 0x4B2600 / 0x4B26A0 con el hook apagado; la DLL usa su CEventDanger)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        for (const auto& pr : env.reals[f]) {
            env.watches.emplace_back(pr.exe, 16);
            real.push_back({{"exe", pr.exe}, {"dll", pr.dll != 0}, {"hooked", pr.hooked}, {"dll_addr", pr.dll}, {"dll_module", ModuleOf(pr.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: la tabla de model infos (SetUpWheelColModel, DoNitroEffect), la camara y el contador de fotogramas (DoHeliDustEffect); se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kModelTable + 4 * kModelLo, 4 * kModelCount); regionsOf[0] = regionsOf[2] = {regions.size() - 1};
    regions.emplace_back(kCamPos, kCamBytes); regions.emplace_back(kFrameCounter, 4); regionsOf[6] = {regions.size() - 2, regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
