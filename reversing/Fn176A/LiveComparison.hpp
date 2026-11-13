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

// Banco par a par Fn176A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 26 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall) y V (RH_ScopedVMTInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cuatro arenas propias (A, B, C y D con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair): CPed::IsPlayer 0x5DF8F0, cuerpos de destructor
// 0x4AD960, 0x647080, 0x653C90, 0x656B00, 0x691A50, 0x692030 y 0x691630, ~CTask 0x61A3A0, constructor base de CTaskSimple 0x61A390, CTask::operator new 0x61A5A0 y delete 0x61A5B0, constructor de CTaskInteriorSitInChair 0x675C30,
// CEntity::RegisterReference 0x571B70 y CleanUpOldReference 0x571A00, CAnimBlendAssociation::SetDeleteCallback 0x4CEBC0 y SetFinishCallback 0x4CEBE0, CAnimManager::RemoveAnimBlockRef 0x4D3FD0, IKChainManager_c::IsLooking
// 0x6181A0 y AbortLookAt 0x618280, CCarEnterExit::GetPositionToOpenCarDoor 0x64E740, FindPlayerVehicle 0x56E0D0, CPad::GetPad 0x53FB70 y StartShake 0x53F920, CAESound::Initialise 0x4EFE50, SetPosition 0x4EF880, StopSound 0x4EF1C0
// y destructor 0x4EF660, CAESoundManager::RequestNewSound 0x4EFB10, CAEWaterCannonAudioEntity::Terminate 0x5030A0 y cuatro metodos de CControllerConfigManager (todos con hook). Memoria global del juego tocada (restaurada tras
// cada lado): el puntero al pool de eventos 0xB744AC, CPad::padNumber 0xB73400, m_bMenuActive 0xBA67A4, los botones 0xB70CF4, la camara activa 0xB6F081 y los modos de camara 0xB6F1A8.
// Se observan: los bytes cambiados de las cuatro arenas (con las direcciones reales de las arenas, que el banco anota en la entrada de cada caso), los cambios de la memoria global, los eventos de los falsos (con sus
// argumentos), el valor devuelto (punteros como etiqueta de arena; EAX completo en constantes y en las funciones de pad; solo AL en bool) y el estado x87 (siempre vacio). EAX/ECX/EDX tras la llamada quedan en
// original_residue / candidate_residue (informativos).

namespace F176ALive {
using namespace F176AFix;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F176AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    // Version compacta (FurnitureManager_c::Constructor escribe unos 4200 bytes): numero de bytes cambiados y FNV-1a de la lista (desplazamiento como int32 little endian y byte nuevo)
    json ChangesCompact() const {
        u32 h = 2166136261u, n = 0;
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) {
                const i32 off = i32(int64(i) - int64(kGuard));
                const u8* b = reinterpret_cast<const u8*>(&off);
                for (int k = 0; k < 4; ++k) { h ^= b[k]; h *= 16777619u; }
                h ^= m[i]; h *= 16777619u; ++n;
            }
        return {{"n", n}, {"h", h}};
    }
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
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo, de constructor y de IsPlayer / IsLooking / RequestNewSound
inline u32 gDtorWrite{};             // dword que el falso del cuerpo / constructor deja en [this]
inline u32 gTaskNew{};               // lo que devuelve CTask::operator new
inline u32 gPlayerVeh{}, gPad{};     // FindPlayerVehicle y CPad::GetPad
inline std::array<u32, 3> gDoor{};   // lo que escribe el falso de GetPositionToOpenCarDoor
inline u32 gDefaultCb{}, gDefaultCbBody{};   // CDefaultAnimCallback::DefaultAnimCB de la DLL (y su cuerpo): se anota como la del EXE (0x4D1390)
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Cuerpo del destructor / constructor base (thiscall, sin argumentos de pila): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// Igual pero SIN escribir nada (Terminate 0x5030A0 y el destructor de CAESound 0x4EF660): evento [direccion, ECX (etiqueta), dword que tenia [ECX] al entrar].
template<u32 ADDR> inline u32 __fastcall FkRec(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    return 0;
}
// CEntity::CleanUpOldReference (0x571A00) / RegisterReference (0x571B70) (thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (etiqueta), argumento (etiqueta)]. No escribe nada.
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* ref) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = Tag(ref);
}
// CTask::operator new (0x61A5A0; cdecl, el tamano): evento [direccion, tamano]; devuelve gTaskNew (un objeto de la arena B o nulo).
inline void* __cdecl FkTaskNew(u32 size) {
    Ev& e = PushEv(0x61A5A0, 1);
    e.v[0] = size;
    return reinterpret_cast<void*>(uintptr_t(gTaskNew));
}
// Constructor de CTaskInteriorSitInChair (0x675C30; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), a, b, c]; deja gDtorWrite en [ECX]; devuelve gRet.
inline u32 __fastcall FkCtor3(void* self, void*, u32 a, u32 b, u32 c) {
    Ev& e = PushEv(0x675C30, 4);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c;
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// CAnimManager::RemoveAnimBlockRef (0x4D3FD0; cdecl, el indice del bloque): evento [direccion, indice].
inline void __cdecl FkRemoveRef(u32 index) {
    Ev& e = PushEv(0x4D3FD0, 1);
    e.v[0] = index;
}
// CAnimBlendAssociation::SetDeleteCallback (0x4CEBC0) / SetFinishCallback (0x4CEBE0) (thiscall, dos dwords de pila, `ret 8`): evento [direccion, ECX (etiqueta), funcion (la DefaultAnimCB de la DLL se anota como 0x4D1390), datos].
template<u32 ADDR> inline void __fastcall FkSetCb(void* self, void*, u32 callback, u32 data) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self);
    e.v[1] = (callback == gDefaultCb || callback == gDefaultCbBody) ? 0x4D1390u : callback;
    e.v[2] = data;
}
// CPed::IsPlayer (0x5DF8F0; thiscall, sin pila, `ret`): evento [direccion, ECX (crudo)]; devuelve gRet (el llamador solo mira AL).
inline u32 __fastcall FkIsPlayer(void* self, void*) {
    Ev& e = PushEv(0x5DF8F0, 1);
    e.v[0] = Tag(self);
    return gRet;
}
// CCarEnterExit::GetPositionToOpenCarDoor (0x64E740; cdecl, salida oculta, vehiculo, puerta): evento [direccion, salida (etiqueta), vehiculo (etiqueta), puerta]; escribe gDoor en la salida y la devuelve.
inline u32* __cdecl FkDoorPos(u32* out, void* veh, u32 door) {
    Ev& e = PushEv(0x64E740, 3);
    e.v[0] = Tag(out); e.v[1] = Tag(veh); e.v[2] = door;
    out[0] = gDoor[0]; out[1] = gDoor[1]; out[2] = gDoor[2];
    return out;
}
// FindPlayerVehicle (0x56E0D0; cdecl, jugador y bandera): evento [direccion, jugador, bandera]; devuelve gPlayerVeh.
inline u32 __cdecl FkFindPlayerVeh(u32 player, u32 flag) {
    Ev& e = PushEv(0x56E0D0, 2);
    e.v[0] = player; e.v[1] = flag;
    return gPlayerVeh;
}
// CPad::GetPad (0x53FB70; cdecl, un indice): evento [direccion, indice]; devuelve gPad.
inline u32 __cdecl FkGetPad(u32 index) {
    Ev& e = PushEv(0x53FB70, 1);
    e.v[0] = index;
    return gPad;
}
// CPad::StartShake (0x53F920; thiscall, tres dwords de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), tiempo, BYTE bajo de la frecuencia (el EXE deja basura en los bytes altos), tercer argumento].
inline void __fastcall FkStartShake(void* self, void*, u32 time, u32 freq, u32 third) {
    Ev& e = PushEv(0x53F920, 4);
    e.v[0] = Tag(self); e.v[1] = time; e.v[2] = freq & 0xFFu; e.v[3] = third;
}
// IKChainManager_c::IsLooking (0x6181A0; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (crudo), ped (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkIsLooking(void* self, void*, void* ped) {
    Ev& e = PushEv(0x6181A0, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ped);
    return gRet;
}
// IKChainManager_c::AbortLookAt (0x618280; thiscall, ped y tiempo, `ret 8`): evento [direccion, ECX (crudo), ped (etiqueta), tiempo].
inline void __fastcall FkAbortLookAt(void* self, void*, void* ped, u32 time) {
    Ev& e = PushEv(0x618280, 3);
    e.v[0] = Tag(self); e.v[1] = Tag(ped); e.v[2] = time;
}
// CAESound::Initialise (0x4EFE50; thiscall, catorce dwords de pila, `ret 38h`): evento [direccion, ECX (etiqueta), los 14 dwords]. No escribe nada.
inline void __fastcall FkInit14(void* self, void*, u32 a0, u32 a1, u32 a2, u32 a3, u32 a4, u32 a5, u32 a6, u32 a7, u32 a8, u32 a9, u32 a10, u32 a11, u32 a12, u32 a13) {
    Ev& e = PushEv(0x4EFE50, 15);
    e.v[0] = Tag(self);
    const u32 v[14]{a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13};
    for (u32 i = 0; i < 14; ++i) e.v[1 + i] = (i == 2) ? Tag(uintptr_t(v[i])) : v[i];   // el tercero es la entidad (etiqueta de arena)
}
// CAESound::SetPosition (0x4EF880; thiscall, tres floats de pila, `ret 0Ch`): evento [direccion, ECX (etiqueta), x, y, z].
inline void __fastcall FkSetPos(void* self, void*, u32 x, u32 y, u32 z) {
    Ev& e = PushEv(0x4EF880, 4);
    e.v[0] = Tag(self); e.v[1] = x; e.v[2] = y; e.v[3] = z;
}
// CAESound::StopSound (0x4EF1C0; thiscall, sin pila, `ret`): evento [direccion, ECX (etiqueta)].
inline void __fastcall FkStop(void* self, void*) {
    Ev& e = PushEv(0x4EF1C0, 1);
    e.v[0] = Tag(self);
}
// CAESoundManager::RequestNewSound (0x4EFB10; thiscall, un puntero de pila, `ret 4`): evento [direccion, ECX (crudo), sonido (etiqueta)]; devuelve gRet.
inline u32 __fastcall FkRequest(void* self, void*, void* sound) {
    Ev& e = PushEv(0x4EFB10, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(sound);
    return gRet;
}
// CControllerConfigManager::UpdateJoyButtonState (0x52F510; thiscall, un dword, `ret 4`): evento [direccion, ECX (crudo), pad].
inline void __fastcall FkUpdState(void* self, void*, u32 pad) {
    Ev& e = PushEv(0x52F510, 2);
    e.v[0] = Tag(self); e.v[1] = pad;
}
// Los cuatro metodos de botones de CControllerConfigManager (thiscall, dos dwords, `ret 8`): evento [direccion, ECX (crudo), boton, segundo argumento].
template<u32 ADDR> inline void __fastcall FkBtn(void* self, void*, u32 button, u32 second) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self); e.v[1] = button; e.v[2] = second;
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
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }
inline size_t Off(u32 base, i32 delta) { return size_t(int64(base) + delta); }

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx; call.eax = c.eax;
    const auto& a = c.a;
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    gRet = 0; gDtorWrite = 0; gTaskNew = 0; gPlayerVeh = 0; gPad = 0; gDoor = {};
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    call.ecx = self;
    arenaA.Put32(o, a[3]);
    switch (kKind[c.fn]) {
    case kBleed:
        arenaA.Put32(o + 0xC, a[4]);
        gRet = a[6];
        call.args[0] = a[5]; call.argCount = 1;
        break;
    case kEvtSdd: {
        const i32 delta = i32(a[5]);
        const i32 idx = delta / 0x44;   // truncado hacia cero, como el EXE
        arenaB.Put32(a[4] + 0, self - u32(delta)); arenaB.Put32(a[4] + 4, u32(arenaD.Obj(a[6]))); arenaB.Put32(a[4] + 8, a[12]); arenaB.Put32(a[4] + 0xC, a[10]);
        arenaD.Put8(Off(a[6], idx), u8(a[11]));
        PutRaw32(kEventPoolGlobal, u32(arenaB.Obj(a[4])));
        gRet = a[8]; gDtorWrite = a[9];
        call.args[0] = a[7]; call.argCount = 1;
        break;
    }
    case kWater: {
        const i32 id = i32(a[5]);
        const size_t slot = Off(o + 0x90, 4 * id);
        const u32 sound = a[14] == 0 ? 0u : u32(arenaB.Obj(a[15]));
        arenaA.Put32(slot, sound);
        if (sound) { arenaB.Put32(a[15] + 0x14, a[17]); arenaB.Put32(a[15] + 0x1C, a[18]); }
        gRet = a[16];
        call.args = {a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11], a[12], a[13]}; call.argCount = 10;
        break;
    }
    case kFurnCtor:
    case kEmpty:
    case kConst:
    case kWcDtor:
        break;
    case kTarget: {
        const u32 task = u32(arenaB.Obj(a[4]));
        arenaA.Put32(o + 0xC, task);
        const u32 veh = u32(arenaC.Obj(a[5]));
        arenaB.Put32(a[4] + 8, veh); arenaB.Put32(a[4] + 0xC, a[6]);
        arenaC.Put32(a[5] + 4, a[7]); arenaC.Put32(a[5] + 8, a[8]); arenaC.Put32(a[5] + 0xC, a[9]);
        const u32 matrix = a[10] == 0 ? 0u : u32(arenaD.Obj(a[11]));
        arenaC.Put32(a[5] + 0x14, matrix);
        if (matrix) { arenaD.Put32(a[11] + 0x30, a[12]); arenaD.Put32(a[11] + 0x34, a[13]); arenaD.Put32(a[11] + 0x38, a[14]); }
        gDoor = {a[15], a[16], a[17]};
        call.args[0] = u32(arenaB.Obj(a[18])); call.argCount = 1;
        break;
    }
    case kPassCtor:
        gRet = a[8]; gDtorWrite = a[7];
        call.args = {a[4], a[5], a[6]}; call.argCount = 3;
        break;
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kHitKills:
        arenaA.Put32(o + 0x10, a[4]);
        gRet = a[6];
        call.args[0] = a[5]; call.argCount = 1;
        break;
    case kShake: {
        const u32 car = a[4] == 0 ? u32(arenaB.Obj(a[5])) : a[6];
        arenaA.Put32(o + 8, car);
        if (a[4] == 0) { arenaB.Put32(a[5] + 0x44, a[9]); arenaB.Put32(a[5] + 0x48, a[10]); arenaB.Put32(a[5] + 0x4C, a[11]); arenaB.Put32(a[5] + 0x8C, a[12]); }
        gPlayerVeh = a[7] == 0 ? car : (a[7] == 1 ? 0u : a[8]);
        gPad = a[13] == 0 ? u32(arenaC.Obj(a[14])) : a[15];
        break;
    }
    case kSitDtor:
        arenaA.Put32(o + 0x14, a[4] == 0 ? 0u : a[5]);
        gRet = a[6]; gDtorWrite = a[7];
        break;
    case kSitClone:
        arenaA.Put32(o + 8, a[4]); arenaA.Put32(o + 0xC, a[5]); arenaA.Put32(o + 0x10, a[6]);
        gTaskNew = a[7] == 0 ? 0u : u32(arenaB.Obj(a[8]));
        gRet = a[9]; gDtorWrite = a[10];
        break;
    case kSitAbort:
        arenaA.Put32(o + 0x14, a[4] == 0 ? 0u : u32(arenaB.Obj(a[5])));
        arenaA.Put8(o + 0x2A, u8(a[10]));
        gRet = a[9];
        call.args = {a[7], a[6], a[8]}; call.argCount = 3;
        break;
    case kHoldDtor: {
        arenaA.Put8(o + 0x35, u8(a[4]));
        const u32 entity = a[5] == 0 ? 0u : u32(arenaB.Obj(a[6]));
        arenaA.Put32(o + 8, entity);
        if (entity) arenaB.Put32(a[6] + 0x1C, a[7]);
        arenaA.Put32(o + 0x38, a[8]); arenaA.Put32(o + 0x2C, a[9]);
        gRet = a[10]; gDtorWrite = a[11];
        break;
    }
    case kPadDown:
    case kPadUp: {
        arenaB.Put32(a[15], a[4]);
        call.eax = u32(arenaB.Obj(a[15]));
        const u8 pad = u8(a[5]), menu = u8(a[6]), cam = u8(a[11]);
        PutRaw(kPadNumber, &pad, 1); PutRaw(kMenuActive, &menu, 1);
        PutRaw(kButtonStates, &a[7], 16);
        PutRaw(kActiveCam, &cam, 1);
        for (u32 i = 0; i < kCamCount; ++i) { const u16 mode = u16(a[12 + i]); PutRaw(kCamModes + i * kCamStride, &mode, 2); }
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
        obs["changes"] = {{"A", kKind[fn] == kFurnCtor ? arenaA.ChangesCompact() : arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kKind[fn]) {
        case kEvtSdd: case kSdd: case kPassCtor: case kFurnCtor: case kSitClone: case kTarget: obs["ret"] = Tag(uintptr_t(eax)); break;   // this / el objeto nuevo / la salida como etiqueta de arena
        case kConst: case kPadDown: case kPadUp: obs["ret"] = eax; break;                                                                    // valor completo de EAX
        case kBleed: case kHitKills: case kSitAbort: obs["ret"] = eax & 0xFFu; break;                                                        // bool: solo AL
        default: obs["ret"] = nullptr; break;                                                                                                // void
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}

// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN176A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN176A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn176a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn176a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn176a"); }
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
    gDefaultCb = std::bit_cast<u32>(&CDefaultAnimCallback::DefaultAnimCB); gDefaultCbBody = u32(Body(gDefaultCb));
    auto S = [](const Pair& p, void* fake) { return StubSpec{p, fake}; };
    std::map<uintptr_t, Pair> collabCache;   // una sola ruta por colaborador aunque lo usen varias funciones
    // Colaborador con hook RH en la DLL (apagado durante el banco): el mismo falso en el EXE y en el cuerpo de la DLL
    auto F = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked || !it->second.dll) ready = false;   // se esperaba hook RH con cuerpo en el ASI
        return S(it->second, reinterpret_cast<void*>(fake));
    };
    // Colaborador que es OTRA funcion probada del lote (con hook RH): se reutiliza su ruta (hook apagado); el falso va en su direccion del EXE y en su cuerpo de la DLL.
    auto Rt = [&](unsigned idx, auto* fake) {
        Pair p{kEntries[idx], Body(env.route[idx]->Body()), true};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        hooksFound.push_back({{"exe", p.exe}, {"class", ownHooks[kFn[idx]]["class"]}, {"name", ownHooks[kFn[idx]]["name"]}, {"tested_function", true}});
        return S(p, reinterpret_cast<void*>(fake));
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    env.stubs[0] = {F(0x5DF8F0, &FkIsPlayer)};
    env.stubs[1] = {F(0x4AD960, &FkBody<0x4AD960>)};
    env.stubs[2] = {F(0x4EFE50, &FkInit14), F(0x4EF880, &FkSetPos), F(0x4EF1C0, &FkStop), F(0x4EFB10, &FkRequest)};
    env.stubs[6] = {F(0x64E740, &FkDoorPos)};
    env.stubs[7] = {F(0x61A390, &FkBody<0x61A390>), F(0x571B70, &FkRef<0x571B70>)};
    env.stubs[8] = {F(0x647080, &FkBody<0x647080>), Del()};
    env.stubs[9] = {F(0x5DF8F0, &FkIsPlayer)};
    env.stubs[10] = {F(0x653C90, &FkBody<0x653C90>), Del()};
    env.stubs[11] = {F(0x56E0D0, &FkFindPlayerVeh), F(0x53FB70, &FkGetPad), F(0x53F920, &FkStartShake)};
    env.stubs[12] = {F(0x656B00, &FkBody<0x656B00>), Del()};
    env.stubs[14] = {F(0x4CEBC0, &FkSetCb<0x4CEBC0>), F(0x61A3A0, &FkBody<0x61A3A0>)};
    env.stubs[15] = {F(0x61A5A0, &FkTaskNew), F(0x675C30, &FkCtor3)};
    env.stubs[16] = {F(0x6181A0, &FkIsLooking), F(0x618280, &FkAbortLookAt), F(0x4CEBC0, &FkSetCb<0x4CEBC0>)};
    env.stubs[18] = {F(0x571A00, &FkRef<0x571A00>), F(0x4CEBE0, &FkSetCb<0x4CEBE0>), F(0x4D3FD0, &FkRemoveRef), F(0x61A3A0, &FkBody<0x61A3A0>)};
    env.stubs[19] = {Rt(18, &FkBody<0x691630>), Del()};
    env.stubs[20] = {Rt(18, &FkBody<0x691630>), Del()};
    env.stubs[21] = {F(0x691A50, &FkBody<0x691A50>), Del()};
    env.stubs[22] = {F(0x692030, &FkBody<0x692030>), Del()};
    env.stubs[23] = {F(0x5030A0, &FkRec<0x5030A0>), F(0x4EF660, &FkRec<0x4EF660>)};
    env.stubs[24] = {F(0x53FB70, &FkGetPad), F(0x52F510, &FkUpdState), F(0x52DAB0, &FkBtn<0x52DAB0>), F(0x530ED0, &FkBtn<0x530ED0>)};
    env.stubs[25] = {F(0x53FB70, &FkGetPad), F(0x52F510, &FkUpdState), F(0x52DC20, &FkBtn<0x52DC20>), F(0x531070, &FkBtn<0x531070>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL)";
    for (unsigned f : {3u, 4u, 5u, 13u, 17u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[1] = "toda la funcion salvo el cuerpo del destructor (falseado); el borrado de la ranura del pool (CEvent::operator delete) corre real sobre un pool sintetico";
    dllReal[3] = "toda la funcion (el EXE llama a los iteradores de vectores del CRT, que corren reales)";
    dllReal[4] = "toda la funcion (el EXE llama a los iteradores de vectores del CRT, que corren reales)";
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
    regions.emplace_back(kEventPoolGlobal, 4); regionsOf[1] = {regions.size() - 1};
    const size_t first = regions.size();
    regions.emplace_back(kPadNumber, 1); regions.emplace_back(kMenuActive, 1); regions.emplace_back(kButtonStates, 16); regions.emplace_back(kActiveCam, 1);
    regions.emplace_back(kCamModes, (kCamCount - 1) * kCamStride + 2);
    for (size_t i = first; i < regions.size(); ++i) { regionsOf[24].push_back(i); regionsOf[25].push_back(i); }
    Run(env, ready, registered, bodies, collaborators);
}
}
