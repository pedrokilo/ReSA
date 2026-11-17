#pragma once
// Fixtures del banco Fn221A (lote 221A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 221A, sin validar hasta este banco). 25 funciones (plantillas: Fn220A para constructor, destructor con borrado y cuerpo de los atractores y para el esqueleto, Fn151A para el
// destructor con borrado y GetSeekPos de CTaskComplexSeekEntity, Fn167A para la casilla de la IAT). Las clases del listado salen del desensamblado (ver PendienteValidar/221A.md y fixture_oracle.py).
//   idx  direccion  funcion                                                            forma
//   0..2    CPedLookAtAttractor    constructor 0x5EE960, destructor con borrado 0x5EE9D0, cuerpo del destructor 0x5EEA20   (clase 6)
//   3..5    CPedScriptedAttractor  0x5EEA30, 0x5EEAA0, 0x5EEAF0   (clase 7)
//   6..8    CPedParkAttractor      0x5EEB00, 0x5EEB70, 0x5EEBC0   (clase 8)
//   9..11   CPedStepAttractor      0x5EEBD0, 0x5EEC40, 0x5EEC90   (clase 9)
//   12  0x5EF920  BroadcastArrivalList  stdcall(peaton, atractor, lista) `ret 0Ch`
//   13  0x5EF980  RegisterPedList       stdcall(peaton, efecto, entidad, estado, lista) `ret 14h`
//   14  0x493870  CTaskComplexSeekEntity<RadiusAngleOffset>::ScalarDeletingDestructor  thiscall(banderas) `ret 4`
//   15  0x493960  CTheScripts::IsEntityWithinAnySearchLight  cdecl(entidad, int32* salida), bool en AL
//   16  0x496DC0  CTaskComplexSeekEntity<RadiusAngleOffset>::GetSeekPos  thiscall(salida, peaton) `ret 8`
//   17  0x4993D0  CTaskComplexBeCop::Constructor  thiscall(estado, dir, tarea) `ret 0Ch`
//   18..20  ExeProtectionStub::MD5Init 0x401000, MD5Update 0x40104A, MD5Final 0x40118B  cdecl
//   21..24  ExeProtectionStub::ResolveKernelImports 0x401FBB, ResolveLoaderImports 0x402370, InitCallbackTable 0x4024C0, CheckOsVersionSuite 0x4028A2  cdecl sin argumentos
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

namespace F221AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 128;        // dwords de entrada por caso

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
// Un float (como bits) "interesante": normales, ceros, infinitos, NaN, denormales
inline u32 FloatBits(Gen& g) {
    const u32 w = g.W();
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800001u, 0x00000001u, 0x007FFFFFu, 0x42C80000u, w, w, w & 0xBFFFFFFFu, (w & 0x807FFFFFu) | 0x3F000000u});
}

// Clases de funcion (forma)
enum Kind : u32 { kACtor, kASdd, kABody, kBcast, kReg, kSeekSdd, kLight, kSeekPos, kBeCop, kMd5Init, kMd5Upd, kMd5Fin, kResK, kResL, kCbTab, kOsVer };
inline constexpr std::array<Kind, kFunctions> kKind{kACtor, kASdd, kABody, kACtor, kASdd, kABody, kACtor, kASdd, kABody, kACtor, kASdd, kABody,
                                                    kBcast, kReg, kSeekSdd, kLight, kSeekPos, kBeCop, kMd5Init, kMd5Upd, kMd5Fin, kResK, kResL, kCbTab, kOsVer};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x5EE960, 0x5EE9D0, 0x5EEA20, 0x5EEA30, 0x5EEAA0, 0x5EEAF0, 0x5EEB00, 0x5EEB70, 0x5EEBC0, 0x5EEBD0, 0x5EEC40, 0x5EEC90,
                                                    0x5EF920, 0x5EF980, 0x493870, 0x493960, 0x496DC0, 0x4993D0, 0x401000, 0x40104A, 0x40118B, 0x401FBB, 0x402370, 0x4024C0, 0x4028A2};
// Clase de atractor (0 ATM, 1 Seat, 2 Stop, 3 Pizza, 4 Shelter, 5 TriggerScript, 6 LookAt, 7 Scripted, 8 Park, 9 Step) de cada funcion de las clases de atractor; 0xFF = ninguna
inline constexpr std::array<u32, kFunctions> kCls{6, 6, 6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
inline constexpr std::array<u32, 10> kClsVt{0x86C550, 0x86C568, 0x86C580, 0x86C598, 0x86C5B4, 0x86C5CC, 0x86C5E4, 0x86C5FC, 0x86C614, 0x86C62C};   // vtable del EXE de cada clase
inline constexpr std::array<u32, 10> kClsMaxPeds{5, 1, 5, 5, 5, 1, 1, 1, 1, 1};                                                                  // maximo de peatones que pasa cada constructor
inline constexpr std::array<u32, 10> kClsCtor{0x5EE470, 0x5EE540, 0x5EE610, 0x5EE6E0, 0x5EE7C0, 0x5EE890, 0x5EE960, 0x5EEA30, 0x5EEB00, 0x5EEBD0};   // constructor de cada clase en el EXE
// Siete floats (como bits) que cada constructor lee de .rdata y pasa al constructor de CPedAttractor
inline constexpr std::array<std::array<u32, 7>, 10> kClsFloats{{
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E4CCCCDu, 0x3E19999Au, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E000000u, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E4CCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E4CCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3F000000u, 0x40C8F5C3u, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E000000u, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E000000u, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E000000u, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E000000u, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu},
    {0x3F800000u, 0x46EA6000u, 0x453B8000u, 0x3E000000u, 0x3DCCCCCDu, 0x3DCCCCCDu, 0x3DCCCCCDu}}};
// Bytes del objeto principal que cada funcion toca a partir de su puntero (en la arena A)
inline constexpr u32 SpanOf(Kind k) {
    switch (k) {
    case kBcast: case kReg: return 0x10u;
    case kSeekSdd: return 4u;
    case kLight: return 0x18u;
    case kSeekPos: return 0x50u;
    case kBeCop: return 0x50u;
    case kMd5Init: case kMd5Upd: case kMd5Fin: return 0x68u;
    case kResK: case kResL: case kCbTab: case kOsVer: return 4u;
    default: return 0x10u;
    }
}
// Memoria global del juego tocada
inline constexpr u32 kAttractorPool = 0xB744C4, kBenchMarker = 0xA9B0B0;
inline constexpr u32 kLights = 0xA94D68, kLightBytes = 8 * 0x7C;     // ScriptSearchLightArray: 8 registros de 0x7C bytes
inline constexpr u32 kProt = 0xC9E000, kProtBytes = 0xA00;           // datos del bloque protector (punteros a API, cadenas y estructuras)
inline constexpr u32 kAttractorSize = 0xC4;      // tamano del hueco del pool de atractores (el EXE divide por la constante magica 0x5397829D)
inline constexpr u32 kMarkerValue = 0x5A;         // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa condiciona sus mutaciones a ella
// Desplazamientos de la salida de GetSeekPos en la arena A respecto a `this` (modos 1..5 de a[8])
inline constexpr std::array<u32, 5> kSeekAlias{0x00, 0x08, 0x0C, 0x38, 0x40};
// Direcciones del EXE de los colaboradores que anotan eventos
inline constexpr u32 kEvBaseCtor = 0x5EDFB0, kEvBaseDtor = 0x5EC410, kEvBroadcast = 0x5EEF80, kEvRegisterPed = 0x5EEE30, kEvPushBack = 0x5EE400, kEvTaskDelete = 0x61A5B0, kEvSeekBody = 0x493890;
inline constexpr u32 kEvSearch = 0x493280, kEvWander = 0x66F450, kEvLoaderMark = 0x402370, kEvGpa = 0x81E40C;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[90] = semilla de la region del bloque protector,
//   a[92..95] = semillas de las arenas A, B, C y D
//   kACtor: a[4] = efecto (crudo), a[5] = entidad (crudo), a[6] = estado de movimiento (crudo)
//   kASdd: a[4] = banderas (pila), a[5] = EAX del falso del cuerpo 0x5EC410, a[6] = dword que deja en [this], a[7] = desplazamiento del pool en B, a[8] = desplazamiento de los estados en D, a[9] = indice (0..63),
//         a[10] = resto, a[11] = primer hueco libre (+0Ch del pool)
//   kABody: a[5] = EAX del falso, a[6] = dword que deja en [this]
//   kBcast: a[4] = 0 sin inicio / 1 con bloque, a[5] = tamano en elementos (dword con signo), a[6] = desplazamiento de los elementos en B, a[7] = peaton, a[8] = modo del atractor (0 nulo, 1 elemento a[9], 2 crudo a[25]),
//         a[9] = indice, a[10] = EAX del falso de BroadcastArrival, a[11..22] = elementos, a[23] = limite crudo, a[24] = fin crudo si no hay inicio, a[25] = atractor crudo
//   kReg: a[4] = 0 sin inicio / 1 con bloque, a[5] = tamano (dword con signo), a[6] = desplazamiento de los elementos en B, a[7] = desplazamiento del efecto en B (0x100 + ...), a[8] = tipo del efecto (BYTE +0x34),
//         a[9] = entidad (cruda), a[10] = peaton (crudo), a[11] = estado (crudo), a[12] = desplazamiento de los registros en C, a[13] = EAX del falso de RegisterPed, a[16 + k] = selector del elemento k (0 ninguno, 1 efecto,
//         2 entidad, 3 ambos), a[24 + k] = efecto crudo del elemento k, a[32 + k] = entidad cruda del elemento k, a[40] = capacidad del pool (1..8), a[41] = desplazamiento de la estructura del pool en D,
//         a[42] = desplazamiento del almacenamiento en D, a[43] = primer hueco libre (dword con signo), a[44 + i] = estado del hueco i
//   kSeekSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos; a[5] = dword que el falso del cuerpo deja en [this]; a[6] = banderas
//   kLight: a[3] = dword en +0 de la entidad; a[4] = 0 posicion en +4, 1 matriz en B a[5]; a[5] = desplazamiento de la matriz en B; a[6..8] = posicion en +4; a[9..11] = posicion en +0x30 de la matriz; a[12] = desplazamiento
//         de la salida en C; a[13] = dword previo en la salida; a[14] = mascara de los falsos; a[15] = bits altos de EAX de los falsos; a[16 + i] = BYTE activo del foco i (dword); a[24 + i] = identificador del foco i
//   kSeekPos: a[3] = entidad (+0xC); a[4] = peaton; a[5..7] = floats del falso del calculador; a[8] = modo de la salida (0 en B a[9]; 1..5 en A en this + kSeekAlias); a[9] = desplazamiento en B
//   kBeCop: a[4] = estado, a[5] = direccion (0..255), a[6] = tarea
//   kMd5Init: nada mas
//   kMd5Upd: a[3] = contador bajo, a[4] = contador alto, a[5..8] = estado, a[9] = desplazamiento de la entrada en C, a[10] = longitud
//   kMd5Fin: a[3] = contador bajo, a[4] = contador alto, a[5..8] = estado, a[9..24] = relleno (0xC9E068), a[25] = 0 relleno estandar (0x80 y ceros) / 1 el de a[9..24]
//   kResK: a[3] = modulo de 0xC9E408, a[4] = modulo de 0xC9E410, a[5] = semilla de los resultados de GetProcAddress, a[6] = porcentaje de resultados nulos
//   kResL: a[3] = modulo de 0xC9E408, a[4..6] = resultados de la API de 3 argumentos (0xC9E5CF), a[7..8] = resultados de la API de 1 argumento (0xC9E1F3), a[9..14] = resultados de GetProcAddress
//   kCbTab: nada mas
//   kOsVer: a[3] = version mayor (+4), a[4] = mascara de suites (word +0x98), a[5] = resultado de RegOpenKey, a[6] = resultado de RegQueryValueEx (primera), a[7] = resultado de LocalAlloc (0 nulo / 1 buffer),
//         a[8] = resultado de RegQueryValueEx (segunda), a[9] = desplazamiento del buffer en C, a[10] = tipo de lista (0 coincide el primero, 1 coincide uno posterior, 2 ninguno, 3 lista vacia),
//         a[11] = longitud de la cadena de busqueda (0..15) escrita en 0xC9E1A8, a[12] = clave (valor de 0xC9E157 tras RegOpenKey), a[15] = tamano que devuelve la primera consulta,
//         a[16..31] = imagen de 64 bytes de la lista de cadenas que escribe la segunda consulta, a[32..35] = imagen de 16 bytes de la cadena de busqueda (0xC9E1A8)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    const Kind k = kKind[c.fn];
    a[1] = kind;
    a[2] = f / 4;
    a[90] = c.seed * 7u + 11u;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    a[0] = Offset(g, kind, SpanOf(k));
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x86C538u, 0x887D5Cu, 0x86C550u});
    switch (k) {
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
    case kBcast: {
        a[4] = g.P(12) ? 0u : 1u;
        const u32 size = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 10u, 12u, g.R(13), u32(-i32(1 + g.R(3)))});
        a[5] = size;
        a[6] = AuxOffset(g, 0x40);
        a[7] = Raw(g);
        const u32 j = i32(size) > 0 ? g.R(size) : 0u;
        a[8] = g.Pk({0u, 1u, 1u, 1u, 1u, 1u, 2u});
        a[9] = j;
        a[10] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFFFFu, Raw(g), Raw(g)});
        for (u32 i = 0; i < 12; ++i) a[11 + i] = g.P(8) ? 0u : ((g.P(10) && i > 0) ? a[10 + i] : Raw(g));
        a[23] = Raw(g);
        a[24] = Raw(g);
        a[25] = Raw(g);
        break;
    }
    case kReg: {
        a[4] = g.P(10) ? 0u : 1u;
        a[5] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, g.R(9), u32(-i32(1 + g.R(2)))});
        a[6] = 4 * g.R(0x40);
        a[7] = 0x200u + 4 * g.R(0xC0);
        a[14] = Raw(g);
        a[15] = Raw(g);
        a[8] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, g.R(10), g.R(10), g.R(10), 10u, 11u, 0x80u, 0xFFu, g.R(256), g.R(256)});
        a[9] = Raw(g);
        a[10] = Raw(g);
        a[11] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, Raw(g)});
        a[12] = AuxOffset(g, 0x120);
        a[13] = g.Pk({0u, 1u, 0x100u, 0xFFFFFFFFu, Raw(g)});
        for (u32 i = 0; i < 8; ++i) {
            a[16 + i] = g.Pk({0u, 0u, 1u, 2u, 3u, 3u});
            a[24 + i] = Raw(g);
            a[32 + i] = Raw(g);
        }
        a[40] = g.Pk({1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 1u + g.R(8)});
        a[41] = 4 * g.R(0x10);
        a[42] = 0x100u + 4 * g.R(0x80);
        const u32 size = a[40];
        a[43] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, size - 1u, g.R(size), g.R(size)});
        const u32 flavor = g.R(10);   // 0: todos usados, 1: solo uno libre, el resto: mitad libres
        const u32 freeIdx = g.R(size);
        for (u32 i = 0; i < 8; ++i) {
            u32 st = g.W() & 0xFFu;
            if (flavor == 0) st &= 0x7Fu;
            else if (flavor == 1) st = (i == freeIdx) ? (st | 0x80u) : (st & 0x7Fu);
            a[44 + i] = st;
        }
        break;
    }
    case kSeekSdd:
        a[4] = Raw(g);
        a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u, 3u, 0xFFFFFFFEu, 0x100u, 0x101u, g.W(), (g.W() & 0xFFFFFF00u) | 1u, (g.W() & 0xFFFFFF00u)});
        break;
    case kLight: {
        a[4] = g.P(55) ? 1u : 0u;
        a[5] = AuxOffset(g, 0x40);
        for (u32 i = 0; i < 3; ++i) { a[6 + i] = FloatBits(g); a[9 + i] = FloatBits(g); }
        a[12] = AuxOffset(g, 4);
        a[13] = g.Pk({0u, 0xFFFFFFFFu, g.W()});
        a[14] = g.Pk({0u, 0u, 0xFFu, 1u, 2u, 4u, 0x80u, 0x10u, g.W() & 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        a[15] = g.Pk({0u, 0xFFFFFFFFu, g.W()});
        for (u32 i = 0; i < 8; ++i) {
            a[16 + i] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, 0x101u, g.W(), (g.W() & 0xFFFFFF00u)});
            a[24 + i] = g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, g.W(), g.R(0x10000)});
        }
        break;
    }
    case kSeekPos:
        a[3] = Raw(g);
        a[4] = Raw(g);
        a[5] = FloatBits(g); a[6] = FloatBits(g); a[7] = FloatBits(g);
        a[8] = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u});
        a[9] = Offset(g, g.R(4), 12);
        break;
    case kBeCop:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu, Raw(g)});
        a[5] = g.Pk({0u, 1u, 2u, 3u, 0x7Fu, 0x80u, 0xFFu, g.R(256), g.R(256)});
        a[6] = Raw(g);
        break;
    case kMd5Init:
        break;
    case kMd5Upd: {
        const u32 idx = g.P(35) ? 0u : g.R(64);
        const u32 hiBits = g.P(8) ? (0x7FFFFFu - g.R(64)) : g.R(0x800000u);
        a[3] = (hiBits << 9) | (idx << 3) | g.R(8);
        a[4] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.R(100)});
        for (u32 i = 0; i < 4; ++i) a[5 + i] = g.W();
        const u32 len = g.Pk({0u, 1u, 2u, 3u, 7u, 8u, 15u, 16u, 31u, 32u, 33u, 55u, 56u, 57u, 63u, 64u, 65u, 100u, 127u, 128u, 129u, 191u, 192u, 200u, 256u, 300u, 511u, 512u, 600u, 1000u, 1024u, 1500u, 2000u, g.R(2000), g.R(200)});
        a[10] = len;
        const u32 unaligned = g.P(45) ? 1u : 0u;
        u32 off = 4 * g.R((u32(kWin) - len - 8) / 4);
        if (unaligned) off += 1 + g.R(3);
        a[9] = off;
        break;
    }
    case kMd5Fin: {
        const u32 idx = g.Pk({0u, 1u, 0x37u, 0x38u, 0x39u, 0x3Fu, g.R(64), g.R(64), g.R(64), 0x30u});
        const u32 hiBits = g.R(0x800000u);
        a[3] = (hiBits << 9) | (idx << 3) | g.R(8);
        a[4] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.R(100)});
        for (u32 i = 0; i < 4; ++i) a[5 + i] = g.W();
        for (u32 i = 0; i < 16; ++i) a[9 + i] = g.W();
        a[25] = g.P(35) ? 0u : 1u;
        break;
    }
    case kResK:
        a[3] = g.Pk({0u, g.W(), g.W(), 0x7FFE0000u});
        a[4] = g.Pk({0u, g.W(), g.W(), 0x7FFE0000u});
        a[5] = g.W();
        a[6] = g.Pk({0u, 0u, 5u, 20u, 50u, 100u});
        break;
    case kResL:
        a[3] = g.Pk({0u, g.W(), g.W(), 0x7FFE0000u});
        for (u32 i = 0; i < 3; ++i) a[4 + i] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, 1u, g.W(), 0x4000u});
        for (u32 i = 0; i < 2; ++i) a[7 + i] = g.Pk({0u, g.W(), g.W(), 0x7FFE0000u});
        for (u32 i = 0; i < 6; ++i) a[9 + i] = g.Pk({0u, g.W(), g.W(), 0x75000000u});
        break;
    case kCbTab:
        break;
    case kOsVer: {
        a[3] = g.Pk({0u, 1u, 3u, 4u, 5u, 5u, 6u, 6u, 10u, 0x80000000u, 0xFFFFFFFFu, g.R(12), g.R(12)});
        a[4] = g.Pk({0u, 0x10u, 0xFFFFu, 0x0F0Fu, 0x8010u, 1u, g.W() & 0xFFFFu, g.W() & 0xFFFFu});
        a[5] = g.Pk({0u, 0u, 0u, 1u, 2u, 0x80000000u, g.W()});
        a[6] = g.Pk({0u, 0u, 0u, 1u, 2u, g.W()});
        a[7] = g.Pk({0u, 1u, 1u, 1u});
        a[8] = g.Pk({0u, 0u, 0u, 1u, 2u, g.W()});
        a[9] = AuxOffset(g, 0x80);
        const u32 type = g.Pk({0u, 1u, 2u, 3u});
        a[10] = type;
        const u32 L = g.Pk({0u, 1u, 3u, 5u, 8u, 12u, g.R(16), g.R(16)});
        a[11] = L;
        a[12] = g.Pk({0u, 1u, g.W(), g.W()});
        a[15] = g.Pk({0u, 0x10u, 0x40u, 0x41u, g.R(0x40), g.W()});
        // cadena de busqueda (16 bytes en 0xC9E1A8): L letras de 'a'..'c', un NUL y letras de relleno
        u8 search[16]{};
        for (u32 i = 0; i < 16; ++i) search[i] = u8('a' + g.R(3));
        search[L < 15 ? L : 15] = 0;
        // lista de cadenas (64 bytes): entradas de 'a'..'c' terminadas en NUL y un NUL final; las que no coinciden empiezan por 'z'
        u8 list[64]{};
        u32 pos = 0;
        const u32 entries = type == 3 ? 0u : 1u + g.R(4);
        for (u32 e = 0; e < entries && pos < 50; ++e) {
            const bool match = (type == 0 && e == 0) || (type == 1 && e == 1);
            u32 len = 1 + g.R(6);
            if (match) len = L > 0 ? 1 + g.R(L < 15 ? L : 15) : 1;
            for (u32 k = 0; k < len && pos < 56; ++k) list[pos++] = match ? (k < L && k < 15 ? search[k] : u8('a' + g.R(3))) : (k == 0 ? u8('z') : u8('a' + g.R(3)));
            list[pos++] = 0;
        }
        for (u32 i = 0; i < 16; ++i) { std::memcpy(&a[16 + i], list + 4 * i, 4); }
        for (u32 i = 0; i < 4; ++i) { std::memcpy(&a[32 + i], search + 4 * i, 4); }
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
