#pragma once
// Fixtures del banco Fn156A (lote 156A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 17 asignadas: 16 reconstruidas;
// 0x746800 (comprobacion de integridad con llamada indirecta __usercall por EAX/EBX/ECX) se salta con evidencia (README.md).
//   idx  direccion  nombre real                                                                     forma
//    0   0x4B8340   CEventLeaderExitedCarAsDriver::CloneEditable (vtable 0x85B898, ranura 16)       thiscall() `ret`: CEvent::operator new 4B5620 (0x14), ctor 4AC450, [mem] = 0x85B898; eax = mem
//    1   0x661020   CTaskLeanOnVehicle::GetTaskType (vtable 0x86FAF4, ranura 4)                     thiscall() `ret`: mov eax,4C1h
//    2   0x664A30   CTaskComplexPassObject::ScalarDeletingDestructor (vtable 0x86FB18)              thiscall(flags) `ret 4`: cuerpo 661230 (con hook), CTask::operator delete 61A5B0 si bit 0
//    3   0x6610A0   CTaskLeanOnVehicle::Clone (ranura 1)                                             thiscall() `ret`: CTask::operator new 61A5A0 (0x2C), ctor 660F90 (vehiculo, duracion, byte); eax = lo que deja el ctor
//    4   0x7467D0   VideoPlayer::SumMasked24 (comprobacion de integridad)                           cdecl(inicio, fin) `ret`: suma de los 24 bits bajos de cada dword mientras inicio < fin (sin signo)
//    5   0x679AE0   CTaskSimpleJump::GetTaskType (vtable 0x87054C, ranura 4)                         thiscall() `ret`: mov eax,0D2h
//    6   0x67D960   CTaskComplexJump::ScalarDeletingDestructor (vtable 0x870570)                    thiscall(flags) `ret 4`: cuerpo 67A060 (con hook), CTask::operator delete 61A5B0 si bit 0
//    7   0x679AF0   CTaskSimpleJump cuerpo del destructor (marco SEH)                                thiscall() `ret`: vtable 87054Ch, SetFinishCallback 4CEBE0 si hay animacion (+28h), CleanUpOldReference 571A00 si hay entidad (+1Ch), ~CTask 61A3A0
//    8   0x63A140   CEventLeaderQuitEnteringCarAsDriver::GetLifeTime (vtable 0x86E6B0, ranura 3)    thiscall() `ret`: xor eax,eax
//    9   0x4B1040   CEventLeaderQuitEnteringCarAsDriver::TakesPriorityOver (ranura 11)              thiscall(evento) `ret 4`: mov al,1 (solo AL)
//   10   0x63A1A0   CEventLeaderQuitEnteringCarAsDriver::AffectsPed (ranura 5)                      thiscall(ped) `ret 4`: xor al,al (solo AL)
//   11   0x63A130   CEventLeaderQuitEnteringCarAsDriver::GetEventType (ranura 1)                    thiscall() `ret`: mov eax,39h
//   12   0x63A1E0   CEventLeaderQuitEnteringCarAsDriver::GetEventPriority (ranura 2)                thiscall() `ret`: mov eax,30h
//   13   0x63A1D0   CEventLeaderQuitEnteringCarAsDriver cuerpo del destructor                        thiscall() `ret`: [this] = 0x86E6B0; jmp 4AC010 (~CEvent, con hook)
//   14   0x63A1B0   CEventLeaderQuitEnteringCarAsDriver::ScalarDeletingDestructor (vtable 0x86E6B0)  cuerpo 63A1D0 (funcion 13), CEvent::operator delete 4B5630 si bit 0
//   15   0x63A150   CEventLeaderQuitEnteringCarAsDriver::Clone (ranura 4)                           thiscall() `ret`: CEvent::operator new 4B5620 (0xC), ctor 4ABFC0, [mem] = 0x86E6B0; eax = mem
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F156AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 16;

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
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B o C), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

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
enum Kind : u32 { kEvClone, kConst, kSdd, kLeanClone, kSum24, kJumpBody, kConstAL, kVtBody };
inline constexpr std::array<Kind, kFunctions> kKind{
    kEvClone, kConst, kSdd, kLeanClone, kSum24, kConst, kSdd, kJumpBody, kConst, kConstAL, kConstAL, kConst, kConst, kVtBody, kSdd, kEvClone};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4B8340, 0x661020, 0x664A30, 0x6610A0, 0x7467D0, 0x679AE0, 0x67D960, 0x679AF0, 0x63A140, 0x4B1040, 0x63A1A0, 0x63A130, 0x63A1E0, 0x63A1D0, 0x63A1B0, 0x63A150};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor, ~CTask, ~CEvent; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0x661230, 0, 0, 0, 0x67A060, 0x61A3A0, 0, 0, 0, 0, 0, 0x4AC010, 0x63A1D0, 0};
// Funcion de liberacion de los destructores con borrado (CTask::operator delete, CEvent::operator delete)
inline constexpr std::array<u32, kFunctions> kDelete{0, 0, 0x61A5B0, 0, 0, 0, 0x61A5B0, 0, 0, 0, 0, 0, 0, 0, 0x4B5630, 0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x85B898, 0x86FAF4, 0x86FB18, 0x86FAF4, 0, 0x87054C, 0x870570, 0x87054C, 0x86E6B0, 0x86E6B0, 0x86E6B0, 0x86E6B0, 0x86E6B0, 0x86E6B0, 0x86E6B0, 0x86E6B0};
// Bytes que cada funcion toca a partir de su puntero `this` (Sum24 calcula el suyo por caso)
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 0x14, 8, 4, 4, 0x2C, 4, 4, 4, 4, 4, 4, 4, 4};
// Tamano que reserva cada Clone (la memoria devuelta por el operator new falso se usa como objeto)
inline constexpr u32 kEvCloneSize0 = 0x14, kEvCloneSize15 = 0xC, kLeanSize = 0x2C;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kConst: a[3] = dword en [this] (no se lee)
//   kConstAL: a[3] = dword en [this] (no se lee); a[4] = argumento de pila (puntero crudo; no se lee)
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kVtBody: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite
//   kEvClone: a[3] = modo de la memoria (0 nula, 1 en B a[4]); a[4] = desplazamiento; a[5] = gDtorWrite; a[6] = crudo; a[7] = selector del EAX del constructor (0 memoria, 1 memoria + 4, 2 cero, 3 crudo a[6])
//   kLeanClone: a[3] = vehiculo (+8); a[4] = duracion (+0xC); a[5] = dword en +0x10 (solo se lee el byte bajo); a[6] = modo de la memoria (0 nula, 1 en B a[7], 2 en A a[7]); a[7] = desplazamiento;
//         a[8] = selector del EAX del constructor (0 memoria, 1 memoria + 4, 2 cero, 3 crudo a[10]); a[9] = gDtorWrite; a[10] = crudo
//   kJumpBody: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite; a[6] = modo de la animacion (+0x28) (0 nula, 1 en B a[7]); a[7] = desplazamiento en B;
//         a[8] = modo de la entidad de trepada (+0x1C) (0 nula, 1 en B a[9], 2 cruda a[10]); a[9] = desplazamiento en B; a[10] = crudo
//   kSum24: a[3] = modo (0 fin = inicio + 4n; 1 vacio; 2 fin < inicio; 3 fin = inicio + 4n - r (r de 1 a 3); 4 inicio >= fin crudos; 5 inicio >= fin crudos con signo invertido; 6 una sola lectura (fin = inicio + r));
//         a[4] = n (dwords); a[5] = r; a[6], a[7] = inicio y fin crudos (modos 4 y 5)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    switch (kKind[c.fn]) {
    case kConst:
        c.a[3] = Raw(g);
        break;
    case kConstAL:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kVtBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kEvClone:
        c.a[3] = g.Pk({0u, 1u, 1u, 1u});
        c.a[4] = AuxOffset(g, c.fn == 0 ? kEvCloneSize0 : kEvCloneSize15);
        c.a[5] = Raw(g);
        c.a[6] = Raw(g);
        c.a[7] = g.R(4);
        break;
    case kLeanClone:
        c.a[3] = Raw(g);
        c.a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 1000u, 0xFFFFu, 0x10000u, g.W(), g.W(), g.R(100000)});
        c.a[5] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x80u, 0x7Fu, 0xFFFFFF00u, 0xFFFFFF01u, 0x01010101u, 0xFFFFFFFFu, g.W(), g.W()});
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = c.a[6] == 2 ? Offset(g, g.R(4), kLeanSize) : AuxOffset(g, kLeanSize);
        c.a[8] = g.R(4);
        c.a[9] = Raw(g);
        c.a[10] = Raw(g);
        break;
    case kJumpBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u});
        c.a[7] = AuxOffset(g, 0x40);
        c.a[8] = g.Pk({0u, 1u, 2u, 2u});
        c.a[9] = AuxOffset(g, 0x40);
        c.a[10] = Raw(g);
        break;
    case kSum24: {
        const u32 mode = g.Pk({0u, 0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 3u, 3u, 4u, 5u, 6u});
        c.a[3] = mode;
        const u32 count = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 8u, 15u, 16u, 255u, 256u, 257u, 300u, 511u, 512u, 1000u, g.R(1000), g.R(1000), g.R(64), g.R(8)});
        c.a[4] = count;
        c.a[5] = 1u + g.R(3);
        c.a[6] = 0; c.a[7] = 0;
        if (mode == 4) {
            const u32 e = g.Pk({0u, 1u, 0x1000u, 0x400000u, 0x7FFFFFFFu, 0xFFFFFFFEu, g.R(0x10000), g.W() & 0x7FFFFFFFu});
            const u32 b = e + g.Pk({0u, 0u, 1u, 4u, 0x100u});
            c.a[6] = b < e ? e : b;           // inicio >= fin (sin signo), sin envolver
            c.a[7] = e;
        } else if (mode == 5) {
            const u32 e = g.Pk({0u, 1u, 0x1000u, 0x400000u, 0x7FFFFFFFu, g.R(0x10000), g.W() & 0x7FFFFFFFu});
            c.a[6] = 0x80000000u | (g.W() & 0x7FFFFFFFu);   // con signo es negativo (menor que el fin), sin signo es mayor
            c.a[7] = e;
        }
        // el objeto es la ventana de lectura: inicio y n dwords (la lectura puede pasar hasta 3 bytes del ultimo dword)
        c.a[0] = Offset(g, kind, 4 * count + 8);
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
