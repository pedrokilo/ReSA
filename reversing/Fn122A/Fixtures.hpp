#pragma once
// Fixtures del banco Fn122A (lote 122A): 16384 casos por funcion (16384 pares por funcion). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
// Diez funciones diminutas del EXE (thiscall, ECX = this, salvo la primera; sin argumentos de pila). Las clases del listado se atribuian por cercania: las reales salen del desensamblado,
// de la vtable que escribe cada una y de sus llamadores.
//   0 CDebug::DebugInitTextBuffer                        0x532240  `mov word ptr ds:[0B71508h],0; ret` (10 B)            llama CGame::Init1 (0x5BF905); no usa this
//   1 CCivilianPed::~CCivilianPed (cuerpo)               0x5DDBE0  `mov [ecx],86C0A8h; jmp 5E8620` (11 B)                vtable 0x86C0A8, salta a CPed::~CPed (0x5E8620, hook)
//   2 cBuoyancy::cBuoyancy (constructor)                 0x6C2740  `mov eax,ecx; xor ecx,ecx; mov [eax+4Ch],ecx; mov [eax+50h],ecx; ret` (11 B)   llama el inicializador estatico de mod_Buoyancy (0x853880)
//   3 CTaskComplexEnterCarAsDriver::~ (cuerpo)           0x640330  `mov [ecx],86EAACh; jmp 63DFA0` (11 B)                vtable 0x86EAAC, salta a CTaskComplexEnterCar::~ (0x63DFA0, hook)
//   4 CTaskComplexEnterCarAsPassenger::~ (cuerpo)        0x640380  `mov [ecx],86EADCh; jmp 63DFA0` (11 B)                vtable 0x86EADC, idem
//   5 CTaskComplexFacial::~ (cuerpo)                     0x690D90  `mov [ecx],870ADCh; jmp 61A3D0` (11 B)                vtable 0x870ADC, salta a CTaskComplex::~ (0x61A3D0, hook)
//   6 CTaskComplexPartnerDeal::~ (cuerpo)                0x684200  `mov [ecx],870754h; jmp 683A40` (11 B)                vtable 0x870754, salta a CTaskComplexPartner::~ (0x683A40, SIN hook)
//   7 CTaskSimpleAchieveHeading::~ (cuerpo)              0x667E70  `mov [ecx],86FD74h; jmp 61A3A0` (11 B)                vtable 0x86FD74, salta a CTask::~ (0x61A3A0, hook)
//   8 CTaskSimpleGoTo::~ (segundo cuerpo)                0x667A00  `mov [ecx],86FD1Ch; jmp 61A3A0` (11 B)                vtable 0x86FD1C, idem
//   9 CTaskSimpleSetStayInSamePlace::~ (cuerpo)          0x62F5D0  `mov [ecx],86DD50h; jmp 61A3A0` (11 B)                vtable 0x86DD50, idem
//
// Cada caso se ejecuta por la entrada del EXE. `this` apunta a una arena propia (con guardas de 0x40) en un desplazamiento alineado, desalineado o al borde de la ventana (la escritura entra en la guarda).
// El objeto parte de una imagen previa conocida (relleno de la arena y dwords previos a[6..9]) que el oraculo usa para predecir los bytes que cambian.
// Familia = n % 16: tipo = familia % 4 (0 alineado, 1 desalineado, 2 al borde de la ventana, 3 alineado con dword previo "interesante") y relleno = familia / 4.
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F122AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 10;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kObjWin = 0x180, kAWin = 0x40;   // arena del objeto (this) y arena secundaria centinela (no la toca nadie)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 kGlobalAddr = 0xB71508u;    // palabra que pone a 0 la funcion 0
constexpr std::size_t kGlobalBytes = 16;  // region vigilada (4 dwords desde 0xB71508)

// vtable que escribe cada funcion (la del EXE; 0 = no escribe vtable), direccion del colaborador al que salta (0 = ninguno), extremo de la escritura a partir de this (bytes) y si devuelve `this` en EAX
inline constexpr std::array<u32, kFunctions> kVtable{0u, 0x86C0A8u, 0u, 0x86EAACu, 0x86EADCu, 0x870ADCu, 0x870754u, 0x86FD74u, 0x86FD1Cu, 0x86DD50u};
inline constexpr std::array<u32, kFunctions> kCollab{0u, 0x5E8620u, 0u, 0x63DFA0u, 0x63DFA0u, 0x61A3D0u, 0x683A40u, 0x61A3A0u, 0x61A3A0u, 0x61A3A0u};
inline constexpr std::array<u32, kFunctions> kSpan{0u, 4u, 0x54u, 4u, 4u, 4u, 4u, 4u, 4u, 4u};
inline constexpr std::array<u32, kFunctions> kReturnsThis{0u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
inline constexpr std::array<u32, 8> kDtorVtables{0x86C0A8u, 0x86EAACu, 0x86EADCu, 0x870ADCu, 0x870754u, 0x86FD74u, 0x86FD1Cu, 0x86DD50u};

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 100001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

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

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { return Rand(n, nk()); }
};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de this en la arena (funcion 0: 0), a[1] = tipo (familia % 4), a[2] = relleno de la arena (0 aleatorio, 1 ceros, 2 unos, 3 0xCD),
//   a[3] = 0 (reservado), a[4] = ventana de la arena del objeto, a[5] = extremo de la escritura a partir de this (bytes),
//   a[6..9] = dwords previos: funciones 1, 3..9: a[6] = dword previo en [this] (vtable previa); funcion 2: a[6] = previo en this+0x4C, a[7] = previo en this+0x50;
//   funcion 0: a[6..9] = cuatro dwords previos desde 0xB71508 (la funcion solo escribe los dos primeros bytes).
inline u32 Interesting(Gen& g, u32 own, u32 other) {
    // aleatorio / ceros / unos / 0xDEADBEEF / la propia / la de otra funcion / la propia con un byte distinto / con dos bytes distintos / un byte igual
    switch (g.Pk({0u, 0u, 1u, 2u, 3u, 3u, 4u, 5u, 6u, 7u})) {
    case 0: return g.W();
    case 1: return 0u;
    case 2: return 0xFFFFFFFFu;
    case 3: return 0xDEADBEEFu;
    case 4: return own;
    case 5: return other;
    case 6: return own ^ (0xFFu << (8 * g.R(4)));
    case 7: return own ^ (0xFFFFu << (8 * g.R(3)));
    default: return (g.W() & ~(0xFFu << (8 * g.R(4)))) | (own & 0xFFu);
    }
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[3] = 0;
    c.a[4] = u32(kObjWin);
    c.a[5] = kSpan[c.fn];
    if (c.fn == 0) {
        // funcion sin this: cuatro dwords previos de la region global; el tipo decide la palabra baja (la que se pone a 0) y los vecinos (que no deben cambiar)
        c.a[0] = 0;
        for (int i = 0; i < 4; ++i) c.a[6 + i] = g.W();
        switch (kind) {
        case 0: break;                                                       // todo aleatorio
        case 1: c.a[6] &= 0xFFFF0000u; break;                               // palabra baja ya a cero, vecinos no nulos
        case 2: c.a[6] = 0xFFFFFFFFu; break;                                // todo unos
        default: c.a[6] = Interesting(g, 0xBEEF0000u, 0x0000BEEFu); if (g.P(50)) c.a[6] |= 0x00010000u; break;
        }
        if (g.P(25)) c.a[7] = 0;
        return;
    }
    switch (kind) {
    case 0: c.a[0] = 4 * g.R(0x40); break;
    case 1: c.a[0] = 4 * g.R(0x3F) + 1 + g.R(3); break;
    case 2: c.a[0] = u32(kObjWin) - c.a[5] - 4 + g.R(8); break;             // desde 4 bytes antes del borde hasta 3 bytes dentro de la guarda posterior
    default: c.a[0] = 4 * g.R(0x40); break;
    }
    const u32 own = kVtable[c.fn];
    const u32 other = kDtorVtables[g.R(8)];
    if (c.fn == 2) {
        c.a[6] = Interesting(g, 0u, other);
        c.a[7] = Interesting(g, 0u, other);
        return;
    }
    c.a[6] = Interesting(g, own, other);
    c.a[7] = c.a[8] = c.a[9] = 0;
}

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.edx = Rand(n, 2); c.ecx = Rand(n, 3) | 1u; c.stack = StackSeed(n);
    MakeAll(c, n);
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    return j.End();
}
}
