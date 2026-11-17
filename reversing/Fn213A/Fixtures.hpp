#pragma once
// Fixtures del banco Fn213A (lote 213A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (sin hook antes del lote; las clases del listado salen del desensamblado, ver README.md y fixture_oracle.py). 32 entradas.
//   idx  direccion  nombre real                                              forma
//   0-3  0x550BD0, 0x550CA0, 0x550D70, 0x550E40  constructores de CPool (NodeRoute 0x24, TaskAllocator 0x20, PedIntelligence 0x294, PedAttractor 0xC4)   thiscall(capacidad, nombre) `ret 8`
//   4-7  0x550C30, 0x550D00, 0x550DD0, 0x550EA0  cuerpos de destructor de CPool                                                                       thiscall() `ret`
//   8-24 0x5512F0 + k * 0x60 (17)                 destructores con borrado de CPool (uno por pool)                                                      thiscall(banderas) `ret 4`
//   25   0x5521D0   CPtrListDoubleLink<CBuilding*>::IsMemberOfList                                                                                      thiscall(elemento) `ret 4`: solo AL
//   26   0x45B730   CReplay: escritura del paquete de coche                                                                                            thiscall(veh, indice) `ret 8`
//   27   0x45BF90   escritura del paquete de BMX (tipo 0x0F)                                                                                           igual
//   28   0x45C070   escritura del paquete de moto (tipo 2)                                                                                              igual
//   29   0x45BA90   lectura con interpolacion del paquete de vehiculo                                                                                  thiscall(veh, float) `ret 8`
//   30   0x45BFF0   lectura del paquete de BMX                                                                                                          igual
//   31   0x45C0D0   lectura del paquete de moto                                                                                                         igual
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

namespace F213AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 32;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 64;         // dwords de entrada por caso

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
// Bloque grande de los estados de los pools (los constructores escriben 0x80 en cada estado): puntero que devuelve el falso de operator new[] cuando la capacidad es positiva
constexpr std::size_t kBig = 0x800000 + 0x200;

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
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kSlots> a{};
    std::array<u32, 4> sd{};    // semillas de las arenas A..D
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    bool Pm(u32 permille) { return Rand(n, nk()) % 1000 < permille; }
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
// La pila del hilo del banco (cerca de 0x00EB0000) se etiqueta aparte (Tag): un puntero crudo que caiga ahi se confundiria con un objeto de la pila, asi que ninguno se genera en esa zona
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
// Un float uniforme en [lo, hi] como bits (la aritmetica en double es determinista; el oraculo solo lee los bits)
inline u32 UniformBits(Gen& g, double lo, double hi) {
    const double u = double(g.R(1u << 24)) / 16777216.0;
    const float v = float(lo + u * (hi - lo));
    return std::bit_cast<u32>(v);
}
// Mezcla: la mitad uniforme en [lo, hi], una cuarta parte de valores especiales de la lista y otra de FloatBits
inline u32 Mixed(Gen& g, double lo, double hi, std::initializer_list<u32> specials) {
    const u32 r = g.R(8);
    if (r < 4) return UniformBits(g, lo, hi);
    if (r < 6) return g.Pk(specials);
    return FloatBits(g);
}

// Clases de funcion (forma)
enum Kind : u32 { kPoolCtor, kPoolDtor, kPoolScalar, kIsMember, kStoreCar, kStoreBmx, kStoreBike, kExtractVeh, kExtractBmx, kExtractBike };
inline constexpr std::array<Kind, kFunctions> kKind{
    kPoolCtor, kPoolCtor, kPoolCtor, kPoolCtor, kPoolDtor, kPoolDtor, kPoolDtor, kPoolDtor,
    kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar, kPoolScalar,
    kIsMember, kStoreCar, kStoreBmx, kStoreBike, kExtractVeh, kExtractBmx, kExtractBike};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{
    0x550BD0, 0x550CA0, 0x550D70, 0x550E40, 0x550C30, 0x550D00, 0x550DD0, 0x550EA0,
    0x5512F0, 0x551350, 0x5513B0, 0x551410, 0x551470, 0x5514D0, 0x551530, 0x551590, 0x5515F0, 0x551650, 0x5516B0, 0x551710, 0x551770, 0x5517D0, 0x551830, 0x551890, 0x5518F0,
    0x5521D0, 0x45B730, 0x45BF90, 0x45C070, 0x45BA90, 0x45BFF0, 0x45C0D0};
// Tamano de elemento de cada constructor de pool
inline constexpr std::array<u32, 4> kElem{0x24, 0x20, 0x294, 0xC4};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14,
    8, 0xA18, 0xA18, 0xA18, 0xA18, 0xA18, 0xA18};

// ------------------------------------------------------------------------------------------------ disposicion de las entradas (a[])
//   a[0] = desplazamiento del objeto principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]]
//   kPoolCtor (0-3): a[4] = capacidad (int32), a[5] = lo que devuelve operator new[] la primera vez (almacenamiento), a[6] = desplazamiento de los estados en el bloque grande (capacidad > 0),
//         a[7] = puntero crudo de los estados (capacidad <= 0), a[8] = nombre (crudo, no se guarda)
//   kPoolDtor (4-7): a[4] = tamano (+8, con signo), a[5] = dword de +0x10 (propietario: solo se prueba el byte bajo), a[6] = +0, a[7] = +4, a[8] = +0xC (punteros y contador crudos)
//   kPoolScalar (8-24): como kPoolDtor mas a[9] = banderas (dword con ruido; solo el bit 0 del byte bajo)
//   kIsMember (25): a[4] = nodos (0 = cabeza nula), a[5..12] = desplazamientos de los nodos en B, a[13..20] = elementos, a[21] = modo de busqueda (0 existente, 1 ausente, 2 nulo), a[22] = indice, a[23] = elemento ausente
//   kStoreCar/Bmx/Bike (26-28): a[4] = desplazamiento del paquete en B, a[5] = indice (dword con ruido), a[6] = modo del puntero de la matriz (+0x14: 0 nulo, 1 en C, 2 crudo), a[7] = desplazamiento en C, a[8] = crudo,
//         a[9] = vida, a[10] = pedal, a[11] = tipo de vehiculo (+0x590), a[12] = paneles (+0x5B4), a[13..15] = velocidad, a[16] = modelo (word +0x22), a[17], a[18] = bytes +0x434, +0x435,
//         a[19] = banderas de la entidad (+0x1C), a[20] = banderas fisicas (+0x40), a[21] = rotacion de la torreta (+0x94C), a[22] = direccion (+0x494), a[23..26] = compresion de suspension (+0x7D4),
//         a[27..30] = rotacion de rueda (+0x828), a[31], a[32] = angulos de puerta (+0x5F4, +0x60C), a[33..38] = estados de puerta (+0x5A9..+0x5AE), a[39] = byte +0x594,
//         a[40] = FindPlayerVehicle (0 este vehiculo, 1 otro crudo, 2 nulo), a[41] = gbFirstPersonRunThisFrame (byte), a[42] = desplazamiento de la vtabla en C, a[43] = desplazamiento del RideAnimData en D,
//         a[44] = LeanAngle (bits), a[45] = BarSteerAngle (bits), a[46] = el otro vehiculo (crudo)
//   kExtractVeh/Bmx/Bike (29-31): a[4] = desplazamiento del paquete en B, a[5] = t (bits), a[6..8] = puntero de la matriz como arriba, a[9] = vida (+2), a[10] = pedal (+3), a[11] = paneles (dword +0x1C),
//         a[12..14] = velocidad (+0x20..+0x22), a[15] = banderas fisicas (+0x2F), a[16] = direccion (+0x23), a[17..20] = suspension (+0x24..+0x27), a[21..24] = ruedas (+0x28..+0x2B),
//         a[25], a[26] = puertas (+0x18, +0x19), a[27] = puertas arrancadas (+0x2C), a[28] = inclinacion (+0x34), a[29] = giro del manillar (+0x35), a[30] = tipo de vehiculo (+0x590), a[31] = modelo (word +0x22),
//         a[32] = CGame::currArea (dword), a[33..38] = lo que devuelve GetDoorStatus para cada puerta (0..4), a[39] = sin uso, a[40] = desplazamiento de la vtabla en C,
//         a[41] = desplazamiento del RideAnimData en D, a[42] = 1: vehiculo nulo (solo 0x45BA90: el EXE no hace nada; las de moto y BMX no comprueban nulos)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.sd[0] = c.seed * 13u + 50u; c.sd[1] = c.sd[0] + 1u; c.sd[2] = c.sd[0] + 2u; c.sd[3] = c.sd[0] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x887D5Cu, 0x870114u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kPoolCtor: {
        const u32 elem = kElem[c.fn];
        u32 cap = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 8u, 16u, 64u, 70u, 140u, 500u, 0x100u, 0x3FFu, 1u + g.R(0x400), 1u + g.R(0x400), 1u + g.R(0x4000)});
        if (g.P(14)) cap = g.Pk({0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFBu, 0xFFFFFFC0u, 0x80000000u, 0x80000001u, 0xFFF00000u, 0xFFFFFF00u, g.W() | 0x80000000u, 0xFFFFFFFFu});
        if (g.Pm(25)) cap = (elem == 0x294 && g.P(50)) ? 0x600000u + g.R(0x1FFFFF) : 0x20000u + g.R(0x7DFFFF);
        a[4] = cap;
        a[5] = g.P(15) ? 0u : Raw(g);
        a[6] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u, 0x10u, g.R(0x40)});
        a[7] = Raw(g);
        a[8] = Raw(g);
        break;
    }
    case kPoolDtor:
    case kPoolScalar: {
        a[4] = g.Pk({0u, 0u, 1u, 1u, 2u, 0x40u, 1u + g.R(500), 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x7FFFFFFFu, g.W()});
        a[5] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, g.W(), g.W()});
        a[6] = Raw(g); a[7] = Raw(g); a[8] = Raw(g);
        a[9] = g.Pk({0u, 1u, 1u, 2u, 3u, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, g.W()});
        break;
    }
    case kIsMember: {
        const u32 cnt = a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 8u, 3u});
        u32 slot[16];
        for (u32 i = 0; i < 16; ++i) slot[i] = i;
        for (u32 i = 15; i > 0; --i) std::swap(slot[i], slot[g.R(i + 1)]);
        for (u32 i = 0; i < 8; ++i) a[5 + i] = 0x100u * slot[i] + 8u * g.R(0x1F);
        for (u32 i = 0; i < 8; ++i) a[13 + i] = (i > 0 && g.P(18)) ? a[13 + g.R(i)] : (g.P(12) ? 0u : Raw(g));
        a[21] = g.Pk({0u, 0u, 1u, 2u});
        a[22] = cnt ? g.R(cnt) : 0u;
        a[23] = Raw(g);
        break;
    }
    case kStoreCar:
    case kStoreBmx:
    case kStoreBike: {
        a[4] = AuxOffset(g, 0x40);
        a[5] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0xFFFFFF00u, 0x80000000u, g.W(), g.W(), g.R(110)});
        a[6] = g.Pk({0u, 1u, 1u, 2u}); a[7] = AuxOffset(g, 0x48); a[8] = Raw(g);
        a[9] = Mixed(g, -100.0, 1500.0, {0x447A0000u, 0x447A0001u, 0x4479FFFFu, 0x447A0000u, 0u, 0x80000000u, 0x44800000u});
        a[10] = Mixed(g, -2.0, 3.0, {0x3F800000u, 0xBF800000u, 0u, 0x80000000u, 0x3C23D70Au});
        a[11] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 0xFFu, 0x100u, 0x80000000u, g.W(), g.R(8)});
        a[12] = g.W();
        const u32 spec[] = {0x40800000u, 0xC0800000u, 0x40800001u, 0xC0800001u, 0x407FFFFFu, 0xC07FFFFFu, 0u, 0x80000000u, 0x3A03126Fu, 0xBA03126Fu};
        for (u32 i = 0; i < 3; ++i) a[13 + i] = Mixed(g, -5.0, 5.0, {spec[g.R(10)], spec[g.R(10)], spec[g.R(10)]});
        a[16] = g.Pk({0x1B0u, 0x1B0u, 0x1B0u, 0x1AFu, 0x1B1u, 0u, 0xFFFFu, 0x1B0u, 0x190u, g.R(0x10000), g.R(0x10000)});
        a[17] = g.Pk({0u, 0xFFu, g.R(256), g.R(256)}); a[18] = g.Pk({0u, 0xFFu, g.R(256), g.R(256)});
        a[19] = g.Pk({0u, 0xFFFFFFFFu, 0x80u, 0x40000000u, 0x80000000u, 0xC0000080u, g.W(), g.W(), g.W()});
        a[20] = g.Pk({0u, 0xFFFFFFFFu, 0x20000000u, 0xDFFFFFFFu, g.W(), g.W(), g.W()});
        a[21] = Mixed(g, -5.0, 5.0, {0u, 0x80000000u, 0x3F800000u});
        a[22] = Mixed(g, -4.0, 4.0, {0u, 0x80000000u, 0x3F800000u});
        for (u32 i = 0; i < 4; ++i) a[23 + i] = Mixed(g, -4.0, 4.0, {0u, 0x80000000u});
        for (u32 i = 0; i < 4; ++i) a[27 + i] = Mixed(g, -5.0, 5.0, {0u, 0x80000000u});
        a[31] = Mixed(g, -8.0, 8.0, {0u, 0x80000000u}); a[32] = Mixed(g, -8.0, 8.0, {0u, 0x80000000u});
        for (u32 i = 0; i < 6; ++i) a[33 + i] = g.Pk({4u, 4u, 4u, 0u, 1u, 2u, 3u, 5u, 0xFFu, g.R(256)});
        a[39] = g.Pk({0u, 0xFFu, g.R(256)});
        a[40] = g.Pk({0u, 0u, 1u, 2u});
        a[41] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, g.R(256)});
        a[42] = AuxOffset(g, 0xC4);
        a[43] = AuxOffset(g, 0x10);
        a[44] = Mixed(g, -3.0, 3.0, {0u, 0x80000000u, 0x3F800000u});
        a[45] = Mixed(g, -3.0, 3.0, {0u, 0x80000000u, 0x3F800000u});
        a[46] = Raw(g);
        break;
    }
    case kExtractVeh:
    case kExtractBmx:
    case kExtractBike: {
        a[4] = AuxOffset(g, 0x40);
        a[5] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x3DCCCCCDu, 0x3F7FFFFFu, 0x3F800001u, 0x7FC00000u, 0x7F800001u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x80000001u, FloatBits(g), FloatBits(g), UniformBits(g, 0.0, 1.0), UniformBits(g, 0.0, 1.0), UniformBits(g, 0.0, 1.0), UniformBits(g, -1.0, 2.0)});
        a[6] = g.Pk({0u, 1u, 1u, 2u}); a[7] = AuxOffset(g, 0x48); a[8] = Raw(g);
        const auto byteField = [&]() { return g.Pk({0u, 0x7Fu, 0x80u, 0xFFu, 1u, 0xFEu, g.R(256), g.R(256), g.R(256)}); };
        for (u32 i = 9; i <= 10; ++i) a[i] = byteField();
        a[11] = g.W();
        for (u32 i = 12; i <= 14; ++i) a[i] = byteField();
        a[15] = g.Pk({0u, 0xFFu, 1u, 0x0Eu, 0x0Fu, 0xF1u, g.R(256), g.R(256)});
        a[16] = byteField();
        for (u32 i = 17; i <= 24; ++i) a[i] = byteField();
        a[25] = g.Pk({0u, 0u, 0x7Fu, 0x80u, 0xFFu, 1u, g.R(256), g.R(256)}); a[26] = g.Pk({0u, 0u, 0x7Fu, 0x80u, 0xFFu, 1u, g.R(256), g.R(256)});
        a[27] = g.Pk({0u, 0xFFu, 0x3Fu, 1u, 2u, 4u, 8u, 0x10u, 0x20u, g.R(256), g.R(256), g.R(256)});
        a[28] = byteField(); a[29] = byteField();
        a[30] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 0xFFu, 0x100u, 0x80000000u, g.W(), g.R(8)});
        a[31] = g.Pk({0x1B0u, 0x1B0u, 0x1B0u, 0x1AFu, 0x1B1u, 0u, 0xFFFFu, 0x190u, g.R(0x10000), g.R(0x10000)});
        a[32] = g.Pk({0u, 1u, 2u, 3u, 0xFFu, g.R(256), g.W(), g.W()});
        for (u32 i = 33; i <= 38; ++i) a[i] = g.Pk({0u, 1u, 2u, 3u, 4u, 4u, 4u});
        a[39] = g.W();
        a[40] = AuxOffset(g, 0xC4);
        a[41] = AuxOffset(g, 0x10);
        a[42] = (kKind[c.fn] == kExtractVeh && g.P(8)) ? 1u : 0u;
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
    std::size_t used = c.a.size();
    while (used > 0 && c.a[used - 1] == 0) --used;
    j.A("a", std::vector<u32>(c.a.begin(), c.a.begin() + used));
    j.A("sd", std::vector<u32>(c.sd.begin(), c.sd.end()));
    return j.End();
}
}
