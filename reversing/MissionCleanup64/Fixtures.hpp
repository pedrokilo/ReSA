#pragma once
// Fixtures del banco MissionCleanup64: CMissionCleanup::Process (0x468560) y CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects (0x4652D0). Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   Process  0x468560  thiscall (), ret simple, void. Unico llamador del EXE: 0x469471 (ECX = 0xA90850); despues `xor al,al`: nadie lee EAX/ECX/EDX.
//   CheckIf  0x4652D0  thiscall (), ret simple, void. Unico llamador del EXE: 0x46A058 (ECX = 0xA90850); la llamada siguiente no lee EAX/ECX/EDX.
//
// Cada caso es una descripcion SEMANTICA: la lista de 75 entradas de la limpieza (tipo y handle), los bytes de banderas de los tres pools, los datos de las entidades (CheckIf),
// el estado global real que lee Process, el jugador falso y las respuestas de los colaboradores sustituidos (mascaras / listas por numero de llamada).
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace MC64Fix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCasesP = 2048, kCasesC = 2048, kFns = 2;
constexpr u32 kEntries = 75, kSlots = 6, kVehSize = 0xA18, kPedSize = 0x7C4, kObjSize = 0x19C, kEnts = 3 * kSlots, kGuardWords = 4;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
// relleno de fondo de la memoria real: el byte del banco en la direccion a para el caso n (el oraculo lo reproduce)
constexpr u32 BgByte(u32 n, u32 a) { return (Rand(n + 0x6A09E667u, a) >> 8) & 0xFFu; }

// ------------------------------------------------------------------------------------------------ memoria real del EXE que Process lee o escribe
// (direccion, bytes): todos los operandos absolutos de 0x468560..0x468D0C, mas las filas de las tablas indexadas por los indices falsos 0..3 (idx*8 / idx*0x40 / idx*4 / idx*0x2D4 ...).
struct Region { u32 addr, size; };
inline constexpr Region kRegions[]{
    {0x86C6C0, 4}, {0x8A5B20, 4}, {0x8A625C, 4}, {0x8CCB84, 4}, {0x8CDE56, 1}, {0x8D0934, 4}, {0x8D0A24, 1}, {0x8D2530, 8},
    {0x9654B0, 1}, {0x96A7C8, 1}, {0x96A8A8, 1}, {0x96A8B0, 1}, {0x96AB90, 4}, {0x96C008, 1},
    {0xA43F68, 0x10}, {0xA44114, 0x1C}, {0xA44488, 2}, {0xA4448A, 1}, {0xA4448C, 8}, {0xA44494, 3}, {0xA44498, 8}, {0xA444A0, 4}, {0xA444A5, 1}, {0xA444A8, 1},
    {0xA447EC, 4}, {0xA447F0, 1}, {0xA4892C, 0x30}, {0xA44B60, 7},
    {0xB6EC2C, 1}, {0xB6F06D, 9}, {0xB6FD18, 1}, {0xB728E0, 4}, {0xB72914, 4}, {0xB72994, 1}, {0xB73458, 0x134},
    {0xB7CD74, 1}, {0xB7CDDD, 2}, {0xB7CE48, 4}, {0xB7CE74, 1}, {0xB7CEEB, 1}, {0xB7CFD8, 4},
    {0xBA18D9, 1}, {0xBA677C, 1}, {0xBAA3F8, 4}, {0xBAB1DC, 2}, {0xBB4A72, 1},
    {0xC0FCB2, 3}, {0xC0994C, 4}, {0xC09C20, 4}, {0xC09EF4, 4}, {0xC0A1C8, 4},
    {0xC17898, 4}, {0xC17928, 8}, {0xC17968, 8}, {0xC179A8, 8}, {0xC179E8, 8},
    {0xC1CC00, 2}, {0xC1CC18, 1}, {0xC3A1A0, 4}, {0xC7C70C, 2}, {0xC87ACD, 1}};
// filas de secuencias: [C178F0 + i*0x40 + 0x38, 8) para i = 0..3
inline constexpr u32 kSeqBase = 0xC178F0, kSeqStride = 0x40, kFxBase = 0xA44114, kGroupBase = 0xC0994C, kGroupStride = 0x2D4;
inline constexpr u32 kPtrPlayer = 0xB7CD98, kPtrPedPool = 0xB74490, kPtrVehPool = 0xB74494, kPtrObjPool = 0xB7449C;
inline constexpr u32 kPadBase = 0xB73458;

// kinds de colaborador (indice en los eventos). Los de la cola de Process 56..58 no se registran uno a uno: solo cuenta y hash.
enum : u32 {
    kFade = 1, kDisableTrains, kReleaseTrains, kAmbientPlanes, kFillZones, kClearGangWar, kRespraysOpen, kFpw, kRemovePickups, kRoadBlocks, kCopBikes, kReleaseNodes, kUnMark, kTidyNodes, kRecordings,
    kWideOff, kInitScriptable, kDucking, kPostFx, kExtraColour, kMissionAudio, kWeather, kSpecialChar, kSuppressed, kForceRender, kAllocator, kDecisionMaker, kAlwaysFollow, kNeverFollow, kReappear,
    kSpeech, kSpeechScript, kFindTask, kTaskFunc, kClearSkip, kFpp, kRespawn, kFireFlags, kStuckInit, kCamRestore, kSafe, kHelp, kCleanVeh, kCleanPed, kCleanObj, kGetIdx, kFxKill, kRemoveFx,
    kRemoveGroup, kFlush, kUnloadDm, kRemoveSearch, kRemoveCheckpoint, kRemoveTxd, kRemoveEntity, kPedFlag, kAcqOne, kAcqAll, kTaskType,
    kHasCol, kHaveIpl, kIsStatic, kMoving, kPlaceAuto, kPlaceBike, kKinds
};
inline constexpr bool IsTail(u32 kind) { return kind == kPedFlag || kind == kAcqOne || kind == kAcqAll; }

struct Ent { u32 flags{}, hasMat{}, area{}, vtype{}, pos[3]{}, mpos[3]{}; };
struct Spec {
    u32 fn{}, n{};
    u32 regs[4]{}, family{};
    i32 type[kEntries]{};
    u32 handle[kEntries]{};
    u32 flags[3][8]{};            // bytes de banderas de los pools (0 = vehiculos, 1 = peatones, 2 = objetos)
    Ent ent[kEnts]{};             // 0..5 vehiculos, 6..11 peatones, 12..17 objetos (solo CheckIf)
    u32 hcl{}, ipl{}, stat{};     // mascaras de resultado por numero de llamada (bit k & 31)
    // Process: estado real
    u32 c4448A{}, mode{}, g96AB91{}, g96AB90{}, v72914{}, g96A8B0{}, gA444A8{}, focus{}, money{}, f86C6C0{}, seq3C[4]{}, fx[4]{};
    // Process: jugador falso
    u32 state530{}, pd34{}, pd40{}, pdGroup{}, w14{}, w1C{}, p734{}, pd0Null{};
    i32 gsi[16]{};
    u32 hasTask{}, taskType{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

inline void FillList(Gen& g, Spec& s, u32 fam, bool dense) {
    static const i32 types[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0x7F, -1, 0, 1, 2, 3};
    u32 count = dense ? kEntries : (g.C(15) ? g.P(4) : g.P(kEntries));
    if (fam == 0 && g.C(50)) count = g.P(4);
    for (u32 i = 0; i < kEntries; ++i) {
        const bool live = i < count || g.C(5);
        s.type[i] = live ? types[g.P(sizeof(types) / sizeof(types[0]))] : 0;
        if (live && g.C(8)) s.type[i] = static_cast<i32>(g.R() & 0xFF);
        if (s.type[i] > 127) s.type[i] -= 256;
        const u32 slot = g.P(kSlots);
        const u32 pool = s.type[i] == 1 ? 0u : (s.type[i] == 2 ? 1u : 2u);
        u32 low = g.C(75) ? s.flags[pool][slot] : (g.R() & 0xFF);
        u32 h = (slot << 8) | low;
        if (s.type[i] != 1 && s.type[i] != 2 && s.type[i] != 3) h = g.C(50) ? (g.R() & 0xFFFFu) : g.R();
        s.handle[i] = h;
    }
}

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    Gen g(n + (fn ? 0x40000u : 0u));
    s.fn = fn;
    s.n = n;
    s.family = n % 8;
    const u32 fam = s.family;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Rand(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Rand(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Rand(n, 3) : g.R();
    for (u32 p = 0; p < 3; ++p)
        for (u32 i = 0; i < 8; ++i) s.flags[p][i] = g.C(25) ? (0x80u | (g.R() & 0x7Fu)) : (g.R() & 0x7Fu);
    s.hcl = g.C(10) ? ~0u : (g.C(10) ? 0u : g.R());
    s.ipl = g.C(10) ? ~0u : (g.C(10) ? 0u : g.R());
    s.stat = g.C(10) ? ~0u : (g.C(10) ? 0u : g.R());
    FillList(g, s, fam, fam == 1 || fam == 5);
    for (u32 i = 0; i < kEnts; ++i) {
        Ent& e = s.ent[i];
        static const u32 vt[]{0, 9, 11, 5, 1, 10, 0xFF, 0, 11, 9};
        e.flags = g.C(70) ? (g.R() | 0x40000u) : (g.R() & ~0x40000u);
        if (g.C(20)) e.flags = 0x40000u;
        e.hasMat = g.C(50) ? 1u : 0u;
        e.area = g.C(60) ? g.P(4) : (g.R() & 0xFF);
        e.vtype = g.C(10) ? g.R() : vt[g.P(10)];
        for (int k = 0; k < 3; ++k) { e.pos[k] = g.R(); e.mpos[k] = g.R(); }
    }
    // Process
    s.c4448A = g.C(50) ? 0u : (g.C(70) ? 1u : g.P(256));
    s.mode = g.C(25) ? 1u : (g.C(25) ? 2u : (g.C(50) ? 0u : g.P(256)));
    s.g96AB91 = g.C(50) ? 0u : (g.C(70) ? 1u : g.P(256));
    s.g96AB90 = g.C(50) ? 0u : (g.C(70) ? 1u : g.P(256));
    s.v72914 = g.C(50) ? 0u : g.R();
    s.g96A8B0 = g.C(25) ? 1u : (g.C(25) ? 2u : (g.C(40) ? 0u : g.P(256)));
    s.gA444A8 = g.C(55) ? 0u : (g.C(60) ? 1u : g.P(256));
    s.focus = g.C(80) ? 0u : 1u;
    s.money = g.C(50) ? 0u : g.R();
    s.f86C6C0 = g.R();
    for (int i = 0; i < 4; ++i) { s.seq3C[i] = g.C(50) ? 0u : g.R(); s.fx[i] = g.C(40) ? 0u : g.R() | 1u; }
    static const u32 states[]{0x36, 0x37, 0, 1, 0x35, 0x38, 0xFFFFFFFFu, 0x37, 0x36, 0x7F};
    s.state530 = g.C(70) ? states[g.P(10)] : g.R();
    s.pd34 = g.R();
    s.pd40 = g.R();
    s.pdGroup = g.C(70) ? g.P(4) : (g.C(50) ? 0u : g.P(4));
    s.w14 = g.R();
    s.w1C = g.R();
    s.p734 = g.R();
    static const i32 gs[]{-1, -2, 0, 1, 2, 3, 0, 1, 2, 3, 3, 2, 1, 0, -1, 0x7FFFFFFF};
    for (int i = 0; i < 16; ++i) s.gsi[i] = g.C(90) ? gs[g.P(15)] : gs[g.P(16)];
    if (g.C(25)) for (int i = 0; i < 16; ++i) if (s.gsi[i] > 3) s.gsi[i] = 3;
    for (int i = 0; i < 16; ++i) if (s.gsi[i] > 3) s.gsi[i] = 3;   // los indices altos leerian fuera de las filas del banco
    s.hasTask = g.C(60) ? 1u : 0u;
    s.taskType = g.C(70) ? 0x640u : (g.C(50) ? 0x63Fu : g.R());
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string I(i32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string ArrI(const i32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += I(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"n\":" + U(s.n) + ",\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"type\":" + ArrI(s.type, kEntries) + ",\"handle\":" + Arr(s.handle, kEntries) +
                    ",\"flags\":[" + Arr(s.flags[0], 8) + "," + Arr(s.flags[1], 8) + "," + Arr(s.flags[2], 8) + "],\"ent\":[";
    for (u32 i = 0; i < kEnts; ++i) {
        const Ent& e = s.ent[i];
        if (i) o += ',';
        o += "{\"flags\":" + U(e.flags) + ",\"mat\":" + U(e.hasMat) + ",\"area\":" + U(e.area) + ",\"vtype\":" + U(e.vtype) + ",\"pos\":" + Arr(e.pos, 3) + ",\"mpos\":" + Arr(e.mpos, 3) + "}";
    }
    o += "],\"hcl\":" + U(s.hcl) + ",\"ipl\":" + U(s.ipl) + ",\"stat\":" + U(s.stat) + ",\"c4448A\":" + U(s.c4448A) + ",\"mode\":" + U(s.mode) + ",\"g96AB91\":" + U(s.g96AB91) + ",\"g96AB90\":" + U(s.g96AB90) +
         ",\"v72914\":" + U(s.v72914) + ",\"g96A8B0\":" + U(s.g96A8B0) + ",\"gA444A8\":" + U(s.gA444A8) + ",\"focus\":" + U(s.focus) + ",\"money\":" + U(s.money) + ",\"f86C6C0\":" + U(s.f86C6C0) +
         ",\"seq3C\":" + Arr(s.seq3C, 4) + ",\"fx\":" + Arr(s.fx, 4) + ",\"state530\":" + U(s.state530) + ",\"pd34\":" + U(s.pd34) + ",\"pd40\":" + U(s.pd40) + ",\"pdGroup\":" + U(s.pdGroup) +
         ",\"w14\":" + U(s.w14) + ",\"w1C\":" + U(s.w1C) + ",\"p734\":" + U(s.p734) + ",\"gsi\":" + ArrI(s.gsi, 16) + ",\"hasTask\":" + U(s.hasTask) + ",\"taskType\":" + U(s.taskType) + "}";
    return o;
}
}  // namespace MC64Fix
