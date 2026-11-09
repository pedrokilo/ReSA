#pragma once
// Fixtures del banco Fn140A (lote 140A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                           forma
//    0   0x4C5620   CBaseModelInfo::DestructorBody                        thiscall() `ret`: mov dword [ecx],85BB9Ch
//    1   0x4C5640   CTimeModelInfo::ConstructorBody                       thiscall() `ret`: push esi; call 4C4A60; mov [esi],85BCB0h; mov word [esi+22h],-1; eax=this
//    2   0x544990   CPhysical::ScalarDeletingDestructor                   thiscall(flags) `ret 4`: call 542450; test byte [esp+8],1; operator delete 8214BDh; eax=this  (vtable 0x863BA0, ranura 0)
//    3   0x54E8E0   CMatrix::DestructorThunk                              thiscall() `ret`: jmp 59ACD0
//    4   0x6CF180   CTrailer::ScalarDeletingDestructor                    thiscall(flags) `ret 4`: call 6CED10; ... CVehicle::operator delete 6E2D90  (vtable 0x871C28, ranura 0)
//    5   0x6D0450   cTransmission::ConstructorBody                        thiscall() `ret`: mov eax,ecx
//    6   0x537A50   CAnimatedBuilding::ScalarDeletingDestructor           thiscall(flags) `ret 4`: call 537A70; ... CBuilding::operator delete 4040A0 (vtable 0x8639B0, ranura 0)
//    7   0x537A70   CAnimatedBuilding::DestructorBody                     thiscall() `ret`: jmp 535E90
//    8   0x41BCB0   CBridge::ThisIsABridgeObjectMovingUp                  cdecl(uint32) `ret`: xor al,al
//    9   0x41BCE0   sin del float de la pila                              cdecl(float) `ret`: fld dword [esp+4]; fsin           (ST0)
//   10   0x41BCF0   cos del float de la pila                              cdecl(float) `ret`: fld dword [esp+4]; fcos           (ST0)
//   11   0x41BD00   CMatrix::GetRight (copia fuera de linea)              thiscall() `ret`: mov eax,ecx
//   12   0x41BD10   CMatrix::GetForward (copia fuera de linea)            thiscall() `ret`: lea eax,[ecx+10h]
//   13   0x41BD50   CTimer::GetTimeStep (copia fuera de linea)            cdecl() `ret`: fld dword [0xB7CB5C]                    (ST0)
//   14   0x41BD80   CPlaceable::GetMatrix (copia fuera de linea)          thiscall() `ret`: mov eax,[ecx+14h]
//   15   0x68B9F0   CTaskComplexArrestPed::GetTaskType                    thiscall() `ret`: mov eax,44Dh   (vtable 0x8709A8, ranura 4)
//   16   0x6908C0   CTaskComplexPolicePursuit::ScalarDeletingDestructor   thiscall(flags) `ret 4`: call 68D880; ... CTask::operator delete 61A5B0 (vtable 0x8709D4, ranura 0)
//   17   0x63C9C0   CTaskComplexCarDrive::GetTaskType                     thiscall() `ret`: mov eax,2D5h   (vtable 0x86E934, ranura 4)
//   18   0x643220   CTaskComplexCarDriveWander::ScalarDeletingDestructor  thiscall(flags) `ret 4`: call 63CB50; ... CTask::operator delete 61A5B0 (vtable 0x86E96C, ranura 0)
//   19   0x65FB50   CTaskGangHassleVehicle::GetTaskType                   thiscall() `ret`: mov eax,4B5h   (vtable 0x86F9D4, ranura 4)
//   20   0x6642A0   CTaskGangHasslePed::ScalarDeletingDestructor          thiscall(flags) `ret 4`: call 65FF60; ... CTask::operator delete 61A5B0 (vtable 0x86FA00, ranura 0)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, donde hace falta (13), memoria global del juego (se guarda y se restaura tras cada lado).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
// En las funciones de float (9, 10, 13) la familia elige ademas la categoria del argumento (ver FloatBits).
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

namespace F140AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 21;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Bytes que cada funcion toca a partir de su puntero `this` (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable (o nada)
constexpr u32 kSpanCtor = 0x24;       // CTimeModelInfo escribe hasta +0x24
constexpr u32 kSpanMatrixPtr = 0x18;  // GetMatrixCopy lee +0x14

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
    std::array<u32, 16> a{};
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

constexpr double kPi = 3.14159265358979323846264338327950288;
inline u32 BitsOfFloat(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
// Argumento float (bits) de sin / cos / CTimer::GetTimeStep segun la categoria (familia 0..15)
inline u32 FloatBits(Gen& g, u32 category) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    switch (category) {
    case 0:  return g.P(50) ? 0u : 0x80000000u;                                                    // +-0
    case 1:  return sign | (g.W() & 0x007FFFFFu);                                                  // denormales
    case 2:  return sign | ((1u + g.R(24)) << 23) | (g.W() & 0x007FFFFFu);                         // normales diminutos (exponente 1..24)
    case 3:  return BitsOfFloat(float(double(g.W()) / 4294967296.0 * 2.0 - 1.0));                  // [-1, 1)
    case 4:  return BitsOfFloat(float(double(g.W()) / 4294967296.0 * 20.0 - 10.0));                // [-10, 10)
    case 5: {                                                                                      // cerca de k * pi / 2 (+- 2 ulp)
        const double k = double(int(g.R(161)) - 80);
        const u32 b = BitsOfFloat(float(k * kPi / 2.0));
        return b + u32(int(g.R(5)) - 2);
    }
    case 6:  return BitsOfFloat(float(double(g.W()) / 4294967296.0 * 2000.0 - 1000.0));            // [-1000, 1000)
    case 7:  return BitsOfFloat(float(double(g.W()) / 4294967296.0 * 2.0e6 - 1.0e6));              // [-1e6, 1e6)
    case 8:  return sign | ((150u + g.R(60)) << 23) | (g.W() & 0x007FFFFFu);                       // enormes por debajo de 2^63 (exponente 150..209)
    case 9:  return sign | ((190u + g.R(65)) << 23) | (g.W() & 0x007FFFFFu);                       // 2^63 y mayores hasta el maximo (exponente 190..254)
    case 10: return sign | 0x7F800000u;                                                            // infinitos
    case 11: return sign | 0x7FC00000u | (g.W() & 0x003FFFFFu);                                    // NaN silenciosos con carga util
    case 12: return sign | 0x7F800000u | (1u + g.R(0x003FFFFFu));                                  // NaN senalizantes (bit 22 limpio)
    case 13: return g.W();                                                                         // bits cualesquiera
    case 14: return BitsOfFloat(float(double(int(g.R(1441)) - 720) * (kPi / 180.0)));              // grados enteros en radianes (-720..720)
    default: return sign | ((1u + g.R(254)) << 23);                                                // potencias de dos +-2^k
    }
}

// Indices (ver la cabecera)
inline constexpr unsigned kIdxBaseDtor = 0, kIdxTimeCtor = 1, kIdxPhysical = 2, kIdxMatrixThunk = 3, kIdxTrailer = 4, kIdxTransCtor = 5, kIdxAnimDelete = 6, kIdxAnimBody = 7, kIdxBridge = 8,
                          kIdxSin = 9, kIdxCos = 10, kIdxRight = 11, kIdxForward = 12, kIdxTimeStep = 13, kIdxGetMatrix = 14, kIdxArrestType = 15, kIdxPolice = 16, kIdxCarDriveType = 17,
                          kIdxWander = 18, kIdxHassleVehType = 19, kIdxHasslePed = 20;
constexpr bool IsScalarDeleting(u32 fn) { return fn == kIdxPhysical || fn == kIdxTrailer || fn == kIdxAnimDelete || fn == kIdxPolice || fn == kIdxWander || fn == kIdxHasslePed; }
constexpr bool IsFloatFn(u32 fn) { return fn == kIdxSin || fn == kIdxCos || fn == kIdxTimeStep; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; el resto por funcion
//   a[3] = dword previo en [this] / argumento crudo / bits del float; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [ECX] (gDtorWrite); a[6] = banderas (destructores con borrado)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (c.fn) {
    case kIdxTimeCtor:   // constructor: toca +0, +8, +0xA, +0x22 (relleno previo de toda la arena)
        c.a[0] = Offset(g, kind, kSpanCtor);
        c.a[3] = Raw(g);
        break;
    case kIdxBridge:     // a[3] = argumento (uint32) que el EXE no lee
        c.a[0] = 0;
        c.a[3] = Raw(g);
        break;
    case kIdxSin: case kIdxCos: case kIdxTimeStep:   // a[3] = bits del float
        c.a[0] = 0;
        c.a[3] = FloatBits(g, f);
        break;
    case kIdxGetMatrix:  // a[3] = dword en +0x14
        c.a[0] = Offset(g, kind, kSpanMatrixPtr);
        c.a[3] = Raw(g);
        break;
    case kIdxBaseDtor: case kIdxMatrixThunk: case kIdxAnimBody:   // cuerpos de destructor: a[3] = dword previo en [this]
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85BB9Cu, 0x85BCB0u, 0x8638C0u, 0x8585C8u, 0x85BB9Du, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    default:
        if (IsScalarDeleting(c.fn)) {   // destructores con borrado: a[3] = vtable previa en [this]; a[4] = EAX del falso del cuerpo; a[5] = dword que deja en [ECX]; a[6] = banderas
            c.a[0] = Offset(g, kind, kSpanThis);
            c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x863BA0u, 0x871C28u, 0x8639B0u, 0x8709D4u, 0x86E96Cu, 0x86FA00u, 0x863BA1u, g.W(), g.W(), 0xDEADBEEFu});
            c.a[4] = Raw(g);
            c.a[5] = Raw(g);
            c.a[6] = FlagsDword(g);
        } else {                         // constantes y accesores de `this` (5, 11, 12, 15, 17, 19): `this` en la arena A
            c.a[0] = Offset(g, kind, kSpanThis);
            c.a[3] = Raw(g);
        }
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
