#pragma once
// Fixtures del banco Fn192A (lote 192A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 33 funciones (32 del encargo + 0x61FC50).
//   idx  direccion  nombre real                                                         forma
//    0   0x67CBE0   CTaskComplexFallAndStayDown::CreateNextSubTask                      thiscall(ped) `ret 4`: GetTaskType de la subtarea (+8, ranura 4); 0xCF -> 0x678BD0(this, 0x516)
//    1   0x67CC10   CTaskComplexInAirAndLand destructor con borrado                     thiscall(flags) `ret 4`: cuerpo 0x678CC0; con el bit 0 CTask::operator delete 0x61A5B0; devuelve this
//    2   0x552830   CQuadTreeNode<T>::Constructor                                       thiscall(rect*, nivel) `ret 8`
//    3   0x552AF0   CQuadTreeNode<T>::PoolConstructor                                   thiscall(capacidad, nombre) `ret 8`: operator new 0x82119A x2; mapa a 0x80
//    4   0x552B50   CQuadTreeNode<T>::PoolNew                                           thiscall() `ret`: busca hueco con vuelta completa
//    5   0x552BC0   CQuadTreeNode<T>::PoolDelete                                        thiscall(nodo) `ret 4`: indice = (nodo - almacenamiento) / 40 con signo
//    6   0x552C80   CQuadTreeNode<T>::AllocateStorage                                   cdecl() `ret`: ecx = [0xB745BC]; jmp 0x552B50
//    7   0x552C90   CQuadTreeNode<T>::DeleteStorage                                     cdecl(nodo) `ret`: igual que 5 con el pool global
//    8   0x552EE0   CRect::GetRandomPoint (inferido)                                    thiscall(CVector2D*) `ret 4`: rand() % ancho + izquierda, rand() % alto + arriba
//    9   0x553010   CBox::GetRandomPoint (inferido)                                     thiscall(CVector*) `ret 4`: tres rand()
//   10   0x5531C0   CRenderer::Init                                                     cdecl() `ret`
//   11   0x5531D0   CRenderer::Shutdown                                                 cdecl() `ret`
//   12   0x59F610   CObjectPool::CreateAtRef                                            thiscall(ref) `ret 4`
//   13   0x59F660   CObject cuerpo del destructor                                       thiscall() `ret`: script brain, col, radar, modelo, txd, temporales, lista de control, puente del tren, fuego, ~CPhysical
//   14   0x5A2BD0   CObject destructor con borrado                                      thiscall(flags) `ret 4`: cuerpo 0x59F660; con el bit 0 liberacion en linea del pool 0xB7449C (0x19C)
//   15   0x5A2CB0   CPhysical::SetCentreOfMass (inferido)                               thiscall(x, y, z) `ret 0Ch`: tres dwords a +0xA4
//   16   0x5D28B0   CPedSaveStructure destructor                                        thiscall() `ret`: iterador de destructores sobre 13 CWeapon (efecto nulo)
//   17   0x5D28D0   CPlayerPedDataSaveStructure constructor                             thiscall() `ret`: CPedClothesDesc (+8) a cero
//   18   0x5D28F0   CVehicleSaveStructure destructor                                    thiscall() `jmp 0x59ACD0` (CMatrix::Destructor)
//   19   0x4B0A40   CEventScriptCommand::GetLifeTime (V)                                 thiscall() `ret`: eax = 0
//   20..29 0x4B64D0 .. 0x4B6840  destructores con borrado de CEventInAir, VehicleDied, VehicleOnFire, HitByWaterCannon, StuckInAir, CopCarBeingStolen, VehicleDamage,
//                                GotKnockedOverByCar, CarUpsideDown y PotentialWalkIntoFire: thiscall(flags) `ret 4`; cuerpo propio; con el bit 0 liberacion EN LINEA del pool 0xB744AC (0x44)
//   30   0x625B60   CTaskSimpleThrowControl destructor con borrado                      thiscall(flags) `ret 4`: cuerpo 0x61F950; CTask::operator delete 0x61A5B0
//   31   0x625B80   CTaskSimpleBeKickedOnGround destructor con borrado                  thiscall(flags) `ret 4`: cuerpo 0x61FC50; CTask::operator delete 0x61A5B0
//   32   0x61FC50   CTaskSimpleBeKickedOnGround cuerpo del destructor                    thiscall() `ret`: vtable 0x86D7D8; animacion (+0xC): blend delta -4.0f y SetFinishCallback; ~CTask 0x61A3A0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F192AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i64 = std::int64_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 33;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr unsigned kSeedAt = 60;       // a[60..63] = semillas de las arenas A, B, C y D

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
    std::vector<u8> s;          // bytes de estado de los pools (uno por hueco)
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

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNonZero(Gen& g) { const u32 v = Raw(g); return v ? v : 0x1004u; }
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
// Un float finito (sin NaN, infinitos ni denormales) para las coordenadas de los rectangulos: exponentes de 2^-37 a 2^33
inline u32 FiniteBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 exp = g.P(85) ? 100u + g.R(60) : 90u + g.R(90);
    return sign | (exp << 23) | (g.W() & 0x7FFFFFu);
}
// Un par (menor, mayor o al reves) de coordenadas cuya diferencia, redondeada a float, es al menos 1, menor que 2^62 y con los 32 bajos de su parte entera >= 0x100: el EXE y la DLL dividen por esa anchura
// (anchura cero = excepcion de division) y la comprobamos aqui en aritmetica de doble precision (determinista).
inline bool ValidWidth(float w) {
    const double aw = std::fabs(double(w));
    if (!std::isfinite(aw) || aw < 1.0 || aw >= 4.0e18) return false;
    const u32 low = u32(u64(i64(aw)));
    return low >= 0x100u || aw < 4294967296.0 && low != 0u;
}
inline void SpanPair(Gen& g, u32& lo, u32& hi) {
    for (int attempt = 0; attempt < 12; ++attempt) {
        const float x = std::bit_cast<float>(FiniteBits(g));
        const u32 mode = g.R(7);
        double d = mode == 0 ? 1.0 : mode == 1 ? 1.0 + double(g.R(1000)) / 16.0 : mode == 2 ? double(1u + g.R(0x7FFFu)) : mode == 3 ? std::ldexp(1.0 + double(g.R(1000)) / 1000.0, int(20 + g.R(20)))
                 : mode == 4 ? std::ldexp(1.0 + double(g.R(1000)) / 1000.0, int(1 + g.R(8))) : mode == 5 ? double(1u + g.R(40u)) + 0.5 : std::ldexp(1.0, int(31 + g.R(5))) + double(g.R(4096));
        if (g.P(45)) d = -d;
        const float y = static_cast<float>(double(x) + d);
        const float w = static_cast<float>(double(y) - double(x));
        if (ValidWidth(w) && ValidWidth(-w)) { lo = std::bit_cast<u32>(x); hi = std::bit_cast<u32>(y); return; }
    }
    lo = std::bit_cast<u32>(100.5f); hi = std::bit_cast<u32>(250.25f);
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }

// Clases de funcion (forma)
enum Kind : u32 { kFall, kSddTask, kSddThrow, kQtCtor, kPoolCtor, kPoolNew, kAlloc, kPoolDel, kRandRect, kRandBox, kNop, kNoWrite, kCreateAtRef, kObjDtor, kObjSdd, kCentre, kPedCtor, kMatDtor, kConst, kEvSdd, kKicked };
inline constexpr std::array<Kind, kFunctions> kKind{kFall, kSddTask, kQtCtor, kPoolCtor, kPoolNew, kPoolDel, kAlloc, kPoolDel, kRandRect, kRandBox, kNop, kNop, kCreateAtRef, kObjDtor, kObjSdd, kCentre, kNoWrite,
                                                    kPedCtor, kMatDtor, kConst, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kEvSdd, kSddThrow, kSddTask, kKicked};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x67CBE0, 0x67CC10, 0x552830, 0x552AF0, 0x552B50, 0x552BC0, 0x552C80, 0x552C90, 0x552EE0, 0x553010, 0x5531C0, 0x5531D0, 0x59F610, 0x59F660, 0x5A2BD0, 0x5A2CB0,
                                                    0x5D28B0, 0x5D28D0, 0x5D28F0, 0x4B0A40, 0x4B64D0, 0x4B6520, 0x4B6570, 0x4B65C0, 0x4B6660, 0x4B66B0, 0x4B6700, 0x4B67A0, 0x4B67F0, 0x4B6840, 0x625B60, 0x625B80, 0x61FC50};
// Cuerpos de destructor que falsean las funciones 20..29 (en el mismo orden)
inline constexpr std::array<u32, 10> kEventBody{0x4B0D00, 0x4B0DA0, 0x4B1170, 0x4B1320, 0x4B1520, 0x4B17F0, 0x4B1990, 0x4B1C10, 0x4B1D50, 0x4B1F00};
// Bytes que cada funcion toca a partir de su puntero `this` en la arena A (las funciones con el objeto en el pool C usan A para objetos auxiliares)
inline constexpr std::array<u32, kFunctions> kSpan{0x10, 0x10, 0x28, 0x14, 0x10, 0x10, 4, 0x10, 0x10, 0x18, 4, 4, 0x10, 0x40, 4, 0xB0, 0x180, 0x84, 0x48, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x10, 0x10, 0x10};
// Memoria global del juego tocada
inline constexpr u32 kMarker = 0xA9B0B0, kQtPoolPtr = 0xB745BC, kObjectPoolPtr = 0xB7449C, kEventPoolPtr = 0xB744AC, kScriptTable = 0xA90CF0, kModelTable = 0xA9B0C8, kTempObjects = 0xBB4A70, kTrainCrossing = 0x8CD6B8;
inline constexpr u32 kModelCount = 20000, kScriptCount = 70, kScriptStride = 20;
inline constexpr u32 kObjSize = 0x19C, kEventSize = 0x44, kNodeSize = 40;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A (cuando el objeto principal esta en A), a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en A[a[0]], a[60..63] = semillas de las arenas A, B, C y D
//   kFall: a[4] = tipo que devuelve GetTaskType de la subtarea, a[5] = desplazamiento de la subtarea en B (+8 de this la apunta), a[6] = desplazamiento de la vtable en C (ranura +0x10 = falso),
//          a[7] = EAX del falso de 0x678BD0, a[8] = argumento (ped)
//   kSddTask: a[4] = EAX del falso del cuerpo, a[5] = dword que deja el cuerpo en [this], a[6] = banderas
//   kSddThrow: a[4] = 0 sin entidad / 1 en B / 2 cruda, a[5] = desplazamiento en B, a[6] = banderas, a[7] = entidad cruda
//   kQtCtor: a[4] = desplazamiento del rectangulo en B, a[5..8] = sus cuatro dwords, a[9] = nivel
//   kPoolCtor: a[4] = capacidad (con signo), a[5] = nombre (crudo), a[6] = almacenamiento (C), a[7] = mapa (D), a[9] / a[10] = valores de los dos operator new cuando la capacidad no es positiva
//   kPoolNew: a[4] = almacenamiento (C), a[5] = estados (D), a[6] = capacidad, a[7] = cursor, s = estados (capacidad bytes)
//   kAlloc: a[4] = desplazamiento del pool en B, a[5] = EAX del falso de PoolNew
//   kPoolDel: a[4] = almacenamiento (C), a[5] = estados (D), a[6] = capacidad, a[7] = cursor, a[8] = indice del nodo, a[9] = bytes sobre el inicio del nodo, a[10] = 1 si el hueco esta libre
//   kRandRect: a[4..7] = cuatro dwords del CRect (+0, +4, +8, +0xC), a[8] y a[9] = valores de rand(), a[10] = salida en B
//   kRandBox: a[4..9] = seis dwords del CBox, a[10..12] = rand(), a[13] = salida en B
//   kNop, kNoWrite, kPedCtor, kConst: nada mas
//   kCreateAtRef: a[4] = almacenamiento (C), a[5] = estados (D), a[6] = capacidad, a[7] = cursor previo, a[8] = indice, a[9] = byte bajo de la referencia, s = estados (capacidad + 1 bytes, el ultimo es el centinela libre)
//   kObjDtor / kObjSdd: a[4] = pool en B, a[5] = capacidad, a[6] = indice, a[7] = almacenamiento (C), a[8] = estados (D), a[9] = cursor, a[10] = banderas del objeto (+0x140), a[11] = indice del script brain (+0x168),
//         a[12] = modelo (+0x22), a[13] = modelo de referencia (+0x14A), a[14] = txd (+0x16A), a[15] = tipo de objeto (+0x13C), a[16] = fuego (+0x164), a[17] = ficticio en A, a[18] = matriz en A, a[19] = matriz presente,
//         a[20..22] = posicion del ficticio, a[23..25] = posicion de la matriz, a[26] = contador de temporales, a[27] = MI_TRAINCROSSING1, a[28..31] = model infos y colmodels en B, a[32] / a[33] = ranura de col,
//         a[34] = indice de script en la tabla; para kObjSdd: a[11] = banderas del borrado, a[12] = EAX del falso del cuerpo, a[13] = dword del falso (kObjSdd usa a[4..9] y estas)
//   kCentre: a[4..6] = x, y, z
//   kMatDtor: a[4] = EAX del falso, a[5] = dword que deja en [this]
//   kEvSdd: a[4] = pool en B, a[5] = capacidad, a[6] = indice, a[7] = almacenamiento (C), a[8] = estados (D), a[9] = cursor, a[10] = bytes sobre el inicio del hueco, a[11] = banderas, a[12] = EAX del falso, a[13] = dword del falso, s = estados
//   kKicked: a[4] = 1 con animacion, a[5] = desplazamiento de la animacion en B, a[6] = EAX del falso, a[7] = dword que deja el falso de ~CTask
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[kSeedAt] = c.seed * 13u + 50u; a[kSeedAt + 1] = a[kSeedAt] + 1u; a[kSeedAt + 2] = a[kSeedAt] + 2u; a[kSeedAt + 3] = a[kSeedAt] + 3u;
    a[0] = Offset(g, kind, kSpan[c.fn]);
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x86D7D8u, 0x8704ACu});
    auto flagsOf = [&]() { return g.Pk({0u, 1u, 2u, 3u, 0xFEu, 0xFFu, 0x100u, 0x101u, 0x80000001u, g.W(), g.W() | 1u, g.W() & ~1u}); };
    auto states = [&](u32 count, u32 freePct) {
        c.s.assign(count, 0);
        for (u32 i = 0; i < count; ++i) c.s[i] = u8(g.R(128) | (g.P(freePct) ? 0x80u : 0u));
    };
    switch (kKind[c.fn]) {
    case kFall:
        a[4] = g.Pk({0xCFu, 0xCFu, 0xCFu, 0xCFu, 0xCFu, 0xCEu, 0xD0u, 0u, 0xFFFFFFFFu, 0x516u, 1u, 0x7FFFFFFFu, g.W(), 0xCF00u, 0x1CFu});
        a[5] = AuxOffset(g, 0x20); a[6] = AuxOffset(g, 0x80); a[7] = Raw(g); a[8] = Raw(g);
        break;
    case kSddTask:
        a[4] = Raw(g); a[5] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x86D7D8u, 0x8704ACu}); a[6] = flagsOf();
        break;
    case kSddThrow:
        a[4] = g.Pk({0u, 1u, 1u, 2u}); a[5] = AuxOffset(g, 0x10); a[6] = flagsOf(); a[7] = RawNonZero(g);
        break;
    case kQtCtor:
        a[4] = AuxOffset(g, 0x10);
        for (u32 i = 0; i < 4; ++i) a[5 + i] = FloatBits(g);
        a[9] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(8)});
        break;
    case kPoolCtor: {
        a[4] = g.Pk({0u, 1u, 2u, 3u, 7u, 16u, 30u, 100u, 300u, 0x7FFu, 0xFFFFFFFFu, 0xFFFFFFFBu, 0x80000000u, g.R(0x400), g.R(0x400), g.R(64)});
        const i32 size = i32(a[4]);
        a[5] = Raw(g);
        a[6] = AuxOffset(g, 0x40);
        a[7] = AuxOffset(g, size > 0 ? u32(size) : 0x10u);
        a[9] = Raw(g); a[10] = Raw(g);
        break;
    }
    case kPoolNew: {
        const u32 cap = g.Pk({1u, 2u, 3u, 4u, 7u, 16u, 30u, 64u, 100u, 255u, 300u});
        a[6] = cap;
        a[4] = AuxOffset(g, 0x40); a[5] = AuxOffset(g, cap + 1);
        a[7] = g.Pk({0xFFFFFFFFu, 0u, cap - 1u, cap >= 2 ? cap - 2u : 0u, g.R(cap), g.R(cap), g.R(cap)});
        states(cap + 1, g.Pk({0u, 0u, 3u, 10u, 30u, 60u, 90u, 100u}));
        break;
    }
    case kAlloc:
        a[4] = AuxOffset(g, 0x20); a[5] = Raw(g);
        break;
    case kPoolDel: {
        const u32 cap = g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 64u, 80u, 90u});
        a[6] = cap;
        a[4] = AuxOffset(g, kNodeSize * (cap + 1)); a[5] = AuxOffset(g, cap + 1);
        a[8] = g.R(cap);
        const u32 idx = a[8];
        a[7] = g.Pk({0xFFFFFFFFu, 0u, idx - 1u, idx, idx + 1u, cap - 1u, cap, cap + 5u, g.R(cap), g.R(cap), 0x7FFFFFFFu, 0x80000000u, 0x80000001u});
        a[9] = g.Pk({0u, 0u, 0u, 1u, 20u, 39u});
        a[10] = g.Pk({0u, 0u, 1u});
        break;
    }
    case kRandRect: {
        u32 lo, hi;
        SpanPair(g, lo, hi); a[4] = lo; a[6] = hi;
        SpanPair(g, lo, hi); a[5] = lo; a[7] = hi;
        a[8] = g.Pk({0u, 1u, 0x7FFFu, g.R(0x8000), g.R(0x8000), g.R(0x8000), g.R(100)});
        a[9] = g.Pk({0u, 1u, 0x7FFFu, g.R(0x8000), g.R(0x8000), g.R(0x8000), g.R(100)});
        a[10] = AuxOffset(g, 8);
        break;
    }
    case kRandBox: {
        u32 lo, hi;
        SpanPair(g, lo, hi); a[4] = lo; a[7] = hi;
        SpanPair(g, lo, hi); a[5] = lo; a[8] = hi;
        SpanPair(g, lo, hi); a[6] = lo; a[9] = hi;
        for (u32 i = 0; i < 3; ++i) a[10 + i] = g.Pk({0u, 1u, 0x7FFFu, g.R(0x8000), g.R(0x8000), g.R(0x8000), g.R(100)});
        a[13] = AuxOffset(g, 0xC);
        break;
    }
    case kNop:
    case kNoWrite:
    case kPedCtor:
    case kConst:
        break;
    case kCreateAtRef: {
        const u32 cap = g.Pk({1u, 2u, 3u, 5u, 16u, 30u, 100u, 200u, 300u});
        a[6] = cap;
        a[4] = AuxOffset(g, 0x40); a[5] = AuxOffset(g, cap + 2);
        a[7] = Raw(g);
        a[8] = g.R(cap);
        a[9] = g.Pk({0u, 1u, 0x7Fu, 0x80u, 0xFFu, g.R(256)});
        states(cap + 1, g.Pk({0u, 10u, 30u, 60u, 100u, 100u}));
        c.s[a[8]] |= 0x80;      // el hueco de la referencia esta libre (la DLL lo exige con un assert)
        c.s[cap] |= 0x80;       // centinela libre tras el ultimo estado: el EXE no comprueba el limite
        break;
    }
    case kObjDtor:
    case kObjSdd: {
        const bool sdd = kKind[c.fn] == kObjSdd;
        const u32 cap = g.Pk({1u, 2u, 3u, 4u, 5u, 7u, 9u});
        a[5] = cap; a[6] = g.R(cap);
        a[4] = 4 * g.R(0x3C);
        a[7] = Offset(g, kind, kObjSize * cap);
        a[8] = AuxOffset(g, cap);
        a[9] = g.Pk({0xFFFFFFFFu, 0u, a[6], g.W(), 0x80000000u});
        states(cap, 50);
        if (sdd) {
            a[11] = flagsOf(); a[12] = Raw(g); a[13] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x866F60u});
            if (a[11] & 1u) c.s[a[6]] &= 0x7Fu;       // con el borrado la DLL exige el hueco ocupado
            break;
        }
        u32 flags = g.W();
        flags = (flags & ~0x300000u) | (g.P(55) ? (1u + g.R(3)) << 20 : 0u);
        flags = (flags & ~0x10000u) | (g.P(50) ? 0x10000u : 0u);
        a[10] = flags;
        const bool script = (flags & 0x300000u) != 0, noModel = (flags & 0x10000u) != 0;
        a[11] = script ? g.R(kScriptCount) : g.Pk({0u, 0xFFFFu, g.W() & 0xFFFFu, g.R(70)});
        a[12] = noModel ? g.R(kModelCount) : g.Pk({g.R(kModelCount), g.W() & 0xFFFFu, g.W() & 0xFFFFu, 0xFFFFu, 0x8000u});
        a[13] = g.Pk({0xFFFFu, 0xFFFFu, g.R(kModelCount), g.R(kModelCount), 0u});
        a[14] = g.Pk({0xFFFFu, 0xFFFFu, 0u, 1u, g.W() & 0x7FFFu, 0x8000u, g.W() & 0xFFFFu});
        if (a[14] == 0xFFFFu && g.P(0)) a[14] = 0;
        a[15] = g.Pk({3u, 3u, 3u, 0u, 1u, 2u, 4u, 0xFFu, g.R(256)});
        a[16] = g.P(50) ? 0u : RawNonZero(g);
        a[17] = 4 * g.R(0x100);
        a[18] = 0x400 + 4 * g.R(0x2C0);
        a[19] = g.R(2);
        for (u32 i = 0; i < 3; ++i) { a[20 + i] = FloatBits(g); a[23 + i] = FloatBits(g); }
        a[26] = g.Pk({0u, 1u, 2u, 0xFFFFu, g.R(65536), g.R(5)}) | (g.P(50) ? 0u : (g.W() & 0xFFFF0000u));
        a[27] = (g.P(45) ? (a[12] & 0xFFFFu) : (g.W() & 0xFFFFu)) | (g.P(50) ? 0u : (g.W() & 0xFFFF0000u));
        a[28] = 0x100 + 4 * g.R(0x30); a[29] = 0x200 + 4 * g.R(0x30); a[30] = 0x300 + 4 * g.R(0x30); a[31] = 0x400 + 4 * g.R(0x30);
        a[32] = g.Pk({0u, 1u, 0xFFu, g.R(256)}); a[33] = g.R(256);
        a[34] = g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, g.R(65536), g.R(1000)});
        break;
    }
    case kCentre:
        for (u32 i = 0; i < 3; ++i) a[4 + i] = g.P(70) ? FloatBits(g) : Raw(g);
        break;
    case kMatDtor:
        a[4] = Raw(g); a[5] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x863C40u});
        break;
    case kEvSdd: {
        const u32 cap = g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 50u, 56u});
        a[5] = cap; a[6] = g.R(cap);
        a[4] = AuxOffset(g, 0x10);
        a[7] = Offset(g, kind, kEventSize * cap);
        a[8] = AuxOffset(g, cap);
        a[9] = g.Pk({0xFFFFFFFFu, 0u, a[6] - 1u, a[6], a[6] + 1u, cap - 1u, cap, g.R(cap), 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu});
        a[10] = g.Pk({0u, 0u, 0u, 1u, 0x20u, 0x43u, g.R(kEventSize)});
        a[11] = flagsOf(); a[12] = Raw(g); a[13] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x85AB38u});
        states(cap, 50);
        break;
    }
    case kKicked:
        a[4] = g.Pk({0u, 1u, 1u}); a[5] = AuxOffset(g, 0x20); a[6] = Raw(g); a[7] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x86D7D8u, 0x86D48Cu});
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
