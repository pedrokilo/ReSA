#pragma once
// Fixtures del banco Fn148A (lote 148A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                                     forma
//   0   0x4A9F20   FxPrim_c::Constructor (listado: FxPrimBP_c)                thiscall() `ret`, 21 B: [this] = 85A9E0h; [+4] = [+8] = 0; byte [+0Ch] = 1; eax = this
//   1   0x4A9F40   FxPrim_c::Destructor (cuerpo; listado: FxPrimBP_c)         thiscall() `ret`, 15 B: [this] = 85A9E0h; [+4] = [+8] = 0; eax = 0
//   2   0x4A9F50   FxPrim_c::Enable (listado: FxPrimBP_c)                     thiscall(flag) `ret 4`: byte [this+0Ch] = byte bajo del argumento
//   3   0x4A9F60   CAEAudioEntity::Destructor (copia de 0x5DE9E0; listado: FxPrimBP_c)   mov [ecx],859988h; add ecx,8; jmp 4EF660
//   4   0x4A9F70   FxSphere_c::Constructor (listado: FxPrimBP_c)              thiscall() `ret`: dword [this+10h] = 0; eax = this
//   5   0x5F8580   CPedGroupIntelligence::SetDefaultTask                      thiscall(ped, tarea) `ret 8`: SetTask (0x5F7540) con (ped, tarea, this+1ECh, -1, 0)
//   6   0x5F8750   CPedGroupIntelligence::ReportAllBarDefaultTasksFinished    thiscall() `ret`: 3 x ReportAllTasksFinished (0x5F7730) con this+0Ch, this+14Ch y this+0ACh
//   7   0x63CB50   CTaskComplexCarDriveWander::Destructor (cuerpo)            mov [ecx],86E96Ch; jmp 63CA40
//   8   0x643260   CTaskComplexCarDriveMission::ScalarDeletingDestructor (listado: CTaskComplexCarDriveWander)   thiscall(flags) `ret 4`, 30 B: call 63CCC0; (flags&1) -> CTask::operator delete 0x61A5B0; eax = this
//   9   0x643280   CTaskComplexDriveToPoint::ScalarDeletingDestructor         idem; cuerpo 0x63CE70
//  10   0x5F7FB0   CTaskSimpleChat::ScalarDeletingDestructor (listado: CPedGroups)   idem; cuerpo 0x5F7FD0
//  11   0x5FB990   CPedGroupMembership::Destructor (cuerpo; listado: CPedGroups)     thiscall() `ret`: para cada hueco 0..7 no nulo de [this+4+4i]: RemoveMember (0x5F80D0)
//  12   0x5FE960   tUserList::IsPedTypeAllowed (listado: CPedIK)             thiscall(pedType) `ret 4`: AL
//  13   0x5FE9C0   IsPedTypeGang (listado: CPedIK)                           cdecl(int) `ret`: 7 <= tipo <= 16 (con signo); EAX = 0 o 1
//  14   0x4161D0   CLinkList<CCollisionData*>::Shutdown (listado: CCollision)   thiscall() `ret`: delete[] [this+30h] (0x8213AE); [this+30h] = 0
//  15   0x4161F0   CLinkList<CCollisionData*>::Insert(T const&) (listado: CCollision)  thiscall(const T*) `ret 4`: enlace libre -> usado; devuelve el enlace o 0
//  16   0x678CB0   CTaskComplexInAirAndLand::GetTaskType (V, ranura 4)        mov eax,0F0h
//  17   0x678CC0   CTaskComplexInAirAndLand::Destructor (cuerpo)              mov [ecx],8704ACh; jmp 61A3D0
//  18   0x67D340   CTaskSimpleInAir::ScalarDeletingDestructor (listado: CTaskComplexInAirAndLand)   destructor con borrado; cuerpo 0x678D50 (sin hook)
//  19   0x67D360   CTaskSimpleLand::ScalarDeletingDestructor (listado: CTaskComplexInAirAndLand)    destructor con borrado; cuerpo 0x678EE0 (sin hook)
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

namespace F148AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 20;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Memoria global tocada: ninguna (se conserva el mecanismo de regiones, vacio, de Fn144A en Core.hpp)
constexpr u32 kRegionBytes = 0x30u;

// Bytes que cada funcion toca a partir de su puntero `this` (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable (o nada)
constexpr u32 kSpanPrim = 0x10;       // FxPrim_c: +0 .. +0xC
constexpr u32 kSpanSphere = 0x14;     // FxSphere_c: +0x10
constexpr u32 kSpanPgi = 0x2A0;       // CPedGroupIntelligence (solo se calculan direcciones)
constexpr u32 kSpanMember = 0x28;     // CPedGroupMembership
constexpr u32 kSpanUserList = 0x24;   // tUserList
constexpr u32 kSpanLink = 0x34;       // CLinkList
constexpr u32 kSpanNodes = 0x3C;      // 5 enlaces de 0xC

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
    std::array<u32, 20> a{};
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
inline constexpr unsigned kIdxFxPrimCtor = 0, kIdxFxPrimDtor = 1, kIdxFxPrimEnable = 2, kIdxAudioDtor = 3, kIdxFxSphereCtor = 4, kIdxSetDefaultTask = 5, kIdxReportBarDefault = 6, kIdxWanderDtor = 7,
                          kIdxMissionSdd = 8, kIdxDriveToPointSdd = 9, kIdxChatSdd = 10, kIdxMembershipDtor = 11, kIdxUserList = 12, kIdxIsPedTypeGang = 13, kIdxLinkShutdown = 14, kIdxLinkInsert = 15,
                          kIdxInAirType = 16, kIdxInAirLandDtor = 17, kIdxSimpleInAirSdd = 18, kIdxSimpleLandSdd = 19;
inline constexpr bool IsSdd(unsigned fn) { return fn == kIdxMissionSdd || fn == kIdxDriveToPointSdd || fn == kIdxChatSdd || fn == kIdxSimpleInAirSdd || fn == kIdxSimpleLandSdd; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[16..19] = semillas de las arenas A, B, C y reservada; el resto por funcion
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[16] = c.seed * 13u + 50u; c.a[17] = c.a[16] + 1u; c.a[18] = c.a[16] + 2u; c.a[19] = c.a[16] + 3u;
    switch (c.fn) {
    case kIdxFxPrimCtor:    // a[3..6] = dwords previos en +0, +4, +8 y +0xC
        c.a[0] = Offset(g, kind, kSpanPrim);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85A9E0u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        c.a[6] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x00000100u, 0xFFFFFF00u, 0x01010101u, g.W(), g.W()});
        break;
    case kIdxFxPrimDtor:    // a[3..6] = dwords previos en +0, +4, +8 y +0xC (el ultimo no se toca)
        c.a[0] = Offset(g, kind, kSpanPrim);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85A9E0u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        c.a[6] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        break;
    case kIdxFxPrimEnable:  // a[3] = dword previo en +0xC; a[4] = el dword argumento (el EXE escribe solo su byte bajo)
        c.a[0] = Offset(g, kind, kSpanPrim);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x00000100u, 0xFFFFFF00u, 0x01010101u, g.W(), g.W()});
        c.a[4] = g.Pk({0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0x101u, 0x1FFu, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W() & ~0xFFu, g.W() | 1u});
        break;
    case kIdxAudioDtor:     // a[3] = vtable previa en [this]; a[4] = EAX del falso de CAESound::Destructor
        c.a[0] = Offset(g, kind, kSpanPrim);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x859988u, 0x85F310u, 0x85F311u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        break;
    case kIdxFxSphereCtor:  // a[3] = dword previo en +0x10; a[4] = dword previo en +0xC (no se toca)
        c.a[0] = Offset(g, kind, kSpanSphere);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W(), 0x80000000u});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 1u, g.W(), g.W()});
        break;
    case kIdxSetDefaultTask: {
        // a[3] = selector del ped: 0 crudo (a[4]), 1 puntero a la arena C (desplazamiento a[5]); a[6] = selector de la tarea: 0 crudo (a[7]), 1 puntero a la arena B (desplazamiento a[8]); a[9] = EAX del falso de SetTask
        c.a[0] = Offset(g, kind, kSpanPgi);
        c.a[3] = g.Pk({0u, 0u, 1u});
        c.a[4] = Raw(g);
        c.a[5] = Offset(g, g.R(4), 4);
        c.a[6] = g.Pk({0u, 0u, 1u});
        c.a[7] = Raw(g);
        c.a[8] = Offset(g, g.R(4), 4);
        c.a[9] = Raw(g);
        break;
    }
    case kIdxReportBarDefault:   // sin parametros propios (a[3] = dword en [this], no se lee; a[4] = EAX del falso)
        c.a[0] = Offset(g, kind, kSpanPgi);
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kIdxWanderDtor:    // a[3] = vtable previa en [this]; a[4] = dword que el falso del cuerpo deja en [this]; a[5] = EAX del falso
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x86E96Cu, 0x86E934u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x86E934u, 0x86E96Cu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[5] = Raw(g);
        break;
    case kIdxMissionSdd: case kIdxDriveToPointSdd: case kIdxChatSdd: case kIdxSimpleInAirSdd: case kIdxSimpleLandSdd:
        // a[3] = dword previo en [this]; a[4] = banderas (argumento de pila, solo importa el bit 0 del byte bajo); a[5] = dword que el falso del cuerpo deja en [this]; a[6] = EAX del falso del cuerpo (no es this)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x86E9A4u, 0x86E9DCu, 0x86C78Cu, 0x8704D8u, 0x8704FCu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, g.W(), g.W(), g.W() | 1u, g.W() & ~1u});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 0x86E934u, 0x86C788u, 0x8704D4u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[6] = Raw(g);
        break;
    case kIdxMembershipDtor: {
        // a[3] = mascara de huecos 0..7 no nulos (cada hueco no nulo vale el dword crudo a[8+i]); a[4] = indice de la llamada a RemoveMember tras la cual el falso escribe (9 = ninguna);
        // a[5] = hueco que escribe; a[6] = valor que escribe (0 o un dword crudo); a[7] = EAX del falso de RemoveMember
        c.a[0] = Offset(g, kind, kSpanMember);
        c.a[3] = g.Pk({0u, 0xFFu, 0xFFu, 0x01u, 0x80u, 0x55u, 0xAAu, 0x0Fu, 0xF0u, g.R(256), g.R(256), g.R(256), g.R(256)});
        c.a[4] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 9u, 8u});
        c.a[5] = g.R(8);
        c.a[6] = g.Pk({0u, 0u, 0u, g.W() | 1u});
        c.a[7] = Raw(g);
        for (int k = 0; k < 8; ++k) c.a[8 + k] = g.P(15) ? 0xFFFFFFFFu : (g.W() | 1u);
        break;
    }
    case kIdxUserList: {
        // a[3] = dword en +0x20 (solo cuenta su byte bajo); a[4..7] = m_UserTypes; a[8..11] = m_UserTypesByPedType; a[12] = el argumento (tipo de peaton)
        c.a[0] = Offset(g, kind, kSpanUserList);
        c.a[3] = g.Pk({0u, 1u, 1u, 1u, 0x100u, 0xFFFFFF00u, 0x80u, 0xFFu, 0xFFFFFFFFu, g.W(), g.W() & ~0xFFu, g.W() | 1u});
        const u32 ped = g.Pk({0u, 1u, 4u, 6u, 7u, 9u, 16u, 17u, 31u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, g.R(32), g.R(32), g.W()});
        for (int k = 0; k < 4; ++k) {
            c.a[4 + k] = g.Pk({0xFFFFFFFEu, 0xFFFFFFFEu, 0xFFFFFFFFu, 0xFFFFFFFDu, 0u, 0xFFFFFFFFu, g.W(), 0x7FFFFFFEu});
            c.a[8 + k] = g.P(45) ? ped : g.Pk({0u, 1u, 0xFFFFFFFFu, 6u, 7u, 9u, 16u, 17u, g.R(32), g.W()});
        }
        c.a[12] = ped;
        break;
    }
    case kIdxIsPedTypeGang:   // a[3] = el argumento (dword; las comparaciones son con signo)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 1u, 5u, 6u, 7u, 8u, 10u, 15u, 16u, 17u, 18u, 31u, 32u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x7FFFFFFFu, 0x80000007u, 0x00000007u, g.R(40), g.R(40), g.R(40), g.W(), g.W()});
        break;
    case kIdxLinkShutdown:  // a[3] = puntero links (+0x30; crudo: nunca se desreferencia, el falso de delete[] no lo lee)
        c.a[0] = Offset(g, kind, kSpanLink);
        c.a[3] = g.Pk({0u, 0u, 0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0xDEADBEEFu, 0x00400000u + g.R(0x800000), 0x00B74000u + g.R(0x1000), g.W(), g.W() | 1u, 0xFFFFFFFCu, 4u});
        break;
    case kIdxLinkInsert: {
        // a[3] = enlaces libres (0..3); a[4] = enlaces usados (0..2); a[5] = desplazamiento (alineado) del vector de enlaces en la arena B; a[6] = desplazamiento (alineado) del dato en la arena C; a[7] = el dato;
        // a[8..12] = el campo `data` de cada enlace (un dato del EXE, no puntero a las arenas)
        c.a[0] = Offset(g, kind, kSpanLink) & ~3u;
        c.a[3] = g.Pk({0u, 1u, 1u, 2u, 3u, 3u});
        c.a[4] = g.Pk({0u, 1u, 2u});
        c.a[5] = Offset(g, g.R(4), kSpanNodes) & ~3u;
        c.a[6] = Offset(g, g.R(4), 4) & ~3u;
        c.a[7] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x00400000u + g.R(0x800000), 0x00B74000u + g.R(0x1000), 0xDEADBEEFu, 0x00400000u + g.R(0x800000)});
        for (int k = 0; k < 5; ++k) c.a[8 + k] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x00400000u + g.R(0x800000), 0x00B74000u + g.R(0x1000), 0xDEADBEEFu});
        break;
    }
    case kIdxInAirType:     // a[3] = dword en [this] (no se lee)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
        break;
    case kIdxInAirLandDtor: // a[3] = vtable previa en [this]; a[4] = dword que el falso de ~CTaskComplex deja en [this]; a[5] = EAX del falso
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x8704ACu, 0x86E4A8u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x86E4A8u, 0x8704ACu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[5] = Raw(g);
        break;
    default:
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
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
