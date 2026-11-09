#pragma once
// Fixtures del banco Fn132A (lote 132A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado se atribuian por cercania: las reales salen del desensamblado, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                                forma
//   0   0x41B660   CPatrolRoutePool::New                                       thiscall() `ret`: siguiente estado libre del pool (cursor + 1, una vuelta), incrementa el id de 7 bits, devuelve objetos + cursor * 0x1A4 o 0
//   1   0x611F20   CTaskComplexFollowLeaderInFormation::Offsets::GetFormationOffset  thiscall(dx, dy) `ret 8`: base + indice * 8 segun el par de signos
//   2   0x6F9EB0   CScripted2dEffects (CScriptedEffectPairs::Reset)             thiscall() `ret`: cuatro pares a -1 (salvo el PartnerTask del segundo efecto) y NumPairs = 0
//   3   0x5D0570   JPegPlugin (JPegScanlineToRaster)                           cdecl(raster, scanline, first, last) `ret`: pixeles first..last-1 de la scanline (0xBD0170) a RwRGBAToPixel (0x803740) y al cursor global (0xBD0160)
//   4   0x6FBA20   CCoronas (SunReflectionDistanceFactor)                      cdecl(x, y, z, radius) `ret`: ST0 = 1.0 / 0.0 / (distancia - radio) * 0.01 con la x87 a 24 bits (camara = matriz o placement)
//   5   0x6007A0   CDecision::Choices::Pick                                    thiscall(seek, int16* slot, int32* index) `ret 0Ch`: busca la tarea o tira rand() (0x821B1E) y elige la primera acumulada >= roll
//   6   0x683950   CTaskComplexWaitForPed (cuerpo de Clone)                    thiscall() `ret`, marco SEH: operator new(0x2C) (0x61A5A0) + constructor (0x683340)
//   7   0x684D10   CTaskComplexPartnerDeal::Clone                              thiscall() `ret`, marco SEH: operator new(0x74) (0x61A5A0) + constructor (0x684190)
//   8   0x636180   CTaskSimpleTired::Clone                                     thiscall() `ret`, marco SEH: operator new(0x1C) (0x61A5A0) + construccion en linea (vtable 0x86DED0)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, donde hace falta, memoria global del juego (se guarda y se restaura tras cada lado).
// Familia = n % 16: tipo = familia % 4 (ver cada funcion) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F132AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 9;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Extremo de lo que cada funcion toca a partir de su puntero (bytes), para colocar `this` al borde de la ventana
constexpr u32 kSpanPool = 0x10;             // cursor (+0xC) y tamano (+8)
constexpr u32 kSpanPairs = 0x94;            // CScriptedEffectPairs: ultimo byte escrito = +0x90
constexpr u32 kSpanRaster = 0x24;           // cFormat en +0x23
constexpr u32 kSpanChoices = 0x34;
constexpr u32 kSpanWaitTask = 0x19;         // ped (+0xC), radio (+0x10), tiempo (+0x14), byte (+0x18)
constexpr u32 kSpanPartnerTask = 0x59;      // comando (+0x14), companero (+0x38), multiplicador (+0x3C), punto (+0x40), lider (+0x58)
constexpr u32 kSpanTiredTask = 0xC;         // duracion (+8)

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
    std::vector<u8> s;          // datos en bytes (cadena, bytes de tabla)
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

// Bits de un float "interesante": cero con signo, uno, denormales, extremos, infinitos, NaN (silenciosos y senalizantes), limites de x128 (+-32768, +-65536, 2^31, 2^32...), potencias de dos grandes y aleatorios
inline u32 FloatBits(Gen& g) {
    switch (g.R(12)) {
    case 0: case 1: case 2:
        return g.Pk({0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x00000001u, 0x80000001u, 0x00800000u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F800000u, 0xFF800000u,
                     0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFFA00000u, 0x7FFFFFFFu, 0x43000000u, 0xC3000000u});
    case 3: case 4: case 5: {
        static constexpr double ks[] = {32766., 32767., 32768., 32769., 65535., 65536., 65537., 131071., 4194303., 16777215., 16777216., 2147483520., 2147483648., 4294967168., 4294967296., 1., 2., 127., 128., 129., 255., 256.};
        const double k = ks[g.R(u32(std::size(ks)))];
        const double fr = g.Pk({0u, 0u, 1u, 2u, 3u, 4u}) / 4.0;
        const double sign = g.P(50) ? -1.0 : 1.0;
        const float f = float(sign * (k + fr) / 128.0);
        u32 b; std::memcpy(&b, &f, 4); return b;
    }
    case 6: case 7: {
        const int e = int(g.Pk({24u, 30u, 31u, 32u, 33u, 40u, 50u, 55u, 56u, 57u, 62u, 63u, 64u, 70u, 100u, 120u, 121u, 127u})) - 7;
        const float m = 1.0f + float(g.W() & 0x7FFFFFu) / 8388608.0f;
        const float f = (g.P(50) ? -1.0f : 1.0f) * std::ldexp(m, e);
        u32 b; std::memcpy(&b, &f, 4); return b;
    }
    default: {
        const float scale = float(g.Pk({1u, 100u, 300u, 1000u, 100000u, 1000000000u}));
        const float f = (float(g.W()) / 4294967296.0f - 0.5f) * 2.0f * scale;
        u32 b; std::memcpy(&b, &f, 4); return b;
    }
    }
}

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


inline u32 Bits(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
inline float FromBits(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento principal, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; el resto por funcion (ver abajo y fixture_oracle.py)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 win = u32(kWin);
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (c.fn) {
    case 0: {   // CPatrolRoutePool::New: a[0] = this (arena A), a[3] = desplazamiento de los estados (arena B), a[4] = tamano, a[5] = cursor, a[6] = puntero de objetos (crudo); s = los estados
        const u32 size = g.P(40) ? g.Pk({1u, 2u, 3u, 4u, 5u, 8u, 16u, 32u, 64u, 100u}) : 1u + g.R(100);
        c.a[4] = size;
        c.a[6] = g.Pk({0u, 0xFFFFFFFFu, 0x00400000u + g.R(0x800000), g.W(), g.W(), 0x7FFFFE00u, 0xFFFFFE00u, 0x10000u});
        switch (g.R(6)) {
        case 0: c.a[5] = 0xFFFFFFFFu; break;
        case 1: c.a[5] = size - 1; break;
        case 2: c.a[5] = 0; break;
        case 3: c.a[5] = size >= 2 ? size - 2 : 0xFFFFFFFFu; break;
        default: c.a[5] = g.R(size + 1) - 1u; break;     // -1 .. size - 1
        }
        const u32 density = g.Pk({0u, 1u, 3u, 10u, 25u, 50u, 90u, 100u, 5u, 70u});
        std::vector<u8> s(size);
        for (auto& b : s) {
            const bool freeSlot = g.R(100) < density;
            const u8 ref = u8(g.Pk({0u, 1u, 0x7Eu, 0x7Fu, g.W() & 0x7Fu, g.W() & 0x7Fu}));
            b = u8(ref | (freeSlot ? 0x80u : 0u));
        }
        if (kind == 1 && size >= 2) {    // un unico libre (puede estar antes o despues del cursor: vuelta completa)
            for (auto& b : s) b &= 0x7F;
            s[g.R(size)] |= 0x80;
        }
        if (kind == 2) for (auto& b : s) b &= 0x7F;     // pool lleno
        c.s = s;
        c.a[0] = Offset(g, kind == 3 ? 2u : (kind == 2 ? 1u : 0u), kSpanPool);
        c.a[3] = Offset(g, kind == 3 ? 3u : (kind == 2 ? 2u : 0u), size);
        break;
    }
    case 1: {   // GetFormationOffset: a[3] = dx, a[4] = dy (ECX = base cruda, c.ecx)
        auto sign = [&]() { return g.Pk({0xFFFFFFFFu, 0u, 1u}); };
        auto other = [&]() { return g.Pk({2u, 0xFFFFFFFEu, 0x80000000u, 0x7FFFFFFFu, 0x100u, 0xFFFFFF00u, 0xFFFFFFFCu, g.W(), 0x80000001u, 3u}); };
        switch (kind) {
        case 0: c.a[3] = sign(); c.a[4] = sign(); break;
        case 1: c.a[3] = sign(); c.a[4] = other(); break;
        case 2: c.a[3] = other(); c.a[4] = g.P(50) ? sign() : other(); break;
        default: c.a[3] = g.P(70) ? sign() : g.W(); c.a[4] = g.P(70) ? sign() : g.W(); break;
        }
        c.ecx = g.Pk({0u, 0xFFFFFFFFu, 0x00C196E8u, g.W(), g.W(), 0xFFFFFFF8u, 0x7FFFFFFCu});
        break;
    }
    case 2:     // CScriptedEffectPairs::Reset
        c.a[0] = Offset(g, kind, kSpanPairs);
        break;
    case 3: {   // JPegScanlineToRaster: a[0] = raster (arena A), a[3] = first, a[4] = last, a[5] = desplazamiento del cursor (arena B), a[6] = scanline, a[7] = byte de formato
        u32 first, last;
        switch (g.R(10)) {
        case 0: first = 0x40; last = 0x240; break;                                     // el caso real
        case 1: first = g.R(0x240); last = first; break;                               // vacio
        case 2: last = g.R(0x240); first = last + 1 + g.R(0x240 - last); break;       // first > last
        case 3: first = g.Pk({0xFFFFFFFFu, 0x80000000u, 0xFFFFFF00u, 0x241u, 0x1000u}); last = g.Pk({0u, 1u, 0x40u, 0x240u, 0x7FFFFFFFu, 0x80000000u}); if (first < last) std::swap(first, last); break;
        case 4: case 5: first = g.R(0x240); last = std::min(0x240u, first + 1 + g.R(8)); break;
        case 6: first = 0; last = g.Pk({1u, 2u, 3u, 0x240u, 0x100u}); break;
        default: first = g.R(0x230); last = std::min(0x240u, first + 1 + g.R(40)); break;
        }
        c.a[3] = first; c.a[4] = last; c.a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), 599u});
        c.a[7] = g.Pk({0u, 1u, 2u, 4u, 8u, 0x80u, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu, 0x0Eu, 0x01u});
        const u32 cnt = first < last ? last - first : 0;
        c.a[0] = Offset(g, kind == 2 ? 2u : (kind == 1 ? 1u : 0u), kSpanRaster);
        if (kind == 2) c.a[5] = win - 4 * (cnt > 0 ? cnt : 1) - 4 + g.R(0x30);
        else if (kind == 1) c.a[5] = 4 * g.R((win - 4 * cnt) / 4 - 1) + 1 + g.R(3);
        else c.a[5] = 4 * g.R((win - 4 * cnt) / 4);
        break;
    }
    case 4: {   // SunReflectionDistanceFactor: a[3..5] = x, y, z (bits), a[6] = radio, a[7], a[8] = camara (bits), a[9] = 1 si la camara esta en una matriz (arena A, +0x30), a[10] = desplazamiento de la matriz
        const double scale = double(g.Pk({1u, 10u, 100u, 1000u, 3000u, 6000u}));
        auto coord = [&]() { return float((double(g.W()) / 4294967296.0 - 0.5) * 2.0 * scale); };
        const float cx = coord(), cy = coord();
        float px = coord(), py = coord(), radius = float(g.Pk({0u, 1u, 50u, 100u, 250u, 350u, g.R(1000), g.R(5000), g.R(300)}));
        c.a[7] = Bits(cx); c.a[8] = Bits(cy);
        switch (kind) {
        case 0: break;      // aleatorio
        case 1: {           // cerca de los umbrales radio y radio + 100 (distancia con unos pocos ulps de diferencia)
            const double base = double(radius) + (g.P(50) ? 0.0 : 100.0);
            const double step = std::ldexp(1.0, int(std::floor(std::log2(std::max(1.0, base)))) - 23 + int(g.R(5)) - 2);
            const double d = base + double(int(g.R(9)) - 4) * step;
            const double ang = double(g.W()) / 4294967296.0 * 6.283185307179586;
            px = float(double(cx) - d * std::cos(ang)); py = float(double(cy) - d * std::sin(ang));
            break;
        }
        case 2:             // especiales
            c.a[7] = FloatBits(g); c.a[8] = FloatBits(g);
            px = FromBits(FloatBits(g)); py = FromBits(FloatBits(g)); radius = FromBits(FloatBits(g));
            break;
        default: {          // exponentes extremos (la x87 no desborda donde un float si)
            const float big = std::ldexp(1.0f + float(g.W() & 0x7FFFFFu) / 8388608.0f, int(g.Pk({40u, 60u, 80u, 100u, 120u, 126u, 20u})));
            const float tiny = std::ldexp(1.0f + float(g.W() & 0x7FFFFFu) / 8388608.0f, -int(g.Pk({40u, 60u, 80u, 100u, 120u, 126u, 149u})));
            const float v1 = g.P(50) ? big : tiny, v2 = g.P(50) ? big : tiny;
            c.a[7] = Bits(g.P(50) ? v1 : -v1); c.a[8] = Bits(g.P(50) ? v2 : -v2);
            px = g.P(50) ? v2 : -v1; py = g.P(50) ? -v2 : v1;
            radius = g.Pk({0u, 1u, 2u}) == 0 ? tiny : (g.P(50) ? big : float(g.R(200)));
            break;
        }
        }
        c.a[3] = Bits(px); c.a[4] = Bits(py); c.a[5] = g.P(10) ? g.W() : Bits(float(g.R(100)));
        c.a[6] = Bits(radius);
        c.a[9] = g.P(50) ? 1u : 0u;
        c.a[10] = Offset(g, 0u, 0x38);
        break;
    }
    case 5: {   // CDecision::Choices::Pick: a[0] = this (arena A), a[3] = contador, a[4] = tarea buscada, a[5] = valor de rand(), a[6] = desplazamiento del word `slot` (arena B), a[7] = del dword `index` (arena C);
                // s = 6 tareas (dwords) y 6 acumuladas (floats)
        const u32 r = g.Pk({0u, 1u, 2u, 16383u, 16384u, 32766u, 32767u, g.R(32768), g.R(32768), g.R(32768)});
        c.a[5] = r;
        const float roll = float(double(r) * double(FromBits(0x38000100u)));
        const u32 count = kind == 3 ? g.Pk({0u, 1u, 6u, 0xFFFFFFFFu, 0x80000000u, 7u, 8u, 9u, 2u}) : g.Pk({1u, 2u, 3u, 4u, 5u, 6u, 6u, 6u, 0u, 1u, 3u});
        c.a[3] = count;
        std::array<u32, 6> tasks{}, cumul{};
        for (u32 i = 0; i < 6; ++i)
            tasks[i] = g.Pk({0xFFFFFFFFu, 0u, 1u, 200u, 201u, 0x1234u, 0x12340005u, 0x80000000u, 3u + g.R(900), 3u + g.R(900), 3u + g.R(900), 0xFFFF0000u + g.R(0x10000)});
        if (g.P(25)) { const u32 i = g.R(6), j = g.R(6); tasks[i] = tasks[j]; }
        // acumuladas: crecientes y normalizadas (caso real) o especiales
        const u32 shape = g.R(8);
        float acc = 0.0f;
        const u32 used = count > 0 && count <= 6 ? count : 6;
        for (u32 i = 0; i < 6; ++i) {
            float v;
            if (shape < 4) { acc += float(g.R(100) + 1) / 100.0f; v = acc; }
            else v = FromBits(FloatBits(g));
            cumul[i] = Bits(v);
        }
        if (shape < 4) {
            const float inv = 1.0f / acc;
            for (u32 i = 0; i < 6; ++i) cumul[i] = Bits(inv * FromBits(cumul[i]));
        }
        switch (g.R(6)) {   // fija la frontera: un valor igual al roll o a sus vecinos (el <= importa)
        case 0: cumul[g.R(used)] = Bits(roll); break;
        case 1: cumul[g.R(used)] = Bits(std::nextafter(roll, 2.0f)); break;
        case 2: cumul[g.R(used)] = Bits(std::nextafter(roll, -1.0f)); break;
        case 3: for (u32 i = 0; i < 6; ++i) cumul[i] = Bits(std::max(roll, FromBits(cumul[i]))); break;
        default: break;
        }
        // tarea buscada: -1, una de las tareas (de las `count` primeras o de las otras), 200 o ninguna
        switch (g.R(6)) {
        case 0: case 1: c.a[4] = 0xFFFFFFFFu; break;
        case 2: c.a[4] = tasks[g.R(used)]; break;
        case 3: c.a[4] = tasks[g.R(6)]; break;
        case 4: c.a[4] = 200u; break;
        default: c.a[4] = 0x7000u + g.R(100); break;
        }
        std::vector<u8> s(48);
        for (u32 i = 0; i < 6; ++i) { std::memcpy(&s[4 * i], &tasks[i], 4); std::memcpy(&s[24 + 4 * i], &cumul[i], 4); }
        c.s = s;
        c.a[0] = Offset(g, kind == 2 ? 2u : (kind == 1 ? 1u : 0u), kSpanChoices + 0x14);
        c.a[6] = 2 * g.R((win - 8) / 2) + (kind == 1 ? 1 : 0);
        c.a[7] = 4 * g.R((win - 8) / 4);
        break;
    }
    default: {  // Clones (6, 7, 8): a[0] = this (arena A); a[3] = origen de la memoria: 0 nula, 1 arena B (a[4]), 2 solapada con this en A (a[4] = delta con signo); a[5] = EAX del constructor: 0 = la memoria,
                // 1 = a[6], 2 = cero, 3 = memoria + 4; a[7..11] = campos del objeto (ver Setup / fixture_oracle.py); a[16] = byte alto de los campos de un byte
        const u32 span = c.fn == 6 ? kSpanWaitTask : (c.fn == 7 ? kSpanPartnerTask : kSpanTiredTask);
        const u32 memSize = c.fn == 6 ? 0x2C : (c.fn == 7 ? 0x74 : 0x1C);
        c.a[0] = Offset(g, kind, span);
        c.a[3] = g.Pk({0u, 1u, 1u, 1u, 1u, 2u, 2u});
        if (c.a[3] == 2) {
            c.a[4] = u32(g.Pk({0u, 4u, 8u, 0xFFFFFFF8u, 0xFFFFFFFCu, 1u, 0x10u, 0xFFFFFFF0u, 0x20u, 0xFFFFFFC0u, 0xC8u}));
            const i32 pos = i32(c.a[0]) + i32(c.a[4]);
            if (pos < 0 || u32(pos) + memSize + 4 > win) c.a[4] = 0;
        } else {
            c.a[4] = Offset(g, g.P(50) ? 0u : (g.P(50) ? 1u : 3u), memSize);
        }
        c.a[5] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        c.a[6] = g.Pk({0x00400000u + g.R(0x800000), 0u, 0xFFFFFFFFu, 0x00C0FFEEu});
        c.a[7] = g.Pk({g.W(), 0u, 0xFFFFFFFFu, 0x00400000u + g.R(0x800000)});
        c.a[8] = g.P(30) ? FloatBits(g) : g.W();
        c.a[9] = g.W();
        c.a[10] = g.P(30) ? FloatBits(g) : g.W();
        c.a[11] = g.W();
        c.a[16] = g.Pk({0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, g.W() & 0xFFu});
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
