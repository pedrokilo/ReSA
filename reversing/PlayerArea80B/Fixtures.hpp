#pragma once
// Fixtures del banco PlayerArea80B (lote 80B): CPlayerPed::KeepAreaAroundPlayerClear (EXE 0x60C1E0, thiscall sin argumentos, ECX = jugador, `ret`, void).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CPlayerPed::KeepAreaAroundPlayerClear  0x60C1E0  ()   (hook CPlayerPed / KeepAreaAroundPlayerClear, tipo S)
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

namespace Ka80Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (n % 16), 1024 casos cada una
constexpr unsigned kFamSpecial = 1024;   // familia 16: coordenadas especiales (NaN, infinitos, desnormales, enormes, -0)
constexpr unsigned kFamBoundary = 1024;  // familia 17: distancias y productos escalares en el borde (25.0 y 0.0) con ulps
constexpr unsigned kFamMut = 1024;       // familia 18: barrido de mutaciones (16 tipos x 64 posiciones de la traza)
constexpr unsigned kFamVisits = 1024;    // familia 19: 16 ranuras llenas, muchas visitas al mismo peaton
constexpr unsigned kCases = kBaseCases + kFamSpecial + kFamBoundary + kFamMut + kFamVisits;
constexpr unsigned kFamilies = 20;
constexpr unsigned kFunctions = 1;
constexpr unsigned kPeds = 4, kVehs = 3, kSlots = 16, kList = 6;

// Arenas: guarda + ventana + guarda (la ventana es el objeto)
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kPedWin = 0x800, kVehWin = 0x600, kMatWin = 0x80, kIntelWin = 0x200, kTaskWin = 0x80, kEvWin = 0x40;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 80176u); }
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
    template<std::size_t N> Js& A(const char* k, const std::array<u32, N>& v) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < N; ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']'; return *this;
    }
    std::string End() const { return s + "}"; }
};

inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    // jugador (this)
    u32 pf46C{};                       // dword +0x46C (bit 0x100 = en vehiculo)
    u32 pVeh{};                        // m_pVehicle (+0x58C): 0 nulo, 1 vehiculo del jugador, 2 vehiculo 0 de la lista
    u32 pMat{};                        // matriz (+0x14): 0 nula, 1 la del jugador
    std::array<u32, 3> pPos{}, pMatPos{};   // posicion simple (+4) y de la matriz (+0x30)
    // vehiculo del jugador
    u32 vMat{};
    std::array<u32, 3> vPos{}, vMatPos{};
    // escaner (16 ranuras: 0 nulo, 1..4 peatones) y peatones
    std::array<u32, kSlots> slot{};
    std::array<u32, kPeds> created{}, f46C{}, f478{}, pintel{};
    std::array<u32, kPeds> alive{}, member{}, screen{}, task{}, evk{}, ptype{};
    u32 ftInit{};                      // valor inicial de [tarea hallada + 0xC] (fleeFrom)
    // busqueda de vehiculos
    u32 count{};                       // se escribe como int16 (bits bajos)
    std::array<u32, kList> list{};     // 0..2 vehiculo; 3 = nulo
    // vehiculos
    std::array<u32, kVehs> vcreated{}, vstatus{}, vmat{}, vtemp{};
    std::array<std::array<u32, 3>, kVehs> vpos{}, vmpos{};
    std::array<std::array<u32, 2>, kVehs> vfwd{};
    u32 timer{}, tol{};
    // mutacion
    u32 mutAt{}, mutKind{}, mutIdx{}, mutVal{};
    std::array<u32, 3> mutPos{};
};

inline u32 Mask(u32 n, u32 k) {
    switch (Rand(n, k) % 5) { case 0: return 0xFFFFFFFFu; case 1: return 0u; case 2: return Rand(n, k + 1); case 3: return Rand(n, k + 1) | Rand(n, k + 2); default: return Rand(n, k + 1) & Rand(n, k + 2); }
}
inline u32 Coord(u32 n, u32 k) { return B(float(i32(Rand(n, k) % 2001) - 1000) / 10.0f); }   // -100.0 .. 100.0
inline u32 FarCoord(u32 n, u32 k) { return B(float(1000 + Rand(n, k) % 1000) * (Rand(n, k + 1) % 2 ? 1.0f : -1.0f)); }
inline u32 SpecialCoord(u32 n, u32 k) {
    static constexpr u32 sp[]{0x00000000u, 0x80000000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0x7F800000u, 0xFF800000u, 0x7F7FFFFFu, 0xFF7FFFFFu,
                              0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x5F000000u, 0x5A000000u, 0x3F800000u, 0xBF800000u, 0x40A00000u, 0xC0A00000u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7F000000u};
    return sp[Rand(n, k) % std::size(sp)];
}
inline u32 Plus(u32 base, float off) { return B(F(base) + off); }

// Posicion de referencia que usa el EXE (sin mutaciones): la del vehiculo si el jugador esta en uno y tiene vehiculo, si no la propia
inline std::array<u32, 3> RefOf(const C& c) {
    if ((c.pf46C & 0x100u) && c.pVeh) return c.vMat ? c.vMatPos : c.vPos;
    return c.pMat ? c.pMatPos : c.pPos;
}
inline void Place(C& c, unsigned v, u32 n, u32 k, float dist) {   // situa el vehiculo v a ~dist de la referencia (en un octante aleatorio)
    const auto ref = RefOf(c);
    const float a = float(Rand(n, k) % 3) - 1.0f, b = float(Rand(n, k + 1) % 3) - 1.0f, d = float(Rand(n, k + 2) % 3) - 1.0f;
    const std::array<float, 3> off{a * dist, b * dist, d * dist};
    for (int i = 0; i < 3; ++i) c.vmpos[v][i] = Plus(ref[i], off[i]);
    for (int i = 0; i < 3; ++i) c.vpos[v][i] = Plus(ref[i], off[i] * 0.5f);
}
inline u32 Fwd(u32 n, u32 k) {
    static constexpr float sp[]{0.0f, -0.0f, 1.0f, -1.0f, 0.5f, -0.5f, 0.70710677f, -0.70710677f, 2.0f, 1.0e-30f};
    return Rand(n, k) % 3 ? B(sp[Rand(n, k + 1) % std::size(sp)]) : B(float(i32(Rand(n, k + 2) % 2001) - 1000) / 1000.0f);
}

inline C BaseDefaults(unsigned n) {
    C c;
    c.n = n; c.family = n % 16; c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.pf46C = Rand(n, 10); if (Rand(n, 11) % 3) c.pf46C &= ~0x100u; else c.pf46C |= 0x100u;
    c.pVeh = Rand(n, 12) % 4; if (c.pVeh == 3) c.pVeh = 1;
    c.pMat = Rand(n, 13) % 2; c.vMat = Rand(n, 14) % 2;
    for (int i = 0; i < 3; ++i) { c.pPos[i] = Coord(n, 20 + i); c.pMatPos[i] = Coord(n, 23 + i); c.vPos[i] = Coord(n, 26 + i); c.vMatPos[i] = Coord(n, 29 + i); }
    for (unsigned i = 0; i < kSlots; ++i) c.slot[i] = Rand(n, 40 + i) % 3 == 0 ? 1 + Rand(n, 60 + i) % kPeds : 0;
    for (unsigned p = 0; p < kPeds; ++p) {
        c.created[p] = Rand(n, 80 + p) % 6 ? 1u : Rand(n, 84 + p) % 4 == 0 ? 0x101u : Rand(n, 88 + p) % 5;
        c.f46C[p] = Rand(n, 92 + p); if (Rand(n, 96 + p) % 5) c.f46C[p] &= ~0x100u;
        c.f478[p] = Rand(n, 100 + p); if (Rand(n, 104 + p) % 5) c.f478[p] &= ~0x10u;
        c.pintel[p] = Rand(n, 108 + p) % 2;
        c.alive[p] = Mask(n, 112 + p * 3); c.member[p] = Mask(n, 113 + p * 3) & Rand(n, 114 + p * 3); c.screen[p] = Mask(n, 130 + p * 3) | Rand(n, 131 + p * 3);
        c.task[p] = Rand(n, 150 + p) % 2 ? 0u : Rand(n, 154 + p); c.evk[p] = Rand(n, 158 + p) % 2 ? 0u : Rand(n, 162 + p);
        static constexpr u32 types[]{0x38Fu, 0x38Eu, 0x390u, 0x38Fu | 0x100u, 0x38Fu | 0x80000000u, 0u, 0xFFFFFFFFu, 0x20u};
        c.ptype[p] = Rand(n, 166 + p) % 3 ? types[Rand(n, 170 + p) % std::size(types)] : Rand(n, 174 + p);
    }
    c.ftInit = Rand(n, 178);
    c.count = Rand(n, 179) % 8 == 0 ? 0u : (Rand(n, 180) % 10 == 0 ? (Rand(n, 181) % 2 ? 0xFFFFu : 0x8000u) : 1 + Rand(n, 182) % 6);
    for (unsigned i = 0; i < kList; ++i) c.list[i] = Rand(n, 183 + i) % 12 == 0 ? 3u : Rand(n, 190 + i) % 3;
    for (unsigned v = 0; v < kVehs; ++v) {
        c.vcreated[v] = Rand(n, 200 + v) % 5 == 0 ? 2u : Rand(n, 204 + v) % 3 ? 1u : Rand(n, 208 + v) % 6;
        static constexpr u32 st[]{2, 3, 4, 5, 6, 7, 8, 10, 11, 12, 1, 9, 0, 31, 16};
        c.vstatus[v] = (st[Rand(n, 212 + v) % std::size(st)] << 3) | (Rand(n, 216 + v) & 7u);
        c.vmat[v] = Rand(n, 220 + v) % 8 != 0;
        c.vtemp[v] = Rand(n, 224 + v);
        const float dist = Rand(n, 228 + v) % 3 == 0 ? 3.0f : Rand(n, 232 + v) % 2 ? 4.0f : 10.0f * float(1 + Rand(n, 236 + v) % 4);
        Place(c, v, n, 240 + v * 3, dist);
        c.vfwd[v][0] = Fwd(n, 250 + v * 3); c.vfwd[v][1] = Fwd(n, 260 + v * 3);
    }
    c.timer = Rand(n, 270) % 6 == 0 ? 0xFFFFFFFFu - Rand(n, 271) % 6000 : Rand(n, 272);
    c.tol = Rand(n, 273) % 4 == 0 ? B(1.0f) : Rand(n, 274) % 4 == 0 ? 0x7F800001u : Rand(n, 275);
    return c;
}
inline void FixNullMats(C& c) {   // el vehiculo 0 siempre tiene matriz (puede acabar siendo la referencia por pVeh == 2 o la mutacion 3); sin matriz el EXE solo sobrevive si el vehiculo esta lejos (se cae al leer [matriz + 0x10]): lo alejamos del todo
    c.vmat[0] = 1;
    for (unsigned v = 1; v < kVehs; ++v) if (!c.vmat[v]) for (int i = 0; i < 3; ++i) c.vpos[v][i] = FarCoord(c.n, 300 + v * 3 + i);
}
inline void FixList(C& c) {   // bajo la cuenta (int16 con signo) la lista no puede tener entradas nulas: el EXE se cae al leer [0 + 0x4A4]
    const int cnt = int(std::int16_t(c.count & 0xFFFFu));
    for (int i = 0; i < int(kList) && i < cnt; ++i) if (c.list[i] >= kVehs) c.list[i] = Rand(c.n, 950 + i) % kVehs;
}

inline void Mutation(C& c, unsigned n, u32 at, u32 kind) {
    c.mutAt = at; c.mutKind = kind; c.mutIdx = Rand(n, 320); c.mutVal = Rand(n, 321);
    if (Rand(n, 322) % 3 == 0) c.mutVal &= 1;
    for (int i = 0; i < 3; ++i) c.mutPos[i] = Coord(n, 323 + i);   // finitas y acotadas: un vehiculo sin matriz no debe quedar nunca cerca
    c.mutPos[0] = Rand(n, 326) % 4 == 0 ? B(0.0f) : c.mutPos[0];
}

inline C MakeExtra0(unsigned n) {
    C c = BaseDefaults(n);
    const unsigned j = n - kBaseCases;
    if (j < kFamSpecial) {   // familia 16: valores especiales; todos los vehiculos con matriz
        c.family = 16;
        for (unsigned v = 0; v < kVehs; ++v) c.vmat[v] = 1;
        auto sp = [&](u32& x, u32 k) { if (Rand(n, k) % 2) x = SpecialCoord(n, k + 1); };
        for (int i = 0; i < 3; ++i) { sp(c.pPos[i], 400 + i * 2); sp(c.pMatPos[i], 410 + i * 2); sp(c.vPos[i], 420 + i * 2); sp(c.vMatPos[i], 430 + i * 2); }
        for (unsigned v = 0; v < kVehs; ++v) {
            for (int i = 0; i < 3; ++i) { sp(c.vmpos[v][i], 440 + v * 8 + i * 2); sp(c.vpos[v][i], 470 + v * 8 + i * 2); }
            c.vfwd[v][0] = Rand(n, 500 + v) % 2 ? SpecialCoord(n, 503 + v) : c.vfwd[v][0];
            c.vfwd[v][1] = Rand(n, 510 + v) % 2 ? SpecialCoord(n, 513 + v) : c.vfwd[v][1];
        }
        c.count = 1 + Rand(n, 520) % 6;
        for (unsigned v = 0; v < kVehs; ++v) { c.vcreated[v] = 1; c.vstatus[v] = (2u + Rand(n, 530 + v) % 6) << 3; }
        c.mutAt = 0;
        return c;
    }
    if (j < kFamSpecial + kFamBoundary) {   // familia 17: bordes
        c.family = 17;
        for (unsigned v = 0; v < kVehs; ++v) { c.vmat[v] = 1; c.vcreated[v] = 1; c.vstatus[v] = (2u + Rand(n, 540 + v) % 6) << 3; }
        c.count = 1 + Rand(n, 541) % 6;
        c.pf46C &= ~0x100u; c.pMat = Rand(n, 542) % 2;
        c.pPos = {Coord(n, 543), Coord(n, 544), Coord(n, 545)}; c.pMatPos = {Coord(n, 546), Coord(n, 547), Coord(n, 548)};
        const auto ref = RefOf(c);
        static constexpr float offs[][3]{{3, 4, 0}, {0, 3, 4}, {4, 0, 3}, {5, 0, 0}, {0, -5, 0}, {0, 0, 5}, {-3, 0, -4}, {2.5f, 2.5f, 3.5355339f}, {1, 2, 4.5825756f}, {0, 0, 0}, {5.001f, 0, 0}, {4.999f, 0, 0}, {0, 0, 0.1f}, {4.9f, 0.1f, 0.5f}, {3, 4, 1e-4f}, {3, 4, -1e-4f}};
        for (unsigned v = 0; v < kVehs; ++v) {
            const auto& o = offs[Rand(n, 550 + v) % std::size(offs)];
            const int ulps = int(Rand(n, 553 + v) % 5) - 2;
            for (int i = 0; i < 3; ++i) {
                u32 p = B(F(ref[i]) + o[i]);
                p += u32(ulps) * (Rand(n, 556 + v * 3 + i) % 3 == 0 ? 1u : 0u);   // +-2 ulps en algunos ejes
                c.vmpos[v][i] = p;
            }
            // direccion perpendicular o paralela al desplazamiento para que el producto escalar sea 0, -0 o cercano
            const float fx = F(c.vmpos[v][0]) - F(ref[0]), fy = F(c.vmpos[v][1]) - F(ref[1]);
            switch (Rand(n, 565 + v) % 6) {
            case 0: c.vfwd[v] = {B(-fy), B(fx)}; break;
            case 1: c.vfwd[v] = {B(fy), B(-fx)}; break;
            case 2: c.vfwd[v] = {B(fx), B(fy)}; break;
            case 3: c.vfwd[v] = {B(-fx), B(-fy)}; break;
            case 4: c.vfwd[v] = {B(0.0f), B(-0.0f)}; break;
            default: break;
            }
        }
        c.mutAt = 0;
        return c;
    }
    if (j < kFamSpecial + kFamBoundary + kFamMut) {   // familia 18: barrido de mutaciones
        c.family = 18;
        const unsigned m = j - kFamSpecial - kFamBoundary;
        for (unsigned i = 0; i < kSlots; ++i) if (Rand(n, 580 + i) % 3 == 0) c.slot[i] = 1 + Rand(n, 590 + i) % kPeds;
        for (unsigned p = 0; p < kPeds; ++p) { c.created[p] = 1; c.f46C[p] &= ~0x100u; c.f478[p] &= ~0x10u; c.alive[p] |= Rand(n, 600 + p); c.member[p] &= Rand(n, 604 + p); c.screen[p] |= Rand(n, 608 + p); }
        c.count = 1 + Rand(n, 612) % 6;
        for (unsigned v = 0; v < kVehs; ++v) { c.vcreated[v] = 1; c.vstatus[v] = (2u + Rand(n, 613 + v) % 6) << 3; }
        Mutation(c, n, 1 + m % 64, 1 + m / 64);
        FixNullMats(c);
        return c;
    }
    c.family = 19;   // familia 19: 16 ranuras llenas
    for (unsigned i = 0; i < kSlots; ++i) c.slot[i] = 1 + Rand(n, 620 + i) % kPeds;
    for (unsigned p = 0; p < kPeds; ++p) { c.created[p] = Rand(n, 630 + p) % 8 ? 1u : 2u; c.alive[p] = Mask(n, 634 + p); c.member[p] = Rand(n, 640 + p) & Rand(n, 644 + p) & Rand(n, 648 + p); }
    FixNullMats(c);
    return c;
}

inline C MakeC0(unsigned n) {
    if (n >= kBaseCases) return MakeExtra0(n);
    C c = BaseDefaults(n);
    auto allPass = [&] {   // los peatones superan todos los filtros
        for (unsigned p = 0; p < kPeds; ++p) { c.created[p] = 1; c.f46C[p] &= ~0x100u; c.alive[p] = 0xFFFFFFFFu; c.member[p] = 0; c.screen[p] = 0xFFFFFFFFu; c.f478[p] &= ~0x10u; c.task[p] = 0; c.evk[p] = 0; }
    };
    auto fill = [&] { for (unsigned i = 0; i < kSlots; ++i) if (Rand(n, 700 + i) % 3 == 0) c.slot[i] = 1 + Rand(n, 720 + i) % kPeds; };
    auto noVehicles = [&] { c.count = Rand(n, 740) % 2 ? 0u : 0xFFFFu; };
    auto okVeh = [&](unsigned v) { c.vcreated[v] = 1; c.vstatus[v] = (2u + Rand(n, 741 + v) % 6) << 3; };
    switch (c.family) {
    case 0: break;
    case 1: allPass(); fill(); noVehicles(); break;
    case 2: fill(); noVehicles(); break;                                                      // filtros de peatones aleatorios
    case 3: allPass(); fill(); noVehicles(); for (unsigned p = 0; p < kPeds; ++p) { c.task[p] = Rand(n, 750 + p); c.evk[p] = Rand(n, 754 + p); } break;
    case 4: allPass(); fill(); for (unsigned i = 0; i < kSlots; ++i) if (Rand(n, 760 + i) % 2) c.slot[i] = c.slot[(i + 1) % kSlots] ? c.slot[(i + 1) % kSlots] : 1; noVehicles(); break;
    case 5: c.pf46C &= ~0x100u; c.pMat = Rand(n, 770) % 2; c.pVeh = Rand(n, 771) % 3; for (unsigned v = 0; v < kVehs; ++v) { okVeh(v); } c.count = 1 + Rand(n, 772) % 6; break;
    case 6: c.pf46C |= 0x100u; c.pVeh = Rand(n, 773) % 5 == 0 ? 0u : 1 + Rand(n, 774) % 2; for (unsigned v = 0; v < kVehs; ++v) okVeh(v); c.count = 1 + Rand(n, 775) % 6; break;
    case 7: c.count = Rand(n, 776) % 9 == 0 ? 0xFFFFu : Rand(n, 777) % 7; for (unsigned v = 0; v < kVehs; ++v) { if (Rand(n, 778 + v) % 2) okVeh(v); } break;
    case 8: for (unsigned v = 0; v < kVehs; ++v) { okVeh(v); Place(c, v, n, 790 + v * 3, 10.0f + float(Rand(n, 793 + v) % 40)); } c.count = 1 + Rand(n, 800) % 6; break;
    case 9: for (unsigned v = 0; v < kVehs; ++v) { okVeh(v); c.vmat[v] = 1; Place(c, v, n, 810 + v * 3, float(Rand(n, 813 + v) % 5)); } c.count = 1 + Rand(n, 820) % 6; break;
    case 10: for (unsigned v = 0; v < kVehs; ++v) { okVeh(v); c.vmat[v] = 1; Place(c, v, n, 830 + v * 3, 5.0f); c.vfwd[v][0] = Rand(n, 840 + v) % 2 ? B(0.0f) : B(-0.0f); } c.count = 1 + Rand(n, 850) % 6; break;
    case 11: for (unsigned v = 0; v < kVehs; ++v) okVeh(v); c.count = 6; for (unsigned i = 0; i < kList; ++i) c.list[i] = Rand(n, 860 + i) % 2; if (Rand(n, 870) % 2) { c.pf46C |= 0x100u; c.pVeh = 2; } break;
    case 12: allPass(); fill(); noVehicles(); c.tol = Rand(n, 880) % 2 ? 0x7F800001u : 0xFFBFFFFFu; if (Rand(n, 881) % 2) c.tol = B(0.0f); break;
    case 13: Mutation(c, n, 1 + Rand(n, 890) % 40, Rand(n, 891) % 2 ? 1 + Rand(n, 892) % 5 : 16); fill(); for (unsigned p = 0; p < kPeds; ++p) { c.alive[p] |= Rand(n, 895 + p); c.screen[p] |= Rand(n, 899 + p); } for (unsigned v = 0; v < kVehs; ++v) okVeh(v); c.count = 1 + Rand(n, 903) % 6; break;
    case 14: Mutation(c, n, 1 + Rand(n, 904) % 40, 6 + Rand(n, 905) % 5); allPass(); fill(); c.alive[0] = c.alive[1] = 0xFFFFFFFFu; noVehicles(); break;
    default: Mutation(c, n, 1 + Rand(n, 906) % 12, 11 + Rand(n, 907) % 5); for (unsigned v = 0; v < kVehs; ++v) okVeh(v); c.count = 1 + Rand(n, 908) % 6; allPass(); fill(); break;
    }
    if (c.family != 13 && c.family != 14 && c.family != 15 && c.family != 0) c.mutAt = Rand(n, 909) % 8 == 0 ? 1 + Rand(n, 910) % 30 : 0;
    if (c.mutAt && !c.mutKind) Mutation(c, n, c.mutAt, 1 + Rand(n, 911) % 16);
    FixNullMats(c);
    return c;
}

inline C MakeC(unsigned n) {
    C c = MakeC0(n);
    c.vmat[0] = 1;
    FixList(c);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("pf46C", c.pf46C).U("pveh", c.pVeh).U("pmat", c.pMat).A("ppos", c.pPos).A("pmatpos", c.pMatPos).U("vmat", c.vMat).A("vpos", c.vPos).A("vmatpos", c.vMatPos);
    j.A("slot", c.slot).A("created", c.created).A("f46C", c.f46C).A("f478", c.f478).A("pintel", c.pintel);
    j.A("alive", c.alive).A("member", c.member).A("screen", c.screen).A("task", c.task).A("evk", c.evk).A("ptype", c.ptype).U("ftinit", c.ftInit);
    j.U("count", c.count).A("list", c.list).A("vcreated", c.vcreated).A("vstatus", c.vstatus).A("vmat_has", c.vmat).A("vtemp", c.vtemp);
    for (unsigned v = 0; v < kVehs; ++v) {
        const std::string a = "vehpos" + std::to_string(v), b = "vehmpos" + std::to_string(v), d = "vehfwd" + std::to_string(v);
        j.A(a.c_str(), c.vpos[v]).A(b.c_str(), c.vmpos[v]).A(d.c_str(), c.vfwd[v]);
    }
    j.U("timer", c.timer).U("tol", c.tol).U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_idx", c.mutIdx).U("mut_val", c.mutVal).A("mut_pos", c.mutPos);
    return j.End();
}
}
