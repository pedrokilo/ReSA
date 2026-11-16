#pragma once
// Fixtures del banco Fn194A (lote 194A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en el lote 194A SIN validar: reversing/PendienteValidar/194A.md; las clases del listado salen del desensamblado, de las vtables y de los llamadores). 33 asignadas: 33 reconstruidas.
//   idx  direccion  nombre real                                                                      forma
//    0   0x683440   CTaskComplexWaitForPed::CreateNextSubTask (ranura 8)                              thiscall(ped) `ret 4`: xor eax,eax
//    1   0x683450   CTaskComplexWaitForPed::CreateFirstSubTask (ranura 9)                             thiscall(ped) `ret 4`: arranca el temporizador, CTask::operator new 61A5A0 (0x20) + ctor de StandStill 62F310
//    2   0x6834E0   CTaskComplexWaitForPed::ControlSubTask (ranura 10)                                thiscall(ped) `ret 4`: x87 a 24 bits, evento local CEventDeadPed, FindTaskByType, matriz, GetRadianAngleBetweenPoints
//    3   0x669FB0   CTaskComplexWanderGang::CanJoinGang                                                thiscall() `ret`: IsOutOfTime expandido (+0x28 inicio, +0x2C intervalo, +0x30 arrancado, +0x31 parado)
//    4   0x669FF0   CTaskComplexWander::GetDistSqOfClosestPathNodeToPed                                thiscall(ped) `ret 4` float en ST0: dos nodos (+0x18/+0x1A anterior, +0x1C/+0x1E siguiente), x87 a 24 bits
//    5   0x66F4A0   CTaskComplexWander destructor con borrado (vtable 0x86FE84)                         thiscall(flags) `ret 4`: cuerpo 669D20; CTask::operator delete 61A5B0 si bit 0
//    6   0x673EF0   CTaskComplexWander::GetWanderTaskByPedTypeId (sin llamadores)                      cdecl(tipo) `ret`: tabla de saltos sobre tipo-6; operator new 61A5A0, rand/407180, constructor de la tarea
//    7   0x4C5CB0   CModelInfo: constructor de CStore<CAtomicModelInfo,14000>                         thiscall() `ret`: eh vector constructor iterator (821D30) con el constructor de elemento 4C5540; +0 = 0
//    8   0x4C5D10   CStore<CDamageAtomicModelInfo,70>   (elemento 0x24, ctor 4C55A0)                  idem
//    9   0x4C5D70   CStore<CLodAtomicModelInfo,1>       (elemento 0x24, ctor 4C55E0)                  idem
//   10   0x4C5DD0   CStore<CTimeModelInfo,169>          (elemento 0x24, ctor 4C5640)                  idem
//   11   0x4C5E30   CStore<CLodTimeModelInfo,1>         (elemento 0x28, ctor 4C5690)                  idem
//   12   0x4C5E90   CStore<CWeaponModelInfo,51>         (elemento 0x28, ctor 4C5760)                  idem
//   13   0x4C5EF0   CStore<CClumpModelInfo,92>          (elemento 0x24, ctor 4C56F0)                  idem
//   14   0x4C5F50   CStore<CVehicleModelInfo,212>       (elemento 0x308, ctor 4C75E0)                 idem
//   15   0x4C67D0   CStore<CPedModelInfo,278>           (elemento 0x44, ctor 4C57A0)                  idem
//   16   0x4C6200   CBaseModelInfo: salto a la ranura 7 (Shutdown)                                     thiscall() : mov eax,[ecx]; jmp [eax+1Ch]
//   17   0x4C6210   CAtomicModelInfo destructor con borrado (vtable 0x85BBF0)                          thiscall(flags) `ret 4`: cuerpo 4C5590; operator delete 8214BD si bit 0
//   18   0x4C6230   CDamageAtomicModelInfo destructor con borrado (0x85BC30)                           idem, cuerpo 4C55D0
//   19   0x4C6250   CLodAtomicModelInfo destructor con borrado (0x85BC70)                              idem, cuerpo 4C5620
//   20   0x4C6270   CTimeModelInfo destructor con borrado (0x85BCB0)                                   idem, cuerpo 4C5680
//   21   0x4C6290   CLodTimeModelInfo destructor con borrado (0x85BCF0)                                idem, cuerpo 4C56E0
//   22   0x4C62B0   CClumpModelInfo destructor con borrado (0x85BD30)                                  idem, cuerpo 4C5750
//   23   0x4C62D0   CWeaponModelInfo destructor con borrado (0x85BD78)                                 idem, cuerpo 4C5790
//   24   0x4C6350   CPedModelInfo destructor con borrado (0x85BDC0)                                    idem, cuerpo 4C62F0
//   25   0x4C62F0   CPedModelInfo cuerpo del destructor                                               thiscall() `ret`: [this]=85BDC0; si [this+34h]: 40F700 y 40FC40; [this]=85BB9C
//   26   0x4C5790   CWeaponModelInfo cuerpo del destructor                                            thiscall() `ret`: [this]=85BB9C
//   27   0x4C6B10   CModelInfoAccelerator::FreeModelInfoIds                                            thiscall() `ret`: operator delete 8214BD([this]); [this]=0
//   28   0x4C6B70   CModelInfoAccelerator::GetModelInfoIdFile                                          thiscall() `ret`: OpenFile 538900, operator new 82119A (0xA08C) a cero, Read 538950, CloseFile 5389D0
//   29   0x4C6BD0   CModelInfoAccelerator::EndOfLoadPhase                                              thiscall() `ret`: OpenFileForWriting 538910, Write 538970, CloseFile 5389D0, operator delete 8214BD
//   30   0x66CA40   CTaskComplexSeekEntity<FixedPos> constructor (vtable 0x87004C)                      thiscall(8 args) `ret 20h`: base 61A3B0, campos, RegisterReference 571B70
//   31   0x66CB90   CTaskComplexSeekEntity<FixedPos> destructor con borrado                             thiscall(flags) `ret 4`: cuerpo 66CBB0; CTask::operator delete 61A5B0 si bit 0
//   32   0x66CBB0   CTaskComplexSeekEntity<FixedPos> cuerpo del destructor                              thiscall() `ret`: [this]=87004C; CleanUpOldReference 571A00; [this+40h]=859DC4; ~CTaskComplex 61A3D0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D de 4 KB con guardas de 0x40 y la grande E de 0x71000 bytes para los almacenes y el bloque de ids) y los colaboradores falseados (el mismo
// falso en la direccion del EXE y en el cuerpo de la DLL cuando la DLL los llama por su funcion).
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

namespace F194AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 33;
constexpr unsigned kArenas = 5;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;    // ventana de las arenas A, B, C y D
constexpr std::size_t kBigWin = 0x71000; // ventana de la arena E (448004 bytes del almacen de atomicos + margen)
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

// Desplazamiento de un objeto de `span` bytes en una ventana de `win` bytes segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda; solo con `edge`), 3 alineado cerca del borde o del inicio
inline u32 OffsetIn(Gen& g, u32 kind, u32 span, u32 win, bool edge) {
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return edge ? win - span - 4 + g.R(0x30) : win - span - 4 * g.R(8);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
inline u32 Offset(Gen& g, u32 kind, u32 span) { return OffsetIn(g, kind, span, u32(kWin), true); }
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNz(Gen& g) { return Raw(g) | 1u; }
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    const u32 huge = sign | (0x7E800000u + (g.W() & 0x007FFFFFu));
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, huge, huge, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
// Un float finito normal positivo con exponente sesgado en [lo, hi)
inline u32 PosNormal(Gen& g, u32 lo, u32 hi) { return ((lo + g.R(hi - lo)) << 23) | (g.W() & 0x7FFFFFu); }
// Valores de las posiciones y radios de las funciones con x87. Dos NaN de cargas distintas en una misma operacion son ambiguos (la x87 elige segun el orden de los operandos): cada caso lleva como mucho UN NaN de
// usuario (perfil 6, segunda mitad); los infinitos solo producen el NaN indefinido, identico en todos los casos (perfil 6, primera mitad); el perfil 7 mezcla de todo SIN NaN de usuario.
inline u32 SphFloat(Gen& g, u32 profile, bool nanHere = false) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 moderate = sign | PosNormal(g, 110, 150);
    if (profile < 5) return g.P(6) ? sign : moderate;
    if (profile == 5) return g.Pk({moderate, moderate, sign | (g.W() & 0x7FFFFFu), sign | PosNormal(g, 1, 40), sign | PosNormal(g, 40, 110), sign | PosNormal(g, 40, 110), sign | PosNormal(g, 180, 254), sign | 0x7F7FFFFFu, sign | 0x00800000u, sign | 0x3F800000u, sign});
    if (profile == 6) {
        if (nanHere) return sign | 0x7FC00000u | (g.W() & 0x3FFFFFu) | (g.P(50) ? 0u : 0x80000000u);
        return g.P(15) ? (sign | 0x7F800000u) : moderate;
    }
    u32 v;
    do { v = FloatBits(g); } while (IsNanBits(v));
    return v;
}

// Clases de funcion (forma)
enum Kind : u32 { kNext, kFirst, kControl, kCanJoin, kDistSq, kSdd, kWanderNew, kStore, kThunk, kWeaponBody, kPedBody, kFreeIds, kGetFile, kEndLoad, kSeekCtor, kSeekBody };
inline constexpr std::array<Kind, kFunctions> kKind{kNext, kFirst, kControl, kCanJoin, kDistSq, kSdd, kWanderNew,
                                                    kStore, kStore, kStore, kStore, kStore, kStore, kStore, kStore, kStore,
                                                    kThunk, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kSdd, kPedBody, kWeaponBody,
                                                    kFreeIds, kGetFile, kEndLoad, kSeekCtor, kSdd, kSeekBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x683440, 0x683450, 0x6834E0, 0x669FB0, 0x669FF0, 0x66F4A0, 0x673EF0,
                                                    0x4C5CB0, 0x4C5D10, 0x4C5D70, 0x4C5DD0, 0x4C5E30, 0x4C5E90, 0x4C5EF0, 0x4C5F50, 0x4C67D0,
                                                    0x4C6200, 0x4C6210, 0x4C6230, 0x4C6250, 0x4C6270, 0x4C6290, 0x4C62B0, 0x4C62D0, 0x4C6350, 0x4C62F0, 0x4C5790,
                                                    0x4C6B10, 0x4C6B70, 0x4C6BD0, 0x66CA40, 0x66CB90, 0x66CBB0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x8706F8, 0x8706F8, 0x8706F8, 0x8700D4, 0x86FE84, 0x86FE84, 0,
                                                     0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                     0x85BB9C, 0x85BBF0, 0x85BC30, 0x85BC70, 0x85BCB0, 0x85BCF0, 0x85BD30, 0x85BD78, 0x85BDC0, 0x85BDC0, 0x85BB9C,
                                                     0, 0, 0, 0x87004C, 0x87004C, 0x87004C};
// Bytes que cada funcion toca a partir de su puntero `this` (arena A)
inline constexpr std::array<u32, kFunctions> kSpan{0x2C, 0x2C, 0x2C, 0x34, 0x20, 4, 4,
                                                   0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                   4, 4, 4, 4, 4, 4, 4, 4, 4, 0x38, 4,
                                                   4, 0x20, 0x20, 0x58, 4, 0x58};
// Almacenes (CStore): capacidad, tamano de elemento, constructor de elemento del EXE (0x4C5CB0..0x4C67D0 = indices 7..15)
inline constexpr std::array<u32, kFunctions> kCap{0, 0, 0, 0, 0, 0, 0, 14000, 70, 1, 169, 1, 51, 92, 212, 278, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr std::array<u32, kFunctions> kElemSize{0, 0, 0, 0, 0, 0, 0, 0x20, 0x24, 0x24, 0x24, 0x28, 0x28, 0x24, 0x308, 0x44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr std::array<u32, kFunctions> kElemCtor{0, 0, 0, 0, 0, 0, 0, 0x4C5540, 0x4C55A0, 0x4C55E0, 0x4C5640, 0x4C5690, 0x4C5760, 0x4C56F0, 0x4C75E0, 0x4C57A0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Cuerpo (colaborador) de los destructores con borrado y direccion del operator delete que usan (0x61A5B0 tareas, 0x8214BD el resto)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0x669D20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x4C5590, 0x4C55D0, 0x4C5620, 0x4C5680, 0x4C56E0, 0x4C5750, 0x4C5790, 0x4C62F0, 0, 0, 0, 0, 0, 0, 0x66CBB0, 0};
inline constexpr std::array<u32, kFunctions> kDeleteExe{0, 0, 0, 0, 0, 0x61A5B0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x8214BD, 0x8214BD, 0x8214BD, 0x8214BD, 0x8214BD, 0x8214BD, 0x8214BD, 0x8214BD, 0, 0, 0, 0, 0, 0, 0x61A5B0, 0};

// Memoria global del juego tocada
inline constexpr u32 kClock = 0xB7CB84;         // CTimer::m_snTimeInMilliseconds
inline constexpr u32 kPathTable = 0x96F854;     // ThePaths.m_pPathNodes[72]
inline constexpr u32 kPathAreas = 72;
inline constexpr u32 kFileBlock = 0xA08C;       // bytes del bloque de ids de CModelInfoAccelerator

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A (en la E para los almacenes), a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D, a[40] = semilla de E
//   kNext: a[4] = el peaton (pila, crudo)
//   kFirst: a[4] = ped esperado (+0xC, crudo), a[5] = tiempo (+0x14), a[6] = reloj 0xB7CB84, a[7] = modo del resultado de CTask::operator new (0 nulo, 1 arena B, 2 crudo), a[8] = desplazamiento en B, a[9] = valor crudo,
//            a[10] = EAX del falso del constructor, a[11] = el peaton (pila)
//   kControl: a[4] = subtarea (+8, cruda), a[5] = modo del ped esperado (0 nulo, 1 en B), a[6] = desplazamiento en B, a[7] = radio (bits), a[8] = byte de rotar (+0x18), a[9] = byte de arrancado (+0x24), a[10] = contador (+0x28),
//            a[11] = desplazamiento del ped recibido en C, a[12] = modo de su matriz (0 nula, 1 en D), a[13] = desplazamiento de esa matriz en D, a[14..16] = su posicion, a[17] = su inteligencia (+0x47C, cruda),
//            a[18..20] = posicion del ped esperado, a[21] = modo de su matriz, a[22] = desplazamiento de esa matriz en D, a[23..25] = posicion (+0x30) de la matriz del recibido, a[26..28] = idem del esperado,
//            a[29] = inteligencia del esperado (cruda), a[30] = EAX del falso de IsAlive, a[31] = EAX del falso de IsOutOfTime, a[32] = modo de FindTaskByType (0 nulo, 1 tarea en D), a[33] = desplazamiento de la tarea en D,
//            a[41..43] = vector local de la tarea (+0x14), a[44..46] = lo que escribe el falso de la multiplicacion, a[47] = lo que devuelve GetRadianAngleBetweenPoints (bits), a[48] = lo que devuelve LimitRadianAngle,
//            a[49] = reloj, a[50] = perfil de flotantes, a[51] = ped muerto/vivo forzado (0 libre)
//   kCanJoin: a[4] = byte +0x30, a[5] = byte +0x31, a[6] = inicio (+0x28), a[7] = intervalo (+0x2C), a[8] = reloj
//   kDistSq: a[4] = modo del siguiente (0 invalido 0xFFFF, 1 cargado, 2 area valida sin cargar), a[5] = area, a[6] = id, a[7..9] = idem del anterior (a[8] = area; si es igual a la del siguiente se comparte), a[10] = desplazamiento en C
//            de los nodos del siguiente, a[11] = desplazamiento en B de los del anterior, a[12..14] = coordenadas int16 (x, y, z) del siguiente, a[15..17] = del anterior, a[18] = desplazamiento del ped en B, a[19..21] = su posicion,
//            a[22] = modo de su matriz (0 nula, 1 en C), a[23] = desplazamiento de la matriz en C, a[24..26] = posicion (+0x30) de la matriz, a[27] = perfil
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kWanderNew: a[4] = tipo de peaton (pila, crudo), a[5] = modo del resultado de operator new (0 nulo, 1 arena B), a[6] = desplazamiento en B, a[7] = valor de rand(), a[8] = EAX del falso del constructor
//   kStore: a[4..11] = indices de elemento observados, a[12] = relleno inicial del contador (+0) como dword (ya en la arena)
//   kThunk: a[4] = desplazamiento de la tabla virtual en D, a[5] = puntero de la tabla (0 = en la arena D)
//   kWeaponBody: (solo a[3])
//   kPedBody: a[4] = modo de +0x34 (0 nulo, 1 arena B, 2 crudo), a[5] = desplazamiento en B, a[6] = valor crudo
//   kFreeIds: a[4] = modo del puntero (0 nulo, 1 arena E, 2 crudo), a[5] = desplazamiento en E, a[6] = valor crudo
//   kGetFile: a[4] = resultado de OpenFile (handle), a[5] = desplazamiento del bloque que devuelve operator new en E, a[6] = longitud de la ruta, a[7..10] = bytes que escribe el falso de Read (cuatro palabras), a[11] = EAX del falso de Read
//   kEndLoad: a[4] = modo del puntero (+0, 0 nulo, 1 arena E, 2 crudo), a[5] = desplazamiento en E, a[6] = valor crudo, a[7] = byte +0x1B, a[8] = resultado de OpenFileForWriting, a[9] = longitud de la ruta
//   kSeekCtor: a[4] = entidad (cruda), a[5] = intervalo de busqueda, a[6] = intervalo de escaneo, a[7] = distancia maxima (bits), a[8] = radio (bits), a[9] = umbral de altura (bits), a[10] = bool de cansancio (dword de pila), a[11] = bool de cara
//   kSeekBody: a[4] = entidad (+0xC, cruda)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[36] = c.seed * 13u + 50u; a[37] = a[36] + 1u; a[38] = a[36] + 2u; a[39] = a[36] + 3u; a[40] = a[36] + 4u;
    const bool store = kKind[c.fn] == kStore;
    const u32 span = store ? 4 + kCap[c.fn] * kElemSize[c.fn] : kSpan[c.fn];
    a[0] = store ? OffsetIn(g, kind, span, u32(kBigWin), false) : Offset(g, kind, span);
    const u32 vt = kVtable[c.fn];
    a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu});
    const u32 profile = (n / 16) % 8;
    // un float de usuario del caso; `nanAt` = indice del unico NaN (perfil 6 con la segunda mitad)
    const auto SF = [&](u32 slot, u32 nanAt) { return SphFloat(g, profile, profile == 6 && (n / 128) % 2 == 1 && slot == nanAt); };
    switch (kKind[c.fn]) {
    case kNext: a[4] = Raw(g); break;
    case kFirst:
        a[4] = g.P(20) ? 0u : RawNz(g);
        a[5] = Raw(g); a[6] = Raw(g);
        a[7] = g.Pk({0u, 1u, 1u, 1u, 2u});
        a[8] = AuxOffset(g, 0x20);
        a[9] = Raw(g); a[10] = Raw(g); a[11] = Raw(g);
        break;
    case kControl: {
        a[4] = Raw(g);
        a[5] = g.P(12) ? 0u : 1u;
        a[6] = AuxOffset(g, 0x500);
        a[8] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
        a[9] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
        a[10] = g.Pk({0u, 0u, 0u, 1u, 2u, 9u, 10u, 11u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 100u, g.W()});
        a[11] = AuxOffset(g, 0x600);
        a[12] = g.Pk({0u, 1u, 1u});
        a[13] = 0x100u + 0x80u * g.R(4) + 4u * g.R(4);
        a[17] = Raw(g);
        a[22] = 0x600u + 0x80u * g.R(4) + 4u * g.R(4);
        a[21] = g.Pk({0u, 1u, 1u});
        a[29] = Raw(g);
        a[30] = g.Pk({0u, 1u, 1u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, g.W(), g.W() & ~0xFFu, g.W() | 1u});
        a[31] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, g.W(), g.W() & ~0xFFu, g.W() | 1u});
        a[32] = g.Pk({0u, 1u, 1u});
        a[33] = 0x400u + 0x40u * g.R(8) + 4u * g.R(4);
        a[49] = Raw(g);
        a[50] = profile;
        a[51] = g.Pk({0u, 0u, 0u, 0u, 1u, 1u});
        // posiciones (indices 0..17 = ped recibido, ped esperado, matriz del recibido, matriz del esperado, vector de la tarea, salida del falso)
        const u32 nanAt = g.R(19);
        for (u32 i = 0; i < 3; ++i) { a[14 + i] = SF(i, nanAt); a[18 + i] = SF(3 + i, nanAt); a[23 + i] = SF(6 + i, nanAt); a[26 + i] = SF(9 + i, nanAt); a[41 + i] = SF(12 + i, nanAt); a[44 + i] = SF(15 + i, nanAt); }
        // radio: nunca NaN con los demas valores NaN (un solo NaN por caso salvo que la radio sea el NaN)
        a[7] = SF(18, nanAt);
        if (profile == 6 && !((n / 128) % 2 == 1 && nanAt == 18)) a[7] = SphFloat(g, 0);
        a[47] = FloatBits(g); a[48] = FloatBits(g);
        break;
    }
    case kCanJoin: {
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
        a[5] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
        a[6] = g.Pk({0u, g.W(), g.W(), 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.R(100000)});
        a[7] = g.Pk({0u, 0u, 1u, 2u, 5u, 1000u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(5000)});
        a[8] = (a[6] + a[7]) + g.Pk({0u, 0u, 1u, 0xFFFFFFFFu, 2u, 0xFFFFFFFEu, g.W(), 0x80000000u});
        if (g.P(15)) a[8] = g.W();
        break;
    }
    case kDistSq: {
        const auto Mode = [&](u32 base) {
            a[base] = g.Pk({0u, 1u, 1u, 1u, 2u});
            a[base + 1] = g.Pk({0u, 1u, 5u, 71u, g.R(72), g.R(72), g.R(72)});
            a[base + 2] = g.Pk({0u, 1u, 2u, 7u, g.R(0x20), g.R(0x20), g.R(0x20)});
        };
        Mode(4); Mode(7);
        if (g.P(15)) { a[8] = a[5]; a[7] = a[4]; }   // misma area: los dos nodos comparten almacen y modo
        a[10] = 4u * g.R(0x180); a[11] = 0x200u + 4u * g.R(0x100);
        const auto Coord = [&](u32 at) { for (u32 i = 0; i < 3; ++i) a[at + i] = g.Pk({0u, 1u, 0xFFFFu, 8u, 0xFFF8u, 0x7FFFu, 0x8000u, g.W() & 0xFFFFu, g.W() & 0xFFFFu, g.W() & 0xFFFFu, g.R(2000), (0x10000u - g.R(2000)) & 0xFFFFu}); };
        Coord(12); Coord(15);
        a[18] = 4u * g.R(0x40);
        const u32 nanAt = g.R(6);
        for (u32 i = 0; i < 3; ++i) { a[19 + i] = SF(i, nanAt); a[24 + i] = SF(3 + i, nanAt); }
        a[22] = g.Pk({0u, 1u, 1u});
        a[23] = 0xA00u + 4u * g.R(0x60);
        a[27] = profile;
        break;
    }
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kWanderNew:
        a[4] = g.Pk({6u, 18u, 20u, 22u, 6u, 18u, 20u, 22u, 0u, 1u, 5u, 7u, 8u, 19u, 21u, 23u, 17u, 24u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFAu, 0xFFFFFFFBu, g.R(40), g.R(40), g.W()});
        a[5] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[6] = AuxOffset(g, 0x60);
        a[7] = g.Pk({0u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x1000u, 0x0FFFu, 0x3000u, 0x7000u, 0x10000u, g.W(), g.W(), g.R(0x8000), g.R(0x8000), 0xFFFFFFFFu});
        a[8] = Raw(g);
        break;
    case kStore:
        for (u32 i = 0; i < 8; ++i) a[4 + i] = g.R(kCap[c.fn]);
        a[4] = 0; a[5] = kCap[c.fn] - 1;
        break;
    case kThunk:
        a[4] = 0x100u + 4u * g.R(0x300);
        break;
    case kWeaponBody:
        break;
    case kPedBody:
        a[4] = g.Pk({0u, 1u, 1u, 2u});
        a[5] = AuxOffset(g, 0x30);
        a[6] = RawNz(g);
        break;
    case kFreeIds:
        a[4] = g.Pk({0u, 1u, 2u, 2u});
        a[5] = 4u * g.R(0x70000u / 4);
        a[6] = Raw(g);
        break;
    case kGetFile:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 5u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFEu, g.W(), g.W(), g.R(1000) + 1u, 0x100u, 0xFFFFu});
        a[5] = OffsetIn(g, kind, kFileBlock, u32(kBigWin), false);
        a[6] = g.R(14);
        for (u32 i = 0; i < 4; ++i) a[7 + i] = g.W();
        a[11] = Raw(g);
        break;
    case kEndLoad:
        a[4] = g.Pk({0u, 1u, 1u, 2u});
        a[5] = 4u * g.R(0x70000u / 4);
        a[6] = Raw(g);
        a[7] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0x80u, 0xFFu, g.W() & 0xFFu});
        a[8] = g.Pk({0u, 1u, 2u, 3u, 5u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFEu, g.W(), g.W(), g.R(1000) + 1u, 0x100u});
        a[9] = g.R(14);
        break;
    case kSeekCtor:
        a[4] = g.P(25) ? 0u : RawNz(g);
        a[5] = Raw(g); a[6] = Raw(g);
        a[7] = FloatBits(g); a[8] = FloatBits(g); a[9] = FloatBits(g);
        a[10] = FlagsDword(g); a[11] = FlagsDword(g);
        break;
    case kSeekBody:
        a[4] = g.P(25) ? 0u : RawNz(g);
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
