#pragma once
// Fixtures del banco Fn95A (lote 95A): nueve metodos de D3DTextureBuffer y D3DIndexDataBuffer (las pilas de recursos D3D9 de D3DResourceSystem), 16384 casos cada uno (147456 pares).
// Solo C++ estandar (sin tipos del juego).
//
//   0 D3DTextureBuffer::Setup                        0x72FE80  thiscall (formato, ancho, niveles, capacidad), `ret 10h`; void                         (hook D3DTextureBuffer / Setup, tipo S)
//   1 D3DTextureBuffer::Pop(format,width,height,levels) 0x72FF60 thiscall, `ret 10h`; EAX = textura o 0                                                (hook D3DTextureBuffer / Pop-Match, tipo S)
//   2 D3DTextureBuffer::Push                         0x72FFF0  thiscall (textura), `ret 4`; AL = guardada                                             (hook D3DTextureBuffer / Push, tipo S)
//   3 D3DTextureBuffer::Resize                       0x730020  thiscall (capacidad), `ret 4`; void                                                    (hook D3DTextureBuffer / Resize, tipo S)
//   4 D3DTextureBuffer::GetTotalDataSize             0x7300A0  thiscall, `ret`; EAX = suma de pixeles (x87 de 24 bits)                                (hook D3DTextureBuffer / GetTotalDataSize, tipo S)
//   5 D3DTextureBuffer::PushWithoutIncreasingCounter 0x730AD0  thiscall (textura), `ret 4`; AL = guardada                                             (hook D3DTextureBuffer / PushWithoutIncreasingCounter, tipo S)
//   6 D3DIndexDataBuffer::Pop(indexCount)            0x730270  thiscall (indices), `ret 4`; EAX = index buffer o 0                                    (hook D3DIndexDataBuffer / Pop-Size, tipo S)
//   7 D3DIndexDataBuffer::Resize                     0x730330  thiscall (capacidad), `ret 4`; void                                                    (hook D3DIndexDataBuffer / Resize, tipo S)
//   8 D3DIndexDataBuffer::GetTotalDataSize           0x7303B0  thiscall, `ret`; EAX = suma de bytes                                                   (hook D3DIndexDataBuffer / GetTotalDataSize, tipo S)
//
// Familia = n % 16 (1024 casos por familia). Los recursos D3D9 son objetos COM FALSOS (vtable propia: Release, GetLevelCount/GetDesc, GetLevelDesc) que registran sus llamadas.
#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

namespace F95Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 9;

constexpr unsigned kSlots = 24;         // entradas de la pila del buffer
constexpr unsigned kObjs = 10;          // objetos COM falsos
constexpr unsigned kGuardWords = 8;     // palabras de guarda a cada lado de la pila
constexpr u32 kGuardWord = 0x6D6D6D6Du; // contenido conocido de las guardas de la pila
constexpr u32 kRestWord = 0x5C5C5C5Cu;  // contenido conocido de las entradas de la pila que el caso no declara
constexpr u32 kBlockWord = 0xB1B1B1B1u; // contenido inicial de la pila nueva (operator new falso)
constexpr unsigned kBlockWords = 64;
constexpr std::size_t kGuard = 0x40;    // bytes de guarda del buffer
constexpr std::size_t kWin = 0x40;      // ventana del buffer (la estructura mide 0x1C o 0x18)

constexpr bool IsTexture(unsigned fn) { return fn <= 5; }
constexpr unsigned kTexWords = 7;       // D3DTextureBuffer: formato, ancho, niveles, capacidad, contador, cantidad, puntero
constexpr unsigned kIdxWords = 6;       // D3DIndexDataBuffer: formato, campo4, capacidad, contador, cantidad, puntero
constexpr unsigned Words(unsigned fn) { return IsTexture(fn) ? kTexWords : kIdxWords; }
constexpr unsigned CapIdx(unsigned fn) { return IsTexture(fn) ? 3 : 2; }
constexpr unsigned CtrIdx(unsigned fn) { return IsTexture(fn) ? 4 : 3; }
constexpr unsigned SizeIdx(unsigned fn) { return IsTexture(fn) ? 5 : 4; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 95001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
// Relleno del buffer por modo: 0 ruido, 1 ceros, 2 unos, 3 0xCD, 4 0xA5, 5 ruido con ceros dispersos
inline void FillMode(u8* dst, std::size_t bytes, u32 seed, u32 mode) {
    switch (mode % 6) {
    case 1: std::memset(dst, 0x00, bytes); break;
    case 2: std::memset(dst, 0xFF, bytes); break;
    case 3: std::memset(dst, 0xCD, bytes); break;
    case 4: std::memset(dst, 0xA5, bytes); break;
    case 5: FillWords(dst, bytes, seed); for (std::size_t i = 0; i < bytes; ++i) if (Rand(seed, u32(i) + 500u) % 3 == 0) dst[i] = 0; break;
    default: FillWords(dst, bytes, seed); break;
    }
}

struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    Js& A(const char* k, const u32* v, unsigned n) { Key(k); s += '['; for (unsigned i = 0; i < n; ++i) { if (i) s += ','; s += std::to_string(v[i]); } s += ']'; return *this; }
    Js& Raw(const char* k, const std::string& v) { Key(k); s += v; return *this; }
};

// Etiqueta de un puntero a un objeto falso (arena 2 del banco, 0x40 bytes por objeto): la misma que calcula Tag() en Core.hpp.
constexpr u32 kObjTagBase = 0x10000000u + (2u << 20);
constexpr u32 ObjTag(unsigned idx) { return kObjTagBase + idx * 0x40u; }
constexpr u32 kNoObj = 0xFFFFFFFFu;

inline constexpr std::array<u32, 7> kFormats{0u, 21u, 22u, 827611204u, 894720068u, 0x32315659u, 0x65u};
inline constexpr std::array<u32, 12> kDims{1u, 2u, 4u, 8u, 16u, 32u, 64u, 128u, 256u, 512u, 1024u, 4096u};
inline constexpr std::array<u32, 8> kLevelCounts{1u, 1u, 1u, 0u, 2u, 3u, 11u, 0xFFFFFFFFu};
inline constexpr std::array<u32, 6> kRaw{0u, 0xCDCDCDCDu, 0xDEADBEEFu, 0xFFFFFFFFu, 0x80000000u, 0x00000001u};
inline constexpr std::array<u32, 10> kCapEdge{0u, 1u, 0x10u, 0x20u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0x40000000u, 0x3FFFFFFFu, 0x18u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }

// El caso. Las palabras de la estructura son las del buffer (formato.. cantidad); el puntero a la pila lo pone el banco.
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 fillA{}, fillSeed{};
    std::array<u32, 6> w{};               // palabras de datos del buffer (sin el puntero)
    std::array<u32, kSlots> stk{};        // entradas de la pila: cada una es un indice de objeto (si obj[i] != kNoObj) o un valor crudo
    std::array<u32, kSlots> stkObj{};
    std::array<u32, kObjs> lc{};          // GetLevelCount de cada objeto
    std::array<std::array<u32, 8>, kObjs> d{};   // descripcion (D3DSURFACE_DESC: formato 0, ..., ancho 6, alto 7; D3DINDEXBUFFER_DESC: formato 0, ..., tamano 4)
    std::array<u32, 4> a{};               // argumentos
    u32 nargs{};
    u32 logSlots{};                       // entradas de la pila declaradas (el resto vale kRestWord)
};
inline u32 SizeOf(const C& c) { return c.w[SizeIdx(c.fn)]; }
inline u32 CapOf(const C& c) { return c.w[CapIdx(c.fn)]; }
// Valor de una entrada de la pila tal como se escribe en memoria (etiqueta del objeto o valor crudo).
inline u32 StackValue(const C& c, unsigned i) { return i < c.logSlots ? (c.stkObj[i] != kNoObj ? ObjTag(c.stkObj[i]) : c.stk[i]) : kRestWord; }

inline C MakeC(unsigned fn, unsigned n) {
    C c;
    c.fn = fn; c.n = n;
    c.family = n % 16;
    c.seed = Rand(n, 7 + fn * 31);
    c.eax = Rand(n, 1); c.ecx = Rand(n, 3); c.edx = Rand(n, 2); c.stack = StackSeed(n);
    c.fillSeed = Rand(n, 11 + fn);
    c.fillA = Rand(n, 12 + fn) % 6;
    const u32 f = c.family;
    u32 kk = 100 + fn * 60;
    auto nk = [&] { return kk++; };
    auto P = [&](u32 pct) { return Rand(n, nk()) % 100 < pct; };
    auto R = [&](u32 m) { return Rand(n, nk()) % m; };
    const bool tex = IsTexture(fn);
    const unsigned capI = CapIdx(fn), ctrI = CtrIdx(fn), sizeI = SizeIdx(fn);
    // palabras del buffer con ruido por defecto (el formato y los demas campos se ajustan despues)
    for (auto& v : c.w) v = Rand(n, nk());
    for (auto& v : c.stkObj) v = kNoObj;

    // ---- objetos COM falsos
    auto Dim = [&]() -> u32 { return P(75) ? Pick(kDims, n, nk()) : (P(70) ? R(300) + 1 : (P(50) ? R(0x10000) : Rand(n, nk()))); };
    for (unsigned o = 0; o < kObjs; ++o) {
        for (auto& v : c.d[o]) v = Rand(n, nk());
        c.lc[o] = P(60) ? 1u : Pick(kLevelCounts, n, nk());
        if (tex) {
            c.d[o][0] = P(80) ? Pick(std::array<u32, 5>{21u, 22u, 827611204u, 894720068u, 0x32315659u}, n, nk()) : Rand(n, nk());
            c.d[o][6] = Dim(); c.d[o][7] = P(60) ? c.d[o][6] : Dim();
        } else {
            c.d[o][4] = P(70) ? (R(40) + 1) * 200u : (P(50) ? R(0x20000) : Rand(n, nk()));
        }
    }
    // duplicados: algunos objetos comparten descripcion (el primero que se encuentra desde el final gana)
    if (P(35)) { const u32 s = R(kObjs); for (unsigned o = 0; o < kObjs; ++o) if (P(40)) { c.d[o] = c.d[s]; c.lc[o] = c.lc[s]; } }

    // ---- la pila
    auto Stack = [&](u32 size) {   // llena las `size` primeras entradas con objetos (con repeticiones) y el resto con crudos u objetos
        const u32 L = std::min<u32>(kSlots, size + 2);
        c.logSlots = L;
        for (unsigned i = 0; i < L; ++i) {
            if (i < size || P(40)) c.stkObj[i] = R(kObjs);
            else c.stk[i] = P(50) ? Pick(kRaw, n, nk()) : Rand(n, nk());
        }
    };
    auto SizeFor = [&](unsigned fam) -> u32 {   // cantidad de entradas por familia
        switch (fam % 8) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3 + R(6);
        case 4: return kSlots;
        case 5: return kSlots - 1 - R(3);
        default: return R(kSlots + 1);
        }
    };
    auto CapAround = [&](u32 size) -> u32 {   // capacidad coherente o no con la cantidad
        switch (R(8)) {
        case 0: return size;
        case 1: return size + 1;
        case 2: return size ? size - 1 : 0;
        case 3: return 16;
        case 4: return Pick(kCapEdge, n, nk());
        case 5: return size + R(20);
        case 6: return R(40);
        default: return 0x10u << R(2);
        }
    };

    switch (fn) {
    case 0: {   // Setup(formato, ancho, niveles, capacidad)
        c.nargs = 4;
        c.a[0] = P(60) ? Pick(kFormats, n, nk()) : Rand(n, nk());
        c.a[1] = f < 8 ? Pick(kDims, n, nk()) : (P(40) ? Rand(n, nk()) : (u32)(-(int)R(300)));
        c.a[2] = P(50) ? Pick(std::array<u32, 4>{0xFFFFFFFFu, 0u, 1u, 2u}, n, nk()) : Rand(n, nk());
        c.a[3] = f < 4 ? 16u : (f < 8 ? Pick(kCapEdge, n, nk()) : (P(50) ? R(40) : Rand(n, nk())));
        c.logSlots = 0;
        break;
    }
    case 2: case 5: {   // Push / PushWithoutIncreasingCounter(textura)
        c.nargs = 1;
        u32 size = SizeFor(f), cap = CapAround(size);
        if (f >= 12) { size = P(50) ? Pick(std::array<u32, 5>{0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x40000000u}, n, nk()) : (u32)(-(int)(1 + R(4))); if (P(50)) cap = size + 1u + R(3); }
        // la escritura (size < cap con signo) solo cae en la pila o en sus guardas
        if ((i32)size < (i32)cap && !((i32)size >= -(i32)kGuardWords && (i32)size < (i32)(kSlots + kGuardWords))) { cap = size; }
        c.w[0] = Pick(kFormats, n, nk()); c.w[capI] = cap; c.w[sizeI] = size; c.w[ctrI] = P(50) ? R(100) : Rand(n, nk());
        Stack(std::min<u32>(size < 0x80000000u ? size : 0u, kSlots));
        c.a[0] = P(50) ? (P(50) ? 0x7E000000u + R(kObjs) * 0x40u : Rand(n, nk())) : Pick(kRaw, n, nk());
        break;
    }
    case 3: case 7: {   // Resize(capacidad)
        c.nargs = 1;
        const u32 size = SizeFor(f);
        const u32 cap = CapAround(size);
        c.w[capI] = cap; c.w[sizeI] = size; c.w[ctrI] = R(100);
        if (tex) { c.w[0] = Pick(kFormats, n, nk()); }
        Stack(size);
        u32 nc;
        switch (R(8)) {
        case 0: nc = cap & 0x7FFFFFFFu; break;                           // igual (sin cambios) cuando cabe
        case 1: nc = 0; break;
        case 2: nc = size ? size - 1 : 1; break;
        case 3: nc = size; break;
        case 4: nc = size + 1 + R(10); break;
        case 5: nc = R(size + 3); break;
        case 6: nc = Pick(std::array<u32, 6>{0x3FFFFFFFu, 0x40000000u, 0x7FFFFFFFu, 0x20000000u, 0x10000000u, 0x40000001u}, n, nk()); break;
        default: nc = cap * 2u; break;
        }
        if (nc >= 0x80000000u) nc &= 0x7FFFFFFFu;
        if (f == 15 && cap < 0x80000000u) nc = cap;                      // sin cambios
        c.a[0] = nc;
        break;
    }
    case 1: {   // D3DTextureBuffer::Pop(formato, ancho, alto, niveles)
        c.nargs = 4;
        const u32 size = SizeFor(f);
        c.w[0] = Pick(kFormats, n, nk()); c.w[capI] = CapAround(size); c.w[sizeI] = size; c.w[ctrI] = R(100);
        Stack(size);
        c.a[0] = P(60) ? Pick(std::array<u32, 5>{21u, 22u, 827611204u, 894720068u, 0x32315659u}, n, nk()) : Rand(n, nk());
        c.a[1] = Dim(); c.a[2] = Dim(); c.a[3] = P(50) ? 1u : 0u;
        if (size && P(70)) {   // apunta a una entrada de la pila
            const u32 j = R(size);
            const u32 o = c.stkObj[j] != kNoObj ? c.stkObj[j] : 0u;
            c.a[0] = c.d[o][0]; c.a[1] = c.d[o][6]; c.a[2] = c.d[o][7]; c.a[3] = c.lc[o] == 1 ? 1u : 0u;
            switch (R(10)) {   // y a veces la estropea en un solo campo
            case 0: c.a[0] += 1; break;
            case 1: c.a[1] += 1; break;
            case 2: c.a[2] += 1; break;
            case 3: c.a[3] ^= 1u; break;
            case 4: c.a[3] = Pick(std::array<u32, 3>{0xFFFFFFFFu, 2u, 0x80000000u}, n, nk()); break;
            default: break;
            }
        }
        break;
    }
    case 4: {   // D3DTextureBuffer::GetTotalDataSize
        c.nargs = 0;
        const bool generic = f < 8;
        u32 size = generic ? (P(10) ? 0u : (P(8) ? (u32)(-(int)(1 + R(3))) : 1 + R(kSlots))) : (P(12) ? 0u : (P(15) ? Pick(std::array<u32, 5>{0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFEu, 0x10000u}, n, nk()) : 1 + R(60)));
        c.w[0] = generic ? 0u : (P(70) ? Pick(std::array<u32, 5>{21u, 22u, 827611204u, 894720068u, 0x32315659u}, n, nk()) : 1u + R(0xFFFFu));
        c.w[capI] = CapAround(size < kSlots ? size : 16); c.w[sizeI] = size; c.w[ctrI] = R(100);
        Stack(size <= kSlots ? size : 1);   // la rama de otro formato solo mira la entrada 0
        if (!generic && c.logSlots == 0) { c.logSlots = 1; c.stkObj[0] = R(kObjs); }
        // dimensiones: realistas o grandes (productos de 32 bits con y sin signo, totales que desbordan)
        for (unsigned o = 0; o < kObjs; ++o) {
            switch (R(6)) {
            case 0: break;
            case 1: c.d[o][6] = 0xFFFFu; c.d[o][7] = 0x8001u; break;
            case 2: c.d[o][6] = Rand(n, nk()); c.d[o][7] = Rand(n, nk()); break;
            case 3: c.d[o][6] = 1u << R(32); c.d[o][7] = 1u << R(12); break;
            case 4: c.d[o][6] = 0x10000u + R(0x8000u); c.d[o][7] = 0x10000u - R(100u); break;
            default: c.d[o][6] = R(0x40000u); c.d[o][7] = R(64u); break;
            }
            if (P(50)) c.lc[o] = 0;   // x87 (GetLevelCount == 0)
        }
        break;
    }
    case 6: {   // D3DIndexDataBuffer::Pop(indices)
        c.nargs = 1;
        const u32 size = SizeFor(f);
        c.w[0] = 0x65u; c.w[1] = R(1700); c.w[capI] = CapAround(size); c.w[sizeI] = size; c.w[ctrI] = R(100);
        Stack(size);
        c.a[0] = P(40) ? R(2200) : (P(50) ? Rand(n, nk()) : Pick(std::array<u32, 5>{0u, 1u, 0x80000000u, 0x3FFFFFFFu, 0x40000000u}, n, nk()));
        if (size && P(70)) {
            const u32 j = R(size);
            const u32 o = c.stkObj[j] != kNoObj ? c.stkObj[j] : 0u;
            const u32 sz = c.d[o][4];
            c.a[0] = (sz >> 1) + (P(60) ? 0u : (P(50) ? 1u : 0xFFFFFFFFu));   // el tamano justo, uno mas o uno menos
        }
        break;
    }
    default: {   // 8: D3DIndexDataBuffer::GetTotalDataSize
        c.nargs = 0;
        const bool generic = f < 8;
        const u32 size = generic ? (P(10) ? 0u : (P(8) ? (u32)(-(int)(1 + R(3))) : 1 + R(kSlots))) : (P(12) ? 0u : (P(15) ? Pick(std::array<u32, 5>{0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFEu, 0x10000u}, n, nk()) : 1 + R(60)));
        c.w[0] = generic ? 0u : (P(70) ? 0x65u : 1u + R(0xFFFFu)); c.w[1] = R(1700);
        c.w[capI] = CapAround(size < kSlots ? size : 16); c.w[sizeI] = size; c.w[ctrI] = R(100);
        Stack(size <= kSlots ? size : 1);
        if (!generic && c.logSlots == 0) { c.logSlots = 1; c.stkObj[0] = R(kObjs); }
        for (unsigned o = 0; o < kObjs; ++o) {
            switch (R(5)) {
            case 0: break;
            case 1: c.d[o][4] = Rand(n, nk()); break;
            case 2: c.d[o][4] = 0x80000000u + R(0x100u); break;
            case 3: c.d[o][4] = 0xFFFFFF00u + R(0x100u); break;
            default: c.d[o][4] = (R(40) + 1) * 200u; break;
            }
        }
        break;
    }
    }
    if (fn == 0 || fn == 2 || fn == 5) {   // sin objetos en la pila: Setup y Push no leen la pila
        if (fn == 0) c.logSlots = 0;
    }
    return c;
}

// JSON de la entrada: los argumentos, las palabras del buffer (el puntero a la pila va como etiqueta de la arena 1), la pila declarada y los objetos.
inline std::string JsonC(const C& c) {
    Js j;
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("fill", c.fillA);
    j.A("args", c.a.data(), c.nargs);
    std::array<u32, kTexWords> buf{};
    const unsigned nw = Words(c.fn);
    for (unsigned i = 0; i + 1 < nw; ++i) buf[i] = c.w[i];
    buf[nw - 1] = 0x10000000u + (1u << 20) + kGuardWords * 4u;   // puntero a la pila: arena 1, tras las guardas
    j.A("buf", buf.data(), nw);
    if (c.fn != 0) {
        std::array<u32, kSlots> stk{};
        for (unsigned i = 0; i < c.logSlots; ++i) stk[i] = StackValue(c, i);
        j.A("stk", stk.data(), c.logSlots);
    }
    const bool objs = c.fn == 1 || c.fn == 3 || c.fn == 4 || c.fn == 6 || c.fn == 7 || c.fn == 8;
    if (objs) {
        std::string s = "[";
        for (unsigned o = 0; o < kObjs; ++o) {
            if (o) s += ',';
            if (IsTexture(c.fn)) s += "[" + std::to_string(c.lc[o]) + "," + std::to_string(c.d[o][0]) + "," + std::to_string(c.d[o][6]) + "," + std::to_string(c.d[o][7]) + "]";
            else s += "[" + std::to_string(c.d[o][4]) + "]";
        }
        j.Raw("obj", s + "]");
    }
    return j.s + "}";
}
}
