#pragma once
// Fixtures del banco Fn133A (lote 133A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado se atribuian por cercania: las reales salen del desensamblado, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                   forma
//   0   0x5A7A20   CClothes::GetClothesModelFromName             cdecl(const char*) `ret`: indice de la pieza (torso, head, hands, legs, feet, necklace, watch, glasses, hat, extra1) o 0
//   1   0x72FC70   CameraSize                                    cdecl(camera, rect, viewWindow, aspect) `ret`, void: raster, ventana de vista (x87), RsGlobal
//   2   0x694850   CTaskComplexFollowLeaderInFormation::Offsets::Constructor   thiscall() `ret`: escribe 0x84 bytes constantes y devuelve this
//   3   0x616920   CPedPlacement::FindZCoorForPed                cdecl(CVector*) `ret`, AL: dos sondas verticales (0x5674E0) y z + 1 en el sitio
//   4   0x65C530   CTaskComplexSmartFleeEntity::CreateSubTask    thiscall(tipo) `ret 4`, marco SEH: tarea de parado (0xCB) o de huida de punto (0x38E)
//   5   0x477DA0   CTheScripts::IsPointInAngledArea              cdecl(CVector2D*, x1, y1, x2, y2, ancho) `ret`, AL: rectangulo angulado (x87) y dibujo de depuracion
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

namespace F133AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 6;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Extremo de lo que cada funcion toca a partir de su puntero (bytes), para colocar los objetos al borde de la ventana
constexpr u32 kSpanCamera = 0x68;     // CameraSize lee +0x60 y +0x64
constexpr u32 kSpanRaster = 0x14;     // ancho +0xC, alto +0x10
constexpr u32 kSpanRect = 0x10;
constexpr u32 kSpanOffsets = 0x84;
constexpr u32 kSpanVec3 = 0xC;
constexpr u32 kSpanFlee = 0x40;
constexpr u32 kSpanEntityPos = 0x20;  // CEntity: +4 posicion (12 bytes), +0x14 matriz
constexpr u32 kSpanMatrix = 0x40;     // pos en +0x30
constexpr u32 kSpanFleeBlock = 0x44;
constexpr u32 kSpanVec2 = 8;

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

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento principal, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[20..23] = semillas de las arenas A, B, C y de la memoria global; el resto por funcion (ver abajo y fixture_oracle.py)

inline u32 FBitsOf(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// Float de formas variadas: especiales (ceros con signo, denormales, extremos, infinitos, NaN silenciosos y senalizantes) o un valor normal con una escala.
inline u32 FBits(Gen& g, u32 specialPct = 30) {
    if (g.P(specialPct))
        return g.Pk({0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x40000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F800000u, 0xFF800000u,
                     0x7FC00000u, 0xFFC00000u, 0x7FC12345u, 0x7F800001u, 0xFFA00000u, 0x7FA55555u, 0x3DCCCCCDu, 0x42C80000u, 0xC2C80000u, 0x7E800000u, 0x1F800000u, 0xDF800000u});
    const float scale = float(g.Pk({1u, 10u, 100u, 1000u, 100000u}));
    const float f = (float(g.W()) / 4294967296.0f - 0.5f) * 2.0f * scale;
    return FBitsOf(f);
}
// Posicion dentro de [lo, hi) para un objeto de `span` bytes: 0 alineado, 1 desalineado, 2 al final de la zona, 3 cerca del inicio
inline u32 Place(Gen& g, u32 kind, u32 span, u32 lo, u32 hi) {
    const u32 room = hi - lo - span;
    switch (kind) {
    case 0: return lo + 4 * g.R(room / 4 + 1);
    case 1: return lo + 4 * g.R(room / 4 - 1) + 1 + g.R(3);
    case 2: return hi - span - g.R(8) * 4 - g.R(2);
    default: return lo + g.R(8) * 4;
    }
}
inline u32 DimBits(Gen& g) {
    return g.Pk({0u, 1u, 2u, 3u, 5u, 100u, 320u, 640u, 800u, 1024u, 1280u, 1366u, 1920u, 2048u, 4096u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x01000001u, 0x00FFFFFFu,
                 1 + g.R(4000), 1 + g.R(4000), 1 + g.R(4000), g.W()});
}
constexpr const char* kClothesNames[10] = {"torso", "head", "hands", "legs", "feet", "necklace", "watch", "glasses", "hat", "extra1"};

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 win = u32(kWin);
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[20] = c.seed * 13u + 50u; c.a[21] = c.a[20] + 1u; c.a[22] = c.a[20] + 2u; c.a[23] = c.a[20] + 3u;
    switch (c.fn) {
    case 0: {   // GetClothesModelFromName: a[0] = desplazamiento de la cadena en la arena C, a[3] = forma de la cadena
        const u32 which = g.R(10);
        const std::string nm = kClothesNames[which];
        const u32 shape = g.R(100);
        std::string s;
        if (shape < 50) { s = nm; c.a[3] = 0; }
        else if (shape < 60) { s = nm.substr(0, 1 + g.R(u32(nm.size()) - 1)); c.a[3] = 1; }                                      // prefijo
        else if (shape < 68) { s = nm + char('a' + g.R(26)); c.a[3] = 2; }                                                        // con una letra de mas
        else if (shape < 76) { s = nm; const u32 p = g.R(u32(s.size())); s[p] = char(s[p] == 'z' ? 'a' : s[p] + 1); c.a[3] = 3; } // una letra cambiada
        else if (shape < 82) { s = nm; if (g.P(50)) { for (auto& ch : s) ch = char(ch - 32); } else { s[0] = char(s[0] - 32); } c.a[3] = 4; }   // mayusculas
        else if (shape < 86) { s = ""; c.a[3] = 5; }
        else if (shape < 93) { const u32 len = 1 + g.R(12); for (u32 i = 0; i < len; ++i) s += char("abcdefghijklmnopqrstuvwxyz0123456789"[g.R(36)]); c.a[3] = 6; }
        else if (shape < 97) { s = nm; s[g.R(u32(s.size()))] = char(0x80 + g.R(128)); c.a[3] = 7; }                              // byte alto
        else { const u32 v = g.R(4); s = v == 0 ? "extra" : (v == 1 ? "extra12" : (v == 2 ? "torso " : " head")); c.a[3] = 8; }
        c.s.assign(s.begin(), s.end());
        const u32 len = u32(s.size());
        c.a[0] = kind == 0 ? 4 * g.R((win - 40) / 4) : kind == 1 ? 4 * g.R((win - 40) / 4) + 1 + g.R(3) : kind == 2 ? win - len - 1 + g.R(2) : g.R(8);
        break;
    }
    case 1: {   // CameraSize
        // a[0] camara (A, mitad baja), a[3] raster (B), a[4] raster z (C), a[5] 1 = el mismo raster, a[6] rectangulo (A, mitad alta), a[7] 1 = rectangulo dado, a[8] ancho y a[9] alto del raster,
        // a[10], a[11] ancho y alto del rectangulo, a[12], a[13] ancho y alto del modo, a[14] banderas del modo, a[15] indice de modo, a[16] viewWindow, a[17] aspecto, a[18], a[19] tamano previo del raster z
        c.a[0] = Place(g, kind, kSpanCamera, 0, win / 2);
        c.a[3] = Place(g, (kind + 1) % 4, kSpanRaster, 0, win);
        c.a[4] = Place(g, (kind + 2) % 4, kSpanRaster, 0, win);
        c.a[5] = g.P(10) ? 1u : 0u;
        c.a[6] = Place(g, (kind + 3) % 4, kSpanRect, win / 2, win);
        c.a[7] = g.P(65) ? 1u : 0u;
        c.a[8] = DimBits(g); c.a[9] = DimBits(g);
        switch (kind) {
        case 0: c.a[10] = c.a[8]; c.a[11] = c.a[9]; break;
        case 1: c.a[10] = DimBits(g); c.a[11] = c.a[9]; break;
        case 2: c.a[10] = c.a[8]; c.a[11] = DimBits(g); break;
        default: c.a[10] = DimBits(g); c.a[11] = DimBits(g); break;
        }
        c.a[12] = g.P(30) ? c.a[8] : DimBits(g); c.a[13] = g.P(30) ? c.a[9] : DimBits(g);
        c.a[14] = g.P(20) ? g.Pk({0u, 1u}) : (g.P(50) ? (g.W() | 1u) : (g.W() & ~1u));
        c.a[15] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 17u, g.W()});
        c.a[16] = g.P(70) ? FBitsOf(0.1f + float(g.R(3000)) / 1000.0f) : FBits(g, 100);
        c.a[17] = g.P(70) ? g.Pk({0x3FAAAAABu, 0x3FE38E39u, 0x3F800000u, 0x3F400000u, 0x40000000u, 0x3F19999Au}) : FBits(g, 100);
        c.a[18] = DimBits(g); c.a[19] = DimBits(g);
        break;
    }
    case 2:     // Offsets::Constructor: a[0] = this (A)
        c.a[0] = Offset(g, kind, kSpanOffsets);
        break;
    case 3: {   // FindZCoorForPed: a[0] = pos (A), a[3..5] = x, y, z, a[6], a[8] = EAX de la 1a y 2a sonda, a[7], a[9] = z del punto de colision
        c.a[0] = Offset(g, kind, kSpanVec3);
        for (int i = 3; i <= 5; ++i) c.a[i] = g.P(65) ? FBitsOf((float(g.W()) / 4294967296.0f - 0.5f) * float(g.Pk({10u, 1000u, 6000u}))) : FBits(g, 100);
        const auto retv = [&](bool hit) { return hit ? g.Pk({1u, 1u, 1u, 2u, 0xFFu, 0x101u, 0xFFFFFF01u, g.W() | 1u, 0x00BADC01u}) : g.Pk({0u, 0u, 0u, 0x100u, 0xFFFFFF00u, g.W() & ~0xFFu, 0xBADC0000u}); };
        const auto zb = [&]() {
            if (g.P(35)) return g.Pk({0xC2C60000u, 0xC2C60001u, 0xC2C5FFFFu, 0xC2C80000u, 0xC2C80001u, 0xC2C7FFFFu, 0xC2C40000u, 0xC1200000u, 0x00000000u, 0x80000000u, 0x7FC00000u, 0xFFC00000u, 0xC2C60002u, 0xC2C5FFFEu});
            if (g.P(30)) return FBits(g, 100);
            return FBitsOf((float(g.W()) / 4294967296.0f - 0.4f) * float(g.Pk({10u, 300u, 3000u})));
        };
        const bool h1 = kind == 0 || kind == 1 || (kind == 3 && g.P(50)), h2 = kind == 0 || kind == 2 || (kind == 3 && g.P(50));
        c.a[6] = retv(h1); c.a[8] = retv(h2);
        c.a[7] = zb(); c.a[9] = zb();
        if (kind == 0 && g.P(40)) c.a[9] = c.a[7];            // empate
        break;
    }
    case 4: {   // CreateSubTask: a[0] this (A), a[3] entidad (B, mitad baja), a[4] matriz (C), a[5] 1 = con matriz, a[6] tipo, a[7] tiempo (+0x1C), a[8] distancia segura (+0x20), a[9] dword +0x24 (el byte bajo es el grito),
                // a[10] periodo (+0x28), a[11] tolerancia (+0x2C), a[12] reloj (CTimer 0xB7CB84), a[13] valor de rand(), a[14] bloque de operator new (B, mitad alta), a[15] 1 = operator new devuelve nulo,
                // a[16..18] posicion de la entidad (x, y, z crudos)
        c.a[0] = Offset(g, kind, kSpanFlee);
        c.a[3] = Place(g, (kind + 1) % 4, kSpanEntityPos, 0, win / 2);
        c.a[4] = Place(g, (kind + 2) % 4, kSpanMatrix, 0, win);
        c.a[5] = g.P(50) ? 1u : 0u;
        c.a[6] = g.P(70) ? g.Pk({0xCBu, 0x38Eu, 0x516u}) : g.Pk({0u, 1u, 0xCAu, 0xCCu, 0x38Du, 0x38Fu, 0x515u, 0x517u, 0xFFFFFFFFu, 0x80000000u, 0x100000CBu, 0x1000038Eu, g.W()});
        c.a[7] = g.Pk({0u, 1u, 1000000u, 0xFFFFFFFFu, 5000u, g.W()});
        c.a[8] = FBits(g, 40);
        c.a[9] = g.P(50) ? g.Pk({0u, 1u}) : ((g.W() & 0xFFFFFF00u) | g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu}));
        c.a[10] = g.Pk({0u, 1000u, 0xFFFFFFFFu, 1u, 5000u, 0x7FFFFFFFu, g.W()});
        c.a[11] = FBits(g, 40);
        c.a[12] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, g.W(), g.W()});
        c.a[13] = g.Pk({0u, 1u, 32767u, 32768u, 65535u, 65536u, 0x7FFFFFFFu, 0xFFFFFFFFu, g.R(32768), g.R(32768), g.R(32768), g.W(), 21845u});
        c.a[14] = Place(g, kind, kSpanFleeBlock, win / 2, win);
        c.a[15] = g.P(10) ? 1u : 0u;
        for (int i = 16; i <= 18; ++i) c.a[i] = g.P(75) ? FBitsOf((float(g.W()) / 4294967296.0f - 0.5f) * float(g.Pk({10u, 1000u, 6000u}))) : FBits(g, 100);
        break;
    }
    default: {  // IsPointInAngledArea: a[0] = punto (A), a[3], a[4] = x, y del punto, a[5..8] = x1, y1, x2, y2, a[9] = ancho, a[10] = angulo del falso de GetRadianAngleBetweenPoints, a[11] = DbgFlag (byte)
        c.a[0] = Offset(g, kind, kSpanVec2);
        const u32 mode = g.R(100);
        const float scale = float(g.Pk({10u, 200u, 3000u, 6000u}));
        const auto coord = [&]() { return (float(g.W()) / 4294967296.0f - 0.5f) * scale; };
        float x1 = coord(), y1 = coord(), x2 = coord(), y2 = coord();
        float w = (float(g.W()) / 4294967296.0f - 0.5f) * float(g.Pk({10u, 100u, 600u}));
        if (g.P(10)) w = 0.0f;
        const float dx = x2 - x1, dy = y2 - y1;
        constexpr double kPi = 3.14159265358979323846;
        const double theta = std::atan2(double(dy), double(dx)) + kPi / 2;
        float angle = float(theta);
        if (g.P(30)) angle = float(theta - 2 * kPi * g.R(3));
        float px = coord(), py = coord();
        if (mode < 60) {
            // punto en torno al rectangulo: a lo largo de AB (-0.2..1.2) y a lo ancho (-0.2..1.2 de w)
            const double len = std::sqrt(double(dx) * dx + double(dy) * dy);
            const double along = (double(g.R(1401)) - 200.0) / 1000.0, across = (double(g.R(1401)) - 200.0) / 1000.0;
            if (len > 0) {
                px = float(x1 + along * dx + across * w * (-dy / len));
                py = float(y1 + along * dy + across * w * (dx / len));
            }
            if (g.P(15)) { px = x1; py = y1; }
            if (g.P(8)) { px = x2; py = y2; }
        } else if (mode < 75) {
            angle = float(double(g.R(100000)) / 100000.0 * 2 * kPi);
        } else if (mode < 85) {
            u32 b[5] = {FBitsOf(x1), FBitsOf(y1), FBitsOf(x2), FBitsOf(y2), FBitsOf(w)};
            b[g.R(5)] = FBits(g, 100);
            if (g.P(30)) b[g.R(5)] = FBits(g, 100);
            x1 = std::bit_cast<float>(b[0]); y1 = std::bit_cast<float>(b[1]); x2 = std::bit_cast<float>(b[2]); y2 = std::bit_cast<float>(b[3]); w = std::bit_cast<float>(b[4]);
            if (g.P(30)) px = std::bit_cast<float>(FBits(g, 100));
        } else if (mode < 92) {
            switch (g.R(4)) {
            case 0: x2 = x1; y2 = y1; break;                        // longitud cero
            case 1: x2 = x1; break;
            case 2: y2 = y1; break;
            default: w = 0.0f; px = x1 + (x2 - x1) * 0.5f; py = y1 + (y2 - y1) * 0.5f; break;
            }
            angle = float(std::atan2(double(y2 - y1), double(x2 - x1)) + kPi / 2);
        } else if (mode < 96) {
            const float kHalf = float(kPi / 2), kTwo = float(2 * kPi);
            switch (g.R(10)) {
            case 0: angle = -kHalf; break;
            case 1: angle = 0.0f; break;
            case 2: angle = kTwo - kHalf; break;
            case 3: angle = kTwo; break;
            case 4: angle = -kTwo; break;
            case 5: angle = float(g.R(300000)) / 100.0f - 1000.0f; break;     // muchas vueltas
            case 6: angle = std::bit_cast<float>(g.Pk({0x7FC00000u, 0xFFC00000u, 0x7FC12345u})); break;
            case 7: angle = -float(g.R(3000)) / 10.0f - 1.0f; break;
            case 8: angle = float(g.R(3000)) / 10.0f; break;
            default: angle = std::bit_cast<float>(g.Pk({0x00000001u, 0x80000000u, 0x3F800000u, 0xBFC90FDBu, 0xC0490FDBu})); break;
            }
        } else {
            px = std::bit_cast<float>(FBits(g, 100)); py = std::bit_cast<float>(FBits(g, 100));
        }
        c.a[3] = FBitsOf(px); c.a[4] = FBitsOf(py);
        c.a[5] = FBitsOf(x1); c.a[6] = FBitsOf(y1); c.a[7] = FBitsOf(x2); c.a[8] = FBitsOf(y2);
        c.a[9] = FBitsOf(w); c.a[10] = FBitsOf(angle);
        c.a[11] = g.Pk({0u, 1u, 0u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
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
