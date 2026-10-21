#pragma once
// Fixtures del banco CarCtrlRest63D (lote 63, CCarCtrl parte C, tanda 4): 4096 casos por funcion. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / Cases.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::ReconsiderRoute                      0x42FC40  cdecl (CVehicle*), void
//   1 CCarCtrl::FindLinksToGoWithTheseNodes          0x42B470  cdecl (CVehicle*), void
//
// Todos los floats viajan como patrones de 32 bits sin signo. Politica de especiales: a lo sumo UN campo NaN con carga util propia por caso.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Cc63dFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

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

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en el espejo)
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

// ------------------------------------------------------------------------------------------------ floats
constexpr u32 NanBits(u32 slot, bool signaling) {
    const u32 sign = (slot & 1u) << 31;
    return signaling ? (sign | 0x7F800000u | (0x2000u + slot * 0x41u)) : (sign | 0x7FC00000u | (0x100u + slot * 0x37u));
}
inline constexpr std::array<u32, 20> kFloatEdges{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3DCCCCCDu, 0xBDCCCCCDu, 0x3D4CCCCDu, 0x00000001u, 0x80000001u, 0x007FFFFFu,
    0x807FFFFFu, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F000000u, 0x40000000u, 0x3E000000u, 0x45000000u, 0x00400000u, 0x80400000u};
inline u32 OrdinaryFloat(u32 n, u32 k, int minExp, int maxExp) {   // exponentes sesgados (127 = 1.0)
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = u32(minExp) + Rand(n, k + 1) % u32(maxExp - minExp + 1);
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 TinyFloat(u32 n, u32 k) {   // denormal o casi
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = Rand(n, k + 1) % 3 == 0 ? Rand(n, k + 3) % 6u : 0u;
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 HugeFloat(u32 n, u32 k) {
    const u32 sign = Rand(n, k) & 1u;
    return (sign << 31) | ((200u + Rand(n, k + 1) % 55u) << 23) | (Rand(n, k + 2) & 0x7FFFFFu);   // hasta 0x7F7FFFFF
}
inline u32 InfBits(u32 n, u32 k) { return Rand(n, k) & 1u ? 0xFF800000u : 0x7F800000u; }
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline bool IsInfBits(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }
inline float F(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// Valor de relleno para los registros de entrada (EAX/ECX/EDX)
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Resultado de bool de un colaborador: falso = 0 con basura en los bits altos, o AL != 0 con basura en los bits altos
inline u32 BoolRet(u32 n, u32 k, bool wantTrue) {
    static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
    if (!wantTrue) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
    return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
}


// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
// Vehiculo: ventana de 0x600 bytes con guardas de 0x40 a cada lado. 0x40 + 0x600 + 0x40 = 0x680.
constexpr u32 kVehWindow = 0x600, kVehArena = kGuard + kVehWindow + kGuard;
// Matriz falsa (adelante en +0x10/+0x14, posicion en +0x30): ventana de 0x40, guardas de 0x20. 0x20 + 0x40 + 0x20 = 0x80.
constexpr u32 kMatGuard = 0x20, kMatWindow = 0x40, kMatArena = kMatGuard + kMatWindow + kMatGuard;
// Nodos de ruta (0x1C bytes cada uno): 4 areas de 8 nodos con guarda de 1 nodo a cada lado. 0x1C + 4*8*0x1C (0x380) + 0x1C = 0x3B8.
constexpr u32 kNodeBytes = 0x1C, kNodesPerArea = 8, kNodeAreas = 4, kNodeGuard = kNodeBytes, kNodeArena = kNodeGuard + kNodeAreas * kNodesPerArea * kNodeBytes + kNodeGuard;
// Enlaces de nodo (dwords): 4 areas de 32 dwords con guarda de 8 dwords (0x20 bytes) a cada lado. 0x20 + 4*32*4 (0x200) + 0x20 = 0x240.
constexpr u32 kLinksPerArea = 32, kLinkGuard = 0x20, kLinkArena = kLinkGuard + kNodeAreas * kLinksPerArea * 4 + kLinkGuard;
constexpr u32 kPoolAreas = 72;   // NUM_TOTAL_PATH_NODE_AREAS (0x96F854 .. 0x96F974)
constexpr u32 kTargetArena = kVehArena;   // la entidad objetivo de PickNextNodeAccordingStrategy: mismo tamano que el vehiculo (solo se lee +0x14 y +4/+8)

// ------------------------------------------------------------------------------------------------ nodos de ruta sinteticos
struct NodeSet {
    u32 areaIds[kNodeAreas]{};          // numero de area del juego de cada ranura (distintos, < 72)
    u32 unloadedId{};                   // un numero de area distinto de los cargados (puntero nulo en el juego)
    i16 x[kNodeAreas * kNodesPerArea]{}, y[kNodeAreas * kNodesPerArea]{}, z[kNodeAreas * kNodesPerArea]{};
    i16 base[kNodeAreas * kNodesPerArea]{};
    u8 b18[kNodeAreas * kNodesPerArea]{}, b19[kNodeAreas * kNodesPerArea]{}, b1A[kNodeAreas * kNodesPerArea]{};
    u32 link[kNodeAreas][kLinksPerArea]{};
};
// mode: 0 coordenadas pequenas, 1 extremos del int16, 2 muchas iguales (empates), 3 mezcla; linkMode: 0 todos cargados, 1 algunos sin cargar, 2 todos sin cargar, 3 al propio nodo
inline void MakeNodes(u32 n, NodeSet& ns, u32 mode, u32 linkMode, u32 numLinksMax) {
    u32 perm[kPoolAreas];
    for (u32 i = 0; i < kPoolAreas; ++i) perm[i] = i;
    for (u32 i = 0; i < 8; ++i) { const u32 j = i + Rand(n, 10 + i) % (kPoolAreas - i); std::swap(perm[i], perm[j]); }
    for (u32 s = 0; s < kNodeAreas; ++s) ns.areaIds[s] = perm[s];
    ns.unloadedId = perm[5];
    for (u32 i = 0; i < kNodeAreas * kNodesPerArea; ++i) {
        auto coord = [&](u32 k) -> i16 {
            switch (mode) {
            case 0: return i16(i32(Rand(n, k) % 400u) - 200);
            case 1: { static constexpr i16 e[]{-32768, -32767, 32767, 32766, 0, 1, -1, 4095, -4096}; return e[Rand(n, k) % std::size(e)]; }
            case 2: return i16(i32(Rand(n, k) % 3u) * 8);
            default: return Rand(n, k) % 5 == 0 ? i16(Rand(n, k + 1)) : i16(i32(Rand(n, k + 1) % 4000u) - 2000);
            }
        };
        ns.x[i] = coord(100 + i * 4); ns.y[i] = coord(101 + i * 4); ns.z[i] = i16(Rand(n, 102 + i * 4));
        ns.base[i] = i16(Rand(n, 300 + i) % 17u);
        const u32 nl = Rand(n, 400 + i) % (numLinksMax + 1);
        ns.b18[i] = u8((Rand(n, 500 + i) & 0xF0u) | nl);
        ns.b19[i] = u8(Rand(n, 600 + i)); ns.b1A[i] = u8(Rand(n, 700 + i));
    }
    for (u32 a = 0; a < kNodeAreas; ++a) {
        for (u32 l = 0; l < kLinksPerArea; ++l) {
            u32 area = ns.areaIds[Rand(n, 800 + a * 64 + l) % kNodeAreas];
            if (linkMode == 1 && Rand(n, 900 + a * 64 + l) % 3 == 0) area = ns.unloadedId;
            if (linkMode == 2) area = ns.unloadedId;
            u32 node = Rand(n, 1000 + a * 64 + l) % kNodesPerArea;
            ns.link[a][l] = area | (node << 16);
        }
    }
}


// Enlaces de carretera (uint16): 4 areas de 64 con guarda de 0x20 bytes. 0x20 + 4*64*2 (0x200) + 0x20 = 0x240.
constexpr u32 kNaviPerArea = 64, kNaviGuard = 0x20, kNaviArena = kNaviGuard + kNodeAreas * kNaviPerArea * 2 + kNaviGuard;
constexpr u32 kDistList = 16, kNodesOut = 8;

// ================================================================================================ 0: ReconsiderRoute
struct D0 {
    u32 family{}, seed{}, prev{}, now{}, mission{}, first{}, second{}, cur{}, start{}, ending{}, counter{}, playerCoords[3]{}, dest[3]{}, targetMat{}, targetPos[3]{}, decoy[2]{};
    u32 flags3DB{}, model{}, matMode{}, pos[2]{}, vdecoy[2]{}, dist{}, count1{}, count2{}, nodes[kNodesOut]{}, junk{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline D0 MakeD0(u32 n) {
    D0 c; const u32 f = n % 16; c.family = f;
    c.seed = Rand(n, 1) & 0xFFFFu;
    // tiempos: misma ranura de 2000 ms (sale) o distinta; cerca de limites y del desbordamiento de 32 bits
    c.prev = Rand(n, 2) % 4 == 0 ? Rand(n, 3) : Rand(n, 3) % 200000u;
    static constexpr u32 deltas[]{0, 1, 5, 1999, 2000, 2001, 3999, 4000, 100000};
    c.now = c.prev + (Rand(n, 4) % 3 == 0 ? Rand(n, 5) % 10000u : deltas[Rand(n, 5) % std::size(deltas)]);
    if (f == 15) { c.prev = 0xFFFFFFFFu - Rand(n, 6) % 4000u; c.now = c.prev + Rand(n, 7) % 5000u; c.seed = 0xFFFFu - Rand(n, 8) % 100u; }
    if (f >= 4 && f != 15) {   // fuerza el cruce de una ranura de 2000 ms: now - prev >= 2000 con mismo semilla
        c.seed = Rand(n, 9) % 2000u;
        c.prev = 2000u * (Rand(n, 10) % 1000u) + (2000u - c.seed % 2000u) % 2000u - 1u + (Rand(n, 11) % 2);
        c.now = c.prev + 1u + Rand(n, 12) % 3u;
    }
    static constexpr u32 setS[]{2, 4, 8, 15, 17, 43, 52, 54, 61};
    c.mission = Rand(n, 13) % 8 == 0 ? (Rand(n, 14) & 0xFFu) : setS[Rand(n, 15) % std::size(setS)];
    if (f % 4 == 1) c.mission = Rand(n, 16) % 3 == 0 ? 2u : Rand(n, 16) % 3 == 1 ? 8u : 15u;
    // direcciones: first/second distintos de 0xFFFF salvo en un caso de cada diez; cur/start/end elegidos entre first, second y otras
    c.first = (Rand(n, 20) % 10 == 0 ? 0xFFFFu : (Rand(n, 21) % 72u)) | ((Rand(n, 22) % 8u) << 16);
    c.second = ((Rand(n, 23) % 72u)) | ((Rand(n, 24) % 8u) << 16);
    const u32 other1 = (Rand(n, 25) % 72u) | ((Rand(n, 26) % 8u) << 16), other2 = (Rand(n, 27) % 72u) | ((Rand(n, 28) % 8u) << 16);
    auto pick = [&](u32 k) -> u32 {
        switch (Rand(n, k) % 8) {
        case 0: case 1: case 2: return c.first;
        case 3: case 4: case 5: return c.second;
        case 6: return other1;
        default: return Rand(n, k + 1) % 5 == 0 ? other2 : (c.first & 0xFFFFu) | (other2 & 0xFFFF0000u);   // misma area, otro nodo
        }
    };
    c.cur = pick(30); c.start = pick(32); c.ending = pick(34);
    c.counter = Rand(n, 36) % 4 == 0 ? (Rand(n, 37) % 2 ? 255u : Rand(n, 37) & 0xFFu) : 3u + Rand(n, 38) % 3u;
    for (u32 i = 0; i < 3; ++i) {
        c.playerCoords[i] = Rand(n, 40 + i) % 12 == 0 ? NanBits(Rand(n, 43 + i) % 6, Rand(n, 46 + i) % 2) : OrdinaryFloat(n, 50 + 4 * i, 100, 150);
        c.dest[i] = Rand(n, 60 + i) % 12 == 0 ? kFloatEdges[Rand(n, 63 + i) % kFloatEdges.size()] : OrdinaryFloat(n, 70 + 4 * i, 100, 150);
        c.targetPos[i] = Rand(n, 80 + i) % 12 == 0 ? InfBits(n, 83 + i) : OrdinaryFloat(n, 90 + 4 * i, 100, 150);
    }
    c.targetMat = Rand(n, 100) % 2;
    c.decoy[0] = OrdinaryFloat(n, 101, 120, 135); c.decoy[1] = OrdinaryFloat(n, 105, 120, 135);
    c.flags3DB = Rand(n, 110) & 0xFFu;
    c.model = Rand(n, 111) % 4 == 0 ? 0x21Bu : Rand(n, 112) & 0xFFFFu;
    c.matMode = Rand(n, 113) % 2;
    c.pos[0] = OrdinaryFloat(n, 114, 120, 140); c.pos[1] = OrdinaryFloat(n, 118, 120, 140);
    c.vdecoy[0] = OrdinaryFloat(n, 122, 120, 135); c.vdecoy[1] = OrdinaryFloat(n, 126, 120, 135);
    static constexpr u32 dists[]{0u, 0x80000000u, 0x47AFC800u, 0x47AFC7FFu, 0x47AFC801u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00123u, 0x00000001u, 0x3F800000u, 0xC7AFC800u};
    c.dist = Rand(n, 130) % 3 == 0 ? dists[Rand(n, 131) % std::size(dists)] : (Rand(n, 131) % 2 ? OrdinaryFloat(n, 132, 120, 140) & 0x7FFFFFFFu : 0x47000000u + Rand(n, 133) % 0x100000u);
    static constexpr u32 counts[]{0u, 1u, 2u, 3u, 8u, 0xFFFFu, 0x8000u, 0x7FFFu, 0x1234FFFFu, 0xABCD0002u};
    c.count1 = Rand(n, 140) % 2 ? counts[Rand(n, 141) % std::size(counts)] : Rand(n, 141) % 9u;
    c.count2 = Rand(n, 142) % 2 ? counts[Rand(n, 143) % std::size(counts)] : Rand(n, 143) % 9u;
    for (u32 i = 0; i < kNodesOut; ++i) c.nodes[i] = Rand(n, 150 + i);
    c.junk = Rand(n, 160);
    if (f >= 8 && f <= 13) {   // familias "ruta profunda": cruce de ranura, mision valida, ninguna de las seis condiciones de salida, contador > 4, distancia corta y mas de un nodo
        c.seed = Rand(n, 9) % 2000u;
        c.prev = 2000u * (Rand(n, 10) % 1000u) + (2000u - c.seed % 2000u) % 2000u - 1u;
        c.now = c.prev + 1u + Rand(n, 12) % 3u;
        c.mission = setS[Rand(n, 15) % std::size(setS)];
        c.first = ((Rand(n, 21) % 72u)) | ((Rand(n, 22) % 8u) << 16);
        c.cur = (100u + Rand(n, 30) % 20u) | ((Rand(n, 31) % 8u) << 16);
        c.start = (120u + Rand(n, 32) % 20u) | ((Rand(n, 33) % 8u) << 16);
        c.ending = (140u + Rand(n, 34) % 20u) | ((Rand(n, 35) % 8u) << 16);
        c.counter = 4u + Rand(n, 36) % 3u;
        c.dist = OrdinaryFloat(n, 132, 120, 135) & 0x7FFFFFFFu;
        c.count1 = 2u + Rand(n, 141) % 7u;
    }
    c.eax = SeedReg(n, 170); c.ecx = SeedReg(n, 173); c.edx = SeedReg(n, 176); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonD0(const D0& c) {
    Js j; j.U("family", c.family).U("seed", c.seed).U("prev", c.prev).U("now", c.now).U("mission", c.mission).U("first", c.first).U("second", c.second).U("cur", c.cur).U("start", c.start).U("ending", c.ending)
     .U("counter", c.counter).A("player", {c.playerCoords[0], c.playerCoords[1], c.playerCoords[2]}).A("dest", {c.dest[0], c.dest[1], c.dest[2]}).U("target_mat", c.targetMat)
     .A("target_pos", {c.targetPos[0], c.targetPos[1], c.targetPos[2]}).A("decoy", {c.decoy[0], c.decoy[1]}).U("flags3db", c.flags3DB).U("model", c.model).U("mat_mode", c.matMode)
     .A("pos", {c.pos[0], c.pos[1]}).A("vdecoy", {c.vdecoy[0], c.vdecoy[1]}).U("dist", c.dist).U("count1", c.count1).U("count2", c.count2).A("nodes", std::vector<u32>(c.nodes, c.nodes + kNodesOut))
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 1: FindLinksToGoWithTheseNodes
struct D1 {
    u32 family{}, curSlot{}, curNode{}, startSel{}, startIdx{}, startSlot{}, startNode{}, seed{}, pos[3]{}, vdecoy[2]{}, matMode{}, dists[kDistList]{};
    u32 navi[kNodeAreas][kNaviPerArea]{};
    NodeSet ns{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline D1 MakeD1(u32 n) {
    D1 c; const u32 f = n % 16; c.family = f;
    const u32 linkMode = f == 6 ? 2u : f == 7 ? 3u : (Rand(n, 1) % 4 == 0 ? 1u : 0u);
    const u32 maxLinks = f < 4 ? 3u : f == 8 ? 15u : 8u;
    MakeNodes(n, c.ns, f == 9 ? 1u : f == 10 ? 2u : 0u, linkMode, maxLinks);
    c.curSlot = Rand(n, 2) % kNodeAreas; c.curNode = Rand(n, 3) % kNodesPerArea;
    const u32 idx = c.curSlot * kNodesPerArea + c.curNode;
    if (f == 0) c.ns.b18[idx] = u8((c.ns.b18[idx] & 0xF0u) | 1u);    // un solo enlace
    if (f == 1) c.ns.b18[idx] = u8((c.ns.b18[idx] & 0xF0u) | 0u);    // ninguno
    if (f == 2) c.ns.b18[idx] = u8((c.ns.b18[idx] & 0xF0u) | 2u);
    if (f == 8) c.ns.b18[idx] = u8((c.ns.b18[idx] & 0xF0u) | (12u + Rand(n, 4) % 4u));   // mas de 12 enlaces
    // siguiente nodo de la ruta: 0 = uno de los enlaces del nodo actual, 1 = aleatorio, 2 = el propio nodo, 3 = area del actual y nodo menor, 4 = area menor
    c.startSel = f % 5; c.startIdx = Rand(n, 5) % 14;
    c.startSlot = Rand(n, 6) % kNodeAreas; c.startNode = Rand(n, 7) % kNodesPerArea;
    c.seed = Rand(n, 8) % 3 == 0 ? 0u : Rand(n, 9) & 0xFFFFu;
    for (u32 i = 0; i < 3; ++i) c.pos[i] = OrdinaryFloat(n, 10 + 4 * i, 110, 150);
    c.vdecoy[0] = OrdinaryFloat(n, 30, 120, 135); c.vdecoy[1] = OrdinaryFloat(n, 34, 120, 135);
    c.matMode = Rand(n, 38) % 2;
    static constexpr u32 sp[]{0u, 0x80000000u, 0x7F800000u, 0x7FC00000u, 0xFFC00123u, 0x00000001u, 0x3F800000u};
    for (u32 i = 0; i < kDistList; ++i) {
        c.dists[i] = Rand(n, 40 + i) % 6 == 0 ? sp[Rand(n, 60 + i) % std::size(sp)] : (Rand(n, 80 + i) % 4 == 0 ? 0x41200000u : OrdinaryFloat(n, 100 + i, 120, 140) & 0x7FFFFFFFu);
    }
    for (u32 a = 0; a < kNodeAreas; ++a) for (u32 l = 0; l < kNaviPerArea; ++l) c.navi[a][l] = Rand(n, 200 + a * 64 + l) & 0xFFFFu;
    c.eax = SeedReg(n, 170); c.ecx = SeedReg(n, 173); c.edx = SeedReg(n, 176); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonD1(const D1& c) {
    std::vector<u32> links;
    for (u32 a = 0; a < kNodeAreas; ++a) for (u32 l = 0; l < kLinksPerArea; ++l) links.push_back(c.ns.link[a][l]);
    std::vector<u32> nodes;
    for (u32 i = 0; i < kNodeAreas * kNodesPerArea; ++i) {
        nodes.push_back(u32(u16(c.ns.x[i])) | (u32(u16(c.ns.y[i])) << 16));
        nodes.push_back(u32(u16(c.ns.z[i])) | (u32(u16(c.ns.base[i])) << 16));
        nodes.push_back(u32(c.ns.b18[i]));
    }
    Js j; j.U("family", c.family).U("cur_slot", c.curSlot).U("cur_node", c.curNode).U("start_sel", c.startSel).U("start_idx", c.startIdx).U("start_slot", c.startSlot).U("start_node", c.startNode)
     .U("seed", c.seed).A("pos", {c.pos[0], c.pos[1], c.pos[2]}).A("vdecoy", {c.vdecoy[0], c.vdecoy[1]}).U("mat_mode", c.matMode).A("dists", std::vector<u32>(c.dists, c.dists + kDistList))
     .A("area_ids", {c.ns.areaIds[0], c.ns.areaIds[1], c.ns.areaIds[2], c.ns.areaIds[3]}).U("unloaded_id", c.ns.unloadedId).A("nodes", nodes).A("links", links)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}
