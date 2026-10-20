#pragma once
// Fixtures del banco Misc61 (lote 61): cuatro funciones, 4096 casos cada una. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   CMemoryHeap::IntegrityCheck            0x72E8E0  thiscall, void
//   CSearchLight::IsSpottedEntity          0x493900  cdecl (uint32 index, const CEntity&), bool en AL
//   CTaskSimpleSlideToCoord::ProcessPed    0x66C4E0  thiscall (CPed*), `ret 4`, bool en AL (hook VIRTUAL)
//   CTrainNode::GetLightingFromCollision   0x6F5F80  thiscall, tColLighting (1 byte) en AL
//
// Todos los floats viajan como patrones de 32 bits sin signo. Politica de especiales (para que el oraculo no dependa de la regla
// de x87 con dos NaN): a lo sumo UN campo NaN con carga util propia por caso, y los infinitos nunca coexisten con ese NaN.
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Misc61Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
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
// NaN con carga util propia por casilla (todas con significando distinto y nunca igual al indefinido 0xFFC00000): qNaN o sNaN.
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
// Cualquier patron de 32 bits razonable (para funciones que solo copian los floats)
inline u32 AnyFloat(u32 n, u32 k, u32 slot) {
    const u32 sel = Rand(n, k) % 100;
    if (sel < 45) return OrdinaryFloat(n, k + 10, 100, 150);
    if (sel < 65) return kFloatEdges[Rand(n, k + 11) % kFloatEdges.size()];
    if (sel < 72) return TinyFloat(n, k + 12);
    if (sel < 78) return HugeFloat(n, k + 13);
    if (sel < 84) return InfBits(n, k + 14);
    if (sel < 92) return NanBits(slot + Rand(n, k + 15) % 5u, false);
    if (sel < 96) return NanBits(slot + Rand(n, k + 15) % 5u, true);
    return Rand(n, k + 16);
}

// ================================================================================================ CMemoryHeap::IntegrityCheck
// Arena de paginas (cada pagina de 0x1000 bytes). Cada estructura recorrida vive en su propia pagina para que el orden de la primera
// lectura de cada pagina (PAGE_GUARD) delate el orden del recorrido: pagina 0 = objeto CMemoryHeap (this + thisOff), pagina 1 = vector de 64 CommonSize
// (64 * 0x3C = 0xF00 bytes), luego nodos libres, nodos de tamanos comunes, bloques contiguos (1 o 2 paginas), el bloque terminador y la cadena fantasma de m_PrevBlock.
constexpr u32 kHeapPages = 48;
constexpr u32 kPageBytes = 0x1000;
constexpr u32 kHeapThisBytes = 0x48;     // sizeof(CMemoryHeap)
constexpr u32 kCommonSizeBytes = 0x3C;   // sizeof(CommonSize)
constexpr u32 kCommonSizeCount = 64;
constexpr u32 kFreeNodeBytes = 0x18;     // sizeof(HeapFreeBlockDesc)
struct C1 {
    u32 family{}, hasCs{}, thisOff{};
    std::vector<u32> freePage, freeOff;
    std::vector<u32> csEntry, csPage, csOff;   // por nodo, ordenados por entrada (estable)
    std::vector<u32> blkPage, blkLen, blkInUse, blkMemId;
    u32 termPage{}, prevMode{};   // 0 sin cadena, 1 por los bloques, 2 nodos fantasma
    std::vector<u32> ghostPage, ghostOff;
    u32 usedPages{};
    u32 eax{}, edx{}, stack{}, fill{};
};
inline C1 MakeC1(u32 n) {
    C1 c;
    const u32 f = n % 16;
    c.family = f;
    c.hasCs = f & 1u;
    const u32 pm = (f >> 1) & 3u;
    c.prevMode = pm == 0 ? 0u : pm == 1 ? 1u : 2u;
    const bool big = (f >> 3) != 0;
    const u32 nFree = big ? 1 + Rand(n, 1) % 6 : Rand(n, 1) % 3;
    const u32 nCs = c.hasCs ? (big ? 1 + Rand(n, 2) % 10 : Rand(n, 2) % 4) : 0u;
    const u32 nBlk = big ? 1 + Rand(n, 3) % 8 : Rand(n, 3) % 3;
    const u32 nGhost = c.prevMode == 2 ? 1 + Rand(n, 4) % 5 : 0u;
    c.thisOff = (Rand(n, 5) % 0x1E0u) * 4u;   // <= 0x77C: this + 0x48 cabe en la pagina 0
    u32 next = 2;
    for (u32 k = 0; k < nFree; ++k) { c.freePage.push_back(next++); c.freeOff.push_back((Rand(n, 20 + k) % 0x3F0u) * 4u); }
    std::vector<std::array<u32, 3>> cs;   // entrada, pagina, desplazamiento
    for (u32 k = 0; k < nCs; ++k) cs.push_back({Rand(n, 40 + k) % kCommonSizeCount, next++, (Rand(n, 60 + k) % 0x3F0u) * 4u});
    std::stable_sort(cs.begin(), cs.end(), [](const auto& a, const auto& b) { return a[0] < b[0]; });
    for (const auto& e : cs) { c.csEntry.push_back(e[0]); c.csPage.push_back(e[1]); c.csOff.push_back(e[2]); }
    static constexpr u8 inUseChoices[]{0, 1, 1, 1, 2, 0x80, 0xFF};
    for (u32 k = 0; k < nBlk; ++k) {
        const u32 len = Rand(n, 80 + k) % 4 == 0 ? 2u : 1u;
        c.blkPage.push_back(next); c.blkLen.push_back(len); next += len;
        const u32 inUse = inUseChoices[Rand(n, 100 + k) % std::size(inUseChoices)];
        c.blkInUse.push_back(inUse);
        // en uso: memId -1 (se omite) o 0..19; libre: cualquier valor (no se lee)
        c.blkMemId.push_back(inUse ? (Rand(n, 120 + k) % 3 == 0 ? 0xFFFFu : Rand(n, 121 + k) % 20u) : (Rand(n, 122 + k) & 0xFFFFu));
    }
    c.termPage = next++;
    for (u32 k = 0; k < nGhost; ++k) { c.ghostPage.push_back(next++); c.ghostOff.push_back((Rand(n, 140 + k) % 0x3F0u) * 4u); }
    c.usedPages = next;
    c.eax = Rand(n, 70); c.edx = Rand(n, 71); c.stack = StackSeed(n); c.fill = Rand(n, 72);
    return c;
}
inline std::string JsonC1(const C1& c) {
    Js j;
    j.U("family", c.family).U("has_cs", c.hasCs).U("this_off", c.thisOff).A("free_page", c.freePage).A("free_off", c.freeOff)
     .A("cs_entry", c.csEntry).A("cs_page", c.csPage).A("cs_off", c.csOff).A("blk_page", c.blkPage).A("blk_len", c.blkLen)
     .A("blk_inuse", c.blkInUse).A("blk_memid", c.blkMemId).U("term_page", c.termPage).U("prev_mode", c.prevMode)
     .A("ghost_page", c.ghostPage).A("ghost_off", c.ghostOff).U("used_pages", c.usedPages)
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ CSearchLight::IsSpottedEntity
// Arenas (guarda | objeto | guarda): entidad 0x40 + 0x38 + 0x40 = 0xB8; matriz 0x40 + 0x48 + 0x40 = 0xC8.
constexpr u32 kEntityBytes = 0x38, kEntityOff = 0x40, kEntityArena = 0xB8;
constexpr u32 kMatrixBytes = 0x48, kMatrixOff = 0x40, kMatrixArena = 0xC8;
constexpr u32 kEntityPlacementAt = 0x04, kEntityMatrixAt = 0x14, kMatrixPosAt = 0x30;
struct C2 {
    u32 family{}, index{}, getRet{}, cRet{}, hasMatrix{};
    u32 pos[3]{}, decoy[3]{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C2 MakeC2(u32 n) {
    C2 c;
    const u32 f = n % 16;
    c.family = f;
    static constexpr u32 idxEdges[]{0xFFFFFFFFu, 0u, 1u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0x00010002u};
    c.index = Rand(n, 1) % 3 == 0 ? idxEdges[Rand(n, 2) % std::size(idxEdges)] : Rand(n, 3);
    static constexpr u32 negs[]{0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 0xFFFF0000u};
    static constexpr u32 poss[]{0u, 1u, 5u, 0x7FFFFFFFu, 0x40u, 0xFFFFu};
    const u32 grp = f / 4;   // 0 indice negativo, 1 positivo y la 0x493280 devuelve AL = 0, 2 AL distinto de cero, 3 mezcla
    auto negIdx = [&] { return Rand(n, 4) % 2 ? negs[Rand(n, 5) % std::size(negs)] : (0x80000000u | Rand(n, 6)); };
    auto posIdx = [&] { return Rand(n, 4) % 2 ? poss[Rand(n, 5) % std::size(poss)] : (Rand(n, 6) & 0x7FFFFFFFu); };
    auto zeroAl = [&] { return Rand(n, 7) % 2 ? 0u : (Rand(n, 8) | 0xFFu) ^ 0xFFu; };   // 0, o bytes altos arbitrarios con AL = 0
    auto nonzeroAl = [&] {
        static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
        return (Rand(n, 9) % 2 ? 0u : (Rand(n, 10) & 0xFFFFFF00u)) | als[Rand(n, 11) % std::size(als)];
    };
    switch (grp) {
    case 0: c.getRet = negIdx(); c.cRet = Rand(n, 12); break;
    case 1: c.getRet = posIdx(); c.cRet = zeroAl(); break;
    case 2: c.getRet = posIdx(); c.cRet = nonzeroAl(); break;
    default: c.getRet = Rand(n, 13) % 3 == 0 ? negIdx() : posIdx(); c.cRet = Rand(n, 14) % 2 ? zeroAl() : nonzeroAl(); break;
    }
    c.hasMatrix = Rand(n, 15) & 1u;
    for (u32 i = 0; i < 3; ++i) { c.pos[i] = AnyFloat(n, 20 + 8 * i, i); c.decoy[i] = OrdinaryFloat(n, 50 + 4 * i, 110, 140); }
    c.eax = Rand(n, 70); c.ecx = Rand(n, 73); c.edx = Rand(n, 71); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC2(const C2& c) {
    Js j;
    j.U("family", c.family).U("index", c.index).U("get_ret", c.getRet).U("c_ret", c.cRet).U("has_matrix", c.hasMatrix)
     .A("pos", {c.pos[0], c.pos[1], c.pos[2]}).A("decoy", {c.decoy[0], c.decoy[1], c.decoy[2]})
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ CTrainNode::GetLightingFromCollision
// Arena: this 0x40 + 0x0A + 0x36 = 0x80. Campos: x +0, y +2, z +4 (int16), distancia +6 (uint16), iluminacion +8, encontrada +9.
constexpr u32 kNodeBytes = 0x0A, kNodeOff = 0x40, kNodeArena = 0x80;
constexpr u32 kNodeLightAt = 8, kNodeFoundAt = 9;
struct C3 {
    u32 family{}, x{}, y{}, z{}, found{}, cached{}, pvlRet{}, cpLight{}, cpSeed{};
    u32 eax{}, edx{}, stack{};
};
inline C3 MakeC3(u32 n) {
    C3 c;
    const u32 f = n % 16;
    c.family = f;
    static constexpr u32 coord[]{0u, 1u, 0xFFFFu, 0x7FFFu, 0x8000u, 0x8001u, 8u, 0xFFF8u, 0x1000u, 0xF000u};
    auto coordOf = [&](u32 k) { return Rand(n, k) % 2 ? coord[Rand(n, k + 1) % std::size(coord)] : (Rand(n, k + 2) & 0xFFFFu); };
    c.x = coordOf(1); c.y = coordOf(4); c.z = coordOf(7);
    auto garbage = [&](u32 k) { return Rand(n, k) % 2 ? 0u : (Rand(n, k + 1) & 0xFFFFFF00u); };
    static constexpr u32 nz[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x10u};
    c.cached = Rand(n, 10) % 4 == 0 ? (0x48u) : (Rand(n, 11) & 0xFFu);
    c.cpLight = Rand(n, 12) % 4 == 0 ? (Rand(n, 13) % 2 ? 0x48u : 0u) : (Rand(n, 14) & 0xFFu);
    c.cpSeed = Rand(n, 15);
    switch (f % 4) {
    case 0: c.found = nz[Rand(n, 16) % std::size(nz)]; c.pvlRet = Rand(n, 17); break;   // ya encontrada: no llama
    case 1: c.found = 0; c.pvlRet = garbage(18); break;                                // sin colision: 0x48
    case 2: c.found = 0; c.pvlRet = garbage(19) | 1u; break;                           // con colision, AL = 1
    default: c.found = 0; c.pvlRet = garbage(20) | nz[1 + Rand(n, 21) % 5]; break;     // con colision, AL no canonico
    }
    c.eax = Rand(n, 70); c.edx = Rand(n, 71); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC3(const C3& c) {
    Js j;
    j.U("family", c.family).U("x", c.x).U("y", c.y).U("z", c.z).U("found", c.found).U("cached", c.cached).U("pvl_ret", c.pvlRet)
     .U("cp_light", c.cpLight).U("cp_seed", c.cpSeed).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}

// ================================================================================================ CTaskSimpleSlideToCoord::ProcessPed
// Arenas (guarda | objeto | guarda), sumas a mano:
//   tarea  0x40 + 0x80  + 0x40 = 0x100      ped   0x40 + 0x79C + 0x40 = 0x81C      matriz 0x40 + 0x48 + 0x40 = 0xC8
//   inteligencia (solo +0x14 se lee) 0x40 + 0x40 + 0x40 = 0xC0      tarea primaria falsa 0x40 + 0x20 + 0x40 = 0xA0
constexpr u32 kTaskBytes = 0x80, kTaskOff = 0x40, kTaskArena = 0x100;
constexpr u32 kPedBytes = 0x79C, kPedOff = 0x40, kPedArena = 0x81C;
constexpr u32 kIntelBytes = 0x40, kIntelOff = 0x40, kIntelArena = 0xC0;
constexpr u32 kPrimBytes = 0x20, kPrimOff = 0x40, kPrimArena = 0xA0;
constexpr u32 kTaskPosAt = 0x64, kTaskAimAt = 0x70, kTaskSpeedAt = 0x74, kTaskBitsAt = 0x78, kTaskTimerAt = 0x7C;
constexpr u32 kPedPlacementAt = 0x04, kPedMatrixAt = 0x14, kPedIntelAt = 0x47C, kPedShiftAt = 0x4D8, kPedCurRotAt = 0x558, kPedAimRotAt = 0x55C, kPedTypeAt = 0x598;
constexpr u32 kIntelPrimaryDefaultAt = 0x14;
constexpr u32 kStandTime = 20000, kStandBlendBits = 0x41000000u;
struct C4 {
    u32 family{}, regime{}, special{}, nanSlot{};
    u32 bits78{}, timer{}, now{}, animRet{}, pedType{}, intelValid{}, hasMatrix{};
    u32 pos[3]{}, decoy[3]{}, tgt[3]{}, speed{}, right[3]{}, fwd[3]{};
    u32 cur{}, aim{}, taskAim{}, limRet{}, shiftInit[2]{};
    u32 eax{}, edx{}, stack{};
};
inline float F(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// Aritmetica del generador con un redondeo a float por operacion (identica en SSE y x87, en el juego y en el espejo).
inline float Fadd(float a, float b) { volatile float r = a + b; return r; }
inline float Fmul(float a, float b) { volatile float r = a * b; return r; }
// 16 direcciones (coseno, seno) como constantes, sin funciones transcendentes.
inline constexpr float kDir[16][2]{{1.0f, 0.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f}, {0.0f, -1.0f}, {0.6f, 0.8f}, {-0.8f, 0.6f}, {-0.6f, -0.8f}, {0.8f, -0.6f},
                                   {0.28f, 0.96f}, {0.96f, 0.28f}, {-0.28f, 0.96f}, {0.96f, -0.28f}, {0.5f, 0.8660254f}, {-0.5f, 0.8660254f}, {0.8660254f, -0.5f}, {-0.70710677f, -0.70710677f}};
inline C4 MakeC4(u32 n) {
    C4 c;
    const u32 f = n % 16;
    c.family = f;
    const u32 mode = f % 4;                    // bit 0: primera vez; bit 1: animacion en curso
    c.regime = (f / 4) * 2 + Rand(n, 1) % 2;   // 0..7
    c.bits78 = (Rand(n, 2) & 0xFCu) | (mode & 1u ? 1u : 0u) | (mode & 2u ? 2u : 0u);
    // bits 0 y 1 ya fijados; el resto (2..7) arbitrario: la funcion solo debe limpiar el bit 0
    const u32 nowEdges[]{0u, 1u, 1000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFE0Bu, 0xFFFFF82Fu, 0xFFFFFFFEu, 0xFFFFF830u};
    c.now = Rand(n, 3) % 3 == 0 ? nowEdges[Rand(n, 4) % std::size(nowEdges)] : (Rand(n, 5) % 4 == 0 ? Rand(n, 6) : Rand(n, 6) % 600000u);
    const u32 tmode = Rand(n, 7) % 8;
    switch (tmode) {
    case 0: case 1: case 2: c.timer = 0xFFFFFFFFu; break;   // sin fijar
    case 3: c.timer = c.now - 1u; break;
    case 4: c.timer = c.now; break;
    case 5: c.timer = c.now + 1u; break;
    case 6: c.timer = Rand(n, 8) % 3 == 0 ? 0u : 0xFFFFFFFEu; break;
    default: c.timer = Rand(n, 9); break;
    }
    static constexpr u32 rets[]{0u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0xFFFFFF00u};
    c.animRet = Rand(n, 10) % 2 ? rets[Rand(n, 11) % std::size(rets)] : ((Rand(n, 12) & 0xFFFFFF00u) | (Rand(n, 13) % 2 ? 1u : 0u));
    const bool player = Rand(n, 14) % 2;
    c.pedType = player ? Rand(n, 15) % 2 : (Rand(n, 16) % 3 == 0 ? (Rand(n, 17) | 2u) : 2u + Rand(n, 18) % 30u);
    if (Rand(n, 19) % 8 == 0) c.pedType = (Rand(n, 20) % 2) ? 0xFFFFFFFFu : 0x80000000u;
    c.intelValid = ((c.bits78 & 1u) && (c.pedType == 0 || c.pedType == 1)) ? 1u : Rand(n, 21) % 2;   // al llegar a la ruta del jugador el puntero debe ser valido
    c.hasMatrix = Rand(n, 22) % 4 != 0;
    c.special = Rand(n, 23) % 8 == 0 ? 1u : Rand(n, 23) % 8 == 1 ? 2u : 0u;   // 1 = un NaN, 2 = infinitos
    c.nanSlot = Rand(n, 24) % 16;
    // posiciones (ordinarias) y destino segun el regimen
    for (u32 i = 0; i < 3; ++i) c.pos[i] = OrdinaryFloat(n, 30 + 4 * i, 112, 140);
    for (u32 i = 0; i < 3; ++i) c.decoy[i] = OrdinaryFloat(n, 45 + 4 * i, 112, 140);
    const float px = F(c.pos[0]), py = F(c.pos[1]), pz = F(c.pos[2]);
    // direccion del salto al destino
    const u32 dir = Rand(n, 60) % 16;
    const float cs = kDir[dir][0], sn = kDir[dir][1];
    float rad = 0.0f;
    switch (c.regime) {
    case 0: rad = Fmul(float(Rand(n, 61) % 1000), 0.00004f); break;                                            // cerca (< 0.04)
    case 1: rad = Fmul(0.05f, Fadd(1.0f, Fmul(float(int(Rand(n, 61) % 41) - 20), 0.0001f))); break;            // borde 0.05
    case 2: rad = Fadd(0.06f, Fmul(float(Rand(n, 61) % 1000), 0.01f)); break;                                  // lejos ordinario
    case 3: rad = Fadd(1.0f, float(Rand(n, 61) % 100000)); break;                                              // lejos, coordenadas grandes
    default: rad = Rand(n, 61) % 2 ? 0.05f : Fmul(float(Rand(n, 61) % 100), 0.005f); break;
    }
    c.tgt[0] = B(Fadd(px, Fmul(rad, cs))); c.tgt[1] = B(Fadd(py, Fmul(rad, sn))); c.tgt[2] = B(Fadd(pz, Fmul(float(int(Rand(n, 62) % 2001) - 1000), 0.01f)));
    if (c.regime == 1 && Rand(n, 63) % 3 == 0) { c.tgt[0] = B(Fadd(px, 0.05f)); c.tgt[1] = c.pos[1]; }   // exactamente en el borde en un eje
    c.speed = Rand(n, 64) % 4 == 0 ? kFloatEdges[Rand(n, 65) % kFloatEdges.size()] : OrdinaryFloat(n, 66, 110, 135);
    for (u32 i = 0; i < 3; ++i) { c.right[i] = OrdinaryFloat(n, 70 + 4 * i, 100, 131); c.fwd[i] = OrdinaryFloat(n, 90 + 4 * i, 100, 131); }
    if (Rand(n, 85) % 3 == 0) {   // matriz de giro (coseno/seno de la tabla)
        const u32 d2 = Rand(n, 86) % 16;
        c.right[0] = B(kDir[d2][0]); c.right[1] = B(kDir[d2][1]); c.right[2] = 0;
        c.fwd[0] = B(-kDir[d2][1]); c.fwd[1] = B(kDir[d2][0]); c.fwd[2] = 0;
    }
    c.cur = OrdinaryFloat(n, 110, 100, 130);
    c.aim = Rand(n, 112) % 3 == 0 ? c.cur : OrdinaryFloat(n, 113, 100, 130);
    if (Rand(n, 115) % 4 == 0) c.aim = B(Fadd(F(c.cur), Fmul(float(int(Rand(n, 116) % 21) - 10), 0.01f)));
    c.taskAim = Rand(n, 117) % 2 ? OrdinaryFloat(n, 118, 100, 130) : kFloatEdges[Rand(n, 119) % kFloatEdges.size()];
    static constexpr u32 limEdges[]{0u, 0x80000000u, 0x3D4CCCCDu, 0x3DCCCCCCu, 0x3DCCCCCDu, 0x3DCCCCCEu, 0xBDCCCCCCu, 0xBDCCCCCDu, 0xBDCCCCCEu, 0x3F800000u,
                                    0x7FC00123u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x007FFFFFu, 0x3C23D70Au, 0x3E4CCCCDu};
    c.limRet = Rand(n, 120) % 3 == 0 ? limEdges[Rand(n, 121) % std::size(limEdges)] : (Rand(n, 122) % 2 ? OrdinaryFloat(n, 123, 110, 127) : (0x3DC00000u + Rand(n, 124) % 0x20000u));
    c.shiftInit[0] = Rand(n, 125); c.shiftInit[1] = Rand(n, 126);
    // regimenes de estres
    switch (c.regime) {
    case 4:   // velocidad denormal / cero / enorme
        c.speed = Rand(n, 130) % 3 == 0 ? TinyFloat(n, 131) : Rand(n, 130) % 3 == 1 ? HugeFloat(n, 133) : kFloatEdges[Rand(n, 135) % kFloatEdges.size()];
        c.tgt[0] = B(px + 1.0f + float(Rand(n, 136) % 50)); break;   // lejos para entrar en la ruta de deslizamiento
    case 5:   // posiciones y destino diminutos: la suma queda denormal o cero
        for (u32 i = 0; i < 3; ++i) { c.pos[i] = Rand(n, 140 + i) % 4 == 0 ? OrdinaryFloat(n, 150 + 3 * i, 60, 100) : TinyFloat(n, 150 + 3 * i); c.tgt[i] = Rand(n, 160 + i) % 4 == 0 ? OrdinaryFloat(n, 170 + 3 * i, 60, 100) : TinyFloat(n, 170 + 3 * i); }
        c.cur = TinyFloat(n, 190); c.aim = Rand(n, 192) % 2 ? TinyFloat(n, 193) : c.cur;   // resta de angulos con diferencia denormal
        break;
    case 6:   // matriz con denormales / ceros / enormes y destino lejano
        c.tgt[0] = B(px + 2.0f + float(Rand(n, 180) % 50));
        for (u32 i = 0; i < 3; ++i) { c.right[i] = AnyFloat(n, 181 + 4 * i, 0); c.fwd[i] = AnyFloat(n, 201 + 4 * i, 0); }
        break;
    case 7:   // coordenadas enormes (productos que se salen del rango de float en el x87 pero no en el registro)
        for (u32 i = 0; i < 3; ++i) { c.pos[i] = HugeFloat(n, 220 + 3 * i); c.tgt[i] = Rand(n, 230 + i) % 2 ? HugeFloat(n, 240 + 3 * i) : OrdinaryFloat(n, 250 + 3 * i, 100, 150); }
        c.cur = HugeFloat(n, 253); c.aim = Rand(n, 255) % 2 ? HugeFloat(n, 256) : c.cur;   // resta que se sale del rango de float
        break;
    default: break;
    }
    // especiales: un NaN con carga propia (sin infinitos) o infinitos (sin NaN) -- sobre campos que entran en la aritmetica
    u32* fl[16] = {&c.pos[0], &c.pos[1], &c.pos[2], &c.tgt[0], &c.tgt[1], &c.tgt[2], &c.speed, &c.right[0], &c.right[1], &c.right[2], &c.fwd[0], &c.fwd[1], &c.fwd[2], &c.cur, &c.aim, &c.taskAim};
    auto isNan = [](u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; };
    auto isInf = [](u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; };
    // los regimenes 4..7 pueden haber traido ya especiales del generador generico: se limpian y se aplica la politica
    for (u32* p : fl) { if (isNan(*p) || isInf(*p)) *p = 0x3F800000u; }
    if (c.special == 1) {
        u32 slot = c.nanSlot;
        *fl[slot] = NanBits(slot, Rand(n, 260) % 3 == 0);
    } else if (c.special == 2) {
        const u32 cnt = 1 + Rand(n, 261) % 2;
        for (u32 k = 0; k < cnt; ++k) *fl[Rand(n, 262 + k) % 16] = InfBits(n, 264 + k);
    }
    // sin matriz solo es valido el camino sin deslizamiento (el EXE desreferencia la matriz al deslizar): el destino queda dentro del radio, o la suma es NaN
    if (!c.hasMatrix) {
        const float fx = F(c.pos[0]), fy = F(c.pos[1]);
        c.tgt[0] = B(fx); c.tgt[1] = B(fy);
        if (c.special == 0 && Rand(n, 270) % 2) { c.tgt[0] = B(Fadd(fx, Fmul(0.01f, float(int(Rand(n, 271) % 5) - 2)))); c.tgt[1] = B(Fadd(fy, Fmul(0.01f, float(int(Rand(n, 272) % 5) - 2)))); }
        if (c.special == 2) { *fl[0] = 0x3F800000u; *fl[1] = 0x3F800000u; *fl[3] = 0x3F800000u; *fl[4] = 0x3F800000u; }   // sin infinitos en x/y: suma finita < radio
        if (c.special == 1 && c.nanSlot < 6) { /* NaN en posicion/destino: suma NaN -> desplazamiento cero, sin matriz */ }
    }
    c.eax = Rand(n, 280); c.edx = Rand(n, 281); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC4(const C4& c) {
    Js j;
    j.U("family", c.family).U("regime", c.regime).U("special", c.special).U("bits78", c.bits78).U("timer", c.timer).U("now", c.now)
     .U("anim_ret", c.animRet).U("ped_type", c.pedType).U("intel_valid", c.intelValid).U("has_matrix", c.hasMatrix)
     .A("pos", {c.pos[0], c.pos[1], c.pos[2]}).A("decoy", {c.decoy[0], c.decoy[1], c.decoy[2]}).A("tgt", {c.tgt[0], c.tgt[1], c.tgt[2]})
     .U("speed", c.speed).A("right", {c.right[0], c.right[1], c.right[2]}).A("fwd", {c.fwd[0], c.fwd[1], c.fwd[2]})
     .U("cur", c.cur).U("aim", c.aim).U("task_aim", c.taskAim).U("lim_ret", c.limRet).A("shift_init", {c.shiftInit[0], c.shiftInit[1]})
     .U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}  // namespace Misc61Fix
