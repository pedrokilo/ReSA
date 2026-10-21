#pragma once
// Fixtures del banco CarCtrlRest63C (lote 63, CCarCtrl parte C, tanda 2): 4096 casos por funcion. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / Cases.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::SetUpDriverAndPassengersForVehicle        0x4217C0  cdecl (CVehicle*, int, int, bool, bool, int), void
//   1 CCarCtrl::SetCoordsOfScriptCar                      0x4342A0  cdecl (CVehicle*, float, float, float, uint8, uint8), void
//   2 CCarCtrl::ScanForPedDanger                          0x42CE40  cdecl (CVehicle*), void
//   3 CCarCtrl::FindMaximumSpeedForThisCarInTraffic       0x434400  cdecl (CVehicle*), float en ST0
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

namespace Cc63cFix {
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

// ================================================================================================ 0: PickNextNodeAccordingStrategy
struct C0 {
    u32 family{}, mission{}, slot{}, node{}, field41{}, targetMat{}, targetPos[3]{}, targetDecoy[2]{}, coords[3][3]{}, playerVeh{}, retChase{}, retFollow{}, retRandom{};
    NodeSet ns{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C0 MakeC0(u32 n) {
    C0 c; const u32 f = n % 16; c.family = f;
    static constexpr u32 setA[]{2, 4, 43};
    static constexpr u32 setB[]{15, 17, 52, 54, 61, 65, 66, 67, 68};
    static constexpr u32 setC[]{8, 12};
    static constexpr u32 edges[]{0, 1, 3, 5, 69, 70, 71, 127, 128, 129, 130, 190, 254, 255};
    switch (f) {
    case 0: case 7: c.mission = setA[Rand(n, 1) % 3]; break;
    case 1: case 8: c.mission = setB[Rand(n, 1) % 9]; break;
    case 2: case 9: c.mission = setC[Rand(n, 1) % 2]; break;
    case 3: case 10: { u32 m; u32 t = 0; do { m = 2 + Rand(n, 1 + t++) % 67u; } while (m == 2 || m == 4 || m == 43 || m == 8 || m == 12 || m == 15 || m == 17 || m == 52 || m == 54 || m == 61 || m >= 65); c.mission = m; break; }
    case 4: case 11: c.mission = edges[Rand(n, 1) % std::size(edges)]; break;
    case 5: c.mission = Rand(n, 1) & 0xFFu; break;
    case 6: c.mission = 128u + Rand(n, 1) % 128u; break;
    default: c.mission = Rand(n, 1) % 3 == 0 ? (Rand(n, 2) & 0xFFu) : (Rand(n, 3) % 3 == 0 ? setA[Rand(n, 4) % 3] : Rand(n, 3) % 3 == 1 ? setB[Rand(n, 4) % 9] : setC[Rand(n, 4) % 2]); break;
    }
    MakeNodes(n, c.ns, 0, 0, 15);
    c.slot = Rand(n, 20) % kNodeAreas; c.node = Rand(n, 21) % kNodesPerArea;
    // byte +0x1A >> 4 en 8..9 es el caso especial: se fuerza en parte de los casos
    const u32 idx = c.slot * kNodesPerArea + c.node;
    if (f % 4 == 1) c.ns.b1A[idx] = u8((8u + Rand(n, 22) % 2u) << 4 | (Rand(n, 23) & 0xFu));
    if (f % 4 == 2) c.ns.b1A[idx] = u8(((Rand(n, 22) % 2 ? 7u : 10u) << 4) | (Rand(n, 23) & 0xFu));
    c.field41 = Rand(n, 24) & 0xFFu;
    c.targetMat = Rand(n, 25) % 2;
    for (u32 i = 0; i < 3; ++i) c.targetPos[i] = Rand(n, 26 + i) % 6 == 0 ? kFloatEdges[Rand(n, 30 + i) % kFloatEdges.size()] : OrdinaryFloat(n, 34 + 4 * i, 100, 150);
    if (f == 15) c.targetPos[Rand(n, 40) % 3] = NanBits(Rand(n, 41) % 6, Rand(n, 42) % 3 == 0);
    c.targetDecoy[0] = OrdinaryFloat(n, 44, 120, 135); c.targetDecoy[1] = OrdinaryFloat(n, 48, 120, 135);
    for (u32 call = 0; call < 3; ++call)
        for (u32 i = 0; i < 3; ++i) c.coords[call][i] = Rand(n, 52 + call * 4 + i) % 8 == 0 ? NanBits(Rand(n, 70 + i) % 6, Rand(n, 71 + i) % 2 == 0) : OrdinaryFloat(n, 60 + call * 4 + i, 100, 150);
    c.playerVeh = Rand(n, 80) % 3 == 0 ? 0u : 0x00D00000u + (Rand(n, 81) & 0xFFFF0u);
    c.retChase = BoolRet(n, 82, Rand(n, 81) % 2); c.retFollow = BoolRet(n, 86, Rand(n, 87) % 2); c.retRandom = Rand(n, 90);
    c.eax = SeedReg(n, 95); c.ecx = SeedReg(n, 98); c.edx = SeedReg(n, 101); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC0(const C0& c) {
    Js j; j.U("family", c.family).U("mission", c.mission).U("slot", c.slot).U("node", c.node).U("field41", c.field41).U("target_mat", c.targetMat)
     .A("target_pos", {c.targetPos[0], c.targetPos[1], c.targetPos[2]}).A("target_decoy", {c.targetDecoy[0], c.targetDecoy[1]})
     .A("coords", {c.coords[0][0], c.coords[0][1], c.coords[0][2], c.coords[1][0], c.coords[1][1], c.coords[1][2], c.coords[2][0], c.coords[2][1], c.coords[2][2]})
     .U("player_veh", c.playerVeh).U("ret_chase", c.retChase).U("ret_follow", c.retFollow).U("ret_random", c.retRandom)
     .A("area_ids", {c.ns.areaIds[0], c.ns.areaIds[1], c.ns.areaIds[2], c.ns.areaIds[3]})
     .A("b19", {c.ns.b19[c.slot * kNodesPerArea + c.node]}).A("b1a", {c.ns.b1A[c.slot * kNodesPerArea + c.node]})
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 1: JoinCarWithRoadSystem
struct C1 {
    u32 family{}, startSel{}, startSlot{}, startNode{}, fwd[2]{}, pos[3]{}, junkSeed{};
    NodeSet ns{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C1 MakeC1(u32 n) {
    C1 c; const u32 f = n % 16; c.family = f;
    u32 mode = f % 4 == 1 ? 1u : f % 4 == 2 ? 2u : f % 4 == 3 ? 3u : 0u;
    u32 linkMode = f == 12 ? 1u : f == 6 ? 2u : f == 13 ? 3u : (Rand(n, 3) % 5 == 0 ? 1u : 0u);
    u32 maxLinks = f == 4 ? 15u : 4u;
    MakeNodes(n, c.ns, mode, linkMode, maxLinks);
    // seleccion del nodo inicial: 0..3 = ranura cargada, 4 = area sin cargar, 5 = 0xFFFF
    c.startSel = f == 2 ? 5u : f == 3 ? 4u : (Rand(n, 20) % 25 == 0 ? 4u + Rand(n, 21) % 2 : 0u);
    c.startSlot = Rand(n, 22) % kNodeAreas; c.startNode = Rand(n, 23) % kNodesPerArea;
    const u32 idx = c.startSlot * kNodesPerArea + c.startNode;
    if (f == 1 || f == 0) c.ns.b18[idx] = u8((c.ns.b18[idx] & 0xF0u) | 0u);                              // sin enlaces
    if (f == 5) c.ns.b18[idx] = u8((c.ns.b18[idx] & 0xF0u) | (2u + Rand(n, 24) % 4u));
    if (f == 13) {   // un enlace al propio nodo (distancia 0): el enlace 0 apunta al nodo inicial
        c.ns.b18[idx] = u8((c.ns.b18[idx] & 0xF0u) | 3u);
        c.ns.base[idx] = 0;
        c.ns.link[c.startSlot][0] = c.ns.areaIds[c.startSlot] | (c.startNode << 16);
    }
    auto ordf = [&](u32 k, int lo, int hi) { return OrdinaryFloat(n, k, u32(lo), u32(hi)); };
    c.fwd[0] = ordf(30, 120, 130); c.fwd[1] = ordf(34, 120, 130);
    switch (f) {
    case 7: c.fwd[0] = Rand(n, 40) % 2 ? 0u : 0x80000000u; c.fwd[1] = Rand(n, 41) % 2 ? 0u : 0x80000000u; break;
    case 8: c.fwd[Rand(n, 40) % 2] = NanBits(Rand(n, 41) % 6, Rand(n, 42) % 3 == 0); break;
    case 9: c.fwd[0] = Rand(n, 40) % 2 ? InfBits(n, 41) : TinyFloat(n, 42); c.fwd[1] = Rand(n, 43) % 2 ? 0u : TinyFloat(n, 44); break;
    case 10: c.fwd[0] = Rand(n, 40) % 2 ? 0u : ordf(45, 120, 130); c.fwd[1] = Rand(n, 41) % 2 ? 0u : ordf(46, 120, 130); break;   // un componente cero
    case 11: c.fwd[0] = kFloatEdges[Rand(n, 40) % kFloatEdges.size()]; c.fwd[1] = kFloatEdges[Rand(n, 41) % kFloatEdges.size()]; break;
    default: break;
    }
    for (u32 i = 0; i < 3; ++i) c.pos[i] = ordf(50 + 4 * i, 120, 150);
    c.junkSeed = Rand(n, 60);
    c.eax = SeedReg(n, 95); c.ecx = SeedReg(n, 98); c.edx = SeedReg(n, 101); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC1(const C1& c) {
    std::vector<u32> nodes;
    for (u32 i = 0; i < kNodeAreas * kNodesPerArea; ++i) {
        nodes.push_back(u32(u16(c.ns.x[i])) | (u32(u16(c.ns.y[i])) << 16));
        nodes.push_back(u32(u16(c.ns.base[i])) | (u32(c.ns.b18[i]) << 16) | (u32(c.ns.b19[i]) << 24));
    }
    std::vector<u32> links;
    for (u32 a = 0; a < kNodeAreas; ++a) for (u32 l = 0; l < kLinksPerArea; ++l) links.push_back(c.ns.link[a][l]);
    Js j; j.U("family", c.family).U("start_sel", c.startSel).U("start_slot", c.startSlot).U("start_node", c.startNode).A("fwd", {c.fwd[0], c.fwd[1]}).A("pos", {c.pos[0], c.pos[1], c.pos[2]})
     .A("area_ids", {c.ns.areaIds[0], c.ns.areaIds[1], c.ns.areaIds[2], c.ns.areaIds[3]}).U("unloaded_id", c.ns.unloadedId).A("nodes", nodes).A("links", links)
     .U("junk_seed", c.junkSeed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 2: JoinCarWithRoadSystemGotoCoors
struct C2 {
    u32 family{}, posn[3]{}, alias{}, model{}, flags3DB{}, matMode{}, pos[2]{}, decoy[2]{}, arg3{}, arg4{};
    u32 searchCount{}, nodes[8]{}, badCount{}, badNode0{}, badModify{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C2 MakeC2(u32 n) {
    C2 c; const u32 f = n % 16; c.family = f;
    auto ordf = [&](u32 k, int lo, int hi) { return OrdinaryFloat(n, k, u32(lo), u32(hi)); };
    for (u32 i = 0; i < 3; ++i) c.posn[i] = ordf(10 + 4 * i, 110, 150);
    if (f == 1) c.posn[Rand(n, 20) % 3] = NanBits(Rand(n, 21) % 6, Rand(n, 22) % 2 == 0);
    if (f == 2) c.posn[Rand(n, 20) % 3] = kFloatEdges[Rand(n, 21) % kFloatEdges.size()];
    if (f == 3) c.posn[Rand(n, 20) % 3] = InfBits(n, 21);
    c.alias = f == 4 ? 1u : 0u;      // posn apunta al propio destino del vehiculo (+0x3EC)
    c.model = f == 5 || f == 6 ? 0x21Bu : Rand(n, 23) % 5 == 0 ? 0x21Bu : Rand(n, 24) & 0xFFFFu;
    if (f == 7) c.model = 0x21Au + Rand(n, 25) % 3u;
    c.flags3DB = Rand(n, 26) & 0xFFu;
    c.matMode = Rand(n, 27) % 2;
    c.pos[0] = ordf(30, 100, 150); c.pos[1] = ordf(34, 100, 150); c.decoy[0] = ordf(38, 120, 135); c.decoy[1] = ordf(42, 120, 135);
    static constexpr u32 junk[]{0u, 1u, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80000000u};
    c.arg3 = Rand(n, 46) % 3 == 0 ? junk[Rand(n, 47) % std::size(junk)] : Rand(n, 47) % 2;
    c.arg4 = Rand(n, 48) % 3 == 0 ? junk[Rand(n, 49) % std::size(junk)] : Rand(n, 49) % 2;
    // resultado del buscador de rutas y de RemoveBadStartNode: contadores con signo de 16 bits
    static constexpr u32 counts[]{0u, 1u, 2u, 3u, 7u, 8u, 0xFFFFu, 0x8000u, 0x7FFFu, 0xFFFEu};
    c.searchCount = Rand(n, 50) % 2 ? counts[Rand(n, 51) % std::size(counts)] : Rand(n, 51) % 9u;
    for (u32 i = 0; i < 8; ++i) c.nodes[i] = Rand(n, 60 + i);
    c.badModify = Rand(n, 70) % 3 == 0 ? 1u : 0u;
    c.badCount = c.badModify ? (Rand(n, 71) % 2 ? counts[Rand(n, 72) % std::size(counts)] : Rand(n, 72) % 9u) : c.searchCount;
    c.badNode0 = Rand(n, 73);
    if (f >= 8 && f <= 11) { c.badModify = 1; c.badCount = f - 8 + (f == 11 ? 0u : 0u); }   // 0, 1, 2, 3 tras RemoveBadStartNode
    if (f == 12) { c.searchCount = 1; c.badModify = 0; c.badCount = 1; }
    if (f == 13) { c.searchCount = 2; c.badModify = 0; c.badCount = 2; }
    if (f == 14) { c.searchCount = 8; c.badModify = 1; c.badCount = 0xFFFFu; }
    c.eax = SeedReg(n, 95); c.ecx = SeedReg(n, 98); c.edx = SeedReg(n, 101); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC2(const C2& c) {
    Js j; j.U("family", c.family).A("posn", {c.posn[0], c.posn[1], c.posn[2]}).U("alias", c.alias).U("model", c.model).U("flags3db", c.flags3DB).U("mat_mode", c.matMode)
     .A("pos", {c.pos[0], c.pos[1]}).A("decoy", {c.decoy[0], c.decoy[1]}).U("arg3", c.arg3).U("arg4", c.arg4).U("search_count", c.searchCount)
     .A("nodes", std::vector<u32>(c.nodes, c.nodes + 8)).U("bad_count", c.badCount).U("bad_node0", c.badNode0).U("bad_modify", c.badModify)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}
