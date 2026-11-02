#pragma once
// Fixtures del banco Car84C (lote 84C): CCarCtrl::PickNextNodeToFollowPath (EXE 0x427740), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::PickNextNodeToFollowPath  0x427740  cdecl, un argumento (vehiculo), `ret` simple, devuelve bool   (hook CCarCtrl / PickNextNodeToFollowPath, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
// Mundo de caminos de juguete: 3 ranuras de area (id de area distinto en 0..63; las ranuras 0 y 1 siempre cargadas, la 2 casi siempre), 4 nodos por area, 16 enlaces de nodo y de navegacion por area y 8 enlaces de
// carretera (14 bytes) por area.
// Reglas de seguridad (el EXE no comprueba nada de esto): el nodo siguiente tiene el area cargada y el nodo es < 4; los identificadores de enlace de carretera (el siguiente del vehiculo, los de la tabla de
// navegacion y los de las mutaciones) estan en areas cargadas y son < 8; la base de enlaces de cada nodo es <= 6 (<= 13 con la mutacion de la base) y los tres primeros nodos de la ruta efectiva estan en las
// posiciones 13, 14 y 15 de la tabla de enlaces de cada ranura (el EXE busca el nodo elegido en la tabla del nodo actual sin limite de indices); el contador de la ruta nunca supera 8.
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

namespace C84CFix {
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

constexpr u32 kSlots = 3, kNodesPer = 4, kLinksPer = 16, kNaviPer = 16, kNavNodesPer = 8, kNodeBytes = 0x1C, kLinkBytes = 14;
// Arenas: guarda + ventana + guarda. La del vehiculo mide sizeof(CVehicle) = 0x5A0 (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x5A0;
constexpr std::size_t kMatWin = 0x80;      // la matriz del vehiculo: solo se lee la posicion (+0x30, +0x34, +0x38)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kNodeArena = Arena(kSlots * kNodesPer * kNodeBytes), kNlArena = Arena(kSlots * kLinksPer * 4), kNaviArena = Arena(kSlots * kNaviPer * 2),
                      kNnArena = Arena(kSlots * kNavNodesPer * kLinkBytes), kMatArena = Arena(kMatWin);

constexpr u32 kTimeAddr = 0xB7CB84;       // CTimer::m_snTimeInMilliseconds (SwitchVehicleToRealPhysics lo lee dos veces)
constexpr u32 kZoneAddr = 0xC81314;       // zona del clima (word): la funcion no la lee; el banco comprueba que no cambia
constexpr u32 kForbiddenAddr = 0x8A5F44;  // direccion de nodo invalida del EXE: nodo prohibido de DoPathSearch

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 84003u); }
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
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// Un float "especial": cero, desnormal, infinito, NaN (un unico patron), enorme
inline u32 Special(u32 c, u32 k) {
    const u32 r = Rand(c, k) % 32;
    if (r < 10) return Pick(kTiny, c, k + 1);
    if (r < 16) return Pick(kInf, c, k + 1);
    if (r < 24) return 0xFFC00000u;
    if (r < 28) return Pick(kBig, c, k + 1);
    return B(float(int(Rand(c, k + 3) % 2001) - 1000) / 7.0f);
}

// Valor flotante de la mutacion de la velocidad (el oraculo repite la tabla): tabla de especiales o (v % 2001 - 1000) / 8
inline u32 MutFloat(u32 v) {
    static constexpr std::array<u32, 8> t{0u, 0x80000000u, 1u, 0x7F800000u, 0xFF800000u, 0xFFC00000u, 0x7F7FFFFFu, 0x3F800000u};
    return (v & 3u) == 0 ? t[(v >> 2) % 8u] : B(float(int(v % 2001u) - 1000) / 8.0f);
}

// ------------------------------------------------------------------------------------------------ el caso
constexpr u32 kNoMutation = 99;
constexpr u32 kKinds = 20;
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 now{}, rseed{};
    u32 st[3]{};
    // mundo de caminos
    u32 areas[kSlots]{}, loaded[kSlots]{};
    u32 node[kSlots * kNodesPer][3]{};     // [x | y << 16, base | b18 << 16 | b19 << 24, b1A]
    u32 nlink[kSlots][kLinksPer]{};         // direcciones de nodo (area | nodo << 16)
    u32 navi[kSlots][kNaviPer]{};           // direcciones de enlace de carretera (id | area << 10)
    u32 lk[kSlots * kNavNodesPer][3]{};     // [x | y << 16, otroArea | otroNodo << 16, dx | dy << 8 | ancho << 16 | banderas << 24]
    // vehiculo y piloto
    u32 seed16{}, cur{}, start{}, end0{}, fieldC{}, scale{}, curLink{}, nextLink{}, prevLink{}, t3ac{}, t3b0{};
    u32 sm4{}, sm5{}, sm6{}, lane7{}, lane8{}, mission{}, tempact{}, speed{}, f3db{}, s36{}, b8{}, model{};
    // destino, posicion del vehiculo (matriz o transformacion simple)
    u32 dest[3]{}, hasMat{}, mat[3]{}, place[3]{};
    // ruta inicial (+0x418 contador, +0x3F8 nodos), salida del falso de DoPathSearch y del de CalcSpeedScaleFactor
    u32 pcount{}, pn[8]{};
    u32 cnt{}, ps[8]{}, csf{};
    u32 mutAt{}, mutKind{}, mutVal{};
    u32 gate{};
};

// Nodos de la ruta efectiva justo despues de la busqueda: la ruta inicial (con contador > 0 no hay busqueda) o la ruta inicial con los nodos que escribe el falso
inline void Effective(const C& c, u32 out[3]) {
    u32 arr[8];
    for (u32 i = 0; i < 8; ++i) arr[i] = c.pn[i];
    if (i16(c.pcount) <= 0) {
        const i16 n = i16(c.cnt);
        for (i16 i = 0; i < n && i < 8; ++i) arr[i] = c.ps[i];
    }
    out[0] = arr[0]; out[1] = arr[1]; out[2] = arr[2];
}

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
    auto Coord = [&]() -> u32 {   // int16 como u16
        const u32 w = R(100);
        if (w < 3) return 0x7FFFu;
        if (w < 6) return 0x8000u;
        if (w < 9) return 0u;
        return u32(i16(int(R(12001)) - 6000)) & 0xFFFFu;
    };
    // Coordenada del mundo (metros): del orden de las de los nodos (coordenadas / 8) o un especial
    auto World = [&](bool allowSpecial) -> u32 {
        const u32 w = R(100);
        if (allowSpecial && w < 7) return Special(n, nk());
        if (w < 14) return B(float(int(R(12001)) - 6000) * 0.125f);
        return B(float(int(R(12001)) - 6000) * 0.125f + float(R(8)) * 0.125f * 0.5f);
    };

    // ---- reloj (a veces cerca del desbordamiento de 32 bits), rand() del EXE y estaticas
    c.now = P(8) ? 0xFFFFFF00u + R(256) : 1000u + R(2000000u);
    c.rseed = Rand(n, nk());
    c.st[0] = Rand(n, nk()); c.st[1] = Rand(n, nk()); c.st[2] = Rand(n, nk());
    // ---- areas
    c.areas[0] = R(64);
    do { c.areas[1] = R(64); } while (c.areas[1] == c.areas[0]);
    do { c.areas[2] = R(64); } while (c.areas[2] == c.areas[0] || c.areas[2] == c.areas[1]);
    c.loaded[0] = 1; c.loaded[1] = 1; c.loaded[2] = P(85) ? 1u : 0u;
    auto LoadedSlot = [&]() -> u32 { for (;;) { const u32 s = R(kSlots); if (c.loaded[s]) return s; } };
    auto AddrLoaded = [&]() -> u32 { return c.areas[LoadedSlot()] | (R(kNodesPer) << 16); };
    auto AddrAny = [&]() -> u32 { return c.areas[R(kSlots)] | (R(kNodesPer) << 16); };
    auto LinkWord = [&]() -> u32 { return R(kNavNodesPer) | (c.areas[LoadedSlot()] << 10); };
    // ---- vehiculo y piloto
    c.seed16 = P(45) ? 0u : (1u + R(65535));
    c.cur = P(50) ? AddrAny() : Rand(n, nk());
    c.start = AddrLoaded();
    c.end0 = Rand(n, nk());
    c.nextLink = LinkWord();
    c.curLink = R(65536); c.prevLink = R(65536);
    c.fieldC = P(15) ? Rand(n, nk()) : R(100000); c.scale = P(10) ? 0xFFFFFFFFu : R(6000);
    c.t3ac = Rand(n, nk()); c.t3b0 = Rand(n, nk());
    c.sm4 = R(256); c.sm5 = P(80) ? (R(2) ? 1u : 0xFFu) : R(256); c.sm6 = P(80) ? (R(2) ? 1u : 0xFFu) : R(256);
    c.lane7 = P(70) ? R(5) : R(256); c.lane8 = P(70) ? R(5) : R(256);
    c.mission = R(256); c.tempact = R(256);
    c.speed = P(78) ? B(5.0f + float(R(116)) + float(R(8)) / 8.0f) : Special(n, nk());
    c.f3db = P(25) ? 0x08u : P(25) ? 0x10u : R(256);
    c.s36 = R(256); c.b8 = R(256);
    c.model = P(30) ? 0x21Bu : R(65536);
    // ---- destino y posicion
    c.dest[0] = World(true); c.dest[1] = World(true); c.dest[2] = World(true);
    c.hasMat = P(60) ? 1u : 0u;
    for (u32 k = 0; k < 3; ++k) { c.mat[k] = World(true); c.place[k] = World(true); }
    // ---- nodos
    for (u32 i = 0; i < kSlots * kNodesPer; ++i) {
        const u32 x = Coord(), y = Coord();
        const u32 base = R(7);
        c.node[i][0] = x | (y << 16);
        c.node[i][1] = base | (R(256) << 16) | (R(256) << 24);
        c.node[i][2] = P(40) ? (0x20u | R(16)) : R(256);
    }
    auto Flags = [&]() -> u32 { return P(30) ? ((1u + R(7)) | ((1u + R(7)) << 3)) : R(256); };
    for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kNaviPer; ++k) c.navi[s][k] = LinkWord();
    for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) {
        auto Dir = [&]() -> u32 { const u32 d = R(10); return d == 0 ? 0u : d == 1 ? 100u : d == 2 ? u32(u8(-100)) : d == 3 ? 127u : d == 4 ? 0x80u : u32(u8(int(R(201)) - 100)); };
        const u32 x = Coord(), y = Coord();
        c.lk[i][0] = x | (y << 16);
        c.lk[i][1] = Rand(n, nk());
        c.lk[i][2] = Dir() | (Dir() << 8) | (R(256) << 16) | (Flags() << 24);
    }
    // ---- ruta inicial y salida de los falsos
    {
        const u32 w = R(100);
        c.pcount = w < 32 ? 0u : w < 40 ? 0xFFFFu : w < 46 ? 0x8000u : w < 52 ? 0x8001u + R(0x7FFE) : 1u + R(8);
    }
    for (u32 i = 0; i < 8; ++i) { c.pn[i] = AddrAny(); c.ps[i] = AddrAny(); }
    {
        const u32 w = R(100);
        c.cnt = w < 10 ? 0u : w < 22 ? 1u : w < 40 ? 2u : w < 84 ? 3u + R(6) : w < 92 ? 0xFFFFu : 0x8000u;
    }
    if (P(35)) { c.ps[0] = c.start; if (P(50)) c.ps[1] = c.start; if (P(30)) c.ps[2] = c.start; }
    c.csf = P(85) ? B(0.5f + float(R(4000)) / 10.0f) : Special(n, nk());
    c.mutAt = P(30) ? 0u : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + R(kKinds);
    c.mutVal = P(25) ? Pick(kStackSeeds, n, nk()) : Rand(n, nk());

    // ---- sesgos por familia
    auto NoSearch = [&] { c.pcount = 1u + R(8); };
    auto Search = [&] { c.pcount = P(50) ? 0u : (P(50) ? 0xFFFFu : 0x8000u); };
    auto Other = [&](u32 avoid) -> u32 { u32 a; do { a = AddrAny(); } while (a == avoid); return a; };
    switch (f) {
    case 0: break;
    case 1: NoSearch(); break;
    case 2:   // busqueda con menos de 2 nodos: true
        Search(); c.cnt = (R(4) == 0) ? 0u : (R(3) == 0) ? 1u : (R(2) ? 0xFFFFu : 0x8000u);
        break;
    case 3:   // busqueda, el primer nodo no es el siguiente
        Search(); c.cnt = 2u + R(7); c.ps[0] = Other(c.start);
        break;
    case 4:   // busqueda, el primer nodo es el siguiente y el segundo no: se quita uno
        Search(); c.cnt = 2u + R(7); c.ps[0] = c.start; c.ps[1] = Other(c.start);
        break;
    case 5:   // busqueda, los dos primeros son el siguiente: se quitan dos
        Search(); c.cnt = 3u + R(6); c.ps[0] = c.start; c.ps[1] = c.start;
        break;
    case 6:   // cuenta justa: 2 con el primero (y el segundo) igual al siguiente, o 3
        Search(); c.cnt = P(50) ? 2u : 3u; c.ps[0] = c.start; c.ps[1] = P(50) ? c.start : Other(c.start); c.ps[2] = P(50) ? c.start : Other(c.start);
        break;
    case 7:   // sin busqueda, el elegido en el mismo area que el siguiente con el nodo menor o mayor
        NoSearch();
        c.pn[0] = (c.start & 0xFFFFu) | (R(kNodesPer) << 16);
        break;
    case 8:   // enlaces lejanos: el cuadrado de la distancia supera 256 casi siempre y se cambia el carril al azar
        if (P(60)) NoSearch(); else Search();
        for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) c.lk[i][0] = ((i & 1u) ? 0x0800u : 0xF800u) | (u32(i16((i & 2u) ? 1500 : -1500)) << 16);
        c.f3db = P(30) ? 0x08u : P(30) ? 0x10u : R(8);
        break;
    case 9:   // enlaces cercanos: cuadrado de la distancia <= 256
        if (P(60)) NoSearch(); else Search();
        for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) c.lk[i][0] = (u32(i16(int(R(41)) - 20)) & 0xFFFFu) | (u32(i16(int(R(41)) - 20)) << 16);
        break;
    case 10: case 11: case 12:   // barrido de mutaciones: cada tipo en casos que recorren varias etapas
        if (P(50)) NoSearch(); else { Search(); c.cnt = P(25) ? R(2) : 2u + R(7); }
        c.mutAt = 0;
        if (f == 10) c.mutKind = 1u + (n / 16) % 10u;
        else if (f == 11) c.mutKind = 11u + (n / 16) % 10u;
        else c.mutKind = 1u + R(kKinds);
        break;
    case 13:  // nodo elegido de tipo 2 (+0x1A & 0xF0 == 0x20): a la fisica real
        if (P(50)) NoSearch(); else { Search(); c.cnt = 2u + R(7); }
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) if (P(75)) c.node[i][2] = 0x20u | R(16);
        c.s36 = P(50) ? ((2u << 3) | R(8)) : R(256);
        break;
    case 14: case 15: break;   // todo aleatorio
    case 16:  // especiales (NaN, infinitos, desnormales, enormes) en velocidad, factor y posicion
        if (P(50)) NoSearch(); else { Search(); c.cnt = 2u + R(7); }
        c.speed = Special(n, nk()); c.csf = Special(n, nk());
        for (u32 k = 0; k < 3; ++k) { if (P(50)) c.dest[k] = Special(n, nk()); if (P(40)) c.mat[k] = Special(n, nk()); if (P(40)) c.place[k] = Special(n, nk()); }
        break;
    default:  // 17: carriles extremos, banderas y signos variados
        if (P(50)) NoSearch(); else { Search(); c.cnt = 2u + R(7); }
        c.lane7 = R(256); c.lane8 = R(256);
        c.f3db = P(30) ? 0x08u : P(30) ? 0x10u : R(256);
        c.sm5 = R(256); c.sm6 = R(256);
        for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) c.lk[i][2] = (c.lk[i][2] & 0x00FFFFFFu) | (R(256) << 24);
        break;
    }
    // ---- reglas de seguridad: la ruta efectiva (tres primeros nodos) esta en la tabla de enlaces de cada ranura, en 13, 14 y 15; el resto de la tabla son nodos al azar o esos tres
    {
        u32 x[3];
        Effective(c, x);
        for (u32 s = 0; s < kSlots; ++s) {
            for (u32 k = 0; k < 13; ++k) c.nlink[s][k] = P(30) ? x[R(3)] : AddrAny();
            const u32 rot = R(3);
            for (u32 k = 0; k < 3; ++k) c.nlink[s][13 + k] = x[(k + rot) % 3];
        }
    }
    // ---- reglas de seguridad: el nodo siguiente tiene el area cargada y el contador de la ruta nunca supera 8 (ya es asi); la mutacion tardia ocurre despues de la ultima busqueda
    // las mutaciones 15..20 modifican lo que se lee al final: ocurren tras la llamada a CalcSpeedScaleFactor (la segunda llamada a un falso si hubo busqueda, la primera si no)
    if (c.mutAt != kNoMutation && c.mutKind >= 15) c.mutAt = i16(c.pcount) <= 0 ? 1u : 0u;
    c.gate = 1;
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("now", c.now).U("rseed", c.rseed);
    j.A("st", std::vector<u32>(c.st, c.st + 3));
    j.A("areas", std::vector<u32>(c.areas, c.areas + kSlots)).A("loaded", std::vector<u32>(c.loaded, c.loaded + kSlots));
    {
        std::vector<u32> v;
        for (u32 i = 0; i < kSlots * kNodesPer; ++i) for (u32 k = 0; k < 3; ++k) v.push_back(c.node[i][k]);
        j.A("node", v);
    }
    {
        std::vector<u32> v;
        for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kLinksPer; ++k) v.push_back(c.nlink[s][k]);
        j.A("nlink", v);
    }
    {
        std::vector<u32> v;
        for (u32 s = 0; s < kSlots; ++s) for (u32 k = 0; k < kNaviPer; ++k) v.push_back(c.navi[s][k]);
        j.A("navi", v);
    }
    {
        std::vector<u32> v;
        for (u32 i = 0; i < kSlots * kNavNodesPer; ++i) for (u32 k = 0; k < 3; ++k) v.push_back(c.lk[i][k]);
        j.A("lk", v);
    }
    j.U("seed16", c.seed16).U("cur", c.cur).U("start", c.start).U("end0", c.end0).U("field_c", c.fieldC).U("scale", c.scale).U("cur_link", c.curLink).U("next_link", c.nextLink).U("prev_link", c.prevLink);
    j.U("t3ac", c.t3ac).U("t3b0", c.t3b0).U("sm4", c.sm4).U("sm5", c.sm5).U("sm6", c.sm6).U("lane7", c.lane7).U("lane8", c.lane8).U("mission", c.mission).U("tempact", c.tempact);
    j.U("speed", c.speed).U("f3db", c.f3db).U("s36", c.s36).U("b8", c.b8).U("model", c.model);
    j.A("dest", std::vector<u32>(c.dest, c.dest + 3)).U("has_mat", c.hasMat).A("mat", std::vector<u32>(c.mat, c.mat + 3)).A("place", std::vector<u32>(c.place, c.place + 3));
    j.U("pcount", c.pcount).A("pn", std::vector<u32>(c.pn, c.pn + 8)).U("cnt", c.cnt).A("ps", std::vector<u32>(c.ps, c.ps + 8)).U("csf", c.csf);
    j.U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("gate", c.gate);
    return j.End();
}
}
