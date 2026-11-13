#pragma once
// Fixtures del banco Fn185A (lote 185A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en el lote 185A SIN validar: ver reversing/PendienteValidar/185A.md; las clases del listado salen del desensamblado, de las vtables y de los llamadores).
//   idx  direccion  nombre real                                                                         forma
//    0   0x4048E0   CPool<CVehicle,CHeli>::GetAtRef(ref)                                                thiscall(ref) `ret 4`: ranura = ref >> 8 (con signo); BYTE de estado == byte bajo de ref ? almacenamiento + ranura * 0xA18 : 0
//    1   0x44CB10   COnscreenTimerEntry::Process                                                        thiscall() `ret`: x87 24 bits (paso * 0.02f) * 1000.0f, _ftol2; suma o resta sobre ScriptSpace[varId]; pitido 0x21
//    2   0x44CC20   COnscreenTimer::ProcessForDisplay                                                   thiscall() `ret`: reloj "%02d:%02d" y cuatro contadores ("%d" / "%d / %d") sobre ScriptSpace
//    3   0x44CD30   COnscreenTimer::Process                                                             thiscall() `ret`: sin replay (byte 0xA43088 != 1) ni pausa (+0x151): salta a 0x44CB10
//    4   0x53E600   DoFade                                                                              cdecl() `ret`: fundido de pantalla (TheCamera.Fade/ProcessFade/..., CSprite2d::DrawRect)
//    5   0x53E8D0   CLinkList<CPed*>::Clear                                                              thiscall() `ret`: enlaces de 0xC bytes, pasa los usados a la lista libre
//    6   0x5A1D10   CObject::ConstructorDefault                                                          thiscall() `ret`: CPhysical (542260), vtable 866F60, [+170] = 0, Init (59F840), byte [+13C] = 0
//    7   0x5A1DF0   CObject::ConstructorFromDummy                                                        thiscall(dummy) `ret 4`: CPhysical, vtable, SetModelIndexNoCreate, matriz o RwObject, Init, bytes, bit 0x200, bLightObject
//    8   0x5A1EF0   CObject::operator new(size, ref)                                                     cdecl(2) `ret`: ranura = ref >> 8; CPool::NewAt (59F610); almacenamiento + ranura * 0x19C
//    9   0x5A1F20   CObject::operator delete(obj)                                                        cdecl(1) `ret`: ranura = (obj - almacenamiento) / 0x19C; estado |= 0x80; cursor = min(cursor, ranura)
//   10   0x5BF3D0   cHandlingDataMgr::Initialise                                                        thiscall() `ret`: LoadHandlingData (5BD830) y cinco floats inmediatos en +0..+0x10
//   11   0x681E10   CTaskComplexPartnerChat::GetTaskType (V)                                            thiscall() `ret`: mov eax,4B4h
//   12   0x681E20   CTaskComplexPartnerShove::GetTaskType (V)                                           thiscall() `ret`: mov eax,4B9h
//   13   0x684E30   CTaskComplexPartnerChat::Clone (V)                                                   thiscall() `ret`: new(78h) 61A5A0; constructor 684290 con diez dwords
//   14   0x684ED0   CTaskComplexPartnerShove::Clone (V)                                                  thiscall() `ret`: new(74h) 61A5A0; constructor 6846F0 con ocho dwords
//   15   0x684F60   CTaskComplexPartner destructor con borrado                                           thiscall(flags) `ret 4`: cuerpo 683A40; delete 61A5B0 si bit 0 del BYTE
//   16   0x684F80   CTaskComplexPartnerDeal destructor con borrado                                       cuerpo 684200
//   17   0x684FA0   CTaskComplexPartnerGreet destructor con borrado                                      cuerpo 684280 (vtable 87078C y salto a 683A40)
//   18   0x684FC0   CTaskComplexPartnerChat destructor con borrado                                       cuerpo 684320
//   19   0x684FE0   CTaskComplexPartnerShove destructor con borrado                                      cuerpo 684750
//   20   0x695890   CTaskComplexFollowPedFootsteps::Clone (V)                                           thiscall() `ret`: new(20h) 61A5A0; constructor 694E20 con el ped (+0xC)
//   21   0x695B20   CTaskComplexFollowLeaderInFormation::ConstructorFormation                            thiscall(5) `ret 14h`: base 61A3B0; campos; GetFormationOffset 611F20; RegisterReference 571B70
//   22   0x695BC0   CTaskComplexFollowLeaderInFormation destructor con borrado                          cuerpo 694A40
//   23   0x695BE0   CTaskComplexFollowLeaderAnyMeans destructor con borrado                              cuerpo 694B30
//   24   0x695C00   CTaskComplexSeekEntityAiming destructor con borrado                                  cuerpo 694C10
//   25   0x695DC0   CTaskComplexFollowPedFootsteps::DestructorBody                                       thiscall() `ret`: vtable 870CC0; CleanUpOldReference 571A00; CPointRoute::operator delete 41B7D0; 61A3D0
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

namespace F185AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 26;
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
enum Kind : u32 { kPoolRef, kTimerEntry, kTimerDisplay, kTimerProc, kFade, kLlClear, kObjDef, kObjDummy, kObjNew, kObjDel, kHandling, kConst, kCloneChat, kCloneShove, kSdd, kCloneFoot, kFormation, kFootBody };
inline constexpr std::array<Kind, kFunctions> kKind{kPoolRef, kTimerEntry, kTimerDisplay, kTimerProc, kFade, kLlClear, kObjDef, kObjDummy, kObjNew, kObjDel, kHandling, kConst, kConst, kCloneChat, kCloneShove,
                                                    kSdd, kSdd, kSdd, kSdd, kSdd, kCloneFoot, kFormation, kSdd, kSdd, kSdd, kFootBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4048E0, 0x44CB10, 0x44CC20, 0x44CD30, 0x53E600, 0x53E8D0, 0x5A1D10, 0x5A1DF0, 0x5A1EF0, 0x5A1F20, 0x5BF3D0, 0x681E10, 0x681E20, 0x684E30, 0x684ED0,
                                                    0x684F60, 0x684F80, 0x684FA0, 0x684FC0, 0x684FE0, 0x695890, 0x695B20, 0x695BC0, 0x695BE0, 0x695C00, 0x695DC0};
// Cuerpo (colaborador) que llaman los destructores con borrado (0 = ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x683A40, 0x684200, 0x683A40, 0x684320, 0x684750, 0, 0, 0x694A40, 0x694B30, 0x694C10, 0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x10, 0x40, 0x158, 0x158, 4, 0x30, 0x180, 0x180, 4, 4, 0x14, 4, 4, 0x78, 0x74, 4, 4, 4, 4, 4, 0x20, 0x28, 4, 4, 4, 0x20};
// Tamano de los objetos auxiliares sinteticos (arenas B, C y D)
inline constexpr u32 kObjSpan = 0x40;

// Memoria global del juego tocada
inline constexpr u32 kScriptSpace = 0xA49960, kScriptWin = 0x1000, kScriptWinSize = 0x100;          // ScriptSpace + 0x1000: ventana de 0x100 bytes sembrada por el banco (los id de variable apuntan dentro)
inline constexpr u32 kTimeStep = 0xB7CB5C, kWideScreen = 0xB6F065, kReplayMode = 0xA43088, kWantsHud = 0xBAA45D;
inline constexpr u32 kPause1 = 0xB7CB49, kPause2 = 0xB7CB48, kTimeMs = 0xB7CB84, kJustLoaded = 0xC16EDC, kStillFade = 0xC16EDD, kFadeTimer = 0xC16EE0, kFadeOutTime = 0x8D2BD4, kFadeValue = 0xC3EFAB;
inline constexpr u32 kFadeRgb = 0xC3EFA8, kSplash = 0xB6F07D, kWidth = 0xC17044, kHeight = 0xC17048, kObjPool = 0xB7449C;
inline constexpr u32 kFreeAddr = 0x8214BD, kNewAddr = 0x82119A;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this] (salvo funciones que lo usan), a[60..63] = semillas de las arenas A, B, C y D
//   kPoolRef: a[4] = capacidad, a[5] = almacenamiento (0 en C, 1 crudo, 2 nulo), a[6] = desplazamiento en C, a[7] = crudo no nulo, a[8] = desplazamiento de los estados en B (64 bytes), a[9] = ranura, a[10] = modo (0 coincide, 1 no coincide,
//             2 coincide con el bit 7, 3 solo difiere en el bit 7), a[11] = byte de estado, a[12] = mascara no nula
//   kTimerEntry: a[4] = variable (0 ninguna, 1 en la ventana), a[5] = desplazamiento en la ventana (0..0xFC), a[6] = valor previo, a[7] = paso (bits float, 0xB7CB5C), a[8] = direccion (+0x39), a[9] = cuenta atras (+0x3C),
//             a[10] = pantalla ancha (0xB6F065), a[11] = byte previo en +4, a[12] = byte previo en +0x38, a[13] = semilla de la ventana
//   kTimerDisplay: a[4] = 0xBAA45D, a[5] = reloj (0 ninguno, 1 en la ventana), a[6] = desplazamiento del reloj, a[7] = semilla de la ventana, a[8 + 5 * i .. 12 + 5 * i] = contador i: variable (0/1), desplazamiento, tipo (word +0x12),
//             segunda variable (0 = id 0 / 1 = ventana), desplazamiento de la segunda; a[28 + i] = byte previo de habilitado (+0x3E)
//   kTimerProc: a[4] = byte de 0xA43088, a[5] = byte +0x151
//   kFade: a[4], a[5] = pausas (0xB7CB49, 0xB7CB48), a[6] = JustLoaded, a[7] = hora, a[8] = FadeTimer, a[9] = StillToFadeOut, a[10] = FadeOutTime, a[11] = FadeValue, a[12] = pantalla de carga, a[13..15] = R, G, B, a[16], a[17] = ancho, alto
//   kLlClear: a[4] = enlaces usados (0..5), a[5] = enlaces libres (0..3), a[6] = base de los nodos en B, a[8..15] = permutacion de los 8 huecos
//   kObjDef: nada mas
//   kObjDummy: a[4] = desplazamiento del dummy en B, a[5] = modelo (dword; el EXE usa el word con signo), a[6] = RwObject del dummy (0 nulo, 1 en C, 2 crudo), a[7] = desplazamiento en C, a[8] = crudo no nulo, a[9] = matriz del dummy (0 nula, 1 en D, 2 cruda),
//             a[10] = desplazamiento en D, a[11] = crudo, a[12] = byte +2E del dummy, a[13] = byte +2F, a[14] = banderas +1C del dummy, a[15] = banderas +1C de this, a[16] = RwObject de this (0 nulo, 1 con el byte de tipo a[17]),
//             a[17] = byte de tipo, a[18] = desplazamiento del objeto RW en C, a[19] = desplazamiento en D de la matriz que reserva el falso de AllocateMatrix, a[20] = desplazamiento en D del atomo que devuelve GetFirstAtomic,
//             a[21] = EAX de IsCBPCPipelineAttached, a[22] = byte +18 sin uso
//   kObjNew: a[4] = capacidad, a[5] = desplazamiento del pool en B, a[6] = almacenamiento (0 en C, 1 crudo), a[7] = desplazamiento en C, a[8] = crudo, a[9] = desplazamiento de los estados en D (hasta 0x41 bytes), a[10] = ranura,
//             a[11] = ultimo libre previo, a[12] = tamano (primer argumento), a[13] = byte bajo de la referencia, a[14] = semilla de los estados, a[15] = 1 si la ranura esta libre
//   kObjDel: a[4] = capacidad, a[5] = desplazamiento del pool en B, a[6] = almacenamiento (0 en C, 1 crudo), a[7] = desplazamiento en C, a[8] = crudo, a[9] = desplazamiento de los estados en D, a[10] = ranura, a[11] = ultimo libre previo,
//             a[12] = 1 si la ranura esta libre, a[13] = semilla de los estados
//   kHandling: nada mas
//   kConst: nada mas
//   kCloneChat/kCloneShove: a[4] = companero (+0x38), a[5] = lider (+0x58), a[6] = multiplicador (+0x3C), a[7] = cuenta (+0x5B), a[8] = conversacion (+0x74), a[9] = +0x75, a[10..12] = punto (+0x40),
//             a[13] = memoria nueva (0 nula, 1 en B), a[14] = desplazamiento en B, a[15] = EAX del falso del constructor, a[16] = dword que deja en [memoria]
//   kCloneFoot: a[4] = ped (+0xC), a[13..16] = memoria, desplazamiento, EAX, dword como en kClone
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kFormation: a[4] = grupo (crudo), a[5] = ped (0 nulo, 1 en B, 2 crudo), a[6] = desplazamiento en B, a[7] = crudo no nulo, a[8] = dx, a[9] = dy, a[10] = distancia (bits float), a[11] = desplazamiento en C de la entrada de la tabla,
//             a[12], a[13] = los dos floats de la entrada, a[14] = EAX del falso de RegisterReference
//   kFootBody: a[4] = ped (+0xC: 0 nulo, 1 en B, 2 crudo), a[5] = desplazamiento en B, a[6] = crudo no nulo, a[7] = ruta (+0x18: 0 nula, 1 en C, 2 cruda), a[8] = desplazamiento en C, a[9] = crudo no nulo, a[10] = dword que deja el cuerpo, a[11] = EAX del cuerpo
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
// Dword de la ventana de ScriptSpace en la posicion i (0..63): valores de tiempo en milisegundos que interesan al reloj (59 / 60 segundos, horas, signos) y aleatorios
inline u32 WindowWord(u32 seed, u32 i) {
    static constexpr std::array<u32, 16> t{0u, 1u, 0xFFFFFFFFu, 59u, 60u, 61u, 999u, 1000u, 59999u, 60000u, 3599999u, 3600000u, 5999999u, 6000001u, 0x7FFFFFFFu, 0x80000000u};
    const u32 k = Rand(seed, i) % 24u;
    if (k < 16u) return t[k];
    if (k < 20u) return Rand(seed, i + 100u) % 20000000u;
    if (k < 22u) return u32(-i32(Rand(seed, i + 150u) % 20000000u));
    return Rand(seed, i + 200u);
}
inline u32 ByteVal(Gen& g) { return g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256)}); }
inline u32 Slot(Gen& g, u32 cap) { return g.Pk({0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap)}); }
inline u32 PtrMode(Gen& g) { return g.Pk({0u, 1u, 1u, 2u}); }
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[60] = c.seed * 13u + 50u; c.a[61] = c.a[60] + 1u; c.a[62] = c.a[60] + 2u; c.a[63] = c.a[60] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0x86D48Cu, 0x86F334u, 0x870664u, 0x866F60u, 0xDEADBEEFu});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kPoolRef: {
        const u32 cap = g.Pk({1u, 2u, 3u, 8u, 16u, 32u, 64u, 64u});
        a[4] = cap;
        a[5] = g.Pk({0u, 0u, 0u, 1u, 2u});
        a[6] = AuxOffset(g, 0x40);
        a[7] = RawNz(g);
        a[8] = AuxOffset(g, 0x40);
        a[9] = Slot(g, cap);
        a[10] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 3u});
        a[11] = g.R(256);
        a[12] = 1u + g.R(255);
        break;
    }
    case kTimerEntry: {
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u, 1u});
        a[5] = g.R(0xFDu);
        a[6] = g.Pk({0u, 1u, 2u, 19u, 20u, 21u, 999u, 1000u, 1999u, 2000u, 3000u, 11999u, 12000u, 12001u, 13000u, 13999u, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x7FFFFFE0u,
                     g.W(), g.R(100000), g.R(20000), g.R(20000), 1000u * g.R(14) + g.R(1000), 1000u * g.R(14)});
        const u32 real = g.Pk({FBits(0.25f), FBits(0.5f), FBits(1.0f), FBits(1.0f), FBits(1.5f), FBits(2.0f), FBits(3.0f), FBits(10.0f), FBits(50.0f), FBits(100.5f), 0u, FBits(-1.0f), FBits(0.001f)});
        a[7] = g.P(65) ? real : FloatBits(g);
        a[8] = g.Pk({0u, 0u, 1u, 1u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        a[9] = g.Pk({0u, 1u, 12u, 12u, 12u, 13u, 5u, 0xFFFFFFFFu, 0x80000000u, g.R(30), g.W()});
        a[10] = g.Pk({0u, 0u, 0u, 1u, 2u, 0xFFu});
        a[11] = ByteVal(g); a[12] = ByteVal(g);
        a[13] = g.W();
        break;
    }
    case kTimerDisplay: {
        a[4] = g.Pk({1u, 1u, 1u, 1u, 1u, 0u, 2u, 0xFFu});
        a[5] = g.Pk({0u, 1u, 1u, 1u});
        a[6] = g.R(0xFDu);
        a[7] = g.W();
        for (u32 i = 0; i < 4; ++i) {
            a[8 + 5 * i] = g.Pk({0u, 1u, 1u, 1u});
            a[9 + 5 * i] = g.R(0xFDu);
            a[10 + 5 * i] = g.Pk({0u, 1u, 2u, 2u, 2u, 3u, 4u, 0xFFFFu, 0x8000u, 0x7FFFu, 0x10002u, g.R(65536)}) & 0xFFFFu;
            a[11 + 5 * i] = 1u;   // la segunda variable apunta a la ventana (con id 0 la DLL tiene un assert de GetPointerToScriptVariable que el EXE no tiene)
            a[12 + 5 * i] = g.R(0xFDu);
            a[28 + i] = ByteVal(g);
        }
        break;
    }
    case kTimerProc:
        a[4] = g.Pk({0u, 0u, 1u, 1u, 1u, 2u, 0xFFu, 3u});
        a[5] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0xFFu});
        break;
    case kFade: {
        const bool paused = g.P(18);
        a[4] = paused ? g.Pk({0u, 1u, 1u, 2u, 0xFFu}) : 0u;
        a[5] = paused ? (a[4] == 0 ? g.Pk({1u, 2u, 0xFFu}) : g.Pk({0u, 0u, 1u, 0xFFu})) : 0u;
        a[6] = g.Pk({0u, 0u, 0u, 1u});
        a[7] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.R(100000), g.W(), g.W()});
        const u32 fot = g.Pk({0u, 1u, 1000u, 2000u, 5000u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.R(10000)});
        a[10] = fot;
        const u32 diff = g.Pk({fot - 1u, fot, fot + 1u, fot, fot + 1u, 0u, 1u, g.W(), 0xFFFFFFFFu, 0x80000000u, g.R(10000)});
        a[8] = a[7] - diff;
        a[9] = g.Pk({0u, 1u, 1u, 1u});
        a[11] = g.Pk({0u, 0u, 1u, 2u, 100u, 127u, 128u, 254u, 255u, 255u, g.R(256)});
        a[12] = g.Pk({0u, 0u, 0u, 1u});
        a[13] = g.Pk({0u, 255u, g.R(256), g.R(256)}); a[14] = g.Pk({0u, 255u, g.R(256), g.R(256)}); a[15] = g.Pk({0u, 255u, g.R(256), g.R(256)});
        const std::array<u32, 25> dims{0u, 1u, 640u, 800u, 1024u, 1280u, 1366u, 1600u, 1920u, 2560u, 3840u, 0xFFFFFFFBu, 0xFFFFFFFFu, 0x7FFFFFFBu, 0x7FFFFFFCu, 0x7FFFFFFFu, 0x80000000u, 16777211u, 16777212u, 16777216u, 16777217u, 33554433u, 0x80000001u, 16777215u, 5u};
        a[16] = g.P(70) ? g.Pk({640u, 800u, 1024u, 1280u, 1920u, g.R(4096)}) : dims[g.R(25)];
        a[17] = g.P(70) ? g.Pk({480u, 600u, 768u, 720u, 1080u, g.R(4096)}) : dims[g.R(25)];
        break;
    }
    case kLlClear: {
        a[4] = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 5u});
        a[5] = g.Pk({0u, 1u, 1u, 2u, 3u});
        a[6] = AuxOffset(g, 8 * 0x20);
        std::array<u32, 8> perm{0, 1, 2, 3, 4, 5, 6, 7};
        for (u32 i = 7; i > 0; --i) std::swap(perm[i], perm[g.R(i + 1)]);
        for (u32 i = 0; i < 8; ++i) a[8 + i] = perm[i];
        break;
    }
    case kObjDef:
    case kHandling:
    case kConst:
        break;
    case kObjDummy:
        a[4] = AuxOffset(g, 0x40);
        a[5] = g.Pk({0u, 1u, 0xFFFFu, 0x7FFFu, 0x8000u, 0x12345678u, g.W(), g.R(20000), 0xFFFF8000u});
        a[6] = g.Pk({0u, 1u, 1u, 2u});
        a[7] = AuxOffset(g, 0x10);
        a[8] = RawNz(g);
        a[9] = g.Pk({0u, 0u, 1u, 1u, 2u});
        a[10] = AuxOffset(g, 0x40);
        a[11] = RawNz(g);
        a[12] = ByteVal(g); a[13] = ByteVal(g);
        a[14] = g.Pk({0u, 0x200u, 0xFFFFFFFFu, 0xFFFFFDFFu, g.W(), g.W() | 0x200u, g.W() & ~0x200u});
        a[15] = g.Pk({0u, 0x200u, 0xFFFFFFFFu, 0xFFFFFDFFu, 0x10000000u, g.W(), g.W() | 0x200u, g.W() & ~0x200u});
        a[16] = g.Pk({0u, 1u, 1u, 1u});
        a[17] = g.Pk({1u, 1u, 1u, 2u, 2u, 0u, 0xFFu, 3u});
        a[18] = AuxOffset(g, 0x10);
        a[19] = AuxOffset(g, 0x40);
        a[20] = AuxOffset(g, 0x40);
        a[21] = g.Pk({0u, 0u, 1u});   // IsCBPCPipelineAttached devuelve un bool (0/1 en EAX): el EXE prueba EAX completo y la DLL solo AL
        a[22] = ByteVal(g);
        break;
    case kObjNew: {
        const u32 cap = g.Pk({1u, 2u, 3u, 8u, 16u, 30u, 64u});
        a[4] = cap;
        a[5] = AuxOffset(g, 0x20);
        a[6] = g.Pk({0u, 0u, 1u});
        a[7] = AuxOffset(g, 0x10);
        a[8] = 0x400000u + 4u * g.R(0x200000);
        a[9] = AuxOffset(g, 0x41);
        a[10] = Slot(g, cap);
        a[11] = g.Pk({0xFFFFFFFFu, 0u, cap, 5u, g.W(), 0x7FFFFFFFu, 0x80000000u, g.R(cap)});
        a[12] = Raw(g);
        a[13] = g.R(256);
        a[14] = g.W();
        a[15] = 1u;   // la ranura de destino esta libre (la plantilla de la DLL tiene un assert si no lo esta; el EXE no lo comprueba)
        break;
    }
    case kObjDel: {
        const u32 cap = g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 64u});
        a[4] = cap;
        a[5] = AuxOffset(g, 0x20);
        a[6] = g.Pk({0u, 0u, 1u});
        a[7] = AuxOffset(g, 0x10);
        a[8] = 0x400000u + 4u * g.R(0x200000);
        a[9] = AuxOffset(g, 0x41);
        a[10] = Slot(g, cap);
        const u32 idx = a[10];
        a[11] = g.Pk({0xFFFFFFFFu, 0u, idx - 1u, idx, idx + 1u, cap - 1u, cap, cap + 5u, g.R(cap), g.R(cap), 0x7FFFFFFFu, 0x80000000u, 0x80000001u});
        a[12] = 0u;   // la ranura esta ocupada (la plantilla de la DLL tiene un assert si no lo esta; el EXE no lo comprueba)
        a[13] = g.W();
        break;
    }
    case kCloneChat:
    case kCloneShove:
        a[4] = Raw(g);
        a[5] = g.Pk({0u, 1u, 2u, 0xFFu, g.R(256)});
        a[6] = FloatBits(g);
        a[7] = g.Pk({0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0xFEu, g.R(256)});
        a[8] = g.Pk({0u, 1u, 2u, 0xFFu, g.R(256)});
        a[9] = g.Pk({0u, 1u});   // +0x75 es un char que la DLL pasa como bool (0/1); los objetos reales solo guardan 0 o 1
        a[10] = FloatBits(g); a[11] = FloatBits(g); a[12] = FloatBits(g);
        a[13] = g.Pk({0u, 1u, 1u, 1u});
        a[14] = AuxOffset(g, 0x80);
        a[15] = Raw(g); a[16] = Raw(g);
        break;
    case kCloneFoot:
        a[4] = Raw(g);
        a[13] = g.Pk({0u, 1u, 1u, 1u});
        a[14] = AuxOffset(g, 0x80);
        a[15] = Raw(g); a[16] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kFormation:
        a[4] = Raw(g);
        a[5] = g.Pk({0u, 1u, 2u, 2u});
        a[6] = AuxOffset(g, 0x10);
        a[7] = RawNz(g);
        a[8] = g.Pk({0xFFFFFFFFu, 0u, 1u, 2u, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(5) - 2u});
        a[9] = g.Pk({0xFFFFFFFFu, 0u, 1u, 2u, 0xFFFFFFFEu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(5) - 2u});
        a[10] = FloatBits(g);
        a[11] = AuxOffset(g, 0x10);
        a[12] = FloatBits(g); a[13] = FloatBits(g);
        a[14] = Raw(g);
        break;
    case kFootBody:
        a[4] = PtrMode(g);
        a[5] = AuxOffset(g, 0x10);
        a[6] = RawNz(g);
        a[7] = PtrMode(g);
        a[8] = AuxOffset(g, 0x10);
        a[9] = RawNz(g);
        a[10] = Raw(g); a[11] = Raw(g);
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
