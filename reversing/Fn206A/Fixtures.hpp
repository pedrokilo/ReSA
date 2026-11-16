#pragma once
// Fixtures del banco Fn206A (lote 206A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en C++ con hook en el commit a1216d1f7c, sin banco; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 30 funciones.
//   idx  direccion  nombre real                                                                           forma
//    0   0x5EB6F0   CPedAttractorManager FindAssociatedAttractor<T> (instancia compartida)                 stdcall(efecto, entidad, lista) `ret 0Ch`
//    1   0x5EB740   CPedAttractorManager GetPedUsingEffect<T>                                              stdcall(efecto, entidad, lista) `ret 0Ch`
//    2   0x5EB7B0   CPedAttractorManager GetRelevantAttractor<T>                                            stdcall(peaton, efecto, entidad, lista) `ret 10h`
//    3   0x5EC660   CPedAttractorManager BroadcastDeparture<T>                                             stdcall(peaton, atractor, lista) `ret 0Ch`
//    4   0x5EC740   CPedAttractorManager DeRegisterPed<T>                                                  stdcall(peaton, atractor, lista) `ret 0Ch`
//    5   0x5EB930   envoltura stdcall de std::uninitialized_copy 0x5EAE10 (4 bytes, A)                      stdcall(origen, fin, destino) `ret 0Ch`
//    6   0x5EB960   envoltura de 0x5EAE40 (20 bytes)
//    7   0x5EB990   envoltura de 0x5EAE70 (4 bytes, B)
//    8   0x5EB9D0   envoltura de 0x5EAEA0 (12 bytes)
//    9   0x5EBA00   envoltura de 0x5EAEE0 (4 bytes, C)
//   10   0x5EC820   vaciado de un SArray de elementos sin destructor                                       thiscall() `ret`
//   11   0x5EC4B0   CPedAttractor destructor con borrado                                                   thiscall(banderas) `ret 4`
//   12   0x61FC20   CTaskSimpleBeKickedOnGround constructor                                                thiscall() `ret`
//   13   0x61FC40   CTaskSimpleBeKickedOnGround GetTaskType                                                `mov eax, 3EFh; ret`
//   14   0x61FCC0   CTaskSimpleBeKickedOnGround MakeAbortable                                              thiscall(peaton, prioridad, evento) `ret 0Ch`
//   15   0x61FD20   CTaskSimpleBeKickedOnGround StartAnim                                                  thiscall(peaton) `ret 4`
//   16   0x61FE00   CTaskSimpleBeKickedOnGround ProcessPed                                                 thiscall(peaton) `ret 4`
//   17   0x623120   CTaskSimpleBeKickedOnGround Clone                                                      thiscall() `ret`
//   18   0x61FE30   CTaskSimpleBeHitWhileMoving constructor                                                thiscall(modo, direccion) `ret 8`
//   19   0x61FE60   CTaskSimpleBeHitWhileMoving GetTaskType                                                `mov eax, 3F1h; ret`
//   20   0x61FEE0   CTaskSimpleBeHitWhileMoving MakeAbortable                                              thiscall(peaton, prioridad, evento) `ret 0Ch`
//   21   0x61FF40   CTaskSimpleBeHitWhileMoving StartAnim (x87, rand del CRT del EXE)                      thiscall(peaton) `ret 4`
//   22   0x620290   CTaskSimpleBeHitWhileMoving ProcessPed                                                 thiscall(peaton) `ret 4`
//   23   0x623190   CTaskSimpleBeHitWhileMoving Clone                                                      thiscall() `ret`
//   24   0x6348A0   CTaskComplexUseMobilePhone constructor                                                 thiscall(duracion) `ret 4`
//   25   0x6348D0   CTaskComplexUseMobilePhone GetTaskType                                                 `mov eax, 640h; ret`
//   26   0x6348E0   CTaskComplexUseMobilePhone cuerpo del destructor                                       thiscall() : [this] = 86E454h; jmp 0x61A3D0
//   27   0x6348F0   CTaskComplexUseMobilePhone PutAway                                                     thiscall(peaton) `ret 4` (no usa this)
//   28   0x634930   CTaskComplexUseMobilePhone MakeAbortable                                              thiscall(peaton, prioridad, evento) `ret 0Ch`
//   29   0x634A60   CTaskComplexUseMobilePhone CreateNextSubTask                                           thiscall(peaton) `ret 4`
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40, ventana de 0x2000: la mitad baja (0..0xFFF) para objetos, la alta de D para las vtables falsas) y los colaboradores falseados
// (el mismo falso en la direccion del EXE y en el cuerpo de la DLL). Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las
// arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F206AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 30;
constexpr unsigned kArenas = 4;
constexpr unsigned kA = 96;             // dwords de datos de un caso
constexpr unsigned kSeed0 = 92;         // a[92..95] = semillas de las arenas A, B, C y D

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x2000;   // ventana de cada arena
constexpr std::size_t kLow = 0x1000;   // mitad baja: los objetos del caso viven aqui (la alta, de 0x1000 a 0x1FFF, guarda las vtables falsas en D)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr u32 kVtObj = 0x1000, kVtEvt = 0x1100, kVtTask = 0x1200;   // vtables falsas en la arena D (desplazamientos respecto a la ventana): atractores, eventos y tareas

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
    std::array<u32, kA> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en la mitad baja de una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kLow) - span) / 4 + 1); }
// Mitad baja (0..0x7FC) o alta (0x800..0xFFC) de la zona baja de una arena auxiliar, para colocar dos objetos disjuntos en la misma arena
inline u32 AuxLow(Gen& g, u32 span) { return 4 * g.R((0x800u - span) / 4 + 1); }
inline u32 AuxHigh(Gen& g, u32 span) { return 0x800u + 4 * g.R((0x800u - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const auto avoidStack = [](u32 v) { return (v >= 0x00080000u && v < 0x00400000u) ? (v | 0x80000000u) : v; };   // la pila del hilo esta en esa zona: Tag() la anota aparte
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = avoidStack(g.W()), w6 = avoidStack(g.W()), w7 = avoidStack(g.W());
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un dword no nulo
inline u32 RawNZ(Gen& g) { u32 v = Raw(g); return v ? v : 0x1234567u; }
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
// Un float sin NaN
inline u32 FloatNoNan(Gen& g) { u32 v = FloatBits(g); while (IsNanBits(v)) v = FloatBits(g); return v; }
// Un float (como bits) finito (ni NaN ni infinito)
inline u32 FloatFinite(Gen& g) { u32 v = FloatBits(g); while ((v & 0x7F800000u) == 0x7F800000u) v = FloatBits(g); return v; }
inline bool IsInfBits(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }

// Clases de funcion (forma)
enum Kind : u32 { kFind, kPedUsing, kRelevant, kBcast, kDereg, kUcopy, kTidy, kAdtor, kBkCtor, kTaskType, kMakeAbort, kBkStart, kProcPed, kBkClone, kBhCtor, kBhClone, kBhStart, kMpCtor, kMpDtorBody, kPutAway,
                  kMpAbort, kMpNext };
inline constexpr std::array<Kind, kFunctions> kKind{kFind, kPedUsing, kRelevant, kBcast, kDereg, kUcopy, kUcopy, kUcopy, kUcopy, kUcopy, kTidy, kAdtor, kBkCtor, kTaskType, kMakeAbort, kBkStart, kProcPed, kBkClone,
                                                    kBhCtor, kTaskType, kMakeAbort, kBhStart, kProcPed, kBhClone, kMpCtor, kTaskType, kMpDtorBody, kPutAway, kMpAbort, kMpNext};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x5EB6F0, 0x5EB740, 0x5EB7B0, 0x5EC660, 0x5EC740, 0x5EB930, 0x5EB960, 0x5EB990, 0x5EB9D0, 0x5EBA00, 0x5EC820, 0x5EC4B0, 0x61FC20, 0x61FC40, 0x61FCC0,
                                                    0x61FD20, 0x61FE00, 0x623120, 0x61FE30, 0x61FE60, 0x61FEE0, 0x61FF40, 0x620290, 0x623190, 0x6348A0, 0x6348D0, 0x6348E0, 0x6348F0, 0x634930, 0x634A60};
// Bytes que cada funcion toca a partir de su puntero `this` (o del primer objeto)
inline constexpr std::array<u32, kFunctions> kSpan{0x10, 0x10, 0x10, 0x10, 0x10, 4, 4, 4, 4, 4, 0x10, 4, 0x10, 4, 0x10, 0x10, 0x10, 4, 0x18, 4, 0x18, 0x18, 0x18, 0x18, 0x20, 4, 0x20, 4, 0x20, 0x20};
// Memoria global del juego tocada
inline constexpr u32 kClock = 0xB7CB84, kFrameCounter = 0xB7CB4C, kAttractorPool = 0xB744C4, kBenchMarker = 0xA9B0B0;
inline constexpr u32 kAttractorSize = 0xC4;      // tamano del hueco del pool de atractores (el EXE divide por la constante magica 0x5397829D)
inline constexpr u32 kSlot = 0x40;               // separacion de los atractores en la arena C

// ------------------------------------------------------------------------------------------------ generador
// a[0] = desplazamiento de `this` (o del primer objeto) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[92..95] = semillas de las arenas A, B, C y D
//   kFind/kPedUsing/kRelevant: a[4] = modo de la lista (0 inicio nulo, 1 vacia, 2 normal, 3 fin antes del inicio, 4 fin nulo), a[5] = n (1..8), a[6] = desplazamiento del vector de punteros en B, a[7] = efecto (pila),
//         a[8] = entidad (pila), a[9] = peaton (pila, kRelevant), a[10] = bits del retorno de IsRegisteredWithPed por llamada, a[11] = basura de EAX de ese falso, a[12] = fin crudo si el modo es 0, a[13] = bytes hacia atras,
//         a[16..23] = efecto de cada atractor, a[24..31] = entidad de cada atractor, a[32..39] = orden (elemento i -> atractor a[32+i] % 8), a[40..47] = modo de la cola +20h/+24h de cada atractor (0 inicio nulo, 1 vacia, 2 uno,
//         3 -1, 4 dos), a[48..55] = peaton de cabeza (en D) de cada atractor, a[56..63] = fin crudo si el modo es 0
//   kBcast/kDereg: a[4] = modo de la lista (0 inicio nulo, 1 vacia, >= 2 normal), a[5] = n (1..6), a[6] = desplazamiento del vector en B, a[7] = peaton (pila), a[8] = seleccion del atractor (0 nulo, 1 en la lista,
//         2 ausente, 3 en la lista), a[9] = indice del elemento, a[10] = atractor ausente (crudo), a[11] = banderas de modificacion del falso (bit0 colas, bit1 fin de la lista), a[12] = inicio de la cola 1 que deja el falso,
//         a[13] = tamano de esa cola (0..2), a[14] = inicio de la cola 2, a[15] = tamano, a[16..21] = codigo de la cola +10h/+14h de cada atractor, a[24..29] = codigo de la cola +20h/+24h (0 inicio nulo, 1 vacia, 2 uno,
//         3 dos, 4 -1), a[32..37] = inicio crudo de la cola 1, a[40..45] = inicio crudo de la cola 2, a[48..53] = fin crudo si inicio nulo (cola 1), a[56..61] = fin crudo (cola 2), a[22] = nuevo numero de elementos
//         que deja el falso en la lista (0..n), a[23] = basura de EAX del falso
//   kUcopy: a[4+2i] = modo del argumento i (0 crudo, 1 arena A, 2 arena B, 3 nulo), a[5+2i] = valor o desplazamiento, a[10] = modo del retorno del falso (0 crudo, 1 arena D, 2 nulo), a[11] = valor o desplazamiento
//   kTidy: a[4] = modo del inicio (0 nulo, 1 crudo, 2 arena B), a[5] = valor crudo o desplazamiento, a[6] = fin, a[7] = limite
//   kAdtor: a[4] = banderas (pila), a[5] = EAX del falso del cuerpo, a[6] = dword que deja en [this], a[7] = desplazamiento del pool en B, a[8] = desplazamiento de los estados en D, a[9] = indice (0..63), a[10] = resto,
//         a[11] = primer hueco libre (+0Ch del pool)
//   kBkCtor: nada mas; kTaskType: nada mas
//   kMakeAbort: a[4] = hay animacion, a[5] = desplazamiento de la animacion en B, a[6] = prioridad (pila), a[7] = peaton (pila), a[8] = evento (pila), a[9] = m_BlendDelta inicial (+1Ch), a[10] = banderas (+2Eh)
//   kBkStart: a[4] = valor que devuelve KickingWhenDown en la llamada 1 (se prueba AL), a[16] = en la llamada 2, a[5] = resultado de GetFirstAssociation, a[6] = desplazamiento de la animacion nueva en B,
//         a[7] = clump (+18h del peaton: 0xFFFFFFFF nulo, si no el desplazamiento en D de un RwObject valido de tipo rpCLUMP: GetRpClump comprueba el tipo), a[8] = BoneTag, a[9] = jerarquia, a[10] = indice de hueso, a[11] = matrices, a[12] = desplazamiento del peaton en C, a[13] = desplazamiento del hueso en D, a[14] = [this+0Ch] previo,
//         a[15] = m_BlendDelta (+1Ch) de la animacion nueva
//   kProcPed: a[4] = BYTE +8 de la tarea, a[5] = dword de la animacion (+0Ch o +14h), a[6] = peaton (pila), a[7] = basura de EAX del falso
//   kBkClone: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B; kBhClone: ademas a[6] = +0Ch, a[7] = +10h de la tarea origen
//   kBhCtor: a[4] = modo (pila), a[5] = direccion (pila)
//   kBhStart: a[4] = modo (+0Ch), a[5] = direccion (+10h), a[6] = [this+14h] previo (0 o la animacion nueva), a[7] = clump (+18h: 0xFFFFFFFF nulo, si no el desplazamiento en D de un RwObject de tipo rpCLUMP), a[8] = semilla (word +20h), a[9] = peaton enganchado (+FCh), a[10] = reloj, a[11] = contador
//         de fotogramas, a[12] = valores de IsPlayer (byte 0 llamada 1, byte 1 llamada 2, resto basura), a[13..18] = valores de rand() por llamada, a[19] = hay asociacion existente (GetAssociation), a[20] = desplazamiento
//         de esa asociacion en B (mitad baja), a[21] = su m_BlendDelta, a[22] = su tiempo actual (+20h), a[23] = su identificador (word +2Ch), a[24] = desplazamiento de la jerarquia en D, a[25] = su tiempo total (+10h),
//         a[26] = desplazamiento de la animacion nueva en B (mitad alta), a[27] = m_BlendDelta de la animacion nueva, a[28] = desplazamiento del peaton en C, a[29] = +744h previo del peaton
//   kMpCtor: a[4] = duracion (pila); kMpDtorBody: a[4] = subtarea (0 nula, 1 en B), a[5] = desplazamiento de la subtarea en B
//   kPutAway: a[4] = desplazamiento del peaton en C, a[5] = arma guardada (+70Ch), a[6] = hay datos de jugador (+480h), a[7] = desplazamiento de los datos en D, a[8] = BYTE +85h
//   kMpAbort: a[4] = desplazamiento de la subtarea en B, a[5] = duracion (+0Ch), a[6] = prioridad (pila), a[8] = evento (0 nulo, 1 en D), a[9] = desplazamiento del evento en D, a[10] = desplazamiento del peaton en C,
//         a[11] = banderas +474h, a[12] = banderas +478h, a[13] = arma guardada (+70Ch), a[14] = reloj, a[15] = tipo de la subtarea en la llamada 1, a[16] = en la llamada 2, a[17] = valor que devuelve MakeAbortable de la
//         subtarea, a[18] = tipo del evento, a[19] = valor de IsPlayer, a[20] = BYTE +1Ch previo, a[21] = BYTE +18h previo, a[22] = +10h previo, a[23] = +14h previo, a[24] = basura de EAX del falso de PutAway
//   kMpNext: a[4] = desplazamiento de la subtarea en B, a[5] = duracion (+0Ch), a[7] = desplazamiento del peaton en C, a[8] = tipo de la subtarea, a[9] = BYTE +1Dh, a[10] = BYTE +1Ch, a[11] = BYTE +18h, a[12] = reloj,
//         a[13] = memoria de CTask::operator new (0 nula, 1 en D), a[14] = desplazamiento en D, a[15] = valor de CreateFirstSubTask, a[16] = +10h previo, a[17] = +14h previo, a[18] = banderas +474h, a[19] = +478h
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[kSeed0] = c.seed * 13u + 50u; c.a[kSeed0 + 1] = c.a[kSeed0] + 1u; c.a[kSeed0 + 2] = c.a[kSeed0] + 2u; c.a[kSeed0 + 3] = c.a[kSeed0] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x86C538u, 0x86D7D8u, 0x86E454u});
    auto& a = c.a;
    const auto smallVal = [&] { return g.Pk({0u, 1u, 2u, 3u, 0x10u, 0xFFFFFFFFu, g.W(), g.W()}); };
    const auto byteVal = [&] { return g.Pk({0u, 0u, 1u, 0xFFu, 0x80u, g.W() & 0xFFu}); };
    switch (kKind[c.fn]) {
    case kFind:
    case kPedUsing:
    case kRelevant: {
        a[4] = g.Pk({0u, 1u, 2u, 2u, 2u, 2u, 2u, 3u, 4u});
        a[5] = 1 + g.R(8);
        a[6] = AuxLow(g, 0x40);
        a[7] = g.Pk({0u, 0x1111u, 0x2222u, 0x3333u, RawNZ(g)});
        a[8] = g.Pk({0u, 0xAAAAu, 0xBBBBu, RawNZ(g)});
        a[9] = Raw(g);
        a[10] = g.W();
        a[11] = g.W() & 0xFFFFFF00u;
        a[12] = Raw(g);
        a[13] = 1 + g.R(11);
        for (u32 s = 0; s < 8; ++s) {
            a[16 + s] = g.P(40) ? a[7] : g.Pk({0u, 0x1111u, 0x2222u, 0x3333u, 0x4444u});
            a[24 + s] = g.P(40) ? a[8] : g.Pk({0u, 0xAAAAu, 0xBBBBu, 0xCCCCu});
            a[32 + s] = g.R(8);
            a[40 + s] = g.Pk({0u, 1u, 2u, 2u, 2u, 3u, 4u});
            a[48 + s] = Raw(g);
            a[56 + s] = Raw(g);
        }
        break;
    }
    case kBcast:
    case kDereg: {
        a[4] = g.Pk({0u, 1u, 2u, 2u, 3u, 3u, 4u});
        a[5] = 1 + g.R(6);
        a[6] = AuxLow(g, 0x40);
        a[7] = Raw(g);
        a[8] = g.Pk({0u, 1u, 1u, 1u, 2u, 3u, 3u, 3u});
        a[9] = g.R(6);
        a[10] = RawNZ(g);
        a[11] = g.Pk({0u, 0u, 1u, 2u, 3u});
        a[12] = Raw(g); a[13] = g.R(3); a[14] = Raw(g); a[15] = g.R(3);
        for (u32 s = 0; s < 6; ++s) {
            a[16 + s] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u});
            a[24 + s] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u});
            a[32 + s] = RawNZ(g);
            a[40 + s] = RawNZ(g);
            a[48 + s] = Raw(g);
            a[56 + s] = Raw(g);
        }
        a[22] = g.R(a[5] + 1);
        a[23] = g.W() & 0xFFFFFF00u;
        break;
    }
    case kUcopy:
        for (u32 i = 0; i < 3; ++i) {
            a[4 + 2 * i] = g.Pk({0u, 0u, 1u, 2u, 3u});
            a[5 + 2 * i] = a[4 + 2 * i] == 0 ? Raw(g) : AuxOffset(g, 0x40);
        }
        a[10] = g.Pk({0u, 0u, 1u, 2u});
        a[11] = a[10] == 0 ? Raw(g) : AuxOffset(g, 0x40);
        break;
    case kTidy:
        a[4] = g.Pk({0u, 1u, 1u, 2u});
        a[5] = a[4] == 1 ? RawNZ(g) : AuxOffset(g, 0x40);
        a[6] = Raw(g); a[7] = Raw(g);
        break;
    case kAdtor: {
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
    case kBkCtor:
    case kTaskType:
        break;
    case kMakeAbort:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x40);
        a[6] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000001u, 0x80000002u, 4u, g.W(), 1u, 2u});
        a[7] = Raw(g); a[8] = Raw(g);
        a[9] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xC0800000u, FloatBits(g), g.W()});
        a[10] = g.Pk({0u, 0xFFu, 4u, g.W() & 0xFFu, g.W() & 0xFFu});
        break;
    case kBkStart:
        a[4] = g.Pk({1u, 1u, 1u, 2u, 0xFFu, 0x100u, 0u, g.W()});
        a[16] = g.Pk({1u, 1u, 1u, 2u, 0xFFu, 0x100u, 0u, g.W()});
        a[5] = g.Pk({0u, 0u, RawNZ(g), RawNZ(g), 1u, 0x800u});
        a[6] = AuxOffset(g, 0x40);
        a[7] = g.Pk({0xFFFFFFFFu, AuxHigh(g, 0x10), AuxHigh(g, 0x10)}); a[8] = Raw(g); a[9] = Raw(g); a[10] = g.Pk({0u, 1u, 2u, 5u, 0xFFFFFFFFu, 0x3FFFFFFu, g.W(), Raw(g)}); a[11] = Raw(g);
        a[12] = AuxOffset(g, 0x500);
        a[13] = AuxLow(g, 0x40);
        a[14] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x61FD10u});
        a[15] = g.Pk({0u, 0x41000000u, 0xC0800000u, g.W()});
        break;
    case kProcPed:
        a[4] = g.Pk({0u, 1u, 2u, 0xFFu, 0x100u, g.W(), g.W() & 0xFFFFFF00u, (g.W() & 0xFFFFFF00u) | 1u});
        a[5] = g.Pk({0u, 0u, 1u, RawNZ(g), RawNZ(g)});
        a[6] = Raw(g);
        a[7] = g.W() & 0xFFFFFF00u;
        break;
    case kBkClone:
    case kBhClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x40);
        a[6] = Raw(g); a[7] = Raw(g);
        break;
    case kBhCtor:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, Raw(g)});
        a[5] = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, Raw(g)});
        break;
    case kBhStart: {
        a[4] = g.Pk({4u, 4u, 4u, 2u, 2u, 2u, 0u, 1u, 3u, 5u, 0xFFFFFFFFu, g.W()});
        a[5] = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 3u, 4u, 5u, 0xFFFFFFFFu, g.W()});
        a[6] = g.Pk({0u, 0u, 1u});
        a[7] = g.Pk({0xFFFFFFFFu, AuxHigh(g, 0x10), AuxHigh(g, 0x10)});
        a[8] = g.Pk({0u, 1u, 0xFFFFu, g.W() & 0xFFFFu, g.W() & 0xFFFFu, 999u, 1000u});
        a[9] = g.Pk({0u, 0u, 0u, 0u, RawNZ(g)});
        a[10] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.W(), g.R(100000), 0xFFFF0000u + g.R(0x10000)});
        a[11] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.R(100000)});
        a[12] = byteVal() | (byteVal() << 8) | (g.W() & 0xFFFF0000u);
        for (u32 i = 0; i < 6; ++i) a[13 + i] = g.W();
        a[19] = g.Pk({0u, 1u, 1u, 1u});
        a[20] = AuxLow(g, 0x40);
        a[21] = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u, FloatBits(g), FloatBits(g), FloatBits(g)});
        a[22] = FloatBits(g);
        a[23] = g.Pk({0x6Eu, 0x6Fu, 0x6Eu, 0x6Fu, 0x70u, 0u, 0x10u, 0x16Eu, 0x6Eu | 0x10000u, g.W() & 0xFFFFu});
        a[24] = AuxLow(g, 0x20);
        a[25] = g.Pk({0u, 0x3F800000u, 0x40000000u, 0x41200000u, 0x7FC00000u, 0x7F800000u, 0xFF800000u, 0x7F800001u, 0xFFC00001u, FloatBits(g), FloatBits(g), FloatBits(g), FloatFinite(g), FloatFinite(g)});
        a[26] = AuxHigh(g, 0x40);
        a[27] = g.Pk({0u, 0x41000000u, 0xC0800000u, g.W()});
        a[28] = AuxOffset(g, 0x800);
        a[29] = g.Pk({0u, 0xFFFFFFFFu, g.W()});
        // el tiempo total y el actual: como mucho un NaN entre ellos (la x87 elige el de mayor mantisa entre dos: no se modela, aunque _ftol de un NaN siempre da 0)
        if (IsNanBits(a[25]) && IsNanBits(a[22])) a[22] = FloatFinite(g);
        break;
    }
    case kMpCtor:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 5000u, g.W(), Raw(g)});
        break;
    case kMpDtorBody:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 4);
        break;
    case kPutAway:
        a[4] = AuxOffset(g, 0x720);
        a[5] = g.Pk({0u, 1u, 0x37u, 0x14Au, 0xFFFFFFFFu, g.W(), Raw(g)});
        a[6] = g.Pk({0u, 1u, 1u, 1u});
        a[7] = AuxOffset(g, 0x90);
        a[8] = g.Pk({0u, 1u, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        break;
    case kMpAbort:
        a[4] = AuxOffset(g, 0x40);
        a[5] = smallVal();
        a[6] = g.Pk({0u, 1u, 2u, 2u, 2u, 3u, 0xFFFFFFFFu, 0x80000002u, 4u, g.W()});
        a[8] = g.Pk({0u, 1u, 1u});
        a[9] = AuxOffset(g, 0x40);
        a[10] = AuxOffset(g, 0x720);
        a[11] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x400000u, 0xFFBFFFFFu});
        a[12] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 2u, 0xFFFFFFFDu});
        a[13] = g.Pk({0u, 0x37u, 0x37u, 0x14Au, g.W(), 1u});
        a[14] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.R(100000)});
        a[15] = g.Pk({0xCAu, 0xCAu, 0x641u, 0x642u, 0x643u, 0x643u, 0x640u, 0u, 0xFFFFFFFFu, 0x80000000u, g.W()});
        a[16] = g.Pk({0x643u, 0x643u, 0xCAu, 0x641u, 0x642u, 0u, g.W()});
        a[17] = g.Pk({0u, 1u, 1u, g.W(), g.W() & 0xFFFFFF00u, (g.W() & 0xFFFFFF00u) | 1u});
        a[18] = g.Pk({0x3Au, 0x3Au, 0x3Au, 0u, 1u, 0x39u, 0x3Bu, 0xFFFFFFFFu, g.W()});
        a[19] = g.Pk({0u, 1u, 1u, 0x100u, g.W()});
        a[20] = g.Pk({0u, 1u, 2u, 0xFFu, g.W() & 0xFFu});
        a[21] = g.Pk({0u, 1u, 2u, 0xFFu, g.W() & 0xFFu});
        a[22] = g.Pk({0u, 0xFFFFFFFFu, g.W()}); a[23] = g.Pk({0u, 0xFFFFFFFFu, g.W()});
        a[24] = g.W() & 0xFFFFFF00u;
        break;
    case kMpNext:
        a[4] = AuxOffset(g, 0x40);
        a[5] = smallVal();
        a[7] = AuxOffset(g, 0x720);
        a[8] = g.Pk({0x641u, 0x641u, 0x642u, 0x642u, 0x643u, 0x643u, 0xCAu, 0xCAu, 0x640u, 0u, 0xFFFFFFFFu, 0x644u, 0x7FFFFFFFu, 0x80000000u, g.W()});
        a[9] = g.Pk({0u, 0u, 1u, 2u, 0xFFu, g.W() & 0xFFu});
        a[10] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, g.W() & 0xFFu});
        a[11] = g.Pk({0u, 1u, 2u, 0xFFu, g.W() & 0xFFu});
        a[12] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.R(100000)});
        a[13] = g.Pk({0u, 1u, 1u, 1u});
        a[14] = AuxOffset(g, 0x40);
        a[15] = g.Pk({0u, Raw(g), Raw(g), 1u});
        a[16] = g.Pk({0u, 0xFFFFFFFFu, g.W()}); a[17] = g.Pk({0u, 0xFFFFFFFFu, g.W()});
        a[18] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x400000u, 0xFFBFFFFFu});
        a[19] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 2u, 0xFFFFFFFDu});
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
