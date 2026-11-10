#pragma once
// Fixtures del banco Fn155A (lote 155A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables leidas del PE y de los llamadores, ver README.md y fixture_oracle.py). 17 asignadas: ninguna tenia hook
// (se comprobo el codigo, docs/hooks.csv y git worktree list). Los nombres del listado eran en parte erroneos (0x625250 es CTaskSimpleGunControl, 0x64C200 CTaskSimpleCarSetPedSlowDraggedOut y 0x64E2A0
// CTaskComplexCarSlowBeDraggedOut, no CTaskSimpleUseGun ni CTaskSimpleCarSlowBeDraggedOut).
//   idx  direccion  nombre real                                                              forma
//    0   0x4DEFE0   CAEGlobalWeaponAudioEntity::PickAmbientGunFire                           thiscall(&disparos, &arma, &retardo) `ret 0Ch`: RNG 0x4D9C10 (0,1) y segun el resultado otras dos
//    1   0x625250   CTaskSimpleGunControl::ScalarDeletingDestructor (vtable 0x86D76C)        thiscall(flags) `ret 4`: cuerpo 61F4D0 (con hook), CTask::operator delete 61A5B0
//    2   0x61DF30   CTaskSimpleUseGun cuerpo del destructor (vtable 0x86D724, marco SEH)      thiscall() `ret`: CleanUpOldReference(+1Ch), SetDeleteCallback(+2Ch), ~CTask 61A3A0
//    3   0x6091B0   CEventDontJoinPlayerGroup::GetLifeTime (vtable 0x86D0E0, ranura 3)        thiscall() `ret`: xor eax,eax
//    4   0x609220   CEventDontJoinPlayerGroup::AffectsPed (ranura 5)                         thiscall(ped) `ret 4`: mov al,1
//    5   0x6091A0   CEventDontJoinPlayerGroup::GetEventType (ranura 1)                       thiscall() `ret`: mov eax,5Eh
//    6   0x609230   CEventDontJoinPlayerGroup::ScalarDeletingDestructor (ranura 0)           thiscall(flags) `ret 4`: cuerpo 609140 (con hook), CEvent::operator delete 4B5630 (con hook)
//    7   0x6091C0   CEventDontJoinPlayerGroup::Clone (ranura 4)                              thiscall() `ret`: CEvent::operator new 4B5620 (0x10) y constructor 6090E0(+0Ch); null si no hay memoria
//    8   0x64C200   CTaskSimpleCarSetPedSlowDraggedOut::ScalarDeletingDestructor (0x86EF30)  cuerpo 648430 (con hook), CTask::operator delete 61A5B0
//    9   0x64E2A0   CTaskComplexCarSlowBeDraggedOut::ScalarDeletingDestructor (0x86EF54)     cuerpo 64C580 (con hook), CTask::operator delete 61A5B0
//   10   0x648330   CTaskSimpleCarSlowBeDraggedOut::ComputeAnimID                            thiscall(&grupo, &animId) `ret 8`: puerta (+14h) -> animId 17Ah/17Bh, grupo = CVehicleAnimGroup::GetGroup 6E3B00
//   11   0x4B8330   CEventLeaderExitedCarAsDriver::GetLifeTime (vtable 0x85B898, ranura 3)    thiscall() `ret`: xor eax,eax
//   12   0x4B8390   CEventLeaderExitedCarAsDriver::AffectsPed (ranura 5)                      thiscall(ped) `ret 4`: xor al,al
//   13   0x4B8320   CEventLeaderExitedCarAsDriver::GetEventType (ranura 1)                    thiscall() `ret`: mov eax,37h
//   14   0x4B84D0   CEventLeaderExitedCarAsDriver::GetEventPriority (ranura 2)                thiscall() `ret`: mov eax,2Fh
//   15   0x4B83C0   CEventLeaderExitedCarAsDriver cuerpo del destructor                       thiscall() `ret`: [ecx]=85B898h; jmp 4AC480 (cuerpo de ~CEventEditableResponse, con hook)
//   16   0x4B83A0   CEventLeaderExitedCarAsDriver::ScalarDeletingDestructor (ranura 0)        thiscall(flags) `ret 4`: cuerpo 4B83C0 (esta funcion 15), CEvent::operator delete 4B5630 (con hook)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F155AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 17;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
}

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 24> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { return Rand(n, nk()); }
};

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kConstAL1, kPick, kSdd, kUseGunBody, kClone, kAnimId, kEvBody };
inline constexpr std::array<Kind, kFunctions> kKind{
    kPick, kSdd, kUseGunBody, kConst, kConstAL1, kConst, kSdd, kClone, kSdd, kSdd, kAnimId, kConst, kConstAL1, kConst, kConst, kEvBody, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4DEFE0, 0x625250, 0x61DF30, 0x6091B0, 0x609220, 0x6091A0, 0x609230, 0x6091C0, 0x64C200, 0x64E2A0, 0x648330, 0x4B8330, 0x4B8390, 0x4B8320, 0x4B84D0, 0x4B83C0, 0x4B83A0};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0x61F4D0, 0x61A3A0, 0, 0, 0, 0x609140, 0, 0x648430, 0x64C580, 0, 0, 0, 0, 0, 0x4AC480, 0x4B83C0};
// Funcion de liberacion de los destructores con borrado (CTask::operator delete, CEvent::operator delete); 0 = ninguna
inline constexpr std::array<u32, kFunctions> kDelete{0, 0x61A5B0, 0, 0, 0, 0, 0x4B5630, 0, 0x61A5B0, 0x61A5B0, 0, 0, 0, 0, 0, 0, 0x4B5630};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0x86D76C, 0x86D724, 0, 0, 0, 0x86D0E0, 0x86D0E0, 0x86EF30, 0x86EF54, 0x86EF0C, 0, 0, 0, 0, 0x85B898, 0x85B898};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 0x3C, 4, 4, 4, 4, 0x10, 4, 4, 0x18, 4, 4, 4, 4, 4, 4};
inline constexpr u32 kEvDefaultCb = 0x4D1390;                // CDefaultAnimCallback::DefaultAnimCB del EXE (el falso de SetDeleteCallback normaliza la direccion de la DLL a esta)
inline constexpr u32 kGroupBase = 0xC1CDC0;                  // CVehicleAnimGroupData::m_vehicleAnimGroups; cada grupo mide 0x94
inline constexpr u32 kVehicleHandling = 0x384;               // CVehicle::m_pHandlingData
inline constexpr u32 kHandlingGroupByte = 0xDE;              // tHandlingData::m_nAnimGroup

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; a[16..23] = datos extra
//   kConst: a[3] = dword en [this] (no se lee); kConstAL1: ademas a[4] = argumento de pila (el ped, no se lee)
//   kPick: a[3], a[4], a[5] = valores que devuelve el RNG falso (la 1.a llamada decide el caso: 0, 1 u otro); a[6] = modo de solape de las tres salidas (0 distintas; 1 p2 = p1; 2 p3 = p2; 3 p3 = p1; 4 todas iguales;
//         5 p2 = p1 + 1; 6 p3 = p1 - 2); a[7], a[8], a[9] = desplazamientos de las salidas en la arena B
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kUseGunBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = modo de la entidad objetivo (+1Ch: 0 nula, 1 en B a[7], 2 cruda a[8]); a[9] = modo de la animacion (+2Ch: 0 nula, 1 en B a[10], 2 cruda a[11])
//   kClone: a[3] = m_player (+0Ch) crudo; a[4] = 0 crudo / 1 puntero a la arena C (a[5]); a[6] = 0 sin memoria (operator new devuelve nulo) / 1 objeto en B (a[7]); a[8] = dword que el constructor falso deja en el objeto
//   kAnimId: a[3] = puerta (+14h); a[4] = byte del grupo de animacion del vehiculo (+0DEh del handling); a[5] = dword previo del animId; a[6] = EAX del GetGroup falso; a[7] = modo de las salidas (0 grupo en B a[10] y animId en C a[11];
//         1 el mismo puntero en C a[11]; 2 animId = la puerta (this+14h); 3 animId en handling+0DCh; 4 animId en handling+0DEh; 5 grupo = this+10h; 6 grupo = handling+0DEh); a[8] = vehiculo en B (span 388h); a[9] = handling en C (span E0h);
//         a[10] = salida del grupo en B; a[11] = animId en C; a[16] = dword en handling+0DCh (el byte 0DEh se sustituye por a[4])
//   kEvBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    switch (kKind[c.fn]) {
    case kConst:
        c.a[3] = Raw(g);
        break;
    case kConstAL1:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kPick: {
        const auto rv = [&] { return g.Pk({g.R(20), g.R(400), g.R(1500), g.W(), Raw(g), 0u, 1u, 0x7FFFFFFFu, 0x80000000u}); };
        c.a[3] = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 1u, 1u, 1u, 1u, 2u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, g.W()});
        c.a[4] = rv();
        c.a[5] = rv();
        c.a[6] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u});
        for (unsigned i = 7; i <= 9; ++i) c.a[i] = Offset(g, g.R(4), 4);
        break;
    }
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kUseGunBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = g.P(70) ? (Raw(g) | 1u) : Raw(g);
        c.a[9] = g.Pk({0u, 1u, 1u, 2u});
        c.a[10] = Offset(g, g.R(4), 0x20);
        c.a[11] = g.P(70) ? (Raw(g) | 1u) : Raw(g);
        break;
    case kClone:
        c.a[3] = Raw(g);
        c.a[4] = g.P(35) ? 1u : 0u;
        c.a[5] = Offset(g, g.R(4), 4);
        c.a[6] = g.P(80) ? 1u : 0u;
        c.a[7] = Offset(g, g.R(4), 0x10);
        c.a[8] = Raw(g);
        break;
    case kAnimId:
        c.a[3] = g.Pk({8u, 9u, 10u, 11u, 8u, 9u, 10u, 11u, 8u, 9u, 10u, 11u, 0u, 7u, 12u, 0xFFFFFFF8u, 0xFFFFFFFFu, 0x80000008u, 0x7FFFFFFFu, 0x100u, 0x108u, g.W()});
        c.a[4] = g.Pk({0u, 1u, 29u, 30u, 0xFFu, g.R(30), g.R(30), g.R(256), g.R(256)});
        c.a[5] = Raw(g);
        c.a[6] = Raw(g);
        c.a[7] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u});
        c.a[8] = Offset(g, g.R(4), 0x388);
        c.a[9] = Offset(g, g.R(4), 0xE0);
        c.a[10] = Offset(g, g.R(4), 4);
        c.a[11] = Offset(g, g.R(4), 4);
        c.a[16] = g.W();
        break;
    case kEvBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    }
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.H("s", c.s);
    return j.End();
}
}
