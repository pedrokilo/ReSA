#pragma once
// Fixtures del banco PlayerGroup75 (lote 75): CPlayerPed::MakeThisPedJoinOurGroup (EXE 0x60C840, thiscall, ECX = jugador, 1 argumento en pila (CPed* peaton), `ret 4`, sin valor de retorno).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::MakeThisPedJoinOurGroup  0x60C840  (CPed* peaton)   (hook CPlayerPed / MakeThisPedJoinOurGroup, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: n < 16384: familia = n % 16 (1024 casos por familia); despues las familias 16..19 (ver MakeExtra).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Pg75Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (n % 16), 1024 casos cada una
constexpr unsigned kFamStat = 1024;      // familia 16: estadistica 0xB5 contra 1.0f / 2.0f (NaN, infinitos, desnormales, +-0, bordes) y limites
constexpr unsigned kFamCount = 1024;     // familia 17: recuentos de miembros y limites en los bordes (7, limite, limite +- 1, signos)
constexpr unsigned kFamMut = 1024;       // familia 18: mutaciones del indice de grupo, del limite y de las banderas entre llamadas
constexpr unsigned kFamGroups = 8 * 8 * 8;   // familia 19: indice de grupo 0..7 x indice del otro jugador 0..7 x ramas de union (512)
constexpr unsigned kCases = kBaseCases + kFamStat + kFamCount + kFamMut + kFamGroups;
constexpr unsigned kFamilies = 20;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kPlayerWin = 0x800, kDataWin = 0x100, kPedWin = 0x800, kIntelWin = 0x100, kTaskWin = 0x30, kEvWin = 0x20, kOtherGroupWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPlayerArena = Arena(kPlayerWin), kDataArena = Arena(kDataWin), kPedArena = Arena(kPedWin), kIntelArena = Arena(kIntelWin), kTaskArena = Arena(kTaskWin), kEvArena = Arena(kEvWin),
                      kOtherGroupArena = Arena(kOtherGroupWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 75176u); }
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
inline bool IsNan(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

inline constexpr std::array<u32, 2> kNan{0x7FC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// NaN con signo, carga util y tipo (QNaN/SNaN) arbitrarios; `kind`: 0 QNaN, 1 SNaN, otro = aleatorio.
inline u32 NanBits(u32 c, u32 k, u32 kind) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1);
    u32 payload;
    switch (s % 6) {
    case 0: payload = 0; break;
    case 1: payload = 1 + (r % 0xFF); break;
    case 2: payload = 0x3FFFFFu; break;
    case 3: payload = 0x200000u | (r & 0xFFu); break;
    case 4: payload = 0x1FFFFFu; break;
    default: payload = (r >> 3) & 0x3FFFFFu; break;
    }
    const bool quiet = kind == 0 ? true : kind == 1 ? false : ((r >> 30) & 1) != 0;
    if (!quiet && payload == 0) payload = 1;   // un sNaN exige carga util distinta de cero
    return ((r >> 31) << 31) | 0x7F800000u | (quiet ? 0x400000u : 0u) | payload;
}

// ------------------------------------------------------------------------------------------------ el caso
// Todo lo que el EXE lee de memoria o recibe de un colaborador y puede variar. `site` = fake donde se muta la memoria (0 = ninguno).
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 flags478{}, flags470{}, flags474{};          // dwords de banderas del peaton (+0x478: bit 10 = drogado; +0x470: bit 20 se limpia; +0x474: bit 13 se activa)
    u32 hasTask{}, pedType{}, flagA{}, flagB{}, flagC{};   // tarea 1000 activa; tipo del peaton; bytes globales 0x96917C..E
    u32 group{}, limit{}, otherPlayer{}, otherGroup{}, otherLimit{};   // datos del jugador: indice del grupo (+0x38) y limite del script (+0x43); el otro jugador de FindPlayerPed
    u32 isMember{}, memberHi{};
    u32 maxMembers{}, stat1{}, stat2{}, joinFlags{}, joinHi{};
    u32 counts[4]{};
    u32 pedsGroup{}, taskFail{}, eventFail{};
    u32 ref{}, blip{}, colorA{}, colorB{}, colorC{};
    u32 mutSite{}, mutGroup{}, mutLimit{}, mutFlags{}, mutFlagValue470{}, mutFlagValue474{};
};

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    // por defecto: camino de union (peaton normal, sin tarea 1000, tipo 8, no es miembro, sitio libre)
    c.flags478 = Rand(n, 10) & ~0x400u;
    c.flags470 = Rand(n, 11); c.flags474 = Rand(n, 12);
    c.hasTask = 0;
    c.pedType = 8;
    c.flagA = Rand(n, 13) % 2; c.flagB = Rand(n, 14) % 2; c.flagC = Rand(n, 15) % 2;
    c.group = Rand(n, 16) % 8; c.limit = Rand(n, 17) % 9; c.otherPlayer = Rand(n, 18) % 4 == 0; c.otherGroup = Rand(n, 19) % 8; c.otherLimit = Rand(n, 20) % 9;
    c.isMember = 0; c.memberHi = Rand(n, 21) % 2 ? 0u : Rand(n, 22) << 8;
    c.maxMembers = Rand(n, 23) % 7;
    c.stat1 = B(0.0f); c.stat2 = B(0.0f);
    c.joinFlags = Rand(n, 24) % 2; c.joinHi = Rand(n, 25) % 2 ? 0u : Rand(n, 26) << 8;
    for (int i = 0; i < 4; ++i) c.counts[i] = Rand(n, 27 + i) % 9;
    c.pedsGroup = Rand(n, 31) % 2; c.taskFail = Rand(n, 32) % 16 == 0; c.eventFail = Rand(n, 33) % 16 == 0;
    c.ref = Rand(n, 34); c.blip = Rand(n, 35) % 2 ? Rand(n, 36) : Rand(n, 36) % 64;
    c.colorA = Rand(n, 37) & 0xFF; c.colorB = Rand(n, 38) & 0xFF; c.colorC = Rand(n, 39) & 0xFF;
    return c;
}

inline u32 Stat(u32 c, u32 k) {
    static constexpr u32 sp[]{0x3F800000u, 0x40000000u, 0x3F800001u, 0x3F7FFFFFu, 0x40000001u, 0x3FFFFFFFu, 0xBF800000u, 0xC0000000u, 0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x007FFFFFu};
    const u32 r = Rand(c, k) % 8;
    if (r < 4) return sp[Rand(c, k + 1) % std::size(sp)];
    if (r == 4) return Pick(kNan, c, k + 2);
    if (r == 5) return NanBits(c, k + 3, 2);
    if (r == 6) return B(float(Rand(c, k + 4) % 4));
    return Rand(c, k + 5);
}

inline C MakeExtra(unsigned n) {
    C c = BaseDefaults(n);
    const unsigned j = n - kBaseCases;
    if (j < kFamStat) {   // familia 16: la estadistica 0xB5 contra 1.0f y 2.0f (cada llamada devuelve su valor) y los dos limites
        c.family = 16;
        c.stat1 = Stat(n, 40); c.stat2 = Stat(n, 50);
        if (Rand(n, 60) % 3 == 0) c.stat2 = c.stat1;
        c.maxMembers = Rand(n, 61) % 9; c.limit = Rand(n, 62) % 9; c.joinFlags = Rand(n, 63) % 2;
        for (int i = 0; i < 4; ++i) c.counts[i] = Rand(n, 64 + i) % 9;
        return c;
    }
    if (j < kFamStat + kFamCount) {   // familia 17: recuentos en los bordes de 7 y del limite
        c.family = 17;
        const u32 k = Rand(n, 70) % 6;
        c.maxMembers = Rand(n, 71) % 10; c.limit = Rand(n, 72) % 10; c.joinFlags = Rand(n, 73) % 2;
        const u32 lim = std::min(c.maxMembers, c.limit);   // limite efectivo (si no hay mutacion ni estadistica)
        const u32 base = k < 3 ? 7u : lim;
        for (int i = 0; i < 4; ++i) c.counts[i] = u32(i32(base) + i32(Rand(n, 74 + i) % 3) - 1);
        if (Rand(n, 80) % 8 == 0) for (int i = 0; i < 4; ++i) c.counts[i] = u32(i32(Rand(n, 81 + i) % 20) - 10);   // recuentos negativos (comparacion con signo)
        if (Rand(n, 90) % 8 == 0) c.counts[0] = Pick(std::array<u32, 4>{0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x80000007u}, n, 91);
        return c;
    }
    if (j < kFamStat + kFamCount + kFamMut) {   // familia 18: mutaciones entre llamadas
        c.family = 18;
        c.mutSite = 1 + Rand(n, 100) % 7;
        c.mutGroup = Rand(n, 101) % 8; c.mutLimit = Rand(n, 102) % 9;
        c.mutFlags = Rand(n, 103) % 2;
        c.mutFlagValue470 = Rand(n, 104); c.mutFlagValue474 = Rand(n, 105);
        c.joinFlags = Rand(n, 106) % 2;
        return c;
    }
    c.family = 19;   // familia 19: indice de grupo x indice del otro jugador x ramas
    const unsigned g = j - kFamStat - kFamCount - kFamMut;
    c.group = g % 8; c.otherGroup = (g / 8) % 8; c.otherPlayer = (g / 64) % 2; c.pedsGroup = (g / 128) % 2;
    c.maxMembers = 6; c.limit = 8; c.joinFlags = (g / 256) % 2; c.counts[0] = 3; c.counts[1] = 3; c.counts[2] = 3; c.counts[3] = 3;
    return c;
}

inline C MakeC(unsigned n) {
    if (n >= kBaseCases) return MakeExtra(n);
    C c = BaseDefaults(n);
    auto setSite = [&] {
        c.mutSite = 1 + Rand(n, 100) % 7; c.mutGroup = Rand(n, 101) % 8; c.mutLimit = Rand(n, 102) % 9;
    };
    switch (c.family) {
    case 0:   // el peaton va drogado: solo dice una frase (bit 10 del dword +0x478), con otros bits al azar
        c.flags478 = Rand(n, 110) | 0x400u;
        if (Rand(n, 111) % 3 == 0) { c.hasTask = 1; c.pedType = Rand(n, 112) % 20; }
        break;
    case 1:   // tarea 1000 activa: sale
        c.hasTask = 1; c.pedType = Rand(n, 113) % 2 ? 8u : Rand(n, 114) % 20;
        break;
    case 2:   // ni tipo 8 ni banderas globales: sale; con alguna de las tres banderas (o el tipo 8) sigue
        c.pedType = Rand(n, 115) % 2 ? 8u : Rand(n, 116) % 20;
        if (c.pedType == 8) c.pedType = 9;
        c.flagA = Rand(n, 117) % 3 == 0; c.flagB = Rand(n, 118) % 3 == 0; c.flagC = Rand(n, 119) % 3 == 0;
        if (Rand(n, 120) % 4 == 0) c.flagA = c.flagB = c.flagC = 0;
        break;
    case 3:   // ya es miembro: sale (con AL y basura en el resto de EAX)
        c.isMember = 1; c.pedType = Rand(n, 121) % 2 ? 8u : 9u; c.flagA = 1;
        break;
    case 4:   // la estadistica 0xB5 vale 1.0f o 2.0f: limite 0, no hay sitio salvo recuento negativo
        c.stat1 = Rand(n, 122) % 2 ? B(1.0f) : B(2.0f); c.stat2 = Rand(n, 123) % 2 ? B(2.0f) : B(1.0f);
        if (Rand(n, 124) % 3 == 0) { c.stat1 = B(0.0f); }
        c.maxMembers = Rand(n, 125) % 9;
        break;
    case 5:   // sin sitio: el recuento supera el limite (se niega y recibe CEventDontJoinPlayerGroup)
        c.joinFlags = Rand(n, 126) % 2; c.maxMembers = Rand(n, 127) % 4; c.limit = 4 + Rand(n, 128) % 4;
        c.counts[0] = c.counts[1] = c.counts[2] = c.counts[3] = Rand(n, 129) % 2 ? 8u : c.maxMembers + Rand(n, 130) % 3;
        break;
    case 6:   // 609380 verdadero y recuento < 7: se une aunque supere el limite
        c.joinFlags = 1; c.counts[0] = Rand(n, 131) % 7; c.maxMembers = 0; c.limit = 0;
        break;
    case 7:   // 609380 verdadero y recuento >= 7
        c.joinFlags = 1; c.counts[0] = 7 + Rand(n, 132) % 3; c.maxMembers = Rand(n, 133) % 9;
        for (int i = 1; i < 4; ++i) c.counts[i] = Rand(n, 134 + i) % 9;
        break;
    case 8:   // union con asignacion de tarea que falla
        c.taskFail = 1; c.joinFlags = 1; c.counts[0] = 1;
        break;
    case 9:   // union con evento que falla
        c.eventFail = 1; c.joinFlags = 1; c.counts[0] = 1;
        break;
    case 10:  // el peaton ya estaba en otro grupo: sale de el
        c.pedsGroup = 1; c.joinFlags = 1; c.counts[0] = 2;
        break;
    case 11:  // otro jugador en FindPlayerPed (el indice de grupo de la tarea viene de ahi)
        c.otherPlayer = 1; c.joinFlags = 1; c.counts[0] = 2;
        break;
    case 12:  // mutaciones del indice de grupo / limite entre llamadas
        setSite(); c.joinFlags = Rand(n, 136) % 2; c.counts[0] = Rand(n, 137) % 9;
        break;
    case 13:  // colores y referencias de blip
        c.joinFlags = 1; c.counts[0] = 1; c.colorA = Rand(n, 138); c.colorB = Rand(n, 139); c.colorC = Rand(n, 140); c.ref = Rand(n, 141); c.blip = Rand(n, 142);
        break;
    case 14:  // bytes altos de EAX basura en todos los booleanos
        c.memberHi = Rand(n, 143) << 8; c.joinHi = Rand(n, 144) << 8; c.joinFlags = Rand(n, 145) % 2;
        break;
    default:  // 15: mezcla
        if (Rand(n, 146) % 3 == 0) setSite();
        c.stat1 = Rand(n, 147) % 4 == 0 ? Stat(n, 148) : c.stat1; c.stat2 = Rand(n, 149) % 4 == 0 ? Stat(n, 150) : c.stat2;
        break;
    }
    // banderas globales: el caso normal necesita (tipo 8 o alguna bandera)
    if (c.family != 2 && c.pedType != 8 && !c.flagA && !c.flagB && !c.flagC) c.flagC = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("flags478", c.flags478).U("flags470", c.flags470).U("flags474", c.flags474).U("has_task", c.hasTask).U("ped_type", c.pedType).U("flag_a", c.flagA).U("flag_b", c.flagB).U("flag_c", c.flagC);
    j.U("group", c.group).U("limit", c.limit).U("other_player", c.otherPlayer).U("other_group", c.otherGroup).U("other_limit", c.otherLimit);
    j.U("is_member", c.isMember).U("member_hi", c.memberHi).U("max_members", c.maxMembers).U("stat1", c.stat1).U("stat2", c.stat2).U("join_flags", c.joinFlags).U("join_hi", c.joinHi);
    j.U("count0", c.counts[0]).U("count1", c.counts[1]).U("count2", c.counts[2]).U("count3", c.counts[3]);
    j.U("peds_group", c.pedsGroup).U("task_fail", c.taskFail).U("event_fail", c.eventFail).U("ref", c.ref).U("blip", c.blip).U("color_a", c.colorA).U("color_b", c.colorB).U("color_c", c.colorC);
    j.U("mut_site", c.mutSite).U("mut_group", c.mutGroup).U("mut_limit", c.mutLimit).U("mut_flags", c.mutFlags).U("mut_flag_470", c.mutFlagValue470).U("mut_flag_474", c.mutFlagValue474);
    return j.End();
}
}
