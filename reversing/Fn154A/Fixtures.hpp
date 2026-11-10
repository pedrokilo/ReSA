#pragma once
// Fixtures del banco Fn154A (lote 154A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 18 asignadas: 0x638A40 (CTaskComplexSequence::CreateNextSubTask,
// V), 0x4F21C0 (CAEStreamingChannel::Stop, V) y 0x59BB60 (CMatrix::Update, S) YA tenian hook y se saltaron; se prueban las 15 restantes:
//   idx  direccion  nombre real                                                              forma
//    0   0x6644C0   CTaskComplexSignalAtPed::ScalarDeletingDestructor (vtable 0x86FA8C)      thiscall(flags) `ret 4`: cuerpo 660AB0 (con hook), CTask::operator delete 61A5B0
//    1   0x6644E0   CTaskGoToVehicleAndLean::ScalarDeletingDestructor (vtable 0x86FAC8)      cuerpo 660EE0 (con hook)
//    2   0x67CB50   CTaskComplexFallAndGetUp::ScalarDeletingDestructor (vtable 0x870454)     cuerpo 678790 (sin hook: funcion propia de la DLL)
//    3   0x678300   CTaskSimpleFall::GetTaskType (vtable 0x870430, ranura 4)                 thiscall() `ret`: mov eax,0CFh
//    4   0x678310   CTaskSimpleFall cuerpo del destructor (marco SEH)                        thiscall() `ret`: vtable 870430h, SetFinishCallback(4D1390h,0) si [+14h], ~CTask (61A3A0)
//    5   0x4C3740   CEventHandlerHistory::Flush                                               thiscall() `ret`: delete virtual de +8, +4 y +0Ch (en ese orden), a cero
//    6   0x4C3E30   CEventHandlerHistory::Flush (copia)                                       idem
//    7   0x4ACDA0   CEventObjectCollision::GetEventType (vtable 0x85ACA8, ranura 1)          thiscall() `ret`: mov eax,5
//    8   0x4ACDC0   CEventObjectCollision::GetEventPriority (ranura 2)                        thiscall() `ret`: mov eax,39h
//    9   0x4ACDB0   CEventObjectCollision::GetLifeTime (ranura 3)                             thiscall() `ret`: xor eax,eax
//   10   0x4ACEF0   CEventObjectCollision::TakesPriorityOver (ranura 11)                      thiscall(evento) `ret 4`: mov al,1 (solo AL)
//   11   0x4ACE80   CEventObjectCollision::IsHeadOnCollision                                  thiscall(ped) `ret 4`: normal sin z, Normalise (59C910), -dot > 0.9397f; x87 de 24 bits; AL
//   12   0x602240   CPedIntelligence::AddInterestingEntity                                    thiscall(entidad) `ret 4`: desplaza +288h..+290h con CleanUpOldReference y RegisterReference
//   13   0x59BD90   CMatrix::AttachRW                                                         thiscall(matriz, bandera) `ret 8`: RwMatrixDestroy si [+40h] y dword [+44h]; UpdateRwMatrix si matriz
//   14   0x506CC0   CAEAudioEntity cuerpo del destructor (copia de 0x5DE9E0)                  thiscall() `ret`: vtable 859988h y salto a ~CAESound (4EF660) con ECX = this+8
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

namespace F154AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 15;

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


// Clases de funcion (forma)
enum Kind : u32 { kConst, kConstAL1, kSdd, kFallBody, kFlush, kHeadOn, kAddInt, kAttach, kAudioDtor };
inline constexpr std::array<Kind, kFunctions> kKind{kSdd, kSdd, kSdd, kConst, kFallBody, kFlush, kFlush, kConst, kConst, kConst, kConstAL1, kHeadOn, kAddInt, kAttach, kAudioDtor};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x6644C0, 0x6644E0, 0x67CB50, 0x678300, 0x678310, 0x4C3740, 0x4C3E30, 0x4ACDA0, 0x4ACDC0, 0x4ACDB0, 0x4ACEF0, 0x4ACE80, 0x602240, 0x59BD90, 0x506CC0};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x660AB0, 0x660EE0, 0x678790, 0, 0x61A3A0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86FA8C, 0x86FAC8, 0x870454, 0, 0x870430, 0, 0, 0x85ACA8, 0x85ACA8, 0x85ACA8, 0x85ACA8, 0x85ACA8, 0, 0, 0x859988};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 0x18, 0x1C, 0x1C, 4, 4, 4, 4, 0x30, 0x294, 0x48, 0x7C};
inline constexpr u32 kEvDtor = 0x7E1EA5E1u;                    // evento del destructor virtual falso de Flush
inline constexpr u32 kThreshold = 0x3F70902Eu;                 // 0.9397f (0x85AAE4 del EXE)

inline bool IsNan(u32 w) { return (w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu) != 0; }

// Un float de 32 bits para IsHeadOnCollision: sin magnitudes que desborden (|exponente| <= 32): la x87 de 24 bits tiene exponente ampliado y el SSE de la DLL no (ver README.md)
inline u32 LimFloat(Gen& g) {
    const u32 w1 = (g.W() & 0x80000000u) | ((96u + g.R(64)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 w2 = (g.W() & 0x80000000u) | ((120u + g.R(8)) << 23) | (g.W() & 0x7FFFFFu);   // cerca de 1
    const u32 w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = (g.W() & 0x80000000u) | (g.W() & 0x007FFFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, w3, w4, 0x00000001u, 0x007FFFFFu, 0x80000001u, kThreshold, kThreshold ^ 0x80000000u, w1, w1, w2, w2, w5});
}
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kConst: a[3] = dword en [this] (no se lee); kConstAL1: a[3] = dword en [this], a[4] = argumento (no se lee)
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kFallBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = animacion (+0x14; 0 o cruda)
//   kFlush: this = historial en A (+4 no temporal, +8 temporal, +0xC guardado); a[3] = modo del no temporal (0 nulo, 1 objeto propio); a[4] = modo del temporal (0 nulo, 1 propio, 2 el del no temporal);
//         a[5] = modo del guardado (0 nulo, 1 propio, 2 el del no temporal, 3 el del temporal); a[6], a[7], a[8] = desplazamientos en B de los tres objetos (8 bytes; vtable falsa en +0); a[9] = desplazamiento en C de la vtable falsa
//         (ranura 0 = destructor falso); a[10] = dword previo en +0
//   kHeadOn: this = evento en A (normal del impacto en +0x18); a[3..5] = x, y, z de la normal (bits); a[6] = desplazamiento del ped en B (matriz en +0x14); a[7] = desplazamiento de la matriz en C (avance en +0x10);
//         a[8..10] = avance x, y, z (bits); a[16..18] = vector que deja el Normalise falso (bits)
//   kAddInt: this = CPedIntelligence en A; a[3..5] = casillas previas de +0x288, +0x28C y +0x290 (0 o crudas); a[6] = entidad nueva (0 o cruda)
//   kAttach: this = CMatrix en A; a[3] = matriz enlazada previa (+0x40); a[4] = dword de propiedad previo (+0x44); a[5] = matriz nueva (argumento 1); a[6] = dword de la bandera (argumento 2; solo cuenta el byte bajo)
//   kAudioDtor: a[3] = dword previo en [this]
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x86D48Cu, 0x86D4A8u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    const auto rawOrNull = [&] { return g.P(35) ? 0u : Raw(g); };
    switch (kKind[c.fn]) {
    case kConst:
        c.a[3] = Raw(g);
        break;
    case kConstAL1:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kFallBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.P(45) ? 0u : Raw(g);
        break;
    case kFlush:
        c.a[3] = g.P(70) ? 1u : 0u;
        c.a[4] = g.Pk({0u, 1u, 1u, 2u});
        c.a[5] = g.Pk({0u, 1u, 1u, 2u, 3u});
        c.a[6] = 4 * g.R(0x40);
        c.a[7] = 0x400 + 4 * g.R(0x40);
        c.a[8] = 0x800 + 4 * g.R(0x40);
        c.a[9] = 4 * g.R(0x100);
        c.a[10] = prev();
        break;
    case kHeadOn: {
        u32 nrm[3], fwd[3], out[3];
        for (auto& w : nrm) w = LimFloat(g);
        for (auto& w : fwd) w = LimFloat(g);
        for (auto& w : out) w = LimFloat(g);
        if (g.P(40)) {   // frontera del umbral: una componente = -0.9397f +- unos ulp con el avance 1 en ese eje y 0 en los demas
            const u32 k = g.R(3);
            const u32 d = g.Pk({0u, 1u, 2u, 8u, 0x100u});
            for (unsigned i = 0; i < 3; ++i) { fwd[i] = 0u; out[i] = g.P(50) ? 0u : (g.W() & 0x80000000u); }
            fwd[k] = 0x3F800000u;
            out[k] = (kThreshold ^ 0x80000000u) + (g.P(50) ? d : u32(0) - d);
        }
        c.a[3] = nrm[0]; c.a[4] = nrm[1]; c.a[5] = nrm[2];
        c.a[6] = Offset(g, g.R(4), 0x18);
        c.a[7] = Offset(g, g.R(4), 0x1C);
        c.a[8] = fwd[0]; c.a[9] = fwd[1]; c.a[10] = fwd[2];
        c.a[16] = out[0]; c.a[17] = out[1]; c.a[18] = out[2];
        break;
    }
    case kAddInt:
        c.a[3] = rawOrNull(); c.a[4] = rawOrNull(); c.a[5] = rawOrNull(); c.a[6] = rawOrNull();
        if (g.P(15)) c.a[6] = c.a[3];
        if (g.P(10)) c.a[4] = c.a[3];
        if (g.P(10)) c.a[5] = c.a[4];
        break;
    case kAttach:
        c.a[3] = rawOrNull();
        c.a[4] = g.Pk({0u, 0u, 1u, 1u, 0x100u, 0x01000000u, 0xFFFFFFFFu, 0xFFu, 0x10000u, g.W()});
        c.a[5] = rawOrNull();
        c.a[6] = FlagsDword(g);
        break;
    case kAudioDtor:
        c.a[3] = prev();
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
