#pragma once
// Fixtures del banco Fn85I (lote 85I): una funcion, 16384 casos (16384 pares). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskSimpleFight::StartAnim                    0x623B10  thiscall (tarea en ECX; ped y comando de ataque en la pila), `ret 8`; no devuelve nada que lean los llamadores   (hook CTaskSimpleFight / StartAnim, tipo S)
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

namespace F85IFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kGroups = 48, kGroupsWin = kGroups * 0x14, kInfoWin = 0x70, kNameWin = 0x100, kTaskWin = 0x28, kPedWin = 0x740, kPdWin = 0x40, kAssocWin = 0x40, kIntWin = 0x20;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kTaskArena = Arena(kTaskWin), kPedArena = Arena(kPedWin), kPdArena = Arena(kPdWin), kAssocArena = Arena(kAssocWin), kIntArena = Arena(kIntWin),
                      kInfoArena = Arena(kInfoWin), kNameArena = Arena(kNameWin), kGroupsArena = Arena(kGroupsWin);

constexpr u32 kGroupsAddr = 0xB4EA34;       // CAnimManager::ms_aAnimAssocGroups (puntero al arreglo de grupos, 0x14 bytes por grupo; +0 = CAnimBlock*)
constexpr u32 kBlocksAddr = 0xB5D4A0;       // CAnimManager::ms_aAnimBlocks (0x20 bytes por bloque; +0x10 = IsLoaded)
constexpr std::size_t kBlockRegion = 16 * 0x20;
constexpr u32 kComboAddr = 0xC170D0;        // CTaskSimpleFight::m_aComboData (0x88 bytes por fila; +0 = grupo de animacion, +0x44 = campo de reposicion, +0x85 = byte alto de las banderas)
constexpr std::size_t kComboRegion = 12 * 0x88;
constexpr unsigned kAssocs = 4;             // 0 animacion actual, 1 alternativa (mutaciones), 2 animacion de reposo, 3 la que devuelve BlendAnimation

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 85009u); }
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

// Floats "especiales": ceros, desnormales, unos, medios, infinitos, NaN (silenciosos y senalizantes), vecinos de 1.0
inline constexpr std::array<u32, 22> kSpecialF{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u,
                                               0x7F800001u, 0xFF800001u, 0x3F7FFFFFu, 0x3F800001u, 0x7F7FFFFFu, 0x00800000u, 0x80800000u, 0x40000000u, 0xBF7FFFFFu, 0x3F7FFFFEu};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 attack{};                   // dword de pila: el signo decide, el byte bajo es el movimiento
    u32 cmd{};                      // byte +0x26 de la tarea (comando siguiente)
    u32 combo{}, move{}, cont{}, last{}, finished{}, ref{}, cur{};   // tarea: bytes +0x24, +0x25, +0x14, +0x27, +8, +0xA y dword +0xC (grupo requerido)
    u32 hasAnim{}, hasIdle{};       // la tarea tiene animacion (+0x1C) / de reposo (+0x20)
    u32 aAmt[kAssocs]{}, aDlt[kAssocs]{}, aTime[kAssocs]{}, aSpd[kAssocs]{}, aFl[kAssocs]{};   // +0x18, +0x1C, +0x20, +0x24 y dword +0x2C (banderas = +0x2E) de cada asociacion
    u32 rowGroup[12]{}, rowFlags[12]{}, rowLoop[12]{};   // filas 0..11 de la tabla de combos (fila = combo - 4): grupo (+0), byte +0x85 y float +0x44
    u32 slot{}, wtype{}, binfo{}, fstyle{};
    u32 gblk[kGroups]{};            // bloque de cada grupo: 0 = nulo; k + 1 = bloque k de ms_aAnimBlocks
    u32 lookup{};                   // bloque (0..15) que devuelve GetAnimationBlock
    u32 loaded[16]{};               // byte IsLoaded de los bloques 0..15
    u32 clump{}, pflags{}, agroup{}, mstate{}, mswim{};   // ped: +0x18, +0x46C, +0x4D4, +0x534, +0x538
    u32 hasPd{};                    // el ped tiene datos de jugador (+0x480)
    u32 pdA[3]{}, pdB[3]{};         // floats +0xC, +0x10 y +0x14 de los datos de jugador (y de los alternativos)
    u32 isplayer{}, duck{}, stat{}; // AL de 0x5DF8F0 (con basura), valor de 0x6010A0 (0 o no) y float de 0x559AF0
    u32 mutSite{}, mutB{};
    u32 gate{};
};

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
    auto Junk = [&](u32 al) { return (Rand(n, nk()) & ~0xFFu) | al; };
    auto Fl = [&](float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); };
    auto Sp = [&] { return kSpecialF[R(u32(kSpecialF.size()))]; };
    auto FltAny = [&](u32 specialPct) { return P(specialPct) ? Sp() : B(Fl(-2.0f, 2.0f)); };
    // --- datos aleatorios base
    c.combo = 4u + R(12);
    c.move = P(50) ? R(6) : R(256);
    c.cont = P(50) ? 0u : R(256);
    c.last = R(256);
    c.finished = P(50) ? 0u : (P(70) ? 1u : R(256));
    c.ref = P(50) ? 0u : (P(70) ? 1u : Pk({2u, 0x80u, 0xFFu}));
    c.cur = P(15) ? 0x21u : R(40);
    c.hasAnim = P(50) ? 1u : 0u;
    c.hasIdle = P(60) ? 1u : 0u;
    for (unsigned i = 0; i < kAssocs; ++i) {
        c.aAmt[i] = FltAny(35); c.aDlt[i] = FltAny(35); c.aTime[i] = FltAny(35); c.aSpd[i] = Rand(n, nk());
        c.aFl[i] = Rand(n, nk());
    }
    for (u32 i = 0; i < 12; ++i) {
        c.rowGroup[i] = P(15) ? 0x21u : R(40);
        c.rowFlags[i] = R(256);
        if (P(60)) c.rowFlags[i] |= 4u; else c.rowFlags[i] &= ~4u;
        c.rowLoop[i] = FltAny(40);
    }
    c.slot = u32(i32(R(16)) - 3) & 0xFFu;
    c.wtype = P(70) ? R(46) : Rand(n, nk());
    c.binfo = P(40) ? 4u : 5u + R(11);
    c.fstyle = P(25) ? 4u : 4u + R(12);
    for (u32 g = 0; g < kGroups; ++g) c.gblk[g] = P(35) ? 0u : 1u + R(16);
    c.lookup = R(16);
    for (u32 k = 0; k < 16; ++k) c.loaded[k] = P(45) ? 0u : Pk({1u, 1u, 1u, 2u, 0x80u, 0xFFu});
    c.clump = Rand(n, nk()) | 0x00400000u;
    c.pflags = Rand(n, nk());
    if (P(50)) c.pflags |= 0x4000000u; else c.pflags &= ~0x4000000u;
    c.agroup = R(60);
    c.mstate = P(60) ? R(8) : Pk({0u, 1u, 3u, 4u, 5u, 6u, 7u, 8u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x100u});
    c.mswim = Rand(n, nk());
    c.hasPd = P(60) ? 1u : 0u;
    for (unsigned i = 0; i < 3; ++i) { c.pdA[i] = P(30) ? Sp() : Rand(n, nk()); c.pdB[i] = P(30) ? Sp() : Rand(n, nk()); }
    c.isplayer = Junk(P(50) ? 1u : 0u);
    c.duck = P(50) ? 1u : 0u;
    c.stat = FltAny(35);
    const u32 hi = R(0x800000u) << 8;
    c.attack = hi | R(256);                       // no negativo
    // Ayudas por estado: elige el comando siguiente
    auto Pick = [&](std::initializer_list<u32> l) { c.cmd = Pk(l); };
    auto Gacs = [&] {   // la rama de GetAvailableComboSet(ped, 0): filas y grupos coherentes con el estilo
        const u32 sub = (n / 16u) % 8u;
        u32 st = c.binfo == 4u ? c.fstyle : c.binfo;
        const u32 r = st - 4u;
        switch (sub) {
        case 0: c.rowFlags[r] &= ~4u; break;                                                                                      // sin bandera: devuelve 4
        case 1: c.rowFlags[r] |= 4u; c.rowGroup[r] = 0x21u; break;                                                                 // grupo 0x21: devuelve el estilo
        case 2: c.rowFlags[r] |= 4u; if (c.rowGroup[r] == 0x21u) c.rowGroup[r] = 1u + R(39); c.cur = c.rowGroup[r]; c.ref = c.ref ? c.ref : 1u; break;   // mismo grupo, referenciado
        case 3: c.rowFlags[r] |= 4u; if (c.rowGroup[r] == 0x21u) c.rowGroup[r] = 1u + R(39); c.cur = c.rowGroup[r]; c.ref = 0u; break;               // mismo grupo, sin referencia
        case 4: case 5: case 6: case 7: {                                                                                          // grupo distinto
            c.rowFlags[r] |= 4u; if (c.rowGroup[r] == 0x21u) c.rowGroup[r] = 1u + R(39);
            c.cur = (c.rowGroup[r] + 1 + R(30)) % 40u;
            if (sub == 4) c.ref = c.ref ? c.ref : 1u; else if (sub == 5) c.ref = 0u;
            const u32 blk = c.gblk[c.rowGroup[r]] ? c.gblk[c.rowGroup[r]] - 1 : c.lookup;
            c.loaded[blk] = sub == 6 ? 0u : Pk({1u, 2u, 0x80u, 0xFFu});
            break;
        }
        }
    };
    // --- por familia
    switch (f) {
    case 0:   // ataque negativo: solo se pone a cero el comando siguiente
        c.attack = 0x80000000u | (c.attack & 0x7FFFFFFFu);
        c.cmd = P(60) ? R(0x13) : R(256);
        break;
    case 1:   // comando fuera de 0..0x12 (negativo o mayor): solo el cierre
        c.cmd = P(50) ? 0x13u + R(0x6D) : 0x80u + R(0x80);
        break;
    case 2:   // sin comando, estado de movimiento alto: el jugador termina la tarea
        c.cmd = 0;
        c.mstate = Pk({4u, 5u, 6u, 7u, 0x100u, 0x7FFFFFFFu, 4u, 5u});
        c.isplayer = Junk(P(70) ? 1u : 0u);
        Gacs();
        break;
    case 3: case 4: case 5:   // sin comando: combo disponible, animacion de reposo, objetivo en el suelo
        c.cmd = 0;
        if (P(70)) c.mstate = Pk({0u, 1u, 2u, 3u, 0x80000000u, 0xFFFFFFFFu});
        Gacs();
        if (f == 4) { c.hasIdle = 1; c.isplayer = Junk(1u); c.aAmt[2] = FltAny(45); c.aDlt[2] = FltAny(45); }
        if (f == 5) { c.hasIdle = P(30) ? 1u : 0u; c.isplayer = Junk(P(70) ? 1u : 0u); }
        break;
    case 6:   // 1 y 15..18 con datos de jugador
        Pick({1u, 0xFu, 0x10u, 0x11u, 0x12u, 0x10u, 0x11u});
        c.hasPd = 1;
        break;
    case 7:   // 1 y 15..18 sin datos de jugador
        Pick({1u, 0xFu, 0x10u, 0x11u, 0x12u, 0x10u, 0x11u});
        c.hasPd = 0;
        break;
    case 8:   // 0x12 y 0xF con las banderas de agacharse
        Pick({0x12u, 0x12u, 0x12u, 0xFu});
        if (P(30)) c.hasPd = 0;
        break;
    case 9:   // 2: banderas de la fila
        c.cmd = 2;
        c.rowFlags[c.combo - 4u] = P(50) ? (c.rowFlags[c.combo - 4u] | 2u) : (c.rowFlags[c.combo - 4u] & ~2u);
        break;
    case 10:  // 3..10 sin jugador
        c.cmd = 3u + R(8);
        c.isplayer = Junk(0u);
        break;
    case 11:  // 3..10 con jugador (no hace nada) o mezcla
        c.cmd = 3u + R(8);
        c.isplayer = Junk(P(70) ? 1u : 0u);
        break;
    case 12:  // 11..14, movimiento 3 y distintos tiempos
        c.cmd = 11u + R(4);
        if (P(60)) c.attack = (c.attack & ~0xFFu) | 3u;
        c.aTime[3] = P(40) ? Pk({0u, 0x80000000u, 0x7FC00000u, 0x7F800001u}) : FltAny(30);
        break;
    case 13:  // 11..14, otros movimientos y modificador especial
        c.cmd = 11u + R(4);
        c.attack = (c.attack & ~0xFFu) | Pk({0u, 1u, 2u, 4u, 5u, 0x7Fu, 0x80u, 0xFFu, 3u, 0x103u});
        c.hasPd = P(70) ? 1u : 0u;
        c.stat = P(25) ? Pk({0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu}) : (P(40) ? Sp() : B(Fl(0.0f, 2.0f)));
        break;
    case 14: { // mutaciones
        c.mutSite = P(10) ? 7u : 1u + R(7);
        c.mutB = R(256);
        switch (c.mutSite) {
        case 1: c.cmd = P(60) ? 3u + R(8) : (P(50) ? 0u : 11u + R(4)); c.mutB = P(50) ? 3u : R(256); c.isplayer = Junk(0u); break;
        case 2: c.cmd = Pk({2u, 3u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 0u}); c.isplayer = Junk(0u); break;
        case 3: c.cmd = 11u + R(4); c.mutB = 4u + R(12); if (P(70)) c.attack = (c.attack & ~0xFFu) | 3u; if (P(60)) c.aTime[3] = 0x3F800000u; break;
        case 4: c.cmd = Pk({1u, 0x10u, 0x11u, 0x10u, 0x11u, 0u}); c.hasPd = 1; if (c.cmd == 0) { c.isplayer = Junk(1u); c.mstate = R(4); Gacs(); } break;
        case 5: c.cmd = 0; c.mstate = R(4); c.isplayer = Junk(1u); Gacs(); break;
        case 6: c.cmd = 11u + R(4); c.hasPd = 1; if (P(60)) c.attack = (c.attack & ~0xFFu) | 3u; break;
        default:
            c.cmd = P(60) ? 0u : Pk({3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u});
            c.isplayer = Junk(0u);
            if (c.cmd == 0) {   // la primera IsPlayer (estado de movimiento alto, no jugador) cambia el comando siguiente a 0xB..0xE y GetAvailableComboSet pide el bloque sin cargar
                c.mstate = Pk({4u, 5u, 6u, 7u});
                c.mutB = 0xBu + R(4);
                const u32 st = c.binfo == 4u ? c.fstyle : c.binfo, r = st - 4u;
                c.rowFlags[r] |= 4u; if (c.rowGroup[r] == 0x21u) c.rowGroup[r] = 1u + R(39);
                c.cur = (c.rowGroup[r] + 1 + R(30)) % 40u; c.ref = 0u;
                c.loaded[c.gblk[c.rowGroup[r]] ? c.gblk[c.rowGroup[r]] - 1 : c.lookup] = 0u;
            }
            break;
        }
        c.mutB &= 0xFFu;
        break;
    }
    default:  // mezcla
        c.cmd = P(8) ? R(256) : R(0x13);
        if (P(5)) c.attack |= 0x80000000u;
        if (c.cmd == 0) Gacs();
        break;
    }
    if (c.cmd == 0) c.hasPd = 1;                   // el EXE no comprueba los datos de jugador del caso 0
    // un caso mutado nunca usa un ataque negativo
    if (c.mutSite) c.attack &= 0x7FFFFFFFu;
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("attack", c.attack).U("cmd", c.cmd).U("combo", c.combo).U("move", c.move).U("cont", c.cont).U("last", c.last).U("finished", c.finished).U("ref", c.ref).U("cur", c.cur)
     .U("hasanim", c.hasAnim).U("hasidle", c.hasIdle)
     .A("aamt", std::vector<u32>(c.aAmt, c.aAmt + kAssocs)).A("adlt", std::vector<u32>(c.aDlt, c.aDlt + kAssocs)).A("atime", std::vector<u32>(c.aTime, c.aTime + kAssocs))
     .A("aspd", std::vector<u32>(c.aSpd, c.aSpd + kAssocs)).A("afl", std::vector<u32>(c.aFl, c.aFl + kAssocs))
     .A("rowgroup", std::vector<u32>(c.rowGroup, c.rowGroup + 12)).A("rowflags", std::vector<u32>(c.rowFlags, c.rowFlags + 12)).A("rowloop", std::vector<u32>(c.rowLoop, c.rowLoop + 12))
     .U("slot", c.slot).U("wtype", c.wtype).U("binfo", c.binfo).U("fstyle", c.fstyle)
     .A("gblk", std::vector<u32>(c.gblk, c.gblk + kGroups)).U("lookup", c.lookup).A("loaded", std::vector<u32>(c.loaded, c.loaded + 16))
     .U("clump", c.clump).U("pflags", c.pflags).U("agroup", c.agroup).U("mstate", c.mstate).U("mswim", c.mswim).U("haspd", c.hasPd)
     .A("pda", std::vector<u32>(c.pdA, c.pdA + 3)).A("pdb", std::vector<u32>(c.pdB, c.pdB + 3))
     .U("isplayer", c.isplayer).U("duck", c.duck).U("stat", c.stat).U("mutsite", c.mutSite).U("mutb", c.mutB);
    j.U("gate", c.gate);
    return j.End();
}
}
