#pragma once
// Fixtures del banco Fn187A (lote 187A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 19 asignadas: 19 reconstruidas.
//   idx  direccion  nombre real                                                                      forma
//    0   0x4AF3D0   CEventAttractor::GetEventType                                                    thiscall() `ret` V: mov eax,1Bh
//    1   0x4AF3F0   CEventAttractor::GetEventPriority                                                thiscall() `ret` V: xor eax,eax
//    2   0x4AF3E0   CEventAttractor::GetLifeTime                                                     thiscall() `ret` V: xor eax,eax
//    3   0x4B74B0   CEventVehicleToSteal::Clone                                                      thiscall() `ret` V: pool de eventos 4B5570 en linea; ctor 4AF670(vehiculo +0Ch)
//    4   0x4B7520   CEventVehicleThreat::CloneEditable                                                thiscall() `ret` V: idem; ctor 4AFB50(vehiculo +14h)
//    5   0x4B7590   CEventKnockOffBike::Clone                                                         thiscall() `ret` V: idem; ctor 4AFCF0 (11 argumentos)
//    6   0x4B7630   CEventGunAimedAt::CloneEditable                                                   thiscall() `ret` V: idem; ctor 4B0700(+14h)
//    7   0x4B76A0   CEventInAir::Clone                                                                thiscall() `ret` V: pool 4B5570; construye en linea (+4 = 0, BYTE +8 = 0, vtable 85B3C0)
//    8   0x60A9A0   CPlayerPed destructor con borrado                                                 thiscall(flags) `ret 4`: cuerpo 6093B0; CPed::operator delete 5E4760
//    9   0x60D5B0   CPlayerPed::CPlayerPed(playerId, groupCreated)                                    thiscall(id, flag) `ret 8`: CPed 5E8030 ... (ver oraculo)
//   10   0x60F8C0   funcion vacia                                                                     `ret`
//   11   0x692680   CTaskSimpleDuckWhileShotsWhizzing::CTaskSimpleDuckWhileShotsWhizzing(WORD)        thiscall(WORD) `ret 4`: CTask 61A390 (real) y campos en linea
//   12   0x6926E0   CTaskSimpleDuckWhileShotsWhizzing::GetTaskType                                   thiscall() `ret` V: mov eax,137h
//   13   0x6926F0   CTaskSimpleDuckWhileShotsWhizzing cuerpo del destructor                           thiscall() `ret`: vtable 870BEC; salto a 692030
//   14   0x692700   CTaskSimpleDuckWhileShotsWhizzing::MakeAbortable                                  thiscall(ped, prioridad, evento) `ret 0Ch` V
//   15   0x692D60   CTaskSimpleDuckWhileShotsWhizzing::Clone                                           thiscall() `ret` V: new(28h) 61A5A0; ctor 692680(WORD +0Ch)
//   16   0x692DD0   CTaskSimpleShakeFist destructor con borrado                                       thiscall(flags) `ret 4`: cuerpo 690BB0; delete 61A5B0
//   17   0x693B80   CTaskSimpleDuckWhileShotsWhizzing destructor con borrado                           thiscall(flags) `ret 4`: cuerpo 6926F0; delete 61A5B0
//   18   0x694640   CTaskSimpleDuckWhileShotsWhizzing::ProcessPed                                      thiscall(ped) `ret 4` V: BlendAnimation 4D4610; SetDeleteCallback 4CEBC0; ProcessPed 694390
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F187AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 19;
constexpr unsigned kArenas = 4;

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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kSeed = 60;          // a[60..63] = semillas de las arenas A, B, C y D
constexpr unsigned kGSeed = 56;         // a[56], a[57] = semillas de las regiones globales del constructor de CPlayerPed (jugadores, grupos)
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 64> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en una zona [lo, lo + size) de una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxIn(Gen& g, u32 lo, u32 size, u32 span) { return lo + 4 * g.R((size - span) / 4 + 1); }
inline u32 AuxOffset(Gen& g, u32 span) { return AuxIn(g, 0, u32(kWin), span); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un dword distinto de cero
inline u32 NonZero(Gen& g) {
    const u32 v = Raw(g);
    return v ? v : g.Pk({1u, 0x100u, 0x80000000u, 0xFFFFFF00u});
}
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Bits de un float que no son NaN senalizador (la x87 los convertiria en silenciosos al cargarlos): ceros, infinitos, silenciosos, desnormales, normales y bits aleatorios con el exponente corregido
inline u32 FloatBits(Gen& g) {
    u32 w = g.W();
    if (((w >> 23) & 0xFFu) == 0xFFu && (w & 0x7FFFFFu) && !(w & 0x400000u)) w |= 0x400000u;   // sNaN -> qNaN
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00001u, 0x00000001u, 0x007FFFFFu, 0x80000001u, 0x42C80000u, 0x41F00000u, w, w, w, 0x3F000000u, 0x43480000u});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kVoid, kEvClone1, kEvKnock, kEvInAir, kSdd, kPlayerCtor, kDuckCtor, kDuckBody, kMakeAbort, kTaskClone, kProcessPed };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kConst, kConst, kEvClone1, kEvClone1, kEvKnock, kEvClone1, kEvInAir, kSdd, kPlayerCtor, kVoid, kDuckCtor, kConst, kDuckBody, kMakeAbort, kTaskClone, kSdd, kSdd, kProcessPed};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4AF3D0, 0x4AF3F0, 0x4AF3E0, 0x4B74B0, 0x4B7520, 0x4B7590, 0x4B7630, 0x4B76A0, 0x60A9A0, 0x60D5B0, 0x60F8C0, 0x692680, 0x6926E0, 0x6926F0, 0x692700, 0x692D60, 0x692DD0, 0x693B80, 0x694640};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x85B1B8, 0x85B1B8, 0x85B1B8, 0x85B210, 0x85B298, 0x85B2E0, 0x85B330, 0x85B3C0, 0x86D168, 0x86D168, 0, 0x870BEC, 0x870BEC, 0x870BEC, 0x870BEC, 0x870BEC, 0x870A94, 0x870BEC, 0x870BEC};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 0x10, 0x18, 0x3C, 0x18, 4, 4, 0x7A4, 4, 0x28, 4, 4, 0x18, 0x10, 4, 4, 0x14};
// Desplazamiento (respecto a `this`) del dato que el Clone de un evento pasa a su constructor (kEvClone1)
inline constexpr std::array<u32, kFunctions> kFieldOff{0, 0, 0, 0xC, 0x14, 0, 0x14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr u32 kEventPoolGlobal = 0xB744AC;   // puntero al pool de eventos (que el EXE lee en linea en los Clone de eventos)
inline constexpr u32 kTimerGlobal = 0xB7CB84;       // CTimer::m_snTimeInMilliseconds
inline constexpr u32 kPlayersBase = 0xB7CD9C, kPlayersStride = 0x190, kPlayersRegion = 0x190 * 4;     // CWorld::Players[id].m_PlayerData (id 0..3)
inline constexpr u32 kGroupsRegion = 0xC09BF0, kGroupsBytes = 0x2D4 * 7 + 4;                             // byte +0x2D0 de los grupos 0..7 (0xC09920 + g * 0x2D4 + 0x2D0)
inline constexpr u32 kPlayIdlesGlobal = 0xC0BC10;
inline constexpr u32 kVtOff = 0xE00;   // desplazamiento de las vtables de los objetos falsos en sus arenas

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this] (salvo indicacion), a[60..63] = semillas de las arenas A, B, C y D
//   kConst, kVoid: nada mas
//   kEvClone1: a[3] = dato (+kFieldOff), a[4] = modo del puntero del pool (0 crudo, 1 en D), a[5] = su valor crudo o desplazamiento en D, a[6] = memoria (0 nula, 1 en B), a[7] = su desplazamiento en B, a[8] = EAX del constructor, a[9] = dword que deja en [mem]
//   kEvKnock:  a[3..14] = dwords que el Clone lee (a[3] = +0x24, a[4] = +0x28, a[5] = +0x2C, a[6] = +0x30, a[7] = +0x38, a[8] = +0x34, a[9..11] = +0xC.., a[12..14] = +0x18..),
//              a[15] = modo del pool, a[16] = su valor, a[17] = memoria (0 nula, 1 en B), a[18] = desplazamiento en B, a[19] = EAX del constructor, a[20] = dword que deja en [mem]
//   kEvInAir:  a[3] = modo del pool, a[4] = su valor, a[5] = memoria (0 nula, 1 en B), a[6] = desplazamiento en B
//   kSdd:      a[3] = vtable previa, a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kPlayerCtor: a[3] = playerId (0..3), a[4] = bandera de grupo creado (0 o 1), a[5] = dword en +0x71C, a[6] = indice de grupo que devuelve AddGroup (0..7), a[7] = indice del bloque "playidles", a[8] = bits de la salud,
//              a[9] = desplazamiento de la inteligencia en C (+0x47C), a[10] = EAX de los falsos, a[11] = dword que deja el falso de CPed en +0, a[12] = idem en +4
//   kDuckCtor: a[3] = argumento (dword: solo cuenta la WORD baja), a[4] = tiempo (0xB7CB84)
//   kDuckBody: a[3] = dword previo en [this], a[5] = dword que deja el falso del cuerpo base en [this]
//   kMakeAbort: a[3] = desplazamiento del ped en B, a[4] = prioridad (pila, dword), a[5] = evento (0 nulo, 1 objeto en C), a[6] = su desplazamiento en C, a[7] = tipo que devuelve su GetEventType, a[8] = animacion de agacharse (+0x10),
//              a[9] = animacion de movimiento (+0x14), a[10] = EAX del falso de CTaskSimpleDuck::MakeAbortable, a[11] = dword en +0x46C del ped
//   kTaskClone: a[3] = dato (+0xC), a[5] = memoria (0 nula, 1 en B), a[6] = su desplazamiento en B, a[7] = EAX del constructor, a[8] = dword que deja en [mem]
//   kProcessPed: a[3] = desplazamiento del ped en B, a[4] = clump (+0x18 del ped), a[5] = animacion previa (+0x10), a[6] = lo que devuelve BlendAnimation, a[7] = EAX del falso de CTaskSimpleDuck::ProcessPed
inline void MakePool(Gen& g, C& c, unsigned mode, unsigned val) {
    auto& a = c.a;
    a[mode] = g.Pk({0u, 1u, 1u});
    a[val] = a[mode] ? AuxOffset(g, 0x20) : Raw(g);
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[kSeed] = c.seed * 13u + 50u; c.a[kSeed + 1] = c.a[kSeed] + 1u; c.a[kSeed + 2] = c.a[kSeed] + 2u; c.a[kSeed + 3] = c.a[kSeed] + 3u;
    c.a[kGSeed] = c.seed * 7u + 11u; c.a[kGSeed + 1] = c.a[kGSeed] + 5u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConst:
    case kVoid:
        break;
    case kEvClone1:
        a[3] = Raw(g);
        MakePool(g, c, 4, 5);
        a[6] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[7] = AuxOffset(g, 0x44);
        a[8] = Raw(g);
        a[9] = Raw(g);
        break;
    case kEvKnock:
        for (u32 i = 3; i <= 14; ++i) a[i] = g.P(70) ? g.W() : Raw(g);
        MakePool(g, c, 15, 16);
        a[17] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[18] = AuxOffset(g, 0x44);
        a[19] = Raw(g);
        a[20] = Raw(g);
        break;
    case kEvInAir:
        MakePool(g, c, 3, 4);
        a[5] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[6] = AuxOffset(g, 0x44);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kPlayerCtor:
        a[3] = g.R(4);
        a[4] = g.Pk({0u, 1u});
        a[5] = g.Pk({0u, 0u, 1u, Raw(g), NonZero(g), NonZero(g)});
        a[6] = g.R(8);
        a[7] = Raw(g);
        a[8] = FloatBits(g);
        a[9] = AuxIn(g, 0, 0x800, 0xD0);
        a[10] = Raw(g);
        a[11] = Raw(g); a[12] = Raw(g);
        break;
    case kDuckCtor:
        a[3] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, 0xFFFF0000u, 0xFFFFFFFFu, 0x8000u, 0x7FFFu, g.W(), g.W(), g.R(0x10000), g.R(2000)});
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), g.R(10000000)});
        break;
    case kDuckBody:
        a[5] = Raw(g);
        break;
    case kMakeAbort:
        a[3] = AuxIn(g, 0, 0xC00, 0x470);
        a[4] = g.Pk({2u, 2u, 2u, 0u, 1u, 3u, 0xFFFFFFFFu, 0x80000000u, 0x102u, Raw(g), g.R(8)});
        a[5] = g.Pk({0u, 1u, 1u, 1u});
        a[6] = AuxIn(g, 0, 0xC00, 8);
        a[7] = g.Pk({0x31u, 0x31u, 0x31u, 0x31u, 0x30u, 0x32u, 0u, 0x131u, 0x80000031u, 0xFFFFFFFFu, Raw(g), g.R(0x100)});
        a[8] = g.Pk({0u, 0u, NonZero(g), NonZero(g), Raw(g)});
        a[9] = g.Pk({0u, 0u, NonZero(g), NonZero(g), Raw(g)});
        a[10] = g.Pk({0u, 1u, 0x100u, 0xFFu, 0xFFFFFF00u, 0xFFFFFF01u, 0x80u, Raw(g), Raw(g)});
        a[11] = g.Pk({0u, 0xFFFFFFFFu, 0x04000000u, 0xFBFFFFFFu, g.W(), g.W(), g.W() | 0x04000000u, g.W() & 0xFBFFFFFFu});
        break;
    case kTaskClone:
        a[3] = Raw(g);
        a[5] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[6] = AuxOffset(g, 0x28);
        a[7] = Raw(g);
        a[8] = Raw(g);
        break;
    case kProcessPed:
        a[3] = AuxIn(g, 0, 0xC00, 0x1C);
        a[4] = g.Pk({0u, 1u, 1u});   // clump: nulo o un objeto RenderWare en D con tipo 2 (rpCLUMP; la DLL lo comprueba)
        a[8] = AuxOffset(g, 8);
        a[5] = g.Pk({0u, 0u, 0u, 1u, NonZero(g), NonZero(g), 0x100u});
        a[6] = g.Pk({0u, 1u, Raw(g), Raw(g), NonZero(g)});
        a[7] = g.Pk({0u, 1u, 0x100u, 0xFFu, 0xFFFFFF00u, 0xFFFFFF01u, 0x80u, Raw(g), Raw(g)});
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
    return j.End();
}
}
