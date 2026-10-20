#pragma once
// Fixtures del banco D3DResource62 (lote 62): siete funciones de D3DResourceSystem, 4096 casos cada una. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   D3DResourceSystem::CancelBuffering      0x730900  cdecl, void
//   D3DResourceSystem::CreateIndexBuffer    0x7306A0  cdecl (numIndices, format, void** pp), HRESULT en EAX
//   D3DResourceSystem::CreateTexture        0x730510  cdecl (width, height, levels, format, void** pp), HRESULT en EAX
//   D3DResourceSystem::DestroyIndexBuffer   0x730D30  cdecl (void* ib)
//   D3DResourceSystem::DestroyTexture       0x730B70  cdecl (void* texture)
//   D3DResourceSystem::Init                 0x730830  cdecl, void
//   D3DResourceSystem::Shutdown             0x730A00  cdecl, void
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): estado de las caches (bloque de globales 0xC87C60..0xC87FE8 + byte 0x8D6084),
// argumentos y comportamiento de los falsos (recursos COM, dispositivo, colaboradores del EXE, new/delete).
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace D3D62Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 7;
// indices de funcion
constexpr u32 kCancel = 0, kCreateIb = 1, kCreateTex = 2, kDestroyIb = 3, kDestroyTex = 4, kInit = 5, kShutdown = 6;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }

// ------------------------------------------------------------------------------------------------ disposicion (palabras de 32 bits)
// Bloque de globales del EXE: 0xC87C60 + 4 * palabra.
constexpr u32 kRegionBase = 0xC87C60;
constexpr u32 kRegionWords = 226;            // 0xC87C60 .. 0xC87FE8
constexpr u32 kUseAddr = 0x8D6084;           // UseD3DResourceBuffering (byte)
constexpr u32 kDeviceAddr = 0xC97C28;        // _RwD3DDevice
constexpr u32 kTexBase = 2, kTexStride = 7;  // D3DTextureBuffer: fmt, width, levels, capacity, num, size, ptr (0x1C bytes) x 17
constexpr u32 kIbBase = 122, kIbStride = 6;  // D3DIndexDataBuffer: fmt, field_4, capacity, num, size, ptr (0x18 bytes) x 16
constexpr u32 kGenBase = 218;                // buffer generico de index buffers (0xC87FC8)
constexpr u32 kFreeTexWord = 224, kFreeIbWord = 225;
constexpr u32 kTexBuffers = 17, kIbBuffers = 16;
static_assert(kRegionBase + 4 * (kTexBase + kTexStride * kTexBuffers) == 0xC87E44, "fin de TextureBuffers");
static_assert(kRegionBase + 4 * kIbBase == 0xC87E48 && kRegionBase + 4 * kGenBase == 0xC87FC8 && kRegionBase + 4 * kFreeTexWord == 0xC87FE0 && kRegionBase + 4 * kFreeIbWord == 0xC87FE4, "disposicion");
static_assert(kRegionBase + 4 * kRegionWords == 0xC87FE8, "tamano del bloque");
// Pilas (arreglos de punteros de los buffers): 17 de texturas (0..16), 16 de index buffers (17..32) y la del generico (33). 8 palabras de guarda + 32 posiciones + 8 de guarda.
constexpr u32 kStacks = 34, kStackSlots = 32, kStackGuard = 8, kStackWords = kStackGuard + kStackSlots + kStackGuard;
static_assert(kStackWords == 48, "palabras por pila");
constexpr u32 kStackOfTex(u32 i) { return i; }
constexpr u32 kStackOfIb(u32 i) { return 17 + i; }   // i == 16: generico (33)
constexpr u32 kResCount = 128, kArgRes = 127;        // recursos falsos por tipo (texturas "R", index buffers "B"); 127 = el recurso que se pasa como argumento
constexpr u32 kResWords = 8;                         // vtable, id, niveles, formato, ancho, alto, tamano de IB, relleno
constexpr u32 kNewBlocks = 20, kNewBlockWords = 16;  // bloques que devuelve el new falso (19 = el que asigna el Setup falso)
constexpr u32 kOutWords = 4;                         // *pp = out[1]
constexpr u32 kGarbageSeed = 0x6A09E667u;

inline u32 Garbage(u32 n, u32 k) {   // basura que nunca coincide con un valor que escriban las funciones (dos bits altos a 1, y nunca 0xFFFFFFFF)
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// ------------------------------------------------------------------------------------------------ especificacion de un caso
struct Buf {
    u32 idx{}, a{}, b{}, c{}, cap{}, num{}, size{}, hasPtr{1};
    std::vector<u32> items;   // ids de recursos en las posiciones 0..items.size()-1 de la pila
};
struct Spec {
    u32 fn{};
    u32 use{1};
    u32 ntex{};
    std::vector<Buf> tex, ib;   // descritos (el resto del bloque es basura); ib idx 16 = generico
    bool hasFree{};
    u32 ftex{}, fib{};
    u32 args[3]{};              // argumentos escalares (numIndices+format, o width)
    u32 argx[3]{};              // CreateTexture: height, levels, format
    u32 res[5]{};               // recurso de argumento: niveles, formato, ancho, alto, tamano de IB
    u32 popRet{};               // 0 = nulo, k+1 = recurso k
    u32 devRet{}, devWrite{}, devRes{};
    u32 rszMode{};
    u32 reK{0xFFFFFFFFu}, reKind{}, reIdx{}, reVal{};
    std::vector<u32> newRet;    // por llamada a new: 1 = bloque del banco; otro = valor crudo
    u32 prior{};                // Init: 0 basura, 1 estado de un Init previo, 2 estado previo con contadores usados
    u32 outInit{};
    u32 stack{}, eax{}, ecx{}, edx{};
    u32 family{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_, u32 salt = 0) : n(n_ + salt * 0x10000u) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
    template <class T, std::size_t N> T Pick(const std::array<T, N>& a) { return a[R() % N]; }
};

// ids de pila: avanzan por el caso (los recursos se pueden repetir entre pilas)
struct Ids {
    u32 base, cnt{0};
    explicit Ids(u32 n) : base(Rand(n, 777)) {}
    u32 Next() { return (base + 5 * cnt++) % 100; }
};

inline Buf MakeBuf(Gen& g, Ids& ids, u32 idx, u32 size, u32 nItems, bool needPtr) {
    Buf b;
    b.idx = idx;
    b.a = 0x15 + g.P(40);
    b.b = g.C(60) ? (16u << g.P(6)) : g.R() & 0x3FFu;
    b.c = g.C(50) ? g.P(3) : (g.C(50) ? 0xFFFFFFFFu : 1u);
    b.cap = 1 + g.P(16);
    b.num = g.P(9);
    b.size = size;
    b.hasPtr = (needPtr || g.C(65)) ? 1u : 0u;
    if (b.hasPtr)
        for (u32 j = 0; j < nItems; ++j) b.items.push_back(ids.Next());
    return b;
}

inline void FillRegs(Spec& s, unsigned n) {
    s.stack = StackSeed(n);
    s.eax = Rand(n, 301);
    s.ecx = Rand(n, 302);
    s.edx = Rand(n, 303);
}
inline u32 DevRetValue(Gen& g) {
    static constexpr std::array<u32, 8> v{0u, 0u, 0u, 0x8876086Cu, 0x8007000Eu, 1u, 0x80004005u, 0x7FFFFFFFu};
    return g.C(88) ? g.Pick(v) : g.R();
}
inline u32 UseValue(Gen& g) {
    static constexpr std::array<u32, 6> v{0u, 1u, 1u, 1u, 0x80u, 0xFFu};
    return g.C(15) ? 0u : g.Pick(v);
}
inline u32 SizeValue(Gen& g, u32 maxPos) {   // tamano de pila para Cancel/Shutdown: incluye 0 y negativos
    const u32 r = g.P(100);
    if (r < 28) return 0;
    if (r < 78) return 1 + g.P(4);
    if (r < 90) return 5 + g.P(2);
    if (r < 95) return g.C(50) ? 0xFFFFFFFFu : (0x80000000u | g.R());
    return maxPos == 0 ? 0u : g.P(maxPos + 1);
}

// ------------------------------------------------------------------------------------------------ CancelBuffering / Shutdown
inline Spec MakeCancel(u32 n, bool shutdown) {
    Spec s;
    s.fn = shutdown ? kShutdown : kCancel;
    Gen g(n, shutdown ? 6 : 0);
    Ids ids(n + (shutdown ? 6000 : 0));
    s.family = n % 8;
    s.use = UseValue(g);
    u32 ntex = 0;
    switch (n % 8) {
    case 0: ntex = 0; break;
    case 1: ntex = 1; break;
    case 2: ntex = 2 + g.P(4); break;
    case 3: ntex = 6 + g.P(5); break;
    case 4: ntex = 17; break;
    case 5: ntex = 0x80000000u | g.R(); break;
    case 6: ntex = 0xFFFFFFFFu; break;
    default: ntex = 1 + g.P(4); break;
    }
    s.ntex = ntex;
    const bool positive = static_cast<i32>(ntex) > 0;
    u32 nd = positive ? std::min<u32>(ntex, 17u) : 1u;
    const bool reent = (n % 9 == 4) || (n % 13 == 5);
    if (reent && positive && nd < 17) ++nd;   // un buffer extra por si el falso sube NumTextureBuffers
    for (u32 i = 0; i < nd; ++i) {
        const u32 sz = SizeValue(g, 6);
        const bool pos = static_cast<i32>(sz) > 0;
        s.tex.push_back(MakeBuf(g, ids, i, sz, 6, pos));
    }
    for (u32 i = 0; i <= 16; ++i) {
        const u32 sz = SizeValue(g, 6);
        const bool pos = static_cast<i32>(sz) > 0;
        s.ib.push_back(MakeBuf(g, ids, i, sz, 6, pos));
    }
    s.hasFree = g.C(50);
    s.ftex = g.P(20);
    s.fib = g.P(20);
    if (reent) {
        // total aproximado de Release esperados para elegir en cual se produce la mutacion
        u32 total = 0;
        if (positive)
            for (u32 i = 0; i < std::min<u32>(ntex, 17u); ++i)
                if (static_cast<i32>(s.tex[i].size) > 0) total += s.tex[i].size;
        for (const Buf& b : s.ib)
            if (static_cast<i32>(b.size) > 0) total += b.size;
        const u32 kind = 1 + g.P(4);
        s.reK = g.P(total + 2);
        s.reKind = kind;
        if (kind == 1) {
            s.reVal = g.P(nd + 1);
        } else if (kind == 2) {
            std::vector<u32> cand;
            for (const Buf& b : s.tex) if (b.hasPtr) cand.push_back(b.idx);
            if (cand.empty()) s.reKind = 0; else { s.reIdx = cand[g.P(static_cast<u32>(cand.size()))]; s.reVal = g.P(7); }
        } else if (kind == 3) {
            std::vector<u32> cand;
            for (u32 i = 0; i < 16; ++i) if (s.ib[i].hasPtr) cand.push_back(i);
            if (cand.empty()) s.reKind = 0; else { s.reIdx = cand[g.P(static_cast<u32>(cand.size()))]; s.reVal = g.P(7); }
        } else {
            if (!s.ib[16].hasPtr) s.reKind = 0; else { s.reIdx = 16; s.reVal = g.P(7); }
        }
    }
    FillRegs(s, n);
    return s;
}

// ------------------------------------------------------------------------------------------------ CreateIndexBuffer
inline Spec MakeCreateIb(u32 n) {
    Spec s;
    s.fn = kCreateIb;
    Gen g(n, 1);
    Ids ids(n + 1000);
    u32 num = 0;
    s.family = n % 16;
    switch (n % 16) {
    case 0: num = 0; break;
    case 1: num = 1; break;
    case 2: num = 100; break;
    case 3: num = 101; break;
    case 4: num = 1600; break;
    case 5: num = 1601; break;
    case 6: num = 0x7FFFFFFFu; break;
    case 7: num = 1 + g.P(1600); break;
    case 8: num = 100 * (1 + g.P(18)); break;
    case 9: num = 1601 + g.P(100000); break;
    case 14: num = 100 * g.P(17) + 1; break;
    case 15: num = 0x7FFFFF00u + g.P(0xFF); break;
    default: num = 1 + g.P(1700); break;
    }
    s.args[0] = num;
    s.args[1] = g.R();   // format (sin uso)
    s.use = 1;
    const i32 group = static_cast<i32>(num - 1u) / 100;
    if (group < 16) {
        const u32 sz = g.C(38) ? 0u : 1 + g.P(3);
        s.ib.push_back(MakeBuf(g, ids, static_cast<u32>(group), sz, 4, sz != 0));
    }
    {   // senuelo: otro buffer con contenido que la funcion no debe tocar
        u32 d = g.P(16);
        if (group < 16 && d == static_cast<u32>(group)) d = (d + 1) % 16;
        s.ib.push_back(MakeBuf(g, ids, d, 1 + g.P(3), 4, true));
    }
    s.popRet = g.C(45) ? 0u : 1 + g.P(60);
    s.devRet = DevRetValue(g);
    s.devWrite = g.P(3);
    s.devRes = 90 + g.P(10);
    s.outInit = Garbage(n, 5);
    FillRegs(s, n);
    return s;
}

// ------------------------------------------------------------------------------------------------ CreateTexture
// Ancho de buffer que el EXE (fild + fcomp contra el ancho redondeado a float) considera igual a `w` (con signo, como CreateTexture).
inline u32 FloatBufWidth(u32 w, bool& ok) {
    const float f = static_cast<float>(static_cast<i32>(w));
    ok = f >= -2147483648.0f && f < 2147483648.0f;
    return ok ? static_cast<u32>(static_cast<i32>(f)) : 0u;
}
inline Spec MakeCreateTex(u32 n) {
    Spec s;
    s.fn = kCreateTex;
    Gen g(n, 2);
    Ids ids(n + 2000);
    s.family = n % 16;
    static constexpr std::array<u32, 28> widths{0u, 1u, 2u, 4u, 8u, 16u, 31u, 32u, 33u, 64u, 100u, 128u, 255u, 256u, 257u, 512u, 1024u, 4096u, 65536u, 0xFFFFFFu, 0x1000000u, 0x1000001u,
                                                0x1000003u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFF00u, 0x2000001u};
    static constexpr std::array<u32, 9> levelsV{0xFFFFFFFBu, 0xFFFFFFFFu, 0u, 1u, 2u, 3u, 100u, 0x7FFFFFFFu, 0x80000000u};
    static constexpr std::array<u32, 8> fmts{0x15u, 0x16u, 0x1Au, 0x1Bu, 0x32u, 0x31545844u, 0x35545844u, 0x1u};
    const u32 w = g.C(80) ? g.Pick(widths) : g.P(300);
    const u32 h = g.C(78) ? w : (g.C(50) ? g.Pick(widths) : g.P(300));
    const u32 lv = g.Pick(levelsV);
    const u32 fmt = g.C(85) ? g.Pick(fmts) : g.R();
    s.args[0] = w;
    s.argx[0] = h;
    s.argx[1] = lv;
    s.argx[2] = fmt;
    static constexpr std::array<u32, 14> counts{0u, 1u, 2u, 3u, 4u, 5u, 8u, 12u, 16u, 17u, 18u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu};
    u32 ntex = g.C(70) ? 2 + g.P(16) : g.Pick(counts);
    if (w == h && static_cast<i32>(ntex) > 17) ntex = 17;
    s.ntex = ntex;
    const u32 effLv = static_cast<i32>(lv) > 1 ? 0u : lv;
    s.use = 1;
    if (w == h && static_cast<i32>(ntex) > 1) {
        const u32 cnt = ntex;
        bool okf = false;
        const u32 fw = FloatBufWidth(w, okf);
        u32 match = 0xFFFFFFFFu;
        if (okf && g.C(60)) match = 1 + g.P(cnt - 1);
        for (u32 i = 1; i < cnt; ++i) {
            const u32 sz = g.C(35) ? 0u : 1 + g.P(3);
            Buf b = MakeBuf(g, ids, i, sz, 4, sz != 0);
            if (i == match) {
                b.b = fw;
                b.c = effLv;
                b.a = fmt;
            } else {
                // casi coincidentes: delatan el orden y la exactitud de la comparacion
                const u32 r = g.P(10);
                if (r < 2 && okf) { b.b = fw; b.c = effLv; b.a = fmt + 1; }
                else if (r < 4 && okf) { b.b = fw; b.c = effLv ^ 1u; b.a = fmt; }
                else if (r < 6 && okf) { b.b = fw + 1; b.c = effLv; b.a = fmt; }
                else if (r < 7 && okf) { b.b = fw - 1; b.c = effLv; b.a = fmt; }
                else if (r < 8 && okf) { b.b = fw; b.c = effLv; b.a = fmt; }   // tambien coincide: gana el primero
            }
            s.tex.push_back(b);
        }
    }
    s.popRet = g.C(40) ? 0u : 1 + g.P(60);
    s.devRet = DevRetValue(g);
    s.devWrite = g.P(3);
    s.devRes = 90 + g.P(10);
    s.outInit = Garbage(n, 6);
    FillRegs(s, n);
    return s;
}

// ------------------------------------------------------------------------------------------------ DestroyIndexBuffer
inline Spec MakeDestroyIb(u32 n) {
    Spec s;
    s.fn = kDestroyIb;
    Gen g(n, 3);
    Ids ids(n + 3000);
    s.family = n % 8;
    s.use = UseValue(g);
    u32 bytes = 0;
    switch (n % 8) {
    case 0: bytes = 0; break;
    case 1: bytes = 200 * (1 + g.P(8)); break;
    case 2: bytes = 2 * (100 * g.P(18) + 1 + g.P(2) * 99); break;
    case 3: bytes = 2 * (1600 + g.P(300)); break;
    case 4: bytes = 1 + 2 * g.P(900); break;                 // impar
    case 5: bytes = 0xFFFFFFFFu - g.P(5); break;
    default: bytes = 2 * (1 + g.P(1700)); break;
    }
    s.res[4] = bytes;
    const u32 idx = ((bytes >> 1) - 1u) / 100u;
    const u32 cap0 = g.C(10) ? 0u : 1 + g.P(8);
    u32 size = g.P(cap0 + 3);
    if (g.C(5)) size = 20 + g.P(8);
    Buf b = MakeBuf(g, ids, idx >= 16 ? 16u : idx, size, size, true);
    b.cap = (g.C(8)) ? 0x7FFFFFFFu : cap0;
    if (g.C(6)) b.cap = 0xFFFFFFFFu;   // capacidad negativa
    s.ib.push_back(b);
    {   // senuelo
        u32 d = g.P(17);
        if (d == b.idx) d = (d + 1) % 17;
        s.ib.push_back(MakeBuf(g, ids, d, 1 + g.P(3), 4, true));
    }
    s.rszMode = g.P(4);
    FillRegs(s, n);
    return s;
}

// ------------------------------------------------------------------------------------------------ DestroyTexture
inline Spec MakeDestroyTex(u32 n) {
    Spec s;
    s.fn = kDestroyTex;
    Gen g(n, 4);
    Ids ids(n + 4000);
    s.family = n % 8;
    s.use = UseValue(g);
    static constexpr std::array<u32, 22> sizesV{0u, 1u, 0x10u, 0x1Fu, 0x20u, 0x21u, 0x40u, 0x80u, 0x100u, 0x101u, 0x200u, 0x1000u, 0x80000000u, 0xFFFFFFFFu, 0x100000u, 0x1000001u, 0x1000000u,
                                                0x1000003u, 0x7FFFFFFFu, 0x3Fu, 0xFFu, 0x2000001u};
    static constexpr std::array<u32, 6> lcV{0u, 1u, 1u, 2u, 3u, 11u};
    static constexpr std::array<u32, 8> fmts{0x15u, 0x16u, 0x1Au, 0x1Bu, 0x32u, 0x31545844u, 0x35545844u, 0x1u};
    u32 w = 0, h = 0;
    switch (n % 8) {
    case 0: case 1: case 2: w = g.Pick(sizesV); h = w; break;             // cuadradas (coincidencia / buffer nuevo / generico)
    case 3: case 4: w = 0x20u << g.P(4); h = w; break;                      // 0x20..0x100
    case 5: w = g.P(0x140); h = g.P(0x140); break;                          // generico / liberar segun lados
    case 6: w = g.Pick(sizesV); h = g.C(50) ? w : g.Pick(sizesV); break;
    default: w = g.C(50) ? g.Pick(sizesV) : g.P(0x120); h = g.C(70) ? w : g.P(0x120); break;
    }
    s.res[0] = g.Pick(lcV);
    s.res[1] = g.C(85) ? g.Pick(fmts) : g.R();
    s.res[2] = w;
    s.res[3] = h;
    static constexpr std::array<u32, 12> counts{0u, 1u, 2u, 3u, 4u, 6u, 9u, 16u, 17u, 18u, 0x7FFFFFFFu, 0x80000000u};
    u32 ntex = g.C(75) ? g.P(18) : g.Pick(counts);
    if (g.C(5)) ntex = 0xFFFFFFFFu;
    const bool sq = (w == h);
    if (sq && static_cast<i32>(ntex) > 17) ntex = 17;
    if (sq && static_cast<i32>(ntex) < 0 && w >= 0x20 && w <= 0x100) ntex = g.P(2);
    s.ntex = ntex;
    const u32 singleLevel = s.res[0] > 1 ? 0u : 1u;
    if (sq && static_cast<i32>(ntex) > 1) {
        const u32 cnt = ntex;
        // el ancho que compara el EXE es el del recurso (sin signo) redondeado UNA vez a float
        const double wd = static_cast<double>(static_cast<float>(static_cast<double>(w)));
        const bool okd = wd < 2147483648.0;
        const u32 bw = okd ? static_cast<u32>(static_cast<i32>(wd)) : 0u;
        u32 match = 0xFFFFFFFFu;
        if (okd && g.C(55)) match = 1 + g.P(cnt - 1);
        for (u32 i = 1; i < cnt; ++i) {
            const u32 cap = 1 + g.P(8);
            const u32 size = g.P(cap + 3);
            Buf b = MakeBuf(g, ids, i, size, size, true);
            b.cap = (i == match && g.C(8)) ? 0x7FFFFFFFu : cap;
            if (i == match) {
                b.b = bw;
                b.c = singleLevel;
                b.a = s.res[1];
            } else {
                const u32 r = g.P(10);
                if (r < 2 && okd) { b.b = bw; b.c = singleLevel; b.a = s.res[1] + 1; }
                else if (r < 4 && okd) { b.b = bw; b.c = singleLevel ^ 1u; b.a = s.res[1]; }
                else if (r < 6 && okd) { b.b = bw + 1; b.c = singleLevel; b.a = s.res[1]; }
                else if (r < 8 && okd) { b.b = bw; b.c = singleLevel; b.a = s.res[1]; }
            }
            s.tex.push_back(b);
        }
    }
    {   // buffer generico (0): se usa en la ruta final
        const u32 cap = 1 + g.P(8);
        const u32 size = g.P(cap + 3);
        Buf b = MakeBuf(g, ids, 0, size, size, true);
        b.cap = cap;
        s.tex.insert(s.tex.begin(), b);
    }
    s.rszMode = g.P(4);
    FillRegs(s, n);
    return s;
}

// ------------------------------------------------------------------------------------------------ Init
inline Spec MakeInit(u32 n) {
    Spec s;
    s.fn = kInit;
    Gen g(n, 5);
    s.family = n % 4;
    s.use = g.C(30) ? 1u : g.P(3);
    s.ntex = g.C(30) ? 1u : g.R();
    s.prior = n % 4 == 1 ? 1u : (n % 4 == 2 ? 2u : 0u);
    for (u32 i = 0; i < 18; ++i) {
        const u32 r = g.P(100);
        s.newRet.push_back(r < 96 ? 1u : (r < 97 ? 0u : (r < 98 ? 0xFFFFFFFFu : 0xDEAD0000u + i)));
    }
    s.hasFree = true;
    s.ftex = g.P(20);
    s.fib = g.P(20);
    FillRegs(s, n);
    return s;
}

inline Spec MakeSpec(u32 fn, u32 n) {
    switch (fn) {
    case kCancel: return MakeCancel(n, false);
    case kCreateIb: return MakeCreateIb(n);
    case kCreateTex: return MakeCreateTex(n);
    case kDestroyIb: return MakeDestroyIb(n);
    case kDestroyTex: return MakeDestroyTex(n);
    case kInit: return MakeInit(n);
    default: return MakeCancel(n, true);
    }
}

// ------------------------------------------------------------------------------------------------ JSON
inline std::string U(u32 v) { return std::to_string(v); }
inline std::string JsonBuf(const Buf& b) {
    std::string s = "[" + U(b.idx) + "," + U(b.a) + "," + U(b.b) + "," + U(b.c) + "," + U(b.cap) + "," + U(b.num) + "," + U(b.size) + "," + U(b.hasPtr) + ",[";
    for (std::size_t i = 0; i < b.items.size(); ++i) { if (i) s += ','; s += U(b.items[i]); }
    return s + "]]";
}
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"use\":" + U(s.use) + ",\"ntex\":" + U(s.ntex) + ",\"tex\":[";
    for (std::size_t i = 0; i < s.tex.size(); ++i) { if (i) o += ','; o += JsonBuf(s.tex[i]); }
    o += "],\"ib\":[";
    for (std::size_t i = 0; i < s.ib.size(); ++i) { if (i) o += ','; o += JsonBuf(s.ib[i]); }
    o += "],\"free\":[" + U(s.hasFree ? 1 : 0) + "," + U(s.ftex) + "," + U(s.fib) + "],\"args\":[" + U(s.args[0]) + "," + U(s.args[1]) + "," + U(s.args[2]) + "],\"argx\":[" +
         U(s.argx[0]) + "," + U(s.argx[1]) + "," + U(s.argx[2]) + "],\"res\":[" + U(s.res[0]) + "," + U(s.res[1]) + "," + U(s.res[2]) + "," + U(s.res[3]) + "," + U(s.res[4]) +
         "],\"fk\":[" + U(s.popRet) + "," + U(s.devRet) + "," + U(s.devWrite) + "," + U(s.devRes) + "," + U(s.rszMode) + "," + U(s.reK) + "," + U(s.reKind) + "," + U(s.reIdx) + "," + U(s.reVal) +
         "],\"newRet\":[";
    for (std::size_t i = 0; i < s.newRet.size(); ++i) { if (i) o += ','; o += U(s.newRet[i]); }
    o += "],\"prior\":" + U(s.prior) + ",\"out\":" + U(s.outInit) + ",\"regs\":[" + U(s.stack) + "," + U(s.eax) + "," + U(s.ecx) + "," + U(s.edx) + "]}";
    return o;
}
}  // namespace D3D62Fix
