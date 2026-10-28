#pragma once
// Fixtures del banco MatrixOps76 (lote 76): CMatrix::Attach (EXE 0x59BD10, thiscall, ECX = this, 2 argumentos en pila (RwMatrix*, bool como dword), `ret 8`, void) y
// CMatrix::operator= (EXE 0x59BBC0, thiscall, ECX = this, 1 argumento en pila (const CMatrix*), `ret 4`, el EXE devuelve this en EAX).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CMatrix::Attach       0x59BD10  (hook CMatrix / Attach, tipo S)
//   1 CMatrix::operator=    0x59BBC0  (hook CMatrix / operator=, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto por funcion: n < 16384: familia = n % 16 (1024 casos por familia); despues las familias 16 y 17 (1024 cada una).
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

namespace Mx76Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;   // familias 0..15 (n % 16), 1024 casos cada una
constexpr unsigned kFamX1 = 1024;        // familia 16
constexpr unsigned kFamX2 = 1024;        // familia 17
constexpr unsigned kCases = kBaseCases + kFamX1 + kFamX2;
constexpr unsigned kFamilies = 18;
constexpr unsigned kFunctions = 2;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). CMatrix = 0x48 bytes (18 dwords), RwMatrix = 0x40 bytes (16 dwords).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kMatWin = 0x48, kRwWin = 0x40;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kMatArena = Arena(kMatWin), kRwArena = Arena(kRwWin);

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 76176u); }
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
    Js& A(const char* k, const u32* v, std::size_t n) {
        Key(k); s += '[';
        for (std::size_t i = 0; i < n; ++i) { if (i) s += ','; s += std::to_string(v[i]); }
        s += ']';
        return *this;
    }
    std::string End() const { return s + "}"; }
};

// NaN con signo, carga util y tipo (QNaN/SNaN) arbitrarios; `kind`: 0 QNaN, 1 SNaN, otro = aleatorio.
inline u32 NanBits(u32 c, u32 k, u32 kind) {
    const u32 r = Rand(c, k), s = Rand(c, k + 1);
    u32 payload;
    switch (s % 6) {
    case 0: payload = 0; break;
    case 1: payload = 1 + (r % 0xFF); break;
    case 2: payload = 0x3FFFFFu; break;
    case 3: payload = 0x200000u | (r & 0xFFu); break;
    case 4: payload = 0x1FFFFFu; break;
    default: payload = (r >> 3) & 0x3FFFFFu; break;
    }
    const bool quiet = kind == 0 ? true : kind == 1 ? false : ((r >> 30) & 1) != 0;
    if (!quiet && payload == 0) payload = 1;   // un sNaN exige carga util distinta de cero
    return ((r >> 31) << 31) | 0x7F800000u | (quiet ? 0x400000u : 0u) | payload;
}
// Patron de 32 bits "interesante" para un float: especiales, NaN de todos los tipos, desnormales, +-0, +-inf, extremos y aleatorios.
inline u32 Special(u32 c, u32 k) {
    static constexpr u32 sp[]{0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x7F7FFFFFu, 0xFF7FFFFFu,
                              0x00800000u, 0x80800000u, 0x7FC00000u, 0xFFC00000u, 0x7FA00000u, 0xFFA00000u, 0x7F800001u, 0xFF800001u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x40490FDBu, 0x3F000000u};
    const u32 r = Rand(c, k) % 10;
    if (r < 4) return sp[Rand(c, k + 1) % std::size(sp)];
    if (r == 4) return NanBits(c, k + 2, 1);
    if (r == 5) return NanBits(c, k + 3, 0);
    if (r == 6) return NanBits(c, k + 4, 2);
    if (r == 7) return B(float(i32(Rand(c, k + 5) % 2001) - 1000) / 8.0f);
    return Rand(c, k + 6);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Valores del dword de propiedad (+0x44): el EXE lo LEE entero y lo ESCRIBE entero (movzx del byte del argumento)
inline u32 OwnsWord(u32 c, u32 k) {
    static constexpr u32 sp[]{0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0x200u, 0x10000u, 0x01000000u, 0x80000000u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x00010001u, 0xAA000000u, 0x01010101u};
    return Rand(c, k) % 4 == 0 ? Rand(c, k + 1) : sp[Rand(c, k + 2) % std::size(sp)];
}
// Punteros "sueltos" que nunca caen dentro de una arena (el destructor de RenderWare es un falso: solo se registra su valor)
inline u32 JunkPtr(u32 c, u32 k) {
    static constexpr u32 sp[]{1u, 2u, 0x10u, 0x1000u, 0xFFFFFFFFu, 0xFFFFFFF0u, 0xC0000001u, 0xDEADC0DEu, 0xFFFF0000u, 0x80000000u, 0xC0DEC0DEu, 0x7FFFFFFFu};
    return sp[Rand(c, k) % std::size(sp)];
}

// ------------------------------------------------------------------------------------------------ el caso
// Punteros por clase (la arena concreta la resuelve LiveComparison):
//   Attach:  oldKind = valor previo de +0x40: 0 nulo, 1 arena rwOld, 2 suelto (oldJunk), 3 arena rwNew (igual que el nuevo), 4 la propia arena de `this`.
//            newKind = RwMatrix* del argumento: 0 arena rwNew, 1 la propia arena de `this` (alias), 2 arena rwOld.
//   operator=: dstKind = valor de +0x40 de `this`: 0 nulo, 1 arena rwOld, 2 arena `other` (alias con el origen), 3 la propia arena de `this`, 4 arena rwNew.
//              srcKind = 0 arena `other`, 1 la propia `this` (autoasignacion).
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 thisW[18]{}, otherW[18]{}, rwNewW[16]{}, rwOldW[16]{};
    u32 oldKind{}, oldJunk{}, owns{}, newKind{}, boolArg{};
    u32 dstKind{}, srcKind{};
};

inline C Defaults(unsigned n) {
    C c;
    c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    for (int i = 0; i < 18; ++i) { c.thisW[i] = Rand(n, 100 + i); c.otherW[i] = Rand(n, 130 + i); }
    for (int i = 0; i < 16; ++i) { c.rwNewW[i] = Rand(n, 160 + i); c.rwOldW[i] = Rand(n, 190 + i); }
    c.oldKind = 0; c.oldJunk = JunkPtr(n, 5); c.owns = Rand(n, 6) % 2; c.newKind = 0; c.boolArg = Rand(n, 8) % 2;
    c.dstKind = 0; c.srcKind = 0;
    return c;
}
inline void SpecialAll(C& c, u32 n, u32 k, bool newM, bool other, bool old) {
    for (int i = 0; i < 16; ++i) {
        if (newM) c.rwNewW[i] = Special(n, k + i);
        if (old) c.rwOldW[i] = Special(n, k + 20 + i);
    }
    if (other) for (int i = 0; i < 16; ++i) c.otherW[i] = Special(n, k + 40 + i);
}
// Argumento bool: el byte bajo es el valor, el resto es basura de la pila
inline u32 BoolArg(u32 n, u32 k) {
    const u32 low = Rand(n, k) % 3 == 0 ? Rand(n, k + 1) & 0xFFu : Rand(n, k + 1) % 2;
    return Rand(n, k + 2) % 2 ? low : (Rand(n, k + 3) & 0xFFFFFF00u) | low;
}

inline C MakeAttach(unsigned n) {
    C c = Defaults(n);
    if (n >= kBaseCases) c.family = 16 + (n - kBaseCases) / kFamX1;
    switch (c.family) {
    case 0:   // sin matriz previa
        c.oldKind = 0; c.owns = Rand(n, 20) % 2;
        break;
    case 1:   // matriz previa propia (se destruye)
        c.oldKind = 1; c.owns = 1; c.boolArg = Rand(n, 21) % 2;
        break;
    case 2:   // matriz previa ajena (no se destruye)
        c.oldKind = Rand(n, 22) % 2 ? 1u : 2u; c.owns = 0; c.boolArg = Rand(n, 23) % 2;
        break;
    case 3:   // propiedad sin matriz
        c.oldKind = 0; c.owns = OwnsWord(n, 24);
        break;
    case 4:   // dword de propiedad con el byte bajo a cero y basura en los tres de relleno: el EXE lo lee entero
        c.oldKind = Rand(n, 25) % 3 == 0 ? 2u : 1u; c.owns = (Rand(n, 26) | 0x100u) & 0xFFFFFF00u;
        break;
    case 5:   // byte bajo distinto de cero con basura arriba
        c.oldKind = Rand(n, 27) % 3 == 0 ? 2u : 1u; c.owns = (Rand(n, 28) & 0xFFFFFF00u) | (1u + Rand(n, 29) % 255);
        break;
    case 6:   // el argumento bool con todos los valores del byte y basura arriba
        c.oldKind = Rand(n, 30) % 2; c.owns = Rand(n, 31) % 2; c.boolArg = BoolArg(n, 32);
        break;
    case 7:   // la RwMatrix nueva con especiales en todos los dwords (se copian 12, 4 no)
        SpecialAll(c, n, 40, true, false, false);
        c.oldKind = Rand(n, 33) % 2; c.owns = OwnsWord(n, 34); c.boolArg = BoolArg(n, 35);
        break;
    case 8:   // solo los dwords que NO se copian (banderas y relleno) con especiales; el resto normal
        for (int i : {3, 7, 11, 15}) c.rwNewW[i] = Special(n, 41 + i);
        c.oldKind = Rand(n, 36) % 2; c.owns = Rand(n, 37) % 2;
        break;
    case 9:   // punteros previos sueltos
        c.oldKind = 2; c.oldJunk = JunkPtr(n, 38); c.owns = OwnsWord(n, 39);
        break;
    case 10:  // la misma RwMatrix que la previa
        c.oldKind = 3; c.owns = OwnsWord(n, 42); c.boolArg = BoolArg(n, 43);
        break;
    case 11:  // la RwMatrix nueva es la propia CMatrix (alias)
        c.newKind = 1; c.oldKind = Rand(n, 44) % 5; c.oldJunk = JunkPtr(n, 45); c.owns = OwnsWord(n, 46); c.boolArg = BoolArg(n, 47);
        if (Rand(n, 48) % 2) for (int i = 0; i < 16; ++i) c.thisW[i] = Special(n, 50 + i);
        break;
    case 12:  // matrices con valores neutros: ceros, unos, signos
        for (int i = 0; i < 16; ++i) c.rwNewW[i] = Rand(n, 400 + i) % 3 == 0 ? 0x80000000u : Rand(n, 400 + i) % 3 == 1 ? 0u : 0x3F800000u;
        for (int i = 0; i < 16; ++i) if (Rand(n, 70 + i) % 4 == 0) c.rwNewW[i] ^= 0x80000000u;
        c.oldKind = Rand(n, 71) % 2; c.owns = Rand(n, 72) % 2;
        break;
    case 13:  // el destino anterior es la RwMatrix de otro tipo (newKind = rwOld)
        c.newKind = 2; c.oldKind = Rand(n, 73) % 4; c.oldJunk = JunkPtr(n, 74); c.owns = OwnsWord(n, 75); c.boolArg = BoolArg(n, 76);
        break;
    case 14:  // la CMatrix inicial con especiales (los dwords copiados se sobrescriben; los demas permanecen)
        for (int i = 0; i < 16; ++i) c.thisW[i] = Special(n, 80 + i);
        c.oldKind = Rand(n, 77) % 3; c.owns = OwnsWord(n, 78);
        break;
    default:  // mezcla (15 y 16/17)
        c.oldKind = Rand(n, 79) % 5; c.oldJunk = JunkPtr(n, 150); c.owns = OwnsWord(n, 151); c.newKind = Rand(n, 152) % 3; c.boolArg = BoolArg(n, 153);
        if (Rand(n, 154) % 2) SpecialAll(c, n, 90, true, false, false);
        if (Rand(n, 155) % 2) for (int i = 0; i < 16; ++i) c.thisW[i] = Special(n, 110 + i);
        if (c.family == 16) { c.oldKind = 1 + Rand(n, 156) % 2; c.owns = OwnsWord(n, 157) | (Rand(n, 158) % 2 ? 0x100u : 1u); }
        if (c.family == 17) { c.boolArg = BoolArg(n, 159); c.owns = Rand(n, 160) % 2; c.oldKind = 1; }
        break;
    }
    return c;
}

inline C MakeAssign(unsigned n) {
    C c = Defaults(n);
    if (n >= kBaseCases) c.family = 16 + (n - kBaseCases) / kFamX1;
    switch (c.family) {
    case 0:   // sin RwMatrix asociada
        c.dstKind = 0;
        break;
    case 1:   // con RwMatrix asociada propia
        c.dstKind = 1;
        break;
    case 2:   // origen con especiales (se copian 16 dwords enteros: tambien banderas y relleno)
        SpecialAll(c, n, 40, false, true, false);
        c.dstKind = Rand(n, 20) % 2;
        break;
    case 3:   // autoasignacion con RwMatrix
        c.srcKind = 1; c.dstKind = 1;
        break;
    case 4:   // autoasignacion sin RwMatrix
        c.srcKind = 1; c.dstKind = 0;
        break;
    case 5:   // la RwMatrix asociada es la propia `other`
        c.dstKind = 2;
        if (Rand(n, 21) % 2) SpecialAll(c, n, 60, false, true, false);
        break;
    case 6:   // la RwMatrix asociada es la propia `this`
        c.dstKind = 3;
        if (Rand(n, 22) % 2) SpecialAll(c, n, 80, false, true, false);
        break;
    case 7:   // +0x40/+0x44 del origen son basura: no se copian
        c.dstKind = Rand(n, 23) % 2; c.otherW[16] = Special(n, 24); c.otherW[17] = Special(n, 25);
        break;
    case 8:   // el destino tiene RwMatrix con contenido especial previo (se sobrescribe)
        SpecialAll(c, n, 100, false, false, true);
        c.dstKind = 1;
        break;
    case 9:   // RwMatrix asociada con origen especial
        SpecialAll(c, n, 120, false, true, true);
        c.dstKind = 1;
        break;
    case 10:  // RwMatrix asociada = rwNew
        c.dstKind = 4; SpecialAll(c, n, 140, true, Rand(n, 26) % 2, false);
        break;
    case 11:  // todos ceros / unos / signos
        for (int i = 0; i < 18; ++i) c.otherW[i] = Rand(n, 420 + i) % 3 == 0 ? 0x80000000u : Rand(n, 420 + i) % 3 == 1 ? 0u : 0x3F800000u;
        for (int i = 0; i < 16; ++i) if (Rand(n, 200 + i) % 4 == 0) c.otherW[i] ^= 0x80000000u;
        c.dstKind = Rand(n, 28) % 2;
        break;
    case 12:  // destino con especiales previos en `this`, origen normal
        for (int i = 0; i < 18; ++i) c.thisW[i] = Special(n, 220 + i);
        c.dstKind = Rand(n, 29) % 2 ? 1u : 0u;
        break;
    case 13:  // autoasignacion con valores especiales
        c.srcKind = 1; c.dstKind = Rand(n, 30) % 2;
        for (int i = 0; i < 16; ++i) c.thisW[i] = Special(n, 240 + i);
        break;
    case 14:  // la propia `this` es la RwMatrix asociada y es la fuente
        c.srcKind = 1; c.dstKind = 3;
        for (int i = 0; i < 16; ++i) c.thisW[i] = Special(n, 260 + i);
        break;
    default:  // 15, 16, 17: mezcla
        c.dstKind = Rand(n, 31) % 5; c.srcKind = Rand(n, 32) % 4 == 0;
        if (Rand(n, 33) % 2) SpecialAll(c, n, 280, true, true, true);
        if (Rand(n, 34) % 2) for (int i = 0; i < 18; ++i) c.thisW[i] = Special(n, 300 + i);
        if (c.family == 16) { c.dstKind = 1 + Rand(n, 35) % 4; c.srcKind = 0; }
        if (c.family == 17) { c.srcKind = 1; c.dstKind = Rand(n, 36) % 5; }
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack);
    j.A("this_w", c.thisW, 18).A("other_w", c.otherW, 18).A("rw_new_w", c.rwNewW, 16).A("rw_old_w", c.rwOldW, 16);
    j.U("old_kind", c.oldKind).U("old_junk", c.oldJunk).U("owns", c.owns).U("new_kind", c.newKind).U("bool_arg", c.boolArg).U("dst_kind", c.dstKind).U("src_kind", c.srcKind);
    return j.End();
}
}
