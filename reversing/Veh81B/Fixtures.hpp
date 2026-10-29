#pragma once
// Fixtures del banco Veh81B (lote 81B): CVehicle::UpdateTractorLink (EXE 0x6E0050), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CVehicle::UpdateTractorLink  0x6E0050  thiscall, ECX = this (tractor), dos bool (dwords en la pila), `ret 8`, void   (hook CVehicle / UpdateTractorLink, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) y las familias 16 y 17 (1024 casos cada una).
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

namespace V81Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 2;    // familias 16..17, 1024 casos cada una
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). La del vehiculo mide sizeof(CVehicle) = 0x5A0 (LiveComparison.hpp lo comprueba con static_assert).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x9F8, kMatWin = 0x48;   // el tractor y el remolque se leen hasta +0x9EC: la ventana cubre un CTrailer (0x9F4)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin);

constexpr u32 kTimeAddr = 0xB7CB5C;   // CTimer::ms_fTimeStep

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 81081u); }
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

// ------------------------------------------------------------------------------------------------ valores notables
// NaN: canonicos (quiet con signo +/-), NO canonicos (carga distinta, sNaN con y sin signo, carga maxima, carga minima) e infinitos
// Un unico patron de NaN (el indefinido de la x87, que producen inf-inf y 0*inf): con dos NaN distintos en una operacion la carga que sale depende del orden de operandos que elija el compilador
inline constexpr std::array<u32, 12> kNan{0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u, 0xFFC00000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};   // +-0, desnormales y el menor normal
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 14> kUnit{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x3DCCCCCDu, 0x3F99999Au, 0x3FA66666u, 0x41200000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
// Float con exponente insesgado e (-126..127) y mantisa/signo aleatorios; Denorm: desnormal con signo y mantisa aleatorios (nunca cero)
inline u32 ExpFloat(u32 c, u32 k, int e) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | (u32(e + 127) << 23) | (Rand(c, k + 500) & 0x7FFFFFu); }
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
inline u32 Sgn(u32 bits, u32 neg) { return (bits & 0x7FFFFFFFu) | (neg << 31); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float: modo 0 moderado, 1 pequeno (productos que caen en el rango desnormal), 2 grande (desbordamiento), 3 especial (cero, desnormal, inf, NaN), 4 unitario, 5 mezcla de todo
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 11);
    case 1: return r < 3 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2: return r < 3 ? Pick(kBig, c, k + 1) : RandFloat(c, k + 2, 20, 120);
    case 3:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 100);
    case 4: return r < 4 ? RandFloat(c, k + 2, -2, 2) : Pick(kUnit, c, k + 1);
    default: {
        const u32 m = Rand(c, k + 3) % 5;
        return FloatOf(c, k + 5, m == 4 ? 3u : m);
    }
    }
}



// ------------------------------------------------------------------------------------------------ el caso
// Arenas (indices de etiqueta): 0 tractor (this), 1 remolque T, 2 remolque alternativo T2, 3 matriz del tractor A, 4 matriz del tractor B, 5 matriz del remolque A, 6 matriz del remolque B.
// Orden de las lecturas del EXE (eventos = llamadas a colaboradores; un falso puede mutar el estado tras cada una):
//   0 enganche del remolque (virtual +0xEC), 1 barra del tractor (virtual +0xF0), 2 GetSpeed del remolque, 3 GetSpeed del tractor, 4 Normalise, 5 CrossProduct, 6 ApplyForce.
constexpr unsigned kEvents = 7;
constexpr u32 kNoMutation = 99;        // mutAt fuera de rango = sin mutacion
// Desplazamientos (bytes) de los 12 floats que el banco controla de cada matriz: 3x3 (0x00 0x04 0x08 / 0x10 0x14 0x18 / 0x20 0x24 0x28) y posicion (0x30 0x34 0x38)
constexpr std::array<u32, 12> kMatOff{0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38};
constexpr u32 kLuggageSub = 11u, kLuggageSentinel = 0xC47A0000u;
constexpr u32 kMutKinds = 14;
// Mutaciones (un falso las aplica tras el evento mutAt):
//  1 remolque del tractor (+0x4C8) := T2           2 matriz del tractor (+0x14) := A (mutVal par) o B           3 matriz del remolque T (+0x14) := A (par) o B
//  4 masa del tractor := mutVal2                    5 masa de giro del tractor := mutVal2                         6 masa de T := mutVal2
//  7 paso de tiempo := mutVal2                      8 modelo del tractor (word +0x22) := mutVal                    9 angulo del gancho (word +0x86C) := mutVal
// 10 T pasa a ser de equipaje (+0x594, +0x9EC)     11 T deja de ser de equipaje (+0x594 := mutVal)                12 float (indice mutVal % 12) de la matriz A del tractor := mutVal2
// 13 masa de T2 := mutVal2                          14 float (indice mutVal % 12) de la matriz A del remolque := mutVal2
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 arg0{}, arg1{};                 // dwords que empuja quien llama: solo el byte bajo es el bool
    u32 hasTrailer{};                   // +0x4C8 no nulo
    u32 hitch[3]{}, bar[3]{};           // vectores que escriben los virtuales
    u32 retHitch{}, retBar{};           // EAX que devuelven (solo AL decide)
    u32 model{}, hoist{};               // words +0x22 y +0x86C del tractor
    u32 vmass{}, vturn{}, tmass[2]{};   // masas
    u32 tsub[2]{}, tsent[2]{};          // +0x594 y +0x9EC de T y T2
    u32 vm[12]{}, vm2[12]{}, tm[12]{}, tm2[12]{};   // matrices (ver kMatOff)
    u32 vfb[3]{}, tfb[2][3]{};          // posiciones de reserva (+0x04) del tractor, T y T2
    u32 com[3]{};                       // centro de masas del tractor (+0xA4)
    u32 hasVMat{}, hasTMat{};           // matriz del tractor / de T no nula (T2 siempre tiene)
    u32 spd[2][3]{};                    // resultados de GetSpeed del remolque y del tractor
    u32 norm[3]{};                      // lo que deja Normalise
    u32 cross[3]{};                     // resultado de CrossProduct
    u32 crossMode{};                    // 0 el falso escribe en el puntero de salida y lo devuelve; 1 escribe basura ahi y devuelve otro buffer con el resultado
    u32 decoy[3]{};                     // basura del modo 1
    u32 ts{};                           // CTimer::ms_fTimeStep
    u32 mutAt{}, mutKind{}, mutVal{}, mutVal2{};
    u32 gate{};                         // informativo: 0 sin remolque, 1 sale por el enganche, 2 por la barra, 3 por el gancho, 4 completa
};

inline bool IsTowModel(u32 m) { return (m & 0xFFFFu) == 0x20Du || (m & 0xFFFFu) == 0x213u; }
// Flujo de control (sin aritmetica): hasta donde llega la funcion segun la entrada. Valores de `gate`.
inline unsigned Reach(const C& c) {
    if (!c.hasTrailer) return 0;
    if ((c.retHitch & 0xFFu) == 0) return 1;
    if ((c.retBar & 0xFFu) == 0) return 2;
    u32 model = c.model & 0xFFFFu, hoist = c.hoist & 0xFFFFu;
    if (c.mutAt <= 1) {
        if (c.mutKind == 8) model = c.mutVal & 0xFFFFu;
        if (c.mutKind == 9) hoist = c.mutVal & 0xFFFFu;
    }
    if (IsTowModel(model) && i32(hoist) > 20000 - 100) return 3;
    return 4;
}

inline u32 RetOk(u32 n, u32 k) { const u32 lo = 1u + Rand(n, k) % 255u; return (Rand(n, k + 1) % 3 == 0 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u)) | lo; }
inline u32 RetFail(u32 n, u32 k) { return Rand(n, k) % 3 == 0 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u); }
inline u32 BoolDword(u32 n, u32 k, bool v) { return (Rand(n, k) % 3 == 0 ? 0u : (Rand(n, k + 1) & 0xFFFFFF00u)) | (v ? 1u : 0u); }

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    const u32 f = c.family;
    auto fl = [&](u32 k, u32 mode = 0u) { return FloatOf(n, k, mode); };
    c.hasTrailer = 1;
    c.arg0 = BoolDword(n, 10, Rand(n, 11) % 3 == 0);
    c.arg1 = BoolDword(n, 12, Rand(n, 13) % 2 == 0);
    c.retHitch = RetOk(n, 20); c.retBar = RetOk(n, 22);
    for (int i = 0; i < 3; ++i) { c.hitch[i] = fl(30 + i); c.bar[i] = fl(33 + i); }
    // modelo: la mayoria no es grua ni tractor; el angulo del gancho alrededor de 20000 - 100
    {
        static constexpr std::array<u32, 8> models{0x20Du, 0x213u, 0x20Cu, 0x214u, 0x20Du, 0x213u, 0xFFFFu, 0u};
        c.model = Rand(n, 40) % 4 == 0 ? Pick(models, n, 41) : (Rand(n, 42) % 3 == 0 ? Rand(n, 43) & 0xFFFFu : 400u + Rand(n, 44) % 300u);
        if (Rand(n, 45) % 4 == 0) c.model |= 0x10000u * (Rand(n, 46) & 0xFFFFu);   // el word alto del dword no se lee (cmp ax)
        static constexpr std::array<u32, 12> hoists{19899u, 19900u, 19901u, 0u, 1u, 20000u, 0xFFFFu, 0x8000u, 0x7FFFu, 10000u, 19800u, 20100u};
        c.hoist = Rand(n, 47) % 2 ? Pick(hoists, n, 48) : (Rand(n, 49) & 0xFFFFu);
        if (Rand(n, 50) % 4 == 0) c.hoist |= 0x10000u * (Rand(n, 51) & 0xFFFFu);
    }
    c.vmass = fl(60); c.vturn = fl(61); c.tmass[0] = fl(62); c.tmass[1] = fl(63);
    for (int i = 0; i < 2; ++i) {
        const bool lug = Rand(n, 64 + i) % 3 == 0;
        c.tsub[i] = lug ? kLuggageSub : (Rand(n, 66 + i) % 2 ? Rand(n, 68 + i) % 14u : Rand(n, 70 + i));
        c.tsent[i] = lug && Rand(n, 72 + i) % 8 != 0 ? kLuggageSentinel : (Rand(n, 74 + i) % 2 ? (B(-1000.0f) ^ (Rand(n, 76 + i) % 4 == 0 ? 1u << (Rand(n, 78 + i) % 32) : 0u)) : Rand(n, 80 + i));
    }
    for (int i = 0; i < 12; ++i) { c.vm[i] = fl(100 + i); c.vm2[i] = fl(112 + i); c.tm[i] = fl(124 + i); c.tm2[i] = fl(136 + i); }
    for (int i = 0; i < 3; ++i) { c.vfb[i] = fl(150 + i); c.tfb[0][i] = fl(153 + i); c.tfb[1][i] = fl(156 + i); c.com[i] = fl(190 + i); }
    c.hasVMat = 1; c.hasTMat = 1;
    for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) c.spd[g][i] = fl(160 + 3 * g + i);
    for (int i = 0; i < 3; ++i) { c.norm[i] = fl(170 + i, Rand(n, 173 + i) % 3 == 0 ? 4u : 0u); c.cross[i] = fl(176 + i); c.decoy[i] = Rand(n, 180 + i); }
    c.crossMode = Rand(n, 183) % 6 == 0;
    c.ts = Rand(n, 184) % 2 ? B(1.0f / 60.0f * float(1 + Rand(n, 185) % 90)) : fl(186, 4u);
    c.mutAt = Rand(n, 220) % 3 == 0 ? Rand(n, 221) % kEvents : kNoMutation;
    c.mutKind = c.mutAt == kNoMutation ? 0u : 1u + Rand(n, 222) % kMutKinds;
    c.mutVal = Rand(n, 223) % 4 == 0 ? Pick(kStackSeeds, n, 224) : Rand(n, 225);
    c.mutVal2 = fl(226, Rand(n, 227) % 3 == 0 ? 3u : 0u);
    // proceed: la funcion llega al final (los dos virtuales devuelven AL != 0 y el gancho no corta)
    auto proceed = [&] {
        c.hasTrailer = 1; c.retHitch = RetOk(n, 230); c.retBar = RetOk(n, 232);
        if (Reach(c) < 4) { c.model = 400; c.hoist = 0; }
    };
    auto allFloats = [&](u32 mode, u32 salt) {
        for (int i = 0; i < 3; ++i) { c.hitch[i] = fl(salt + i, mode); c.bar[i] = fl(salt + 3 + i, mode); }
        for (int g = 0; g < 2; ++g) for (int i = 0; i < 3; ++i) c.spd[g][i] = fl(salt + 6 + 3 * g + i, mode);
        for (int i = 0; i < 12; ++i) { c.vm[i] = fl(salt + 20 + i, mode); c.tm[i] = fl(salt + 40 + i, mode); }
        for (int i = 0; i < 3; ++i) { c.norm[i] = fl(salt + 60 + i, mode); c.cross[i] = fl(salt + 64 + i, mode); c.vfb[i] = fl(salt + 68 + i, mode); c.tfb[0][i] = fl(salt + 72 + i, mode); c.com[i] = fl(salt + 76 + i, mode); }
    };
    switch (f) {
    case 0:                                                      // sin banderas: velocidad relativa por el factor de masas
        c.arg0 = BoolDword(n, 240, false); c.arg1 = BoolDword(n, 241, false); proceed(); break;
    case 1:                                                      // arg0: velocidad relativa sin tocar (el segundo bool no importa)
        c.arg0 = BoolDword(n, 240, true); proceed(); break;
    case 2:                                                      // arg1: (0.1 * c) / max(1.0, paso de tiempo)
        c.arg0 = BoolDword(n, 240, false); c.arg1 = BoolDword(n, 241, true); proceed();
        if (Rand(n, 242) % 2) c.ts = fl(243, Rand(n, 244) % 2 ? 4u : 5u);
        break;
    case 3:                                                      // remolque de equipaje: pierde la componente a lo largo de su eje
        c.tsub[0] = kLuggageSub; c.tsent[0] = kLuggageSentinel; proceed();
        if (Rand(n, 245) % 3 == 0) c.tsub[0] = Rand(n, 246) % 2 ? 10u : 12u;
        if (Rand(n, 247) % 5 == 0) c.tsent[0] ^= 1u << (Rand(n, 248) % 32);
        if (Rand(n, 249) % 5 == 0) { c.tsub[0] = kLuggageSub; c.tsent[0] = kLuggageSentinel; c.mutAt = Rand(n, 250) % 4; c.mutKind = Rand(n, 251) % 2 ? 10u : 11u; }
        break;
    case 4:                                                      // sin remolque: sale sin llamar a nadie
        c.hasTrailer = 0; break;
    case 5:                                                      // el enganche del remolque devuelve AL = 0 (basura en los bits altos)
        c.retHitch = RetFail(n, 252); break;
    case 6:                                                      // la barra del tractor devuelve AL = 0
        c.retBar = RetFail(n, 253);
        if (Rand(n, 254) % 4 == 0) { c.mutAt = 0; c.mutKind = 1; }   // el remolque cambia tras el primer evento: la barra recibe el nuevo
        break;
    case 7: {                                                    // grua y tractor: el angulo del gancho frente a 20000 - 100
        static constexpr std::array<u32, 6> tow{0x20Du, 0x213u, 0x20Du, 0x213u, 0x20Cu, 0x214u};
        proceed(); c.model = Pick(tow, n, 255);
        static constexpr std::array<u32, 10> h{19899u, 19900u, 19901u, 19898u, 0u, 20000u, 0xFFFFu, 0x7FFFu, 0x8000u, 19950u};
        c.hoist = Rand(n, 256) % 4 == 0 ? Rand(n, 257) & 0xFFFFu : Pick(h, n, 258);
        if (Rand(n, 259) % 3 == 0) { c.mutAt = Rand(n, 260) % 3; c.mutKind = Rand(n, 261) % 2 ? 8u : 9u; if (c.mutKind == 8) c.mutVal = Pick(tow, n, 262); else c.mutVal = Rand(n, 263) % 2 ? Pick(h, n, 264) : 100u; }
        break;
    }
    case 8:                                                      // floats especiales en posiciones, velocidades y matrices (NaN, inf, ceros, desnormales)
        proceed(); allFloats(Rand(n, 265) % 3 == 0 ? 5u : 3u, 400); c.hasVMat = Rand(n, 266) % 6 != 0; c.hasTMat = Rand(n, 267) % 6 != 0; break;
    case 9:                                                      // masas especiales (cero, infinito, NaN, negativas, enormes)
        proceed(); c.vmass = fl(268, 3u); c.vturn = fl(269, 3u); c.tmass[0] = fl(270, 3u); c.tmass[1] = fl(271, 3u);
        c.arg0 = BoolDword(n, 272, false); c.arg1 = BoolDword(n, 273, Rand(n, 274) % 3 == 0);
        if (Rand(n, 275) % 3 == 0) { c.tsub[0] = kLuggageSub; c.tsent[0] = kLuggageSentinel; }
        break;
    case 10: {                                                   // paso de tiempo: 1.0 exacto, alrededor, cero, negativo, NaN, infinito, diminuto, enorme
        static constexpr std::array<u32, 18> tsv{0x3F800000u, 0x3F7FFFFFu, 0x3F800001u, 0u, 0x80000000u, 0xBF800000u, 0xFFC00000u, 0x7F800000u, 0xFF800000u, 1u, 0x007FFFFFu, 0x7F7FFFFFu, 0x7E800000u, 0x7F000000u, 0x3F000000u, 0x40000000u, 0x3C23D70Au, 0x42C80000u};
        proceed(); c.arg0 = BoolDword(n, 276, false); c.arg1 = BoolDword(n, 277, true); c.ts = Rand(n, 278) % 5 == 0 ? fl(279, 3u) : Pick(tsv, n, 280);
        for (int i = 0; i < 3; ++i) if (Rand(n, 281 + i) % 3 == 0) { c.hitch[i] = fl(284 + i, 2u); c.bar[i] = fl(287 + i, 1u); }
        if (Rand(n, 290) % 6 == 0) { c.mutAt = Rand(n, 291) % 4; c.mutKind = 7; }
        break;
    }
    case 11:                                                     // barrido de mutaciones: cada evento x cada tipo
        proceed(); c.mutAt = (n / 16) % kEvents; c.mutKind = 1u + (n / 16 / kEvents) % kMutKinds;
        c.arg0 = BoolDword(n, 292, false); c.arg1 = BoolDword(n, 293, Rand(n, 294) % 3 == 0);
        if (Rand(n, 295) % 2) { c.tsub[0] = kLuggageSub; c.tsent[0] = kLuggageSentinel; c.tsub[1] = kLuggageSub; c.tsent[1] = kLuggageSentinel; }
        if (c.mutKind == 8 || c.mutKind == 9) {
            c.model = c.mutKind == 8 ? 400u : (Rand(n, 296) % 2 ? 0x20Du : 0x213u);
            c.hoist = Rand(n, 297) % 2 ? 19900u : 19901u;
            c.mutVal = c.mutKind == 8 ? (Rand(n, 298) % 2 ? 0x20Du : 0x213u) : (Rand(n, 299) % 2 ? 19901u : 100u);
        }
        break;
    case 12:                                                     // matrices nulas (tractor y/o remolque): posicion de reserva; la matriz aparece mas tarde
        proceed(); c.hasVMat = Rand(n, 300) % 3 != 0; c.hasTMat = Rand(n, 301) % 3 != 0;
        if (Rand(n, 302) % 2) { c.tsub[0] = kLuggageSub; c.tsent[0] = kLuggageSentinel; }
        c.arg0 = BoolDword(n, 303, Rand(n, 304) % 4 == 0); c.arg1 = BoolDword(n, 305, Rand(n, 306) % 2 == 0);
        break;
    case 13:                                                     // CrossProduct devuelve otro buffer; resultados y direccion especiales
        proceed(); c.crossMode = 1;
        for (int i = 0; i < 3; ++i) { c.cross[i] = fl(307 + i, Rand(n, 310 + i) % 3 == 0 ? 3u : 0u); c.norm[i] = fl(313 + i, Rand(n, 316 + i) % 3 == 0 ? 3u : 0u); }
        c.vmass = fl(319, Rand(n, 320) % 4 == 0 ? 3u : 0u); c.vturn = fl(321, Rand(n, 322) % 4 == 0 ? 3u : 0u);
        break;
    case 14: case 15:                                            // todo aleatorio
        if (Rand(n, 323) % 4 == 0) { proceed(); allFloats(5u, 500); }
        if (Rand(n, 324) % 3 == 0) c.hasVMat = 0;
        if (Rand(n, 325) % 3 == 0) c.hasTMat = 0;
        if (Rand(n, 326) % 8 == 0) c.hasTrailer = 0;
        break;
    case 16:                                                     // NaN e infinitos en varios sitios a la vez
        proceed(); allFloats(0u, 600);
        {
            u32* t[] = {c.hitch, c.bar, c.spd[0], c.spd[1], c.norm, c.cross, c.vm, c.vm + 3, c.vm + 6, c.vm + 9, c.tm + 6, c.tm + 9, &c.vmass, &c.tmass[0]};
            for (int k = 0; k < 14; ++k)
                if (Rand(n, 700 + k) % 3 == 0) t[k][Rand(n, 720 + k) % (k < 12 ? 3 : 1)] = Rand(n, 730 + k) % 2 ? Pick(kNan, n, 740 + k) : Pick(kInf, n, 750 + k);
        }
        if (Rand(n, 760) % 3 == 0) c.ts = Rand(n, 761) % 2 ? Pick(kNan, n, 762) : Pick(kInf, n, 762);
        c.arg0 = BoolDword(n, 763, false); c.arg1 = BoolDword(n, 764, Rand(n, 765) % 2 == 0);
        if (Rand(n, 766) % 2) { c.tsub[0] = kLuggageSub; c.tsent[0] = kLuggageSentinel; }
        break;
    default:                                                     // 17: ceros con signo, desnormales y sumas en el limite de la cancelacion
        proceed();
        {
            u32* t[] = {c.hitch, c.bar, c.spd[0], c.spd[1], c.vm, c.vm + 3, c.vm + 6, c.vm + 9, c.tm + 6, c.tm + 9, c.norm, c.cross};
            for (int k = 0; k < 12; ++k) for (int i = 0; i < 3; ++i) {
                const u32 w = Rand(n, 770 + 3 * k + i) % 4;
                t[k][i] = w == 0 ? Pick(kTiny, n, 810 + 3 * k + i) : w == 1 ? Denorm(n, 850 + 3 * k + i) : w == 2 ? Sgn(ExpFloat(n, 890 + 3 * k + i, -126 + int(Rand(n, 930 + 3 * k + i) % 12)), Rand(n, 970 + 3 * k + i) % 2) : fl(1010 + 3 * k + i);
            }
            c.vmass = Rand(n, 1100) % 3 == 0 ? Pick(kTiny, n, 1101) : fl(1102); c.vturn = Rand(n, 1103) % 3 == 0 ? Pick(kTiny, n, 1104) : fl(1105);
            c.ts = Rand(n, 1106) % 3 == 0 ? Denorm(n, 1107) : c.ts;
        }
        c.arg1 = BoolDword(n, 1108, Rand(n, 1109) % 2 == 0);
        if (Rand(n, 1110) % 2) { c.tsub[0] = kLuggageSub; c.tsent[0] = kLuggageSentinel; }
        break;
    }
    // Validez: el EXE desreferencia sin comprobar nulo la matriz del tractor (0x6E02E8) y, si T es de equipaje, la de T (0x6E02AE). Una matriz nula solo vale si un falso la rellena a tiempo
    // (tras los eventos 2 o 3: GetSpeed) o si no se llega a desreferenciar.
    if (Reach(c) == 4 && !c.hasVMat) {
        c.mutAt = 2 + Rand(n, 1200) % 2; c.mutKind = 2; c.mutVal = Rand(n, 1201);
    }
    {
        const bool reaches = Reach(c) == 4;
        const bool trailerIsT = !(c.mutKind == 1 && c.mutAt <= 3);   // el remolque que se relee en 0x6E0293
        u32 sub = c.tsub[0], sent = c.tsent[0];
        if (c.mutAt <= 3 && c.mutKind == 10) { sub = kLuggageSub; sent = kLuggageSentinel; }
        if (c.mutAt <= 3 && c.mutKind == 11) sub = c.mutVal;
        const bool luggage = sub == kLuggageSub && sent == kLuggageSentinel;
        const bool tMatNull = !c.hasTMat && !(c.mutAt <= 3 && c.mutKind == 3);
        if (reaches && trailerIsT && luggage && tMatNull) {
            if (c.mutAt == kNoMutation) { c.mutAt = 2 + Rand(n, 1202) % 2; c.mutKind = 3; c.mutVal = Rand(n, 1203); }
            else c.hasTMat = 1;
        }
    }
    c.gate = Reach(c);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.U("arg0", c.arg0).U("arg1", c.arg1).U("has_trailer", c.hasTrailer).U("ret_hitch", c.retHitch).U("ret_bar", c.retBar).U("model", c.model).U("hoist", c.hoist);
    j.A("hitch", c.hitch, 3).A("bar", c.bar, 3).U("vmass", c.vmass).U("vturn", c.vturn).A("tmass", c.tmass, 2).A("tsub", c.tsub, 2).A("tsent", c.tsent, 2);
    j.A("vm", c.vm, 12).A("vm2", c.vm2, 12).A("tm", c.tm, 12).A("tm2", c.tm2, 12).A("vfb", c.vfb, 3).A("tfb0", c.tfb[0], 3).A("tfb1", c.tfb[1], 3).A("com", c.com, 3);
    j.U("has_vmat", c.hasVMat).U("has_tmat", c.hasTMat).A("spd0", c.spd[0], 3).A("spd1", c.spd[1], 3).A("norm", c.norm, 3).A("cross", c.cross, 3);
    j.U("cross_mode", c.crossMode).A("decoy", c.decoy, 3).U("ts", c.ts).U("mut_at", c.mutAt).U("mut_kind", c.mutKind).U("mut_val", c.mutVal).U("mut_val2", c.mutVal2).U("gate", c.gate);
    return j.End();
}
}
