#pragma once
// Fixtures del banco PathFind62 (lote 62): seis funciones de CPathFind, 4096 casos cada una. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   CPathFind::FindNodeClosestToCoors          0x44F460  thiscall, CNodeAddress por puntero oculto, `ret 2Ch` (11 argumentos)
//   CPathFind::FindNthNodeClosestToCoors       0x44F8C0  thiscall, CNodeAddress por puntero oculto, `ret 30h` (12 argumentos)
//   CPathFind::FindNextNodeWandering           0x451B70  thiscall, void, `ret 20h` (8 argumentos)
//   CPathFind::CompleteNewInterior             0x452270  thiscall, void, `ret 4`
//   CPathFind::FindParkingNodeInArea           0x4513F0  thiscall, CVector por puntero oculto, `ret 1Ch` (7 argumentos)
//   CPathFind::GeneratePedCreationCoors_Interior 0x44ECA0 thiscall, bool en AL, `ret 18h` (6 argumentos)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Politica de especiales: los floats que la funcion REENVIA por valor a un colaborador
// (posicion, distancia maxima) usan solo NaN silenciosos (el x87 convierte un NaN senalizador al cargarlo); un caso lleva a lo sumo un NaN con carga propia.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <iterator>
#include <string>
#include <vector>

namespace PathFind62Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
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
// Aritmetica del generador con un redondeo a float por operacion (identica en SSE y x87 de 24 bits, en el juego y en el espejo).
inline float Fadd(float a, float b) { volatile float r = a + b; return r; }
inline float Fsub(float a, float b) { volatile float r = a - b; return r; }
inline float Fmul(float a, float b) { volatile float r = a * b; return r; }

// ------------------------------------------------------------------------------------------------ floats
inline u32 QNan(u32 slot) { return ((slot & 1u) << 31) | 0x7FC00000u | (0x100u + slot * 0x37u); }
inline u32 OrdinaryFloat(u32 n, u32 k, int minExp, int maxExp) {   // exponentes sesgados (127 = 1.0)
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = u32(minExp) + Rand(n, k + 1) % u32(maxExp - minExp + 1);
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
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
// float en [lo, lo + span) con resolucion de 1/1000
inline float Span(u32 n, u32 k, float lo, float span) { return Fadd(lo, Fmul(float(Rand(n, k) % 1000000u) * 0.000001f, span)); }

// _ftol del EXE (0x821B40): 32 bits bajos del entero de 64 bits truncado; NaN, infinitos y |v| >= 2^63 dan el entero indefinido (bajos = 0)
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
// distancia minima de (x, y) a los cuatro bordes de su region, con las operaciones y el orden de 0x44F460
inline float EdgeDist(float x, float y, int rx, int ry) {
    float minD = Fsub(x, Fsub(Fmul(float(rx), 750.0f), 3000.0f));
    const float east = Fsub(Fsub(Fmul(float(rx + 1), 750.0f), 3000.0f), x);
    if (!(minD < east)) minD = east;
    const float south = Fsub(y, Fsub(Fmul(float(ry), 750.0f), 3000.0f));
    if (!(minD < south)) minD = south;
    const float north = Fsub(Fsub(Fmul(float(ry + 1), 750.0f), 3000.0f), y);
    if (!(minD < north)) minD = north;
    return minD;
}

// ------------------------------------------------------------------------------------------------ mundo falso de rutas
// Arenas (guarda | objeto | guarda), sumas a mano:
//   this (CPathFind) 0x40 + 0x3C80 + 0x40 = 0x3D00
//   nodos de un area (24 x 0x1C = 0x2A0)  0x40 + 0x2A0 + 0x40 = 0x320
//   enlaces de un area (160 x 4 = 0x280)  0x40 + 0x280 + 0x40 = 0x300
constexpr u32 kThisBytes = 0x3C80, kThisOff = 0x40, kThisArena = 0x3D00;
constexpr u32 kNodeBytes = 0x1C, kMaxNodes = 24, kNodesBytes = 0x2A0, kNodesOff = 0x40, kNodesArena = 0x320;
constexpr u32 kMaxLinks = 160, kLinksBytes = 0x280, kLinksOff = 0x40, kLinksArena = 0x300;
constexpr u32 kMaxSlots = 5;
// Arenas pequenas de parametros de salida (CNodeAddress, CVector, floats...): 0x40 + 0x10 + 0x40 = 0x90
constexpr u32 kSmallObj = 0x40, kSmallBytes = 0x10, kSmallArena = 0x90;
constexpr u32 kOffPathNodes = 0x804, kOffNodeLinks = 0xA44, kOffLinkLengths = 0xB64, kOffIntersections = 0xC84, kOffNumNodes = 0xFA4, kOffNumVeh = 0x10C4, kOffNumPed = 0x11E4,
              kOffNumCarLinks = 0x1304, kOffNumAddresses = 0x1424, kOffInteriorIds = 0x3548;
constexpr u32 kNumAreas = 72;

struct World {
    u32 nAreas{};
    u32 area[kMaxSlots]{}, nVeh[kMaxSlots]{}, nPed[kMaxSlots]{};
    std::vector<u32> nodes[kMaxSlots];   // 7 dwords por nodo (nVeh + nPed nodos)
    std::vector<u32> links[kMaxSlots];   // CNodeAddress como dword (area | nodo << 16)
    u32 Total(u32 s) const { return nVeh[s] + nPed[s]; }
    int SlotOf(u32 a) const { for (u32 s = 0; s < nAreas; ++s) if (area[s] == a) return int(s); return -1; }
};
// word 2: x | y << 16 (int16), word 3: z | totalDist << 16, word 4: baseLink | areaId << 16, word 5: nodeId | width << 16 | flood << 24, word 6: bytes +0x18..+0x1B
inline u32 PackXY(int x, int y) { return u32(u16(x)) | (u32(u16(y)) << 16); }
inline void AddNode(std::vector<u32>& v, u32 w0, u32 w1, int x, int y, int z, int total, int base, u32 areaId, u32 nodeId, u32 width, u32 flood, u32 flags) {
    v.push_back(w0); v.push_back(w1); v.push_back(PackXY(x, y)); v.push_back(u32(u16(z)) | (u32(u16(total)) << 16));
    v.push_back(u32(u16(base)) | (areaId << 16)); v.push_back((nodeId & 0xFFFFu) | ((width & 0xFFu) << 16) | ((flood & 0xFFu) << 24)); v.push_back(flags);
}
inline std::vector<u32> PickAreas(u32 n, u32 k, u32 count, u32 interiorPct) {
    std::vector<u32> out;
    for (u32 i = 0; out.size() < count && i < 64; ++i) {
        const u32 a = Rand(n, k + i) % 100 < interiorPct ? 64u + Rand(n, k + 100 + i) % 8u : Rand(n, k + 200 + i) % 64u;
        if (std::find(out.begin(), out.end(), a) == out.end()) out.push_back(a);
    }
    return out;
}
inline void JsonWorld(Js& j, const World& w) {
    std::vector<u32> a(w.area, w.area + w.nAreas), v(w.nVeh, w.nVeh + w.nAreas), p(w.nPed, w.nPed + w.nAreas);
    j.A("areas", a).A("nveh", v).A("nped", p);
    static const char* const nk[kMaxSlots]{"nodes0", "nodes1", "nodes2", "nodes3", "nodes4"};
    static const char* const lk[kMaxSlots]{"links0", "links1", "links2", "links3", "links4"};
    for (u32 s = 0; s < w.nAreas; ++s) { j.A(nk[s], w.nodes[s]); j.A(lk[s], w.links[s]); }
}

// ================================================================================================ 1: FindNodeClosestToCoors (0x44F460)
// Colaborador sustituido: la busqueda por region 0x44F2C0 (sin reconstruir). Guion: por area, un candidato (nodo, distancia); si distancia < *dist (orden estricto) escribe resultado y *dist.
constexpr u32 kScriptN = 8;
struct P1 {
    u32 family{}, pos[3]{}, nodeType{}, maxDist{}, unk2{}, unk3{}, unk4{}, boats{}, unk6{};
    u32 ns{}, sArea[kScriptN]{}, sNode[kScriptN]{}, sDist[kScriptN]{};
    u32 eax{}, edx{}, stack{};
};
inline u32 AxisCoord(u32 n, u32 k) {
    const u32 kind = Rand(n, k) % 24;
    if (kind < 12) return B(Fadd(float(-3000 + 750 * int(Rand(n, k + 1) % 8)), Fmul(float(Rand(n, k + 2) % 750000u) * 0.001f, 1.0f)));
    if (kind < 14) return B(float(-3000 + 750 * int(Rand(n, k + 1) % 9)));              // justo en un borde de region
    if (kind == 14) return Ulp(B(float(-3000 + 750 * int(Rand(n, k + 1) % 9))), Rand(n, k + 3) & 1u ? 1 : -1);
    if (kind == 15) return B(Span(n, k + 1, -3000.0f, 6000.0f));
    if (kind < 18) return B(Rand(n, k + 1) & 1u ? Fadd(3000.0f, float(Rand(n, k + 2) % 3000u)) : Fsub(-3000.0f, float(Rand(n, k + 2) % 3000u)));   // fuera del mapa
    if (kind == 18) return Rand(n, k + 1) % 2 ? B(float(Rand(n, k + 2) % 2 ? 1e13 : -1e13)) : (Rand(n, k + 2) % 2 ? 0x5E0E1B1Bu : 0xDE2F1ACCu);   // enormes: scaled > 2^32
    if (kind == 19) return Rand(n, k + 1) % 2 ? QNan(Rand(n, k + 2) % 5) : InfBits(n, k + 2);
    if (kind == 20) return TinyFloat(n, k + 1);
    if (kind == 21) return Rand(n, k + 1) % 2 ? 0u : 0x80000000u;
    return B(Span(n, k + 1, -3000.0f, 6000.0f));
}
inline P1 MakeP1(u32 n) {
    P1 c;
    c.family = n % 16;
    c.pos[0] = AxisCoord(n, 1); c.pos[1] = AxisCoord(n, 10); c.pos[2] = OrdinaryFloat(n, 20, 115, 135);
    const float fx = F(c.pos[0]), fy = F(c.pos[1]);
    const int rx = Region(fx), ry = Region(fy);
    const float minD = EdgeDist(fx, fy, rx, ry);
    auto thr = [&](u32 j) { float t = minD; for (u32 i = 0; i < j; ++i) t = Fadd(t, 750.0f); return B(t); };
    const u32 mdm = Rand(n, 30) % 14;
    switch (mdm) {
    case 0: case 1: case 2: case 3: c.maxDist = 0x497423FEu; break;
    case 4: c.maxDist = B(minD); break;
    case 5: c.maxDist = Ulp(B(minD), 1); break;
    case 6: c.maxDist = Ulp(B(minD), -1); break;
    case 7: c.maxDist = Ulp(thr(Rand(n, 31) % 5), Rand(n, 32) % 3 == 0 ? 1 : Rand(n, 32) % 3 == 1 ? -1 : 0); break;
    case 8: c.maxDist = B(Span(n, 33, 0.0f, 3000.0f)); break;
    case 9: { static constexpr u32 sp[]{0u, 0x80000000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x7F7FFFFFu, 0x3F800000u}; c.maxDist = Rand(n, 34) % 3 ? sp[Rand(n, 35) % std::size(sp)] : QNan(Rand(n, 36) % 5); break; }
    case 10: c.maxDist = OrdinaryFloat(n, 37, 100, 145); break;
    default: c.maxDist = thr(Rand(n, 38) % 5); break;
    }
    static constexpr u32 ntLow[]{0u, 1u, 2u, 0xFFu, 3u, 0x80u};
    c.nodeType = (Rand(n, 40) % 4 ? ntLow[Rand(n, 41) % std::size(ntLow)] : (Rand(n, 42) & 0xFFu)) | (Rand(n, 43) % 3 == 0 ? (Rand(n, 44) & 0xFFFFFF00u) : 0u);
    static constexpr u32 w16[]{0u, 1u, 2u, 0x80u, 0x100u, 0xFFFFu, 0x8000u, 0xFFu};
    c.unk2 = Rand(n, 45) % 3 ? w16[Rand(n, 46) % 2] : w16[Rand(n, 47) % std::size(w16)];
    c.unk3 = Rand(n, 48);
    c.unk4 = Rand(n, 49) % 3 ? w16[Rand(n, 50) % 2] : w16[Rand(n, 51) % std::size(w16)];
    c.boats = Rand(n, 52) % 3 ? w16[Rand(n, 53) % 2] : w16[Rand(n, 54) % std::size(w16)];
    c.unk6 = (Rand(n, 55) % 2 ? 0u : (Rand(n, 56) | 1u)) | (Rand(n, 57) % 2 ? 0u : (Rand(n, 58) & 0xFFFFFF00u));
    if (Rand(n, 59) % 6 == 0) c.unk6 = (c.unk6 & 0xFFFFFF00u);   // byte bajo cero con basura arriba
    c.ns = Rand(n, 60) % 3 == 0 ? Rand(n, 61) % 3 : 1 + Rand(n, 61) % (kScriptN - 1);
    for (u32 i = 0; i < c.ns; ++i) {
        const u32 w = Rand(n, 70 + i) % 10;
        int ax = rx, ay = ry;
        if (w >= 4 && w < 8) { ax += int(Rand(n, 80 + i) % 9) - 4; ay += int(Rand(n, 90 + i) % 9) - 4; }
        c.sArea[i] = w >= 8 ? 64u + Rand(n, 100 + i) % 8 : (ax >= 0 && ax < 8 && ay >= 0 && ay < 8) ? u32(ax + ay * 8) : u32(rx + ry * 8);
        c.sNode[i] = Rand(n, 110 + i) % 1000;
        const u32 dm = Rand(n, 120 + i) % 12;
        switch (dm) {
        case 0: c.sDist[i] = B(minD); break;
        case 1: c.sDist[i] = Ulp(B(minD), 1); break;
        case 2: c.sDist[i] = Ulp(B(minD), -1); break;
        case 3: case 4: c.sDist[i] = Ulp(thr(1 + Rand(n, 130 + i) % 4), Rand(n, 131 + i) % 3 == 0 ? 1 : Rand(n, 131 + i) % 3 == 1 ? -1 : 0); break;
        case 5: c.sDist[i] = B(Span(n, 132 + i, 0.0f, 2000.0f)); break;
        case 6: c.sDist[i] = Rand(n, 133 + i) % 2 ? c.maxDist : Ulp(c.maxDist, Rand(n, 134 + i) % 2 ? 1 : -1); break;
        case 7: { static constexpr u32 sp[]{0u, 0x80000000u, 0xBF800000u, 0x7F800000u, 0x00000001u, 0x7F7FFFFFu}; c.sDist[i] = Rand(n, 135 + i) % 4 ? sp[Rand(n, 136 + i) % std::size(sp)] : QNan(Rand(n, 137 + i) % 5); break; }
        case 8: c.sDist[i] = OrdinaryFloat(n, 138 + i, 100, 145); break;
        default: c.sDist[i] = B(Span(n, 139 + i, 0.0f, 4000.0f)); break;
        }
    }
    c.eax = Rand(n, 150); c.edx = Rand(n, 151); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonP1(const P1& c) {
    Js j;
    j.U("family", c.family).A("pos", {c.pos[0], c.pos[1], c.pos[2]}).U("node_type", c.nodeType).U("max_dist", c.maxDist).U("unk2", c.unk2).U("unk3", c.unk3).U("unk4", c.unk4)
     .U("boats", c.boats).U("unk6", c.unk6)
     .A("s_area", std::vector<u32>(c.sArea, c.sArea + c.ns)).A("s_node", std::vector<u32>(c.sNode, c.sNode + c.ns)).A("s_dist", std::vector<u32>(c.sDist, c.sDist + c.ns))
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ 2: FindNthNodeClosestToCoors (0x44F8C0)
// Colaborador sustituido: FindNodeClosestToCoors (0x44F460 y el cuerpo de la DLL). Guion: resultado k-esimo (area, nodo) o invalido.
struct P2 {
    u32 family{}; World w;
    u32 pos[3]{}, nodeType{}, maxDist{}, lowTraffic{}, unkn{}, nth{}, boats{}, ignore{}, hasOut{};
    u32 ns{}, rArea[kScriptN]{}, rNode[kScriptN]{};
    u32 eax{}, edx{}, stack{};
};
inline void RandomNodeFlags(u32 n, u32 k, std::vector<u32>& v, u32 area, u32 id, u32 numLinksMask) {
    (void)numLinksMask;
    const u32 f0 = Rand(n, k + 3) & 0xFFu, f1 = Rand(n, k + 4) & 0xFFu, f2 = Rand(n, k + 5) & 0xFFu, f3 = Rand(n, k + 6) & 0xFFu;
    AddNode(v, Rand(n, k), Rand(n, k + 1), int(Rand(n, k + 2) & 0xFFFFu) - 0x8000, int(Rand(n, k + 7) & 0xFFFFu) - 0x8000, int(Rand(n, k + 8) & 0xFFFFu) - 0x8000,
            int(Rand(n, k + 9) & 0xFFFFu), int(Rand(n, k + 10) & 0x7Fu), area, id, Rand(n, k + 11) & 0xFFu, Rand(n, k + 12) & 0xFFu, f0 | (f1 << 8) | (f2 << 16) | (f3 << 24));
}
inline P2 MakeP2(u32 n) {
    P2 c;
    c.family = n % 16;
    const std::vector<u32> areas = PickAreas(n, 1, 1 + Rand(n, 0) % 4, 30);
    c.w.nAreas = u32(areas.size());
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        c.w.area[s] = areas[s];
        c.w.nVeh[s] = Rand(n, 300 + s) % 6; c.w.nPed[s] = Rand(n, 310 + s) % 6;
        if (Rand(n, 320 + s) % 8 == 0) c.w.nVeh[s] = c.w.nPed[s] = 0;
        for (u32 i = 0; i < c.w.Total(s); ++i) RandomNodeFlags(n, 400 + s * 100 + i * 13, c.w.nodes[s], areas[s], i, 0);
    }
    for (u32 i = 0; i < 3; ++i) c.pos[i] = OrdinaryFloat(n, 20 + 3 * i, 110, 140);
    if (Rand(n, 24) % 6 == 0) c.pos[Rand(n, 25) % 3] = QNan(Rand(n, 26) % 5);
    static constexpr u32 ntLow[]{0u, 1u, 0u, 1u, 0u, 1u, 0u, 1u, 2u, 3u, 0xFFu, 0x80u};
    c.nodeType = ntLow[Rand(n, 27) % std::size(ntLow)] | (Rand(n, 28) % 4 == 0 ? (Rand(n, 29) & 0xFFFFFF00u) : 0u);
    c.maxDist = Rand(n, 30) % 3 ? 0x497423FEu : (Rand(n, 31) % 3 ? B(Span(n, 32, 0.0f, 1000.0f)) : QNan(Rand(n, 33) % 5));
    c.lowTraffic = Rand(n, 34) & 1u; c.unkn = Rand(n, 35) & 1u; c.boats = Rand(n, 36) & 1u; c.ignore = Rand(n, 37) & 1u;
    const u32 nm = Rand(n, 38) % 20;
    static constexpr i32 nthEdge[]{0, -1, -7, 7, 8, 0x7FFFFFFF, i32(0x80000000u), 6, 5};
    c.nth = nm < 8 ? 1u : nm < 12 ? 2u + Rand(n, 39) % 3 : nm < 17 ? u32(Rand(n, 39) % 6) : u32(nthEdge[Rand(n, 39) % std::size(nthEdge)]);
    c.hasOut = Rand(n, 40) & 1u;
    c.ns = 1 + Rand(n, 41) % kScriptN;
    for (u32 k = 0; k < c.ns; ++k) {
        bool valid = Rand(n, 50 + k) % 100 < (k == 0 ? 75u : 85u);
        std::vector<u32> withNodes;
        for (u32 s = 0; s < c.w.nAreas; ++s) if (c.w.Total(s)) withNodes.push_back(s);
        if (withNodes.empty()) valid = false;
        if (valid) { const u32 s = withNodes[Rand(n, 60 + k) % withNodes.size()]; c.rArea[k] = c.w.area[s]; c.rNode[k] = Rand(n, 70 + k) % c.w.Total(s); }
        else { c.rArea[k] = 0xFFFFu; c.rNode[k] = Rand(n, 80 + k) & 0xFFFFu; }
    }
    c.eax = Rand(n, 150); c.edx = Rand(n, 151); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonP2(const P2& c) {
    Js j;
    j.U("family", c.family);
    JsonWorld(j, c.w);
    j.A("pos", {c.pos[0], c.pos[1], c.pos[2]}).U("node_type", c.nodeType).U("max_dist", c.maxDist).U("low_traffic", c.lowTraffic).U("unkn", c.unkn).U("nth", c.nth)
     .U("boats", c.boats).U("ignore", c.ignore).U("has_out", c.hasOut)
     .A("r_area", std::vector<u32>(c.rArea, c.rArea + c.ns)).A("r_node", std::vector<u32>(c.rNode, c.rNode + c.ns))
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ 3: FindNextNodeWandering (0x451B70)
// Colaboradores: FindNodeClosestToCoors (sustituido) y CPathNode::GetPosition (0x420A10, codigo real en ambos lados).
struct P3 {
    u32 family{}; World w;
    u32 pos[3]{}, nodeType{}, dir{}, outDirInit{}, targetInit{};
    u32 originKind{};   // 0 puntero nulo, 1 area -1, 2 nodo valido cargado, 3 area valida sin cargar
    u32 origin{};       // dword del origen
    u32 resKind{}, result{};   // resultado de la busqueda (guion): dword completo
    u32 eax{}, edx{}, stack{};
};
inline u32 SlotAddr(const World& w, u32 s, u32 node) { return w.area[s] | (node << 16); }
inline P3 MakeP3(u32 n) {
    P3 c;
    c.family = n % 16;
    const std::vector<u32> areas = PickAreas(n, 1, 1 + Rand(n, 0) % 3, 25);
    c.w.nAreas = u32(areas.size());
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        c.w.area[s] = areas[s];
        const u32 cnt = 2 + Rand(n, 300 + s) % 7;   // 2..8 nodos
        c.w.nPed[s] = cnt; c.w.nVeh[s] = 0;
        const u32 nLinkEntries = 20 + Rand(n, 310 + s) % 60;
        for (u32 i = 0; i < nLinkEntries; ++i) {
            const u32 t = Rand(n, 320 + s * 100 + i) % 100;
            if (t < 80) { const u32 ts = Rand(n, 330 + s * 100 + i) % c.w.nAreas; c.w.links[s].push_back(c.w.area[ts] | (Rand(n, 340 + s * 100 + i) << 16)); }
            else if (t < 92) { u32 a = Rand(n, 350 + s * 100 + i) % 72; for (u32 q = 0; q < 72 && std::find(areas.begin(), areas.end(), a) != areas.end(); ++q) a = (a + 1) % 72; c.w.links[s].push_back(a | ((Rand(n, 360 + s * 100 + i) & 0xFFu) << 16)); }
            else c.w.links[s].push_back(c.w.area[s] | (Rand(n, 370 + s * 100 + i) << 16));
        }
    }
    // los enlaces hacia areas de la propia lista deben apuntar a nodos existentes: se corrigen al conocer todos los recuentos
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        for (u32& l : c.w.links[s]) {
            const int ts = c.w.SlotOf(l & 0xFFFFu);
            if (ts >= 0) l = (l & 0xFFFFu) | ((((l >> 16) % c.w.Total(u32(ts)))) << 16);
        }
    }
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        const u32 cnt = c.w.Total(s);
        const int baseX = int(Rand(n, 500 + s) % 600u) - 300, baseY = int(Rand(n, 510 + s) % 600u) - 300;
        for (u32 i = 0; i < cnt; ++i) {
            const u32 k = 600 + s * 100 + i * 11;
            const u32 numLinks = Rand(n, k) % 100 < 15 ? 0u : 1 + Rand(n, k + 1) % 6;
            int base = int(Rand(n, k + 2) % u32(c.w.links[s].size() - numLinks + 1));
            if (Rand(n, k + 3) % 20 == 0) base = 0;
            const u32 areaField = Rand(n, k + 4) % 12 == 0 ? c.w.area[Rand(n, k + 5) % c.w.nAreas] : c.w.area[s];
            // si el campo area apunta a otra area, la base debe caber en los enlaces de ESA area
            const int ts = c.w.SlotOf(areaField);
            if (u32(base) + numLinks > c.w.links[u32(ts)].size()) base = 0;
            int x, y;
            const u32 pm = Rand(n, k + 6) % 10;
            if (pm == 0 && i > 0) { x = int(static_cast<i16>(c.w.nodes[s][(i - 1) * 7 + 2] & 0xFFFFu)); y = int(static_cast<i16>(c.w.nodes[s][(i - 1) * 7 + 2] >> 16)); }   // coincidente con el anterior
            else { x = baseX + int(Rand(n, k + 7) % 400u) * (pm < 3 ? 1 : 5) - 200; y = baseY + int(Rand(n, k + 8) % 400u) * (pm < 3 ? 1 : 5) - 200; }
            const u32 widthPick = Rand(n, k + 9) % 8;
            const u32 width = widthPick < 3 ? Rand(n, k + 10) % 56u : widthPick < 5 ? 56u : widthPick == 5 ? 0xFFu : widthPick == 6 ? 57u : 55u;   // 56 = 7.0 de radio
            const u32 f0 = (Rand(n, k + 11) & 0xF0u) | numLinks;
            const u32 f1 = Rand(n, k + 12) & 0xFFu, f2 = Rand(n, k + 13) & 0xFFu, f3 = Rand(n, k + 14) & 0xFFu;
            AddNode(c.w.nodes[s], Rand(n, k + 15), Rand(n, k + 16), x, y, int(Rand(n, k + 17) % 2000u) - 1000, int(Rand(n, k + 18) & 0xFFFFu), base, areaField, i, width, Rand(n, k + 19) & 0xFFu, f0 | (f1 << 8) | (f2 << 16) | (f3 << 24));
        }
    }
    // origen
    const u32 ok = Rand(n, 20) % 20;
    const u32 os = Rand(n, 21) % c.w.nAreas, on = Rand(n, 22) % c.w.Total(os);
    c.originKind = ok < 1 ? 0u : ok < 3 ? 1u : ok < 5 ? 3u : 2u;
    switch (c.originKind) {
    case 0: c.origin = 0; break;
    case 1: c.origin = 0xFFFFu | ((Rand(n, 23) & 0xFFFFu) << 16); break;
    case 2: c.origin = SlotAddr(c.w, os, on); break;
    default: { u32 a = 0; for (u32 t = 0; t < 72; ++t) { a = (Rand(n, 24) + t) % 72; if (c.w.SlotOf(a) < 0) break; } c.origin = a | ((Rand(n, 25) & 0xFFFFu) << 16); break; }
    }
    // posicion: cerca del nodo origen (o de uno cualquiera) con distancias alrededor del radio max(7, anchura/8)
    const u32 refS = c.originKind == 2 ? os : Rand(n, 26) % c.w.nAreas, refN = c.originKind == 2 ? on : Rand(n, 27) % c.w.Total(refS);
    const u32* nw = &c.w.nodes[refS][refN * 7];
    const float nx = Fmul(float(static_cast<i16>(nw[2] & 0xFFFFu)), 0.125f), ny = Fmul(float(static_cast<i16>(nw[2] >> 16)), 0.125f), nz = Fmul(float(static_cast<i16>(nw[3] & 0xFFFFu)), 0.125f);
    const float radius = std::max(7.0f, Fmul(float((nw[5] >> 16) & 0xFFu), 0.125f));
    float dist = 0.0f;
    switch (Rand(n, 28) % 9) {
    case 0: dist = 0.0f; break;
    case 1: dist = radius; break;
    case 2: dist = F(Ulp(B(radius), 1)); break;
    case 3: dist = F(Ulp(B(radius), -1)); break;
    case 4: dist = Fmul(radius, 0.5f); break;
    case 5: dist = Fmul(radius, 3.0f); break;
    case 6: dist = float(Rand(n, 29) % 20000u) * 0.01f; break;
    case 7: dist = Fmul(radius, 100.0f); break;
    default: dist = Fmul(radius, 1.0000001f); break;
    }
    const u32 axis = Rand(n, 30) % 4;   // 0: solo x, 1: solo y, 2: solo z, 3: diagonal
    float dx = axis == 0 || axis == 3 ? dist : 0.0f, dy = axis == 1 || axis == 3 ? dist : 0.0f, dz = axis == 2 ? dist : 0.0f;
    if (axis == 3) { dx = Fmul(dist, 0.6f); dy = Fmul(dist, 0.8f); }
    if (Rand(n, 31) & 1u) dx = -dx;
    c.pos[0] = B(Fadd(nx, dx)); c.pos[1] = B(Fadd(ny, dy)); c.pos[2] = B(Fadd(nz, dz));
    if (Rand(n, 32) % 14 == 0) c.pos[Rand(n, 33) % 3] = QNan(Rand(n, 34) % 5);
    if (Rand(n, 35) % 14 == 0) c.pos[Rand(n, 36) % 3] = InfBits(n, 37);
    if (Rand(n, 38) % 10 == 0) c.pos[0] = B(Span(n, 39, -4000.0f, 8000.0f)), c.pos[1] = B(Span(n, 40, -4000.0f, 8000.0f));
    static constexpr u32 ntLow[]{0u, 1u, 1u, 0u, 2u, 0xFFu};
    c.nodeType = ntLow[Rand(n, 41) % std::size(ntLow)] | (Rand(n, 42) % 4 == 0 ? (Rand(n, 43) & 0xFFFFFF00u) : 0u);
    c.dir = (Rand(n, 44) % 2 ? (n & 0xFFu) : (Rand(n, 45) & 0xFFu)) | (Rand(n, 54) % 4 == 0 ? (Rand(n, 55) & 0xFFFFFF00u) : 0u);   // el EXE hace movzx del byte: la basura de los bytes altos no cuenta
    c.outDirInit = Rand(n, 46) & 0xFFu; c.targetInit = Rand(n, 47);
    // resultado de la busqueda (solo se usa si la funcion busca): nodo valido, invalido o area sin cargar
    const u32 rk = Rand(n, 48) % 10;
    c.resKind = rk < 6 ? 0u : rk < 8 ? 1u : 2u;
    if (c.originKind == 0 && c.resKind == 0) c.resKind = 1;   // el EXE escribe en *origen aunque sea nulo: un nodo valido con origen nulo no se prueba
    if (c.resKind == 0) { const u32 s = Rand(n, 49) % c.w.nAreas; c.result = SlotAddr(c.w, s, Rand(n, 50) % c.w.Total(s)); }
    else if (c.resKind == 1) c.result = 0xFFFFu | ((Rand(n, 51) & 0xFFFFu) << 16);
    else { u32 a = 0; for (u32 t = 0; t < 72; ++t) { a = (Rand(n, 52) + t) % 72; if (c.w.SlotOf(a) < 0) break; } c.result = a | ((Rand(n, 53) & 0xFFFFu) << 16); }
    c.eax = Rand(n, 150); c.edx = Rand(n, 151); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonP3(const P3& c) {
    Js j;
    j.U("family", c.family);
    JsonWorld(j, c.w);
    j.A("pos", {c.pos[0], c.pos[1], c.pos[2]}).U("node_type", c.nodeType).U("dir", c.dir).U("out_dir_init", c.outDirInit).U("target_init", c.targetInit)
     .U("origin_kind", c.originKind).U("origin", c.origin).U("res_kind", c.resKind).U("result", c.result)
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ 4: CompleteNewInterior (0x452270)
// Globales (0x96EA98..0x96F050 y 0x977B70..0x977BA4) fijados por el caso y restaurados despues. Colaboradores sustituidos: CMemoryMgr::Malloc (0x72F420), 0x450FB0 (sin reconstruir)
// y AddDynamicLinkBetween2Nodes_For1Node (0x44E000). Arena de Malloc: 0x2800 bytes + guardas.
constexpr u32 kMallocBytes = 0x2800, kMallocOff = 0x40, kMallocArena = 0x2880;
constexpr u32 kGlobAAddr = 0x96EA98, kGlobABytes = 0x96F050 - 0x96EA98;   // 0x5B8
constexpr u32 kGlobBAddr = 0x977B70, kGlobBBytes = 0x30;
constexpr u32 kGAExtLinks = 0x96EAB8 - 0x96EA98, kGAConnects = 0x96EAC0 - 0x96EA98, kGADontWander = 0x96EC40 - 0x96EA98, kGAZ = 0x96EC80 - 0x96EA98, kGAY = 0x96ED80 - 0x96EA98,
              kGAX = 0x96EE80 - 0x96EA98, kGANumNodes = 0x96EF80 - 0x96EA98, kGASlot = 0x96EF84 - 0x96EA98, kGAInteriorId = 0x96EF88 - 0x96EA98, kGABuilding = 0x96F031 - 0x96EA98;
constexpr u32 kGBExtNodes = 0x977B7C - 0x977B70;
struct P4 {
    u32 family{}; World w;     // area exterior 0 (para la inundacion del primer nodo exterior)
    u32 numNodes{}, slot{}, numExt{}, interiorId{}, building{};
    u32 x[64]{}, y[64]{}, z[64]{};
    u8 connects[64][6]{}; u8 dontWander[64]{};
    u32 intLinked[8]{}, extLinked[8]{};
    u32 outKind{};   // 0 sin puntero, 1 con puntero
    u32 outInit{};
    u32 eax{}, edx{}, stack{}, fill{};
};
inline u32 CoordF(u32 n, u32 k) {
    const u32 t = Rand(n, k) % 100;
    if (t < 62) return B(Span(n, k + 1, -4000.0f, 8000.0f));
    if (t < 74) { static constexpr u32 e[]{0u, 0x80000000u, 0x3E000000u, 0xBE000000u, 0x3F000000u, 0x45800000u, 0xC5800000u, 0x457FFF00u, 0x46000000u, 0x3F800000u}; return e[Rand(n, k + 1) % std::size(e)]; }
    if (t < 82) return B(Fmul(float(int(Rand(n, k + 1) % 2000000u) - 1000000), 16.0f));
    if (t < 88) return OrdinaryFloat(n, k + 1, 150, 190);
    if (t < 93) return Rand(n, k + 1) % 2 ? QNan(Rand(n, k + 2) % 5) : InfBits(n, k + 2);
    if (t < 96) return TinyFloat(n, k + 1);
    return Rand(n, k + 1);
}
inline P4 MakeP4(u32 n) {
    P4 c;
    c.family = n % 16;
    c.w.nAreas = 1;
    c.w.area[0] = Rand(n, 1) % 64; c.w.nPed[0] = 1 + Rand(n, 2) % 4;
    for (u32 i = 0; i < c.w.Total(0); ++i) {
        std::vector<u32>& v = c.w.nodes[0];
        AddNode(v, Rand(n, 10 + i), Rand(n, 20 + i), int(Rand(n, 30 + i) & 0xFFFFu) - 0x8000, int(Rand(n, 40 + i) & 0xFFFFu) - 0x8000, 0, 0, 0, c.w.area[0], i, 0, Rand(n, 50 + i) & 0xFFu, Rand(n, 60 + i));
    }
    const u32 nm = Rand(n, 70) % 40;
    c.numNodes = nm == 0 ? 0u : nm < 5 ? 1u : nm < 32 ? 1 + Rand(n, 71) % 16 : nm < 38 ? 17 + Rand(n, 71) % 48 : 64u;
    c.slot = Rand(n, 72) % 8;
    {   const u32 e = Rand(n, 73) % 12; c.numExt = e < 4 ? 0u : e < 9 ? 1 + Rand(n, 74) % 3 : e == 9 ? 8u : e == 10 ? u32(-1) : 4u; }
    c.interiorId = Rand(n, 75) % 3 == 0 ? Rand(n, 76) : Rand(n, 76) % 50;
    c.building = Rand(n, 77) & 1u;
    for (u32 i = 0; i < 64; ++i) { c.x[i] = CoordF(n, 100 + i * 4); c.y[i] = CoordF(n, 400 + i * 4); c.z[i] = CoordF(n, 700 + i * 4); c.dontWander[i] = u8(Rand(n, 1000 + i) % 4 == 0 ? Rand(n, 1100 + i) : Rand(n, 1100 + i) & 1u); }
    const u32 density = Rand(n, 78) % 6;   // enlaces por fila
    for (u32 i = 0; i < 64; ++i) {
        for (u32 j = 0; j < 6; ++j) {
            const u32 r = Rand(n, 1200 + i * 6 + j);
            u8 v = 0xFF;
            if (j < density && r % 100 < 85) {
                const u32 lim = c.numNodes ? c.numNodes : 1;
                const u32 q = Rand(n, 1700 + i * 6 + j) % 100;
                v = u8(q < 80 ? Rand(n, 2200 + i * 6 + j) % lim : q < 92 ? Rand(n, 2200 + i * 6 + j) % 64 : q < 96 ? (Rand(n, 2200 + i * 6 + j) & 0x7Fu) | 0x80u : i);
            }
            c.connects[i][j] = v;
        }
    }
    for (u32 i = 0; i < 8; ++i) {
        c.intLinked[i] = Rand(n, 2800 + i) % 3 == 0 ? Rand(n, 2810 + i) : Rand(n, 2810 + i) % 64;
        c.extLinked[i] = Rand(n, 2820 + i) % 4 == 0 ? Rand(n, 2830 + i) : (c.w.area[0] | ((Rand(n, 2840 + i) % c.w.Total(0)) << 16));
    }
    c.extLinked[0] = c.w.area[0] | ((Rand(n, 2850) % c.w.Total(0)) << 16);   // el primero se lee de verdad (inundacion): nodo exterior valido
    for (u32 i = c.numNodes; i < 64; ++i) { c.x[i] = c.y[i] = c.z[i] = 0; c.dontWander[i] = 0; }   // el EXE solo lee los primeros numNodes: el resto del arreglo se carga a cero
    c.outKind = Rand(n, 2860) % 4 != 0; c.outInit = Rand(n, 2861);
    c.eax = Rand(n, 2870); c.edx = Rand(n, 2871); c.stack = StackSeed(n); c.fill = Rand(n, 2872);
    return c;
}
inline std::string JsonP4(const P4& c) {
    Js j;
    j.U("family", c.family);
    JsonWorld(j, c.w);
    j.U("num_nodes", c.numNodes).U("slot", c.slot).U("num_ext", c.numExt).U("interior_id", c.interiorId).U("building", c.building);
    std::vector<u32> v;
    v.assign(c.x, c.x + c.numNodes); j.A("x", v);
    v.assign(c.y, c.y + c.numNodes); j.A("y", v);
    v.assign(c.z, c.z + c.numNodes); j.A("z", v);
    v.clear(); for (u32 i = 0; i < 64; ++i) for (u32 k = 0; k < 6; ++k) v.push_back(c.connects[i][k]);
    j.A("connects", v);
    v.assign(c.dontWander, c.dontWander + c.numNodes); j.A("dont_wander", v);
    v.assign(c.intLinked, c.intLinked + 8); j.A("int_linked", v);
    v.assign(c.extLinked, c.extLinked + 8); j.A("ext_linked", v);
    j.U("out_kind", c.outKind).U("out_init", c.outInit).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack).U("fill", c.fill);
    return j.End();
}

// ================================================================================================ 5: FindParkingNodeInArea (0x4513F0)
struct P5 {
    u32 family{}; World w;
    u32 box[6]{};   // minX, maxX, minY, maxY, minZ, maxZ (bits)
    u32 cursor{};
    u32 eax{}, edx{}, stack{};
};
inline P5 MakeP5(u32 n) {
    P5 c;
    c.family = n % 16;
    const std::vector<u32> areas = PickAreas(n, 1, 1 + Rand(n, 0) % 4, 15);
    c.w.nAreas = u32(areas.size());
    int bx = int(Rand(n, 2) % 2000u) - 1000, by = int(Rand(n, 3) % 2000u) - 1000, bz = int(Rand(n, 4) % 4000u) - 2000;   // centro (en unidades de 1/8)
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        c.w.area[s] = areas[s];
        c.w.nVeh[s] = Rand(n, 300 + s) % 9; c.w.nPed[s] = Rand(n, 310 + s) % 3;
        if (Rand(n, 320 + s) % 8 == 0) c.w.nVeh[s] = 0;
        for (u32 i = 0; i < c.w.Total(s); ++i) {
            const u32 k = 400 + s * 100 + i * 9;
            const u32 pm = Rand(n, k) % 10;
            int x = bx + int(Rand(n, k + 1) % 80u) - 40, y = by + int(Rand(n, k + 2) % 80u) - 40, z = bz + int(Rand(n, k + 3) % 80u) - 40;
            if (pm == 0) x = int(Rand(n, k + 4) & 0xFFFFu) - 0x8000;   // fuera
            const u32 typ = Rand(n, k + 5) % 10;
            const u32 f2 = (typ < 6 ? 0x20u : typ == 6 ? 0x10u : typ == 7 ? 0x30u : typ == 8 ? 0x00u : 0x2Fu) | (Rand(n, k + 6) & 0x0Fu);
            AddNode(c.w.nodes[s], Rand(n, k + 7), Rand(n, k + 8), x, y, z, 0, 0, areas[s], i, 0, 0, (Rand(n, k + 6) & 0xFFu) | ((Rand(n, k + 9) & 0xFFu) << 8) | (f2 << 16) | (Rand(n, k + 10) & 0xFF000000u));
        }
    }
    // caja: casi siempre alrededor del centro; bordes exactamente en valores de nodos (cota estricta)
    auto bound = [&](u32 k, int center, int half, bool upper) -> u32 {
        const u32 m = Rand(n, k) % 40;
        float v = Fmul(float(center + (upper ? half : -half)), 0.125f);
        if (m == 0) return QNan(Rand(n, k + 1) % 5);
        if (m == 1) return InfBits(n, k + 1);
        if (m == 2 && c.w.nAreas && c.w.Total(0)) { const u32* nw = &c.w.nodes[0][(Rand(n, k + 2) % c.w.Total(0)) * 7]; v = Fmul(float(static_cast<i16>(k % 3 == 0 ? nw[2] & 0xFFFFu : k % 3 == 1 ? nw[2] >> 16 : nw[3] & 0xFFFFu)), 0.125f); return B(v); }
        if (m == 3) return B(Fadd(v, upper ? 0.125f : -0.125f));
        if (m == 4) return B(Fadd(v, upper ? -0.125f : 0.125f));
        if (m == 5) return B(Fadd(v, upper ? 1000.0f : -1000.0f));
        return B(v);
    };
    const int half = 50 + int(Rand(n, 5) % 60);
    c.box[0] = bound(10, bx, half, false); c.box[1] = bound(11, bx, half, true);
    c.box[2] = bound(12, by, half, false); c.box[3] = bound(13, by, half, true);
    c.box[4] = bound(14, bz, half, false); c.box[5] = bound(15, bz, half, true);
    if (Rand(n, 6) % 12 == 0) std::swap(c.box[0], c.box[1]);
    static constexpr u32 cur[]{0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 100u, 4u, 5u};
    c.cursor = Rand(n, 7) % 3 ? Rand(n, 8) % 6 : cur[Rand(n, 9) % std::size(cur)];
    c.eax = Rand(n, 150); c.edx = Rand(n, 151); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonP5(const P5& c) {
    Js j;
    j.U("family", c.family);
    JsonWorld(j, c.w);
    j.A("box", {c.box[0], c.box[1], c.box[2], c.box[3], c.box[4], c.box[5]}).U("cursor", c.cursor).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ 6: GeneratePedCreationCoors_Interior (0x44ECA0)
// Colaboradores sustituidos: rand() del CRT del EXE (0x821B1E, guion) y CWorld::FindGroundZFor3DCoord (0x5696C0, guion). FindRegionForCoors (0x44D830) es codigo real.
constexpr u32 kGroundN = 12;
struct P6 {
    u32 family{}; World w;
    u32 x{}, y{};
    u32 randMode{}, randSeed{};
    u32 groundFound[kGroundN]{}, groundDelta[kGroundN]{};
    u32 outCoordsInit[3]{}, outOriInit{};
    u32 eax{}, edx{}, stack{};
};
inline P6 MakeP6(u32 n) {
    P6 c;
    c.family = n % 16;
    // area de la region de (x, y): primera del mundo; coordenadas dentro de la region (o la propia region por recorte)
    const u32 regionArea = Rand(n, 1) % 64;
    std::vector<u32> areas{regionArea};
    for (u32 a : PickAreas(n, 5, Rand(n, 2) % 4, 0)) if (std::find(areas.begin(), areas.end(), a) == areas.end() && areas.size() < kMaxSlots) areas.push_back(a);
    if (Rand(n, 3) % 10 == 0) areas.erase(areas.begin());   // la region de (x, y) no esta cargada
    if (areas.empty()) areas.push_back((regionArea + 1) % 64);
    const int rx = int(regionArea % 8), ry = int(regionArea / 8);
    const float fx = Fadd(float(-3000 + 750 * rx), Fmul(float(Rand(n, 4) % 750000u) * 0.001f, 1.0f)), fy = Fadd(float(-3000 + 750 * ry), Fmul(float(Rand(n, 6) % 750000u) * 0.001f, 1.0f));
    c.x = B(fx); c.y = B(fy);
    c.w.nAreas = u32(areas.size());
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        c.w.area[s] = areas[s];
        c.w.nVeh[s] = Rand(n, 300 + s) % 4; c.w.nPed[s] = Rand(n, 310 + s) % 7;
        if (Rand(n, 320 + s) % 10 == 0) c.w.nPed[s] = 0;
        const u32 nl = 30 + Rand(n, 330 + s) % 40;
        for (u32 i = 0; i < nl; ++i) c.w.links[s].push_back(0);   // se rellenan abajo
    }
    const int cx = int(Fmul(fx, 8.0f)), cy = int(Fmul(fy, 8.0f));
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        for (u32 i = 0; i < c.w.Total(s); ++i) {
            const u32 k = 400 + s * 100 + i * 17;
            const u32 pm = Rand(n, k) % 10;
            const int span = pm < 6 ? 300 : pm < 8 ? 440 : pm == 8 ? 399 : 401;   // 400 = 50 unidades en 1/8
            int x = cx + int(Rand(n, k + 1) % u32(2 * span + 1)) - span, y = cy + int(Rand(n, k + 2) % u32(2 * span + 1)) - span;
            x = std::max(-32768, std::min(32767, x)); y = std::max(-32768, std::min(32767, y));
            const u32 zm = Rand(n, k + 3) % 10;
            int z = zm < 6 ? 7300 + int(Rand(n, k + 4) % 20000u) : zm == 6 ? 7200 : zm == 7 ? 7201 : zm == 8 ? 7199 : int(Rand(n, k + 4) & 0x7FFFu);
            if (zm == 9 && Rand(n, k + 5) & 1u) z = -int(Rand(n, k + 4) % 30000u);
            const u32 nlk = Rand(n, k + 6) % 100 < 12 ? 0u : 1 + Rand(n, k + 7) % 6;
            const int base = int(Rand(n, k + 8) % u32(c.w.links[s].size() - nlk + 1));
            const u32 f0 = (Rand(n, k + 9) & 0xF0u) | nlk;
            AddNode(c.w.nodes[s], Rand(n, k + 10), Rand(n, k + 11), x, y, z, 0, base, c.w.area[s], i, 0, 0, f0 | ((Rand(n, k + 12) & 0xFFu) << 8) | ((Rand(n, k + 13) & 0xFFFFu) << 16));
        }
    }
    for (u32 s = 0; s < c.w.nAreas; ++s) {
        for (u32& l : c.w.links[s]) {
            const u32 t = Rand(n, 900 + s * 200 + u32(&l - c.w.links[s].data())) % 100;
            const u32 ts = Rand(n, 1100 + s * 200 + u32(&l - c.w.links[s].data())) % c.w.nAreas;
            if (t < 70 && c.w.Total(ts)) l = c.w.area[ts] | ((Rand(n, 1300 + s * 200 + u32(&l - c.w.links[s].data())) % c.w.Total(ts)) << 16);
            else if (t < 80) l = (64u + Rand(n, 1500 + u32(&l - c.w.links[s].data())) % 8) | ((Rand(n, 1501) & 0xFFFFu) << 16);   // area >= 0x40
            else if (t < 92) { u32 a = (Rand(n, 1600 + u32(&l - c.w.links[s].data())) + s) % 64; for (u32 q = 0; q < 64 && c.w.SlotOf(a) >= 0; ++q) a = (a + 1) % 64; l = a | ((Rand(n, 1601) & 0xFFFFu) << 16); }   // area sin cargar
            else l = 0xFFFFu | ((Rand(n, 1700 + u32(&l - c.w.links[s].data())) & 0xFFFFu) << 16);
        }
    }
    c.randMode = Rand(n, 20) % 8; c.randSeed = Rand(n, 21);
    for (u32 k = 0; k < kGroundN; ++k) {
        const u32 fm = Rand(n, 30 + k) % 10;
        c.groundFound[k] = fm < 6 ? 1u : fm == 6 ? 0u : fm == 7 ? 2u : fm == 8 ? 0x80u : 0xFFu;
        if (fm == 6) c.groundFound[k] = 0;
        static constexpr u32 d[]{0xC0000000u, 0xBF800000u, 0xC0400000u, 0xC0A00000u, 0x3F800000u, 0x3F7FFFFFu, 0xC0A00001u, 0xC09FFFFFu, 0x00000000u, 0x7F7FFFFFu, 0x7FC00123u, 0x7F800000u, 0xFF800000u, 0xC1200000u, 0x40000000u, 0xC0000001u};
        const u32 dm = Rand(n, 50 + k) % 10;
        c.groundDelta[k] = dm < 5 ? d[Rand(n, 70 + k) % 6] : dm < 8 ? d[Rand(n, 70 + k) % std::size(d)] : B(Span(n, 90 + k, 0.0f, 6.0f));
    }
    if (Rand(n, 110) % 5 == 0) for (u32 k = 0; k < kGroundN; ++k) { c.groundFound[k] = 0; }   // nunca hay suelo: agota los 200 intentos
    for (u32 i = 0; i < 3; ++i) c.outCoordsInit[i] = Rand(n, 120 + i);
    c.outOriInit = Rand(n, 125);
    c.eax = Rand(n, 150); c.edx = Rand(n, 151); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonP6(const P6& c) {
    Js j;
    j.U("family", c.family);
    JsonWorld(j, c.w);
    j.U("x", c.x).U("y", c.y).U("rand_mode", c.randMode).U("rand_seed", c.randSeed)
     .A("ground_found", std::vector<u32>(c.groundFound, c.groundFound + kGroundN)).A("ground_delta", std::vector<u32>(c.groundDelta, c.groundDelta + kGroundN))
     .A("out_init", {c.outCoordsInit[0], c.outCoordsInit[1], c.outCoordsInit[2]}).U("out_ori_init", c.outOriInit)
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
// valor k-esimo de rand() del guion (0..32767)
inline u32 ScriptRand(u32 mode, u32 seed, u32 k) {
    switch (mode) {
    case 0: return 0;
    case 1: return 0x7FFF;
    case 2: return (k * 64u) & 0x7FFFu;
    case 3: return (Rand(seed, k) & 1u) ? 0x7FFFu : 0u;
    default: return Rand(seed, k) & 0x7FFFu;
    }
}
// hash acumulado de eventos (identico en C++ y en el oraculo): 15 dwords por evento (tipo + 14 valores)
inline u32 HashStep(u32 h, u32 v) { return Mix((h + v) * 0x01000193u ^ 0xA5A5A5A5u); }
}  // namespace PathFind62Fix
