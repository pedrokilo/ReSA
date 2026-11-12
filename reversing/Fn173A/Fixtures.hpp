#pragma once
// Fixtures del banco Fn173A (lote 173A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado y de los llamadores, ver README.md y fixture_oracle.py). 21 reconstruidas (de 28 asignadas: 7 direcciones saltadas, ver README.md).
//   idx  direccion  nombre real                                                                         forma
//    0   0x699E20   CPedList::FillUpHoles                                                               thiscall() `ret`: 30 punteros desde +4: copia los no nulos al principio y rellena el resto con ceros (no toca +0)
//    1   0x699F00   CPointList::Clear (el listado decia CPedList)                                       thiscall() `ret`: [this]=0; seis dwords desde +0x124 a cero
//    2   0x732D60   CLinkList<AlphaObjectInfo>::Shutdown                                                thiscall() `ret`: delete[] ([this+0x50]) (CRT 0x8213AE); [this+0x50]=0
//    3   0x732D80   CLinkList<CPed*>::Shutdown                                                          thiscall() `ret`: delete[] ([this+0x30]) (CRT 0x8213AE); [this+0x30]=0
//    4   0x733910   CLinkList<AlphaObjectInfo>::InsertSorted                                             thiscall(dato) `ret 4`: recorre la lista usada con la comparacion x87 de la distancia (+8); copia el dato; mueve el enlace libre
//    5   0x733F40   CLinkList<AlphaObjectInfo>::Clear                                                   thiscall() `ret`: pasa todos los enlaces usados a la cabeza de la lista libre
//    6   0x734610   CreateDebugFont                                                                     cdecl() `ret` (FINAL 0)
//    7   0x734620   DestroyDebugFont                                                                    cdecl() `ret`
//    8   0x734640   FlushObrsPrintfs                                                                    cdecl() `ret`
//    9   0x4C4910   CDamageAtomicModelInfo::CreateInstance(RwMatrix*) (fragmentos 0x4C4910, 0x4C4923)  thiscall(matriz) `ret 4`: selector 0xA9B0B0; clona la plantilla danada (+0x20); frame; copia 64 bytes; asigna; o salta a 0x4C44D0
//   10   0x4C4960   CDamageAtomicModelInfo::CreateInstance()                                            thiscall() `ret`: igual sin matriz; sin selector inline: AddRef 4C4BA0, clon de +0x1C, frame, RemoveRef 4C4BB0
//   11   0x4C55D0   CDamageAtomicModelInfo cuerpo del destructor                                         thiscall() `ret`: [this]=85BB9Ch
//   12   0x4C55E0   CLodAtomicModelInfo cuerpo del constructor (el listado decia CDamageAtomicModelInfo) thiscall() `ret`: 4C4A60; [+20h]=[+22h]=0 (words); [this]=85BC70h; eax=this
//   13   0x6E76E0   CWaterLevel::SyncWater                                                              cdecl() `ret`: [0xC228A4] = [0xB7CB84]
//   14   0x552380   CPtrNodeSingleLink operator new                                                      cdecl(tamano) `ret`: ecx=[0xB74484]; jmp 552240
//   15   0x552390   CPtrNodeSingleLink operator delete                                                   cdecl(nodo) `ret`: libera el slot en el pool simple (indice = diferencia >> 3)
//   16   0x5523C0   CPtrNodeDoubleLink operator new                                                      cdecl(tamano) `ret`: ecx=[0xB74488]; jmp 5522E0
//   17   0x5523D0   CPtrNodeDoubleLink operator delete                                                   cdecl(nodo) `ret`: libera el slot en el pool doble (indice = diferencia / 12)
//   18   0x4CFA30   CAnimBlendSequence::CompressQuaternion (el listado decia CAnimBlendHierarchy)        cdecl(int16[4], float[4]) `ret`: x4096, _ftol, word
//   19   0x4CFA90   CAnimBlendSequence::DecompressQuaternion                                             cdecl(float[4], int16[4]) `ret`: movsx, x1/4096
//   20   0x4CFB10   CAnimBlendSequence::DecompressTranslation                                            cdecl(float[3], int16[3]) `ret`: movsx, x1/1024
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F173AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 21;
constexpr unsigned kArenas = 4;

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
    std::array<u32, 40> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
// Un float para la compresion (x4096 y truncado): ademas de los valores de FloatBits, multiplos de 1/4096 con restos proximos a la unidad, bordes de int16 (7, 8, 9: x4096 = 28672, 32768, 36864) y de los enteros de 32 y 64 bits
inline u32 CompressBits(Gen& g) {
    if (g.P(40)) return FloatBits(g);
    const i32 k = i32(g.R(90000)) - 45000;
    const u32 fr = g.R(6);
    const float frac = fr < 2 ? 0.0f : (fr == 2 ? 0.5f : (fr == 3 ? 0.999f : (fr == 4 ? -0.001f : -0.5f)));
    float v = (float(k) + frac) / 4096.0f;
    const u32 pick = g.R(10);
    if (pick == 0) v = 8.0f + float(g.R(3)) - 1.0f;
    u32 bits = std::bit_cast<u32>(v);
    if (pick == 1) bits = 0x5F000000u + g.R(3) - 1u;   // alrededor de 2^63 / 4096 * 2^12 (borde del entero de 64 bits tras multiplicar)
    if (pick == 2) bits = 0x4F000000u + g.R(3) - 1u;   // alrededor de 2^31 (borde del entero de 32 bits)
    if (pick == 3) bits = 0x46000000u + g.R(3) - 1u;   // 8192: x4096 = 2^25
    return bits;
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }

// Clases de funcion (forma)
enum Kind : u32 { kFill, kPlClear, kShutdown, kInsertSorted, kLlClear, kNop, kDmg, kDtor7, kLodCtor, kSync, kPoolNew, kPoolDel, kCompress, kDecompress4, kDecompress3 };
inline constexpr std::array<Kind, kFunctions> kKind{kFill, kPlClear, kShutdown, kShutdown, kInsertSorted, kLlClear, kNop, kNop, kNop, kDmg, kDmg, kDtor7, kLodCtor, kSync, kPoolNew, kPoolDel, kPoolNew, kPoolDel,
                                                    kCompress, kDecompress4, kDecompress3};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x699E20, 0x699F00, 0x732D60, 0x732D80, 0x733910, 0x733F40, 0x734610, 0x734620, 0x734640, 0x4C4910, 0x4C4960, 0x4C55D0, 0x4C55E0, 0x6E76E0, 0x552380, 0x552390,
                                                    0x5523C0, 0x5523D0, 0x4CFA30, 0x4CFA90, 0x4CFB10};
// Bytes que cada funcion toca a partir de su puntero `this` (o del puntero de salida)
inline constexpr std::array<u32, kFunctions> kSpan{0x7C, 0x13C, 0x54, 0x34, 0x54, 0x54, 4, 4, 4, 0x24, 0x24, 4, 0x24, 4, 4, 4, 4, 4, 8, 16, 12};
// Tamano del nodo del pool: 8 (simple) o 12 (doble)
inline constexpr u32 PoolNodeSize(u32 fn) { return fn == 14 || fn == 15 ? 8u : 12u; }
// Memoria global del juego tocada
inline constexpr u32 kDamageFlag = 0xA9B0B0, kPoolSingle = 0xB74484, kPoolDouble = 0xB74488, kWaterOffset = 0xC228A4, kClock = 0xB7CB84;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o de la salida) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   kFill: a[4..33] = los 30 punteros desde this+4 (ceros con densidad variable)
//   kPlClear, kNop, kDtor7: nada mas (los datos son el relleno de la arena)
//   kShutdown: a[4] = valor del campo `links`, a[5] = desplazamiento en B, a[6] = 0 nulo / 1 en B / 2 crudo
//   kInsertSorted, kLlClear: a[4] = enlaces usados (0..5), a[5] = enlaces libres (0..3), a[6] = base de los nodos en B, a[8..15] = permutacion de los 8 huecos de nodo, a[16..23] = distancias (bits float) de los 8 huecos,
//         a[24..26] = el dato (objeto, callback, distancia) para InsertSorted, a[27] = desplazamiento del dato en C
//   kDmg: a[4] = m_pRwObject (+0x1C, crudo), a[5] = plantilla danada (+0x20, crudo), a[6] = selector (byte, 0xA9B0B0), a[7] = desplazamiento de la matriz en C, a[8] = lo que devuelve el falso de RpAtomicClone,
//         a[9] = modo del frame (0 en D, 1 en D, 2 crudo), a[10] = desplazamiento del frame en D, a[11] = frame crudo, a[12] = EAX del falso de CAtomicModelInfo::CreateInstance(matriz)
//   kLodCtor: a[4] = EAX del falso de 0x4C4A60, a[5] = dword que deja en [this]
//   kSync: a[4] = reloj (0xB7CB84), a[5] = desfase previo (0xC228A4)
//   kPoolNew: a[4] = desplazamiento del pool en B, a[5] = EAX del falso de New, a[6] = tamano de pila (8 o 12 segun la funcion)
//   kPoolDel: a[4] = desplazamiento del pool en B, a[5] = capacidad, a[6] = indice del nodo, a[7] = desplazamiento del almacenamiento en C, a[8] = desplazamiento de los estados en D, a[9] = ultimo libre (+0xC), a[10] = 1 si el slot esta libre
//   kCompress: a[4..7] = floats de entrada, a[8] = desplazamiento de la entrada en B (la salida en A es a[0])
//   kDecompress4/3: a[4..7] = int16 de entrada (cada dword: los 16 bits bajos; los altos, ruido), a[8] = desplazamiento de la entrada en B (la salida en A es a[0])
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85BB9Cu, 0x85BC70u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kFill: {
        const u32 density = g.Pk({0u, 10u, 30u, 50u, 70u, 90u, 100u});   // porcentaje de huecos (nulos)
        for (u32 i = 0; i < 30; ++i) a[4 + i] = g.P(density) ? 0u : (g.P(20) ? 0xFFFFFFFFu : (g.W() | 1u));
        break;
    }
    case kPlClear:
    case kNop:
    case kDtor7:
        break;
    case kShutdown:
        a[6] = g.Pk({0u, 1u, 1u, 2u, 2u});
        a[5] = AuxOffset(g, 0x10);
        a[4] = a[6] == 2 ? Raw(g) : 0u;
        break;
    case kInsertSorted:
    case kLlClear: {
        a[4] = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 5u});
        a[5] = g.Pk({0u, 1u, 1u, 2u, 3u});
        a[6] = AuxOffset(g, 8 * 0x20);
        std::array<u32, 8> perm{0, 1, 2, 3, 4, 5, 6, 7};
        for (u32 i = 7; i > 0; --i) std::swap(perm[i], perm[g.R(i + 1)]);
        for (u32 i = 0; i < 8; ++i) a[8 + i] = perm[i];
        const u32 mode = g.R(4);   // 0 aleatorias, 1 crecientes, 2 muchas iguales, 3 sin NaN
        u32 prev = 0;
        for (u32 i = 0; i < 8; ++i) {
            u32 v = FloatBits(g);
            if (mode == 1) v = std::bit_cast<u32>(float(i * 10 + g.R(5)) * (g.P(10) ? -1.0f : 1.0f));
            if (mode == 2 && i > 0 && g.P(60)) v = prev;
            if (mode == 3) while (IsNanBits(v)) v = FloatBits(g);
            a[16 + i] = v; prev = v;
        }
        a[24] = g.W(); a[25] = Raw(g);
        const u32 dm = g.R(6);
        const u32 ex = a[16 + g.R(8)];   // la distancia del dato: igual a una existente, un ulp mas o menos, o libre
        a[26] = dm == 0 ? ex : (dm == 1 ? ex + 1u : (dm == 2 ? ex - 1u : FloatBits(g)));
        a[27] = AuxOffset(g, 0x10);
        break;
    }
    case kDmg:
        a[4] = Raw(g); a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0x101u, g.W()});
        a[7] = AuxOffset(g, 0x40);
        a[8] = Raw(g);
        a[9] = g.Pk({0u, 0u, 0u, 1u, 2u});
        a[10] = AuxOffset(g, 0x50);
        a[11] = Raw(g);
        a[12] = Raw(g);
        if ((a[6] & 0xFFu) == 0 && g.P(30)) a[4] = 0;
        if ((a[6] & 0xFFu) != 0 && g.P(30)) a[5] = 0;
        break;
    case kLodCtor:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kSync:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), g.W()});
        a[5] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        break;
    case kPoolNew:
        a[4] = AuxOffset(g, 0x20);
        a[5] = g.Pk({0u, 0xFFFFFFFFu, g.W(), Raw(g), 1u});
        a[6] = PoolNodeSize(c.fn);
        break;
    case kPoolDel: {
        const u32 sz = PoolNodeSize(c.fn);
        a[4] = AuxOffset(g, 0x20);
        const u32 cap = g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 64u, 100u, 128u, 300u});
        a[5] = cap;
        a[6] = g.Pk({0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap), g.R(cap)});
        a[7] = AuxOffset(g, sz * cap);
        a[8] = AuxOffset(g, cap);
        const u32 idx = a[6];
        a[9] = g.Pk({0xFFFFFFFFu, 0u, idx - 1u, idx, idx + 1u, cap - 1u, cap, cap + 5u, g.R(cap), g.R(cap), 0x7FFFFFFFu, 0x80000000u, 0x80000001u});
        a[10] = g.Pk({0u, 0u, 1u});
        break;
    }
    case kCompress:
        for (u32 i = 0; i < 4; ++i) a[4 + i] = CompressBits(g);
        a[8] = AuxOffset(g, 0x10);
        break;
    case kDecompress4:
    case kDecompress3:
        for (u32 i = 0; i < 4; ++i) a[4 + i] = g.Pk({0u, 1u, 0xFFFFu, 0x8000u, 0x7FFFu, 0x8001u, 0x1000u, 0xF000u, 0x400u, g.R(65536), g.R(65536), g.R(65536)}) | (g.W() & 0xFFFF0000u);
        a[8] = AuxOffset(g, 0x10);
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
