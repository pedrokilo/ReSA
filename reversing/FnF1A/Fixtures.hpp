#pragma once
// Fixtures del banco FnF1A (lote F1A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN validar antes del lote; enganchadas en origin/main con validated=0). Diez: las seis del informe F1A mas las cuatro descompresoras DXT de D3D9RasterFormat (R10A).
//   idx  direccion  nombre                                              forma
//    0   0x617FC0   IKChainManager_c::Constructor                      thiscall() `ret`, marco SEH: 32 elementos de 0x58 B (solo +0 y +4 a cero), dos listas (+0xB00, +0xB0C) de 3 dwords a cero; EAX = this
//    1   0x618030   IKChainManager_c::Destructor                       thiscall() `ret`, marco SEH: no escribe
//    2   0x6E3EB0   WaterCreatureManager_c::Constructor                thiscall() `ret`, marco SEH: 128 elementos de 0x34 B (+0, +4, +8 a cero), listas en +0x1A00 y +0x1A0C; EAX = this
//    3   0x6E3F20   WaterCreatureManager_c::Destructor                 thiscall() `ret`, marco SEH: no escribe
//    4   0x6E67A0   CWaterLevel::IsPointInCameraDepthRange             cdecl(const CVector*, float, bool) `ret`: x87 con camara falsa en Scene.m_pRwCamera (0xC1703C); resultado en AL
//    5   0x509230   CAudioZones::BugstarWriteScreenShotEntry           thiscall(float x3, grid, title, image, imageSize) `ret 1Ch`: serializa en una cadena local; colaboradores 0x508B80, 0x508D30 y 0x509190 falsos
//    6   0x81E720   D3D9RasterFormat::DecompressDxt1                   cdecl(ancho, alto, destino, origen) `ret`: devuelve 1 si algun bloque de tres colores uso el indice 3
//    7   0x81EA70   D3D9RasterFormat::DecompressDxt3                   cdecl(ancho, alto, destino, origen) `ret` (el llamador deja un quinto dword)
//    8   0x81EDC0   D3D9RasterFormat::DecompressDxt5                   cdecl(ancho, alto, destino, origen) `ret`; llama a 0x81F0C0
//    9   0x81F0C0   D3D9RasterFormat::DecodeDxt5AlphaBlock            cdecl(destino, bloque de alfa, separacion en pixeles, mascara) `ret`
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

namespace FF1AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 10;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x2000;   // ventana de cada arena
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
    std::array<u32, 40> a{};
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
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
// Un float para la compresion (x4096 y truncado): ademas de los valores de FloatBits, multiplos de 1/4096 con restos proximos a la unidad, bordes de int16 (7, 8, 9: x4096 = 28672, 32768, 36864) y de los enteros de 32 y 64 bits
inline u32 CompressBits(Gen& g) {
    if (g.P(40)) return FloatBits(g);
    const i32 k = i32(g.R(90000)) - 45000;
    const u32 fr = g.R(6);
    const float frac = fr < 2 ? 0.0f : (fr == 2 ? 0.5f : (fr == 3 ? 0.999f : (fr == 4 ? -0.001f : -0.5f)));
    float v = (float(k) + frac) / 4096.0f;
    const u32 pick = g.R(10);
    if (pick == 0) v = 8.0f + float(g.R(3)) - 1.0f;
    u32 bits = std::bit_cast<u32>(v);
    if (pick == 1) bits = 0x5F000000u + g.R(3) - 1u;   // alrededor de 2^63 / 4096 * 2^12 (borde del entero de 64 bits tras multiplicar)
    if (pick == 2) bits = 0x4F000000u + g.R(3) - 1u;   // alrededor de 2^31 (borde del entero de 32 bits)
    if (pick == 3) bits = 0x46000000u + g.R(3) - 1u;   // 8192: x4096 = 2^25
    return bits;
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }

// Clases de funcion (forma)
enum Kind : u32 { kIk, kWc, kNop, kCam, kBug, kDxt1, kDxt3, kDxt5, kAlpha };
inline constexpr std::array<Kind, kFunctions> kKind{kIk, kNop, kWc, kNop, kCam, kBug, kDxt1, kDxt3, kDxt5, kAlpha};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x617FC0, 0x618030, 0x6E3EB0, 0x6E3F20, 0x6E67A0, 0x509230, 0x81E720, 0x81EA70, 0x81EDC0, 0x81F0C0};
// Bytes que cada funcion toca a partir de su puntero `this` (kIk, kWc, kNop) o que reserva el objeto de la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0xB18, 0xB18, 0x1A1C, 0x1A1C, 4, 4, 4, 4, 4, 4};
// Memoria global del juego tocada: Scene.m_pRwCamera
inline constexpr u32 kScenePtr = 0xC1703C;
inline constexpr u32 kDxtBlockBytes[3] = {8, 16, 16};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   kIk, kWc, kNop: nada mas (los datos son el relleno de la arena)
//   kCam: a[4..6] = punto (bits float), a[7] = radio, a[8] = near, a[9] = far, a[10] = bandera (dword; la funcion solo lee el byte bajo), a[11..26] = los 16 dwords de la matriz de vista (RwMatrix, +0x20 de la camara),
//         a[27] = desplazamiento del punto en C, a[28] = desplazamiento de la camara en B, a[29] = modo (0 bits aleatorios, 1 estructurada, 2 identidad con empates, 3 estructurada con valores especiales)
//   kBug: a[4..6] = x, y, z (bits), a[7] = desplazamiento de `grid` en C, a[8] = desplazamiento de `title` en C, a[9] = desplazamiento de la imagen en D, a[10] = tamano de la imagen, a[11] = longitud de title,
//         a[12] = longitud de grid, a[13] = semilla de las cadenas (el byte i es 1 + Rand(semilla + k, i) % 255; k = 0 title, 1 grid)
//   kDxt1/3/5: a[4] = ancho, a[5] = alto (int32), a[6] = desplazamiento del destino en B, a[7] = desplazamiento del origen en C, a[8] = mascara de retoques (bit j % 32: en el bloque j los dos extremos de color pasan
//         a ser iguales y los dos extremos de alfa tambien)
//   kAlpha: a[4] = separacion en pixeles (int32), a[5] = mascara que conserva, a[6] = desplazamiento del destino en B, a[7] = desplazamiento del bloque de alfa en C, a[8] = retoque (bit 0: a1 = a0)
inline u32 CoordBits(Gen& g) {
    const u32 pick = g.R(10);
    const u32 sign = g.P(40) ? 0x80000000u : 0u;
    if (pick <= 3) { const float v = (float(g.R(60001)) - 30000.0f) / 16.0f + float(g.R(1000)) / 1000.0f; return std::bit_cast<u32>(v); }
    if (pick == 4) return std::bit_cast<u32>(float(g.R(2000000)) / 4.0f - 250000.0f);
    if (pick == 5) return sign | (0x4B000000u + g.R(0x500000u));                                   // 8388608 .. ~2e7 enteros grandes
    if (pick == 6) return sign | ((120u + g.R(10)) << 23) | (g.W() & 0x7FFFFFu);                    // 1e10 .. 1e19 aprox
    if (pick == 7) return sign | ((140u + g.R(14)) << 23) | (g.W() & 0x7FFFFFu);                    // hasta 3.4e38
    if (pick == 8) return sign | ((90u + g.R(36)) << 23) | (g.W() & 0x7FFFFFu);                     // pequenos
    return FloatBits(g);
}
inline u32 StructF(Gen& g, float range) { return std::bit_cast<u32>((float(g.R(2000001)) - 1000000.0f) / 1000000.0f * range); }
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
    case kIk: case kWc: case kNop:
        break;
    case kCam: {
        a[27] = AuxOffset(g, 0x10); a[28] = AuxOffset(g, 0x90);
        const u32 mode = g.R(4);
        a[29] = mode;
        for (u32 i = 0; i < 16; ++i) a[11 + i] = g.W();                  // relleno (las palabras de banderas y relleno de RwMatrix no cuentan)
        if (mode == 0) {
            for (u32 i = 0; i < 3; ++i) a[4 + i] = FloatBits(g);
            a[7] = FloatBits(g); a[8] = FloatBits(g); a[9] = FloatBits(g);
            for (u32 i : {0u, 1u, 2u, 4u, 5u, 6u, 8u, 9u, 10u, 12u, 13u, 14u}) a[11 + i] = FloatBits(g);
        } else if (mode == 2) {
            const float one = 1.0f;
            const u32 ob = std::bit_cast<u32>(one);
            for (u32 i : {0u, 1u, 2u, 4u, 5u, 6u, 8u, 9u, 10u, 12u, 13u, 14u}) a[11 + i] = 0u;
            a[11 + 0] = ob; a[11 + 5] = ob; a[11 + 10] = ob;
            if (g.P(15)) a[11 + 10] = FloatBits(g);
            const float px = float(int(g.R(2001)) - 1000) * 0.25f, py = float(int(g.R(2001)) - 1000) * 0.25f, pz = float(int(g.R(40001)) - 20000) * 0.0625f;
            const float cz = float(int(g.R(40001)) - 20000) * 0.03125f;
            a[11 + 14] = std::bit_cast<u32>(cz);
            a[4] = std::bit_cast<u32>(px); a[5] = std::bit_cast<u32>(py); a[6] = std::bit_cast<u32>(pz);
            const float z = pz + cz;
            const u32 zb = std::bit_cast<u32>(z);
            const u32 nb = g.Pk({zb, zb + 1u, zb - 1u, zb, std::bit_cast<u32>(z - 5.0f), std::bit_cast<u32>(z + 5.0f)});
            const u32 fb = g.Pk({zb, zb + 1u, zb - 1u, zb, std::bit_cast<u32>(z - 5.0f), std::bit_cast<u32>(z + 5.0f)});
            a[7] = g.Pk({0u, 0u, 0u, 0x80000000u, std::bit_cast<u32>(1.0f), std::bit_cast<u32>(-2.0f)});
            a[8] = nb; a[9] = fb;
            if (g.P(8)) a[8] = FloatBits(g);
            if (g.P(8)) a[9] = FloatBits(g);
        } else {
            for (u32 i : {0u, 1u, 2u, 4u, 5u, 6u, 8u, 9u, 10u}) a[11 + i] = StructF(g, 2.0f);
            for (u32 i : {12u, 13u, 14u}) a[11 + i] = StructF(g, 1000.0f);
            for (u32 i = 0; i < 3; ++i) a[4 + i] = StructF(g, 1000.0f);
            auto F = [&](u32 i) { return double(std::bit_cast<float>(a[11 + i])); };
            const double z = ((F(10) * double(std::bit_cast<float>(a[6])) + F(2) * double(std::bit_cast<float>(a[4]))) + F(6) * double(std::bit_cast<float>(a[5]))) + F(14);
            a[7] = g.Pk({0u, 0u, std::bit_cast<u32>(0.5f), std::bit_cast<u32>(-3.0f), std::bit_cast<u32>(100.0f), std::bit_cast<u32>(1.0f)});
            const double r = double(std::bit_cast<float>(a[7]));
            const u32 sc = g.R(7);
            const double dn = sc == 0 ? 5.0 : (sc == 1 ? -5.0 : (sc == 2 ? 0.0 : (sc == 3 ? 1e-3 : (sc == 4 ? -1e-3 : 200.0))));
            a[8] = std::bit_cast<u32>(float(z + r + dn));
            const u32 sf = g.R(6);
            a[9] = std::bit_cast<u32>(float(z + (sf == 0 ? 5.0 : (sf == 1 ? -5.0 : (sf == 2 ? 0.0 : (sf == 3 ? 1e-3 : (sf == 4 ? -1e-3 : 500.0)))))));
            if (mode == 3) {
                const u32 k = 1 + g.R(3);
                for (u32 i = 0; i < k; ++i) a[g.Pk({4u, 5u, 6u, 7u, 8u, 9u, 11u, 15u, 19u, 22u, 23u, 24u, 25u})] = FloatBits(g);
            }
        }
        a[10] = g.Pk({0u, 1u, 1u, 0x100u, 0x80u, 0xFFu, g.W(), 0xFFFFFF00u, 2u});
        break;
    }
    case kBug:
        for (u32 i = 0; i < 3; ++i) a[4 + i] = CoordBits(g);
        a[7] = 0x800 + 4 * g.R(0x100); a[8] = 4 * g.R(0x100);
        a[10] = g.Pk({0u, 0u, 1u, 3u, 4u, 5u, g.R(8), g.R(64), g.R(201), 200u});
        a[9] = AuxOffset(g, 0x100);
        a[11] = g.Pk({0u, 1u, 2u, 5u, 12u, g.R(61), g.R(61), 60u});
        a[12] = g.Pk({0u, 1u, 3u, 8u, 12u, g.R(61), g.R(61), 60u});
        a[13] = g.W();
        break;
    case kDxt1: case kDxt3: case kDxt5: {
        const i32 W[] = {4, 4, 8, 8, 12, 16, 16, 16, 20, 24, 28, 32, 32, 32, 5, 6, 7, 9, 10, 13, 17, 31, 1, 2, 3, 0, -1, -4, -8, -5};
        const i32 H[] = {4, 4, 8, 8, 12, 16, 16, 20, 24, 28, 32, 32, 5, 6, 7, 9, 10, 13, 17, 31, 1, 2, 3, 0, -1, -4, -8, 36};
        i32 w = W[g.R(30)], h = H[g.R(28)];
        if (g.P(75)) { w = 4 * i32(1 + g.R(8)); h = 4 * i32(1 + g.R(8)); }
        const i32 bpr = w / 4, bh = h / 4;
        u32 span = 4;
        if (bpr > 0 && bh > 0) span = u32(((4 * bh - 1) * w + 4 * bpr) * 4);
        a[4] = u32(w); a[5] = u32(h);
        a[6] = Offset(g, kind, span);
        const u32 srcSpan = (bpr > 0 && bh > 0 ? u32(bpr * bh) : 1u) * kDxtBlockBytes[kKind[c.fn] - kDxt1] + 16;
        a[7] = AuxOffset(g, srcSpan);
        a[8] = g.Pk({0u, 0u, 0xFFFFFFFFu, g.W(), g.W(), 0xAAAAAAAAu});
        break;
    }
    case kAlpha: {
        const i32 S[] = {4, 4, 4, 8, 16, 32, 5, 7, 1, 0, 2, 3, -4, -8, -16, -3, 64, 100, -100};
        const i32 s = S[g.R(19)];
        a[4] = u32(s);
        a[5] = g.Pk({0x00FFFFFFu, 0x00FFFFFFu, 0u, 0xFFFFFFFFu, g.W(), 0xFF000000u, 0x0000FF00u});
        const i32 lo = 4 * (s < 0 ? 3 * s : 0), hi = 4 * ((s > 0 ? 3 * s : 0) + 4);
        const u32 span = u32(hi - lo);
        a[6] = u32(i32(Offset(g, kind, span)) - lo);
        a[7] = AuxOffset(g, 0x10);
        a[8] = g.Pk({0u, 1u, 0u, 1u, 2u});
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
