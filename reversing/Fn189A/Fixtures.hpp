#pragma once
// Fixtures del banco Fn189A (lote 189A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 189A, sin validar hasta este banco; las clases del listado salen del desensamblado y de los llamadores, ver README.md y fixture_oracle.py). 29 funciones.
//   idx  direccion  nombre real                                                                     forma
//    0   0x407670   CDirectory destructor con borrado                                               thiscall(flags) `ret 4`: ~CDirectory (0x5322D0, REAL); operator delete (0x8214BD) con el bit 0 de [esp+4]; eax = this
//    1   0x407800   CStreaming::IsObjectInCdImage                                                   cdecl(indice) `ret`: AL = ([0x8E4CCC + indice*20] != 0), sin comprobar el indice
//    2   0x407A10   CStreaming::ForceLayerToRead                                                    cdecl(arg) `ret`: BYTE [0x9654C4] = 1; DWORD [0x9654C8] = arg
//    3   0x407A30   CStreaming::SetLoadVehiclesInLoadScene                                          cdecl(byte) `ret`: BYTE [0x8A5A88] = [esp+4] (sin normalizar)
//    4   0x407AC0   CStreaming::RemoveUnusedModelsInLoadedList                                      `ret`
//    5   0x407BE0   CStreaming::ImGonnaUseStreamingMemory                                           `ret`
//    6   0x407BF0   CStreaming::IHaveUsedStreamingMemory                                            `ret`
//    7   0x407D30   CStreaming::GetDefaultAmbulanceModel                                            cdecl() `ret`: eax = [[0xBA6718]*4 + 0x8A5AB4]
//    8   0x407DC0   CStreaming::GetDefaultFireEngineModel                                           cdecl() `ret`: eax = [[0xBA6718]*4 + 0x8A5AD4]
//    9   0x407F20   CStreaming::LoadInitialVehicles                                                 `ret`
//   10   0x408230   CLinkList<CEntity*>::Insert(CEntity* const&)                                     thiscall(&dato) `ret 4`: primer enlace libre (+0x20); copia el dato; lo pasa a la cabeza de la lista usada
//   11   0x4082A0   CPool<CPed,CCopPed>::GetAt                                                       thiscall(indice) `ret 4`: nulo si el bit 7 del BYTE de estado esta puesto; si no, almacenamiento + indice*0x7C4
//   12   0x4096C0   CStreaming::StartRenderEntities                                                  `ret`: DWORD [0x8E48A0] = 0x9654F0
//   13   0x40B490   CStreaming::SetMissionDoesntRequireSpecialChar                                   cdecl(id) `jmp`: 0x409C90(id + 0x122)
//   14   0x40C1C0   CStreaming::RemoveStreamedScriptModel                                            cdecl(id) `jmp`: 0x4089A0(id + 0x6676)
//   15   0x40C450   CStreaming::ProcessEntitiesInSectorList (lista, flags)                           cdecl(lista, flags) `ret`: recorre la lista de punteros; scan code, estado de carga, area, tiempo, CreateRwObject, RequestModel
//   16   0x5B8340   constructor de CPool<CVehicleStructure>                                          thiscall(capacidad, nombre) `ret 8`: operator new[] x2 (0x821195); +0, +4, +0x10 = 1, +8, +0xC = -1; estados = 0x80
//   17   0x5EAAA0   std::fill, elementos de 4 bytes (A)                                              cdecl(primero, ultimo, &valor): lee el valor en cada vuelta
//   18   0x5EAAC0   std::fill, elementos de 20 bytes                                                 igual con `rep movs` de 5 dwords
//   19   0x5EAAF0   std::fill, elementos de 4 bytes (B)
//   20   0x5EAB10   std::fill, elementos de 4 bytes (C)
//   21   0x5EAB80   std::copy hacia delante, 20 bytes                                                cdecl(out, primero, ultimo, destino): *out = final del destino; eax = out
//   22   0x5EAC50   std::copy_backward, 20 bytes                                                     cdecl(out, primero, ultimo, final del destino): *out = inicio del destino; eax = out
//   23   0x5EACC0   std::copy_backward, 12 bytes (CVector)
//   24   0x5EAE10   std::uninitialized_copy, 4 bytes (A)                                             cdecl(primero, ultimo, destino): copia solo si el destino no es nulo; eax = final del destino
//   25   0x5EAE40   std::uninitialized_copy, 20 bytes
//   26   0x5EAE70   std::uninitialized_copy, 4 bytes (B)
//   27   0x5EAEA0   std::uninitialized_copy, 12 bytes
//   28   0x5EAEE0   std::uninitialized_copy, 4 bytes (C)
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

namespace F189AFix {
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
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un valor distinto de cero para punteros "crudos" que la funcion no desreferencia
inline u32 RawNZ(Gen& g) { const u32 v = Raw(g); return v ? v : 0x00C00000u + g.R(0x1000); }

// Clases de funcion (forma)
enum Kind : u32 { kDirDtor, kInCd, kForce, kSetVeh, kNop, kLevel, kLinkIns, kPedGetAt, kStartRender, kMission, kProcess, kPoolCtor, kFill4, kFill20, kCopy20, kCopyBack20, kCopyBack12, kUnin4, kUnin20, kUnin12 };
inline constexpr std::array<Kind, kFunctions> kKind{kDirDtor, kInCd, kForce, kSetVeh, kNop, kNop, kNop, kLevel, kLevel, kNop, kLinkIns, kPedGetAt, kStartRender, kMission, kMission, kProcess, kPoolCtor,
                                                    kFill4, kFill20, kFill4, kFill4, kCopy20, kCopyBack20, kCopyBack12, kUnin4, kUnin20, kUnin4, kUnin12, kUnin4};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x407670, 0x407800, 0x407A10, 0x407A30, 0x407AC0, 0x407BE0, 0x407BF0, 0x407D30, 0x407DC0, 0x407F20, 0x408230, 0x4082A0, 0x4096C0, 0x40B490, 0x40C1C0,
                                                    0x40C450, 0x5B8340, 0x5EAAA0, 0x5EAAC0, 0x5EAAF0, 0x5EAB10, 0x5EAB80, 0x5EAC50, 0x5EACC0, 0x5EAE10, 0x5EAE40, 0x5EAE70, 0x5EAEA0, 0x5EAEE0};
// Bytes que cada funcion toca a partir del puntero primario (a[0]: `this`, lista, pool, o el puntero de salida `out`)
inline constexpr std::array<u32, kFunctions> kSpan{0x10, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x34, 8, 4, 4, 4, 4, 0x14, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
// Tamano del elemento de las funciones de la STL
inline constexpr u32 ElemSize(Kind k) { return (k == kFill4 || k == kUnin4) ? 4u : ((k == kCopyBack12 || k == kUnin12) ? 12u : 20u); }

// Memoria global del juego tocada
inline constexpr u32 kMarker = 0xA9B0B0;                 // marca 0x5A de la prueba negativa (funciones vacias)
inline constexpr u32 kIdStack = 0xC87C18, kMemId = 0xC87B1C, kMemUsedMgr = 0xC87B34, kMemUsedBytes = 0x8E4CB4;   // pila de ids de memoria y contadores (las funciones vacias no deben tocarlos)
inline constexpr u32 kInfoBase = 0x8E4CC0, kInfoTable = 0xA9B0C8, kCurrLevel = 0xBA6718, kAmbTable = 0x8A5AB4, kFireTable = 0x8A5AD4, kTables = 0x8A5AB4;
inline constexpr u32 kForceByte = 0x9654C4, kLoadVeh = 0x8A5A88, kRenderLink = 0x8E48A0;
inline constexpr u32 kScanCode = 0xB7CD78, kCurrArea = 0xB72914, kPoolSingle = 0xB74484;   // 0xB74484 = CPtrNodeSingleLinkPool* (ProcessEntities: pool falso)
inline constexpr u32 kFakePool = 0x680, kFakeStates = 0x700;                              // pool falso en la arena D (cabecera de 0x14 bytes y 8 estados)
inline constexpr std::array<i32, 4> kCdSlots{0, 1, 7000, 26315};                                    // indices reales controlados de IsObjectInCdImage
inline constexpr std::array<i32, 8> kModelIdx{-3, -2, -1, 0, 1, 5, 100, 20000};                      // indices de modelo controlados de ProcessEntitiesInSectorList (los negativos prueban el movsx)

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del puntero primario en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[92..95] = semillas de las arenas A, B, C y D
//   kDirDtor: a[4] = entradas crudas (modo 2), a[5], a[6] = capacidad y numero (ruido), a[7] = byte de propiedad, a[8] = modo (0 nulo, 1 arena B, 2 crudo), a[9] = desplazamiento en B, a[10] = banderas (dword de pila)
//   kInCd: a[4] = modo (0 alias en la arena B, 1 ranura real), a[5] = ranura real (kCdSlots), a[6] = m_CDSize, a[7] = desplazamiento en B del dword de m_CDSize, a[8] = bits altos del indice (0..3, indice += a8 << 30)
//   kForce: a[4] = argumento, a[5] = dword previo en 0x9654C4, a[6] = dword previo en 0x9654C8
//   kSetVeh: a[4] = argumento (dword de pila), a[5] = dword previo en 0x8A5A88
//   kNop: a[4..21] = pila de ids de memoria (18 dwords), a[22] = m_id, a[23] = m_memUsed, a[24] = ms_memoryUsedBytes
//   kLevel: a[4] = modo (0 real, 1 alias en B), a[5] = nivel real, a[6] = desplazamiento en B, a[7] = bits altos (0..3), a[8..31] = 24 dwords previos de las tablas (0x8A5AB4..), a[32] = dword del alias
//   kLinkIns: a[4] = enlaces usados (0..5), a[5] = enlaces libres (0..4), a[6] = base de los nodos en B, a[8..15] = permutacion de los 8 huecos de nodo, a[24] = el dato (puntero crudo), a[27] = desplazamiento del dato en C
//   kPedGetAt: a[4] = modo del almacenamiento (0 crudo, 1 arena C), a[5] = valor crudo o desplazamiento en C, a[6] = base de los estados en D, a[7] = indice (0..0x43F), a[8] = byte de estado en el indice, a[9] = capacidad (+8, solo para el assert de la DLL)
//   kStartRender: a[4], a[5] = dwords previos en 0x8E48A0 y 0x8E48A4
//   kMission: a[4] = argumento
//   kProcess: a[4] = nodos (0..6), a[6] = base de nodos en B (8 por nodo: CPtrNodeSingleLink, en un pool falso de 8 ranuras en D), a[7] = base de entidades en C (0x40 por entidad), a[8..13] = entidad de cada nodo (0..3), a[14] = banderas de streaming (argumento),
//         a[15] = scan code actual (0xB7CD78), a[16..19] = scan code de cada entidad (+0x2C), a[20..23] = hueco de modelo de cada entidad (0..7, kModelIdx), a[24..27] = banderas (+0x1C), a[28..31] = byte de area (+0x2F)
//         (0 = el actual, 1 = 13, 2 = otro), a[32..35] = rwobject de la entidad (+0x18), a[36..43] = dword del estado de carga (+0x10 de la ranura) de cada hueco de modelo, a[44] = area actual (0xB72914),
//         a[45] = vtable de las entidades en D, a[46] = vtable de los model infos en D, a[47] = base de los model infos en D (0x40 por hueco), a[48] = base de las horas en D (4 por hueco),
//         a[49..56] = rwobject del model info (+0x1C), a[57..64] = el model info tiene horas (0 no, 1 si), a[65..72] = horas (encendido | apagado << 8 | ruido), a[73] = bits de "en rango" por llamada,
//         a[74] = ruido de EAX del falso, a[75] = el falso de CreateRwObject reescribe el modelo (0 no, 1 si), a[76] = el nuevo indice de modelo
//   kPoolCtor: a[4] = capacidad, a[5] = desplazamiento del almacenamiento devuelto (B), a[6] = desplazamiento de los estados devueltos (C), a[7] = nombre crudo, a[8], a[9] = retornos crudos si capacidad <= 0
//   STL: a[4] = n (elementos), a[5] = desalineacion del origen (0..3), a[6] = modo (0 disjunto en C, 1 solapado en B), a[7] = desplazamiento del origen en B (con desalineacion), a[8] = desplazamiento en elementos del
//        destino (solapado, con signo), a[9] = desplazamiento del destino en C, a[10] = desalineacion del destino, a[11] = modo del valor (0 en D, 1 dentro del rango), a[12] = desplazamiento del valor en D,
//        a[13] = elemento del valor (modo 1), a[14] = destino nulo (solo si n <= 1)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    a[0] = Offset(g, kind, kSpan[c.fn]);
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu});
    switch (kKind[c.fn]) {
    case kDirDtor:
        a[8] = g.Pk({0u, 1u, 1u, 2u, 2u});
        a[9] = AuxOffset(g, 4);
        a[4] = Raw(g); a[5] = g.W(); a[6] = g.W();
        a[7] = g.Pk({0u, 1u, 1u, 0xFFu, 2u, 0x80u, g.W() & 0xFFu});
        a[10] = g.Pk({0u, 1u, 1u, 2u, 0xFEu, 0xFFFFFFFFu, 0x100u, 0x101u, g.W(), g.W() | 1u});
        break;
    case kInCd:
        a[4] = g.Pk({0u, 0u, 0u, 1u});
        a[5] = g.R(4);
        a[6] = g.Pk({0u, 0u, 1u, 0x80000000u, 0xFFu, 0xFFFFFFFFu, 0x100u, 0x10000u, 0x1000000u, g.W(), g.W()});
        a[7] = 0x10 + AuxOffset(g, 0x30);
        a[8] = g.R(4);
        break;
    case kForce:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), Raw(g)});
        a[5] = g.Pk({0u, 1u, 0x01010101u, 0xFFFFFFFFu, 0x100u, g.W(), g.W()});
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W()});
        break;
    case kSetVeh:
        a[4] = g.Pk({0u, 1u, 2u, 0x100u, 0x101u, 0x80u, 0xFFu, 0xFFFFFF00u, g.W(), g.W()});
        a[5] = g.Pk({0u, 1u, 0x01010101u, g.W(), g.W()});
        break;
    case kNop:
        for (u32 i = 4; i <= 24; ++i) a[i] = g.P(20) ? 0u : g.W();
        break;
    case kLevel:
        a[4] = g.Pk({0u, 0u, 1u});
        a[5] = g.R(c.fn == 7 ? 16u : 12u);
        a[6] = 0x10 + AuxOffset(g, 0x30);
        a[7] = g.R(4);
        for (u32 i = 8; i <= 31; ++i) a[i] = g.P(50) ? g.R(700) : g.W();
        a[32] = g.Pk({0u, 0xFFFFFFFFu, g.R(700), g.W(), g.W()});
        break;
    case kLinkIns: {
        a[4] = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 5u});
        a[5] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u});
        a[6] = AuxOffset(g, 8 * 0x20);
        std::array<u32, 8> perm{0, 1, 2, 3, 4, 5, 6, 7};
        for (u32 i = 7; i > 0; --i) std::swap(perm[i], perm[g.R(i + 1)]);
        for (u32 i = 0; i < 8; ++i) a[8 + i] = perm[i];
        a[24] = Raw(g);
        a[27] = AuxOffset(g, 4);
        break;
    }
    case kPedGetAt:
        a[4] = g.Pk({0u, 1u, 1u});
        a[5] = a[4] == 1 ? AuxOffset(g, 0x20) : Raw(g);
        a[6] = 0x80 + 4 * g.R(0x240) + g.R(4);
        a[7] = g.R(0x440);                // indice 0..0x43F (la plantilla de la DLL comprueba el limite con assert: los fixtures lo cumplen; el EXE no lo comprueba)
        a[8] = g.Pk({0u, 1u, 0x7Fu, 0x80u, 0x81u, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        a[9] = 0x440 + g.R(0x100);        // capacidad del pool (+8): solo para el assert de la DLL
        break;
    case kStartRender:
        a[4] = g.Pk({0u, 0x9654F0u, g.W(), g.W()});
        a[5] = g.Pk({0u, g.W()});
        break;
    case kMission:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.R(300), g.R(300), g.R(30000), g.W(), g.W(), 0xFFFFFEDEu, 0xFFFF0000u});
        break;
    case kProcess: {
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u});
        a[6] = AuxOffset(g, 0x40);
        a[7] = AuxOffset(g, 0x100);
        for (u32 i = 0; i < 6; ++i) a[8 + i] = g.R(4);
        a[14] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, g.W(), g.W()});
        a[15] = g.Pk({0u, 1u, 0xFFFFu, g.W(), g.W() & 0xFFFFu, g.W() & 0xFFFFu});
        for (u32 e = 0; e < 4; ++e) {
            a[16 + e] = g.P(25) ? (a[15] & 0xFFFFu) : (g.P(50) ? g.R(0x10000) : ((a[15] + 1u + g.R(3)) & 0xFFFFu));
            a[20 + e] = g.R(8);
            u32 fl = g.W();
            fl = g.P(80) ? (fl | 0x80u) : (fl & ~0x80u);
            fl = g.P(18) ? (fl | 0x400u) : (fl & ~0x400u);
            fl = g.P(18) ? (fl | 0x80000u) : (fl & ~0x80000u);
            a[24 + e] = fl;
            a[28 + e] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u});
            a[32 + e] = g.P(50) ? 0u : RawNZ(g);
        }
        for (u32 j = 0; j < 8; ++j) a[36 + j] = g.Pk({0u, 1u, 1u, 1u, 2u, 3u, 4u, 0xFFu, g.W() & 0xFFu, 0x101u, g.W()});
        a[44] = g.Pk({0u, 1u, 2u, 3u, 5u, 13u, 13u, 0xFFu, 0x100u, 0x10000u, g.W()});
        a[45] = 4 * g.R(8);               // vtable de las entidades en D: 0x00..0x3C
        a[46] = 0x40 + 4 * g.R(8);        // vtable de los model infos: 0x40..0x7C
        a[48] = 0x80 + 4 * g.R(8);        // horas: 0x80..0xBC (4 bytes por hueco, 8 huecos: hasta 0xDC)
        a[47] = 0x100 + 4 * g.R(0x40);    // model infos: 0x100..0x1FC (0x40 por hueco: hasta 0x3FC)
        for (u32 j = 0; j < 8; ++j) {
            a[49 + j] = g.P(50) ? 0u : RawNZ(g);
            a[57 + j] = g.P(50) ? 1u : 0u;
            a[65 + j] = g.R(24) | (g.R(24) << 8) | (g.P(30) ? (g.W() & 0xFFFF0000u) : 0u);
        }
        a[73] = g.W();
        a[74] = g.W();
        a[75] = g.P(30) ? 1u : 0u;
        a[76] = g.Pk({0u, 0xFFFFu, g.W() & 0xFFFFu, 0x8000u, 5u});
        break;
    }
    case kPoolCtor: {
        const i32 cap = i32(g.Pk({0u, 1u, 2u, 3u, 5u, 16u, 50u, 50u, 50u, 63u, 64u, 100u, 255u, 300u, g.R(0x300), g.R(0x300), 0xFFFFFFFFu, 0xFFFFFFFBu, 0x80000000u, 0xF0000000u, 0x80000001u}));
        a[4] = u32(cap);
        a[5] = AuxOffset(g, 0x20);
        a[6] = g.R(4) + 4 * g.R((u32(kWin) - 0x310) / 4);
        a[7] = Raw(g);
        a[8] = Raw(g); a[9] = Raw(g);
        break;
    }
    case kFill4: case kFill20: case kCopy20: case kCopyBack20: case kCopyBack12: case kUnin4: case kUnin20: case kUnin12: {
        const u32 stride = ElemSize(kKind[c.fn]);
        const u32 maxN = stride == 4 ? 64u : (stride == 12 ? 50u : 40u);
        const u32 pick = g.R(12);
        const u32 cnt = pick == 0 ? 0u : (pick == 1 ? 1u : (pick == 2 ? 2u : (pick == 3 ? 3u : (pick == 4 ? maxN : (pick == 5 ? 4u : (pick == 6 ? 5u : g.R(maxN + 1)))))));
        a[4] = cnt;
        a[5] = g.R(4);
        a[6] = g.P(35) ? 1u : 0u;
        a[8] = u32(i32(g.R(2 * (cnt < 6 ? 6u : cnt) + 1)) - i32(cnt < 6 ? 6u : cnt));
        a[9] = 4 * g.R((u32(kWin) - cnt * stride - 8) / 4);
        a[10] = g.R(4);
        a[11] = g.P(30) ? 1u : 0u;
        a[12] = 4 * g.R((u32(kWin) - 0x20) / 4);
        a[13] = cnt ? g.R(cnt) : 0u;
        const bool nullOk = g.P(30);
        a[14] = (cnt <= 1 && nullOk && (kKind[c.fn] == kUnin4 || kKind[c.fn] == kUnin20 || kKind[c.fn] == kUnin12)) ? 1u : 0u;
        a[7] = a[6] == 1 ? 0x400 + 4 * g.R(0x40) + a[5] : 4 * g.R((u32(kWin) - cnt * stride - 8) / 4) + a[5];
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
