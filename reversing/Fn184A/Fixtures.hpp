#pragma once
// Fixtures del banco Fn184A (lote 184A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 26 asignadas: 26 reconstruidas.
//   idx  direccion  nombre real                                                                      forma
//    0   0x460CD0   CTaskComplexWander::GetTaskType                                                  thiscall() `ret` V: mov eax,390h
//    1   0x460CE0   CTaskComplexWanderCop::Clone                                                     thiscall() `ret` V: new(50h) 61A5A0; ctor 460C80(estado +0Ch, BYTE +10h ampliado a cero)
//    2   0x460D50   CTaskComplexWanderCop::GetWanderType                                             thiscall() `ret` V: mov eax,1
//    3   0x460D60   CTaskComplexWanderCop destructor con borrado                                      thiscall(flags) `ret 4`: cuerpo 460D80; delete 61A5B0
//    4   0x460D80   CTaskComplexWanderCop cuerpo del destructor                                      thiscall() `ret`: vtable 859C80; delete virtual (ranura 0, bandera 1) de +28h; cuerpo 669D20
//    5   0x49EA80   FxEntitySystem constructor                                                       thiscall() `ret`: ListItem_c 4A8DB0 (+0 y +4 a cero); devuelve this
//    6   0x49EA90   Fx_c::Init                                                                       thiscall() `ret`: g_fxMan.Init, LoadFxProject, SetWindData, InitStaticSystems, +50h = 0 (AL = 1)
//    7   0x49EAE0   Fx_c::Reset                                                                      thiscall() `ret`: DestroyAllFxSystems; salto a InitStaticSystems
//    8   0x4A11C0   FxEntitySystem destructor con borrado                                            thiscall(flags) `ret 4`: 49EA70 (ret); delete 8214BD
//    9   0x4A11E0   Fx_c::CreateEntityFx                                                              thiscall(entidad, nombre, pos*, matriz*) `ret 10h`: CreateFxSystem 4A9BE0; new(10h) 82119A; AddItem 4A8DF0; Play 4AA2F0
//   10   0x4A1280   Fx_c::DestroyEntityFx                                                             thiscall(entidad) `ret 4`: recorre la lista (+44h); RemoveItem 4A8E30; Kill 4AA3F0; delete 8214BD
//   11   0x4A12D0   Fx_c::ExitEntitySystems                                                           thiscall() `ret`: recorre la lista; RemoveItem; DestroyFxSystem 4A9810; delete 8214BD
//   12   0x4A1320   Fx_c::Exit                                                                        thiscall() `ret`: ExitEntitySystems, ExitStaticSystems 49E850; salto a g_fxMan.Exit 4A9A10
//   13   0x4B1ED0   CEventPotentialWalkIntoFire::GetEventType                                         thiscall() `ret` V: mov eax,2Eh
//   14   0x4B1EE0   CEventPotentialWalkIntoFire::GetLifeTime                                          thiscall() `ret` V: xor eax,eax
//   15   0x4B1EF0   CEventPotentialWalkIntoFire::GetEventPriority                                     thiscall() `ret` V: mov eax,1Fh
//   16   0x4B1F90   CEventFireNearby::AffectsPed                                                      thiscall(ped) `ret 4` V: FindActiveTaskByType 681740 (tipo 25Ch); CPed::IsAlive 5E0170
//   17   0x4B6990   CEventGunShotWhizzedBy destructor con borrado                                     thiscall(flags) `ret 4`: cuerpo 4B2070; pool de eventos 0B744ACh en linea
//   18   0x4B69E0   CEventSeenPanickedPed destructor con borrado                                      idem, cuerpo 4B2130
//   19   0x4B6A30   CEventAreaCodes destructor con borrado                                            idem, cuerpo 4B2210
//   20   0x4B6A80   CEventPlayerCommandToGroup destructor con borrado                                 idem, cuerpo 4B2470
//   21   0x4B6AD0   CEventDanger destructor con borrado                                               idem, cuerpo 4B26A0
//   22   0x674EA0   CEventInteriorUseInfo::Constructor                                                thiscall(info, interior, animTime, BYTE loop) `ret 10h`: CEvent 4ABFC0; vtable 8701A8
//   23   0x674ED0   CEventInteriorUseInfo cuerpo del destructor                                       thiscall() `ret`: vtable 8701A8; salto a 4AC010
//   24   0x674F00   CEventInteriorUseInfo::Clone                                                      thiscall() `ret` V: new(1Ch) 4B5620; CEvent 4ABFC0; campos +0Ch, +10h, +14h, BYTE +18h; vtable
//   25   0x674F90   CEventInteriorUseInfo destructor con borrado                                      thiscall(flags) `ret 4`: cuerpo 674ED0; delete 4B5630
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

namespace F184AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 26;
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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kSeed = 60;          // a[60..63] = semillas de las arenas A, B, C y D
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 64> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en una zona [lo, lo + size) de una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxIn(Gen& g, u32 lo, u32 size, u32 span) { return lo + 4 * g.R((size - span) / 4 + 1); }
inline u32 AuxOffset(Gen& g, u32 span) { return AuxIn(g, 0, u32(kWin), span); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un dword distinto de cero (para valores que el EXE prueba contra cero): 0x100 y 0x80000000 tienen el BYTE bajo a cero
inline u32 NonZero(Gen& g) {
    const u32 v = Raw(g);
    return v ? v : g.Pk({1u, 0x100u, 0x80000000u, 0xFFFFFF00u});
}
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un dword cuyo BYTE bajo es el dato (direccion de paseo, accion de bucle): la mitad alta es basura (el EXE solo lee el BYTE)
inline u32 ByteDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W();
    return g.Pk({0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80000000u, 0xFFFFFFFFu, w1, w2, g.R(256), g.R(256)});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kWClone, kSdd, kSddNB, kWDtor, kFxCtor, kFxInit, kFxReset, kFxCreate, kFxDestroy, kFxExitEnt, kFxExit, kAffects, kEvtSdd, kIuiCtor, kIuiBody, kIuiClone };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kWClone, kConst, kSdd, kWDtor, kFxCtor, kFxInit, kFxReset, kSddNB, kFxCreate, kFxDestroy, kFxExitEnt, kFxExit, kConst, kConst, kConst, kAffects,
                                                    kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd, kEvtSdd, kIuiCtor, kIuiBody, kIuiClone, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x460CD0, 0x460CE0, 0x460D50, 0x460D60, 0x460D80, 0x49EA80, 0x49EA90, 0x49EAE0, 0x4A11C0, 0x4A11E0, 0x4A1280, 0x4A12D0, 0x4A1320, 0x4B1ED0, 0x4B1EE0, 0x4B1EF0,
                                                    0x4B1F90, 0x4B6990, 0x4B69E0, 0x4B6A30, 0x4B6A80, 0x4B6AD0, 0x674EA0, 0x674ED0, 0x674F00, 0x674F90};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86FE84, 0x859C80, 0x859C80, 0x859C80, 0x859C80, 0, 0, 0, 0, 0, 0, 0, 0, 0x85B6A0, 0x85B6A0, 0x85B6A0, 0x85B6E8,
                                                     0x85B7C0, 0x85B7C0, 0x85B7C0, 0x85B7C0, 0x85B7C0, 0x8701A8, 0x8701A8, 0x8701A8, 0x8701A8};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x14, 4, 4, 0x2C, 8, 0x70, 0x70, 4, 0x70, 0x70, 0x70, 0x70, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x1C, 4, 0x1C, 4};
inline constexpr u32 kHwFx = 0xA9AE80;     // g_fxMan (ECX de los metodos de FxManager_c)
inline constexpr u32 kWindDir = 0xC813E0, kWind = 0xC812F0;
inline constexpr u32 kEventPoolGlobal = 0xB744AC;   // puntero al pool de eventos (destructores con borrado de eventos)
inline constexpr u32 kVtOff = 0xE00;   // desplazamiento de las vtables de los objetos falsos en sus arenas
inline constexpr u32 kListOff = 0x44;  // lista m_FxEntities de Fx_c
inline constexpr u32 kMaxNodes = 5;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this] (salvo indicacion), a[60..63] = semillas de las arenas A, B, C y D
//   kConst:    nada mas
//   kWClone:   a[3] = estado de movimiento (+0Ch), a[4] = direccion (+10h, dword: solo cuenta el BYTE bajo), a[5] = memoria (0 nula, 1 en B), a[6] = su desplazamiento en B, a[7] = EAX del constructor, a[8] = dword que deja en [mem]
//   kSdd:      a[3] = vtable previa, a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kSddNB:    a[3] = dword previo en [this], a[6] = banderas (pila)
//   kWDtor:    a[3] = vtable previa, a[4] = tarea hija (+28h: 0 nula, 1 objeto en B con vtable falsa), a[5] = su desplazamiento en B, a[6] = EAX del falso del cuerpo base, a[7] = dword que deja en [this]
//   kFxCtor:   nada mas
//   kFxInit:   a[3] = dword previo en +50h, a[4] = EAX de los falsos
//   kFxReset, kFxExit: a[4] = EAX de los falsos
//   listas (kFxCreate, kFxDestroy, kFxExitEnt): a[3] = numero de nodos, a[4..8] = sus desplazamientos en B, a[11..15] = sus sistemas (+8), a[16..20] = sus entidades (+0Ch), a[21] = recuento extra (recuento = nodos + extra; 0 con lista vacia)
//   kFxCreate: a[22] = entidad (pila, cruda), a[23] = nombre (desplazamiento en D), a[24] = posicion (desplazamiento en D), a[25] = matriz (0 nula, 1 cruda, 2 en D), a[26] = valor crudo o desplazamiento de la matriz,
//              a[27] = EAX de CreateFxSystem (sistema; 0 = no se crea), a[28] = memoria nueva (desplazamiento en C), a[29] = EAX de Play
//   kFxDestroy: a[9] = entidad (pila), a[10] = mascara de los nodos cuya entidad coincide (el resto usa a[16..20], distintas de a[9])
//   kAffects:  a[3] = desplazamiento del ped en B, a[4] = el de la inteligencia en C, a[5] = EAX de FindActiveTaskByType, a[6] = EAX de IsAlive
//   kEvtSdd:   a[4] = desplazamiento del pool en B, a[5] = distancia this - almacenamiento (con signo), a[6] = desplazamiento de los estados en D, a[7] = banderas (pila), a[8] = EAX del falso del cuerpo, a[9] = dword que deja en [this],
//              a[10] = ultimo libre (+0Ch del pool), a[11] = byte previo del estado del slot, a[12] = tamano (+8 del pool)
//   kIuiCtor:  a[3] = informacion (pila), a[4] = interior, a[5] = tiempo de animacion, a[6] = accion de bucle (dword: solo cuenta el BYTE bajo), a[8..14] = dwords que deja el falso de CEvent en +0..+0x18
//   kIuiBody:  a[3] = dword previo en [this], a[5] = dword que deja el falso del cuerpo base en [this]
//   kIuiClone: a[3] = memoria (0 nula, 1 en B), a[4] = su desplazamiento, a[5..8] = informacion, interior, tiempo y accion (+0Ch, +10h, +14h, +18h del origen), a[9..15] = dwords que deja el falso de CEvent en +0..+0x18
inline void MakeNodes(Gen& g, C& c, u32 maxN) {
    auto& a = c.a;
    a[3] = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 3u, 4u, 5u});
    if (a[3] > maxN) a[3] = a[3] % (maxN + 1);
    const u32 slot = g.R(kMaxNodes);
    for (u32 i = 0; i < kMaxNodes; ++i) a[4 + i] = 0x40u + 0x40u * ((i + slot) % kMaxNodes) + 0x10u * g.R(3);   // el nodo i usa la ranura (i + slot) % 5: desplazamientos distintos
    for (u32 i = 0; i < kMaxNodes; ++i) a[11 + i] = Raw(g);
    for (u32 i = 0; i < kMaxNodes; ++i) a[16 + i] = Raw(g);
    a[21] = a[3] ? g.Pk({0u, 0u, 0u, 1u, 2u, 5u, 0x7FFFFFFFu, 0xFFFFFFF0u, g.W(), g.R(100)}) : 0u;
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[kSeed] = c.seed * 13u + 50u; c.a[kSeed + 1] = c.a[kSeed] + 1u; c.a[kSeed + 2] = c.a[kSeed] + 2u; c.a[kSeed + 3] = c.a[kSeed] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConst:
        break;
    case kWClone:
        a[3] = Raw(g);
        a[4] = ByteDword(g);
        a[5] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[6] = AuxOffset(g, 0x50);
        a[7] = Raw(g);
        a[8] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kSddNB:
        a[6] = FlagsDword(g);
        break;
    case kWDtor:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxIn(g, 0, 0xC00, 0x10);
        a[6] = Raw(g);
        a[7] = Raw(g);
        break;
    case kFxCtor:
        break;
    case kFxInit:
        a[3] = Raw(g);
        a[4] = Raw(g);
        break;
    case kFxReset:
    case kFxExit:
        a[4] = Raw(g);
        break;
    case kFxCreate:
        MakeNodes(g, c, 3);
        a[22] = Raw(g);
        a[23] = AuxOffset(g, 0x10);
        a[24] = AuxOffset(g, 0xC);
        a[25] = g.Pk({0u, 1u, 2u, 2u});
        a[26] = a[25] == 1 ? Raw(g) : AuxOffset(g, 0x40);
        a[27] = g.Pk({0u, 1u, 0x100u, 0x80000000u, Raw(g), Raw(g), Raw(g), NonZero(g), NonZero(g), NonZero(g)});
        a[28] = AuxOffset(g, 0x10);
        a[29] = Raw(g);
        break;
    case kFxDestroy:
        MakeNodes(g, c, 5);
        a[9] = Raw(g);
        a[10] = g.Pk({0u, 0x1Fu, 0x1Fu, g.R(32), g.R(32), g.R(32), g.R(32)});
        for (u32 i = 0; i < kMaxNodes; ++i) if (a[16 + i] == a[9]) a[16 + i] = a[9] ^ 1u;
        break;
    case kFxExitEnt:
        MakeNodes(g, c, 5);
        break;
    case kAffects:
        a[3] = AuxOffset(g, 0x480);
        a[4] = AuxOffset(g, 0x10);
        a[5] = g.Pk({0u, 0u, 0u, 1u, 0x100u, 0xFFFFFFFFu, 0x80000000u, NonZero(g), NonZero(g), Raw(g)});
        a[6] = g.Pk({0u, 1u, 0x100u, 0xFFu, 0xFFFFFF00u, 0xFFFFFF01u, 0x80u, Raw(g), Raw(g)});
        break;
    case kEvtSdd: {
        const i32 idx = i32(g.Pk({0u, 1u, 2u, 3u, 7u, 16u, 100u, 299u, g.R(300), g.R(300), g.R(300), 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFF8u, 0xFFFFFFF0u}));
        const u32 extra = g.Pk({0u, 1u, 0x43u, g.R(0x44), g.R(0x44)});
        a[4] = AuxOffset(g, 0x20);
        a[5] = u32(idx * 0x44 + (idx < 0 ? -i32(extra) : i32(extra)));
        a[6] = 0x40u + 4u * g.R((u32(kWin) - 0x40u - 0x300u - 0x40u) / 4u);
        a[7] = FlagsDword(g);
        a[8] = Raw(g); a[9] = Raw(g);
        a[10] = g.Pk({u32(idx - 1), u32(idx), u32(idx + 1), 0u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 300u, g.W(), g.R(400), u32(idx - 1), u32(idx)});
        a[11] = g.Pk({0u, 0x80u, 0x7Fu, 0xFFu, g.R(256), g.R(256)});
        a[12] = g.Pk({300u, 512u, 1000u, g.W() | 0x100u});
        break;
    }
    case kIuiCtor:
        a[3] = Raw(g); a[4] = Raw(g); a[5] = Raw(g); a[6] = ByteDword(g);
        for (u32 i = 8; i <= 14; ++i) a[i] = g.W();
        break;
    case kIuiBody:
        a[5] = Raw(g);
        break;
    case kIuiClone:
        a[3] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[4] = AuxOffset(g, 0x1C);
        a[5] = Raw(g); a[6] = Raw(g); a[7] = Raw(g); a[8] = ByteDword(g);
        for (u32 i = 9; i <= 15; ++i) a[i] = g.W();
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
    return j.End();
}
}
