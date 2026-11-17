#pragma once
// Fixtures del banco Fn220A (lote 220A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 220A, sin validar hasta este banco). 29 funciones (plantillas: Fn219A para los insert y los falsos de la STL, Fn206A para el destructor con borrado de atractores y el pool,
// Fn218A para el esqueleto del banco). Las clases del listado salen del desensamblado (ver PendienteValidar/220A.md y fixture_oracle.py).
//   idx  direccion  funcion                                                            forma
//    0   0x5ED750   vector<CVector>::_Insert_n (12 bytes)                              thiscall(pos, n, &x) `ret 0Ch`
//    1   0x5EDA50   vector<T*>::_Insert_n (4 bytes, instancia C)                       idem
//    2   0x5EDD90   vector<CPedTaskPair>::insert (20 bytes)                            thiscall(salida, pos, &x) `ret 0Ch`; devuelve la salida
//    3   0x5EDE50   vector<CVector>::insert (12 bytes)                                 idem
//    4   0x5EDF10   CPedAttractorManager::Constructor                                  thiscall() `ret`: pone a cero 30 dwords (+4/+8/+C de cada lista) y devuelve this
//    5   0x5EE200   push_back de 4 bytes, instancia A                                  thiscall(&x) `ret 4`
//    6   0x5EE270   push_back de 20 bytes                                              idem
//    7   0x5EE300   push_back de 4 bytes, instancia B                                  idem
//    8   0x5EE370   push_back de 12 bytes                                              idem
//    9   0x5EE400   push_back de 4 bytes, instancia C                                  idem
//  10..12  CPedATMAttractor     constructor 0x5EE470, destructor con borrado 0x5EE4E0, cuerpo del destructor 0x5EE530
//  13..15  CPedSeatAttractor    0x5EE540, 0x5EE5B0, 0x5EE600
//  16..18  CPedStopAttractor    0x5EE610, 0x5EE680, 0x5EE6D0
//  19..21  CPedPizzaAttractor   0x5EE6E0, 0x5EE760, 0x5EE7B0
//  22, 24, 25  CPedShelterAttractor  constructor 0x5EE7C0, destructor con borrado 0x5EE830, cuerpo 0x5EE880;  23: GetType 0x5EE820 (`mov eax, 4; ret`, hook V)
//  26..28  CPedTriggerScriptAttractor  0x5EE890, 0x5EE900, 0x5EE950
//   constructor: thiscall(efecto, entidad, estado) `ret 0Ch`: llama a CPedAttractor::Constructor 0x5EDFB0 (falso) con (efecto, entidad, estado, maximo de peatones, siete floats de .rdata) y escribe su vtable; devuelve this
//   destructor con borrado: thiscall(banderas BYTE) `ret 4`: cuerpo (real) y, si bit 0, libera el atractor en el pool (0xB744C4) en linea; devuelve this
//   cuerpo del destructor: thiscall() : [this] = vtable de la clase; salta a CPedAttractor::Destructor 0x5EC410 (falso)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL, o solo en la direccion del EXE
// cuando la DLL los llama por ella). Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4
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

namespace F220AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 29;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 96;         // dwords de entrada por caso

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
    std::array<u32, kSlots> a{};
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

// Clases de funcion (forma)
enum Kind : u32 { kInsN12, kInsN4, kIns20, kIns12, kMgr, kPb4, kPb20, kPb12, kACtor, kASdd, kABody, kType };
inline constexpr std::array<Kind, kFunctions> kKind{kInsN12, kInsN4, kIns20, kIns12, kMgr, kPb4, kPb20, kPb4, kPb12, kPb4,
                                                    kACtor, kASdd, kABody, kACtor, kASdd, kABody, kACtor, kASdd, kABody, kACtor, kASdd, kABody,
                                                    kACtor, kType, kASdd, kABody,
                                                    kACtor, kASdd, kABody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x5ED750, 0x5EDA50, 0x5EDD90, 0x5EDE50, 0x5EDF10, 0x5EE200, 0x5EE270, 0x5EE300, 0x5EE370, 0x5EE400,
                                                    0x5EE470, 0x5EE4E0, 0x5EE530, 0x5EE540, 0x5EE5B0, 0x5EE600, 0x5EE610, 0x5EE680, 0x5EE6D0, 0x5EE6E0, 0x5EE760, 0x5EE7B0,
                                                    0x5EE7C0, 0x5EE820, 0x5EE830, 0x5EE880,
                                                    0x5EE890, 0x5EE900, 0x5EE950};
// Clase de atractor (0 ATM, 1 Seat, 2 Stop, 3 Pizza, 4 Shelter, 5 TriggerScript) de cada funcion de las clases de atractor; 0xFF = ninguna
inline constexpr std::array<u32, kFunctions> kCls{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                                  0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3,
                                                  4, 4, 4, 4,
                                                  5, 5, 5};
inline constexpr std::array<u32, 6> kClsVt{0x86C550, 0x86C568, 0x86C580, 0x86C598, 0x86C5B4, 0x86C5CC};   // vtable del EXE de cada clase
inline constexpr std::array<u32, 6> kClsMaxPeds{5, 1, 5, 5, 5, 1};                                          // maximo de peatones que pasa cada constructor
// Siete floats (como bits) que cada constructor lee de .rdata (0x86C3EC.., 0x86C40C.., ...) y pasa al constructor de CPedAttractor
inline constexpr std::array<std::array<u32, 7>, 6> kClsFloats{{
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E4CCCCDu, 0x3E19999Au, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E000000u, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E4CCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E4CCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3F000000u, 0x40C8F5C3u, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E000000u, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu}}};
// Bytes del objeto que cada funcion toca a partir de su puntero `this`
inline constexpr u32 SpanOf(Kind k) { return k == kMgr ? 0xA0u : (k == kType ? 4u : 0x10u); }
// Memoria global del juego tocada
inline constexpr u32 kAttractorPool = 0xB744C4, kBenchMarker = 0xA9B0B0;
inline constexpr u32 kAttractorSize = 0xC4;      // tamano del hueco del pool de atractores (el EXE divide por la constante magica 0x5397829D)
inline constexpr u32 kMarkerValue = 0x5A;         // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa condiciona sus mutaciones a ella

inline constexpr u32 StrideOf(Kind k) { return (k == kInsN12 || k == kIns12 || k == kPb12) ? 12u : ((k == kIns20 || k == kPb20) ? 20u : 4u); }
inline constexpr bool IsStl(Kind k) { return k == kInsN12 || k == kInsN4 || k == kIns20 || k == kIns12 || k == kPb4 || k == kPb20 || k == kPb12; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[92..95] = semillas de las arenas A, B, C y D
//   kInsN12, kInsN4 (0, 1): a[4] = 0 vector sin inicio / 1 con bloque, a[5] = capacidad (elementos), a[6] = tamano, a[7] = desplazamiento de los elementos en B, a[8] = indice de pos, a[9] = n, a[10..12] = palabras del valor,
//         a[15] = desplazamiento del valor en C, a[16] = desplazamiento del bloque nuevo en D, a[17] = EAX de los falsos de envoltorio, a[18] = fin crudo si no hay inicio
//   kIns20, kIns12 (2, 3): a[4] = 0 sin inicio / 1 con bloque, a[6] = tamano, a[7] = desplazamiento de los elementos en B, a[8] = indice de pos, a[9] = 1 pos cruda (a[19]), a[10..14] = palabras del valor,
//         a[15] = desplazamiento del valor en C, a[16] = salida (0 en C a[17], 1 = this + 4, 2 = this + 8, 3 = this + 0xC), a[17] = desplazamiento de la salida en C, a[18] = 1 el falso del insert cambia el inicio a a[20],
//         a[21], a[22] = limite y fin crudos si no hay inicio
//   kPb4, kPb20, kPb12 (5..9): a[4] = 0 sin inicio / 1 con bloque, a[5] = capacidad (dword con signo), a[6] = tamano (dword con signo), a[7] = desplazamiento de los elementos en B, a[10..14] = palabras del valor,
//         a[15] = desplazamiento del valor en C, a[18], a[19] = limite y fin crudos si no hay inicio
//   kMgr (4): nada mas (el objeto de A, de 0xA0 bytes, lleva el relleno de la arena)
//   kACtor: a[4] = efecto (crudo), a[5] = entidad (crudo), a[6] = estado de movimiento (crudo)
//   kASdd: a[4] = banderas (pila), a[5] = EAX del falso del cuerpo 0x5EC410, a[6] = dword que deja en [this], a[7] = desplazamiento del pool en B, a[8] = desplazamiento de los estados en D, a[9] = indice (0..63),
//         a[10] = resto, a[11] = primer hueco libre (+0Ch del pool)
//   kABody: a[5] = EAX del falso, a[6] = dword que deja en [this]
//   kType: nada mas
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    const Kind k = kKind[c.fn];
    a[1] = kind;
    a[2] = f / 4;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    a[0] = Offset(g, kind, SpanOf(k));
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x86C538u, 0x887D5Cu, 0x86C550u});
    switch (k) {
    case kInsN12:
    case kInsN4: {
        const u32 S = StrideOf(k), maxSize = S == 12 ? 0x15555555u : 0x3FFFFFFFu;
        a[4] = g.P(15) ? 0u : 1u;
        const u32 cap = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 10u, 12u});
        a[5] = a[4] ? cap : 0u;
        a[6] = a[4] ? g.Pk({0u, cap, cap, g.R(cap + 1), g.R(cap + 1)}) : 0u;
        a[7] = AuxOffset(g, 0x100);
        const u32 size = a[6];
        a[8] = g.Pk({0u, size, size, g.R(size + 1), g.R(size + 1)});
        u32 count = g.Pk({0u, 1u, 1u, 1u, 2u, 2u, 3u, 4u, 5u, 7u, 10u, 12u});
        if (g.P(5)) count = g.Pk({maxSize - size + 1u, maxSize - size + 2u, 0xFFFFFFFFu, maxSize + 1u, maxSize - size, 0x80000000u, maxSize - size - 1u});
        if (count > 12u && count <= maxSize - size) a[8] = size;   // sin cola que copiar: el destino de la copia de la cola seria una direccion arbitraria
        if (!a[4]) a[8] = 0;
        a[9] = count;
        for (u32 i = 0; i < S / 4; ++i) a[10 + i] = Raw(g);
        a[15] = AuxOffset(g, 0x14);
        a[16] = AuxOffset(g, 0x300);
        a[17] = Raw(g);
        a[18] = Raw(g);
        break;
    }
    case kIns20:
    case kIns12: {
        a[4] = g.P(15) ? 0u : 1u;
        a[6] = a[4] ? g.Pk({0u, 1u, 1u, 2u, 3u, 5u, 8u, 12u, g.R(13)}) : 0u;
        a[7] = AuxOffset(g, 0x140);
        const u32 size = a[6];
        a[8] = g.Pk({0u, size, size, g.R(size + 1), g.R(size + 1), size + 1u});
        a[9] = g.P(12) ? 1u : 0u;
        for (u32 i = 0; i < 5; ++i) a[10 + i] = Raw(g);
        a[15] = AuxOffset(g, 0x14);
        a[16] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[17] = AuxOffset(g, 4);
        a[18] = g.P(30) ? 1u : 0u;
        a[19] = Raw(g);
        a[20] = Raw(g);
        a[21] = Raw(g);
        a[22] = Raw(g);
        break;
    }
    case kPb4:
    case kPb20:
    case kPb12: {
        const u32 s = g.R(10);
        u32 cap{}, size{};
        a[4] = 1u;
        if (s < 4) { cap = 1 + g.R(12); size = g.R(cap); }                      // cabe: camino rapido
        else if (s < 6) { cap = g.R(13); size = cap; }                          // lleno
        else if (s == 6) { cap = g.R(10); size = cap + 1 + g.R(3); }            // tamano mayor que la capacidad
        else if (s == 7) { cap = u32(-i32(1 + g.R(3))); size = g.R(13); }       // capacidad negativa (sin signo enorme): camino rapido
        else if (s == 8) { size = u32(-i32(1 + g.R(3))); cap = g.R(5); }        // tamano negativo
        else { a[4] = 0u; }                                                     // sin inicio
        a[5] = cap; a[6] = size;
        a[7] = AuxOffset(g, 0x100);
        for (u32 i = 0; i < 5; ++i) a[10 + i] = Raw(g);
        a[15] = AuxOffset(g, 0x14);
        a[18] = Raw(g); a[19] = Raw(g);
        break;
    }
    case kMgr:
    case kType:
        break;
    case kACtor:
        a[4] = Raw(g);
        a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, Raw(g)});
        break;
    case kASdd: {
        a[4] = g.Pk({0u, 1u, 1u, 3u, 0xFFFFFFFEu, 0x100u, 0x101u, g.W(), (g.W() & 0xFFFFFF00u) | 1u, (g.W() & 0xFFFFFF00u)});
        a[5] = g.Pk({0u, Raw(g), Raw(g)});
        a[6] = g.Pk({0u, g.W(), 0x86C538u});
        a[7] = AuxOffset(g, 0x14);
        a[8] = 8u + AuxOffset(g, 0x100);
        a[9] = g.Pk({0u, 1u, 2u, 3u, 5u, 7u, 15u, 30u, 31u, 62u, 63u, g.R(40), g.R(64)});   // indice dentro del pool (0..63) y hueco usado: CPool::Delete de la DLL lo exige (assert)
        a[10] = g.Pk({0u, 1u, kAttractorSize - 1u, kAttractorSize / 2u, g.R(kAttractorSize), g.R(kAttractorSize), g.R(kAttractorSize), 1u});
        const u32 idx = a[9];
        a[11] = g.Pk({0xFFFFFFFFu, 0u, idx - 1u, idx, idx + 1u, 5u, 100u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, g.R(40)});
        break;
    }
    case kABody:
        a[5] = g.Pk({0u, Raw(g), Raw(g)});
        a[6] = g.Pk({0u, g.W(), 0x86C538u});
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
