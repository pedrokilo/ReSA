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

// Banco par a par Fn185A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 26 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall / RH_ScopedGlobalInstall / RH_ScopedOverloadedInstall) y V (RH_ScopedVMTInstall).
// El banco invoca la direccion del EXE con objetos sinteticos en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL
// (MakeCollab / Pair): AudioEngine.ReportFrontendAudioEvent 0x506EA0, TheCamera.SetFadeColour / Fade / ProcessFade / ProcessMusicFade (0x50BF00, 0x50AC20, 0x50B5D0, 0x50B6D0), CSprite2d::DrawRect 0x727B60,
// CPhysical::CPhysical 0x542260, CObject::Init 0x59F840, SetModelIndexNoCreate 0x533700 (virtual), AttachToRwObject 0x533ED0, AllocateMatrix 0x54F560, UpdateMatrix 0x54F1B0, SetMatrix 0x54F610,
// DetachFromRwObject 0x533FB0, GetFirstAtomic 0x734820, IsCBPCPipelineAttached 0x5D7F40, LoadHandlingData 0x5BD830, CTask::operator new 0x61A5A0 y delete 0x61A5B0, los constructores de Chat 0x684290, Shove 0x6846F0
// y FollowPedFootsteps 0x694E20, los cuerpos de destructor 0x683A40, 0x684200, 0x684320, 0x684750, 0x694A40, 0x694B30, 0x694C10 y 0x61A3D0, CEntity::CleanUpOldReference 0x571A00, CPointRoute::operator delete 0x41B7D0,
// GetFormationOffset 0x611F20 y RegisterReference 0x571B70; COnscreenTimerEntry::Process 0x44CB10 (otra funcion probada del lote, indice 1) para COnscreenTimer::Process. Corren REALES con el hook apagado: el constructor
// base de CTaskComplex 0x61A3B0 y el cuerpo 0x684280 de Greet (vtable 0x87078C y salto a 0x683A40). Memoria global del juego tocada (restaurada tras cada lado): ventana de 0x100 bytes de ScriptSpace (+0x1000), paso del
// temporizador 0xB7CB5C, pantalla ancha 0xB6F065, bandera de HUD 0xBAA45D, modo de replay 0xA43088, variables del fundido (pausas, hora, FadeTimer, FadeOutTime, FadeValue, colores, pantalla de carga, ancho y alto de pantalla)
// y el puntero al pool de objetos 0xB7449C. Los pools son sinteticos. Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas), los cambios de la memoria global, los eventos de los
// falsos (con sus argumentos), el valor devuelto y el estado x87 (siempre vacio). En el constructor con vtable (0x695B20) la DLL puede dejar la suya (dentro del ASI): se comprueba (`vtable_ok`) y se normaliza al valor
// del EXE antes de comparar. EAX/ECX/EDX tras la llamada quedan en original_residue / candidate_residue (informativos).

namespace F185ALive {
using namespace F185AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F185AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
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
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de constructor
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gTaskNew{};               // lo que devuelve CTask::operator new
inline u32 gPtr{};                   // lo que devuelve GetFormationOffset / AllocateMatrix
inline u32 gAtomic{}, gCbpc{};       // lo que devuelven GetFirstAtomic y IsCBPCPipelineAttached
inline DWORD gBenchThread{};

// CTask::operator delete (0x61A5B0; cdecl, un argumento valido): evento [direccion, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(0x61A5B0, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Cuerpo del destructor (thiscall, sin argumentos de pila, `ret`): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CEntity::RegisterReference (0x571B70; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escribe nada.
inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(0x571B70, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// CEntity::CleanUpOldReference (0x571A00; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escribe nada.
inline void __fastcall FkCleanup(void* self, void*, void* ref) {
    Ev& e = PushEv(0x571A00, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// CPointRoute::operator delete (0x41B7D0; cdecl, un puntero): evento [direccion, puntero (etiqueta)].
inline void __cdecl FkRouteDelete(void* p) {
    Ev& e = PushEv(0x41B7D0, 1);
    e.v[0] = Tag(p);
}
// CTask::operator new (0x61A5A0; cdecl, el tamano): evento [direccion, tamano]; devuelve gTaskNew (un objeto de la arena B o nulo).
inline void* __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gTaskNew));
}
// Constructor de CTaskComplexPartnerChat (0x684290; thiscall, diez dwords de pila, `ret 28h`): evento [direccion, ECX, nombre (etiqueta), companero (etiqueta), lider, multiplicador, cuenta, conversacion, byte, x, y, z]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtorChat(void* self, void*, u32 name, u32 partner, u32 leader, u32 mult, u32 count, u32 conv, u32 a8, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(0x684290, 11);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(name)); e.v[2] = Tag(uintptr_t(partner)); e.v[3] = leader; e.v[4] = mult; e.v[5] = u32(int32(int8(count))); e.v[6] = conv; e.v[7] = a8; e.v[8] = x; e.v[9] = y; e.v[10] = z;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskComplexPartnerShove (0x6846F0; thiscall, ocho dwords de pila, `ret 20h`): evento [direccion, ECX, nombre (etiqueta), companero (etiqueta), lider, multiplicador, cuenta, x, y, z]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtorShove(void* self, void*, u32 name, u32 partner, u32 leader, u32 mult, u32 count, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(0x6846F0, 9);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(name)); e.v[2] = Tag(uintptr_t(partner)); e.v[3] = leader; e.v[4] = mult; e.v[5] = u32(int32(int8(count))); e.v[6] = x; e.v[7] = y; e.v[8] = z;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Constructor de CTaskComplexFollowPedFootsteps (0x694E20; thiscall, un dword de pila, `ret 4`): evento [direccion, ECX, ped (etiqueta)]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtorFoot(void* self, void*, u32 ped) {
    Ev& e = PushEv(0x694E20, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(ped));
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// GetFormationOffset (0x611F20; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX (crudo), dx, dy]; devuelve gPtr.
inline u32 __fastcall FkFormOff(void* self, void*, u32 dx, u32 dy) {
    Ev& e = PushEv(0x611F20, 3);
    e.v[0] = Tag(self); e.v[1] = dx; e.v[2] = dy;
    return gPtr;
}
// cHandlingDataMgr::LoadHandlingData (0x5BD830; thiscall, sin pila): evento [direccion, ECX (etiqueta), cinco dwords de +0..+0x10 al entrar].
inline void __fastcall FkLoadHandling(void* self, void*) {
    Ev& e = PushEv(0x5BD830, 6);
    e.v[0] = Tag(self);
    for (u32 i = 0; i < 5; ++i) e.v[1 + i] = static_cast<const u32*>(self)[i];
}
// AudioEngine.ReportFrontendAudioEvent (0x506EA0; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (crudo), evento, float1, float2].
inline void __fastcall FkAudio(void* self, void*, u32 ev, u32 f1, u32 f2) {
    Ev& e = PushEv(0x506EA0, 4);
    e.v[0] = Tag(self); e.v[1] = ev; e.v[2] = f1; e.v[3] = f2;
}
// CCamera::Fade (0x50AC20; thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX, duracion (bits), direccion].
inline void __fastcall FkFade(void* self, void*, u32 duration, u32 dir) {
    Ev& e = PushEv(0x50AC20, 3);
    e.v[0] = Tag(self); e.v[1] = duration; e.v[2] = dir;
}
// CCamera::SetFadeColour (0x50BF00; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX, r, g, b].
inline void __fastcall FkSetFadeColour(void* self, void*, u32 r, u32 g, u32 b) {
    Ev& e = PushEv(0x50BF00, 4);
    e.v[0] = Tag(self); e.v[1] = r; e.v[2] = g; e.v[3] = b;
}
// CCamera::ProcessFade (0x50B5D0) y ProcessMusicFade (0x50B6D0): thiscall sin pila; evento [direccion, ECX].
template<u32 ADDR> inline void __fastcall FkCamNoArg(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
}
// CSprite2d::DrawRect (0x727B60; cdecl, un puntero al rectangulo y un puntero al color): evento [direccion, cuatro floats del rectangulo (bits), color (dword)].
inline void __cdecl FkDrawRect(const u32* rect, const u32* color) {
    Ev& e = PushEv(0x727B60, 5);
    for (u32 i = 0; i < 4; ++i) e.v[i] = rect[i];
    e.v[4] = *color;
}
// CPhysical::CPhysical (0x542260; thiscall, sin pila): evento [direccion, ECX, dword de [ECX] al entrar].
inline void __fastcall FkPhys(void* self, void*) {
    Ev& e = PushEv(0x542260, 2);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self);
}
// CObject::Init (0x59F840; thiscall, sin pila): evento [direccion, ECX, dword de [ECX], dword de [ECX + 0x170]].
inline void __fastcall FkInit(void* self, void*) {
    Ev& e = PushEv(0x59F840, 3);
    e.v[0] = Tag(self); e.v[1] = BitsOf(self); e.v[2] = BitsOf(static_cast<const u8*>(self) + 0x170);
}
// CEntity::SetModelIndexNoCreate (0x533700; thiscall, un dword, `ret 4`): evento [direccion, ECX, indice].
inline void __fastcall FkSetModelIdx(void* self, void*, u32 id) {
    Ev& e = PushEv(0x533700, 2);
    e.v[0] = Tag(self); e.v[1] = id;
}
// CEntity::AttachToRwObject (0x533ED0; thiscall, dos dwords, `ret 8`): evento [direccion, ECX, objeto RW (etiqueta), actualizar]. Devuelve nada.
inline void __fastcall FkAttach(void* self, void*, u32 rw, u32 upd) {
    Ev& e = PushEv(0x533ED0, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(uintptr_t(rw)); e.v[2] = upd;
}
// CPlaceable::AllocateMatrix (0x54F560; thiscall, sin pila): evento [direccion, ECX]; deja gPtr en [ECX + 0x14] (la matriz reservada).
inline void __fastcall FkAllocMat(void* self, void*) {
    Ev& e = PushEv(0x54F560, 1);
    e.v[0] = Tag(self);
    memcpy(static_cast<u8*>(self) + 0x14, &gPtr, 4);
}
// CSimpleTransform::UpdateMatrix (0x54F1B0; thiscall, un puntero, `ret 4`): evento [direccion, ECX, matriz (etiqueta)].
inline void __fastcall FkUpdateMat(void* self, void*, void* mat) {
    Ev& e = PushEv(0x54F1B0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(mat);
}
// CPlaceable::SetMatrix (0x54F610; thiscall, un puntero, `ret 4`): evento [direccion, ECX, matriz (etiqueta)].
inline void __fastcall FkSetMat(void* self, void*, void* mat) {
    Ev& e = PushEv(0x54F610, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(mat);
}
// CEntity::DetachFromRwObject (0x533FB0; thiscall, sin pila): evento [direccion, ECX].
inline void __fastcall FkDetach(void* self, void*) {
    Ev& e = PushEv(0x533FB0, 1);
    e.v[0] = Tag(self);
}
// GetFirstAtomic (0x734820; cdecl, un puntero): evento [direccion, clump (etiqueta)]; devuelve gAtomic (siempre un atomo valido de la arena D).
inline u32 __cdecl FkFirstAtomic(void* clump) {
    Ev& e = PushEv(0x734820, 1);
    e.v[0] = Tag(clump);
    return gAtomic;
}
// CCustomBuildingRenderer::IsCBPCPipelineAttached (0x5D7F40; cdecl, un puntero): evento [direccion, atomo (etiqueta)]; devuelve gCbpc (EAX completo).
inline u32 __cdecl FkCbpc(void* atomic) {
    Ev& e = PushEv(0x5D7F40, 1);
    e.v[0] = Tag(atomic);
    return gCbpc;
}
// COnscreenTimerEntry::Process (0x44CB10; thiscall, sin pila): evento [direccion, ECX (etiqueta)].
inline void __fastcall FkTimerEntry(void* self, void*) {
    Ev& e = PushEv(0x44CB10, 1);
    e.v[0] = Tag(self);
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
    u32 argCount{}, popBytes{}, ecx{}, eax{};
};

inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutWin(u32 seed) { for (u32 i = 0; i < 64; ++i) { const u32 w = WindowWord(seed, i); PutRaw(kScriptSpace + kScriptWin + 4 * i, &w, 4); } }
inline u32 PtrOf(u32 mode, u32 arenaPtr, u32 raw) { return mode == 0 ? 0u : (mode == 1 ? arenaPtr : raw); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx; call.eax = c.eax;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[60]); arenaB.Pattern(a[2], a[61]); arenaC.Pattern(a[2], a[62]); arenaD.Pattern(a[2], a[63]);
    gRet = 0; gDtorWrite = 0; gTaskNew = 0; gPtr = 0; gAtomic = 0; gCbpc = 0;
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    const auto kind = kKind[c.fn];
    arenaA.Put32(o, a[3]);
    switch (kind) {
    case kPoolRef: {
        call.ecx = self;
        const u32 storage = a[5] == 0 ? u32(arenaC.Obj(a[6])) : (a[5] == 1 ? a[7] : 0u);
        arenaA.Put32(o + 0, storage); arenaA.Put32(o + 4, u32(arenaB.Obj(a[8]))); arenaA.Put32(o + 8, a[4]);
        const u32 slot = a[9];
        const u8 bm = u8(a[11]);
        u8 low;
        switch (a[10]) {
        case 0: low = bm; arenaB.Put8(a[8] + slot, bm); break;
        case 1: low = u8(bm ^ a[12]); arenaB.Put8(a[8] + slot, bm); break;
        case 2: low = u8(bm | 0x80); arenaB.Put8(a[8] + slot, low); break;
        default: low = u8(bm ^ 0x80); arenaB.Put8(a[8] + slot, bm); break;
        }
        call.args[0] = (slot << 8) | low; call.argCount = 1;
        break;
    }
    case kTimerEntry: {
        call.ecx = self;
        PutWin(a[13]);
        PutRaw(kScriptSpace + kScriptWin + a[5], &a[6], 4);
        const u32 var = a[4] ? kScriptWin + a[5] : 0u;
        arenaA.Put32(o + 0, var); arenaA.Put8(o + 4, u8(a[11])); arenaA.Put8(o + 0x38, u8(a[12])); arenaA.Put8(o + 0x39, u8(a[8])); arenaA.Put32(o + 0x3C, a[9]);
        PutRaw(kTimeStep, &a[7], 4);
        const u8 ws = u8(a[10]); PutRaw(kWideScreen, &ws, 1);
        break;
    }
    case kTimerDisplay: {
        call.ecx = self;
        PutWin(a[7]);
        const u8 wants = u8(a[4]); PutRaw(kWantsHud, &wants, 1);
        arenaA.Put32(o + 0, a[5] ? kScriptWin + a[6] : 0u);
        for (u32 i = 0; i < 4; ++i) {
            const u32 co = o + 0x40 + 0x44 * i;
            arenaA.Put32(co + 0, a[8 + 5 * i] ? kScriptWin + a[9 + 5 * i] : 0u);
            arenaA.Put32(co + 4, a[11 + 5 * i] ? kScriptWin + a[12 + 5 * i] : 0u);
            arenaA.Put32(co + 0x10, (arenaA.Get32(co + 0x10) & 0x0000FFFFu) | ((a[10 + 5 * i] & 0xFFFFu) << 16));   // el tipo es el word +0x12
            arenaA.Put8(co + 0x3E, u8(a[28 + i]));
        }
        break;
    }
    case kTimerProc: {
        call.ecx = self;
        const u8 mode = u8(a[4]); PutRaw(kReplayMode, &mode, 1);
        arenaA.Put8(o + 0x151, u8(a[5]));
        break;
    }
    case kFade: {
        const u8 p1 = u8(a[4]), p2 = u8(a[5]), jl = u8(a[6]), st = u8(a[9]), fv = u8(a[11]), sp = u8(a[12]);
        PutRaw(kPause1, &p1, 1); PutRaw(kPause2, &p2, 1); PutRaw(kJustLoaded, &jl, 1); PutRaw(kStillFade, &st, 1);
        PutRaw(kTimeMs, &a[7], 4); PutRaw(kFadeTimer, &a[8], 4); PutRaw(kFadeOutTime, &a[10], 4);
        const u8 rgb[4]{u8(a[13]), u8(a[14]), u8(a[15]), fv};
        PutRaw(kFadeRgb, rgb, 4);
        PutRaw(kSplash, &sp, 1); PutRaw(kWidth, &a[16], 4); PutRaw(kHeight, &a[17], 4);
        break;
    }
    case kLlClear: {
        call.ecx = self;
        const u32 nUsed = a[4], nFree = a[5];
        std::vector<u32> used, freed;
        for (u32 i = 0; i < nUsed + nFree; ++i) {
            const u32 node = u32(arenaB.Obj(a[6] + 0x20 * a[8 + i]));
            (i < nUsed ? used : freed).push_back(node);
        }
        auto chain = [&](u32 head, const std::vector<u32>& mids, u32 tail) {
            std::vector<u32> ch{head};
            ch.insert(ch.end(), mids.begin(), mids.end());
            ch.push_back(tail);
            for (size_t i = 0; i < ch.size(); ++i) {
                if (i > 0) memcpy(reinterpret_cast<void*>(uintptr_t(ch[i]) + 4), &ch[i - 1], 4);
                if (i + 1 < ch.size()) memcpy(reinterpret_cast<void*>(uintptr_t(ch[i]) + 8), &ch[i + 1], 4);
            }
        };
        chain(self + 0, used, self + 0xC);
        chain(self + 0x18, freed, self + 0x24);
        break;
    }
    case kObjDef:
    case kHandling:
    case kConst:
    case kFootBody:
        call.ecx = self;
        if (kind == kFootBody) {
            gRet = a[11]; gDtorWrite = a[10];
            arenaA.Put32(o + 0xC, PtrOf(a[4], u32(arenaB.Obj(a[5])), a[6]));
            arenaA.Put32(o + 0x18, PtrOf(a[7], u32(arenaC.Obj(a[8])), a[9]));
        }
        break;
    case kObjDummy: {
        call.ecx = self;
        const u32 d = a[4];
        arenaB.Put32(d + 0x20, (a[5] << 16) | (c.seed & 0xFFFFu));
        arenaB.Put32(d + 0x18, PtrOf(a[6], u32(arenaC.Obj(a[7])), a[8]));
        arenaB.Put32(d + 0x14, PtrOf(a[9], u32(arenaD.Obj(a[10])), a[11]));
        arenaB.Put8(d + 0x2E, u8(a[12])); arenaB.Put8(d + 0x2F, u8(a[13])); arenaB.Put32(d + 0x1C, a[14]);
        arenaA.Put32(o + 0x1C, a[15]);
        if (a[16]) { arenaC.Put8(a[18], u8(a[17])); arenaA.Put32(o + 0x18, u32(arenaC.Obj(a[18]))); } else arenaA.Put32(o + 0x18, 0);
        gPtr = u32(arenaD.Obj(a[19])); gAtomic = u32(arenaD.Obj(a[20])); gCbpc = a[21];
        call.args[0] = u32(arenaB.Obj(d)); call.argCount = 1;
        break;
    }
    case kObjNew:
    case kObjDel: {
        const bool isNew = kind == kObjNew;
        const u32 cap = a[4], po = a[5];
        const u32 storage = a[6] == 0 ? u32(arenaC.Obj(a[7])) : a[8];
        arenaB.Put32(po + 0, storage); arenaB.Put32(po + 4, u32(arenaD.Obj(a[9]))); arenaB.Put32(po + 8, cap); arenaB.Put32(po + 0xC, a[11]);
        const u32 seed = isNew ? a[14] : a[13];
        for (u32 i = 0; i < cap; ++i) arenaD.Put8(a[9] + i, u8(Rand(seed, i)));
        arenaD.Put8(a[9] + cap, 0x80);
        const u32 slot = a[10];
        const u8 st = arenaD.m[kGuard + a[9] + slot];
        const bool free = isNew ? a[15] != 0 : a[12] != 0;
        arenaD.Put8(a[9] + slot, u8(free ? (st | 0x80) : (st & 0x7F)));
        *reinterpret_cast<u32*>(kObjPool) = u32(arenaB.Obj(po));
        if (isNew) { call.args[0] = a[12]; call.args[1] = (slot << 8) | a[13]; call.argCount = 2; call.popBytes = 8; }
        else { call.args[0] = storage + slot * 0x19Cu; call.argCount = 1; call.popBytes = 4; }
        break;
    }
    case kCloneChat:
    case kCloneShove:
    case kCloneFoot: {
        call.ecx = self;
        if (kind == kCloneFoot) {
            arenaA.Put32(o + 0xC, a[4]);
        } else {
            arenaA.Put32(o + 0x38, a[4]); arenaA.Put8(o + 0x58, u8(a[5])); arenaA.Put32(o + 0x3C, a[6]); arenaA.Put8(o + 0x5B, u8(a[7]));
            arenaA.Put32(o + 0x40, a[10]); arenaA.Put32(o + 0x44, a[11]); arenaA.Put32(o + 0x48, a[12]);
            if (kind == kCloneChat) { arenaA.Put8(o + 0x74, u8(a[8])); arenaA.Put8(o + 0x75, u8(a[9])); }
        }
        gTaskNew = a[13] == 0 ? 0u : u32(arenaB.Obj(a[14]));
        gRet = a[15]; gDtorWrite = a[16];
        break;
    }
    case kSdd:
        call.ecx = self;
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kFormation: {
        call.ecx = self;
        arenaC.Put32(a[11], a[12]); arenaC.Put32(a[11] + 4, a[13]);
        gPtr = u32(arenaC.Obj(a[11]));
        call.args = {a[4], PtrOf(a[5], u32(arenaB.Obj(a[6])), a[7]), a[8], a[9], a[10]}; call.argCount = 5;
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
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, c.edx, c.stack, call.popBytes);
        });
        json obs;
        if (fn == 21) {   // la vtable de la DLL (en el ASI) se normaliza a la del EXE
            const u32 expVt = 0x870C3Cu;
            const u32 vt = arenaA.Get32(c.a[0]);
            const bool ok = vt == expVt || (candidate && vt != 0 && ModuleOf(vt) == env.asi);
            obs["vtable_ok"] = ok;
            if (ok) arenaA.Put32(c.a[0], expVt);
        }
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kPoolRef: case kObjDef: case kObjDummy: case kObjNew: case kCloneChat: case kCloneShove: case kCloneFoot: case kSdd: case kFormation: obs["ret"] = Tag(uintptr_t(eax)); break;   // puntero como etiqueta de arena
        case kConst: obs["ret"] = eax; break;                                                                                                                                               // valor completo de EAX
        default: obs["ret"] = nullptr; break;                                                                                                                                               // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN185A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN185A_CASES", buffer, sizeof(buffer)) > 0) {
        const unsigned v = unsigned(std::atoi(buffer));
        if (v >= kFamilies && v <= kCases && v % kFamilies == 0) return v;
    }
    return kCases;
}
// Funcion unica opcional (variable FN185A_ONLY = indice): para iterar rapido; el analisis exige todas, asi que una corrida parcial no se acredita.
inline int OnlyFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN185A_ONLY", buffer, sizeof(buffer)) > 0) return std::atoi(buffer);
    return -1;
}
inline void Run(Env& env, bool ready, const json& registered, const json& bodies, const json& collaborators) {
    for (unsigned f = 0; f < kFunctions; ++f) {
        const uintptr_t body = env.route[f] ? env.route[f]->Body() : 0;
        ready &= body && ModuleOf(body) == env.asi;
    }
    json watched = json::array(); for (const auto& w : env.watches) watched.push_back({{"address", w.address}, {"bytes", HexBytes(reinterpret_cast<void*>(w.address), w.saved.size())}});
    SaveFP(savedFP.data()); savedMXCSR = ReadMXCSR(); savedFPActive = true;
    const unsigned nCases = CasesPerFunction();
    const int only = OnlyFunction();
    json cases = json::object(); for (const char* nm : kFn) cases[nm] = nCases;
    Write({{"event", "begin"}, {"registered_hook", ready}, {"registered", registered}, {"functions", kFunctions}, {"pairs", kFunctions * nCases}, {"cases", cases},
        {"direct_functions", json::array()}, {"compare_dead_x87_registers", kCompareDeadX87Registers}, {"compare_volatile_registers", kCompareVolatileRegisters},
        {"x87_comparison", "fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"}, {"collaborators_stubbed", true}, {"rng_native", false},
        {"collaborators", collaborators}, {"watched_entries", watched}, {"bodies", bodies}, {"asi_module", env.asi}, {"original_module", env.game},
        {"initial_fp", HexBytes(savedFP.data(), 108)}, {"initial_mxcsr", savedMXCSR}, {"script_head", HexBytes(reinterpret_cast<const void*>(uintptr_t(kScriptSpace)), 8)}, {"script_vars_size", CTheScripts::GetSizeOfVariableSpace()}, {"only", only}});
    Totals totals;
    if (ready) {
        gBenchThread = GetCurrentThreadId();
        for (unsigned f = 0; f < kFunctions; ++f) {
            if (only >= 0 && int(f) != only) continue;
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
    if (!std::filesystem::exists(dir / "fn185a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn185a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn185a"); }
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
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    // Constructores base y rutinas que corren REALES con el hook apagado (el EXE corre sus bytes; la DLL escribe los mismos campos en linea)
    auto R = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;
        return it->second;
    };
    env.reals[17] = {R(0x684280)};
    env.reals[21] = {R(0x61A3B0)};
    env.stubs[1] = {F(0x506EA0, &FkAudio)};
    env.stubs[3] = {Rt(1, &FkTimerEntry)};
    env.stubs[4] = {F(0x50BF00, &FkSetFadeColour), F(0x50AC20, &FkFade), F(0x50B5D0, &FkCamNoArg<0x50B5D0>), F(0x50B6D0, &FkCamNoArg<0x50B6D0>), F(0x727B60, &FkDrawRect)};
    env.stubs[6] = {F(0x542260, &FkPhys), F(0x59F840, &FkInit)};
    env.stubs[7] = {F(0x542260, &FkPhys), FV(0x533700, &FkSetModelIdx), F(0x533ED0, &FkAttach), F(0x54F560, &FkAllocMat), F(0x54F1B0, &FkUpdateMat), F(0x54F610, &FkSetMat), F(0x533FB0, &FkDetach),
                    F(0x59F840, &FkInit), F(0x734820, &FkFirstAtomic), F(0x5D7F40, &FkCbpc)};
    env.stubs[10] = {F(0x5BD830, &FkLoadHandling)};
    env.stubs[13] = {F(0x61A5A0, &FkTaskNew), F(0x684290, &FkCtorChat)};
    env.stubs[14] = {F(0x61A5A0, &FkTaskNew), F(0x6846F0, &FkCtorShove)};
    env.stubs[15] = {F(0x683A40, &FkBody<0x683A40>), F(0x61A5B0, &FkDelete)};
    env.stubs[16] = {F(0x684200, &FkBody<0x684200>), F(0x61A5B0, &FkDelete)};
    env.stubs[17] = {F(0x683A40, &FkBody<0x683A40>), F(0x61A5B0, &FkDelete)};
    env.stubs[18] = {F(0x684320, &FkBody<0x684320>), F(0x61A5B0, &FkDelete)};
    env.stubs[19] = {F(0x684750, &FkBody<0x684750>), F(0x61A5B0, &FkDelete)};
    env.stubs[20] = {F(0x61A5A0, &FkTaskNew), F(0x694E20, &FkCtorFoot)};
    env.stubs[21] = {F(0x611F20, &FkFormOff), F(0x571B70, &FkRef)};
    env.stubs[22] = {F(0x694A40, &FkBody<0x694A40>), F(0x61A5B0, &FkDelete)};
    env.stubs[23] = {F(0x694B30, &FkBody<0x694B30>), F(0x61A5B0, &FkDelete)};
    env.stubs[24] = {F(0x694C10, &FkBody<0x694C10>), F(0x61A5B0, &FkDelete)};
    env.stubs[25] = {F(0x571A00, &FkCleanup), F(0x41B7D0, &FkRouteDelete), F(0x61A3D0, &FkBody<0x61A3D0>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {0u, 2u, 5u, 8u, 9u, 11u, 12u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[2] = "toda la funcion (sin colaboradores; el EXE lleva el reloj y los contadores en linea, la DLL llama a ProcessForDisplayClock / ProcessForDisplayCounter reales; sprintf real en ambos lados)";
    dllReal[8] = "toda la funcion (CPool::NewAt de la DLL frente al 0x59F610 del EXE, sobre un pool sintetico)";
    dllReal[9] = "toda la funcion (CPool::Delete de la DLL frente a las instrucciones del EXE, sobre un pool sintetico)";
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
    // Memoria global del juego tocada; se restaura tras cada lado
    regions.clear();
    for (auto& v : regionsOf) v.clear();
    auto reg = [&](u32 at, size_t n, std::initializer_list<unsigned> fns) { regions.emplace_back(at, n); for (unsigned f : fns) regionsOf[f].push_back(regions.size() - 1); };
    reg(kScriptSpace + kScriptWin, kScriptWinSize, {1, 2}); reg(kTimeStep, 4, {1}); reg(kWideScreen, 1, {1});
    reg(kWantsHud, 1, {2});
    reg(kReplayMode, 1, {3});
    reg(kPause1 - 1, 2, {4}); reg(kTimeMs, 4, {4}); reg(kJustLoaded, 8, {4}); reg(kFadeOutTime, 4, {4}); reg(kFadeRgb, 4, {4}); reg(kSplash, 1, {4}); reg(kWidth, 8, {4});
    reg(kObjPool, 4, {8, 9});
    Run(env, ready, registered, bodies, collaborators);
}
}
