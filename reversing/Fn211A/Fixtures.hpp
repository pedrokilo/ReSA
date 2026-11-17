#pragma once
// Fixtures del banco Fn211A (lote 211A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN validar en el lote 211A; 27 direcciones del listado, 25 reconstruidas y enganchadas; 0x5D84C0 y 0x5D8590 (rutinas de UV, __usercall con registros de entrada) se saltaron, ver README.md).
//   idx  direccion  nombre real                                                           forma
//    0   0x6A3060   CAutomobile::SetUpWheelColModel (V)                                    thiscall(CColModel*) `ret 4`: copia caja y esfera; seis esferas de rueda; 4 o 6 esferas; retorno AL
//    1   0x6A32B0   CAutomobile::BurstTyre (V)                                             thiscall(componente, efecto) `ret 8`: estado de la rueda, estadistica, audio, fisica real, fuerzas
//    2   0x6A3BD0   CAutomobile::DoNitroEffect                                             thiscall(float) `ret 4`: escapes, nivel del agua, efectos "nitro"
//    3   0x6A6280   CAutomobile GetRideAnimData (ranura 47)                                thiscall() `xor eax,eax; ret`
//    4   0x6A6290   CVehicle::FlagToDestroyWhenNextProcessed (ranura 21)                   thiscall() `ret` (1 byte)
//    5   0x6A6590   CAutomobile::SetModelIndex (V)                                         thiscall(indice) `ret 4`: CVehicle::SetModelIndex, 25 dwords a cero, FillFrameArray
//    6   0x6B0690   CAutomobile::DoHeliDustEffect                                          thiscall(float, float) `ret 8`: sonda vertical, agua, camara, peligro, efecto "heli_dust"
//    7   0x6B1330   CAutomobile destructor con borrado (ranura 0)                          thiscall(banderas) `ret 4`: cuerpo 0x6A61E0, operator delete 0x6E2D90 con el bit 0
//    8   0x5D8980   IsEnvironmentMappingSupported                                          cdecl() : capacidades D3D9 (RwD3D9GetCaps 0x7FAD20); EAX 0/1
//    9   0x5D8BE0   EnvMatDataFromBuffer                                                   cdecl(buffer, datos): _ftol2 de x*8 / x*255, textura +8, word +6 = 0, sin escribir +5
//   10   0x5D8C50   EnvMatDataToBuffer                                                     cdecl(datos, buffer): byte con signo * 0,125, byte * (1/255), textura
//   11   0x5D9060   CPool<12 B>::CPool (material)                                          thiscall(tamano, nombre) `ret 8`: new(tamano*12), new(tamano), flags a 0x80
//   12   0x5D91B0   CPool<12 B>::CPool (atomic)                                            idem
//   13   0x5D9300   CPool<8 B>::CPool (especular)                                          idem con elemento de 8 bytes
//   14-19 0x5D8F70 0x5D8FC0 0x5D9010 0x5D90C0 0x5D9210 0x5D9360  CPool::~CPool (cuerpo)   thiscall() `ret`: con tamano > 0 libera objetos y flags si es propietario y pone a cero
//   20   0x5D9450   CPool destructor con borrado                                           thiscall(banderas) `ret 4`
//   21   0x5D9110   CPool<12 B>::New (material)                                            thiscall() `ret`
//   22   0x5D9260   CPool<12 B>::New (atomic)                                              idem
//   23   0x5D93B0   CPool<8 B>::New (especular)                                            idem con elemento de 8 bytes
//   24   0x5D9180   CPool<12 B>::Delete                                                    thiscall(objeto) `ret 4`
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

namespace F211AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;
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
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 40> a{};
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
// Desplazamiento alineado en [lo, lo + range) (multiplo de 4)
inline u32 Slot(Gen& g, u32 lo, u32 range) { return lo + 4 * g.R(range / 4); }

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
inline bool IsSnanBits(u32 b) { return IsNanBits(b) && !(b & 0x400000u); }
// Igual que FloatBits, sin NaN (los productos con dos NaN de cargas distintas dependen del orden de operandos de la x87 frente al SSE; ver README.md)
inline u32 FloatNoNan(Gen& g) { u32 v = FloatBits(g); while (IsNanBits(v)) v = FloatBits(g); return v; }
// Igual, pero los NaN son silenciosos (rutas que pasan por el x87 con fld/fstp o fabs)
inline u32 FloatQuiet(Gen& g) { u32 v = FloatBits(g); return IsSnanBits(v) ? (v | 0x400000u) : v; }
// Un float de magnitud moderada en [lo, hi] (sin denormales ni desbordes en productos)
inline u32 FloatMod(Gen& g, float lo = -2000.f, float hi = 2000.f) {
    const float t = float(g.R(1000001)) / 1000000.0f;
    return std::bit_cast<u32>(lo + (hi - lo) * t);
}
inline u32 QNan(Gen& g) { return 0x7FC00000u | (g.W() & 0x3FFFFFu); }

// Clases de funcion (forma)
enum Kind : u32 { kSetUp, kBurst, kNitro, kRideAnim, kFlagDestroy, kSetModel, kHeli, kSdd, kEnvSupp, kFromBuf, kToBuf, kPoolCtor, kPoolDtor, kPoolSdd, kPoolNew, kPoolDel };
inline constexpr std::array<Kind, kFunctions> kKind{kSetUp, kBurst, kNitro, kRideAnim, kFlagDestroy, kSetModel, kHeli, kSdd, kEnvSupp, kFromBuf, kToBuf, kPoolCtor, kPoolCtor, kPoolCtor,
                                                    kPoolDtor, kPoolDtor, kPoolDtor, kPoolDtor, kPoolDtor, kPoolDtor, kPoolSdd, kPoolNew, kPoolNew, kPoolNew, kPoolDel};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x6A3060, 0x6A32B0, 0x6A3BD0, 0x6A6280, 0x6A6290, 0x6A6590, 0x6B0690, 0x6B1330, 0x5D8980, 0x5D8BE0, 0x5D8C50, 0x5D9060, 0x5D91B0, 0x5D9300,
                                                    0x5D8F70, 0x5D8FC0, 0x5D9010, 0x5D90C0, 0x5D9210, 0x5D9360, 0x5D9450, 0x5D9110, 0x5D9260, 0x5D93B0, 0x5D9180};
// Bytes que cada funcion toca a partir de su puntero `this` (o del puntero de salida)
inline constexpr std::array<u32, kFunctions> kSpan{0x670, 0x6B0, 0x980, 4, 4, 0x6B0, 0x990, 4, 4, 0xC, 0x18, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14};
// Tamano del elemento del pool (12 o 8)
inline constexpr u32 PoolElem(u32 fn) { return fn == 13 || fn == 23 ? 8u : 12u; }
// Memoria global del juego tocada
inline constexpr u32 kModelTable = 0xA9B0C8, kModelLo = 0x190, kModelCount = 0xB0, kCamPos = 0xB6F02C, kCamBytes = 0x14, kCamMatrix = 0xB6F03C, kFrameCounter = 0xB7CB4C;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o de la salida) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   El significado de a[4..] depende de la funcion (ver LiveComparison.hpp: Setup, y fixture_oracle.py).
inline u32 PickModel(Gen& g) { return g.Pk({0x1B0u, 0x1D1u, 0x1F5u, 0x208u, 0x213u, 0x214u, 0x23Bu, 0x190u + g.R(0xB0), 0x190u + g.R(0xB0), 0x190u + g.R(0xB0)}); }
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kSetUp: {
        // a[4] = modelo, a[5] = byte +0x36, a[6] = bit0 nodo LM presente, bit1 nodo RM presente, a[7] = base de los seis fotogramas en B, a[8..25] = posiciones (x,y,z) de los fotogramas LF, LB, RF, RB, LM, RM,
        // a[26] = tamano de rueda delantera (mi+0x40), a[27] = trasera (mi+0x44), a[28] = mi en D, a[29] = modelo de colision del vehiculo en D, a[30] = CColModel de la rueda en C, a[31] = CColData en C,
        // a[32] = esferas en C
        a[4] = PickModel(g);
        a[5] = (g.R(32) << 3) | g.Pk({0u, 1u, 3u, 4u, 5u, 6u, 7u});
        a[6] = g.Pk({0u, 1u, 2u, 3u, 3u, 3u});
        a[7] = 4 * g.R(0x100);
        const bool sane = g.P(30);
        for (u32 i = 0; i < 18; ++i) a[8 + i] = sane ? FloatMod(g, -4.f, 4.f) : FloatBits(g);
        a[26] = g.P(40) ? FloatMod(g, 0.f, 2.f) : FloatBits(g);
        a[27] = g.P(40) ? FloatMod(g, 0.f, 2.f) : FloatBits(g);
        a[28] = 4 * g.R(0x100);
        a[29] = 0x800 + 4 * g.R(0x40);
        a[30] = 4 * g.R(0x40);
        a[31] = 0x200 + 4 * g.R(0x20);
        a[32] = 0x400 + 4 * g.R(0x80);
        break;
    }
    case kBurst: {
        // a[4] = modelo (+0x22), a[5] = byte +0x42B, a[6] = dword +0x40, a[7] = componente (dword de pila), a[8] = efecto fisico (dword de pila), a[9] = byte +0x36, a[10] = byte de estado de la rueda en el CDamageManager (+0x5A5 + rueda; GetWheelStatus corre real),
        // a[11], a[12] = bits de los dos aleatorios, a[13] = masa (+0x8C), a[14] = masa de giro (+0x90), a[15] = matriz en B, a[16..18] = vector derecho, a[19..21] = vector adelante,
        // a[22] = fuente del unico NaN (0 ninguna, 1..4 aleatorios y masas, 5 derecha, 6 adelante)
        a[4] = g.P(25) ? 0x1B0u : PickModel(g);
        a[5] = g.P(25) ? (0x80u | g.R(128)) : g.R(128);
        a[6] = g.P(25) ? (g.W() | 0x20000000u) : (g.W() & ~0x20000000u);
        const u32 comp = g.Pk({13u, 14u, 15u, 16u, 13u, 14u, 15u, 16u, 0u, 1u, 2u, 3u, 12u, 17u, 255u, g.R(256)});
        a[7] = comp | (g.P(50) ? (g.W() & 0xFFFFFF00u) : 0u);
        a[8] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x100u, 0xFFu}) | (g.P(50) ? (g.W() & 0xFFFFFF00u) : 0u);
        a[9] = g.P(40) ? (0x10u | g.R(8)) : ((g.R(32) << 3) | g.R(8));
        a[10] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 0x40u, 0xFFu});
        a[22] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u});
        const bool sane = g.P(35);
        a[11] = a[22] == 1 ? QNan(g) : (sane ? FloatMod(g, -0.03f, 0.03f) : (g.P(50) ? FloatNoNan(g) : FloatMod(g, -0.03f, 0.03f)));
        a[12] = a[22] == 2 ? QNan(g) : (sane ? FloatMod(g, -0.03f, 0.03f) : (g.P(50) ? FloatNoNan(g) : FloatMod(g, -0.03f, 0.03f)));
        a[13] = a[22] == 3 ? QNan(g) : (sane ? FloatMod(g, 0.f, 5000.f) : FloatNoNan(g));
        a[14] = a[22] == 4 ? QNan(g) : (sane ? FloatMod(g, 0.f, 5000.f) : FloatNoNan(g));
        a[15] = AuxOffset(g, 0x40);
        const u32 nv = a[22] == 5 ? g.R(3) : 9u, nf = a[22] == 6 ? g.R(3) : 9u;
        for (u32 i = 0; i < 3; ++i) {
            a[16 + i] = nv == i ? QNan(g) : (sane ? FloatMod(g, -1.f, 1.f) : FloatNoNan(g));
            a[19 + i] = nf == i ? QNan(g) : (sane ? FloatMod(g, -1.f, 1.f) : FloatNoNan(g));
        }
        {   // un producto con dos NaN (p. ej. inf * 0 por un componente NaN) depende del orden de operandos de la x87 frente al SSE: el vector derecho no lleva NaN si algun escalar lo es
            const float s1 = std::bit_cast<float>(a[11]) * std::bit_cast<float>(a[13]), s2 = std::bit_cast<float>(a[12]) * std::bit_cast<float>(a[14]);
            if (std::isnan(s1) || std::isnan(s2))
                for (u32 i = 0; i < 3; ++i) if (IsNanBits(a[16 + i])) a[16 + i] = 0x3F800000u;
        }
        break;
    }
    case kNitro: {
        // a[4] = modelo, a[5] = dword +0x40 (bit 27: tocando el agua), a[6] = CHandlingData en C, a[7] = su dword +0xCC (bit 13: doble escape), a[8] = tipo del m_pRwObject (0 nulo, 1 en B), a[9] = su desplazamiento en B,
        // a[10] = dword +4 del objeto RW (el fotograma, crudo), a[11] = fx0 (+0x978: tipo 0 nulo, 1 en D, 2 crudo), a[12] = desplazamiento en D, a[13] = fx1 (+0x97C: tipo), a[14] = desplazamiento en D,
        // a[15] = matriz (+0x14: 0 cruda, 1 en B), a[16] = mi en D, a[17] = estructura del vehiculo en D, a[18..20] = posicion del escape (x, y, z), a[21] = potencia (bits), a[22], a[23] = valores devueltos
        // por GetWaterLevel, a[24], a[25] = niveles (bits), a[26..29] = estados devueltos por GetPlayStatus, a[30], a[31] = valor de CreateFxSystem (tipo 0 nulo, 1 en D, 2 crudo) y su desplazamiento en D
        a[4] = PickModel(g);
        a[5] = g.P(55) ? (g.W() | 0x8000000u) : (g.W() & ~0x8000000u);
        a[6] = Slot(g, 0, 0x300);
        a[7] = g.P(50) ? (g.W() | 0x2000u) : (g.W() & ~0x2000u);
        a[8] = g.Pk({0u, 1u, 1u});
        a[9] = AuxOffset(g, 0x20);
        a[10] = g.Pk({0u, g.W(), Raw(g)});
        a[11] = g.Pk({0u, 0u, 1u, 1u, 2u});
        a[12] = Slot(g, 0x400, 0x100);
        a[13] = g.Pk({0u, 0u, 1u, 1u, 2u});
        a[14] = Slot(g, 0x600, 0x100);
        a[15] = g.Pk({0u, 1u, 1u});
        a[16] = Slot(g, 0, 0x100);
        a[17] = Slot(g, 0x800, 0x100);
        const bool sane = g.P(40);
        for (u32 i = 0; i < 3; ++i) a[18 + i] = sane ? FloatMod(g, -5.f, 5.f) : FloatNoNan(g);
        a[21] = g.P(60) ? FloatQuiet(g) : FloatMod(g, -10.f, 10.f);
        a[22] = g.Pk({0u, 1u, 1u, 1u});
        a[23] = g.Pk({0u, 1u, 1u, 1u});
        const u32 z = a[20];
        for (u32 i = 0; i < 2; ++i) {
            const u32 pick = g.R(8);
            a[24 + i] = pick == 0 ? z : (pick == 1 ? z + 1u : (pick == 2 ? z - 1u : (pick == 3 ? FloatQuiet(g) : (pick == 4 ? 0x7FC00000u : (pick == 5 ? (z ^ 0x80000000u) : FloatMod(g, -6.f, 6.f))))));
        }
        for (u32 i = 0; i < 4; ++i) a[26 + i] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u});
        a[30] = g.Pk({0u, 1u, 1u, 2u});
        a[31] = Slot(g, 0xA00, 0x100);
        break;
    }
    case kRideAnim:
    case kFlagDestroy:
        break;
    case kSetModel:
        // a[4] = indice (pila), a[5] = dword +0x18 (objeto RW, crudo), a[6] = elemento de m_aCarNodes donde escribe el falso de CVehicle::SetModelIndex (0..24), a[7] = su valor,
        // a[8] = elemento donde escribe el falso de FillFrameArray, a[9] = su valor
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), g.R(0x1000), 0x213u});
        a[5] = Raw(g);
        a[6] = g.R(25); a[7] = g.W() | 1u;
        a[8] = g.R(25); a[9] = g.W() | 1u;
        break;
    case kHeli: {
        // a[4] = subtipo (+0x594), a[5] = rotor (+0x84C), a[6] = 0 sin matriz / 1 con matriz en B, a[7] = matriz en B, a[8..10] = posicion, a[11] = modelo, a[12] = tipo del polvo (+0x580: 0 nulo, 1 en D, 2 crudo),
        // a[13] = constante de tiempo (+0x984), a[14] = 0 sin matriz de camara / 1 en C, a[15] = su desplazamiento, a[16..18] = posicion de camara (global 0xB6F02C), a[19..21] = posicion de la matriz de camara,
        // a[22] = contador de fotogramas, a[23] = valor devuelto por la sonda, a[24] = z del impacto, a[25] = tipo de superficie B del impacto (byte), a[26] = valor de IsSand, a[27] = valor de GetWaterLevel,
        // a[28] = nivel, a[29] = timeConstMult, a[30] = fxMaxZMult, a[31] = valor devuelto por CreateFxSystem (tipo 0 nulo, 1 en D, 2 crudo), a[32] = grupo global (tipo 0 nulo, 1 en D, 2 crudo),
        // a[33] = desplazamiento del polvo en D, a[34] = desplazamiento del grupo en D
        a[4] = g.Pk({3u, 3u, 3u, 3u, 4u, 4u, 4u, 0u, 1u, 2u, 5u, g.W()});
        const u32 rpick = g.R(9);
        a[5] = rpick == 0 ? 0x3DE66667u : (rpick == 1 ? 0x3DE66668u : (rpick == 2 ? 0x3DE66666u : (rpick == 3 ? 0x7FC00000u : (rpick == 4 ? 0u : (rpick == 5 ? 0x80000000u : (rpick == 6 ? 0xBF800000u : FloatMod(g, 0.f, 2.f)))))));
        a[6] = g.Pk({0u, 1u, 1u});
        a[7] = Slot(g, 0, 0x300);
        const float groundZ = float(int(g.R(2001)) - 1000) / 10.0f;
        const bool nearGround = g.P(70);
        const float h = nearGround ? float(g.R(4000)) / 100.0f : float(g.R(200000)) / 100.0f - 100.0f;
        const bool nearCam = g.P(80);
        const float cx = float(int(g.R(2001)) - 1000) / 10.0f, cy = float(int(g.R(2001)) - 1000) / 10.0f;
        const float dx = nearCam ? float(int(g.R(1001)) - 500) / 10.0f : float(int(g.R(40001)) - 20000) / 10.0f;
        const float dy = nearCam ? float(int(g.R(1001)) - 500) / 10.0f : float(int(g.R(40001)) - 20000) / 10.0f;
        const float zf = groundZ + h;
        a[8] = std::bit_cast<u32>(cx + dx); a[9] = std::bit_cast<u32>(cy + dy);
        a[10] = std::bit_cast<u32>(zf);
        const u32 camz = std::bit_cast<u32>(g.P(70) ? zf + float(int(g.R(601)) - 300) / 10.0f : float(int(g.R(2001)) - 1000) / 10.0f);
        if (g.P(12)) a[8] = FloatBits(g);
        if (g.P(8)) a[9] = FloatBits(g);
        if (g.P(8)) a[10] = FloatBits(g);
        a[11] = g.Pk({0x1F5u, 0x1D1u, 0x208u, 0x1B0u, 0x1B0u, g.R(0x8000)});
        a[12] = g.Pk({0u, 0u, 1u, 2u});
        const u32 tpick = g.R(8);
        a[13] = tpick == 0 ? 0u : (tpick == 1 ? FloatQuiet(g) : FloatMod(g, 0.f, 4.f));
        a[14] = g.Pk({0u, 1u, 1u});
        a[15] = Slot(g, 0, 0x300);
        a[16] = std::bit_cast<u32>(cx); a[17] = std::bit_cast<u32>(cy); a[18] = camz;
        a[19] = std::bit_cast<u32>(cx); a[20] = std::bit_cast<u32>(cy); a[21] = camz;
        if (g.P(8)) a[16 + g.R(3)] = FloatBits(g);
        if (g.P(8)) a[19 + g.R(3)] = FloatBits(g);
        a[22] = g.Pk({0u, 20u, 40u, 60u, 20u * g.R(100000u), g.W(), 1u, 19u, 21u, 0xFFFFFFFFu});
        a[23] = g.Pk({0u, 1u, 1u});
        a[24] = g.P(80) ? std::bit_cast<u32>(groundZ) : FloatBits(g);
        a[25] = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, g.R(256), g.R(256)});
        a[26] = g.Pk({0u, 1u, 1u});
        a[27] = g.Pk({0u, 1u, 1u, 1u});
        const u32 wp = g.R(8);
        const u32 gz = a[24];
        a[28] = wp == 0 ? gz : (wp == 1 ? gz + 1u : (wp == 2 ? gz - 1u : (wp == 3 ? FloatBits(g) : (wp == 4 ? std::bit_cast<u32>(groundZ + float(g.R(300)) / 10.0f - 10.0f) : (wp == 5 ? 0xC47A0000u : FloatMod(g, -200.f, 200.f))))));
        a[29] = g.P(55) ? FloatMod(g, 0.f, 3.f) : FloatBits(g);
        a[30] = g.P(55) ? FloatMod(g, 0.f, 3.f) : FloatBits(g);
        a[31] = g.Pk({0u, 0u, 1u, 1u, 2u});
        a[32] = g.Pk({0u, 1u, 1u, 2u});
        {   // a veces la constante de tiempo actual ya es el valor objetivo (rama de igualdad): se calcula con floats de C++ (sin agua)
            const float maxz = ((a[11] & 0xFFFFu) == 0x1F5u || (a[11] & 0xFFFFu) == 0x1D1u ? 3.0f : 30.0f) * std::bit_cast<float>(a[30]);
            const float d = std::bit_cast<float>(a[10]) - std::bit_cast<float>(a[24]);
            const float t = (maxz - d) / maxz * std::bit_cast<float>(a[29]);
            const float sq = t * t;
            if (g.P(15) && std::isfinite(sq)) a[13] = std::bit_cast<u32>(sq);
        }
        a[33] = Slot(g, 0x800, 0x100);
        a[34] = Slot(g, 0x900, 0x100);
        break;
    }
    case kSdd:
        // a[4] = banderas (dword de pila, solo el byte bajo cuenta), a[5] = dword que el falso del cuerpo deja en [this]
        a[4] = g.Pk({0u, 1u, 1u, 0xFFu, 0xFEu, 2u, 3u, 0x100u, 0x101u}) | (g.P(50) ? (g.W() & 0xFFFFFF00u) : 0u);
        a[5] = g.Pk({0u, g.W(), 0x871120u});
        break;
    case kEnvSupp: {
        // a[4] = ShadeCaps (+0x38), a[5] = TextureOpCaps (+0x90), a[6] = MaxTextureBlendStages (+0x94), a[7] = MaxSimultaneousTextures (+0x98), a[8] = estructura en D
        a[4] = g.P(30) ? g.W() : ((g.W() & ~0x200u) | (g.P(65) ? 0x200u : 0u));
        a[5] = (g.W() & ~(0x2000u | 0x1000000u)) | (g.P(70) ? 0x2000u : 0u) | (g.P(70) ? 0x1000000u : 0u);
        a[6] = g.Pk({0u, 1u, 2u, 3u, 8u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), 2u, 4u});
        a[7] = g.Pk({0u, 1u, 2u, 3u, 8u, 0xFFFFFFFFu, 0x80000000u, g.W(), 2u, 2u, 4u});
        a[8] = AuxOffset(g, 0xA0);
        break;
    }
    case kFromBuf: {
        // a[4..8] = floats (escala x, escala y, traslacion x, traslacion y, brillo), a[9] = textura (+0x14), a[10] = bufer en B (el dato de salida es a[0] en A)
        for (u32 i = 0; i < 5; ++i) {
            const u32 pick = g.R(10);
            const float scale = i == 4 ? 255.0f : 8.0f;
            const float v = float(int(g.R(60001)) - 30000) / 30000.0f * (i == 4 ? 1.1f : 17.0f);
            a[4 + i] = pick < 4 ? std::bit_cast<u32>(v) : (pick == 4 ? std::bit_cast<u32>(float(int(g.R(700)) - 350) / scale) : (pick == 5 ? FloatQuiet(g) : (pick == 6 ? FloatNoNan(g) : (pick == 7 ? std::bit_cast<u32>(float(g.R(600)) / scale + 0.001f) : (pick == 8 ? FloatMod(g, -40.f, 40.f) : FloatBits(g))))));
        }
        a[9] = Raw(g);
        a[10] = AuxOffset(g, 0x18);
        break;
    }
    case kToBuf:
        // a[4..8] = bytes del dato (los cinco primeros; el 6.o y el 7.o son relleno), a[9] = textura (+8), a[10] = dato en B (la salida es a[0] en A)
        for (u32 i = 0; i < 5; ++i) a[4 + i] = g.Pk({0u, 1u, 0x7Fu, 0x80u, 0xFFu, 0x81u, g.R(256), g.R(256), g.R(256)});
        a[9] = Raw(g);
        a[10] = AuxOffset(g, 0xC);
        break;
    case kPoolCtor: {
        // a[4] = tamano, a[5] = que devuelve el primer new (0 nulo, 1 arena C, 2 crudo), a[6] = desplazamiento en C, a[7] = desplazamiento del segundo new en D (flags), a[8] = nombre (dword de pila, crudo)
        const u32 pos = g.Pk({1u, 2u, 3u, 7u, 16u, 31u, 100u, 255u, 1000u, 0xE00u, 5u, 1u + g.R(0x300)});
        a[4] = g.P(78) ? (g.P(8) ? 0u : pos) : g.Pk({0xFFFFFFFFu, 0xFFFFFFFBu, 0x80000000u, 0xFFFFF000u, 0x80000001u, 0xC0000000u});
        a[5] = g.Pk({0u, 1u, 1u, 2u});
        a[6] = Slot(g, 0, 0x800);
        a[7] = AuxOffset(g, (a[4] != 0 && a[4] < 0x1000u) ? a[4] : 4u);
        a[8] = Raw(g);
        break;
    }
    case kPoolDtor:
    case kPoolSdd: {
        // a[4] = objetos (tipo: 0 nulo, 1 arena C, 2 crudo), a[5] = su desplazamiento/valor, a[6] = flags (tipo), a[7] = su desplazamiento/valor, a[8] = tamano, a[9] = ultimo libre, a[10] = byte de propietario (+0x10),
        // a[11] = banderas de pila (solo kPoolSdd)
        a[4] = g.Pk({1u, 1u, 2u, 0u}); a[5] = a[4] == 2 ? Raw(g) : AuxOffset(g, 0x10);
        a[6] = g.Pk({1u, 1u, 2u, 0u}); a[7] = a[6] == 2 ? Raw(g) : AuxOffset(g, 0x10);
        a[8] = g.Pk({1u, 2u, 100u, 4096u, 0x7FFFFFFFu, 0u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFF0u, g.W(), g.R(100), g.R(0x10000)});
        a[9] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 5u});
        a[10] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, g.R(256)});
        a[11] = g.Pk({0u, 1u, 1u, 0xFFu, 0xFEu, 2u, 0x100u, 0x101u}) | (g.P(50) ? (g.W() & 0xFFFFFF00u) : 0u);
        break;
    }
    case kPoolNew: {
        // a[4] = tamano (1..255), a[5] = ultimo libre (en [-8, tamano - 1]), a[6] = objetos en C (desplazamiento), a[7] = flags en D (desplazamiento), a[8] = densidad de flags libres (porcentaje),
        // a[9] = relleno de los flags: 0 por densidad, 1 todos libres, 2 ninguno libre, 3 solo uno libre (a[10] = indice)
        const u32 cap = g.Pk({1u, 2u, 3u, 5u, 8u, 16u, 31u, 64u, 100u, 200u, 255u, 1u + g.R(255)});
        a[4] = cap;
        const u32 lp = g.R(10);
        a[5] = lp == 0 ? 0xFFFFFFFFu : (lp == 1 ? cap - 1u : (lp == 2 ? u32(-i32(1 + g.R(8))) : (lp == 3 ? 0u : g.R(cap))));
        if (a[5] != 0xFFFFFFFFu && i32(a[5]) >= i32(cap)) a[5] = cap - 1u;
        a[6] = AuxOffset(g, cap * 12);
        a[7] = 8 + AuxOffset(g, cap + 16);
        a[8] = g.Pk({0u, 10u, 30u, 50u, 70u, 90u, 100u});
        a[9] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 3u});
        a[10] = g.R(cap);
        break;
    }
    case kPoolDel: {
        // a[4] = tamano del pool (no lo lee la funcion), a[5] = ultimo libre (con signo), a[6] = indice del objeto (con signo), a[7] = resto (0..11) sumado a la direccion, a[8] = objetos en C, a[9] = flags en D,
        // a[10] = 1 si el hueco ya estaba libre
        const u32 cap = g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 64u, 100u, 128u, 255u});
        a[4] = cap;
        const u32 ip = g.R(8);
        a[6] = ip == 0 ? 0u : (ip == 1 ? cap - 1u : (ip == 2 ? u32(-i32(1 + g.R(8))) : (ip == 3 ? cap + g.R(4) : g.R(cap))));
        const u32 idx = a[6];
        a[5] = g.Pk({0xFFFFFFFFu, 0u, idx - 1u, idx, idx + 1u, cap - 1u, cap, cap + 5u, g.R(cap), g.R(cap), 0x7FFFFFFFu, 0x80000000u, 0x80000001u});
        a[7] = g.Pk({0u, 0u, 0u, 1u, 5u, 11u, g.R(12)});
        a[8] = AuxOffset(g, 0x40);
        a[9] = 0x10 + AuxOffset(g, cap + 0x40);
        a[10] = g.Pk({0u, 0u, 1u});
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
    return j.End();
}
}
