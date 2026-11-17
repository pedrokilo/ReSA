#pragma once
// Fixtures del banco Fn222A (lote 222A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (16 entradas, todas con hook y validated=0 antes del banco; las clases del listado salen del desensamblado, ver README.md y fixture_oracle.py).
//   idx  direccion  nombre real                                              forma
//    0   0x4032A4   CProtectionStub::Empty                                   stdcall(1 argumento) `ret 4`: no hace nada (conserva EBX/ESI/EDI/EBP)
//    1   0x403638   CProtectionStub::BlockEncrypt(bloque, tabla)             cdecl: cifrado de 8 bytes con 12 rondas; el EXE reescribe el bloque en cada ronda
//    2   0x4036C1   CProtectionStub::BlockDecrypt(bloque, tabla)             cdecl: descifrado inverso
//    3   0x403E00   CBuilding::Constructor (cuerpo)                           thiscall() `ret`: CEntity::CEntity (0x532A90) y despues +0x36, +0x1C, vtable 0x8585C8; devuelve this
//    4   0x447000   CAEAudioEntity::Destructor-6                              thiscall() `ret`: [this] = 0x859988 y salto a CAESound::~CAESound (0x4EF660) sobre this + 8
//    5   0x447050   CAEAudioEntity::Destructor-7                              idem
//    6   0x463490   CAutoPilot::SetCarMissionUnlessCrashAndBurn(mision)       thiscall(BYTE) `ret 4`: escribe [this+0x2A] salvo que valga 0x39 o 0x3A
//    7   0x4634D0   CDecisionMaker::SetDefault                                thiscall() `ret`: 41 llamadas a CDecision::SetDefault (0x600530), paso 0x3C
//    8   0x46A2F0   CEntity::IsInvisibleOrRenderDamaged                       thiscall() `ret`: AL = (flags & 0x200) || !(flags & 0x80)
//    9   0x46A340   CTheScripts::ScriptTextPairSet(a, b)                      thiscall(a, b) `ret 8`: copia dos cadenas (a a +0, b a +0x18) hasta el cero inclusive; devuelve this
//   10   0x533020   CEntity::Add() (virtual)                                  GetBoundRect (hueco 9, salida en la pila) y Add(rect) (hueco 1) por la vtable del objeto (releida entre las dos)
//   11   0x5335E0   CPtrNodeSingleLinkPool::AddItem(item)                     thiscall(item) `ret 4`: nodo (operator new 0x552380), devuelve el nodo
//   12   0x533610   CPtrNodeSingleLinkPool::RemoveItem(item)                  thiscall(item) `ret 4`: operator delete de nodos simples (0x552390)
//   13   0x533670   CPtrNodeDoubleLinkPool::AddItem(item)                     idem con nodos de 12 bytes (0x5523C0)
//   14   0x5336B0   CPtrNodeDoubleLinkPool::RemoveItem(item)                  idem (0x5523D0)
//   15   0x536C20   CEntity::ScalarDeletingDestructor(banderas BYTE)          thiscall(BYTE) `ret 4`: CEntity::~CEntity (0x535E90) y, con el bit 0, operator delete (0x8214BD); devuelve this
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL, o solo en la direccion
// del EXE cuando la DLL los llama por ella). Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4
// (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F222AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 16;
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
// La pila del hilo del banco (cerca de 0x00EB0000) se etiqueta aparte (Tag): un puntero crudo que caiga ahi se confundiria con un objeto de la pila, asi que ninguno se genera en esa zona
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}

// Clases de funcion (forma)
enum Kind : u32 { kStdEmpty, kEnc, kDec, kBldCtor, kAudDtor, kMission, kDecision, kInvis, kTextPair, kEntAdd, kSAdd, kSRem, kDAdd, kDRem, kScalarDel };
inline constexpr std::array<Kind, kFunctions> kKind{kStdEmpty, kEnc, kDec, kBldCtor, kAudDtor, kAudDtor, kMission, kDecision, kInvis, kTextPair, kEntAdd, kSAdd, kSRem, kDAdd, kDRem, kScalarDel};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4032A4, 0x403638, 0x4036C1, 0x403E00, 0x447000, 0x447050, 0x463490, 0x4634D0, 0x46A2F0, 0x46A340, 0x533020, 0x5335E0, 0x533610, 0x533670, 0x5336B0, 0x536C20};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{8, 0x80, 0x80, 0x40, 0x10, 0x10, 0x30, 0x9A8, 0x20, 0x80, 8, 0x20, 0x20, 0x20, 0x20, 0x10};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   kStdEmpty (0): a[5] = 0: a[4] = el argumento de pila (una constante sin los bits bajos 1 ni 2); si no: el argumento es un puntero a la arena B en a[4] mas a[5] - 1 bytes
//   kEnc (1), kDec (2): el bloque de 8 bytes esta en A en a[0]; a[4] = modo de la tabla de 26 dwords (0 en B alineada, 1 en B desalineada, 2 en A solapada con el bloque, 3 en A detras del bloque),
//         a[5] = desplazamiento de la tabla (en B, o en la ventana de A)
//   kBldCtor (3): a[4], a[5] = dwords que el falso de CEntity::CEntity escribe en +0 y +0x1C, a[6] = byte que escribe en +0x36
//   kAudDtor (4, 5): a[4] = dword que el falso de CAESound::~CAESound escribe en [this + 8]
//   kMission (6): a[4] = mision actual (byte de +0x2A), a[5] = argumento (dword con ruido en los bits altos)
//   kDecision (7): nada mas
//   kInvis (8): a[4] = banderas (dword de +0x1C)
//   kTextPair (9): a[4] = longitud de la primera cadena, a[5] = longitud de la segunda, a[6] = origen de la primera (0 en B, 1 en C, 2 en A delante del destino), a[7] = su desplazamiento (o la separacion),
//         a[8] = origen de la segunda (0 externa, 2 en A detras de +0x18, 3 lee lo que escribio la primera), a[9] = su desplazamiento (o la separacion), a[10], a[12] = semillas de los caracteres,
//         a[11] = arena de la segunda externa (0 B, 1 C)
//   kEntAdd (10): a[4] = modo (0 el falso de GetBoundRect no toca el objeto, 1 cambia la vtable del objeto a la segunda), a[5], a[6] = desplazamientos de las vtables 1 (en B) y 2 (en C),
//         a[7..10] = palabras del rectangulo del primer falso, a[11..14] = las del segundo
//   kSAdd (11), kDAdd (13): a[4] = cabeza (0 nula, 1 nodo en C, 2 = this, 3 = this + 4, 4 = this - 4), a[5] = desplazamiento del nodo de C, a[6] = lugar del nodo nuevo (0 en D, 1 en D desalineado, 2 = this,
//         3 = this - 4, 4 = this - 8, 5 = this + 4, 6 = la cabeza), a[7] = su desplazamiento en D, a[8] = elemento crudo, a[9] = modo del elemento (0 el crudo, 1 = this, 2 = el nodo de C)
//   kSRem (12), kDRem (14): a[4] = longitud de la cadena (0..6), a[5] = desplazamiento del primer nodo en B, a[6] = separacion, a[7] = indice del buscado, a[8] = elementos (0 distintos, 1 repetidos),
//         a[9] = semilla de los elementos, a[10] = busqueda (0 el elemento del indice, 1 un valor ausente, 2 el valor cero), a[11] = incoherencia de los anteriores (solo dobles: 0 coherente, 1 el anterior del buscado
//         nulo, 2 apunta a basura de C), a[12] = desplazamiento de la basura en C
//   kScalarDel (15): a[4] = banderas (dword con ruido), a[5] = dword que el falso de CEntity::~CEntity escribe en [this]
inline u32 ItemValue(u32 seed, u32 mode, u32 i) {   // elemento del nodo i de una cadena
    if (mode == 1) return 0x887D00u + (Rand(seed, i) % 3u) * 8u;
    if (Rand(seed, 200 + i) % 6u == 0u) return 0u;
    return 0x10000000u + (Rand(seed, 100 + i) & 0x00FFFFF0u);
}
inline u32 StrChar(u32 seed, u32 i) { return 1u + Rand(seed, 500u + i) % 255u; }
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x887D5Cu, 0x870114u});
    auto& a = c.a;
    const auto ExtOff = [&](u32 span) { return 0x40u + 4u * g.R((u32(kWin) - 0x100u - span) / 4u) + (g.P(25) ? 1u + g.R(3) : 0u); };
    switch (kKind[c.fn]) {
    case kStdEmpty:
        if (g.P(50)) {
            a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xDEADBEEFu, 0x887D5Cu, 4u, 0x870114u});
            a[5] = 0;
        } else {
            a[4] = AuxOffset(g, 8);
            a[5] = 1u + g.R(4);
        }
        break;
    case kEnc:
    case kDec: {
        a[4] = g.Pk({0u, 0u, 1u, 2u, 2u, 2u, 3u});
        const i32 o = i32(a[0]);
        if (a[4] == 0) a[5] = AuxOffset(g, 0x68);
        else if (a[4] == 1) a[5] = AuxOffset(g, 0x6C) + 1 + g.R(3);
        else if (a[4] == 2) {
            const i32 delta = i32(g.Pk({0u, 4u, 0xFFFFFFFCu, 0xFFFFFFF8u, 1u, 0xFFFFFFFFu, 7u, 0xFFFFFFF0u, 0xFFFFFF9Cu, 0xFFFFFFA0u, 0xFFFFFFA4u, 0xFFFFFFA8u, 0xFFFFFFFEu, 0xFFFFFFFDu})) + (g.P(40) ? i32(g.R(0x6E)) - 0x66 : 0);
            i32 key = o + delta;
            if (key < 0) key = 0;
            a[5] = u32(key);
        } else a[5] = u32(o) + 8u + 4u * g.R(8);
        break;
    }
    case kBldCtor:
        a[4] = g.W(); a[5] = g.Pk({g.W(), 0u, 0xFFFFFFFFu, 1u, 0xFFFFFFFEu}); a[6] = g.Pk({g.W(), 0u, 0xFFu, 6u, 1u, 7u, 0x80u});
        break;
    case kAudDtor:
        a[4] = g.W();
        break;
    case kMission:
        a[4] = g.Pk({0x38u, 0x39u, 0x39u, 0x3Au, 0x3Au, 0x3Bu, 0u, 0xFFu, 0x37u, 0x80u, g.R(256)});
        a[5] = g.Pk({g.W(), g.W() & 0xFFu, 0x39u, 0x3Au, 0xFFFFFF39u, 0x100u + g.R(256), 0u, 0x10u + g.R(0x30)});
        break;
    case kDecision:
        break;
    case kInvis: {
        u32 flags = g.W();
        const u32 b7 = g.Pk({0u, 1u, 2u}), b9 = g.Pk({0u, 1u, 2u});
        if (b7 == 0) flags &= ~0x80u; else if (b7 == 1) flags |= 0x80u;
        if (b9 == 0) flags &= ~0x200u; else if (b9 == 1) flags |= 0x200u;
        a[4] = flags;
        break;
    }
    case kTextPair: {
        a[6] = g.Pk({0u, 0u, 1u, 1u, 2u});
        a[8] = g.Pk({0u, 0u, 2u, 3u, 3u});
        if (a[6] == 2 && a[8] == 2) a[8] = 0;
        a[4] = g.Pk({0u, 1u, 2u, 5u, 0x16u, 0x17u, 0x18u, 0x19u, 0x1Fu, 0x20u, 0x30u, g.R(0x31)});
        a[5] = g.Pk({0u, 1u, 2u, 5u, 0x16u, 0x17u, 0x18u, 0x19u, 0x1Fu, 0x20u, 0x30u, g.R(0x31)});
        if (a[8] == 2 && a[4] > 0x16) a[4] = g.R(0x17);
        a[7] = a[6] == 2 ? 1u + g.R(0x40) : ExtOff(0x40);
        if (a[8] == 0) a[9] = ExtOff(0x40);
        else if (a[8] == 2) a[9] = 1u + g.R(0x20);
        else {
            u32 j = g.R(a[4] + 1);
            if (a[4] >= 0x18 && j < 0x18) j = 0x18 + g.R(a[4] - 0x18 + 1);
            a[9] = j;
        }
        a[10] = g.W(); a[12] = g.W();
        a[11] = g.P(50) ? 1u : 0u;
        break;
    }
    case kEntAdd:
        a[4] = g.P(50) ? 1u : 0u;
        a[5] = 0x40 + 4 * g.R(0x300); a[6] = 0x40 + 4 * g.R(0x300);
        for (int k = 0; k < 8; ++k) a[7 + k] = FloatBits(g);
        break;
    case kSAdd:
    case kDAdd:
        a[4] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u});
        a[5] = AuxOffset(g, 0x20);
        a[6] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u});
        a[7] = AuxOffset(g, 0x20) + (a[6] == 1 ? 1u + g.R(3) : 0u);
        a[8] = g.Pk({Raw(g), Raw(g), 0u, 1u, 0xFFFFFFFFu, 0x887D5Cu});
        a[9] = g.Pk({0u, 0u, 1u, 2u});
        if (kKind[c.fn] == kDAdd && (a[6] == 2 || (a[6] == 6 && a[4] == 2)) && a[9] == 0) a[9] = 1u + g.R(2);
        break;
    case kSRem:
    case kDRem: {
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u, 3u});
        a[5] = 0x40 + 4 * g.R(0x80);
        a[6] = 4u * (3u + g.R(6));
        a[7] = a[4] ? g.R(a[4] + (g.P(20) ? 2u : 0u)) : g.R(3);
        a[8] = g.P(35) ? 1u : 0u;
        a[9] = g.W();
        a[10] = a[7] < a[4] ? g.Pk({0u, 0u, 0u, 0u, 2u}) : g.Pk({1u, 1u, 2u});
        a[11] = kKind[c.fn] == kDRem ? g.Pk({0u, 0u, 0u, 1u, 2u}) : 0u;
        a[12] = AuxOffset(g, 0x20);
        break;
    }
    case kScalarDel:
        a[4] = g.Pk({0u, 1u, 2u, 3u, g.W(), g.W() | 1u, g.W() & ~1u, 0xFFFFFF01u, 0x100u, 0x101u, 0xFFu});
        a[5] = g.W();
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
