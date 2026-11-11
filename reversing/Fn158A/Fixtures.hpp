#pragma once
// Fixtures del banco Fn158A (lote 158A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables leidas del PE y de los llamadores, ver README.md y fixture_oracle.py). 23 asignadas, ninguna
// con hook (se comprobo el codigo, docs/hooks.csv y git worktree list). Nombres reales: CEventLeaderEntryExit (vtable 0x859540), CTaskSimpleUninterruptable (0x859580), CTaskSimpleGoToPointFine (0x86F964),
// 0x663620 es el destructor con borrado de CTaskComplexTrackEntity (0x86F998), 0x6620D0 es CTaskGangHasslePed::Clone (0x86FA00) y 0x647770 es CTaskSimpleCarForcePedOut::GetTaskType (0x86EE94).
//   idx  direccion  nombre real                                                              forma
//    0   0x43E1E0   CEventLeaderEntryExit::HasEditableResponse (ranura 9)                    thiscall() `ret`: xor al,al
//    1   0x43E1F0   CEventLeaderEntryExit::GetSourceEntity (ranura 10)                       thiscall() `ret`: xor eax,eax
//    2   0x43E210   CEventLeaderEntryExit::GetLifeTime (ranura 3)                            thiscall() `ret`: xor eax,eax
//    3   0x43E280   CEventLeaderEntryExit::CanBeInterruptedBySameEvent (ranura 15)           thiscall() `ret`: mov al,1
//    4   0x43E200   CEventLeaderEntryExit::GetEventType (ranura 1)                           thiscall() `ret`: mov eax,4Dh
//    5   0x43E2C0   CEventLeaderEntryExit::GetEventPriority (ranura 2)                       thiscall() `ret`: mov eax,37h
//    6   0x43E2B0   CEventLeaderEntryExit cuerpo del destructor                              thiscall() `ret`: [ecx]=859540h; jmp 4B2210 (cuerpo de ~CEventAreaCodes, con hook)
//    7   0x43E290   CEventLeaderEntryExit::ScalarDeletingDestructor (ranura 0)               thiscall(flags) `ret 4`: cuerpo 43E2B0 (funcion 6), CEvent::operator delete 4B5630
//    8   0x43E220   CEventLeaderEntryExit::Clone (ranura 4)                                  thiscall() `ret`: CEvent::operator new 4B5620 (0x10), ctor 4B2190(+0Ch), [obj]=859540h; devuelve el objeto
//    9   0x43E300   CTaskSimple::GetSubTask (vtable 0x86D4A8, ranura 2; compartida)           thiscall() `ret`: xor eax,eax
//   10   0x43E310   CTaskSimple::IsSimple (vtable 0x86D4A8, ranura 3; compartida)             thiscall() `ret`: mov al,1
//   11   0x43E320   CTaskSimple::SetPedPosition (vtable 0x86D4A8, ranura 8; compartida)         thiscall(ped) `ret 4`: xor al,al
//   12   0x43E3A0   CTaskSimpleUninterruptable::ProcessPed (ranura 7)                         thiscall(ped) `ret 4`: xor al,al
//   13   0x43E380   CTaskSimpleUninterruptable::GetTaskType (ranura 4)                        thiscall() `ret`: mov eax,0C9h
//   14   0x43E390   CTaskSimpleUninterruptable::MakeAbortable (ranura 6)                      thiscall(ped, prioridad, evento) `ret 0Ch`: cmp dword [esp+8],2; sete al
//   15   0x43E3D0   CTaskSimpleUninterruptable cuerpo del destructor                          thiscall() `ret`: [ecx]=859580h; jmp 61A3A0 (~CTask, con hook)
//   16   0x43E3B0   CTaskSimpleUninterruptable::ScalarDeletingDestructor (ranura 0)           thiscall(flags) `ret 4`: cuerpo 43E3D0 (funcion 15), CTask::operator delete 61A5B0
//   17   0x43E330   CTaskSimpleUninterruptable::Clone (ranura 1)                              thiscall() `ret`: CTask::operator new 61A5A0 (8), ctor 61A390, [obj]=859580h; devuelve el objeto
//   18   0x65EEE0   CTaskSimpleGoToPointFine::GetTaskType (ranura 4)                          thiscall() `ret`: mov eax,39Eh
//   19   0x65EEF0   CTaskSimpleGoToPointFine cuerpo del destructor                           thiscall() `ret`: [ecx]=86F964h; jmp 667A00 (~CTaskSimpleGoTo, con hook)
//   20   0x663620   CTaskComplexTrackEntity::ScalarDeletingDestructor (0x86F998; el listado decia CTaskSimpleGoToPointFine)  thiscall(flags) `ret 4`: cuerpo 65F460 (con hook), CTask::operator delete 61A5B0
//   21   0x6620D0   CTaskGangHasslePed::Clone (0x86FA00; el listado decia CTaskSimpleGoToPointFine) thiscall() `ret`: CTask::operator new 61A5A0 (0x2C), ctor 65FED0(+0Ch,+10h,+14h,+18h); devuelve el EAX del ctor
//   22   0x647770   CTaskSimpleCarForcePedOut::GetTaskType (0x86EE94; el listado decia CTaskSimpleCarJumpOut) thiscall() `ret`: mov eax,32Fh
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

namespace F158AFix {
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

// Clases de funcion (forma)
enum Kind : u32 { kConst, kBool, kAbort, kSdd, kEvBody, kEvClone, kTaskClone, kGangClone };
inline constexpr std::array<Kind, kFunctions> kKind{
    kBool, kConst, kConst, kBool, kConst, kConst, kEvBody, kSdd, kEvClone,
    kConst, kBool, kBool, kBool, kConst, kAbort, kEvBody, kSdd, kTaskClone,
    kConst, kEvBody, kSdd, kGangClone, kConst};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x43E1E0, 0x43E1F0, 0x43E210, 0x43E280, 0x43E200, 0x43E2C0, 0x43E2B0, 0x43E290, 0x43E220,
                                                    0x43E300, 0x43E310, 0x43E320, 0x43E3A0, 0x43E380, 0x43E390, 0x43E3D0, 0x43E3B0, 0x43E330,
                                                    0x65EEE0, 0x65EEF0, 0x663620, 0x6620D0, 0x647770};
// Argumentos de pila de las funciones kBool (`ret n`: los limpia el callee): 0 o 1 (el ped, que no se lee)
inline constexpr std::array<u32, kFunctions> kNArgs{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor o cuerpo de la base; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0, 0x4B2210, 0x43E2B0, 0, 0, 0, 0, 0, 0, 0, 0x61A3A0, 0x43E3D0, 0, 0, 0x667A00, 0x65F460, 0, 0};
// Funcion de liberacion de los destructores con borrado (CTask::operator delete, CEvent::operator delete); 0 = ninguna
inline constexpr std::array<u32, kFunctions> kDelete{0, 0, 0, 0, 0, 0, 0, 0x4B5630, 0, 0, 0, 0, 0, 0, 0, 0, 0x61A5B0, 0, 0, 0, 0x61A5B0, 0, 0};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0, 0, 0, 0, 0x859540, 0x859540, 0x859540, 0, 0, 0, 0, 0, 0, 0x859580, 0x859580, 0x859580, 0, 0x86F964, 0x86F998, 0x86FA00, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 4, 4, 4, 0x10, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x2C, 4};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; a[16..23] = datos extra
//   kConst: a[3] = dword en [this] (no se lee); kBool: ademas a[4] = argumento de pila (el ped, no se lee) cuando kNArgs = 1
//   kAbort: a[3] = ped crudo, a[4] = prioridad (dword completo de la pila: solo 2 da verdadero), a[5] = evento crudo
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kEvBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite
//   kEvClone: a[3] = m_ped (+0Ch) crudo; a[4] = 0 crudo / 1 puntero a la arena C (a[5]); a[6] = 0 sin memoria (operator new devuelve nulo) / 1 objeto en B (a[7]); a[8] = dword que el constructor falso deja en el objeto; a[9] = EAX del constructor falso
//   kTaskClone: a[3] = dword previo en [this] (no se lee); a[6] = 0 sin memoria / 1 objeto en B (a[7]); a[8] = dword que el constructor falso deja en el objeto; a[9] = EAX del constructor falso
//   kGangClone: como kEvClone (a[3..9]) y ademas a[16], a[17], a[18] = dwords +10h, +14h, +18h del objeto
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
    case kBool:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kAbort:
        c.a[3] = Raw(g);
        c.a[4] = g.Pk({2u, 2u, 2u, 2u, 2u, 0u, 1u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000002u, 0x102u, 0x10002u, 0x20000002u, 0xFFFFFFFEu, 0x7FFFFFFFu, g.R(8), g.R(8), g.W()});
        c.a[5] = Raw(g);
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
    case kEvClone:
    case kGangClone:
        c.a[3] = Raw(g);
        c.a[4] = g.P(35) ? 1u : 0u;
        c.a[5] = Offset(g, g.R(4), 4);
        c.a[6] = g.P(80) ? 1u : 0u;
        c.a[7] = Offset(g, g.R(4), kSpan[c.fn] == 0x2C ? 0x2C : 0x10);
        c.a[8] = Raw(g);
        c.a[9] = g.Pk({0u, Raw(g), Raw(g), Raw(g), 1u});
        c.a[16] = Raw(g);
        c.a[17] = Raw(g);
        c.a[18] = Raw(g);
        break;
    case kTaskClone:
        c.a[3] = prev();
        c.a[6] = g.P(80) ? 1u : 0u;
        c.a[7] = Offset(g, g.R(4), 8);
        c.a[8] = Raw(g);
        c.a[9] = g.Pk({0u, Raw(g), Raw(g), Raw(g), 1u});
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
