#pragma once
// Fixtures del banco Fn160A (lote 160A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 23 asignadas: las 23 reconstruidas.
//   idx  direccion  nombre real                                                                         forma
//    0   0x68B050   CTaskSimpleSwim::Clone (vtable 0x870920, ranura 1)                                  Clone: CTask::operator new 61A5A0 (0x64), ctor 688930(&pos +14h, ped +20h); null si no hay memoria
//    1   0x68B0C0   CTaskSimplePlayerOnFoot::ScalarDeletingDestructor (vtable 0x8708EC)                 thiscall(flags) `ret 4`: cuerpo 6857D0 (con hook propio), CTask::operator delete 61A5B0 si bit 0
//    2   0x68B0E0   CTaskSimpleSwim::ScalarDeletingDestructor (vtable 0x870920)                         idem, cuerpo 688A00 (SIN hook: falso en el EXE y en la envoltura de la DLL)
//    3   0x66B690   CTaskComplexGoToAttractor::GetTaskType (vtable 0x86FF3C, ranura 4)                  thiscall() `ret`: mov eax,393h
//    4   0x66B6B0   CTaskComplexGoToAttractor::MakeAbortable (ranura 6)                                 thiscall(ped, prioridad, evento) `ret 0Ch`: salto a [[this+8]+18h] con los mismos argumentos
//    5   0x66B710   CTaskComplexGoToAttractor::ControlSubTask (ranura 10)                               thiscall(ped) `ret 4`: eax = [this+8]
//    6   0x66D1E0   CTaskComplexGoToPointAnyMeans::Clone (vtable 0x86FF68, ranura 1)                    Clone: CTask::operator new 61A5A0 (0x34), ctor 66B790(estado +18h, &pos +0Ch, vehiculo +20h, radio +1Ch, modelo +24h)
//    7   0x6705B0   CTaskComplexGoToPointAnyMeans::ScalarDeletingDestructor                             idem 1, cuerpo 66B830 (con hook propio)
//    8   0x67A050   CTaskComplexJump::GetTaskType (vtable 0x870570, ranura 4)                           thiscall() `ret`: mov eax,0D3h
//    9   0x67A100   CTaskComplexJump::ControlSubTask (ranura 10)                                        thiscall(ped) `ret 4`: eax = [this+8]
//   10   0x67C610   CTaskSimpleClimb::Clone (vtable 0x87059C, ranura 1)                                 Clone: CTask::operator new 61A5A0 (0x30), ctor 67A110(entidad +24h, &punto +18h, rumbo +14h, bytes +0Fh, +0Dh, +0Bh)
//   11   0x67DBC0   CTaskSimpleClimb::ScalarDeletingDestructor                                          idem 1, cuerpo 67A1D0 (con hook, funcion propia)
//   12   0x662350   CTaskComplexGangLeader::ScalarDeletingDestructor (vtable 0x86F8FC)                  idem 1, cuerpo 65DF30 (con hook propio)
//   13   0x667310   CPathNode::GetNodeCoors(CVector*)                                                   thiscall(destino) `ret 4`: tres int16 de +8, +A, +C por 0.125 (se leen antes de escribir) a destino (x, y, z); void
//   14   0x667460   AnimNameIsEmpty (cadena de ECX contra la vacia 0x858B54, un byte)                   thiscall() `ret`: al = (primer byte == 0)
//   15   0x5FED80   CEventPedCollisionWithPlayer cuerpo del destructor                                  thiscall() `ret`: [this] = 86C970h; salto a 4ACA70 (cuerpo de ~CEventPedCollisionWithPed, con hook)
//   16   0x5FED90   CEventPedCollisionWithPlayer::GetEventType (vtable 0x86C970, ranura 1)              thiscall() `ret`: mov eax,3
//   17   0x5FEDA0   CEventPedCollisionWithPlayer::Clone (ranura 4)                                      Clone: CEvent::operator new 4B5620 (0x34), ctor 4AC990(7 argumentos), [obj] = 86C970h; devuelve el objeto
//   18   0x5FEE20   CEventPedCollisionWithPlayer::ScalarDeletingDestructor                              idem 1, cuerpo 5FED80 (con hook propio), CEvent::operator delete 4B5630 si bit 0
//   19   0x5FEE80   CEventPlayerCollisionWithPed cuerpo del destructor                                  idem 15 con vtable 86C9B0h
//   20   0x5FEE90   CEventPlayerCollisionWithPed::GetEventType (vtable 0x86C9B0, ranura 1)              thiscall() `ret`: mov eax,4
//   21   0x5FEEA0   CEventPlayerCollisionWithPed::Clone (ranura 4)                                      idem 17 con vtable 86C9B0h
//   22   0x5FEF20   CEventPlayerCollisionWithPed::ScalarDeletingDestructor                              idem 18, cuerpo 5FEE80 (con hook propio)
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

namespace F160AFix {
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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kArgs = 32;     // a[0..15] como en Fn159A (a[12..15] = semillas de las arenas y de la memoria global); a[16..27] = palabras de los objetos de los Clone
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kArgs> a{};
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
// Un float sin NaN senalizador: el EXE copia estos campos con fld/fstp (un NaN senalizador saldria silencioso) o con push; el C++ los copia tal cual. Fuera de los fixtures: NaN senalizador (ver README.md)
inline u32 Quiet(u32 w) { return ((w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu)) ? (w | 0x00400000u) : w; }
inline u32 FloatBits(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W();
    return Quiet(g.Pk({0u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x00000001u, 0x80000000u, 0x00800000u, 0x41200000u, w1, w2, w3}));
}
// Palabra de un objeto de Clone: ceros, unos, bytes con el bit alto, palabras de 16 bits con signo, aleatoria
inline u32 Word(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W(), w4 = g.W() & 0x80808080u, w5 = g.W() | 0x80008000u, w6 = g.W() & 0x7F7F7F7Fu;
    return g.Pk({0u, 0xFFFFFFFFu, 0x80u, 0xFFu, 0x7Fu, 0x8080u, 0x01010101u, 0xFF00FF00u, 0x00010001u, 0xFFFF0000u, 0x0000FFFFu, w1, w2, w3, w4, w5, w6});
}

// Clases de funcion (forma)
enum Kind : u32 { kSdd, kConst, kRetField, kAbort, kClone, kEvBody, kNode, kEmpty };
inline constexpr std::array<Kind, kFunctions> kKind{kClone, kSdd, kSdd, kConst, kAbort, kRetField, kClone, kSdd, kConst, kRetField, kClone, kSdd, kSdd, kNode, kEmpty,
                                                    kEvBody, kConst, kClone, kSdd, kEvBody, kConst, kClone, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x68B050, 0x68B0C0, 0x68B0E0, 0x66B690, 0x66B6B0, 0x66B710, 0x66D1E0, 0x6705B0, 0x67A050, 0x67A100, 0x67C610, 0x67DBC0,
                                                    0x662350, 0x667310, 0x667460, 0x5FED80, 0x5FED90, 0x5FEDA0, 0x5FEE20, 0x5FEE80, 0x5FEE90, 0x5FEEA0, 0x5FEF20};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0x6857D0, 0x688A00, 0, 0, 0, 0, 0x66B830, 0, 0, 0, 0x67A1D0, 0x65DF30, 0, 0, 0x4ACA70, 0, 0, 0x5FED80, 0x4ACA70, 0, 0, 0x5FEE80};
// Liberacion que llaman los destructores con borrado: CTask::operator delete (0x61A5B0) o CEvent::operator delete (0x4B5630)
inline constexpr std::array<u32, kFunctions> kDelExe{0, 0x61A5B0, 0x61A5B0, 0, 0, 0, 0, 0x61A5B0, 0, 0, 0, 0x61A5B0, 0x61A5B0, 0, 0, 0, 0, 0, 0x4B5630, 0, 0, 0, 0x4B5630};
// vtable que puede traer el objeto antes (dwords previos plausibles de [this]) y que dejan los cuerpos de evento y los Clone de evento
inline constexpr std::array<u32, kFunctions> kVtable{0x870920, 0x8708EC, 0x870920, 0, 0, 0, 0x86FF68, 0x86FF68, 0, 0, 0x87059C, 0x87059C, 0x86F8FC, 0, 0,
                                                     0x86C970, 0, 0x86C970, 0x86C970, 0x86C9B0, 0, 0x86C9B0, 0x86C9B0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x30, 4, 4, 4, 0xC, 0xC, 0x30, 4, 4, 0xC, 0x30, 4, 4, 0x10, 4, 4, 4, 0x3C, 4, 4, 4, 0x3C, 4};
// Clone: palabras de objeto (a[16..]: dword i en this+8+4*i), indice de la palabra puntero (se sustituye por el puntero a la arena o crudo) e indice de la palabra float (sin NaN senalizador; 99 = ninguna)
inline constexpr std::array<u32, kFunctions> kCloneWords{7, 0, 0, 0, 0, 0, 8, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, 11, 0};
inline constexpr std::array<u32, kFunctions> kClonePtr{6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0};
inline constexpr std::array<u32, kFunctions> kCloneFloat{99, 0, 0, 0, 0, 0, 5, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0};
// Tamano (bytes) que pasa el Clone a CTask::operator new / CEvent::operator new
inline constexpr std::array<u32, kFunctions> kCloneSize{0x64, 0, 0, 0, 0, 0, 0x34, 0, 0, 0, 0x30, 0, 0, 0, 0, 0, 0, 0x34, 0, 0, 0, 0x34, 0};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kConst: a[3] = dword en [this] (no se lee)
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelve el falso del cuerpo (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kEvBody: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite
//   kRetField: a[3] = modo del valor de +8 (0 crudo a[4], 1 objeto de la arena B a[4], 2 objeto de la arena C a[4], 3 el propio this); a[5] = ped (argumento de pila, crudo); a[6] = dword previo en [this]
//   kAbort: a[3] = ubicacion de la subtarea (0 en B a[4], 1 en C a[4], 2 el propio this); a[5] = EAX que devuelve el falso de la subtarea; a[6] = ped; a[7] = prioridad; a[8] = evento (los tres crudos); a[9] = dword previo en [this]
//   kClone: a[3] = modo de la palabra puntero (0 cruda a[4], 1 objeto de la arena C a[4]); a[5] = 0 sin memoria (operator new devuelve nulo) / 1 objeto en B a[6]; a[7] = dword que el constructor falso deja en el objeto
//         (gDtorWrite); a[8] = EAX del constructor falso de evento (gRet; los de tarea devuelven this); a[16..] = palabras del objeto fuente
//   kNode: a[3] = dword en +8 (x = bajos 16 bits, y = altos); a[4] = dword en +0xC (z = bajos 16 bits); a[5] = destino (0 en B a[6], 1 en C a[6], 2 en A solapado: this+8+a[6] con signo, 3 en A en otro sitio a[6])
//   kEmpty: a[3] = dword en [this] (el primer byte decide)
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
        break;
    case kRetField:
        c.a[3] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u});
        c.a[4] = c.a[3] == 0 ? Raw(g) : (c.a[3] == 3 ? 0u : AuxOffset(g, 0x40));
        c.a[5] = Raw(g);
        c.a[6] = prev();
        break;
    case kAbort:
        c.a[3] = g.Pk({0u, 0u, 1u, 1u, 2u});
        c.a[4] = c.a[3] == 2 ? 0u : AuxOffset(g, 0x40);
        c.a[5] = Raw(g);
        c.a[6] = Raw(g);
        c.a[7] = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, g.W(), g.W()});
        c.a[8] = Raw(g);
        c.a[9] = prev();
        break;
    case kClone: {
        c.a[3] = g.Pk({0u, 0u, 1u, 1u, 1u});
        c.a[4] = c.a[3] == 0 ? Raw(g) : AuxOffset(g, 0x40);
        c.a[5] = g.Pk({0u, 1u, 1u, 1u, 1u});
        c.a[6] = AuxOffset(g, 0x68);
        c.a[7] = Raw(g);
        c.a[8] = Raw(g);
        for (u32 i = 0; i < kCloneWords[c.fn]; ++i) c.a[16 + i] = i == kCloneFloat[c.fn] ? FloatBits(g) : Word(g);
        break;
    }
    case kNode: {
        const auto coord = [&] { return g.Pk({0u, 1u, 0xFFFFu, 0x7FFFu, 0x8000u, 0x8001u, 8u, 0xFFF8u, g.W() & 0xFFFFu, g.W() & 0xFFFFu, g.W() & 0xFFFFu, g.W() & 0xFFFFu}); };
        c.a[3] = coord() | (coord() << 16);
        c.a[4] = coord() | (g.P(50) ? 0u : (g.W() & 0xFFFF0000u));
        c.a[5] = g.Pk({0u, 0u, 1u, 1u, 2u, 2u, 2u, 3u});
        if (c.a[5] == 2) c.a[6] = u32(i32(g.R(17)) - 11);   // -11..5: el destino de 12 bytes solapa con los seis bytes de +8..+D
        else c.a[6] = c.a[5] == 3 ? AuxOffset(g, 0x10) : AuxOffset(g, 0x10) + g.R(4);
        break;
    }
    case kEmpty:
        c.a[3] = g.P(45) ? (g.W() & 0xFFFFFF00u) : g.W();
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
    return j.End();
}
}
