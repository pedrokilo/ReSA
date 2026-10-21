#pragma once
// Fixtures del banco CarCtrlSteer63b (lote 63, CCarCtrl parte B1, tanda 2): SteerAICarWithPhysicsFollowPreRecordedPath (0x432DD0), 4096 casos. Solo C++ estandar (sin tipos del juego).
//
//   CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath  0x432DD0  cdecl (vehicle, float* steer, float* accel, float* brake, bool* handbrake), 5 argumentos, void
//
// Cada caso es una descripcion SEMANTICA (la leen el banco y fixture_oracle.py): vehiculo, estado de la reproduccion (arrays 0x97D6D0..0x97D840 del EXE), ventana de 12 registros de la
// grabacion (32 bytes cada uno: posicion en +0x14, velocidad en +4/+6), resultados de los colaboradores sustituidos por orden de llamada.
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>

namespace CS63BFix {
using i16 = std::int16_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kFns = 1;
constexpr unsigned kPerFn = 4096;
constexpr unsigned kCases = kFns * kPerFn;
constexpr unsigned kFamilies = 8;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 99991u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline u32 Bits(float f) { u32 v; std::memcpy(&v, &f, 4); return v; }
inline float Flt(u32 v) { float f; std::memcpy(&f, &v, 4); return f; }
inline u64 DBits(double d) { u64 v; std::memcpy(&v, &d, 8); return v; }
inline double DFlt(u64 v) { double d; std::memcpy(&d, &v, 8); return d; }

// ------------------------------------------------------------------------------------------------ disposicion
constexpr u32 kVehSize = 0x900, kMatWords = 24, kGuardWords = 4, kOutWords = 8, kFrameSize = 0x20, kWindow = 12, kRecFrames = 52;
constexpr u32 kVPos = 0x04, kVMat = 0x14, kVSpeed = 0x44, kVMission = 0x3BA, kVRecId = 0x424;
constexpr u32 kMTop = 0x10, kMPos = 0x30;
// arrays de CVehicleRecording (16 entradas): paused 0x97D6D0 (bool), speed 0x97D700 (float), size 0x97D780 (int32), index 0x97D7C0 (int32), buffer 0x97D800 (puntero)
constexpr u32 kPausedAddr = 0x97D6D0, kSpeedAddr = 0x97D700, kSizeAddr = 0x97D780, kIndexAddr = 0x97D7C0, kBufferAddr = 0x97D800, kRecBase = 0x97D6C0, kRecWords = 0x1C0 / 4, kSlots = 16;
constexpr u32 kGarbageSeed = 0x510E527Fu;
inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0xC0000000u | (Rand(n + kGarbageSeed, k) & 0x3FFFFFFFu);
    return v == 0xFFFFFFFFu ? 0xC0000000u : v;
}

// tipos de colaborador (indice en los eventos)
enum : u32 { kAtan = 1, kWeave, kMaxSteer, kStopPlayback, kKinds };

struct Spec {
    u32 family{}, regs[4]{};
    u32 recId{};                 // byte +0x424 (int8)
    u32 slot{};                  // indice 0..15 que usa el caso (si recId es valido)
    u32 hasBase{1}, paused{}, f0{}, nf{}, startFrame{}, extraSize{};   // ventana: registros f0..f0+nf-1; el contador empieza en el registro startFrame (absoluto); tamano = (f0+nf)*32 + extraSize
    u32 playSpeed{};
    u32 vpos[3]{}, mpos[3]{}, mtop[3]{}, msp[3]{};
    u32 mission{};
    u32 frame[kWindow][5]{};     // x, y, z (float), velocidades w4 y w6 (int16 en un u32)
    u64 atan[2]{};
    u32 weave{}, maxSteer{};
    u32 outPre[4]{};
    u32 other[kRecWords]{};      // contenido de los demas elementos de los arrays (basura determinista)
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};
constexpr u32 kQNaN = 0x7FC00000u, kInf = 0x7F800000u, kNegInf = 0xFF800000u, kNegZero = 0x80000000u;

template <std::size_t N>
inline u32 PickF(Gen& g, const float (&v)[N]) { return Bits(v[g.P(u32(N))]); }
inline float Grid(Gen& g, int span, float step) { return float(int(g.P(u32(2 * span + 1))) - span) * step; }
inline u64 PickAtan(Gen& g) {
    switch (g.P(12)) {
    case 0: return DBits(0.0);
    case 1: return DBits(3.14159265358979);
    case 2: return DBits(-3.14159265358979);
    case 3: return DBits(double(g.P(2000001)) / 1000000.0 * 12.0 - 12.0);
    case 4: return DBits(double(g.P(2000001)) / 1000000.0 * 40.0 - 40.0);
    case 5: return DBits(std::numeric_limits<double>::quiet_NaN());
    case 6: return DBits(double(Flt(Bits(float(g.P(2000001)) / 1000000.0f * 6.0f - 6.0f))));
    default: return DBits((double(g.R() >> 8) / double(1u << 24)) * 6.283185307179586 - 3.141592653589793);
    }
}

inline Spec MakeSpec(u32 n) {
    Spec s;
    Gen g(n);
    s.family = n % kFamilies;
    s.regs[0] = StackSeed(n);
    s.regs[1] = g.C(30) ? Garbage(n, 1) : g.R();
    s.regs[2] = g.C(30) ? Garbage(n, 2) : g.R();
    s.regs[3] = g.C(30) ? Garbage(n, 3) : g.R();
    for (u32 i = 0; i < kRecWords; ++i) s.other[i] = Garbage(n, 200 + i);
    for (int i = 0; i < 4; ++i) s.outPre[i] = Garbage(n, 70 + i);
    for (int i = 0; i < 3; ++i) {
        s.vpos[i] = g.C(60) ? Bits(Grid(g, 400, 0.25f)) : Garbage(n, 10 + i);
        s.mpos[i] = g.C(60) ? Bits(Grid(g, 400, 0.25f)) : Garbage(n, 20 + i);
    }
    // identificador de la reproduccion: valido casi siempre; negativo a veces
    s.slot = g.P(kSlots);
    s.recId = g.C(8) ? (g.C(50) ? 0xFFu : (0x80u + g.P(0x80u))) : s.slot;
    s.hasBase = g.C(94) ? 1u : 0u;
    s.paused = g.C(10) ? 1u : 0u;
    {
        static const float t[]{0.0f, 0.5f, 1.0f, 2.0f, 1.5f, 0.25f, 4.0f, -1.0f, 0.1f, 3.0f};
        s.playSpeed = g.C(5) ? kQNaN : (g.C(70) ? PickF(g, t) : Bits(float(g.P(4001)) * 0.001f));
    }
    s.f0 = g.C(15) ? 24u + g.P(20) : g.P(6);                 // ventanas altas: contador > 0x320 (25 registros)
    s.nf = 3 + g.P(kWindow - 2);                              // 3..12 registros
    if (s.f0 + s.nf > 46) s.nf = 46 - s.f0;                   // el avance nunca sale del bufer del banco
    s.startFrame = s.f0 + (g.C(10) ? s.nf + g.P(3) : g.P(s.nf)); // casi siempre dentro de la ventana; a veces al final (fin de la grabacion al avanzar)
    if (s.startFrame > 46) s.startFrame = 46;
    s.extraSize = g.C(15) ? g.P(0x60) : 0u;
    s.hasBase = s.hasBase;
    s.mission = g.R() & 0xFF;
    // registros: trayectoria que pasa cerca del vehiculo
    float px = Flt(s.mpos[0]), py = Flt(s.mpos[1]), pz = Flt(s.mpos[2]);
    for (int i = 0; i < 3; ++i) { /* mpos puede ser basura: las distancias pueden ser NaN, es parte del caso */ }
    const float step = g.C(50) ? 5.0f + float(g.P(5)) : 8.0f + float(g.P(30));
    const float dirx = Grid(g, 10, 0.1f), diry = Grid(g, 10, 0.1f);
    const float basex = px + Grid(g, 20, 1.0f), basey = py + Grid(g, 20, 1.0f);
    for (u32 i = 0; i < kWindow; ++i) {
        float fx = basex + dirx * step * float(int(i) - 4) + Grid(g, 8, 0.5f);
        float fy = basey + diry * step * float(int(i) - 4) + Grid(g, 8, 0.5f);
        float fz = pz + Grid(g, 4, 0.5f);
        s.frame[i][0] = g.C(2) ? kQNaN : Bits(fx);
        s.frame[i][1] = Bits(fy);
        s.frame[i][2] = Bits(fz);
        const u32 w4 = g.C(10) ? (g.C(50) ? 0u : 0x8000u) : u32(int(g.P(40001)) - 20000) & 0xFFFFu;
        const u32 w6 = g.C(10) ? (g.C(50) ? 0u : 0x7FFFu) : u32(int(g.P(40001)) - 20000) & 0xFFFFu;
        s.frame[i][3] = w4;
        s.frame[i][4] = w6;
    }
    {
        // vehiculo: matriz presente (el EXE desreferencia la matriz en la rama de giro); direccion de avance
        switch (g.P(8)) {
        case 0: s.mtop[0] = 0; s.mtop[1] = 0; break;
        case 1: s.mtop[0] = kQNaN; s.mtop[1] = Bits(Grid(g, 20, 0.05f)); break;
        default: { const double a = double(g.P(62832)) * 0.0001; s.mtop[0] = Bits(float(std::cos(a))); s.mtop[1] = Bits(float(std::sin(a))); break; }
        }
        s.mtop[2] = Bits(Grid(g, 20, 0.05f));
        static const float sp[]{0.0f, 0.01f, 0.02f, 0.03f, 0.05f, 0.1f, 0.2f, 0.5f, -0.02f, 0.0166f};
        for (int i = 0; i < 3; ++i) s.msp[i] = g.C(4) ? kQNaN : (g.C(70) ? PickF(g, sp) : Bits(float(g.P(1601)) * 0.001f - 0.8f));
    }
    for (int i = 0; i < 2; ++i) s.atan[i] = PickAtan(g);
    {
        static const float w[]{0.0f, 0.5f, -0.5f, 1.0f, 3.0f, -3.0f, 2.0f, 0.1f};
        s.weave = g.C(8) ? kQNaN : (g.C(50) ? PickF(g, w) : Bits(float(g.P(12001)) * 0.001f - 6.0f));
    }
    {
        static const float m[]{0.2f, 0.7f, 0.9f, 0.5f, 0.0f, 0.3f, 0.35f, 0.6f, 0.1f, -0.2f};
        s.maxSteer = g.C(6) ? kQNaN : (g.C(70) ? PickF(g, m) : Bits(float(g.P(1001)) * 0.001f));
    }
    return s;
}

inline std::string U(u64 v) { return std::to_string(v); }
inline std::string Arr(const u32* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string Arr64(const u64* a, std::size_t n) { std::string o = "["; for (std::size_t i = 0; i < n; ++i) { if (i) o += ','; o += U(a[i]); } return o + "]"; }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fam\":" + U(s.family) + ",\"regs\":" + Arr(s.regs, 4) + ",\"recid\":" + U(s.recId) + ",\"slot\":" + U(s.slot) + ",\"base\":" + U(s.hasBase) + ",\"paused\":" + U(s.paused) + ",\"f0\":" + U(s.f0) +
                    ",\"nf\":" + U(s.nf) + ",\"start\":" + U(s.startFrame) + ",\"extra\":" + U(s.extraSize) + ",\"speed\":" + U(s.playSpeed) + ",\"vpos\":" + Arr(s.vpos, 3) + ",\"mpos\":" + Arr(s.mpos, 3) +
                    ",\"mtop\":" + Arr(s.mtop, 3) + ",\"msp\":" + Arr(s.msp, 3) + ",\"mission\":" + U(s.mission) + ",\"frames\":[";
    for (u32 i = 0; i < kWindow; ++i) { if (i) o += ','; o += Arr(s.frame[i], 5); }
    o += "],\"atan\":" + Arr64(s.atan, 2) + ",\"weave\":" + U(s.weave) + ",\"maxsteer\":" + U(s.maxSteer) + ",\"outpre\":" + Arr(s.outPre, 4) + "}";
    return o;
}
}  // namespace CS63BFix
