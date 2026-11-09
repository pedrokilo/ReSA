#pragma once
// Fixtures del banco Fn141A (lote 141A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables leidas del propio EXE y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                                          forma
//    0   0x5D6830   CCustomBuildingDNPipeline::AdvanceWorkingIndex                      cdecl(arg) `ret`: [C02C14]=([C02C14]+1)&0xF; [C02C18]=arg
//    1   0x5D6E90   CCustomBuildingDNPipeline::GetExtraNightColors                      cdecl(geometria) `ret`: mov eax,[ecx+eax] con ecx=[8D12BC] (lee el dword, no la direccion)
//    2   0x532540   CDummy::Constructor (cuerpo)                                        thiscall() `ret`: call 532A90; byte[+36h]=(byte&FDh)|5; [this]=8638C0h
//    3   0x532560   CDummy::Destructor (cuerpo)                                         thiscall() `ret`: mov [ecx],8638C0h; jmp 535E90 (~CEntity)
//    4   0x608F00   CWeaponInfo::GetAnimLoopStart                                       thiscall(bool) `ret 4`: fld dword [ecx+4Ch o +40h]  (resultado en ST0)
//    5   0x608F20   CWeaponInfo::GetAnimLoopEnd                                         thiscall(bool) `ret 4`: fld dword [ecx+50h o +44h]
//    6   0x631910   CTaskComplexObserveTrafficLightsAndAchieveHeading::Constructor      thiscall(duracion, rumbo) `ret 8`: call 61A3B0; [+0Ch]=a; [this]=86DFC4h; [+10h]=b
//    7   0x631940   CTaskComplexObserveTrafficLightsAndAchieveHeading::GetTaskType      thiscall() `ret`: mov eax,0E1h (vtable 0x86DFC4, ranura 4)
//    8   0x6CE440   CQuadBike::ScalarDeletingDestructor (vtable 0x871AE8, ranura 0)     thiscall(banderas) `ret 4`: call 6CDC30; si bit0: call 6E2D90
//    9   0x6CED10   CTrailer::Destructor (cuerpo)                                       thiscall() `ret`: mov [ecx],871C28h; jmp 6A61E0 (~CAutomobile)
//   10   0x5F6900   CTaskAllocatorKillThreatsBasicRandomGroup::ScalarDeletingDestructor thiscall(banderas) `ret 4`: call 5F6920; si bit0: call 69D990 (vtable 0x86C80C)
//   11   0x5F6920   CTaskAllocatorKillThreatsBasicRandomGroup::Destructor (cuerpo)       thiscall() `ret`: mov [ecx],86C80Ch; jmp 69C780
//   12   0x633290   CTaskComplexAttractorPartnerWait::Destructor (cuerpo)               thiscall() `ret`: mov [ecx],86E284h; jmp 61A3D0
//   13   0x638FD0   CTaskComplexUseAttractorPartner::ScalarDeletingDestructor (0x86E2B0) thiscall(banderas) `ret 4`: call 633300; si bit0: call 61A5B0
//   14   0x63CE70   CTaskComplexDriveToPoint::Destructor (cuerpo)                       thiscall() `ret`: mov [ecx],86E9DCh; jmp 63CA40
//   15   0x645350   CTaskComplexDrivePointRoute::ScalarDeletingDestructor (0x86EBF8)     thiscall(banderas) `ret 4`: call 643490; si bit0: call 61A5B0
//   16   0x63D100   CTaskComplexEnterAnyCarAsDriver::Destructor (cuerpo)                 thiscall() `ret`: mov [ecx],86EA14h; jmp 61A3D0
//   17   0x6436E0   CTaskComplexShuffleSeats::ScalarDeletingDestructor (0x86EA40)        thiscall(banderas) `ret 4`: call 63D1A0; si bit0: call 61A5B0
//   18   0x668630   CTaskComplexGoToPointAndStandStill::Destructor (cuerpo)              thiscall() `ret`: mov [ecx],86FDA8h; jmp 61A3D0
//   19   0x66DD50   CTaskComplexGoToPointAiming::ScalarDeletingDestructor (0x86FE00)     thiscall(banderas) `ret 4`: call 668870; si bit0: call 61A5B0
//   20   0x636830   CTaskComplexUseEffectRunning::ScalarDeletingDestructor (0x86E594)    thiscall(banderas) `ret 4`: call 636850; si bit0: call 61A5B0
//   21   0x636850   CTaskComplexUseEffectRunning::Destructor (cuerpo)                    thiscall() `ret`: mov [ecx],86E594h; jmp 632270
//   22   0x636910   CTaskComplexUseEffectSprinting::ScalarDeletingDestructor (0x86E5C0)  thiscall(banderas) `ret 4`: call 636930; si bit0: call 61A5B0
//   23   0x636930   CTaskComplexUseEffectSprinting::Destructor (cuerpo)                  thiscall() `ret`: mov [ecx],86E5C0h; jmp 632270
//
// Cada caso se ejecuta por la entrada del EXE con una arena propia (A, ventana de 4 KiB con guardas de 0x40) y, para 0 y 1, memoria global real del juego (se guarda y se restaura tras cada lado).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de la arena = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F141AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 24;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de la arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Formas (ver la cabecera)
enum Shape : unsigned { kShG0, kShG1, kShDummyCtor, kShBody, kShFloat, kShObsCtor, kShConst, kShScalar };
inline constexpr std::array<Shape, kFunctions> kShape{kShG0, kShG1, kShDummyCtor, kShBody, kShFloat, kShFloat, kShObsCtor, kShConst, kShScalar, kShBody, kShScalar, kShBody,
                                                      kShBody, kShScalar, kShBody, kShScalar, kShBody, kShScalar, kShBody, kShScalar, kShScalar, kShBody, kShScalar, kShBody};
// Vtable que deja el cuerpo (0 = no aplica)
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0x8638C0, 0x8638C0, 0, 0, 0x86DFC4, 0, 0x871AE8, 0x871C28, 0x86C80C, 0x86C80C, 0x86E284, 0x86E2B0, 0x86E9DC, 0x86EBF8, 0x86EA14, 0x86EA40,
                                                    0x86FDA8, 0x86FE00, 0x86E594, 0x86E594, 0x86E5C0, 0x86E5C0};
// Colaborador de cuerpo (el que llama el cuerpo o el borrado con banderas) y operator delete (0 = no aplica)
inline constexpr std::array<u32, kFunctions> kCollab{0, 0, 0x532A90, 0x535E90, 0, 0, 0x61A3B0, 0, 0x6CDC30, 0x6A61E0, 0x5F6920, 0x69C780, 0x61A3D0, 0x633300, 0x63CA40, 0x643490, 0x61A3D0, 0x63D1A0,
                                                     0x61A3D0, 0x668870, 0x636850, 0x632270, 0x636930, 0x632270};
inline constexpr std::array<u32, kFunctions> kDelete{0, 0, 0, 0, 0, 0, 0, 0, 0x6E2D90, 0, 0x69D990, 0, 0, 0x61A5B0, 0, 0x61A5B0, 0, 0x61A5B0, 0, 0x61A5B0, 0x61A5B0, 0, 0x61A5B0, 0};
// Bytes que cada funcion toca a partir de `this` (para colocarlo al borde de la ventana)
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 0x38, 4, 0x54, 0x54, 0x14, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};

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
// Relleno de una region: 0 aleatorio (semilla), 1 ceros, 2 unos, 3 0xCD
inline void FillKind(u8* dst, std::size_t bytes, u32 kind, u32 seed) {
    if (kind == 0) FillWords(dst, bytes, seed);
    else std::memset(dst, kind == 1 ? 0x00 : (kind == 2 ? 0xFF : 0xCD), bytes);
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
    Js& H(const char* k, const std::vector<u8>& v) {
        constexpr char d[] = "0123456789abcdef";
        Key(k); s += '"';
        for (u8 b : v) { s += d[b >> 4]; s += d[b & 15]; }
        s += '"'; return *this;
    }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 16> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
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

// Desplazamiento de un objeto de `span` bytes en la ventana segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x10);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// Un dword "interesante" para argumentos y datos crudos (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// vtable previa de [this]: la propia, la de otra clase del lote, ceros, unos, un bit distinto, la de CTask, aleatoria
inline u32 PrevVtable(Gen& g, u32 own) {
    const u32 pick = kVtable[g.R(kFunctions)];
    const u32 other = pick ? pick : 0x86D48Cu;
    switch (g.R(8)) {
    case 0: return own;
    case 1: return other;
    case 2: return 0u;
    case 3: return 0xFFFFFFFFu;
    case 4: return own ^ (1u << g.R(32));
    case 5: return 0x86D48Cu;
    default: return g.W();
    }
}
inline u32 FloatBits(Gen& g) {
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800001u, 0xFFBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x80000001u, 0x7F7FFFFFu, 0x41200000u, 0x3DCCCCCDu,
                 g.W(), g.W(), g.W(), g.W(), g.W()});
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de la arena A, de las regiones globales y reservadas; el resto por forma:
//   G0   : a[3] = argumento, a[4] = indice previo en 0xC02C14, a[5] = dword previo en 0xC02C18
//   G1   : a[3] = desplazamiento del plugin (dword de 0x8D12BC), a[4] = dword que hay en [geometria + desplazamiento] (= arena A + a[0])
//   DUMMY: a[3] = dword que el falso de ~CEntity-ctor deja en [this], a[4] = byte que deja en +0x36, a[5] = EAX del falso
//   BODY : a[3] = dword previo en [this], a[4] = dword que el falso del cuerpo base deja en [this], a[5] = EAX del falso
//   FLOAT: a[3] = argumento de pila (el EXE lee solo el byte bajo), a[4..7] = floats en +0x40, +0x44, +0x4C, +0x50
//   OBS  : a[3] = duracion, a[4] = bits del rumbo, a[5] = dword que deja el falso de la base en [this], a[6] = EAX del falso
//   CONST: a[3] = dword en [this]
//   SCAL : a[6] = dword previo en [this], a[7] = banderas (argumento de pila), a[8] = dword que deja el falso del cuerpo en [this], a[9] = EAX de los falsos
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 own = kVtable[c.fn];
    switch (kShape[c.fn]) {
    case kShG0:
        c.a[3] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), 0xFu, 0x10u, 0x7FFFFFFFu});
        c.a[4] = g.Pk({0u, 1u, 0xEu, 0xFu, 0x10u, 0x11u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), g.W() & 0xFu, g.W()});
        c.a[5] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W(), 0x7FFFFFFFu});
        break;
    case kShG1:
        c.a[3] = g.Pk({0u, 1u, 4u, 0x30u, 0xFFFFFFFFu, 0xFFFFFFFCu, 0xFFFFFFF0u, 0x80000000u, 0x7FFFFFFFu, 0x1000u, 0x12345678u, g.W(), g.W(), g.W() | 0x80000000u, g.W() & 0xFFFu});
        c.a[4] = Raw(g);
        break;
    case kShDummyCtor:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x86D48Cu, 0x8638C0u, 0x85F000u, g.W(), g.W()});
        c.a[4] = g.Pk({0u, 0xFFu, 0x02u, 0x05u, 0xFDu, 0x07u, 0xF8u, 0x55u, 0xAAu, g.W() & 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        c.a[5] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), 0x00BADC00u + g.R(256)});
        break;
    case kShBody:
        c.a[3] = PrevVtable(g, own);
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x86D48Cu, own, g.W(), g.W(), 0x86D4CCu});
        c.a[5] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), 0x00BADC00u + g.R(256)});
        break;
    case kShFloat:
        c.a[3] = g.Pk({0u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x10000u, 0x80000000u, g.W() & ~0xFFu, g.W() | 1u, g.W()});
        c.a[4] = FloatBits(g); c.a[5] = FloatBits(g); c.a[6] = FloatBits(g); c.a[7] = FloatBits(g);
        break;
    case kShObsCtor:
        c.a[3] = Raw(g);
        c.a[4] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0x40490FDBu, 0xC0490FDBu, 0x7FC00000u, 0x7F800001u, 0xFF800000u, 0x00000001u, g.W(), g.W(), g.W()});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 0x86D48Cu, own, g.W(), g.W()});
        c.a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), 0x00BADC00u + g.R(256)});
        break;
    case kShConst:
        c.a[3] = Raw(g);
        break;
    case kShScalar:
        c.a[6] = PrevVtable(g, own);
        switch (g.R(9)) {
        case 0: c.a[7] = 0; break;
        case 1: c.a[7] = 1; break;
        case 2: c.a[7] = g.Pk({2u, 4u, 0x80u, 0xFEu, 0x100u, 0x10000u, 0x80000000u, 0xFFFFFF00u, 0xFFFFFE00u}); break;
        case 3: c.a[7] = g.Pk({3u, 5u, 0x81u, 0xFFu, 0x101u, 0x10001u, 0x80000001u, 0xFFFFFF01u, 0xFFFFFF03u, 0x12345679u}); break;
        case 4: c.a[7] = g.W() & ~1u; break;
        case 5: c.a[7] = g.W() | 1u; break;
        case 6: c.a[7] = 0xFFFFFFFFu; break;
        default: c.a[7] = g.W(); break;
        }
        switch (g.R(7)) {
        case 0: c.a[8] = 0x86D48Cu; break;
        case 1: c.a[8] = own; break;
        case 2: c.a[8] = c.a[6]; break;
        case 3: c.a[8] = 0; break;
        case 4: c.a[8] = 0xFFFFFFFFu; break;
        case 5: c.a[8] = c.a[6] ^ (0xFFu << (8 * g.R(4))); break;
        default: c.a[8] = g.W(); break;
        }
        c.a[9] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W(), 0x00BADC00u + g.R(256)});
        break;
    }
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
    j.H("s", c.s);
    return j.End();
}
}
