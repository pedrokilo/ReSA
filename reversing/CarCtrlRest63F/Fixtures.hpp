#pragma once
// Fixtures del banco CarCtrlRest63F (lote 63, CCarCtrl parte C, tanda 6): 4096 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / Cases.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::IsThisAnAppropriateNode               0x42DAB0  cdecl (CVehicle*, CNodeAddress x3, bool), bool en AL
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

namespace Cc63fFix {
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



// Informacion de modelo falsa (byte +0x4D, puntero +0x14 a un modelo de colision con floats en +0x0C, +0x10 y +0x14), pedestre conductor falso (byte +0x484, dword +0x598)
constexpr u32 kModelInfoSlots = 20000;
constexpr u32 kInfoBytes = 0x80, kColBytes = 0x40, kDriverBytes = 0x600;

// ================================================================================================ 0: IsThisAnAppropriateNode
struct F0 {
    u32 family{}, n1[2]{}, n2[2]{}, n3[2]{}, arg5{}, model{}, createdBy{}, mission{}, hasDriver{}, driver484{}, driver598{}, info4D{}, col[3]{};
    u32 right[3]{}, pos[3]{}, decoy[2]{}, lawRet{}, parkRet{}, count{};
    NodeSet ns{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline F0 MakeF0(u32 n) {
    F0 c; const u32 f = n % 16; c.family = f;
    MakeNodes(n, c.ns, f == 14 ? 1u : Rand(n, 1) % 3 == 0 ? 3u : 0u, 0u, 15u);
    // nodos: n1 y n3 de las ranuras cargadas (n3 a veces sin cargar o igual a n1); n2 siempre cargado (el EXE no lo comprueba)
    c.n1[0] = Rand(n, 2) % kNodeAreas; c.n1[1] = Rand(n, 3) % kNodesPerArea;
    c.n2[0] = Rand(n, 4) % kNodeAreas; c.n2[1] = Rand(n, 5) % kNodesPerArea;
    c.n3[0] = Rand(n, 6) % kNodeAreas; c.n3[1] = Rand(n, 7) % kNodesPerArea;
    if (f == 15) { c.n3[0] = c.n1[0]; c.n3[1] = c.n1[1]; }                                    // n1 == n3
    if (f == 13) { c.n3[0] = c.n1[0]; c.n3[1] = (c.n1[1] + 1) % kNodesPerArea; }              // misma area, otro nodo
    if (Rand(n, 8) % 40 == 0) c.n3[0] = 99u;                                                  // 99: area sin cargar (n3)
    const u32 i3 = c.n3[0] % kNodeAreas * kNodesPerArea + c.n3[1];
    u32 kind;
    switch (f) {
    case 0: case 8: kind = 1; break;
    case 1: case 9: kind = 2; break;
    case 2: kind = 5; break;
    case 3: kind = 10; break;
    case 4: kind = 8; break;
    case 5: kind = 9; break;
    case 6: { static constexpr u32 k[]{3, 4, 6, 7}; kind = k[Rand(n, 9) % 4]; break; }
    case 7: kind = 0; break;
    case 10: kind = 11 + Rand(n, 9) % 5u; break;
    default: kind = Rand(n, 9) % 16u; break;
    }
    if (c.n3[0] != 99u) c.ns.b1A[i3] = u8((kind << 4) | (Rand(n, 10) & 0xFu));
    c.arg5 = Rand(n, 11) % 4 == 0 ? (Rand(n, 12) % 2 ? 1u : Rand(n, 12) | 0x100u) : 0u;
    c.model = Rand(n, 13) % 6 == 0 ? 0x21Bu : Rand(n, 14) % kModelInfoSlots;
    c.createdBy = Rand(n, 15) % 5 == 0 ? 2u : (Rand(n, 16) & 0xFFu) == 2 ? 3u : Rand(n, 16) & 0xFFu;
    c.mission = Rand(n, 17) % 4 == 0 ? 1u : Rand(n, 18) & 0xFFu;
    c.hasDriver = Rand(n, 19) % 3 != 0;
    c.driver484 = Rand(n, 20) % 4 == 0 ? 2u : Rand(n, 21) & 0xFFu;
    c.driver598 = Rand(n, 22) % 4 == 0 ? 0x14u : Rand(n, 23);
    c.info4D = Rand(n, 24) % 5 == 0 ? 5u : Rand(n, 25) & 0xFFu;
    auto colf = [&](u32 k, float lo, float hi) -> u32 {
        switch (Rand(n, k) % 8) {
        case 0: return B(lo); case 1: return B(hi); case 2: return B(std::nextafter(hi, 1e30f)); case 3: return B(std::nextafter(lo, -1e30f));
        case 4: return NanBits(Rand(n, k + 1) % 6, Rand(n, k + 2) % 2 == 0);
        default: return B(lo + (hi - lo) * float(Rand(n, k + 3) % 1024u) / 1024.0f);
        }
    };
    c.col[0] = colf(30, 1.0f, 2.0f); c.col[1] = colf(34, 1.0f, 4.0f); c.col[2] = colf(38, 0.5f, 3.0f);
    // col[0] = +0x0C (umbral 2.0), col[1] = +0x10 (umbral 4.0), col[2] = +0x14 (umbrales 1.5 y 2.0)
    if (Rand(n, 40) % 3 == 0) c.col[2] = Rand(n, 41) % 2 ? B(1.5f) : B(2.0f);
    if (Rand(n, 42) % 3 == 0) c.col[0] = B(2.0f);
    if (Rand(n, 43) % 3 == 0) c.col[1] = B(4.0f);
    for (u32 i = 0; i < 3; ++i) {
        c.right[i] = Rand(n, 50 + i) % 12 == 0 ? (Rand(n, 53 + i) % 2 ? 0u : NanBits(Rand(n, 56 + i) % 6, Rand(n, 59 + i) % 2 == 0)) : OrdinaryFloat(n, 60 + 4 * i, 120, 130);
        c.pos[i] = OrdinaryFloat(n, 80 + 4 * i, 100, 140);
    }
    if (f == 11) { c.pos[Rand(n, 70) % 3] = Rand(n, 71) % 2 ? NanBits(Rand(n, 72) % 6, Rand(n, 73) % 2 == 0) : InfBits(n, 74); }
    if (f == 12) { for (u32 i = 0; i < 3; ++i) c.pos[i] = TinyFloat(n, 75 + i); }
    c.decoy[0] = OrdinaryFloat(n, 92, 120, 135); c.decoy[1] = OrdinaryFloat(n, 96, 120, 135);
    c.lawRet = BoolRet(n, 100, Rand(n, 103) % 4 == 0);
    c.parkRet = BoolRet(n, 110, Rand(n, 113) % 6 == 0);
    c.count = Rand(n, 120) % 2 ? 0u : (Rand(n, 121) % 3 == 0 ? Rand(n, 122) & 0xFFFFu : 1u + Rand(n, 123) % 2u);
    // familias "limpias" (tipos 1 y 2): la mayoria de los casos supera las comprobaciones previas y llega a IsLawEnforcementVehicle / IsAnyoneParking / FindObjectsKindaColliding
    if ((f == 0 || f == 1 || f == 8 || f == 9) && Rand(n, 200) % 4 != 0 && c.n3[0] != 99u) {
        c.createdBy = 0; c.driver484 = 0; c.driver598 = 0; c.info4D = 0;
        c.lawRet = BoolRet(n, 201, false); c.parkRet = BoolRet(n, 205, false);
        if (Rand(n, 209) % 2) c.arg5 = 1;
        c.ns.b18[c.n2[0] % kNodeAreas * kNodesPerArea + c.n2[1]] = u8(c.ns.b18[c.n3[0] % kNodeAreas * kNodesPerArea + c.n3[1]]);   // sin diferencia de agua entre los nodos 2 y 3
    }
    c.eax = SeedReg(n, 130); c.ecx = SeedReg(n, 133); c.edx = SeedReg(n, 136); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonF0(const F0& c) {
    std::vector<u32> nodes;
    for (u32 i = 0; i < kNodeAreas * kNodesPerArea; ++i) {
        nodes.push_back(u32(u16(c.ns.x[i])) | (u32(u16(c.ns.y[i])) << 16));
        nodes.push_back(u32(u16(c.ns.z[i])) | (u32(c.ns.b18[i]) << 16) | (u32(c.ns.b19[i]) << 24));
        nodes.push_back(u32(c.ns.b1A[i]));
    }
    Js j; j.U("family", c.family).A("n1", {c.n1[0], c.n1[1]}).A("n2", {c.n2[0], c.n2[1]}).A("n3", {c.n3[0], c.n3[1]}).U("arg5", c.arg5).U("model", c.model).U("created_by", c.createdBy).U("mission", c.mission)
     .U("has_driver", c.hasDriver).U("driver484", c.driver484).U("driver598", c.driver598).U("info4d", c.info4D).A("col", {c.col[0], c.col[1], c.col[2]}).A("right", {c.right[0], c.right[1], c.right[2]})
     .A("pos", {c.pos[0], c.pos[1], c.pos[2]}).A("decoy", {c.decoy[0], c.decoy[1]}).U("law_ret", c.lawRet).U("park_ret", c.parkRet).U("count", c.count)
     .A("area_ids", {c.ns.areaIds[0], c.ns.areaIds[1], c.ns.areaIds[2], c.ns.areaIds[3]}).A("nodes", nodes)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}
