#pragma once
// Fixtures del banco CarPolice75 (lote 75): CCarCtrl::CreatePoliceChase (EXE 0x42C2B0), 21504 casos. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::CreatePoliceChase  0x42C2B0  cdecl (CVehicle* plantilla, int32 tipo de ped, CNodeAddress nodo), el llamador limpia 12 bytes, resultado en AL   (hook CCarCtrl / CreatePoliceChase, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n % 16 (1024 casos por familia) para n < 16384 y 1024 casos por cada familia 16..20 despues.
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
#include <utility>
#include <vector>

namespace Cp75Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kBaseCases = 16384;    // familias 0..15
constexpr unsigned kExtraFamilies = 5;    // familias 16..20
constexpr unsigned kCases = kBaseCases + kExtraFamilies * 1024;
constexpr unsigned kFamilies = 16 + kExtraFamilies;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto). Los vehiculos son una ventana de 0x5A0 bytes (el EXE toca hasta +0x594); los peds, 0x4A0 (hasta +0x47C).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x5A0, kPedWin = 0x4A0, kIntelWin = 0x100, kMatWin = 0x48, kTaskWin = 0x40, kNodeWin = 0x1C * 20, kInfoWin = 0x40;
constexpr unsigned kModels = 1101;        // indices de modelo con region propia (tabla de carga 0x8E4CD0 y tabla de modelos 0xA9B0C8)
constexpr std::size_t kArena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 75001u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

// ------------------------------------------------------------------------------------------------ JSON minimo
struct Js {
    std::string s{"{"};
    bool first{true};
    void Key(const char* k) { if (!first) s += ','; first = false; s += '"'; s += k; s += "\":"; }
    Js& U(const char* k, u32 v) { Key(k); s += std::to_string(v); return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
// NaN: canonicos (quiet con signo +/-), NO canonicos (carga distinta, sNaN con y sin signo, carga maxima, carga minima) e infinitos
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};   // +-0, desnormales y el menor normal
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr std::array<u32, 14> kUnit{0x3F800000u, 0xBF800000u, 0x3F000000u, 0xBF000000u, 0x40000000u, 0xC0000000u, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800001u, 0xBF800001u, 0x3DCCCCCDu, 0x3F99999Au, 0x3FA66666u, 0x41200000u};
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
inline u32 RandFloat(u32 c, u32 k, int lo, int hi) {   // exponente insesgado en [lo, hi], signo y mantisa aleatorios
    const u32 r = Rand(c, k), s = Rand(c, k + 1000);
    const u32 span = u32(hi - lo + 1);
    const u32 e = u32(lo + int(s % span) + 127);
    return ((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFFu);
}
// Float con exponente insesgado e (-126..127) y mantisa/signo aleatorios; Denorm: desnormal con signo y mantisa aleatorios (nunca cero)
inline u32 ExpFloat(u32 c, u32 k, int e) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | (u32(e + 127) << 23) | (Rand(c, k + 500) & 0x7FFFFFu); }
inline u32 Denorm(u32 c, u32 k) { const u32 r = Rand(c, k); return ((r >> 31) << 31) | ((Rand(c, k + 500) & 0x7FFFFFu) | 1u); }
inline u32 Sgn(u32 bits, u32 neg) { return (bits & 0x7FFFFFFFu) | (neg << 31); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Un float: modo 0 moderado, 1 pequeno (productos que caen en el rango desnormal), 2 grande (desbordamiento), 3 especial (cero, desnormal, inf, NaN), 4 unitario, 5 mezcla de todo
inline u32 FloatOf(u32 c, u32 k, u32 mode) {
    const u32 r = Rand(c, k) % 32;
    switch (mode) {
    case 0: return r == 0 ? B(0.0f) : RandFloat(c, k + 2, -4, 11);
    case 1: return r < 3 ? Pick(kTiny, c, k + 1) : RandFloat(c, k + 2, -80, -20);
    case 2: return r < 3 ? Pick(kBig, c, k + 1) : RandFloat(c, k + 2, 20, 120);
    case 3:
        if (r < 8) return Pick(kTiny, c, k + 1);
        if (r < 13) return Pick(kInf, c, k + 1);
        if (r < 21) return Pick(kNan, c, k + 1);
        if (r < 24) return Pick(kBig, c, k + 1);
        return RandFloat(c, k + 2, -20, 100);
    case 4: return r < 4 ? RandFloat(c, k + 2, -2, 2) : Pick(kUnit, c, k + 1);
    default: {
        const u32 m = Rand(c, k + 3) % 5;
        return FloatOf(c, k + 5, m == 4 ? 3u : m);
    }
    }
}



// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    u32 rnd{};            // retorno de rand() del EXE (0x821B1E): solo cuentan los 16 bits bajos
    u32 gangs{};          // float en 0xC0BC38 (CPopCycle::m_NumGangs_Cars)
    u32 ped_type{};       // int32 (segundo argumento)
    u32 area{}, node{};   // CNodeAddress (dos words)
    u32 model{};          // int32: retorno de ChoosePoliceCarModel
    u32 loaded{};         // byte de la tabla 0x8E4CD0 + 20 * modelo (solo cuenta el byte bajo)
    u32 veh_null{};       // GetNewVehicleDependingOnCarModel devuelve nulo
    u32 node_pos[3]{};    // posicion que devuelve CPathNode::GetPosition
    u32 hit1{}, hit1z{}, hit2{}, hit2z{};   // retorno de ProcessVerticalLine (dword: solo cuenta AL) y z del golpe, para +1000 y -1000
    u32 height{};         // retorno (float) de GetHeightAboveRoad del coche nuevo
    u32 on_screen{}, visible{}, colliding{};   // GetIsOnScreen (AL), IsSphereVisible (AL), contador de FindObjectsKindaColliding (word)
    u32 radius{};         // float en +0x24 de la esfera de colision del modelo del coche nuevo
    u32 pmodel{};         // word +0x22 del coche nuevo (indice en la tabla de modelos)
    u32 ptype{};          // int32 +0x590 del coche nuevo (tipo de vehiculo)
    u32 cruise{};         // byte +0x3D0 de la plantilla
    u32 tmat{};           // la plantilla tiene matriz
    u32 fwd[3]{};         // fila +0x10 de la matriz de la plantilla
    u32 head{};           // rumbo (+0x10 de la posicion compacta de la plantilla)
    u32 pcount{}, pmask{};   // byte +0x488 de la plantilla y mascara de pasajeros no nulos (hueco i = bit i)
    u32 task_null{};      // CTask::operator new devuelve nulo
    u32 garbage{};        // basura de los bytes altos de los retornos booleanos
};

inline float Unit(u32 c, u32 k) { return float(int(Rand(c, k) % 2001u) - 1000) / 1000.0f; }   // [-1, 1]
inline u32 Nudge(u32 bits, int ulps) { return u32(i32(bits) + ulps); }
inline u32 Spec(u32 n, u32 k) { return FloatOf(n, k, 3u); }
// Un booleano en un dword: basura en los bytes altos con probabilidad 1/2 (los llamadores y los falsos leen AL)
inline u32 Bool(u32 n, u32 k, bool v) { return (Rand(n, k) % 2 ? (Rand(n, k + 1) & 0xFFFFFF00u) : 0u) | (v ? 1u : 0u); }
inline constexpr std::array<i32, 12> kModelList{596, 597, 598, 599, 523, 427, 490, 528, 601, 432, 433, 0};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n < kBaseCases ? n % 16 : 16 + (n - kBaseCases) / 1024;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.ecx = SeedReg(n, 3); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.rnd = Rand(n, 10) % 4 == 0 ? Rand(n, 11) : Rand(n, 11) % 65536u;
    c.gangs = Rand(n, 12) % 3 == 0 ? 0u : B(float(int(Rand(n, 13) % 800u) + 1) / 100.0f);
    c.ped_type = Rand(n, 14) % 3 == 0 ? u32(14 + int(Rand(n, 15) % 10u)) : u32(int(Rand(n, 15) % 45u) - 4);
    c.area = Rand(n, 16) % 64u; c.node = Rand(n, 17) % 20u;
    c.model = Rand(n, 18) % 10 < 7 ? u32(kModelList[Rand(n, 19) % kModelList.size()]) : (Rand(n, 20) % 9 == 0 ? u32(-1 - int(Rand(n, 21) % 1000u)) : Rand(n, 22) % kModels);
    c.loaded = Rand(n, 23) % 10 < 7 ? 1u : Pick(std::array<u32, 6>{0u, 2u, 3u, 0x80u, 0xFFu, 0x101u}, n, 24);
    c.veh_null = Rand(n, 25) % 16 == 0;
    for (int i = 0; i < 2; ++i) c.node_pos[i] = B(Unit(n, 30 + i) * 3000.0f);
    c.node_pos[2] = B(Unit(n, 32) * 100.0f);
    const float nz = F(c.node_pos[2]);
    c.hit1 = Bool(n, 33, Rand(n, 35) % 4 != 0); c.hit1z = B(nz + Unit(n, 36) * 30.0f);
    c.hit2 = Bool(n, 37, Rand(n, 39) % 4 != 0); c.hit2z = B(nz + Unit(n, 40) * 30.0f);
    c.height = B(0.5f + float(Rand(n, 41) % 300u) / 100.0f);
    c.on_screen = Bool(n, 42, Rand(n, 44) % 2 == 0); c.visible = Bool(n, 45, Rand(n, 47) % 3 == 0);
    c.colliding = Rand(n, 48) % 4 == 0 ? (Rand(n, 49) % 5 + 1) | (Rand(n, 50) % 2 ? 0u : (Rand(n, 51) & 0xFFFF0000u)) : (Rand(n, 52) % 2 ? 0u : (Rand(n, 53) & 0xFFFF0000u));
    c.radius = B(1.0f + float(Rand(n, 54) % 500u) / 100.0f);
    c.pmodel = Rand(n, 55) % kModels;
    c.ptype = Rand(n, 56) % 4 == 0 ? 9u : Rand(n, 57) % 12;
    c.cruise = Rand(n, 58) % 256;
    c.tmat = Rand(n, 59) % 4 != 0;
    for (int i = 0; i < 3; ++i) c.fwd[i] = B(Unit(n, 60 + i));
    c.head = B(Unit(n, 63) * 7.0f);
    c.pcount = Rand(n, 64) % 9; c.pmask = Rand(n, 65) % 256;
    c.task_null = Rand(n, 66) % 12 == 0;
    c.garbage = Rand(n, 67);
    switch (c.family) {
    case 0:   // el caso base (todo aleatorio)
        break;
    case 1: {  // sorteo de bandas: la frontera 8192 de los 16 bits bajos de rand()
        c.gangs = B(float(int(Rand(n, 100) % 800u) + 1) / 100.0f);
        c.ped_type = Rand(n, 101) % 2 ? u32(int(Rand(n, 102) % 14u)) : u32(24 + int(Rand(n, 102) % 20u));
        static constexpr u32 low[]{8190u, 8191u, 8192u, 8193u, 0u, 1u, 65535u, 32768u, 0x2000u, 0x1FFFu};
        c.rnd = (Rand(n, 103) % 2 ? Rand(n, 104) & 0xFFFF0000u : 0u) | low[Rand(n, 105) % std::size(low)];
        break;
    }
    case 2: {  // tipo de ped en las fronteras 14..23 y valores extremos, con el valor de 0xC0BC38 de todas clases
        static constexpr u32 pt[]{13u, 14u, 23u, 24u, 0u, u32(-1), 0x80000000u, 0x7FFFFFFFu, 15u, 22u, 12u, 25u, 0xFFFFu, 0x10000u, 14u, 23u};
        c.ped_type = pt[Rand(n, 100) % std::size(pt)];
        static constexpr u32 gv[]{0u, 0x80000000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 1u, 0x80000001u, 0x7F800000u, 0xFF800000u, 0x3F800000u, 0x007FFFFFu, 0xBF800000u};
        c.gangs = gv[Rand(n, 101) % std::size(gv)];
        c.rnd = Rand(n, 102) % 2 ? (Rand(n, 103) & 0xFFFF0000u) | (Rand(n, 104) % 2 ? 0x2000u : 0x1FFFu) : Rand(n, 105);
        break;
    }
    case 3: {  // modelo: negativos, cero y la frontera -1; el byte de carga con todos sus valores
        static constexpr u32 md[]{u32(-1), u32(-2), 0x80000000u, 0u, 1u, 596u, 1100u, u32(-1000), 0u, 595u, 597u, 1099u};
        c.model = md[Rand(n, 100) % std::size(md)];
        c.loaded = Rand(n, 101) % 2 ? (Rand(n, 102) % 3 == 0 ? 1u : Rand(n, 103) % 256) : (Rand(n, 104) & 0xFFFFFF00u) | (Rand(n, 105) % 2);
        break;
    }
    case 4:   // las salidas tempranas: reserva nula, modelo no cargado, tarea nula
        c.veh_null = Rand(n, 100) % 3 == 0; c.task_null = Rand(n, 101) % 2; c.loaded = Rand(n, 102) % 4 == 0 ? 1u : 0u;
        break;
    case 5:   // direcciones de nodo: todas las areas y nodos, y basura en los dos words
        c.area = (n / 16) % 64; c.node = Rand(n, 100) % 20;
        break;
    case 6: {  // golpes verticales: todas las combinaciones, distancias relativas a la z del punto (mas cerca, mas lejos, igual, simetrico)
        const float z = F(c.node_pos[2]);
        c.hit1 = Bool(n, 100, Rand(n, 101) % 2 == 0); c.hit2 = Bool(n, 102, Rand(n, 103) % 2 == 0);
        const float d1 = Unit(n, 104) * 20.0f;
        c.hit1z = B(z + d1);
        switch (Rand(n, 105) % 6) {
        case 0: c.hit2z = B(z - d1); break;                                  // simetrico: igual distancia
        case 1: c.hit2z = c.hit1z; break;                                    // el mismo golpe
        case 2: c.hit2z = B(z + d1 * 0.5f); break;                           // mas cerca
        case 3: c.hit2z = B(z + d1 * 2.0f); break;                           // mas lejos
        case 4: c.hit2z = Nudge(B(z - d1), int(Rand(n, 106) % 3u) - 1); break;   // casi simetrico
        default: c.hit2z = B(z + Unit(n, 107) * 20.0f); break;
        }
        break;
    }
    case 7: {  // golpes especiales y la frontera de 1e9: golpe exacto 1e9 (se acepta), un ulp mas (se descarta), NaN, infinitos, desnormales
        static constexpr u32 sp[]{0x4E6E6B28u, 0x4E6E6B29u, 0x4E6E6B27u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7F800001u, 0xFFC12345u, 0u, 0x80000000u, 1u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x4E6E6B28u};
        c.hit1 = Bool(n, 100, Rand(n, 101) % 4 != 0); c.hit2 = Bool(n, 102, Rand(n, 103) % 2 == 0);
        c.hit1z = sp[Rand(n, 104) % std::size(sp)]; c.hit2z = Rand(n, 105) % 2 ? sp[Rand(n, 106) % std::size(sp)] : c.hit1z;
        if (Rand(n, 107) % 3 == 0) c.node_pos[2] = Spec(n, 108);
        break;
    }
    case 8: {  // altura virtual especial con suelos finitos o especiales (suma float(altura + suelo))
        c.height = Spec(n, 100);
        if (Rand(n, 101) % 3 == 0) c.hit1z = Spec(n, 102);
        c.hit1 = Bool(n, 103, true);
        c.hit2 = Bool(n, 104, Rand(n, 105) % 2 == 0);
        if (Rand(n, 106) % 2) c.hit2z = c.hit1z;
        break;
    }
    case 9: {  // dos NaN en la suma: altura NaN (silencioso o de señal, cargas distintas, signos) frente a un suelo NaN en memoria
        static constexpr u32 nn[]{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu, 0x7FC00000u, 0x7F800002u};
        c.height = nn[Rand(n, 100) % std::size(nn)];
        c.hit1 = Bool(n, 101, true); c.hit1z = nn[Rand(n, 102) % std::size(nn)];
        c.hit2 = Bool(n, 103, Rand(n, 104) % 4 == 0); c.hit2z = nn[Rand(n, 105) % std::size(nn)];
        if (Rand(n, 106) % 3 == 0) c.hit1z = Pick(kInf, n, 107);
        if (Rand(n, 108) % 4 == 0) c.height = Pick(kInf, n, 109);
        break;
    }
    case 10:  // pantalla, visibilidad y colisiones: todas las combinaciones con basura en los bytes altos
        c.on_screen = Bool(n, 100, Rand(n, 101) % 2 == 0); c.visible = Bool(n, 102, Rand(n, 103) % 2 == 0);
        c.colliding = Rand(n, 104) % 2 ? (Rand(n, 105) & 0xFFFF0000u) | (Rand(n, 106) % 2 ? 0u : Rand(n, 107) % 3 + 1) : Rand(n, 108) % 2 ? 0u : 0x10000u;
        c.radius = Rand(n, 109) % 3 == 0 ? Spec(n, 110) : c.radius;
        break;
    case 11:  // velocidad de crucero: los 256 valores del byte (incluido 246, escala 0) con el adelante de una matriz normal o con inf
        c.cruise = (n / 16) % 256;
        c.tmat = 1;
        if (c.cruise == 246 || Rand(n, 100) % 6 == 0) for (int i = 0; i < 3; ++i) if (Rand(n, 101 + i) % 2) c.fwd[i] = Pick(kInf, n, 104 + i);
        break;
    case 12:  // plantilla sin matriz: el adelante sale de (-sen(rumbo), cos(rumbo), 0) con rumbos normales y grandes
        c.tmat = 0;
        c.head = Rand(n, 100) % 3 == 0 ? RandFloat(n, 101, -4, 6) : B(Unit(n, 102) * 12.0f);
        break;
    case 13:  // adelante especial con matriz (NaN no canonicos, infinitos, desnormales, ceros con signo)
        c.tmat = 1;
        for (int i = 0; i < 3; ++i) if (Rand(n, 100 + i) % 2 || i == 0) c.fwd[i] = Spec(n, 103 + i);
        break;
    case 14:  // pasajeros: todas las cuentas y mascaras
        c.pcount = (n / 16) % 9; c.pmask = Rand(n, 100) % 256;
        if (Rand(n, 101) % 5 == 0) c.pmask = 0xFFu;
        break;
    case 15:  // tipo de vehiculo del coche nuevo, modelo y radio especiales
        c.ptype = Rand(n, 100) % 3 == 0 ? Rand(n, 101) : Rand(n, 102) % 14;
        c.pmodel = Rand(n, 103) % 4 == 0 ? Pick(std::array<u32, 6>{0u, 1u, 1100u, 596u, 595u, 599u}, n, 104) : c.pmodel;
        c.radius = Rand(n, 105) % 2 ? Spec(n, 106) : c.radius;
        break;
    case 16:  // posicion del nodo especial (NaN, inf, desnormal): el punto viaja crudo a los falsos y la z del punto entra en la comparacion de golpes
        for (int i = 0; i < 3; ++i) if (Rand(n, 100 + i) % 2 || i == 2) c.node_pos[i] = Spec(n, 103 + i);
        c.hit1 = Bool(n, 106, Rand(n, 107) % 4 != 0); c.hit2 = Bool(n, 108, Rand(n, 109) % 2 == 0);
        break;
    case 17:  // exponentes extremos del adelante con crucero alto: los productos desbordan o caen al rango desnormal del float
        c.tmat = 1;
        c.cruise = Rand(n, 100) % 2 ? 245u + Rand(n, 101) % 11 : Rand(n, 102) % 256;
        for (int i = 0; i < 3; ++i) c.fwd[i] = Rand(n, 103 + i) % 3 == 0 ? Denorm(n, 106 + i) : ExpFloat(n, 109 + i, Rand(n, 112 + i) % 2 ? int(Rand(n, 115 + i) % 40u) - 126 : int(Rand(n, 118 + i) % 30u) + 100);
        break;
    case 18:  // rumbos enormes, infinitos y NaN sin matriz (fsin / fcos fuera de rango)
        c.tmat = 0;
        c.head = Rand(n, 100) % 2 ? Spec(n, 101) : ExpFloat(n, 102, 55 + int(Rand(n, 103) % 20u));
        break;
    case 19:  // mezcla de todo con valores especiales
        c.height = FloatOf(n, 100, 5u); c.hit1z = FloatOf(n, 101, 5u); c.hit2z = FloatOf(n, 102, 5u);
        for (int i = 0; i < 3; ++i) { c.fwd[i] = FloatOf(n, 103 + i, 5u); c.node_pos[i] = FloatOf(n, 106 + i, 5u); }
        c.head = FloatOf(n, 109, 5u); c.radius = FloatOf(n, 110, 5u); c.gangs = FloatOf(n, 111, 5u);
        break;
    default:  // 20: el byte de crucero cerca del desbordamiento (+10 pasa de 255) y registros de entrada con basura
        c.cruise = Rand(n, 100) % 2 ? 240u + Rand(n, 101) % 16 : Rand(n, 102) % 12;
        c.eax = Rand(n, 103); c.ecx = Rand(n, 104); c.edx = Rand(n, 105);
        break;
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack);
    j.U("rnd", c.rnd).U("gangs", c.gangs).U("ped_type", c.ped_type).U("area", c.area).U("node", c.node).U("model", c.model).U("loaded", c.loaded).U("veh_null", c.veh_null);
    j.U("node_x", c.node_pos[0]).U("node_y", c.node_pos[1]).U("node_z", c.node_pos[2]);
    j.U("hit1", c.hit1).U("hit1z", c.hit1z).U("hit2", c.hit2).U("hit2z", c.hit2z).U("height", c.height).U("on_screen", c.on_screen).U("visible", c.visible).U("colliding", c.colliding);
    j.U("radius", c.radius).U("pmodel", c.pmodel).U("ptype", c.ptype).U("cruise", c.cruise).U("tmat", c.tmat).U("fwd_x", c.fwd[0]).U("fwd_y", c.fwd[1]).U("fwd_z", c.fwd[2]).U("head", c.head);
    j.U("pcount", c.pcount).U("pmask", c.pmask).U("task_null", c.task_null).U("garbage", c.garbage);
    return j.End();
}
}
