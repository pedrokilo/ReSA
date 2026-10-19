#pragma once
// Fixtures del banco RadarBlip28: CRadar::DrawEntityBlip en 0x587000 (cdecl, 2 argumentos en pila: blipIndex, isSprite; `ret` plano).
// Solo C++ estandar (sin tipos del juego): las incluye LiveComparison.hpp/Core.hpp. El oraculo (fixture_oracle.py) trabaja con la fila `input` que escribe Core.hpp
// a partir de esta estructura F.
//
// Un caso queda fijado por n (0..kCases-1). La mezcla es de radices (no aleatoria) para que ninguna combinacion de interes quede sin cubrir:
//   d0 = n % 16            tipo de camino (ver Path y kPathOf: los caminos productivos aparecen dos veces)
//   d1 = (n / 16) % 2      pase de iconos (isSprite != 0 en su byte bajo)
//   d2 = (n / 32) % 2      RadarZoomValue != 0
//   d3 = (n / 64) % 4      bandera de pantalla del blip (0 ninguna, 1 solo marcador, 2 solo icono, 3 ambos; con sesgo hacia los que dibujan)
//   d4 = (n / 256) % 2     mapa del menu abierto (m_bDrawingMap)
//   d5 = (n / 512) % 2     blip de corto alcance
//   d6 = (n / 1024) % 2    blip persistente (remain) y variante de ShowMissionBlips
//   d7 = (n / 2048) % 4    variantes varias (icono, camara, pila x87, estaticas)
// 16*2*2*4*2*2*2*4 = 8192 combinaciones exactas (kCases = 12288: los casos 8192.. repiten las combinaciones con otros floats); el resto (floats, punteros, estados de las tablas) sale de Rand(n, k).
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>

namespace RadarBlip28Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 12288;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline float Fl(u32 b) { return std::bit_cast<float>(b); }
inline u32 Bits(float f) { return std::bit_cast<u32>(f); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n < kStackSeeds.size() ? kStackSeeds[n] : Rand(n + 0x1000u, 99); }

enum Path : u32 {
    P_CAR, P_CAR_MISS, P_CHAR_FOOT, P_CHAR_VEH, P_CHAR_ODD, P_OBJECT, P_PICKUP, P_SPOT, P_AIR_NEAR, P_AIR_FAR, P_INVALID, P_TRICK
};

// Disposicion (CEntity/CPed/CVehicle/CObject): +0x04 posicion compacta (x,y,z), +0x14 puntero a matriz, +0x36 byte de tipo (bits 0-2), peds: +0x46D byte alto de banderas (bit 0 = en vehiculo),
// +0x58C m_pVehicle, +0x78C m_pEnex. Matriz: posicion en +0x30.
struct Ent {
    u8 typeByte, hasMat, flags46d;
    i8 vehKind;                 // m_pVehicle del peaton: -1 nulo, 0..3 vehiculo del pool, 4 vehiculo extra (fuera del pool)
    u32 pos[3], matPos[3];
    u32 enex;                   // m_pEnex (+0x78C); 0 = nulo (en objetos no se escribe: tipo != 3)
};

constexpr int kPoolSlots = 4;

struct F {
    u32 kind, n;
    // llamada
    i32 blipIndex; u32 isSprite, eax, edx, stack;
    u8 trace[40];
    // globales
    u8 drawingMap, showMission, showAll, zoom, dbg, airLoc, camActive;
    u32 width, height, orient, range, timeMs;
    u16 camModes[3];
    u8 statics[20];             // 0xBAA370..0xBAA383
    // mundo: pools falsos (0 vehiculos, 1 peds, 2 objetos)
    u8 states[3][8];
    Ent ents[3][kPoolSlots];
    Ent extraVeh;
    // FindPlayerVehicle: 0 nulo, 1 siempre pv[0], 2 pv[min(llamada, 3)]
    u8 pvMode;
    Ent pv[4];
    // salidas de los colaboradores sustituidos (por numero de llamada)
    u32 xform[4][2], limitPt[3][2], limitDist[3], screen[2][2];
    u32 colour; u8 revealed;
    i32 pickupIdx, scriptIdx;
    u32 heading, centre[3];
    u32 spotTarget[3], spotRadius;
    i16 pickupRaw[3];
};

struct Gen {
    u32 n, k{};
    u32 R() { return Rand(n, 1000 + k++); }
    u32 Below(u32 m) { return R() % m; }
    bool Chance(u32 pct) { return Below(100) < pct; }
    float Range(float lo, float hi) { return lo + (hi - lo) * float(Below(100000)) / 100000.0f; }
    template<class T, std::size_t N> T Pick(const std::array<T, N>& a) { return a[Below(u32(N))]; }
};

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 12> kMag{0x00000000u, 0x3DCCCCCDu, 0x3F000000u, 0x3F666665u, 0x3F666666u, 0x3F666667u, 0x3F7FFFFFu, 0x3F800000u, 0x3F800001u, 0x3FC00000u, 0x40400000u, 0x42C80000u};
inline constexpr std::array<u32, 9> kDist{0x00000000u, 0x3F000000u, 0x3F7FFFFFu, 0x3F800000u, 0x3F800001u, 0x40000000u, 0x41200000u, 0x447A0000u, 0x7FC00000u};
inline constexpr std::array<u32, 8> kRadius{0x3F800000u, 0x3F8CCCCDu, 0x3F99999Au, 0x3F000000u, 0x40A00000u, 0x40200000u, 0xC0400000u, 0x7FC00000u};
// desplazamiento del z del jugador respecto al del blip: bajo (z - 2 > cz), alto (z + 4 < cz) y misma altura (con los bordes exactos)
inline constexpr std::array<u32, 3> kDeltaLow{0xC1200000u, 0xC0000001u, 0xC0600000u};
inline constexpr std::array<u32, 3> kDeltaHigh{0x40800001u, 0x41200000u, 0x40900000u};
inline constexpr std::array<u32, 8> kDeltaSame{0xC0000000u, 0xBFFFFFFFu, 0x00000000u, 0x407FFFFFu, 0x40800000u, 0x40000000u, 0x3F800000u, 0xBF800000u};
inline constexpr std::array<u32, 8> kSizes{0u, 1u, 3u, 0x7FFFu, 0x8000u, 0xFFFFu, 0xFFFEu, 0x1234u};
inline constexpr std::array<std::pair<u32, u32>, 12> kScreens{{{640, 480}, {800, 600}, {1024, 768}, {1280, 720}, {1366, 768}, {1920, 1080}, {2560, 1440}, {3840, 2160}, {1, 1}, {0, 0}, {1000, 3}, {5, 1000}}};
inline constexpr std::array<u32, 6> kRange{0x447A0000u, 0x43340000u, 0x438DEAAAu, 0x42480000u, 0x457A0000u, 0x3F800000u};

inline u32 RandFloatBits(Gen& g, float lo, float hi) { return Bits(g.Range(lo, hi)); }
inline u32 Pos3Z(Gen& g) { return g.Chance(2) ? 0x7FC00000u : Bits(g.Range(-50.0f, 400.0f)); }

inline Ent MakeEnt(Gen& g, int pool) {
    Ent e{};
    const u32 hi = g.R() & 0xF8u;                                   // bits 3-7 del byte de tipo: basura
    if (pool == 0) e.typeByte = u8(hi | (g.Chance(8) ? 3u : 2u));
    else if (pool == 1) e.typeByte = u8(hi | (g.Chance(8) ? 2u : 3u));
    else { static constexpr u32 t[]{0, 1, 2, 4, 5, 6, 7}; e.typeByte = u8(hi | t[g.Below(7)]); }
    e.hasMat = g.Chance(50);
    for (int i = 0; i < 2; ++i) { e.pos[i] = RandFloatBits(g, -3000.0f, 3000.0f); e.matPos[i] = RandFloatBits(g, -3000.0f, 3000.0f); }
    e.pos[2] = Pos3Z(g); e.matPos[2] = Pos3Z(g);
    e.flags46d = u8(g.R());
    e.vehKind = g.Chance(15) ? i8(-1) : g.Chance(70) ? i8(g.Below(4)) : i8(4);
    e.enex = (pool != 2 && g.Chance(50)) ? 0x00A10000u + 0x40u * g.Below(16) : 0u;
    return e;
}

inline void MakeRadarPoint(Gen& g, u32 out[2], bool allowNan) {
    const u32 m = g.Chance(70) ? kMag[g.Below(8)] : g.Pick(kMag);
    const float f = Fl(m);
    u32 sel = g.Below(allowNan ? 12u : 5u);
    if (sel >= 5 && sel < 11) sel -= 5;                              // con NaN permitido, 1 de cada 12 (el resto reparte los cinco estilos)
    switch (sel) {
    case 0: out[0] = m; out[1] = 0; break;
    case 1: out[0] = 0; out[1] = Bits(-f); break;
    case 2: out[0] = Bits(f * 0.6f); out[1] = Bits(f * 0.8f); break;
    case 3: out[0] = Bits(g.Range(-1.2f, 1.2f)); out[1] = Bits(g.Range(-1.2f, 1.2f)); break;
    case 4: out[0] = Bits(-f); out[1] = m; break;
    default: out[0] = (g.Chance(50) ? 0x7FC00000u : 0xFFC00000u); out[1] = (g.Chance(50) ? 0u : Bits(f)); break;      // x = NaN (y finito: una sola NaN por operacion)
    }
}

// Posicion que el EXE usa para el blip (aproximada, solo para colocar umbrales de altura)
inline float ApproxZ(const F& f, const Ent* e) {
    float z = 0.0f;
    if (e) z = Fl(e->hasMat ? e->matPos[2] : e->pos[2]);
    (void)f;
    return z;
}

struct Strip { float x, y, dir, len; };
inline constexpr std::array<Strip, 4> kStrips{{{1750.0f, -2494.0f, 180.0f, 1000.0f}, {-1373.0f, 120.0f, 315.0f, 1500.0f}, {1478.0f, 1461.0f, 90.0f, 1200.0f}, {175.0f, 2502.0f, 180.0f, 1000.0f}}};

inline constexpr std::array<u32, 16> kPathOf{P_CAR, P_CAR, P_CAR_MISS, P_CHAR_FOOT, P_CHAR_FOOT, P_CHAR_VEH, P_CHAR_VEH, P_CHAR_ODD, P_OBJECT, P_PICKUP, P_SPOT, P_AIR_NEAR, P_AIR_NEAR, P_AIR_FAR, P_INVALID, P_TRICK};

inline F MakeF(unsigned n) {
    F f{};
    Gen g{n};
    f.n = n;
    const u32 d0 = kPathOf[n % 16], d1 = (n / 16) % 2, d2 = (n / 32) % 2, d4 = (n / 256) % 2, d5 = (n / 512) % 2, d6 = (n / 1024) % 2, d7 = (n / 2048) % 4;
    u32 d3 = (n / 64) % 4;                                          // pantalla del blip: los valores 0 y 1 (no dibujan) se mezclan con los que si
    if (d3 == 0 && g.Chance(60)) d3 = 3;
    if (d3 == 1 && g.Chance(60)) d3 = 2;
    f.kind = n % 16;
    f.eax = g.R(); f.edx = g.R(); f.stack = StackSeed(n);

    // ---- llamada
    f.blipIndex = (n % 97 == 0) ? 0 : (n % 89 == 0) ? 174 : i32(g.Below(175));
    if (d1) { static constexpr u32 v[]{1u, 2u, 0x80u, 0xFFu, 0xFFFFFF01u, 0x01000001u, 0x00AB0001u}; f.isSprite = v[g.Below(7)]; }
    else { static constexpr u32 v[]{0u, 0x100u, 0xFFFFFF00u, 0x0000AB00u, 0x80000000u}; f.isSprite = v[g.Below(5)]; }

    // ---- globales
    f.drawingMap = d4;
    f.showMission = d6 ? 1 : g.Chance(80);
    f.showAll = g.Chance(50);
    f.zoom = d2 ? u8(1 + g.Below(255)) : u8(0);
    f.dbg = g.Chance(30);
    f.airLoc = u8(g.Below(4));
    f.camActive = u8(g.Below(3));
    { const auto sc = g.Pick(kScreens); f.width = sc.first; f.height = sc.second; }
    f.orient = g.Chance(20) ? 0u : Bits(g.Range(-3.3f, 3.3f));
    f.range = g.Pick(kRange);
    f.timeMs = g.R() % 3 == 0 ? g.R() : 100000u + g.Below(1000000);
    for (int i = 0; i < 3; ++i) { static constexpr u16 modes[]{1, 1, 0, 2, 3, 4, 0x101, 0xFFFF, 0x8001, 0x37}; f.camModes[i] = modes[g.Below(10)]; }
    if (d7 == 0 && g.Chance(70)) f.camModes[f.camActive] = 1;
    if (d7 == 1) f.camModes[f.camActive] = (g.Chance(50) ? 0x0101 : u16(g.Below(60) + 2));

    // ---- mundo: tablas de los pools
    for (int p = 0; p < 3; ++p) {
        for (int s = 0; s < 8; ++s) f.states[p][s] = g.Chance(70) ? u8(1 + g.Below(0x7E)) : u8(0x80 + g.Below(0x7F));
        for (int s = 0; s < kPoolSlots; ++s) f.ents[p][s] = MakeEnt(g, p);
    }
    f.extraVeh = MakeEnt(g, 0);
    f.extraVeh.typeByte = u8((f.extraVeh.typeByte & 0xF8u) | 2u);
    f.pvMode = 0;

    // ---- colaboradores
    for (auto& x : f.xform) MakeRadarPoint(g, x, true);
    for (int i = 0; i < 3; ++i) {
        MakeRadarPoint(g, f.limitPt[i], true);
        f.limitDist[i] = g.Pick(kDist);
    }
    for (auto& s : f.screen) { s[0] = Bits(g.Range(-100.0f, 800.0f)); s[1] = Bits(g.Range(-100.0f, 600.0f)); }
    f.colour = g.R();
    if (g.Chance(30)) f.colour = (f.colour & 0xFFFFFF00u) | (g.Chance(50) ? 0xFFu : 0u);
    f.revealed = g.Chance(60);
    f.heading = g.Chance(5) ? 0x7FC00000u : Bits(g.Range(-7.0f, 7.0f));
    f.centre[0] = Bits(g.Range(-3000.0f, 3000.0f)); f.centre[1] = Bits(g.Range(-3000.0f, 3000.0f));
    for (int i = 0; i < 3; ++i) f.spotTarget[i] = (i == 2) ? Pos3Z(g) : RandFloatBits(g, -3000.0f, 3000.0f);
    f.spotRadius = g.Chance(15) ? 0u : Bits(g.Range(0.5f, 120.0f));
    for (auto& r : f.pickupRaw) r = i16(g.R());
    f.pickupRaw[2] = i16(int(g.Below(4000)) - 800);
    for (int i = 0; i < 4; ++i) f.pv[i] = MakeEnt(g, 0);

    // ---- traza del blip
    u8* t = f.trace;
    auto put32 = [&](int at, u32 v) { std::memcpy(t + at, &v, 4); };
    auto put16 = [&](int at, u16 v) { std::memcpy(t + at, &v, 2); };
    for (int i = 0; i < 40; ++i) t[i] = u8(g.R());                  // relleno (incluye los bytes de alineacion, que no deben cambiar)
    put32(0, g.Chance(70) ? g.Below(32) : g.R());
    put32(8, RandFloatBits(g, -3000.0f, 3000.0f)); put32(12, RandFloatBits(g, -3000.0f, 3000.0f)); put32(16, RandFloatBits(g, -50.0f, 400.0f));
    put32(0x18, g.Pick(kRadius));
    put16(0x1C, u16(g.Chance(50) ? g.Pick(kSizes) : g.R()));
    put32(0x20, g.Chance(30) ? 0x00A20000u + 0x40u * g.Below(8) : 0u);
    { u8 sp = g.Chance(50) ? u8(0) : u8(g.R()); if (d7 == 2 && g.Chance(50)) sp = u8(0xFB + g.Below(5)); t[0x24] = sp; }
    u8 f25 = u8(g.R());
    f25 = u8((f25 & ~0x04u) | ((d5 && g.Chance(45)) ? 0x04u : 0u));
    f25 = u8((f25 & ~0x10u) | ((d6 && g.Chance(70)) ? 0x10u : 0u));
    t[0x25] = f25;

    // ---- camino
    u32 type = 1;
    u32 handleKind = 0;                                             // 0 acierta, 1 falla por estado, 2 indice fuera de rango
    switch (d0) {
    case P_CAR: type = 1; break;
    case P_CAR_MISS: type = 1; handleKind = g.Chance(50) ? 1u : 2u; break;
    case P_CHAR_FOOT: case P_CHAR_VEH: case P_CHAR_ODD: type = 2; break;
    case P_OBJECT: type = 3; if (g.Chance(20)) handleKind = g.Chance(50) ? 1u : 2u; break;
    case P_PICKUP: type = 7; break;
    case P_SPOT: type = 6; break;
    case P_AIR_NEAR: case P_AIR_FAR: type = 8; break;
    case P_INVALID: { static constexpr u32 v[]{0, 4, 5, 9, 10, 11, 12, 13, 14, 15}; type = v[g.Below(10)]; break; }
    default: type = g.Chance(50) ? 1u : 2u; break;                    // P_TRICK
    }
    t[0x26] = u8((t[0x26] & 0xC0u) | (type << 2) | d3);

    // entidad del pool apuntada por el blip
    const int pool = type == 1 ? 0 : type == 2 ? 1 : 2;
    const u32 slot = g.Below(kPoolSlots);
    if (type >= 1 && type <= 3) {
        Ent& e = f.ents[pool][slot];
        if (d0 == P_CHAR_FOOT) { e.flags46d = u8(e.flags46d & ~1u); e.typeByte = u8((e.typeByte & 0xF8u) | 3u); }
        if (d0 == P_CHAR_VEH) { e.flags46d = u8(e.flags46d | 1u); e.vehKind = g.Chance(85) ? i8(g.Below(5)) : i8(4); }
        if (d0 == P_CHAR_ODD) { e.flags46d = u8(e.flags46d | 1u); e.vehKind = -1; if (g.Chance(40)) e.flags46d = u8(e.flags46d & ~1u); }
        if (d0 == P_TRICK) { e.typeByte = u8((e.typeByte & 0xF8u) | (pool == 2 ? 4u : (g.Chance(50) ? 3u : 2u))); if (g.Chance(60)) e.enex = 0x00A30000u + 0x40u * g.Below(8); }
        if (d0 == P_CAR && g.Chance(40)) e.enex = 0;
        // vehiculos referidos desde un peaton: deben ser validos para la posicion
        if (handleKind == 0) f.states[pool][slot] = u8(1 + g.Below(0x7E));
        u32 handle;
        if (handleKind == 0) handle = (slot << 8) | f.states[pool][slot];
        else if (handleKind == 1) handle = (slot << 8) | u8(f.states[pool][slot] ^ (1u << g.Below(8)));
        else { u32 idx = 8 + g.Below(40); u8 b = u8(g.R()); if (b == 0x80) b = 0x81; handle = (idx << 8) | b; }
        put32(4, handle);
    } else if (type == 7) {
        put32(4, g.R());
        f.pickupIdx = g.Chance(20) ? i32(-1 - int(g.Below(3))) : i32(g.Below(4));
    } else if (type == 6) {
        put32(4, g.R());
        f.scriptIdx = g.Chance(20) ? i32(-1 - int(g.Below(3))) : i32(g.Below(4));
    } else {
        put32(4, g.R());
    }

    // sprite del blip y bandera de pantalla coherentes con el pase
    if (d1 && g.Chance(85) && t[0x24] == 0) t[0x24] = u8(1 + g.Below(63));
    if (!d1 && g.Chance(85)) t[0x24] = 0;

    // ---- jugador y vehiculo del jugador
    const bool air = d0 == P_AIR_NEAR || d0 == P_AIR_FAR;
    if (air) {
        f.pvMode = g.Chance(50) ? 1 : 2;
        const Strip s = kStrips[f.airLoc];
        // distancia objetivo: cerca (< 500) o lejos / NaN
        static constexpr float nearD[]{0.0f, 10.0f, 100.0f, 299.99f, 480.0f, 499.9f, 499.99f};
        static constexpr float farD[]{500.0f, 500.1f, 600.0f, 1000.0f, 5000.0f, 500.0f};
        float baseDx, baseDy;
        {
            const float dist = d0 == P_AIR_NEAR ? nearD[g.Below(7)] : farD[g.Below(6)];
            if (dist == 500.0f && g.Chance(70)) { baseDx = 300.0f; baseDy = 400.0f; }
            else { const float a = g.Range(0.0f, 6.2831853f); baseDx = std::cos(a) * dist; baseDy = std::sin(a) * dist; }
        }
        for (int i = 0; i < 4; ++i) {
            Ent& v = f.pv[i];
            float dx = baseDx, dy = baseDy;
            if (f.pvMode == 2 && i > 0) { dx += g.Range(-3.0f, 3.0f); dy += g.Range(-3.0f, 3.0f); }      // cada llamada devuelve un vehiculo distinto (de que llamada sale cada termino)
            if (f.pvMode == 2 && g.Chance(10)) { dx = -dx; }                                              // signos cruzados: el producto puede salir negativo (raiz de negativo = NaN)
            v.pos[0] = Bits(s.x + dx); v.pos[1] = Bits(s.y + dy); v.matPos[0] = v.pos[0]; v.matPos[1] = v.pos[1];
            v.hasMat = g.Chance(50);
            if (d0 == P_AIR_FAR && i == 0 && g.Chance(8)) { v.pos[0] = 0x7FC00000u; v.matPos[0] = 0x7FC00000u; }   // distancia NaN
        }
        // Estaticas del destello
        const u32 now = f.timeMs;
        u32 last;
        switch (g.Below(10)) {
        case 0: last = now; break; case 1: last = now - 1; break; case 2: last = now - 2; break; case 3: last = now - 3; break; case 4: last = now - 4; break;
        case 5: last = now - 5; break; case 6: last = now + 1; break; case 7: last = now - 100000u; break; case 8: last = now - 0xFFFFFFFDu; break;
        default: last = g.R(); break;
        }
        u32 guard = g.Below(4);
        if (g.Chance(40)) guard |= (g.R() & 0xFFFFFF00u);
        const i16 len = i16(s.len);
        i16 stepA;
        switch (g.Below(10)) {
        case 0: stepA = i16(len - 100); break; case 1: stepA = i16(len - 99); break; case 2: stepA = i16(len - 101); break; case 3: stepA = i16(-len); break;
        case 4: stepA = i16(32700 + int(g.Below(68))); break; case 5: stepA = i16(-32768); break; case 6: stepA = i16(len); break; case 7: stepA = 0; break;
        default: stepA = i16(g.R()); break;
        }
        i16 stepB;
        switch (g.Below(6)) {
        case 0: stepB = i16(stepA + 100); break; case 1: stepB = stepA; break; case 2: stepB = i16(stepA - 100); break; case 3: stepB = i16(-len); break;
        default: stepB = i16(g.R()); break;
        }
        std::memset(f.statics, 0, sizeof f.statics);
        for (int i = 0; i < 20; ++i) f.statics[i] = u8(g.R());          // los bytes de relleno deben quedar intactos
        f.statics[0] = g.Chance(50) ? 1 : 0;
        std::memcpy(f.statics + 4, &last, 4);
        std::memcpy(f.statics + 8, &stepB, 2);
        std::memcpy(f.statics + 0xC, &stepA, 2);
        std::memcpy(f.statics + 0x10, &guard, 4);
        // el punto de la pista (posicion original del blip): cerca de la pista para que los valores no sean triviales
        put32(8, Bits(s.x + g.Range(-200.0f, 200.0f))); put32(12, Bits(s.y + g.Range(-200.0f, 200.0f))); put32(16, Bits(g.Range(0.0f, 100.0f)));
        // el blip debe poder dibujarse en ambos pases; el sprite de una pista es 0 salvo variante
        if (!(d7 == 2)) t[0x24] = 0;
    } else {
        for (int i = 0; i < 20; ++i) f.statics[i] = u8(g.R());
        f.statics[0] = g.Chance(50) ? 1 : 0;
        f.pvMode = d2 ? (g.Chance(d0 == P_SPOT ? 30 : 65) ? 0 : (g.Chance(50) ? 1 : 2)) : (g.Chance(50) ? 0 : 1);
        if (d0 == P_SPOT && g.Chance(60)) { const u32 m = kMag[g.Below(6)]; f.limitPt[0][0] = m; f.limitPt[0][1] = 0; }       // dentro del radar (<= 0.9) para los circulos
    }

    // ---- salidas de pantalla: elegir el centro del jugador respecto a la altura del blip
    {
        const Ent* e = nullptr;
        if (type >= 1 && type <= 3) e = &f.ents[pool][slot];
        float zref = ApproxZ(f, e);
        if (type == 6) zref = Fl(f.spotTarget[2]);
        if (type == 7) zref = float(f.pickupRaw[2]) * 0.125f;
        if (type == 2 && e && (e->flags46d & 1) && e->vehKind >= 0) { const Ent& v = e->vehKind == 4 ? f.extraVeh : f.ents[0][e->vehKind]; zref = Fl(v.hasMat ? v.matPos[2] : v.pos[2]); }
        if (std::isnan(zref)) zref = 0.0f;
        const u32 cls = g.Below(3);
        const u32 delta = cls == 0 ? g.Pick(kDeltaLow) : cls == 1 ? g.Pick(kDeltaHigh) : g.Pick(kDeltaSame);
        f.centre[2] = g.Chance(8) ? 0x7FC00000u : Bits(zref + Fl(delta));
    }
    // Pickup: el z del pickup mas 2.0 (proyectado en la altura); la basura del resto de campos del pickup no importa
    return f;
}
}
