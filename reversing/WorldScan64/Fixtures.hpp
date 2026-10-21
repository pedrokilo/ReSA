#pragma once
// Fixtures del banco WorldScan64 (lote 64): CWorldScan::ScanWorld (0x72CAE0) y CWorldScan::SetExtraRectangleToScan (0x72D5E0). Solo C++ estandar (sin tipos del juego): las incluyen
// Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   CWorldScan::ScanWorld                  0x72CAE0  cdecl(CVector2D* points, int32 count, void(__cdecl*)(int32 x, int32 y)); void
//   CWorldScan::SetExtraRectangleToScan    0x72D5E0  cdecl(float minX, float maxX, float minY, float maxY); void
//
// El caso especifica los puntos (patrones de bits de float), el estado previo de las celdas extra (contador 0xC815B8, X[4] 0xC815A8, Y[4] 0xC81598) y los argumentos del rectangulo.
// Los floats se generan SIEMPRE finitos y sin NaN (el EXE indexaria fuera de rango con NaN), con |coordenada| < 700 y a lo sumo 5 puntos (el EXE guarda 5 bytes de "visitado").
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace WS64Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 2;
constexpr u32 kScan = 0, kExtra = 1;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }

// Region de las celdas extra: 0xC81598 + 4 * palabra. Palabras 0..3 = Y[4], 4..7 = X[4], 8 = contador.
constexpr u32 kRegionBase = 0xC81598;
constexpr u32 kRegionWords = 9;
constexpr u32 kYWord = 0, kXWord = 4, kCountWord = 8;
constexpr u32 kMaxEvents = 700;
constexpr u32 kPointSlots = 8;             // el banco reserva 8 puntos (los 5 primeros son entrada; el resto es relleno)
constexpr u32 kGarbageSeed = 0x6A09E667u;

inline u32 FBits(float f) { return std::bit_cast<u32>(f); }
inline float FOf(u32 b) { return std::bit_cast<float>(b); }

struct Spec {
    u32 fn{};
    u32 n{};                                  // numero de puntos (ScanWorld)
    std::array<u32, 2 * kPointSlots> p{};     // x0,y0,x1,y1,... (bits de float)
    u32 pre[kRegionWords]{};                  // estado previo de la region (Y[4], X[4], contador)
    std::array<u32, 4> a{};                   // SetExtraRectangleToScan: minX, maxX, minY, maxY (bits)
    u32 stack{}, eax{}, ecx{}, edx{};
    u32 family{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
    // entero en [-range, range] dividido entre div (division exacta cuando div es potencia de 2)
    float Q(int range, int div) { return float(int(P(u32(2 * range + 1))) - range) / float(div); }
    float Cell() { return float(int(P(29)) - 14); }
};

constexpr u32 kFamilies = 14;
constexpr u32 kExtraFamilies = 8;

inline u32 Garbage(u32 n, u32 k) {
    u32 v = 0x7F000000u | (Rand(n + kGarbageSeed, k) & 0x00FFFFFFu);   // flotante enorme finito, nunca coincide con una entrada
    return v;
}

inline void SetPt(Spec& s, u32 i, float x, float y) { s.p[2 * i] = FBits(x); s.p[2 * i + 1] = FBits(y); }
inline float PX(const Spec& s, u32 i) { return FOf(s.p[2 * i]); }
inline float PY(const Spec& s, u32 i) { return FOf(s.p[2 * i + 1]); }

inline void MakeScan(Spec& s, Gen& g) {
    const u32 fam = s.family;
    float ox = float(int(g.P(81)) - 40), oy = float(int(g.P(81)) - 40);
    if (fam == 10) { ox = float(int(g.P(601)) - 300); oy = float(int(g.P(601)) - 300); }
    auto box = [&](int range, int div) { return g.Q(range, div); };
    switch (fam) {
    case 0:  // cuadrilatero aleatorio /8
        s.n = 4;
        for (u32 i = 0; i < 4; ++i) SetPt(s, i, ox + 7.0f + box(56, 8), oy + 7.0f + box(56, 8));
        break;
    case 1: {  // rectangulo alineado con los ejes, cuatro ordenes de esquinas
        s.n = 4;
        const float x0 = ox + box(16, 4), y0 = oy + box(16, 4), x1 = x0 + 0.25f * float(1 + g.P(48)), y1 = y0 + 0.25f * float(1 + g.P(48));
        const u32 ord = g.P(4);
        const float cx[4][4] = {{x0, x1, x1, x0}, {x0, x0, x1, x1}, {x1, x1, x0, x0}, {x1, x0, x0, x1}};
        const float cy[4][4] = {{y0, y0, y1, y1}, {y0, y1, y1, y0}, {y1, y0, y0, y1}, {y1, y1, y0, y0}};
        for (u32 i = 0; i < 4; ++i) SetPt(s, i, cx[ord][i], cy[ord][i]);
        break;
    }
    case 2:  // triangulo
        s.n = 3;
        for (u32 i = 0; i < 3; ++i) SetPt(s, i, ox + 7.0f + box(56, 8), oy + 7.0f + box(56, 8));
        break;
    case 3:  // cinco puntos aleatorios
        s.n = 5;
        for (u32 i = 0; i < 5; ++i) SetPt(s, i, ox + 7.0f + box(56, 8), oy + 7.0f + box(56, 8));
        break;
    case 4: {  // duplicados: 3 o 4 puntos base repetidos en posiciones aleatorias (n <= 5)
        const u32 base = 3 + g.P(2);
        float bx[4], by[4];
        for (u32 i = 0; i < base; ++i) { bx[i] = ox + 7.0f + box(56, 8); by[i] = oy + 7.0f + box(56, 8); }
        s.n = 5;
        u32 src = 0;
        for (u32 i = 0; i < 5; ++i) {
            u32 pick;
            if (i < base && !g.C(25)) pick = src++;
            else pick = g.P(base);
            if (i >= base && g.C(50)) pick = i - 1 < base ? i - 1 : 0;   // repetido consecutivo
            SetPt(s, i, bx[pick], by[pick]);
        }
        if (g.C(30)) { s.n = 4; }
        break;
    }
    case 5: {  // coordenadas enteras (empates con los limites de las filas)
        s.n = 3 + g.P(3);
        for (u32 i = 0; i < s.n; ++i) SetPt(s, i, ox + float(int(g.P(13))), oy + float(int(g.P(13))));
        break;
    }
    case 6: {  // trapecios: dos pares de puntos con la misma y (aristas horizontales)
        s.n = 4;
        const float ya = oy + box(40, 4), yb = ya + 0.25f * float(1 + g.P(40));
        const float xa = ox + box(24, 4);
        SetPt(s, 0, xa, ya);
        SetPt(s, 1, xa + 0.25f * float(1 + g.P(40)), ya);
        SetPt(s, 2, xa + 0.25f * float(g.P(60)), yb);
        SetPt(s, 3, xa - 0.25f * float(g.P(20)), yb);
        break;
    }
    case 7: {  // patron de WaterLevel: rectangulo girado de 5 puntos con el ultimo igual al primero
        s.n = 5;
        const float cx = ox + 7.0f, cy = oy + 7.0f, w = 0.5f * float(1 + g.P(12)), h = 0.5f * float(1 + g.P(12));
        const float sk = 0.25f * float(g.P(12));
        SetPt(s, 0, cx - w, cy - h);
        SetPt(s, 1, cx + w + sk, cy - h);
        SetPt(s, 2, cx + w, cy + h);
        SetPt(s, 3, cx - w - sk, cy + h);
        SetPt(s, 4, cx - w, cy - h);
        break;
    }
    case 8: case 9:  // aleatorio; las celdas extra se rellenan abajo
        s.n = 3 + g.P(3);
        for (u32 i = 0; i < s.n; ++i) SetPt(s, i, ox + 7.0f + box(56, 8), oy + 7.0f + box(56, 8));
        break;
    case 10:  // gran desplazamiento, fracciones /10
        s.n = 3 + g.P(3);
        for (u32 i = 0; i < s.n; ++i) SetPt(s, i, ox + float(int(g.P(121))) / 10.0f, oy + float(int(g.P(121))) / 10.0f);
        break;
    case 11: {  // degenerado: 2 puntos, o 3 puntos alineados (diagonal o vertical)
        const u32 kind = g.P(5);
        if (kind == 0) { s.n = 2; SetPt(s, 0, ox + box(40, 4), oy + box(40, 4)); SetPt(s, 1, ox + box(40, 4), oy + box(40, 4)); }
        else if (kind == 1) { s.n = 2; const float x = ox + box(40, 4), y = oy + box(40, 4); SetPt(s, 0, x, y); SetPt(s, 1, x, y); }
        else {
            s.n = 3 + g.P(3);
            const float dx = kind == 2 ? 0.0f : 0.5f * float(1 + g.P(4)), dy = 0.5f * float(1 + g.P(4));
            const float x = ox + box(20, 4), y = oy + box(20, 4);
            for (u32 i = 0; i < s.n; ++i) SetPt(s, i, x + dx * float(i), y + dy * float(i));
        }
        break;
    }
    case 12: {  // lamina: todos los puntos dentro de una sola fila
        s.n = 3 + g.P(3);
        const float y = oy + float(int(g.P(9))), fy = 0.0625f * float(1 + g.P(14));
        for (u32 i = 0; i < s.n; ++i) SetPt(s, i, ox + 7.0f + box(56, 8), y + fy * float(g.P(4)) / 4.0f);
        break;
    }
    default:  // coordenadas decimales con mantisa completa (/100)
        s.n = 3 + g.P(3);
        for (u32 i = 0; i < s.n; ++i) SetPt(s, i, ox + float(int(g.P(1401))) / 100.0f, oy + float(int(g.P(1401))) / 100.0f);
        break;
    }
    // relleno basura (las posiciones >= n no las lee la funcion; el banco comprueba que no se tocan)
    for (u32 i = s.n; i < kPointSlots; ++i) { s.p[2 * i] = Garbage(s.family * 977u + i, 1); s.p[2 * i + 1] = Garbage(s.family * 977u + i, 2); }
    // celdas extra
    u32 extras = 0;
    if (fam == 8) extras = 1 + g.P(4);
    else if (fam == 9) extras = 1 + g.P(4);
    else if (g.C(15)) extras = 1 + g.P(4);
    if (extras) {
        float minx = 1e9f, miny = 1e9f, maxx = -1e9f, maxy = -1e9f;
        for (u32 i = 0; i < s.n; ++i) {
            minx = std::fmin(minx, PX(s, i)); maxx = std::fmax(maxx, PX(s, i)); miny = std::fmin(miny, PY(s, i)); maxy = std::fmax(maxy, PY(s, i));
        }
        const int fx0 = int(std::floor(minx)), fx1 = int(std::floor(maxx)), fy0 = int(std::floor(miny)), fy1 = int(std::floor(maxy));
        s.pre[kCountWord] = extras;
        for (u32 e = 0; e < 4; ++e) {
            int cx, cy;
            if (fam == 9) { cx = fx1 + 1 + int(g.P(6)); cy = fy0 - int(g.P(5)); }
            else { cx = fx0 + int(g.P(u32(fx1 - fx0 + 1))); cy = fy0 + int(g.P(u32(fy1 - fy0 + 1))); }
            if (e > 0 && g.C(15)) { cx = int(s.pre[kXWord + e - 1]); cy = int(s.pre[kYWord + e - 1]); }   // repetida
            if (fam != 9 && g.C(20)) cx += int(g.P(3)) - 1;
            s.pre[kXWord + e] = u32(cx);
            s.pre[kYWord + e] = u32(cy);
        }
    }
}

inline void MakeExtra(Spec& s, Gen& g) {
    const u32 fam = s.family % kExtraFamilies;
    u32 k = 0;
    float minX, maxX, minY, maxY;
    const float ox = float(int(g.P(41)) - 20), oy = float(int(g.P(41)) - 20);
    switch (fam) {
    case 0: minX = ox; maxX = ox + float(g.P(3)); minY = oy; maxY = oy + float(g.P(3)); break;            // limites enteros
    case 1: minX = ox + 0.125f * float(g.P(8)); maxX = minX + 0.125f * float(1 + g.P(7)); minY = oy + 0.125f * float(g.P(8)); maxY = minY + 0.125f * float(1 + g.P(7)); break;
    case 2: k = 1 + g.P(3); minX = ox + 0.25f * float(g.P(4)); maxX = minX + 0.25f * float(1 + g.P(8)); minY = oy; maxY = oy + 0.5f * float(1 + g.P(2)); break;
    case 3: minX = ox + 2.0f; maxX = ox + 2.0f - 0.25f * float(g.P(8)); minY = oy; maxY = oy + float(g.P(4)); break;           // minX >= maxX
    case 4: minX = ox; maxX = ox + float(g.P(12)); minY = oy + 2.0f; maxY = oy + 2.0f - 0.25f * float(g.P(8)); break;       // minY >= maxY con filas vacias
    case 5: minX = -30.0f + 0.5f * float(g.P(8)); maxX = minX + 1.0f + 0.5f * float(g.P(2)); minY = -45.0f + 0.5f * float(g.P(8)); maxY = minY + 0.5f * float(1 + g.P(4)); break;
    case 6: k = g.P(2); minX = ox; maxX = ox + 1.0f; minY = oy; maxY = oy + float(1 + g.P(3)); break;
    default: {  // limites un ulp por encima / por debajo de un entero
        const float xi = ox + 1.0f, yi = oy + 1.0f;
        minX = g.C(50) ? std::nextafter(xi, 1e9f) : std::nextafter(xi, -1e9f); maxX = g.C(50) ? std::nextafter(xi + 1.0f, 1e9f) : std::nextafter(xi + 1.0f, -1e9f);
        minY = g.C(50) ? std::nextafter(yi, 1e9f) : std::nextafter(yi, -1e9f); maxY = g.C(50) ? std::nextafter(yi + 1.0f, 1e9f) : std::nextafter(yi + 1.0f, -1e9f);
        break;
    }
    }
    // a lo sumo 4 celdas en total (el EXE no comprueba el limite)
    auto cells = [&] {
        const long long wx = (long long)std::ceil(maxX) - (long long)std::floor(minX), wy = (long long)std::ceil(maxY) - (long long)std::floor(minY);
        return (wx > 0 && wy > 0) ? wx * wy : 0ll;
    };
    if (long long(k) + cells() > 4) { minX = ox; maxX = ox + 1.0f; minY = oy; maxY = oy + 1.0f; if (k > 3) k = 3; }
    s.pre[kCountWord] = k;
    for (u32 e = 0; e < 4; ++e) { s.pre[kXWord + e] = u32(int(g.P(30)) - 15); s.pre[kYWord + e] = u32(int(g.P(30)) - 15); }
    s.a = {FBits(minX), FBits(maxX), FBits(minY), FBits(maxY)};
}

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn;
    Gen g(n + fn * 0x20000u);
    s.family = n % (fn == kScan ? kFamilies : kExtraFamilies);
    s.stack = StackSeed(n);
    s.eax = g.C(30) ? Garbage(n, 1) : g.R();
    s.ecx = g.C(30) ? Garbage(n, 2) : g.R();
    s.edx = g.C(30) ? Garbage(n, 3) : g.R();
    // relleno previo de la region (basura que no coincide con celdas validas)
    for (u32 i = 0; i < kRegionWords - 1; ++i) s.pre[i] = 1000u + g.P(5000);
    s.pre[kCountWord] = 0;
    if (fn == kScan) MakeScan(s, g); else MakeExtra(s, g);
    return s;
}

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"n\":" + U(s.n) + ",\"p\":[";
    for (u32 i = 0; i < 2 * kPointSlots; ++i) o += (i ? "," : "") + U(s.p[i]);
    o += "],\"pre\":[";
    for (u32 i = 0; i < kRegionWords; ++i) o += (i ? "," : "") + U(s.pre[i]);
    o += "],\"a\":[" + U(s.a[0]) + "," + U(s.a[1]) + "," + U(s.a[2]) + "," + U(s.a[3]) + "],\"regs\":[" + U(s.stack) + "," + U(s.eax) + "," + U(s.ecx) + "," + U(s.edx) + "]}";
    return o;
}
}  // namespace WS64Fix
