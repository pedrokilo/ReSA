#pragma once
// Fixtures del banco Fn201A (lote 201A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 32 asignadas: 32 reconstruidas.
//   idx  direccion  nombre real                                                                         forma
//    0..9  0x54FC40, 0x54FC90, 0x54FCE0, 0x54FD30, 0x54FD80, 0x54FDD0, 0x54FE20, 0x54FE70, 0x54FEC0, 0x54FF10  (el listado decia CPlaceable)
//                                                                                                       thiscall() `ret`: si [this+8] (int32 CON SIGNO) > 0: si BYTE [this+0x10] != 0 delete[] [this] y delete[] [this+4] (CRT 0x8213AE); [this], [+4], [+8], [+0xC] = 0 (codigo muerto, diez copias identicas)
//   10   0x6956B0   CTaskComplexSeekEntityAnyMeansXYOffset destructor con borrado (ranura 0)            thiscall(flags) `ret 4`: cuerpo 6956D0; CTask::operator delete 61A5B0 si bit 0
//   11   0x6956D0   CTaskComplexSeekEntityAnyMeansXYOffset cuerpo del destructor                        thiscall() `ret`: [this]=870CF0h; si [+0xC] != 0: CleanUpOldReference 571A00(ecx=entidad, &[+0xC]); [+0x24]=859DC4h; call 61A3D0
//   12   0x696260   CTaskComplexFollowPedFootsteps destructor con borrado (el listado decia XYOffset)   como 10 con el cuerpo 695DC0
//   13   0x6962A0   CTaskComplexFollowLeaderInFormation::CreateSubTask (el listado decia XYOffset)      thiscall(tipo, ped) `ret 8`: subtarea segun el tipo (ver fixture_oracle.py)
//   14   0x4B1130   CEventVehicleOnFire::GetEventType                                                   mov eax,4Fh
//   15   0x4B1140   CEventVehicleOnFire::GetLifeTime                                                    xor eax,eax
//   16   0x4B1150   CEventVehicleOnFire::GetLocalSoundLevel                                             fld dword [858B34h] = 60.0f (ST0)
//   17   0x4B1160   CEventVehicleOnFire::GetEventPriority                                              mov eax,36h
//   18   0x4B77B0   CEventHitByWaterCannon::Clone (vtable 0x85B488)                                    sin constructor: new 4B5570 (pool 0xB744AC); [+4]=0; [+8]=0; [mem]=85B488h; copia +0xC..+0x23
//   19   0x4B7810   CEventInWater::Clone (vtable 0x85B4C8)                                             sin constructor: new; [+4]=0; fld/fstp [+0xC]; [+8]=0; [mem]=85B4C8h
//   20   0x4B7840   CEventStuckInAir::Clone                                                             new; ctor 4B1490([+0xC])
//   21   0x4B78B0   CEventCopCarBeingStolen::Clone                                                      new; ctor 4B1740([+0xC], [+0x10])
//   22   0x4B7960   CEventGotKnockedOverByCar::CloneEditable (el listado decia VehicleOnFire)           new; ctor 4B1B60([+0x14])
//   23   0x4B79D0   CEventCarUpsideDown::Clone                                                          new; ctor 4B1CC0([+0xC])
//   24   0x4B7A40   CEventPotentialWalkIntoFire::CloneEditable                                          new; ctor 4B1E20(&[+0x14], [+0x20], [+0x28])
//   25   0x4B7A70   CEventFireNearby::CloneEditable (vtable 0x85B6E8)                                   sin constructor: new; [+4]=0; [+8]=0; [+0xC]=1; word [+0xE]=0C8h; word [+0x10]=0FFFFh; [mem]=85B6E8h; copia +0x14..+0x1F
//   26   0x4B7AC0   CEventGunShotWhizzedBy::CloneEditable                                              new; ctor 4B1FD0([+0x14], &[+0x18], &[+0x24], BYTE [+0x30])
//   27   0x4B7B30   CEventSeenPanickedPed::CloneEditable                                                new; ctor 4B2080([+0x14])
//   28   0x4B7BA0   CEventAreaCodes::Clone                                                              new; ctor 4B2190([+0xC])
//   29   0x4B7C10   CEventDanger::CloneEditable                                                         new; ctor 4B2600([+0x14], [+0x18])
//   30   0x4B7C80   CEvent destructor con borrado (ranura 0, vtable 0x85AB38)                           thiscall(flags) `ret 4`: [this]=85AB38h; con bit 0 liberacion EN LINEA en el pool 0xB744AC (indice = (this - almacenamiento) / 0x44 con signo)
//   31   0x4AC8F0   CEventVehicleCollision::GetEventType                                                mov eax,1
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

namespace F201AFix {
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
    std::array<u32, 64> a{};
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
enum Kind : u32 { kArr, kConst, kConstF, kSdd, kXyBody, kFormSub, kCloneDirect, kCloneCtor, kEvSdd };
inline constexpr std::array<Kind, kFunctions> kKind{kArr, kArr, kArr, kArr, kArr, kArr, kArr, kArr, kArr, kArr, kSdd, kXyBody, kSdd, kFormSub, kConst, kConst, kConstF, kConst,
                                                    kCloneDirect, kCloneDirect, kCloneCtor, kCloneCtor, kCloneCtor, kCloneCtor, kCloneCtor, kCloneDirect, kCloneCtor, kCloneCtor, kCloneCtor, kCloneCtor, kEvSdd, kConst};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x54FC40, 0x54FC90, 0x54FCE0, 0x54FD30, 0x54FD80, 0x54FDD0, 0x54FE20, 0x54FE70, 0x54FEC0, 0x54FF10, 0x6956B0, 0x6956D0, 0x696260, 0x6962A0, 0x4B1130, 0x4B1140,
                                                    0x4B1150, 0x4B1160, 0x4B77B0, 0x4B7810, 0x4B7840, 0x4B78B0, 0x4B7960, 0x4B79D0, 0x4B7A40, 0x4B7A70, 0x4B7AC0, 0x4B7B30, 0x4B7BA0, 0x4B7C10, 0x4B7C80, 0x4AC8F0};
// vtable que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x870CF0, 0x870CF0, 0x870CC0, 0x870C3C, 0x85B440, 0x85B440, 0x85B440, 0x85B440, 0x85B488, 0x85B4C8, 0x85B508, 0x85B548, 0x85B618, 0x85B660,
                                                     0x85B6A0, 0x85B6E8, 0x85B730, 0x85B778, 0x85B7C0, 0x85B848, 0x85AB38, 0x85AC28};
// Bytes que cada funcion lee/toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x28, 0x28, 0x28, 0x20, 4, 4, 4, 4, 0x24, 0x10, 0x10, 0x14, 0x18, 0x10, 0x2C, 0x20, 0x34, 0x18, 0x10, 0x1C, 4, 4};
// Un objeto de pool de eventos mide 0x44 bytes; un ped sintetico ocupa 0x5A0 (se leen +4, +0x14, +0x558 y +0x58C)
inline constexpr u32 kSlot = 0x44, kPedSpan = 0x5A0;
// Memoria global del juego tocada: puntero del pool de eventos
inline constexpr u32 kEventPoolPtr = 0xB744AC;
// Tipos de tarea de CreateSubTask
inline constexpr u32 kT_StandStill = 0xCB, kT_Pause = 0xCA, kT_Enter = 700, kT_Leave = 0x2C0, kT_Drive = 0x2C5, kT_Seek = 0x38B, kT_Heading = 0x386;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D
//   kConst, kConstF: nada mas
//   kArr: a[4] = contador (+8, int32), a[5] = puntero A (+0), a[6] = puntero B (+4), a[7] = entero (+0xC), a[8] = dword de +0x10 (el BYTE bajo es el "es dueno")
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kXyBody: a[4] = modo de la entidad (+0xC): 0 nula, 1 cruda (a[5]), 2 en B (a[6]); a[5] = valor crudo, a[6] = desplazamiento en B, a[7] = dword previo en +0x24, a[8] = EAX del falso de 0x61A3D0, a[9] = dword que deja en [this]
//   kCloneDirect/kCloneCtor: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = EAX del falso del constructor, a[7] = dword que el falso deja en [mem], a[8] = desplazamiento del puntero del pool en D,
//         a[10 + off/4] = dword del objeto copiado en el desplazamiento `off` (4 <= off < span)
//   kEvSdd: a[4] = capacidad del pool, a[5] = indice del objeto (con signo), a[6] = resto 0..0x43, a[7] = desplazamiento de la estructura del pool en B, a[8] = desplazamiento de los estados en D, a[9] = ultimo libre (+0xC), a[10] = banderas (pila)
//   kFormSub: ver los comentarios del generador
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
    case kArr:
        a[4] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 3u, 4u, 0xFFFFFFFEu, g.R(100), g.W(), g.W() | 0x80000000u, g.R(1000) + 1u});
        a[5] = g.Pk({0u, 1u, 0xFFFFFFFFu, Raw(g), Raw(g), Raw(g), 1u});
        a[6] = g.Pk({0u, 0xFFFFFFFFu, Raw(g), Raw(g), Raw(g), 2u});
        a[7] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W(), 0x80000000u});
        a[8] = g.Pk({0u, 1u, 0xFFu, 0x80u, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 2u, g.W(), g.W(), g.W() | 1u, g.W() & ~0xFFu});
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kXyBody:
        a[4] = g.Pk({0u, 1u, 1u, 2u, 2u});
        a[5] = Raw(g);
        a[6] = AuxOffset(g, 0x10);
        a[7] = g.Pk({0u, 0xFFFFFFFFu, 0x859DC4u, g.W(), g.W(), 0xDEADBEEFu});
        a[8] = Raw(g); a[9] = Raw(g);
        break;
    case kCloneDirect:
    case kCloneCtor: {
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, kSlot);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[7] = Raw(g);
        a[8] = 0x800u + 4u * g.R(0x40);   // el puntero del pool apunta a esta direccion de D (solo se etiqueta, nunca se lee)
        // dwords del objeto copiado: flotantes interesantes para los campos que el EXE copia sin aritmetica (vectores, aceleracion), crudos para el resto
        const bool floats = c.fn == 18 || c.fn == 19 || c.fn == 25;
        for (u32 off = 4; off < kSpan[c.fn]; off += 4) a[10 + off / 4] = floats ? FloatBits(g) : (off == 0x20 && c.fn == 24 ? FloatBits(g) : Raw(g));
        if (c.fn == 19 && g.P(30)) a[10 + 3] = 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u) | (g.P(50) ? 0x80000000u : 0u);   // aceleracion (+0xC): NaN senalizante
        break;
    }
    case kEvSdd: {
        const u32 cap = g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 48u, 60u});
        a[4] = cap;
        const i32 idx = g.P(8) ? i32(g.R(3)) - 3 : (g.P(8) ? i32(cap) + i32(g.R(3)) : i32(g.R(cap)));
        a[5] = u32(idx);
        a[6] = g.Pk({0u, 0u, 1u, 0x43u, 0x22u, g.R(0x44)});
        a[7] = AuxOffset(g, 0x10);
        a[8] = 8u + 4u * g.R(0x1C0);   // estados: indices de -8 a cap+8 caben en la ventana
        a[9] = g.Pk({0xFFFFFFFFu, 0u, u32(idx - 1), u32(idx), u32(idx + 1), cap - 1u, cap, cap + 5u, g.R(cap), g.R(cap), 0x7FFFFFFFu, 0x80000000u, 0x80000001u});
        a[10] = FlagsDword(g);
        break;
    }
    case kFormSub: {
        // tipo de subtarea
        a[4] = g.Pk({kT_StandStill, kT_StandStill, kT_Pause, kT_Pause, kT_Enter, kT_Enter, kT_Leave, kT_Leave, kT_Drive, kT_Drive, kT_Seek, kT_Seek, kT_Heading, kT_Heading, kT_Heading, kT_Heading, kT_Heading, kT_Heading,
                     0x516u, 0x516u, 0u, 1u, 0xC9u, 0xCCu, 0x2BBu, 0x2BDu, 0x2C1u, 0x2C4u, 0x2C6u, 0x385u, 0x387u, 0x38Au, 0x38Cu, 0x517u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x100000CAu, 0x800002C5u, g.W(), g.R(0x800)});
        const u32 type = a[4];
        const bool needsPed = type == kT_Leave || type == kT_Drive || type == kT_Heading;
        a[5] = needsPed ? 1u : g.Pk({0u, 1u, 1u});                      // ped crudo (solo se compara) o en B
        a[6] = 4u * g.R((0x800u - kPedSpan) / 4u);                      // ped en B (primera mitad)
        a[7] = Raw(g);                                                  // ped crudo
        a[8] = 4u * g.R((0x800u - kPedSpan) / 4u);                      // otro ped en C (primera mitad)
        a[9] = 0x800u + 4u * g.R(0x20);                                 // grupo en D (segunda mitad; solo se etiqueta group + 8)
        a[10] = type == kT_Enter ? 2u : g.Pk({0u, 1u, 2u, 2u, 2u});     // lider: nulo, crudo o en D
        a[11] = 4u * g.R((0x800u - kPedSpan) / 4u);                     // lider en D (primera mitad)
        a[12] = FloatBits(g); a[13] = FloatBits(g); a[14] = FloatBits(g);   // m_Pos (+0x14)
        a[15] = g.Pk({0u, 1u});                                         // matriz del ped
        a[16] = 0x800u + 4u * g.R(0x10);                                // matriz del ped en B (segunda mitad)
        a[17] = FloatBits(g); a[18] = FloatBits(g); a[19] = FloatBits(g);   // posicion propia del ped (+4)
        a[20] = FloatBits(g);                                           // rotacion (+0x558)
        a[21] = Raw(g);                                                 // vehiculo del ped (+0x58C)
        a[22] = FloatBits(g); a[23] = FloatBits(g); a[24] = FloatBits(g);   // posicion de la matriz del ped (+0x30)
        a[25] = Raw(g);                                                 // vehiculo del lider (+0x58C)
        a[26] = g.Pk({0u, 1u});                                         // matriz del otro ped
        a[27] = 0x800u + 4u * g.R(0x10);                                // matriz del otro ped en C (segunda mitad)
        a[28] = FloatBits(g); a[29] = FloatBits(g); a[30] = FloatBits(g);   // posicion propia del otro ped
        a[31] = FloatBits(g); a[32] = FloatBits(g); a[33] = FloatBits(g);   // posicion de la matriz del otro ped
        a[34] = Raw(g);   // lider crudo
        a[35] = type == kT_Seek ? 1u : g.Pk({0u, 1u, 1u, 1u});          // memoria de CTask::operator new (el EXE caeria al escribir en nulo + 0x44 en la rama 0x38B)
        a[40] = 0x900u + 4u * g.R(0x180);                               // memoria en C
        a[41] = type == kT_Seek ? g.Pk({0u, 0u, 1u}) : g.Pk({0u, 0u, 1u, 2u});                                 // EAX del constructor: la memoria, otro objeto en C o crudo
        a[42] = 0x900u + 4u * g.R(0x180);
        a[43] = Raw(g);
        a[44] = Raw(g);                                                 // dword que el constructor falso deja en [memoria]
        a[45] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 8u, 16u, 40u, 0xFFFFFFFFu, 0xFFFFFFFEu, g.R(8), g.R(8)});   // CountMembers
        a[46] = g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 0xFFFF0000u, 0xFFFFFFFFu, g.W(), g.W(), g.R(0x10000), g.R(0x10000)});
        // segunda tirada: casi la mitad de las veces sale 20 (rand_in_range(r, 0, 50) == 20 para r & 0xFFFF en [13108, 13762])
        a[47] = g.P(45) ? 13108u + g.R(655) + (g.P(30) ? (g.W() & 0xFFFF0000u) : 0u) : g.Pk({0u, 13107u, 13763u, 0xFFFFu, g.W(), g.R(0x10000), g.R(0x10000)});
        a[48] = g.Pk({0u, 1u, 2u, 2u, 2u});                             // GetMember: nulo, el propio ped o el otro
        a[49] = a[5] == 0 ? g.Pk({0u, 1u, 1u}) : g.Pk({0u, 1u, 1u, 2u});                                 // GetLeader: nulo, el otro o el propio ped
        for (u32 i = 0; i < 3; ++i) { a[50 + i] = FloatBits(g); a[53 + i] = FloatBits(g); }   // salidas de VectorSubtract y de Normalise
        a[56] = FloatBits(g); a[57] = FloatBits(g);                    // GetRadianAngleBetweenPoints y LimitRadianAngle
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
