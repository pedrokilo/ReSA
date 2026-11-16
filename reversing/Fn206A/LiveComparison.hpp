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

// Banco par a par Fn206A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 30 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Todas son hooks S (RH_ScopedNamedGlobalInstall / RH_ScopedInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40 y ventana de 0x2000; las vtables falsas viven en la mitad alta de D).
//
// Colaboradores: falso identico en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / F) para los que tienen hook y que la DLL llama por su funcion: IsRegisteredWithPed 0x5EB4C0, DeRegisterPed 0x5EC5B0, las cinco
// copias de std::uninitialized_copy 0x5EAE10 / 0x5EAE40 / 0x5EAE70 / 0x5EAEA0 / 0x5EAEE0, el cuerpo del destructor de CPedAttractor 0x5EC410, CLocalisation::KickingWhenDown 0x56D270, RpAnimBlendClumpGetFirstAssociation 0x4D6A70,
// CAnimManager::BlendAnimation 0x4D4610, SetCurrentTime 0x4CEA80, SetFinishCallback 0x4CEBE0, GetAnimHierarchyFromSkinClump 0x734A40, RpAnimBlendClumpGetAssociation 0x4D68B0, CPed::IsPlayer 0x5DF8F0, ClearLookFlag 0x5E1950,
// ClearAimFlag 0x5DEF20, RemoveWeaponModel 0x5E3990, SetCurrentWeapon 0x5E6280, CTask::operator new 0x61A5A0, los constructores 0x61A900 (CTaskSimpleRunAnim) y 0x48E750 (CTaskSimplePause) y StartAnim de las dos tareas de golpe
// (0x61FD20, 0x61FF40) y PutAway (0x6348F0) cuando los llama otra funcion del lote.
// Falso solo en la direccion del EXE (la DLL los llama por esa direccion): operator delete 0x8214BD del CRT y las rutinas de RenderWare RpHAnimIDGetIndex 0x7C51A0, RpHAnimHierarchyGetMatrixArray 0x7C5120 y RwV3dTransformPoints
// 0x7EDD90; rand() del CRT 0x821B1E (en la DLL, CGeneral::GetRandomNumber lleva el mismo falso).
// Las llamadas virtuales (subtareas, atractores y eventos) van a vtables falsas de la arena D (la misma memoria para EXE y DLL).
// Colaboradores REALES (hook apagado, sin falso): los constructores base del EXE (0x61A390, 0x61A3B0) y el cuerpo del destructor base 0x61A3D0, y GetRandomNumberInRange<int> 0x407180 (el EXE corre su codigo con el rand falso).
// Las vtables que escriben los constructores/destructores se comparan como clase (EXE o vtable de la DLL: la vtable de C++ de la DLL es distinta de la del EXE por diseno; ver README.md).
// Memoria global del juego tocada (restaurada tras cada lado): marca del banco 0xA9B0B0, reloj 0xB7CB84, contador de fotogramas 0xB7CB4C y puntero del pool de atractores 0xB744C4.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F206ALive {
using namespace F206AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F206AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { const u16 w = u16(v); memcpy(m.data() + kGuard + off, &w, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); }
    u32 Get32(size_t off) const { u32 v{}; memcpy(&v, m.data() + kGuard + off, 4); return v; }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]; `skip` = desplazamientos (respecto a la ventana) de dwords de vtable, cuyos 4 bytes se omiten
    json Changes(const std::vector<u32>& skip = {}) const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i) {
            if (m[i] == r[i]) continue;
            const int64 rel = int64(i) - int64(kGuard);
            bool masked = false;
            for (const u32 s : skip) masked |= rel >= int64(s) && rel < int64(s) + 4;
            if (!masked) out.push_back(json::array({rel, m[i]}));
        }
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kWin>& ArenaOf(unsigned i) { return i == 0 ? arenaA : (i == 1 ? arenaB : (i == 2 ? arenaC : arenaD)); }

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de copia
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de CTask::operator new (0 = sin memoria)
inline u32 gIsReg{}, gIsRegGarb{}, gIsRegIdx{};          // IsRegisteredWithPed: bits por llamada, basura de EAX y numero de llamada
inline std::array<u32, 2> gKick{};  inline u32 gKickIdx{};   // KickingWhenDown: valor de cada llamada
inline u32 gFirstAssoc{}, gBlendRet{}, gHier{}, gBoneIdx{}, gMatArr{}, gAssocRet{};
inline u32 gIsP{}, gIsPIdx{};         // IsPlayer: un byte por llamada
inline std::array<u32, 6> gRnd{};  inline u32 gRndIdx{};   // rand(): valor de cada llamada
inline std::array<u32, 2> gSubTypes{};  inline u32 gSubTypeIdx{};
inline u32 gSubAbort{}, gEvType{}, gCfst{};
inline u32 gModFlags{}, gListPtr{}, gListEndNew{};
inline std::array<u32, 4> gModQ{};
inline DWORD gBenchThread{};
inline constexpr u32 kEvVDtor = 0x7E1EA5E0, kEvSubType = 0x7E1EA5E1, kEvSubAbort = 0x7E1EA5E2, kEvEvType = 0x7E1EA5E3, kEvCfst = 0x7E1EA5E4, kEvVBcast = 0x7E1EA5E5, kEvTrap = 0x7E1EA5FF;

// operator new de CTask (0x61A5A0; cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return gNew;
}
// operator delete escalar del CRT (0x8214BD) (cdecl, un puntero que puede no ser memoria valida): evento [direccion, puntero (etiqueta)]. No libera nada ni lo lee.
template<u32 ADDR> inline void __cdecl FkDeletePtr(void* p) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(p);
}
// Cuerpo sin argumentos de pila (thiscall): evento [direccion, ECX (etiqueta), dword que tenia [ECX]]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Funcion thiscall sin argumentos que no toca memoria (ClearLookFlag, ClearAimFlag): evento [direccion, ECX (etiqueta)].
template<u32 ADDR> inline void __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
}
// Funcion thiscall con un puntero en la pila (`ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No toca memoria.
template<u32 ADDR> inline void __fastcall FkThisPtr(void* self, void*, void* arg) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(arg);
}
// Funcion thiscall con un dword crudo en la pila (`ret 4`): evento [direccion, ECX (etiqueta), argumento (crudo)].
template<u32 ADDR> inline void __fastcall FkThisRaw(void* self, void*, u32 arg) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = arg;
}
// Constructor con un argumento de pila (thiscall, `ret 4`): evento [direccion, ECX (etiqueta), argumento (crudo)]; devuelve this.
template<u32 ADDR> inline u32 __fastcall FkCtor1(void* self, void*, u32 a) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = a;
    return u32(uintptr_t(self));
}
// Constructor con seis dwords de pila (thiscall, `ret 18h`): evento [direccion, ECX (etiqueta), a..f (crudos)]; devuelve this.
template<u32 ADDR> inline u32 __fastcall FkCtor6(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 e2, u32 f) {
    Ev& e = PushEv(ADDR, 7);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = e2; e.v[6] = f;
    return u32(uintptr_t(self));
}
// std::uninitialized_copy (cdecl, tres punteros): evento [direccion, origen, fin, destino (etiquetas)]; devuelve gRet.
template<u32 ADDR> inline u32 __cdecl FkUCopy(void* first, void* last, void* dest) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(first); e.v[1] = Tag(last); e.v[2] = Tag(dest);
    return gRet;
}
// Modificacion que dejan los falsos de BroadcastDeparture / DeRegisterPed: colas del atractor (+10h/+14h, +20h/+24h) y fin de la lista
inline void ApplyMod(void* self) {
    if (gModFlags & 1) {
        auto* p = static_cast<u32*>(self);
        p[0x10 / 4] = gModQ[0]; p[0x14 / 4] = gModQ[1]; p[0x20 / 4] = gModQ[2]; p[0x24 / 4] = gModQ[3];
    }
    if (gModFlags & 2) memcpy(reinterpret_cast<u8*>(gListPtr) + 8, &gListEndNew, 4);
}
// Vtable falsa. Ranura 1: destructor con borrado (thiscall, banderas en la pila, `ret 4`): evento [kEvVDtor, ECX (etiqueta), banderas]
inline u32 __fastcall FkVDtor(void* self, void*, u32 flags) { Ev& e = PushEv(kEvVDtor, 2); e.v[0] = Tag(self); e.v[1] = flags; return u32(uintptr_t(self)); }
// Ranura 4: GetTaskType (thiscall, sin argumentos): evento [kEvSubType, ECX (etiqueta)]; devuelve gSubTypes[llamada]
inline u32 __fastcall FkSubType(void* self, void*) { Ev& e = PushEv(kEvSubType, 1); e.v[0] = Tag(self); const u32 v = gSubTypes[gSubTypeIdx < 2 ? gSubTypeIdx : 1]; ++gSubTypeIdx; return v; }
// Ranura 5: BroadcastDeparture del atractor (thiscall, el peaton en la pila, `ret 4`): evento [kEvVBcast, ECX (etiqueta), peaton (etiqueta)]
inline u32 __fastcall FkVBcast(void* self, void*, void* ped) { Ev& e = PushEv(kEvVBcast, 2); e.v[0] = Tag(self); e.v[1] = Tag(ped); ApplyMod(self); return 0; }
// Ranura 6: MakeAbortable de la subtarea (thiscall, peaton, prioridad y evento en la pila, `ret 0Ch`): evento [kEvSubAbort, ECX (etiqueta), peaton (etiqueta), prioridad, evento (etiqueta)]; devuelve gSubAbort
inline u32 __fastcall FkSubAbort(void* self, void*, void* ped, u32 prio, void* ev) {
    Ev& e = PushEv(kEvSubAbort, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = prio; e.v[3] = Tag(ev);
    return gSubAbort;
}
// Ranura 9: CreateFirstSubTask (thiscall, el peaton en la pila, `ret 4`): evento [kEvCfst, ECX (etiqueta), peaton (etiqueta)]; devuelve gCfst
inline u32 __fastcall FkCfst(void* self, void*, void* ped) { Ev& e = PushEv(kEvCfst, 2); e.v[0] = Tag(self); e.v[1] = Tag(ped); return gCfst; }
// Ranura 1 de la vtable del evento: GetEventType (thiscall, sin argumentos): evento [kEvEvType, ECX (etiqueta)]; devuelve gEvType
inline u32 __fastcall FkEvType(void* self, void*) { Ev& e = PushEv(kEvEvType, 1); e.v[0] = Tag(self); return gEvType; }
// Cualquier otra ranura de las vtables falsas: evento [kEvTrap, ECX (etiqueta)] (una llamada a otra ranura seria una diferencia)
inline void __fastcall FkTrap(void* self, void*) { Ev& e = PushEv(kEvTrap, 1); e.v[0] = Tag(self); }
// CPedAttractor::DeRegisterPed (0x5EC5B0; thiscall, el peaton en la pila, `ret 4`): evento [direccion, ECX (etiqueta), peaton (etiqueta)]
inline void __fastcall FkDeReg(void* self, void*, void* ped) { Ev& e = PushEv(0x5EC5B0, 2); e.v[0] = Tag(self); e.v[1] = Tag(ped); ApplyMod(self); }
// CPedAttractor::IsRegisteredWithPed (0x5EB4C0; thiscall, el peaton en la pila, `ret 4`): evento [direccion, ECX (etiqueta), peaton (etiqueta)]; devuelve el bit de la llamada (+ basura en los bytes altos)
inline u32 __fastcall FkIsReg(void* self, void*, void* ped) {
    Ev& e = PushEv(0x5EB4C0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ped);
    const u32 bit = gIsRegIdx < 32 ? (gIsReg >> gIsRegIdx) & 1u : 0u;
    ++gIsRegIdx;
    return bit | gIsRegGarb;
}
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, dos argumentos, `ret 8`): evento [direccion, ECX (etiqueta), funcion (cruda), datos (etiqueta), dword de +0x1C de la animacion (0 si ECX es nulo)]. No escribe nada.
inline void __fastcall FkSfc(void* self, void*, u32 cb, void* data) {
    Ev& e = PushEv(0x4CEBE0, 4);
    e.v[0] = Tag(self); e.v[1] = cb; e.v[2] = Tag(data);
    e.v[3] = self ? BitsOf(static_cast<const u8*>(self) + 0x1C) : 0u;
}
// CLocalisation::KickingWhenDown (0x56D270; cdecl, sin argumentos; AL): evento [direccion]; devuelve el valor de la llamada
inline u32 __cdecl FkKick() { PushEv(0x56D270, 0); const u32 v = gKick[gKickIdx < 2 ? gKickIdx : 1]; ++gKickIdx; return v; }
// RpAnimBlendClumpGetFirstAssociation (0x4D6A70; cdecl: clump, banderas): evento [direccion, clump (etiqueta), banderas]; devuelve gFirstAssoc
inline u32 __cdecl FkFirstAssoc(void* clump, u32 flags) { Ev& e = PushEv(0x4D6A70, 2); e.v[0] = Tag(clump); e.v[1] = flags; return gFirstAssoc; }
// CAnimManager::BlendAnimation (0x4D4610; cdecl: clump, grupo, animacion, float como dword): evento [direccion, clump (etiqueta), grupo, animacion, bits del float]; devuelve gBlendRet
inline u32 __cdecl FkBlend(void* clump, u32 group, u32 id, u32 bits) { Ev& e = PushEv(0x4D4610, 4); e.v[0] = Tag(clump); e.v[1] = group; e.v[2] = id; e.v[3] = bits; return gBlendRet; }
// RpAnimBlendClumpGetAssociation (0x4D68B0; cdecl: clump, identificador): evento [direccion, clump (etiqueta), identificador]; devuelve gAssocRet
inline u32 __cdecl FkAssoc(void* clump, u32 id) { Ev& e = PushEv(0x4D68B0, 2); e.v[0] = Tag(clump); e.v[1] = id; return gAssocRet; }
// CAnimBlendAssociation::SetCurrentTime (0x4CEA80; thiscall, un float en la pila, `ret 4`): evento [direccion, ECX (etiqueta), bits]
inline void __fastcall FkSetTime(void* self, void*, u32 bits) { Ev& e = PushEv(0x4CEA80, 2); e.v[0] = Tag(self); e.v[1] = bits; }
// GetAnimHierarchyFromSkinClump (0x734A40; cdecl, un argumento): evento [direccion, clump (etiqueta)]; devuelve gHier
inline u32 __cdecl FkAnimHier(void* clump) { Ev& e = PushEv(0x734A40, 1); e.v[0] = Tag(clump); return gHier; }
// RpHAnimIDGetIndex (0x7C51A0; cdecl, dos argumentos): evento [direccion, jerarquia (etiqueta), BoneTag (crudo)]; devuelve gBoneIdx
inline u32 __cdecl FkBoneIdx(void* hier, u32 boneTag) { Ev& e = PushEv(0x7C51A0, 2); e.v[0] = Tag(hier); e.v[1] = boneTag; return gBoneIdx; }
// RpHAnimHierarchyGetMatrixArray (0x7C5120; cdecl, un argumento): evento [direccion, jerarquia (etiqueta)]; devuelve gMatArr
inline u32 __cdecl FkMatArr(void* hier) { Ev& e = PushEv(0x7C5120, 1); e.v[0] = Tag(hier); return gMatArr; }
// RwV3dTransformPoints (0x7EDD90; cdecl: salida, entrada, numero, matriz): evento [direccion, salida (etiqueta), entrada (etiqueta), numero, matriz (etiqueta), los tres dwords de la entrada]; devuelve la salida
inline u32 __cdecl FkTransform(void* out, const u32* in, u32 count, void* matrix) {
    Ev& e = PushEv(0x7EDD90, 7);
    e.v[0] = Tag(out); e.v[1] = Tag(in); e.v[2] = count; e.v[3] = Tag(matrix); e.v[4] = in[0]; e.v[5] = in[1]; e.v[6] = in[2];
    return u32(uintptr_t(out));
}
// CPed::IsPlayer (0x5DF8F0; thiscall, sin argumentos, `ret`): evento [direccion, ECX (etiqueta)]; devuelve el byte de la llamada de gIsP
inline u32 __fastcall FkIsPlayer(void* self, void*) {
    Ev& e = PushEv(0x5DF8F0, 1);
    e.v[0] = Tag(self);
    const u32 v = gIsPIdx < 4 ? (gIsP >> (8 * gIsPIdx)) & 0xFFu : 0u;
    ++gIsPIdx;
    return v;
}
// rand() del CRT (0x821B1E en el EXE) / CGeneral::GetRandomNumber (DLL): evento [0x821B1E]; devuelve el valor de la llamada (los llamadores solo prueban los bits bajos o usan los 16 bits bajos)
inline u32 __cdecl FkRand() { PushEv(0x821B1E, 0); const u32 v = gRnd[gRndIdx < 6 ? gRndIdx : 5]; ++gRndIdx; return v; }

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
    std::array<u32, 12> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};
// Dwords de vtable que la funcion escribe (arena, desplazamiento, vtable del EXE): se omiten de los cambios y se anotan como clase ("vt" si es la del EXE o una de la DLL)
struct VptrSlot { unsigned arena; u32 off; u32 exeVt; };
inline std::vector<VptrSlot> vptrSlots;

// Vtables falsas en la arena D: la de objetos (ranura 1 destructor, 4 GetTaskType, 5 BroadcastDeparture, 6 MakeAbortable, 9 CreateFirstSubTask; el resto cae en FkTrap) y la de eventos (ranura 1 GetEventType)
inline void InitVtables() {
    for (u32 i = 0; i < 0x40; ++i) { arenaD.Put32(kVtObj + 4 * i, u32(uintptr_t(&FkTrap))); arenaD.Put32(kVtEvt + 4 * i, u32(uintptr_t(&FkTrap))); arenaD.Put32(kVtTask + 4 * i, u32(uintptr_t(&FkTrap))); }
    // atractores (vtable del EXE 0x86C538: GetType en la ranura 0, destructor con borrado en la 1, BroadcastDeparture en la 5)
    arenaD.Put32(kVtObj + 4 * 1, u32(uintptr_t(&FkVDtor)));
    arenaD.Put32(kVtObj + 4 * 5, u32(uintptr_t(&FkVBcast)));
    // tareas (vtable de CTask: destructor con borrado en la ranura 0, GetTaskType en la 4, MakeAbortable en la 6, CreateFirstSubTask en la 9)
    arenaD.Put32(kVtTask + 4 * 0, u32(uintptr_t(&FkVDtor)));
    arenaD.Put32(kVtTask + 4 * 4, u32(uintptr_t(&FkSubType)));
    arenaD.Put32(kVtTask + 4 * 6, u32(uintptr_t(&FkSubAbort)));
    arenaD.Put32(kVtTask + 4 * 9, u32(uintptr_t(&FkCfst)));
    arenaD.Put32(kVtEvt + 4 * 1, u32(uintptr_t(&FkEvType)));
}
inline u32 VtObj() { return u32(arenaD.Obj(kVtObj)); }
inline u32 VtEvt() { return u32(arenaD.Obj(kVtEvt)); }
inline u32 VtTask() { return u32(arenaD.Obj(kVtTask)); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[kSeed0]); arenaB.Pattern(a[2], a[kSeed0 + 1]); arenaC.Pattern(a[2], a[kSeed0 + 2]); arenaD.Pattern(a[2], a[kSeed0 + 3]);
    InitVtables();
    gRet = 0; gDtorWrite = 0; gNew = 0; gIsReg = 0; gIsRegGarb = 0; gIsRegIdx = 0; gKick = {}; gKickIdx = 0; gFirstAssoc = 0; gBlendRet = 0; gHier = 0; gBoneIdx = 0; gMatArr = 0; gAssocRet = 0;
    gIsP = 0; gIsPIdx = 0; gRnd = {}; gRndIdx = 0; gSubTypes = {}; gSubTypeIdx = 0; gSubAbort = 0; gEvType = 0; gCfst = 0; gModFlags = 0; gListPtr = 0; gListEndNew = 0; gModQ = {};
    vptrSlots.clear();
    *reinterpret_cast<u8*>(kBenchMarker) = 0x5A;   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    const auto stdcall_ = [&](u32 n) { call.argCount = n; call.popBytes = 0; };
    const auto putPed = [&](u32 po) { return u32(arenaC.Obj(po)); };
    switch (kKind[c.fn]) {
    case kFind:
    case kPedUsing:
    case kRelevant: {
        const u32 mode = a[4], n = a[5], bo = a[6];
        for (u32 s = 0; s < 8; ++s) {
            arenaC.Put32(kSlot * s + 4, a[16 + s]); arenaC.Put32(kSlot * s + 8, a[24 + s]);
            const u32 qm = a[40 + s], P = u32(arenaD.Obj(0x20 * s));
            u32 qb{}, qe{};
            switch (qm) { case 0: qb = 0; qe = a[56 + s]; break; case 1: qb = P; qe = P; break; case 2: qb = P; qe = P + 4; break; case 3: qb = P; qe = P - 4; break; default: qb = P; qe = P + 8; break; }
            arenaC.Put32(kSlot * s + 0x20, qb); arenaC.Put32(kSlot * s + 0x24, qe);
            if (qm) arenaD.Put32(0x20 * s, a[48 + s]);
        }
        const u32 begin = mode == 0 ? 0u : u32(arenaB.Obj(bo));
        u32 end{};
        switch (mode) { case 0: end = a[12]; break; case 1: end = begin; break; case 2: end = begin + 4 * n; break; case 3: end = begin - a[13]; break; default: end = 0; break; }
        if (mode) for (u32 i = 0; i < n; ++i) arenaB.Put32(bo + 4 * i, u32(arenaC.Obj(kSlot * (a[32 + i] % 8))));
        arenaA.Put32(o + 4, begin); arenaA.Put32(o + 8, end);
        gIsReg = a[10]; gIsRegGarb = a[11];
        if (kKind[c.fn] == kRelevant) { call.args[0] = a[9]; call.args[1] = a[7]; call.args[2] = a[8]; call.args[3] = thisA; stdcall_(4); }
        else { call.args[0] = a[7]; call.args[1] = a[8]; call.args[2] = thisA; stdcall_(3); }
        break;
    }
    case kBcast:
    case kDereg: {
        const u32 mode = a[4], n = a[5], bo = a[6];
        for (u32 s = 0; s < 6; ++s) {
            const u32 base = kSlot * s;
            arenaC.Put32(base, VtObj());
            const u32 codes[2]{a[16 + s], a[24 + s]}, begins[2]{a[32 + s], a[40 + s]}, ends[2]{a[48 + s], a[56 + s]}, offs[2]{0x10, 0x20};
            for (int q = 0; q < 2; ++q) {
                u32 qb{}, qe{};
                switch (codes[q]) { case 0: qb = 0; qe = ends[q]; break; case 1: qb = begins[q]; qe = begins[q]; break; case 2: qb = begins[q]; qe = begins[q] + 4; break; case 3: qb = begins[q]; qe = begins[q] + 8; break; default: qb = begins[q]; qe = begins[q] - 4; break; }
                arenaC.Put32(base + offs[q], qb); arenaC.Put32(base + offs[q] + 4, qe);
            }
        }
        const u32 begin = mode == 0 ? 0u : u32(arenaB.Obj(bo));
        const u32 end = mode == 0 ? a[14] : (mode == 1 ? begin : begin + 4 * n);
        if (mode) for (u32 i = 0; i < n; ++i) arenaB.Put32(bo + 4 * i, u32(arenaC.Obj(kSlot * i)));
        arenaA.Put32(o + 4, begin); arenaA.Put32(o + 8, end);
        const u32 j = mode >= 2 ? a[9] % n : a[9] % 6;
        const u32 att = a[8] == 0 ? 0u : (a[8] == 2 ? a[10] : u32(arenaC.Obj(kSlot * j)));
        gListPtr = thisA; gModFlags = a[11];
        gModQ = {a[12], a[12] + 4 * a[13], a[14], a[14] + 4 * a[15]};
        gListEndNew = begin + 4 * a[22];
        call.args[0] = a[7]; call.args[1] = att; call.args[2] = thisA; stdcall_(3);
        break;
    }
    case kUcopy: {
        for (u32 i = 0; i < 3; ++i) {
            const u32 m = a[4 + 2 * i], v = a[5 + 2 * i];
            call.args[i] = m == 0 ? v : (m == 1 ? u32(arenaA.Obj(v)) : (m == 2 ? u32(arenaB.Obj(v)) : 0u));
        }
        gRet = a[10] == 0 ? a[11] : (a[10] == 1 ? u32(arenaD.Obj(a[11])) : 0u);
        stdcall_(3);
        break;
    }
    case kTidy: {
        const u32 begin = a[4] == 0 ? 0u : (a[4] == 1 ? a[5] : u32(arenaB.Obj(a[5])));
        arenaA.Put32(o + 4, begin); arenaA.Put32(o + 8, a[6]); arenaA.Put32(o + 0xC, a[7]);
        break;
    }
    case kAdtor: {
        gRet = a[5]; gDtorWrite = a[6];
        const i32 idx = i32(a[9]);
        const int64 diff = int64(idx) * kAttractorSize + a[10];
        const u32 pool = a[7];
        arenaB.Put32(pool + 0, u32(int64(thisA) - diff)); arenaB.Put32(pool + 4, u32(arenaD.Obj(a[8]))); arenaB.Put32(pool + 8, 0x40); arenaB.Put32(pool + 0xC, a[11]);
        arenaD.m[kGuard + a[8] + u32(idx)] &= 0x7Fu;   // el hueco esta usado
        *reinterpret_cast<u32*>(kAttractorPool) = u32(arenaB.Obj(pool));
        call.args[0] = a[4]; stdcall_(1);
        break;
    }
    case kBkCtor:
        vptrSlots.push_back({0, o, 0x86D7D8});
        break;
    case kTaskType:
        break;
    case kMakeAbort: {
        const u32 ao = c.fn == 14 ? 0xC : 0x14;
        arenaA.Put32(o + ao, a[4] ? u32(arenaB.Obj(a[5])) : 0u);
        if (a[4]) { arenaB.Put32(a[5] + 0x1C, a[9]); arenaB.Put8(a[5] + 0x2E, a[10]); }
        call.args[0] = a[7]; call.args[1] = a[6]; call.args[2] = a[8]; stdcall_(3);
        break;
    }
    case kBkStart: {
        const u32 po = a[12], ao = a[6];
        gKick = {a[4], a[16]}; gFirstAssoc = a[5]; gBlendRet = u32(arenaB.Obj(ao)); gHier = a[9]; gBoneIdx = a[10]; gMatArr = a[11];
        arenaA.Put32(o + 0xC, a[14]);
        arenaC.Put32(po + 0x18, a[7] == 0xFFFFFFFFu ? 0u : u32(arenaD.Obj(a[7])));
        if (a[7] != 0xFFFFFFFFu) arenaD.Put8(a[7], 2);   // RwObject de tipo rpCLUMP (CEntity::GetRpClump de la DLL comprueba el tipo)
        arenaC.Put32(po + 0x490, u32(arenaD.Obj(a[13])));
        arenaD.Put32(a[13] + 0x14, a[8]);
        arenaB.Put32(ao + 0x1C, a[15]);
        call.args[0] = putPed(po); stdcall_(1);
        break;
    }
    case kProcPed: {
        const u32 ao = c.fn == 16 ? 0xC : 0x14;
        arenaA.Put8(o + 8, a[4]); arenaA.Put32(o + ao, a[5]);
        call.args[0] = a[6]; stdcall_(1);
        break;
    }
    case kBkClone:
    case kBhClone:
        if (kKind[c.fn] == kBhClone) { arenaA.Put32(o + 0xC, a[6]); arenaA.Put32(o + 0x10, a[7]); }
        gNew = a[4] ? u32(arenaB.Obj(a[5])) : 0u;
        if (a[4]) vptrSlots.push_back({1, a[5], kKind[c.fn] == kBkClone ? 0x86D7D8u : 0x86D7FCu});
        break;
    case kBhCtor:
        vptrSlots.push_back({0, o, 0x86D7FC});
        call.args[0] = a[4]; call.args[1] = a[5]; stdcall_(2);
        break;
    case kBhStart: {
        const u32 po = a[28], fo = a[20], no = a[26], ho = a[24];
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x10, a[5]);
        arenaA.Put32(o + 0x14, a[6] ? u32(arenaB.Obj(no)) : 0u);
        arenaC.Put32(po + 0x18, a[7] == 0xFFFFFFFFu ? 0u : u32(arenaD.Obj(a[7]))); if (a[7] != 0xFFFFFFFFu) arenaD.Put8(a[7], 2);   // RwObject de tipo rpCLUMP
        arenaC.Put16(po + 0x20, a[8]); arenaC.Put32(po + 0xFC, a[9]); arenaC.Put32(po + 0x744, a[29]);
        arenaB.Put32(fo + 0x1C, a[21]); arenaB.Put32(fo + 0x20, a[22]); arenaB.Put16(fo + 0x2C, a[23]); arenaB.Put32(fo + 0x14, u32(arenaD.Obj(ho)));
        arenaD.Put32(ho + 0x10, a[25]);
        arenaB.Put32(no + 0x1C, a[27]);
        *reinterpret_cast<u32*>(kClock) = a[10]; *reinterpret_cast<u32*>(kFrameCounter) = a[11];
        gIsP = a[12];
        for (u32 i = 0; i < 6; ++i) gRnd[i] = a[13 + i];
        gAssocRet = a[19] ? u32(arenaB.Obj(fo)) : 0u;
        gBlendRet = u32(arenaB.Obj(no));
        call.args[0] = putPed(po); stdcall_(1);
        break;
    }
    case kMpCtor:
        vptrSlots.push_back({0, o, 0x86E454});
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kMpDtorBody: {
        const u32 sub = a[4] ? u32(arenaB.Obj(a[5])) : 0u;
        if (a[4]) arenaB.Put32(a[5], VtTask());
        arenaA.Put32(o + 8, sub);
        vptrSlots.push_back({0, o, 0x86D48C});
        break;
    }
    case kPutAway: {
        const u32 po = a[4];
        arenaC.Put32(po + 0x70C, a[5]);
        arenaC.Put32(po + 0x480, a[6] ? u32(arenaD.Obj(a[7])) : 0u);
        if (a[6]) arenaD.Put8(a[7] + 0x85, a[8]);
        call.args[0] = putPed(po); stdcall_(1);
        break;
    }
    case kMpAbort: {
        const u32 so = a[4], po = a[10];
        arenaB.Put32(so, VtTask());
        arenaA.Put32(o + 8, u32(arenaB.Obj(so))); arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[22]); arenaA.Put32(o + 0x14, a[23]); arenaA.Put8(o + 0x18, a[21]); arenaA.Put8(o + 0x1C, a[20]);
        arenaC.Put32(po + 0x474, a[11]); arenaC.Put32(po + 0x478, a[12]); arenaC.Put32(po + 0x70C, a[13]);
        u32 ev = 0;
        if (a[8]) { ev = u32(arenaD.Obj(a[9])); arenaD.Put32(a[9], VtEvt()); }
        *reinterpret_cast<u32*>(kClock) = a[14];
        gSubTypes = {a[15], a[16]}; gSubAbort = a[17]; gEvType = a[18]; gIsP = a[19];
        call.args[0] = putPed(po); call.args[1] = a[6]; call.args[2] = ev; stdcall_(3);
        break;
    }
    case kMpNext: {
        const u32 so = a[4], po = a[7];
        arenaA.Put32(o, VtTask());
        arenaB.Put32(so, VtTask());
        arenaA.Put32(o + 8, u32(arenaB.Obj(so))); arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[16]); arenaA.Put32(o + 0x14, a[17]); arenaA.Put8(o + 0x1D, a[9]); arenaA.Put8(o + 0x1C, a[10]); arenaA.Put8(o + 0x18, a[11]);
        arenaC.Put32(po + 0x474, a[18]); arenaC.Put32(po + 0x478, a[19]);
        *reinterpret_cast<u32*>(kClock) = a[12];
        gNew = a[13] ? u32(arenaD.Obj(a[14])) : 0u;
        gSubTypes = {a[8], a[8]}; gCfst = a[15];
        call.args[0] = putPed(po); stdcall_(1);
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
        gFloatRet = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        json obs;
        std::array<std::vector<u32>, 4> skip;
        json vptrs = json::array();
        for (const auto& v : vptrSlots) {
            skip[v.arena].push_back(v.off);
            u32 val{}; memcpy(&val, ArenaOf(v.arena).m.data() + kGuard + v.off, 4);
            const bool ok = val == v.exeVt || ModuleOf(val) == env.asi;
            vptrs.push_back(json::array({v.arena, v.off, ok ? json("vt") : json(val)}));
        }
        obs["changes"] = {{"A", arenaA.Changes(skip[0])}, {"B", arenaB.Changes(skip[1])}, {"C", arenaC.Changes(skip[2])}, {"D", arenaD.Changes(skip[3])}};
        obs["vptrs"] = vptrs;
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kBcast: case kDereg: case kMakeAbort: case kProcPed: case kMpAbort: obs["ret"] = eax & 0xFFu; break;                                    // bool: solo AL
        case kTaskType: obs["ret"] = eax; break;                                                                                                      // valor crudo
        case kFind: case kPedUsing: case kRelevant: case kUcopy: case kAdtor: case kBkCtor: case kBkClone: case kBhCtor: case kBhClone: case kMpCtor: case kMpNext:
            obs["ret"] = Tag(uintptr_t(eax)); break;                                                                                                  // puntero como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                                                                         // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN206A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN206A_CASES", buffer, sizeof(buffer)) > 0) {
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
        unsigned firstFn = 0;   // solo para depurar: FN206A_FIRST = primera funcion que se ejecuta (la corrida de aprobacion las ejecuta todas)
        unsigned lastFn = kFunctions;
        {char b[16]{}; if (GetEnvironmentVariableA("FN206A_FIRST", b, sizeof(b)) > 0) firstFn = unsigned(std::atoi(b));}
        {char b[16]{}; if (GetEnvironmentVariableA("FN206A_LAST", b, sizeof(b)) > 0) lastFn = unsigned(std::atoi(b)) + 1;}
        for (unsigned f = firstFn; f < lastFn && f < kFunctions; ++f) {
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
    if (!std::filesystem::exists(dir / "fn206a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn206a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn206a"); }
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
        if (!(found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi)) Write({{"event", "not_ready_function"}, {"function", kFn[f]}, {"found", found}, {"body", found ? env.route[f]->Body() : 0}, {"body_module", found ? ModuleOf(env.route[f]->Body()) : 0}});
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
    // Colaborador REAL que debe tener hook RH (hook apagado): corre el codigo del EXE
    auto R = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        const Pair p = it->second;
        if (!p.hooked || !p.dll) ready = false;
        return p;
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (CRT, RW): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH cuya llamada, en la DLL, va a una funcion propia (direccion dada) y no a la del EXE: el falso va en la direccion del EXE y en esa funcion (rand: CGeneral::GetRandomNumber)
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    env.stubs[2] = {F(0x5EB4C0, &FkIsReg)};
    env.stubs[4] = {F(0x5EC5B0, &FkDeReg)};
    env.stubs[5] = {F(0x5EAE10, &FkUCopy<0x5EAE10>)};
    env.stubs[6] = {F(0x5EAE40, &FkUCopy<0x5EAE40>)};
    env.stubs[7] = {F(0x5EAE70, &FkUCopy<0x5EAE70>)};
    env.stubs[8] = {F(0x5EAEA0, &FkUCopy<0x5EAEA0>)};
    env.stubs[9] = {F(0x5EAEE0, &FkUCopy<0x5EAEE0>)};
    env.stubs[10] = {E(0x8214BD, &FkDeletePtr<0x8214BD>)};
    env.stubs[11] = {F(0x5EC410, &FkBody<0x5EC410>)};
    env.reals[12] = {R(0x61A390)};
    env.stubs[14] = {F(0x4CEBE0, &FkSfc)};
    env.stubs[15] = {F(0x56D270, &FkKick), F(0x4D6A70, &FkFirstAssoc), F(0x4D4610, &FkBlend), F(0x4CEA80, &FkSetTime), F(0x734A40, &FkAnimHier), E(0x7C51A0, &FkBoneIdx), E(0x7C5120, &FkMatArr),
                     E(0x7EDD90, &FkTransform), F(0x4CEBE0, &FkSfc)};
    env.stubs[16] = {F(0x61FD20, &FkThisPtr<0x61FD20>)};
    env.stubs[17] = {F(0x61A5A0, &FkNew)};
    env.reals[17] = {R(0x61A390)};
    env.reals[18] = {R(0x61A390)};
    env.stubs[20] = {F(0x4CEBE0, &FkSfc)};
    env.stubs[21] = {F(0x5DF8F0, &FkIsPlayer), D(0x821B1E, std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), &FkRand), F(0x4D68B0, &FkAssoc), F(0x4D4610, &FkBlend), F(0x4CEA80, &FkSetTime),
                     F(0x5E1950, &FkThis<0x5E1950>), F(0x5DEF20, &FkThis<0x5DEF20>), F(0x4CEBE0, &FkSfc)};
    env.reals[21] = {R(0x407180)};   // GetRandomNumberInRange<int>: hook de la DLL apagado, el EXE corre su codigo real con el rand falso (la DLL convierte en linea con GetRandomIntInRange)
    env.stubs[22] = {F(0x61FF40, &FkThisPtr<0x61FF40>)};
    env.stubs[23] = {F(0x61A5A0, &FkNew)};
    env.reals[23] = {R(0x61A390)};
    env.reals[24] = {R(0x61A3B0)};
    env.reals[26] = {R(0x61A3D0)};
    env.stubs[27] = {F(0x5E3990, &FkThisRaw<0x5E3990>), F(0x5E6280, &FkThisRaw<0x5E6280>)};
    env.stubs[28] = {F(0x5DF8F0, &FkIsPlayer), F(0x6348F0, &FkThisPtr<0x6348F0>)};
    env.stubs[29] = {F(0x61A5A0, &FkNew), F(0x61A900, &FkCtor6<0x61A900>), F(0x48E750, &FkCtor1<0x48E750>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en el cuerpo de la DLL, o solo en la direccion del EXE si la DLL la llama por ella); las llamadas virtuales van a las vtables falsas de la arena D";
    for (unsigned f : {0u, 1u, 3u, 13u, 19u, 25u}) dllReal[f] = "toda la funcion (sin colaboradores; las llamadas virtuales van a las vtables falsas de la arena D)";
    for (unsigned f : {12u, 17u, 18u, 23u, 24u, 26u}) dllReal[f] = "toda la funcion; los constructores y destructores base del EXE (0x61A390, 0x61A3B0, 0x61A3D0) son colaboradores reales (hook apagado) y en la DLL son los de C++ de las clases base";
    dllReal[11] = "toda la funcion; el cuerpo del destructor 0x5EC410 es falso y el borrado del pool de atractores (EXE: en linea; DLL: CPedAttractor::operator delete) corre real sobre el pool sintetico";
    dllReal[21] = "toda la funcion; rand() (0x821B1E) y los colaboradores con hook son falsos; GetRandomNumberInRange<int> 0x407180 corre real en el EXE (rand falso) y en la DLL es la conversion en linea";
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
    // Memoria global del juego tocada: reloj (BeHit StartAnim, MakeAbortable y CreateNextSubTask de CTaskComplexUseMobilePhone), contador de fotogramas (BeHit StartAnim), puntero del pool de atractores; se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kClock, 4); regionsOf[21] = regionsOf[28] = regionsOf[29] = {regions.size() - 1};
    regions.emplace_back(kFrameCounter, 4); regionsOf[21].push_back(regions.size() - 1);
    regions.emplace_back(kAttractorPool, 4); regionsOf[11] = {regions.size() - 1};
    regions.emplace_back(kBenchMarker, 1);
    for (unsigned f = 0; f < kFunctions; ++f) regionsOf[f].push_back(regions.size() - 1);
    Run(env, ready, registered, bodies, collaborators);
}
}
