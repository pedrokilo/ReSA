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
#include "TaskComplexSeekEntity.h"

// Banco par a par Fn194A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// 33 funciones del EXE (ver Fixtures.hpp), cada una con 16384 pares EXE/DLL. Hook S (RH_ScopedInstall / RH_ScopedNamedGlobalInstall / RH_ScopedNamedInstall). El banco invoca la direccion del EXE con objetos sinteticos
// en cinco arenas propias (A, B, C y D de 4 KB y la grande E con guardas de 0x40). Falsos de los colaboradores: el mismo falso en la direccion del EXE y en el cuerpo de la DLL (MakeCollab / Pair) para los que tienen hook
// y la DLL llama por su funcion: CTask::operator new 0x61A5A0 y delete 0x61A5B0, constructor de CTaskSimpleStandStill 0x62F310, CPed::IsAlive 0x5E0170, CTaskTimer::IsOutOfTime 0x420E30, FindTaskByType 0x600EE0,
// CGeneral 0x53CBE0 / 0x53CB50, CEventGroup::Add 0x4AB420, RegisterReference 0x571B70 / CleanUpOldReference 0x571A00, constructores de las tareas de paseo 0x460C80 / 0x658770 / 0x48E610 / 0x672690 / 0x48E4F0, cuerpos
// de destructor de model infos (0x4C5590, 0x4C55D0, 0x4C5620, 0x4C5680, 0x4C56E0, 0x4C5750, 0x4C5790, 0x4C62F0) y de tareas (0x669D20, 0x66CBB0), CColModel 0x40F700 / 0x40FC40 y los de CFileMgr (0x538900, 0x538910,
// 0x538950, 0x538970, 0x5389D0). Solo en la direccion del EXE (la DLL los llama por ella): operator new 0x82119A, operator delete 0x8214BD, 0x59C890 (matriz por vector) y rand 0x821B1E (en la DLL, la funcion propia
// CGeneral::GetRandomNumber). Corren REALES con su hook apagado: el constructor de elemento de cada almacen (y 0x4C4A60), 0x40FE60 (resta de vectores), 0x420A10 (CPathNode::GetPosition) y 0x407180 (entero aleatorio).
// Memoria global del juego tocada (restaurada tras cada lado): el reloj 0xB7CB84 y la tabla de areas de nodos 0x96F854 (72 punteros). Se observan: los bytes cambiados de las arenas (con las direcciones reales de las arenas,
// que el banco anota en la entrada de cada caso; de la grande E solo un resumen), los cambios de la memoria global, los eventos de los falsos (con sus argumentos), el valor devuelto y el estado x87.

namespace F194ALive {
using namespace F194AFix;
using SeekFixed = CTaskComplexSeekEntity<CEntitySeekPosCalculatorFixedPos>;
inline constexpr std::array<uintptr_t, kFunctions> kEntries = [] { std::array<uintptr_t, kFunctions> a{}; for (size_t i = 0; i < kFunctions; ++i) a[i] = kEntry[i]; return a; }();

// ================================================================================================ arenas (alineadas a 4096)
template<size_t WIN> struct Arena {
    static constexpr size_t N = F194AFix::Arena(WIN);
    alignas(4096) std::array<uint8, N> m{};
    alignas(4096) std::array<uint8, N> r{};
    uintptr_t Obj(size_t off = 0) const { return uintptr_t(m.data()) + kGuard + off; }
    void Pattern(u32 kind, u32 seed) { FillKind(m.data(), N, kind, seed); }
    void Snap() { r = m; }
    void Put32(size_t off, u32 v) { memcpy(m.data() + kGuard + off, &v, 4); }
    void Put8(size_t off, u8 v) { m[kGuard + off] = v; }
    void Put16(size_t off, u16 v) { memcpy(m.data() + kGuard + off, &v, 2); }
    // Cambios respecto a la imagen de referencia: [desplazamiento respecto a la ventana, byte nuevo]
    json Changes() const {
        json out = json::array();
        for (size_t i = 0; i < N; ++i)
            if (m[i] != r[i]) out.push_back(json::array({int64(i) - int64(kGuard), m[i]}));
        return out;
    }
};
inline Arena<kWin> arenaA, arenaB, arenaC, arenaD;
inline Arena<kBigWin> arenaE;

// ================================================================================================ falsos de los colaboradores
inline u32 gRet{};                   // EAX que devuelven los falsos de cuerpo y de constructor
inline u32 gDtorWrite{};             // dword que el falso del cuerpo deja en [this]
inline u32 gNew{};                   // lo que devuelven los falsos de operator new
inline u32 gRand{};                  // lo que devuelve rand()
inline u32 gAlive{}, gOut{}, gTask{}, gRadian{}, gLimit{};   // ControlSubTask
inline std::array<u32, 3> gMul{};
inline u32 gHandle{}, gRead{};       // CFileMgr
inline std::array<u32, 4> gMarker{};
inline DWORD gBenchThread{};

// operator delete / liberacion (cdecl, un argumento): evento [direccion del EXE, argumento (etiqueta), dword de [argumento] al entrar]. No libera nada. El argumento es siempre una direccion de arena.
template<u32 ADDR> inline void __cdecl FkDelete(void* p) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(p);
    e.v[1] = BitsOf(p);
}
// Igual sin leer la memoria del argumento (puede ser crudo): evento [direccion, argumento (etiqueta)].
template<u32 ADDR> inline void __cdecl FkDeleteRaw(void* p) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(p);
}
// Cuerpo del destructor (thiscall, sin argumentos de pila): evento [direccion del EXE, ECX (etiqueta de arena), dword que tenia [ECX] al entrar]; deja gDtorWrite en [ECX]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkBody(void* self, void*) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self);
    e.v[1] = BitsOf(self);
    memcpy(self, &gDtorWrite, 4);
    return gRet;
}
// thiscall sin argumentos que no toca memoria: evento [direccion, ECX (etiqueta)].
template<u32 ADDR> inline u32 __fastcall FkThis(void* self, void*) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = Tag(self);
    return 0;
}
// CTask::operator new (0x61A5A0) / operator new del CRT (0x82119A): cdecl, el tamano; evento [direccion, tamano]; devuelve gNew.
template<u32 ADDR> inline u32 __cdecl FkNew(u32 size) {
    Ev& e = PushEv(ADDR, 1);
    e.v[0] = size;
    return gNew;
}
// Constructor de CTaskSimpleStandStill (0x62F310; thiscall, cuatro argumentos de pila, `ret 10h`): evento [direccion, ECX (etiqueta), a, b, c, d]; devuelve gRet.
inline u32 __fastcall FkStand(void* self, void*, u32 a, u32 b, u32 c, u32 d) {
    Ev& e = PushEv(0x62F310, 5);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c; e.v[4] = d;
    return gRet;
}
// Constructores de las tareas de paseo: Cop (0x460C80; dos argumentos, `ret 8`) y los demas (tres argumentos, `ret 0Ch`): evento [direccion, ECX (etiqueta), argumentos]; devuelve gRet.
template<u32 ADDR> inline u32 __fastcall FkCtor2(void* self, void*, u32 a, u32 b) {
    Ev& e = PushEv(ADDR, 3);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b;
    return gRet;
}
template<u32 ADDR> inline u32 __fastcall FkCtor3(void* self, void*, u32 a, u32 b, u32 c) {
    Ev& e = PushEv(ADDR, 4);
    e.v[0] = Tag(self); e.v[1] = a; e.v[2] = b; e.v[3] = c;
    return gRet;
}
// rand() del CRT del EXE (0x821B1E) y CGeneral::GetRandomNumber de la DLL: evento [direccion]; devuelve el valor del caso (el EXE enmascara con 0xFFFF; la DLL usa AX).
inline u32 __cdecl FkRand() { PushEv(0x821B1E, 0); return gRand; }
// CPed::IsAlive (0x5E0170) y CTaskTimer::IsOutOfTime (0x420E30): thiscall sin argumentos; evento [direccion, ECX (etiqueta)]; devuelven el EAX del caso (solo se mira AL).
inline u32 __fastcall FkIsAlive(void* self, void*) { Ev& e = PushEv(0x5E0170, 1); e.v[0] = Tag(self); return gAlive; }
inline u32 __fastcall FkIsOut(void* self, void*) { Ev& e = PushEv(0x420E30, 1); e.v[0] = Tag(self); return gOut; }
// CPedIntelligence::FindTaskByType (0x600EE0; thiscall, el tipo, `ret 4`): evento [direccion, ECX (etiqueta), tipo]; devuelve gTask.
inline u32 __fastcall FkFind(void* self, void*, u32 type) { Ev& e = PushEv(0x600EE0, 2); e.v[0] = Tag(self); e.v[1] = type; return gTask; }
// CMatrix * CVector (0x59C890; cdecl, salida, matriz, vector): evento [direccion, salida (etiqueta), matriz (etiqueta), x, y, z del vector]; escribe gMul en la salida y la devuelve.
inline u32 __cdecl FkMul(u32* out, void* matrix, const u32* in) {
    Ev& e = PushEv(0x59C890, 5);
    e.v[0] = Tag(out); e.v[1] = Tag(matrix); e.v[2] = in[0]; e.v[3] = in[1]; e.v[4] = in[2];
    out[0] = gMul[0]; out[1] = gMul[1]; out[2] = gMul[2];
    return u32(uintptr_t(out));
}
// CGeneral::GetRadianAngleBetweenPoints (0x53CBE0; cdecl, cuatro floats como bits): evento [direccion, x, y, z, w]; devuelve el float del caso en ST0.
inline float __cdecl FkRadian(u32 x, u32 y, u32 z, u32 w) {
    Ev& e = PushEv(0x53CBE0, 4);
    e.v[0] = x; e.v[1] = y; e.v[2] = z; e.v[3] = w;
    return std::bit_cast<float>(gRadian);
}
// CGeneral::LimitRadianAngle (0x53CB50; cdecl, un float como bits): evento [direccion, bits]; devuelve el float del caso en ST0.
inline float __cdecl FkLimit(u32 a) {
    Ev& e = PushEv(0x53CB50, 1);
    e.v[0] = a;
    return std::bit_cast<float>(gLimit);
}
// CEntity::RegisterReference (0x571B70) / CleanUpOldReference (0x571A00): thiscall, un puntero; evento [direccion, ECX (etiqueta), argumento (etiqueta)].
template<u32 ADDR> inline void __fastcall FkRef(void* self, void*, void* arg) {
    Ev& e = PushEv(ADDR, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(arg);
}
// CEventGroup::Add (0x4AB420; thiscall, evento y bool, `ret 8`): evento [direccion, ECX (etiqueta), evento (etiqueta), bool, +4 (dword), +8 (byte), +0xC (byte), +0xE (word), +0x10 (word), +0x14 (etiqueta), +0x18 (byte), +0x1C (dword)].
inline u32 __fastcall FkAdd(void* self, void*, const u8* ev, u32 flag) {
    Ev& e = PushEv(0x4AB420, 11);
    u32 d4{}, d14{}, d1c{}; u16 w0e{}, w10{};
    memcpy(&d4, ev + 4, 4); memcpy(&d14, ev + 0x14, 4); memcpy(&d1c, ev + 0x1C, 4); memcpy(&w0e, ev + 0xE, 2); memcpy(&w10, ev + 0x10, 2);
    e.v[0] = Tag(self); e.v[1] = Tag(ev); e.v[2] = flag; e.v[3] = d4; e.v[4] = ev[8]; e.v[5] = ev[0xC]; e.v[6] = w0e; e.v[7] = w10; e.v[8] = Tag(uintptr_t(d14)); e.v[9] = ev[0x18]; e.v[10] = d1c;
    return 0;
}
// Ranuras de la tabla virtual falsa (thiscall sin argumentos): evento [0x4C6200 + ranura, ECX (etiqueta)].
template<u32 K> inline void __fastcall FkSlot(void* self, void*) { Ev& e = PushEv(0x4C6200 + K, 1); e.v[0] = Tag(self); }
template<u32... K> inline std::array<uintptr_t, sizeof...(K)> SlotTable(std::integer_sequence<u32, K...>) { return {reinterpret_cast<uintptr_t>(&FkSlot<K>)...}; }
// CFileMgr: OpenFile (0x538900; cdecl, ruta y modo), OpenFileForWriting (0x538910; ruta), Read (0x538950; handle, buffer, tamano), Write (0x538970), CloseFile (0x5389D0).
inline u32 __cdecl FkOpen(const char* path, const char* mode) {
    Ev& e = PushEv(0x538900, 3);
    e.v[0] = Tag(path); e.v[1] = FnvStr(path); e.v[2] = FnvStr(mode);
    return gHandle;
}
inline u32 __cdecl FkOpenW(const char* path) {
    Ev& e = PushEv(0x538910, 2);
    e.v[0] = Tag(path); e.v[1] = FnvStr(path);
    return gHandle;
}
inline u32 __cdecl FkRead(u32 handle, u8* buf, u32 size) {
    Ev& e = PushEv(0x538950, 3);
    e.v[0] = handle; e.v[1] = Tag(buf); e.v[2] = size;
    memcpy(buf, gMarker.data(), 16);
    return gRead;
}
inline u32 __cdecl FkWrite(u32 handle, void* buf, u32 size) {
    Ev& e = PushEv(0x538970, 3);
    e.v[0] = handle; e.v[1] = Tag(buf); e.v[2] = size;
    return 0;
}
inline u32 __cdecl FkClose(u32 handle) {
    Ev& e = PushEv(0x5389D0, 1);
    e.v[0] = handle;
    return 0;
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
    bool floatRet{};
};

inline void PutRaw(uintptr_t at, const void* data, size_t n) { memcpy(reinterpret_cast<void*>(at), data, n); }
inline void PutRaw32(uintptr_t at, u32 v) { PutRaw(at, &v, 4); }
inline Arena<kWin>& ArenaOf(u32 i) { switch (i & 3) { case 0: return arenaA; case 1: return arenaB; case 2: return arenaC; default: return arenaD; } }
inline bool UsesE(Kind k) { return k == kStore || k == kGetFile; }

// Escrituras que el EXE hace en cada elemento de un almacen (de 0x4C4A60 y del constructor del elemento), en orden: [desplazamiento, longitud, valor]
inline std::vector<std::array<u32, 3>> ElemWrites(unsigned fn) {
    std::vector<std::array<u32, 3>> w{{0, 4, 0x85BB9C}, {8, 2, 0}, {0xA, 2, 0xFFFF}};
    const auto add = [&](std::initializer_list<std::array<u32, 3>> l) { w.insert(w.end(), l.begin(), l.end()); };
    switch (fn) {
    case 7: add({{0, 4, 0x85BBF0}}); break;
    case 8: add({{0, 4, 0x85BC30}}); break;
    case 9: add({{0x20, 2, 0}, {0x22, 2, 0}, {0, 4, 0x85BC70}}); break;
    case 10: add({{0, 4, 0x85BCB0}, {0x22, 2, 0xFFFF}}); break;
    case 11: add({{0x20, 2, 0}, {0x22, 2, 0}, {0, 4, 0x85BCF0}, {0x26, 2, 0xFFFF}}); break;
    case 12: add({{0, 4, 0x85BD78}}); break;
    case 13: add({{0, 4, 0x85BD30}}); break;
    case 14:
        add({{0x5C, 4, 0}, {0x2D0, 1, 0}, {0x4E, 1, 0}, {0, 4, 0x85C5C8}, {0x304, 4, 0xFFFFFFFF}});
        for (u32 i = 0; i < 9; ++i) add({{0x2D6 + 4 * i, 4, 0xFFFFFFFF}});
        add({{0x2FA, 4, 0xFFFFFFFF}, {0x2FE, 4, 0xFFFFFFFF}});
        break;
    case 15: add({{0, 4, 0x85BDC0}, {0x34, 4, 0}}); break;
    }
    return w;
}
// Resumen del almacen tras la llamada: contador, si cada elemento es lo esperado, cambios totales (cuenta y FNV), cambios fuera del almacen y los elementos de muestra con sus cambios
inline json StoreObs(unsigned fn, const C& c) {
    const auto& a = c.a;
    const u32 cap = kCap[fn], sz = kElemSize[fn];
    const size_t objAt = kGuard + a[0], elemsAt = objAt + 4, endAt = elemsAt + size_t(cap) * sz;
    // punteros crudos: con std::array en Debug cada acceso comprueba limites y 460 KB por caso y lado serian minutos
    const u8* const em = arenaE.m.data();
    const u8* const er = arenaE.r.data();
    constexpr size_t kN = Arena<kBigWin>::N;
    u32 count{}; memcpy(&count, em + objAt, 4);
    const auto writes = ElemWrites(fn);
    bool allok = true;
    std::vector<u8> expected(sz);
    for (u32 i = 0; i < cap; ++i) {
        memcpy(expected.data(), er + elemsAt + size_t(i) * sz, sz);
        for (const auto& w : writes) memcpy(expected.data() + w[0], &w[2], w[1]);
        if (memcmp(expected.data(), em + elemsAt + size_t(i) * sz, sz)) allok = false;
    }
    u32 n = 0, h = 2166136261u, outside = 0;
    for (size_t i = 0; i < kN;) {
        if (i + 64 <= kN && memcmp(em + i, er + i, 64) == 0) { i += 64; continue; }   // salta los bloques sin cambios
        const size_t end = i + 64 < kN ? i + 64 : kN;
        for (; i < end; ++i)
            if (em[i] != er[i]) {
                ++n;
                const i32 off = i32(int64(i) - int64(kGuard));
                const u8* b = reinterpret_cast<const u8*>(&off);
                for (int k = 0; k < 4; ++k) { h ^= b[k]; h *= 16777619u; }
                h ^= em[i]; h *= 16777619u;
                if ((i < objAt) || (i >= endAt)) ++outside;
            }
    }
    json samples = json::array();
    for (u32 k = 0; k < 8; ++k) {
        const u32 idx = a[4 + k];
        json ch = json::array();
        const size_t at = elemsAt + size_t(idx) * sz;
        for (u32 i = 0; i < sz; ++i)
            if (em[at + i] != er[at + i]) ch.push_back(json::array({i, em[at + i]}));
        samples.push_back(json::array({idx, ch}));
    }
    return {{"count", count}, {"allok", allok}, {"n", n}, {"h", h}, {"outside", outside}, {"samples", samples}};
}
// Resumen del bloque de ids que reserva GetModelInfoIdFile: primeros 32 bytes, si el resto esta a cero y cuantos bytes cambiaron fuera del bloque
inline json BlockObs(const C& c) {
    const size_t at = kGuard + c.a[5];
    const u8* const em = arenaE.m.data();
    const u8* const er = arenaE.r.data();
    constexpr size_t kN = Arena<kBigWin>::N;
    bool tailZero = true;
    for (size_t i = 32; i < kFileBlock; ++i) tailZero &= em[at + i] == 0;
    u32 outside = 0;
    for (size_t i = 0; i < kN;) {
        if (i + 64 <= kN && memcmp(em + i, er + i, 64) == 0) { i += 64; continue; }
        const size_t end = i + 64 < kN ? i + 64 : kN;
        for (; i < end; ++i)
            if (em[i] != er[i] && (i < at || i >= at + kFileBlock)) ++outside;
    }
    return {{"head", HexBytes(em + at, 32)}, {"tail_zero", tailZero}, {"outside", outside}};
}

// Prepara arenas y memoria global del caso; devuelve la llamada
inline Call Setup(const C& c) {
    Call call; call.ecx = c.ecx; call.eax = c.eax;
    const auto& a = c.a;
    const Kind k = kKind[c.fn];
    arenaA.Pattern(a[2], a[36]); arenaB.Pattern(a[2], a[37]); arenaC.Pattern(a[2], a[38]); arenaD.Pattern(a[2], a[39]);
    if (UsesE(k)) arenaE.Pattern(a[2], a[40]);
    gRet = 0; gDtorWrite = 0; gNew = 0; gRand = 0; gAlive = gOut = gTask = gRadian = gLimit = 0; gMul = {}; gHandle = gRead = 0; gMarker = {};
    const u32 o = a[0];
    const u32 self = u32(arenaA.Obj(o));
    call.ecx = self;
    if (k == kStore) { call.ecx = u32(arenaE.Obj(o)); arenaE.Put32(o, a[3]); }
    else arenaA.Put32(o, a[3]);
    const auto PutPath = [&](u32 len) {   // ruta de CModelInfoAccelerator (+6, 20 bytes): `len` letras y un terminador
        for (u32 i = 0; i < len; ++i) arenaA.Put8(o + 6 + i, u8('a' + (c.seed + i * 7) % 26));
        arenaA.Put8(o + 6 + len, 0);
    };
    switch (k) {
    case kNext:
        call.args[0] = a[4]; call.argCount = 1;
        break;
    case kFirst:
        arenaA.Put32(o + 0xC, a[4]); arenaA.Put32(o + 0x14, a[5]);
        PutRaw32(kClock, a[6]);
        gNew = a[7] == 0 ? 0u : (a[7] == 1 ? u32(arenaB.Obj(a[8])) : a[9]);
        gRet = a[10];
        call.args[0] = a[11]; call.argCount = 1;
        break;
    case kControl: {
        const bool hasPed = a[5] != 0;
        const u32 waited = hasPed ? u32(arenaB.Obj(a[6])) : 0u;
        arenaA.Put32(o + 8, a[4]); arenaA.Put32(o + 0xC, waited); arenaA.Put32(o + 0x10, a[7]);
        arenaA.Put8(o + 0x18, u8(a[8])); arenaA.Put8(o + 0x24, u8(a[9])); arenaA.Put32(o + 0x28, a[10]);
        const u32 ped = u32(arenaC.Obj(a[11]));
        arenaC.Put32(a[11] + 4, a[14]); arenaC.Put32(a[11] + 8, a[15]); arenaC.Put32(a[11] + 0xC, a[16]);
        const u32 matC = a[12] ? u32(arenaD.Obj(a[13])) : 0u;
        arenaC.Put32(a[11] + 0x14, matC);
        if (matC) { arenaD.Put32(a[13] + 0x30, a[23]); arenaD.Put32(a[13] + 0x34, a[24]); arenaD.Put32(a[13] + 0x38, a[25]); }
        arenaC.Put32(a[11] + 0x47C, a[17]);
        if (hasPed) {
            arenaB.Put32(a[6] + 4, a[18]); arenaB.Put32(a[6] + 8, a[19]); arenaB.Put32(a[6] + 0xC, a[20]);
            const u32 matB = a[21] ? u32(arenaD.Obj(a[22])) : 0u;
            arenaB.Put32(a[6] + 0x14, matB);
            if (matB) { arenaD.Put32(a[22] + 0x30, a[26]); arenaD.Put32(a[22] + 0x34, a[27]); arenaD.Put32(a[22] + 0x38, a[28]); }
            arenaB.Put32(a[6] + 0x47C, a[29]);
        }
        if (a[32]) { arenaD.Put32(a[33] + 0x14, a[41]); arenaD.Put32(a[33] + 0x18, a[42]); arenaD.Put32(a[33] + 0x1C, a[43]); gTask = u32(arenaD.Obj(a[33])); }
        gMul = {a[44], a[45], a[46]}; gRadian = a[47]; gLimit = a[48]; gAlive = a[30]; gOut = a[31];
        PutRaw32(kClock, a[49]);
        call.args[0] = ped; call.argCount = 1;
        break;
    }
    case kCanJoin:
        arenaA.Put8(o + 0x30, u8(a[4])); arenaA.Put8(o + 0x31, u8(a[5])); arenaA.Put32(o + 0x28, a[6]); arenaA.Put32(o + 0x2C, a[7]);
        PutRaw32(kClock, a[8]);
        break;
    case kDistSq: {
        const u32 pedOff = a[18];
        const u32 ped = u32(arenaB.Obj(pedOff));
        arenaB.Put32(pedOff + 4, a[19]); arenaB.Put32(pedOff + 8, a[20]); arenaB.Put32(pedOff + 0xC, a[21]);
        const u32 mat = a[22] ? u32(arenaC.Obj(a[23])) : 0u;
        arenaB.Put32(pedOff + 0x14, mat);
        if (mat) { arenaC.Put32(a[23] + 0x30, a[24]); arenaC.Put32(a[23] + 0x34, a[25]); arenaC.Put32(a[23] + 0x38, a[26]); }
        for (u32 i = 0; i < kPathAreas; ++i) PutRaw32(kPathTable + 4 * i, 0);
        const bool nextValid = a[4] != 0, lastValid = a[7] != 0;
        const bool same = nextValid && lastValid && a[5] == a[8];
        const u32 nextMode = a[4], lastMode = same ? a[4] : a[7];
        if (nextMode == 1) PutRaw32(kPathTable + 4 * a[5], u32(arenaC.Obj(a[10])));
        if (lastMode == 1 && !same) PutRaw32(kPathTable + 4 * a[8], u32(arenaB.Obj(a[11])));
        if (nextMode == 1) for (u32 i = 0; i < 3; ++i) arenaC.Put16(a[10] + a[6] * 0x1C + 8 + 2 * i, u16(a[12 + i]));
        if (lastMode == 1) {
            if (same) for (u32 i = 0; i < 3; ++i) arenaC.Put16(a[10] + a[9] * 0x1C + 8 + 2 * i, u16(a[15 + i]));
            else for (u32 i = 0; i < 3; ++i) arenaB.Put16(a[11] + a[9] * 0x1C + 8 + 2 * i, u16(a[15 + i]));
        }
        arenaA.Put16(o + 0x1C, nextValid ? u16(a[5]) : u16(0xFFFF)); arenaA.Put16(o + 0x1E, u16(a[6]));
        arenaA.Put16(o + 0x18, lastValid ? u16(a[8]) : u16(0xFFFF)); arenaA.Put16(o + 0x1A, u16(a[9]));
        call.args[0] = ped; call.argCount = 1; call.floatRet = true;
        break;
    }
    case kSdd:
        gRet = a[4]; gDtorWrite = a[5];
        call.args[0] = a[6]; call.argCount = 1;
        break;
    case kWanderNew:
        gNew = a[5] == 0 ? 0u : u32(arenaB.Obj(a[6]));
        gRand = a[7]; gRet = a[8];
        call.args[0] = a[4]; call.argCount = 1; call.popBytes = 4;
        break;
    case kStore:
        break;
    case kThunk: {
        const auto slots = SlotTable(std::make_integer_sequence<u32, 16>{});
        for (u32 i = 0; i < 16; ++i) arenaD.Put32(a[4] + 4 * i, u32(slots[i]));
        arenaA.Put32(o, u32(arenaD.Obj(a[4])));
        break;
    }
    case kWeaponBody:
        break;
    case kPedBody:
        arenaA.Put32(o + 0x34, a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaB.Obj(a[5])) : a[6]));
        break;
    case kFreeIds:
        arenaA.Put32(o, a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaE.Obj(a[5])) : a[6]));
        break;
    case kGetFile:
        PutPath(a[6]);
        gHandle = a[4]; gNew = u32(arenaE.Obj(a[5])); gMarker = {a[7], a[8], a[9], a[10]}; gRead = a[11];
        break;
    case kEndLoad:
        arenaA.Put32(o, a[4] == 0 ? 0u : (a[4] == 1 ? u32(arenaE.Obj(a[5])) : a[6]));
        arenaA.Put8(o + 0x1B, u8(a[7]));
        PutPath(a[9]);
        gHandle = a[8];
        break;
    case kSeekCtor:
        for (u32 i = 0; i < 8; ++i) call.args[i] = a[4 + i];
        call.argCount = 8;
        break;
    case kSeekBody:
        arenaA.Put32(o + 8, 0); arenaA.Put32(o + 0xC, a[4]);
        break;
    }
    return call;
}

inline void CaseRun(Env& env, Totals& t, unsigned fn, unsigned n) {
    const C c = MakeC(fn, n);
    const Kind kind = kKind[fn];
    json input = json::parse(JsonC(c));
    input["arena"] = json::array({u32(uintptr_t(arenaA.m.data())), u32(uintptr_t(arenaB.m.data())), u32(uintptr_t(arenaC.m.data())), u32(uintptr_t(arenaD.m.data())), u32(uintptr_t(arenaE.m.data()))});   // direcciones reales de las arenas (para el oraculo)
    Side sides[2]; json rows[2];
    curFn = fn;
    for (int s = 0; s < 2; ++s) {
        const bool candidate = s == 1;
        PrepareFloatingState();
        const Call call = Setup(c);
        arenaA.Snap(); arenaB.Snap(); arenaC.Snap(); arenaD.Snap();
        if (UsesE(kind)) arenaE.Snap();
        SnapRegions();
        TagClear(); TagAdd(arenaA.m.data(), arenaA.m.size()); TagAdd(arenaB.m.data(), arenaB.m.size()); TagAdd(arenaC.m.data(), arenaC.m.size()); TagAdd(arenaD.m.data(), arenaD.m.size()); TagAdd(arenaE.m.data(), arenaE.m.size());
        nev = 0;
        sides[s] = Execute(env, fn, kEntries[fn], candidate, [&](uintptr_t entry) {
            return Gate(entry, call.args.data(), call.argCount, call.eax, call.ecx, c.edx, c.stack, call.popBytes, call.floatRet);
        });
        // vtables que la DLL deja distintas del EXE y que no son parte de lo que se compara (ver README): el cuerpo del destructor de la clase del EXE (66CBB0) deja las del EXE; la funcion de la DLL, las suyas
        if (fn == 32) {
            for (auto* arr : {&arenaA.m, &arenaA.r}) {
                memset(arr->data() + kGuard + c.a[0], 0, 4);
                memset(arr->data() + kGuard + c.a[0] + 0x40, 0, 4);
            }
        }
        // el nibble alto de +0x54 del constructor no lo escribe el EXE (basura de la reserva)
        if (fn == 30) {
            uint8& m = arenaA.m[kGuard + c.a[0] + 0x54];
            m = u8((m & 0x0F) | (arenaA.r[kGuard + c.a[0] + 0x54] & 0xF0));
        }
        json obs;
        obs["changes"] = {{"A", arenaA.Changes()}, {"B", arenaB.Changes()}, {"C", arenaC.Changes()}, {"D", arenaD.Changes()}};
        if (kind == kStore) obs["store"] = StoreObs(fn, c);
        if (kind == kGetFile) obs["block"] = BlockObs(c);
        obs["gchanges"] = RegionChanges();
        obs["events"] = EvsJson();
        const u32 eax = sides[s].reg.eax;
        switch (kind) {
        case kNext: obs["ret"] = eax; break;
        case kFirst: case kControl: case kSdd: case kWanderNew: case kStore: case kSeekCtor: obs["ret"] = Tag(uintptr_t(eax)); break;
        case kCanJoin: case kGetFile: obs["ret"] = eax & 0xFFu; break;
        case kDistSq: obs["ret"] = gFloatRet; break;
        default: obs["ret"] = nullptr; break;
        }
        rows[s] = CloseSide(env, sides[s], obs);
    }
    Finish(t, kFn[fn], n, input, rows, sides);
}


// ================================================================================================ ejecucion
// Casos por funcion: kCases salvo que la variable de entorno FN194A_CASES (solo para las rondas de la prueba negativa, hereda el proceso del juego) pida menos (multiplo de 16, minimo 16)
inline unsigned CasesPerFunction() {
    char buffer[16]{};
    if (GetEnvironmentVariableA("FN194A_CASES", buffer, sizeof(buffer)) > 0) {
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
    if (!std::filesystem::exists(dir / "fn194a-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "fn194a-runtime.jsonl"); Write({{"event", "waiting_for_player"}}); ReversingProbe::InstallFailureCapture("fn194a"); }
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
        registered[kFn[f]] = found && env.route[f]->Registered(); bodies[kFn[f]] = found && env.route[f]->Body() != 0 && ModuleOf(env.route[f]->Body()) == env.asi;
        ready &= found && env.route[f]->Registered() && env.route[f]->Body() && ModuleOf(env.route[f]->Body()) == env.asi;
        if (!found) ready = false;
        ownHooks[kFn[f]] = found ? json({{"class", cat}, {"name", name}, {"address", kEntries[f]}, {"virtual", vbody != 0}, {"body", env.route[f]->Body()}, {"body_module", ModuleOf(env.route[f]->Body())}}) : json(nullptr);
    }
    json hooksFound = json::array();
    if (!ready) { collaborators = ownHooks; Run(env, false, registered, bodies, collaborators); return; }
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
    // Colaborador SIN hook RH que la DLL llama por la direccion del EXE (operator new y delete del CRT): el falso va solo en el EXE (se exige que no tenga hook)
    auto E = [&](uintptr_t exe, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador CON hook RH que la DLL llama por la direccion del EXE (plugin::Call): se apaga el hook (ruta) y el falso va solo en el EXE
    auto FE = [&](uintptr_t exe, auto* fake) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        if (!it->second.hooked) ready = false;
        return S(Pair{exe, 0, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador SIN hook RH con funcion propia de la DLL (direccion dada): el falso va en la direccion del EXE y en esa funcion
    auto U = [&](uintptr_t exe, uintptr_t dllAddress, auto* fake) {
        std::string cat, name;
        if (FindHook(exe, cat, name)) ready = false;
        const uintptr_t body = Body(dllAddress);
        if (!body || ModuleOf(body) != env.asi) ready = false;
        return S(Pair{exe, body, false}, reinterpret_cast<void*>(fake));
    };
    // Colaborador REAL (corre el codigo del EXE en ambos lados): si tiene hook se apaga (ruta); si no, es codigo del EXE sin mas
    auto Real = [&](uintptr_t exe) {
        auto it = collabCache.find(exe);
        if (it == collabCache.end()) it = collabCache.emplace(exe, MakeCollab(env, exe, hooksFound)).first;
        return it->second;
    };
    auto Del = [&] { return F(0x61A5B0, &FkDelete<0x61A5B0>); };   // CTask::operator delete (con hook)
    auto CrtDel = [&] { return E(0x8214BD, &FkDelete<0x8214BD>); };  // operator delete del CRT
    env.stubs[1] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x62F310, &FkStand)};
    env.stubs[2] = {F(0x5E0170, &FkIsAlive), F(0x420E30, &FkIsOut), F(0x600EE0, &FkFind), FE(0x59C890, &FkMul), F(0x53CBE0, &FkRadian), F(0x53CB50, &FkLimit), F(0x4AB420, &FkAdd), F(0x571B70, &FkRef<0x571B70>), F(0x571A00, &FkRef<0x571A00>)};
    env.reals[2] = {Real(0x40FE60)};
    env.reals[4] = {Real(0x420A10)};
    env.stubs[5] = {F(0x669D20, &FkBody<0x669D20>), Del()};
    env.stubs[6] = {F(0x61A5A0, &FkNew<0x61A5A0>), F(0x460C80, &FkCtor2<0x460C80>), F(0x658770, &FkCtor3<0x658770>), F(0x48E610, &FkCtor3<0x48E610>), F(0x672690, &FkCtor3<0x672690>), F(0x48E4F0, &FkCtor3<0x48E4F0>),
                    U(0x821B1E, std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), &FkRand)};
    env.reals[6] = {Real(0x407180)};
    for (unsigned f = 7; f <= 15; ++f) env.reals[f] = {Real(kElemCtor[f]), Real(0x4C4A60)};
    env.stubs[17] = {F(0x4C5590, &FkBody<0x4C5590>), CrtDel()};
    env.stubs[18] = {F(0x4C55D0, &FkBody<0x4C55D0>), CrtDel()};
    env.stubs[19] = {F(0x4C5620, &FkBody<0x4C5620>), CrtDel()};
    env.stubs[20] = {F(0x4C5680, &FkBody<0x4C5680>), CrtDel()};
    env.stubs[21] = {F(0x4C56E0, &FkBody<0x4C56E0>), CrtDel()};
    env.stubs[22] = {F(0x4C5750, &FkBody<0x4C5750>), CrtDel()};
    env.stubs[23] = {Rt(26, &FkBody<0x4C5790>), CrtDel()};
    env.stubs[24] = {Rt(25, &FkBody<0x4C62F0>), CrtDel()};
    env.stubs[25] = {F(0x40F700, &FkThis<0x40F700>), F(0x40FC40, &FkDeleteRaw<0x40FC40>)};
    env.stubs[27] = {E(0x8214BD, &FkDeleteRaw<0x8214BD>)};
    env.stubs[28] = {F(0x538900, &FkOpen), E(0x82119A, &FkNew<0x82119A>), F(0x538950, &FkRead), F(0x5389D0, &FkClose)};
    env.stubs[29] = {F(0x538910, &FkOpenW), F(0x538970, &FkWrite), F(0x5389D0, &FkClose), E(0x8214BD, &FkDeleteRaw<0x8214BD>)};
    env.stubs[30] = {F(0x571B70, &FkRef<0x571B70>)};
    {   // 0x66CBB0 es el cuerpo que prueba la funcion 32 (hook "Destructor", envoltorio con el destructor expandido); el destructor con borrado de la DLL llama a otra funcion propia (DestructorBodyExe)
        Pair p{0x66CBB0, Body(std::bit_cast<uintptr_t>(&SeekFixed::DestructorBodyExe)), false};
        if (!p.dll || ModuleOf(p.dll) != env.asi) ready = false;
        env.stubs[31] = {S(p, reinterpret_cast<void*>(&FkBody<0x66CBB0>)), Del()};
    }
    env.stubs[32] = {F(0x571A00, &FkRef<0x571A00>)};
    const char* dllReal[kFunctions];
    for (unsigned f = 0; f < kFunctions; ++f) dllReal[f] = "toda la funcion salvo los colaboradores falseados (falso identico en la direccion del EXE y en la funcion de la DLL, o solo en la direccion del EXE si la DLL la llama por ella)";
    for (unsigned f : {0u, 3u, 16u, 26u}) dllReal[f] = "toda la funcion (sin colaboradores)";
    dllReal[2] = "toda la funcion salvo los falsos; la resta de vectores (0x40FE60, real con hook apagado en el EXE; en la DLL, resta en linea), el constructor y el destructor del evento CEventDeadPed (reales) corren de verdad";
    dllReal[4] = "toda la funcion; CPathNode::GetPosition (0x420A10) corre real con el hook apagado en el EXE";
    dllReal[6] = "toda la funcion salvo CTask::operator new, los cinco constructores de tareas y rand (falsos); el entero aleatorio 0x407180 corre real con el hook apagado";
    for (unsigned f = 7; f <= 15; ++f) dllReal[f] = "toda la funcion; el constructor de elemento y 0x4C4A60 (hook apagado) y el iterador de vectores del CRT (0x821D30) corren reales en ambos lados";
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
    regions.emplace_back(kClock, 4);
    for (unsigned f : {1u, 2u, 3u}) regionsOf[f] = {regions.size() - 1};
    regions.emplace_back(kPathTable, 4 * kPathAreas); regionsOf[4] = {regions.size() - 1};
    Run(env, ready, registered, bodies, collaborators);
}
}
