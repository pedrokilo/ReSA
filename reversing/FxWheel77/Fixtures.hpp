#pragma once
// Fixtures del banco FxWheel77 (lote 77): ocho funciones de Fx_c del EXE, 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 Fx_c::AddWheelDust   0x4A09C0  thiscall, ECX = this (+0x54 = calidad), (CVehicle*, CVector pos por valor, bool giro, float luz), `ret 18h`, void   (hook Fx_c / AddWheelDust, tipo S)
//   1 Fx_c::AddWheelSand   0x4A0610  igual que AddWheelDust
//   2 Fx_c::AddGlass       0x49F970  thiscall, (const CVector& pos, const RwRGBA& color, float escala, int32 cantidad), `ret 10h`, void
//   3 Fx_c::AddDebris      0x49F750  igual que AddGlass
//   4 Fx_c::AddWheelGrass  0x49FF20  thiscall, `this` no se usa, (CVehicle*, CVector pos por valor, bool giro, float luz), `ret 18h`, void
//   5 Fx_c::AddWheelGravel 0x4A0170  igual que AddWheelGrass
//   6 Fx_c::AddWheelMud    0x4A03C0  igual que AddWheelGrass
//   7 Fx_c::TriggerGunshot 0x4A0DE0  thiscall, (CEntity*, const CVector& origen, const CVector& destino, bool fogonazo), `ret 10h`, void
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia).
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

namespace Fw77Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;
using i16 = std::int16_t;

constexpr unsigned kCases = 16384;        // por funcion: 16 familias de 1024
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 8;
enum Fn : u32 { kDust = 0, kSand = 1, kGlass = 2, kDebris = 3, kGrass = 4, kGravel = 5, kMud = 6, kGunshot = 7 };
constexpr bool IsWheelDust(u32 fn) { return fn == kDust || fn == kSand; }
constexpr bool IsWheelDirt(u32 fn) { return fn == kGrass || fn == kGravel || fn == kMud; }
constexpr bool IsWheel(u32 fn) { return IsWheelDust(fn) || IsWheelDirt(fn); }
constexpr bool IsDebris(u32 fn) { return fn == kGlass || fn == kDebris; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 77077u); }
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
inline std::vector<u32> V3(const u32* p) { return std::vector<u32>(p, p + 3); }

// ------------------------------------------------------------------------------------------------ valores notables (NaN incluidos los no canonicos y los senalizantes)
inline constexpr std::array<u32, 28> kSpecial{0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x41C80000u, 0xC1C80000u, 0x40000000u, 0x3E800000u,
    0x7F7FFFFFu, 0xFF7FFFFFu, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F7FFFFFu, 0x3F800001u, 0x34000000u, 0x33800000u, 0x4B800000u, 0x7F000000u,
    0x7E800000u, 0x5F800000u, 0x1F800000u, 0x20000000u, 0x441C4000u, 0x441C4001u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
// Cargas utiles distintas dos a dos (la regla x87 de dos NaN usa la de mayor mantisa; con cargas iguales no se comparan)
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00002u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFEu, 0x7F800001u, 0x7FA00000u, 0xFFA00003u, 0xFF800004u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
inline constexpr std::array<u32, 12> kDenorm{1u, 0x80000001u, 2u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x001FFFFFu, 0x801FFFFFu, 0x00200000u, 0x00200001u, 0x80200001u};
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 PosFloat(u32 c, u32 k, int lo, int hi) { return RandFloat(c, k, lo, hi) & 0x7FFFFFFFu; }   // sin signo
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 Neg(u32 b) { return b ^ 0x80000000u; }
// Una magnitud especial para un float de entrada: NaN, infinito, ceros, desnormales, muy grandes, normales
inline u32 MixComponent(u32 c, u32 k) {
    switch (Rand(c, k) % 8) {
    case 0: return Pick(kNan, c, k + 100);
    case 1: return Pick(kInf, c, k + 100);
    case 2: return Rand(c, k + 100) & 0x80000000u;
    case 3: return Rand(c, k + 100) & 0x807FFFFFu;
    case 4: return RandFloat(c, k + 100, 60, 127);
    case 5: return kSpecial[Rand(c, k + 100) % kSpecial.size()];
    case 6: return RandFloat(c, k + 100, -3, 6);
    default: return RandFloat(c, k + 100, -20, -3);
    }
}
// Mueve un float finito `k` unidades en la ultima posicion, conservando el signo.
inline u32 Ulps(u32 bits, int k) {
    const u32 sign = bits & 0x80000000u;
    i32 mag = i32(bits & 0x7FFFFFFFu) + k;
    if (mag < 0) mag = 0;
    if (mag > 0x7F7FFFFF) mag = 0x7F7FFFFF;
    return sign | u32(mag);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}

// ------------------------------------------------------------------------------------------------ arenas y memoria global
constexpr u32 kGuard = 0x40;
constexpr u32 kVecWindow = 0x10, kVecArena = kGuard + kVecWindow + kGuard;                  // 0x90   un vector (pos)
constexpr u32 kColWindow = 0x10, kColArena = kGuard + kColWindow + kGuard;                  // 0x90   color RwRGBA (4 bytes) con holgura
constexpr u32 kMatWindow = 0x40, kMatArena = kGuard + kMatWindow + kGuard;                  // 0xC0   matriz de la camara (posicion en +0x30)
constexpr u32 kSysWindow = 0x20, kSysArena = kGuard + kSysWindow + kGuard;                  // 0xA0   sistemas de particulas falsos
constexpr u32 kThisWindow = 0x70, kThisArena = kGuard + kThisWindow + kGuard;               // 0xF0   objeto `this` (Fx_c: +0x54 = calidad)
constexpr u32 kVehWindow = 0x5A0, kVehArena = kGuard + kVehWindow + kGuard;                 // 0x620  vehiculo falso (CVehicle)
constexpr u32 kPedWindow = 0x20, kPedArena = kGuard + kPedWindow + kGuard;                  // 0xA0   peds falsos (solo identidad)
constexpr u32 kEntWindow = 0x40, kEntArena = kGuard + kEntWindow + kGuard;                  // 0xC0   entidad falsa de TriggerGunshot (+4 posicion, +0x14 matriz, +0x18 objeto RenderWare)
constexpr u32 kRwWindow = 0x10, kRwArena = kGuard + kRwWindow + kGuard;                     // 0x90   objeto RenderWare (+4 = marco padre)
constexpr u32 kFrameWindow = 0x100, kFrameArena = kGuard + kFrameWindow + kGuard;           // 0x180  marco RenderWare (+0x10 = matriz de modelado)
constexpr u32 kRwMatWindow = 0x40, kRwMatArena = kGuard + kRwMatWindow + kGuard;            // 0xC0   matriz que crea FxRwMatrixCreate
static_assert(kVecArena == 0x90 && kMatArena == 0xC0 && kSysArena == 0xA0 && kThisArena == 0xF0 && kVehArena == 0x620, "arenas");
constexpr u32 kCamPlacement = 0xB6F02C, kCamMatrixPtr = 0xB6F03C, kCamBase = 0xB6F028, kCamBytes = 0x20;   // TheCamera: m_placement (+4) y m_matrix (+0x14)
constexpr u32 kFxBase = 0xA9AE00, kFxBytes = 0x48;                                         // g_fx: punteros a sistemas (+0 .. +0x44)
constexpr u32 kSlotCardebris = 0xA9AE0C, kSlotSand = 0xA9AE18, kSlotWheelDirt = 0xA9AE3C, kSlotGlass = 0xA9AE40;
constexpr u32 kPrimCounter = 0xA9ADE4;                                                     // contador de primitivas de AddDebris (estatico del EXE)
constexpr u32 kFrameCounter = 0xB7CB4C, kTimeStep = 0xB7CB5C;                              // CTimer::m_FrameCounter y CTimer::ms_fTimeStep

constexpr u32 SlotOf(u32 fn) { return fn == kGlass ? kSlotGlass : fn == kDebris ? kSlotCardebris : IsWheelDust(fn) ? kSlotSand : kSlotWheelDirt; }

// ------------------------------------------------------------------------------------------------ el caso
// rand() es el REAL del CRT del EXE (0x821B1E). El estado inicial de cada lado es `randSeed`. randMode: 0 = estado cualquiera; 1 = el sorteo numero `randK` vale un borde de kEdgeOut.
struct C {
    u32 fn{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 thisKind{};    // 0 arena, 1 g_fx real, 2 puntero basura, 3 nulo
    u32 quality{};     // Fx_c::m_FxQuality de la arena `this`
    u32 hasMat{};      // la camara tiene matriz (posicion +0x30) o solo posicion compacta (+4)
    u32 amount{};      // int32 con signo (AddDebris/AddGlass)
    u32 light{};       // lightMult (patron de bits)
    u32 scale{};       // escala de AddDebris/AddGlass
    u32 color{};       // RwRGBA como dword (r en el byte bajo)
    u32 randMode{}, randSeed{}, randK{};
    u32 swapSlot{};    // el falso de AddParticle cambia el sistema entre dos entre llamadas (el EXE lo relee en cada vuelta)
    u32 counter{};     // contador de primitivas de AddDebris (0xA9ADE4)
    u32 cam[3]{}, other[3]{}, pos[3]{};
    // vehiculo
    u32 model{};       // word en +0x22
    u32 vel[3]{};      // +0x44
    u32 gas{};         // +0x49C
    u32 subtype{};     // +0x594
    u32 driverKind{};  // 0 nulo, 1 = ped que devuelve FindPlayerPed(0), 2 = el de FindPlayerPed(1), 3 = otro puntero
    u32 ped0Kind{};    // FindPlayerPed(0): 0 nulo, 1 ped A
    u32 ped1Kind{};    // FindPlayerPed(1): 0 nulo, 1 ped B
    u32 pvKind{};      // FindPlayerVehicle: 0 nulo, 1 el vehiculo falso, 2 puntero basura
    u32 spinning{};    // dword de la pila del argumento bool (byte bajo = valor; el resto basura)
    u32 frame{};       // CTimer::m_FrameCounter
    u32 timestep{};    // CTimer::ms_fTimeStep
    // TriggerGunshot
    u32 entKind{};       // 0 entidad nula, 1 entidad (arena)
    u32 hasEntMatrix{};  // la entidad tiene matriz (+0x14)
    u32 rwKind{};        // el objeto RenderWare de la entidad (+0x18): 0 nulo, 1 presente
    u32 flash{};         // dword de la pila del argumento bool doGunflash (byte bajo = valor; el resto basura)
    u32 fxA{}, fxB{};    // sistema que devuelve el primer / segundo CreateFxSystem: 0 nulo, 1 sistema A, 2 sistema B
    u32 createdKind{};   // FxRwMatrixCreate: 0 nulo, 1 matriz
    u32 epos[3]{}, eother[3]{}, target[3]{};   // posicion usada de la entidad, la otra (ruido) y el destino
};

inline void Split3(u32 c, u32 k, double len, double out[3]) {
    double a = double(Rand(c, k) % 2001) / 1000.0 - 1.0;
    double b = double(Rand(c, k + 1) % 2001) / 1000.0 - 1.0;
    double d = double(Rand(c, k + 2) % 2001) / 1000.0 - 1.0;
    double m = a * a + b * b + d * d;
    if (m < 1e-6) { a = 1.0; b = 0.0; d = 0.0; m = 1.0; }
    const double s = len / std::sqrt(m);
    out[0] = a * s; out[1] = b * s; out[2] = d * s;
}
inline u32 SmallAmount(u32 n, u32 k, u32 hi) { return 1 + Rand(n, k) % hi; }
inline u32 NegAmount(u32 n, u32 k) { static constexpr u32 v[]{0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFEu, 0xFFFFFF9Cu, 0xFFFF0000u, 0xC0000000u, 0u, 0u}; return v[Rand(n, k) % std::size(v)]; }

// Congruencia lineal del CRT del EXE y su inversa (A es impar: inversa modulo 2^32 por Newton)
constexpr u32 kLcgA = 0x343FDu, kLcgC = 0x269EC3u;
constexpr u32 InvOdd(u32 a) { u32 x = a; for (int i = 0; i < 5; ++i) x *= 2u - a * x; return x; }
constexpr u32 kLcgInv = InvOdd(kLcgA);
static_assert(kLcgA * kLcgInv == 1u, "inversa de 0x343FD modulo 2^32");
constexpr u32 LcgNext(u32 s) { return s * kLcgA + kLcgC; }
constexpr u32 LcgBack(u32 s) { return (s - kLcgC) * kLcgInv; }
constexpr u32 LcgOut(u32 s) { return (s >> 16) & 0x7FFFu; }
// Estado inicial tal que el sorteo numero k (0 = el primero) devuelve `out`; low/top16 completan los 17 bits que el sorteo no ve (bits 0..15 y 31)
inline u32 SeedForDraw(u32 k, u32 out, u32 low16, u32 bit31) {
    u32 s = (out << 16) | (low16 & 0xFFFFu) | ((bit31 & 1u) << 31);
    for (u32 i = 0; i <= k; ++i) s = LcgBack(s);
    return s;
}
inline constexpr std::array<u32, 13> kEdgeOut{0u, 1u, 2u, 4999u, 5000u, 9999u, 10000u, 16383u, 16384u, 19999u, 29999u, 32766u, 32767u};
// Numero de sorteos de las primeras vueltas, para colocar el sorteo forzado dentro de lo que se consume
inline u32 DrawBudget(const C& f) {
    if (IsWheelDust(f.fn)) return 3;      // al menos una vuelta de 3 sorteos
    if (IsWheelDirt(f.fn)) return 18;     // tres vueltas de 6
    const i32 amount = static_cast<i32>(f.amount);
    return 1 + 3 * (amount > 0 ? (amount < 3 ? u32(amount) : 3u) : 0u);
}
inline void ApplyRand(C& f, u32 c) {
    if (f.randMode == 1) {
        const u32 budget = DrawBudget(f);
        f.randK = Rand(c, 17) % budget;
        f.randSeed = SeedForDraw(f.randK, kEdgeOut[Rand(c, 21) % kEdgeOut.size()], Rand(c, 22), Rand(c, 23));
    } else {
        f.randMode = 0;
        static constexpr u32 sp[]{0u, 1u, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x269EC3u};
        f.randSeed = Rand(c, 19) % 12 == 0 ? sp[Rand(c, 20) % std::size(sp)] : Rand(c, 24);
    }
}

// Velocidad: modulo y direccion; magnitudes entre 0,01 y ~30 (cuenta de particulas de 1 a ~120)
inline void NormalVelocity(C& f, u32 c, double maxMag) {
    double d[3];
    const double mag = std::pow(10.0, -2.0 + double(Rand(c, 140) % 2001) / 2000.0 * (std::log10(maxMag) + 2.0));
    Split3(c, 141, mag, d);
    for (int i = 0; i < 3; ++i) f.vel[i] = B(float(d[i]));
}
inline u32 StepFloat(u32 c, u32 k) { static constexpr float v[]{1.0f, 0.5f, 2.0f, 0.2f, 3.0f, 1.5f, 0.75f, 0.01f, 0.0f, 4.0f}; return Rand(c, k) % 3 == 0 ? RandFloat(c, k + 1, -4, 2) & 0x7FFFFFFFu : B(v[Rand(c, k + 2) % std::size(v)]); }

inline C MakeC(u32 fn, unsigned n) {
    C f{};
    f.fn = fn;
    f.family = n % kFamilies;
    const u32 c = n * 8u + 0x77003u + fn * 0x1000193u;
    f.seed = Rand(c, 6) | 1u;
    f.eax = SeedReg(c, 7); f.ecx = SeedReg(c, 8); f.edx = SeedReg(c, 9); f.stack = StackSeed(Rand(c, 11) % 12);
    f.thisKind = IsWheelDust(fn) ? Rand(c, 3) % 2 : Rand(c, 3) % 4;
    f.hasMat = Rand(c, 1) % 5 < 3 ? 0u : 1u;
    f.quality = Rand(c, 2) % 8 == 0 ? Pick(kSpecial, c, 4) : Rand(c, 5) % 6;
    if (IsWheelDust(fn)) f.quality = Rand(c, 2) % 5 == 0 ? (u32(i32(Rand(c, 4) % 9) - 3)) : Rand(c, 5) % 4;
    f.amount = SmallAmount(c, 12, 4);
    f.light = Rand(c, 13) % 3 == 0 ? B(1.2f) : Rand(c, 14) % 2 ? RandFloat(c, 15, -4, 4) : B(0.0f);
    f.scale = Rand(c, 25) % 3 == 0 ? B(1.0f) : RandFloat(c, 26, -4, 4);
    f.color = Rand(c, 27);
    f.randMode = Rand(c, 16) % 8 == 0 ? 1u : 0u;
    f.swapSlot = Rand(c, 18) % 3 == 0 ? 1u : 0u;
    f.counter = Rand(c, 28) % 6 == 0 ? Rand(c, 29) : Rand(c, 29) % 4;
    for (int i = 0; i < 3; ++i) f.other[i] = MixComponent(c, 20 + i);
    // vehiculo y servicios
    f.model = Rand(c, 30) & 0xFFFFu;
    f.gas = Rand(c, 31) % 3 == 0 ? B(0.0f) : RandFloat(c, 32, -4, 1);
    f.subtype = Rand(c, 33) % 3 == 0 ? Rand(c, 34) % 14 : (Rand(c, 34) % 2 ? 0u : 9u + Rand(c, 35) % 2);
    f.driverKind = Rand(c, 36) % 4;
    f.ped0Kind = Rand(c, 37) % 4 != 0;
    f.ped1Kind = Rand(c, 38) % 4 != 0;
    f.pvKind = Rand(c, 39) % 3;
    f.spinning = (Rand(c, 40) % 2 ? 0u : 1u) | (Rand(c, 41) % 3 == 0 ? (Rand(c, 42) & 0xFFFFFF00u) : 0u);
    f.frame = Rand(c, 43);
    f.timestep = StepFloat(c, 44);
    NormalVelocity(f, c, 30.0);
    auto camNormal = [&](int hi) { for (int i = 0; i < 3; ++i) f.cam[i] = RandFloat(c, 41 + i, -4, hi); };
    auto fromDelta = [&](const double d[3]) { for (int i = 0; i < 3; ++i) f.pos[i] = B(float(double(F(f.cam[i])) - d[i])); };
    auto inRange = [&](double hi) { double d[3]; camNormal(11); Split3(c, 50, double(Rand(c, 53) % 2000) / 2000.0 * hi, d); fromDelta(d); };
    // Para que las compuertas de cadencia dejen pasar a buena parte de los casos: la suma modelo + contador con los bits bajos a cero
    auto passGate = [&](u32 mask, u32 pct) {
        if (Rand(c, 45) % 100 < pct) {
            const u32 base = IsWheelDust(fn) ? u32(i32(i16(f.model))) : (f.model & 0xFFu);
            f.frame = ((Rand(c, 46) & ~mask) - base) ;
        }
    };
    double d[3];
    switch (f.family) {
    case 0: inRange(24.0); passGate(7u, 60); break;                                                                         // dentro de rango
    case 1: {                                                                                                               // alrededor de 64 (8 de distancia): 63..65
        camNormal(8); Split3(c, 50, 8.0 * (1.0 + (double(Rand(c, 53) % 2001) - 1000.0) * 1e-5), d); fromDelta(d);
        f.pos[Rand(c, 54) % 3] = Ulps(f.pos[Rand(c, 54) % 3], int(Rand(c, 55) % 5) - 2); passGate(7u, 70); break;
    }
    case 2: {                                                                                                               // alrededor de 625 y de 400
        camNormal(8); const double len = Rand(c, 56) % 2 ? 25.0 : 20.0;
        Split3(c, 50, len * (1.0 + (double(Rand(c, 53) % 2001) - 1000.0) * 1e-6), d); fromDelta(d);
        f.pos[Rand(c, 54) % 3] = Ulps(f.pos[Rand(c, 54) % 3], int(Rand(c, 55) % 7) - 3); passGate(7u, 70); break;
    }
    case 3: {                                                                                                               // sumas exactas 625, 400, 64
        static constexpr int quads[][3] = {{25, 0, 0}, {0, 7, 24}, {0, 15, 20}, {12, 15, 16}, {7, 24, 0}, {20, 0, 0}, {12, 16, 0}, {0, 0, 20}, {8, 0, 0}, {0, 8, 0}, {0, 0, 8}, {4, 0, 0}};
        const auto& q = quads[Rand(c, 50) % std::size(quads)];
        const int perm[3][3] = {{0, 1, 2}, {1, 2, 0}, {2, 0, 1}};
        const auto& p = perm[Rand(c, 51) % 3];
        for (int i = 0; i < 3; ++i) {
            const double v = double(q[p[i]]) * ((Rand(c, 52 + i) & 1) ? -1.0 : 1.0);
            f.cam[i] = B(float(double(int(Rand(c, 60 + i) % 2001) - 1000)));
            f.pos[i] = B(float(double(F(f.cam[i])) - v));
        }
        passGate(7u, 70); break;
    }
    case 4: camNormal(11); for (int i = 0; i < 3; ++i) f.pos[i] = RandFloat(c, 70 + i, 10, 40); break;                      // lejos
    case 5: inRange(10.0);                                                                                                  // NaN (cualquier carga)
            for (int j = 0; j < 1 + int(Rand(c, 56) % 2); ++j) { if (Rand(c, 57 + j) & 1) f.cam[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); else f.pos[Rand(c, 60 + j) % 3] = Pick(kNan, c, 62 + j); }
            passGate(7u, 60); break;
    case 6: inRange(10.0);                                                                                                  // infinitos
            for (int j = 0; j < 1 + int(Rand(c, 56) % 3); ++j) {
                const u32 inf = Pick(kInf, c, 62 + j); const unsigned axis = Rand(c, 60 + j) % 3; const u32 mode = Rand(c, 66 + j) % 4;
                if (mode == 0) f.cam[axis] = inf; else if (mode == 1) f.pos[axis] = inf; else if (mode == 2) { f.cam[axis] = inf; f.pos[axis] = inf; } else { f.cam[axis] = inf; f.pos[axis] = Neg(inf); }
            }
            break;
    case 7: for (int i = 0; i < 3; ++i) { f.cam[i] = Pick(kTiny, c, 41 + i); f.pos[i] = Pick(kTiny, c, 70 + i); } passGate(7u, 70); break;   // ceros y desnormales: en rango
    case 8: inRange(24.0); f.quality = Rand(c, 5) % 3 == 0 ? Pick(kSpecial, c, 4) : u32(i32(Rand(c, 4) % 11) - 4);                // calidad: -4..6 y especiales
            passGate(Rand(c, 47) % 2 ? 7u : 3u, 60); break;
    case 9: inRange(24.0); f.frame = Rand(c, 47) % 4 == 0 ? Pick(kSpecial, c, 48) : f.frame;                                  // contador y modelo extremos
            f.model = Pick(kSpecial, c, 49) & 0xFFFFu; if (Rand(c, 56) % 2) f.model |= 0x8000u; passGate(3u, 50); break;
    case 10: inRange(24.0); NormalVelocity(f, c, 300.0); passGate(7u, 70);                                                   // velocidades grandes (hasta ~600 particulas) y giro
             if (Rand(c, 57) % 6 == 0) NormalVelocity(f, c * 7u + 1u, 3000.0); break;
    case 11: inRange(24.0); f.gas = Rand(c, 57) % 4 == 0 ? Pick(kNan, c, 58) : Rand(c, 57) % 4 == 1 ? Pick(kInf, c, 59) : Rand(c, 57) % 4 == 2 ? Pick(kTiny, c, 60) : MixComponent(c, 61);   // pedal especial
             passGate(7u, 70); break;
    case 12: inRange(24.0); for (int i = 0; i < 3; ++i) f.vel[i] = Rand(c, 62 + i) % 3 == 0 ? f.vel[i] : MixComponent(c, 65 + i);  // velocidad especial (NaN, inf, desnormales, enormes)
             if (Rand(c, 70) % 3 == 0) for (int i = 0; i < 3; ++i) f.vel[i] = Pick(kDenorm, c, 71 + i);
             passGate(7u, 70); break;
    case 13: inRange(24.0); f.timestep = Rand(c, 57) % 4 == 0 ? Pick(kNan, c, 58) : Rand(c, 57) % 4 == 1 ? Pick(kInf, c, 59) : Rand(c, 57) % 4 == 2 ? Pick(kDenorm, c, 60) : MixComponent(c, 61);   // paso de tiempo especial
             passGate(7u, 70); break;
    case 14: inRange(24.0); f.light = Rand(c, 99) % 4 == 0 ? Pick(kNan, c, 100) : Rand(c, 99) % 4 == 1 ? Pick(kInf, c, 101) : Rand(c, 99) % 4 == 2 ? Pick(kTiny, c, 102) : MixComponent(c, 103);   // luz especial
             f.randMode = Rand(c, 16) % 2; f.scale = MixComponent(c, 104); f.color = Rand(c, 105) % 2 ? 0xFFFFFFFFu : Rand(c, 106) % 2 ? 0u : f.color; passGate(7u, 70); break;
    default:                                                                                                                // 15: todo mezclado
        camNormal(7);
        for (int i = 0; i < 3; ++i) {
            f.pos[i] = Rand(c, 70 + i) % 4 == 0 ? MixComponent(c, 75 + i) : B(float(double(F(f.cam[i])) + (double(Rand(c, 80 + i) % 6001) - 3000.0) / 100.0));
            if (Rand(c, 85 + i) % 7 == 0) f.cam[i] = MixComponent(c, 88 + i);
            if (Rand(c, 91 + i) % 6 == 0) f.vel[i] = MixComponent(c, 94 + i);
        }
        if (Rand(c, 19) % 4 == 0) f.light = MixComponent(c, 104);
        if (Rand(c, 20) % 4 == 0) f.gas = MixComponent(c, 107);
        if (Rand(c, 21) % 4 == 0) f.timestep = MixComponent(c, 108);
        f.randMode = Rand(c, 16) % 4 == 0 ? 1u : 0u; passGate(7u, 50);
        break;
    }
    // Reparto del resto de campos de cada funcion
    if (IsDebris(fn)) {
        // pos es un vector en una arena; el color es un RwRGBA; la cantidad depende de la familia
        if (f.family == 8) f.amount = NegAmount(c, 12);
        else if (f.family == 13) { static constexpr u32 big[]{13u, 31u, 64u, 100u, 255u, 256u, 400u}; f.amount = Rand(c, 12) % 3 ? 5u + Rand(c, 13) % 20u : big[Rand(c, 13) % std::size(big)]; }
        else if (f.family == 1 || f.family == 10) f.amount = 5 + Rand(c, 12) % 12;
        else if (f.family == 15) f.amount = Rand(c, 12) % 8 == 0 ? NegAmount(c, 14) : SmallAmount(c, 12, 6);
        if (f.family == 0 || f.family == 9) f.color = Rand(c, 12) % 4 == 0 ? Pick(std::array<u32, 6>{0u, 0xFFFFFFFFu, 0xFF000000u, 0x000000FFu, 0x80808080u, 0x01020304u}, c, 13) : f.color;
        if (f.family == 12) f.scale = MixComponent(c, 110);
    }
    if (IsWheelDust(fn)) {
        // El numero de particulas es el EAX de _ftol2(|v * paso| * q): hay que acotarlo (un valor entre 2^31 y 2^55 da un entero de 32 bits cualquiera y hasta 2 mil millones de vueltas).
        // Se estima en double; si cae en [20000, 2^55) se reduce la velocidad (para que salga entre 1 y ~3000 vueltas); por encima de 2^55 la x87 da multiplos de 2^32 (EAX = 0, una vuelta) y se deja.
        const double ts = double(F(f.timestep)), vx = double(F(f.vel[0])) * ts, vy = double(F(f.vel[1])) * ts, vz = double(F(f.vel[2])) * ts;
        const double q = (f.subtype == 10 || f.subtype == 9 || f.subtype == 2) ? 2.0 : 1.5;
        const double est = std::sqrt(vz * vz + vy * vy + vx * vx) * q;
        if (std::isfinite(est) && est >= 20000.0 && est < 3.6e16) {
            double d[3];
            Split3(c, 200, double(Rand(c, 203) % 3000) / (2.0 * ts == 0.0 ? 1.0 : 2.0 * std::fabs(ts)) , d);
            for (int i = 0; i < 3; ++i) f.vel[i] = B(float(d[i]));
        }
    }
    if (fn == kGunshot) {
        f.entKind = Rand(c, 150) % 3 == 0 ? 0u : 1u;
        f.hasEntMatrix = Rand(c, 151) % 2;
        f.rwKind = Rand(c, 152) % 7 != 0;
        f.flash = (Rand(c, 153) % 4 == 0 ? 0u : (Rand(c, 154) % 2 ? 1u : ((Rand(c, 155) & 0xFFu) | 1u))) | (Rand(c, 156) % 3 == 0 ? (Rand(c, 157) & 0xFFFFFF00u) : 0u);
        f.fxA = Rand(c, 158) % 5 == 0 ? 0u : 1u + Rand(c, 159) % 2;
        f.fxB = Rand(c, 160) % 5 == 0 ? 0u : 1u + Rand(c, 161) % 2;
        f.createdKind = Rand(c, 162) % 10 != 0;
        for (int i = 0; i < 3; ++i) {
            f.epos[i] = Rand(c, 163 + i) % 3 == 0 ? RandFloat(c, 166 + i, -4, 11) : B(float(double(F(f.pos[i])) + (double(Rand(c, 170 + i) % 4001) - 2000.0) / 100.0));
            f.eother[i] = MixComponent(c, 173 + i);
            f.target[i] = RandFloat(c, 176 + i, -6, 4);
        }
        switch (f.family) {
        case 8: f.entKind = 0; break;                                                                            // sin entidad
        case 9: f.entKind = 1; f.hasEntMatrix = 0; break;                                                        // entidad sin matriz (AllocateMatrix + UpdateMatrix)
        case 10: f.entKind = 1; f.hasEntMatrix = 1; f.rwKind = 1; break;                                         // entidad con matriz y objeto RenderWare
        case 11: for (int i = 0; i < 3; ++i) f.epos[i] = Rand(c, 180 + i) % 4 == 0 ? RandFloat(c, 183 + i, -4, 11) : MixComponent(c, 186 + i);   // posicion de la entidad especial
                 f.entKind = 1; break;
        case 12: for (int i = 0; i < 3; ++i) f.target[i] = MixComponent(c, 189 + i); f.entKind = Rand(c, 192) % 2 ? 0u : 1u; break;    // destino especial (solo se pasa a CreateMatFromVec)
        case 13: f.fxA = Rand(c, 193) % 2 ? 0u : f.fxA; f.fxB = Rand(c, 194) % 2 ? 0u : f.fxB; f.flash = Rand(c, 195) % 2 ? 1u : 0u; f.createdKind = Rand(c, 196) % 3 != 0; break;   // resultados nulos
        case 14: f.flash |= 1u; f.fxA = 1; f.fxB = 2; break;
        default: break;
        }
    }
    ApplyRand(f, c);
    return f;
}
inline std::string JsonC(const C& c) {
    Js j; j.U("fn", c.fn).U("family", c.family).U("seed", c.seed).U("this_kind", c.thisKind).U("quality", c.quality).U("has_mat", c.hasMat).U("amount", c.amount).U("light", c.light).U("scale", c.scale).U("color", c.color)
     .U("rand_mode", c.randMode).U("rand_seed", c.randSeed).U("rand_k", c.randK).U("swap_slot", c.swapSlot).U("counter", c.counter)
     .A("cam", V3(c.cam)).A("other", V3(c.other)).A("pos", V3(c.pos)).U("model", c.model).A("vel", V3(c.vel)).U("gas", c.gas).U("subtype", c.subtype)
     .U("driver_kind", c.driverKind).U("ped0_kind", c.ped0Kind).U("ped1_kind", c.ped1Kind).U("pv_kind", c.pvKind).U("spinning", c.spinning).U("frame", c.frame).U("timestep", c.timestep)
     .U("ent_kind", c.entKind).U("has_ent_matrix", c.hasEntMatrix).U("rw_kind", c.rwKind).U("flash", c.flash).U("fx_a", c.fxA).U("fx_b", c.fxB).U("created_kind", c.createdKind)
     .A("epos", V3(c.epos)).A("eother", V3(c.eother)).A("target", V3(c.target))
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}
