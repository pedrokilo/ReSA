#pragma once
// Fixtures del banco Car84L (lote 84L): CCarCtrl::GenerateOneRandomCar (EXE 0x430050), 18432 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::GenerateOneRandomCar  0x430050..0x431F3A  cdecl sin argumentos, `ret` simple, void   (hook CCarCtrl / GenerateOneRandomCar, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// La funcion crea un coche aleatorio: lee contadores y reloj del juego, pregunta a ~40 colaboradores (sustituidos por falsos identicos en el EXE y en la DLL que devuelven lo que dice el caso) y
// escribe en el vehiculo que devuelve GetNewVehicleDependingOnCarModel (una arena con su matriz). El mundo de caminos son arenas del banco enlazadas desde ThePaths (cuatro tablas por area).
// Reglas de seguridad (el EXE no comprueba nada de esto): todo modelo que devuelven los falsos tiene su entrada en la tabla de modelos (cuatro arenas de modelo), todo nodo y tramo que lee el EXE
// pertenece a un area cargada con id < 8 y las tablas de enlaces de nodo y de navegacion tienen entradas validas.
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

namespace C84LFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 2;    // familias 16..17, 1024 casos cada una
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;
constexpr unsigned kSlots = 3, kLinks = 8, kLinkBytes = 14, kSlotWords = 28;   // 8 * 14 = 112 bytes = 28 palabras por area
constexpr unsigned kNodes = 8, kNodeBytes = 0x1C, kNL = 40;                   // nodos por area, bytes por nodo, entradas de enlaces por area
constexpr unsigned kVariants = 4;                                             // arenas de modelo (informacion de modelo + modelo de colision)
constexpr unsigned kTableEntry = 20;                                          // tabla 0x8E4CD0 (informacion de streaming por modelo): entradas de 20 bytes

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x9A0, kMatWin = 0x48, kVecWin = 0x10, kWantedWin = 0x40, kPvWin = 0x60, kZoneWin = 0x20, kTgtWin = 0x40, kPedWin = 0x500, kVtWin = 0x100,
                      kModelWin = 0x60, kColWin = 0x40, kLinkWin = kLinks * kLinkBytes, kNodeWin = kSlots * kNodes * kNodeBytes, kNlWin = kSlots * kNL * 4, kNaviWin = kSlots * kNL * 2;
constexpr std::size_t ArenaBytes(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = ArenaBytes(kVehWin), kMatArena = ArenaBytes(kMatWin), kVecArena = ArenaBytes(kVecWin), kWantedArena = ArenaBytes(kWantedWin), kPvArena = ArenaBytes(kPvWin),
                      kZoneArena = ArenaBytes(kZoneWin), kTgtArena = ArenaBytes(kTgtWin), kPedArena = ArenaBytes(kPedWin), kVtArena = ArenaBytes(kVtWin), kModelArena = ArenaBytes(kModelWin),
                      kColArena = ArenaBytes(kColWin), kLinkArena = ArenaBytes(kLinkWin), kNodeArena = ArenaBytes(kNodeWin), kNlArena = ArenaBytes(kNlWin), kNaviArena = ArenaBytes(kNaviWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84120u); }
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

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const u32* v, int count) { Key(k); s += '['; for (int i = 0; i < count; ++i) { if (i) s += ','; s += std::to_string(v[i]); } s += ']'; return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ globales del juego que la funcion lee o escribe (direccion, bytes)
struct GlobalDef { u32 addr; u32 size; };
inline constexpr std::array<GlobalDef, 31> kGlobals{{
    {0xB7CD74, 1}, {0x969094, 4}, {0x969098, 4}, {0x96909C, 4}, {0x9690A8, 4}, {0x9690AC, 4}, {0x8A5B20, 4}, {0x8A5B24, 4}, {0x9690B8, 4}, {0x9690BC, 4},
    {0xB7CB84, 4}, {0xB7CB4C, 4}, {0xB6F9B4, 4}, {0xB6F104, 4}, {0xB6F108, 4}, {0xB6F11C, 4}, {0xB6F03C, 4}, {0xB72914, 4}, {0xB72958, 1}, {0xB72959, 1},
    {0x96914F, 1}, {0x969176, 1}, {0x8CD76C, 2}, {0x8E6E68, 1}, {0xC0BC30, 4}, {0xC0BC34, 4}, {0xC0BC38, 4}, {0xC0BC3C, 4}, {0xC0BC64, 4}, {0xC0BC6C, 4}, {0x9690CC, 1}}};
enum GlobalId { gArea, gRandom, gLaw, gMission, gParked, gPermanent, gMult, gMultInt, gLastLaw, gMadTime, gTimeMs, gFrame, gCycle, gCamX, gCamY, gCamZ, gCamTarget, gCutscene,
                gB72958, gB72959, gMadSwitch, gB969176, gUpgrade, gB8E6E68, gC30, gC34, gC38, gC3C, gQuar, gQuarId, gB9690CC };

// Modelos que pueden devolver los falsos (todos tienen entrada en la tabla de modelos): los que la funcion compara, mas algunos comunes
inline constexpr std::array<u32, 28> kModels{0x1AE, 0x1AF, 0x1B5, 0x1EA, 0x1BE, 0x1C4, 0x1ED, 0x1A7, 0x1E4, 0x213, 0x214, 0x1C9, 0x1CF, 0x1CD, 0x209, 0x20A, 0x245, 0x24A, 0x1A4, 0x1A5, 0x1B3, 0x17F, 0x1D3, 0x254, 0x25C, 0x1CE, 0x1B1, 0x1AB};

// Mutaciones (tras el evento numero mutAt el falso muta el estado):
//  1 contador de seguidos (+0x590)  2 subtipo (+0x594)  3 banderas (+0x36)  4 nodo siguiente (+0x394)  5 tramo (+0x3A6)  6 modelo (+0x22)  7 conductor (+0x460)  8 velocidad de crucero (+0x3D0)
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 8;

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 g[kGlobals.size()]{};                    // valores iniciales de las globales
    u32 centre[3]{}, coors[3]{}, camPos[3]{};   // FindPlayerCentreOfWorld, FindPlayerCoors y posicion de la entidad de la camara (0xB6F03C o la estatica 0xB6F02C)
    u32 wLevel{}, wB18{}, wB19{}, wB1A{};       // CWanted: +0x2C, +0x18, +0x19, +0x1A
    u32 fewer{}, motor{}, gang{}, riots{}, policeModel{}, chooseModel{}, chosenPed{}, isBoat{};
    u32 pvPresent{}, pvSpeed[2]{};
    u32 ccOk{}, ccOrigin[3]{}, ccNa1[2]{}, ccNa2[2]{}, ccF11{};   // GenerateCarCreationCoors2: devuelve, origen, nodos (area, id) y float de salida
    u32 colCount{}, zoneFlags{}, pickModel{}, appear{}, onScreen{}, chase{}, convoy{}, wantedLevel{};
    u32 spd[4]{};                                // 0x41CA50, 0x41CAA0, 0x41CA40, 0x41C9D0
    u32 colours[4]{};
    u32 height{};                                // GetHeightAboveRoad (virtual +0xD4)
    u32 vl1[2]{}, vl2[2]{}, water[2]{};         // ProcessVerticalLine (golpe, z) dos veces, GetWaterLevel (devuelve, nivel)
    u32 pn[8]{};                                 // PickNextNodeRandomly: +0x394 nodo, +0x3A4/+0x3A6 tramos, +0x3B5/+0x3B6/+0x3B8 bytes, +0x39C, +0x3A0
    u32 vehModelOverride{};                      // 0: GetNewVehicle usa el modelo pedido; otro: ese
    u32 vehSeed{};                               // relleno de las partes del vehiculo que el caso no fija
    u32 veh[10]{};                               // +0x36 (byte), +0x42C..+0x42E (bytes), +0x590, +0x594, +0x460 (conductor presente), +0x4A6, +0x4C0, +0x18
    u32 mat[12]{};                               // matriz del vehiculo
    u32 mi[kVariants][4]{};                      // informacion de modelo: +0x3C, +0x50 (byte); modelo de colision: +4, +0x10, +0x24 -> mi[v][0]=+0x3C, [1]=+0x50, [2]=col +4, [3]=col +0x10, col +0x24 = miR
    u32 miR[kVariants]{};
    u32 tbl[28]{};                               // tabla 0x8E4CD0: byte 0 de la entrada de cada modelo de kModels
    u32 areas[kSlots]{};
    u32 links[kSlots][kSlotWords]{};             // tramos: 112 bytes por area
    u32 nodeW[kSlots][kNodes][7]{};              // nodos: x, y, z (int16 en +8, +0xA, +0xC), base de enlaces (int16 +0x10), bytes +0x18, +0x19, +0x1A
    u32 nl[kSlots][kNL]{};                       // enlaces de nodo (direcciones de nodo)
    u32 navi[kSlots][kNL]{};                     // enlaces de navegacion (direcciones de tramo, 16 bits)
    u32 mutAt{}, mutKind{}, mutVal{};
    u32 gate{};
};

inline u8 LinkByte(const C& c, u32 s, u32 k, u32 off) {
    const u32 pos = k * kLinkBytes + off;
    return u8(c.links[s][pos / 4] >> (8 * (pos % 4)));
}
inline void SetLinkByte(C& c, u32 s, u32 k, u32 off, u32 v) {
    const u32 pos = k * kLinkBytes + off;
    u32& w = c.links[s][pos / 4];
    w = (w & ~(0xFFu << (8 * (pos % 4)))) | ((v & 0xFFu) << (8 * (pos % 4)));
}
inline void SetLink16(C& c, u32 s, u32 k, u32 off, int v) { SetLinkByte(c, s, k, off, u32(v)); SetLinkByte(c, s, k, off + 1, u32(v) >> 8); }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.vehSeed = Rand(n, 8);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    const u32 sweep = n < kBaseCases ? n / 16 : (n - kBaseCases) % 1024;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto Pick = [&](const auto& a) { return a[Rand(n, nk()) % std::size(a)]; };
    auto Fl = [&](int lo, int hi, float div) -> u32 { return B(float(int(R(u32(hi - lo + 1))) + lo) / div); };
    // `deep` = probabilidad de pasar cada compuerta de salida temprana (varia por familia para llegar a todas las ramas)
    static constexpr u32 kDeep[16] = {99, 80, 99, 95, 99, 90, 99, 98, 92, 99, 85, 97, 99, 99, 93, 99};
    const u32 deep = f < 16 ? kDeep[f] : 95;
    auto Gate = [&] { return P(deep); };

    // ---- areas
    {
        u32 used[3];
        for (u32 s = 0; s < kSlots; ++s) {
            u32 a;
            bool dup;
            do {
                a = R(64);
                dup = false;
                for (u32 t = 0; t < s; ++t) dup |= used[t] == a;
            } while (dup);
            used[s] = a;
        }
        for (u32 s = 0; s < kSlots; ++s) c.areas[s] = used[s];
    }
    auto Word = [&](u32 s, u32 id) -> u32 { return (c.areas[s] << 10) | id; };
    auto LoadedWord = [&]() -> u32 { return Word(R(kSlots), R(kLinks)); };
    auto LoadedNodeId = [&](u32& area, u32& id) { area = c.areas[R(kSlots)]; id = R(kNodes); };

    // ---- globales
    {
        u32* g = c.g;
        g[gArea] = P(80) ? 0u : R(8);
        // contadores: la suma frente al limite (multiplicador * entero * float) decide la primera salida
        const u32 cap = Gate() ? 6u : 40u;
        g[gRandom] = R(cap); g[gLaw] = R(cap / 2 + 1); g[gMission] = R(cap / 3 + 1); g[gParked] = R(cap / 3 + 1); g[gPermanent] = R(cap / 3 + 1);
        g[gMult] = B(P(88) ? Pick(std::array<float, 3>{1.0f, 1.5f, 2.0f}) : Pick(std::array<float, 2>{0.5f, 0.25f}));
        g[gMultInt] = P(88) ? Pick(std::array<u32, 3>{40u, 80u, 500u}) : Pick(std::array<u32, 3>{5u, 20u, 60u});
        g[gLastLaw] = Rand(n, nk()) & 0x3FFFFu;
        g[gMadTime] = B(Pick(std::array<float, 6>{-5.0f, 0.0f, 100.0f, 400.0f, 800.0f, 5000.0f}));
        g[gTimeMs] = g[gLastLaw] + Pick(std::array<u32, 9>{0u, 4999u, 5000u, 5001u, 7999u, 8000u, 8001u, 100000u, 0xFFFFFFFFu});
        g[gFrame] = Rand(n, nk());
        g[gCycle] = B(Pick(std::array<float, 6>{0.0f, 0.5f, 1.0f, 3.0f, 8.0f, -1.0f}));
        g[gCamX] = Fl(-64, 64, 16.0f); g[gCamY] = Fl(-64, 64, 16.0f);
        g[gCamZ] = B(Pick(std::array<float, 6>{0.0f, 0.1f, 0.5f, 1.0f, 4.0f, 30.0f}));
        g[gCamTarget] = P(50) ? 1u : 0u;      // 1: apunta a la arena de la camara; 0: nulo (la estatica 0xB6F02C)
        g[gCutscene] = P(95) ? 0u : 1u;
        g[gB72958] = P(10) ? 1u : 0u; g[gB72959] = P(40) ? 1u : 0u; g[gMadSwitch] = P(40) ? 1u : 0u; g[gB969176] = P(30) ? 1u : 0u;
        g[gUpgrade] = Pick(std::array<u32, 4>{0x2A2u, 0x2A5u, 0x2A8u, 0x2B0u});
        g[gB8E6E68] = P(50) ? 1u : 0u;
        { const bool big = P(90); const u32 lo = big ? 12u : 0u; g[gC30] = B(float(R(40) + lo) / 4.0f); g[gC34] = B(float(R(40) + lo) / 4.0f); g[gC38] = B(float(R(40) + lo) / 4.0f); g[gC3C] = B(float(R(40) + lo) / 4.0f); }
        g[gQuar] = P(70) ? 0u : 1u;
        g[gQuarId] = Pick(std::array<u32, 5>{0x11u, 0x12u, 0x13u, 0x14u, 0x10u});
        g[gB9690CC] = Rand(n, nk()) & 0xFFu;
    }
    for (u32 k = 0; k < 3; ++k) { c.centre[k] = Fl(-3000, 3000, 8.0f); c.coors[k] = P(50) ? c.centre[k] : Fl(-3000, 3000, 8.0f); c.camPos[k] = Fl(-3000, 3000, 8.0f); }
    c.wLevel = P(75) ? 0u : R(7);
    c.wB18 = R(4); c.wB19 = R(6); c.wB1A = R(4);
    c.fewer = P(8) ? 1u : 0u;
    c.motor = B(P(90) ? Pick(std::array<float, 3>{1.0f, 2.0f, 4.0f}) : Pick(std::array<float, 3>{0.25f, 0.5f, 0.0f}));
    c.gang = P(10) ? 1u : 0u;
    c.riots = P(8) ? 1u : 0u;
    c.policeModel = Pick(std::array<u32, 6>{0x1A4u, 0x1A5u, 0x1B3u, 0x1D3u, 0x254u, 0x25Cu});
    c.chooseModel = P(4) ? 0xFFFFFFFFu : P(30) ? Pick(std::array<u32, 6>{0x1CFu, 0x1CDu, 0x209u, 0x20Au, 0x245u, 0x24Au}) : Pick(kModels);   // los seis primeros: camiones que pueden formar convoy
    c.chosenPed = P(30) ? Pick(std::array<u32, 2>{0xDu, 0x18u}) : R(24);
    c.isBoat = P(10) ? 1u : 0u;
    c.pvPresent = P(60) ? 1u : 0u;
    c.pvSpeed[0] = P(30) ? 0u : Fl(-60, 60, 8.0f); c.pvSpeed[1] = P(30) ? 0u : Fl(-60, 60, 8.0f);
    c.ccOk = Gate() ? 1u : 0u;
    for (u32 k = 0; k < 3; ++k) c.ccOrigin[k] = Fl(-3000, 3000, 8.0f);
    LoadedNodeId(c.ccNa1[0], c.ccNa1[1]);
    if (P(50)) { c.ccNa2[0] = c.ccNa1[0]; c.ccNa2[1] = (c.ccNa1[1] + 1 + R(kNodes - 1)) % kNodes; } else LoadedNodeId(c.ccNa2[0], c.ccNa2[1]);
    c.ccF11 = Pick(std::array<float, 4>{0.0f, 0.5f, 1.0f, 3.0f}) == 0.0f ? 0u : B(Pick(std::array<float, 3>{0.5f, 1.0f, 3.0f}));
    c.colCount = Gate() ? 0u : 1u + R(3);
    c.zoneFlags = Rand(n, nk()) & 0xFFu;
    c.pickModel = P(8) ? 0xFFFFFFFFu : Pick(kModels);
    c.appear = P(45) ? 4u : Pick(std::array<u32, 6>{0u, 1u, 2u, 3u, 4u, 5u});   // 4: el sorteo del conductor loco es 1 de 10
    c.onScreen = P(20) ? 1u : 0u;
    c.chase = P(40) ? 1u : 0u; c.convoy = P(40) ? 1u : 0u;
    c.wantedLevel = P(60) ? 0u : R(7);
    for (u32 k = 0; k < 4; ++k) c.spd[k] = R(256);
    for (u32 k = 0; k < 4; ++k) c.colours[k] = R(256);
    c.height = B(Pick(std::array<float, 5>{0.0f, 0.5f, 0.9f, 1.2f, -0.3f}));
    for (u32 k = 0; k < 2; ++k) {
        c.vl1[k] = k == 0 ? (P(75) ? 1u : 0u) : Fl(-200, 200, 4.0f);
        c.vl2[k] = k == 0 ? (P(50) ? 1u : 0u) : Fl(-200, 200, 4.0f);
    }
    c.water[0] = P(10) ? 1u : 0u; c.water[1] = Fl(-100, 100, 4.0f);
    // el suelo (z de los golpes) cerca de la altura de creacion (que sale de los nodos) para recorrer las comparaciones de altura
    c.pn[0] = LoadedWord() & 0xFFFFu;   // se rehace mas abajo con nodos validos
    for (u32 k = 1; k < 8; ++k) c.pn[k] = Rand(n, nk());
    c.vehModelOverride = P(10) ? Pick(kModels) : 0u;
    c.veh[0] = Rand(n, nk()) & 0xFFu;                    // +0x36
    c.veh[1] = Rand(n, nk()) & 0xFFu; c.veh[2] = Rand(n, nk()) & 0xFFu; c.veh[3] = Rand(n, nk()) & 0xFFu;   // +0x42C, +0x42D, +0x42E
    c.veh[4] = P(80) ? 0u : R(3);                        // +0x590
    c.veh[5] = P(30) ? Pick(std::array<u32, 3>{0xAu, 9u, 0u}) : R(20);   // +0x594
    c.veh[6] = P(40) ? 1u : 0u;                          // +0x460 conductor presente
    c.veh[7] = R(0x100); c.veh[8] = Rand(n, nk()); c.veh[9] = Rand(n, nk());
    for (u32 k = 0; k < 12; ++k) c.mat[k] = Fl(-16, 16, 16.0f);
    for (u32 v = 0; v < kVariants; ++v) {
        c.mi[v][0] = P(15) ? 0xAu : R(10); c.mi[v][1] = R(0x80);   // +0x3C: 0xA sale con mas de un carril
        c.mi[v][2] = Fl(-40, -8, 8.0f); c.mi[v][3] = Fl(8, 40, 8.0f);   // caja de colision: y minima negativa, maxima positiva (longitud del coche)
        c.miR[v] = Fl(8, 40, 8.0f);
    }
    for (u32 k = 0; k < 28; ++k) c.tbl[k] = P(92) ? 1u : R(4);
    c.mutAt = P(20) ? R(8) : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = Rand(n, nk());

    // ---- mundo de caminos
    for (u32 s = 0; s < kSlots; ++s) {
        for (u32 k = 0; k < kLinks; ++k) {
            for (u32 b = 0; b < kLinkBytes; ++b) SetLinkByte(c, s, k, b, R(256));
            SetLink16(c, s, k, 0, int(R(2000)) - 1000); SetLink16(c, s, k, 2, int(R(2000)) - 1000);
            SetLinkByte(c, s, k, 8, P(80) ? u32(int(R(201)) - 100) : R(256));
            SetLinkByte(c, s, k, 9, P(80) ? u32(int(R(201)) - 100) : R(256));
            if (P(92)) SetLinkByte(c, s, k, 0xB, (1u + R(7)) | ((1u + R(7)) << 3) | (P(50) ? 0x40u : 0u));   // carriles (bits 0-2 y 3-5) distintos de cero
        }
        for (u32 i = 0; i < kNodes; ++i) {
            u32* w = c.nodeW[s][i];
            w[0] = u32(int(R(2000)) - 1000) & 0xFFFFu; w[1] = u32(int(R(2000)) - 1000) & 0xFFFFu; w[2] = u32(int(R(200)) - 100) & 0xFFFFu;
            w[3] = R(9);
            w[4] = (P(50) ? 1u + R(7) : 1u + R(15)) | (P(50) ? 0x80u : 0u) | (P(30) ? 0x60u : 0u);   // +0x18: bit 7 (1AE), 0x0F cuenta de enlaces (>= 1: el EXE divide por ella)
            w[5] = Rand(n, nk()) & 0xFFu; w[6] = P(deep) ? (0xFu | (R(16) << 4)) : Rand(n, nk()) & 0xFFu;   // +0x1A: densidad en el nibble bajo (15: el sorteo siempre pasa)
        }
        for (u32 j = 0; j < kNL; ++j) {
            c.nl[s][j] = c.areas[R(kSlots)] | (R(kNodes) << 16);
            c.navi[s][j] = Word(R(kSlots), R(kLinks));
        }
    }
    // la funcion elige un enlace del nodo hasta que su direccion de tramo difiera de la primera elegida: si todos los `cuenta` enlaces del nodo (entradas base..base+cuenta-1) fueran iguales no terminaria
    for (u32 pass = 0; pass < 4; ++pass)
        for (u32 s = 0; s < kSlots; ++s)
            for (u32 i = 0; i < kNodes; ++i) {
                const u32 base = c.nodeW[s][i][3], cnt = c.nodeW[s][i][4] & 0xFu;
                if (cnt < 2) continue;
                bool same = true;
                for (u32 k = 1; k < cnt; ++k) same &= c.navi[s][base + k] == c.navi[s][base];
                if (same) c.navi[s][base + cnt - 1] = Word((((c.navi[s][base] >> 10) == c.areas[0]) ? 1u : 0u), R(kLinks));
            }
    // La creacion sobrevive a las comprobaciones de distancia y de suelo solo si el coche queda a menos de ~40 m del centro del jugador y el suelo (z de los golpes verticales) esta a menos de 7 m de la z
    // que sale de los nodos: con `near` los nodos se agrupan junto al centro del jugador y con `flat` todos tienen la misma z y los golpes caen cerca de ella
    {
        const bool closeBy = P(deep >= 95 ? 85 : 55), flat = P(85);
        const int z0 = int(R(25)) - 12;
        for (u32 s = 0; s < kSlots; ++s)
            for (u32 i = 0; i < kNodes; ++i) {
                if (closeBy) {
                    const int px = int(std::floor(F(c.centre[0]) * 8.0f)), py = int(std::floor(F(c.centre[1]) * 8.0f));
                    c.nodeW[s][i][0] = u32(std::clamp(px + int(R(321)) - 160, -32768, 32767)) & 0xFFFFu;
                    c.nodeW[s][i][1] = u32(std::clamp(py + int(R(321)) - 160, -32768, 32767)) & 0xFFFFu;
                    SetLink16(c, s, i, 0, std::clamp(px + int(R(321)) - 160, -32768, 32767));   // las posiciones de los tramos tambien (de ellas sale la posicion del coche)
                    SetLink16(c, s, i, 2, std::clamp(py + int(R(321)) - 160, -32768, 32767));
                }
                if (flat) c.nodeW[s][i][2] = u32(z0 * 8) & 0xFFFFu;
            }
        if (flat) { c.vl1[1] = B(float(z0) + float(int(R(15)) - 7) / 2.0f); c.vl2[1] = B(float(z0) + float(int(R(15)) - 7) / 2.0f); }
    }
    // el nodo que PickNextNodeRandomly deja como siguiente y los tramos: cargados
    {
        u32 a, id;
        LoadedNodeId(a, id);
        c.pn[0] = a | (id << 16);
        c.pn[1] = LoadedWord(); c.pn[2] = LoadedWord();
    }
    // un NaN con carga distinta del indefinido no se usa
    auto Clean = [](u32& v) { if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x7FFFFFu) != 0) v = 0xFFC00000u; };
    for (u32 k = 0; k < 3; ++k) { Clean(c.centre[k]); Clean(c.coors[k]); Clean(c.camPos[k]); Clean(c.ccOrigin[k]); }
    for (u32 k = 0; k < 12; ++k) Clean(c.mat[k]);
    Clean(c.veh[8]); Clean(c.veh[9]);
    c.gate = 1;
    (void)sweep;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("g", c.g, int(kGlobals.size()));
    j.U("fewer", c.fewer).U("motor", c.motor).U("gang", c.gang).U("riots", c.riots).U("police_model", c.policeModel).U("choose_model", c.chooseModel).U("chosen_ped", c.chosenPed);
    j.A("centre", c.centre, 3).A("coors", c.coors, 3).A("cam", c.camPos, 3).U("cc_ok", c.ccOk).A("cc_na1", c.ccNa1, 2).A("cc_na2", c.ccNa2, 2).U("col_count", c.colCount).U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("gate", c.gate);
    return j.End();
}
}
