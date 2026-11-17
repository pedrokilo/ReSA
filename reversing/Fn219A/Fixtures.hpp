#pragma once
// Fixtures del banco Fn219A (lote 219A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (30 entradas, todas con hook y validated=0 antes del banco; las clases del listado salen del desensamblado, ver README.md y fixture_oracle.py).
//   idx  direccion  nombre real                                              forma
//    0   0x4A8DC0   ListItem_c::~ListItem_c (cuerpo vacio)                   thiscall() `ret`
//    1   0x4A8DD0   TList_c::TList_c                                          thiscall() `ret`: +0, +4, +8 = 0; devuelve this
//    2   0x4A8DE0   TList_c::~TList_c (cuerpo vacio)                          thiscall() `ret`
//    3   0x4A8E30   TList_c::RemoveItem(item)                                 thiscall(item) `ret 4`: EAX = recuento anterior
//    4   0x4A8E70   TList_c::RemoveHead                                      thiscall() `ret`: devuelve la cabeza anterior o 0
//    5   0x4A9010   TList_c::GetItemOffset(desdeCabeza BYTE, desplazamiento) thiscall `ret 8`
//    6   0x4A9090   FxFrustumInfo_c::FxFrustumInfo_c                         thiscall() `ret`: [+0x10] = 0 (llama a 0x4A9F70); devuelve this
//  7-15   0x5EC040 0x5EC0B0 0x5EC180 0x5EC1F0 0x5EC260 0x5EC2D0 0x5EC340 0x5EC3B0 0x5EC3E0  vaciado de vector (_Tidy): delete([+4]) si no es nulo; +4, +8, +0xC = 0
//   16  0x5EC070   fill_n que devuelve el final (4 bytes, B)                  thiscall(dest,n,&valor) `ret 0Ch`: devuelve dest + n*4
//   17  0x5EC0E0   idem 20 bytes                                              devuelve dest + n*20
//   18  0x5EC110   idem 4 bytes (C)
//   19  0x5EC140   idem 12 bytes                                              devuelve dest + n*12
//   20  0x5EC1B0   idem 4 bytes (D)
//   21  0x5EC220   vector<T*>::erase(pos) (4 bytes)                           thiscall(salida, posicion) `ret 8`
// 22-26  0x5ECAB0 0x5ECB30 0x5ECBB0 0x5ECC30 0x5ECCB0  vector::_Xlen (instancias B..F): lanza length_error, no vuelve
//   27   0x5ECF10   vector<T*>::insert(pos, n, valor) (instancia A)          thiscall(pos,n,&valor) `ret 0Ch`
//   28   0x5ED1B0   vector<CPedTaskPair>::insert(pos, n, valor) (20 bytes)    idem
//   29   0x5ED4B0   vector<T*>::insert(pos, n, valor) (instancia B)          idem
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL, o solo en la direccion
// del EXE cuando la DLL los llama por ella). Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4
// (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F219AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 30;
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
// La pila del hilo del banco (cerca de 0x00EB0000) se etiqueta aparte (Tag): un puntero crudo que caiga ahi se confundiria con un objeto de la pila, asi que ninguno se genera en esa zona
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}

// Clases de funcion (forma)
enum Kind : u32 { kEmpty, kListCtor, kRemoveItem, kRemoveHead, kGetOffset, kFrustum, kVecTidy, kFillNEnd4, kFillNEnd20, kFillNEnd12, kErase, kXlen, kInsert4, kInsert20 };
inline constexpr std::array<Kind, kFunctions> kKind{kEmpty, kListCtor, kEmpty, kRemoveItem, kRemoveHead, kGetOffset, kFrustum,
                                                    kVecTidy, kVecTidy, kVecTidy, kVecTidy, kVecTidy, kVecTidy, kVecTidy, kVecTidy, kVecTidy,
                                                    kFillNEnd4, kFillNEnd20, kFillNEnd4, kFillNEnd12, kFillNEnd4, kErase,
                                                    kXlen, kXlen, kXlen, kXlen, kXlen, kInsert4, kInsert20, kInsert4};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4A8DC0, 0x4A8DD0, 0x4A8DE0, 0x4A8E30, 0x4A8E70, 0x4A9010, 0x4A9090,
                                                    0x5EC040, 0x5EC0B0, 0x5EC180, 0x5EC1F0, 0x5EC260, 0x5EC2D0, 0x5EC340, 0x5EC3B0, 0x5EC3E0,
                                                    0x5EC070, 0x5EC0E0, 0x5EC110, 0x5EC140, 0x5EC1B0, 0x5EC220,
                                                    0x5ECAB0, 0x5ECB30, 0x5ECBB0, 0x5ECC30, 0x5ECCB0, 0x5ECF10, 0x5ED1B0, 0x5ED4B0};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0x10, 0xC, 0x10, 0xC, 0xC, 0xC, 0x60,
                                                   0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
                                                   0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0x10,
                                                   4, 4, 4, 4, 4, 0x10, 0x10, 0x10};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   kEmpty (0, 2), kListCtor (1), kFrustum (6), kXlen: nada mas (el objeto de A lleva el relleno de la arena)
//   kRemoveItem (3): a[4] = elemento (0 en B, 1 la propia lista, 2 en C), a[5] = desplazamiento del elemento, a[6] = modo de `siguiente` (+4), a[7] = su desplazamiento, a[8] = modo de `anterior` (+0),
//         a[9] = su desplazamiento, a[10] = recuento. Modos de puntero: 0 nulo, 1 nodo en C, 2 nodo en D, 3 la lista (this), 4 el propio elemento, 5 this + 4, 6 this + 8
//   kRemoveHead (4): a[4] = escenario (0 vacia, 1 un elemento, 2 varios con siguiente nulo, 3 varios con siguiente, 4 un elemento con cabeza cruda), a[5] = desplazamiento de la cabeza en B, a[6] = modo de `siguiente`,
//         a[7] = su desplazamiento, a[8] = cola cruda, a[9] = recuento, a[10] = cabeza cruda
//   kGetOffset (5): a[4] = longitud de la cadena (0..6), a[5] = desplazamiento base de los nodos en B, a[6] = separacion, a[7] = banderas (bit 0 cabeza nula, 1 cola nula, 2 ciclo, 3 corta `siguiente`, 4 corta `anterior`,
//         5 extremos crudos), a[8] = indice del corte, a[9] = argumento BYTE (dword con ruido), a[10] = desplazamiento (con signo), a[11], a[12] = cabeza y cola crudas
//   kVecTidy (7..15): a[4] = modo del puntero (0 nulo, 1 en B, 2 crudo), a[5] = desplazamiento en B, a[6] = puntero crudo, a[7], a[8] = +8 y +0xC previos
//   kFillNEnd4/20/12 (16..20): a[4] = n, a[5] = desplazamiento del valor en B, a[6], a[11..14] = palabras del valor, a[7] = destino (0 en A, 1 nulo (n = 1), 2 crudo (n = 0), 3 en B solapado con el valor),
//         a[8] = destino crudo, a[10] = delta (palabras con signo) del destino solapado
//   kErase (21): a[4] = tamano del vector (1..12), a[5] = desplazamiento de los elementos en B, a[6] = indice de la posicion, a[7] = salida (0 en C, 1 = this + 8, 2 = un elemento de B, 3 = this + 4, 4 = this + 0xC),
//         a[8] = desplazamiento de la salida en C o indice del elemento, a[9] = holgura del limite de capacidad (elementos)
//   kInsert4 (27, 29): a[4] = 0 vector vacio (inicio nulo) / 1 con bloque, a[5] = capacidad, a[6] = tamano, a[7] = desplazamiento de los elementos en B, a[8] = indice de pos, a[9] = n, a[10] = valor,
//         a[11] = desplazamiento del valor en C, a[12] = desplazamiento del bloque nuevo en D, a[13] = EAX del falso del envoltorio de copia
//   kInsert20 (28): igual que kInsert4 pero a[10..14] = las cinco palabras del valor, a[15] = desplazamiento del valor en C, a[16] = desplazamiento del bloque nuevo en D, a[17] = EAX de los falsos
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x887D5Cu, 0x870114u});
    auto& a = c.a;
    // desplazamiento de un nodo de 8 bytes en una arena auxiliar (alineado, o con 10 % de desalineacion de 1 a 3 bytes)
    const auto NodeOff = [&](u32 span) { return AuxOffset(g, span) + (g.P(10) ? 1 + g.R(3) : 0u); };
    switch (kKind[c.fn]) {
    case kEmpty:
    case kListCtor:
    case kFrustum:
    case kXlen:
        break;
    case kRemoveItem:
        a[4] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u});
        a[5] = NodeOff(0x20);
        a[6] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u});
        a[7] = NodeOff(0x20);
        a[8] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u});
        a[9] = NodeOff(0x20);
        a[10] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(100)});
        break;
    case kRemoveHead:
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 3u, 4u});
        a[5] = NodeOff(0x20);
        a[6] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u});
        a[7] = NodeOff(0x20);
        a[8] = Raw(g);
        a[9] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(100)});
        a[10] = Raw(g);
        break;
    case kGetOffset: {
        const u32 len = a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 3u, 4u});
        a[5] = 0x40 + 4 * g.R(0x40);
        a[6] = 4 * (2 + g.R(8));
        u32 flags = 0;
        if (g.P(10)) flags |= 1u;
        if (g.P(10)) flags |= 2u;
        if (len >= 2 && g.P(10)) flags |= 4u;
        if (len >= 2 && g.P(12)) flags |= 8u;
        if (len >= 2 && g.P(12)) flags |= 16u;
        a[8] = len ? g.R(len) : 0u;
        a[9] = (g.W() & 0xFFFFFF00u) | g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
        i32 off = i32(g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 0xFFFFFFFFu, 0xFFFFFFFBu, 0x80000000u, 0x7FFFFFFFu, g.R(10)}));
        if ((flags & 4u) && off > 14) off = 1 + i32(g.R(14));   // con ciclo el recorrido no termina solo: se acota
        if (off <= 0 && g.P(30)) flags |= 32u;
        a[7] = flags;
        a[10] = u32(off);
        a[11] = Raw(g); a[12] = Raw(g);
        break;
    }
    case kVecTidy:
        a[4] = g.Pk({0u, 1u, 1u, 2u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = Raw(g);
        a[7] = g.W(); a[8] = g.W();
        break;
    case kFillNEnd4:
    case kFillNEnd20:
    case kFillNEnd12: {
        a[4] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 8u});
        a[5] = 0x40 + 4 * g.R((u32(kWin) - 0x180) / 4);
        a[6] = Raw(g);
        a[7] = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 3u});
        if (a[7] == 1) a[4] = 1;
        if (a[7] == 2) a[4] = 0;
        if (a[7] == 3 && a[4] > 4) a[4] = 4;
        a[8] = AvoidStack(g.W());
        a[10] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFDu, 5u, 6u});
        a[11] = Raw(g); a[12] = Raw(g); a[13] = Raw(g); a[14] = Raw(g);
        break;
    }
    case kErase: {
        const u32 size = a[4] = g.Pk({1u, 1u, 2u, 3u, 4u, 5u, 8u, 12u, 1u + g.R(12)});
        a[5] = AuxOffset(g, 0x40);
        a[6] = g.Pk({0u, size - 1, size - 1, g.R(size), g.R(size), g.R(size)});
        a[7] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u});
        a[8] = a[7] == 2 ? g.R(size) : AuxOffset(g, 4);
        a[9] = g.Pk({0u, 0u, 1u, 2u});
        break;
    }
    case kInsert4: {
        a[4] = g.P(15) ? 0u : 1u;
        const u32 cap = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 10u, 12u});
        a[5] = a[4] ? cap : 0u;
        a[6] = a[4] ? g.Pk({0u, cap, cap, g.R(cap + 1), g.R(cap + 1)}) : 0u;
        a[7] = AuxOffset(g, 0x40);
        const u32 size = a[6];
        a[8] = g.Pk({0u, size, size, g.R(size + 1), g.R(size + 1)});
        u32 count = g.Pk({0u, 1u, 1u, 1u, 2u, 2u, 3u, 4u, 5u, 7u, 10u, 12u});
        if (g.P(5)) count = g.Pk({0x3FFFFFFFu - size + 1u, 0x3FFFFFFFu - size + 2u, 0xFFFFFFFFu, 0x40000000u, 0x3FFFFFFFu - size, 0x80000000u, 0x3FFFFFFFu - size - 1u});
        if (count > 64u && count <= 0x3FFFFFFFu - size) a[8] = size;   // sin cola que copiar: el destino de la copia de la cola seria una direccion arbitraria
        if (!a[4]) a[8] = 0;
        a[9] = count;
        a[10] = Raw(g);
        a[11] = AuxOffset(g, 4);
        a[12] = AuxOffset(g, 0x100);
        a[13] = Raw(g);
        break;
    }
    case kInsert20: {
        a[4] = g.P(15) ? 0u : 1u;
        const u32 cap = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 10u, 12u});
        a[5] = a[4] ? cap : 0u;
        a[6] = a[4] ? g.Pk({0u, cap, cap, g.R(cap + 1), g.R(cap + 1)}) : 0u;
        a[7] = AuxOffset(g, 0x140);
        const u32 size = a[6];
        a[8] = g.Pk({0u, size, size, g.R(size + 1), g.R(size + 1)});
        u32 count = g.Pk({0u, 1u, 1u, 1u, 2u, 2u, 3u, 4u, 5u, 7u, 10u, 12u});
        if (g.P(5)) count = g.Pk({0xCCCCCCCu - size + 1u, 0xCCCCCCCu - size + 2u, 0xFFFFFFFFu, 0x10000000u, 0xCCCCCCCu - size, 0x80000000u, 0xCCCCCCCu - size - 1u});
        if (count > 14u && count <= 0xCCCCCCCu - size) a[8] = size;   // sin cola que copiar (el destino de la copia de la cola seria una direccion arbitraria)
        if (!a[4]) a[8] = 0;
        a[9] = count;
        for (int k = 0; k < 5; ++k) a[10 + k] = Raw(g);
        a[15] = AuxOffset(g, 0x14);
        a[16] = AuxOffset(g, 0x400);
        a[17] = Raw(g);
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
