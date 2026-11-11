#pragma once
// Fixtures del banco Fn159A (lote 159A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 23 asignadas: las 23 reconstruidas.
//   idx  direccion  nombre real                                                                       forma
//    0   0x64BEE0   CTaskSimpleCarJumpOut::ScalarDeletingDestructor (vtable 0x86EE70)                 thiscall(flags) `ret 4`: cuerpo 647490 (con hook), CTask::operator delete 61A5B0 si bit 0
//    1   0x64BF50   CTaskSimpleCarForcePedOut::ScalarDeletingDestructor (vtable 0x86EE94)             idem, cuerpo 647790 (con hook)
//    2   0x64BF70   CTaskSimpleCarSetPedOut::ScalarDeletingDestructor (vtable 0x86EEB8)               idem, cuerpo 647950 (con hook)
//    3   0x64BF90   CTaskSimpleCarSlowDragPedOut::ScalarDeletingDestructor (vtable 0x86EEE8)          idem, cuerpo 648070 (con hook)
//    4   0x64D290   CTaskComplexGoToCarDoorAndStandStill::ScalarDeletingDestructor (vtable 0x86EC4C)  idem, cuerpo 64A580 (con hook)
//    5   0x6375B0   CTaskSimpleDieInCar::ScalarDeletingDestructor (vtable 0x86DDE0)                   idem, cuerpo 62FC70 (con hook)
//    6   0x6375D0   CTaskComplexDieInCar::ScalarDeletingDestructor (vtable 0x86DE04)                  idem, cuerpo 62FCF0 (SIN hook: falso en el EXE y en la envoltura de la DLL)
//    7   0x67CA00   CTaskComplexGetUpAndStandStill::ScalarDeletingDestructor (vtable 0x870404)        idem, cuerpo 678160 (con hook)
//    8   0x67CA20   CTaskSimpleFall::ScalarDeletingDestructor (vtable 0x870430)                       idem, cuerpo 678310 (con hook)
//    9   0x67D550   CTaskComplexFallToDeath::ScalarDeletingDestructor (vtable 0x870520)               idem, cuerpo 6790B0 (con hook)
//   10   0x67D570   CTaskSimpleJump::ScalarDeletingDestructor (vtable 0x87054C)                       idem, cuerpo 679AF0 (con hook)
//   11   0x62FA50   CTaskSimpleDie::GetTaskType (vtable 0x86DDBC, ranura 4)                           thiscall() `ret`: mov eax,0D4h
//   12   0x677F70   CTaskSimpleGetUp::GetTaskType (vtable 0x8703E0, ranura 4)                         thiscall() `ret`: mov eax,0CDh
//   13   0x678ED0   CTaskSimpleLand::GetTaskType (vtable 0x8704FC, ranura 4)                          thiscall() `ret`: mov eax,0F2h
//   14   0x62FB40   CTaskSimpleDie cuerpo del destructor (marco SEH)                                  thiscall() `ret`: vtable 86DDBCh, SetFinishCallback 4CEBE0 si hay animacion (+24h), ~CTask 61A3A0
//   15   0x677F80   CTaskSimpleGetUp cuerpo del destructor (marco SEH)                                idem: vtable 8703E0h, animacion en +0Ch
//   16   0x678EE0   CTaskSimpleLand cuerpo del destructor (marco SEH)                                 idem: vtable 8704FCh, animacion en +08h
//   17   0x537550   CBoundingBox constructor (alias 1; el listado decia CFileLoader)                  thiscall() `ret`: call 40FAF0; eax = this
//   18   0x537560   CBoundingBox constructor (alias 2)                                                idem
//   19   0x537570   CBoundingBox constructor (alias 3)                                                idem
//   20   0x537A10   CAnimatedBuilding::Constructor                                                     thiscall() `ret`: call 403E00 (CBuilding, SIN hook: real en el EXE; en la DLL se pliega: CEntity 532A90, +36h, +1Ch), [this] = 8639B0h; eax = this
//   21   0x537290   CloneAtomicToClumpCB                                                               cdecl(atomic, clump) `ret`: RpAtomicClone 749E60, RpAtomicSetFrame 74BF20(clone, frame->root), RpClumpAddAtomic 74A490(clump, clone); eax = atomic
//   22   0x537510   CCollisionData::GetNoOfShdwVerts                                                   thiscall() `ret`: [+20h] triangulos de sombra (con signo), [+2Ch] puntero; mayor indice de vertice + 1
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

namespace F159AFix {
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
    std::vector<u8> s;          // datos en bytes (triangulos de sombra de GetNoOfShdwVerts)
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
enum Kind : u32 { kSdd, kConst, kAnimBody, kBBox, kBuildCtor, kCloneAtomic, kShadow };
inline constexpr std::array<Kind, kFunctions> kKind{
    kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kConst, kConst, kConst, kAnimBody, kAnimBody, kAnimBody, kBBox, kBBox, kBBox, kBuildCtor, kCloneAtomic, kShadow};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x64BEE0, 0x64BF50, 0x64BF70, 0x64BF90, 0x64D290, 0x6375B0, 0x6375D0, 0x67CA00, 0x67CA20, 0x67D550, 0x67D570, 0x62FA50, 0x677F70, 0x678ED0,
                                                    0x62FB40, 0x677F80, 0x678EE0, 0x537550, 0x537560, 0x537570, 0x537A10, 0x537290, 0x537510};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor, ~CTask, constructor de CBoundingBox o de CEntity; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x647490, 0x647790, 0x647950, 0x648070, 0x64A580, 0x62FC70, 0x62FCF0, 0x678160, 0x678310, 0x6790B0, 0x679AF0, 0, 0, 0,
                                                      0x61A3A0, 0x61A3A0, 0x61A3A0, 0x40FAF0, 0x40FAF0, 0x40FAF0, 0x532A90, 0, 0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86EE70, 0x86EE94, 0x86EEB8, 0x86EEE8, 0x86EC4C, 0x86DDE0, 0x86DE04, 0x870404, 0x870430, 0x870520, 0x87054C, 0x86DDBC, 0x8703E0, 0x8704FC,
                                                     0x86DDBC, 0x8703E0, 0x8704FC, 0, 0, 0, 0x8585C8, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x2C, 0x2C, 0x2C, 0x18, 0x18, 0x18, 0x38, 0xA8, 0x30};
// Triangulos de sombra: maximo de entradas
constexpr u32 kMaxTris = 400;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kConst: a[3] = dword en [this] (no se lee)
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kAnimBody: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite; a[6] = modo de la animacion (0 nula, 1 en B a[7], 2 cruda a[8]); a[7] = desplazamiento en B; a[8] = crudo
//   kBBox: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite
//   kBuildCtor: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite; a[6] = dword previo en +1Ch; a[7] = dword previo en +34h (el byte +36h es su tercer byte)
//   kCloneAtomic: a[3] = modo del marco (0 en B, 1 en C, 2 el propio atomic, 3 en A en otro sitio); a[4] = desplazamiento del marco; a[5] = modo de la raiz (+A0h del marco) (0 cruda a[6], 1 en B a[6], 2 en C a[6], 3 cero);
//         a[6] = crudo o desplazamiento; a[7] = modo del clon (0 en C a[8], 1 crudo a[8], 2 cero); a[8] = desplazamiento o crudo; a[9] = clump (crudo); a[10] = gRet
//   kShadow: a[3] = modo (0 contador cero, 1 contador negativo, 2 contador positivo con los triangulos en B, 3 idem en C); a[4] = contador; a[5] = puntero crudo (modos 0 y 1); a[6] = desplazamiento de los triangulos;
//         c.s = los bytes de los triangulos (contador * 8)
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
    case kAnimBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = AuxOffset(g, 0x40);
        c.a[8] = Raw(g);
        break;
    case kBBox:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kBuildCtor:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 0xFFFFFFFEu, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0x100u, g.W(), g.W(), g.W() & ~1u, g.W() | 1u});
        c.a[7] = g.Pk({0u, 0x00060000u, 0x00F90000u, 0x00FF0000u, 0x00010000u, 0x00080000u, 0xFFFFFFFFu, 0xFF00FFFFu, g.W(), g.W(), g.W(), g.W() & 0xFF06FFFFu, g.W() | 0x00F90000u});
        break;
    case kCloneAtomic:
        c.a[3] = g.Pk({0u, 0u, 1u, 2u, 3u});
        c.a[4] = c.a[3] == 2 ? 0u : AuxOffset(g, 0xA8);
        c.a[5] = g.Pk({0u, 0u, 1u, 2u, 3u});
        c.a[6] = c.a[5] == 0 ? Raw(g) : AuxOffset(g, 0x40);
        c.a[7] = g.Pk({0u, 0u, 1u, 2u});
        c.a[8] = c.a[7] == 0 ? AuxOffset(g, 0x40) : Raw(g);
        c.a[9] = Raw(g);
        c.a[10] = Raw(g);
        break;
    case kShadow: {
        const u32 mode = g.Pk({0u, 1u, 2u, 2u, 2u, 2u, 2u, 3u, 3u});
        c.a[3] = mode;
        c.a[5] = Raw(g);
        if (mode == 0) {
            c.a[4] = 0;
        } else if (mode == 1) {
            c.a[4] = g.Pk({0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFF0u, 0xFFFF0000u, g.W() | 0x80000000u, g.W() | 0x80000000u});
        } else {
            u32 count = g.Pk({1u, 1u, 2u, 3u, 4u, 5u, 8u, 16u, 17u, 31u, 32u, 64u, 100u, 255u, 256u, 300u, kMaxTris, 1u + g.R(kMaxTris), 1u + g.R(kMaxTris), 1u + g.R(20), 1u + g.R(8)});
            if (count > kMaxTris) count = kMaxTris;
            c.a[4] = count;
            // desplazamiento de los triangulos en la arena auxiliar (la lectura de 16 bits admite cualquier alineacion)
            c.a[6] = AuxOffset(g, 8 * count + 4) + g.R(4);
            // indices: 0 aleatorios, 1 pequenos, 2 maximo en el primer triangulo, 3 maximo en el ultimo, 4 todos cero, 5 todos 0xFFFF, 6 maximo en la tercera posicion del triangulo central, 7 maximo repetido
            const u32 pat = g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u});
            c.s.assign(std::size_t(8) * count, 0);
            for (u32 t = 0; t < count; ++t) {
                u16 idx[3];
                for (int k = 0; k < 3; ++k) {
                    switch (pat) {
                    case 0: idx[k] = u16(g.W()); break;
                    case 1: idx[k] = u16(g.R(100)); break;
                    case 4: idx[k] = 0; break;
                    case 5: idx[k] = 0xFFFF; break;
                    default: idx[k] = u16(g.R(1000)); break;
                    }
                }
                if (pat == 2 && t == 0) idx[g.R(3)] = u16(2000 + g.R(60000));
                if (pat == 3 && t == count - 1) idx[g.R(3)] = u16(2000 + g.R(60000));
                if (pat == 6 && t == count / 2) idx[2] = u16(2000 + g.R(60000));
                if (pat == 7 && t % 3 == 0) idx[1] = 1500;
                std::memcpy(c.s.data() + 8 * t, idx, 6);
                c.s[8 * t + 6] = u8(g.W());
                c.s[8 * t + 7] = u8(g.W());
            }
        }
        if (mode < 2) c.a[6] = 0;
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
