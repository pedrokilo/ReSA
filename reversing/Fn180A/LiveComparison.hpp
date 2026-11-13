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

// Banco par a par Fn180A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 22 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): metodos de CAEAudioHardware (0x4D94D0,
// 0x4D8870, 0x4D9500, 0x4D8F30, 0x4D8960), Release de COM, constructor por defecto de CObject 0x5A1D10 (sin hook: solo en la direccion del EXE), 0x734A40, 0x532B20, 0x7C51A0 (RenderWare, sin hook: solo en la direccion del
// EXE), SetModelIndex 0x532AE0 y SetIsStatic 0x5A0760 (hooks virtuales), 0x4D6720, 0x734E50, 0x734940, cuerpo de CObject 0x59F660 y CObject::operator delete 0x5A1F20 (funciones propias de la DLL sin hook: el falso va en la
// direccion del EXE y en esa funcion), cuerpos de destructor de tareas (0x648FC0, 0x649100, 0x649280, 0x61A6F0, 0x653090, 0x6531D0, 0x61A3D0), CTask::operator new 0x61A5A0 y operator delete 0x61A5B0, CPointRoute::operator delete
// 0x41B7D0, constructores de tareas (0x61A390, 0x61A900, 0x62F310, 0x667CD0, 0x667E20, 0x668120), ComputeTargetPos 0x649160 y CVehicleAnimGroup::GetGroup 0x6E3B00. Memoria global del juego tocada (restaurada tras cada lado): la
// tabla de model infos 0xA9B0C8, el puntero del pool de TXD 0xC8800C, los bytes de pausa 0xB7CB48/49 y 0xBA6831, el contador de canales 0xB5F898 y los cuatro WORD de la grua desde 0x8CD6FC.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los eventos de los falsos (con sus argumentos), el valor devuelto (punteros como
// etiqueta de arena; EAX completo en constantes; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue.

// Funciones propias de la DLL sin hook (Object.cpp): cuerpo del destructor de CObject (0x59F660)
namespace CObjectBody { void __fastcall Destructor(CObject* self, void* edx); }

namespace F180ALive {
using namespace F180AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F180AFix::Arena(WIN);
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
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de operator new (0 = sin memoria)
inline std::array<u32, 3> gVec{};    // lo que escribe el falso de ComputeTargetPos en +0xC..+0x14
inline u32 gHier{}, gBone{}, gTex{}, gTex1{}, gClumpNew{}, gXorModel{}, gXorModel34{}, gXorStatic{};   // CHandObject::Constructor
inline u32 gSelf{};                  // canal de audio cuyo destructor libera los COM falsos
inline u32 gRelMode{}, gRelObj3{}, gRelCalls{};   // el primer Release modifica +0xC del canal
inline u32 gMutObj{}, gMutOn{}, gMutChan{}, gMutFactor{}, gMutCalls{};   // el primer SetChannelFrequencyScalingFactor modifica el objeto
inline DWORD gBenchThread{};
inline constexpr u32 kEvRelease = 0xC0DE;

// Metodo del hardware o de otro objeto con n dwords de pila (thiscall, el callee limpia): evento [direccion del EXE, ECX (etiqueta), argumentos crudos]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkT0(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkT3(void* self, void*, u32 a1, u32 a2, u32 a3) {
    Ev& e = PushEv(ADDR, 4);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3;
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkT4(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4) {
    Ev& e = PushEv(ADDR, 5);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4;
    return gRet;
}
// SetChannelFrequencyScalingFactor (0x4D8960; tres dwords de pila, `ret 0Ch`): evento [direccion, ECX, canal, 0, factor]; la PRIMERA llamada modifica el canal (+4) y el factor (+0x1C) del objeto de la prueba (si gMutOn)
inline u32 __fastcall FkScale(void* self, void*, u32 a1, u32 a2, u32 a3) {
    Ev& e = PushEv(0x4D8960, 4);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3;
    if (gMutCalls++ == 0 && gMutOn) {
        memcpy(reinterpret_cast<void*>(uintptr_t(gMutObj) + 4), &gMutChan, 4);
        memcpy(reinterpret_cast<void*>(uintptr_t(gMutObj) + 0x1C), &gMutFactor, 4);
    }
    return gRet;
}
// SetChannelVolume (0x4D8870; cuatro dwords de pila, `ret 10h`): evento [direccion, ECX, canal, 0, volumen, 0]; la PRIMERA llamada modifica el canal (+4) del objeto de la prueba (si gMutOn)
inline u32 __fastcall FkVolume(void* self, void*, u32 a1, u32 a2, u32 a3, u32 a4) {
    Ev& e = PushEv(0x4D8870, 5);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = a2; e.v[3] = a3; e.v[4] = a4;
    if (gMutCalls++ == 0 && gMutOn) memcpy(reinterpret_cast<void*>(uintptr_t(gMutObj) + 4), &gMutChan, 4);
    return gRet;
}
// Cuerpo de destructor / constructor sin argumentos de pila (thiscall): evento [direccion del EXE del cuerpo, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
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
// Constructor con tres, cuatro y seis dwords de pila (thiscall): evento [direccion, ECX (etiqueta), argumentos]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor3(void* self, void*, u32 a, u32 b, u32 c) {
    Ev& e = PushEv(ADDR, 4);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkCtor4(void* self, void*, u32 a, u32 b, u32 c, u32 d) {
    Ev& e = PushEv(ADDR, 5);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkCtor6(void* self, void*, u32 a, u32 b, u32 c, u32 d, u32 f, u32 g) {
    Ev& e = PushEv(ADDR, 7);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d; e.v[5] = f; e.v[6] = g;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskSimpleGoToPoint (0x667CD0; cinco dwords de pila, el segundo es un puntero a CVector): evento [direccion, ECX, a, puntero (etiqueta), x, y, z del vector, c, d, e]
inline u32 __fastcall FkGoToPoint(void* self, void*, u32 a, const u32* pos, u32 c, u32 d, u32 f) {
    Ev& e = PushEv(0x667CD0, 9);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = Tag(pos); e.v[3] = pos[0]; e.v[4] = pos[1]; e.v[5] = pos[2]; e.v[6] = c; e.v[7] = d; e.v[8] = f;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskComplexGoToPointAndStandStill (0x668120; seis dwords de pila, el segundo es un puntero a CVector): evento [direccion, ECX, a, puntero (etiqueta), x, y, z del vector, c, d, e, f]
inline u32 __fastcall FkGoToStand(void* self, void*, u32 a, const u32* pos, u32 c, u32 d, u32 f, u32 g) {
    Ev& e = PushEv(0x668120, 10);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = Tag(pos); e.v[3] = pos[0]; e.v[4] = pos[1]; e.v[5] = pos[2]; e.v[6] = c; e.v[7] = d; e.v[8] = f; e.v[9] = g;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// ComputeTargetPos (0x649160; sin argumentos de pila): evento [direccion, ECX (etiqueta)]; escribe gVec en +0xC..+0x14
inline u32 __fastcall FkTargetPos(void* self, void*) {
    Ev& e = PushEv(0x649160, 1);
    e.v[0] = Tag(self);
    memcpy(static_cast<u8*>(self) + 0xC, gVec.data(), 12);
    return gRet;
}
// CVehicleAnimGroup::GetGroup (0x6E3B00; un dword de pila): evento [direccion, ECX (crudo), id]; devuelve gRet
inline u32 __fastcall FkGetGroup(void* self, void*, u32 id) {
    Ev& e = PushEv(0x6E3B00, 2);
    e.v[0] = u32(uintptr_t(self)); e.v[1] = id;
    return gRet;
}
// Constructor por defecto de CObject (0x5A1D10): evento [direccion, ECX (etiqueta), dword +0x1C, dword +0x34 al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkObjCtor(void* self, void*) {
    Ev& e = PushEv(0x5A1D10, 3);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(static_cast<u8*>(self) + 0x1C);
    e.v[2] = BitsOf(static_cast<u8*>(self) + 0x34);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CObject::SetIsStatic (0x5A0760; un dword de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento, dwords +0x1C, +0x34, +0x13C, +0x17C, +0x184 al entrar]; aplica XOR gXorStatic a +0x1C; devuelve gRet.
inline u32 __fastcall FkSetStatic(void* self, void*, u32 a1) {
    Ev& e = PushEv(0x5A0760, 7);
    u8* p = static_cast<u8*>(self);
    e.v[0] = Tag(self); e.v[1] = a1; e.v[2] = BitsOf(p + 0x1C); e.v[3] = BitsOf(p + 0x34); e.v[4] = BitsOf(p + 0x13C); e.v[5] = BitsOf(p + 0x17C); e.v[6] = BitsOf(p + 0x184);
    u32 flags = BitsOf(p + 0x1C) ^ gXorStatic;
    memcpy(p + 0x1C, &flags, 4);
    return gRet;
}
// CEntity::SetModelIndex (0x532AE0; un dword de pila, `ret 4`): evento [direccion, ECX (etiqueta), indice]; escribe el clump nuevo en +0x18 y aplica XOR a +0x1C y +0x34 (como el real toca las banderas); devuelve gRet.
inline u32 __fastcall FkSetModelIndex(void* self, void*, u32 idx) {
    Ev& e = PushEv(0x532AE0, 2);
    e.v[0] = Tag(self); e.v[1] = idx;
    u8* p = static_cast<u8*>(self);
    memcpy(p + 0x18, &gClumpNew, 4);
    u32 f1c = BitsOf(p + 0x1C) ^ gXorModel, f34 = BitsOf(p + 0x34) ^ gXorModel34;
    memcpy(p + 0x1C, &f1c, 4); memcpy(p + 0x34, &f34, 4);
    return gRet;
}
// GetAnimHierarchyFromSkinClump (0x734A40; cdecl, un argumento): evento [direccion, argumento]; devuelve gHier.
inline u32 __cdecl FkAnimHier(u32 clump) { Ev& e = PushEv(0x734A40, 1); e.v[0] = clump; return gHier; }
// RpHAnimIDGetIndex (0x7C51A0; cdecl, dos argumentos): evento [direccion, jerarquia, etiqueta]; devuelve gBone.
inline u32 __cdecl FkBoneIndex(u32 hier, u32 tagId) { Ev& e = PushEv(0x7C51A0, 2); e.v[0] = hier; e.v[1] = tagId; return gBone; }
// RpAnimBlendClumpInit (0x4D6720; cdecl, un argumento): evento [direccion, argumento]; devuelve gRet.
inline u32 __cdecl FkClumpInit(u32 clump) { Ev& e = PushEv(0x4D6720, 1); e.v[0] = clump; return gRet; }
// RwTexDictionaryFindHashNamedTexture (0x734E50; cdecl, dos argumentos): evento [direccion, diccionario, clave]; devuelve gTex.
inline u32 __cdecl FkFindTex(u32 dict, u32 key) { Ev& e = PushEv(0x734E50, 2); e.v[0] = dict; e.v[1] = key; return gTex; }
// GetFirstTexture (0x734940; cdecl, un argumento): evento [direccion, diccionario]; devuelve gTex1.
inline u32 __cdecl FkFirstTex(u32 dict) { Ev& e = PushEv(0x734940, 1); e.v[0] = dict; return gTex1; }
// IUnknown::Release de los COM falsos (stdcall, un argumento): evento [marca, objeto (etiqueta), vtable de [canal], +8 (etiqueta), +0xC (etiqueta), contador 0xB5F898 al entrar]; la primera llamada modifica +0xC del
// canal segun gRelMode (0 nada, 1 cero, 2 el COM de la arena D); devuelve gRet.
inline u32 __stdcall FkRelease(void* p) {
    Ev& e = PushEv(kEvRelease, 5);
    u8* t = reinterpret_cast<u8*>(uintptr_t(gSelf));
    e.v[0] = Tag(p); e.v[1] = BitsOf(t); e.v[2] = Tag(uintptr_t(BitsOf(t + 8))); e.v[3] = Tag(uintptr_t(BitsOf(t + 0xC))); e.v[4] = BitsOf(reinterpret_cast<const void*>(uintptr_t(kChannelCounter)));
    if (gRelCalls++ == 0) {
        if (gRelMode == 1) { const u32 z = 0; memcpy(t + 0xC, &z, 4); }
        else if (gRelMode == 2) memcpy(t + 0xC, &gRelObj3, 4);
    }
    return gRet;
}
inline void PutGlobal32(uintptr_t at, u32 v) { memcpy(reinterpret_cast<void*>(at), &v, 4); }
inline void PutGlobal16(uintptr_t at, u16 v) { memcpy(reinterpret_cast<void*>(at), &v, 2); }
inline void PutGlobal8(uintptr_t at, u8 v) { *reinterpret_cast<u8*>(at) = v; }

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

// COM falso: [objeto] = vtable (en kVtOff de su arena), con la ranura 2 (Release) = FkRelease
template<size_t W> inline u32 PlaceCom(Arena<W>& ar, u32 off) {
    ar.Put32(off, u32(ar.Obj(kVtOff)));
    ar.Put32(kVtOff, 0xDEAD0000u); ar.Put32(kVtOff + 4, 0xDEAD0001u); ar.Put32(kVtOff + 8, u32(reinterpret_cast<uintptr_t>(&FkRelease)));
    return u32(ar.Obj(off));
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[kSeed]); arenaB.Pattern(a[2], a[kSeed + 1]); arenaC.Pattern(a[2], a[kSeed + 2]); arenaD.Pattern(a[2], a[kSeed + 3]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gVec = {}; gHier = gBone = gTex = gTex1 = gClumpNew = gXorModel = gXorModel34 = gXorStatic = 0; gSelf = 0; gRelMode = gRelObj3 = gRelCalls = 0;
    gMutObj = gMutOn = gMutChan = gMutFactor = gMutCalls = 0;
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    call.ecx = thisA;
    switch (kKind[c.fn]) {
    case kStop:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 8, a[4]);
        break;
    case kStartTP:
        arenaA.Put32(o + 4, a[3]); arenaA.Put32(o + 0x18, a[4]);
        gRet = a[5];
        gMutObj = thisA; gMutOn = a[7]; gMutChan = a[6];
        break;
    case kChkPause:
        arenaA.Put32(o + 4, a[3]); arenaA.Put32(o + 0x1C, a[4]);
        PutGlobal8(kPauseUser, u8(a[5])); PutGlobal8(kPauseCode, u8(a[6])); PutGlobal8(kMenuSwitch, u8(a[7]));
        gMutObj = thisA; gMutOn = a[10]; gMutChan = a[8]; gMutFactor = a[9];
        gRet = a[11];
        break;
    case kChanCtor:
        call.args[0] = a[3]; call.args[1] = a[4]; call.args[2] = a[5]; call.args[3] = a[6]; call.argCount = 4;
        break;
    case kChanDtor: {
        u32 p8 = 0, q0 = 0;
        if (a[4] == 1) p8 = PlaceCom(arenaB, a[5]); else if (a[4] == 2) p8 = PlaceCom(arenaC, a[5]);
        if (a[6] == 3) q0 = p8; else if (a[6] == 1) q0 = PlaceCom(arenaB, a[7]); else if (a[6] == 2) q0 = PlaceCom(arenaC, a[7]);
        gRelObj3 = PlaceCom(arenaD, a[11]);
        gRelMode = a[8];
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 8, p8); arenaA.Put32(o + 0xC, q0);
        PutGlobal32(kChannelCounter, a[10]);
        gRet = a[9]; gSelf = thisA;
        break;
    }
    case kHandCtor: {
        const u32 ped = u32(arenaB.Obj(a[17]));
        const u32 pedMi = u32(arenaC.Obj(a[18])), handMiOwn = u32(arenaC.Obj(a[19]));
        const u32 handMi = a[20] == 0 ? handMiOwn : pedMi;
        const i32 txd = i32(a[14] & 0xFFFFu);
        arenaA.Put32(o, a[3]);
        const u32 pedClump = a[7] == 0 ? 0u : u32(arenaD.Obj(kClumpPed)), newClump = a[10] == 0 ? 0u : (a[10] == 1 ? u32(arenaD.Obj(kClumpNew)) : u32(arenaD.Obj(kClumpPed)));
        arenaD.Put8(kClumpPed, 2); arenaD.Put8(kClumpNew, 2);
        arenaB.Put32(a[17] + 0x18, pedClump);
        arenaB.Put32(a[17] + 0x20, ((a[6] & 0xFFFFu) << 16) | 0x5A5Au);
        arenaC.Put32(a[18] + 4, a[16]);
        arenaC.Put32(a[18] + 8, ((a[14] & 0xFFFFu) << 16) | (a[15] & 0xFFFFu));   // el indice de TXD es el WORD de +0xA (mitad alta del dword de +8)
        arenaD.Put32(kPoolObjects + 12 * txd, a[13]);
        arenaD.Put8(kPoolFlags + txd, u8(a[21] & 0x7F));
        arenaD.Put32(kPoolStruct, u32(arenaD.Obj(kPoolObjects))); arenaD.Put32(kPoolStruct + 4, u32(arenaD.Obj(kPoolFlags))); arenaD.Put32(kPoolStruct + 8, 0x1000u);
        PutGlobal32(kModelTable + 4 * (a[6] & 0xFFFFu), pedMi);
        PutGlobal32(kModelTable + 4 * a[4], handMi);
        PutGlobal32(kTxdPoolPtr, u32(arenaD.Obj(kPoolStruct)));
        gDtorWrite = a[26]; gRet = a[28]; gHier = a[8]; gBone = a[9]; gClumpNew = newClump; gTex = a[11]; gTex1 = a[12];
        gXorModel = a[24]; gXorModel34 = a[25]; gXorStatic = a[27];
        call.args[0] = a[4]; call.args[1] = ped; call.args[2] = a[5]; call.argCount = 3;
        break;
    }
    case kSddObj:
    case kSdd:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kCrane:
        for (unsigned i = 0; i < 4; ++i) PutGlobal16(kCraneWords[i], u16(a[4 + i]));
        call.args[0] = a[3]; call.argCount = 1; call.popBytes = 4;
        break;
    case kAnimId: {
        const u32 veh = u32(arenaB.Obj(a[4])), handling = u32(arenaC.Obj(a[5]));
        arenaA.Put32(o + 0x10, veh); arenaA.Put32(o + 0x14, a[3]);
        arenaB.Put32(a[4] + 0x384, handling);
        arenaC.Put8(a[5] + 0xDE, u8(a[6]));
        gRet = a[7];
        const u32 outAnim = u32(arenaD.Obj(a[8])), outGroup = a[10] ? outAnim : u32(arenaD.Obj(a[9]));
        call.args[0] = outGroup; call.args[1] = outAnim; call.argCount = 2;
        break;
    }
    case kSubBoat:
        gNew = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[6]; gDtorWrite = a[7]; gVec = {a[8], a[9], a[10]};
        call.args[0] = a[3]; call.argCount = 1;
        break;
    case kHitClone:
    case kAbClone:
        gNew = a[3] == 0 ? 0u : u32(arenaB.Obj(a[4]));
        gRet = a[5]; gDtorWrite = a[6];
        break;
    case kAbCtor:
        arenaA.Put32(o, a[3]);
        gRet = a[4]; gDtorWrite = a[5];
        break;
    case kSubGo:
        arenaA.Put32(o + 0xC, a[8]); arenaA.Put32(o + 0x10, a[9]); arenaA.Put32(o + 0x14, a[10]); arenaA.Put32(o + 0x18, a[11]); arenaA.Put32(o + 0x1C, a[12]);
        arenaA.Put32(o + 0x20, a[13]); arenaA.Put32(o + 0x24, a[14]); arenaA.Put32(o + 0x28, a[15]);
        gNew = a[4] == 0 ? 0u : u32(arenaB.Obj(a[5]));
        gRet = a[6]; gDtorWrite = a[7];
        call.args[0] = a[3]; call.argCount = 1;
        break;
    case kRoute:
        arenaA.Put32(o, a[3]); arenaA.Put32(o + 0x20, a[4] == 0 ? 0u : u32(arenaB.Obj(a[5])));
        gDtorWrite = a[6]; gRet = a[7];
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
            return Gate(entry, call.args.data(), call.argCount, c.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = kKind[fn] == kHandCtor ? json::array() : RegionChanges();   // la tabla de model infos (80000 bytes) solo se vigila con el hash
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kChanCtor: case kHandCtor: case kSddObj: case kSdd: case kSubBoat: case kHitClone: case kAbCtor: case kAbClone: case kSubGo: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kCrane: obs["ret"] = eax & 0xFFu; break;                                                                                                                                  // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                                                                          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN180A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN180A_CASES", buffer, sizeof(buffer)) > 0) {
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

inline void Update() {
    if (done) return; char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "fn180a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn180a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn180a"); }
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
    // Colaborador con hook RH VIRTUAL en la DLL (RH_ScopedVMTInstall)
    auto FV = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollabVirtual(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE: el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto ByExeAddress = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH cuya llamada, en la DLL, va a una funcion propia (direccion dada) y no a la del EXE: el falso va en la direccion del EXE y en esa funcion
    auto D = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        Pair p{exe, Body(dllAddress), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    constexpr uintptr_t kObjectDelete = 0x5A1F20;
    const uintptr_t objectDeleteDll = std::bit_cast<uintptr_t>(static_cast<void (*)(void*)>(&CObject::operator delete));
    env.stubs[1]  = {F(0x4D94D0, &FkT0<0x4D94D0>), F(0x4D8870, &FkVolume), F(0x4D9500, &FkT3<0x4D9500>), F(0x4D8F30, &FkT0<0x4D8F30>)};
    env.stubs[2]  = {F(0x4D8960, &FkScale)};
    env.stubs[5]  = {ByExeAddress(0x5A1D10, &FkObjCtor), F(0x734A40, &FkAnimHier), F(0x532B20, &FkT0<0x532B20>), ByExeAddress(0x7C51A0, &FkBoneIndex), FV(0x532AE0, &FkSetModelIndex),
                     F(0x4D6720, &FkClumpInit), F(0x734E50, &FkFindTex), F(0x734940, &FkFirstTex), FV(0x5A0760, &FkSetStatic)};
    env.stubs[4]  = {};
    env.stubs[6]  = {D(0x59F660, std::bit_cast<uintptr_t>(&CObjectBody::Destructor), &FkBody<0x59F660>), D(kObjectDelete, objectDeleteDll, &FkDelete<0x5A1F20>)};
    env.stubs[8]  = {F(0x6E3B00, &FkGetGroup)};
    env.stubs[9]  = {F(0x648FC0, &FkBody<0x648FC0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[10] = {F(0x649100, &FkBody<0x649100>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[11] = {F(0x62F310, &FkCtor4<0x62F310>), F(0x61A5A0, &FkNew<0x61A5A0>), F(0x649160, &FkTargetPos), F(0x668120, &FkGoToStand)};
    env.stubs[12] = {F(0x649280, &FkBody<0x649280>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[13] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x61A390, &FkBody<0x61A390>)};
    env.stubs[14] = {F(0x61A6F0, &FkBody<0x61A6F0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[15] = {F(0x61A6F0, &FkBody<0x61A6F0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[16] = {F(0x653090, &FkBody<0x653090>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[17] = {F(0x6531D0, &FkBody<0x6531D0>), F(0x61A5B0, &FkDelete<0x61A5B0>)};
    env.stubs[18] = {F(0x61A900, &FkCtor6<0x61A900>)};
    env.stubs[19] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x61A900, &FkCtor6<0x61A900>)};
    env.stubs[20] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x667CD0, &FkGoToPoint), F(0x62F310, &FkCtor4<0x62F310>), F(0x667E20, &FkCtor3<0x667E20>)};
    env.stubs[21] = {F(0x41B7D0, &FkDelete<0x41B7D0>), F(0x61A3D0, &FkBody<0x61A3D0>)};
    // el canal destruido (fn 4) libera COM falsos: el Release va por la vtable de las arenas, no por un parche
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 3u, 7u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[4] = "toda la funcion; los dos Release de COM van a los falsos de las vtables de las arenas, iguales en ambos lados";
    dllReal[5] = "toda la funcion salvo el constructor de CObject 0x5A1D10 y RpHAnimIDGetIndex 0x7C51A0 (sin hook: falso en la direccion del EXE, que la DLL llama), SetModelIndex 0x532AE0 y SetIsStatic 0x5A0760 (hooks virtuales) y los demas colaboradores con hook";
    dllReal[6] = "toda la funcion salvo el cuerpo del destructor de CObject (0x59F660; CObjectBody::Destructor, funcion propia de la DLL) y CObject::operator delete (0x5A1F20; funcion propia de la DLL): falso en la direccion del EXE y en ellas";
    for (unsigned f = 0; f < kFunctions; ++f) {
        json stubbed = json::array(), real = json::array();
        env.watches.emplace_back(kEntries[f], 16);
        for (const auto& s : env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe, 16);
            stubbed.push_back({{"exe", s.pair.exe}, {"dll", s.pair.dll != 0}, {"hooked", s.pair.hooked}, {"dll_addr", s.pair.dll}, {"dll_module", ModuleOf(s.pair.dll)}});
        }
        collaborators[kFn[f]] = {{"dll_real", json::array({dllReal[f]})}, {"stubbed", stubbed}, {"real", real}, {"hooks_found", hooksFound}, {"own_hook", ownHooks[kFn[f]]}, {"dll_only", json::array()}, {"iat", json(nullptr)}};
    }
    // Memoria global del juego tocada: se guarda al empezar y se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kPauseCode, 2); regions.emplace_back(kMenuSwitch, 1); regionsOf[2] = {regions.size() - 2, regions.size() - 1};   // pausa de codigo y de usuario, interruptor del menu
    regions.emplace_back(kChannelCounter, 4); regionsOf[4] = {regions.size() - 1};
    regions.emplace_back(kModelTable, kModelCount * 4); regions.emplace_back(kTxdPoolPtr, 4); regionsOf[5] = {regions.size() - 2, regions.size() - 1};
    regions.emplace_back(kCraneBase, kCraneBytes); regionsOf[7] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
