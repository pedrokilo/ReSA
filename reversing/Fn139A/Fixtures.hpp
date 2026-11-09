#pragma once
// Fixtures del banco Fn139A (lote 139A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                              forma
//   0   0x4AE390   CEventPotentialWalkIntoVehicle::GetEventType           thiscall() `ret`: mov eax,38h        (vtable 0x85AED0, ranura 1)
//   1   0x4AE3A0   CEventPotentialWalkIntoVehicle::GetLifeTime            thiscall() `ret`: xor eax,eax        (ranura 3)
//   2   0x4AE3B0   CEventPotentialWalkIntoVehicle::GetEventPriority       thiscall() `ret`: mov eax,1Dh        (ranura 2)
//   3   0x4AE5C0   CEventPotentialWalkIntoVehicle::TakesPriorityOver      thiscall(event&) `ret 4`: mov al,1   (ranura 11)
//   4   0x4B0D70   CEventVehicleDied::GetEventType                        thiscall() `ret`: mov eax,23h        (vtable 0x85B400, ranura 1)
//   5   0x4B0D80   CEventVehicleDied::GetLifeTime                         thiscall() `ret`: xor eax,eax        (ranura 3)
//   6   0x4B0D90   CEventVehicleDied::GetEventPriority                    thiscall() `ret`: mov eax,48h        (ranura 2)
//   7   0x4B0EE0   CEventLeaderEnteredCarAsDriver::TakesPriorityOver      thiscall(event&) `ret 4`: mov al,1   (vtable 0x85A198, ranura 11; el listado la daba por de CEventVehicleDied)
//   8   0x56F200   CWanted::DestructorBody (cuerpo de ~CWanted)           thiscall() `ret`: add ecx,21Ch; jmp 4E6E00 (~CAEPoliceScannerAudioEntity; el listado decia CPlayerInfo)
//   9   0x571970   stub de script (clase desconocida, listado: CPlayerInfo)  thiscall(a, b, c) `ret 0Ch`: xor al,al
//  10   0x571980   stub de script (idem)                                  thiscall(a, b, c) `ret 0Ch`: xor al,al
//  11   0x6300B0   CTaskComplexDie::GetTaskType                           thiscall() `ret`: mov eax,0D9h       (vtable 0x86DE78, ranura 4)
//  12   0x6300C0   CTaskComplexDie::Destructor (cuerpo)                    thiscall() `ret`: mov [ecx],86DE78h; jmp 61A3D0 (~CTaskComplex)
//  13   0x630580   CTaskComplexDie::ControlSubTask                        thiscall(ped) `ret 4`: mov eax,[ecx+8]  (ranura 10)
//  14   0x4B1960   CEventVehicleDamage::GetLifeTime                       thiscall() `ret`: xor eax,eax        (vtable 0x85B588, ranura 3)
//  15   0x4B1970   CEventVehicleDamage::GetLocalSoundLevel                thiscall() `ret`: fld dword [85ADBCh] (55.0f en ST0; ranura 12)
//  16   0x4B1980   CEventVehicleDamage::GetEventPriority                  thiscall() `ret`: mov eax,31h        (ranura 2)
//  17   0x4B1B50   CEventSpecial::Destructor (cuerpo)                     thiscall() `ret`: mov [ecx],85AB38h (el llamador 0x4B6750 usa ECX despues; el listado decia CEventVehicleDamage)
//  18   0x4C56E0   CLodTimeModelInfo::Destructor (cuerpo)                 thiscall() `ret`: mov [ecx],85BB9Ch
//  19   0x4C56F0   CClumpModelInfo::Constructor (el listado decia CLodTimeModelInfo)  thiscall() `ret`: call 4C4A60 (+8 = 0, +0xA = 0xFFFF); mov [esi],85BD30h; mov eax,esi
//  20   0x4B2030   CEventGunShotWhizzedBy::GetEventType                   thiscall() `ret`: mov eax,31h        (vtable 0x85B730, ranura 1)
//  21   0x4B2040   CEventGunShotWhizzedBy::GetLifeTime                    thiscall() `ret`: xor eax,eax        (ranura 3)
//  22   0x4B2050   CEventGunShotWhizzedBy::CanBeInterruptedBySameEvent    thiscall() `ret`: mov al,1           (ranura 15)
//  23   0x4B2060   CEventGunShotWhizzedBy::GetEventPriority               thiscall() `ret`: mov eax,24h        (ranura 2)
//  24   0x4B2070   CEventGunShotWhizzedBy::Destructor (cuerpo)            thiscall() `ret`: mov [ecx],85B730h; jmp 4AC720 (~CEventGunShot)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40).
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

namespace F139AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Bytes que cada funcion toca a partir de su puntero `this` (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable (o nada)
constexpr u32 kSpanWanted = 0x220;    // CWanted::DestructorBody: ECX = this + 0x21C (un puntero a un dword)
constexpr u32 kSpanSubTask = 0x0C;    // CTaskComplexDie::ControlSubTask lee la subtarea en +8
constexpr u32 kSpanCtor = 0x0C;       // CClumpModelInfo::Constructor escribe +0 (dword), +8 (word) y +0xA (word)

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
    std::array<u32, 16> a{};
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

// Indices (ver la cabecera)
inline constexpr unsigned kIdxPwvPriorityOver = 3, kIdxLeaderPriorityOver = 7, kIdxWanted = 8, kIdxStubA = 9, kIdxStubB = 10, kIdxDieDtor = 12, kIdxDieControl = 13, kIdxSoundLevel = 15,
                          kIdxSpecialDtor = 17, kIdxLodDtor = 18, kIdxClumpCtor = 19, kIdxCanBeInterrupted = 22, kIdxGunShotDtor = 24;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y reserva; el resto por funcion
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (c.fn) {
    case kIdxPwvPriorityOver: case kIdxLeaderPriorityOver:   // TakesPriorityOver (evento): a[3] = argumento crudo (no se desreferencia), a[4] = dword que ocupa [this]
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kIdxWanted:   // a[4] = EAX del falso de ~CAEPoliceScannerAudioEntity (0x4E6E00)
        c.a[0] = Offset(g, kind, kSpanWanted);
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kIdxStubA: case kIdxStubB:   // a[3], a[4], a[5] = argumentos de pila (id de 16 bits en el EXE, 1, cadena): crudos; a[6] = 0: ECX es el objeto de la arena, 1: ECX crudo (el del caso)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, 0xFFFFFFFFu, g.R(0x10000), g.W(), g.W()});
        c.a[4] = g.Pk({1u, 0u, 2u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W()});
        c.a[5] = g.Pk({0x859E40u, 0u, 0xFFFFFFFFu, Raw(g), g.W(), g.W()});
        c.a[6] = g.R(2);
        c.a[7] = Raw(g);   // dword que ocupa [this] (los stubs no lo leen)
        break;
    case kIdxDieDtor:   // a[3] = vtable previa en [this]; a[4] = EAX del falso de ~CTaskComplex (0x61A3D0)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x86DE78u, 0x86D4CCu, 0x86D48Cu, 0x86DE79u, 0x86DE74u, 0x870404u, 0x870520u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        break;
    case kIdxGunShotDtor:   // a[3] = vtable previa; a[4] = EAX del falso de ~CEventGunShot (0x4AC720)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85B730u, 0x85ABE0u, 0x85AB38u, 0x85B731u, 0x85B72Cu, 0x85AB80u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        break;
    case kIdxSpecialDtor:   // a[3] = vtable previa
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85B5D0u, 0x85AB80u, 0x85AB38u, 0x85AB39u, 0x85B5CCu, g.W(), g.W(), 0xDEADBEEFu});
        break;
    case kIdxLodDtor:       // a[3] = vtable previa
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85BCF0u, 0x85BB9Cu, 0x85BD30u, 0x85BB9Du, 0x85BB98u, g.W(), g.W(), 0xDEADBEEFu});
        break;
    case kIdxDieControl:    // a[3] = dword en +8 (m_pSubTask), a[4] = ped (crudo; no se lee), a[5] = dword en +0 (vtable)
        c.a[0] = Offset(g, kind, kSpanSubTask);
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kIdxClumpCtor:     // a[3] = dword previo en +0, a[4] = dword previo en +8 (el constructor escribe los words +8 y +0xA)
        c.a[0] = Offset(g, kind, kSpanCtor);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85BD30u, 0x85BB9Cu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 0xFFFF0000u, 0xFFFFFFFFu, 0x0000FFFFu, 0x12345678u, g.W(), g.W()});
        break;
    default:         // constantes (0..2, 4..6, 11, 14, 16, 20, 21, 23), GetLocalSoundLevel (15) y CanBeInterruptedBySameEvent (22): `this` en la arena A
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);   // dword en [this] (el EXE no lo lee)
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
