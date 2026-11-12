#pragma once
// Fixtures del banco Fn169A (lote 169A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; los nombres salen del desensamblado, de las vtables leidas del PE y de los llamadores, ver README.md y fixture_oracle.py). 23 asignadas; el listado atribuia varias a
// clases equivocadas (0x4B0F70 es CEventLeaderExitedCarAsDriver::TakesPriorityOver, 0x4AC0F0/0x4AC130/0x4AC160/0x4B2950 son de CInformFriendsEvent, 0x421190/0x4211A0/0x4211B0/0x61A620 de CTaskComplex,
// 0x61A660 es el destructor con borrado de CTask y 0x61A6F0 el cuerpo del destructor de CTaskSimpleAnim).
//   idx  direccion  nombre real                                                              forma
//    0   0x48E220   CEventLeaderEnteredCarAsDriver cuerpo del destructor (vtable 0x85A198)   thiscall() `ret`: [ecx]=85A198h, CleanUpOldReference(+14h) si no es nulo, call 4AC480
//    1   0x48E280   CEventLeaderEnteredCarAsDriver::GetEventType (ranura 1)                  thiscall() `ret`: mov eax,36h
//    2   0x48E290   CEventLeaderEnteredCarAsDriver::GetLifeTime (ranura 3)                   thiscall() `ret`: xor eax,eax
//    3   0x48E2A0   CEventLeaderEnteredCarAsDriver::CloneEditable (ranura 16)                thiscall() `ret`: CEvent::operator new 4B5620 (0x18) y constructor 48E1C0(+14h); null si no hay memoria
//    4   0x48E300   CEventLeaderEnteredCarAsDriver::AffectsPed (ranura 5)                    thiscall(ped) `ret 4`: xor al,al
//    5   0x48E310   CEventLeaderEnteredCarAsDriver::CanBeInterruptedBySameEvent (ranura 15)  thiscall() `ret`: mov al,1
//    6   0x48E320   CEventLeaderEnteredCarAsDriver::ScalarDeletingDestructor (ranura 0)      thiscall(flags) `ret 4`: cuerpo 48E220 (funcion 0), CEvent::operator delete 4B5630 (con hook)
//    7   0x48E340   CEventLeaderEnteredCarAsDriver::GetEventPriority (ranura 2)              thiscall() `ret`: mov eax,2Eh
//    8   0x4B0F70   CEventLeaderExitedCarAsDriver::TakesPriorityOver (vtable 0x85B898, ranura 11)  thiscall(evento) `ret 4`: mov al,1
//    9   0x4ABFE0   CEvent::AffectsPed (vtable 0x85AB38, ranura 5)                            thiscall(ped) `ret 4`: mov al,1
//   10   0x4ABFF0   CEvent::AffectsPedGroup (ranura 6)                                       thiscall(grupo) `ret 4`: mov al,1
//   11   0x4AC000   CEvent::CanBeInterruptedBySameEvent (ranura 15)                          thiscall() `ret`: xor al,al
//   12   0x4AC020   CEvent::IsValid (ranura 14)                                               thiscall(ped) `ret 4`: byte +8 o GetLifeTime virtual (ranura 3) frente a +4 (con signo)
//   13   0x4AC0F0   CInformFriendsEvent destructor de elemento (copia)                        thiscall() `ret`: CleanUpOldReference(&ped), delete evento (virtual), tiempo = -1
//   14   0x4AC130   CInformFriendsEvent constructor con argumentos                            thiscall(ped, evento, retardo) `ret 0Ch`: RegisterReference(&ped), tiempo = reloj + retardo
//   15   0x4AC160   CInformFriendsEvent::Process (un elemento)                                thiscall() `ret`: si ha vencido, CEventGroup::Add 4AB420 en el grupo del ped (+47Ch,+68h), limpia el ped, borra el evento
//   16   0x4B2950   CInformFriendsEvent destructor de elemento                                thiscall() `ret`: igual que 13
//   17   0x421190   CTaskComplex::GetSubTask (vtable 0x86D4CC, ranura 2)                      thiscall() `ret`: mov eax,[ecx+8]
//   18   0x4211A0   CTaskComplex::IsSimple (ranura 3)                                         thiscall() `ret`: xor al,al
//   19   0x4211B0   CTaskComplex::MakeAbortable (ranura 6)                                    thiscall(ped, prioridad, evento) `ret 0Ch`: salto a la ranura 6 de la vtable de la subtarea (+8)
//   20   0x61A620   CTaskComplex::ScalarDeletingDestructor (ranura 0)                        thiscall(flags) `ret 4`: cuerpo 61A3D0 (con hook); con bit 0 liberacion EN LINEA del pool de tareas 0xB744A8
//   21   0x61A660   CTask::ScalarDeletingDestructor (vtable 0x86D48C, ranura 0)               thiscall(flags) `ret 4`: [ecx]=86D48Ch; con bit 0 liberacion EN LINEA del pool de tareas
//   22   0x61A6F0   CTaskSimpleAnim cuerpo del destructor (vtable 0x86D504)                     thiscall() `ret`: SetFinishCallback 4CEBE0, bit 4 de +2Eh, velocidad -4.0, ~CTask 61A3A0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F169AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;

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
    std::array<u32, 24> a{};
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

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un dword de 32 bits como argumento "float" de SetUp: patrones con NaN (silenciosos y senalizantes), infinitos, ceros con signo, denormales, extremos y aleatorios
inline u32 FloatBitsOf(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu, w6 = 0x3F800000u + (g.W() & 0x7FFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFFFFFFFFu, w1, w2, w3, w4, w5, w6});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kConstAL0, kConstAL1, kEntBody, kClone, kSdd, kSddPool, kIsValid, kFlush, kSet, kProc, kRead8, kMakeAbort, kAnimDtor };
inline constexpr std::array<Kind, kFunctions> kKind{
    kEntBody, kConst, kConst, kClone, kConstAL1, kConstAL0, kSdd, kConst, kConstAL1, kConstAL1, kConstAL1, kConstAL0, kIsValid, kFlush, kSet, kProc, kFlush, kRead8, kConstAL0, kMakeAbort, kSddPool, kSddPool, kAnimDtor};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x48E220, 0x48E280, 0x48E290, 0x48E2A0, 0x48E300, 0x48E310, 0x48E320, 0x48E340, 0x4B0F70, 0x4ABFE0, 0x4ABFF0, 0x4AC000, 0x4AC020, 0x4AC0F0, 0x4AC130, 0x4AC160,
                                                    0x4B2950, 0x421190, 0x4211A0, 0x4211B0, 0x61A620, 0x61A660, 0x61A6F0};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x4AC480, 0, 0, 0, 0, 0, 0x48E220, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x61A3D0, 0, 0x61A3A0};
// Funcion de liberacion de los destructores con borrado (CEvent::operator delete); 0 = ninguna (la de tareas es en linea)
inline constexpr std::array<u32, kFunctions> kDelete{0, 0, 0, 0, 0, 0, 0x4B5630, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x85A198, 0, 0, 0x85A198, 0, 0, 0x85A198, 0, 0, 0, 0, 0, 0x85AB38, 0, 0, 0, 0, 0, 0, 0, 0x86D4CC, 0x86D48C, 0x86D504};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x18, 4, 4, 0x18, 4, 4, 4, 4, 4, 4, 4, 4, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 4, 0xC, 4, 4, 0x10};
inline constexpr u32 kTaskPoolPtr = 0xB744A8;                  // puntero del pool de tareas (global del EXE)
inline constexpr u32 kGlobalTimer = 0xB7CB84;                  // reloj de juego (CTimer::m_snTimeInMilliseconds)
inline constexpr u32 kEvLife = 0x7E1EA5E0u;                    // evento del GetLifeTime falso (sin direccion de EXE propia)
inline constexpr u32 kEvDtor = 0x7E1EA5E1u;                    // evento del destructor virtual con borrado falso de un evento
inline constexpr u32 kEvAbort = 0x7E1EA5E2u;                   // evento del MakeAbortable virtual falso de la subtarea
inline constexpr u32 kEvDefaultCb = 0x4D1390;                  // CDefaultAnimCallback::DefaultAnimCB del EXE (el falso de SetFinishCallback normaliza la direccion de la DLL a esta)
inline constexpr u32 kFakeVtable = 0xF00;                      // desplazamiento (arena C) de la vtable falsa de los eventos y subtareas sinteticos

inline bool IsNan(u32 w) { return (w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu) != 0; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; a[16..23] = datos extra
//   kConst, kConstAL0, kConstAL1: a[3] = dword en [this] (no se lee); kConstAL1: a[4] = argumento de pila (no se lee)
//   kEntBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = modo del vehiculo (+14h: 0 nulo, 1 en B a[7], 2 crudo a[8])
//   kClone: a[3] = vehiculo (+14h) crudo; a[4] = 0 crudo / 1 puntero a la arena C (a[5]); a[6] = 0 sin memoria (operator new devuelve nulo) / 1 objeto en B (a[7]); a[8] = dword que el constructor falso deja en el objeto
//   kSdd, kSddPool: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas; kSddPool ademas a[7..11]: d = objeto - almacenamiento
//         (int32); cursor del pool (int32); desplazamiento de la celda de estados en C; desplazamiento del pool falso en B; dword en pool+8
//   kIsValid: this en A (vtable falsa en C a[7]); a[3] = valor de GetLifeTime (int32); a[4] = m_nTimeActive (+4); a[5] = dword en +8 (el EXE prueba solo el byte bajo); a[6] = argumento de pila (el ped, no se lee)
//   kFlush: a[3] = modo del ped (+0: 0 nulo, 1 en B a[4], 2 crudo a[5]); a[6] = modo del evento (+4: 0 nulo, 1 objeto en C a[7] con la vtable falsa); a[8] = dword previo del instante (+8); a[9] = gRet
//   kSet: a[3] = modo del ped (como kFlush; argumento 1); a[6] = evento (argumento 2, crudo); a[7] = retardo (argumento 3); a[8] = reloj de juego (0xB7CB84); a[9] = dword previo del instante (+8)
//   kProc: a[3] = modo del ped (0 nulo, 1 ped en B a[4] con inteligencia en C a[5]); a[6] = modo del evento (0 nulo, 1 objeto en C a[7]); a[8] = instante (+8); a[9] = reloj de juego; a[10] = gRet del Add falso
//   kRead8: a[3] = dword en +8
//   kMakeAbort: this en A; subtarea en C a[6] (+8 de this) con la vtable falsa (ranura 6 = MakeAbortable falso); a[3], a[4], a[5] = ped, prioridad y evento (dwords crudos de la pila); a[7] = EAX que devuelve el falso
//   kAnimDtor: a[3] = dword previo en [this]; a[4] = dword en +0xC (el bit 2 del byte bajo es m_bHoldLastFrame); a[5], a[11] = gDtorWrite, gRet; a[6] = modo de la animacion (+8: 0 nula, 1 en B a[7]); a[8] = bits de m_BlendAmount
//         (+0x18); a[9] = bits de m_BlendDelta (+0x1C); a[10] = dword en +0x2C (la palabra +0x2E es la alta)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    const auto poolParams = [&](u32 first) {   // d, cursor, celda de estados en C, pool en B, dword en +8 (elementos de 0x80 bytes: indice = d >> 7, desplazamiento aritmetico)
        const u32 index = g.R(0x500);
        const u32 d = g.Pk({0u, 0x80u, 0x7Fu, 0x81u, 0x100u, u32(-0x80), u32(-1), u32(-0x7F), u32(-0x81), u32(0x80 * index + g.R(0x80)), u32(-(0x80 * int(index) + int(g.R(0x80)))), 0x80u * 0x7FFu, g.R(0x80), u32(-int(g.R(0x80)))});
        const i32 idx = i32(d) >> 7;
        c.a[first] = d;
        c.a[first + 1] = g.Pk({0u, u32(-1), 0x7FFFFFFFu, 0x80000000u, u32(idx - 1), u32(idx), u32(idx + 1), g.R(0x500), g.W(), u32(idx) + 1u, u32(idx) - 1u});
        c.a[first + 2] = Offset(g, g.R(4), 4);
        c.a[first + 3] = Offset(g, g.R(4), 0x10);
        c.a[first + 4] = g.W();
    };
    const auto pedMode = [&] { return g.Pk({0u, 1u, 1u, 2u}); };   // 0 nulo, 1 en la arena B, 2 crudo
    switch (kKind[c.fn]) {
    case kConst: case kConstAL0:
        c.a[3] = Raw(g);
        break;
    case kConstAL1:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kEntBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 2u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Raw(g);
        break;
    case kClone:
        c.a[3] = Raw(g);
        c.a[4] = g.P(35) ? 1u : 0u;
        c.a[5] = Offset(g, g.R(4), 4);
        c.a[6] = g.P(80) ? 1u : 0u;
        c.a[7] = Offset(g, g.R(4), 0x18);
        c.a[8] = Raw(g);
        break;
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kSddPool:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        poolParams(7);
        break;
    case kIsValid:
        c.a[3] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.R(100), u32(-int(g.R(100))), g.W()});
        c.a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, c.a[3], c.a[3] + 1u, c.a[3] - 1u, g.R(100), u32(-int(g.R(100))), g.W()});
        c.a[5] = g.Pk({0u, 1u, 2u, 0x100u, 0x80u, 0xFFu, 0x01000000u, 0xFFFFFF00u, 0xFFFFFFFFu, g.W(), g.W()});
        c.a[6] = Raw(g);
        c.a[7] = Offset(g, g.R(4), 0x10);
        break;
    case kFlush:
        c.a[3] = pedMode();
        c.a[4] = Offset(g, g.R(4), 4);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u});
        c.a[7] = 4 * g.R(0x100);
        c.a[8] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, g.W(), g.W()});
        c.a[9] = Raw(g);
        break;
    case kSet:
        c.a[3] = pedMode();
        c.a[4] = Offset(g, g.R(4), 4);
        c.a[5] = Raw(g);
        c.a[6] = Raw(g);
        c.a[7] = g.Pk({0u, 1u, 300u, 799u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.R(2000), g.W(), u32(-int(g.R(2000)))});
        c.a[8] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.R(0x100000), g.W(), g.W()});
        c.a[9] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        break;
    case kProc: {
        c.a[3] = g.Pk({0u, 1u, 1u, 1u});
        c.a[4] = 4 * g.R(0x200);
        c.a[5] = 0x400 + 4 * g.R(0x80);
        c.a[6] = g.Pk({0u, 1u, 1u});
        c.a[7] = 4 * g.R(0x100);
        c.a[9] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.R(0x100000), g.W(), g.W()});
        const u32 now = c.a[9];
        // el instante frente al reloj (comparacion SIN signo): vencido, justo en el limite, futuro, extremos
        c.a[8] = g.Pk({now, now - 1u, now + 1u, 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, now - 1000u, now + 1000u, g.W(), g.W(), 0xFFFFFFFFu});
        c.a[10] = Raw(g);
        break;
    }
    case kRead8:
        c.a[3] = Raw(g);
        break;
    case kMakeAbort:
        c.a[3] = Raw(g);
        c.a[4] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, g.W(), Raw(g)});
        c.a[5] = Raw(g);
        c.a[6] = 4 * g.R(0x100);
        c.a[7] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, g.W(), g.W(), 2u});
        break;
    case kAnimDtor: {
        c.a[3] = prev();
        c.a[4] = g.Pk({0u, 4u, 0xFFFFFFFBu, 0xFFFFFFFFu, g.W() | 4u, g.W() & ~4u, g.W(), g.W() | 4u, g.W() & ~4u, 1u, 2u, 0x100u});
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 1u});
        c.a[7] = Offset(g, g.R(4), 0x30);
        const bool positive = g.P(40);
        c.a[8] = positive ? g.Pk({0x3F800000u, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0x7F800000u, 0x3F000000u, 0x40000000u, g.W() & 0x7FFFFFFFu}) : FloatBitsOf(g);
        c.a[9] = positive && g.P(70) ? g.Pk({0u, 0x80000000u, 0x3F800000u, 0x00000001u, 0x7F800000u, 0xC0800000u, g.W() & 0x7FFFFFFFu, g.W()}) : FloatBitsOf(g);
        c.a[10] = g.W();
        c.a[11] = Raw(g);
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
