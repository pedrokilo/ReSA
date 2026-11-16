#pragma once
// Fixtures del banco Fn193A (lote 193A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 34 direcciones, ninguna saltada.
//   idx  direccion  nombre real                                                                      forma
//    0   0x625BA0   CTaskSimpleBeHitWhileMoving destructor con borrado (vtable 0x86D7FC)               thiscall(flags) `ret 4`: cuerpo 61FE70; CTask::operator delete 61A5B0 si bit 0
//    1   0x61FE70   CTaskSimpleBeHitWhileMoving cuerpo del destructor                                  thiscall() `ret`: [ecx]=86D7FCh; anim [+14h]: [anim+1Ch]=C0800000h, SetFinishCallback 4CEBE0(4D1390,0); 61A3A0
//    2   0x625BC0   CTaskSimpleChoking destructor con borrado (0x86D820)                                idem; cuerpo 620370
//    3   0x625BE0   CTaskSimpleBeHit destructor con borrado (0x86D844)                                  idem; cuerpo 620810
//    4   0x625C00   CTaskComplexReactToAttack destructor con borrado (0x86D868)                         idem; cuerpo 620B90
//    5   0x620B90   CTaskComplexReactToAttack cuerpo del destructor                                     thiscall() `ret`: [ecx]=86D868h; [+14h] no nulo: CleanUpOldReference 571A00(&+14h); 61A3D0
//    6   0x625C20   CTaskComplexReactToAttack::CreateNextSubTask (ranura 8)                             thiscall(ped) `ret 4`: [+0Dh] != 0: nulo; tipo de la subtarea (ranura 4) C8/D0/3F0: CreateSubTask 620C30(516h)
//    7   0x625C70   CTaskComplexReactToAttack::CreateFirstSubTask (ranura 9)                            thiscall(ped) `ret 4`: IsPlayer, GetPad, GetTarget, Normalise 44E480 (x87), ApplyMoveForce 5429F0, CreateSubTask 620C30
//    8   0x625E20   CTaskComplexKillPedOnFoot destructor con borrado (0x86D894)                         idem; cuerpo 620EF0
//    9   0x626A80   CTaskComplexKillPedOnFootStealth destructor con borrado (0x86D8C0)                  idem; cuerpo 620FF0
//   10   0x626AA0   CTaskComplexKillPedOnFootMelee destructor con borrado (0x86D8EC)                    idem; cuerpo 621090
//   11   0x6597A0   CTaskComplexUseWaterCannon::GetTaskType (ranura 4)                                  thiscall() `ret`: mov eax,6Dh
//   12   0x6597B0   CTaskComplexUseWaterCannon cuerpo del destructor                                    thiscall() `ret`: [ecx]=86F5D0h; jmp 61A3D0 (borra la subtarea)
//   13   0x659870   CTaskComplexExtinguishFireOnFoot constructor (0x86F5FC)                             thiscall(pos*) `ret 4`: base 61A3B0; [esi]=86F5FCh; copia 3 dwords a +0Ch; eax = this
//   14   0x6598A0   CTaskComplexExtinguishFireOnFoot::GetTaskType (ranura 4)                            thiscall() `ret`: mov eax,6Eh
//   15   0x6598B0   CTaskComplexExtinguishFireOnFoot cuerpo del destructor                              thiscall() `ret`: [ecx]=86F5FCh; jmp 61A3D0
//   16   0x6598C0   CTaskComplexExtinguishFireOnFoot::CreateNextSubTask (ranura 8)                      thiscall(ped) `ret 4`: tipo de la subtarea 387h: new(3Ch)+ctor 61F3F0; 3FCh: ranura 9 del propio objeto
//   17   0x659980   CTaskComplexExtinguishFireOnFoot::ControlSubTask (ranura 10)                       thiscall(ped) `ret 4`: mov eax,[ecx+8]
//   18   0x659990   CTaskComplexExtinguishFireOnFoot::FindNearestFire                                  thiscall(ped) `ret 4`: FindNearestFire 538F40 (x87): ((dz*dz + dx*dx) + dy*dy) < 100.0f
//   19   0x659D70   CTaskComplexExtinguishFireOnFoot::Clone (ranura 1)                                 thiscall() `ret`: new(18h) 61A5A0; base 61A3B0; vtable 86F5FCh; copia +0Ch
//   20   0x659DE0   CTaskSimpleAbseil destructor con borrado (0x86F45C)                                 idem; cuerpo 659E00
//   21   0x659E00   CTaskSimpleAbseil cuerpo del destructor                                             thiscall() `ret`: jmp 61A6F0 (~CTaskSimpleAnim)
//   22   0x659E10   CTaskSimpleGiveCPR destructor con borrado (0x86F4C8)                                idem; cuerpo 6588A0
//   23   0x659E70   CTaskComplexTreatAccident destructor con borrado (0x86F4EC)                         idem; cuerpo 658AE0
//   24   0x65A700   CTaskComplexExtinguishFireOnFoot destructor con borrado (0x86F5FC)                  idem; cuerpo 6598B0
//   25   0x65A720   CTaskComplexExtinguishFireOnFoot::CreateFirstSubTask (ranura 9)                     thiscall(ped) `ret 4`: FindNearestFire 659990; copia posicion; new(28h); ctor 668120
//   26   0x49E620   Fx_c constructor                                                                    thiscall() `ret`: lista de +44h (4A8DD0: tres dwords a cero); eax = this
//   27   0x49E640   Fx_c::Update                                                                        thiscall(camera, dt) `ret 8`: ecx=0A9AE80h; jmp FxManager_c::Update 4A9A80
//   28   0x49E650   Fx_c::Render                                                                        thiscall(camera, bool) `ret 8`: ecx=0A9AE80h; jmp FxManager_c::Render 4A92A0
//   29   0x49E660   Fx_c::InitStaticSystems                                                             thiscall() `ret`: 17 x CreateFxSystem 4A9BE0 (nombre, &cero, 0, 1) -> [this+0 .. +40h]
//   30   0x49E850   Fx_c::ExitStaticSystems                                                             thiscall() `ret`: 17 x DestroyFxSystem 4A9810 ([this+0 .. +40h])
//   31   0x6833C0   CTaskComplexWaitForPed::GetTaskType                                                 thiscall() `ret`: mov eax,4BDh
//   32   0x6833D0   CTaskComplexWaitForPed cuerpo del destructor (0x8706F8)                              thiscall() `ret`: [ecx]=8706F8h; [+0Ch] no nulo: CleanUpOldReference 571A00(&+0Ch); 61A3D0
//   33   0x683430   CTaskComplexWaitForPed::MakeAbortable                                               thiscall(3 args) `ret 0Ch`: mov al,1
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

namespace F193AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 34;
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
// Igual, pero dentro de la mitad baja (0) o alta (1) de la ventana: objetos distintos de una misma arena que no deben solaparse (cada uno cabe en media ventana)
inline u32 HalfOffset(Gen& g, u32 half, u32 span) { return half * 0x800u + 4 * g.R((0x800u - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un dword no nulo (puntero crudo que la funcion no desreferencia)
inline u32 RawNonZero(Gen& g) { u32 v = Raw(g); return v ? v : 0x00401000u + g.R(0x100000); }

// Un float "interesante" (bits): cero con signo, unos, bordes del 100.0f y 30.0f, denormales, NaN (silenciosos y de senal), infinitos, extremos y aleatorios moderados
inline u32 FloatBits(Gen& g) {
    const float r1 = float(i32(g.R(2000001)) - 1000000) / 1000.0f, r2 = float(i32(g.R(200001)) - 100000) / 100000.0f, r3 = float(i32(g.R(201)) - 100) * 0.5f;
    const u32 b1 = std::bit_cast<u32>(r1), b2 = std::bit_cast<u32>(r2), b3 = std::bit_cast<u32>(r3), b4 = 0x00000001u + g.R(0x7FFFFF), b5 = 0x80000001u + g.R(0x7FFFFF), b6 = g.W();
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40A00000u, 0xC0A00000u, 0x42C80000u, 0x41F00000u, 0x00000001u, 0x007FFFFFu, 0x80000001u, 0x00800000u, 0x7FC00000u, 0xFFC00001u, 0x7F800001u, 0x7F800000u, 0xFF800000u,
                 0x7F7FFFFFu, 0xFF7FFFFFu, 0x1E3CE508u, b1, b1, b1, b2, b3, b3, b4, b5, b6});
}
// Un float para posiciones: moderado la mayoria de las veces, con los especiales de FloatBits de vez en cuando
inline u32 PosBits(Gen& g) { return g.P(35) ? FloatBits(g) : std::bit_cast<u32>(float(i32(g.R(40001)) - 20000) / 8.0f); }

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kMakeAb, kSdd, kBodyRef, kBodyVt, kBodyJmp, kBhwm, kWcBody, kExtCtor, kExtClone, kExtNext, kCtrlSub, kExtFind, kExtFirst, kRtaNext, kRtaFirst, kFxCtor, kFxUpd, kFxInit, kFxExit };
inline constexpr std::array<Kind, kFunctions> kKind{kSdd, kBhwm, kSdd, kSdd, kSdd, kBodyRef, kRtaNext, kRtaFirst, kSdd, kSdd, kSdd, kConst, kWcBody, kExtCtor, kConst, kBodyVt, kExtNext, kCtrlSub, kExtFind, kExtClone, kSdd, kBodyJmp,
                                                    kSdd, kSdd, kSdd, kExtFirst, kFxCtor, kFxUpd, kFxUpd, kFxInit, kFxExit, kConst, kBodyRef, kMakeAb};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x625BA0, 0x61FE70, 0x625BC0, 0x625BE0, 0x625C00, 0x620B90, 0x625C20, 0x625C70, 0x625E20, 0x626A80, 0x626AA0, 0x6597A0, 0x6597B0, 0x659870, 0x6598A0, 0x6598B0, 0x6598C0, 0x659980,
                                                    0x659990, 0x659D70, 0x659DE0, 0x659E00, 0x659E10, 0x659E70, 0x65A700, 0x65A720, 0x49E620, 0x49E640, 0x49E650, 0x49E660, 0x49E850, 0x6833C0, 0x6833D0, 0x683430};
// Valor que devuelven las funciones constantes (por indice; 0 si no es constante)
inline constexpr std::array<u32, kFunctions> kConstValue = [] { std::array<u32, kFunctions> a{}; a[11] = 0x6D; a[14] = 0x6E; a[31] = 0x4BD; return a; }();
// Campo de referencia (desplazamiento del puntero que el cuerpo del destructor limpia) de las funciones kBodyRef
inline constexpr std::array<u32, kFunctions> kRefOff = [] { std::array<u32, kFunctions> a{}; a[5] = 0x14; a[32] = 0xC; return a; }();
static_assert(kEntry[11] == 0x6597A0 && kEntry[31] == 0x6833C0 && kEntry[33] == 0x683430 && kEntry[29] == 0x49E660 && kEntry[25] == 0x65A720);
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86D7FC, 0x86D7FC, 0x86D820, 0x86D844, 0x86D868, 0x86D868, 0x86D868, 0x86D868, 0x86D894, 0x86D8C0, 0x86D8EC, 0x86F5D0, 0x86F5D0, 0x86F5FC, 0x86F5FC, 0x86F5FC, 0x86F5FC, 0x86F5FC,
                                                     0x86F5FC, 0x86F5FC, 0x86F45C, 0x86F45C, 0x86F4C8, 0x86F4EC, 0x86F5FC, 0x86F5FC, 0, 0, 0, 0, 0, 0x8706F8, 0x8706F8, 0x8706F8};
static_assert(kVtable[31] == 0x8706F8 && kVtable[25] == 0x86F5FC && kVtable[20] == 0x86F45C && kVtable[26] == 0);
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 0x18, 0x18, 0x24, 4, 4, 4, 4, 0x18, 0x18, 4, 4, 0x18, 0x18, 4, 0x18, 4, 4, 4, 4, 4, 0x18, 0x50, 4, 4, 0x48, 0x48, 4, 0x10, 4};
static_assert(kSpan[7] == 0x24 && kSpan[26] == 0x50 && kSpan[29] == 0x48 && kSpan[32] == 0x10 && kSpan[33] == 4);
// Tamanos sinteticos: un ped ocupa 0x560 bytes en la arena B (se leen +0x14, +0x46C, +0x470, +0x540); entidad, matriz y pad
inline constexpr u32 kPedSpan = 0x560, kEntSpan = 0x40, kMatSpan = 0x40, kPadSpan = 0x120;
// Tamano de la tabla de funciones virtuales sintetica de las subtareas y de los objetos (20 ranuras)
inline constexpr u32 kVtabBytes = 0x50;
inline constexpr u32 kFxPtrs = 17;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D
//   kConst: nada mas (EAX sembrado por el caso)
//   kMakeAb: a[4..6] = ped, prioridad y evento (pila, crudos)
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
//   kBodyRef: a[4] = dword del campo de referencia (this + kRefOff), a[5] = EAX del falso de la base 61A3D0, a[6] = dword que el falso deja en [this]
//   kBodyVt: a[4] = EAX del falso de la base 61A3D0, a[5] = dword que el falso deja en [this]
//   kBodyJmp: a[4] = EAX del falso de 61A6F0, a[5] = dword que el falso deja en [this]
//   kBhwm: a[4] = animacion (0 nula, 1 en B), a[5] = desplazamiento en B (0x40 bytes), a[6] = dword que el falso de 61A3A0 deja en [this], a[7] = EAX de ese falso
//   kWcBody: a[4] = subtarea (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = desplazamiento de la tabla virtual en D
//   kExtCtor: a[4] = desplazamiento en B de los tres floats de la posicion, a[5..7] = sus tres dwords
//   kExtClone: a[4] = memoria (0 nula, 1 en B), a[5..7] = posicion de this+0xC, a[8] = desplazamiento de la memoria en B
//   kExtNext: a[4] = desplazamiento en D de la tabla virtual de `this` (mitad baja), a[5] = desplazamiento de la subtarea en B (mitad baja), a[6] = desplazamiento en D de la tabla de la subtarea (mitad alta),
//         a[7] = tipo que devuelve la ranura 4 de la subtarea, a[8] = memoria (0 nula, 1 en B), a[9] = desplazamiento de la memoria en B (mitad alta), a[10] = EAX del falso del constructor,
//         a[11] = dword que el falso deja en [mem], a[12] = EAX de la ranura 9 de `this`, a[13] = ped (pila, crudo), a[14..16] = this+0xC, +0x10, +0x14
//   kCtrlSub: a[4] = dword en this+8, a[5] = ped (pila, crudo)
//   kExtFind: a[4] = desplazamiento del ped en B, a[5] = matriz del ped (0 nula, 1 en C), a[6] = desplazamiento de la matriz en C, a[7..9] = posicion del ped (la de +4 o la de la matriz +0x30), a[10] = fuego (0 nulo, 1 en D),
//         a[11] = desplazamiento del fuego en D, a[12..14] = posicion del fuego (+4), a[15] = el falso de FindNearestFire reescribe la posicion del ped (0 no, 1 si), a[16..18] = posicion nueva
//   kExtFirst: a[4] = fuego (0 nulo, 1 en B), a[5] = desplazamiento del fuego en B, a[6..8] = posicion del fuego (+4), a[9] = memoria (0 nula, 1 en C), a[10] = desplazamiento de la memoria en C, a[11] = EAX del falso del constructor,
//         a[12] = dword que el falso deja en [mem], a[13] = ped (pila, crudo)
//   kRtaNext: a[4] = byte this+0xD, a[5] = desplazamiento de la subtarea en B (mitad baja), a[6] = desplazamiento en D de la tabla de la subtarea, a[7] = tipo de la ranura 4, a[8] = EAX del falso de CreateSubTask, a[9] = ped (pila, crudo)
//   kRtaFirst: a[4] = desplazamiento del ped en B, a[5] = arma (this+0x10), a[6] = estado (this+0x20), a[7] = agresor (desplazamiento de la entidad en C), a[8] = matriz del agresor (0 nula, 1 en C), a[9] = desplazamiento de su matriz en C,
//         a[10..12] = posicion del agresor (matriz+0x30 o +4), a[13] = matriz del ped (0 nula, 1 en D), a[14] = desplazamiento en D, a[15..17] = posicion del ped, a[18] = banderas +0x46C, a[19] = banderas +0x470, a[20] = salud (bits),
//         a[21] = DisablePlayerControls (word), a[22..27] = palabras +0, +2, +0x10, +0x12, +0x14, +0x16 del pad, a[28] = desplazamiento del pad en D, a[29] = EAX de GetTarget, a[30] = EAX de IsPlayer (1a llamada),
//         a[31] = EAX de IsPlayer (2a llamada), a[32] = EAX de CreateSubTask, a[33] = x de Normalise (bits), a[34] = y de Normalise (bits), a[35] = mascara que el falso de Normalise aplica con XOR a +0x46C del ped
//   kFxCtor: nada mas
//   kFxUpd: a[4] = camara (pila, cruda), a[5] = segundo argumento (pila, crudo)
//   kFxInit: a[4] = semilla de los 17 valores que devuelve el falso de CreateFxSystem
//   kFxExit: a[4..20] = los 17 punteros de this+0 .. +0x40 (crudos)
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
        break;
    case kMakeAb:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kBodyRef:
        a[4] = g.P(40) ? 0u : RawNonZero(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kBodyVt:
    case kBodyJmp:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kBhwm:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x40);
        a[6] = Raw(g);
        a[7] = Raw(g);
        break;
    case kWcBody:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = AuxOffset(g, kVtabBytes);
        break;
    case kExtCtor:
        a[4] = AuxOffset(g, 0xC);
        a[5] = PosBits(g); a[6] = PosBits(g); a[7] = PosBits(g);
        break;
    case kExtClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = PosBits(g); a[6] = PosBits(g); a[7] = PosBits(g);
        a[8] = AuxOffset(g, 0x18);
        break;
    case kExtNext:
        a[4] = HalfOffset(g, 0, kVtabBytes);
        a[5] = HalfOffset(g, 0, 0x10);
        a[6] = HalfOffset(g, 1, kVtabBytes);
        a[7] = g.Pk({0x387u, 0x387u, 0x387u, 0x387u, 0x3FCu, 0x3FCu, 0x3FCu, 0x3FCu, 0u, 1u, 0xFFFFFFFFu, 0x386u, 0x388u, 0x3FBu, 0x3FDu, 0x1F4u, 0x7FFFFFFFu, 0x80000000u, 0x10003FCu, 0x80000387u, g.W(), g.R(0x800)});
        a[8] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[9] = HalfOffset(g, 1, 0x40);
        a[10] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[11] = Raw(g);
        a[12] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u, 0x1234u});
        a[13] = Raw(g);
        a[14] = PosBits(g); a[15] = PosBits(g); a[16] = PosBits(g);
        break;
    case kCtrlSub:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kExtFind: {
        a[4] = AuxOffset(g, 0x20);
        a[5] = g.Pk({0u, 0u, 1u, 1u});
        a[6] = AuxOffset(g, kMatSpan);
        // posicion del ped y del fuego: cuatro de cada diez veces, un vector de distancia cercana a 10 (el umbral de 100.0f al cuadrado) con pasos de unidades de precision
        const u32 mode = g.R(10);
        if (mode < 4) {
            const std::array<std::array<float, 3>, 6> dirs{{{10.f, 0.f, 0.f}, {0.f, 10.f, 0.f}, {0.f, 0.f, 10.f}, {6.f, 8.f, 0.f}, {0.f, 6.f, 8.f}, {8.f, 0.f, 6.f}}};
            auto d = dirs[g.R(6)];
            for (auto& v : d) {
                if (v != 0.f) {
                    const i32 steps = i32(g.R(7)) - 3;
                    u32 bits = std::bit_cast<u32>(v);
                    bits = u32(i32(bits) + steps);
                    v = std::bit_cast<float>(bits);
                }
                if (g.P(50)) v = -v;
            }
            const float px = float(i32(g.R(41)) - 20), py = float(i32(g.R(41)) - 20), pz = float(i32(g.R(41)) - 20);
            a[7] = std::bit_cast<u32>(px); a[8] = std::bit_cast<u32>(py); a[9] = std::bit_cast<u32>(pz);
            a[12] = std::bit_cast<u32>(px - d[0]); a[13] = std::bit_cast<u32>(py - d[1]); a[14] = std::bit_cast<u32>(pz - d[2]);
        } else {
            a[7] = PosBits(g); a[8] = PosBits(g); a[9] = PosBits(g);
            a[12] = g.P(30) ? a[7] : PosBits(g); a[13] = g.P(30) ? a[8] : PosBits(g); a[14] = g.P(30) ? a[9] : PosBits(g);
        }
        a[10] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[11] = AuxOffset(g, 0x10);
        a[15] = g.Pk({0u, 0u, 0u, 1u});
        a[16] = PosBits(g); a[17] = PosBits(g); a[18] = PosBits(g);
        break;
    }
    case kExtFirst:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = PosBits(g); a[7] = PosBits(g); a[8] = PosBits(g);
        a[9] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[10] = AuxOffset(g, 0x40);
        a[11] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[12] = Raw(g);
        a[13] = Raw(g);
        break;
    case kRtaNext:
        a[4] = g.Pk({0u, 0u, 0u, 1u, 0xFFu, 0x100u, 0x80u, g.W() & 0xFFu, g.W() & 0xFFFFFF00u, g.W()});
        a[5] = HalfOffset(g, 0, 0x10);
        a[6] = HalfOffset(g, 1, kVtabBytes);
        a[7] = g.Pk({0xC8u, 0xC8u, 0xD0u, 0xD0u, 0x3F0u, 0x3F0u, 0u, 1u, 0xFFFFFFFFu, 0xC7u, 0xC9u, 0xCFu, 0xD1u, 0x3EFu, 0x3F1u, 0x516u, 0x7FFFFFFFu, 0x80000000u, 0x100000C8u, 0x800000D0u, g.W(), g.R(0x800)});
        a[8] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[9] = Raw(g);
        break;
    case kRtaFirst: {
        a[4] = AuxOffset(g, kPedSpan);
        a[5] = g.Pk({0x1Bu, 0x1Bu, 0x19u, 0x19u, 0x18u, 0x18u, 0x18u, 0x18u, 0x1Au, 0x1Cu, 9u, 8u, 0xAu, 0xBu, 0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x10000019u, 0x80000018u, g.W(), g.R(40), g.R(40), u32(-i32(g.R(40)))});
        a[6] = g.Pk({3u, 3u, 3u, 3u, 3u, 0u, 1u, 2u, 4u, 0xFFFFFFFFu, 0x80000003u, 0x10003u, g.W(), g.R(8)});
        a[7] = HalfOffset(g, 0, kEntSpan);
        a[8] = g.Pk({0u, 1u, 1u});
        a[9] = HalfOffset(g, 1, kMatSpan);
        a[10] = PosBits(g); a[11] = PosBits(g); a[12] = PosBits(g);
        a[13] = g.Pk({0u, 1u, 1u});
        a[14] = HalfOffset(g, 0, kMatSpan);
        a[15] = PosBits(g); a[16] = PosBits(g); a[17] = PosBits(g);
        if (g.P(15)) { a[10] = a[15]; a[11] = a[16]; a[12] = a[17]; }
        a[18] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000001u, g.W(), g.W() | 1u, g.W() & ~1u});
        a[19] = g.Pk({0x4000000u, 0x4000000u, 0x4000000u, 0x4000000u, 0x4000000u, 0u, 0xFFFFFFFFu, 0xFBFFFFFFu, 0x3FFFFFFu, 0x4000001u, g.W(), g.W() | 0x4000000u, g.W() & ~0x4000000u});
        a[20] = g.Pk({0x41EFFFFFu, 0x41F00000u, 0x41F00001u, 0x41EFFFFEu, 0u, 0x80000000u, 0xBF800000u, 0x7FC00000u, 0xFFC00001u, 0x7F800000u, 0xFF800000u, 0x00000001u, 0x7F7FFFFFu, 0x42C80000u, FloatBits(g), FloatBits(g), FloatBits(g)});
        a[21] = g.Pk({0u, 0u, 0u, 1u, 0x100u, 0xFFFFu, 0x8000u, g.W() & 0xFFFFu});
        for (u32 i = 22; i <= 27; ++i) a[i] = g.Pk({0u, 0u, 0u, 0u, 1u, 0x80u, 0x100u, 0xFFFFu, 0x8000u, g.W() & 0xFFFFu});
        if (g.P(40)) for (u32 i = 22; i <= 27; ++i) a[i] = 0;   // pad sin ninguna entrada (la funcion sigue por el resto de las comprobaciones)
        a[28] = HalfOffset(g, 1, kPadSpan) & ~0x3u;
        a[29] = g.Pk({0u, 0u, 1u, 0x100u, 0xFFFFFF00u, g.W()});
        a[30] = g.Pk({0u, 1u, 1u, 1u, 0x100u, 0xFFFFFF00u, g.W()});
        a[31] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, g.W()});
        a[32] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[33] = FloatBits(g); a[34] = FloatBits(g);
        a[35] = g.Pk({0u, 0u, 0u, 1u, 0xFFFFFFFFu, g.W()});
        break;
    }
    case kFxCtor:
        break;
    case kFxUpd:
        a[4] = Raw(g); a[5] = g.P(50) ? FloatBits(g) : Raw(g);
        break;
    case kFxInit:
        a[4] = g.W();
        break;
    case kFxExit:
        for (u32 i = 0; i < kFxPtrs; ++i) a[4 + i] = Raw(g);
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
