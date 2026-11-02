#pragma once
// Fixtures del banco Car84K (lote 84K): CCarCtrl::GenerateEmergencyServicesCar (EXE 0x42F9C0), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::GenerateEmergencyServicesCar  0x42F9C0  cdecl sin argumentos, void   (hook CCarCtrl / GenerateEmergencyServicesCar, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// La funcion no hace aritmetica float: solo compara contadores (suma de 32 bits y comparacion con signo), tiempos (sin signo) y copia tres palabras crudas de la posicion del herido / del fuego.
// Los falsos devuelven lo que dice el caso. Reglas de seguridad (el EXE no comprueba nada de esto): el nivel de zona (0xBA6718) esta en 0..7 (tablas de 8 modelos),
// el jugador siempre tiene datos de jugador y busqueda, y el herido (si lo hay) tiene un ped valido.
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

namespace C84KFix {
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

// Arenas: guarda + ventana + guarda
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPedWin = 0x500, kDataWin = 0x10, kWantWin = 0x40, kAccWin = 0x10, kAccPedWin = 0x40, kMatWin = 0x48, kFireWin = 0x20, kMgrWin = 0x10;
constexpr std::size_t kPedArena = Arena(kPedWin), kDataArena = Arena(kDataWin), kWantArena = Arena(kWantWin), kAccArena = Arena(kAccWin), kAccPedArena = Arena(kAccPedWin),
                      kMatArena = Arena(kMatWin), kFireArena = Arena(kFireWin), kMgrArena = Arena(kMgrWin);

constexpr u32 kAllowAddr = 0x8A5B28;      // CCarCtrl::bAllowEmergencyServicesToBeCreated (byte)
constexpr u32 kMaxAddr = 0x8A5B24;        // CCarCtrl::MaxNumberOfCarsInUse
constexpr u32 kCountsAddr = 0x969094;     // NumRandomCars, NumMissionCars (0x96909C), NumLawEnforcerCars (0x969098), NumParkedCars (0x9690A0), NumAmbulancesOnDuty (0x9690A8), NumFireTrucksOnDuty (0x9690AC), LastTimeAmbulanceCreated (0x9690B0), LastTimeFireTruckCreated (0x9690B4)
constexpr u32 kAreaAddr = 0xB72914;       // CGame::currArea
constexpr u32 kLevelAddr = 0xBA6718;      // CTheZones::m_CurrLevel
constexpr u32 kTimeAddr = 0xB7CB84;       // CTimer::m_snTimeInMilliseconds

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84011u); }
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

// ------------------------------------------------------------------------------------------------ JSON minimo
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

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 6> kNan{0xFFC00000u, 0x7FC00000u, 0x7FA00000u, 0xFFA12345u, 0x7F800001u, 0xFFFFFFFFu};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Una palabra "especial" (la funcion solo la copia): cero, desnormal, infinito, NaN de varias cargas (senalizadores incluidos), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return Pick(kNan, c, k + 1);
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 allow{}, gw{}, wl{}, area{}, level{};
    u32 amb{}, fire{}, parked{}, mission{}, law{}, random{}, maxc{};
    u32 lastA{}, lastF{}, now{};
    u32 nfree{};
    u32 player[3]{};
    u32 nearest{}, hasMat{};
    u32 mpos[3]{}, ppos[3]{};
    u32 sret[2]{}, gret[2]{};                // StreamAmbulanceAndMedic / StreamFireEngineAndFireman y GenerateOne: resultado por llamada (indice global, 0 = la primera que se hace)
    u32 nfires{}, firePresent{}, fpos[3]{};
    u32 gate{};
};

inline constexpr std::array<u32, 12> kWantedEdge{0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x80000001u, 0xFFFFFFFEu};
inline constexpr std::array<u32, 10> kBigCount{0u, 1u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFEu, 0x40000000u, 0xC0000000u, 0x00010000u, 0xFFFF0000u};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    u32 kk = 100;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    auto World = [&] { return B(float(R(6001)) / 10.0f - 300.0f); };
    auto Pos = [&](u32* v, bool special) { for (u32 k = 0; k < 3; ++k) v[k] = special && P(25) ? Special(n, nk()) : World(); };
    auto SmallCount = [&] { return R(10); };

    // ---- base aleatoria
    c.allow = P(85) ? 1u : 0u;
    c.gw = P(10) ? 1u : 0u;
    c.wl = P(70) ? R(4) : Pick(kWantedEdge, n, nk());
    c.area = P(80) ? 0u : (P(50) ? 1u + R(30) : Rand(n, nk()));
    c.level = P(85) ? 1u + R(3) : R(8);
    c.amb = P(45) ? 0u : (P(70) ? 1u + R(5) : Pick(kBigCount, n, nk()));
    c.fire = P(45) ? 0u : (P(70) ? 1u + R(5) : Pick(kBigCount, n, nk()));
    c.parked = SmallCount(); c.mission = SmallCount(); c.law = SmallCount(); c.random = SmallCount();
    c.maxc = P(80) ? 40u + R(100) : Pick(kBigCount, n, nk());
    c.lastA = P(85) ? R(1000000) : Rand(n, nk());
    c.lastF = P(85) ? R(1000000) : Rand(n, nk());
    c.now = P(50) ? c.lastA + 30001u + R(100000) : (P(50) ? c.lastF + 0x88B9u + R(100000) : R(2000000));
    c.nfree = P(75) ? R(6) : Pick(kWantedEdge, n, nk());
    Pos(c.player, true);
    c.nearest = P(70) ? 1u : 0u;
    c.hasMat = P(50) ? 1u : 0u;
    Pos(c.mpos, true); Pos(c.ppos, true);
    for (u32 k = 0; k < 2; ++k) { c.sret[k] = P(70) ? 1u : 0u; c.gret[k] = P(70) ? 1u : 0u; }
    c.nfires = P(60) ? 3u + R(5) : (P(50) ? R(3) : (P(50) ? Pick(std::array<u32, 6>{0xFFFFu, 0x8000u, 0x10000u, 0x10003u, 0xFFFF0002u, 0x12345678u}, n, nk()) : Rand(n, nk())));
    c.firePresent = P(70) ? 1u : 0u;
    Pos(c.fpos, true);

    // ---- piezas reutilizables
    auto Pass = [&] {           // todas las compuertas previas pasan
        c.allow = 1; c.gw = 0; c.wl = R(4); c.area = 0; c.level = 1 + R(3);
        c.parked = SmallCount(); c.mission = SmallCount(); c.law = SmallCount(); c.random = SmallCount();
        c.maxc = 60u + R(100);
        Pos(c.player, false);
    };
    auto AmbOK = [&] {          // ambulancia: todo bien
        c.amb = 0; c.nfree = 2u + R(4); c.nearest = 1; c.sret[0] = 1; c.gret[0] = 1;
        c.lastA = R(1000000); c.now = c.lastA + 30001u + R(100000);
        Pos(c.mpos, false); Pos(c.ppos, false);
    };
    auto FireOK = [&] {         // bomberos: todo bien (la primera llamada de Stream/GenerateOne puede ser la de la ambulancia: ambos indices a 1)
        c.fire = 0; c.nfires = 3u + R(5); c.firePresent = 1; c.sret[0] = 1; c.sret[1] = 1; c.gret[0] = 1; c.gret[1] = 1;
        c.lastF = R(1000000); c.now = std::max(c.now, c.lastF + 0x88B9u + R(100000));
        Pos(c.fpos, false);
    };
    switch (f) {
    case 0: break;
    case 1:   // ambulancia con exito, sin bomberos
        Pass(); AmbOK(); c.fire = 1 + R(3);
        break;
    case 2:   // ambulancia con exito, herido sin matriz (posicion compacta) o con matriz, posiciones especiales
        Pass(); AmbOK(); c.fire = 1 + R(3); c.hasMat = R(2);
        Pos(c.mpos, true); Pos(c.ppos, true); Pos(c.player, true);
        break;
    case 3:   // ambulancia: una condicion falla
        Pass(); AmbOK(); c.fire = 1 + R(3);
        switch (R(6)) {
        case 0: c.nearest = 0; break;
        case 1: c.sret[0] = 0; break;
        case 2: c.now = c.lastA + 30000u - R(3); break;
        case 3: c.gret[0] = 0; break;
        case 4: c.nfree = R(2); break;
        default: c.nfree = 1; c.sret[0] = R(2); break;
        }
        break;
    case 4:   // ambulancia: menos de dos accidentes libres (StreamAmbulanceAndMedic(false)), bomberos aleatorios
        Pass(); c.amb = 0; c.nfree = P(50) ? R(2) : Pick(std::array<u32, 6>{0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 0x80000001u, 1u, 0u}, n, nk());
        if (P(50)) FireOK();
        break;
    case 5:   // bomberos con exito (la ambulancia se salta o falla)
        Pass(); FireOK();
        c.amb = P(50) ? 1u + R(5) : 0u;
        if (c.amb == 0) { c.nfree = R(2); }
        break;
    case 6:   // bomberos: una condicion falla
        Pass(); FireOK(); c.amb = 1 + R(3);
        switch (R(6)) {
        case 0: c.nfires = R(3); break;
        case 1: c.firePresent = 0; break;
        case 2: c.sret[0] = 0; c.sret[1] = 0; break;
        case 3: c.now = c.lastF + 0x88B8u - R(3); break;
        case 4: c.gret[0] = 0; c.gret[1] = 0; break;
        default: c.nfires = 2; break;
        }
        break;
    case 7:   // las dos ramas a la vez, resultados internos aleatorios
        Pass(); c.amb = 0; c.fire = 0;
        c.nfree = R(5); c.nearest = P(80) ? 1u : 0u; c.nfires = R(6); c.firePresent = P(80) ? 1u : 0u;
        c.lastA = R(1000000); c.lastF = R(1000000); c.now = std::max(c.lastA + 30001u, c.lastF + 0x88B9u) + R(100000);
        for (u32 k = 0; k < 2; ++k) { c.sret[k] = R(2); c.gret[k] = R(2); }
        if (P(30)) c.now = R(2000000);
        break;
    case 8:   // retornos tempranos
        Pass(); AmbOK(); FireOK();
        switch (R(6)) {
        case 0: c.allow = 0; break;
        case 1: c.gw = 1; break;
        case 2: c.wl = 4u + R(3); break;
        case 3: c.area = P(50) ? 1u + R(8) : Rand(n, nk()) | 1u; break;
        case 4: c.level = 0; break;
        default: c.parked = 1000u; c.maxc = 5u + R(20); break;
        }
        break;
    case 9:   // fronteras del nivel de busqueda y de la suma de contadores
        Pass(); AmbOK(); FireOK();
        if (P(50)) {
            c.wl = Pick(kWantedEdge, n, nk());
        } else {
            const u32 w = R(7);
            const u32 sumOthers = c.amb + c.fire + c.parked + c.mission + c.law + c.random;
            if (w <= 1) c.maxc = sumOthers;                   // igual: continua
            else if (w == 5) c.maxc = sumOthers - 1u;         // uno menos: vuelve
            else if (w == 2) { c.parked = Pick(kBigCount, n, nk()); c.mission = Pick(kBigCount, n, nk()); c.maxc = P(50) ? 0u : Rand(n, nk()); }  // suma con desbordamiento
            else if (w == 3) { c.random = 0x7FFFFFFFu; c.law = 1u + R(2); c.maxc = 0x7FFFFFFFu; }
            else { c.maxc = 0x80000000u + R(3); }
        }
        break;
    case 10:  // fronteras de tiempo de la ambulancia (la suma `ultimo + 30000` desborda a 32 bits)
        Pass(); AmbOK(); c.fire = 1 + R(3);
        {
            const u32 w = R(7);
            if (w == 0) { c.lastA = 0xFFFFFFFFu - R(40000); c.now = R(40000); }
            else if (w == 1) { c.lastA = 0xFFFFFFFFu - 30000u; c.now = 0xFFFFFFFFu; }
            else if (w == 2) { c.now = c.lastA + 30000u; }
            else if (w == 3) { c.now = c.lastA + 30001u; }
            else if (w == 4) { c.now = c.lastA + 29999u; }
            else if (w == 5) { c.lastA = Rand(n, nk()); c.now = Rand(n, nk()); }
            else { c.lastA = 0x7FFFFFFFu - R(30010); c.now = 0x7FFFFFFFu + R(60000); }
        }
        break;
    case 11:  // fronteras de tiempo de los bomberos (+0x88B8)
        Pass(); FireOK(); c.amb = 1 + R(3);
        {
            const u32 w = R(7);
            if (w == 0) { c.lastF = 0xFFFFFFFFu - R(40000); c.now = R(40000); }
            else if (w == 1) { c.lastF = 0xFFFFFFFFu - 0x88B8u; c.now = 0xFFFFFFFFu; }
            else if (w == 2) { c.now = c.lastF + 0x88B8u; }
            else if (w == 3) { c.now = c.lastF + 0x88B9u; }
            else if (w == 4) { c.now = c.lastF + 0x88B7u; }
            else if (w == 5) { c.lastF = Rand(n, nk()); c.now = Rand(n, nk()); }
            else { c.lastF = 0x7FFFFFFFu - R(0x88C0); c.now = 0x7FFFFFFFu + R(0x11000); }
        }
        break;
    case 12:  // fronteras del numero de accidentes libres (con signo) y de fuegos (16 bits)
        Pass(); AmbOK(); FireOK();
        if (P(50)) c.nfree = Pick(kWantedEdge, n, nk());
        if (P(70)) c.nfires = Pick(std::array<u32, 12>{0u, 1u, 2u, 3u, 4u, 0xFFFFu, 0x8000u, 0x10000u, 0x10002u, 0xFFFF0003u, 0x00020000u, 0xABCD0002u}, n, nk());
        if (P(30)) c.amb = 1 + R(2);
        break;
    case 13:  // palabras especiales en las posiciones
        Pass(); AmbOK(); FireOK();
        Pos(c.player, true); Pos(c.mpos, true); Pos(c.ppos, true); Pos(c.fpos, true);
        c.hasMat = R(2); c.fire = P(50) ? 0u : 1u; c.amb = P(30) ? 1u : 0u;
        break;
    case 14:  // todos los niveles de zona (tablas de modelos por nivel) con las dos ramas
        Pass(); AmbOK(); FireOK();
        c.level = 1u + R(7); c.hasMat = R(2);
        c.amb = P(25) ? 1u : 0u;
        break;
    case 15:  // GenerateOne devuelve cero o uno por llamada, y se actualizan los ultimos tiempos
        Pass(); AmbOK(); FireOK();
        c.amb = 0; c.fire = 0; c.nfree = 2u + R(3); c.nfires = 3u + R(3);
        c.lastA = R(1000000); c.lastF = R(1000000); c.now = std::max(c.lastA + 30001u, c.lastF + 0x88B9u) + R(100000);
        c.gret[0] = R(2); c.gret[1] = R(2); c.sret[0] = P(90) ? 1u : 0u; c.sret[1] = P(90) ? 1u : 0u;
        break;
    case 16:  // aleatorio con exito frecuente y posiciones especiales
        Pass(); AmbOK(); FireOK();
        if (P(50)) c.amb = 1 + R(2);
        if (P(50)) c.fire = 1 + R(2);
        c.hasMat = R(2); c.level = 1u + R(7);
        Pos(c.mpos, true); Pos(c.ppos, true); Pos(c.fpos, true); Pos(c.player, true);
        break;
    default:  // 17: combinacion aleatoria con exito frecuente y todos los contadores extremos
        Pass(); AmbOK(); FireOK();
        if (P(30)) c.wl = Pick(kWantedEdge, n, nk());
        if (P(30)) c.nfree = Pick(kWantedEdge, n, nk());
        if (P(30)) { c.parked = Pick(kBigCount, n, nk()); c.maxc = Rand(n, nk()); }
        if (P(30)) c.now = Rand(n, nk());
        if (P(30)) c.nfires = Rand(n, nk());
        c.hasMat = R(2); c.level = R(8);
        break;
    }
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("allow", c.allow).U("gw", c.gw).U("wl", c.wl).U("area", c.area).U("level", c.level);
    j.U("amb", c.amb).U("fire", c.fire).U("parked", c.parked).U("mission", c.mission).U("law", c.law).U("random", c.random).U("maxc", c.maxc);
    j.U("lastA", c.lastA).U("lastF", c.lastF).U("now", c.now).U("nfree", c.nfree);
    j.A("player", std::vector<u32>(c.player, c.player + 3)).U("nearest", c.nearest).U("hasMat", c.hasMat);
    j.A("mpos", std::vector<u32>(c.mpos, c.mpos + 3)).A("ppos", std::vector<u32>(c.ppos, c.ppos + 3));
    j.A("sret", std::vector<u32>(c.sret, c.sret + 2)).A("gret", std::vector<u32>(c.gret, c.gret + 2));
    j.U("nfires", c.nfires).U("firePresent", c.firePresent).A("fpos", std::vector<u32>(c.fpos, c.fpos + 3));
    j.U("gate", c.gate);
    return j.End();
}
}
