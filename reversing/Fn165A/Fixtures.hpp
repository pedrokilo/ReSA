#pragma once
// Fixtures del banco Fn165A (lote 165A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables leidas del PE y de los llamadores, ver README.md y fixture_oracle.py). 26 asignadas, ninguna con hook
// (se comprobo el codigo y docs/hooks.csv). Varios nombres del listado eran erroneos (0x5DEA00..0x5DEAD0 son CEventSoundQuiet, 0x5DEB90 CPedIntelligence, 0x4CEC00/0x4CEF30 CAnimBlendStaticAssociation, 0x4D1570
// RpAnimBlendNodeUpdateKeyFrames, 0x63A2E0 CTaskUtilityLineUpPedWithCar, 0x643C60..0x643CE0 cinco clases de tareas distintas y 0x590DE0..0x590EC0 las clases de mobiliario de interiores, no CLoadingScreen).
//   idx  direccion  nombre real                                                              forma
//    0   0x4F2930   CAETwinLoopSoundEntity::StopSound                                        thiscall() `ret`: CAESound::StopSound 4EF1C0 sobre las voces de +A0 y +A4 (la segunda se lee tras la primera llamada)
//    1   0x4F2B80   CAETwinLoopSoundEntity cuerpo del destructor (marco SEH)                 thiscall() `ret`: vtable 85F438; si WORD[+88]: StopSoundAndForget 4EF850 sobre +A0 y +A4 y WORD=0; vtable 859988; ~CAESound 4EF660 en +8
//    2   0x5DEA00   CEventSoundQuiet cuerpo del destructor (vtable 0x86C2B0, marco SEH)       thiscall() `ret`: [esi]=86C2B0h; CleanUpOldReference 571A00 de +14h; cuerpo ~CEventEditableResponse 4AC480
//    3   0x5DEA60   CEventSoundQuiet::GetEventType (ranura 1)                                 thiscall() `ret`: mov eax,3Eh
//    4   0x5DEA70   CEventSoundQuiet::GetLifeTime (ranura 3)                                  thiscall() `ret`: xor eax,eax
//    5   0x5DEA80   CEventSoundQuiet::GetSourceEntity                                          thiscall() `ret`: mov eax,[ecx+14h]
//    6   0x5DEA90   CEventSoundQuiet::GetLocalSoundLevel                                       thiscall() `ret`: fld dword [ecx+18h] (ST0)
//    7   0x5DEAA0   CEventSoundQuiet::CanBeInterruptedBySameEvent                              thiscall() `ret`: mov al,1
//    8   0x5DEAB0   CEventSoundQuiet::ScalarDeletingDestructor (ranura 0)                     thiscall(flags) `ret 4`: cuerpo 5DEA00 (funcion 2), CEvent::operator delete 4B5630
//    9   0x5DEAD0   CEventSoundQuiet::GetEventPriority (ranura 2)                              thiscall() `ret`: mov eax,8
//   10   0x5DEB90   CPedIntelligence destructor con borrado (sin vtable)                      thiscall(flags) `ret 4`: ~CPedIntelligence 607300 y operator delete 6074E0
//   11   0x4CEB60   CAnimBlendAssociation::GetNode                                            thiscall(indice) `ret 4`: [ecx+10h] + 18h * indice
//   12   0x4CEC00   CAnimBlendStaticAssociation cuerpo del destructor (vtable 0x85C6CC)       thiscall() `ret`: [ecx]=85C6CCh; CMemoryMgr::Free 72F430 del puntero de +0Ch si no es nulo
//   13   0x4CEF30   CAnimBlendStaticAssociation destructor con borrado (cuerpo en linea)       thiscall(flags) `ret 4`: igual que 12 y operator delete 8214BD si bit 0
//   14   0x4D1570   RpAnimBlendNodeUpdateKeyFrames (EAX = contexto; pila: frames, n)           cdecl-like `ret`: por cada array de nodos y frame sin velocidad con secuencia: FindKeyFrame 4D0240(nodo, tiempo)
//   15   0x63A2E0   CTaskUtilityLineUpPedWithCar destructor con borrado (sin vtable)          thiscall(flags) `ret 4`: cuerpo 64FC00 y operator delete 8214BD
//   16   0x643C60   CTaskComplexEnterCarAsDriver destructor con borrado (vtable 0x86EAAC)    cuerpo 640330, CTask::operator delete 61A5B0
//   17   0x643C80   CTaskComplexEnterCarAsPassenger destructor con borrado (0x86EADC)        cuerpo 640380, 61A5B0
//   18   0x643CA0   CTaskComplexOpenDriverDoor destructor con borrado (0x86EB0C)              cuerpo 6403D0, 61A5B0
//   19   0x643CC0   CTaskComplexOpenPassengerDoor destructor con borrado (0x86EB3C)           cuerpo 640420, 61A5B0
//   20   0x643CE0   CTaskComplexDragPedFromCar destructor con borrado (0x86EB6C)              cuerpo 6404D0 (sin hook: funcion propia de la DLL), 61A5B0
//   21   0x590DE0   FurnitureGroup_c::FurnitureGroup_c                                        thiscall() `ret`: lista en +0 (cabeza, cola, contador) a cero (4A8DD0); EAX = this
//   22   0x590E00   ListItem_c<FurnitureSubGroup_c> constructor (codigo muerto)               thiscall() `ret`: +0 y +4 a cero (4A8DB0); EAX = this
//   23   0x590E20   FurnitureSubGroup_c::FurnitureSubGroup_c (marco SEH)                      thiscall() `ret`: base (+0, +4) y lista en +0Ch (+0Ch, +10h, +14h) a cero; EAX = this
//   24   0x590E70   FurnitureSubGroup_c::~FurnitureSubGroup_c (marco SEH)                     thiscall() `ret`: dos llamadas a funciones vacias (4A8DE0, 4A8DC0); no escribe
//   25   0x590EC0   Furniture_c::Furniture_c                                                   thiscall() `ret`: +0 y +4 a cero (4A8DB0); EAX = this
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

namespace F165AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 26;

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

// Bits de un float "interesante". Con allowSignaling = false no hay NaN senalizantes (el EXE los pasa por fld/fstp, que los vuelve silenciosos, y el C++ copia los bits); con true si (ambos lados usan fld).
inline u32 FloatBits(Gen& g, bool allowSignaling) {
    u32 w = g.W();
    if (!allowSignaling && (w & 0x7F800000u) == 0x7F800000u && (w & 0x7FFFFFu) && !(w & 0x400000u)) w |= 0x400000u;
    const u32 snan = allowSignaling ? 0x7F812345u : 0x7FC00001u;
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x00000001u, 0x007FFFFFu, 0x80000001u, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x42C80000u, 0x3DCCCCCDu,
                 0x7FC12345u, snan, w, w, w});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kConstAL, kLoad, kFloat, kSdd, kBodyRef, kTwinStop, kTwinDtor, kGetNode, kStaticBody, kStaticSdd, kUpdateKF, kCtor, kNop };
inline constexpr std::array<Kind, kFunctions> kKind{
    kTwinStop, kTwinDtor, kBodyRef, kConst, kConst, kLoad, kFloat, kConstAL, kSdd, kConst, kSdd, kGetNode, kStaticBody, kStaticSdd, kUpdateKF, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kCtor, kCtor, kCtor, kNop, kCtor};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4F2930, 0x4F2B80, 0x5DEA00, 0x5DEA60, 0x5DEA70, 0x5DEA80, 0x5DEA90, 0x5DEAA0, 0x5DEAB0, 0x5DEAD0, 0x5DEB90, 0x4CEB60, 0x4CEC00, 0x4CEF30, 0x4D1570, 0x63A2E0,
                                                    0x643C60, 0x643C80, 0x643CA0, 0x643CC0, 0x643CE0, 0x590DE0, 0x590E00, 0x590E20, 0x590E70, 0x590EC0};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0x4AC480, 0, 0, 0, 0, 0, 0x5DEA00, 0, 0x607300, 0, 0, 0, 0, 0x64FC00, 0x640330, 0x640380, 0x6403D0, 0x640420, 0x6404D0, 0, 0, 0, 0, 0};
// Funcion de liberacion de los destructores con borrado (CEvent::operator delete, CPedIntelligence::operator delete, operator delete del CRT, CTask::operator delete); 0 = ninguna
inline constexpr std::array<u32, kFunctions> kDelete{0, 0, 0, 0, 0, 0, 0, 0, 0x4B5630, 0, 0x6074E0, 0, 0, 0x8214BD, 0, 0x8214BD, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0, 0, 0, 0, 0};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x85F438, 0x85F438, 0x86C2B0, 0, 0, 0, 0, 0, 0x86C2B0, 0, 0, 0, 0x85C6CC, 0x85C6CC, 0, 0, 0x86EAAC, 0x86EADC, 0x86EB0C, 0x86EB3C, 0x86EB6C, 0, 0, 0, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this` (en la funcion 14 es el contexto)
inline constexpr std::array<u32, kFunctions> kSpan{0xA8, 0xA8, 0x18, 4, 4, 0x18, 0x1C, 4, 4, 4, 4, 0x14, 0x10, 0x10, 0x40, 4, 4, 4, 4, 4, 4, 0xC, 8, 0x18, 4, 8};
inline constexpr u32 kGlobalClump = 0xB4EA0C;                // gpAnimBlendClump (CAnimBlendClumpData*); +0Ch = m_PedPosition

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; a[16..23] = datos extra
//   kConst: a[3] = dword en [this] (no se lee); kConstAL: igual (sin argumento de pila: `ret`); kLoad: a[3] = dword en this+14h (el valor devuelto), a[4] = dword en this+18h
//   kFloat: a[3] = bits del float en this+18h (ST0 al salir)
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kBodyRef: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = modo de la entidad (+14h: 0 nula, 1 en B a[7], 2 cruda a[8])
//   kTwinStop: a[3] = modo de la voz +A0 (0 nula, 1 en B a[7], 2 cruda a[8]); a[4] = modo de la voz +A4 (0 nula, 1 en B a[9], 2 cruda a[10], 3 igual que la voz +A0); a[5] = valor que el falso deja en +A4 tras la primera llamada
//         si a[6] = 1; a[11] = dword en +88
//   kTwinDtor: a[3] = dword en this+88h (solo cuenta el WORD bajo); a[4], a[5] = modos de las voces +A0 y +A4 (como en kTwinStop, con a[7], a[8], a[9], a[10]); a[6] = dword previo en [this]
//   kGetNode: a[3] = dword en this+10h; a[4] = indice (argumento de pila)
//   kStaticBody / kStaticSdd: a[3] = dword previo en [this]; a[6] = modo del puntero de +0Ch (0 nulo, 1 en B a[7], 2 crudo a[8], 3 = this); a[9] = banderas (kStaticSdd)
//   kUpdateKF: a[3] = n (argumento con signo); a[4] = numero de arrays de nodos (0..3); a[5] = modo de m_PedPosition (0 nulo, 1 distinto de cero a[16]); a[6] = frames en B; a[7] = objeto del clump en B (+0Ch = pedPosition);
//         a[8..10] = asociaciones de los tres arrays en B; a[11] = arrays de nodos en C (0x130 de separacion); a[17..19] = bits del tiempo de cada asociacion (+20h); a[20..22] = mascara de secuencias presentes por array;
//         a[23] = semilla de las banderas de los frames (bit 3 = HasVelocity); EAX de entrada = contexto (arena A en a[0])
//   kCtor, kNop: sin datos extra (el relleno de las arenas basta)
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
    const auto voiceMode = [&](bool allowSame) { return allowSame ? g.Pk({0u, 0u, 1u, 1u, 1u, 2u, 3u}) : g.Pk({0u, 0u, 1u, 1u, 1u, 2u}); };
    switch (kKind[c.fn]) {
    case kConst:
    case kConstAL:
        c.a[3] = Raw(g);
        break;
    case kLoad:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kFloat:
        c.a[3] = FloatBits(g, true);
        break;
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kBodyRef:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = g.P(70) ? (Raw(g) | 1u) : Raw(g);
        break;
    case kTwinStop:
        c.a[3] = voiceMode(false);
        c.a[4] = voiceMode(true);
        c.a[5] = g.Pk({0u, 0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        c.a[6] = g.Pk({0u, 0u, 1u, 1u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Raw(g) | 1u;
        c.a[9] = Offset(g, g.R(4), 4);
        c.a[10] = Raw(g) | 1u;
        c.a[11] = g.W();
        break;
    case kTwinDtor: {
        const u32 w = g.W();
        c.a[3] = g.Pk({0u, 1u, 2u, 0xFFFFu, 0x10000u, 0x10001u, 0xFFFF0000u, 0x100u, 0xFFFFFFFFu, w, w, w});
        c.a[4] = voiceMode(false);
        c.a[5] = voiceMode(true);
        c.a[6] = prev();
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Raw(g) | 1u;
        c.a[9] = Offset(g, g.R(4), 4);
        c.a[10] = Raw(g) | 1u;
        break;
    }
    case kGetNode:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x10000000u, 0x30000000u, 0x7FFFFFF8u, 0x80000000u, 0xFFFFFFE8u, g.W(), g.W(), Raw(g)});
        c.a[4] = g.Pk({0u, 1u, 2u, 3u, 5u, 11u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 0x55555555u, 0xAAAAAAABu, 0x0AAAAAAAu, g.R(64), g.W(), g.W()});
        break;
    case kStaticBody:
    case kStaticSdd:
        c.a[3] = prev();
        c.a[6] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Raw(g) | 1u;
        c.a[9] = FlagsDword(g);
        break;
    case kUpdateKF: {
        c.a[3] = g.Pk({0u, 1u, 2u, 3u, 4u, 6u, 8u, 12u, 3u, 5u, 12u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFBu});
        c.a[4] = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 3u});
        c.a[5] = g.Pk({0u, 1u, 1u});
        const u32 fo = 4 * g.R(0x2E0 / 4);
        c.a[6] = kind == 1 ? fo + 1 + g.R(3) : fo;
        const u32 co = 0x400 + 4 * g.R(0x20);
        c.a[7] = kind == 1 ? co + 1 + g.R(3) : co;
        for (unsigned k = 0; k < 3; ++k) {
            const u32 ao = 0x800 + 0x100 * k + 4 * g.R(0x20);
            c.a[8 + k] = kind == 1 ? ao + 1 + g.R(3) : ao;
        }
        const u32 no = 4 * g.R(0x300 / 4);
        c.a[11] = kind == 1 ? no + 1 + g.R(3) : no;
        c.a[16] = Raw(g) | 1u;
        for (unsigned k = 0; k < 3; ++k) c.a[17 + k] = FloatBits(g, false);
        for (unsigned k = 0; k < 3; ++k) c.a[20 + k] = g.Pk({0u, 0xFFFu, 0xFFFu, 0xAAAu, 0x555u, g.W() & 0xFFFu, g.W() & 0xFFFu, g.W() & 0xFFFu, 1u, 0x800u});
        c.a[23] = g.W();
        break;
    }
    case kCtor:
    case kNop:
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
