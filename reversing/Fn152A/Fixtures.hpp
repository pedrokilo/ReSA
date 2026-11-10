#pragma once
// Fixtures del banco Fn152A (lote 152A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py).
//   idx  direccion  nombre real                                                              forma
//    0   0x492CB0   tUserList::AddPedType (el listado decia CTaskSimpleHandsUp)            thiscall(tipo) `ret 4`: [+20h]=1; primera entrada con m_UserTypes==-1: =-2 y m_UserTypesByPedType=tipo
//    1   0x678D40   CTaskSimpleInAir::GetTaskType (vtable 0x8704D8, ranura 4)             thiscall() `ret`: mov eax,0F1h
//    2   0x678D50   CTaskSimpleInAir cuerpo del destructor (SEH)                           thiscall() `ret`: vtable 8704D8h; si [+1Ch]: SetDeleteCallback(4D1390h,0); si [+38h]: CleanUpOldReference(&[+38h]); ~CTask
//    3   0x53ED30   TestModelIndices (funcion libre, el listado decia App)                 cdecl() `ret`
//    4   0x53EDC0   CPad::IsLeftShoulder1Pressed                                          thiscall() `ret`: palabra [+8] != 0 y palabra [+38h] == 0 -> 1
//    5   0x53EE20   CPad::IsRightShoulder1Pressed                                         palabras [+0Ch] y [+3Ch]
//    6   0x53EE60   CPad::IsDPadUpPressed                                                 palabras [+10h] y [+40h]
//    7   0x53EF40   CPad::IsTrianglePressed                                               palabras [+1Eh] y [+4Eh]
//    8   0x53EF60   CPad::IsCirclePressed                                                 palabras [+22h] y [+52h]
//    9   0x4AE640   CEventPotentialWalkIntoObject::GetEventType (vtable 0x85AF18, ranura 1)    thiscall() `ret`: mov eax,2Ch
//   10   0x4AE650   CEventPotentialWalkIntoObject::GetLifeTime (ranura 3)                  thiscall() `ret`: xor eax,eax
//   11   0x4AE660   CEventPotentialWalkIntoObject::GetEventPriority (ranura 2)              thiscall() `ret`: mov eax,1Bh
//   12   0x4AE6D0   CEventPotentialWalkIntoObject::TakesPriorityOver (ranura 11)            thiscall(evento) `ret 4`: mov al,1
//   13   0x4B4AA0   CEventKnockOffBike::operator= (el listado decia CEventPotentialWalkIntoObject)  thiscall(otro) `ret 4`: From(otro) 4AFDD0 (con hook); eax = this
//   14   0x4B4A50   CEventKnockOffBike copia (constructor, marco SEH)                     thiscall(otro) `ret 4`: [+4]=0; byte [+8]=0; vtable 85B2E0h; From(otro); eax = this
//   15   0x6426E0   CTaskSimpleCarDrive destructor con borrado (vtable 0x86E904)          thiscall(flags) `ret 4`: cuerpo 63C460 (con hook), CTask::operator delete 61A5B0 si bit 0
//   16   0x642700   CTaskSimpleCarDrive::StartRollingDoor                                 thiscall(ped) `ret 4`: GetGroup 6E3B00, AddAnimation 4D3AA0, [+0Ch]=anim, SetFinishCallback 4CEBE0
//   17   0x6C0560   CBmx::BlowUpCar (vtable 0x871528, ranura 41)                          thiscall(entidad, bool) `ret 8`: vacia
//   18   0x6C0570   CBmx destructor con borrado (vtable 0x871528)                         thiscall(flags) `ret 4`: cuerpo 6BF9D0, CVehicle::operator delete 6E2D90 si bit 0
//   19   0x6BF9D0   CBmx cuerpo del destructor (SEH)                                      thiscall() `ret`: vtable 871528h; CAEVehicleAudioEntity::Terminate(+138h) 4FB8C0; ~CBike 6B57A0
//   20   0x62F270   CTaskComplexLeaveCarAndDie cuerpo del destructor                      thiscall() (`jmp`): vtable 86DD00h; ~CTaskComplexLeaveCar 63B970
//   21   0x62F250   CTaskComplexLeaveCarAndDie destructor con borrado (vtable 0x86DD00)   thiscall(flags) `ret 4`: cuerpo 62F270, CTask::operator delete si bit 0
//   22   0x6422A0   CTaskComplexLeaveCarAndWander destructor con borrado (vtable 0x86E880) thiscall(flags) `ret 4`: cuerpo 63BE50 (con hook), CTask::operator delete si bit 0
//   23   0x62F2D0   IsExtraSunnyWeather (funcion libre; el listado decia CTaskComplexLeaveCarAndDie)  cdecl(palabra) `ret`: AL = tiempo en {0,2,6,0Bh,0Dh,11h}
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

namespace F152AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 24;

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
constexpr std::size_t kSlots = 24;
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kSlots> a{};
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

// Clases de funcion (forma)
enum Kind : u32 { kSdd, kConst, kBoolArg, kNopCdecl, kNopThis, kPad, kUserList, kInAirBody, kAssign, kCopyCtor, kBmxBody, kLeaveBody, kRollDoor, kWeather };
inline constexpr std::array<Kind, kFunctions> kKind{
    kUserList, kConst, kInAirBody, kNopCdecl, kPad, kPad, kPad, kPad, kPad, kConst, kConst, kConst,
    kBoolArg, kAssign, kCopyCtor, kSdd, kRollDoor, kNopThis, kSdd, kBmxBody, kLeaveBody, kSdd, kSdd, kWeather};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x492CB0, 0x678D40, 0x678D50, 0x53ED30, 0x53EDC0, 0x53EE20, 0x53EE60, 0x53EF40, 0x53EF60, 0x4AE640, 0x4AE650, 0x4AE660,
                                                    0x4AE6D0, 0x4B4AA0, 0x4B4A50, 0x6426E0, 0x642700, 0x6C0560, 0x6C0570, 0x6BF9D0, 0x62F270, 0x62F250, 0x6422A0, 0x62F2D0};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor, ~CTask, ~CBike, ~CTaskComplexLeaveCar; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0x61A3A0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x63C460, 0, 0, 0x6BF9D0, 0x6B57A0, 0x63B970, 0x62F270, 0x63BE50, 0};
// Funcion de liberacion de los destructores con borrado (CTask::operator delete, CVehicle::operator delete)
inline constexpr std::array<u32, kFunctions> kDelete{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x61A5B0, 0, 0, 0x6E2D90, 0, 0, 0x61A5B0, 0x61A5B0, 0};
// vtable de la clase (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0x8704D8, 0x8704D8, 0, 0, 0, 0, 0, 0, 0x85AF18, 0x85AF18, 0x85AF18, 0x85AF18, 0x85B2E0, 0x85B2E0, 0x86E904, 0x86E904, 0x871528, 0x871528, 0x871528, 0x86DD00, 0x86DD00, 0x86E880, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x24, 4, 0x3C, 4, 0x58, 0x58, 0x58, 0x58, 0x58, 4, 4, 4, 4, 0x10, 0x10, 4, 0x10, 4, 4, 4, 4, 4, 4, 4};
// Pad: desplazamiento de la palabra "nueva" (la "anterior" esta 0x30 mas arriba)
inline constexpr std::array<u32, kFunctions> kPadOff{0, 0, 0, 0, 0x08, 0x0C, 0x10, 0x1E, 0x22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Constantes que devuelven las funciones de kConst
inline constexpr std::array<u32, kFunctions> kConstant{0, 0xF1, 0, 0, 0, 0, 0, 0, 0, 0x2C, 0, 0x1B, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

inline u32 Junk16(Gen& g) { return g.W() & 0xFFFFu; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y reserva
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kConst: a[3] = dword en [this] (no se lee)
//   kBoolArg: a[3] = dword en [this]; a[4] = argumento crudo (evento)
//   kNopCdecl: nada; kNopThis: a[3] = dword en [this]; a[4], a[5] = argumentos crudos
//   kPad: a[3] = palabra nueva; a[4] = palabra anterior; a[5], a[6] = basura de la otra mitad del dword de cada una
//   kUserList: a[3..6] = m_UserTypes; a[7..10] = m_UserTypesByPedType; a[11] = dword en +0x20; a[16] = tipo de peaton (argumento)
//   kInAirBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = animacion (+0x1C); a[7] = entidad (+0x38)
//   kAssign / kCopyCtor: a[3] = dword previo en [this]; a[4] = gRet (EAX del falso de From); a[5] = gDtorWrite (el falso lo escribe en [this+4]); a[6] = desplazamiento del otro evento en B;
//                        a[7] = dword previo en +4; a[8] = dword previo en +8
//   kBmxBody / kLeaveBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite
//   kRollDoor: a[3] = dword previo en +0xC de la tarea; a[6] = desplazamiento del peaton en B; a[7] = clump (+0x18 del peaton); a[8] = desplazamiento del vehiculo en C; a[9] = desplazamiento del
//              handling en B; a[10] = grupo de animacion (byte +0xDE del handling); a[11] = relleno del dword +0xDC del handling; a[16], a[17] = EAX de GetGroup y de AddAnimation
//   kWeather: a[3] = argumento de pila (solo cuenta la palabra baja)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    switch (kKind[c.fn]) {
    case kSdd:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kConst: case kNopCdecl:
        c.a[3] = Raw(g);
        break;
    case kBoolArg: case kNopThis:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kPad: {
        const u32 anyWord = g.Pk({1u, 0xFFFFu, 0x8000u, 0x0100u, 0x00FFu, 0x7FFFu, Junk16(g) | 1u, Junk16(g) | 0x100u, 2u, 0x4000u});
        c.a[3] = g.P(50) ? 0u : anyWord;
        c.a[4] = g.P(50) ? 0u : g.Pk({1u, 0xFFFFu, 0x8000u, 0x0100u, 0x00FFu, Junk16(g) | 0x80u});
        if (g.P(10)) c.a[4] = c.a[3];
        c.a[5] = g.Pk({0u, 0xFFFFu, Junk16(g), Junk16(g), 1u});
        c.a[6] = g.Pk({0u, 0xFFFFu, Junk16(g), Junk16(g), 1u});
        break;
    }
    case kUserList: {
        for (unsigned i = 0; i < 4; ++i) c.a[3 + i] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFEu, 0u, 1u, 0xFFFFFFFDu, g.W(), 0x80000000u, 0x7FFFFFFFu});
        for (unsigned i = 0; i < 4; ++i) c.a[7 + i] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, 1u, 5u, g.W(), 0xDEADBEEFu});
        if (g.P(12)) for (unsigned i = 0; i < 4; ++i) c.a[3 + i] = 0xFFFFFFFFu;      // todas libres
        if (g.P(12)) for (unsigned i = 0; i < 4; ++i) c.a[3 + i] = g.Pk({0xFFFFFFFEu, 0u, 3u});   // ninguna libre
        c.a[11] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), 0x00000100u, 0xFFFFFF00u, 0x01010101u, g.W()});
        c.a[16] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.W()});
        break;
    }
    case kInAirBody:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x86D48Cu, 0x86D4A8u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.P(45) ? 0u : (Raw(g) | 4u);
        c.a[7] = g.P(45) ? 0u : (Raw(g) | 4u);
        break;
    case kAssign: case kCopyCtor:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AB38u, 0x85AF18u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = Offset(g, g.R(4), 0x3C);
        c.a[7] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0x7FFFFFFFu});
        c.a[8] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0x01010101u, 0xFFFFFF00u, 0x000000FFu});
        break;
    case kBmxBody: case kLeaveBody:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x86D48Cu, 0x85AA6Cu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kRollDoor: {
        c.a[3] = Raw(g);
        c.a[6] = g.Pk({4u * g.R(0x80), 4u * g.R(0x80) + 1u + g.R(3), 0u, 1u, 0x1FCu});                // peaton (0x590 bytes) en la mitad baja de B
        c.a[7] = Raw(g);
        c.a[8] = Offset(g, g.R(4), 0x388);                                                            // vehiculo en C
        c.a[9] = g.Pk({0x800u + 4u * g.R(0x1C0), 0x800u + 4u * g.R(0x1C0) + 1u + g.R(3), 0x800u, 0xF1Cu}); // handling (0xE0 bytes) en la mitad alta de B
        c.a[10] = g.Pk({0u, 1u, 2u, 0x1Du, 0x1Eu, 0x1Fu, 0x80u, 0xFFu, g.R(256), g.R(256), g.R(30), g.R(30)});
        c.a[11] = g.W();
        c.a[16] = Raw(g);
        c.a[17] = Raw(g);
        break;
    }
    case kWeather: {
        const u32 w = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 0xFFFFu, 0x8000u, 0x100u, 0x10000u, g.R(0x20), g.R(0x20), g.R(0x20), g.W() & 0xFFFFu, g.W()});
        const u32 hi = g.Pk({0u, 0u, 0xFFFF0000u, g.W() & 0xFFFF0000u, 0x00010000u});
        c.a[3] = (w & 0xFFFFu) | (w > 0xFFFFu ? w & 0xFFFF0000u : hi);
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
