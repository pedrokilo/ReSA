#pragma once
// Fixtures del banco Fn181A (lote 181A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas y enganchadas en el commit 5cdf45c8c8, SIN validar; las clases del listado salen del desensamblado, ver README.md y fixture_oracle.py). 29 funciones.
//   idx  direccion  nombre real                                                                 forma
//    0   0x407000   CEntity::IsInCurrentArea                                                     thiscall() `ret`: AL = (BYTE [this+2Fh] cero-extendido) == [0B72914h] o == 13
//    1   0x45AD40   CRemote::SetPlayerRemoteControlledCar                                        cdecl(vehiculo) `ret`: estados 3 / 8 / 9, referencias y camara
//    2   0x45AF40   CMatrix::ScaleAll                                                            thiscall(float) `ret 4`: x87 24 bits, 4 filas x 3 componentes
//    3   0x45AFB0   CPhysical::SetTurnSpeed                                                      thiscall(x, y, z) `ret 0Ch`: copia tres dwords a +50h
//    4   0x4AFCB0   CEventKnockOffBike::GetEventType (V)                                         mov eax,8
//    5   0x4AFCC0   CEventKnockOffBike::GetLifeTime (V)                                          xor eax,eax
//    6   0x4AFCD0   CEventKnockOffBike::GetLocalSoundLevel (V)                                   fld dword [858B34h] (60.0f en ST0)
//    7   0x4AFCE0   CEventKnockOffBike::GetEventPriority (V)                                     mov eax,46h
//    8   0x4B06F0   CEventKnockOffBike::IsCriminalEvent (V)                                       mov al,1
//    9   0x4B0810   CEventGunAimedAt::TakesPriorityOver (V)                                      thiscall(evento) `ret 4`
//   10   0x4B4EE0   CEventGunAimedAt::AffectsPed (V)                                             thiscall(ped) `ret 4`
//   11   0x5BD420   CVehicleAnimGroup::CVehicleAnimGroup                                         thiscall() `ret`: ceros, eax = this
//   12   0x5BD4E0   CDecisionMaker::operator=                                                    thiscall(origen) `ret 4`: 41 x CDecision::From (0x6006B0), eax = this
//   13   0x5BD510   CDecisionMakerTypes copia en +0xC1B4                                         thiscall(origen) `ret 4`
//   14   0x5BD540   idem +0xCB50
//   15   0x5BD570   idem +0xD4EC
//   16   0x5BD5A0   idem +0xDE88
//   17   0x5BD5D0   idem +0xE824
//   18   0x62FC80   CTaskComplexDieInCar::CTaskComplexDieInCar(arma)                               thiscall(arma) `ret 4`
//   19   0x62FCB0   CTaskComplexDieInCar::GetTaskType (V)                                        mov eax,0D7h
//   20   0x635F90   CTaskComplexDieInCar::Clone (V)                                              thiscall() `ret`, marco SEH
//   21   0x636010   CTaskSimpleDrownInCar::Clone (V)                                             thiscall() `ret`, marco SEH
//   22   0x636060   CTaskComplexDie::Clone (V)                                                   thiscall() `ret`, marco SEH, constructor de 9 argumentos
//   23   0x7316D0   CTxdStore: GetDefaultTxdSlot                                                  cdecl(indice) `ret`: movsx word [0C88004h + indice*2]
//   24   0x731800   CPool<TxdDef>::Flush                                                         thiscall() `ret`
//   25   0x731AD0   CPool<TxdDef>::CPool(capacidad, nombre)                                       thiscall(cap, nombre) `ret 8`
//   26   0x731B30   CPool<TxdDef>::Flush (segunda copia)                                          thiscall() `ret`
//   27   0x731B80   CPool<TxdDef>::New                                                           thiscall() `ret`
//   28   0x731C20   CPool<TxdDef> destructor con borrado                                          thiscall(flags) `ret 4`
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F181AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 29;
constexpr unsigned kArenas = 4;
constexpr unsigned kParams = 64;

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
    std::array<u32, kParams> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4, 1u, 1u, 0x101u});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline float F(u32 bits) { return std::bit_cast<float>(bits); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }
// Un float de magnitud moderada (coordenadas del mundo y desplazamientos): enteros, fracciones, valores alrededor de 3 (el cuadrado de la distancia se compara con 9) y alguno diminuto
inline u32 CoordBits(Gen& g) {
    const float sign = g.P(50) ? -1.0f : 1.0f;
    const u32 m = g.R(8);
    float v;
    if (m == 0) v = float(g.R(200000)) / 64.0f;
    else if (m == 1) v = float(g.R(6000));
    else if (m == 2) v = float(g.R(4000)) / 1000.0f;
    else if (m == 3) v = 3.0f + float(int(g.R(9)) - 4) / 1048576.0f;
    else if (m == 4) v = 1.7320508f + float(int(g.R(5)) - 2) / 8388608.0f;
    else if (m == 5) v = float(g.R(1000000)) / 3.0f;
    else if (m == 6) v = std::bit_cast<float>(0x00800000u + (g.W() & 0x0FFFFFFFu));
    else v = float(g.R(100)) / 7.0f;
    return B(sign * v);
}

// Bits de un float con exponente dado (sesgado 0..255) y mantisa
inline u32 ExpBits(u32 sign, u32 e, u32 mant) { return (sign ? 0x80000000u : 0u) | ((e & 0xFFu) << 23) | (mant & 0x7FFFFFu); }

// Clases de funcion (forma)
enum Kind : u32 { kConst, kConstF, kBool, kArea, kRemote, kScale, kTurn, kGunPrio, kGunAffects, kVag, kDm, kDieCtor, kCloneDie, kCloneDrown, kCloneComplex, kTxdSlot, kFlush, kPCtor, kPNew, kPSdd };
inline constexpr std::array<Kind, kFunctions> kKind{kArea, kRemote, kScale, kTurn, kConst, kConst, kConstF, kConst, kBool, kGunPrio, kGunAffects, kVag, kDm, kDm, kDm, kDm, kDm, kDm, kDieCtor, kConst,
                                                    kCloneDie, kCloneDrown, kCloneComplex, kTxdSlot, kFlush, kPCtor, kFlush, kPNew, kPSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x407000, 0x45AD40, 0x45AF40, 0x45AFB0, 0x4AFCB0, 0x4AFCC0, 0x4AFCD0, 0x4AFCE0, 0x4B06F0, 0x4B0810, 0x4B4EE0, 0x5BD420, 0x5BD4E0, 0x5BD510, 0x5BD540,
                                                    0x5BD570, 0x5BD5A0, 0x5BD5D0, 0x62FC80, 0x62FCB0, 0x635F90, 0x636010, 0x636060, 0x7316D0, 0x731800, 0x731AD0, 0x731B30, 0x731B80, 0x731C20};
// Bytes que cada funcion toca a partir de su puntero `this` (o del objeto)
inline constexpr std::array<u32, kFunctions> kSpan{0x30, 0x430, 0x40, 0x5C, 4, 4, 4, 4, 4, 0x18, 0x18, 0x80, 0x99C, 0x99C, 0x99C, 0x99C, 0x99C, 0x99C, 0x1C, 4, 0x1C, 8, 0x28, 4, 0x14, 0x14, 0x14, 0x14, 0x14};
// Desplazamiento (respecto a `this` del EXE) de la copia de decision maker que escribe cada funcion kDm
inline constexpr std::array<u32, kFunctions> kDmOffset{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xC1B4, 0xCB50, 0xD4EC, 0xDE88, 0xE824, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Memoria global del juego tocada
inline constexpr u32 kCurrArea = 0xB72914, kPlayerInFocus = 0xB7CD74, kPlayers = 0xB7CE48, kPlayersBytes = 0x320, kTxdSlots = 0xC87F00, kTxdSlotsBytes = 0x710, kTxdSlotsBase = 0xC88004;
inline constexpr u32 kPlayerStride = 0x190, kTxdIndexMin = 0xFFFFFFC0u, kTxdIndexMax = 0x2FF;
// Constantes de las funciones de retorno fijo
inline constexpr u32 ConstValue(u32 fn) { return fn == 4 ? 8u : (fn == 5 ? 0u : (fn == 7 ? 70u : (fn == 19 ? 215u : (fn == 8 ? 1u : 0x42700000u)))); }
// Disposicion fija dentro de la arena D de AffectsPed (el peaton esta en B, quien apunta en C)
inline constexpr u32 kDT1 = 0x40, kDT2 = 0x80, kDIntel = 0x100, kDMatrix = 0x200, kDVehicle = 0x300, kDPlayer = 0x900;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o del objeto) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[60..63] = semillas de las arenas A, B, C y D
//   kConst, kConstF, kBool, kVag: nada mas
//   kArea: a[4] = BYTE +0x2F, a[5] = CGame::currArea (dword)
//   kRemote: a[4] = jugador en foco (0/1), a[5] = vehiculo remoto actual (0 nulo, 1 el nuevo, 2 el de B), a[6] = desplazamiento del vehiculo de B, a[7] = primer FindPlayerVehicle (0 nulo, 1 el nuevo, 2 el de B,
//         3 el de C), a[8] = desplazamiento del vehiculo de C, a[9] = segundo FindPlayerVehicle (1, 2, 3), a[10..12] = BYTE +0x36, +0x428, +0x42D del vehiculo nuevo (en A), a[13] = BYTE +0x36 del de B, a[14] = el de C
//   kScale: a[4] = factor (bits), a[8..19] = 12 componentes (bits)
//   kTurn: a[4..6] = x, y, z (bits)
//   kGunPrio: a[4] = m_AimedBy (0 nulo, 1 en C), a[5] = desplazamiento en C, a[6] = desplazamiento del otro evento en B, a[7] = IsPlayer (dword), a[8] = origen del otro evento (0 nulo, 1 m_AimedBy, 2 otro en D, 3 crudo),
//         a[9] = desplazamiento en D, a[10] = crudo, a[11] = tipo del otro evento, a[12] = su prioridad, a[13] = prioridad propia, a[14] = valor de CEvent::TakesPriorityOver (dword)
//   kGunAffects: a[4] = m_AimedBy (0 nulo, 1 en C), a[5] = desplazamiento en C, a[6] = desplazamiento del peaton en B, a[7] = IsPlayer (dword), a[8] = primera tarea activa (0 nula, 1 T1, 2 T2), a[9] = segunda (1 o 2),
//         a[10] = tipo de T1, a[11] = tipo de T2, a[12] = IsInSeeingRange (dword), a[13] = IsAlive, a[14] = AreInSameGroup, a[15] = IsPassenger, a[16] = dword +0x46C, a[17] = vehiculo (0 nulo, 1 en D),
//         a[18] = dword +0x460 del vehiculo (conductor), a[19] = tipo de peaton (+0x598), a[20] = matriz de quien apunta (0 nula, 1 en D), a[21] = FindPlayerPed (0 nulo, 1 en D, 2 crudo), a[22] = crudo,
//         a[23..25] = posicion (+4) de quien apunta, a[26..28] = posicion de la matriz (+0x30)
//   kDm: a[4] = desplazamiento del origen en B, a[5] = valor de retorno de los falsos de CDecision::From
//   kDieCtor: a[4] = arma, a[5] = dword que deja el falso del constructor base, a[6] = su retorno
//   kCloneDie: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = arma (+0xC), a[7] = arma tras la reserva, a[8] = cambiar el arma tras la reserva (0/1), a[9] = dword del falso, a[10] = su retorno
//   kCloneDrown: a[4], a[5], a[9], a[10] como kCloneDie
//   kCloneComplex: a[4], a[5] como kCloneDie; a[6..12] = dwords +0xC, +0x10, +0x14, +0x18, +0x1C, +0x20, +0x24 de la tarea; a[13] = cambiar un campo tras la reserva (0/1), a[14] = cual (0..6), a[15] = valor nuevo,
//         a[16] = retorno del falso del constructor
//   kTxdSlot: a[4] = indice (con signo, -64 .. 0x2FF)
//   kFlush / kPSdd / kPCtor / kPNew (elemento de 0xC bytes): kFlush: a[4] = almacenamiento (0 nulo, 1 en B, 2 crudo), a[5] = desplazamiento en B, a[6] = crudo, a[7] = mapa (0 nulo, 1 en C, 2 crudo),
//         a[8] = desplazamiento en C, a[9] = crudo, a[10] = capacidad (con signo), a[11] = primer libre, a[12] = BYTE +0x10; kPSdd: ademas a[13] = banderas de pila; kPCtor: a[4..6] primer new, a[7..9] segundo,
//         a[10] = capacidad, a[11] = nombre; kPNew: a[4] = almacenamiento (crudo), a[5] = mapa en D, a[6] = capacidad, a[7] = primer libre, a[8] = BYTE +0x10, a[9] = modo del mapa, a[10] = indice libre unico,
//         a[11] = estilo del contador
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[60] = c.seed * 13u + 50u; c.a[61] = c.a[60] + 1u; c.a[62] = c.a[60] + 2u; c.a[63] = c.a[60] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85AAB4u, 0x8710D0u});
    auto& a = c.a;
    const auto dw = [&] { return g.Pk({0u, 1u, 1u, 1u, 0x100u, 0xFFFFFF01u, 0xFFFFFF00u, 0x12345601u, 0x12345600u, 0x101u}); };   // dword cuyo byte bajo manda (AL del falso)
    switch (kKind[c.fn]) {
    case kConst: case kConstF: case kBool: case kVag:
        break;
    case kArea: {
        a[4] = g.Pk({0u, 1u, 2u, 13u, 0xFFu, 0x80u, 0xFEu, g.R(256), g.R(256), g.R(30), 0x7Fu});
        a[5] = g.Pk({0u, 1u, 2u, 13u, 0xFFu, 0xFFFFFFFFu, a[4], a[4], a[4], a[4], 0xFFFFFF00u | a[4], 0xFFFFFF00u | a[4], 0xFFFFFF00u | a[4], 0x100u | a[4], g.R(30), g.W(), 0xFFFFFFFEu});
        break;
    }
    case kRemote: {
        a[4] = g.Pk({0u, 0u, 1u});
        a[5] = g.Pk({0u, 1u, 2u, 2u});
        a[6] = AuxOffset(g, 0x430);
        a[7] = g.Pk({0u, 1u, 2u, 3u, 3u});
        a[8] = AuxOffset(g, 0x430);
        a[9] = g.Pk({1u, 2u, 3u});
        a[10] = g.Pk({0u, 0xFFu, g.R(256), g.R(256), g.R(256)});
        a[11] = g.Pk({0u, 0xFFu, 0xEFu, 0x10u, 0x08u, g.R(256), g.R(256)});
        a[12] = g.Pk({0u, 2u, 0xFFu, 0xFDu, 4u, g.R(256), g.R(256)});
        a[13] = g.R(256); a[14] = g.R(256);
        break;
    }
    case kScale: {
        const u32 mode = g.R(8);   // 0 coordenadas, 1 y 2 producto cerca del limite de los denormales, 3 cerca del desbordamiento, 4 con NaN / infinitos, 5 factor cero / uno / menos uno, 6 y 7 aleatorio
        a[4] = mode == 5 ? g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u, 0x3F000000u}) : (mode == 6 ? FloatBits(g) : CoordBits(g));
        const u32 fe = (a[4] >> 23) & 0xFFu;
        for (u32 i = 0; i < 12; ++i) {
            u32 v = CoordBits(g);
            if (mode == 1 || mode == 2) { const i32 e = i32(128 - i32(fe) + i32(g.R(41)) - 20); v = ExpBits(g.P(50), u32(std::clamp(e, 1, 254)), g.W()); }
            else if (mode == 3) { const i32 e = i32(254 - i32(fe) + i32(g.R(11)) - 4); v = ExpBits(g.P(50), u32(std::clamp(e, 1, 254)), g.W()); }
            else if (mode == 4) { if (g.P(35)) v = FloatBits(g); if (g.P(12)) a[4] = FloatBits(g); }
            else if (mode == 6 || mode == 7) v = g.P(70) ? FloatBits(g) : v;
            a[8 + i] = v;
        }
        break;
    }
    case kTurn:
        for (u32 i = 0; i < 3; ++i) a[4 + i] = g.P(70) ? FloatBits(g) : g.W();
        break;
    case kGunPrio: {
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x40); a[6] = AuxOffset(g, 0x40);
        a[7] = dw();
        a[8] = g.Pk({0u, 1u, 1u, 2u, 3u});
        a[9] = AuxOffset(g, 0x40); a[10] = Raw(g);
        a[11] = g.Pk({9u, 36u, 39u, 0u, 1u, 8u, 100u, 0xFFFFFFFFu, g.W(), 9u, 36u, 39u, 10u});
        a[12] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.R(100), g.R(100), g.R(100), g.W(), 70u});
        a[13] = g.P(35) ? g.Pk({a[12], a[12], a[12] + 1u, a[12] - 1u}) : g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.R(100), g.R(100), g.W(), 45u});
        a[14] = dw();
        break;
    }
    case kGunAffects: {
        a[4] = g.P(85) ? 1u : 0u;
        a[5] = AuxOffset(g, 0x40);
        a[6] = AuxOffset(g, 0x5A0);
        a[7] = g.P(10) ? dw() : 0u;
        a[8] = g.Pk({0u, 0u, 1u, 2u, 2u});
        a[9] = g.Pk({1u, 2u});
        const auto ty = [&] { return g.Pk({0x259u, 0x259u, 0x259u, 0u, 1u, 0x258u, 0x25Au, g.W(), 0xFFFFFFFFu}); };
        a[10] = ty(); a[11] = ty();
        a[12] = g.P(80) ? g.Pk({1u, 1u, 0x101u, 0xFFFFFF01u, 0x12345601u}) : g.Pk({0u, 0x100u, 0xFFFFFF00u});
        a[13] = g.P(80) ? g.Pk({1u, 1u, 0x101u, 0xFFFFFF01u}) : g.Pk({0u, 0x100u, 0xFFFFFF00u});
        a[14] = g.P(80) ? g.Pk({0u, 0u, 0x100u, 0xFFFFFF00u}) : g.Pk({1u, 0x101u, 0xFFFFFF01u});
        a[15] = g.Pk({0u, 1u, 1u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u});
        a[16] = g.Pk({0x100u, 0x100u, 0u, 0xFFFFFFFFu, 0xFFFFFEFFu, g.W() | 0x100u, g.W() & ~0x100u, 0x1FFu});
        a[17] = g.Pk({0u, 1u, 1u});
        a[18] = g.Pk({0u, 0u, Raw(g), g.W()});
        a[19] = g.Pk({6u, 6u, 6u, 6u, 0u, 1u, 5u, 7u, 0x106u, 0xFFFFFFFAu, g.W()});
        a[20] = g.Pk({0u, 1u, 1u});
        a[21] = g.Pk({0u, 1u, 2u});
        a[22] = Raw(g);
        for (u32 i = 0; i < 6; ++i) a[23 + i] = CoordBits(g);
        break;
    }
    case kDm:
        a[4] = AuxOffset(g, 0x99C);
        a[5] = Raw(g);
        break;
    case kDieCtor:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kCloneDie:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x1C);
        a[6] = Raw(g); a[7] = Raw(g); a[8] = g.Pk({0u, 1u, 1u});
        a[9] = Raw(g); a[10] = Raw(g);
        break;
    case kCloneDrown:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 8);
        a[9] = Raw(g); a[10] = Raw(g);
        break;
    case kCloneComplex:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x28);
        for (u32 i = 0; i < 7; ++i) a[6 + i] = (i == 3 || i == 4) ? FloatBits(g) : (i == 5 ? g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, g.W(), 0xFFFFFFFFu, 0x100u, 0xFFFFFF07u}) : Raw(g));
        a[13] = g.Pk({0u, 1u, 1u});
        a[14] = g.R(7);
        a[15] = (a[14] == 3 || a[14] == 4) ? FloatBits(g) : g.W();
        a[16] = Raw(g);
        break;
    case kTxdSlot:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, kTxdIndexMin, 0x2FFu, g.R(0x300), g.R(0x300), g.R(0x300), 0u - g.R(64), 0u - g.R(64)});
        break;
    case kFlush:
    case kPSdd: {
        a[4] = g.Pk({0u, 1u, 1u, 2u}); a[5] = AuxOffset(g, 0x10); a[6] = Raw(g);
        a[7] = g.Pk({0u, 1u, 1u, 2u}); a[8] = AuxOffset(g, 0x10); a[9] = Raw(g);
        a[10] = g.Pk({0u, 1u, 2u, 0xCu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, g.R(0x400), g.R(0x400), g.W(), g.R(100) + 1u, 0x100u});
        a[11] = g.Pk({0u, 0xFFFFFFFFu, 5u, g.W(), g.W(), g.R(100)});
        a[12] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, g.R(256), 0x7Fu});
        a[13] = FlagsDword(g);
        break;
    }
    case kPCtor: {
        a[10] = g.Pk({0u, 1u, 2u, 3u, 7u, 16u, 100u, 0x100u, 0x200u, 0x3FFu, 0x400u, g.R(0x400) + 1u, g.R(0x400) + 1u, g.R(0x400) + 1u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x80000001u, 0xFFFFFF00u,
                      0xFFFFFFF0u, 0xC0000000u, g.W() | 0x80000000u});
        const bool pos = i32(a[10]) > 0;
        a[4] = g.Pk({0u, 1u, 1u, 1u, 2u}); a[5] = AuxOffset(g, 0x10); a[6] = Raw(g);
        a[7] = pos ? 1u : g.Pk({0u, 1u, 2u}); a[8] = AuxOffset(g, pos ? (a[10] + 3u) & ~3u : 4u); a[9] = Raw(g);
        a[11] = Raw(g);
        break;
    }
    case kPNew: {
        a[4] = Raw(g);
        a[6] = g.Pk({1u, 2u, 3u, 5u, 8u, 16u, 30u, 64u, 100u, 128u, 255u, 300u, 1u, 2u, g.R(300) + 1u, g.R(300) + 1u});
        a[5] = AuxOffset(g, a[6] + 4u);
        a[7] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, a[6] - 1u, a[6] - 1u, a[6] - 2u, g.R(a[6]), g.R(a[6]), g.R(a[6])});
        if (i32(a[7]) < -1) a[7] = 0xFFFFFFFFu;
        a[8] = g.Pk({0u, 1u, 1u, 0x80u, 0xFFu});
        a[9] = g.R(5);
        a[10] = g.Pk({0u, a[6] - 1u, g.R(a[6]), g.R(a[6]), g.R(a[6]), (a[7] + 1u) % a[6], (a[7] + 2u) % a[6]});
        a[11] = g.R(3);
        break;
    }
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
