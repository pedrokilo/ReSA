// Fixtures del banco Dependencies23B (CVehicle::FlyingControl, 0x6D85F0): vehiculo, datos de vuelo, matriz, colmodelo y colaboradores falsos de cada caso.
// Una sola fuente para el banco dentro del juego (LiveComparison.hpp) y para el arnes sin juego (offline/main.cpp). Sin dependencias del proyecto.
// Rutas: 0..9 = flightModel 0..9, 10 = flightModel 40 (> 8: solo prologo y cola). Cada ruta lleva 1536 casos (1,5 x los 1024 exigidos).
#pragma once
#include <cstdint>
#include <cstring>
#include <cmath>
#include <vector>
using uint8 = std::uint8_t; using uint16 = std::uint16_t; using uint32 = std::uint32_t; using uint64 = std::uint64_t; using int16 = std::int16_t; using int32 = std::int32_t;
namespace Dependencies23BFix {
inline constexpr int kRoutes = 11;
inline constexpr unsigned kCasesPerRoute = 1536;
inline constexpr size_t kArenaSize = 0x4000;
namespace L { inline constexpr size_t veh = 0x100, fh = 0x1200, mat = 0x1300, driver = 0x1400, pad = 0x1C00, col = 0x1D00, other = 0x1E00; }
// ---------------------------------------------------------------- generador
inline uint64 rngState;
inline uint32 Rnd() { rngState ^= rngState << 13; rngState ^= rngState >> 7; rngState ^= rngState << 17; return uint32(rngState >> 16); }
inline uint32 RndN(uint32 n) { return Rnd() % n; }
inline float FromBits(uint32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline uint32 Bits(float f) { uint32 b; std::memcpy(&b, &f, 4); return b; }
inline constexpr uint32 kPool[] = {
    0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0xBF000000, 0x40000000, 0x40490FDB, 0xC0490FDB, 0x7F7FFFFF, 0xFF7FFFFF, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000,
    0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000, 0x7FC12345, 0xFFC54321, 0x7FA00000, 0xFFA00001, 0x7F800001, 0x3F7FFFFF, 0x3F800001, 0xBF7FFFFF, 0xBF800001, 0x4B000000, 0x5F000000, 0x1F800000,
    0x71 << 24, 0xF1 << 24, 0x0DA24260, 0x7149F2CA, 0x3EAAAAAB, 0x3F333333, 0x358637BD, 0x42C80000, 0x3C23D70A, 0x3F666666, 0x3DCCCCCD, 0x00400000
};
inline uint32 StressBits() { const uint32 b = kPool[RndN(sizeof(kPool) / 4)]; return RndN(5) == 0 ? b + int(RndN(5)) - 2 : b; }
inline float RandRange(float lo, float hi) { return lo + (hi - lo) * (float(RndN(1000001)) / 1000000.0f); }
// valor de estres con la probabilidad `pct` (en %), si no un valor normal en [lo, hi]
inline float V(float lo, float hi, int pct) { return RndN(100) < uint32(pct) ? FromBits(StressBits()) : RandRange(lo, hi); }


struct Cfg {
    bool isPlayer{}; int16 steerLR{}, steerUD{}, gunUD{}, gunLR{}, accel{}, brake{}; bool lookL{}, lookR{}; float ceiling{}; uint32 speedSeed{}; bool colNull{};
    uint32 speed[3]{};
};
struct Case {
    int route{}; int model{}; uint32 args[4]{}; uint32 fsw{};
    std::vector<uint8> arena;
    Cfg cfg;
    float ts{}; uint32 wind[3]{}; uint32 ts_bits{};
};
inline void FillFloat(uint8* p, float v) { std::memcpy(p, &v, 4); }
inline Case MakeCase(int route, uint64 seed, uintptr_t base) {
    rngState = seed * 0x9E3779B97F4A7C15ull + 88172645463325252ull;
    for (int i = 0; i < 8; ++i) Rnd();
    Case c;
    c.route = route;
    static constexpr int kModelOfRoute[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 40};
    c.model = kModelOfRoute[route % 11];
    const int stress = RndN(100) < 45 ? 12 : 2;                // % de valores de estres por campo
    c.arena.assign(kArenaSize, 0xA5);
    uint8* a = c.arena.data();
    for (size_t i = L::veh; i < L::veh + 0xA20; ++i) a[i] = uint8(Rnd());                      // basura en el vehiculo (los campos usados se fijan abajo)
    uint8* veh = a + L::veh;
    // matriz casi ortonormal
    {
        float yaw = RandRange(-3.14f, 3.14f), pitch = RandRange(-0.6f, 0.6f), roll = RandRange(-0.6f, 0.6f);
        float cy = std::cos(yaw), sy = std::sin(yaw), cp = std::cos(pitch), sp = std::sin(pitch), cr = std::cos(roll), sr = std::sin(roll);
        float m[12] = { cy * cr - sy * sp * sr, sy * cr + cy * sp * sr, -cp * sr, 0, -sy * cp, cy * cp, sp, 0, cy * sr + sy * sp * cr, sy * sr - cy * sp * cr, cp * cr, 0 };
        float* mf = reinterpret_cast<float*>(a + L::mat);
        for (int i = 0; i < 12; ++i) mf[i] = RndN(100) < uint32(stress) ? FromBits(StressBits()) : m[i];
        mf[3] = mf[7] = mf[11] = 0.f;
        mf[12] = V(-3000.f, 3000.f, stress); mf[13] = V(-3000.f, 3000.f, stress); mf[14] = V(0.f, 200.f, stress); mf[15] = 0.f;
        *reinterpret_cast<uint32*>(veh + 0x14) = uint32(base + L::mat);
    }
    *reinterpret_cast<uint16*>(veh + 0x22) = uint16(RndN(4) == 0 ? 0x21b : (RndN(3) == 0 ? 0x1cc : (RndN(3) == 0 ? 0x208 : 0x1d0 + RndN(60))));
    veh[0x36] = uint8((RndN(3) == 0 ? 0 : (RndN(2) ? 4 : 8)) << 3 | RndN(8));                // estado
    *reinterpret_cast<uint32*>(veh + 0x40) = RndN(2) ? (Rnd() | 0x100u) : (Rnd() & ~0x100u);
    for (int i = 0; i < 3; ++i) { FillFloat(veh + 0x44 + 4 * i, V(-1.2f, 1.2f, stress)); FillFloat(veh + 0x50 + 4 * i, V(-0.2f, 0.2f, stress)); FillFloat(veh + 0xA4 + 4 * i, V(-1.f, 1.f, stress)); }
    FillFloat(veh + 0x8C, V(500.f, 8000.f, stress)); FillFloat(veh + 0x90, V(500.f, 12000.f, stress));
    veh[0x42b] = uint8(RndN(2) ? 0x10 : 0) | uint8(Rnd() & 0xEF);
    veh[0x4A4] = uint8(RndN(4) == 0 ? 2 : 1 + RndN(3));
    FillFloat(veh + 0x494, V(-1.f, 1.f, stress));
    *reinterpret_cast<uint32*>(veh + 0x590) = RndN(3) == 0 ? 0 : (RndN(2) ? 1 : 0x30000000 + RndN(0x100));
    *reinterpret_cast<uint32*>(veh + 0x594) = RndN(3) == 0 ? 4 : RndN(10);
    for (int i = 0; i < 4; ++i) FillFloat(veh + 0x7D4 + 4 * i, RndN(3) == 0 ? V(0.f, 1.f, stress) : 1.0f);
    FillFloat(veh + 0x84C, V(0.f, 1.f, stress));
    *reinterpret_cast<uint16*>(veh + 0x86C) = uint16(RndN(3) == 0 ? 0 : Rnd());
    *reinterpret_cast<uint32*>(veh + 0x9A0) = Rnd();
    FillFloat(veh + 0x9CC, V(0.f, 1.f, stress));
    // datos de vuelo
    float* fh = reinterpret_cast<float*>(a + L::fh);
    for (int i = 0; i < 0x58 / 4; ++i) fh[i] = V(0.f, 1.f, stress);
    *reinterpret_cast<int32*>(fh) = int32(RndN(24));
    fh[0x38 / 4] = V(0.f, 0.5f, stress);                       // windMult
    for (int i = 0; i < 3; ++i) fh[0x40 / 4 + i] = RndN(100) < uint32(stress) ? FromBits(StressBits()) : RandRange(0.7f, 1.0f);       // turnRes
    for (int i = 0; i < 3; ++i) fh[0x4C / 4 + i] = RndN(3) == 0 ? V(-0.2f, 0.f, stress) : V(0.f, 50.f, stress);                    // speedRes
    *reinterpret_cast<uint32*>(veh + 0x388) = RndN(40) == 0 ? 0u : uint32(base + L::fh);
    *reinterpret_cast<uint32*>(veh + 0x460) = RndN(6) == 0 ? 0u : uint32(base + L::driver);
    float* col = reinterpret_cast<float*>(a + L::col);
    for (int i = 0; i < 6; ++i) col[i] = V(-6.f, 6.f, stress);
    // colaboradores
    c.cfg.isPlayer = RndN(4) != 0;
    c.cfg.steerLR = int16(RndN(3) == 0 ? 0 : int(RndN(257)) - 128); c.cfg.steerUD = int16(RndN(3) == 0 ? 0 : int(RndN(257)) - 128);
    c.cfg.gunUD = int16(RndN(3) == 0 ? 0 : int(RndN(257)) - 128); c.cfg.gunLR = int16(RndN(3) == 0 ? 0 : int(RndN(257)) - 128);
    c.cfg.accel = int16(RndN(3) == 0 ? 0 : RndN(256)); c.cfg.brake = int16(RndN(3) == 0 ? 0 : RndN(256));
    c.cfg.lookL = RndN(3) == 0; c.cfg.lookR = RndN(3) == 0;
    c.cfg.ceiling = V(-20.f, 40.f, stress); c.cfg.colNull = false;
    for (int i = 0; i < 3; ++i) c.cfg.speed[i] = Bits(V(-1.f, 1.f, stress));
    // argumentos: -9999.99 (centinela) en parte de los casos
    const uint32 sentinel = 0xC61C3FF6u;
    for (int i = 0; i < 4; ++i) c.args[i] = RndN(3) == 0 ? sentinel : Bits(V(-1.f, 1.f, stress));
    if (RndN(6) == 0) c.args[3] = Bits(V(0.f, 2.f, stress));
    c.ts = RndN(30) == 0 ? FromBits(StressBits()) : RandRange(0.05f, 3.0f);
    for (int i = 0; i < 3; ++i) c.wind[i] = Bits(V(-0.3f, 0.3f, stress));
    c.ts_bits = Rnd();
    static const uint32 fsws[] = {0x0000, 0x0020, 0x003F, 0x0001, 0x4500, 0x0004, 0x0002};
    c.fsw = fsws[RndN(7)];
    return c;
}
} // namespace Dependencies23BFix
