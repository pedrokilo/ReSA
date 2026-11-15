#pragma once
// Fixtures del banco Fn190A (lote 190A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 28 direcciones: 25 reconstruidas y 3 saltadas.
//   idx  direccion  nombre real                                                                      forma
//    0   0x420F00   CEventAcquaintancePedHate::GetLifeTime (vtable 0x858E68, ranura 3)                thiscall() `ret`: xor eax,eax
//    1   0x420F10   CEventAcquaintancePedHate::GetSourceEntity (ranura 10)                           thiscall() `ret`: mov eax,[ecx+14h]
//    2   0x420F20   CEventAcquaintancePedHate::CanBeInterruptedBySameEvent (ranura 15)                thiscall() `ret`: mov al,1 (solo AL definido)
//    3   0x420F30   CEventAcquaintancePedHate cuerpo del destructor                                   thiscall() `ret`: [ecx]=858E68h; jmp 4AF890
//    4   0x420F40   CEventAcquaintancePedHate::GetEventType (ranura 1)                                thiscall() `ret`: mov eax,24h
//    5   0x420F50   CEventAcquaintancePedHate::CloneEditable (ranura 16)                              thiscall() `ret`: new 4B5620 (18h); ctor 4AF820 (ped de +14h); [mem]=858E68h; eax = mem
//    6   0x420FB0   CEventAcquaintancePedHate destructor con borrado (ranura 0)                       thiscall(flags) `ret 4`: call 420F30; CEvent::operator delete 4B5630 si bit 0
//    7   0x420FD0   CEventAcquaintancePedHate::GetEventPriority (ranura 2)                            thiscall() `ret`: mov eax,1Ah
//    8   0x4B58E0   CEventGunShot destructor con borrado (vtable 0x85ABE0)                            thiscall(flags) `ret 4`: cuerpo 4AC720; operator delete de CEvent EN LINEA (pool 0xB744AC)
//    9   0x4B5930   CEventVehicleCollision destructor con borrado (0x85AC28)                         idem; cuerpo 4AC920
//   10   0x4B5980   CEventPedCollisionWithPed destructor con borrado (0x85AC68)                      idem; cuerpo 4ACA70
//   11   0x4B59D0   CEventObjectCollision destructor con borrado (0x85ACA8)                           idem; cuerpo 4ACDD0
//   12   0x4B5A20   CEventBuildingCollision destructor con borrado (0x85ACE8)                         idem; cuerpo 4ACFF0
//   13   0x4B5A70   CEventDraggedOutCar destructor con borrado (0x85AD30)                            idem; cuerpo 4AD330
//   14   0x622180   CTaskComplexKillAllThreats constructor (vtable 0x86DA1C)                          thiscall(3 dwords) `ret 0Ch`: base 61A3B0; +0Ch, vtable, +10h, +14h; eax = this
//   15   0x6221B0   CTaskComplexKillAllThreats::GetTaskType (ranura 4)                               thiscall() `ret`: mov eax,3F6h
//   16   0x6221C0   CTaskComplexKillAllThreats cuerpo del destructor                                  thiscall() `ret`: [ecx]=86DA1Ch; jmp 61A3D0 (borra la subtarea)
//   17   0x6221D0   CTaskComplexKillAllThreats::ScanForThreat                                         thiscall(ped) `ret 4`: CPedAcquaintanceScanner local; ScanEntities 607A90; devuelve la primera salida
//   18   0x622230   CTaskComplexKillAllThreats::ControlSubTask (ranura 10)                            thiscall(ped) `ret 4`: mov eax,[ecx+8]
//   19   0x6293A0   CTaskComplexKillAllThreats destructor con borrado (ranura 0)                      thiscall(flags) `ret 4`: call 6221C0; CTask::operator delete 61A5B0 si bit 0
//   20   0x6293C0   CTaskComplexKillAllThreats::CreateNextSubTask (ranura 8)                          thiscall(ped) `ret 4`: tipo de la subtarea (ranura 4); 3E8: ScanForThreat; new(38h); ctor 620E30
//   21   0x629480   CTaskComplexKillAllThreats::CreateFirstSubTask (ranura 9)                         thiscall(ped) `ret 4`: ScanForThreat; amenaza: como 20; si no new(20h) + ctor de StandStill 62F310
//   22   0x629550   CTaskComplexKillPedGroupOnFoot destructor con borrado (vtable 0x86DA48)          thiscall(flags) `ret 4`: call 6222B0; CTask::operator delete 61A5B0 si bit 0
//   23   0x710BA0   CStencilShadows ayuda FindForOwner                                                cdecl(propietario): recorre 0xC6A16C por +14h comparando [nodo]
//   24   0x711280   CStencilShadowObject::Create                                                     thiscall(entidad, tipo BYTE) `ret 8`: GetColModel 535300; Malloc 72F420; RegisterReference 571B70; UpdateHierarchy 710BC0
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

namespace F190AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;
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

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kConstAl, kLoadField, kHateBody, kHateClone, kSdd, kEvSdd, kKatCtor, kKatBody, kScan, kCtrlSub, kNext, kFirst, kFind, kCreate };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kLoadField, kConstAl, kHateBody, kConst, kHateClone, kSdd, kConst, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kKatCtor, kConst, kKatBody,
                                                    kScan, kCtrlSub, kSdd, kNext, kFirst, kSdd, kFind, kCreate};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x420F00, 0x420F10, 0x420F20, 0x420F30, 0x420F40, 0x420F50, 0x420FB0, 0x420FD0, 0x4B58E0, 0x4B5930, 0x4B5980, 0x4B59D0, 0x4B5A20, 0x4B5A70,
                                                    0x622180, 0x6221B0, 0x6221C0, 0x6221D0, 0x622230, 0x6293A0, 0x6293C0, 0x629480, 0x629550, 0x710BA0, 0x711280};
// Valor que devuelven las funciones constantes (por indice; 0 si no es constante)
inline constexpr std::array<u32, kFunctions> kConstValue{0, 0, 1, 0, 0x24, 0, 0, 0x1A, 0, 0, 0, 0, 0, 0, 0, 0x3F6, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x858E68, 0x858E68, 0x858E68, 0x858E68, 0x858E68, 0x858E68, 0x858E68, 0x858E68, 0x85ABE0, 0x85AC28, 0x85AC68, 0x85ACA8, 0x85ACE8, 0x85AD30,
                                                     0x86DA1C, 0x86DA1C, 0x86DA1C, 0x86DA1C, 0x86DA1C, 0x86DA1C, 0x86DA1C, 0x86DA1C, 0x86DA48, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x18, 4, 4, 4, 0x18, 4, 4, 4, 4, 4, 4, 4, 4, 0x18, 4, 0x18, 4, 0x18, 4, 0x18, 0x18, 4, 4, 0x1C};
// Un ped sintetico ocupa 0x500 bytes en la arena B (se lee +0x47C); un nodo de sombra se coloca cada 0x40 bytes
inline constexpr u32 kPedSpan = 0x500, kNodeStride = 0x40, kMaxNodes = 12;
// Tamano de un elemento del pool de eventos (CEventDamage) y de la tabla de funciones virtuales sintetica de las subtareas (20 ranuras)
inline constexpr u32 kEventElem = 0x44, kVtabBytes = 0x50;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D
//   kConst, kConstAl: nada mas (EAX / EAX&0xFF sembrado por el caso)
//   kLoadField: a[4] = dword en this+0x14
//   kHateBody: a[4] = EAX del falso (cuerpo de CEventAcquaintancePed 4AF890), a[5] = dword que el falso deja en [this]
//   kHateClone: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = EAX del falso del constructor 4AF820, a[7] = dword que el falso deja en [mem], a[8] = ped (this+0x14, crudo)
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
//   kEvSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas, a[7] = this - almacenamiento del pool (int32), a[8] = desplazamiento del mapa de estados en C,
//         a[9] = cursor inicial del pool, a[10] = desplazamiento de la estructura del pool en D
//   kKatCtor: a[4..6] = los tres dwords de pila
//   kKatBody: a[4] = subtarea (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = desplazamiento de la tabla virtual en D
//   kScan: a[4] = desplazamiento del ped en B, a[5] = inteligencia (0 cruda, 1 en C), a[6] = desplazamiento en C, a[7] = valor crudo, a[8] = valor que el falso de ScanEntities deja en la primera salida
//   kCtrlSub: a[4] = dword en this+8, a[5] = ped (pila, crudo)
//   kNext, kFirst: a[4] = desplazamiento de la subtarea en B, a[5] = desplazamiento de la tabla virtual en D, a[6] = tipo que devuelve la ranura 4 de la subtarea, a[7] = amenaza (devuelve el falso de ScanForThreat),
//         a[8] = memoria (0 nula, 1 en B), a[9] = desplazamiento de la memoria en B, a[10] = EAX del falso del constructor, a[11] = dword que el falso deja en [mem], a[12] = ped (pila, crudo),
//         a[13..15] = this+0xC, +0x10, +0x14
//   kFind: a[4] = numero de nodos (0..12), a[5] = indice del nodo cuyo propietario es el buscado (== numero de nodos: ninguno), a[6] = propietario buscado (pila), a[7..18] = propietarios de los nodos,
//         a[19..30] = (arena << 16 | desplazamiento) de cada nodo (arena 0 = B, 1 = C)
//   kCreate: a[4] = tipo (pila, solo cuenta el BYTE), a[5] = entidad (pila, cruda), a[6] = desplazamiento del modelo de colision en C, a[7] = datos de colision (0 nulos, 1 en D), a[8] = desplazamiento en D,
//         a[9] = byte en +7 de los datos, a[10] = numero de triangulos de sombra (+0x20), a[11] = memoria que devuelve el falso de CMemoryMgr::Malloc
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
    case kConstAl:
        break;
    case kLoadField:
        a[4] = Raw(g);
        break;
    case kHateBody:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kHateClone:
        mem(0x18);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[7] = Raw(g);
        a[8] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kEvSdd: {
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        const u32 mapOff = g.P(25) ? g.Pk({0u, 0x10u, 0xFFFu, 0x800u, 0xF00u}) : g.R(0x1000);
        const i32 lo = -i32(mapOff) - 0x30, hi = 0xFFF - i32(mapOff) + 0x30;
        i32 idx = i32(g.Pk({0u, 1u, 0xFFFFFFFFu, 2u, 0xFFFFFFFEu, g.R(200), u32(-i32(g.R(200))), g.R(3000), u32(-i32(g.R(3000))), u32(lo), u32(hi), u32(lo + 1), u32(hi - 1), g.R(40), u32(-i32(g.R(40)))}));
        idx = std::clamp(idx, lo, hi);
        const u32 r = g.R(68);
        const i32 delta = idx > 0 ? idx * 68 + i32(r) : (idx < 0 ? idx * 68 - i32(r) : (g.P(50) ? i32(r) : -i32(r)));
        a[7] = u32(delta);
        a[8] = mapOff;
        a[9] = g.Pk({u32(idx), u32(idx + 1), u32(idx - 1), 0u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W(), u32(idx + i32(g.R(10)) - 5), 1000u, u32(idx + 2), u32(idx - 2), 0x100u});
        a[10] = AuxOffset(g, 0x10);
        break;
    }
    case kKatCtor:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kKatBody:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = AuxOffset(g, kVtabBytes);
        break;
    case kScan:
        a[4] = AuxOffset(g, kPedSpan);
        a[5] = g.Pk({0u, 1u, 1u, 1u});
        a[6] = AuxOffset(g, 0x10);
        a[7] = Raw(g);
        a[8] = g.Pk({0u, 0u, 0xFFFFFFFFu, 1u, g.W(), g.W(), 0x00B74000u + g.R(0x1000), Raw(g)});
        break;
    case kCtrlSub:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kNext:
    case kFirst:
        a[4] = AuxOffset(g, 0x10);
        a[5] = AuxOffset(g, kVtabBytes);
        a[6] = g.Pk({0x3E8u, 0x3E8u, 0x3E8u, 0x3E8u, 0xCBu, 0xCBu, 0u, 1u, 0xFFFFFFFFu, 0x3E7u, 0x3E9u, 0x1F4u, 0x7FFFFFFFu, 0x80000000u, 0x10003E8u, 0x800003E8u, g.W(), g.R(0x800)});
        a[7] = g.Pk({0u, 0u, 1u, 0xFFFFFFFFu, g.W(), g.W(), 0x00B74000u + g.R(0x1000), Raw(g)});
        a[8] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[9] = AuxOffset(g, 0x40);
        a[10] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[11] = Raw(g);
        a[12] = Raw(g);
        a[13] = Raw(g); a[14] = Raw(g); a[15] = Raw(g);
        break;
    case kFind: {
        const u32 cnt = g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 12u, g.R(13), g.R(13)});
        a[4] = cnt;
        const u32 hit = g.P(20) ? cnt : g.R(cnt + 1);
        a[5] = hit;
        a[6] = Raw(g);
        for (u32 i = 0; i < kMaxNodes; ++i) {
            a[7 + i] = Raw(g);
            const u32 arena = g.R(2);
            a[19 + i] = (arena << 16) | (i * kNodeStride + g.R(0x20) / 4 * 4);
        }
        for (u32 i = 0; i < cnt; ++i) {
            if (i == hit) a[7 + i] = a[6];                      // el propietario buscado
            else if (a[7 + i] == a[6] && g.P(70)) a[7 + i] ^= 0x10u;   // relleno distinto del buscado salvo colision deliberada (el primero que coincida gana)
        }
        break;
    }
    case kCreate:
        a[4] = g.Pk({1u, 2u, 1u, 2u, 1u, 2u, 0u, 3u, 0x101u, 0x102u, 0x100u, 0x200u, 0x80u, 0xFFu, 0xFFFFFF01u, 0xFFFFFF02u, 0x80000001u, g.W(), g.R(4), 0x201u});
        a[5] = Raw(g);
        a[6] = AuxOffset(g, 0x40);
        a[7] = g.Pk({0u, 1u, 1u, 1u, 1u, 1u});
        a[8] = AuxOffset(g, 0x40);
        a[9] = g.Pk({4u, 4u, 4u, 0xFFu, 0u, 3u, 8u, 0x14u, 0xFBu, g.R(256), 0x04u, 0xFCu});
        a[10] = g.Pk({1u, 2u, 3u, 4u, 100u, 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x10000u, 0xFFFFu, 0x8000u, g.R(1000), g.R(100000), g.W(), 0x2AAAAAAu, 0x2AAAAABu, 0x11111111u, 0x1000000u, g.R(20), g.R(20)});
        a[11] = Raw(g);
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
