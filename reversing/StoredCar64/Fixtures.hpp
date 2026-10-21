#pragma once
// Fixtures del banco StoredCar64 (lote 64): CStoredCar::StoreCar (0x449760) y CStoredCar::RestoreCar (0x447E40). Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp.
//
//   CStoredCar::StoreCar(CVehicle*)  0x449760  thiscall, ret 4, void. Ningun llamador del EXE (0x4499C8, 0x449B18, 0x44A4BB) lee EAX/ECX/EDX.
//   CStoredCar::RestoreCar()         0x447E40  thiscall, sin argumentos, devuelve el vehiculo en EAX (los llamadores 0x44856A y 0x4485DB lo leen: se compara; ECX/EDX solo se registran).
//
// Cada caso es una lista de ESCRITURAS PREVIAS en la arena del banco (region, desplazamiento en bytes, tamano 1/2/4, valor) encima de un relleno determinista (Garbage): las lee el banco
// (BuildCase) y el oraculo independiente fixture_oracle.py, que modela las dos funciones desde el desensamblado. Los punteros de la arena se escriben con marcas (kMark*).
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace SC64Fix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kStoreCases = 4096, kRestoreCases = 8192, kFns = 2;
constexpr unsigned kCasesOf[kFns]{kStoreCases, kRestoreCases};

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }

constexpr u32 kGarbageSeed = 0x6A09E667u;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}
// la semilla de relleno de un caso (distinta por funcion)
constexpr u32 GarbageSeed(u32 fn, u32 n) { return n + fn * 0x100000u; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kVehWords = 0xA18 / 4, kScWords = 16, kMatWords = 24, kMiWords = 16, kGuardWords = 4;
// regiones de la arena sobre las que escriben los casos
enum : u32 { RS = 0, RV0, RV1, RM0, RM1, RMI0, RMI1, RMI2, RMI3, kRegions };
// marcas de puntero (se sustituyen por la direccion real al construir el caso)
constexpr u32 kMarkMat0 = 0xFFFF0100u, kMarkMat1 = 0xFFFF0101u;
// modelos de prueba (sus entradas reales de CStreaming::ms_aInfoForModel y CModelInfo::ms_modelInfoPtrs se fijan por caso y se restauran)
constexpr u32 kTm[4]{5000, 5001, 5002, 5003};
constexpr u32 kStateBase = 0x8E4CD0, kModelPtrBase = 0xA9B0C8, kCompsAddr = 0x8A6458;

// colaboradores sustituidos (indice en las mascaras y en los eventos)
enum : u32 { kGetRemap = 1, kReqModel, kReqUpg, kHasUpg, kNew, kCtor, kSetup, kSetRemap, kKinds };

struct Pre { u32 region, off, size, val; };
struct Spec {
    u32 fn{}, family{}, regs[3]{};        // regs: semilla de pila, EAX y EDX de entrada
    std::vector<Pre> pre;
    u32 remap{};                          // StoreCar: lo que devuelve CVehicle::GetRemapIndex (EAX completo)
    u32 state[4]{}, vtype[4]{};           // RestoreCar: byte de estado de carga y tipo de vehiculo de cada modelo de prueba
    u32 hasMask{0xFFFFFFFFu}, ctor1{0};   // mascara de CStreaming::HasVehicleUpgradeLoaded por llamada; el constructor devuelve V1 en lugar de la memoria reservada
    u32 comps[2]{};                       // valores previos de 0x8A6458 y 0x8A6459
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};
constexpr u32 kQNaN = 0x7FC00000u, kSNaN = 0x7F800001u, kInf = 0x7F800000u, kNegInf = 0xFF800000u;

// patrones de float interesantes (posicion y avance)
inline u32 PickFloat(Gen& g) {
    if (g.C(70)) return Bits((float(int(g.R() % 20001)) - 10000.0f) * 0.01f);
    static const u32 odd[]{kQNaN, kSNaN, 0xFFC00000u, 0xFF800001u, kInf, kNegInf, 0x80000000u, 0u, 0x00000001u, 0x007FFFFFu, 0x80000001u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F800000u, 0xBF800000u};
    return odd[g.P(sizeof(odd) / sizeof(odd[0]))];
}
// avance del vehiculo (matriz +0x10/+0x14/+0x18): su producto por 100 se trunca
inline u32 PickForward(Gen& g) {
    const u32 pick = g.P(100);
    if (pick < 40) {   // k/100 y sus vecinos mas cercanos (el redondeo a 24 bits del producto decide el truncamiento)
        const float v = float(int(g.P(261)) - 130) / 100.0f;
        const u32 b = Bits(v);
        const u32 d = g.P(5);
        return d == 0 ? b : (d == 1 ? b + 1 : (d == 2 ? b - 1 : (d == 3 ? b + 2 : b - 2)));
    }
    if (pick < 55) { static const float v[]{1.0f, -1.0f, 0.5f, -0.5f, 0.0f, -0.0f, 1.27f, 1.28f, -1.28f, -1.29f, 1.2799999f, 2.55f, 2.56f, 0.99999994f, 0.999f, 0.285f, 0.29f, 0.57f, 0.58f}; return Bits(v[g.P(sizeof(v) / sizeof(v[0]))]); }
    if (pick < 85) return Bits((float(int(g.R() % 30001)) - 15000.0f) / 10000.0f);
    if (pick < 93) { static const float v[]{1.0e30f, -1.0e30f, 3.0e38f, -3.4e38f, 4.2e9f, 9.2e16f, 9.3e16f, -9.3e16f, 2147483648.0f, 21474836.0f, 4294967296.0f, 2.2e7f}; return Bits(v[g.P(sizeof(v) / sizeof(v[0]))]); }
    static const u32 odd[]{kQNaN, kSNaN, 0xFFC00000u, kInf, kNegInf, 0x80000000u, 0x00000001u, 0x007FFFFFu, 0x80000001u, 0x7F7FFFFFu};
    return odd[g.P(sizeof(odd) / sizeof(odd[0]))];
}

struct Builder {
    Spec& s;
    void P8(u32 region, u32 off, u32 v) { s.pre.push_back({region, off, 1, v & 0xFFu}); }
    void P16(u32 region, u32 off, u32 v) { s.pre.push_back({region, off, 2, v & 0xFFFFu}); }
    void P32(u32 region, u32 off, u32 v) { s.pre.push_back({region, off, 4, v}); }
};

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn;
    Gen g(GarbageSeed(fn, n) * 3u + 7u);
    Builder b{s};
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 3) : g.R();
    s.comps[0] = g.R() & 0xFFu; s.comps[1] = g.R() & 0xFFu;
    if (fn == 0) {
        // ---- StoreCar: vehiculo V0 con matriz M0 (el EXE desreferencia la matriz sin comprobar: siempre existe)
        s.family = n % 8;
        b.P32(RV0, 0x14, kMarkMat0);
        b.P16(RV0, 0x22, g.C(30) ? (g.R() & 0xFFFFu) : (400u + g.P(300)));
        b.P32(RM0, 4 * 4, PickForward(g)); b.P32(RM0, 4 * 5, PickForward(g)); b.P32(RM0, 4 * 6, PickForward(g));
        for (int k = 0; k < 3; ++k) b.P32(RM0, 4 * (12 + k), PickFloat(g));
        for (int k = 0; k < 3; ++k) if (g.C(50)) b.P32(RV0, 4 + 4 * k, PickFloat(g));   // posicion del placement: distinta de la de la matriz
        u32 phys = g.R();
        if (g.C(25)) phys = 1u << (g.C(50) ? 18 : (g.C(50) ? 19 : (g.C(50) ? 23 : (g.C(50) ? 20 : 21))));
        if (g.C(10)) phys = 0u;
        b.P32(RV0, 0x40, phys);
        b.P8(RV0, 0x42E, g.C(40) ? 0x10u : g.R());
        u32 hf = g.R();
        if (g.C(25)) hf = (g.C(50) ? 0x20000u : 0u) | (g.C(50) ? 0x80000u : 0u);
        b.P32(RV0, 0x38C, hf);
        static const u32 types[]{0, 9, 0, 9, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11, 0xFFFFFFFFu, 0x80000000u};
        b.P32(RV0, 0x590, types[g.P(16)]);
        b.P8(RV0, 0x4A8, g.R());
        for (int i = 0; i < 15; ++i) b.P16(RV0, 0x43A + 2 * i, g.C(35) ? 0xFFFFu : (g.R() & 0xFFFFu));
        s.remap = g.C(50) ? g.P(4) : g.R();
        return s;
    }
    // ---- RestoreCar
    s.family = n % 16;
    for (int k = 0; k < 4; ++k) {
        s.state[k] = g.C(75) ? 1u : (g.C(60) ? 0u : g.R() & 0xFFu);
        static const u32 vt[]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 1, 5, 9, 10, 4, 0xFFFFFFFFu, 100u};
        s.vtype[k] = vt[g.P(20)];
    }
    const u32 km = g.P(4);                                                    // modelo de prueba del vehiculo guardado
    if (s.family < 12) { s.vtype[km] = s.family; if (g.C(85)) s.state[km] = 1; }   // cada tipo 0..11 muchas veces
    else if (g.C(90)) s.state[km] = 1;
    s.hasMask = g.C(70) ? 0xFFFFFFFFu : (g.C(25) ? 0u : g.R());
    s.ctor1 = g.C(30) ? 1u : 0u;
    // vehiculo guardado
    b.P16(RS, 0x12, kTm[km]);
    for (int k = 0; k < 3; ++k) b.P32(RS, 4 * k, PickFloat(g));
    b.P32(RS, 0x0C, g.R());
    b.P8(RS, 0x10, g.R());
    for (int i = 0; i < 15; ++i) b.P16(RS, 0x14 + 2 * i, g.C(40) ? 0xFFFFu : (g.R() & 0xFFFFu));
    static const u32 packed[]{0x80u, 0x7Fu, 0u, 1u, 0xFFu, 100u, 0x9Cu, 0x64u, 0x81u};
    for (int k = 0; k < 3; ++k) b.P8(RS, 0x3C + k, g.C(30) ? packed[g.P(9)] : g.R());
    for (int k = 0x32; k <= 0x3B; ++k) b.P8(RS, k, g.R());
    // vehiculos nuevos V0 (reserva) y V1 (la devuelve el constructor si ctor1)
    b.P32(RV0, 0x14, kMarkMat0); b.P32(RV1, 0x14, kMarkMat1);
    for (u32 r = RV0; r <= RV1; ++r) {
        b.P32(r, 0x40, g.R());
        b.P32(r, 0x38C, g.R());
        b.P8(r, 0x428, g.R()); b.P8(r, 0x42A, g.R()); b.P8(r, 0x42E, g.R()); b.P8(r, 0x42F, g.R()); b.P8(r, 0x4A8, g.R()); b.P8(r, 0x614, g.R()); b.P8(r, 0x1BE, g.R());
        b.P32(r, 0x590, g.C(50) ? 0u : (g.C(40) ? 9u : g.R()));
    }
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s, u32 n) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"gs\":" + U(GarbageSeed(s.fn, n)) + ",\"regs\":" + Arr(s.regs, 3) + ",\"remap\":" + U(s.remap) +
                    ",\"state\":" + Arr(s.state, 4) + ",\"vtype\":" + Arr(s.vtype, 4) + ",\"has\":" + U(s.hasMask) + ",\"ctor1\":" + U(s.ctor1) + ",\"comps\":" + Arr(s.comps, 2) + ",\"pre\":[";
    for (std::size_t i = 0; i < s.pre.size(); ++i) {
        if (i) o += ',';
        o += "[" + U(s.pre[i].region) + "," + U(s.pre[i].off) + "," + U(s.pre[i].size) + "," + U(s.pre[i].val) + "]";
    }
    return o + "]}";
}
}  // namespace SC64Fix
