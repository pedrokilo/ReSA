#pragma once
// Fixtures del banco PickUpControl68 (lote 68): CTaskComplexGoPickUpEntity::ControlSubTask (0x691D50), 16384 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CTaskComplexGoPickUpEntity::ControlSubTask  0x691D50  thiscall, ECX = this (tarea), (CPed*), `ret 4`, EAX = subtarea (m_pSubTask) o nulo si la aborta
//     (hook CTaskComplexGoPickUpEntity / ControlSubTask, tipo V: vtable 0x870B98, ranura 10)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Pu68Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 1;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// Sin NaN senalizantes (el x87 los silencia al copiar): solo silenciosos
inline constexpr std::array<u32, 6> kNan{0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0xFFC00001u, 0x7FC12345u, 0xFFC54321u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 8> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Resultado de bool de un colaborador: falso = AL 0 con basura en los bits altos, o AL != 0 con basura en los bits altos
inline u32 BoolRet(u32 n, u32 k, bool wantTrue) {
    static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
    if (!wantTrue) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
    return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
}
// Valor de un byte: canonico o notable (banderas con signo)
inline u32 ByteOf(u32 n, u32 k, std::initializer_list<u32> special) {
    const u32 r = Rand(n, k) % 8;
    if (r < 5) return special.begin()[Rand(n, k + 1) % special.size()] & 0xFFu;
    return Rand(n, k + 2) & 0xFFu;
}
// float adyacente (en bits ordenados) a v, k pasos (k puede ser negativo)
inline u32 Step(u32 v, int k) {
    if (((v >> 23) & 0xFFu) == 0xFFu) return v;                       // NaN/inf: sin cambio
    i32 key = (v >> 31) ? -i32(v & 0x7FFFFFFFu) : i32(v);              // clave ordenada: los negativos van al reves
    key += k;
    return key < 0 ? 0x80000000u | u32(-key) : u32(key);
}

// Constantes del EXE (leidas del desensamblado, 0x691D50): pi 0x40490FDB, 2 pi 0x40C90FDB, pi/4 0x3F490FDB, 0.75, 1/128, plazos 5000 (0x8D2FF8) y 8000 (0x8D2FFC)
inline constexpr u32 kPi = 0x40490FDBu, kTwoPi = 0x40C90FDBu, kQuarterPi = 0x3F490FDBu;

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
constexpr u32 kTaskWindow = 0x34, kTaskArena = kGuard + kTaskWindow + kGuard;               // 0x40 + 0x34 + 0x40 = 0xB4    CTaskComplexGoPickUpEntity
constexpr u32 kSubWindow = 0x40, kSubArena = kGuard + kSubWindow + kGuard;                  // 0x40 + 0x40 + 0x40 = 0xC0    subtarea (CTaskComplexGoToPointAndStandStill / CTaskSimplePickUpEntity)
constexpr u32 kPedWindow = 0x490, kPedArena = kGuard + kPedWindow + kGuard;                 // 0x40 + 0x490 + 0x40 = 0x510  ped falso (+4 posicion, +0x14 matriz, +0x480 datos de jugador)
constexpr u32 kMatWindow = 0x48, kMatArena = kGuard + kMatWindow + kGuard;                  // 0x40 + 0x48 + 0x40 = 0xC8    matriz falsa (posicion en +0x30)
constexpr u32 kPadWindow = 0x40, kPadArena = kGuard + kPadWindow + kGuard;                  // 0x40 + 0x40 + 0x40 = 0xC0    pad falso (solo se compara su puntero; nunca se lee)
constexpr u32 kBlockWindow = 0x20, kBlockArena = kGuard + kBlockWindow + kGuard;            // 0x40 + 0x20 + 0x40 = 0xA0    CAnimBlock (IsLoaded en +0x10)
constexpr u32 kGroupCount = 256, kGroupStride = 0x14;
constexpr u32 kGroupsWindow = kGroupCount * kGroupStride, kGroupsArena = kGuard + kGroupsWindow + kGuard;   // 0x40 + 0x1400 + 0x40 = 0x1480  ms_aAnimAssocGroups falso
static_assert(kTaskArena == 0xB4 && kSubArena == 0xC0 && kPedArena == 0x510 && kMatArena == 0xC8 && kPadArena == 0xC0 && kBlockArena == 0xA0 && kGroupsArena == 0x1480, "arenas");
// Memoria global del juego que el banco toca (se guarda al empezar y se restaura tras cada lado)
constexpr u32 kGroupsPtr = 0xB4EA34;                                                       // CAnimManager::ms_aAnimAssocGroups (puntero)
constexpr u32 kBlocksBase = 0xB5D4A0;                                                      // CAnimManager::ms_aAnimBlocks
constexpr u32 kTimeAddr = 0xB7CB84;                                                        // CTimer::m_snTimeInMilliseconds
constexpr u32 kCamAddr = 0xB6F178;                                                         // orientacion de la camara
constexpr u32 kModelBase = 0x63E7;                                                         // IFPToModelId(0) = 25575
constexpr u32 kTypePickUp = 0x134, kTypeGoTo = 0x387;

// ================================================================================================ 0: ControlSubTask
struct C {
    u32 family{}, group{}, refd{}, viaGroup{}, blk{}, loaded{};
    u32 subKind{}, subType{}, hasAssoc{}, dropped{}, tx{}, ty{};
    u32 now{}, start{};
    u32 pdata{}, hasMat{}, px{}, py{}, mx{}, my{};
    u32 padPtr{}, ud{}, lr{}, jump{}, sprint{}, rad{}, cam{};
    u32 eax{}, edx{}, stack{}, seed{};
};
inline u32 Sext16(u32 v) { return u32(i32(i16(v & 0xFFFFu))); }
// Valor devuelto por GetPedWalkUp/LeftRight: AX con signo mas basura en los bits altos de EAX (el EXE usa movsx ax)
inline u32 StickRet(u32 n, u32 k, i32 v) { return (Rand(n, k) % 3 == 0 ? 0u : (Rand(n, k + 1) & 0xFFFF0000u)) | (u32(v) & 0xFFFFu); }
inline i32 StickVal(u32 n, u32 k) {
    const u32 r = Rand(n, k) % 10;
    if (r < 4) return i32(Rand(n, k + 1) % 257) - 128;
    if (r < 7) return Pick(std::array<i32, 14>{0, 1, -1, 96, -96, 95, -95, 97, 127, -128, 32767, -32768, 128, 12}, n, k + 2);
    if (r < 9) return i32(Rand(n, k + 3) % 65536) - 32768;
    return 0;
}
// Pares (arriba/abajo, izquierda/derecha) cuya longitud cae junto a 96 (= 0.75 * 128): la funcion compara sqrt(ud^2 + lr^2) / 128 con 0.75f
inline void NearLimit(u32 n, u32 k, i32& ud, i32& lr) {
    static constexpr i32 pairs[][2]{{96, 0}, {0, 96}, {-96, 0}, {0, -96}, {96, 1}, {1, 96}, {-1, -96}, {67, 68}, {68, 68}, {67, 69}, {68, 67}, {-68, 68}, {95, 14}, {94, 20}, {60, 75}, {75, 60},
                                    {96, -1}, {95, 1}, {0, 97}, {97, 0}, {0, 95}, {95, 0}};
    const auto& p = pairs[Rand(n, k) % std::size(pairs)];
    ud = p[0]; lr = p[1];
}
inline C MakeC(u32 n) {
    C c; const u32 f = n % 16; c.family = f;
    // ---- bloque de animaciones del grupo
    c.group = Rand(n, 1) % 6 == 0 ? 0u : (Rand(n, 2) % 5 == 0 ? Pick(std::array<u32, 8>{1u, 2u, 72u, 73u, 255u, 128u, 0u, 100u}, n, 3) : 1 + Rand(n, 4) % (kGroupCount - 1));
    c.refd = Rand(n, 5) % 100 < 60 ? 0u : ByteOf(n, 5, {0u, 1u, 2u, 0x80u, 0xFFu});
    c.viaGroup = Rand(n, 6) % 2;
    c.blk = Rand(n, 7) % 100 < 70 ? 0u : (Rand(n, 8) % 2 ? 1u : 2u);      // 0 bloque de la arena, 1 bloque del EXE con IsLoaded == 1, 2 bloque del EXE con IsLoaded == 0
    c.loaded = Rand(n, 9) % 100 < 55 ? 1u : Pick(std::array<u32, 6>{0u, 2u, 0x80u, 0xFFu, 3u, 0x101u}, n, 10) & 0xFFu;   // IsLoaded del bloque de la arena (solo importa el byte)
    // ---- subtarea
    c.subKind = Rand(n, 11) % 12 == 0 ? 0u : 1u;
    {
        const u32 r = Rand(n, 12) % 10;
        c.subType = r < 4 ? kTypeGoTo : r < 7 ? kTypePickUp
                  : Pick(std::array<u32, 14>{0x133u, 0x135u, 0x386u, 0x388u, 0u, 1u, 0xCBu, 0x134u | 0x10000u, 0x387u | 0x80000000u, 0xFFFFFFFFu, 0x389u, 0x100u, 0x187u, 0x34u}, n, 13);
    }
    c.hasAssoc = Rand(n, 14) % 3 == 0;
    c.dropped = ByteOf(n, 15, {0u, 0u, 0u, 1u, 2u, 0x80u, 0xFFu});
    c.tx = Rand(n, 16) % 6 == 0 ? Pick(std::array<u32, 6>{0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x3F800000u}, n, 17) : RandFloat(n, 18, -3, 12);
    c.ty = Rand(n, 19) % 6 == 0 ? Pick(std::array<u32, 6>{0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xBF800000u}, n, 20) : RandFloat(n, 21, -3, 12);
    // ---- tiempos: la funcion compara (ahora > inicio + plazo) sin signo; el plazo es 5000 (ir al punto) o 8000 (recoger)
    {
        const u32 deadline = c.subType == kTypePickUp ? 8000u : 5000u;
        const u32 r = Rand(n, 22) % 10;
        c.start = Rand(n, 23) % 4 == 0 ? Pick(std::array<u32, 8>{0u, 1u, 0xFFFFFFFFu, 0xFFFFF000u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFE0C0u, 0xFFFFEC78u}, n, 24) : Rand(n, 25) % 0x4000000u;
        const u32 limit = c.start + deadline;
        if (r < 5) c.now = limit + u32(Pick(std::array<i32, 7>{-1, 0, 1, -2, 2, 100, -100}, n, 26));
        else if (r < 7) c.now = c.start + Rand(n, 27) % 20000u;
        else if (r < 8) c.now = Pick(std::array<u32, 5>{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 1u}, n, 28);
        else c.now = Rand(n, 29);
    }
    // ---- ped, pad y camara
    c.pdata = Rand(n, 30) % 8 != 0;
    c.hasMat = Rand(n, 31) % 2;
    c.px = Rand(n, 32) % 6 == 0 ? Pick(std::array<u32, 6>{0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x3F800000u}, n, 33) : RandFloat(n, 34, -3, 12);
    c.py = Rand(n, 35) % 6 == 0 ? Pick(std::array<u32, 6>{0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xBF800000u}, n, 36) : RandFloat(n, 37, -3, 12);
    c.mx = Rand(n, 38) % 6 == 0 ? Pick(std::array<u32, 6>{0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x3F800000u}, n, 39) : RandFloat(n, 40, -3, 12);
    c.my = Rand(n, 41) % 6 == 0 ? Pick(std::array<u32, 6>{0u, 0x80000000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xBF800000u}, n, 42) : RandFloat(n, 43, -3, 12);
    c.padPtr = Rand(n, 44) % 8 != 0;
    c.ud = StickRet(n, 45, StickVal(n, 47));
    c.lr = StickRet(n, 48, StickVal(n, 50));
    c.jump = BoolRet(n, 51, Rand(n, 52) % 4 == 0);
    c.sprint = BoolRet(n, 53, Rand(n, 54) % 4 == 0);
    c.cam = Rand(n, 55) % 3 == 0 ? Pick(std::array<u32, 7>{0u, 0x80000000u, 0x3F800000u, kPi, 0xBF490FDBu, kTwoPi, 0x7FC00000u}, n, 56) : (Rand(n, 57) % 2 ? 0u : RandFloat(n, 58, -4, 2));
    c.rad = Rand(n, 59) % 2 ? RandFloat(n, 60, -4, 3) : Pick(std::array<u32, 12>{0u, 0x80000000u, kPi, 0xC0490FDBu, kTwoPi, 0xC0C90FDBu, kQuarterPi, 0xBF490FDBu, 0x7FC00000u, 0x7F800000u, 0xFF800000u, 0x3F800000u}, n, 61);

    // ---- familias: fuerzan una ruta distinta de la funcion (el resto de campos siguen aleatorios)
    auto fixTime = [&](bool exceeded, u32 deadline) {      // fija ahora respecto al plazo, con el desbordamiento sin signo y los bordes +-1
        const u32 limit = c.start + deadline;
        if (exceeded) c.now = limit + 1u + (Rand(n, 70) % 3 == 0 ? Rand(n, 71) % 1000u : (Rand(n, 72) % 2 ? 0u : Rand(n, 73) % 3u));
        else c.now = limit - (Rand(n, 74) % 3 == 0 ? Rand(n, 75) % 1000u : (Rand(n, 76) % 2 ? 0u : Rand(n, 77) % 3u));
        if (Rand(n, 78) % 6 == 0) { const u32 big = Pick(std::array<u32, 4>{0xFFFFFFFFu - 6000u, 0xFFFFFFFFu - 9000u, 0u, 0x7FFFFFF0u}, n, 79); c.start = big; const u32 lim = big + deadline; c.now = exceeded ? lim + 1u + Rand(n, 80) % 5u : lim - Rand(n, 81) % 5u; }
    };
    // posicion efectiva del ped y rumbo hacia el destino, como float (la funcion resta en float y hace fpatan en doble)
    auto heading = [&]() -> float {
        const float pxx = F(c.hasMat ? c.mx : c.px), pyy = F(c.hasMat ? c.my : c.py);
        const volatile float dx = F(c.tx) - pxx, dy = F(c.ty) - pyy;
        return float(std::atan2(double(-dx), double(dy)));
    };
    // fija la salida del falso de GetRadianAngleBetweenPoints (rad) y la camara (cam) para que angulo = rad - cam valga el pedido (float)
    auto setAngle = [&](float a) {
        if (Rand(n, 82) % 2) c.cam = Rand(n, 83) % 2 ? 0u : 0x80000000u;
        const volatile float cam = F(c.cam);
        const volatile float r = a + cam;
        c.rad = B(r);
    };
    auto goToFull = [&] {          // ruta completa de ir al punto: subtarea 0x387, plazo no vencido, datos de jugador y pad presentes
        c.subKind = 1; c.subType = kTypeGoTo; fixTime(false, 5000u); c.pdata = 1; c.padPtr = 1;
    };
    switch (f) {
    case 0: c.subKind = 0; break;                                                                                                    // sin subtarea
    case 1: c.subKind = 1; c.subType = Pick(std::array<u32, 12>{0x133u, 0x135u, 0x386u, 0x388u, 0u, 1u, 0xCBu, 0x134u | 0x10000u, 0x387u | 0x80000000u, 0xFFFFFFFFu, 0x389u, 0x187u}, n, 84); break;   // otro tipo
    case 2: c.subKind = 1; c.subType = kTypePickUp; c.hasAssoc = 1; if (Rand(n, 85) % 2) fixTime(true, 8000u); break;                 // recoger con animacion: sin comprobar plazo
    case 3: c.subKind = 1; c.subType = kTypePickUp; c.hasAssoc = 0; c.dropped = ByteOf(n, 86, {1u, 2u, 0x80u, 0xFFu, 0x10u}) | 1u; if (Rand(n, 87) % 2) fixTime(true, 8000u); break;   // recoger ya soltado
    case 4: c.subKind = 1; c.subType = kTypePickUp; c.hasAssoc = 0; c.dropped = 0; fixTime(false, 8000u); break;                      // recoger dentro de plazo
    case 5: c.subKind = 1; c.subType = kTypePickUp; c.hasAssoc = 0; c.dropped = 0; fixTime(true, 8000u); break;                       // recoger fuera de plazo: aborta
    case 6: c.subKind = 1; c.subType = kTypeGoTo; fixTime(true, 5000u); break;                                                         // ir al punto fuera de plazo: aborta
    case 7: c.subKind = 1; c.subType = kTypeGoTo; fixTime(false, 5000u); c.pdata = 0; break;                                          // sin datos de jugador
    case 8: c.subKind = 1; c.subType = kTypeGoTo; fixTime(false, 5000u); c.pdata = 1; c.padPtr = 0; break;                            // sin pad
    case 9: goToFull(); c.jump = BoolRet(n, 88, true); break;                                                                          // salto: aborta
    case 10: goToFull(); c.jump = BoolRet(n, 89, false); c.sprint = BoolRet(n, 90, true); break;                                      // correr: aborta
    case 11: {                                                                                                                          // palanca corta: devuelve la subtarea
        goToFull(); c.jump = BoolRet(n, 91, false); c.sprint = BoolRet(n, 92, false);
        i32 ud, lr; NearLimit(n, 93, ud, lr);
        if (Rand(n, 94) % 2) { ud = ud * (Rand(n, 95) % 2 ? 1 : -1); lr = lr * (Rand(n, 96) % 2 ? 1 : -1); }
        if (Rand(n, 97) % 3 == 0) { ud = StickVal(n, 98) % 100; lr = StickVal(n, 100) % 100; }
        c.ud = StickRet(n, 101, ud); c.lr = StickRet(n, 103, lr);
        break;
    }
    case 12: case 13: case 14: {                                                                                                        // palanca larga: decide el angulo
        goToFull(); c.jump = BoolRet(n, 105, false); c.sprint = BoolRet(n, 106, false);
        i32 ud, lr;
        if (Rand(n, 107) % 2) NearLimit(n, 108, ud, lr), ud = ud + (Rand(n, 109) % 2), lr = lr + (Rand(n, 110) % 2); else { ud = StickVal(n, 111); lr = StickVal(n, 113); }
        if (!(double(ud) * ud + double(lr) * lr > 9216.0)) { ud = 97 + Rand(n, 115) % 31; lr = Rand(n, 116) % 2 ? 0 : -1 * i32(Rand(n, 117) % 50); }
        c.ud = StickRet(n, 118, ud); c.lr = StickRet(n, 120, lr);
        const float T = heading();
        const float pi = F(kPi), twoPi = F(kTwoPi), q = F(kQuarterPi);
        const int ulp = int(Rand(n, 122) % 7) - 3;
        if (f == 12) {            // dentro del intervalo [T - pi, T + pi]: abort si angulo - T > pi/4 (borde +-3 ulp)
            const volatile float edge = T + q;
            setAngle(F(Step(B(edge), ulp)));
        } else if (f == 13) {     // por encima de T + pi: resta 2 pi (borde +-3 ulp)
            const volatile float edge = T + pi;
            setAngle(F(Step(B(edge), ulp + (Rand(n, 123) % 2 ? 0 : 5))));
        } else {                  // por debajo de T - pi: suma 2 pi (borde +-3 ulp)
            const volatile float edge = T - pi;
            setAngle(F(Step(B(edge), ulp - (Rand(n, 124) % 2 ? 0 : 5))));
        }
        (void)twoPi;
        break;
    }
    default: {                                                                                                                          // 15: mezcla con NaN, infinitos y subnormales en destino, posicion y angulo
        goToFull(); c.jump = BoolRet(n, 125, Rand(n, 126) % 8 == 0); c.sprint = BoolRet(n, 127, Rand(n, 128) % 8 == 0);
        const u32 r = Rand(n, 129) % 6;
        if (r == 0) c.tx = Pick(kNan, n, 130); else if (r == 1) c.ty = Pick(kInf, n, 131); else if (r == 2) c.rad = Pick(kNan, n, 132);
        else if (r == 3) c.cam = Pick(kNan, n, 133); else if (r == 4) { c.tx = Pick(kTiny, n, 134); c.ty = Pick(kTiny, n, 135); c.px = c.mx = 0; c.py = c.my = 0x80000000u; }
        else { c.rad = Pick(kInf, n, 136); }
        if (Rand(n, 137) % 2) { const i32 big = Pick(std::array<i32, 6>{32767, -32768, 128, -128, 200, -200}, n, 138); c.ud = StickRet(n, 139, big); c.lr = StickRet(n, 141, Rand(n, 143) % 2 ? big : 0); }
        break;
    }
    }
    // la subtarea 0x387 en la ruta completa suele pasar el plazo de 5000; las demas familias dejan el tiempo aleatorio ya fijado arriba
    c.eax = SeedReg(n, 150); c.edx = SeedReg(n, 156); c.stack = StackSeed(n); c.seed = Rand(n, 199);
    return c;
}
inline std::string JsonC(const C& c) {
    Js j; j.U("family", c.family).U("group", c.group).U("refd", c.refd).U("via_group", c.viaGroup).U("blk", c.blk).U("loaded", c.loaded)
     .U("sub_kind", c.subKind).U("sub_type", c.subType).U("has_assoc", c.hasAssoc).U("dropped", c.dropped).U("tx", c.tx).U("ty", c.ty).U("now", c.now).U("start", c.start)
     .U("pdata", c.pdata).U("has_mat", c.hasMat).U("px", c.px).U("py", c.py).U("mx", c.mx).U("my", c.my)
     .U("pad_ptr", c.padPtr).U("ud", c.ud).U("lr", c.lr).U("jump", c.jump).U("sprint", c.sprint).U("rad", c.rad).U("cam", c.cam)
     .U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack);
    return j.End();
}
}
