#pragma once
// Fixtures del banco Fn180A (lote 180A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 22 asignadas: 22 reconstruidas.
//   idx  direccion  nombre real                                                                      forma
//    0   0x4D6CC0   CAEAmbienceTrackManager::StopAmbienceTrack                                       thiscall() `ret`: estado (+8) 0..3 -> 4; bytes +0, +1, +2 a cero
//    1   0x4D6DA0   CAEAmbienceTrackManager::StartTrackPlayback                                      thiscall() `ret` (salto final): DisableBassEq, SetChannelVolume, SetChannelFlags, StartTrackPlayback del hardware 0xB5F8B8
//    2   0x4D6DF0   CAEAmbienceTrackManager::CheckForPause                                           thiscall() `ret`: factor (0 con pausa o menu) y dos SetChannelFrequencyScalingFactor
//    3   0x4D7890   CAEAudioChannel::Constructor (constructor de la base)                            thiscall(4 dwords) `ret 10h`: formato PCM, vtable 0x85F03C, devuelve this
//    4   0x4D7910   CAEAudioChannel::DestructorBody                                                   thiscall() `ret`: vtable, contador 0xB5F898, Release de +8 y de +0xC
//    5   0x59EEB0   CHandObject::Constructor                                                          thiscall(modelo, ped, izquierda) `ret 0Ch`: constructor de CObject 5A1D10, vtable 0x866EE0, colaboradores de RenderWare
//    6   0x59EFE0   CHandObject destructor con borrado                                                thiscall(flags) `ret 4`: cuerpo de CObject (59F000 -> 59F660), delete 5A1F20 si bit 0
//    7   0x59F050   CObject::IsCraneMovingPartModel                                                   cdecl(indice) `ret`: cuatro WORD globales de modelos de la grua
//    8   0x648E90   CTaskSimpleCarFallOut::ComputeAnimIDExe                                           thiscall(grupo*, anim*) `ret 8`: puerta 10/11 -> 0x183, otra -> 0x184; GetGroup 6E3B00
//    9   0x64CD60   CTaskSimpleSetPedAsAutoDriver destructor con borrado                              thiscall(flags) `ret 4`: cuerpo 648FC0; delete 61A5B0
//   10   0x64CD80   CTaskComplexGoToBoatSteeringWheel destructor con borrado                          thiscall(flags) `ret 4`: cuerpo 649100; delete
//   11   0x64CDA0   CTaskComplexGoToBoatSteeringWheel::CreateSubTask                                   thiscall(tipo) `ret 4`: 0xCB StandStill, 0x387 ComputeTargetPos + GoToPointAndStandStill
//   12   0x64CE80   CTaskComplexGetOnBoatSeat destructor con borrado                                   thiscall(flags) `ret 4`: cuerpo 649280; delete
//   13   0x6557A0   CTaskSimpleHitHead::Clone                                                           thiscall() `ret`: new(10h) 61A5A0; ctor base 61A390; vtable 0x86F1D0
//   14   0x655DB0   CTaskSimpleHitFromBehind destructor con borrado                                    thiscall(flags) `ret 4`: cuerpo 655DD0 (jmp 61A6F0); delete
//   15   0x655DE0   CTaskSimpleHitWall destructor con borrado                                           thiscall(flags) `ret 4`: cuerpo 655E00 (jmp 61A6F0); delete
//   16   0x655E30   CTaskSimpleHitHead destructor con borrado                                          thiscall(flags) `ret 4`: cuerpo 653090; delete
//   17   0x655E80   CTaskSimpleEvasiveStep destructor con borrado                                      thiscall(flags) `ret 4`: cuerpo 6531D0; delete
//   18   0x6586A0   CTaskSimpleAbseil::Constructor                                                     thiscall() `ret`: 61A900(0, 98h, 4.0f, 1A7h, "Abseil", 0); vtable 0x86F45C
//   19   0x6586D0   CTaskSimpleAbseil::Clone                                                           thiscall() `ret`: new(20h); mismo constructor en linea
//   20   0x668D50   CTaskComplexGoToPointAndStandStillAndAchieveHeading::CreateSubTask                  thiscall(tipo) `ret 4`: 0x384, 0xCB, 0x386
//   21   0x66E050   CTaskComplexFollowPointRoute::DestructorBody                                        thiscall() `ret`: vtable 0x870078; delete 41B7D0 de +0x20 si no es nulo; cuerpo de CTaskComplex 61A3D0
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

namespace F180AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;
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
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un dword cuyo WORD bajo es el canal (o el id): ceros, bordes de 16 bits y aleatorios; la mitad alta es basura (el EXE solo lee el WORD)
inline u32 ChanDword(Gen& g) {
    const u32 lo = g.Pk({0u, 1u, 2u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu, g.R(0x10000), g.R(0x10000), g.R(64), 0x1234u});
    const u32 hi = g.Pk({0u, 0u, 0xFFFFu, 0x8000u, 0x0001u, g.R(0x10000), g.R(0x10000)});
    return lo | (hi << 16);
}
// Contador de canales (0xB5F898)
inline u32 CounterBits(Gen& g) { return g.Pk({0u, 1u, 2u, 5u, 64u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, g.W(), g.R(100)}); }

// Clases de funcion (forma)
enum Kind : u32 { kStop, kStartTP, kChkPause, kChanCtor, kChanDtor, kHandCtor, kSddObj, kCrane, kAnimId, kSdd, kSubBoat, kHitClone, kAbCtor, kAbClone, kSubGo, kRoute };
inline constexpr std::array<Kind, kFunctions> kKind{kStop, kStartTP, kChkPause, kChanCtor, kChanDtor, kHandCtor, kSddObj, kCrane, kAnimId, kSdd, kSdd, kSubBoat, kSdd, kHitClone, kSdd, kSdd, kSdd, kSdd,
                                                    kAbCtor, kAbClone, kSubGo, kRoute};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4D6CC0, 0x4D6DA0, 0x4D6DF0, 0x4D7890, 0x4D7910, 0x59EEB0, 0x59EFE0, 0x59F050, 0x648E90, 0x64CD60, 0x64CD80, 0x64CDA0, 0x64CE80, 0x6557A0, 0x655DB0, 0x655DE0,
                                                    0x655E30, 0x655E80, 0x6586A0, 0x6586D0, 0x668D50, 0x66E050};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x85AAB4, 0x85AAB4, 0x85AAB4, 0x85F03C, 0x85F03C, 0x866EE0, 0x866EE0, 0, 0x86EFF4, 0x86EFF4, 0x86F018, 0x86F018, 0x86F044, 0x86F1D0, 0x86F168, 0x86F1A0, 0x86F1D0, 0x86F1F4,
                                                     0x86F45C, 0x86F45C, 0x86F0A8, 0x870078};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0xC, 0x1C, 0x20, 0x60, 0x10, 0x18C, 4, 4, 0x18, 4, 4, 0x24, 4, 4, 4, 4, 4, 4, 4, 4, 0x2C, 0x24};
// Entradas de la tabla de model infos y del pool de TXD (globales que toca CHandObject::Constructor)
inline constexpr u32 kModelTable = 0xA9B0C8, kModelCount = 20000, kTxdPoolPtr = 0xC8800C;
inline constexpr u32 kCraneWords[4] = {0x8CD700, 0x8CD6FC, 0x8CD740, 0x8CD71C};   // iman, arnes, mini iman, bola
inline constexpr u32 kCraneBase = 0x8CD6FC, kCraneBytes = 0x48;
inline constexpr u32 kPauseUser = 0xB7CB49, kPauseCode = 0xB7CB48, kMenuSwitch = 0xBA6831, kChannelCounter = 0xB5F898;
inline constexpr u32 kVtOff = 0xE00;   // desplazamiento de las vtables de los COM falsos en sus arenas
// Zonas de la arena D del constructor de CHandObject: pool de TXD (estructura en 0xC00 con la capacidad en +8; objetos de 12 bytes desde 0x600; estados desde 0x100)
inline constexpr u32 kPoolStruct = 0xC00, kPoolObjects = 0x600, kPoolFlags = 0x100;
// Objetos RenderWare falsos (arena D) para los clumps: el assert de CEntity::GetRpClump de la DLL desreferencia el puntero (el EXE no), asi que el primer byte (tipo) es rpCLUMP = 2
inline constexpr u32 kClumpPed = 0xD00, kClumpNew = 0xD40;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this] (salvo indicacion), a[60..63] = semillas de las arenas A, B, C y D
//   kStop:     a[3] = dword en +0 (los bytes +1 y +2 se ponen a cero), a[4] = estado (+8)
//   kStartTP:  a[3] = dword en +4 (canal: WORD), a[4] = volumen (+0x18, bits crudos), a[5] = EAX de los falsos, a[6] = dword que el falso de SetChannelVolume deja en +4, a[7] = lo modifica (1) o no (0)
//   kChkPause: a[3] = dword en +4 (canal: WORD), a[4] = factor (+0x1C), a[5] = byte 0xB7CB49, a[6] = byte 0xB7CB48, a[7] = byte 0xBA6831, a[8] = dword que el primer falso deja en +4, a[9] = dword que deja en +0x1C,
//              a[10] = el primer falso modifica (1) o no (0) el objeto, a[11] = EAX de los falsos
//   kChanCtor: a[3] = DirectSound (crudo), a[4] = id de canal (dword; solo cuenta el WORD bajo), a[5] = frecuencia, a[6] = bits por muestra (dword; solo cuenta el WORD bajo)
//   kChanDtor: a[3] = vtable previa, a[4] = modo del +8 (0 nulo, 1 COM en B, 2 COM en C), a[5] = desplazamiento de ese COM, a[6] = modo de +0xC (0 nulo, 1 B, 2 C, 3 el mismo que +8), a[7] = su desplazamiento,
//              a[8] = lo que deja el primer Release en +0xC (0 nada, 1 cero, 2 el COM de la arena D), a[9] = EAX de Release, a[10] = contador global 0xB5F898, a[11] = desplazamiento del COM de la arena D
//   kHandCtor: a[3] = dword previo en [this], a[4] = indice de modelo de la mano (< 20000), a[5] = izquierda (dword de pila: solo cuenta el BYTE bajo), a[6] = indice de modelo del ped (WORD en ped+0x22), a[7] = clump del ped (+0x18: 0 nulo, 1 objeto RenderWare falso de la arena D),
//              a[8] = EAX de GetAnimHierarchyFromSkinClump, a[9] = EAX de RpHAnimIDGetIndex, a[10] = clump que deja el falso de SetModelIndex en +0x18 (0 nulo, 1 otro objeto falso de la arena D, 2 el mismo que el del ped), a[11] = EAX de la busqueda de textura (0 = no encontrada),
//              a[12] = EAX de GetFirstTexture, a[13] = diccionario del TXD (primer dword del objeto del pool), a[14] = indice de TXD (WORD en modelinfo+0xA, 0..63), a[15] = WORD de +8 (basura), a[16] = clave (modelinfo+4),
//              a[17] = desplazamiento del ped en B, a[18] = desplazamiento del model info del ped en C, a[19] = el de la mano en C, a[20] = reparto (0 separados, 1 la mano usa el objeto del ped, 2 mismo indice),
//              a[21] = byte de estado del objeto del pool (bit 7 siempre a cero), a[24] = XOR del falso de SetModelIndex sobre +0x1C, a[25] = XOR sobre +0x34, a[26] = dword que dejan los falsos de CObject en [this],
//              a[27] = XOR del falso de SetIsStatic sobre +0x1C, a[28] = EAX de los demas falsos
//   kSddObj, kSdd: a[3] = vtable previa, a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kCrane:    a[3] = argumento (dword de pila), a[4..7] = WORD de iman, arnes, mini iman y bola
//   kAnimId:   a[3] = puerta (+0x14), a[4] = desplazamiento del vehiculo en B, a[5] = el del manejo en C, a[6] = byte +0xDE del manejo, a[7] = EAX de GetGroup, a[8] = desplazamiento de la salida del indice en D,
//              a[9] = el de la salida del grupo en D, a[10] = 1 si ambas salidas son la misma
//   kSubBoat:  a[3] = tipo (pila), a[4] = memoria (0 nula, 1 en B), a[5] = su desplazamiento en B, a[6] = EAX de los constructores, a[7] = dword que dejan en [mem], a[8..10] = lo que escribe el falso de ComputeTargetPos en +0xC..+0x14
//   kHitClone, kAbClone: a[3] = memoria (0 nula, 1 en B), a[4] = su desplazamiento, a[5] = EAX del constructor, a[6] = dword que deja en [mem]
//   kAbCtor:   a[3] = dword previo en [this], a[4] = EAX del constructor falso, a[5] = dword que deja en [this]
//   kSubGo:    a[3] = tipo (pila), a[4] = memoria (0 nula, 1 en B), a[5] = su desplazamiento, a[6] = EAX de los constructores, a[7] = dword que dejan en [mem], a[8] = estado de movimiento (+0xC), a[9..11] = posicion (+0x10),
//              a[12] = radio (+0x1C), a[13..15] = +0x20, +0x24, +0x28
//   kRoute:    a[3] = vtable previa, a[4] = ruta (0 nula, 1 en B), a[5] = su desplazamiento, a[6] = dword que deja el falso del cuerpo base en [this], a[7] = EAX de los falsos
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[kSeed] = c.seed * 13u + 50u; c.a[kSeed + 1] = c.a[kSeed] + 1u; c.a[kSeed + 2] = c.a[kSeed] + 2u; c.a[kSeed + 3] = c.a[kSeed] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kStop:
        a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x01010101u, 0x00FFFFFFu, 0xFF000000u, 0x00000100u, 0x00010000u, 0x01000000u, g.W(), g.W()});
        a[4] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, 4u, 4u, 5u, 6u, 7u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, g.W(), g.R(8)});
        break;
    case kStartTP:
        a[3] = ChanDword(g);
        a[4] = FloatBits(g);
        a[5] = Raw(g);
        a[6] = ChanDword(g);
        a[7] = g.Pk({0u, 1u, 1u});
        break;
    case kChkPause:
        a[3] = ChanDword(g);
        a[4] = FloatBits(g);
        a[5] = g.Pk({0u, 0u, 0u, 0u, 1u, 0xFFu, 0x80u, g.R(256)});
        a[6] = g.Pk({0u, 0u, 0u, 0u, 1u, 0xFFu, 0x80u, g.R(256)});
        a[7] = g.Pk({0u, 0u, 0u, 0u, 1u, 0xFFu, 0x80u, g.R(256)});
        a[8] = ChanDword(g);
        a[9] = FloatBits(g);
        a[10] = g.Pk({0u, 1u, 1u});
        a[11] = Raw(g);
        break;
    case kChanCtor:
        a[3] = Raw(g);
        a[4] = g.Pk({0u, 1u, 2u, 0xFFFFu, 0x10000u, 0x10001u, 0xFFFFFFFFu, 0x12345678u, g.W(), g.R(64)});
        a[5] = g.Pk({0u, 1u, 11025u, 22050u, 44100u, 48000u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.R(100000), g.W()});
        a[6] = g.Pk({0u, 1u, 7u, 8u, 9u, 16u, 24u, 32u, 0xFFFFu, 0x10000u, 0x10008u, 0x10010u, 0xFFFFFFFFu, g.W(), g.R(64), 16u, 8u});
        break;
    case kChanDtor:
        a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85F3CCu, 0x85F03Cu, 0x85F3CCu, 0xDEADBEEFu, g.W()});
        a[4] = g.Pk({0u, 1u, 1u, 2u, 2u});
        a[5] = AuxIn(g, 0, 0xC00, 0x20);
        a[6] = g.Pk({0u, 1u, 2u, 3u, 3u});
        a[7] = AuxIn(g, 0, 0xC00, 0x20);
        a[8] = g.Pk({0u, 0u, 1u, 2u, 2u});
        a[9] = Raw(g);
        a[10] = CounterBits(g);
        a[11] = AuxIn(g, 0, 0xC00, 0x20);
        break;
    case kHandCtor: {
        a[4] = g.Pk({0u, 1u, 19999u, 400u, 500u, 595u, g.R(kModelCount), g.R(kModelCount), g.R(kModelCount), 0x4000u, 0x3FFFu});
        a[5] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80u, g.W(), g.W()});
        a[6] = g.Pk({0u, 1u, 19999u, 0u, 7u, 100u, g.R(kModelCount), g.R(kModelCount), g.R(kModelCount), 0x7FFFu});
        a[7] = g.Pk({0u, 1u, 1u, 1u});
        a[8] = Raw(g);
        a[9] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.R(64), 5u});
        a[10] = g.Pk({0u, 1u, 2u, 2u});
        a[11] = g.Pk({0u, 0u, 0u, 1u, 0xDEADBEEFu, g.W()});
        a[12] = Raw(g);
        a[13] = Raw(g);
        {
            a[14] = g.Pk({0u, 1u, 2u, 63u, 5u, g.R(64), g.R(64), g.R(64)});   // indice de TXD valido en el pool (la DLL comprueba 0 <= indice < capacidad)
        }
        a[15] = g.Pk({0u, 0xFFFFu, g.R(0x10000), g.R(0x10000), 1u});
        a[16] = Raw(g);
        a[17] = AuxIn(g, 0, 0x1000, 0x30);
        a[18] = AuxIn(g, 0x000, 0x200, 0x20);
        a[19] = AuxIn(g, 0x200, 0x200, 0x20);
        a[20] = g.Pk({0u, 0u, 0u, 1u, 2u});
        if (a[20] == 2) a[4] = a[6];
        else if (a[4] == a[6]) a[4] = (a[6] + 1) % kModelCount;
        a[21] = g.R(0x80);
        a[24] = g.Pk({0u, 0u, 1u, 0x08000000u, 0x10000400u, 0xFFFFFFFFu, g.W()});
        a[25] = g.Pk({0u, 0u, 0x00070000u, 0x00FF0000u, 0xFFFFFFFFu, g.W()});
        a[26] = g.Pk({0u, 0xFFFFFFFFu, 0x866F60u, 0x868A60u, 0xDEADBEEFu, g.W()});
        a[27] = g.Pk({0u, 0x08000000u, 0x00000001u, 0x10000400u, 0xFFFFFFFFu, 0xF7FFFFFFu, g.W()});
        a[28] = Raw(g);
        break;
    }
    case kSddObj:
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kCrane: {
        const auto word = [&] { return g.Pk({0xFFFFu, 0xFFFFu, g.R(0x10000), g.R(20000), 0u, 1u, g.R(600), 0x8000u, 0x7FFFu}); };
        for (u32 i = 4; i <= 7; ++i) a[i] = word();
        const u32 w = a[4 + g.R(4)];
        a[3] = g.Pk({w, w, w, a[4 + g.R(4)], a[4 + g.R(4)], 0xFFFFFFFFu, 0xFFFFu, 0u, g.R(20000), g.W(), w | 0x10000u, w | 0x80000000u, w | 0xFFFF0000u, 1u, 0x10000u});
        break;
    }
    case kAnimId:
        a[3] = g.Pk({10u, 11u, 10u, 11u, 0u, 1u, 2u, 9u, 12u, 0xFFFFFFFFu, 0x8000000Au, 0x1000000Bu, g.W(), g.R(20)});
        a[4] = AuxIn(g, 0, 0x1000, 0x388);
        a[5] = AuxIn(g, 0, 0x1000, 0xE0);
        a[6] = g.Pk({0u, 1u, 0x7Fu, 0x80u, 0xFFu, g.R(256), g.R(256), g.W()});
        a[7] = Raw(g);
        a[8] = AuxIn(g, 0, 0x1000, 4);
        a[9] = AuxIn(g, 0, 0x1000, 4);
        a[10] = g.Pk({0u, 0u, 0u, 1u});
        if (a[10] == 0 && (a[9] + 4 > a[8] && a[8] + 4 > a[9])) a[9] = a[8] >= 8 ? a[8] - 8 : a[8] + 8;   // salidas distintas: sin solaparse
        break;
    case kSubBoat:
        a[3] = g.Pk({0xCBu, 0x387u, 0x516u, 0xCBu, 0x387u, 0u, 1u, 0x386u, 0x384u, 0x1CBu, 0x80000387u, g.W(), g.R(0x800)});
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x28);
        a[6] = Raw(g);
        a[7] = Raw(g);
        for (u32 i = 8; i <= 10; ++i) a[i] = FloatBits(g);
        break;
    case kHitClone:
        a[3] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[4] = AuxOffset(g, 0x10);
        a[5] = Raw(g);
        a[6] = Raw(g);
        break;
    case kAbCtor:
        a[4] = Raw(g);
        a[5] = Raw(g);
        break;
    case kAbClone:
        a[3] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[4] = AuxOffset(g, 0x20);
        a[5] = Raw(g);
        a[6] = Raw(g);
        break;
    case kSubGo:
        a[3] = g.Pk({0x384u, 0xCBu, 0x386u, 0x516u, 0x384u, 0xCBu, 0x386u, 0u, 0x387u, 0x1CBu, 0x80000384u, g.W(), g.R(0x800)});
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x30);
        a[6] = Raw(g);
        a[7] = Raw(g);
        a[8] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, g.W()});
        for (u32 i = 9; i <= 11; ++i) a[i] = FloatBits(g);
        for (u32 i = 12; i <= 15; ++i) a[i] = FloatBits(g);
        break;
    case kRoute:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 4);
        a[6] = Raw(g);
        a[7] = Raw(g);
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
