#pragma once
// Fixtures del banco Fn197A (lote 197A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 32 asignadas: 32 reconstruidas.
//   idx  direccion  nombre real                                                                         forma
//    0   0x5FF490   CEventHealthLow::CloneEditable (vtable 0x86CBA0, ranura 16)                       thiscall() `ret`: CEvent::operator new 4B5620 (0x14); ctor 4AC450; [mem]=86CBA0h; eax=mem
//    1   0x5FF4E0   CEventHealthLow destructor con borrado (ranura 0)                                  thiscall(flags) `ret 4`: cuerpo 5FF460 (SIN hook en este lote: codigo del EXE); operator delete 4B5630 si bit 0
//    2   0x5FF500   CEventHealthReallyLow::Constructor (vtable 0x86CBE8)                              thiscall() `ret`: ctor 4AC450; [this]=86CBE8h; eax=this
//    3   0x5FF520   CEventHealthReallyLow cuerpo del destructor                                       thiscall() `ret`: [this]=86CBE8h; jmp 4AC480
//    4   0x5FF550   CEventHealthReallyLow::CloneEditable (ranura 16)                                   como 0 con vtable 86CBE8h
//    5   0x5FF5A0   CEventHealthReallyLow destructor con borrado                                       como 1 con el cuerpo 5FF520
//    6   0x5FF5C0   CEventLowAngerAtPlayer::Constructor (vtable 0x86CC30)                               como 2 con 86CC30h
//    7   0x5FF5E0   CEventLowAngerAtPlayer cuerpo del destructor                                        como 3 con 86CC30h
//    8   0x5FF610   CEventLowAngerAtPlayer::CloneEditable                                              como 0 con 86CC30h
//    9   0x5FF660   CEventLowAngerAtPlayer destructor con borrado                                       como 1 con el cuerpo 5FF5E0
//   10   0x5FF680   CEventHighAngerAtPlayer::Constructor (vtable 0x86CC78)                              como 2 con 86CC78h
//   11   0x5FF6A0   CEventHighAngerAtPlayer cuerpo del destructor                                       como 3 con 86CC78h
//   12   0x5FF6D0   CEventHighAngerAtPlayer::CloneEditable                                             como 0 con 86CC78h
//   13   0x5FF720   CEventHighAngerAtPlayer destructor con borrado                                      como 1 con el cuerpo 5FF6A0
//   14   0x5FF740   CEventOnFire::Constructor (vtable 0x86CCC0)                                        thiscall() `ret`: ctor de CEvent 4ABFC0; [this]=86CCC0h; eax=this
//   15   0x5FF760   CEventOnFire cuerpo del destructor                                                 thiscall() `ret`: [this]=86CCC0h; jmp 4AC010
//   16   0x5FF770   CEventOnFire::GetEventType (ranura 1)                                              thiscall() `ret`: mov eax,3Ah
//   17   0x5FF780   CEventOnFire::GetLifeTime (ranura 3)                                               thiscall() `ret`: xor eax,eax
//   18   0x5FF790   CEventOnFire::Clone (ranura 4)                                                     thiscall() `ret`: new 4B5620 (0xC); ctor 4ABFC0; [mem]=86CCC0h; eax=mem
//   19   0x5FF7E0   CEventOnFire::GetLocalSoundLevel (ranura 12)                                        thiscall() `ret`: fld dword [858B34h] = 60.0f (resultado en ST0)
//   20   0x5FF7F0   CEventOnFire destructor con borrado                                                 como 1 con el cuerpo 5FF760
//   21   0x5FF820   CEventEscalator::Constructor (vtable 0x86CD00)                                     thiscall() `ret`: ctor de CEvent 4ABFC0; [this]=86CD00h; eax=this
//   22   0x5FF840   CEventEscalator::GetEventType (ranura 1)                                            thiscall() `ret`: mov eax,4Ah
//   23   0x5FF850   CEventEscalator::GetLifeTime (ranura 3)                                             thiscall() `ret`: xor eax,eax
//   24   0x5FF860   CEventEscalator cuerpo del destructor                                               `jmp 4AC010` puro (sin escribir la vtable)
//   25   0x5FF870   CEventEscalator::GetEventPriority (ranura 2)                                        thiscall() `ret`: mov eax,38h
//   26   0x5FF880   CEventSeenCop::GetEventPriority (ranura 2 de 0x86CB58)                               thiscall() `ret`: mov eax,15h
//   27   0x5FF890   CEventOnFire::GetEventPriority (ranura 2)                                           thiscall() `ret`: mov eax,42h
//   28   0x5FF8A0   CEventAcquaintancePedHateBadlyLit::GetEventPriority (ranura 2 de 0x86CB10)          thiscall() `ret`: mov eax,19h
//   29   0x632190   ShouldLoadSunbatheAnims (funcion libre)                                             cdecl() `ret`: peaton del jugador en foco; si va en vehiculo: (x*x+y*y)+z*z (x87 a 24 bits) > 0x3D23D70B => AL=0; si no AL=1
//   30   0x638290   CTaskComplexSunbathe::CreateSubTask                                                 thiscall(tipo, ped) `ret 8`: subtarea de ese tipo (pausa 48E750, animacion temporizada 61ABE0, animacion 61A900)
//   31   0x638570   CTaskComplexUseEffect destructor con borrado (vtable 0x86E0D8; el listado decia Sunbathe) como 1 con el cuerpo 632270 y CTask::operator delete 61A5B0
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

namespace F197AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 32;
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
enum Kind : u32 { kConst, kConstF, kCtor, kBody, kClone, kSdd, kShould, kCreateSub };
inline constexpr std::array<Kind, kFunctions> kKind{kClone, kSdd, kCtor, kBody, kClone, kSdd, kCtor, kBody, kClone, kSdd, kCtor, kBody, kClone, kSdd, kCtor, kBody,
                                                    kConst, kConst, kClone, kConstF, kSdd, kCtor, kConst, kConst, kBody, kConst, kConst, kConst, kConst, kShould, kCreateSub, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x5FF490, 0x5FF4E0, 0x5FF500, 0x5FF520, 0x5FF550, 0x5FF5A0, 0x5FF5C0, 0x5FF5E0, 0x5FF610, 0x5FF660, 0x5FF680, 0x5FF6A0, 0x5FF6D0, 0x5FF720, 0x5FF740, 0x5FF760,
                                                    0x5FF770, 0x5FF780, 0x5FF790, 0x5FF7E0, 0x5FF7F0, 0x5FF820, 0x5FF840, 0x5FF850, 0x5FF860, 0x5FF870, 0x5FF880, 0x5FF890, 0x5FF8A0, 0x632190, 0x638290, 0x638570};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86CBA0, 0x86CBA0, 0x86CBE8, 0x86CBE8, 0x86CBE8, 0x86CBE8, 0x86CC30, 0x86CC30, 0x86CC30, 0x86CC30, 0x86CC78, 0x86CC78, 0x86CC78, 0x86CC78, 0x86CCC0, 0x86CCC0,
                                                     0x86CCC0, 0x86CCC0, 0x86CCC0, 0x86CCC0, 0x86CCC0, 0x86CD00, 0x86CD00, 0x86CD00, 0x86CD00, 0x86CD00, 0x86CB58, 0x86CCC0, 0x86CB10, 0, 0x86E0AC, 0x86E0D8};
// Bytes que cada funcion toca a partir de su puntero `this` (el CreateSubTask lee hasta +0x2C)
inline constexpr std::array<u32, kFunctions> kSpan{0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0xC, 0xC, 4, 4, 0xC, 4, 0xC, 0xC, 4, 4, 0xC, 4, 4, 4, 4, 4, 0x38, 0x34};
// Un ped sintetico ocupa 0x5A0 bytes (se leen +0x46C y +0x58C), un vehiculo sintetico 0x50 (se leen +0x44, +0x48 y +0x4C) y un bloque de animaciones 0x20 (se lee +0x10)
inline constexpr u32 kPedSpan = 0x5A0, kVehSpan = 0x50, kBlockSpan = 0x20, kSubSpan = 0x20;
// Memoria global del juego: byte del jugador en foco, y la tabla de jugadores (2 x 0x190, el primer dword es el CPed*)
inline constexpr u32 kFocusAddr = 0xB7CD74, kPlayersAddr = 0xB7CD98, kPlayerStride = 0x190, kPlayerCount = 2;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D
//   kConst, kConstF: nada mas
//   kCtor, kBody: a[4] = EAX del falso (constructor base / cuerpo de la clase base), a[5] = dword que el falso deja en [this]
//   kClone: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = EAX del falso del constructor, a[7] = dword que el falso deja en [mem]
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
//   kShould: a[4] = jugador en foco (0 o 1), a[5] = desplazamiento del ped 0 en B, a[6] = banderas del ped 0 (+46Ch), a[7] = vehiculo del ped 0 (0 nulo, 1 en C), a[8] = desplazamiento del vehiculo 0 en C,
//         a[9..11] = velocidad x, y, z del vehiculo 0; a[12] = desplazamiento del ped 1 en D, a[13] = banderas del ped 1, a[14] = vehiculo del ped 1 (0 nulo, 1 en C), a[15] = desplazamiento del vehiculo 1 en C
//         (siempre en la segunda mitad), a[16..18] = velocidad del vehiculo 1
//   kCreateSub: a[4] = tipo (pila), a[5] = ped (pila, crudo), a[6] = subtarea (0 nula, 1 en B), a[7] = desplazamiento en B, a[8] = lo que devuelve su GetTaskType (ranura 4), a[9] = tipo de banista (+20h),
//         a[10] = byte "cargado" del bloque de playa (+10h del bloque de +24h), a[11] = desplazamiento de ese bloque en C, a[12] = byte "cargado" del bloque de tomar el sol (+28h), a[13] = su desplazamiento en C
//         (segunda mitad), a[14] = memoria que devuelve CTask::operator new (0 nula, 1 en D), a[15] = desplazamiento en D, a[16] = EAX del falso del constructor, a[17] = dword que deja en [mem],
//         a[18] = valor del falso del azar (0x407180), a[19] y a[20] = lo que devuelve la primera y la segunda llamada a GetTimeLeft (0x62EB80), a[21] = EAX del falso de ShouldLoadSunbatheAnims
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
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConst:
    case kConstF:
        break;
    case kCtor:
    case kBody:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u}); a[5] = AuxOffset(g, kSpan[c.fn]);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[7] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kShould: {
        a[4] = g.Pk({0u, 0u, 1u, 1u, 0u, 1u});
        a[5] = AuxOffset(g, kPedSpan);
        a[12] = AuxOffset(g, kPedSpan);
        const auto flags = [&] { return g.Pk({0u, 0x100u, 0x100u, 0x100u, 0xFFFFFFFFu, g.W(), g.W() | 0x100u, g.W() & ~0x100u, 0xFFFFFEFFu, 0x80u, 0x101u, 0x200u}); };
        a[6] = flags(); a[13] = flags();
        a[7] = g.Pk({0u, 1u, 1u, 1u, 1u}); a[14] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[8] = 4u * g.R((0x800u - kVehSpan) / 4u); a[15] = 0x800u + 4u * g.R((0x800u - kVehSpan) / 4u);
        // velocidades: la frontera del umbral 0.04 (0.2f al cuadrado), ceros, signos, infinitos, NaN, denormales, magnitudes enormes y diminutas
        const auto speed = [&](u32* s) {
            const u32 mode = g.R(12);
            for (u32 i = 0; i < 3; ++i) s[i] = FloatBits(g);
            if (mode == 0) { s[0] = 0x3E4CCCCDu; s[1] = 0; s[2] = 0; }
            else if (mode == 1) { s[0] = 0x3E4CCCCCu + g.R(3); s[1] = g.P(50) ? 0u : 0x80000000u; s[2] = 0; }
            else if (mode == 2) { s[0] = 0x3DCCCCCDu + g.R(5) - 2u; s[1] = 0x3DCCCCCDu + g.R(5) - 2u; s[2] = 0x3DCCCCCDu + g.R(5) - 2u; }
            else if (mode == 3) { s[0] = 0x3E1D3F5Eu + g.R(9) - 4u; s[1] = 0x3E1D3F5Eu + g.R(9) - 4u; s[2] = 0; }
            else if (mode == 4) { s[0] = g.W() & 0x007FFFFFu; s[1] = g.W() & 0x807FFFFFu; s[2] = g.W() & 0x007FFFFFu; }
            else if (mode == 5) { s[0] = 0x3C000000u + (g.W() & 0x7FFFFFu); s[1] = 0x3C000000u + (g.W() & 0x7FFFFFu); s[2] = 0x3C000000u + (g.W() & 0x7FFFFFu); }
            else if (mode == 6) { s[g.R(3)] = 0x7F800000u | (g.P(50) ? 0x80000000u : 0u); }
            // 7..11: valores de FloatBits tal cual (incluye NaN, senalizantes y con cargas distintas)
        };
        speed(&a[9]); speed(&a[16]);
        break;
    }
    case kCreateSub: {
        a[4] = g.Pk({0xCAu, 0xCAu, 0xCAu, 0xCAu, 0x1A2u, 0x1A2u, 0x1A2u, 0x1A2u, 0x1ACu, 0x1ACu, 0x1ADu, 0x1ADu, 0x1ADu, 0x1AEu, 0x1AEu, 0x516u, 0x516u, 0u, 1u, 0x1AFu, 0x1ABu, 0xC9u, 0xCBu, 0x1A1u,
                     0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x800001ADu, 0x100000CAu, g.W(), g.R(0x800)});
        a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u, 1u});
        a[7] = AuxOffset(g, kSubSpan);
        a[8] = g.P(35) ? a[4] : g.Pk({0u, 0xCAu, 0x1A2u, 0x1ACu, 0x1ADu, 0x1AEu, 0x516u, 0xFFFFFFFFu, g.W(), g.R(0x800)});
        a[9] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 0xFFFFFFFFu, 0x80000000u, 4u, g.W(), g.R(8)});
        a[10] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256), 0x100u});
        a[12] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256), 0x100u});
        a[11] = 4u * g.R((0x800u - kBlockSpan) / 4u); a[13] = 0x800u + 4u * g.R((0x800u - kBlockSpan) / 4u);
        a[14] = g.Pk({0u, 1u, 1u, 1u, 1u}); a[15] = AuxOffset(g, 0x38);
        a[16] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u, 0x80000000u});
        a[17] = Raw(g);
        // valor que devuelve rand() (el EXE y la DLL solo usan los 16 bits bajos): ceros, maximos, bits altos puestos y aleatorios
        a[18] = g.Pk({0u, 1u, 2u, 3u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 0xFFFF0000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.W(), g.R(0x8000), g.R(0x8000), g.R(0x10000)});
        // GetTimeLeft: la primera llamada puede ser cualquier dword (extremos del cuadro de 32 bits); la segunda fija el tope del azar (3000..tope), asi que se limita a magnitudes plausibles (el producto
        // rand * 2^-15 * (tope - 3000) debe caber en un entero de 32 bits: fuera de eso el _ftol del EXE y el cvttss2si de la DLL no coinciden y no es algo que el juego produzca)
        const auto tl1 = [&] { return g.Pk({0u, 1u, 0xFFFFFFFFu, 11000u, 11001u, 10999u, 12000u, 0x7FFFFFFFu, 0x7FFFFC17u, 0x7FFFFC18u, 0x80000000u, 0xFFFFFC18u, 0xFFFFFC17u, 20000u, g.W(), g.R(13000), g.R(100)}); };
        const auto tl2 = [&] { return g.Pk({0u, 1u, 0xFFFFFFFFu, 11000u, 11001u, 10999u, 12000u, 2000u, 2001u, 0xFFFFFC18u, 0xFFFFFC17u, 0xFFFFF448u, 0xFFFFF000u, 20000u, 100000u, g.R(13000), g.R(100)}); };
        a[19] = tl1(); a[20] = tl2();
        a[21] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFFFFu, g.W(), 2u});
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
