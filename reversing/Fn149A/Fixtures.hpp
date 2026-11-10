#pragma once
// Fixtures del banco Fn149A (lote 149A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 24 asignadas: 0x638A40 ya tenia hook en el codigo
// (CTaskComplexSequence::CreateNextSubTask, V) y se salta; las 23 restantes:
//   idx  direccion  nombre real                                                              forma
//    0   0x48E800   CTaskSimplePause::GetTaskType (vtable 0x85A278, ranura 4)               thiscall() `ret`: mov eax,0CAh
//    1   0x48E8A0   CTaskSimplePause::Destructor (cuerpo)                                    thiscall() `ret`: mov [ecx],85A278h; jmp 61A3A0 (~CTask)
//    2   0x48E880   CTaskSimplePause::ScalarDeletingDestructor (vtable 0x85A278, ranura 0)  thiscall(flags) `ret 4`: cuerpo 48E8A0, CTask::operator delete 61A5B0
//    3   0x48E8E0   CTaskSimpleCower::ScalarDeletingDestructor (vtable 0x85A0D0)            cuerpo 48E900 (jmp 61A6F0)
//    4   0x744D50   SecuromAnyPairPending                                                   cdecl() `ret`: (w[B733F6] && !w[B73186]) || (w[B733F8] && !w[B73188]); AL
//    5   0x744D90   SecuromFirstPairPending                                                 cdecl() `ret`: w[B733E6] && !w[B73176]; AL
//    6   0x63D0D0   CTaskComplexEnterAnyCarAsDriver::Constructor                            thiscall() `ret`: call 61A3B0; mov [esi],86EA14h; eax=this
//    7   0x6434F0   CTaskComplexEnterAnyCarAsDriver::ScalarDeletingDestructor (vtable 0x86EA14)  cuerpo 63D100
//    8   0x643A40   CTaskComplexEnterCar::ScalarDeletingDestructor (vtable 0x86E6F0)        cuerpo 63DFA0
//    9   0x6389B0   CTaskSimpleWaitForBus::ScalarDeletingDestructor (vtable 0x86E1B8)       cuerpo 632A90
//   10   0x6389D0   CTaskSimpleWaitForPizza::ScalarDeletingDestructor (vtable 0x86E1DC)     cuerpo 632B70 (sin hook: funcion propia de la DLL)
//   11   0x4ACFB0   CEventBuildingCollision::GetEventType (vtable 0x85ACE8, ranura 1)       thiscall() `ret`: mov eax,6
//   12   0x4ACFC0   CEventBuildingCollision::GetLifeTime (ranura 3)                         thiscall() `ret`: xor eax,eax
//   13   0x4ACFD0   CEventBuildingCollision::CanBeInterruptedBySameEvent (ranura 15)        thiscall() `ret`: mov al,1 (solo AL)
//   14   0x4ACFE0   CEventBuildingCollision::GetEventPriority (ranura 2)                    thiscall() `ret`: mov eax,3Bh
//   15   0x4AD050   CEventBuildingCollision::TakesPriorityOver (ranura 11)                  thiscall(evento) `ret 4`: [ref->GetEventType()] != [this->GetEventType()] (llamadas virtuales ranura 1, primero la del argumento); AL
//   16   0x4AD3A0   CEventDraggedOutCar::AffectsPed (vtable 0x85AD30, ranura 5)             thiscall(ped) `ret 4`: CPed::IsAlive (5E0170) && [this+14h] != 0; AL
//   17   0x404180   CBuilding::DestructorBody                                                thiscall() `ret`: jmp 535E90 (~CEntity)
//   18   0x404130   CBuilding::ScalarDeletingDestructor (vtable 0x8585C8)                  thiscall(flags) `ret 4`: cuerpo 404180 y, con bit 0, operator delete de CBuilding EN LINEA (ranura del pool 0xB74498)
//   19   0x5F6510   CPedGroupDefaultTaskAllocatorRandom::Constructor                        thiscall() `ret`: mov eax,ecx; mov [eax],86C77Ch
//   20   0x4D7990   CAEAudioChannel::IsBufferPlaying                                        thiscall() `ret`: mov eax,[ecx+5Ch]; and eax,1
//   21   0x4D7C00   CAEAudioChannel::ScalarDeletingDestructor (vtable 0x85F03C)             thiscall(flags) `ret 4`: cuerpo en linea (vtable, contador 0xB5F898, dos Release de COM, +8 y +0xC a 0), operator delete del CRT 8214BDh con bit 0
//   22   0x5F8510   CPedGroupIntelligence::SetEventResponseTask                              thiscall(6 dwords) `ret 18h`: SetTask (5F7540) con la tarea principal (pares +0Ch, slot -1) y la secundaria (pares +0ACh, slot dado), force 0
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

namespace F149AFix {
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
inline u32 FloatBits(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu, w6 = 0x3F800000u + (g.W() & 0x7FFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFFFFFFFFu, w1, w2, w3, w4, w5, w6});
}

// Clases de funcion (forma)
enum Kind : u32 { kBody, kScalarDeleting, kScalarDeletingBuilding, kCtor, kCtorNoCall, kConstant, kConstantAL, kSecurom, kStatus, kPriority, kAffects, kChannel, kSetResp };
inline constexpr std::array<Kind, kFunctions> kKind{
    kConstant, kBody, kScalarDeleting, kScalarDeleting, kSecurom, kSecurom, kCtor, kScalarDeleting, kScalarDeleting, kScalarDeleting,
    kScalarDeleting, kConstant, kConstant, kConstantAL, kConstant, kPriority, kAffects, kBody, kScalarDeletingBuilding, kCtorNoCall,
    kStatus, kChannel, kSetResp};
// Entrada de cada funcion en el EXE, colaborador que llama (cuerpo del destructor, constructor base, ~CEntity; 0 si ninguno) y vtable de la clase (informativa: dwords previos de [this])
inline constexpr std::array<u32, kFunctions> kEntry{0x48E800, 0x48E8A0, 0x48E880, 0x48E8E0, 0x744D50, 0x744D90, 0x63D0D0, 0x6434F0, 0x643A40, 0x6389B0, 0x6389D0, 0x4ACFB0, 0x4ACFC0, 0x4ACFD0, 0x4ACFE0, 0x4AD050, 0x4AD3A0, 0x404180, 0x404130, 0x5F6510, 0x4D7990, 0x4D7C00, 0x5F8510};
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0x61A3A0, 0x48E8A0, 0x48E900, 0, 0, 0x61A3B0, 0x63D100, 0x63DFA0, 0x632A90, 0x632B70, 0, 0, 0, 0, 0, 0, 0x535E90, 0x404180, 0, 0, 0, 0};
inline constexpr std::array<u32, kFunctions> kVtable{0x85A278, 0x85A278, 0x85A278, 0x85A0D0, 0, 0, 0x86EA14, 0x86EA14, 0x86E6F0, 0x86E1B8, 0x86E1DC, 0x85ACE8, 0x85ACE8, 0x85ACE8, 0x85ACE8, 0x85ACE8, 0x85AD30, 0x8585C8, 0x8585C8, 0x86C77C, 0x85F03C, 0x85F03C, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x18, 4, 4, 4, 0x60, 0x10, 0xB0};

// Direcciones de memoria global del juego que el banco toca (se guardan al empezar y se restauran tras cada lado)
inline constexpr u32 kSecuromBase = 0xB73170, kSecuromSize = 0x290;     // palabras 0xB733F6, 0xB73186, 0xB733F8, 0xB73188, 0xB733E6 y 0xB73176
inline constexpr u32 kChannelCounter = 0xB5F898;                         // g_numSoundChannelsUsed
inline constexpr u32 kBuildingPoolPtr = 0xB74498;                        // puntero del pool de edificios
// Identificadores de eventos de los falsos que no tienen direccion de EXE propia
inline constexpr u32 kEvRelease = 0x7E1EA5E0u, kEvTypeRef = 0x7E1EA5E1u, kEvTypeOwn = 0x7E1EA5E2u;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   cuerpos (kBody), destructores con borrado y constructor: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas (destructores con borrado)
//   kScalarDeletingBuilding: ademas pool falso en la arena B (a[10] = desplazamiento, a[11] = dword en +8): +0 almacenamiento = this - d (d = a[7] como int32), +4 estados (para que estados[indice] caiga en la arena C, a[9] = desplazamiento),
//     +0xC cursor = a[8]; el puntero global 0xB74498 apunta a ese pool
//   kCtorNoCall, kConstant, kConstantAL: a[3] = dword en [this] (no se lee); kStatus: a[3] = dword en +0x5C
//   kSecurom: a[3..8] = las seis palabras (0xB733F6, 0xB73186, 0xB733F8, 0xB73188, 0xB733E6, 0xB73176)
//   kPriority: a[3] = tipo del argumento (ranura 1 de su vtable), a[4] = tipo propio, a[5] bit 0 = argumento es el propio this, a[6] = desplazamiento del argumento en la arena B, a[7] / a[8] = desplazamientos de las vtables falsas (propia / del argumento) en la arena C
//   kAffects: a[3] = EAX que devuelve el falso de IsAlive, a[4] = dword en +0x14, a[5] bit 0 = ped en la arena B (a[6]) en lugar del crudo a[7]
//   kChannel: a[3] = dword previo en [this], a[4] = modo de +8 (0 nulo, 1 objeto 1, 2 objeto 3), a[5] = modo de +0xC (0 nulo, 1 objeto 3, 2 objeto 1), a[6] = banderas, a[7] = contador global, a[8] / a[9] = desplazamientos de los objetos COM 1 / 3 en la
//     arena B, a[10] / a[11] = desplazamientos de sus vtables falsas en la arena C, a[16] = EAX de los falsos de Release
//   kSetResp: a[3] = ped, a[4] = hasMainTask, a[5] = tarea principal, a[6] = hasSecondaryTask, a[7] = tarea secundaria, a[8] = slot (todos dwords crudos)
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
    case kBody: case kScalarDeleting: case kCtor:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kScalarDeletingBuilding: {
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        // d = t * 56 + resto con trunc(d / 56) = t; el indice cae en [-8, 600] (el estado cae en la arena C)
        const i32 e = 56;
        const i32 cursor = i32(g.Pk({0u, 0xFFFFFFFFu, 1u, 5u, 31u, 0x80000000u, 0x7FFFFFFFu, g.W(), g.W() % 700u, g.W() % 64u}));
        const u32 mode = g.R(10);
        i32 t{};
        switch (mode) {
        case 0: t = i32(g.R(7)) - 3; break;
        case 1: t = i32(g.R(600)); break;
        case 2: t = cursor >= 1 && cursor <= 600 ? cursor - 1 + i32(g.R(3)) : i32(g.R(40)); break;   // alrededor del cursor
        case 3: t = cursor >= 1 && cursor <= 600 ? cursor : i32(g.R(40)); break;
        case 4: t = -i32(g.R(9)); break;
        default: t = i32(g.R(300)); break;
        }
        // resto: en [0, e) si t > 0, en (-e, 0] si t < 0 y en (-e, e) si t = 0
        const i32 rem = t == 0 ? i32(g.R(u32(2 * e - 1))) - (e - 1) : (t > 0 ? i32(g.R(u32(e))) : -i32(g.R(u32(e))));
        c.a[7] = u32(t * e + rem);
        c.a[8] = u32(cursor);
        c.a[9] = 4 * g.R(u32(kWin) / 4) + g.R(4);                         // donde cae estados[indice] en la arena C (el puntero de estados es Obj(a[9]) - indice)
        c.a[10] = Offset(g, kind, 0x10);
        c.a[11] = Raw(g);
        break;
    }
    case kCtorNoCall: case kConstant: case kConstantAL:
        c.a[3] = Raw(g);
        break;
    case kStatus:
        c.a[3] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFEu, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0x100u, 0x101u, g.W(), g.W(), g.W() & ~1u, g.W() | 1u});
        break;
    case kSecurom: {
        for (unsigned i = 3; i <= 8; ++i) {
            const u32 w = g.W();
            c.a[i] = g.Pk({0u, 0u, 0u, 1u, 0xFFFFu, 0x10000u, 0xFFFF0000u, 0x8000u, 0x10001u, w, w & 0xFFFFu, w & 0xFFFF0000u, 0x100u, 0xFF00u});
        }
        break;
    }
    case kPriority: {
        const u32 t1 = g.Pk({0u, 1u, 6u, 0x58u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), 0x7Fu});
        c.a[3] = t1;
        c.a[4] = g.P(35) ? t1 : g.Pk({0u, 1u, 6u, 0x58u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), t1 + 1u, t1 ^ 0x100u});
        c.a[5] = g.P(12) ? 1u : 0u;
        c.a[6] = 4 * g.R(0x100) + (g.P(10) ? 1u + g.R(3) : 0u);       // argumento en la arena B: [0] = puntero de vtable
        c.a[7] = 4 * g.R(0x100);                                       // vtable falsa propia (arena C): ranura 1 = falso de tipo propio
        c.a[8] = 0x400 + 4 * g.R(0x100);                               // vtable falsa del argumento (arena C)
        break;
    }
    case kAffects:
        c.a[3] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x80u, g.W(), g.W(), g.W() & ~0xFFu, g.W() | 1u});
        c.a[4] = g.Pk({0u, 0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x00400000u + g.R(0x800000), g.W(), g.W(), 0x100u});
        c.a[5] = g.P(50) ? 1u : 0u;
        c.a[6] = 4 * g.R(0x200);
        c.a[7] = Raw(g);
        break;
    case kChannel: {
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85F040u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 1u, 1u, 1u, 1u, 2u});
        c.a[5] = g.Pk({0u, 1u, 1u, 1u, 2u, 0u});
        c.a[6] = FlagsDword(g);
        c.a[7] = g.Pk({0u, 1u, 2u, 5u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, g.W() % 64u, g.W() % 64u, g.W()});
        c.a[8] = 4 * g.R(0x100);                    // objeto COM 1 (arena B): [0] = puntero a su vtable falsa
        c.a[9] = 0x400 + 4 * g.R(0x100);            // objeto COM 3 (arena B)
        c.a[10] = 4 * g.R(0x100);                   // vtable falsa 1 (arena C): ranura 2 = Release
        c.a[11] = 0x400 + 4 * g.R(0x100);           // vtable falsa 3 (arena C)
        c.a[16] = Raw(g);
        break;
    }
    case kSetResp:
        c.a[3] = Raw(g);
        c.a[4] = FlagsDword(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        c.a[7] = Raw(g);
        c.a[8] = g.Pk({0xFFFFFFFFu, 0u, 1u, 2u, 3u, 4u, 5u, 0x7FFFFFFFu, 0x80000000u, g.W(), g.W(), g.R(0x100)});
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
