#pragma once
// Fixtures del banco Fn182A (lote 182A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en el lote 182A SIN validar: ver reversing/PendienteValidar/182A.md; las clases del listado salen del desensamblado, de las vtables y de los llamadores).
//   idx  direccion  nombre real                                                                         forma
//    0   0x53BC60   CGame::ActivateMenuOnSetFocus                                                        cdecl() `ret`: si los bytes 0xBA6831 y 0xBA67A4 son 0: byte 0xBA677B = 1
//    1   0x53C860   CGame::ReleaseBlockA (codigo muerto)                                                 thiscall() `ret`: free([this+4]) si no nulo; +4, +8, +0Ch = 0
//    2   0x53C890   CGame::ReleaseBlockB (identica)                                                      idem
//    3   0x572130   CTheZones::GetCurrentZoneLockedOrUnlocked(x, y)                                      cdecl(2 floats) `ret`: x87 24 bits (c+3000)*[865060], _ftol2, byte bajo; ZonesVisited[10*cx-cy+9] != 0 (AL)
//    4   0x572DB0   CPad::IsEscJustPressed                                                               cdecl() `ret`: word [B733A8] != 0 y word [B73138] == 0 (AL)
//    5   0x572E70   CPad::IsMouseRButtonPressed                                                          cdecl() `ret`: byte [B73419] != 0 y byte [B7342D] == 0 (AL)
//    6   0x5D3020   CSaveGarage::CopyGarageIntoSaveGarage(garaje)                                         thiscall(ptr) `ret 4`: bytes +4C/4D/4E -> +0/1/2; dwords 0..3C -> +4..40; strcpy +44; byte +4F -> +4C
//    7   0x5D30C0   CSaveGarage::CopyGarageOutOfSaveGarage(garaje)                                       thiscall(ptr) `ret 4`: inversa; ademas [garaje+40h] = 0 antes del nombre
//    8   0x634440   CTaskSimpleTriggerLookAt::Constructor                                                thiscall(10 dwords) `ret 28h`: base 61A390; campos; vtable 86E3CC; entidad: RegisterReference 571B70
//    9   0x6344F0   CTaskSimpleTriggerLookAt::GetTaskType (V)                                              thiscall() `ret`: mov eax,10Dh
//   10   0x634560   CTaskSimpleTriggerLookAt::Clone (V)                                                   thiscall() `ret`: new(30h) 61A5A0; constructor 634440 (con hueso -1 y tiempo 100 si hueso >= 0 y sin entidad)
//   11   0x634610   CTaskSimpleTriggerLookAt::MakeAbortable (V)                                           thiscall(3) `ret 0Ch`: mov al,1
//   12   0x634620   CTaskSimpleTriggerLookAt::ProcessPed (V)                                              thiscall(ped) `ret 4`: LookAt 618970 si (+2C == 0) o (+8 != 0); devuelve 1
//   13   0x654840   CTaskComplexWalkRoundBuildingAttempt::ConstructorEntity                               thiscall(6 dwords) `ret 18h`: base 61A3B0; campos; Normalise 59C910; dos 41B7C0; RegisterReference 571B70
//   14   0x655C10   CTaskComplexWalkRoundObject::Clone (V)                                                thiscall() `ret`: new(54h) 61A5A0; constructor 655020 con (+0C, &+10, +1C)
//   15   0x657200   CTaskComplexWalkRoundObject destructor con borrado                                    thiscall(flags) `ret 4`: cuerpo 6550D0; delete 61A5B0 si bit 0
//   16   0x657750   CTaskComplexMoveBackAndJump destructor con borrado                                    thiscall(flags) `ret 4`: cuerpo 655370; delete 61A5B0 si bit 0
//   17   0x72FEC0   D3DTextureBuffer::Clear                                                               thiscall() `ret`: Release (COM ranura 2) de las texturas 0..size-1 (+14h con signo); +14h = 0; +10h = 0
//   18   0x72FEF0   D3DTextureBuffer::Destroy                                                             idem y free (8214BD) de la pila (+18h) si no es nula
//   19   0x7301D0   D3DIndexDataBuffer::Clear                                                             igual con numero +10h, contador +0Ch, pila +14h
//   20   0x730200   D3DIndexDataBuffer::Destroy                                                           idem
//   21   0x730450   D3DResourceSystem::GrowableArrayFree                                                  thiscall() `ret`: free([+8]) y [+8] = 0 si no nulo; [+4] = 0
//   22   0x7304B0   D3DResourceSystem::GrowableArrayResize(nueva)                                         thiscall(1) `ret 4`: new(nueva*4) 82119A; copia; free(datos) 8214BD; [+8], [+0]
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

namespace F182AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;    // ventana de las arenas B, C y D
constexpr std::size_t kWinA = 0x1000;   // ventana de la arena A (igual que las demas)
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

// Desplazamiento de un objeto de `span` bytes en la ventana de la arena A segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWinA);
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
inline u32 RawNz(Gen& g) { return Raw(g) | 1u; }   // distinto de cero y nunca una direccion de arena alineada del oraculo
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
    // magnitudes alrededor de la mitad del maximo (el doble de una diferencia desborda el float pero la suma posterior puede volver al rango)
    const u32 huge = sign | (0x7E800000u + (g.W() & 0x007FFFFFu));
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, huge, huge, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline u32 QuietSig(u32 b) { return (b | 0x00400000u) & 0x7FFFFFu; }   // significando de un NaN ya silenciado
// Un float finito normal positivo con exponente sesgado en [lo, hi)
inline u32 PosNormal(Gen& g, u32 lo, u32 hi) { return ((lo + g.R(hi - lo)) << 23) | (g.W() & 0x7FFFFFu); }

// Clases de funcion (forma)
enum Kind : u32 { kMenu, kRel, kZone, kEsc, kMouseR, kGarIn, kGarOut, kTlaCtor, kConst, kTlaClone, kRetTrue, kTlaProc, kBldCtor, kWroClone, kSdd, kBufTex, kBufIdx, kGrowFree, kGrowResize };
inline constexpr std::array<Kind, kFunctions> kKind{kMenu, kRel, kRel, kZone, kEsc, kMouseR, kGarIn, kGarOut, kTlaCtor, kConst, kTlaClone, kRetTrue, kTlaProc, kBldCtor, kWroClone, kSdd, kSdd,
                                                    kBufTex, kBufTex, kBufIdx, kBufIdx, kGrowFree, kGrowResize};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x53BC60, 0x53C860, 0x53C890, 0x572130, 0x572DB0, 0x572E70, 0x5D3020, 0x5D30C0, 0x634440, 0x6344F0, 0x634560, 0x634610, 0x634620, 0x654840, 0x655C10,
                                                    0x657200, 0x657750, 0x72FEC0, 0x72FEF0, 0x7301D0, 0x730200, 0x730450, 0x7304B0};
// Cuerpo (colaborador) que llaman los destructores con borrado (0 = ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x6550D0, 0x655370, 0, 0, 0, 0, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x10, 0x10, 4, 4, 4, 0x70, 0x70, 0x30, 4, 0x30, 4, 0x30, 0x4C, 0x20, 4, 4, 0x1C, 0x1C, 0x18, 0x18, 0xC, 0xC};
// Tamano de los objetos auxiliares sinteticos (arenas B, C y D)
inline constexpr u32 kObjSpan = 0x40;

// Memoria global del juego tocada
inline constexpr u32 kMenuMain = 0xBA6831, kMenuActive = 0xBA67A4, kMenuNext = 0xBA677B;                    // bytes de FrontEndMenuManager
inline constexpr u32 kEscNew = 0xB733A8, kEscOld = 0xB73138, kMouseNew = 0xB73419, kMouseOld = 0xB7342D;   // palabras y bytes de CPad
inline constexpr u32 kZonesVisited = 0xBA3730, kZoneRegion = 0xBA3600, kZoneRegionSize = 0xC00;           // ZonesVisited (100 bytes) y la region que lee la funcion (indice 10*cx-cy+9, cx y cy de un byte)
inline constexpr u32 kComRelease = 0xC0000002, kFreeAddr = 0x8214BD, kNewAddr = 0x82119A;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D
//   kMenu: a[4] = byte 0xBA6831, a[5] = byte 0xBA67A4, a[6] = byte 0xBA677B (valor previo)
//   kRel: a[4] = bloque (0 nulo, 1 en B, 2 crudo), a[5] = desplazamiento en B, a[6] = puntero crudo no nulo, a[7] y a[8] = dwords previos en +8 y +0Ch
//   kZone: a[4] = x (bits), a[5] = y (bits), a[6..30] = los 100 bytes de ZonesVisited
//   kEsc: a[4] = palabra de NewKeyState, a[5] = palabra de OldKeyState; kMouseR: a[4] = byte nuevo, a[5] = byte viejo
//   kGarIn/kGarOut: a[4] = desplazamiento del garaje (el otro objeto) en B, a[5] = longitud del nombre (0..31), a[6..13] = bytes del nombre (todos distintos de cero)
//   kTlaCtor: a[4] = entidad (0 nula, 1 en B, 2 cruda), a[5] = desplazamiento en B, a[6] = cruda no nula, a[7] = tiempo, a[8] = hueso, a[9..11] = desplazamiento (bits), a[12] = torso (0/1), a[13] = velocidad (bits), a[14] = mezcla,
//             a[15] = prioridad, a[16] = EAX del falso de 0x61A390, a[17] = dword que deja en [this]
//   kConst: nada mas
//   kTlaClone: a[4..6] = entidad como en kTlaCtor, a[7] = tiempo (+0Ch), a[8] = hueso (+10h), a[9..11] = desplazamiento, a[12] = torso (byte, cualquiera), a[13] = velocidad, a[14] = mezcla, a[15] = prioridad (byte),
//             a[16] = memoria nueva (0 nula, 1 en B), a[17] = desplazamiento en B, a[18] = EAX del falso del constructor, a[19] = dword que deja en [memoria]
//   kRetTrue: a[4..6] = ped, prioridad, evento (crudos)
//   kTlaProc: a[4..6] = entidad, a[7] = tiempo, a[8] = hueso, a[9..11] = desplazamiento, a[12] = torso (byte), a[13] = velocidad, a[14] = mezcla, a[15] = byte +2Ch, a[16] = prioridad (byte), a[17] = ped (crudo)
//   kBldCtor: a[4] = estado de movimiento, a[5] = entidad (0 nula, 1 en B, 2 cruda), a[6] = desplazamiento en B, a[7] = cruda no nula, a[8..10] = desplazamiento (vector en C a[11]), a[12..14] = posicion (vector en B a[15]),
//             a[16..18] = normal (vector en D a[19]), a[20] = bandera (0/1), a[21] = EAX del falso de 0x61A3B0, a[22] = dword que deja, a[23] = ruta 1 (0 nula, 1 en C), a[24] = desplazamiento en C (0x800 + ...),
//             a[25] = ruta 2 (0 nula, 1 en D), a[26] = desplazamiento en D (0x800 + ...)
//   kWroClone: a[4] = estado (+0Ch), a[5..7] = punto (+10h), a[8] = objeto (+1Ch), a[9] = memoria nueva (0 nula, 1 en B), a[10] = desplazamiento en B, a[11] = EAX del falso del constructor, a[12] = dword que deja
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kBufTex/kBufIdx: a[4] = contador (dword previo), a[5] = numero guardado (con signo), a[6] = pila (0 nula, 1 en B, 2 cruda), a[7] = desplazamiento de las dos pilas en B (la segunda a +0x40), a[8] = desplazamiento de los 16 objetos COM
//             en C (8 bytes cada uno), a[9] = desplazamiento de la tabla de vtable en D, a[10] = mutacion durante Release (0 ninguna, 1 reduce, 2 negativo, 3 otra pila, 4 otra pila y 8, 5 pila nula y 0, 6 pila cruda y 0),
//             a[11] = indice del Release tras el que se aplica, a[12] = numero nuevo, a[13] = puntero crudo no nulo
//   kGrowFree: a[4] = capacidad, a[5] = numero, a[6] = datos (0 nulo, 1 en B, 2 crudo), a[7] = desplazamiento en B, a[8] = crudo no nulo
//   kGrowResize: a[4] = capacidad, a[5] = numero (con signo), a[6] = datos (0 nulo, 1 en B, 2 crudo), a[7] = desplazamiento en B (0x100 bytes), a[8] = capacidad nueva, a[9] = desplazamiento en C de la reserva (0x100 bytes),
//             a[10] = reserva (0 nula, 1 en C), a[11] = crudo no nulo
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
// Coordenada para la funcion de zonas: el mapa (con bordes de celda a 1 ulp), valores medianos (la celda se trunca a un byte), enormes (fuera del rango de 64 bits de _ftol2), infinitos, NaN y denormales
inline u32 ZoneCoord(Gen& g) {
    const u32 pick = g.R(14);
    switch (pick) {
    case 0: case 1: case 2: case 3: return FBits(float(i32(g.R(7000001)) - 3500000) / 1000.0f);
    case 4: case 5: {
        const float c = float(600 * (i32(g.R(302)) - 1) - 3000);
        return u32(i32(FBits(c)) + i32(g.R(5)) - 2);
    }
    case 6: return FBits(float(i32(g.R(400001)) - 200000));
    case 7: return FBits(float(i32(g.R(2000001)) - 1000000) * 1000.0f);
    case 8: return (g.P(50) ? 0x80000000u : 0u) | (0x5C000000u + (g.W() & 0x03FFFFFFu));
    case 9: return (g.P(50) ? 0x80000000u : 0u) | (0x60000000u + (g.W() & 0x1F7FFFFFu));
    case 10: return g.Pk({0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00001u, 0x7F800001u, 0x00000001u, 0x80000001u, 0x007FFFFFu, 0x453B8000u, 0xC53B8000u});
    case 11: return FBits(float(i32(g.R(7000001)) - 3500000) / 1000.0f) + (g.P(30) ? 1u : 0u);
    default: return FloatBits(g);
    }
}
inline u32 NameDword(Gen& g) {
    u32 v = 0;
    for (u32 b = 0; b < 4; ++b) v |= (1u + g.R(255)) << (8 * b);
    return v;
}
// Entidad (puntero que solo se registra o se compara, nunca se desreferencia): 0 nula, 1 en B, 2 cruda no nula
inline void Entity(Gen& g, std::array<u32, 40>& a, u32 at) {
    a[at] = g.Pk({0u, 0u, 1u, 1u, 2u});
    a[at + 1] = AuxOffset(g, 0x10);
    a[at + 2] = RawNz(g);
}
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0x86D48Cu, 0x86E3CCu, 0x86F334u, 0x86F364u, 0x86F390u, 0xDEADBEEFu});
    auto& a = c.a;
    const auto byteVal = [&] { return g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256)}); };
    switch (kKind[c.fn]) {
    case kMenu:
        if (g.P(40)) { a[4] = 0; a[5] = 0; } else { a[4] = byteVal(); a[5] = byteVal(); }
        a[6] = byteVal();
        break;
    case kRel:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 2u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = RawNz(g);
        a[7] = Raw(g); a[8] = Raw(g);
        break;
    case kZone: {
        a[4] = ZoneCoord(g); a[5] = ZoneCoord(g);
        const u32 density = g.Pk({0u, 10u, 30u, 50u, 70u, 90u, 100u});
        for (u32 w = 0; w < 25; ++w) {
            u32 v = 0;
            for (u32 b = 0; b < 4; ++b) v |= (g.P(density) ? (g.P(70) ? 1u : (1u + g.R(255))) : 0u) << (8 * b);
            a[6 + w] = v;
        }
        break;
    }
    case kEsc:
        a[4] = g.Pk({0u, 0u, 1u, 2u, 0x100u, 0x101u, 0xFFFFu, 0x8000u, 0x80u, g.R(65536)});
        a[5] = g.Pk({0u, 0u, 1u, 2u, 0x100u, 0x101u, 0xFFFFu, 0x8000u, 0x80u, g.R(65536)});
        break;
    case kMouseR:
        a[4] = byteVal(); a[5] = byteVal();
        break;
    case kGarIn:
    case kGarOut:
        a[4] = AuxOffset(g, 0x70);
        a[5] = g.Pk({0u, 1u, 7u, 8u, 9u, 15u, 16u, 31u, g.R(32), g.R(32), g.R(32)});
        for (u32 w = 0; w < 8; ++w) a[6 + w] = NameDword(g);
        break;
    case kTlaCtor:
        Entity(g, a, 4);
        a[7] = Raw(g); a[8] = g.Pk({0xFFFFFFFFu, 0u, 1u, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, g.R(32), g.W(), Raw(g)});
        a[9] = FloatBits(g); a[10] = FloatBits(g); a[11] = FloatBits(g);
        a[12] = g.Pk({0u, 1u});
        a[13] = FloatBits(g); a[14] = Raw(g);
        a[15] = g.Pk({3u, 0u, 1u, 2u, 0xFFu, 0x80u, 0x7Fu, 0xFFFFFFFFu, g.W()});
        a[16] = Raw(g); a[17] = Raw(g);
        break;
    case kConst:
        a[3] = Raw(g);
        break;
    case kTlaClone:
        Entity(g, a, 4);
        a[7] = g.Pk({100u, 0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.W(), Raw(g)});
        a[8] = g.Pk({0xFFFFFFFFu, 0u, 1u, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 0x7Fu, g.R(32), g.W(), Raw(g)});
        a[9] = FloatBits(g); a[10] = FloatBits(g); a[11] = FloatBits(g);
        a[12] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        a[13] = FloatBits(g); a[14] = Raw(g);
        a[15] = g.Pk({3u, 0u, 1u, 2u, 0xFFu, 0x80u, 0x7Fu, g.R(256)});
        a[16] = g.Pk({0u, 1u, 1u, 1u});
        a[17] = AuxOffset(g, 0x30);
        a[18] = Raw(g); a[19] = Raw(g);
        break;
    case kRetTrue:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kTlaProc:
        Entity(g, a, 4);
        a[7] = Raw(g);
        a[8] = g.Pk({0xFFFFFFFFu, 0u, 1u, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, g.R(32), g.W(), Raw(g)});
        a[9] = FloatBits(g); a[10] = FloatBits(g); a[11] = FloatBits(g);
        a[12] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, g.R(256)});
        a[13] = FloatBits(g); a[14] = Raw(g);
        a[15] = g.Pk({0u, 0u, 1u, 1u, 2u, 0xFFu, g.R(256)});
        a[16] = g.Pk({3u, 0u, 1u, 2u, 0xFFu, 0x80u, 0x7Fu, g.R(256)});
        a[17] = Raw(g);
        break;
    case kBldCtor:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFu, 0x100u, 0x80u, g.W(), Raw(g)});
        a[5] = g.Pk({0u, 0u, 1u, 1u, 2u});
        a[6] = AuxOffset(g, 0x10);
        a[7] = RawNz(g);
        for (u32 i = 0; i < 3; ++i) { a[8 + i] = FloatBits(g); a[12 + i] = FloatBits(g); a[16 + i] = FloatBits(g); }
        a[11] = 4u * g.R(0x100);
        a[15] = 4u * g.R(0x100);
        a[19] = 4u * g.R(0x100);
        a[20] = g.Pk({0u, 1u});
        a[21] = Raw(g); a[22] = Raw(g);
        a[23] = g.Pk({0u, 1u, 1u, 1u}); a[24] = 0x800u + 4u * g.R(0x100);
        a[25] = g.Pk({0u, 1u, 1u, 1u}); a[26] = 0x800u + 4u * g.R(0x100);
        break;
    case kWroClone:
        a[4] = Raw(g);
        a[5] = FloatBits(g); a[6] = FloatBits(g); a[7] = FloatBits(g);
        a[8] = Raw(g);
        a[9] = g.Pk({0u, 1u, 1u, 1u});
        a[10] = AuxOffset(g, 0x60);
        a[11] = Raw(g); a[12] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kBufTex:
    case kBufIdx: {
        a[4] = Raw(g);
        const u32 size = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 8u, 3u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 0xC0000000u, g.R(9)});
        a[5] = size;
        const bool positive = i32(size) > 0;
        a[6] = positive ? 1u : g.Pk({0u, 1u, 2u});
        a[7] = AuxOffset(g, 0x80);
        a[8] = AuxOffset(g, 0x80);
        a[9] = AuxOffset(g, 0x10);
        a[10] = positive ? g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u}) : 0u;
        a[11] = positive ? g.R(size) : 0u;
        a[12] = g.Pk({0u, 1u, 2u, 3u, 5u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFF0u});
        if (a[10] == 1 && a[11] + 1 >= size) a[10] = 0;
        if (a[10] == 1) a[12] = g.R(size);
        a[13] = RawNz(g);
        break;
    }
    case kGrowFree:
        a[4] = Raw(g); a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u, 2u});
        a[7] = AuxOffset(g, 0x10);
        a[8] = RawNz(g);
        break;
    case kGrowResize: {
        const u32 cap = g.Pk({0u, 1u, 2u, 4u, 8u, 16u, 32u, 64u, 0xFFFFFFFFu, 0x80000000u, g.R(100), g.R(100), g.W()});
        a[4] = cap;
        const u32 newCap = g.P(15) ? cap : g.Pk({0u, 1u, 2u, 3u, 4u, 8u, 16u, 32u, 40u, 64u, 100u, 1000u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFF0u, g.R(70), g.R(70), g.R(70)});
        a[8] = newCap;
        u32 count = g.Pk({0u, 1u, 2u, 3u, 8u, 16u, 32u, 64u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFF0u, g.R(65), g.R(65), g.R(65), g.W()});
        const bool changes = i32(newCap) != i32(cap);
        if (changes && i32(count) > 0 && i32(newCap) > 0 && count > 64) count = g.R(65) + 1u;
        a[5] = count;
        const bool copies = changes && i32(count) > 0 && i32(newCap) > 0;
        a[6] = copies ? 1u : g.Pk({0u, 1u, 2u});
        a[7] = AuxOffset(g, 0x100);
        a[9] = AuxOffset(g, 0x100);
        a[10] = copies ? 1u : g.Pk({0u, 1u, 1u});
        a[11] = RawNz(g);
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
