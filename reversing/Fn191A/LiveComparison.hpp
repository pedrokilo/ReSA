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

// Banco par a par Fn191A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 33 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedGlobalInstall / RH_ScopedNamedGlobalInstall / RH_ScopedOverloadedInstall) y V (RH_ScopedVMTInstall).
// El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40).
//
// Colaboradores: falso identico en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / F) para los que tienen hook y que la DLL llama por su funcion: CTask::operator new 0x61A5A0 y operator delete 0x61A5B0,
// CAnimManager::GetAnimationBlock 0x4D3940 y GetAnimation 0x4D42F0, CAnimBlendAssociation::SetFinishCallback 0x4CEBE0, psCameraShowRaster 0x745240 y CMemoryMgr::Free 0x72F430; y las otras funciones del lote que una funcion llama:
// SetEntryExitFlagWithName 0x43EF20 (desde SetEnabledByName), Destructor 0x678A90 (desde el destructor con borrado) y CreateSubTask 0x678BD0 (desde CreateNextSubTask) (Rt). Falso solo en la direccion del EXE (la DLL los llama por esa
// direccion): operator new[] 0x821195, operator delete[] 0x8213AE y operator delete 0x8214BD del CRT, y las seis funciones RW de RsSetPresetView (0x7F10B0, 0x7F1010, 0x7F0E30, 0x7F0910, 0x7EE1D0, 0x7EE2A0).
// Las llamadas virtuales a la subtarea (destructor con borrado, GetTaskType, MakeAbortable) van a una vtable falsa de la arena C (la misma memoria para EXE y DLL).
// Colaboradores REALES (hook apagado, sin falso): las cadenas de constructores/destructores base del EXE (0x61A390, 0x61A3B0, 0x61A6C0, 0x61A990, 0x667CD0, 0x61A6F0, 0x61A3A0, 0x61A3D0), cuya contraparte en la DLL son los
// constructores y destructores de C++ de las clases base (no se pueden falsear en la DLL): se compara el objeto resultante byte a byte. Las vtables que escriben los constructores/destructores se comparan como clase (EXE o
// vtable de la DLL: la vtable de C++ de la DLL es distinta de la del EXE por diseno; ver README.md).
// Memoria global del juego tocada (restaurada tras cada lado): puntero del pool de CEntryExit 0x96A7D8, lista y contador de vistas 0xC1707C / 0xC17080 y vista actual 0x8D2E30.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F191ALive {
using namespace F191AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F191AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put16(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    void Put8(size_t off, u32 v) { m[kGuard + off] = u8(v); }
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
inline u32 gRet{};                   // EAX que devuelven los falsos
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gNew{};                   // memoria que devuelve el falso de CTask::operator new (0 = sin memoria)
inline u32 gAbortRet{}, gType{};     // EAX de los falsos virtuales MakeAbortable y GetTaskType
inline u32 gBlock{}, gAnim{};        // resultados de los falsos de GetAnimationBlock y GetAnimation
inline std::array<u32, 2> gArr{};    // lo que devuelve operator new[] en la primera y la segunda llamada
inline unsigned gArrIdx{};
inline DWORD gBenchThread{};
inline constexpr u32 kBenchMarker = 0xA9B0B0;
inline constexpr u32 kEvDefaultCb = 0x4D1390;
inline u32 gDllDefaultCb{};
inline constexpr u32 kEvVDtor = 0x7E1EA5E0, kEvVType = 0x7E1EA5E1, kEvVAbort = 0x7E1EA5E2;

// operator new de CTask (0x61A5A0; cdecl, un tamano): evento [direccion, tamano]; devuelve gNew.
inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return gNew;
}
// operator delete de CTask (0x61A5B0) y operator delete escalar del CRT (0x8214BD) (cdecl, un puntero valido): evento [direccion, puntero (etiqueta), dword de [puntero] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// operator delete[] del CRT (0x8213AE; cdecl, un puntero crudo que puede no ser memoria valida): evento [direccion, puntero (etiqueta o crudo)].
inline void __cdecl FkDeleteArray(void* p) {
    Ev& e = PushEv(0x8213AE, 1);
    e.v[0] = Tag(p);
}
// operator new[] del CRT (0x821195; cdecl, un tamano): evento [direccion, tamano]; devuelve gArr[0] la primera vez y gArr[1] la segunda.
inline u32 __cdecl FkNewArray(u32 size) {
    Ev& e = PushEv(0x821195, 1);
    e.v[0] = size;
    return gArr[gArrIdx < 2 ? gArrIdx++ : 1];
}
// Cuerpo del destructor (0x678A90; thiscall, sin argumentos de pila): evento [direccion, ECX (etiqueta), dword que tenia [ECX]]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CAnimManager::GetAnimationBlock(const char*) (0x4D3940; cdecl): evento [direccion, puntero (etiqueta), hash FNV de la cadena]; devuelve gBlock.
inline u32 __cdecl FkGetAnimBlock(const char* name) {
    Ev& e = PushEv(0x4D3940, 2);
    e.v[0] = Tag(name); e.v[1] = FnvStr(name);
    return gBlock;
}
// CAnimManager::GetAnimation(const char*, const CAnimBlock*) (0x4D42F0; cdecl): evento [direccion, puntero (etiqueta), hash FNV de la cadena, bloque (etiqueta)]; devuelve gAnim.
inline u32 __cdecl FkGetAnim(const char* name, const void* block) {
    Ev& e = PushEv(0x4D42F0, 3);
    e.v[0] = Tag(name); e.v[1] = FnvStr(name); e.v[2] = Tag(block);
    return gAnim;
}
// CAnimBlendAssociation::SetFinishCallback (0x4CEBE0; thiscall, dos argumentos de pila, `ret 8`): evento [direccion, ECX (etiqueta), funcion, dato (etiqueta)]. El DefaultAnimCB de la DLL se normaliza al del EXE
// (la direccion de la funcion no es observable por el juego; si el valor no es ninguno de los dos se anota crudo).
inline void __fastcall FkSetFinish(void* self, void*, void* cb, void* data) {
    Ev& e = PushEv(0x4CEBE0, 3);
    const u32 f = u32(uintptr_t(cb));
    e.v[0] = Tag(self);
    e.v[1] = (f == kEvDefaultCb || f == gDllDefaultCb) ? kEvDefaultCb : f;
    e.v[2] = Tag(data);
}
// psCameraShowRaster (0x745240; cdecl, la camara): evento [direccion, camara (etiqueta)]; devuelve gRet.
inline u32 __cdecl FkShowRaster(void* camera) {
    Ev& e = PushEv(0x745240, 1);
    e.v[0] = Tag(camera);
    return gRet;
}
// CMemoryMgr::Free (0x72F430; cdecl, un puntero): evento [direccion, puntero (etiqueta)].
inline void __cdecl FkFree(void* p) {
    Ev& e = PushEv(0x72F430, 1);
    e.v[0] = Tag(p);
}
// Funciones RW de RsSetPresetView (cdecl; el EXE y la DLL las llaman por la direccion del EXE): eventos con la direccion y los argumentos (los vectores se leen como dwords).
inline u32 __cdecl FkFrameSetIdentity(void* frame) { Ev& e = PushEv(0x7F10B0, 1); e.v[0] = Tag(frame); return u32(uintptr_t(frame)); }
inline u32 __cdecl FkFrameRotate(void* frame, const u32* axis, u32 angle, u32 combine) {
    Ev& e = PushEv(0x7F1010, 6);
    e.v[0] = Tag(frame); e.v[1] = axis[0]; e.v[2] = axis[1]; e.v[3] = axis[2]; e.v[4] = angle; e.v[5] = combine;
    return u32(uintptr_t(frame));
}
inline u32 __cdecl FkFrameTranslate(void* frame, const u32* vec, u32 combine) {
    Ev& e = PushEv(0x7F0E30, 6);
    e.v[0] = Tag(frame); e.v[1] = Tag(vec); e.v[2] = vec[0]; e.v[3] = vec[1]; e.v[4] = vec[2]; e.v[5] = combine;
    return u32(uintptr_t(frame));
}
inline u32 __cdecl FkFrameUpdate(void* frame) { Ev& e = PushEv(0x7F0910, 1); e.v[0] = Tag(frame); return u32(uintptr_t(frame)); }
inline u32 __cdecl FkSetNear(void* camera, u32 bits) { Ev& e = PushEv(0x7EE1D0, 2); e.v[0] = Tag(camera); e.v[1] = bits; return u32(uintptr_t(camera)); }
inline u32 __cdecl FkSetFar(void* camera, u32 bits) { Ev& e = PushEv(0x7EE2A0, 2); e.v[0] = Tag(camera); e.v[1] = bits; return u32(uintptr_t(camera)); }
// CEntryExitManager::SetEntryExitFlagWithName (0x43EF20; cdecl, nombre / mascara / activar): evento [direccion, nombre (etiqueta), mascara, byte bajo de activar] (la subrutina real solo lee el byte).
inline void __cdecl FkFlagName(const char* name, u32 mask, u32 enable) {
    Ev& e = PushEv(0x43EF20, 3);
    e.v[0] = Tag(name); e.v[1] = mask; e.v[2] = enable & 0xFFu;
}
// CTaskComplexFallAndStayDown::CreateSubTask (0x678BD0; thiscall, un dword de pila, `ret 4`): evento [direccion, ECX (etiqueta), tipo]; devuelve gRet.
inline u32 __fastcall FkCreateSub(void* self, void*, u32 type) {
    Ev& e = PushEv(0x678BD0, 2);
    e.v[0] = Tag(self); e.v[1] = type;
    return gRet;
}
// Ranuras de la vtable falsa de la subtarea: destructor con borrado (ranura 0; thiscall, banderas en la pila, `ret 4`), GetTaskType (ranura 4) y MakeAbortable (ranura 6; ped, prioridad y evento, `ret 0Ch`).
inline u32 __fastcall FkVDtor(void* self, void*, u32 flags) { Ev& e = PushEv(kEvVDtor, 2); e.v[0] = Tag(self); e.v[1] = flags; return u32(uintptr_t(self)); }
inline u32 __fastcall FkVType(void* self, void*) { Ev& e = PushEv(kEvVType, 1); e.v[0] = Tag(self); return gType; }
inline u32 __fastcall FkVAbort(void* self, void*, void* ped, u32 prio, void* event) {
    Ev& e = PushEv(kEvVAbort, 4);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = prio; e.v[3] = Tag(event);
    return gAbortRet;
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
    std::array<u32, 12> args{};
    u32 argCount{}, popBytes{}, ecx{};
    bool floatRet{};
};
// Dwords de vtable que la funcion escribe (arena, desplazamiento, vtable del EXE): se omiten de los cambios y se anotan como clase ("vt" si es la del EXE o una de la DLL)
struct VptrSlot { unsigned arena; u32 off; u32 exeVt; };
inline std::vector<VptrSlot> vptrSlots;

inline void PutStr(Arena<kWin>& ar, u32 off, u32 seed, u32 len) {
    for (u32 i = 0; i < len; ++i) ar.Put8(off + i, u8(StrChar(seed, i)));
    ar.Put8(off + len, 0);
}
// Subtarea falsa: objeto en B (su primer dword apunta a la vtable falsa de C)
inline void MakeFakeSub(u32 subOff, u32 vtOff) {
    arenaC.Put32(vtOff + 0x0, u32(uintptr_t(&FkVDtor)));
    arenaC.Put32(vtOff + 0x10, u32(uintptr_t(&FkVType)));
    arenaC.Put32(vtOff + 0x18, u32(uintptr_t(&FkVAbort)));
    arenaB.Put32(subOff, u32(arenaC.Obj(vtOff)));
}
inline void PutPoolEntries(const C& c, u32 poolOff, u32 storOff, u32 statesOff, u32 cap, u32 entrySeed, u32 nameSeed, u32 nameLen, u32 statesSeed) {
    arenaB.Put32(poolOff + 0, u32(arenaC.Obj(storOff))); arenaB.Put32(poolOff + 4, u32(arenaD.Obj(statesOff))); arenaB.Put32(poolOff + 8, cap);
    const u32 n = cap < 12 ? cap : 12;
    for (u32 i = 0; i < n; ++i) {
        const auto nm = EntryName(entrySeed, i, nameSeed, nameLen);
        for (u32 j = 0; j < 8; ++j) arenaC.Put8(storOff + 0x3C * i + j, nm[j]);
        arenaC.Put16(storOff + 0x3C * i + 0x30, Rand(entrySeed, 300 + i) & 0xFFFFu);
        arenaD.Put8(statesOff + i, (Rand(statesSeed, i) & 0x7Fu) | (Rand(statesSeed, 500 + i) % 100 < 25 ? 0x80u : 0u));
    }
    (void)c;
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gAbortRet = 0; gType = 0; gBlock = 0; gAnim = 0; gArr = {}; gArrIdx = 0;
    gDllDefaultCb = u32(uintptr_t(&CDefaultAnimCallback::DefaultAnimCB));
    vptrSlots.clear();
    *reinterpret_cast<u8*>(kBenchMarker) = 0x5A;   // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa (mutate.py) condiciona sus mutaciones a ella
    const u32 o = a[0];
    const u32 thisA = u32(arenaA.Obj(o));
    arenaA.Put32(o, a[3]);
    call.ecx = thisA;
    const auto cdecl_ = [&](u32 n) { call.argCount = n; call.popBytes = 4 * n; };
    const auto stdcall_ = [&](u32 n) { call.argCount = n; call.popBytes = 0; };
    switch (kKind[c.fn]) {
    case kConst:
        break;
    case kGotoAbort:
    case kSdAbort: {
        const u32 mode = a[11];
        const u32 sub = mode == 0 ? 0u : (mode == 1 ? u32(arenaB.Obj(a[8])) : a[12]);
        if (mode == 1) MakeFakeSub(a[8], a[9]);
        arenaA.Put32(o + 8, sub);
        gAbortRet = a[7]; gType = a[10];
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; stdcall_(3);
        break;
    }
    case kCenter: {
        for (u32 i = 0; i < 5; ++i) arenaB.Put32(a[4] + 8 + 4 * i, a[5 + i]);
        arenaA.Put32(o + 0xC, u32(arenaB.Obj(a[4])));
        gNew = a[10] ? u32(arenaC.Obj(a[11])) : 0u;
        if (a[10]) vptrSlots.push_back({2, a[11], 0x86FD50});
        call.args[0] = a[12]; stdcall_(1);
        break;
    }
    case kSlide0:
    case kSlide1: {
        for (u32 i = 0; i < 3; ++i) arenaB.Put32(a[4] + 4 * i, a[5 + i]);
        vptrSlots.push_back({0, o, 0x86FFEC});
        call.args[0] = u32(arenaB.Obj(a[4])); call.args[1] = a[8]; call.args[2] = a[9]; stdcall_(3);
        if (kKind[c.fn] == kSlide1) {
            PutStr(arenaC, a[12], a[10], a[11]); PutStr(arenaD, a[15], a[13], a[14]);
            call.args[3] = u32(arenaC.Obj(a[12])); call.args[4] = u32(arenaD.Obj(a[15]));
            call.args[5] = a[16]; call.args[6] = a[17]; call.args[7] = a[18]; call.args[8] = a[19]; stdcall_(9);
            gBlock = a[20]; gAnim = a[21];
        }
        break;
    }
    case kSlideDtor: {
        arenaA.Put32(o + 0xC, a[4]);
        const u32 anim = a[5] ? u32(arenaB.Obj(a[6])) : 0u;
        arenaA.Put32(o + 8, anim);
        if (a[5]) { arenaB.Put32(a[6] + 0x18, a[7]); arenaB.Put32(a[6] + 0x1C, a[8]); arenaB.Put32(a[6] + 0x2C, a[9]); }
        vptrSlots.push_back({0, o, 0x86D48C});
        break;
    }
    case kRsShow:
        gRet = a[5];
        call.args[0] = a[4]; cdecl_(1);
        break;
    case kRsSet: {
        const u32 camera = a[4] ? u32(arenaC.Obj(a[5])) : 0u;
        const u32 frame = a[6] == 0 ? u32(arenaD.Obj(a[7])) : a[6];
        if (a[4]) arenaC.Put32(a[5] + 4, frame);
        const i32 nViews = i32(a[8]);
        const u32 count = nViews > 0 ? u32(nViews) : 1u;
        for (u32 j = 0; j < count; ++j) {
            arenaB.Put32(a[10] + 0x30 * j + 0x20, j + 1 < count ? u32(arenaB.Obj(a[10] + 0x30 * (j + 1))) : 0u);
            arenaB.Put32(a[10] + 0x30 * j + 0x1C, a[18]);
        }
        const bool valid = camera && nViews > 0 && i32(a[9]) >= 0 && i32(a[9]) < nViews;
        if (valid) {
            const u32 k = u32(nViews) - a[9] - 1u;
            for (u32 i = 0; i < 7; ++i) arenaB.Put32(a[10] + 0x30 * k + 4 * i, a[11 + i]);
        }
        *reinterpret_cast<u32*>(kViewCount) = a[8];
        *reinterpret_cast<u32*>(kViewList) = u32(arenaB.Obj(a[10]));
        *reinterpret_cast<u32*>(kViewCurrent) = a[19];
        call.args[0] = camera; call.args[1] = a[9]; cdecl_(2);
        break;
    }
    case kRsDestroy: {
        const u32 count = a[4];
        for (u32 i = 0; i < count; ++i) {
            arenaB.Put32(a[5] + 0x30 * i + 0x20, i + 1 < count ? u32(arenaB.Obj(a[5] + 0x30 * (i + 1))) : 0u);
            arenaB.Put32(a[5] + 0x30 * i + 0x1C, ((a[6] >> i) & 1u) ? a[8 + i] : 0u);
        }
        *reinterpret_cast<u32*>(kViewList) = count ? u32(arenaB.Obj(a[5])) : 0u;
        *reinterpret_cast<u32*>(kViewCount) = a[7];
        break;
    }
    case kPoolDtor:
    case kPoolSdd:
        arenaA.Put32(o + 0, a[6]); arenaA.Put32(o + 4, a[7]); arenaA.Put32(o + 8, a[4]); arenaA.Put32(o + 0xC, a[8]); arenaA.Put32(o + 0x10, a[5]);
        if (kKind[c.fn] == kPoolSdd) { call.args[0] = a[9]; stdcall_(1); }
        break;
    case kGetSlot: {
        arenaB.Put32(a[4] + 0, a[5] == 0 ? u32(arenaC.Obj(a[6])) : a[7]); arenaB.Put32(a[4] + 4, u32(arenaD.Obj(a[8]))); arenaB.Put32(a[4] + 8, a[10]);
        arenaD.Put8(u32(i32(a[8]) + i32(a[9])), a[12]);
        *reinterpret_cast<u32*>(kPoolPtr) = u32(arenaB.Obj(a[4]));
        call.args[0] = a[9]; cdecl_(1);
        break;
    }
    case kFlagName:
    case kFlagIndex: {
        PutPoolEntries(c, a[4], a[5], a[6], a[7], a[10], a[8], a[9], a[12]);
        PutStr(arenaB, a[11], a[8], a[9]);
        if (kKind[c.fn] == kFlagIndex) arenaD.Put8(a[6] + a[13], arenaD.m[kGuard + a[6] + a[13]] & 0x7Fu);
        *reinterpret_cast<u32*>(kPoolPtr) = u32(arenaB.Obj(a[4]));
        if (kKind[c.fn] == kFlagName) call.args[0] = u32(arenaB.Obj(a[11])); else call.args[0] = a[13];
        call.args[1] = a[14]; call.args[2] = a[15]; cdecl_(3);
        break;
    }
    case kPoolCtor:
        gArr[0] = a[6] == 0 ? u32(arenaC.Obj(a[7])) : a[8]; gArr[1] = u32(arenaD.Obj(a[9]));
        call.args[0] = a[4]; call.args[1] = a[5]; stdcall_(2);
        break;
    case kPoolNew: {
        arenaA.Put32(o + 0, a[9] == 0 ? u32(arenaC.Obj(a[10])) : a[11]); arenaA.Put32(o + 4, u32(arenaD.Obj(a[6]))); arenaA.Put32(o + 8, a[4]); arenaA.Put32(o + 0xC, a[5]);
        for (u32 i = 0; i < a[4]; ++i) arenaD.Put8(a[6] + i, (Rand(a[7], i) & 0x7Fu) | (Rand(a[7], 500 + i) % 100 < a[8] ? 0x80u : 0u));
        break;
    }
    case kSetEnabled:
        call.args[0] = a[4] == 0 ? u32(arenaB.Obj(a[5])) : a[6]; call.args[1] = a[7]; cdecl_(2);
        break;
    case kPadRight:
        arenaA.Put32(o + 0x14, a[4]); arenaA.Put32(o + 0x44, a[5]);
        break;
    case kSetVel:
        call.args[0] = a[4]; call.args[1] = a[5]; call.args[2] = a[6]; stdcall_(3);
        break;
    case kComplexDtor: {
        const u32 sub = a[4] ? u32(arenaB.Obj(a[5])) : 0u;
        if (a[4]) MakeFakeSub(a[5], a[6]);
        arenaA.Put32(o + 8, sub);
        vptrSlots.push_back({0, o, 0x86D48C});
        break;
    }
    case kSdCtor1:
        vptrSlots.push_back({0, o, 0x870480});
        call.args[0] = a[4]; call.args[1] = a[5]; stdcall_(2);
        break;
    case kSdCtor2:
        vptrSlots.push_back({0, o, 0x870480});
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kSdFirst:
        arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[6]);
        gNew = a[7] ? u32(arenaB.Obj(a[8])) : 0u;
        if (a[7]) vptrSlots.push_back({1, a[8], 0x870430});
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kSdControl:
        arenaA.Put32(o + 8, a[5]);
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kSdCreateSub:
        arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[6]);
        gNew = a[7] ? u32(arenaB.Obj(a[8])) : 0u;
        if (a[7] && a[4] == 0xCFu) vptrSlots.push_back({1, a[8], 0x870430});
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kSdSdd:
        gRet = a[5]; gDtorWrite = a[6];
        call.args[0] = a[4]; stdcall_(1);
        break;
    case kSdClone:
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x10, a[5]);
        gNew = a[6] ? u32(arenaB.Obj(a[7])) : 0u;
        if (a[6]) vptrSlots.push_back({1, a[7], 0x870480});
        break;
    case kSdNext:
        arenaA.Put32(o + 8, u32(arenaB.Obj(a[5])));
        MakeFakeSub(a[5], a[6]);
        gType = a[7]; gRet = a[8];
        call.args[0] = a[4]; stdcall_(1);
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
        case kConst: case kPadRight: case kRsSet: obs["ret"] = eax; break;                                                                             // valor completo de EAX
        case kGotoAbort: case kSdAbort: obs["ret"] = eax & 0xFFu; break;                                                                               // bool: solo AL
        case kCenter: case kSlide0: case kSlide1: case kRsShow: case kGetSlot: case kPoolCtor: case kPoolNew: case kPoolSdd: case kSdCtor1: case kSdCtor2: case kSdFirst:
        case kSdControl: case kSdCreateSub: case kSdSdd: case kSdClone: case kSdNext: obs["ret"] = Tag(uintptr_t(eax)); break;                       // puntero como etiqueta de arena
        default: obs["ret"] = nullptr; break;                                                                                                          // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN191A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN191A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn191a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn191a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn191a"); }
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
    // Colaborador REAL (con hook RH apagado, sin falso): el EXE corre sus bytes y la DLL su equivalente en C++
    auto R = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (CRT, funciones RW): el falso va solo en la direccion del EXE. Se exige que no tenga hook.
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    env.stubs[2] = {F(0x61A5A0, &FkNew)};
    env.reals[2] = {R(0x667CD0), R(0x61A390)};
    env.reals[3] = {R(0x61A6C0), R(0x61A390)};
    env.stubs[4] = {F(0x4D3940, &FkGetAnimBlock), F(0x4D42F0, &FkGetAnim)};
    env.reals[4] = {R(0x61A990), R(0x61A390)};
    env.stubs[5] = {F(0x4CEBE0, &FkSetFinish)};
    env.reals[5] = {R(0x61A6F0), R(0x61A3A0)};
    env.stubs[6] = {F(0x745240, &FkShowRaster)};
    env.stubs[7] = {E(0x7F10B0, &FkFrameSetIdentity), E(0x7F1010, &FkFrameRotate), E(0x7F0E30, &FkFrameTranslate), E(0x7F0910, &FkFrameUpdate), E(0x7EE1D0, &FkSetNear), E(0x7EE2A0, &FkSetFar)};
    env.stubs[8] = {F(0x72F430, &FkFree)};
    env.stubs[9] = {E(0x8213AE, &FkDeleteArray)};
    env.stubs[13] = {E(0x821195, &FkNewArray)};
    env.stubs[14] = {E(0x8213AE, &FkDeleteArray)};
    env.stubs[16] = {E(0x8213AE, &FkDeleteArray), E(0x8214BD, &FkDelete<0x8214BD>)};
    env.stubs[17] = {Rt(11, &FkFlagName)};
    env.reals[21] = {R(0x61A3D0)};
    env.reals[22] = {R(0x61A3B0)};
    env.reals[24] = {R(0x61A3B0)};
    env.reals[25] = {R(0x61A3D0)};
    env.stubs[27] = {F(0x61A5A0, &FkNew)};
    env.reals[27] = {R(0x61A390)};
    env.stubs[29] = {F(0x61A5A0, &FkNew)};
    env.reals[29] = {R(0x61A390)};
    env.stubs[30] = {F(0x61A5B0, &FkDelete<0x61A5B0>), Rt(25, &FkBody<0x678A90>)};
    env.stubs[31] = {F(0x61A5A0, &FkNew)};
    env.reals[31] = {R(0x61A3B0)};
    env.stubs[32] = {Rt(29, &FkCreateSub)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 1u, 10u, 11u, 12u, 15u, 18u, 19u, 20u, 23u, 28u}) dllReal[f] = "toda la funcion (sin colaboradores; las llamadas virtuales a la subtarea van a la vtable falsa de la arena C)";
    for (unsigned f : {2u, 3u, 4u, 5u, 21u, 22u, 24u, 25u, 27u, 29u, 31u}) dllReal[f] = "toda la funcion; las cadenas de constructores y destructores base del EXE son colaboradores reales (hook apagado) y en la DLL son los constructores/destructores de C++ de las clases base";
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
    // Memoria global del juego tocada: puntero del pool de CEntryExit (GetInSlot y las dos de banderas), lista / contador / vista actual de las vistas preestablecidas (RsSetPresetView, RsDestroyPresetViews); se restauran tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    regions.emplace_back(kPoolPtr, 4); regionsOf[10] = regionsOf[11] = regionsOf[12] = {regions.size() - 1};
    regions.emplace_back(kViewList, 4); regions.emplace_back(kViewCount, 4); regions.emplace_back(kViewCurrent, 4);
    regionsOf[7] = {regions.size() - 3, regions.size() - 2, regions.size() - 1};
    regionsOf[8] = {regions.size() - 3, regions.size() - 2};
    regions.emplace_back(kBenchMarker, 1);
    for (unsigned f = 0; f < kFunctions; ++f) regionsOf[f].push_back(regions.size() - 1);
    Run(env, ready, registered, bodies, collaborators);
}
}
