#pragma once
// Fixtures del banco PedSaveExtract28: CPedSaveStructure::Extract (0x5D44B0, thiscall, `ret 4`, ECX = estructura de 0x18C bytes, 1 argumento: CPed* ped).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp y se compilan tambien aparte (fixture_dump.cpp, ver README.md) para contrastarlas con fixture_mirror.py.
// El oraculo (fixture_oracle.py) trabaja con la fila `input` que registra el banco; este archivo solo decide QUE casos se prueban.
//
// 4608 casos = 16 familias x 288. Familia = n % 16 (ver Make).
//  * La estructura guardada (0x18C bytes) se construye byte a byte; las trece armas llevan tipo (0 = se salta), estado, municion del cargador, municion total y relleno.
//  * El ped es una arena de 0x79C bytes (sin constructor) con relleno previo por modo (ceros / 0xFF / pseudoaleatorio / constante); +0x14 (matriz) lo fija el banco.
//  * La matriz del ped (si hay) es una arena de 0x48 bytes con relleno previo propio.
//  * Los modelos de cada arma los da una tabla por caso (tipo -> modelo1/modelo2) que sirve el falso de CWeaponInfo::GetWeaponInfo.
//  * El pool de entradas/salidas es un pool falso {almacenamiento, estados}; los estados son bytes [-16..2047] alrededor del puntero de estados; el indice de salida solo
//    cae dentro de ese rango (o es -1), asi el EXE (que no comprueba el rango) nunca lee fuera de la arena.
//  * Los reales posicion/vida/armadura: la posicion se genera sin NaN de senalizacion (el EXE la copia con fld/fstp, que convierte un SNaN en QNaN; ver README.md).
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace PedSaveExtract28Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kFamilies = 16;
constexpr unsigned kPerFamily = 288;
constexpr unsigned kCases = kFamilies * kPerFamily;   // 4608

// ------------------------------------------------------------------------------------------------ disposicion de memoria
constexpr u32 kPedSize = 0x79C;
constexpr u32 kSaveSize = 0x18C;
constexpr u32 kMatSize = 0x48;
constexpr u32 kGuard = 0x40;
constexpr u32 kPedAt = kGuard;
constexpr u32 kPedArena = kGuard + kPedSize + kGuard;
constexpr u32 kSaveAt = kGuard;
constexpr u32 kSaveArena = kGuard + kSaveSize + kGuard;
constexpr u32 kMatAt = kGuard;
constexpr u32 kMatArena = kGuard + kMatSize + kGuard;
constexpr i32 kStatesLow = -16;                    // el indice de salida va de -16 a 2047 (o es -1)
constexpr i32 kStatesHigh = 2047;
constexpr u32 kStatesGuard = 0x20;
constexpr u32 kStatesAt = kStatesGuard + 16;       // arena[kStatesAt] = estado del indice 0
constexpr u32 kStatesArena = kStatesGuard + 16 + 2048 + kStatesGuard;
constexpr u32 kMaxWeapons = 13;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno previo: modo 0 = ceros, 1 = 0xFF, 2 = pseudoaleatorio (semilla), 3 = constante.
inline void FillPre(u8* dst, std::size_t bytes, u32 mode, u32 seed, u32 constant) {
    switch (mode) {
    case 0: std::memset(dst, 0x00, bytes); break;
    case 1: std::memset(dst, 0xFF, bytes); break;
    case 2: FillWords(dst, bytes, seed); break;
    default: std::memset(dst, int(constant & 0xFF), bytes); break;
    }
}
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};

// Hash de 32 bits (FNV-1a) para los resumenes (traza y resto del ped): igual en C++ y en el oraculo.
inline u32 Fnv32(const u8* p, std::size_t n, u32 h = 2166136261u) { for (std::size_t i = 0; i < n; ++i) { h ^= p[i]; h *= 16777619u; } return h; }

struct Info {
    u32 type{};
    i32 model1{}, model2{};
};

struct Fx {
    unsigned n{};
    int family{};
    std::array<u8, kSaveSize> save{};
    bool hasMatrix{};
    u32 pedMode{}, pedSeed{}, pedConst{};
    u32 matMode{}, matSeed{}, matConst{};
    u32 statesMode{}, statesSeed{}, statesConst{};
    bool stateValid{};                      // el indice de salida cae en [-16, 2047]: se coloca ese byte de estado
    u32 stateByte{};
    i32 exitIdx{-1};
    std::vector<Info> infos;                // tabla tipo -> modelos (un elemento por tipo distinto no nulo)
    u32 eax{}, edx{}, stack{};
};

// ------------------------------------------------------------------------------------------------ generador
struct Gen {
    u32 c{}, k{};
    explicit Gen(u32 seed) : c(seed) {}
    u32 operator()(u32 m) { return m ? Rand(c, k++) % m : (++k, 0u); }
    u32 Raw() { return Rand(c, k++); }
    bool One(u32 den) { return (*this)(den) == 0; }
};

inline constexpr u32 kFloatNormal[] = {0x00000000u, 0x3F800000u, 0xBF800000u, 0x40490FDBu, 0x447A2000u, 0xC49A5400u, 0x41200000u, 0x3DCCCCCDu, 0x46FFFE00u, 0xC2C80000u, 0x43FA0000u, 0xC4FA0000u};
// Sin NaN de senalizacion (bit 22 puesto en todo NaN): -0, denormales, maximos, infinitos, NaN silenciosos con cargas utiles.
inline constexpr u32 kFloatEdge[] = {0x80000000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00800000u, 0x80800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F800000u, 0xFF800000u,
                                     0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0xFFC12345u, 0xFFE00000u};
inline constexpr u32 kU32Edge[] = {0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 25000u, 25001u, 99999u, 9999u, 30u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFEu, 0xFFFFFFFFu};
inline constexpr u32 kTypeEdge[] = {1u, 2u, 22u, 46u, 47u, 48u, 63u, 64u, 127u, 128u, 255u, 256u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu};
inline constexpr u32 kDupTypes[] = {22u, 30u, 0x80000000u};
inline constexpr i32 kIdEdge[] = {-1, -1, -1, 0, 1, 2, -2, -3, 300, 350, 400, 1000, 4000, 19999, 20000, 65535, 0x7FFFFFFF, i32(0x80000000u), i32(0x80000001u), -65536, 8, -4};
inline constexpr u32 kByteEdge[] = {0u, 1u, 2u, 3u, 4u, 0x7Fu, 0x80u, 0x81u, 0xFFu, 12u, 13u, 14u, 0x40u, 0xFEu};
inline constexpr u32 kSlotEdge[] = {0u, 1u, 2u, 7u, 0x7Fu, 0x80u, 0x81u, 0xFFu, 0xFEu, 12u, 13u, 0x40u};

constexpr u32 Count(std::size_t n) { return u32(n); }
// Real silenciado: un NaN de senalizacion pasa a NaN silencioso (bit 22).
inline u32 Quiet(u32 w) { return ((w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu) != 0u) ? (w | 0x00400000u) : w; }

inline Fx Make(unsigned n) {
    Fx f;
    f.n = n;
    f.family = int(n % kFamilies);
    const int fam = f.family;
    const unsigned j = n / kFamilies;
    Gen g(n * 7919u + 13u);
    auto put32 = [&](u32 off, u32 v) { std::memcpy(f.save.data() + off, &v, 4); };

    f.eax = g.Raw();
    f.edx = g.Raw();
    f.stack = kStackSeeds[g(8)];

    // --- posicion, vida, armadura
    const bool edgeFloat = fam == 7 || (fam == 13 && g.One(2)) || g.One(16);
    for (u32 k = 0; k < 3; ++k) {
        u32 w;
        if (edgeFloat) w = g.One(3) ? Quiet(g.Raw()) : kFloatEdge[g(Count(std::size(kFloatEdge)))];
        else w = g.One(4) ? Quiet(g.Raw()) : kFloatNormal[g(Count(std::size(kFloatNormal)))];
        put32(k * 4, w);
    }
    for (u32 k = 0; k < 2; ++k) {    // vida y armadura: copia de dwords, cualquier patron
        u32 w;
        if (edgeFloat) w = g.One(2) ? g.Raw() : kFloatEdge[g(Count(std::size(kFloatEdge)))];
        else w = g.One(3) ? g.Raw() : kFloatNormal[g(Count(std::size(kFloatNormal)))];
        put32(0xC + k * 4, w);
    }

    // --- armas
    int wm;   // 0 = ninguna, 1 = todas, 2 = dispersas, 3 = tipos extremos dispersos, 4 = tipos repetidos
    switch (fam) {
    case 0: wm = 0; break;
    case 1: case 12: wm = 1; break;
    case 3: wm = 3; break;
    case 4: wm = 1; break;
    case 5: wm = 4; break;
    case 13: wm = int(g(5)); break;
    case 15: wm = int(g(5)); break;
    default: wm = 2; break;
    }
    const bool ammoEdge = fam == 14 || g.One(8);
    for (u32 i = 0; i < kMaxWeapons; ++i) {
        const u32 off = 0x14 + 0x1C * i;
        u32 type = 0;
        switch (wm) {
        case 0: type = 0; break;
        case 1: type = 1 + g(46); break;
        case 2: type = g.One(2) ? 0u : 1 + g(46); break;
        case 3: type = g.One(4) ? 0u : kTypeEdge[g(Count(std::size(kTypeEdge)))]; break;
        default: type = g.One(5) ? 0u : kDupTypes[g(Count(std::size(kDupTypes)))]; break;
        }
        put32(off, type);
        put32(off + 4, g.Raw());
        put32(off + 8, ammoEdge ? kU32Edge[g(Count(std::size(kU32Edge)))] : (g.One(2) ? g(200) : g.Raw()));
        put32(off + 0xC, ammoEdge ? kU32Edge[g(Count(std::size(kU32Edge)))] : (g.One(2) ? g(1000) : g.Raw()));
        put32(off + 0x10, g.Raw());
        put32(off + 0x14, g.Raw());
        put32(off + 0x18, g.Raw());
    }
    // tabla de modelos: un elemento por tipo distinto no nulo
    const bool idEdge = fam == 4;
    for (u32 i = 0; i < kMaxWeapons; ++i) {
        u32 type;
        std::memcpy(&type, f.save.data() + 0x14 + 0x1C * i, 4);
        if (type == 0) continue;
        bool seen = false;
        for (const auto& e : f.infos) seen |= e.type == type;
        if (seen) continue;
        Info in;
        in.type = type;
        auto pick = [&]() -> i32 {
            if (idEdge || g.One(6)) return kIdEdge[g(Count(std::size(kIdEdge)))];
            return g.One(4) ? -1 : i32(300 + g(1200));
        };
        in.model1 = pick();
        in.model2 = pick();
        f.infos.push_back(in);
    }

    // --- cola de la estructura
    const bool byteEdge = fam == 6 || (fam == 13 && g.One(3));
    f.save[0x180] = u8(byteEdge ? kByteEdge[g(Count(std::size(kByteEdge)))] : (g.One(8) ? g(256) : g(4)));
    f.save[0x181] = u8(byteEdge ? kSlotEdge[g(Count(std::size(kSlotEdge)))] : g(13));
    f.save[0x182] = u8(byteEdge ? kByteEdge[g(Count(std::size(kByteEdge)))] : g(256));
    f.save[0x183] = u8(g(256));
    f.save[0x188] = u8(byteEdge ? kByteEdge[g(Count(std::size(kByteEdge)))] : g(256));
    f.save[0x189] = u8(byteEdge ? kByteEdge[g(Count(std::size(kByteEdge)))] : g(256));
    f.save[0x18A] = u8(g(256));
    f.save[0x18B] = u8(g(256));

    // --- indice de entrada/salida y estado del pool
    // modo: 0 = -1, 1 = rango (estado aleatorio), 3 = negativo -2..-16, 5 = cualquiera, 6 = rango usado, 7 = rango libre, 8 = fronteras
    int im;
    switch (fam) {
    case 0: case 3: case 7: case 11: im = 0; break;
    case 1: case 4: case 15: im = 1; break;
    case 2: case 5: case 12: case 14: im = 5; break;
    case 6: im = 3; break;
    case 8: im = 6; break;
    case 9: im = 7; break;
    case 10: im = 8; break;
    default: im = int(g(8)); if (im == 2 || im == 4) im = 5; break;
    }
    i32 idx = -1;
    u32 sb = g.Raw() & 0xFFu;
    switch (im) {
    case 0: idx = -1; break;
    case 1: idx = i32(g(2048)); break;
    case 3: idx = -i32(2 + g(15)); break;
    case 5: {
        const u32 p = g(6);
        idx = p == 0 ? -1 : (p == 1 ? -i32(2 + g(15)) : i32(g(2048)));
        break;
    }
    case 6: idx = i32(g(2048)); sb = g(128); break;
    case 7: idx = i32(g(2048)); sb = 0x80u | g(128); break;
    case 8: {
        static const i32 b[] = {-16, -2, -1, 0, 1, 2, 2046, 2047, 63, 64, 1023, 1024};
        idx = b[g(Count(std::size(b)))];
        static const u32 s[] = {0x00u, 0x7Fu, 0x80u, 0xFFu, 0x01u, 0x81u};
        sb = s[g(Count(std::size(s)))];
        break;
    }
    default: idx = -1; break;
    }
    if (im == 1 || im == 5) {
        static const u32 s[] = {0x00u, 0x7Fu, 0x80u, 0xFFu};
        if (g.One(3)) sb = s[g(4)];
    }
    f.exitIdx = idx;
    f.stateValid = idx != -1;
    f.stateByte = sb;
    put32(0x184, u32(idx));

    // --- ped, matriz y estados
    f.hasMatrix = fam == 11 || fam == 12 ? false : (fam == 0 || fam == 1 ? true : g.One(2));
    auto preMode = [&](u32& mode, u32& seed, u32& cst) {
        if (fam == 15) {
            mode = j % 4;
        } else {
            const u32 m = g(10);
            mode = m == 0 ? 0u : (m == 1 ? 1u : (m == 2 ? 3u : 2u));
        }
        seed = g.Raw();
        cst = g(256);
    };
    preMode(f.pedMode, f.pedSeed, f.pedConst);
    preMode(f.matMode, f.matSeed, f.matConst);
    preMode(f.statesMode, f.statesSeed, f.statesConst);
    return f;
}
}
