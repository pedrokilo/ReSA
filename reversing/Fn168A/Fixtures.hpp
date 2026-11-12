#pragma once
// Fixtures del banco Fn168A (lote 168A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables leidas del PE y de los llamadores, ver README.md y fixture_oracle.py). 23 asignadas, 23 reconstruidas.
//   idx  direccion  nombre real                                                                        forma
//    0   0x65DB50   CEventLeanOnVehicle::GetEventType (vtable 0x86F878, ranura 1)                      thiscall() `ret`: mov eax,5Ch
//    1   0x65DB60   CEventLeanOnVehicle::GetLifeTime (ranura 3)                                        thiscall() `ret`: xor eax,eax
//    2   0x65DB70   CEventLeanOnVehicle::Clone (ranura 4)                                              thiscall() `ret`: CEvent::operator new 4B5620 (0x14), ctor 65DAF0 (vehiculo +0xC, duracion +0x10); eax = lo que deja el ctor
//    3   0x65DBE0   CEventLeanOnVehicle::AffectsPed (ranura 5)                                         thiscall(ped) `ret 4`: mov al,1 (solo AL)
//    4   0x65DBF0   CEventLeanOnVehicle::ScalarDeletingDestructor (ranura 0)                           thiscall(flags) `ret 4`: cuerpo 65DC10 (funcion 5), CEvent::operator delete 4B5630 si bit 0
//    5   0x65DC10   CEventLeanOnVehicle cuerpo del destructor (marco SEH)                              thiscall() `ret`: [this] = 86F878h; CleanUpOldReference 571A00 si [this+0xC]; ~CEvent 4AC010
//    6   0x65DE10   CEventLeanOnVehicle::GetEventPriority (ranura 2; el listado decia CEventPassObject) thiscall() `ret`: mov eax,0Ch
//    7   0x65DD50   CEventPassObject::GetEventType (vtable 0x86F8B8, ranura 1)                         thiscall() `ret`: mov eax,59h
//    8   0x65DD60   CEventPassObject::GetLifeTime (ranura 3)                                           thiscall() `ret`: xor eax,eax
//    9   0x65DD70   CEventPassObject::Clone (ranura 4)                                                 thiscall() `ret`: CEvent::operator new 4B5620 (0x14), ctor 65DC70 (dador +0xC, BYTE +0x10 extendido a cero)
//   10   0x65DDE0   CEventPassObject::AffectsPed (ranura 5)                                            thiscall(ped) `ret 4`: mov al,1 (solo AL)
//   11   0x65DDF0   CEventPassObject::ScalarDeletingDestructor (ranura 0)                              thiscall(flags) `ret 4`: cuerpo 65DCF0 (con hook), CEvent::operator delete 4B5630 si bit 0
//   12   0x65DE20   CEventPassObject::GetEventPriority (ranura 2)                                       thiscall() `ret`: mov eax,0Bh
//   13   0x608FD0   CEventNewGangMember::GetEventType (vtable 0x86D0A0, ranura 1)                       thiscall() `ret`: mov eax,5Ah
//   14   0x608FE0   CEventNewGangMember::GetLifeTime (ranura 3)                                         thiscall() `ret`: xor eax,eax
//   15   0x608FF0   CEventNewGangMember::Clone (ranura 4)                                               thiscall() `ret`: CEvent::operator new 4B5620 (0x10), ctor 608F70 (miembro +0xC)
//   16   0x609050   CEventNewGangMember::AffectsPed (ranura 5)                                          thiscall(ped) `ret 4`: mov al,1 (solo AL)
//   17   0x609060   CEventNewGangMember::ScalarDeletingDestructor (ranura 0)                            thiscall(flags) `ret 4`: cuerpo 609080 (funcion 18), CEvent::operator delete 4B5630 si bit 0
//   18   0x609080   CEventNewGangMember cuerpo del destructor (marco SEH)                               thiscall() `ret`: [this] = 86D0A0h; CleanUpOldReference 571A00 si [this+0xC]; ~CEvent 4AC010
//   19   0x65DE60   CEntitySeekPosCalculatorXYOffset constructor desde desplazamiento (el listado decia CEventPassObject)  thiscall(CVector*) `ret 4`: [this] = 86F8F8h; copia 3 dwords de [arg] a +4, +8, +0Ch; eax = this
//   20   0x65DE90   CEntitySeekPosCalculator cuerpo del destructor (tercera copia; el listado decia CEventPassObject) thiscall() `ret`: mov dword [ecx],859DC4h
//   21   0x605300   CEventScanner constructor (el listado decia CDecisionMakerTypes)                    thiscall() `ret`: temporizadores a cero, atractores (+0x1C: byte 1, 0, Clear 5FFF90), banderas del escaner de conocidos (+0xB8 = 1), [this] = tiempo - ftol(rand() & 0xFFFF * 2^-15 * -3000)
//   22   0x6070F0   CDecisionMakerTypes::CopyDecisionMaker (el listado decia CDecisionMakerTypes)       thiscall(indice, tipo, mision) `ret 0Ch`: elige origen (this+0xCB50 / this+0xE824 / this+4+indice*0x99C) y salta a AddDecisionMaker 607050
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

namespace F168AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;

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
// Un dword no nulo (la referencia que el destructor limpia solo si no es nula)
inline u32 RawNonZero(Gen& g) {
    const u32 v = Raw(g);
    return v ? v : 0x00B74F00u;
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Un float en forma de bits (el constructor del calculador copia dwords, no floats): ceros con signo, unos, infinitos, NaN silenciosos y senalizantes, subnormales, maximos
inline u32 FloatBits(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W();
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FA00000u, 0xFFA00001u, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0x42C80000u, w1, w2, w3});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kConstAL, kClone, kSdd, kEvBody, kXyCtor, kVtOnly, kScanner, kCopyDm };
inline constexpr std::array<Kind, kFunctions> kKind{
    kConst, kConst, kClone, kConstAL, kSdd, kEvBody, kConst,
    kConst, kConst, kClone, kConstAL, kSdd, kConst,
    kConst, kConst, kClone, kConstAL, kSdd, kEvBody,
    kXyCtor, kVtOnly, kScanner, kCopyDm};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{
    0x65DB50, 0x65DB60, 0x65DB70, 0x65DBE0, 0x65DBF0, 0x65DC10, 0x65DE10,
    0x65DD50, 0x65DD60, 0x65DD70, 0x65DDE0, 0x65DDF0, 0x65DE20,
    0x608FD0, 0x608FE0, 0x608FF0, 0x609050, 0x609060, 0x609080,
    0x65DE60, 0x65DE90, 0x605300, 0x6070F0};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor o ~CEvent; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{
    0, 0, 0, 0, 0x65DC10, 0x4AC010, 0,
    0, 0, 0, 0, 0x65DCF0, 0,
    0, 0, 0, 0, 0x609080, 0x4AC010,
    0, 0, 0, 0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{
    0x86F878, 0x86F878, 0x86F878, 0x86F878, 0x86F878, 0x86F878, 0x86F878,
    0x86F8B8, 0x86F8B8, 0x86F8B8, 0x86F8B8, 0x86F8B8, 0x86F8B8,
    0x86D0A0, 0x86D0A0, 0x86D0A0, 0x86D0A0, 0x86D0A0, 0x86D0A0,
    0x86F8F8, 0x859DC4, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{
    4, 4, 0x14, 4, 4, 0x10, 4,
    4, 4, 0x14, 4, 4, 4,
    4, 4, 0x10, 4, 4, 0x10,
    0x10, 4, 0xD4, 4};
// Tamano que reserva cada Clone con operator new (la memoria devuelta por el falso se usa como objeto de B)
inline constexpr std::array<u32, kFunctions> kCloneSize{0, 0, 0x14, 0, 0, 0, 0, 0, 0, 0x14, 0, 0, 0, 0, 0, 0x10, 0, 0, 0, 0, 0, 0, 0};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kConst: a[3] = dword en [this] (no se lee)
//   kConstAL: a[3] = dword en [this] (no se lee); a[4] = argumento de pila (puntero crudo; no se lee)
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kEvBody: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite; a[6] = referencia en +0xC (0 o crudo)
//   kClone: a[3] = modo de la memoria (0 nula, 1 en B a[4]); a[4] = desplazamiento en B; a[5] = gDtorWrite; a[6] = crudo; a[7] = selector del EAX del constructor (0 memoria, 1 memoria + 4, 2 cero, 3 crudo a[6]);
//         a[8] = dword en +0xC; a[9] = dword en +0x10 (solo se usa en los dos primeros: LeanOnVehicle lo pasa entero, PassObject solo su byte bajo)
//   kXyCtor: a[3] = dword previo en [this]; a[4] = origen (0 en B, 1 en A relativo a `this`); a[5] = desplazamiento en B (modo 0) o delta con signo respecto a `this` (modo 1); a[6], a[7], a[8] = los tres dwords
//         del origen (bits de floats)
//   kVtOnly: a[3] = dword previo en [this]
//   kScanner: a[3] = tiempo del juego (CTimer, 0xB7CB84); a[4] = lo que devuelve rand()
//   kCopyDm: a[3] = indice (dword); a[4] = tipo (dword); a[5] = mision (dword de pila; solo cuenta el byte bajo); a[6] = EAX que devuelve el falso de AddDecisionMaker
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
    case kEvBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.P(40) ? 0u : RawNonZero(g);
        break;
    case kClone:
        c.a[3] = g.Pk({0u, 1u, 1u, 1u});
        c.a[4] = AuxOffset(g, kCloneSize[c.fn]);
        c.a[5] = Raw(g);
        c.a[6] = Raw(g);
        c.a[7] = g.R(4);
        c.a[8] = Raw(g);
        if (c.fn == 2) c.a[9] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 1000u, 0xFFFFu, 0x10000u, g.W(), g.W(), g.R(100000)});
        else if (c.fn == 9) c.a[9] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x80u, 0x7Fu, 0xFFFFFF00u, 0xFFFFFF01u, 0x01010101u, 0xFFFFFFFFu, g.W(), g.W()});
        else c.a[9] = g.W();
        break;
    case kXyCtor:
        c.a[3] = prev();
        c.a[4] = g.Pk({0u, 0u, 0u, 1u});
        if (c.a[4] == 0) c.a[5] = g.P(70) ? AuxOffset(g, 12) : g.R(u32(kWin) - 12);
        else c.a[5] = g.Pk({0xFFFFFFF4u, 0xFFFFFFF8u, 0xFFFFFFFCu, 0xFFFFFFFDu, 0xFFFFFFFEu, 0xFFFFFFFFu, 0u, 1u, 2u, 3u, 4u, 8u, 12u});
        c.a[6] = FloatBits(g);
        c.a[7] = FloatBits(g);
        c.a[8] = FloatBits(g);
        break;
    case kVtOnly:
        c.a[3] = prev();
        break;
    case kScanner:
        c.a[3] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 2999u, 3000u, 100000u, g.W(), g.W(), g.R(10000000)});
        c.a[4] = g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0xFFFF0000u, 0xDEADBEEFu, 0x4000u, 0x2AAAu, 0x5555u, g.W(), g.W(), g.R(0x8000), g.R(0x8000)});
        break;
    case kCopyDm:
        c.a[3] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0u, 1u, 2u, 14u, 15u, 19u, 20u, 21u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu, g.R(20), g.R(20), g.W()});
        c.a[4] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0xFFu, 0x100u, 0x80000000u, 0xFFFFFFFFu, g.W(), g.R(2)});
        c.a[5] = g.Pk({0u, 1u, 1u, 0u, 2u, 0x7Fu, 0xFFu, g.R(256)});
        c.a[6] = Raw(g);
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
