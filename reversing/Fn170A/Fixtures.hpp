#pragma once
// Fixtures del banco Fn170A (lote 170A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 22 asignadas con forma probada
// (la 23.a, 0x674930, se salta: ver README.md).
//   idx  direccion  nombre real                                                                         forma
//    0   0x659390   CTaskComplexDriveFireTruck::GetTaskType (V, vtable 0x86F570 ranura 4)             `mov eax,6Bh`
//    1   0x6593A0   CTaskComplexDriveFireTruck cuerpo del destructor (SEH)                            vtable 86F570h; CleanUpOldReference 571A00 de +0Ch y +14h; ~CTaskComplex 61A3D0
//    2   0x659410   CTaskComplexUseSwatRope constructor sin helicoptero (el listado decia FireTruck)  thiscall(ropeId) `ret 4`: base 61A3B0; +0Ch = ropeId; +10h = +18h = 0; byte +14h &= FEh; vtable 86F59Ch
//    3   0x659440   CTaskComplexUseSwatRope::GetTaskType (V, vtable 0x86F59C ranura 4)                `mov eax,6Ch`
//    4   0x659450   CTaskComplexUseSwatRope::IsInterruptable (V, ranura 11)                            thiscall(ped) `ret 4`: xor al,al
//    5   0x659460   CTaskComplexUseSwatRope::AbortUrgently (V, ranura 12)                              thiscall(ped) `ret 4`: xor al,al
//    6   0x65A3C0   CTaskComplexUseSwatRope destructor con borrado                                    cuerpo 6594D0; delete 61A5B0 si bit 0
//    7   0x631990   CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateSubTask                  thiscall(tipo) `ret 4`: 0xE0 / 0x386 / otro; new 61A5A0; base 61A3B0; ctor 667E20
//    8   0x61DE60   CTaskSimpleUseGun constructor (el listado decia CTaskSimpleFightingControl)       thiscall(entidad, vector, orden, rafaga, inmediato) `ret 1Ch`: base 61A390; RegisterReference 571B70
//    9   0x624DA0   CTaskSimpleUseGun destructor con borrado (listado: FightingControl)               cuerpo 61DF30; delete 61A5B0 si bit 0
//   10   0x61C520   CTaskSimpleFight::GetTaskType (V, vtable 0x86D684 ranura 4)                       `mov eax,3F8h`
//   11   0x61DB30   CWeaponInfo::GetBaseComboByName (listado: CTaskSimpleFight)                       cdecl(nombre) `ret`: ocho comparaciones de cadena (repe cmpsb); EAX = 4..0Eh
//   12   0x6726D0   CTaskComplexWanderProstitute destructor con borrado                               cuerpo 6726F0; delete 61A5B0 si bit 0
//   13   0x6726F0   CTaskComplexWanderProstitute cuerpo del destructor (copia del de CTaskComplexWander) `mov [ecx],86FE84h; jmp 61A3D0`
//   14   0x61C0D0   CTaskComplexPlayHandSignalAnim destructor con borrado (listado: RunNamedAnim)      cuerpo 61BDF0; delete 61A5B0 si bit 0
//   15   0x61C150   CWeaponInfo::GetAnimLoopFire (listado: RunNamedAnim)                               thiscall(bool) `ret 4`: fld dword [ecx+54h o +48h]
//   16   0x667720   CPointRoute::Reverse (listado: RunNamedAnim)                                       inversion en el sitio de entradas de 12 bytes con contador con signo en [this]
//   17   0x667900   CNodeRoute::Reverse (listado: RunNamedAnim)                                        idem con entradas de 4 bytes
//   18   0x6677A0   CPatrolRoute cuerpo del destructor (listado: RunNamedAnim)                         eh vector destructor iterator (821E02) de 8 elementos de 28h con destructor trivial (46A380 = ret)
//   19   0x667980   CEntitySeekPosCalculator cuerpo del destructor (3.a copia; listado: RunNamedAnim)  `mov [ecx],859DC4h`
//   20   0x6218C0   CTaskSimpleGangDriveBy cuerpo del destructor (SEH)                                 vtable 86D944h; RemoveAnimBlockRef 4D3FD0; SetDeleteCallback 4CEBC0; CleanUpOldReference 571A00; ~CTask 61A3A0
//   21   0x623750   CTaskComplexKillAllThreats::Clone (listado: GangDriveBy)                           thiscall() `ret`: new 61A5A0; base 61A3B0; copia +10h, +14h, +0Ch; vtable 86DA1Ch
//
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


namespace F170AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;

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
    std::vector<u8> s;          // bytes de la cadena de GetBaseComboByName (con o sin terminador segun el caso)
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B o C), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNz(Gen& g) { return Raw(g) | 1u; }   // distinto de cero y nunca una direccion de arena alineada del oraculo
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
inline u32 FloatBitsOf(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu, w6 = 0x3F800000u + (g.W() & 0x7FFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFFFFFFFFu, w1, w2, w3, w4, w5, w6});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kBool, kFtBody, kSwatCtor, kSdd, kObsSub, kGunCtor, kCombo, kFloat, kRoutePt, kRouteNode, kEmpty, kVtOnly, kBodyBase, kGangBody, kKillClone };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kFtBody, kSwatCtor, kConst, kBool, kBool, kSdd, kObsSub, kGunCtor, kSdd, kConst, kCombo, kSdd, kBodyBase, kSdd, kFloat,
                                                    kRoutePt, kRouteNode, kEmpty, kVtOnly, kGangBody, kKillClone};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x659390, 0x6593A0, 0x659410, 0x659440, 0x659450, 0x659460, 0x65A3C0, 0x631990, 0x61DE60, 0x624DA0, 0x61C520, 0x61DB30, 0x6726D0, 0x6726F0,
                                                    0x61C0D0, 0x61C150, 0x667720, 0x667900, 0x6677A0, 0x667980, 0x6218C0, 0x623750};
// cuerpo (colaborador) que llaman los destructores con borrado (0 = ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0, 0x6594D0, 0, 0, 0x61DF30, 0, 0, 0x6726F0, 0, 0x61BDF0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr std::array<u32, kFunctions> kConstVal{0x6B, 0, 0, 0x6C, 0, 0, 0, 0, 0, 0, 0x3F8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86F570, 0x86F570, 0x86F59C, 0x86F59C, 0x86F59C, 0x86F59C, 0x86F59C, 0x86DFC4, 0x86D724, 0x86D724, 0x86D684, 0x86D684, 0x870148, 0x86FE84,
                                                     0x86D5DC, 0x86D684, 0x86D684, 0x86D684, 0x86D684, 0x859DC4, 0x86D944, 0x86DA1C};
// Bytes que cada funcion toca a partir de su puntero `this` (las rutas y la cadena dependen del caso)
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x1C, 0x1C, 4, 4, 4, 4, 0x14, 0x40, 4, 4, 4, 4, 4, 4, 0x58, 4, 4, 4, 4, 0x40, 0x18};
// Un objeto sintetico pequeno ocupa 0x40 bytes en las arenas B y C
inline constexpr u32 kObjSpan = 0x40;
inline constexpr u32 kGroupSpan = 0x20 * 20;   // tabla de grupos de animacion (20 bytes por grupo) de CTaskSimpleGangDriveBy::DestructorBody

// Cadenas de GetBaseComboByName: nombres exactos del EXE (0x86D6B4..0x86D6F8) en el orden de las comparaciones del EXE
inline const std::array<const char*, 8> kComboNames{"UNARMED", "BBALLBAT", "KNIFE", "GOLFCLUB", "SWORD", "CHAINSAW", "DILDO", "FLOWERS"};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A (en kCombo: de la cadena en la arena C), a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kConst: a[3] = dword en [this] (no se lee)
//   kBool: a[3] = dword previo en [this]; a[4] = argumento (ped, crudo; no se lee)
//   kFtBody: a[3] = dword previo en [this]; a[4] = modo del vehiculo (+0Ch) (0 nulo, 1 en B a[5], 2 crudo a[6]); a[7] = modo del companero (+14h) (0 nulo, 1 en C a[8], 2 crudo a[9]); a[10] = EAX del falso de ~CTaskComplex;
//            a[11] = dword que ese falso deja en [this]; a[16] = dword en +10h (bIsDriver); a[17] = dword en +18h (fuego)
//   kSwatCtor: a[3] = dword previo en [this]; a[4] = ropeId; a[5], a[6], a[7], a[10] = dwords previos en +10h, +14h, +18h, +0Ch; a[8] = EAX del falso del constructor base; a[9] = dword que deja en [this]
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX del falso del cuerpo; a[5] = dword que deja en [this]; a[6] = banderas
//   kObsSub: a[3] = tipo de subtarea (pila); a[4] = rumbo (+10h, bits); a[5] = dword previo en [this]; a[6] = modo de la memoria reservada (0 nulo, 1 en B a[7]); a[7] = desplazamiento en B; a[8] = EAX del falso del
//            constructor de CTaskSimpleAchieveHeading (0 = el objeto, 1 = crudo a[9], 2 = 0); a[9] = crudo; a[10] = dword que el falso del constructor base deja en [objeto]; a[11] = EAX de ese falso
//   kGunCtor: a[3] = dword previo en [this]; a[4] = modo de la entidad (0 nula, 1 en B a[5], 2 cruda a[6]); a[7..9] = vector (bits); a[10] = orden (dword); a[11] = rafaga (dword); a[16] = inmediato (dword);
//            a[17] = EAX del falso del constructor base; a[18] = dword que deja en [this]; a[19] = EAX del falso de RegisterReference (sin uso)
//   kCombo: c.s = bytes escritos a partir del desplazamiento a[0] de la arena C (con terminador salvo si a[3] == 1: entonces siguen los bytes de la arena)
//   kFloat: a[3] = argumento (solo el byte bajo cuenta); a[4] = float en +48h (bits); a[5] = float en +54h (bits)
//   kRoutePt / kRouteNode: a[3] = contador con signo en [this] (el relleno de la arena aporta las entradas)
//   kEmpty / kVtOnly: a[3] = dword previo en [this]
//   kBodyBase: a[3] = dword previo en [this]; a[4] = EAX del falso de ~CTaskComplex; a[5] = dword que deja en [this]
//   kGangBody: a[3] = dword previo en [this]; a[4] = dword en +8..+0Bh (el byte +9 decide); a[5] = grupo (+28h); a[6] = puntero de bloque de la entrada del grupo; a[7] = desplazamiento de la tabla en C;
//            a[8] = modo de la animacion (+2Ch) (0 nula, 1 en B a[9], 2 cruda a[10]); a[16] = modo del objetivo (+34h) (0 nulo, 1 y 2 crudo a[17]); a[18] = dword que deja el falso de ~CTask; a[19] = EAX de ese falso
//   kKillClone: a[3], a[4], a[5] = dwords en +0Ch, +10h, +14h; a[6] = modo de la memoria reservada (0 nulo, 1 en B a[7]); a[7] = desplazamiento en B; a[8] = dword que deja el falso del constructor base; a[9] = su EAX
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, 0x86D48Cu, 0x86D4CCu, 0x86D4A8u, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    const auto mode3 = [&] { return g.Pk({0u, 1u, 1u, 2u}); };
    switch (kKind[c.fn]) {
    case kConst:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = Raw(g);
        break;
    case kBool:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = prev(); c.a[4] = Raw(g);
        break;
    case kFtBody:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = prev();
        c.a[4] = mode3(); c.a[5] = AuxOffset(g, kObjSpan); c.a[6] = RawNz(g);
        c.a[7] = mode3(); c.a[8] = AuxOffset(g, kObjSpan); c.a[9] = RawNz(g);
        c.a[10] = Raw(g); c.a[11] = Raw(g);
        c.a[16] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF01u, g.W()}); c.a[17] = Raw(g);
        break;
    case kSwatCtor:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = prev(); c.a[4] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x80000000u, 100u, g.W(), g.R(64)});
        c.a[5] = Raw(g); c.a[6] = g.Pk({0u, 1u, 0xFEu, 0xFFu, 0x100u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x01010101u, g.W(), g.W() | 1u, g.W() & ~1u}); c.a[7] = Raw(g); c.a[10] = Raw(g);
        c.a[8] = Raw(g); c.a[9] = Raw(g);
        break;
    case kSdd:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = prev(); c.a[4] = Raw(g); c.a[5] = Raw(g); c.a[6] = FlagsDword(g);
        break;
    case kObsSub:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = g.Pk({0xE0u, 0xE0u, 0xE0u, 0xE0u, 0x386u, 0x386u, 0x386u, 0x386u, 0x516u, 0x516u, 0u, 1u, 0xDFu, 0xE1u, 0x385u, 0x387u, 0xFFFFFFFFu, 0x1E0u, 0x80000386u, 0x100E0u, g.W(), g.R(0x600)});
        c.a[4] = FloatBitsOf(g);
        c.a[5] = prev();
        c.a[6] = g.Pk({0u, 1u, 1u, 1u});
        c.a[7] = AuxOffset(g, 0x20);
        c.a[8] = g.Pk({0u, 0u, 1u, 2u});
        c.a[9] = RawNz(g);
        c.a[10] = Raw(g); c.a[11] = Raw(g);
        break;
    case kGunCtor:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = prev();
        c.a[4] = mode3(); c.a[5] = AuxOffset(g, kObjSpan); c.a[6] = RawNz(g);
        for (unsigned k = 0; k < 3; ++k) c.a[7 + k] = FloatBitsOf(g);
        c.a[10] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0x1FFu, 0xFFFFFFFFu, 0x80000001u, g.W(), g.R(10)});
        c.a[11] = g.Pk({0u, 1u, 2u, 5u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 0x1FFFFu, 0xFFFFFFFFu, g.W(), g.R(100)});
        c.a[16] = g.Pk({0u, 1u, 2u, 0x100u, 0xFFu, 0xFFFFFF00u, 0xFFFFFF01u, g.W(), g.W() | 1u});
        c.a[17] = Raw(g); c.a[18] = Raw(g); c.a[19] = Raw(g);
        break;
    case kCombo: {
        // cadena: un nombre exacto (con o sin terminador), con un byte mas, truncada, con otra capitalizacion, aleatoria, vacia, o un nombre precedido o seguido de otro texto
        const u32 mode = g.R(16);
        std::string str;
        const char* name = kComboNames[g.R(8)];
        const unsigned len0 = unsigned(std::strlen(name));
        switch (mode) {
        case 0: case 1: case 2: case 3: case 4: str = name; break;                                   // exacto (con terminador)
        case 5: str = std::string(name) + char('A' + g.R(26)); break;                                // nombre + un caracter
        case 6: str = std::string(name).substr(0, g.R(len0)); break;                                 // prefijo (incluida la cadena vacia)
        case 7: { str = name; const unsigned i = g.R(len0); str[i] = char(str[i] ^ 0x20); break; }    // un caracter con otra capitalizacion
        case 8: { str = name; const unsigned i = g.R(len0); str[i] = char(1 + g.R(255)); break; }    // un caracter cambiado
        case 9: str = std::string(name) + "X" + name; break;
        case 10: { const unsigned len = g.R(12); for (unsigned i = 0; i < len; ++i) str += char(1 + g.R(255)); break; }   // aleatoria
        case 11: str = ""; break;
        case 12: str = std::string("U") + name; break;
        case 13: { str = name; for (auto& ch : str) if (ch >= 'A' && ch <= 'Z') ch = char(ch + 32); break; }   // minusculas
        case 14: str = name; break;                                                                  // exacto SIN terminador (siguen los bytes de la arena)
        default: str = name; break;                                                                  // exacto con un byte despues del terminador
        }
        c.s.assign(str.begin(), str.end());
        if (mode != 14) c.s.push_back(0);
        if (mode == 15) c.s.push_back('Z');
        c.a[3] = mode == 14 ? 1u : 0u;
        c.a[0] = Offset(g, kind, u32(c.s.size()) + 16);
        break;
    }
    case kFloat:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = g.Pk({0u, 1u, 2u, 0x100u, 0xFFu, 0xFFFFFF00u, 0xFFFFFF01u, 0x80000000u, 0x01000000u, g.W(), g.W() | 1u, g.W() & ~0xFFu});
        c.a[4] = FloatBitsOf(g); c.a[5] = FloatBitsOf(g);
        break;
    case kRoutePt: case kRouteNode: {
        const u32 elem = kKind[c.fn] == kRoutePt ? 12u : 4u;
        const u32 branch = g.R(10);
        i32 count;
        if (branch < 2) count = i32(g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000001u, 0xFFFFFFF0u, 0xFFFFFFFEu}));
        else if (branch < 6) count = i32(2 + g.R(7));                      // 2..8 (ruta llena)
        else count = i32(9 + g.R(kKind[c.fn] == kRoutePt ? 60u : 150u));   // mas que las 8 entradas de la ruta: invierte memoria contigua de la arena
        c.a[3] = u32(count);
        c.a[0] = Offset(g, kind, 4 + elem * u32(count > 1 ? count : 1));
        break;
    }
    case kEmpty: case kVtOnly:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = prev();
        break;
    case kBodyBase:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = prev(); c.a[4] = Raw(g); c.a[5] = Raw(g);
        break;
    case kGangBody: {
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = prev();
        c.a[4] = g.Pk({0x0100u, 0x0100u, 0x0100u, 0x0000u, 0xFFFFFFFFu, 0xFFFF00FFu, 0x01u, 0x0200u, 0x80000100u, g.W(), g.W() | 0x100u, g.W() & ~0xFF00u});
        const bool used = ((c.a[4] >> 8) & 0xFFu) != 0;
        c.a[5] = used ? g.R(0x20) : g.Pk({0u, 1u, 0x1Fu, 0xFFFFFFFFu, 0x80000000u, g.W()});
        c.a[6] = g.Pk({0xB5D4A0u, 0xB5D4C0u, 0xB5D4A1u, 0xB5D4BFu, 0xB5D500u, 0xB5D480u, 0xB5D000u, 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xB5D4A0u + 0x20u * g.R(0x40), 0xB5D4A0u - 0x20u * g.R(0x40), g.W(), g.W()});
        c.a[7] = AuxOffset(g, kGroupSpan);
        c.a[8] = mode3(); c.a[9] = AuxOffset(g, kObjSpan); c.a[10] = RawNz(g);
        c.a[16] = g.Pk({0u, 1u, 1u, 2u}); c.a[17] = RawNz(g);
        c.a[18] = Raw(g); c.a[19] = Raw(g);
        break;
    }
    case kKillClone:
        c.a[0] = Offset(g, kind, kSpan[c.fn]);
        c.a[3] = Raw(g); c.a[4] = Raw(g); c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 1u}); c.a[7] = AuxOffset(g, 0x20);
        c.a[8] = Raw(g); c.a[9] = Raw(g);
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
