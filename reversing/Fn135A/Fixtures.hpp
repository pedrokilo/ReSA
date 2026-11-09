#pragma once
// Fixtures del banco Fn135A (lote 135A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook propio antes del lote; las clases del listado se atribuian por cercania: las reales salen del desensamblado, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                                forma
//   0   0x53F2D0   GetMouseSetUp (CMouseControllerState por valor)             cdecl(sret) `ret 4`: diMouseInit si no hay raton, GetCapabilities (sin mirar el HRESULT), botones por tabla y rueda con 3 ejes
//   1   0x747300   getDvdGamePath                                              cdecl() `ret`: recorre GetLogicalDriveStringsA, GetDriveTypeA, GetVolumeInformationA ("GTA_SAN_ANDREAS", 16 bytes), operator new[] / delete[] del EXE
//   2   0x50D590   CCollision / GetNearestDistanceOfPedSphereToCameraNearClip  cdecl(CPed*) `ret`, ST0: minimo de 12 esferas de golpe del peaton frente al plano cercano de la camara activa (x87 24 bits)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, donde hace falta, memoria global del juego (se guarda y se restaura tras cada lado).
// Familia = n % 16: tipo = familia % 4 (colocacion de los objetos en las arenas, ver Offset) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F135AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 3;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Extremo de lo que cada funcion toca a partir de su puntero (bytes), para colocar los objetos al borde de la ventana
constexpr u32 kSpanMouse = 0x14;            // CMouseControllerState (resultado por valor)
constexpr u32 kSpanPed = 0x24;              // el peaton: clump (+0x18) y modelo (word +0x22)
constexpr u32 kSpanModelInfo = 0x38;        // +0x34: colision de golpe
constexpr u32 kSpanResult = 8;              // cadena resultado de getDvdGamePath ("X:\" y el nulo)

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
    std::array<u32, 16> a{};
    std::vector<u8> s;          // datos en bytes (cadena, bytes de tabla)
    std::vector<u8> t, u;       // mas datos en bytes
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
//   a[0] = desplazamiento principal, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; el resto por funcion (ver abajo y fixture_oracle.py)

// Float "de escena" (pequeno), "de rejilla" (enteros pequenos: empates exactos), "enorme" (exponentes que desbordan el float pero no la x87), "especial" o cuantizado
inline u32 SceneBits(Gen& g, u32 mode, float scale) {
    float f;
    switch (mode) {
    case 0: f = (float(g.W()) / 4294967296.0f - 0.5f) * 2.0f * scale; break;                    // escena
    case 1: f = float(int(g.R(9)) - 4) * (g.P(25) ? 0.5f : 1.0f); break;                        // rejilla
    case 2: {                                                                                  // enorme
        const int e = int(g.Pk({60u, 70u, 80u, 100u, 110u, 120u, 126u, 127u})) * (g.P(50) ? 1 : -1);
        f = (g.P(50) ? -1.0f : 1.0f) * std::ldexp(1.0f + float(g.W() & 0x7FFFFFu) / 8388608.0f, e);
        break;
    }
    case 3: return FloatBits(g);                                                              // especial
    default: f = float(int(g.R(2001)) - 1000) / 8.0f; break;                                    // cuantizado
    }
    u32 b; std::memcpy(&b, &f, 4); return b;
}
inline u32 FBits(float v) { u32 b; std::memcpy(&b, &v, 4); return b; }
inline void PutF(std::vector<u8>& v, u32 bits) { for (int k = 0; k < 4; ++k) v.push_back(u8(bits >> (8 * k))); }

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 win = u32(kWin);
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (c.fn) {
    case 0: {   // GetMouseSetUp: a[0] = destino del resultado (arena A), a[3] = hay raton al entrar, a[4] = byte del menu, a[5] = EAX de IsVideoModeExclusive, a[6] = hay raton tras diMouseInit,
                // a[7] = dwButtons, a[8] = dwAxes, a[9] = HRESULT de GetCapabilities
        c.a[0] = Offset(g, kind, kSpanMouse);
        const u32 cls = g.R(4);
        c.a[3] = g.P(cls == 0 ? 70 : 40) ? 1u : 0u;
        c.a[6] = g.P(80) ? 1u : 0u;
        c.a[4] = cls == 3 ? g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu}) : g.Pk({0u, 0u, 0u, 1u});
        c.a[5] = cls == 3 ? g.Pk({0u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, g.W()}) : g.Pk({0u, 1u});
        c.a[9] = cls == 3 ? g.Pk({0u, 1u, 0x80004005u, 0xFFFFFFFFu, g.W()}) : g.Pk({0u, 0u, 0u, 0x80004005u});
        switch (cls) {
        case 0: c.a[7] = g.Pk({2u, 3u, 5u, 8u}); c.a[8] = g.Pk({2u, 3u, 3u, 3u}); break;
        case 1: c.a[7] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 12u, 16u, 0xFFFFFFFFu, 0x80000000u, 0x100u, 0x101u, g.R(20)}); c.a[8] = g.R(6); break;
        case 2: c.a[7] = g.Pk({1u, 2u, 3u, 4u, 8u, 9u}); c.a[8] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu, 0x80000003u, 0x103u, 0x10003u, g.W()}); break;
        default: c.a[7] = g.P(50) ? g.R(16) : g.W(); c.a[8] = g.P(50) ? g.R(6) : g.W(); break;
        }
        break;
    }
    case 1: {   // getDvdGamePath: a[0] = tamano que devuelve la primera llamada, a[3] = longitud que devuelve la segunda, a[5] = EAX de SetErrorMode, a[6] = bloque de unidades (arena B), a[7] = bloque del
                // resultado (arena C), a[8] = bits altos del tipo de unidad (llamadas impares), a[9] = valor distinto de cero que devuelve GetVolumeInformationA;
                // s = bloque de unidades (cadenas con nulo y nulo final), t[0..15] = tipo por llamada, t[16..31] = exito de GetVolumeInformationA, t[32..47] = nombre elegido
        std::vector<u8> s;
        const u32 count = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 10u, 12u, 3u, 4u});
        static constexpr char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789:\\/. -_";
        for (u32 i = 0; i < count && s.size() < 100; ++i) {
            if (g.P(75)) { s.push_back(u8('A' + g.R(26))); s.push_back(':'); s.push_back('\\'); }
            else { const u32 len = g.R(8); for (u32 k = 0; k < len; ++k) s.push_back(u8(alpha[g.R(u32(sizeof(alpha) - 1))])); }
            s.push_back(0);
        }
        s.push_back(0);
        c.s = s;
        const u32 L = u32(s.size());
        c.a[0] = g.P(70) ? L : g.Pk({0u, 1u, 3u, 100u, 300u, g.R(400)});
        switch (g.R(10)) {
        case 0: c.a[3] = L; break;                          // incluye el nulo final: una cadena vacia mas
        case 1: c.a[3] = 0; break;
        case 2: c.a[3] = g.R(L); break;                     // recorta
        default: c.a[3] = L - 1; break;                     // lo que devuelve la API real
        }
        c.a[5] = g.Pk({0u, 1u, 0x8001u, g.W()});
        c.a[6] = kind == 2 ? win - L + g.R(0x30) : (kind == 1 ? 4 * g.R((win - 200) / 4) + 1 + g.R(3) : 4 * g.R((win - 200) / 4));
        c.a[7] = Offset(g, kind, kSpanResult);
        c.a[8] = g.Pk({0u, 0u, 0u, 0x100u, 0x80000000u, 0xFFFFFF00u});
        c.a[9] = g.Pk({1u, 1u, 2u, 0x100u, 0xFFFFFFFFu, 0x80000000u});
        std::vector<u8> t(48);
        for (u32 i = 0; i < 16; ++i) {
            t[i] = u8(g.P(35) ? 5u : g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 6u, 7u, 255u}));
            t[16 + i] = g.P(80) ? 1u : 0u;
            t[32 + i] = u8(g.P(45) ? g.R(2) : g.R(8));
        }
        c.t = t;
        break;
    }
    default: {  // GetNearestDistanceOfPedSphereToCameraNearClip: a[0] = peaton (arena A), a[3] = indice de modelo, a[4] = informacion del modelo (arena B), a[5] = CColModel (C), a[6] = CColData (C),
                // a[7] = esferas (C), a[8] = camara activa, a[9] = plano cercano previo (bits), a[10] = plano cercano tras la animacion (bits);
                // t = 28 bytes (front x, y, z; source x, y, z; sin usar), s = 192 bytes (12 x centro y radio) previos, u = 192 bytes tras la animacion
        c.a[0] = Offset(g, kind, kSpanPed);
        c.a[3] = g.Pk({0u, 1u, 2u, 3u, 1023u, 0xFFFFu, 0xFFFEu, g.R(1024), g.R(1024), g.R(1024)});
        c.a[4] = Offset(g, kind == 2 ? 0u : kind, kSpanModelInfo);
        c.a[5] = 0x10 + 4 * g.R(0x30 / 4);                   // [0x10, 0x40): el CColModel (+0x2C)
        c.a[6] = 0x80 + 4 * g.R(0x10);                       // [0x80, 0xC0): el CColData (+8)
        c.a[7] = kind == 1 ? 0x100 + 4 * g.R(0x300) + 1 + g.R(3) : 0x100 + 4 * g.R(0x300);
        c.a[8] = g.R(3);
        // modo de los valores: 0 escena, 1 rejilla (empates), 2 enorme, 3 especial, 4 cuantizado, 5 lejano (el minimo inicial 999999.0f)
        const u32 mode = g.Pk({0u, 0u, 0u, 1u, 1u, 4u, 4u, 2u, 3u, 5u, 5u, 0u, 2u});
        const u32 sub = g.Pk({0u, 0u, 1u, 4u, 2u, 3u});
        const float scale = float(g.Pk({10u, 100u, 1000u, 100000u}));
        std::vector<u8> tv, sv, uv;
        if (mode == 5) {   // front = (1, 0, 0), source = 0: el valor de una esfera es su centro x menos el radio y el plano cercano
            for (u32 v : {FBits(1.0f), 0u, 0u, 0u, 0u, 0u, 0u}) PutF(tv, v);
        } else {
            for (u32 k = 0; k < 6; ++k) PutF(tv, SceneBits(g, mode == 3 ? (g.P(30) ? 3u : sub) : mode, scale));
            PutF(tv, 0);
        }
        for (u32 i = 0; i < 12; ++i) {
            for (int k = 0; k < 4; ++k) PutF(sv, SceneBits(g, mode == 5 ? 0u : mode, scale));
            if (mode == 5) {
                const float cx = float(g.Pk({999997u, 999998u, 999999u, 1000000u, 1000001u, 1000002u, 1000003u}));
                PutF(uv, FBits(cx));
                PutF(uv, SceneBits(g, 1u, scale)); PutF(uv, SceneBits(g, 1u, scale));
                PutF(uv, FBits(float(g.Pk({0u, 1u, 2u}))));
            } else {
                for (int k = 0; k < 4; ++k) PutF(uv, (mode == 3 && g.P(70)) ? SceneBits(g, sub, scale) : SceneBits(g, mode, scale));
            }
        }
        c.t = tv; c.s = sv; c.u = uv;
        c.a[9] = mode == 5 ? 0u : SceneBits(g, g.Pk({0u, 1u, 4u}), 10.f);
        c.a[10] = mode == 5 ? g.Pk({0u, 0x3F800000u, 0x3F000000u}) : (mode == 3 && g.P(50) ? FloatBits(g) : SceneBits(g, g.Pk({0u, 1u, 4u}), float(g.Pk({1u, 10u, 100u}))));
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
    j.H("s", c.s).H("t", c.t).H("u", c.u);
    return j.End();
}
}
