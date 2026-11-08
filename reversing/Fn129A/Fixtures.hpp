#pragma once
// Fixtures del banco Fn129A (lote 129A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Catorce funciones del EXE de 31 a 58 bytes. Las clases del listado de pendientes se atribuian por cercania: las reales salen del desensamblado, de las vtables y de los llamadores
// (ver README.md y fixture_oracle.py).
//    0 CRGBA::Copy                              0x463370  thiscall (const CRGBA*), `ret 4`; copia 4 bytes uno a uno (carga/escritura alternadas); devuelve this
//    1 CEventEscalator::TakesPriorityOver (V)   0x4B25E0  thiscall (const CEvent&), `ret 4`, ranura 11 de 0x86CD00; tipo(other) y despues tipo(this) por la ranura 1; AL = distintos
//    2 CAEPedAudioEntity::SurfaceVolumeDb       0x4DA510  cdecl (byte); ST0 = 20 * log10(fild [0x8AD914 + b*16] * 0.01f) con la x87 a 24 bits
//    3 CVehicle::SetEngineOn                    0x41BDD0  thiscall (byte), `ret 4`; bit 4 del byte +0x428 = motor roto (bit 1 de +0x42D) ? 0 : bit 0 del argumento
//    4 CAEMP3BankLoader::GetLoopOffset          0x4E0380  thiscall (sonido, slot), `ret 8`; sin inicializar -> -1; si no el dword del slot (int16 con signo) y sonido (uint16)
//    5 CAccidentManager::Constructor            0x56CE30  thiscall; 16 entradas de 8 bytes (iterador del CRT 0x821D30 real + 0x56CDE0 real): dword +0 y bytes +4 y +5 a cero
//    6 CAESmoothFadeThread::Constructor         0x4EE9A0  thiscall; +0 = -1, +4, +0x808, +0x809, +0x80A, +0x80C, +0x810 y +0x818 a cero
//    7 CColModel::Destructor                    0x40F700  thiscall; bit 2 de +0x29 y puntero +0x2C: bit 1 -> 0x416400 + 0x72F430, si no 0x40F070 + operator delete 0x8214BD
//    8 CPlane::SetHeadingMatrix                 0x6C4130  thiscall (float), `ret 4`; matriz en [this+0x14]: right = {sin, -cos, 0}, forward = {cos, sin, 0}, up = {0, 0, 1}
//    9 CPools::PoolDestructorBody               0x4049C0  thiscall; capacidad (+8) > 0: si +0x10 != 0 delete[] 0x8213AE de +0 y de +4; pone a cero +0, +4, +8 y +0xC
//   10 CConversations::EnableConversation       0x43AA40  cdecl (ped, byte); en el arreglo global 0x9691D8 (14 x 0x1C, ped en +8) pone el byte +0x18 de la primera entrada con ese ped
//   11 CShadowCamera::RecreateRaster            0x705480  thiscall; destruye (0x7FB020) y crea (0x7FB230) el raster cuadrado de la camara y lo asigna a la textura (0x7F35D0)
//   12 CVector::Cross_OG                        0x70F890  thiscall (a, b), `ret 8`; producto vectorial en la x87 de 24 bits, cada componente se guarda antes de calcular la siguiente
//   13 CCollisionData::Constructor              0x40F030  thiscall; ceros hasta +0x2F salvo el byte +7, que conserva sus 5 bits altos
//
// Cada caso se ejecuta por la entrada del EXE. `this` apunta a una arena propia (guardas de 0x40) en un desplazamiento alineado, desalineado o al borde de la ventana (la escritura entra en la guarda).
// Familia = n % 16: tipo = familia % 4 (0 alineado, 1 desalineado, 2 al borde de la ventana, 3 alineado con datos "interesantes") y relleno = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD);
// las funciones sin `this` (2, 10) y las de datos de valor (2, 8, 12) definen ademas sus propias 16 familias.
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F129AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 14;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x1000, kAWin = 0x80, kBWin = 0x40;   // arena del objeto (this), arena secundaria (matrices, vectores, camara) y arena terciaria (raster)
constexpr std::size_t kSlotSize = 0x12D4, kSlotWin = 8 * kSlotSize;    // ocho slots de bancos de sonido (la funcion 4 apunta a su tercer slot)
constexpr std::size_t kSlotBase = 2 * kSlotSize;                       // desplazamiento del slot 0 dentro de la ventana
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 kSurfaceTableAddr = 0x8AD914u;       // 256 superficies de 16 bytes (funcion 2)
constexpr std::size_t kSurfaceTableBytes = 0x1000;
constexpr u32 kConversationsAddr = 0x9691D8u;      // 14 conversaciones de 0x1C bytes (funcion 10)
constexpr std::size_t kConversationsBytes = 14 * 0x1C;

// Extremo de lo que cada funcion lee o escribe a partir de this (bytes)
inline constexpr std::array<u32, kFunctions> kSpan{16u, 4u, 0u, 0x42Eu, 0x15u, 0x80u, 0x81Cu, 0x30u, 0x18u, 0x11u, 0u, 8u, 0u, 0x30u};
inline constexpr std::array<bool, kFunctions> kHasThis{true, true, false, true, true, true, true, true, true, true, false, true, false, true};

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
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
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

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de this en la arena (funciones con this) o el dato principal (2: entero de la tabla, 10: clave del peaton), a[1] = tipo (familia % 4), a[2] = relleno de la arena
//   (0 aleatorio, 1 ceros, 2 unos, 3 0xCD), a[3] = 0 (reservado), a[4] = ventana de la arena del objeto, a[5] = extremo de lo que toca la funcion a partir de this (bytes), a[6..15] = por funcion:
//     0: a[6..11] = 24 bytes previos desde this-8; a[12] = 0 origen en la arena secundaria, 1 origen solapado con el destino; a[13] = desplazamiento del origen (con signo, modo 1) o de la arena
//        secundaria (modo 0); a[15] = los cuatro bytes del origen (modo 0)
//     1: a[6] = tipo de this, a[7] = tipo del otro evento, a[8] = desplazamiento del otro evento en la arena secundaria
//     2: a[6] = dword de pila (el byte bajo es la superficie, el resto es basura)
//     3: a[6] = byte previo en +0x428, a[7] = byte previo en +0x42D, a[8] = dword de pila (el byte bajo es el argumento)
//     4: a[6] = marca en el dword leido, a[7], a[8] = dwords de pila (slot, sonido; solo cuentan los 16 bits bajos), a[9] = byte m_IsInitialised
//     5: a[6], a[7], a[8] = multiplicadores de los campos previos (dword +0, byte +4, byte +5); a[9] = mascara (16 bits) de entradas con campos previos distintos de cero; a[11] = numero de activas
//     6: a[6..13] = dwords previos en +0, +4, +0x808 (byte), +0x809 (byte), +0x80A (word), +0x80C (word), +0x810, +0x818
//     7: a[6] = byte de banderas en +0x29, a[7] = modo del puntero +0x2C (0 nulo, 1 arena secundaria, 2 crudo), a[8] = crudo, a[9] = desplazamiento en la arena secundaria
//     8: a[6] = bits del angulo, a[7] = desplazamiento de la matriz en la arena secundaria, a[8] = dword previo en toda la matriz
//     9: a[6], a[7] = punteros +0 y +4 (crudos), a[8] = capacidad (+8), a[9] = ultima ranura (+0xC), a[10] = byte +0x10
//    10: a[6] = dword de pila del byte, a[7] = indice de la conversacion con el peaton, a[8] = indice de la primera entrada duplicada (14: ninguna), a[9] = semilla de los bytes previos
//    11: a[6] = desplazamiento de la camara en la arena secundaria, a[7] = del raster previo en la arena terciaria, a[8] = ancho previo (dword), a[9] = modo del raster nuevo (0 nulo, 1 arena
//        terciaria, 2 crudo), a[10] = crudo, a[11] = modo de la textura (0 nula, 1 arena secundaria, 2 cruda), a[12] = cruda, a[13] = desplazamiento del raster nuevo en la arena terciaria
//    12: a[6], a[7], a[8] = desplazamientos (en la arena secundaria, multiplos de 4) de a, b y el resultado; a[9] = dword previo del resultado; a[10..12] = a; a[13..15] = b
//    13: a[6] = dword previo en +0, +8..+0x2C; a[7] = dword previo en +4
inline u32 Interesting1(Gen& g) {
    switch (g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 5u})) {
    case 0: return g.W();
    case 1: return 0u;
    case 2: return 0xFFFFFFFFu;
    case 3: return 0xDEADBEEFu;
    case 4: return 1u;
    default: return 0xFFu << (8 * g.R(4));
    }
}
// dword de pila con basura en los bytes altos y un byte bajo dado
inline u32 DirtyByte(Gen& g, u32 low) { return (g.P(30) ? 0u : (g.W() & 0xFFFFFF00u)) | (low & 0xFFu); }

// float con signo, exponente y mantisa elegidos
constexpr u32 FBits(u32 sign, u32 exp, u32 man) { return (sign << 31) | ((exp & 0xFFu) << 23) | (man & 0x7FFFFFu); }

// float "normal" de rangos distintos (familias de las funciones 8 y 12)
inline u32 FloatCase(Gen& g, u32 family) {
    const u32 sign = g.R(2);
    switch (family) {
    case 0: return std::bit_cast<u32>((float(g.R(2000001)) - 1000000.0f) / 1000.0f);
    case 1: return std::bit_cast<u32>(float(int(g.R(201)) - 100));
    case 2: return std::bit_cast<u32>((float(g.R(200001)) - 100000.0f) / 100000.0f);
    case 3: return FBits(sign, 120 + g.R(15), g.W());
    case 4: return FBits(sign, 1 + g.R(60), g.W());
    case 5: return FBits(sign, 190 + g.R(60), g.W());
    case 6: return FBits(sign, 0, g.W() | (g.P(30) ? 1u : 0u));
    case 7: return g.P(50) ? (sign << 31) : FBits(sign, 254, g.W());
    case 8: return FBits(sign, 255, 0);
    case 9: return FBits(sign, 255, 0x400000u | g.R(0x400000u));
    case 10: return FBits(sign, 255, 1u + g.R(0x3FFFFFu));
    case 11: return std::bit_cast<u32>(float(int(g.R(2001)) - 1000) * 0.0078125f);
    case 12: return std::bit_cast<u32>(g.P(50) ? -(float(1 + g.R(1000)) / 1024.0f) : (float(1 + g.R(1024)) / 1024.0f));
    case 13: return FBits(sign, 100 + g.R(55), g.W());
    case 14: return std::bit_cast<u32>(float(int(g.R(1u << 20)) - (1 << 19)));
    default: return FloatCase(g, g.R(15));
    }
}

// angulo de la funcion 8: acotado a |x| < 2^20 o >= 2^63 (la reduccion de la x87 usa pi de 66 bits: entre medias el oraculo no la reproduce) y casos especiales
inline u32 AngleCase(Gen& g, u32 family) {
    const u32 sign = g.R(2);
    switch (family) {
    case 0: return std::bit_cast<u32>((float(g.R(2000001)) - 1000000.0f) / 1000.0f);                 // +-1000
    case 1: return std::bit_cast<u32>((float(g.R(200001)) - 100000.0f) / 10000.0f);                  // +-10
    case 2: return std::bit_cast<u32>((float(g.R(2001)) - 1000.0f) * 0.0031415927f);                 // multiplos de pi/1000
    case 3: return FBits(sign, 90 + g.R(37), g.W());                                                  // pequenos y medianos (hasta 2^9)
    case 4: return FBits(sign, 127 + g.R(13), g.W());                                                 // 1 .. 2^12
    case 5: return FBits(sign, 1 + g.R(60), g.W());                                                   // diminutos
    case 6: return FBits(sign, 0, g.W() | 1u);                                                        // denormales
    case 7: return g.P(40) ? (sign << 31) : FBits(sign, 140 + g.R(6), g.W());                         // cero y 2^13 .. 2^18
    case 8: return std::bit_cast<u32>(float(g.R(33)) * 0.78539819f * (g.P(50) ? -1.0f : 1.0f));        // multiplos de pi/4
    case 9: return std::bit_cast<u32>(float(g.R(1000)) * 3.14159274f * (g.P(50) ? -1.0f : 1.0f));      // cerca de multiplos de pi
    case 10: return FBits(sign, 190 + g.R(65), g.P(30) ? 0u : g.W());                                 // >= 2^63 e infinitos (exponente 255 con mantisa 0)
    case 11: return FBits(sign, 255, g.P(50) ? 0u : (0x400000u | g.R(0x400000u)));                    // infinito o NaN silencioso
    case 12: return FBits(sign, 255, 1u + g.R(0x3FFFFFu));                                            // NaN senalizante
    case 13: return std::bit_cast<u32>(float(int(g.R(201)) - 100) * 0.5f);                            // medios enteros
    case 14: return FBits(sign, 126 + g.R(2), g.W());                                                  // 0.5 .. 2
    default: return AngleCase(g, g.R(15));
    }
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 fn = c.fn;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[3] = 0;
    c.a[4] = u32(kObjWin);
    c.a[5] = kSpan[fn];
    if (fn == 2) {
        // entero de la tabla: familias propias; superficie: byte aleatorio con basura en el resto del dword de pila
        u32 v;
        switch (f) {
        case 0: v = 1 + g.R(100); break;
        case 1: v = 0; break;
        case 2: v = u32(-int(1 + g.R(100))); break;
        case 3: v = g.Pk({100u, 100u, 1000u, 10000u, 50u, 200u, 25u, 400u}); break;
        case 4: v = 100u * g.Pk({1u, 10u, 100u, 1000u, 10000u, 100000u, 1000000u, 10000000u}); break;
        case 5: v = 1 + g.R(1000); break;
        case 6: v = 1 + g.R(0x7FFFFFFFu); break;
        case 7: v = g.Pk({0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000001u, 0u, 1u}); break;
        case 8: v = g.W(); break;
        case 9: v = 100u + u32(int(g.R(5)) - 2); break;
        case 10: v = 1 + g.R(3); break;
        case 11: v = 100u * (1 + g.R(1000)); break;
        case 12: v = g.Pk({0u, 1u, 50u, 100u, 150u, 99u, 101u}) + g.R(2); break;
        case 13: v = 1u << g.R(31); break;
        case 14: v = g.R(0x10000); break;
        default: v = g.P(50) ? 1 + g.R(100000) : g.W(); break;
        }
        c.a[0] = v;
        c.a[6] = (g.P(20) ? 0u : g.W() & 0xFFFFFF00u) | g.R(256);
        return;
    }
    if (fn == 10) {
        // clave del peaton (dword) y byte con basura; el arreglo global se rellena con claves del caso
        const u32 j = g.R(14);
        c.a[0] = g.Pk({g.W(), g.W(), 0xDEADBEEFu, 0xFFFFFFFFu, 1u, 0x00BADC00u + g.R(256), 0x10000u + g.R(0x10000)});
        if (c.a[0] == 0) c.a[0] = 1;
        c.a[6] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0x101u, g.W(), g.W(), 0xFFFFFF01u, 0xFFFFFF00u});
        c.a[7] = j;
        c.a[8] = g.P(40) ? j + 1 + g.R(14 - j) : 14u;   // 14: sin duplicado; si no una entrada posterior (o la 14: ninguna) con la misma clave
        c.a[9] = g.W();
        return;
    }
    if (fn == 12) {
        // tres vectores en la arena secundaria (12 bytes cada uno; multiplos de 4 de 0 a 0x74): a, b y el resultado, que puede solaparse con a o b
        const u32 mode = f % 8;   // 0 separados .. 3 igual que a, 4 igual que b, 5 igual que ambos, 6 solapado con a, 7 solapado con b
        const u32 pa = 4 * g.R(0x74 / 4 + 1);
        u32 pb = 4 * g.R(0x74 / 4 + 1);
        u32 po = 4 * g.R(0x74 / 4 + 1);
        switch (mode) {
        case 0: case 1: case 2: break;
        case 3: po = pa; break;
        case 4: po = pb; break;
        case 5: pb = pa; po = pa; break;
        case 6: po = pa + 4 * (1 + g.R(2)); if (po > 0x74) po = pa - 4 * (1 + g.R(1)); break;
        default: po = pb + 4 * (1 + g.R(2)); if (po > 0x74) po = pb - 4; break;
        }
        c.a[6] = pa; c.a[7] = pb; c.a[8] = po;
        c.a[9] = g.W();
        // valores: tanda 0 (familias 0..7) ordinarios; tanda 1 (8..15) con extremos (denormales, enormes, infinitos, NaN)
        static constexpr u32 ordinary[]{0, 1, 2, 3, 4, 11, 12, 13, 14};
        static constexpr u32 extreme[]{5, 6, 7, 8, 9, 10, 5, 6, 3, 13, 0};
        for (int i = 10; i < 16; ++i) c.a[i] = FloatCase(g, f < 8 ? ordinary[g.R(9)] : (g.P(35) ? ordinary[g.R(9)] : extreme[g.R(11)]));
        // casos con ceros, cancelaciones exactas y valores iguales
        switch (g.R(10)) {
        case 0: c.a[10] = c.a[13]; c.a[11] = c.a[14]; c.a[12] = c.a[15]; break;                                         // a == b (cruz = 0 salvo redondeos)
        case 1: c.a[10] = c.a[11] = c.a[12] = g.P(50) ? 0u : 0x80000000u; break;                                        // a = 0
        case 2: c.a[13] = c.a[14] = c.a[15] = g.P(50) ? 0u : 0x80000000u; break;                                        // b = 0
        case 3: c.a[10] = std::bit_cast<u32>(1.0f); c.a[11] = 0u; c.a[12] = 0u; c.a[13] = 0u; c.a[14] = std::bit_cast<u32>(1.0f); c.a[15] = 0u; break;   // x x y
        case 4: c.a[12] = c.a[11]; c.a[15] = c.a[14]; break;
        default: break;
        }
        return;
    }
    if (fn == 8) {
        c.a[6] = AngleCase(g, f);
        c.a[7] = g.R(0x54 + 1);
        c.a[8] = g.Pk({0u, 0xFFFFFFFFu, 0xCDCDCDCDu, 0x3F800000u, g.W(), g.W(), 0xBF800000u, 0x80000000u});
    }
    // funciones con this: desplazamiento segun el tipo (la ventana es de kObjWin bytes y la funcion toca [off, off + span))
    const u32 win = u32(kObjWin), span = c.a[5];
    switch (kind) {
    case 0: c.a[0] = 4 * g.R((win - span) / 4 + 1); break;
    case 1: c.a[0] = 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3); break;
    case 2: c.a[0] = win - span - 4 + g.R(8); break;                           // desde 4 bytes antes del borde hasta 3 bytes dentro de la guarda posterior
    default: c.a[0] = 4 * g.R((win - span) / 4 + 1); break;
    }
    switch (fn) {
    case 0:
        for (int i = 6; i < 12; ++i) c.a[i] = Interesting1(g);
        c.a[12] = g.P(50) ? 1u : 0u;
        if (c.a[12]) c.a[13] = u32(int(g.R(13)) - 4);                            // -4 .. 8 (0: el origen es el propio destino)
        else c.a[13] = g.R(0x7D);
        c.a[15] = Interesting1(g);
        break;
    case 1: {
        const auto type = [&] {
            switch (g.R(7)) {
            case 0: return g.W();
            case 1: return 0u;
            case 2: return 0xFFFFFFFFu;
            case 3: return u32(g.R(300));
            case 4: return 0x80000000u + g.R(4);
            case 5: return 0x00010000u << g.R(16);
            default: return u32(0x100u + g.R(3));
            }
        };
        c.a[6] = type();
        c.a[7] = g.P(30) ? c.a[6] : g.P(30) ? (c.a[6] ^ (1u << g.R(32))) : type();
        c.a[8] = g.R(0x7D);
        break;
    }
    case 3:
        c.a[6] = g.P(10) ? 0u : g.P(10) ? 0xFFu : g.W() & 0xFFu;
        c.a[7] = g.P(10) ? 0u : g.P(10) ? 0xFFu : g.W() & 0xFFu;
        if (g.P(50)) c.a[7] = (c.a[7] | 2u); else c.a[7] &= ~2u;                  // motor roto
        if (g.P(50)) c.a[6] |= 0x10u; else c.a[6] &= ~0x10u;                       // motor encendido
        c.a[8] = DirtyByte(g, g.Pk({0u, 1u, 2u, 3u, 0xFEu, 0xFFu, 0x80u, 0x81u, g.W()}));
        break;
    case 4:
        c.a[6] = g.W() | 1u;
        c.a[7] = DirtyByte(g, 0u);                                                 // se rellena abajo con el slot
        c.a[8] = DirtyByte(g, 0u);
        {
            const int slot = int(g.R(6)) - 2;                                       // -2 .. 3
            const u32 sound = g.Pk({0u, 1u, 2u, 399u, 400u, 401u, g.R(400), g.R(400), g.R(400), g.R(700)});
            c.a[7] = (c.a[7] & 0xFFFF0000u) | (u32(slot) & 0xFFFFu);
            c.a[8] = (c.a[8] & 0xFFFF0000u) | (sound & 0xFFFFu);
        }
        c.a[9] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u & 0xFFu});
        break;
    case 5: {
        c.a[6] = g.P(10) ? 0u : g.W() | 1u; c.a[7] = g.P(10) ? 0u : g.W() | 1u; c.a[8] = g.P(10) ? 0u : g.W() | 1u;
        static constexpr u32 counts[]{0, 1, 1, 2, 3, 4, 6, 8, 16, 16};
        const u32 count = counts[g.R(10)];
        u32 mask = 0, placed = 0;
        if (count >= 16) mask = 0xFFFFu;
        else while (placed < count) { const u32 e = g.R(16); if (!((mask >> e) & 1)) { mask |= 1u << e; ++placed; } }
        c.a[9] = mask; c.a[11] = count;
        break;
    }
    case 6:
        for (int i = 6; i < 14; ++i) c.a[i] = Interesting1(g);
        break;
    case 7:
        c.a[6] = g.Pk({0x04u, 0x06u, 0x02u, 0x00u, 0xFFu, 0x05u, 0x07u, 0x01u, 0x84u, 0x86u, g.W() & 0xFFu, g.W() & 0xFFu});
        c.a[7] = g.Pk({0u, 1u, 1u, 1u, 2u, 2u});
        c.a[8] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFF0000u + g.R(0x10000), 0x00BADC00u + g.R(256), 0x10u});
        c.a[9] = 4 * g.R(0x10);
        break;
    case 9:
        c.a[6] = g.Pk({0u, 0x00BADC00u + g.R(256), 0xC0123456u, 0xFFFFFFFFu, g.W() | 0x80000000u});
        c.a[7] = g.Pk({0u, 0x00BADD00u + g.R(256), 0x9ABCDEF0u, 0xFFFFFFFFu, g.W() | 0x80000000u});
        c.a[8] = g.Pk({0u, 1u, 2u, 140u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, g.R(1000), g.R(1000), g.W()});
        c.a[9] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), 0x7F7F7F7Fu});
        c.a[10] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
        break;
    case 11:
        c.a[6] = g.R(0x1C / 4 + 1) * 4 + (g.P(30) ? g.R(3) : 0u);
        c.a[7] = g.R(0x30 + 1);
        c.a[8] = g.Pk({0u, 1u, 0x100u, 0x200u, 0x400u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(2048), 64u, 128u});
        c.a[9] = g.Pk({0u, 1u, 1u, 2u, 2u});
        c.a[10] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFF0000u + g.R(0x10000), 0x00BADC00u + g.R(256), 0x10u});
        c.a[11] = g.Pk({0u, 1u, 1u, 2u, 2u});
        c.a[12] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFF0000u + g.R(0x10000), 0x00BADC00u + g.R(256), 0x20u});
        c.a[13] = g.R(0x30 + 1);
        break;
    case 13:
        c.a[6] = Interesting1(g);
        c.a[7] = g.P(30) ? 0u : g.P(30) ? 0xFFFFFFFFu : g.W();
        break;
    default: break;
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
