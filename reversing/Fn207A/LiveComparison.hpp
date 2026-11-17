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

// Banco par a par Fn207A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 22 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con
// objetos sinteticos en cuatro arenas propias (A de 0x2000, B y D de 0x1000, C de 0x4000, con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) para
// los que tienen hook RH: CPed::IsPlayer 0x5DF8F0, CPlayerPed::ClearWeaponTarget 0x609C80, CPed::SetCurrentWeapon 0x5E6280, CCamera::ClearPlayerWeaponMode 0x50AB10, CTaskSimpleStandStill::ProcessPed 0x62F370 (V), CTask::operator new
// 0x61A5A0 y delete 0x61A5B0, CTaskSimpleRunAnim 0x61A900, RpAnimBlendClumpGetAssociation 0x4D68B0, CPed::AddWeaponModel 0x5E5ED0, CTaskTimer::IsOutOfTime 0x420E30, CPedIntelligence::FindTaskByType 0x600EE0,
// CTaskSimplePlayerOnFoot::PlayerControlZelda 0x6883D0, ~CTaskComplex 0x61A3D0 y constructor 0x61A3B0, cuerpos de destructor 0x634F20 y 0x635100, CTaskComplexSequence::Flush 0x632C10, CCollision::Test2DLineAgainst2DLine 0x4138D0,
// CVector::Normalise 0x59C910, CWorld::ProcessLineOfSight 0x56BA00, CPathFind::FindNodeClosestToCoors 0x44F460 y These2NodesAreAdjacent 0x44D230, y las funciones del propio lote 0x422E10 y 0x44FA30 (ruta propia con el mismo
// falso); solo en la direccion del EXE (la DLL las llama por ella): 0x6348E0 y 0x6348F0. Las llamadas virtuales (GetTaskType de la subtarea) usan una vtable falsa al final de la arena D. Memoria global del juego tocada
// (restaurada tras cada lado): tabla de secuencias de tareas 0xC178F0 (64 x 0x40) y el byte marcador 0xA9B0B0 (funciones vacias).
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en el contador de nodos; solo AL en los booleanos; bits del float de ST0 en el margen por rumbo; EAX, ECX y EDX en GetRef) y el estado x87
// (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F207ALive {
using namespace F207AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F207AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(int64 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(int64 off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(int64 off, u32 v) { m[size_t(int64(kGuard) + off)] = u8(v); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWinA> arenaA;
inline Arena<kWin> arenaB, arenaD;
inline Arena<kWinC> arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [objeto]
inline u32 gTaskNew{};               // lo que devuelve CTask::operator new
inline u32 gPlayerMask{}, gCall{}, gJunk{}, gMask{};
inline u32 gAssoc91{}, gAssoc92{}, gOut{}, gFind{}, gSwapPd{}, gHitZ{}, gEntity{}, gSetWeapon{};
inline std::array<u32, 3> gVec{};
inline std::array<u32, 6> gProbeZ{};
inline std::array<u32, 4> gAddrs{};  // nodos que devuelve el falso de FindNodeClosestToCoors / entradas que escribe el falso de Record
inline u32 gNRec{}, gAdjMask{};
inline DWORD gBenchThread{};

// Marca de los eventos de las llamadas virtuales (no son direcciones del EXE)
inline constexpr u32 kEvType = 0x50000001u, kEvTrap = 0x5000FFFFu;

// Cuerpo del destructor / constructor base (thiscall, sin argumentos de pila): evento [direccion, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// operator delete (cdecl, un argumento): evento [direccion, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// CTask::operator new (0x61A5A0; cdecl, el tamano): evento [direccion, tamano]; devuelve gTaskNew (un objeto de la arena B o nulo).
inline void* __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gTaskNew));
}
// CPed::IsPlayer (0x5DF8F0; thiscall, sin pila): evento [direccion, ECX (etiqueta)]; devuelve un bit de gPlayerMask por llamada (los demas bits, basura: el llamador solo mira AL).
inline u32 __fastcall FkIsPlayer(void* self, void*) {
    Ev& e = PushEv(0x5DF8F0, 1);
    e.v[0] = Tag(self);
    const u32 n = gCall++;
    return (gPlayerMask & 0xFFFFFF00u) | ((gPlayerMask >> n) & 1u);
}
// CPlayerPed::ClearWeaponTarget (0x609C80), CCamera::ClearPlayerWeaponMode (0x50AB10): thiscall sin pila: evento [direccion, ECX (etiqueta)]
template<u32 ADDR> inline void __fastcall FkVoidThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
}
// CPed::SetCurrentWeapon (0x5E6280; thiscall, un dword, `ret 4`): evento [direccion, ECX (etiqueta), tipo]
inline u32 __fastcall FkSetWeapon(void* self, void*, u32 type) {
    Ev& e = PushEv(0x5E6280, 2);
    e.v[0] = Tag(self); e.v[1] = type;
    return 0;
}
// CTaskSimpleStandStill::ProcessPed (0x62F370, V; thiscall, el ped, `ret 4`): evento [direccion, ECX (etiqueta de pila), ped (etiqueta), dwords +8, +0Ch, +10h, bytes +14h, +15h, +18h, +19h, float +1Ch] del objeto local
inline u32 __fastcall FkStandProcess(void* self, void*, void* ped) {
    Ev& e = PushEv(0x62F370, 10);
    const u8* b = static_cast<const u8*>(self);
    e.v[0] = Tag(self); e.v[1] = Tag(ped);
    e.v[2] = BitsOf(b + 8); e.v[3] = BitsOf(b + 0xC); e.v[4] = BitsOf(b + 0x10);
    e.v[5] = b[0x14]; e.v[6] = b[0x15]; e.v[7] = b[0x18]; e.v[8] = b[0x19]; e.v[9] = BitsOf(b + 0x1C);
    return 0;
}
// CTaskSimpleRunAnim::CTaskSimpleRunAnim (0x61A900; thiscall, grupo, animacion, mezcla, tipo, nombre y bandera, `ret 18h`): evento [direccion, ECX (etiqueta), grupo, animacion, mezcla, tipo, FNV del nombre, bandera];
// deja gDtorWrite en [ECX] y devuelve this
inline u32 __fastcall FkRunAnim(void* self, void*, u32 group, u32 anim, u32 blend, u32 type, const char* name, u32 flag) {
    Ev& e = PushEv(0x61A900, 7);
    e.v[0] = Tag(self); e.v[1] = group; e.v[2] = anim; e.v[3] = blend; e.v[4] = type; e.v[5] = FnvStr(name); e.v[6] = flag;
    memcpy(self, &gDtorWrite, 4);
    return u32(uintptr_t(self));
}
// RpAnimBlendClumpGetAssociation (0x4D68B0; cdecl, clump y animacion): evento [direccion, clump, animacion]; devuelve la asociacion 91h o 92h del banco (o nula)
inline u32 __cdecl FkAssoc(u32 clump, u32 anim) {
    Ev& e = PushEv(0x4D68B0, 2);
    e.v[0] = Tag(uintptr_t(clump)); e.v[1] = anim;
    return anim == 0x91 ? gAssoc91 : (anim == 0x92 ? gAssoc92 : 0xDEAD0000u + anim);
}
// CPed::AddWeaponModel (0x5E5ED0; thiscall, el modelo, `ret 4`): evento [direccion, ECX (etiqueta), modelo]
inline void __fastcall FkAddModel(void* self, void*, u32 model) {
    Ev& e = PushEv(0x5E5ED0, 2);
    e.v[0] = Tag(self); e.v[1] = model;
}
// 0x6348F0 (por la direccion del EXE; thiscall, el ped, `ret 4`): evento [direccion, ECX (etiqueta), ped (etiqueta)]
inline void __fastcall FkQuitModel(void* self, void*, void* ped) {
    Ev& e = PushEv(0x6348F0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ped);
}
// CTaskTimer::IsOutOfTime (0x420E30; thiscall, sin pila): evento [direccion, ECX (etiqueta)]; devuelve gOut (el llamador solo mira AL)
inline u32 __fastcall FkOutOfTime(void* self, void*) {
    Ev& e = PushEv(0x420E30, 1);
    e.v[0] = Tag(self);
    return gOut;
}
// CPedIntelligence::FindTaskByType (0x600EE0; thiscall, el tipo, `ret 4`): evento [direccion, ECX (etiqueta), tipo]; devuelve gFind
inline u32 __fastcall FkFindTask(void* self, void*, u32 type) {
    Ev& e = PushEv(0x600EE0, 2);
    e.v[0] = Tag(self); e.v[1] = type;
    return gFind;
}
// CTaskSimplePlayerOnFoot::PlayerControlZelda (0x6883D0; thiscall, ped y bandera, `ret 8`): evento [direccion, ECX (etiqueta), ped (etiqueta), bandera, datos de jugador (etiqueta), byte +84h, byte +85h, float +18h];
// si gSwapPd != 0 cambia los datos de jugador del ped (+480h) a gSwapPd
inline void __fastcall FkZelda(void* self, void*, void* ped, u32 pad) {
    Ev& e = PushEv(0x6883D0, 7);
    u8* const pedBytes = static_cast<u8*>(ped);
    const u32 pdPtr = BitsOf(pedBytes + 0x480);
    const u8* pd = reinterpret_cast<const u8*>(uintptr_t(pdPtr));
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = pad; e.v[3] = Tag(uintptr_t(pdPtr)); e.v[4] = pd[0x84]; e.v[5] = pd[0x85]; e.v[6] = BitsOf(pd + 0x18);
    if (gSwapPd) memcpy(pedBytes + 0x480, &gSwapPd, 4);
}
// Llamada virtual (vtable falsa de la arena D): GetTaskType (ranura 4); devuelve un dword de [ECX + 0x30]
inline u32 __fastcall FkVirtType(void* self, void*) {
    Ev& e = PushEv(kEvType, 1);
    e.v[0] = Tag(self);
    return BitsOf(reinterpret_cast<const u8*>(self) + 0x30);
}
inline u32 __fastcall FkTrap(void* self, void*) {
    Ev& e = PushEv(kEvTrap, 1);
    e.v[0] = Tag(self);
    return 0;
}
// CTaskComplexSequence::Flush (0x632C10; thiscall, sin pila): evento [direccion, ECX (crudo), byte +38h, dword +3Ch]
inline void __fastcall FkFlush(void* self, void*) {
    Ev& e = PushEv(0x632C10, 3);
    const u8* b = static_cast<const u8*>(self);
    e.v[0] = Tag(self); e.v[1] = b[0x38]; e.v[2] = BitsOf(b + 0x3C);
}
// CCollision::Test2DLineAgainst2DLine (0x4138D0; cdecl, ocho floats): evento [direccion, los ocho dwords]; devuelve un bit de gMask por llamada y basura en los bytes altos
inline u32 __cdecl FkTest2D(u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7) {
    Ev& e = PushEv(0x4138D0, 8);
    e.v[0] = a0; e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4; e.v[5] = a5; e.v[6] = a6; e.v[7] = a7;
    const u32 n = gCall++;
    return (gJunk & 0xFFFFFF00u) | ((gMask >> (n & 31u)) & 1u);
}
// CVector::Normalise (0x59C910; thiscall, sin pila): evento [direccion, ECX (etiqueta), x, y, z al entrar]; deja gVec en el vector
inline void __fastcall FkNormalise(void* self, void*) {
    Ev& e = PushEv(0x59C910, 4);
    u32* v = static_cast<u32*>(self);
    e.v[0] = Tag(self); e.v[1] = v[0]; e.v[2] = v[1]; e.v[3] = v[2];
    v[0] = gVec[0]; v[1] = gVec[1]; v[2] = gVec[2];
}
// CWorld::ProcessLineOfSight (0x56BA00; cdecl, inicio, fin, punto de choque, entidad y ocho banderas): evento [direccion, inicio (3 dwords), fin (3), punto de choque (etiqueta), entidad (etiqueta), ocho banderas];
// escribe gHitZ en [punto + 8] y gEntity en [entidad]; devuelve gRet (el llamador solo mira AL)
inline u32 __cdecl FkLos(u32* start, u32* end, u32* col, u32* entity, u32 b0, u32 b1, u32 b2, u32 b3, u32 b4, u32 b5, u32 b6, u32 b7) {
    Ev& e = PushEv(0x56BA00, 16);
    e.v[0] = start[0]; e.v[1] = start[1]; e.v[2] = start[2]; e.v[3] = end[0]; e.v[4] = end[1]; e.v[5] = end[2];
    e.v[6] = Tag(col); e.v[7] = Tag(entity);
    const u32 bs[8]{b0, b1, b2, b3, b4, b5, b6, b7};
    for (u32 i = 0; i < 8; ++i) e.v[8 + i] = bs[i];
    col[2] = gHitZ; *entity = gEntity;
    return gRet;
}
// CCarCtrl::FlyAIPlaneProbeGround (0x422E10; cdecl, avion, elevacion, rumbo y salida): evento [direccion, avion (etiqueta), elevacion, rumbo, salida (etiqueta)]; si acierta (bit de gMask) escribe gProbeZ[i]
inline u32 __cdecl FkProbe(u32 plane, u32 elevation, u32 heading, u32* outZ) {
    Ev& e = PushEv(0x422E10, 4);
    e.v[0] = Tag(uintptr_t(plane)); e.v[1] = elevation; e.v[2] = heading; e.v[3] = Tag(outZ);
    const u32 n = gCall++;
    const bool hit = (gMask >> (n & 31u)) & 1u;
    if (hit) *outZ = gProbeZ[n % 6];
    return (gJunk & 0xFFFFFF00u) | (hit ? 1u : 0u);
}
// CPathFind::FindNodeClosestToCoors (0x44F460; thiscall, puntero de retorno, posicion y ocho argumentos, `ret 2Ch`): evento [direccion, ECX (etiqueta), retorno (etiqueta), x, y, z, tipo, distancia, cuatro banderas, 1, unk6];
// escribe gAddrs[n & 3] en el retorno y devuelve el puntero de retorno
inline u32* __fastcall FkFindClosest(void* self, void*, u32* ret, u32 x, u32 y, u32 z, u32 type, u32 maxDist, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4) {
    Ev& e = PushEv(0x44F460, 12);
    e.v[0] = Tag(self); e.v[1] = Tag(ret); e.v[2] = x; e.v[3] = y; e.v[4] = z; e.v[5] = type; e.v[6] = maxDist; e.v[7] = a0; e.v[8] = a1; e.v[9] = a2; e.v[10] = a3; e.v[11] = a4;
    *ret = gAddrs[(gCall++) & 3u];
    return ret;
}
// CPathFind::These2NodesAreAdjacent (0x44D230; thiscall, dos direcciones, `ret 8`): evento [direccion, ECX (etiqueta), primera, segunda]; devuelve un bit de gAdjMask por llamada y basura en los bytes altos
inline u32 __fastcall FkAdjacent(void* self, void*, u32 first, u32 second) {
    Ev& e = PushEv(0x44D230, 3);
    e.v[0] = Tag(self); e.v[1] = first; e.v[2] = second;
    const u32 n = gCall++;
    return (gJunk & 0xFFFFFF00u) | ((gAdjMask >> (n & 31u)) & 1u);
}
// CPathFind::RecordNodesClosestToCoors (0x44FA30; thiscall, posicion, tipo, cuenta, salida, distancia y cuatro banderas, `ret 2Ch`): evento [direccion, ECX (etiqueta), x, y, z, tipo, cuenta, salida (etiqueta), distancia, banderas];
// escribe las primeras gNRec entradas de gAddrs en la salida (dwords completos)
inline void __fastcall FkRecord(void* self, void*, u32 x, u32 y, u32 z, u32 type, u32 count, u32* out, u32 maxDist, u32 f0, u32 f1, u32 f2, u32 f3) {
    Ev& e = PushEv(0x44FA30, 12);
    e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z; e.v[4] = type; e.v[5] = count; e.v[6] = Tag(out); e.v[7] = maxDist; e.v[8] = f0; e.v[9] = f1; e.v[10] = f2; e.v[11] = f3;
    for (u32 i = 0; i < gNRec && i < 4; ++i) out[i] = gAddrs[i];
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
    std::array<u32, 20> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }
inline u32 VtblPtr() { return u32(arenaD.Obj(kVtbl)); }

// Mundo de nodos de CPathFind (ver Fixtures.hpp): punteros, contadores y enlaces en el objeto de la arena A, nodos en la C y tablas de enlaces en la D
inline void BuildWorld(u32 seed, u32 o) {
    const World w = MakeWorld(seed);
    for (u32 i = 0; i < 80; ++i) arenaA.Put32(o + 0x804 + 4 * i, 0);
    for (u32 k = 0; k < w.na; ++k) {
        const u32 id = w.id[k];
        arenaA.Put32(o + 0x804 + 4 * id, u32(arenaC.Obj(kNodeBase[k])));
        arenaA.Put32(o + 0xA44 + 4 * id, u32(arenaD.Obj(kLinkBase[k] + 12)));
        arenaA.Put32(o + 0xFA4 + 4 * id, w.n[k]);
        arenaA.Put32(o + 0x10C4 + 4 * id, w.nv[k]);
        for (u32 i = 0; i < 8; ++i) {
            const u32 node = kNodeBase[k] + 0x1C * i;
            arenaC.Put16(node + 8, NodeCoord(seed, k, i, 0)); arenaC.Put16(node + 0xA, NodeCoord(seed, k, i, 1)); arenaC.Put16(node + 0xC, NodeCoord(seed, k, i, 2));
            arenaC.Put16(node + 0x10, u32(NodeLinkBase(seed, k, i)) & 0xFFFFu);
            arenaC.Put8(node + 0x18, NodeFlags18(seed, k, i)); arenaC.Put8(node + 0x19, NodeFlags19(seed, k, i));
        }
        for (i32 j = -3; j <= 50; ++j) arenaD.Put32(kLinkBase[k] + 4 * u32(j + 3), LinkEntry(seed, k, j));
    }
}
// Entrada del falso de FindNodeClosestToCoors / Record a partir de un selector: area 0xFFFF (con palabra de nodo cualquiera) o un nodo de un area poblada
inline u32 ScriptedAddress(u32 seed, u32 sel) {
    if (sel % 8 == 0) return 0xFFFFu | ((sel >> 16) << 16);
    const World w = MakeWorld(seed);
    return (w.id[(sel >> 3) % w.na] & 0xFFFFu) | ((((sel >> 8) % 8) & 0xFFFFu) << 16);
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[kSeedAt]); arenaB.Pattern(a[2], a[kSeedAt + 1]); arenaC.Pattern(a[2], a[kSeedAt + 2]); arenaD.Pattern(a[2], a[kSeedAt + 3]);
    gRet = gDtorWrite = gTaskNew = gPlayerMask = gCall = gJunk = gMask = gAssoc91 = gAssoc92 = gOut = gFind = gSwapPd = gHitZ = gEntity = gSetWeapon = gNRec = gAdjMask = 0;
    gVec = {}; gProbeZ = {}; gAddrs = {};
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    call.ecx = self;
    // vtable falsa: GetTaskType (ranura 4); las demas anotan un evento de alarma
    for (u32 i = 0; i < 12; ++i) arenaD.Put32(kVtbl + 4 * i, u32(uintptr_t(&FkTrap)));
    arenaD.Put32(kVtbl + 0x10, u32(uintptr_t(&FkVirtType)));
    const u32 vt = VtblPtr();
    arenaA.Put32(o, a[3]);
    call.argCount = kArgs[c.fn];
    call.popBytes = kCdecl[c.fn] ? 4 * kArgs[c.fn] : 0;
    switch (kKind[c.fn]) {
    case kPhoneFirst: {
        const u32 ped = a[4];
        arenaC.Put32(ped + 0x474, a[5]); arenaC.Put32(ped + 0x478, a[6]); arenaC.Put32(ped + 0x70C, a[7]); arenaC.Put8(ped + 0x718, a[8]);
        arenaC.Put32(int64(ped) + 0x5A0 + int64(int8_t(a[8])) * 0x1C, a[9]);
        arenaC.Put32(ped + 0x480, a[10] ? u32(arenaD.Obj(a[10])) : 0u);
        if (a[10]) arenaD.Put8(a[10] + 0x85, a[15]);
        gPlayerMask = a[11]; gTaskNew = a[12] ? u32(arenaB.Obj(a[13])) : 0u; gDtorWrite = a[14];
        call.args[0] = u32(arenaC.Obj(ped));
        break;
    }
    case kPhoneControl: {
        const u32 ped = a[4], sub = a[11];
        arenaA.Put32(o + 8, u32(arenaC.Obj(sub)));
        arenaC.Put32(sub, vt); arenaC.Put32(sub + 0x30, a[12]);
        arenaC.Put32(ped + 0x18, u32(arenaD.Obj(a[5]))); arenaD.Put8(a[5], 2); arenaC.Put32(ped + 0x47C, u32(arenaD.Obj(a[6])));
        arenaC.Put32(ped + 0x480, a[7] ? u32(arenaD.Obj(a[7])) : 0u);
        if (a[7]) { arenaD.Put8(a[7] + 0x84, a[8]); arenaD.Put8(a[7] + 0x85, a[9]); arenaD.Put32(a[7] + 0x18, a[10]); }
        if (a[13]) { arenaD.Put32(a[13] + 0x20, a[15]); arenaD.Put32(a[13] + 0x28, a[16]); }
        if (a[14]) { arenaD.Put32(a[14] + 0x20, a[17]); arenaD.Put32(a[14] + 0x28, a[18]); }
        gAssoc91 = a[13] ? u32(arenaD.Obj(a[13])) : 0u; gAssoc92 = a[14] ? u32(arenaD.Obj(a[14])) : 0u;
        gOut = a[19]; gTaskNew = a[20] ? u32(arenaB.Obj(a[21])) : 0u; gDtorWrite = a[22];
        gFind = a[23] ? u32(arenaD.Obj(a[23])) : 0u; gSwapPd = a[24] ? u32(arenaD.Obj(a[25])) : 0u;
        call.args[0] = u32(arenaC.Obj(ped));
        break;
    }
    case kPhoneClone:
        arenaA.Put32(o + 0xC, a[4]);
        gTaskNew = a[5] ? u32(arenaB.Obj(a[6])) : 0u; gDtorWrite = a[7];
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6];
        break;
    case kSeqDtor: {
        arenaA.Put32(o + 0xC, a[4]);
        if (a[4] != 0xFFFFFFFFu) {
            const uintptr_t entry = kSeqTable + uintptr_t(a[4] << 6);
            PutRaw32(entry + 0x3C, a[5]); const u8 flag = u8(a[6]); PutRaw(entry + 0x38, &flag, 1);
        }
        gDtorWrite = a[7];
        break;
    }
    case kSweep: {
        for (u32 i = 0; i < 4; ++i) for (u32 k = 0; k < 3; ++k) arenaB.Put32(a[5 + i] + 4 * k, a[13 + 3 * i + k]);
        for (u32 i = 4; i < 8; ++i) for (u32 k = 0; k < 3; ++k) arenaC.Put32(a[5 + i] + 4 * k, a[13 + 3 * i + k]);
        call.args[0] = a[4];
        for (u32 i = 0; i < 8; ++i) call.args[1 + i] = i < 4 ? u32(arenaB.Obj(a[5 + i])) : u32(arenaC.Obj(a[5 + i]));
        call.args[9] = a[37]; call.args[10] = a[38]; call.args[11] = a[39]; call.args[12] = a[40];
        gMask = a[41]; gJunk = a[42];
        break;
    }
    case kProbe: {
        const u32 plane = a[4];
        for (u32 k = 0; k < 3; ++k) arenaC.Put32(plane + 0x44 + 4 * k, a[18 + k]);
        arenaC.Put32(plane + 0x14, a[5] ? u32(arenaD.Obj(a[5])) : 0u);
        for (u32 k = 0; k < 3; ++k) { if (a[5]) arenaD.Put32(a[5] + 0x30 + 4 * k, a[15 + k]); else arenaC.Put32(plane + 4 + 4 * k, a[15 + k]); }
        call.args[0] = u32(arenaC.Obj(plane)); call.args[1] = a[6]; call.args[2] = a[7]; call.args[3] = u32(arenaB.Obj(a[8]));
        gVec = {a[9], a[10], a[11]}; gRet = a[12]; gHitZ = a[13]; gEntity = a[14];
        break;
    }
    case kClear:
        call.args[0] = a[4]; call.args[1] = a[5]; call.floatRet = true;
        gMask = a[6]; for (u32 i = 0; i < 6; ++i) gProbeZ[i] = a[7 + i];
        gJunk = a[13];
        break;
    case kPoolRef: {
        const i32 idx = i32(a[6]);
        arenaA.Put32(o + 0, a[4]); arenaA.Put32(o + 4, u32(arenaD.Obj(a[5])));
        arenaD.Put8(int64(a[5]) + idx, a[8]);
        i32 diff = idx * 0xA18;
        const i32 rem = i32(a[7]);
        diff += idx < 0 ? -rem : (idx > 0 ? rem : ((a[8] & 1) ? -rem : rem));
        call.args[0] = a[4] + u32(diff);
        break;
    }
    case kNop:
        *reinterpret_cast<u8*>(kMarker) = 0x5A;
        for (u32 i = 0; i < kArgs[c.fn]; ++i) call.args[i] = a[4 + i];
        break;
    case kNorm:
        arenaA.Put32(o, a[4]); arenaA.Put32(o + 4, a[5]);
        break;
    case kRecord:
        BuildWorld(a[4], o);
        for (u32 i = 0; i < 4; ++i) gAddrs[i] = ScriptedAddress(a[4], a[16 + i]);
        call.args = {a[5], a[6], a[7], a[8], a[9], u32(arenaB.Obj(a[10])), a[11], a[12], a[13], a[14], a[15]};
        break;
    case kCircle:
        BuildWorld(a[4], o);
        for (u32 k = 0; k < 3; ++k) arenaB.Put32(a[8] + 4 * k, a[5 + k]);
        call.args = {u32(arenaB.Obj(a[8])), a[9], a[10], a[11], u32(arenaB.Obj(a[12])), a[13], a[14], a[15], a[16], a[17]};
        break;
    case kPair:
        BuildWorld(a[4], o);
        arenaB.Put32(a[9], a[20]); arenaB.Put32(a[10], a[21]); arenaB.Put32(a[11], a[22]);
        gVec = {a[17], a[18], a[19]};
        call.args = {a[5], a[6], a[7], a[8], u32(arenaB.Obj(a[9])), u32(arenaB.Obj(a[10])), u32(arenaB.Obj(a[11])), a[12], a[13], a[14], a[15], a[16]};
        break;
    case kFind2:
        for (u32 i = 0; i < 4; ++i) {
            const u32 sel = a[11 + i];
            gAddrs[i] = sel % 8 == 0 ? (0xFFFFu | ((sel >> 16) << 16)) : (((sel >> 3) % 72) | ((sel >> 16) << 16));
        }
        gNRec = a[10]; gAdjMask = a[15]; gJunk = a[16] * 0x9E3779B1u;
        call.args = {a[4], a[5], a[6], u32(arenaB.Obj(a[7])), u32(arenaB.Obj(a[8])), a[9]};
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
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        const u32 floatBits = gFloatRet;
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kPhoneFirst: case kPhoneControl: case kPhoneClone: case kSdd:
            obs["ret"] = Tag(uintptr_t(eax)); break;                                                                  // this / el objeto nuevo / un puntero como etiqueta de arena
        case kSweep: case kProbe: obs["ret"] = eax & 0xFFu; break;                                                    // bool: solo AL
        case kClear: obs["ret"] = floatBits; break;                                                                    // bits del float de ST0
        case kPoolRef: obs["ret"] = json::array({eax, Tag(uintptr_t(sides[s].reg.ecx)), sides[s].reg.edx}); break;    // EAX, ECX (etiqueta) y EDX
        case kCircle: obs["ret"] = eax; break;                                                                         // contador completo
        default: obs["ret"] = nullptr; break;                                                                          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN207A_CASES (solo para las rondas de la prueba negativa y las pruebas rapidas, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN207A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn207a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn207a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn207a"); }
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (0x6348E0, 0x6348F0): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    env.stubs[0] = {F(0x5DF8F0, &FkIsPlayer), F(0x609C80, &FkVoidThis<0x609C80>), F(0x5E6280, &FkSetWeapon), F(0x50AB10, &FkVoidThis<0x50AB10>), FV(0x62F370, &FkStandProcess), F(0x61A5A0, &FkTaskNew), F(0x61A900, &FkRunAnim)};
    env.stubs[1] = {F(0x4D68B0, &FkAssoc), F(0x5E5ED0, &FkAddModel), ByExeAddress(0x6348F0, &FkQuitModel), F(0x420E30, &FkOutOfTime), F(0x61A5A0, &FkTaskNew), F(0x61A900, &FkRunAnim), F(0x600EE0, &FkFindTask),
                    F(0x6883D0, &FkZelda)};
    env.stubs[2] = {F(0x61A5A0, &FkTaskNew), F(0x61A3B0, &FkBody<0x61A3B0>)};
    env.stubs[3] = {ByExeAddress(0x6348E0, &FkBody<0x6348E0>), Del()};
    env.stubs[4] = {F(0x634F20, &FkBody<0x634F20>), Del()};
    env.stubs[5] = {F(0x635100, &FkBody<0x635100>), Del()};
    env.stubs[6] = {F(0x632C10, &FkFlush), F(0x61A3D0, &FkBody<0x61A3D0>)};
    env.stubs[7] = {F(0x4138D0, &FkTest2D)};
    env.stubs[8] = {F(0x59C910, &FkNormalise), F(0x56BA00, &FkLos)};
    env.stubs[9] = {Rt(8, &FkProbe)};
    env.stubs[18] = {F(0x44F460, &FkFindClosest)};
    env.stubs[20] = {F(0x59C910, &FkNormalise)};
    env.stubs[21] = {Rt(18, &FkRecord), F(0x44D230, &FkAdjacent)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 19u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada; se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kSeqTable, kSeqBytes); regionsOf[6] = {regions.size() - 1};
    regions.emplace_back(kMarker, 1); for (unsigned f : {11u, 12u, 13u, 14u, 15u, 16u}) regionsOf[f] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
