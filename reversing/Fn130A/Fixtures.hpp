#pragma once
// Fixtures del banco Fn130A (lote 130A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado se atribuian por cercania: las reales salen del desensamblado, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                 forma
//   0   0x63A040   CPointRoute::CopyFrom                        thiscall(src) `ret 4`: copia contador y solo las entradas usadas (bucle con signo, relee el contador del destino)
//   1   0x4DFB60   CAEBankLoader::ResetRequests                 thiscall() `ret`: vacia 50 solicitudes (banco/slot/sonido -1, SlotInfo/estado 0) y tres contadores
//   2   0x5B3680   LoadingScreenLoadingFile                     cdecl(path) `ret`: strrchr + sprintf(gString, "Loading %s") + LoadingScreen(...)
//   3   0x536E90   CompressVectorExe                            cdecl(int16* out, float* src) `ret`: tres veces _ftol2(v * 128.0f), word bajo
//   4   0x583AF0   CRadar::HasThisBlipBeenRevealed              cdecl(idx) `ret`: AL; globales + colaborador 0x572130
//   5   0x405A60   CIplDefPool::ScalarDeletingDestructor       thiscall(flags) `ret 4`: libera el almacenamiento (CRT del EXE 0x8213AE / 0x8214BD)
//   6   0x632E00   CTaskSequences::GetAvailableSlot             cdecl(byte) `ret`: primer slot libre en 0..31 o 32..63, o -1
//   7   0x49CD50   CTagManager::ResetAlpha                     cdecl(entity) `ret`: busca el tag de la entidad y llama a SetUserValue (0x732380)
//   8   0x40FAF0   CBoundingBox::Constructor                   thiscall() `ret`: (1,1,1) / (-1,-1,-1), devuelve this
//   9   0x532770   CDummy::ScalarDeletingDestructor            thiscall(flags) `ret 4`: cuerpo 0x532560 + borrado en linea del pool de dummies (0xB744A0)
//  10   0x6912C0   CTaskSimpleAffectSecondaryBehaviour::Destructor  thiscall() `ret`, marco SEH: vtable 0x870B08, borra la tarea guardada (+0x10) por la ranura 0 de su vtable, CTask::Destructor (0x61A3A0)
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

namespace F130AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 11;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Extremo de lo que cada funcion toca a partir de su puntero (bytes), para colocar `this` al borde de la ventana
constexpr u32 kSpanResetRequests = 0x66A;   // ultimo word escrito: this + 0x668 (+2)
constexpr u32 kSpanBBox = 0x18;
constexpr u32 kSpanPool = 0x14;
constexpr u32 kSpanDummy = 4;               // solo el dword de la vtable (el falso del cuerpo)
constexpr u32 kSpanEntity = 0x1C;           // la funcion lee el dword +0x18
constexpr u32 kSpanTaskAffect = 0x14;       // vtable (+0) y tarea guardada (+0x10)

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
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    const u32 win = u32(kWin);
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (c.fn) {
    case 0: {   // CopyFrom: a[3] = desplazamiento del origen, a[4] = 1 si el origen esta en la arena A (misma que el destino), a[5] = contador del origen
        const u32 cnt = (kind == 2 ? g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 12u, 0xFFFFFFFFu, 0xFFFFFFFEu}) :
                         g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 8u, 9u, 10u, 11u, 12u, 13u, 16u, 20u, 24u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFBu, 0xFFFFFF9Cu, 0x80000000u, 0x80000001u}));
        c.a[5] = cnt;
        const u32 span = int(cnt) > 0 ? 4 + 12 * cnt : 4;
        if (kind == 2) {
            c.a[4] = 1;
            c.a[0] = 0x80 + g.R(win - 0x180 - span);
            const int d = g.P(50) ? int(g.Pk({0u, 4u, 8u, 12u, 16u, 0xFFFFFFFCu, 0xFFFFFFF8u, 0xFFFFFFF4u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFDu, 36u, 0xFFFFFFDCu})) : int(g.R(0xC1)) - 0x60;
            c.a[3] = u32(int(c.a[0]) + d);
        } else {
            c.a[4] = 0;
            c.a[0] = Offset(g, kind == 3 ? 2u : kind, span);
            c.a[3] = Offset(g, kind == 3 ? 2u : kind, span);
        }
        break;
    }
    case 1:     // ResetRequests
        c.a[0] = Offset(g, kind, kSpanResetRequests);
        break;
    case 2: {   // LoadingScreenLoadingFile: a[3] = relleno de gString (0 aleatorio, 1 ceros, 2 unos, 3 0x5A), a[4] = longitud
        c.a[3] = g.R(4);
        u32 len = g.Pk({0u, 1u, 2u, 5u, 12u, 20u, 31u, 40u, 63u, 64u, 100u, 200u, 300u, 330u});
        if (g.P(60)) len = g.R(48);
        std::vector<u8> s;
        const u32 shape = g.R(8);   // 0 ruta con directorios, 1 sin barras, 2 solo barras, 3 acaba en barra, 4 barras directas, 5 aleatoria, 6 con '%', 7 con bytes altos
        static constexpr char alpha[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_. -";
        for (u32 i = 0; i < len; ++i) {
            u8 b = u8(alpha[g.R(u32(sizeof(alpha) - 1))]);
            switch (shape) {
            case 0: if (g.R(8) == 0) b = '\\'; break;
            case 1: break;
            case 2: b = '\\'; break;
            case 3: if (g.R(8) == 0) b = '\\'; break;
            case 4: if (g.R(5) == 0) b = '/'; if (g.R(9) == 0) b = '\\'; break;
            case 5: b = u8(1 + g.R(255)); break;
            case 6: if (g.R(4) == 0) b = '%'; if (g.R(10) == 0) b = '\\'; break;
            default: if (g.R(3) == 0) b = u8(0x80 + g.R(128)); if (g.R(12) == 0) b = '\\'; break;
            }
            if (b == 0) b = 'x';
            s.push_back(b);
        }
        if (shape == 3 && !s.empty()) s.back() = '\\';
        c.s = s;
        c.a[4] = u32(s.size());
        c.a[0] = kind == 2 ? win - u32(s.size()) - 1 + g.R(0x30) : (kind == 1 ? 4 * g.R((win - 400) / 4) + 1 + g.R(3) : 4 * g.R((win - 400) / 4));
        break;
    }
    case 3: {   // CompressVector: a[3] = desplazamiento del origen, a[4] = 1 si origen y salida comparten la arena A, a[5..7] = bits de x, y, z
        c.a[5] = FloatBits(g); c.a[6] = FloatBits(g); c.a[7] = FloatBits(g);
        if (kind == 2) {
            c.a[4] = 1;
            c.a[3] = 0x40 + g.R(win - 0x100);
            c.a[0] = u32(int(c.a[3]) + int(g.R(22)) - 8);
        } else {
            c.a[4] = 0;
            c.a[0] = Offset(g, kind, 6);
            c.a[3] = Offset(g, kind, 12);
        }
        break;
    }
    case 4: {   // HasThisBlipBeenRevealed: a[0] = indice, a[3] = byte de "dibujando el mapa", a[4] = byte de banderas del blip, a[5] = ZonesRevealed, a[6], a[7] = bits de x e y, a[8] = EAX del colaborador
        c.a[0] = g.Pk({0u, 1u, 2u, 7u, 100u, 173u, 174u, g.R(175), g.R(175), g.R(175)});
        const u32 revealedLow = g.R(80);
        const u32 revealedAny = g.Pk({0u, 1u, 79u, 80u, 81u, 255u, 256u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFB0u, g.W(), revealedLow});
        const u32 drawNonZero = g.Pk({1u, 2u, 0x80u, 0xFFu, 0x7Fu, 1u, 1u, 3u});
        const u32 drawAny = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu, 1u});
        const u32 flagsBit = (g.W() & 0xFBu) | 4u;
        const u32 flagsAny = g.W() & 0xFFu;
        const auto pos = [&]() { const float v = (float(g.W()) / 4294967296.0f - 0.5f) * float(g.Pk({100u, 6000u, 7000u, 600u, 20000u})); u32 b; std::memcpy(&b, &v, 4); return b; };
        switch (kind) {
        case 0: c.a[3] = drawNonZero; c.a[4] = flagsBit; c.a[5] = revealedLow; c.a[6] = pos(); c.a[7] = pos(); break;
        case 1: c.a[3] = g.P(30) ? 0u : drawNonZero; c.a[4] = g.P(30) ? (flagsAny & ~4u) : flagsBit; c.a[5] = g.P(50) ? revealedLow : revealedAny; c.a[6] = pos(); c.a[7] = pos(); break;
        case 2: c.a[3] = drawNonZero; c.a[4] = flagsBit; c.a[5] = revealedLow; c.a[6] = FloatBits(g); c.a[7] = FloatBits(g); break;
        default: c.a[3] = drawAny; c.a[4] = flagsAny; c.a[5] = revealedAny; c.a[6] = g.W(); c.a[7] = g.W(); break;
        }
        c.a[8] = g.Pk({0u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, g.W(), g.W(), 0x00BADC00u + g.R(256), 1u});
        break;
    }
    case 5: {   // CIplDefPool::ScalarDeletingDestructor: a[0] = this, a[3] = +0, a[4] = +4, a[5] = tamano, a[6] = +0xC, a[7] = banderas, a[8] = byte propietario, a[9] = bytes +0x11..0x13
        c.a[0] = Offset(g, kind, kSpanPool);
        // punteros crudos fuera de las arenas del banco (imagen del EXE o valores pequenos): los falsos no los desreferencian
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x00400000u + g.R(0x800000), 0x00400000u + g.R(0x800000), g.R(0x400000)});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x00400000u + g.R(0x800000), 0x00400000u + g.R(0x800000), g.R(0x400000)});
        c.a[5] = g.Pk({0u, 1u, 2u, 5u, 100u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu, g.W(), 1u, 3u});
        c.a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 7u, 0x80000000u});
        switch (g.R(10)) {
        case 0: c.a[7] = 0; break;
        case 1: c.a[7] = 1; break;
        case 2: c.a[7] = g.Pk({2u, 4u, 0x80u, 0xFEu, 0x100u, 0x10000u, 0x80000000u, 0xFFFFFF00u, 0xFFFFFE00u}); break;
        case 3: c.a[7] = g.Pk({3u, 5u, 0x81u, 0xFFu, 0x101u, 0x10001u, 0x80000001u, 0xFFFFFF01u, 0xFFFFFF03u, 0x12345679u}); break;
        case 4: c.a[7] = g.W() & ~1u; break;
        case 5: c.a[7] = g.W() | 1u; break;
        case 6: c.a[7] = 0xFFFFFFFFu; break;
        default: c.a[7] = g.W(); break;
        }
        c.a[8] = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu, 1u, 1u});
        c.a[9] = g.W() & 0xFFFFFFu;
        break;
    }
    case 6: {   // GetAvailableSlot: a[0] = argumento de pila (el EXE lee solo el byte bajo), a[3] = densidad de slots libres (%), a[4], a[5] = mascara de 64 bits de "primera tarea no nula"; s = 64 bytes de "abierto"
        c.a[0] = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0xFFFFFF00u, 0x12345600u, 0xFFFFFFFFu, g.W(), g.W() & ~0xFFu, 1u, 0u});
        c.a[3] = g.Pk({0u, 1u, 3u, 10u, 25u, 50u, 90u, 100u, 5u, 15u, 70u});
        std::vector<u8> s(64);
        u64 mask = 0;
        for (u32 i = 0; i < 64; ++i) {
            const bool freeOpened = g.R(100) < c.a[3];
            const bool freeTask = g.R(100) < (c.a[3] < 100 ? 70u : 100u);
            s[i] = freeOpened ? 0 : u8(g.Pk({1u, 1u, 2u, 0x80u, 0xFFu, 1u + g.R(255)}));
            if (!freeTask) mask |= u64(1) << i;
        }
        c.s = s;
        c.a[4] = u32(mask); c.a[5] = u32(mask >> 32);
        break;
    }
    case 7: {   // ResetAlpha: a[0] = entidad, a[3] = puntero RenderWare (+0x18), a[4] = numero de tags, a[5..9] = mascara (160 bits) de "la entrada es la entidad"; s = 150 alfas
        c.a[0] = Offset(g, kind, kSpanEntity);
        c.a[3] = g.P(15) ? 0u : g.Pk({0x10000000u + 0x1040u, g.W() | 1u, g.W() | 1u, 0xFFFFFFFFu, 4u});
        const u32 nt = g.Pk({1u, 2u, 3u, 5u, 10u, 50u, 100u, 149u, 150u, 1u + g.R(150), 1u + g.R(150), 1u + g.R(150)});
        c.a[4] = nt;
        std::array<u32, 5> mask{};
        auto setBit = [&](u32 i) { mask[i / 32] |= 1u << (i % 32); };
        // posicion de la coincidencia: la ultima, la primera, una intermedia, varias (duplicadas); siempre una dentro de [0, nt)
        switch (g.R(4)) {
        case 0: setBit(nt - 1); break;
        case 1: setBit(0); break;
        case 2: setBit(g.R(nt)); break;
        default: setBit(g.R(nt)); setBit(g.R(nt)); setBit(g.R(nt)); break;
        }
        // entradas del tag fuera de [0, nt) que tambien apuntan a la entidad (no se miran)
        if (nt < 150 && g.P(40)) setBit(nt + g.R(150 - nt));
        for (u32 i = 0; i < 5; ++i) c.a[5 + i] = mask[i];
        std::vector<u8> s(150);
        for (auto& b : s) b = u8(g.W());
        if (g.P(20)) for (auto& b : s) b = u8(g.Pk({0u, 0xFFu, 0xE4u, 0xE5u}));
        c.s = s;
        break;
    }
    case 8:     // CBoundingBox::Constructor
        c.a[0] = Offset(g, kind, kSpanBBox);
        break;
    case 9: {   // CDummy::ScalarDeletingDestructor: a[0] = this (arena A), a[3] = this - almacenamiento (con signo), a[4] = desplazamiento del vector de estados en C, a[5] = +0xC del pool, a[6] = vtable previa,
                // a[7] = banderas, a[8] = dword que deja el falso del cuerpo, a[9] = EAX del falso, a[10] = desplazamiento del pool en B, a[11] = indice esperado
        c.a[0] = Offset(g, kind, kSpanDummy);
        const int idx = g.P(50) ? int(g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFDu, 7u, 0xFFFFFFF9u, 100u, 0xFFFFFF9Cu})) : int(g.R(3601)) - 1800;
        const int rem = int(g.R(56));
        const int diff = idx > 0 ? idx * 56 + rem : (idx < 0 ? idx * 56 - rem : (g.P(50) ? rem : -rem));
        c.a[3] = u32(diff);
        const int lo = std::max(0, -idx), hi = std::min(int(win) - 1, int(win) - 1 - idx);
        c.a[4] = u32(lo + int(g.R(u32(hi - lo + 1))));
        c.a[11] = u32(idx);
        c.a[5] = g.Pk({u32(idx - 1), u32(idx), u32(idx + 1), 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), u32(idx + 2), u32(idx - 2), u32(idx)});
        c.a[6] = g.Pk({0u, 0xFFFFFFFFu, 0x8638C0u, 0xDEADBEEFu, g.W(), g.W(), 0x859CFCu});
        switch (g.R(10)) {
        case 0: c.a[7] = 0; break;
        case 1: c.a[7] = 1; break;
        case 2: c.a[7] = g.Pk({2u, 4u, 0x80u, 0xFEu, 0x100u, 0x10000u, 0x80000000u, 0xFFFFFF00u, 0xFFFFFE00u}); break;
        case 3: c.a[7] = g.Pk({3u, 5u, 0x81u, 0xFFu, 0x101u, 0x10001u, 0x80000001u, 0xFFFFFF01u, 0xFFFFFF03u, 0x12345679u}); break;
        case 4: c.a[7] = g.W() & ~1u; break;
        case 5: c.a[7] = g.W() | 1u; break;
        case 6: c.a[7] = 0xFFFFFFFFu; break;
        default: c.a[7] = g.W(); break;
        }
        c.a[8] = g.Pk({0u, 0xFFFFFFFFu, 0x8638C0u, 0x863A00u, g.W(), c.a[6], c.a[6] ^ (0xFFu << (8 * g.R(4)))});
        c.a[9] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W(), 0x00BADC00u + g.R(256)});
        c.a[10] = Offset(g, g.P(50) ? 0u : 1u, kSpanPool);
        break;
    }
    default: {  // CTaskSimpleAffectSecondaryBehaviour::Destructor: a[0] = this (arena A), a[3] = vtable previa de this, a[4] = 1 si la tarea guardada (+0x10) no es nula, a[10] = desplazamiento de la tarea guardada en B,
                // a[11] = desplazamiento de su vtable (ranura 0 = falso) en C
        c.a[0] = Offset(g, kind, kSpanTaskAffect);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x870B08u, 0x86D48Cu, 0x870B09u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.P(70) ? 1u : 0u;
        c.a[10] = 4 * g.R((win - 8) / 4);
        c.a[11] = 4 * g.R((win - 64) / 4);
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
