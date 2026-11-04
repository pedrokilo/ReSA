#pragma once
// Fixtures del banco Fn95C (lote 95C): 16384 casos por funcion (6 x 16384 = 98304 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 GetFirstTextureCallback            0x734930  cdecl (textura, puntero a dato): *dato = textura; devuelve nulo                 (hook S)
//   1 RwTexDictionaryGtaStreamRead1      0x731070  cdecl (stream); devuelve el diccionario o nulo                                   (hook S)
//   2 RwTexDictionaryGtaStreamRead2      0x731150  cdecl (stream, diccionario); devuelve el diccionario o nulo                      (hook S)
//   3 RpClumpGtaStreamRead1              0x72E570  cdecl (stream); devuelve bool en AL                                              (hook S)
//   4 RpClumpGtaStreamRead2              0x72E620  cdecl (stream); devuelve el clump o nulo                                         (hook S)
//   5 RpClumpGtaCancelStream             0x72E700  cdecl (); void                                                                   (hook S)
//
// Todos los valores viajan como enteros de 32 bits sin signo. Familia = n % 16 (1024 casos por familia). Los scripts de los falsos (resultado de la k-esima llamada de cada tipo)
// los fija el caso; mas alla de la longitud de un script el falso devuelve fallo (0).
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

namespace F95CFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 6;
constexpr unsigned kFc = 32, kRd = 8, kTex = 24, kGeo = 24, kAtoms = 8, kGa = 32, kFa = 16;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kStreamWin = 0x10, kClumpWin = 0x10, kAtomWin = 0x10, kFramesWin = kFa * 4, kGeomWin = kGa * 4, kDataWin = 0x10;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 95003u); }
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
    Js& A(const char* k, const u32* p, std::size_t n) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < n; ++i) { if (i) s += ','; s += std::to_string(p[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    // fn 0
    u32 cbTex{}, cbData{};
    // stream y fakes (posicion inicial del stream en +0xC y cuanto avanza con cada tipo de llamada: 0 FindChunk, 1 Read, 2 textura, 3 geometria, 4 lista de frames)
    u32 spos{}, step[5]{};
    u32 fcVal[kFc]{}, fcLen[kFc]{}, fcVer[kFc]{};          // RwStreamFindChunk: valor devuelto (0 = falla), longitud y version que escribe
    u32 rdMode[kRd]{}, rdPay[kRd][4]{};                    // RwStreamRead: modo del valor devuelto y los 16 bytes que escribe (menos de `longitud`)
    u32 tex[kTex]{}, pokeMask{}, pokeVal[kTex]{};          // RwTextureGtaStreamRead: textura (0 = falla); tras la k-esima, si el bit k de pokeMask, 0x8D6088 = pokeVal[k]
    u32 createVal{}, txdArg{};
    u32 geo[kGeo]{}, atomNull{}, clumpNull{}, mallocNull{};
    u32 flRet{}, flMode{}, flNum{};                        // _rwFrameListStreamRead: valor devuelto y lo que escribe en la lista (modo del vector, contador)
    // estado inicial de los globales del EXE
    u32 g6088{}, gFE8{}, gAFC{}, g607C{}, hdr[3]{}, glFrames{}, glNum{}, glArr{}, flN{};   // glFrames/glArr: 1 = arena propia, otro valor = crudo
    u32 ga[kGa]{}, fa[kFa]{}, clumpParent0{}, atomFlags0[kAtoms]{};
};

// ------------------------------------------------------------------------------------------------ generadores
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
inline u32 OkVal(Gen& g) { return g.Pk({1u, 1u, 1u, 1u, 0x100u, 0x80000000u, 0xFFFFFFFFu, 2u, g.W() | 1u}); }
inline u32 NonZero(Gen& g) { const u32 w = g.W(); return w ? w : 1u; }
inline u32 Step(Gen& g) { return g.P(55) ? 0u : g.Pk({4u, 8u, 0x10u, 0x14u, 0x800u, 0xFFFFFFF0u, g.W() & 0xFFFFu}); }

// Estado comun: basura en todo lo que la funcion no deberia depender de, y scripts vacios (todo falla salvo lo que cada generador anade)
inline void Base(C& c, Gen& g) {
    c.spos = g.P(70) ? (g.W() & 0xFFFFu) : g.W();
    for (u32 i = 0; i < 5; ++i) c.step[i] = Step(g);
    c.createVal = NonZero(g); c.txdArg = g.P(90) ? NonZero(g) : 0u;
    c.g6088 = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), 5u}); c.gFE8 = g.P(50) ? c.spos : g.W(); c.gAFC = g.P(50) ? c.spos : g.W(); c.g607C = g.W();
    for (auto& h : c.hdr) h = g.W();
    c.glFrames = g.W(); c.glNum = g.W(); c.glArr = g.W(); c.flN = g.W();
    for (auto& v : c.ga) v = NonZero(g);
    for (auto& v : c.fa) v = NonZero(g);
    c.clumpParent0 = g.W();
    for (auto& v : c.atomFlags0) v = g.R(256);
    c.flRet = 0; c.flMode = g.R(3); c.flNum = g.Pk({0u, 1u, 2u, 3u, 8u, 0xFFFFFFFFu, g.R(20)});
    c.pokeMask = 0;
}

// ------------------------------------------------------------------------------------------------ funcion 0
inline void MakeCb0(C& c, Gen& g) {
    c.cbTex = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u, 1u, g.W(), g.W(), g.W(), 0x12345678u});
    c.cbData = g.Pk({0u, 0xFFFFFFFFu, c.cbTex, g.W(), g.W(), 0xDEADBEEFu});
}

// ------------------------------------------------------------------------------------------------ funcion 1: TXD fase 1
inline void MakeTxd1(C& c, Gen& g) {
    const u32 f = c.family;
    u32 len = g.P(60) ? 12u : 2u + g.R(11);
    u32 count = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 12u, 16u, 20u, 24u, g.R(25)});
    if (f == 0) count = 0;
    if (f == 1) count = 1;
    if (f == 8) count |= g.W() & 0xFFFF0000u;
    if (f == 9) count = g.Pk({0xFFFFu, 0x8000u, 0x7FFFu, 100u, 25u, 48u, 0x10000u + g.R(30), 0x1FFFFu});
    if (f == 15) count = 3 + g.R(22);
    if (f == 13) { len = g.Pk({8u, 9u, 10u, 11u, 12u}); }
    if (f == 10) { len = g.Pk({2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u}); }
    u32 fcv = OkVal(g), rdm = 0, pay1 = len;
    bool createNull = false;
    int failTex = -1;
    u32 fault = 0;
    switch (f) {
    case 4: fault = 1; break;
    case 5: fault = 2; break;
    case 6: fault = 3; break;
    case 7: fault = 4; break;
    case 14: fault = g.R(5); break;
    default: if (g.P(8)) fault = 1 + g.R(4); break;
    }
    if (fault == 1) fcv = 0;
    if (fault == 2) { rdm = 1 + g.R(5); if (g.P(30)) { rdm = 0; pay1 = len + 1 + g.R(5); } }
    if (fault == 3) createNull = true;
    if (fault == 4) failTex = int(g.R(12));
    if (f == 5 && fault == 2 && g.P(30)) rdm = 5;
    c.fcVal[0] = fcv; c.fcLen[0] = len; c.fcVer[0] = g.W();
    c.rdMode[0] = rdm;
    c.rdPay[0][0] = count; c.rdPay[0][1] = pay1; c.rdPay[0][2] = g.W(); c.rdPay[0][3] = g.W();
    c.createVal = createNull ? 0u : NonZero(g);
    for (u32 k = 0; k < kTex; ++k) c.tex[k] = NonZero(g);
    if (failTex >= 0) c.tex[failTex] = 0;
    if (f == 11 || g.P(10)) {
        c.pokeMask = g.W() & ((1u << (1 + g.R(6))) - 1u);
        for (u32 k = 0; k < kTex; ++k) c.pokeVal[k] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, g.R(30), g.R(12)});
    }
    if (f == 12) for (u32 i = 0; i < 5; ++i) c.step[i] = g.W() & 0xFFFFu;
}

// ------------------------------------------------------------------------------------------------ funcion 2: TXD fase 2
inline void MakeTxd2(C& c, Gen& g) {
    const u32 f = c.family;
    u32 left = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 10u, 12u, g.R(25)});
    if (f == 0) left = 0;
    if (f == 1) left = 1;
    if (f == 3) left = g.Pk({0xFFFFFFFFu, 0x80000000u, 100u, 25u, 24u, 30u, 0x7FFFFFFFu, 0xFFFFFFF0u});
    if (f == 15) left = 1 + g.R(12);
    c.g6088 = left;
    for (u32 k = 0; k < kTex; ++k) c.tex[k] = NonZero(g);
    if (f == 2 || (f != 15 && g.P(12))) c.tex[g.R(12)] = 0;
    if (f == 4 || g.P(10)) {
        c.pokeMask = g.W() & ((1u << (1 + g.R(6))) - 1u);
        for (u32 k = 0; k < kTex; ++k) c.pokeVal[k] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, g.R(30), g.R(12)});
    }
    if (f == 5) c.gFE8 = g.Pk({c.spos + 0x10u, c.spos - 0x10u, 0u, 0xFFFFFFFFu, c.spos + 0x7FFFFFFFu, g.W()});
    if (f == 6) for (u32 i = 0; i < 5; ++i) c.step[i] = g.W() & 0xFFFFu;
    if (f == 7) { c.spos = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u}); c.gFE8 = g.Pk({0u, 0xFFFFFFFFu, 0x7FFFFFFFu}); }
}

// ------------------------------------------------------------------------------------------------ funcion 3: clump fase 1
inline void MakeClump1(C& c, Gen& g) {
    const u32 f = c.family;
    u32 count = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 10u, 12u, 16u, 20u, 24u, g.R(25)});
    if (f == 11) count = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFF00u, 0xFFFFFFFEu, 0x80000001u, 0xFFFFFFF0u});
    if (f == 12) count = 20 + g.R(13);
    if (f == 13) count = g.Pk({33u, 100u, 0x1000u, 0x40000000u, 0x7FFFFFFFu, 0x3FFFFFFFu, 0x20000000u});
    if (f == 15) count = 1 + g.R(24);
    c.hdr[0] = g.P(8) ? g.W() : g.R(6); c.hdr[1] = g.R(3); c.hdr[2] = g.R(3);
    const i32 sc = i32(count);
    const u32 half = sc > 0 ? u32(sc) / 2 : 0u;
    const bool bigNoMalloc = sc > 32;
    // puntos de fallo: 0 fc0, 1 rd0, 2 fc1, 3 frame list, 4 fc2, 5 fc3, 6 rd1, 7 malloc, 8.. geometria j (fc y luego lectura)
    u32 fault = 99;
    switch (f) {
    case 1: fault = 0; break;
    case 2: fault = 1; break;
    case 3: fault = 2; break;
    case 4: fault = 3; break;
    case 5: fault = 4; break;
    case 6: fault = 5; break;
    case 7: fault = 6; break;
    case 8: fault = 7; if (sc <= 0) count = 4; break;
    case 9: fault = 8; break;
    case 10: fault = 100; break;
    case 14: fault = g.R(8); break;
    default: if (g.P(6)) fault = g.R(8); break;
    }
    if (bigNoMalloc) fault = 7;
    u32 j = half ? g.R(half) : 0;
    c.fcVal[0] = OkVal(g); c.fcLen[0] = g.Pk({12u, 12u, 0x1Cu, g.W()}); c.fcVer[0] = g.W();
    c.rdPay[0][0] = c.hdr[0]; c.rdPay[0][1] = c.hdr[1]; c.rdPay[0][2] = c.hdr[2]; c.rdPay[0][3] = g.W();
    c.fcVal[1] = OkVal(g); c.fcVer[1] = g.W();
    c.flRet = g.Pk({1u, 0xC87AF4u, 0xFFFFFFFFu, g.W() | 1u}); c.flMode = g.R(3); c.flNum = g.Pk({0u, 1u, 2u, 3u, 8u, 0xFFFFFFFFu, g.R(20)});
    c.fcVal[2] = OkVal(g); c.fcVer[2] = g.W();
    c.fcVal[3] = OkVal(g); c.fcLen[3] = g.Pk({4u, 4u, g.W()}); c.fcVer[3] = g.W();
    c.rdPay[1][0] = count; c.rdPay[1][1] = g.W(); c.rdPay[1][2] = g.W(); c.rdPay[1][3] = g.W();
    c.mallocNull = 0;
    for (u32 k = 0; k < half && k < kGeo; ++k) { c.fcVal[4 + k] = OkVal(g); c.fcVer[4 + k] = g.W(); c.geo[k] = NonZero(g); }
    if (fault == 0) c.fcVal[0] = 0;
    if (fault == 1) c.rdMode[0] = g.Pk({1u, 2u, 3u, 4u});
    if (fault == 2) c.fcVal[1] = 0;
    if (fault == 3) c.flRet = 0;
    if (fault == 4) c.fcVal[2] = 0;
    if (fault == 5) c.fcVal[3] = 0;
    if (fault == 6) c.rdMode[1] = g.Pk({1u, 2u, 3u, 4u});
    if (fault == 7) c.mallocNull = 1;
    if (fault == 8 && half) { if (g.P(50)) c.fcVal[4 + j] = 0; else c.geo[j] = 0; }
    if (fault == 100 && half) { c.geo[j] = 0; if (j + 1 < kGeo) c.fcVal[4 + j + 1] = 0; }
    for (u32 i = 0; i < 5; ++i) if (f == 14 || g.P(15)) c.step[i] = g.W() & 0xFFFFu;
}

// ------------------------------------------------------------------------------------------------ funcion 4: clump fase 2
inline void MakeClump2(C& c, Gen& g) {
    const u32 f = c.family;
    u32 natoms = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 4u, 5u});
    u32 glNum = g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 6u, 8u, 10u, 12u});
    if (f == 0) { natoms = 0; }
    if (f == 1) { glNum = 0; }
    if (f == 15) { natoms = 1 + g.R(5); glNum = 1 + g.R(12); }
    c.hdr[0] = natoms; c.hdr[1] = g.R(4); c.hdr[2] = g.R(4);
    if (f == 12) c.hdr[0] = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFF00u});
    c.glNum = glNum; c.glArr = glNum ? 1u : g.Pk({0u, 1u, g.W()});
    c.glFrames = 1u;
    c.flN = g.Pk({0u, 1u, 2u, 3u, 8u, 16u});
    if (f == 13) c.flN = 0;
    if (f == 14) c.flN = g.Pk({1u, 2u, 8u});
    c.g607C = glNum ? (g.P(70) ? g.R(glNum + 1) : g.Pk({0u, glNum, glNum + 3u})) : g.Pk({0u, 0u, 3u, 5u, 0x7FFFFFFFu});
    if (f == 2) c.g607C = 0;
    for (u32 i = 0; i < kGa; ++i) c.ga[i] = g.P(65) ? NonZero(g) : 0u;
    // script nominal con lista de puntos de fallo
    struct Pt { u32 kind, idx; };   // 0 fc (fcVal[idx]=0), 1 geo (geo[idx]=0), 2 rd mode, 3 atom create null, 4 clump null
    std::vector<Pt> pts;
    u32 kfc = 0, kgeo = 0;
    i32 start = i32(c.g607C);
    for (i32 i = start; i < i32(glNum) && kfc < kFc && kgeo < kGeo; ++i) {
        c.fcVal[kfc] = OkVal(g); c.fcVer[kfc] = g.W(); pts.push_back({0, kfc}); ++kfc;
        c.geo[kgeo] = NonZero(g); pts.push_back({1, kgeo}); ++kgeo;
    }
    const i32 na = i32(natoms);
    for (i32 a = 0; a < na && a < i32(kAtoms) && kfc + 3 <= kFc; ++a) {
        c.fcVal[kfc] = OkVal(g); c.fcVer[kfc] = g.W(); pts.push_back({0, kfc}); ++kfc;           // ATOMIC
        c.fcVal[kfc] = OkVal(g); c.fcLen[kfc] = 16; c.fcVer[kfc] = g.W(); pts.push_back({0, kfc}); ++kfc;   // STRUCT
        c.rdPay[a][0] = g.R(kFa); c.rdPay[a][1] = g.R(kGa); c.rdPay[a][2] = g.W(); c.rdPay[a][3] = g.W();
        pts.push_back({2, u32(a)}); pts.push_back({3, u32(a)});
        if (glNum == 0 && kgeo < kGeo && kfc < kFc) {
            c.fcVal[kfc] = OkVal(g); c.fcVer[kfc] = g.W(); pts.push_back({0, kfc}); ++kfc;
            c.geo[kgeo] = NonZero(g); pts.push_back({1, kgeo}); ++kgeo;
        }
    }
    pts.push_back({4, 0});
    bool wantFault = f == 3 || f == 4 || f == 5 || f == 6 || f == 7 || f == 8 || f == 9 || f == 10 || f == 11 || (f != 15 && g.P(8));
    if (wantFault && !pts.empty()) {
        Pt p = pts[g.R(u32(pts.size()))];
        if (f == 11) p = {4, 0};
        if (f == 10) { for (const auto& q : pts) if (q.kind == 3) { p = q; break; } }
        if (f == 9) { for (const auto& q : pts) if (q.kind == 2) { p = q; break; } }
        switch (p.kind) {
        case 0: c.fcVal[p.idx] = 0; break;
        case 1: c.geo[p.idx] = 0; break;
        case 2: c.rdMode[p.idx] = g.Pk({1u, 2u, 3u, 4u}); break;
        case 3: c.atomNull |= 1u << p.idx; break;
        default: c.clumpNull = 1; break;
        }
    }
    // algunos atomicos con longitud distinta (la longitud se relee y debe coincidir con lo leido)
    c.clumpParent0 = g.W();
    for (u32 i = 0; i < 5; ++i) if (f == 6 || g.P(15)) c.step[i] = g.W() & 0xFFFFu;
    if (f == 7) c.spos = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u}), c.gAFC = g.Pk({0u, 0xFFFFFFFFu, 0x7FFFFFFFu});
}

// ------------------------------------------------------------------------------------------------ funcion 5: cancelar
inline void MakeCancel5(C& c, Gen& g) {
    const u32 f = c.family;
    u32 num = g.Pk({0u, 1u, 2u, 3u, 5u, 8u, 12u, 16u, 20u, 32u, g.R(33)});
    if (f == 0) num = 0;
    if (f == 1) num = 1;
    if (f == 2) num = g.Pk({0xFFFFFFFFu, 0x80000000u, 0xFFFFFFF0u, 0xFFFFFF00u});
    if (f == 15) num = 1 + g.R(32);
    c.glNum = num;
    const i32 sn = i32(num);
    c.glArr = sn > 0 ? 1u : g.Pk({0u, 1u, g.W(), 0xFFFFFFFFu});
    for (u32 i = 0; i < kGa; ++i) c.ga[i] = g.P(f == 3 ? 100 : (f == 4 ? 0 : 60)) ? NonZero(g) : 0u;
    c.flN = g.Pk({0u, 1u, 3u, 0xFFFFFFFFu, g.W()});
    c.glFrames = g.Pk({0u, g.W(), 0x1000u, 1u});
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    Gen g(n, 100 + fn * 1000);
    Base(c, g);
    switch (fn) {
    case 0: MakeCb0(c, g); break;
    case 1: MakeTxd1(c, g); break;
    case 2: MakeTxd2(c, g); break;
    case 3: MakeClump1(c, g); break;
    case 4: MakeClump2(c, g); break;
    default: MakeCancel5(c, g); break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    if (c.fn == 0) { j.U("tex", c.cbTex).U("data", c.cbData); return j.End(); }
    const u32 glob[11] = {c.g607C, c.g6088, c.gFE8, c.hdr[0], c.hdr[1], c.hdr[2], c.glFrames, c.flN, c.gAFC, c.glArr, c.glNum};
    j.A("glob", glob, 11);
    if (c.fn == 5) { j.A("ga", c.ga, kGa); return j.End(); }
    j.U("spos", c.spos).A("step", c.step, 5);
    if (c.fn == 1) {
        j.A("fcval", c.fcVal, 1).A("fclen", c.fcLen, 1).A("rdmode", c.rdMode, 1).A("rdpay", &c.rdPay[0][0], 4).U("createval", c.createVal).A("tex", c.tex, kTex);
        j.U("pokemask", c.pokeMask).A("pokeval", c.pokeVal, kTex);
    } else if (c.fn == 2) {
        j.U("txdarg", c.txdArg).A("tex", c.tex, kTex).U("pokemask", c.pokeMask).A("pokeval", c.pokeVal, kTex);
    } else if (c.fn == 3) {
        j.A("fcval", c.fcVal, kFc).A("rdmode", c.rdMode, 2).A("rdpay", &c.rdPay[0][0], 8).A("geo", c.geo, kGeo);
        j.U("flret", c.flRet).U("flmode", c.flMode).U("flnum", c.flNum).U("mallocnull", c.mallocNull).A("ga", c.ga, kGa);
    } else {
        j.A("fcval", c.fcVal, kFc).A("fclen", c.fcLen, kFc).A("rdmode", c.rdMode, kRd).A("rdpay", &c.rdPay[0][0], 4 * kRd).A("geo", c.geo, kGeo);
        j.U("atomnull", c.atomNull).U("clumpnull", c.clumpNull).A("ga", c.ga, kGa).A("fa", c.fa, kFa).U("clumpparent0", c.clumpParent0).A("atomflags0", c.atomFlags0, kAtoms);
    }
    return j.End();
}
}
