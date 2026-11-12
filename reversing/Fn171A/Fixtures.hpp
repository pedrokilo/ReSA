#pragma once
// Fixtures del banco Fn171A (lote 171A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 23 asignadas: 23 reconstruidas.
//   idx  direccion  nombre real                                                                      forma
//    0   0x632B30   CTaskSimpleWaitForPizza cuerpo del constructor (el listado decia ...WaitForBus)  thiscall() `ret`: base 61A390; [this]=86E1DCh; +8,+0Ch dwords y +10h,+11h bytes a 0; eax=this
//    1   0x632B50   CTaskSimpleWaitForPizza::GetTaskType (vtable 0x86E1DC, ranura 4)                  thiscall() `ret`: mov eax,0EFh
//    2   0x632B60   CTaskSimpleWaitForPizza::MakeAbortable (ranura 6)                                 thiscall(3 dwords) `ret 0Ch`: mov al,1
//    3   0x632B70   CTaskSimpleWaitForPizza cuerpo del destructor                                    thiscall() `ret`: [this]=86E1DCh; jmp 61A3A0
//    4   0x632B80   CTaskSimpleWaitForPizza::ProcessPed (ranura 7)                                    thiscall(ped) `ret 4`: temporizador de 2000 ms con el reloj 0xB7CB84
//    5   0x636B40   CTaskSimpleWaitForPizza::Clone (ranura 1)                                         thiscall() `ret`: new(14h) 61A5A0; constructor en linea (61A390 + vtable + campos); eax=memoria
//    6   0x4B86C0   CTaskSimpleOnEscalator::Clone (vtable 0x85B8DC; el listado decia StandStill)     thiscall() `ret`: new(20h); ctor StandStill 62F310 (0,1,0,8.0f); [mem]=85B8DCh; eax=memoria
//    7   0x4B8720   CTaskSimpleOnEscalator::GetTaskType (ranura 4)                                    thiscall() `ret`: mov eax,51Ch
//    8   0x4B8730   CTaskSimpleOnEscalator destructor con borrado (ranura 0)                          thiscall(flags) `ret 4`: call 4B8750; delete this si bit 0
//    9   0x4B8750   CTaskSimpleOnEscalator cuerpo del destructor                                      thiscall() `ret`: [this]=85B8DCh; jmp 62F360
//   10   0x62F520   CTaskSimpleOnEscalator::ProcessPed (ranura 7; el listado decia StandStill)        thiscall(ped) `ret 4`: 62F370; contacto (+584h) con modelo de escalon: atan 53CBE0, limite 53CB50, ped+55Ch
//   11   0x62F350   CTaskSimpleStandStill::GetTaskType (vtable 0x86DD2C, ranura 4)                    thiscall() `ret`: mov eax,0CBh
//   12   0x6490F0   CTaskComplexGoToBoatSteeringWheel::GetTaskType (vtable 0x86F018, ranura 4)        thiscall() `ret`: mov eax,33Eh
//   13   0x649160   CTaskComplexGoToBoatSteeringWheel::ComputeTargetPos                                thiscall() `ret`: model info del vehiculo; Multiply3x3 59C790; + posicion; this+0Ch
//   14   0x64E480   CTaskSimpleCreateCarAndGetIn destructor con borrado (vtable 0x86F070; el listado decia GoToBoat...)  thiscall(flags) `ret 4`: call 64CEA0; delete this si bit 0
//   15   0x64E4B0   CVehicleAnimGroupData::ComputeCriticalBlendTime (el listado decia GoToBoat...)     cdecl(grupo, anim): GetGroup 6E3B00; ComputeCriticalBlendTime 6E3C80 sobre 0xC1CDC0 + grupo*94h; float en ST0
//   16   0x68CD10   CTaskSimpleArrestPed::Clone (vtable 0x870984, ranura 1; el listado decia CopInCar)  thiscall() `ret`: new(14h); ctor 68B620 (ped de +8); eax = el del constructor
//   17   0x68CD70   CTaskComplexArrestPed::Clone (vtable 0x8709A8, ranura 1; el listado decia CopInCar)  thiscall() `ret`: new(24h); ctor 68B990 (ped de +10h); eax = el del constructor
//   18   0x68CF40   CTaskSimpleArrestPed destructor con borrado (ranura 0)                              thiscall(flags) `ret 4`: call 68B690; delete this si bit 0
//   19   0x68CF60   CTaskComplexArrestPed destructor con borrado (ranura 0)                             thiscall(flags) `ret 4`: call 68BA00; delete this si bit 0
//   20   0x6550C0   CTaskComplexWalkRoundObject::GetTaskType (vtable 0x86F364, ranura 4)               thiscall() `ret`: mov eax,1FEh
//   21   0x6550D0   CTaskComplexWalkRoundObject cuerpo del destructor (marco SEH)                        thiscall() `ret`: [this]=86F364h; 571A00 sobre +1Ch; 41B7D0 sobre +20h; 61A3D0
//   22   0x655290   CTaskComplexWalkRoundObject::CreateSubTask                                           thiscall(tipo, ped) `ret 8`: 0xCB new(20h)+62F310(500,0,0,8.0f); 0x389 655140(ped); otro 0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F171AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;
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
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
// Un float que no sea NaN (para evitar sumas con dos NaN: el x87 y el SSE eligen distinto)
inline u32 FloatNotNan(Gen& g) { u32 b; do { b = FloatBits(g); } while (IsNanBits(b)); return b; }

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kMakeAb, kPizzaCtor, kBody, kPizzaProc, kClonePizza, kCloneEsc, kSdd, kEscProc, kTargetPos, kCbt, kCloneCtor1, kWalkBody, kCreateSub };
inline constexpr std::array<Kind, kFunctions> kKind{kPizzaCtor, kConst, kMakeAb, kBody, kPizzaProc, kClonePizza, kCloneEsc, kConst, kSdd, kBody, kEscProc, kConst, kConst, kTargetPos, kSdd, kCbt,
                                                    kCloneCtor1, kCloneCtor1, kSdd, kSdd, kConst, kWalkBody, kCreateSub};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x632B30, 0x632B50, 0x632B60, 0x632B70, 0x632B80, 0x636B40, 0x4B86C0, 0x4B8720, 0x4B8730, 0x4B8750, 0x62F520, 0x62F350, 0x6490F0, 0x649160, 0x64E480, 0x64E4B0,
                                                    0x68CD10, 0x68CD70, 0x68CF40, 0x68CF60, 0x6550C0, 0x6550D0, 0x655290};
// Valor que devuelven las funciones constantes (por indice; 0 si no es constante)
inline constexpr std::array<u32, kFunctions> kConstValue{0, 0xEF, 0, 0, 0, 0, 0, 0x51C, 0, 0, 0, 0xCB, 0x33E, 0, 0, 0, 0, 0, 0, 0, 0x1FE, 0, 0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86E1DC, 0x86E1DC, 0x86E1DC, 0x86E1DC, 0x86E1DC, 0x86E1DC, 0x85B8DC, 0x85B8DC, 0x85B8DC, 0x85B8DC, 0x85B8DC, 0x86DD2C, 0x86F018, 0x86F018, 0x86F070, 0,
                                                     0x870984, 0x8709A8, 0x870984, 0x8709A8, 0x86F364, 0x86F364, 0x86F364};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x14, 4, 4, 4, 0x14, 0x14, 0x20, 4, 0x20, 0x20, 0x20, 4, 4, 0x28, 0x34, 4, 0x14, 0x24, 0x14, 0x24, 4, 0x54, 0x54};
// Un ped sintetico ocupa 0x5A0 bytes en la arena B (se leen +0x55C y +0x584), un vehiculo sintetico 0x60 y su contacto / matriz hasta 0x50
inline constexpr u32 kPedSpan = 0x5A0, kVehSpan = 0x60, kEntSpan = 0x50;
// Entrada de la tabla de model infos y de la tabla de grupos
inline constexpr u32 kModelTable = 0xA9B0C8, kModelCount = 20000;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D
//   kConst: nada mas
//   kMakeAb: a[4..6] = los tres dwords de pila
//   kPizzaCtor, kBody: a[4] = EAX del falso (constructor base / ~CTask / cuerpo de la clase base), a[5] = dword que el falso deja en [this]
//   kPizzaProc: a[4] = inicio (+8), a[5] = intervalo (+0Ch), a[6] = byte iniciado (+10h), a[7] = byte detenido (+11h), a[8] = reloj (0xB7CB84), a[9] = ped (pila, crudo), a[10] = bytes +12h y +13h
//   kClonePizza, kCloneEsc, kCloneCtor1: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = EAX del falso del constructor, a[7] = dword que el falso deja en [mem], a[8] = ped (campo +8 o +10h de this, crudo)
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
//   kEscProc: a[4] = desplazamiento del ped en B, a[5] = contacto (0 nulo, 1 en C), a[6] = desplazamiento en C, a[7] = dword en +20h del contacto (modelo = alto), a[8] y a[9] = los dos modelos de escalon (0x8CD674 / 0x8CD678),
//         a[10], a[11] = velocidad x, y del contacto (+44h, +48h), a[12] = EAX del falso de 62F370, a[13] = float que devuelve 53CBE0, a[14] = float que devuelve 53CB50, a[15] = dword previo en ped+55Ch
//   kTargetPos: a[4] = desplazamiento del vehiculo en B, a[5..7] = posicion del placement (+4..+0Ch), a[8] = matriz (0 nula, 1 en C), a[9] = desplazamiento en C, a[10..12] = posicion de la matriz (+30h..+38h),
//         a[13] = indice de modelo (< 20000), a[14] = dword en vehiculo+20h (los 16 bits altos se sustituyen por el indice), a[15] = desplazamiento del model info en D, a[16] = tipo de vehiculo (+3Ch),
//         a[17] = desplazamiento de la estructura en D, a[18..20] = dummy 0, a[21..23] = dummy 4, a[24..26] = lo que escribe el falso de Multiply3x3, a[27] = dword previo en vehiculo+14h (se sustituye por la matriz)
//   kCbt: a[4] = grupo (int32), a[5] = animacion, a[6] = EAX del falso de GetGroup, a[7] = float que devuelve el falso de ComputeCriticalBlendTime
//   kWalkBody: a[4] = objeto (0 nulo, 1 en B, 2 en C, 3 crudo), a[5] = desplazamiento (B o C), a[6] = objeto crudo, a[7] = ruta (0 nula, 1 en B), a[8] = desplazamiento en B, a[9] = EAX del falso de 61A3D0, a[10] = dword que deja en [this]
//   kCreateSub: a[4] = tipo (pila), a[5] = ped (pila, crudo), a[6] = memoria (0 nula, 1 en B), a[7] = desplazamiento en B, a[8] = EAX del falso del constructor 62F310, a[9] = dword que deja en [mem],
//         a[10] = EAX del falso de 655140
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    const auto mem = [&](u32 span) { c.a[4] = g.Pk({0u, 1u, 1u, 1u, 1u}); c.a[5] = AuxOffset(g, span); };
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConst:
        break;
    case kMakeAb:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kPizzaCtor:
    case kBody:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kPizzaProc: {
        const u32 start = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.W(), 1000u, 0xFFFFF830u});
        const u32 interval = g.Pk({2000u, 2000u, 0u, 1u, 0xFFFFFFFFu, 0x80000000u, 1999u, 2001u, g.W(), g.R(5000), g.R(100)});
        a[4] = start; a[5] = interval;
        a[6] = g.Pk({0u, 0u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        a[7] = g.Pk({0u, 0u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        const u32 end = start + interval;
        const u32 d = g.R(8);
        const u32 now = d == 0 ? end : (d == 1 ? end - 1 : (d == 2 ? end + 1 : (d == 3 ? g.W() : (d == 4 ? start : (d == 5 ? start + 2000u : (d == 6 ? 0xFFFFFFFFu : end + g.R(20)))))));
        a[8] = now;
        a[9] = Raw(g);
        a[10] = g.W() & 0xFFFFu;
        break;
    }
    case kClonePizza:
    case kCloneEsc:
    case kCloneCtor1:
        mem(c.fn == 5 ? 0x14 : (c.fn == 6 ? 0x20 : (c.fn == 16 ? 0x14 : 0x24)));
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[7] = Raw(g);
        a[8] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kEscProc:
        a[4] = AuxOffset(g, kPedSpan);
        a[5] = g.Pk({0u, 1u, 1u, 1u, 1u, 1u});
        a[6] = AuxOffset(g, kEntSpan);
        {
            const u32 m1 = g.Pk({0x1F4u, 0x1F5u, 0u, 0xFFFFu, 0x8000u, 0x7FFFu, g.R(20000), g.R(20000), 0x1F4u}), m2 = g.Pk({0x1F6u, 0x1F7u, 0u, 0xFFFFu, 0x8000u, g.R(20000), 0x1F6u});
            const u32 pick = g.R(6);
            a[8] = (g.W() & 0xFFFF0000u) | m1;
            a[9] = (g.W() & 0xFFFF0000u) | m2;
            const u32 model = pick == 0 ? m1 : (pick == 1 ? m2 : (pick == 2 ? (g.P(50) ? m1 : m2) : (pick == 3 ? g.R(65536) : (pick == 4 ? (m1 ^ 1u) & 0xFFFFu : g.R(20000)))));
            a[7] = (model << 16) | (g.W() & 0xFFFFu);
        }
        a[10] = FloatBits(g); a[11] = FloatBits(g);
        a[12] = Raw(g); a[13] = FloatBits(g); a[14] = FloatBits(g); a[15] = g.W();
        break;
    case kTargetPos: {
        a[4] = AuxOffset(g, kVehSpan);
        a[9] = AuxOffset(g, kEntSpan);
        a[8] = g.Pk({0u, 1u, 1u, 1u});
        a[13] = g.Pk({0u, 1u, 19999u, 400u, 500u, 595u, g.R(kModelCount), g.R(kModelCount), g.R(kModelCount), 0x4000u});
        a[14] = g.W();
        a[15] = 16u * g.R(0x40);
        a[16] = g.Pk({5u, 5u, 5u, 5u, 0u, 1u, 4u, 6u, 0xFFFFFFFFu, g.W(), g.R(10)});
        a[17] = 0x800u + 16u * g.R(0x40);
        for (u32 i = 18; i <= 23; ++i) a[i] = FloatBits(g);
        for (u32 i = 24; i <= 26; ++i) a[i] = FloatBits(g);
        // las sumas posicion + resultado de Multiply3x3 no deben tener dos NaN (x87 y SSE eligen distinto): placement y matriz sin NaN donde el resultado lo sea
        for (u32 i = 0; i < 3; ++i) {
            a[5 + i] = FloatBits(g);
            a[10 + i] = FloatBits(g);
            if (IsNanBits(a[24 + i])) { a[5 + i] = FloatNotNan(g); a[10 + i] = FloatNotNan(g); }
        }
        a[27] = g.W();
        break;
    }
    case kCbt:
        a[4] = g.Pk({0u, 1u, 2u, 29u, 30u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.R(30), g.R(30), g.W(), g.W(), 100u, 0x1000000u});
        a[5] = g.Pk({0u, 1u, 0x15Fu, 0x160u, 0x161u, 0x162u, 0x163u, 0x164u, 0x6Cu, 0x6Du, 0xFFFFFFFFu, g.R(0x200), g.R(0x200), g.W()});
        a[6] = Raw(g);
        a[7] = FloatBits(g);
        break;
    case kWalkBody:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 1u, 2u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = Raw(g);
        if (a[6] == 0) a[6] = 0x12345678u;
        a[7] = g.Pk({0u, 1u, 1u, 1u});
        a[8] = AuxOffset(g, 0x10);
        a[9] = Raw(g); a[10] = Raw(g);
        break;
    case kCreateSub:
        a[4] = g.Pk({0xCBu, 0xCBu, 0xCBu, 0xCBu, 0x389u, 0x389u, 0x389u, 0x516u, 0x516u, 0u, 1u, 0x387u, 0x386u, 0xFFFFFFFFu, 0x1CBu, 0x489u, 0x800000CBu, g.W(), g.R(0x800)});
        a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[7] = AuxOffset(g, 0x20);
        a[8] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[9] = Raw(g);
        a[10] = Raw(g);
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
