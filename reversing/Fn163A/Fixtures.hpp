#pragma once
// Fixtures del banco Fn163A (lote 163A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Cuatro cuerpos de destructor de tareas del EXE (no tenian hook; la DLL los llamaba con plugin::CallMethod o usaba el destructor generado, que deja la vtable de la DLL). Las clases se confirman por
// desensamblado, vtable y llamadores (ver README.md y fixture_oracle.py). Cada una se prueba por su entrada del EXE, con los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
//   idx  direccion  nombre real                                                           forma
//    0   0x634500   CTaskSimpleTriggerLookAt cuerpo del destructor (vtable 0x86E3CC)       thiscall() `ret`, marco SEH: [esi]=86E3CCh; si [esi+8]: CleanUpOldReference 571A00(ecx=[esi+8], arg esi+8); ~CTask 61A3A0
//    1   0x631980   CTaskComplexObserveTrafficLightsAndAchieveHeading cuerpo (0x86DFC4)     thiscall() `ret`: [ecx]=86DFC4h; jmp 61A3D0 (~CTaskComplex, con hook)
//    2   0x62FCF0   CTaskComplexDieInCar cuerpo del destructor (0x86DE04)                  thiscall() `ret`: [ecx]=86DE04h; jmp 61A3D0
//    3   0x688A00   CTaskSimpleSwim cuerpo del destructor (0x870920, marco SEH)            thiscall() `ret`: [esi]=870920h; si BYTE [esi+9]: RemoveAnimBlockRef 4D3FD0(indice del bloque del grupo de nado);
//                                                                                          CleanUpOldReference de [esi+38h] y de [esi+20h]; ~CTask 61A3A0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y los colaboradores falseados. Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado,
// 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F163AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

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
    std::array<u32, 24> a{};
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
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kTlBody, kCcBody, kSwimBody };
inline constexpr std::array<Kind, kFunctions> kKind{kTlBody, kCcBody, kCcBody, kSwimBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x634500, 0x631980, 0x62FCF0, 0x688A00};
// Colaborador final que falsea FkBody (~CTask 0x61A3A0 o ~CTaskComplex 0x61A3D0)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x61A3A0, 0x61A3D0, 0x61A3D0, 0x61A3A0};
// vtable que deja la funcion en [this]
inline constexpr std::array<u32, kFunctions> kVtable{0x86E3CC, 0x86DFC4, 0x86DE04, 0x870920};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x30, 4, 4, 0x64};
inline constexpr u32 kTlEntity = 8;                                   // CTaskSimpleTriggerLookAt::m_pEntity
inline constexpr u32 kSwimFlag = 9, kSwimPed = 0x20, kSwimEntity = 0x38;   // CTaskSimpleSwim::m_bAnimBlockRefAdded, m_pPed y m_pEntity
inline constexpr u32 kSwimGroupOffset = 0x58C;                         // ANIM_GROUP_SWIM (71) * 0x14: el EXE lee [[0xB4EA34] + 0x58C]
inline constexpr u32 kSwimGroupSpan = 0x590;
inline constexpr u32 kAnimBlocks = 0xB5D4A0;                           // ms_aAnimBlocks (bloques de 0x20 bytes)

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   todos: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite (EAX que devuelven los falsos y dword que el falso del cuerpo final deja en [this])
//   kTlBody: a[6] = modo de la entidad (+8: 0 nula, 1 en B a[7], 2 cruda a[8])
//   kSwimBody: a[6]/a[7]/a[8] = modo, desplazamiento en B y valor crudo del ped (+20h); a[9]/a[10]/a[11] = lo mismo para la entidad (+38h); a[16] = BYTE +9 (referencia al bloque de animaciones);
//         a[17] = desplazamiento en C de la tabla de grupos falsa (el banco apunta [0xB4EA34] a ella durante la llamada); a[18] = puntero de bloque de animaciones que hay en tabla+58Ch
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    const auto ref = [&] { return g.P(70) ? (Raw(g) | 1u) : Raw(g); };
    c.a[3] = prev();
    c.a[4] = Raw(g);
    c.a[5] = Raw(g);
    switch (kKind[c.fn]) {
    case kCcBody:
        break;
    case kTlBody:
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = ref();
        break;
    case kSwimBody: {
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = ref();
        c.a[9] = g.Pk({0u, 1u, 1u, 2u});
        c.a[10] = Offset(g, g.R(4), 4);
        c.a[11] = ref();
        const u32 w1 = g.W(), w2 = g.W(), idx = g.R(1200), unal = g.R(32);
        c.a[16] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x7Fu, w1 & 0xFFu, w1 & 0xFFu});
        c.a[17] = Offset(g, g.R(4), kSwimGroupSpan);
        c.a[18] = g.Pk({kAnimBlocks + 32u * idx, kAnimBlocks + 32u * idx, kAnimBlocks + 32u * idx, kAnimBlocks, kAnimBlocks + 32u * idx + unal, 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, kAnimBlocks + 31u, kAnimBlocks - 32u, kAnimBlocks - 1u, w2, w2});
        break;
    }
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
