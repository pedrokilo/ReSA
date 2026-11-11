#pragma once
// Fixtures del banco Fn157A (lote 157A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables leidas del PE y de los llamadores, ver README.md y fixture_oracle.py). 24 asignadas: ninguna tenia hook
// (se comprobo el codigo, docs/hooks.csv y git worktree list). Los nombres del listado eran en su mayoria erroneos (p. ej. 0x65D240..0x65D2C0 no son CTaskComplexFleePoint sino cinco destructores con borrado de
// tareas de huida distintas; 0x6801D0 es CTaskSimpleJetPack y no CTaskSimpleClimb).
//   idx  direccion  nombre real                                                              forma
//    0   0x65D240   CTaskComplexFleeEntity::ScalarDeletingDestructor (vtable 0x86F708)        thiscall(flags) `ret 4`: cuerpo 65B9D0 (con hook), CTask::operator delete 61A5B0
//    1   0x65D260   CTaskComplexSmartFleePoint::ScalarDeletingDestructor (0x86F744)           cuerpo 65BDB0 (con hook)
//    2   0x65D280   CTaskComplexSmartFleeEntity::ScalarDeletingDestructor (0x86F784)          cuerpo 65C4D0 (con hook)
//    3   0x65D2A0   CTaskComplexFleeShooting::ScalarDeletingDestructor (0x86F7C4)             cuerpo 65C9A0 (con hook)
//    4   0x65D2C0   CTaskComplexFleeAnyMeans::ScalarDeletingDestructor (0x86F7F0)             cuerpo 65CD10 (con hook)
//    5   0x637F60   CTaskComplexObserveTrafficLights::ScalarDeletingDestructor (0x86DF98)    cuerpo 6317C0 (con hook)
//    6   0x637F80   CTaskComplexObserveTrafficLightsAndAchieveHeading::ScalarDeletingDestructor (0x86DFC4)   cuerpo 631980 (SIN hook: el destructor generado de la DLL)
//    7   0x637FA0   CTaskComplexCrossRoadLookAndAchieveHeading::ScalarDeletingDestructor (0x86DFF0)  cuerpo 631B50 (con hook)
//    8   0x637FC0   CTaskSimpleTurn180::ScalarDeletingDestructor (0x86E01C)                   cuerpo 631D00 (con hook)
//    9   0x638020   CTaskComplexHitResponse::ScalarDeletingDestructor (0x86E040)              cuerpo 631DA0 (con hook)
//   10   0x639490   CTaskSimpleIKLookAt::ScalarDeletingDestructor (0x86E37C)                  cuerpo 633EF0 (con hook)
//   11   0x6394B0   CTaskSimpleIKPointArm::ScalarDeletingDestructor (0x86E3A4)                cuerpo 634240 (con hook)
//   12   0x6394D0   CTaskSimpleTriggerLookAt::ScalarDeletingDestructor (0x86E3CC)             cuerpo 634500 (SIN hook: la DLL llama a la direccion del EXE)
//   13   0x6394F0   CTaskSimpleClearLookAt::ScalarDeletingDestructor (0x86E404)               cuerpo 6346A0 (con hook)
//   14   0x639510   CTaskComplexUseClosestFreeScriptedAttractor::ScalarDeletingDestructor (0x86E428)   cuerpo 634720 (con hook)
//   15   0x5F6360   CEventPlayerCommandToGroupAttack::GetEventType (vtable 0x86C710, ranura 1)  thiscall() `ret`: mov eax,47h
//   16   0x5F6400   CEventPlayerCommandToGroupAttack::GetEventPriority (ranura 2)              thiscall() `ret`: mov eax,2Ch
//   17   0x5F6450   CPedGroupDefaultTaskAllocatorFollowAnyMeans::Constructor (vtable 0x86C75C)   thiscall() `ret`: mov eax,ecx; mov [eax],86C75Ch
//   18   0x5F63F0   CEventPlayerCommandToGroupAttack cuerpo del destructor                       thiscall() `ret`: [ecx]=86C710h; jmp 4B2470 (cuerpo de ~CEventPlayerCommandToGroup, con hook)
//   19   0x5F63D0   CEventPlayerCommandToGroupAttack::ScalarDeletingDestructor (ranura 0)       thiscall(flags) `ret 4`: cuerpo 5F63F0 (esta funcion 18), CEvent::operator delete 4B5630 (con hook)
//   20   0x5F6370   CEventPlayerCommandToGroupAttack::CloneEditable (ranura 16, marco SEH)     thiscall() `ret`: CEvent::operator new 4B5620 (0x1C) y constructor 4B23D0(0, [this+18h]); [obj]=86C710h; null si no hay memoria
//   21   0x67A1C0   CTaskSimpleClimb::GetTaskType (vtable 0x87059C, ranura 4)                  thiscall() `ret`: mov eax,0FEh
//   22   0x6801D0   CTaskSimpleJetPack::ScalarDeletingDestructor (0x8705C4)                    cuerpo 67E600 (con hook)
//   23   0x67A1D0   CTaskSimpleClimb cuerpo del destructor (marco SEH)                          thiscall() `ret`: [esi]=87059Ch; CleanUpOldReference(+24h); SetDeleteCallback(+2Ch) y [+2Ch]=0; ~CTask 61A3A0
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

namespace F157AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 24;

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

// Clases de funcion (forma)
enum Kind : u32 { kConst, kSdd, kCtor, kEvBody, kClone, kClimbBody };
inline constexpr std::array<Kind, kFunctions> kKind{
    kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd,   // 0..14 destructores con borrado de tareas
    kConst, kConst, kCtor, kEvBody, kSdd, kClone, kConst, kSdd, kClimbBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x65D240, 0x65D260, 0x65D280, 0x65D2A0, 0x65D2C0, 0x637F60, 0x637F80, 0x637FA0, 0x637FC0, 0x638020, 0x639490, 0x6394B0, 0x6394D0, 0x6394F0, 0x639510,
                                                    0x5F6360, 0x5F6400, 0x5F6450, 0x5F63F0, 0x5F63D0, 0x5F6370, 0x67A1C0, 0x6801D0, 0x67A1D0};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x65B9D0, 0x65BDB0, 0x65C4D0, 0x65C9A0, 0x65CD10, 0x6317C0, 0x631980, 0x631B50, 0x631D00, 0x631DA0, 0x633EF0, 0x634240, 0x634500, 0x6346A0, 0x634720,
                                                      0, 0, 0, 0x4B2470, 0x5F63F0, 0, 0, 0x67E600, 0x61A3A0};
// Funcion de liberacion de los destructores con borrado (CTask::operator delete, CEvent::operator delete); 0 = ninguna
inline constexpr std::array<u32, kFunctions> kDelete{0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0,
                                                     0, 0, 0, 0, 0x4B5630, 0, 0, 0x61A5B0, 0};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86F708, 0x86F744, 0x86F784, 0x86F7C4, 0x86F7F0, 0x86DF98, 0x86DFC4, 0x86DFF0, 0x86E01C, 0x86E040, 0x86E37C, 0x86E3A4, 0x86E3CC, 0x86E404, 0x86E428,
                                                     0x86C710, 0x86C710, 0x86C75C, 0x86C710, 0x86C710, 0x86C710, 0x87059C, 0x8705C4, 0x87059C};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x1C, 4, 4, 0x30};
inline constexpr u32 kEvDefaultCb = 0x4D1390;                // CDefaultAnimCallback::DefaultAnimCB del EXE (el falso de SetDeleteCallback normaliza la direccion de la DLL a esta)
inline constexpr u32 kAttackTarget = 0x18;                   // CEventPlayerCommandToGroup::m_target
inline constexpr u32 kClimbEntity = 0x24, kClimbAnim = 0x2C; // CTaskSimpleClimb::m_ClimbEntity y m_Anim

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; a[16..23] = datos extra
//   kConst: a[3] = dword en [this] (no se lee)
//   kCtor: a[3] = dword previo en [this] (el constructor lo sustituye por la vtable de la clase)
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kEvBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite
//   kClimbBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = modo de la entidad (+24h: 0 nula, 1 en B a[7], 2 cruda a[8]); a[9] = modo de la animacion (+2Ch: 0 nula, 1 en B a[10], 2 cruda a[11])
//   kClone (CloneEditable de CEventPlayerCommandToGroupAttack): a[3] = m_target (+18h) crudo; a[4] = 0 crudo / 1 puntero a la arena C (a[5]); a[6] = 0 sin memoria (operator new devuelve nulo) / 1 objeto en B (a[7], span 1Ch);
//         a[8] = dword que el constructor falso deja en el objeto (despues el EXE escribe la vtable)
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
    switch (kKind[c.fn]) {
    case kConst:
        c.a[3] = Raw(g);
        break;
    case kCtor:
        c.a[3] = prev();
        break;
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kEvBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kClimbBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = g.P(70) ? (Raw(g) | 1u) : Raw(g);
        c.a[9] = g.Pk({0u, 1u, 1u, 2u});
        c.a[10] = Offset(g, g.R(4), 0x20);
        c.a[11] = g.P(70) ? (Raw(g) | 1u) : Raw(g);
        break;
    case kClone:
        c.a[3] = Raw(g);
        c.a[4] = g.P(35) ? 1u : 0u;
        c.a[5] = Offset(g, g.R(4), 4);
        c.a[6] = g.P(80) ? 1u : 0u;
        c.a[7] = Offset(g, g.R(4), 0x1C);
        c.a[8] = Raw(g);
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
