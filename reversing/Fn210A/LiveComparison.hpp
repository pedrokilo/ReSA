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

// Banco par a par Fn210A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 17 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40; C mide 0x4000). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) para los que tienen hook RH.
// Las llamadas virtuales (GetTaskType, MakeAbortable, el calculador de GetSeekPos) usan una vtable falsa al final de la arena D. Memoria global del juego tocada (restaurada tras cada lado): CModelInfo::ms_modelInfoPtrs
// (indices -3..7), CWorld::PlayerInFocus, Players[0..3].m_pRemoteVehicle, las 8 entradas de handling usadas, CTimer::m_snTimeInMilliseconds y CTimer::m_FrameCounter.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en constantes) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F210ALive {
using namespace F210AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F210AFix::Arena(WIN);
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
inline Arena<kWin> arenaA, arenaB, arenaD;
inline Arena<kWinC> arenaC;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de CreateSubTask
inline u32 gRet2{};                  // EAX que devuelven los falsos de GetNextSubTaskType y del CreateSubTask de CTaskComplexSeekEntity
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [objeto]
inline std::array<u32, 4> gAllocs{}; // lo que devuelve CTask::operator new, por orden de llamada (un objeto de la arena D o nulo)
inline unsigned gAllocIdx{};
inline std::array<u32, 36> gNorm{};  // salidas del falso de CVector::Normalise, por llamada
inline unsigned gNormCall{};
inline u32 gAbortMask{}, gAbortCall{};
inline u32 gSame{}, gMember{}, gIsPlayer{}, gPlayerPed{}, gFindZ{}, gFindZNew{}, gVeh{}, gAtan{}, gLimit{};
inline std::array<u32, 3> gVec{}, gDiff{}, gMul{};
inline DWORD gBenchThread{};

// Marcas de los eventos de las llamadas virtuales (no son direcciones del EXE)
inline constexpr u32 kEvType = 0x50000001u, kEvCalc = 0x50000002u, kEvEvent = 0x50000003u, kEvAbort = 0x50000004u, kEvTrap = 0x5000FFFFu;

// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion, ECX (etiqueta), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
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
// CTask::operator new (0x61A5A0; cdecl, el tamano): evento [direccion, tamano]; devuelve la siguiente reserva de gAllocs (un objeto de la arena D o nulo).
inline void* __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gAllocs[gAllocIdx < gAllocs.size() ? gAllocIdx++ : gAllocs.size() - 1]));
}
// CPatrolRoute::Reverse (0x66D550; thiscall, sin pila): evento [direccion, ECX (etiqueta), dword de [ECX]].
inline void __fastcall FkReverse(void* self, void*) {
    Ev& e = PushEv(0x66D550, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
}
// CVector::Normalise (0x59C910; thiscall, sin pila): evento [direccion, ECX (etiqueta), x, y, z de entrada]; escribe las tres salidas de la llamada actual (gNorm).
inline void __fastcall FkNormalise(void* self, void*) {
    Ev& e = PushEv(0x59C910, 4);
    u32* v = static_cast<u32*>(self);
    e.v[0] = Tag(self); e.v[1] = v[0]; e.v[2] = v[1]; e.v[3] = v[2];
    const unsigned at = 3 * (gNormCall < 12 ? gNormCall : 11);
    ++gNormCall;
    v[0] = gNorm[at]; v[1] = gNorm[at + 1]; v[2] = gNorm[at + 2];
}
// Llamadas virtuales (vtable falsa de la arena D): GetTaskType (ranura 4), calculador (ranura 0), GetEventType (ranura 1) y MakeAbortable (ranura 6)
inline u32 __fastcall FkVirtType(void* self, void*) {
    Ev& e = PushEv(kEvType, 1);
    e.v[0] = Tag(self);
    return BitsOf(reinterpret_cast<const u8*>(self) + 0x30);
}
inline u32 __fastcall FkVirtEvent(void* self, void*) {
    Ev& e = PushEv(kEvEvent, 1);
    e.v[0] = Tag(self);
    return BitsOf(reinterpret_cast<const u8*>(self) + 0x30);
}
// MakeAbortable (ranura 6; thiscall, ped, prioridad y evento, `ret 0Ch`): evento [marca, ECX (etiqueta), ped (etiqueta), prioridad, evento (etiqueta)]; devuelve un bit de gAbortMask por llamada.
inline u32 __fastcall FkMakeAbort(void* self, void*, void* ped, u32 priority, void* event) {
    Ev& e = PushEv(kEvAbort, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = priority; e.v[3] = Tag(event);
    return (gAbortMask >> (gAbortCall++ & 31u)) & 1u;
}
inline u32 __fastcall FkTrap(void* self, void*) {
    Ev& e = PushEv(kEvTrap, 1);
    e.v[0] = Tag(self);
    return 0;
}
// Calculador de GetSeekPos (ranura 0; thiscall, ped, entidad y salida, `ret 0Ch`): evento [marca, ECX (etiqueta), ped, entidad, salida (etiquetas)]; escribe gVec en la salida.
inline void __fastcall FkCalc(void* self, void*, void* ped, void* entity, u32* out) {
    Ev& e = PushEv(kEvCalc, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = Tag(entity); e.v[3] = Tag(out);
    out[0] = gVec[0]; out[1] = gVec[1]; out[2] = gVec[2];
}
// Constructores (thiscall; dejan gDtorWrite en [ECX] y devuelven this, como el constructor real). Eventos [direccion, ECX (etiqueta), argumentos (los punteros como etiquetas)].
#define FK_CTOR_TAIL memcpy(self, &gDtorWrite, 4); return u32(uintptr_t(self))
inline u32 __fastcall FkGoToCtor(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e5) {                       // CTaskSimpleGoToPoint 0x667CD0
    Ev& e = PushEv(0x667CD0, 6);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = Tag(uintptr_t(b)); e.v[3] = c; e.v[4] = d; e.v[5] = e5;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkRunNamed(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9) {   // CTaskSimpleRunNamedAnim 0x61A990
    Ev& e = PushEv(0x61A990, 10);
    const u32 v[9]{Tag(uintptr_t(a1)), Tag(uintptr_t(a2)), a3, a4, a5, a6, a7, a8, a9};
    e.v[0] = Tag(self);
    for (u32 i = 0; i < 9; ++i) e.v[1 + i] = v[i];
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkSimpleCtor(void* self, void*) {                                                           // CTaskSimple 0x61A390
    Ev& e = PushEv(0x61A390, 1);
    e.v[0] = Tag(self);
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkGotoStandPtr(void* self, void*, u32 ms, u32 pos, u32 maxDist, u32 msRadius, u32 b1, u32 b2) {   // CTaskComplexGoToPointAndStandStill 0x668120 (el punto es un puntero a la ruta: solo se etiqueta)
    Ev& e = PushEv(0x668120, 7);
    e.v[0] = Tag(self); e.v[1] = ms; e.v[2] = Tag(uintptr_t(pos)); e.v[3] = maxDist; e.v[4] = msRadius; e.v[5] = b1; e.v[6] = b2;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkGotoStandVec(void* self, void*, u32 ms, const u32* pos, u32 maxDist, u32 msRadius, u32 b1, u32 b2) {   // idem, con el punto en la pila (se anotan sus tres dwords)
    Ev& e = PushEv(0x668120, 10);
    e.v[0] = Tag(self); e.v[1] = ms; e.v[2] = Tag(pos); e.v[3] = pos[0]; e.v[4] = pos[1]; e.v[5] = pos[2]; e.v[6] = maxDist; e.v[7] = msRadius; e.v[8] = b1; e.v[9] = b2;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkFollowNode(void* self, void*, u32 ms, const u32* pos, u32 f1, u32 f2, u32 f3, u32 b, u32 i, u32 b2) {   // CTaskComplexFollowNodeRoute 0x66EA30 (el punto siempre esta en la pila)
    Ev& e = PushEv(0x66EA30, 12);
    e.v[0] = Tag(self); e.v[1] = ms; e.v[2] = Tag(pos); e.v[3] = pos[0]; e.v[4] = pos[1]; e.v[5] = pos[2]; e.v[6] = f1; e.v[7] = f2; e.v[8] = f3; e.v[9] = b; e.v[10] = i; e.v[11] = b2;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkTired(void* self, void*, u32 a) {                                                         // CTaskSimpleTired 0x630F20
    Ev& e = PushEv(0x630F20, 2);
    e.v[0] = Tag(self); e.v[1] = a;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkStand(void* self, void*, u32 a, u32 b, u32 c, u32 d) {                                    // CTaskSimpleStandStill 0x62F310
    Ev& e = PushEv(0x62F310, 5);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkSeqCtor(void* self, void*) {                                                              // CTaskComplexSequence 0x632BD0
    Ev& e = PushEv(0x632BD0, 1);
    e.v[0] = Tag(self);
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkSeqAdd(void* self, void*, void* task) {                                                   // CTaskComplexSequence::AddTask 0x632D10
    Ev& e = PushEv(0x632D10, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(task);
    return 0;
}
inline u32 __fastcall FkTurn(void* self, void*, u32 entity, u32 f1, u32 f2) {                                     // CTaskComplexTurnToFaceEntityOrCoord 0x66B890
    Ev& e = PushEv(0x66B890, 4);
    e.v[0] = Tag(self); e.v[1] = entity; e.v[2] = f1; e.v[3] = f2;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkLeave(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e5) {                            // CTaskComplexLeaveCar 0x63B8C0
    Ev& e = PushEv(0x63B8C0, 6);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(a)); e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = e5;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkCarDrive(void* self, void*, u32 vehicle, u32 time) {                                      // CTaskSimpleCarDriveTimed 0x5FF940
    Ev& e = PushEv(0x5FF940, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(vehicle)); e.v[2] = time;
    FK_CTOR_TAIL;
}
inline u32 __fastcall FkSeekCtor(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8) {   // CTaskComplexSeekEntity<FixedPos>::Constructor 0x66CA40
    Ev& e = PushEv(0x66CA40, 9);
    const u32 v[8]{a1, a2, a3, a4, a5, a6, a7, a8};
    e.v[0] = Tag(self);
    for (u32 i = 0; i < 8; ++i) e.v[1 + i] = v[i];
    FK_CTOR_TAIL;
}
// Funciones propias del lote usadas como colaboradores (con el mismo falso en el EXE y en el cuerpo de la DLL)
inline u32 __fastcall FkCreateSubFp(void* self, void*, u32 type) {                                                // CTaskComplexFollowPatrolRoute::CreateSubTask 0x670B00
    Ev& e = PushEv(0x670B00, 2);
    e.v[0] = Tag(self); e.v[1] = type;
    return gRet;
}
inline u32 __fastcall FkNextTypeFp(void* self, void*) {                                                           // CTaskComplexFollowPatrolRoute::GetNextSubTaskType 0x670A70
    Ev& e = PushEv(0x670A70, 1);
    e.v[0] = Tag(self);
    return gRet2;
}
inline u32 __fastcall FkSeekCreateSub(void* self, void*, u32 type, void* ped) {                                   // CTaskComplexSeekEntity<FixedPos>::CreateSubTask 0x673070
    Ev& e = PushEv(0x673070, 3);
    e.v[0] = Tag(self); e.v[1] = type; e.v[2] = Tag(ped);
    return gRet2;
}
inline u32 __fastcall FkSeekGetPos(void* self, void*, u32* out, void* ped) {                                      // CTaskComplexSeekEntity<FixedPos>::GetSeekPos 0x673BA0
    Ev& e = PushEv(0x673BA0, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(out); e.v[2] = Tag(ped);
    out[0] = gVec[0]; out[1] = gVec[1]; out[2] = gVec[2];
    return u32(uintptr_t(out));
}
// Colaboradores de ControlSubTask
inline u32 __cdecl FkSame(void* ped, void* entity) {                                                              // CPedGroups::AreInSameGroup 0x5F7F40
    Ev& e = PushEv(0x5F7F40, 2);
    e.v[0] = Tag(ped); e.v[1] = Tag(entity);
    return gSame;
}
inline void __fastcall FkDettach(void* self, void*) {                                                             // CPed::DettachPedFromEntity 0x5E7EC0
    Ev& e = PushEv(0x5E7EC0, 1);
    e.v[0] = Tag(self);
}
inline void __fastcall FkDifference(void* self, void*, u32* a, u32* b) {                                          // CVector::Difference 0x40FE00 (this = salida)
    Ev& e = PushEv(0x40FE00, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(a); e.v[2] = Tag(b);
    u32* out = static_cast<u32*>(self);
    out[0] = gDiff[0]; out[1] = gDiff[1]; out[2] = gDiff[2];
}
inline u32 __cdecl FkFindZ(u32* pos) {                                                                            // CPedPlacement::FindZCoorForPed 0x616920
    Ev& e = PushEv(0x616920, 4);
    e.v[0] = Tag(pos); e.v[1] = pos[0]; e.v[2] = pos[1]; e.v[3] = pos[2];
    if (gFindZ & 1) pos[2] = gFindZNew;
    return gFindZ;
}
inline u32 __cdecl FkInvert(void* src, void* dst) {                                                               // CMatrix::Invert 0x59B920
    Ev& e = PushEv(0x59B920, 2);
    e.v[0] = Tag(src); e.v[1] = Tag(dst);
    return u32(uintptr_t(dst));
}
inline void __fastcall FkFromMul(void* self, void*, void* matrix, const u32* vec) {                               // CVector::FromMultiply3x3 0x59C6D0 (this = salida)
    Ev& e = PushEv(0x59C6D0, 6);
    e.v[0] = Tag(self); e.v[1] = Tag(matrix); e.v[2] = Tag(vec); e.v[3] = vec[0]; e.v[4] = vec[1]; e.v[5] = vec[2];
    u32* out = static_cast<u32*>(self);
    out[0] = gMul[0]; out[1] = gMul[1]; out[2] = gMul[2];
}
inline float __cdecl FkAtan(u32 x1, u32 y1, u32 x2, u32 y2) {                                                     // CGeneral::GetRadianAngleBetweenPoints 0x53CBE0 (ST0)
    Ev& e = PushEv(0x53CBE0, 4);
    e.v[0] = x1; e.v[1] = y1; e.v[2] = x2; e.v[3] = y2;
    return std::bit_cast<float>(gAtan);
}
inline float __cdecl FkLimit(u32 angle) {                                                                         // CGeneral::LimitRadianAngle 0x53CB50 (ST0)
    Ev& e = PushEv(0x53CB50, 1);
    e.v[0] = angle;
    return std::bit_cast<float>(gLimit);
}
inline void __fastcall FkAttach(void* self, void*, void* boat, u32 vx, u32 vy, u32 vz, u32 heading, u32 f, u32 weapon) {   // CPed::AttachPedToEntity 0x5E7CB0
    Ev& e = PushEv(0x5E7CB0, 8);
    e.v[0] = Tag(self); e.v[1] = Tag(boat); e.v[2] = vx; e.v[3] = vy; e.v[4] = vz; e.v[5] = heading & 0xFFFFu; e.v[6] = f; e.v[7] = weapon;
}
inline void __fastcall FkMatDtor(void* self, void*) {                                                             // CMatrix::~CMatrix 0x59ACD0
    Ev& e = PushEv(0x59ACD0, 1);
    e.v[0] = Tag(self);
}
inline void __fastcall FkSetTarget(void* self, void*, void* ped, const u32* pos, u32 maxDist, u32 msRadius, u32 thr, u32 b) {   // CTaskComplexFollowNodeRoute::SetTarget 0x671750
    Ev& e = PushEv(0x671750, 10);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = Tag(pos); e.v[3] = pos[0]; e.v[4] = pos[1]; e.v[5] = pos[2]; e.v[6] = maxDist; e.v[7] = msRadius; e.v[8] = thr; e.v[9] = b;
}
inline void __fastcall FkGoToPoint(void* self, void*, const u32* pos, u32 maxDist, u32 msRadius, u32 b) {         // CTaskComplexGoToPointAndStandStill::GoToPoint 0x46FE60
    Ev& e = PushEv(0x46FE60, 8);
    e.v[0] = Tag(self); e.v[1] = Tag(pos); e.v[2] = pos[0]; e.v[3] = pos[1]; e.v[4] = pos[2]; e.v[5] = maxDist; e.v[6] = msRadius; e.v[7] = b;
}
inline u32 __fastcall FkIsPlayer(void* self, void*) {                                                             // CPed::IsPlayer 0x5DF8F0
    Ev& e = PushEv(0x5DF8F0, 1);
    e.v[0] = Tag(self);
    return gIsPlayer;
}
inline u32 __cdecl FkFindPlayerPed(u32 id) {                                                                      // CWorld::FindPlayerPed 0x56E210
    Ev& e = PushEv(0x56E210, 1);
    e.v[0] = id;
    return gPlayerPed;
}
inline u32 __fastcall FkIsMember(void* self, void*, void* ped) {                                                  // CPedGroupMembership::IsMember 0x5F6A10
    Ev& e = PushEv(0x5F6A10, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ped);
    return gMember;
}
// CPed::Say (0x5EFFE0; thiscall, seis dwords de pila, `ret 18h`)
inline void __fastcall FkSay(void* self, void*, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5) {
    Ev& e = PushEv(0x5EFFE0, 7);
    e.v[0] = Tag(self); e.v[1] = a0; e.v[2] = a1; e.v[3] = a2; e.v[4] = a3; e.v[5] = a4; e.v[6] = a5;
}
// CWorld::FindPlayerVehicle (0x56E0D0; cdecl, dos dwords): evento [direccion, jugador, solo conductor]; devuelve gVeh.
inline u32 __cdecl FkFindPlayerVehicle(u32 player, u32 onlyDriver) {
    Ev& e = PushEv(0x56E0D0, 2);
    e.v[0] = player; e.v[1] = onlyDriver;
    return gVeh;
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
    std::array<u32, 16> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};

inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }
inline u32 VtblPtr() { return u32(arenaD.Obj(kVtbl)); }
inline u32 Sel3(u32 s, u32 v1, u32 v2, u32 raw) { return s == 0 ? 0u : (s == 1 ? v1 : (s == 2 ? v2 : raw)); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[kSeedAt]); arenaB.Pattern(a[2], a[kSeedAt + 1]); arenaC.Pattern(a[2], a[kSeedAt + 2]); arenaD.Pattern(a[2], a[kSeedAt + 3]);
    gRet = gRet2 = gDtorWrite = gAllocIdx = gNormCall = gAbortMask = gAbortCall = gSame = gMember = gIsPlayer = gPlayerPed = gFindZ = gFindZNew = gVeh = gAtan = gLimit = 0;
    gAllocs = {}; gNorm = {}; gVec = {}; gDiff = {}; gMul = {};
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = self;
    // vtable falsa: GetTaskType (ranura 4), calculador (ranura 0), GetEventType (ranura 1), MakeAbortable (ranura 6); las demas anotan un evento de alarma
    for (u32 i = 0; i < 12; ++i) arenaD.Put32(kVtbl + 4 * i, u32(uintptr_t(&FkTrap)));
    arenaD.Put32(kVtbl + 0, u32(uintptr_t(&FkCalc))); arenaD.Put32(kVtbl + 4, u32(uintptr_t(&FkVirtEvent))); arenaD.Put32(kVtbl + 0x10, u32(uintptr_t(&FkVirtType))); arenaD.Put32(kVtbl + 0x18, u32(uintptr_t(&FkMakeAbort)));
    const u32 vt = VtblPtr();
    switch (kKind[c.fn]) {
    case kConst:
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kFpNext:
        arenaA.Put32(o + 0x1C, u32(arenaB.Obj(a[4]))); arenaB.Put32(a[4], a[5]);
        arenaA.Put16(o + 0xC, a[7]); arenaA.Put16(o + 0xE, a[8]); arenaA.Put16(o + 0x12, a[6]); arenaA.Put16(o + 0x10, a[9]); arenaA.Put32(o + 0x20, a[10]);
        break;
    case kFpCreate:
        arenaA.Put32(o + 0x1C, u32(arenaB.Obj(a[5]))); arenaB.Put32(a[5], a[6]);
        arenaA.Put16(o + 0xC, a[7]); arenaA.Put16(o + 0xE, a[8]); arenaA.Put16(o + 0x10, a[9]); arenaA.Put16(o + 0x12, a[10]);
        arenaA.Put32(o + 0x14, a[11]); arenaA.Put32(o + 0x18, a[12]); arenaA.Put32(o + 0x20, a[13]);
        gAllocs[0] = a[14] ? u32(arenaD.Obj(a[15])) : 0u; gDtorWrite = a[16];
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kFpNextSub:
        arenaA.Put32(o + 8, u32(arenaC.Obj(a[4])));
        arenaC.Put32(a[4], vt); arenaC.Put32(a[4] + 0x30, a[5]);
        arenaA.Put32(o + 0x1C, u32(arenaB.Obj(a[9]))); arenaA.Put16(o + 0x12, a[10]);
        arenaB.Put8(u32(i32(a[9]) + 4 + 0x28 * i32(i16(a[10]))), a[11]);
        gRet = a[7]; gRet2 = a[8];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kFpFirst: {
        arenaA.Put32(o + 0x1C, u32(arenaB.Obj(a[14]))); arenaB.Put32(a[14], a[13]);
        for (u32 p = 0; p < 12; ++p) for (u32 k = 0; k < 3; ++k) arenaB.Put32(a[14] + 0x144 + 12 * p + 4 * k, a[15 + 3 * p + k]);
        arenaA.Put32(o + 0x20, a[87]); arenaA.Put16(o + 0x12, a[88]); arenaA.Put16(o + 0xC, a[89]); arenaA.Put16(o + 0xE, a[90]); arenaA.Put16(o + 0x10, a[91]);
        for (u32 k = 0; k < 3; ++k) { arenaC.Put32(a[4] + 4 + 4 * k, a[7 + k]); arenaD.Put32(a[6] + 0x30 + 4 * k, a[10 + k]); }
        arenaC.Put32(a[4] + 0x14, a[5] ? u32(arenaD.Obj(a[6])) : 0u);
        for (u32 i = 0; i < 36; ++i) gNorm[i] = a[51 + i];
        gRet = a[92]; gRet2 = a[93];
        call.args[0] = u32(arenaC.Obj(a[4])); call.argCount = 1;
        break;
    }
    case kClone:
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x10, a[5]); arenaA.Put32(o + 0x14, a[6]); arenaA.Put32(o + 0x18, a[7]); arenaA.Put32(o + 0x1C, a[8]); arenaA.Put32(o + 0x20, a[9]); arenaA.Put32(o + 0x24, a[10]);
        arenaA.Put32(o + 0x44, a[11]); arenaA.Put32(o + 0x48, a[12]); arenaA.Put32(o + 0x4C, a[13]); arenaA.Put32(o + 0x50, a[14]); arenaA.Put8(o + 0x54, a[15]);
        gAllocs[0] = u32(arenaD.Obj(a[16])); gDtorWrite = a[17];
        break;
    case kSeekCreate:
        arenaA.Put32(o + 0xC, a[6]); arenaA.Put32(o + 0x18, a[7]); arenaA.Put32(o + 0x1C, a[8]); arenaA.Put32(o + 0x24, a[9]); arenaA.Put32(o + 0x50, a[10]); arenaA.Put8(o + 0x54, a[11]); arenaA.Put32(o + 0x40, vt);
        arenaC.Put32(a[5] + 0x58C, a[12]);
        gVec = {a[13], a[14], a[15]};
        for (u32 i = 0; i < 3; ++i) gAllocs[i] = a[16 + 2 * i] ? u32(arenaD.Obj(a[17 + 2 * i])) : 0u;
        gDtorWrite = a[22];
        call.args[0] = a[4]; call.args[1] = u32(arenaC.Obj(a[5])); call.argCount = 2;
        break;
    case kSeekControl: {
        const u32 eP = u32(arenaC.Obj(a[5])), v1 = u32(arenaC.Obj(a[10])), v2 = u32(arenaC.Obj(a[11])), pP = u32(arenaC.Obj(a[16])), ppP = u32(arenaC.Obj(a[78]));
        const u32 entity = a[4] ? 0u : eP;
        arenaA.Put32(o + 0xC, entity);
        arenaA.Put32(o + 0x14, a[58]); arenaA.Put32(o + 0x18, a[59]); arenaA.Put32(o + 0x1C, a[60]); arenaA.Put32(o + 0x20, a[61]); arenaA.Put32(o + 0x24, a[62]);
        arenaA.Put32(o + 0x28, a[50]); arenaA.Put32(o + 0x2C, a[51]); arenaA.Put8(o + 0x30, a[52]); arenaA.Put8(o + 0x31, a[53]);
        arenaA.Put32(o + 0x34, a[54]); arenaA.Put32(o + 0x38, a[55]); arenaA.Put8(o + 0x3C, a[56]); arenaA.Put8(o + 0x3D, a[57]);
        arenaA.Put32(o + 0x40, vt); arenaA.Put32(o + 0x50, a[63]); arenaA.Put8(o + 0x54, a[64]);
        arenaA.Put32(o + 8, u32(arenaD.Obj(a[65])));
        arenaD.Put32(a[65], vt); arenaD.Put32(a[65] + 0x30, a[66]); arenaD.Put8(a[65] + 0x24, a[67]);
        // entidad a buscar (E)
        arenaC.Put8(a[5] + 0x36, a[6]); arenaC.Put32(a[5] + 0x40, a[7]);
        arenaC.Put32(a[5] + 0x568, Sel3(a[8], v1, v2, eP)); arenaC.Put32(a[5] + 0x584, Sel3(a[9], v1, v2, eP));
        // vehiculos V1 y V2
        arenaC.Put8(a[10] + 0x36, a[12]); arenaC.Put32(a[10] + 0x590, a[13]); arenaC.Put8(a[11] + 0x36, a[14]); arenaC.Put32(a[11] + 0x590, a[15]);
        const u32 vOff[2]{a[10], a[11]}, vMat[2]{a[36], a[44]}, vMode[2]{a[35], a[43]};
        for (u32 i = 0; i < 2; ++i) {
            const u32 pos = i ? 40u : 32u, mpos = i ? 45u : 37u;
            for (u32 k = 0; k < 3; ++k) { arenaC.Put32(vOff[i] + 4 + 4 * k, a[pos + k]); arenaD.Put32(vMat[i] + 0x30 + 4 * k, a[mpos + k]); }
            arenaC.Put32(vOff[i] + 0x14, vMode[i] ? u32(arenaD.Obj(vMat[i])) : 0u);
        }
        // ped (argumento)
        arenaC.Put32(a[16] + 0x568, Sel3(a[17], v1, v2, a[48] | 1u)); arenaC.Put32(a[16] + 0x584, Sel3(a[18], v1, v2, a[49] | 1u)); arenaC.Put32(a[16] + 0xFC, a[19]);
        for (u32 k = 0; k < 3; ++k) { arenaC.Put32(a[16] + 4 + 4 * k, a[22 + k]); arenaD.Put32(a[21] + 0x30 + 4 * k, a[25 + k]); }
        arenaC.Put32(a[16] + 0x14, a[20] ? u32(arenaD.Obj(a[21])) : 0u);
        arenaC.Put32(a[16] + 0x534, a[28]); arenaC.Put16(a[16] + 0x20, a[29]); arenaC.Put8(a[16] + 0x718, a[30]); arenaC.Put32(a[16] + 0x5A0 + 0x1C * (a[30] % 13), a[31]);
        // jugador (FindPlayerPed) y sus datos
        arenaC.Put32(a[78] + 0x480, u32(arenaD.Obj(a[79]))); arenaD.Put32(a[79] + 0x38, a[80]);
        gPlayerPed = ppP;
        PutRaw32(kTimeMs, a[48]); PutRaw32(kFrame, a[49]);
        gAbortMask = a[68]; gVec = {a[69], a[70], a[71]}; gIsPlayer = a[72]; gSame = a[73]; gMember = a[74];
        gAllocs[0] = a[75] ? u32(arenaD.Obj(a[76])) : 0u; gDtorWrite = a[77];
        gDiff = {a[81], a[82], a[83]}; for (u32 i = 0; i < 3; ++i) gNorm[i] = a[84 + i];
        gFindZ = a[87]; gFindZNew = a[88]; gMul = {a[89], a[90], a[91]}; gRet2 = a[92]; gAtan = a[93]; gLimit = a[94];
        call.args[0] = pP; call.argCount = 1;
        break;
    }
    case kSeekPos:
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x40, vt);
        gVec = {a[6], a[7], a[8]};
        call.args[0] = u32(arenaB.Obj(a[9])); call.args[1] = a[5]; call.argCount = 2;
        break;
    case kDebug: {
        arenaA.Put16(o + 0x22, a[4]);
        for (u32 k = 0; k < kModelSlots; ++k) {
            PutRaw32(kModelTable + 4u * u32(int(k) + kModelLo), u32(arenaD.Obj(0x40 * k)));
            arenaD.Put8(0x40 * k + 0x4A, kHandlingIds[a[46 + k] & 7]);
        }
        const u8 focus = u8(a[6]); PutRaw(kPlayerInFocus, &focus, 1);
        const u32 other = a[44];
        for (u32 k = 0; k < kPlayerCount; ++k) PutRaw32(kPlayers + kPlayerStride * k + kRemoteAt, Sel3(a[7 + k], self, other, other));
        for (u32 e = 0; e < 8; ++e) {
            const uintptr_t at = kHandling + kHandlingStride * kHandlingIds[e];
            PutRaw32(at + 4, a[12 + 4 * e]); PutRaw32(at + 0xC, a[13 + 4 * e]); PutRaw32(at + 0x10, a[14 + 4 * e]); PutRaw32(at + 0x24, a[15 + 4 * e]);
        }
        gVeh = Sel3(a[11], self, other, other);
        break;
    }
    case kRemove: {
        const u32 other = a[9];
        for (u32 k = 0; k < 4; ++k) arenaA.Put32(o + 0x8E8 + 4 * k, a[5 + k] == 0 ? 0u : (a[5 + k] == 1 ? a[4] : other));
        call.args[0] = a[4]; call.argCount = 1;
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
        const u32 floatBits = gFloatRet;
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kSdd: case kFpCreate: case kFpNextSub: case kFpFirst: case kClone: case kSeekCreate: case kSeekControl: case kSeekPos:
            obs["ret"] = Tag(uintptr_t(eax)); break;                                                                 // this / el objeto nuevo / un puntero como etiqueta de arena
        case kConst: case kFpNext: obs["ret"] = eax; break;                                                          // valor completo de EAX
        default: obs["ret"] = nullptr; break;                                                                        // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN210A_CASES (solo para las rondas de la prueba negativa y las pruebas rapidas, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN210A_CASES", buffer, sizeof(buffer)) > 0) {
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
        char only[16]{};   // solo para el desarrollo del banco: FN210A_ONLY=<indice> corre una sola funcion (la corrida final y las rondas negativas no lo usan)
        const int onlyFn = GetEnvironmentVariableA("FN210A_ONLY", only, sizeof(only)) > 0 ? std::atoi(only) : -1;
        for (unsigned f = 0; f < kFunctions; ++f) {
            if (onlyFn >= 0 && int(f) != onlyFn) continue;
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
    if (!std::filesystem::exists(dir / "fn210a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn210a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn210a"); }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (0x5FFFD0): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    env.stubs[1] = {F(0x66D550, &FkReverse)};
    env.stubs[2] = {F(0x61A5A0, &FkTaskNew), F(0x667CD0, &FkGoToCtor), F(0x61A990, &FkRunNamed), F(0x61A390, &FkSimpleCtor), F(0x668120, &FkGotoStandPtr), F(0x66EA30, &FkFollowNode)};
    env.stubs[3] = {Rt(2, &FkCreateSubFp), Rt(1, &FkNextTypeFp)};
    env.stubs[4] = {F(0x59C910, &FkNormalise), Rt(2, &FkCreateSubFp), Rt(1, &FkNextTypeFp)};
    env.stubs[5] = {F(0x6709F0, &FkBody<0x6709F0>), Del()};
    env.stubs[6] = {F(0x66B960, &FkBody<0x66B960>), Del()};
    env.stubs[7] = {F(0x66BC00, &FkBody<0x66BC00>), Del()};
    env.stubs[8] = {F(0x66C4C0, &FkBody<0x66C4C0>), Del()};
    env.stubs[9] = {F(0x66C7A0, &FkBody<0x66C7A0>), Del()};
    env.stubs[10] = {F(0x61A5A0, &FkTaskNew), F(0x66CA40, &FkSeekCtor)};
    env.stubs[11] = {F(0x61A5A0, &FkTaskNew), F(0x630F20, &FkTired), F(0x62F310, &FkStand), F(0x632BD0, &FkSeqCtor), F(0x632D10, &FkSeqAdd), F(0x66B890, &FkTurn), F(0x63B8C0, &FkLeave), F(0x5FF940, &FkCarDrive),
                     F(0x66EA30, &FkFollowNode), F(0x668120, &FkGotoStandVec)};
    env.stubs[12] = {F(0x5F7F40, &FkSame), F(0x5E7EC0, &FkDettach), F(0x40FE00, &FkDifference), F(0x59C910, &FkNormalise), F(0x616920, &FkFindZ), F(0x59B920, &FkInvert), F(0x59C6D0, &FkFromMul), F(0x53CBE0, &FkAtan),
                     F(0x53CB50, &FkLimit), F(0x5E7CB0, &FkAttach), F(0x59ACD0, &FkMatDtor), F(0x61A5A0, &FkTaskNew), F(0x62F310, &FkStand), F(0x630F20, &FkTired), F(0x66EA30, &FkFollowNode),
                     F(0x671750, &FkSetTarget), F(0x46FE60, &FkGoToPoint), F(0x5DF8F0, &FkIsPlayer), F(0x56E210, &FkFindPlayerPed), F(0x5F6A10, &FkIsMember), F(0x5EFFE0, &FkSay), Rt(11, &FkSeekCreateSub), Rt(13, &FkSeekGetPos)};
    env.stubs[15] = {F(0x56E0D0, &FkFindPlayerVehicle)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 13u, 14u, 16u}) dllReal[f] = "toda la funcion (sin colaboradores; las llamadas virtuales usan una vtable falsa en la arena D)";
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
    // DebugCode (15): tabla de modelos, jugador en foco, vehiculo remoto de los cuatro jugadores y las 8 entradas de handling usadas
    regions.emplace_back(kModelTable + 4u * u32(kModelLo), 4u * kModelSlots); regionsOf[15].push_back(regions.size() - 1);
    regions.emplace_back(kPlayerInFocus, 1); regionsOf[15].push_back(regions.size() - 1);
    for (u32 k = 0; k < kPlayerCount; ++k) { regions.emplace_back(kPlayers + kPlayerStride * k + kRemoteAt, 4); regionsOf[15].push_back(regions.size() - 1); }
    for (u32 e = 0; e < 8; ++e) { regions.emplace_back(kHandling + kHandlingStride * kHandlingIds[e], kHandlingStride); regionsOf[15].push_back(regions.size() - 1); }
    // ControlSubTask (12): reloj y contador de fotogramas
    regions.emplace_back(kTimeMs, 4); regionsOf[12].push_back(regions.size() - 1);
    regions.emplace_back(kFrame, 4); regionsOf[12].push_back(regions.size() - 1);
    Run(env, ready, registered, bodies, collaborators);
}
}
