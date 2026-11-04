#pragma once
// Fixtures del banco Fn89D (lote 89D): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CFont::PrintSegment                    0x719B40  cdecl, 5 argumentos en pila (left, top, begin, end, spacing; los llama el llamador: 0x14 bytes); void   (hook CFont / PrintSegment, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia).
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

namespace F89DFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

// Memoria real del juego que toca la funcion: [0xC716A8, 0xC71AD0) = puntero pEmptyChar (+0), hueco y buffer (0xC716B0..), estaticos de CFont (0xC71A54..0xC71AD0); y los enteros 0xC17044 / 0xC17048.
constexpr std::uintptr_t kImgBase = 0xC716A8, kImgEnd = 0xC71AD0, kStatBase = 0xC71A54, kBufStart = 0xC716B0, kLimit = 0xC7187C;
constexpr std::size_t kImgSize = kImgEnd - kImgBase, kStatSize = kImgEnd - kStatBase, kStatWords = kStatSize / 4;
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kTextWin = 0x140;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr u8 kFill = 0xA5;               // contenido inicial del buffer

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 89004u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

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
    Js& S(const char* k, const std::string& v) { Key(k); s += '"'; s += v; s += '"'; return *this; }
    std::string End() const { return s + "}"; }
};

inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}

// Desplazamientos (respecto a kStatBase = 0xC71A54) de los campos de CFont que lee la funcion
namespace Off {
constexpr std::size_t PS2Symbol = 0x00, Color = 0x0C, ScaleX = 0x10, ScaleY = 0x14, Slant = 0x18, RefX = 0x1C, RefY = 0x20, Prop = 0x29, Blip = 0x2A, Texture = 0x40, Style = 0x41, Shadow = 0x42,
                      Drop = 0x43, OutlineSize = 0x47, Outline = 0x48, Images = 0x74, StateTex = 0x78;   // 0xC71A54 + 0x74 = 0xC71AC8 (RenderState.m_bContainImages), + 0x78 = 0xC71ACC (m_wFontTexture)
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 left{}, top{}, spacing{};
    u32 pe{};                                       // pEmptyChar inicial (direccion absoluta)
    u32 maxw{}, maxh{};                             // enteros 0xC17044 / 0xC17048
    u32 stat[kStatWords]{};                         // imagen inicial de 0xC71A54..0xC71AD0
    u32 boff{}, len{};                              // desplazamiento de `begin` en la arena de texto; end = begin + len (con signo)
    std::string text;                               // contenido de la arena de texto (hex), desde el inicio de la arena
    u32 tadv[4]{}, tmode[4]{}, tcolor[4]{}, tsym[4]{};   // ParseToken falso (llamada 0..3): avance, 1 = escribe el color, color, simbolo (0xFF = no tocar)
    u32 mutSite{}, mutVal{};
    u32 gate{};
};

inline void PutB(C& c, std::size_t off, u32 v) { reinterpret_cast<u8*>(c.stat)[off] = u8(v); }
inline void PutW(C& c, std::size_t off, u32 v) { const u16 w = u16(v); std::memcpy(reinterpret_cast<u8*>(c.stat) + off, &w, 2); }
inline void PutD(C& c, std::size_t off, u32 v) { std::memcpy(reinterpret_cast<u8*>(c.stat) + off, &v, 4); }
inline u32 GetB(const C& c, std::size_t off) { return reinterpret_cast<const u8*>(c.stat)[off]; }

inline std::string Hex(const std::vector<u8>& v) {
    constexpr char d[] = "0123456789abcdef";
    std::string o; o.reserve(v.size() * 2);
    for (u8 b : v) { o += d[b >> 4]; o += d[b & 15]; }
    return o;
}

// ------------------------------------------------------------------------------------------------ generador
inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Pk = [&](std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); };
    auto Fl = [&](float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); };
    // --- estaticos de CFont: relleno aleatorio y campos concretos
    FillWords(reinterpret_cast<u8*>(c.stat), kStatSize, c.seed * 7u + 3u);
    const bool special = f == 11;
    PutB(c, Off::PS2Symbol, P(75) ? 0u : 1 + R(15));
    PutD(c, Off::Color, Rand(n, nk()));
    PutD(c, Off::Color + 0x04, B(special && P(25) ? F(Special(n, nk())) : Fl(0.1f, 3.0f)));    // escala x (0xC71A64)
    PutD(c, Off::Color + 0x08, B(Fl(0.1f, 3.0f)));                                              // escala y (0xC71A68)
    PutD(c, Off::Slant, 0u);
    PutD(c, Off::RefX, B(special && P(25) ? F(Special(n, nk())) : Fl(-200.0f, 700.0f)));
    PutD(c, Off::RefY, B(special && P(25) ? F(Special(n, nk())) : Fl(-200.0f, 500.0f)));
    PutB(c, Off::Prop, R(2)); PutB(c, Off::Blip, f == 14 ? 1u : (P(15) ? 1u : 0u));
    const u32 tex = R(7);
    PutB(c, Off::Texture, tex); PutB(c, Off::Style, R(4)); PutD(c, Off::Drop, Rand(n, nk()));
    PutB(c, Off::Shadow, 0); PutB(c, Off::OutlineSize, 0); PutB(c, Off::Outline, R(2));
    PutB(c, Off::Images, R(2));
    PutW(c, Off::StateTex, (f == 8 || P(25)) ? (tex + 1 + R(6)) & 0xFFFFu : (P(10) ? (Rand(n, nk()) & 0xFF00u) | tex : tex));
    const bool slant = f == 3 || f == 5 || (f != 2 && f != 4 && f != 1 && P(40));
    if (slant) PutD(c, Off::Slant, B(P(70) ? Fl(-1.0f, 1.0f) : F(Pk({0x80000000u, 1u, 0x7F800000u, 0xFFC00000u, 0xBF800000u, 0x3F800000u}))));
    else if (P(20) && f != 3 && f != 5) PutD(c, Off::Slant, Pk({0u, 0x80000000u}));
    // sombra y contorno (byte con signo)
    auto Eff = [&] { return Pk({1u, 2u, 3u, 4u, 5u, 0xFFu, 0xFEu, 0x7Fu, 0x80u, 0x81u, 6u, 8u, 0xF0u, R(256)}); };
    switch (f) {
    case 2: case 3: PutB(c, Off::Shadow, Eff()); break;
    case 4: case 5: PutB(c, Off::OutlineSize, Eff()); break;
    case 6: PutB(c, Off::Shadow, Eff()); PutB(c, Off::OutlineSize, Eff()); break;
    case 1: break;
    case 10: PutB(c, Off::Shadow, P(50) ? Eff() : 0u); PutB(c, Off::OutlineSize, P(60) ? Eff() : 0u); break;
    default:
        if (P(35)) PutB(c, Off::Shadow, Eff());
        if (P(35)) PutB(c, Off::OutlineSize, Eff());
        break;
    }
    // enteros del tamano de pantalla
    c.maxw = P(80) ? Pk({640u, 800u, 1024u, 1280u, 1920u, 480u, 720u}) : R(5000);
    c.maxh = P(80) ? Pk({448u, 600u, 768u, 1024u, 1080u, 480u, 360u}) : R(5000);
    if (f == 10 && P(30)) { c.maxw = Pk({0u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x01000001u, 0x00FFFFFFu, 0x1000000u + R(100)}); c.maxh = Pk({0u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x01000003u, 0x00FFFFFFu, 0x1000000u + R(100)}); }
    if (f == 11 && P(40)) { c.maxw = R(0x80000000u) * 2u + R(2); c.maxh = R(0x80000000u) * 2u + R(2); }
    // --- texto y argumentos
    c.boff = R(8);
    u32 len = f == 13 ? Pk({0u, 0u, 1u, 2u}) : (f == 0 && P(5) ? 40 + R(200) : R(f == 7 || f == 14 ? 24 : 20));
    if (f == 15) len = R(36);
    const bool negative = f == 13 && P(25);
    if (f == 9 && P(30)) len = R(8);
    c.len = negative ? u32(-i32(1 + R(8))) : len;
    const u32 tl = c.boff + (negative ? 0 : len) + 8;
    std::vector<u8> text(tl);
    const u32 tilde = f == 7 ? 30 : (f == 14 ? 40 : (f == 0 ? 8 : 4));
    for (u32 i = 0; i < tl; ++i) {
        u8 ch = u8(Rand(n, 500 + i) & 0xFF);
        const u32 r = Rand(n, 800 + i) % 100;
        if (r < tilde) ch = '~';
        else if (r < 85) ch = u8('A' + Rand(n, 900 + i) % 26);
        text[i] = ch;
    }
    c.text = Hex(text);
    for (u32 k = 0; k < 4; ++k) {
        c.tadv[k] = 1 + (P(60) ? R(4) : R(8));
        c.tmode[k] = P(70) ? 1u : 0u;
        c.tcolor[k] = Rand(n, nk());
        c.tsym[k] = P(35) ? 1 + R(15) : 0xFFu;
        if (P(10)) c.tsym[k] = 0;
    }
    c.left = B(special && P(30) ? F(Special(n, nk())) : Fl(-100.0f, 700.0f));
    c.top = B(special && P(30) ? F(Special(n, nk())) : Fl(-100.0f, 500.0f));
    c.spacing = B(special && P(30) ? F(Special(n, nk())) : Fl(0.0f, 700.0f));
    if (f == 10 && P(20)) { c.left = Pk({B(1.0e30f), 0xFF7FFFFFu, 0x00800000u, 0x80000001u, B(16777216.0f)}); }
    // --- pEmptyChar inicial
    {
        const i32 slen = i32(c.len);
        const u32 limit = u32(kLimit) - u32(slen);
        if (f == 9) c.pe = limit - 12 + R(24);
        else if (f == 8 && P(60)) c.pe = u32(kBufStart) + R(0x100);
        else if (P(12)) c.pe = limit - 4 + R(8);
        else c.pe = u32(kBufStart) + R(0x1B0);
        if (c.pe < u32(kBufStart)) c.pe = u32(kBufStart) + R(0x20);
        if (c.pe >= 0xC71A00u) c.pe = u32(kBufStart) + R(0x1B0);
        if (P(5)) c.pe = u32(kBufStart);
    }
    // --- mutaciones (una escritura dentro del vaciado del buffer falso)
    if (f == 12) {
        c.mutSite = 1 + R(6);
        switch (c.mutSite) {
        case 1: case 2: c.mutVal = Rand(n, nk()); break;
        case 3: case 4: c.mutVal = Pk({0u, 1u, 2u, 3u, 0xFFu, 0x80u, R(256)}); break;
        case 5: c.mutVal = P(30) ? Pk({0u, 0x80000000u, 0x7F800000u}) : B(Fl(-1.0f, 1.0f)); break;
        default: c.mutVal = Pk({640u, 0u, 100u, R(3000)}); break;
        }
        if (P(50)) PutW(c, Off::StateTex, (tex + 1) & 0xFFFFu);   // vaciado por textura distinta (antes de leer nada)
        else { PutW(c, Off::StateTex, tex); c.pe = u32(kLimit) - u32(i32(c.len)) + R(8); }   // primer vaciado por buffer lleno, dentro de la primera llamada anidada
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    auto V = [](const u32* p, std::size_t k) { return std::vector<u32>(p, p + k); };
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("left", c.left).U("top", c.top).U("spacing", c.spacing).U("pe", c.pe).U("maxw", c.maxw).U("maxh", c.maxh).A("stat", V(c.stat, kStatWords));
    j.U("boff", c.boff).U("len", c.len).S("text", c.text);
    j.A("tadv", V(c.tadv, 4)).A("tmode", V(c.tmode, 4)).A("tcolor", V(c.tcolor, 4)).A("tsym", V(c.tsym, 4));
    j.U("mutsite", c.mutSite).U("mutval", c.mutVal).U("gate", c.gate);
    return j.End();
}
}
