#pragma once
// Fixtures del banco Fn86C (lote 86C): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp
// y un programa autonomo de calibracion de cobertura (fuera del repo).
//
//   0 CPathFind::GeneratePedCreationCoors   0x44E790  thiscall (this = CPathFind en ECX), bool en AL, `ret 30h` (12 argumentos en pila)   (hook CPathFind / GeneratePedCreationCoors, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Familia = n % 16 (1024 casos por familia). Politica de especiales (como PathFind62): los floats que la funcion
// REENVIA por valor a un colaborador (posicion, distancia maxima) usan solo NaN silenciosos (el x87 convierte un NaN senalizador al cargarlo); un caso lleva a lo sumo un NaN con carga propia.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Fn86CFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFns = 1;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 86003u); }
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
    Js& A(const char* k, const std::vector<u32>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < v.size(); ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

inline float F(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// Aritmetica del generador con un redondeo a float por operacion (identica en SSE y x87 de 24 bits para valores normales).
inline float Fadd(float a, float b) { volatile float r = a + b; return r; }
inline float Fsub(float a, float b) { volatile float r = a - b; return r; }
inline float Fmul(float a, float b) { volatile float r = a * b; return r; }

inline u32 QNan(u32 slot) { return ((slot & 1u) << 31) | 0x7FC00000u | (0x100u + slot * 0x37u); }
inline u32 HugeFloat(u32 n, u32 k) {
    const u32 sign = Rand(n, k) & 1u;
    return (sign << 31) | ((200u + Rand(n, k + 1) % 55u) << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 TinyFloat(u32 n, u32 k) {
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = Rand(n, k + 1) % 3 == 0 ? Rand(n, k + 3) % 6u : 0u;
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 InfBits(u32 n, u32 k) { return Rand(n, k) & 1u ? 0xFF800000u : 0x7F800000u; }
// siguiente / anterior valor representable en magnitud (d = +1 aleja de cero, d = -1 acerca)
inline u32 Ulp(u32 bits, int d) {
    const u32 sign = bits & 0x80000000u, mag = bits & 0x7FFFFFFFu;
    if (mag >= 0x7F800000u) return bits;
    if (d < 0 && mag == 0) return sign | 0u;
    return sign | u32(i32(mag) + d);
}
// float en [lo, lo + span) con resolucion de 1/1000000
inline float Span(u32 n, u32 k, float lo, float span) { return Fadd(lo, Fmul(float(Rand(n, k) % 1000000u) * 0.000001f, span)); }

// _ftol del EXE (0x821B40): 32 bits bajos del entero de 64 bits truncado; NaN, infinitos y |v| >= 2^63 dan 0
inline i32 FtolLow32(float v) {
    if (v != v) return 0;
    const double d = v;
    if (d >= 9223372036854775808.0 || d < -9223372036854775808.0) return 0;
    return i32(std::int64_t(d));
}
// region (0..7) de una coordenada: (p + 3000) * 0.0013333333 (0x3AAEC33E), truncada y acotada
inline int Region(float p) {
    volatile float s = Fmul(Fsub(p, -3000.0f), F(0x3AAEC33Eu));
    const i32 r = FtolLow32(s);
    return r <= 0 ? 0 : (r < 7 ? r : 7);
}

// ------------------------------------------------------------------------------------------------ mundo falso de rutas
// Arenas (guarda | objeto | guarda), sumas a mano:
//   this (CPathFind) 0x40 + 0x3C80 + 0x40 = 0x3D00
//   nodos de un area (24 x 0x1C = 0x2A0)  0x40 + 0x2A0 + 0x40 = 0x320
//   enlaces de un area (160 x 4 = 0x280)  0x40 + 0x280 + 0x40 = 0x300
//   intersecciones de un area (160 x 1 = 0xA0) 0x40 + 0xA0 + 0x40 = 0x120
constexpr u32 kThisBytes = 0x3C80, kThisOff = 0x40, kThisArena = 0x3D00;
constexpr u32 kNodeBytes = 0x1C, kMaxNodes = 24, kNodesBytes = 0x2A0, kNodesOff = 0x40, kNodesArena = 0x320;
constexpr u32 kMaxLinks = 160, kLinksBytes = 0x280, kLinksOff = 0x40, kLinksArena = 0x300;
constexpr u32 kInterBytes = 0xA0, kInterOff = 0x40, kInterArena = 0x120;
constexpr u32 kMaxSlots = 3;
// Arenas pequenas de parametros de salida (CVector, CNodeAddress, float, matriz...): 0x40 + 0x40 + 0x40 = 0x140
constexpr u32 kSmallObj = 0x40, kSmallBytes = 0x40, kSmallArena = 0x140;
constexpr u32 kOffPathNodes = 0x804, kOffNodeLinks = 0xA44, kOffIntersections = 0xC84, kOffNumNodes = 0xFA4, kOffNumVeh = 0x10C4, kOffNumPed = 0x11E4;
constexpr u32 kNumAreas = 72;
constexpr u32 kGroundN = 16;

struct World {
    u32 nAreas{};
    u32 area[kMaxSlots]{}, nVeh[kMaxSlots]{}, nPed[kMaxSlots]{};
    std::vector<u32> nodes[kMaxSlots];   // 7 dwords por nodo (nVeh + nPed nodos)
    std::vector<u32> links[kMaxSlots];   // CNodeAddress como dword (area | nodo << 16)
    std::vector<u32> inter[kMaxSlots];   // bytes de CPathIntersectionInfo empaquetados de 4 en 4 (kInterBytes / 4 palabras)
    u32 Total(u32 s) const { return nVeh[s] + nPed[s]; }
    int SlotOf(u32 a) const { for (u32 s = 0; s < nAreas; ++s) if (area[s] == a) return int(s); return -1; }
};
// word 2: x | y << 16 (int16), word 3: z | totalDist << 16, word 4: baseLink | areaId << 16, word 5: nodeId | width << 16 | flood << 24, word 6: bytes +0x18..+0x1B
inline u32 PackXY(int x, int y) { return u32(u16(x)) | (u32(u16(y)) << 16); }
inline void AddNode(std::vector<u32>& v, u32 w0, u32 w1, int x, int y, int z, int total, int base, u32 areaId, u32 nodeId, u32 width, u32 flood, u32 flags) {
    v.push_back(w0); v.push_back(w1); v.push_back(PackXY(x, y)); v.push_back(u32(u16(z)) | (u32(u16(total)) << 16));
    v.push_back(u32(u16(base)) | (areaId << 16)); v.push_back((nodeId & 0xFFFFu) | ((width & 0xFFu) << 16) | ((flood & 0xFFu) << 24)); v.push_back(flags);
}
inline void JsonWorld(Js& j, const World& w) {
    std::vector<u32> a(w.area, w.area + w.nAreas), v(w.nVeh, w.nVeh + w.nAreas), p(w.nPed, w.nPed + w.nAreas);
    j.A("areas", a).A("nveh", v).A("nped", p);
    static const char* const nk[kMaxSlots]{"nodes0", "nodes1", "nodes2"};
    static const char* const lk[kMaxSlots]{"links0", "links1", "links2"};
    static const char* const ik[kMaxSlots]{"inter0", "inter1", "inter2"};
    for (u32 s = 0; s < w.nAreas; ++s) { j.A(nk[s], w.nodes[s]); j.A(lk[s], w.links[s]); j.A(ik[s], w.inter[s]); }
}

// ------------------------------------------------------------------------------------------------ el caso
struct P {
    u32 family{}; World w;
    u32 x{}, y{}, minD1{}, maxD1{}, minD2{}, maxD2{};
    u32 lowTraffic{};                                   // dword de la pila (solo el byte bajo cuenta)
    u32 matrixMode{};                                   // 0: matriz nula (0x420D40), 1: matriz no nula (0x420C40)
    u32 randMode{}, randSeed{};                         // guion de rand() del CRT del EXE
    u32 camMode{}, camSeed{};                           // guion de IsSphereVisible: visible(k)
    u32 groundFound[kGroundN]{}, groundDelta[kGroundN]{};
    u32 outCoordsInit[3]{}, outOriInit{}, outAddrInit[2]{};
    u32 eax{}, edx{}, stack{};
};
// valor k-esimo de rand() del guion (0..32767)
inline u32 ScriptRand(u32 mode, u32 seed, u32 k) {
    switch (mode) {
    case 0: return 0;
    case 1: return 0x7FFF;
    case 2: return (k * 64u) & 0x7FFFu;
    case 3: return (Rand(seed, k) & 1u) ? 0x7FFFu : 0u;
    case 4: return Rand(seed, k) & 0x7FFFu;
    case 5: return (Rand(seed, k) & 0x7FFFu) | 1u;
    case 6: return (Rand(seed, k) & 0x7FFFu) & ~1u;
    default: return (k * 37u + 11u) & 0x7FFFu;
    }
}
// resultado del k-esimo IsSphereVisible: AL (0 = no visible); el resto de EAX es basura
inline u32 CamResult(u32 mode, u32 seed, u32 k) {
    const bool vis = (ScriptRand(mode, seed, k) & 1u) != 0;
    const u32 junk = Rand(seed, 1000u + k) & ~0xFFu;
    return vis ? (junk | (1u + (Rand(seed, 2000u + k) % 255u))) : junk;
}
// hash acumulado de eventos (identico en C++ y en el oraculo): 15 dwords por evento (tipo + 14 valores)
inline u32 HashStep(u32 h, u32 v) { return Mix((h + v) * 0x01000193u ^ 0xA5A5A5A5u); }

inline P MakeP(u32 n) {
    P c;
    c.family = n % 16;
    const u32 f = c.family;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto Pc = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    // --- area de la region de (x, y), mundo
    const u32 regionArea = R(64);
    std::vector<u32> areas{regionArea};
    const u32 extra = R(3);
    for (u32 i = 0; i < extra; ++i) {
        const u32 a = Rand(n, 5 + i) % 64;
        if (std::find(areas.begin(), areas.end(), a) == areas.end() && areas.size() < kMaxSlots) areas.push_back(a);
    }
    if (Pc(f == 12 ? 25 : 6)) areas.erase(areas.begin());   // la region de (x, y) no esta cargada
    if (areas.empty()) areas.push_back((regionArea + 1) % 64);
    const int rx = int(regionArea % 8), ry = int(regionArea / 8);
    const float fx = Fadd(float(-3000 + 750 * rx), Fmul(float(Rand(n, 4) % 750000u) * 0.001f, 1.0f)), fy = Fadd(float(-3000 + 750 * ry), Fmul(float(Rand(n, 6) % 750000u) * 0.001f, 1.0f));
    c.x = B(fx); c.y = B(fy);
    c.w.nAreas = u32(areas.size());
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        c.w.area[s] = areas[s];
        c.w.nVeh[s] = Rand(n, 300 + s) % 4; c.w.nPed[s] = 1 + Rand(n, 310 + s) % 8;
        if (Rand(n, 320 + s) % 12 == 0 || (f == 12 && Rand(n, 321 + s) % 3 == 0)) c.w.nPed[s] = 0;
        const u32 nl = 28 + Rand(n, 330 + s) % 40;
        for (u32 i = 0; i < nl; ++i) c.w.links[s].push_back(0);   // se rellenan abajo
        for (u32 i = 0; i < kInterBytes / 4; ++i) c.w.inter[s].push_back(0);
    }
    const int cx = int(Fmul(fx, 8.0f)), cy = int(Fmul(fy, 8.0f));
    const u32 crossPct = f == 7 ? 60 : 22, offPct = f == 6 ? 55 : 18;
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        for (u32 i = 0; i < c.w.Total(s); ++i) {
            const u32 k = 400 + s * 100 + i * 17;
            const u32 pm = Rand(n, k) % 10;
            const int span = pm < 7 ? 240 : pm < 9 ? 420 : 760;   // 8 unidades de mundo = 64 en 1/8
            int x = cx + int(Rand(n, k + 1) % u32(2 * span + 1)) - span, y = cy + int(Rand(n, k + 2) % u32(2 * span + 1)) - span;
            x = std::max(-32768, std::min(32767, x)); y = std::max(-32768, std::min(32767, y));
            const u32 zm = Rand(n, k + 3) % 10;
            int z = zm < 7 ? int(Rand(n, k + 4) % 4000u) - 800 : zm == 7 ? 0 : zm == 8 ? int(Rand(n, k + 4) & 0x7FFFu) : -int(Rand(n, k + 4) % 32768u);
            const u32 nlk = Rand(n, k + 6) % 100 < 10 ? 0u : 1 + Rand(n, k + 7) % 5;
            int base = int(Rand(n, k + 8) % u32(c.w.links[s].size() - nlk + 1));
            if (f == 8 && Rand(n, k + 14) % 3 == 0) base = -int(Rand(n, k + 15) % 8u);   // base de enlaces negativa (lee la guarda de la arena)
            u32 b18 = (Rand(n, k + 9) & 0xD0u) | nlk;
            if (Rand(n, k + 16) % 100 < offPct) b18 |= 0x20u;
            const u32 tm = Rand(n, k + 17) % 10;
            const u32 traffic = f == 11 ? Rand(n, k + 18) % 16u : (tm < 7 ? 8u + Rand(n, k + 18) % 8u : Rand(n, k + 18) % 16u);
            const u32 b1a = (Rand(n, k + 19) & 0xF0u) | traffic;
            AddNode(c.w.nodes[s], Rand(n, k + 10), Rand(n, k + 11), x, y, z, 0, base, c.w.area[s], i, 0, 0, b18 | ((Rand(n, k + 12) & 0xFFu) << 8) | (b1a << 16) | ((Rand(n, k + 13) & 0xFFu) << 24));
        }
    }
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        const u32 invalid = f == 8 ? 40 : 18;
        for (u32 i = 0; i < c.w.links[s].size(); ++i) {
            const u32 t = Rand(n, 900 + s * 200 + i) % 100;
            const u32 ts = Rand(n, 1100 + s * 200 + i) % c.w.nAreas;
            u32& l = c.w.links[s][i];
            if (t >= invalid && c.w.Total(ts)) l = c.w.area[ts] | ((Rand(n, 1300 + s * 200 + i) % c.w.Total(ts)) << 16);
            else if (t < invalid / 3 + 1) l = (64u + Rand(n, 1500 + i) % 8) | ((Rand(n, 1501 + i) & 0xFFFFu) << 16);   // area >= 0x40
            else if (t < invalid) { u32 a = (Rand(n, 1600 + i) + s) % 64; for (u32 q = 0; q < 64 && c.w.SlotOf(a) >= 0; ++q) a = (a + 1) % 64; l = a | ((Rand(n, 1601 + i) & 0xFFFFu) << 16); }   // area sin cargar
            else l = 0xFFFFu | ((Rand(n, 1700 + i) & 0xFFFFu) << 16);
        }
        for (u32 i = 0; i < kInterBytes; ++i) {
            u32 byte = Rand(n, 2400 + s * 300 + i) & 0xFFu;
            byte = Rand(n, 2800 + s * 300 + i) % 100 < crossPct ? (byte | 1u) : (byte & ~1u);
            c.w.inter[s][i / 4] |= byte << (8 * (i % 4));
        }
    }
    // --- parametros
    auto Win = [&](float lo, float span) { return B(Span(n, nk(), lo, span)); };
    c.minD1 = Win(0.0f, 15.0f); c.maxD1 = B(Fadd(F(c.minD1), Span(n, nk(), 15.0f, 90.0f)));
    c.minD2 = Win(0.0f, 30.0f); c.maxD2 = B(Fadd(F(c.minD2), Span(n, nk(), 15.0f, 130.0f)));
    c.lowTraffic = (Rand(n, nk()) & ~0xFFu) | (Pc(f == 6 ? 50 : 30) ? 1u + R(255) : 0u);
    c.matrixMode = Pc(35) ? 1u : 0u;
    c.randMode = R(8); c.randSeed = Rand(n, nk());
    c.camMode = R(8); c.camSeed = Rand(n, nk());
    for (u32 k = 0; k < kGroundN; ++k) {
        const u32 fm = Rand(n, 30 + k) % 10;
        c.groundFound[k] = fm < 7 ? 1u : fm == 7 ? 0u : fm == 8 ? 0x80u : 0xFFu;
        static constexpr u32 d[]{0xBF800000u, 0x3F800000u, 0xBFC00000u, 0x3F000000u, 0x00000000u, 0xC0000000u, 0x40000000u, 0xC0800000u, 0xC0A00000u, 0xBE800000u};
        const u32 dm = Rand(n, 50 + k) % 10;
        c.groundDelta[k] = dm < 7 ? d[Rand(n, 70 + k) % std::size(d)] : B(Span(n, 90 + k, -6.0f, 12.0f));
    }
    for (u32 i = 0; i < 3; ++i) c.outCoordsInit[i] = Rand(n, 120 + i);
    c.outOriInit = Rand(n, 125); c.outAddrInit[0] = Rand(n, 126); c.outAddrInit[1] = Rand(n, 127);
    c.eax = Rand(n, 150); c.edx = Rand(n, 151); c.stack = StackSeed(n);
    // --- por familia
    switch (f) {
    case 0:                                                                  // tipico: no visible, ventanas amplias, suelo casi siempre
        c.matrixMode = 0; c.camMode = 0; c.randMode = 4 + R(2); c.lowTraffic = Pc(50) ? c.lowTraffic : (c.lowTraffic & ~0xFFu);
        for (u32 k = 0; k < kGroundN; ++k) { c.groundFound[k] = 1; c.groundDelta[k] = B(Span(n, 90 + k, -3.5f, 5.0f)); }
        break;
    case 1:                                                                  // matriz no nula y visibilidad mixta
        c.matrixMode = 1; c.camMode = 3 + R(2) * 3; c.randMode = 4;
        for (u32 k = 0; k < kGroundN; ++k) { c.groundFound[k] = 1; c.groundDelta[k] = B(Span(n, 90 + k, -3.5f, 5.0f)); }
        break;
    case 2:                                                                  // siempre visible: ventana 1 (minD1 < dist < maxD1)
        c.camMode = 1; c.randMode = 4;
        if (Pc(50)) c.minD1 = B(Span(n, nk(), 20.0f, 80.0f));
        if (Pc(30)) c.maxD1 = B(Span(n, nk(), 0.0f, 30.0f));
        break;
    case 3:                                                                  // nunca visible: ventana 2 y paridad de rand()
        c.camMode = 0; c.randMode = Pc(50) ? 3 : 4 + R(4);
        if (Pc(40)) c.minD2 = B(Span(n, nk(), 20.0f, 60.0f));
        if (Pc(30)) c.maxD2 = B(Span(n, nk(), 0.0f, 40.0f));
        break;
    case 4: {                                                                // borde del suelo: |suelo - z| contra 3.0
        static constexpr u32 ed[]{0x3F800000u, 0x3F7FFFFFu, 0x3F800001u, 0xC0A00000u, 0xC09FFFFFu, 0xC0A00001u, 0x7FC00123u, 0x7F800000u, 0xFF800000u, 0x40400000u, 0xC0400000u, 0x00000000u};
        c.camMode = Pc(50) ? 0u : 1u; c.randMode = 4 + R(2);
        for (u32 k = 0; k < kGroundN; ++k) { c.groundFound[k] = 1; c.groundDelta[k] = Pc(70) ? ed[R(u32(std::size(ed)))] : B(Span(n, 90 + k, -6.0f, 12.0f)); }
        break;
    }
    case 5:                                                                  // sin suelo nunca: agota las 300 iteraciones
        c.camMode = R(8); c.randMode = R(8);
        for (u32 k = 0; k < kGroundN; ++k) c.groundFound[k] = Pc(85) ? 0u : c.groundFound[k] & 0xFFu;
        break;
    case 6: case 7: case 8: break;                                            // trafico bajo / cruces de calle / enlaces invalidos (arriba)
    case 9: {                                                                // valores especiales de los argumentos
        const u32 which = R(8);
        auto Sp = [&](u32 slot) { const u32 q = R(10); return q < 3 ? QNan(slot) : q < 5 ? InfBits(n, nk()) : q < 7 ? HugeFloat(n, nk()) : q < 9 ? TinyFloat(n, nk()) : 0u; };
        u32* args[6]{&c.x, &c.y, &c.minD1, &c.maxD1, &c.minD2, &c.maxD2};
        *args[which % 6] = Sp(which);
        if (Pc(25)) *args[R(6)] = Sp(which + 1);
        c.randMode = 4; c.camMode = R(8);
        break;
    }
    case 10: {                                                               // bordes exactos de las ventanas con rand() = 0 (t = 0: la posicion es la del primer nodo)
        c.randMode = 0; c.camMode = 1; c.matrixMode = Pc(50) ? 1u : 0u;
        const u32 s = 0;
        if (c.w.SlotOf(regionArea) == 0 && c.w.nPed[0] > 0 && c.w.nVeh[0] < c.w.Total(0)) {
            const u32 idx = c.w.nVeh[0];
            u32* nw = &c.w.nodes[0][idx * 7];
            nw[6] = (nw[6] & 0xFFFFFF00u) | ((nw[6] & 0xD0u) | 3u);              // 3 enlaces, sin bit 0x20
            nw[6] = (nw[6] & 0xFFF0FFFFu) | (0x0Fu << 16);                          // trafico 15
            c.w.nodes[0][6] = (c.w.nodes[0][6] & 0xFFF0FF00u) | (c.w.nodes[0][6] & 0xD0u) | (0x0Fu << 16) | (c.w.nodes[0][6] & 0x0Fu);   // el nodo 0 (destino): trafico 15, sin 0x20
            const int base = i16(nw[4] & 0xFFFFu);
            for (u32 li = 0; li < 3; ++li) {
                const int at = base + int(li);
                if (at >= 0 && at < int(c.w.links[s].size())) { c.w.links[s][u32(at)] = c.w.area[0] | (0u << 16); c.w.inter[s][u32(at) / 4] &= ~(0xFFu << (8 * (u32(at) % 4))); }
            }
            const float nx = float(i16(nw[2] & 0xFFFFu)) * 0.125f, ny = float(i16(nw[2] >> 16)) * 0.125f;
            const float dy = Fsub(ny, F(c.y)), dx = Fsub(nx, F(c.x));
            const float dist = std::sqrt(Fadd(Fmul(dy, dy), Fmul(dx, dx)));
            const u32 db = B(dist), side = R(4);
            c.minD1 = side == 0 ? db : side == 1 ? Ulp(db, 1) : side == 2 ? Ulp(db, -1) : B(0.0f);
            c.maxD1 = side == 0 ? B(Fadd(dist, 5.0f)) : side == 3 ? Ulp(db, 1) : B(Fadd(dist, 50.0f));
            if (side == 3) c.minD1 = B(0.0f);
            if (Pc(30)) c.maxD1 = Ulp(db, int(R(3)) - 1), c.minD1 = B(0.0f);
            for (u32 k = 0; k < kGroundN; ++k) { c.groundFound[k] = 1; c.groundDelta[k] = B(Span(n, 90 + k, -3.5f, 5.0f)); }
        }
        break;
    }
    case 11:                                                                 // umbrales de trafico: guiones de rand() altos y bajos
        c.randMode = R(8); c.camMode = R(8);
        break;
    case 12:                                                                 // sin nodos de peaton / area sin cargar (arriba) y pocas rand
        c.randMode = R(8);
        break;
    default:                                                                 // mezcla
        break;
    }
    if (c.matrixMode == 0 && f == 1) c.matrixMode = 1;
    return c;
}

inline std::string JsonP(const P& c) {
    Js j;
    j.U("family", c.family);
    JsonWorld(j, c.w);
    j.U("x", c.x).U("y", c.y).U("min1", c.minD1).U("max1", c.maxD1).U("min2", c.minD2).U("max2", c.maxD2).U("low_traffic", c.lowTraffic).U("matrix_mode", c.matrixMode)
     .U("rand_mode", c.randMode).U("rand_seed", c.randSeed).U("cam_mode", c.camMode).U("cam_seed", c.camSeed)
     .A("ground_found", std::vector<u32>(c.groundFound, c.groundFound + kGroundN)).A("ground_delta", std::vector<u32>(c.groundDelta, c.groundDelta + kGroundN))
     .A("out_init", {c.outCoordsInit[0], c.outCoordsInit[1], c.outCoordsInit[2]}).U("out_ori_init", c.outOriInit).A("out_addr_init", {c.outAddrInit[0], c.outAddrInit[1]})
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}  // namespace Fn86CFix
