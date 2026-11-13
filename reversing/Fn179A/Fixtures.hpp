#pragma once
// Fixtures del banco Fn179A (lote 179A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas y enganchadas en el commit 9427ae3710, SIN validar; las clases del listado salen del desensamblado y de las vtables, ver README.md y fixture_oracle.py). 25 funciones.
//   idx  direccion  nombre real                                                                         forma
//    0   0x410610   CPool<ColDef>::Flush (pool de CColStore)                                            thiscall() `ret`: cap > 0: si BYTE +0x10: delete[] (+0), delete[] (+4); +0, +4, +8, +0xC = 0
//    1   0x410F80   CPool<ColDef>::CPool(capacidad, nombre)                                             thiscall(cap, nombre) `ret 8`: new[] (cap*0x2C), new[] (cap); +0x10 = 1, +8 = cap, +0xC = -1; mapa a 0x80; eax = this
//    2   0x410FE0   CPool<ColDef>::Flush (segunda copia identica)                                       thiscall() `ret`
//    3   0x411030   CPool<ColDef>::New                                                                  thiscall() `ret`: cursor +0xC, una vuelta, bit 0x80 del mapa libre; ocupa; eax = +0 + idx * 0x2C o 0
//    4   0x4110E0   CPool<ColDef> destructor con borrado                                                thiscall(flags) `ret 4`: cuerpo de Flush; operator delete (0x8214BD) con bit 0; eax = this
//    5   0x609270   CEventPlayerCommandToGroupGather::GetEventType (V, vtable 0x86D120, ranura 1)       thiscall() `ret`: mov eax,4Ch
//    6   0x609280   CEventPlayerCommandToGroupGather::CloneEditable (V, ranura 16)                      thiscall() `ret`: CEvent::operator new 4B5620 (0x1C), ctor 4B23D0 (2, [this+0x18]), [mem] = 86D120h; eax = mem
//    7   0x6092E0   CEventPlayerCommandToGroupGather::ScalarDeletingDestructor                          thiscall(flags) `ret 4`: cuerpo 609300 (funcion 8); CEvent::operator delete 4B5630 con bit 0; eax = this
//    8   0x609300   CEventPlayerCommandToGroupGather cuerpo del destructor                              thiscall() `ret`: [this] = 86D120h; jmp 4B2470
//    9   0x609310   CEventNewGangMember::GetEventPriority (V, ranura 2)                                 thiscall() `ret`: mov eax,2Ah
//   10   0x609320   CEventDontJoinPlayerGroup::GetEventPriority (V, ranura 2)                            thiscall() `ret`: mov eax,2Bh
//   11   0x609330   CEventPlayerCommandToGroupGather::GetEventPriority (V, ranura 2)                     thiscall() `ret`: mov eax,2Dh
//   12   0x6093B0   CPlayerPed cuerpo del destructor (marco SEH)                                         thiscall() `ret`: [this] = 86D168h; dos bloques de animacion (+0x48, +0x4C de [this+0x480]); RemoveGroup si falta 0x200000; ~CPed 5E8620
//   13   0x65EBA0   CTaskComplexGangFollower::GetTaskType (V, ranura 4)                                  thiscall() `ret`: mov eax,4B7h
//   14   0x65ED40   CTaskComplexGangFollower::CalculateOffsetPosition                                   thiscall(destino) `ret 4`: x87 24 bits; FromMultiply3x3 59C6D0 o copia; dword20 al destino
//   15   0x6634E0   CTaskSimpleGoToPointFine::ScalarDeletingDestructor                                   thiscall(flags) `ret 4`: cuerpo 65EEF0; CTask::operator delete 61A5B0 con bit 0; eax = this
//   16   0x69D9C0   CTaskAllocatorKillOnFoot::ScalarDeletingDestructor                                   thiscall(flags) `ret 4`: cuerpo 69BBF0; liberacion en linea del pool 0xB744BC con bit 0; eax = this
//   17   0x69DA00   CTaskAllocatorAttack::ScalarDeletingDestructor                                       cuerpo 69C2D0
//   18   0x69DA40   CTaskAllocatorKillThreatsBasic::ScalarDeletingDestructor                              cuerpo 69C780
//   19   0x69DA80   CTaskAllocatorKillThreatsDriveby::ScalarDeletingDestructor                           cuerpo 69CB60
//   20   0x69DAC0   CTaskAllocator::ScalarDeletingDestructor (base)                                      thiscall(flags) `ret 4`: [this] = 870E30h SIEMPRE; liberacion en linea con bit 0
//   21   0x69DB00   CTaskAllocatorPlayerCommandAttack::ScalarDeletingDestructor                           cuerpo 69DB40 (funcion 22)
//   22   0x69DB40   CTaskAllocatorPlayerCommandAttack cuerpo del destructor                              thiscall() `ret`: [this] = 870E78h; jmp 69C2D0
//   23   0x72A620   CAEWeatherAudioEntity::Constructor                                                   thiscall() `ret`: +0x10 = 0; [this] = 872A74h; BYTE +0x7C = 0; eax = this
//   24   0x72A640   CWeather::UpdateWeatherRegion                                                        cdecl(CVector*) `ret`: posicion dada o de la camara; palabra 0xC81314 = region
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

namespace F179AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;
constexpr unsigned kArenas = 4;
constexpr unsigned kParams = 64;

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
    std::array<u32, kParams> a{};
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
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4, 1u, 1u, 0x101u});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline float F(u32 bits) { return std::bit_cast<float>(bits); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }
// Un float de magnitud moderada (coordenadas del mundo y desplazamientos): enteros, fracciones, valores alrededor de 3 (el cuadrado de la distancia se compara con 9) y alguno diminuto
inline u32 CoordBits(Gen& g) {
    const float sign = g.P(50) ? -1.0f : 1.0f;
    const u32 m = g.R(8);
    float v;
    if (m == 0) v = float(g.R(200000)) / 64.0f;
    else if (m == 1) v = float(g.R(6000));
    else if (m == 2) v = float(g.R(4000)) / 1000.0f;
    else if (m == 3) v = 3.0f + float(int(g.R(9)) - 4) / 1048576.0f;
    else if (m == 4) v = 1.7320508f + float(int(g.R(5)) - 2) / 8388608.0f;
    else if (m == 5) v = float(g.R(1000000)) / 3.0f;
    else if (m == 6) v = std::bit_cast<float>(0x00800000u + (g.W() & 0x0FFFFFFFu));
    else v = float(g.R(100)) / 7.0f;
    return B(sign * v);
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kFlush, kPCtor, kPNew, kPSdd, kClone, kSdd, kEvBody, kPlayerDtor, kGangOff, kAllocSdd, kAllocBase, kWCtor, kWRegion };
inline constexpr std::array<Kind, kFunctions> kKind{kFlush, kPCtor, kFlush, kPNew, kPSdd, kConst, kClone, kSdd, kEvBody, kConst, kConst, kConst, kPlayerDtor, kConst, kGangOff, kSdd, kAllocSdd, kAllocSdd,
                                                    kAllocSdd, kAllocSdd, kAllocBase, kAllocSdd, kEvBody, kWCtor, kWRegion};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x410610, 0x410F80, 0x410FE0, 0x411030, 0x4110E0, 0x609270, 0x609280, 0x6092E0, 0x609300, 0x609310, 0x609320, 0x609330, 0x6093B0, 0x65EBA0, 0x65ED40,
                                                    0x6634E0, 0x69D9C0, 0x69DA00, 0x69DA40, 0x69DA80, 0x69DAC0, 0x69DB00, 0x69DB40, 0x72A620, 0x72A640};
// Colaborador-cuerpo que llama la funcion y falsea FkBody (cuerpo del destructor; 0 si ninguno) y operator delete que llama (0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0, 0, 0x609300, 0x4B2470, 0, 0, 0, 0, 0, 0, 0x65EEF0, 0x69BBF0, 0x69C2D0, 0x69C780, 0x69CB60, 0, 0x69DB40, 0x69C2D0, 0, 0};
inline constexpr std::array<u32, kFunctions> kDelExe{0, 0, 0, 0, 0x8214BD, 0, 0, 0x4B5630, 0, 0, 0, 0, 0, 0, 0, 0x61A5B0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// vtable que escribe la funcion (o que puede traer el objeto antes)
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0, 0, 0, 0x86D120, 0x86D120, 0x86D120, 0x86D120, 0x86D0A0, 0x86D0E0, 0x86D120, 0x86D168, 0x86F938, 0x86F938, 0x86F964, 0x870E48, 0x870E60,
                                                     0x870E90, 0x870EA8, 0x870E30, 0x870E78, 0x870E78, 0x872A74, 0};
// Bytes que cada funcion toca a partir de su puntero `this` (o del puntero de datos)
inline constexpr std::array<u32, kFunctions> kSpan{0x14, 0x14, 0x14, 0x14, 0x14, 4, 0x1C, 4, 4, 4, 4, 4, 0x488, 4, 0x40, 4, 4, 4, 4, 4, 4, 4, 4, 0x80, 0xC};
// Memoria global del juego tocada
inline constexpr u32 kAllocPool = 0xB744BC, kAnimGroups = 0xB4EA34, kOffsetsTable = 0xC196E8, kMovingTable = 0xC19728, kCameraBase = 0xB6F028, kWeatherRegion = 0xC81314;
// Constantes de las funciones de retorno fijo
inline constexpr u32 ConstValue(u32 fn) { return fn == 5 ? 0x4C : (fn == 9 ? 0x2A : (fn == 10 ? 0x2B : (fn == 11 ? 0x2D : 0x4B7))); }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o de la posicion / el objeto) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[60..63] = semillas de las arenas A, B, C y D
//   kConst: nada mas
//   kFlush: a[4] = modo de almacenamiento (0 nulo, 1 en B, 2 crudo), a[5] = desplazamiento en B, a[6] = crudo; a[7] = modo del mapa (0 nulo, 1 en C, 2 crudo), a[8] = desplazamiento en C, a[9] = crudo;
//         a[10] = capacidad (con signo), a[11] = primer libre (+0xC), a[12] = BYTE +0x10
//   kPSdd: igual que kFlush; a[13] = banderas de pila
//   kPCtor: a[4] = modo del primer new[] (0 nulo, 1 en B, 2 crudo), a[5] = desplazamiento en B, a[6] = crudo; a[7] = modo del segundo (0 nulo, 1 en C, 2 crudo), a[8] = desplazamiento en C, a[9] = crudo;
//         a[10] = capacidad (con signo), a[11] = nombre (crudo)
//   kPNew: a[4] = almacenamiento (crudo), a[5] = desplazamiento del mapa en D, a[6] = capacidad (>= 1), a[7] = primer libre (-1 .. capacidad - 1), a[8] = BYTE +0x10, a[9] = modo del mapa
//         (0 ninguno libre, 1 uno solo libre, 2 ~10%, 3 ~50%, 4 todos libres), a[10] = indice del libre unico, a[11] = estilo del contador (0 ceros, 1 0x7F, 2 aleatorio)
//   kClone: a[3] = modo de la memoria (0 nula, 1 en B a[4]); a[4] = desplazamiento en B; a[5] = gDtorWrite; a[6] = crudo; a[7] = selector del EAX del constructor; a[8] = dword en +0x18 (objetivo)
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX de los falsos (gRet); a[5] = gDtorWrite; a[6] = banderas
//   kEvBody: a[3] = dword previo; a[4] = gRet; a[5] = gDtorWrite
//   kPlayerDtor: a[4] = desplazamiento de los datos del jugador en B, a[5] = palabra +0x478, a[6] = grupo 1 (+0x48), a[7] = grupo 2 (+0x4C), a[8] = dword +0x38, a[9] = desplazamiento del array de grupos en D,
//         a[10] = bloque del grupo 1, a[11] = bloque del grupo 2, a[12] = cambio de +0x480 tras la llamada N a RemoveAnimBlockRef (0 ninguno), a[13] = datos alternativos en B (sin solaparse), a[14] = gDtorWrite,
//         a[15], a[16] = grupos 1 y 2 de los datos alternativos, a[17] = su dword +0x38, a[18], a[19] = bloques de esos grupos
//   kGangOff: a[4] = desplazamiento del lider en B, a[5] = modo de la matriz (0 nula, 1 en C), a[6] = desplazamiento en C, a[7] = estado de movimiento (+0x534), a[8..10] = posicion propia del lider (+4),
//         a[11..13] = posicion de la matriz (+0x30), a[14..16] = m_PedPosn, a[17..19] = dword20, a[20] = indice (BYTE +0x3C), a[21] = BYTE +0x3D, a[22] = desplazamiento del destino en D,
//         a[23] = modo del delta, a[24..31] = Offsets[0..3] (x, y), a[32..39] = MovingOffsets[0..3]
//   kAllocSdd / kAllocBase: a[3] = dword previo; a[4] = gRet; a[5] = gDtorWrite; a[6] = banderas; a[7] = desplazamiento del pool en B; a[8] = capacidad; a[9] = indice del objeto; a[10] = desplazamiento del mapa en D;
//         a[11] = primer libre (+0xC); a[12] = contador de 7 bits del mapa en la ranura del objeto (el bit 0x80 queda a cero: la DLL tiene un `assert` con la ranura libre)
//   kWCtor: a[3] = dword previo en [this]
//   kWRegion: a[4] = modo de la posicion (0 nula, 1 en A), a[5..7] = posicion, a[8] = modo de la matriz de la camara (0 nula, 1 en C), a[9] = desplazamiento en C, a[10..12] = posicion de la camara (+4),
//         a[13..15] = posicion de la matriz (+0x30), a[16] = palabra previa de 0xC81314, a[17] = clase de la region objetivo
inline void CoordFor(Gen& g, u32 region, u32& x, u32& y) {
    // cajas de cada region (x, y): 3: x > 1000, y > 910; 4: -850 < x < 1000, y > 1280; 2: x < -1430, -580 < y < 1430; 1: 250 < x < 3000, -3000 < y < -850; 0: resto
    const auto edge = [&](float lo, float hi) {   // dentro de (lo, hi) con parte de los casos pegados a los bordes
        const u32 m = g.R(8);
        if (m == 0) return std::nextafter(lo, hi);
        if (m == 1) return std::nextafter(hi, lo);
        if (m == 2) return lo;
        if (m == 3) return hi;
        if (m == 4) return std::nextafter(lo, -INFINITY);
        if (m == 5) return std::nextafter(hi, INFINITY);
        return lo + (hi - lo) * float(g.R(10000)) / 10000.0f;
    };
    switch (region) {
    case 3: x = B(edge(1000.0f, 6000.0f)); y = B(edge(910.0f, 6000.0f)); break;
    case 4: x = B(edge(-850.0f, 1000.0f)); y = B(edge(1280.0f, 6000.0f)); break;
    case 2: x = B(edge(-6000.0f, -1430.0f)); y = B(edge(-580.0f, 1430.0f)); break;
    case 1: x = B(edge(250.0f, 3000.0f)); y = B(edge(-3000.0f, -850.0f)); break;
    default: x = B(edge(-6000.0f, 6000.0f)); y = B(edge(-6000.0f, 6000.0f)); break;
    }
    if (g.P(8)) x = FloatBits(g);
    if (g.P(8)) y = FloatBits(g);
}
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[60] = c.seed * 13u + 50u; c.a[61] = c.a[60] + 1u; c.a[62] = c.a[60] + 2u; c.a[63] = c.a[60] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, g.W(), g.W(), 0xDEADBEEFu, 0x85AAB4u, 0x8710D0u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConst:
    case kWCtor:
        break;
    case kFlush:
    case kPSdd: {
        a[4] = g.Pk({0u, 1u, 1u, 2u}); a[5] = AuxOffset(g, 0x10); a[6] = Raw(g);
        a[7] = g.Pk({0u, 1u, 1u, 2u}); a[8] = AuxOffset(g, 0x10); a[9] = Raw(g);
        a[10] = g.Pk({0u, 1u, 2u, 0x2Cu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFFEu, g.R(0x400), g.R(0x400), g.W(), g.R(100) + 1u, 0x100u});
        a[11] = g.Pk({0u, 0xFFFFFFFFu, 5u, g.W(), g.W(), g.R(100)});
        a[12] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, g.R(256), 0x7Fu});
        a[13] = FlagsDword(g);
        break;
    }
    case kPCtor: {
        a[10] = g.Pk({0u, 1u, 2u, 3u, 7u, 16u, 100u, 0x100u, 0x200u, 0x3FFu, 0x400u, g.R(0x400) + 1u, g.R(0x400) + 1u, g.R(0x400) + 1u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x80000001u, 0xFFFFFF00u,
                      0xFFFFFFF0u, 0xC0000000u, g.W() | 0x80000000u});
        const bool pos = i32(a[10]) > 0;
        a[4] = g.Pk({0u, 1u, 1u, 1u, 2u}); a[5] = AuxOffset(g, 0x10); a[6] = Raw(g);
        a[7] = pos ? 1u : g.Pk({0u, 1u, 2u}); a[8] = AuxOffset(g, pos ? (a[10] + 3u) & ~3u : 4u); a[9] = Raw(g);
        a[11] = Raw(g);
        break;
    }
    case kPNew: {
        a[4] = Raw(g);
        a[6] = g.Pk({1u, 2u, 3u, 5u, 8u, 16u, 30u, 64u, 100u, 128u, 255u, 300u, 1u, 2u, g.R(300) + 1u, g.R(300) + 1u});
        a[5] = AuxOffset(g, a[6] + 4u);
        a[7] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, a[6] - 1u, a[6] - 1u, a[6] - 2u, g.R(a[6]), g.R(a[6]), g.R(a[6])});
        if (i32(a[7]) < -1) a[7] = 0xFFFFFFFFu;
        a[8] = g.Pk({0u, 1u, 1u, 0x80u, 0xFFu});
        a[9] = g.R(5);
        a[10] = g.Pk({0u, a[6] - 1u, g.R(a[6]), g.R(a[6]), g.R(a[6]), (a[7] + 1u) % a[6], (a[7] + 2u) % a[6]});
        a[11] = g.R(3);
        break;
    }
    case kClone:
        a[3] = g.Pk({0u, 1u, 1u, 1u});
        a[4] = AuxOffset(g, 0x1C);
        a[5] = Raw(g);
        a[6] = Raw(g);
        a[7] = g.R(4);
        a[8] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kEvBody:
        a[4] = Raw(g); a[5] = g.Pk({Raw(g), vt, vt, 0u});
        break;
    case kPlayerDtor: {
        a[4] = AuxOffset(g, 0x90);
        a[5] = g.Pk({0u, 0x200000u, 0x200000u, 0xFFFFFFFFu, 0xFFDFFFFFu, 0x200001u, 0x1FFFFFu, g.W(), g.W(), g.W() | 0x200000u, g.W() & ~0x200000u});
        const auto grp = [&] { return g.Pk({0u, 0u, 1u, 2u, 0xFFFFFFFFu, 5u, 99u, 100u, 0xFFFFFFE2u, 0xFFFFFFE3u, g.R(100), g.R(100), g.R(100), 0xFFFFFFFFu - g.R(30)}); };
        a[6] = grp(); a[7] = g.P(25) ? a[6] : grp();
        a[8] = Raw(g);
        a[9] = 700u + 4u * g.R(100);
        a[10] = g.Pk({Raw(g), 0x00B5D4A0u, 0x00B5D4A0u + 0x20u, 0x00B5D4A0u + 0x1Fu, 0x00B5D4A0u - 1u, 0x00B5D4A0u + 0x20u * g.R(100), 0x00B5D4A0u + 0x20u * g.R(100), g.W(), 0u});
        a[11] = g.Pk({Raw(g), 0x00B5D4A0u, 0x00B5D4A0u + 0x20u * g.R(100), 0x00B5D4A0u - 0x20u, g.W(), 0u, a[10]});
        a[12] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        for (int tries = 0; tries < 16; ++tries) {   // los datos alternativos no se solapan con los primeros
            a[13] = AuxOffset(g, 0x90);
            if (a[13] + 0x90u <= a[4] || a[4] + 0x90u <= a[13]) break;
        }
        if (!(a[13] + 0x90u <= a[4] || a[4] + 0x90u <= a[13])) a[13] = a[4] >= 0x90u ? 0u : 0x100u;
        a[14] = Raw(g);
        a[15] = grp(); a[16] = grp(); a[17] = Raw(g);
        a[18] = g.Pk({Raw(g), 0x00B5D4A0u + 0x20u * g.R(100), g.W(), 0u});
        a[19] = g.Pk({Raw(g), 0x00B5D4A0u + 0x20u * g.R(100), g.W(), 0u});
        break;
    }
    case kGangOff: {
        a[4] = AuxOffset(g, 0x540);
        a[5] = g.Pk({0u, 1u, 1u, 1u});
        a[6] = AuxOffset(g, 0x3C);
        a[7] = g.Pk({0u, 1u, 2u, 3u, 4u, 4u, 4u, 5u, 6u, 6u, 7u, 7u, 8u, 9u, 0xFFFFFFFFu, g.W()});
        if (a[5] == 0 && (a[7] == 4 || a[7] == 6 || a[7] == 7)) a[5] = 1u;   // el EXE usa la matriz del lider sin comprobar nulo con esos estados
        const u32 mode = g.R(6);   // 0 aleatorio, 1 sin diferencia, 2 alrededor de 3 en un eje, 3 alrededor de 3 repartido, 4 con NaN / infinitos, 5 pequeno
        a[23] = mode;
        for (u32 i = 0; i < 3; ++i) { a[8 + i] = CoordBits(g); a[11 + i] = CoordBits(g); }
        const u32 usedBase = a[5] ? 11u : 8u;   // la posicion que usa la funcion (matriz si la hay)
        const auto lead = [&](u32 i) { return F(a[usedBase + i]); };
        for (u32 i = 0; i < 3; ++i) a[14 + i] = CoordBits(g);
        if (mode == 1) for (u32 i = 0; i < 3; ++i) a[14 + i] = a[usedBase + i];
        if (mode == 2) {
            const u32 axis = g.R(3);
            const float mag = 3.0f + float(int(g.R(9)) - 4) / 4194304.0f;
            for (u32 i = 0; i < 3; ++i) a[14 + i] = a[usedBase + i];
            if (std::isfinite(lead(axis))) a[14 + axis] = B(lead(axis) - (g.P(50) ? mag : -mag));
        }
        if (mode == 3) {
            const float comp = 1.7320508f + float(int(g.R(7)) - 3) / 8388608.0f;
            for (u32 i = 0; i < 3; ++i) if (std::isfinite(lead(i))) a[14 + i] = B(lead(i) + (g.P(50) ? comp : -comp));
        }
        if (mode == 4) { for (u32 i = 0; i < 3; ++i) if (g.P(50)) a[14 + i] = FloatBits(g); if (g.P(40)) a[8 + g.R(6)] = FloatBits(g); }
        if (mode == 5) for (u32 i = 0; i < 3; ++i) a[14 + i] = B(lead(i) + float(int(g.R(2001)) - 1000) / 1000.0f);
        for (u32 i = 0; i < 3; ++i) a[17 + i] = g.Pk({0u, 0x80000000u, g.W(), CoordBits(g), CoordBits(g)});
        a[20] = g.R(4);
        a[21] = g.Pk({0u, 0x10u, 0x10u, 0x10u, 0xFFu, 0xEFu, 0x01u, g.R(256), g.R(256)});
        a[22] = AuxOffset(g, 0x10);
        for (u32 i = 0; i < 16; ++i) a[24 + i] = g.P(80) ? CoordBits(g) : FloatBits(g);
        break;
    }
    case kAllocSdd:
    case kAllocBase: {
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        a[8] = g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 64u, 100u, 128u, 300u});
        a[9] = g.Pk({0u, a[8] - 1u, g.R(a[8]), g.R(a[8]), g.R(a[8]), g.R(a[8])});
        a[7] = AuxOffset(g, 0x20);
        a[10] = AuxOffset(g, a[8] + 4u);
        a[11] = g.Pk({0xFFFFFFFFu, 0u, a[9] - 1u, a[9], a[9] + 1u, a[8] - 1u, a[8], a[8] + 5u, g.R(a[8]), g.R(a[8]), 0x7FFFFFFFu, 0x80000000u, 0x80000001u});
        a[12] = g.Pk({0u, 1u, 0x7Fu, 0x7Eu, g.R(128), g.R(128)});
        break;
    }
    case kWRegion: {
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[8] = g.Pk({0u, 1u, 1u});
        a[9] = AuxOffset(g, 0x3C);
        const u32 region = g.R(6);
        a[17] = region;
        u32 x{}, y{};
        CoordFor(g, region > 4 ? 0u : region, x, y);
        a[5] = x; a[6] = y; a[7] = CoordBits(g);
        u32 cx{}, cy{};
        CoordFor(g, g.R(5), cx, cy);
        a[10] = cx; a[11] = cy; a[12] = CoordBits(g);
        CoordFor(g, g.R(5), cx, cy);
        a[13] = cx; a[14] = cy; a[15] = CoordBits(g);
        // con la posicion nula la que cuenta es la de la camara: se lleva la caja objetivo a la que se va a usar
        if (a[4] == 0) { if (a[8]) { a[13] = x; a[14] = y; } else { a[10] = x; a[11] = y; } }
        a[16] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x10000u, 0xFFFF0000u, 3u});
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
