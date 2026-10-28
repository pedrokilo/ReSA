#pragma once
// Fixtures del banco CarNodes75 (lote 75): CCarCtrl::FindNodesThisCarIsNearestTo (EXE 0x42BD20), 18432 casos. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / LiveComparison.hpp.
//
//   0 CCarCtrl::FindNodesThisCarIsNearestTo  0x42BD20  cdecl, `ret` (CVehicle*, CNodeAddress&, CNodeAddress&), sin valor de retorno   (hook CCarCtrl / FindNodesThisCarIsNearestTo, tipo S)
//
// Todos los floats viajan como patrones de 32 bits sin signo. Reparto: familia = n / 1024 (18 familias de 1024 casos).
// Mundo falso de rutas: hasta 4 "casillas" (areas) con nodos propios; los colaboradores (regiones, coordenadas de region, DistToLine y Normalise) devuelven valores programados por el caso.
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

namespace Cn75Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kFamilies = 18;
constexpr unsigned kCases = kFamilies * 1024;
constexpr unsigned kFunctions = 1;

// Arenas: guarda + ventana + guarda (la ventana es el objeto).
constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kVehWin = 0x20, kMatWin = 0x48, kOutWin = 0x8, kNodesWin = 8 * 0x1C, kTableWin = 96 * 4;
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr std::size_t kVehArena = Arena(kVehWin), kMatArena = Arena(kMatWin), kOutArena = Arena(kOutWin), kNodesArena = Arena(kNodesWin), kTableArena = Arena(kTableWin);
constexpr int kTableBias = 32;   // el puntero m_pNodeLinks[area] apunta a la entrada 32 de la arena: las bases negativas (hasta -32) siguen dentro de la ventana

constexpr u32 kPathNodesAddr = 0x96F854;     // ThePaths.m_pPathNodes[72]
constexpr u32 kNodeLinksAddr = 0x96FA94;     // ThePaths.m_pNodeLinks[72]
constexpr u32 kNumVehNodesAddr = 0x970114;   // ThePaths.m_anNumVehicleNodes[72]
constexpr u32 kPathSlots = 72;
constexpr u32 kThePaths = 0x96F050;
constexpr unsigned kSlots = 4;
constexpr unsigned kMaxNodes = 5;
constexpr unsigned kMaxTable = 64;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 75175u); }
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
    Js& Raw(const char* k, const std::string& v) { Key(k); s += v; return *this; }
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ valores notables
inline constexpr std::array<u32, 12> kNan{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFA00000u, 0x7FC12345u, 0xFFC54321u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0x7FC00001u, 0xFFBFFFFFu};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
inline constexpr u32 kInitBest = 0x47C34FF3u;   // mejor puntuacion inicial (99999.99)
template <class A> inline u32 Pick(const A& a, u32 c, u32 k) { return a[Rand(c, k) % a.size()]; }
// float uniforme en [lo, hi] (resolucion de 2^-20 del intervalo)
inline u32 Uni(u32 c, u32 k, float lo, float hi) { return B(lo + (hi - lo) * (float(Rand(c, k) & 0xFFFFFu) / 1048575.0f)); }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
inline bool IsFinite(u32 b) { return (b & 0x7F800000u) != 0x7F800000u; }

// ------------------------------------------------------------------------------------------------ el caso
struct Node {i32 x{}, y{}, z{}, base{}; u32 flags{};};   // int16 +8,+A,+C; int16 +0x10; byte +0x18
struct Slot {
    u32 area{}, loaded{}, count{}, np{};
    Node nd[kMaxNodes]{};
    u32 nt{};                                    // entradas definidas de la tabla de enlaces
    i32 tIdx[kMaxTable]{};                       // indice respecto al puntero de la tabla (puede ser negativo)
    u32 tVal[kMaxTable]{};                       // CNodeAddress (area en los 16 bits bajos, nodo en los altos)
};
constexpr unsigned kScripts = 8;
struct C {
    u32 n{}, family{}, seed{};
    u32 eax{}, edx{}, stack{};
    u32 hasMat{};
    u32 mpos[3]{}, ppos[3]{};        // posicion de la matriz (+0x30..) y compacta (+4..)
    u32 fwd[3]{};                    // matriz +0x10, +0x14, +0x18
    u32 regionX{}, regionY{};        // devueltos por FindXRegionForCoors / FindYRegionForCoors
    u32 xCoord{}, yCoord{};          // devueltos por FindXCoorsForRegion / FindYCoorsForRegion
    u32 nSlots{};
    Slot s[kSlots]{};
    u32 dist[kScripts]{};            // retornos de DistToLine (cicla)
    u32 norm[kScripts][3]{};         // salidas de Normalise (cicla)
};

inline C MakeC(unsigned n) {
    C c;
    c.n = n;
    c.family = n / 1024;
    const u32 m = n % 1024;
    const u32 fam = c.family;
    c.seed = Rand(n, 7);
    c.eax = SeedReg(n, 1); c.edx = SeedReg(n, 2); c.stack = StackSeed(n);
    c.hasMat = Rand(n, 29) % 7 != 0;
    // posiciones: multiplos de 1/8 (los nodos tambien lo son) con rango moderado
    for (int i = 0; i < 3; ++i) {
        c.mpos[i] = B(float(int(Rand(n, 40 + i) % 40000) - 20000) * 0.125f);
        c.ppos[i] = B(float(int(Rand(n, 44 + i) % 40000) - 20000) * 0.125f);
    }
    {
        const float fx = F(Uni(n, 48, -1.0f, 1.0f)), fy = F(Uni(n, 49, -1.0f, 1.0f)), fz = F(Uni(n, 50, -0.3f, 0.3f));
        c.fwd[0] = B(fx); c.fwd[1] = B(fy); c.fwd[2] = B(fz);
    }
    auto pos = [&](int i) { return F(c.hasMat ? c.mpos[i] : c.ppos[i]); };
    // region devuelta; categoria de dxr/dyr: 0 = menor de 200 (casilla anterior), 1 = entre 200 y 550 (solo la casilla), 2 = mayor de 550 (casilla siguiente)
    c.regionX = Rand(n, 52) % 8; c.regionY = Rand(n, 53) % 8;
    int catX = int(Rand(n, 54) % 3), catY = int(Rand(n, 55) % 3);
    auto setCoords = [&] {
        static constexpr float lo[3] = {20.0f, 205.0f, 555.0f}, hi[3] = {195.0f, 545.0f, 700.0f};
        const float dx = lo[catX] + (hi[catX] - lo[catX]) * (float(Rand(n, 56) & 0xFFFFu) / 65535.0f);
        const float dy = lo[catY] + (hi[catY] - lo[catY]) * (float(Rand(n, 57) & 0xFFFFu) / 65535.0f);
        c.xCoord = B(pos(0) - float(std::lround(dx)));
        c.yCoord = B(pos(1) - float(std::lround(dy)));
    };
    setCoords();
    // guiones de DistToLine y Normalise
    for (unsigned i = 0; i < kScripts; ++i) {
        c.dist[i] = Uni(n, 100 + i, 0.0f, 120.0f);
        float v[3] = {F(Uni(n, 110 + i * 3, -1.0f, 1.0f)), F(Uni(n, 111 + i * 3, -1.0f, 1.0f)), F(Uni(n, 112 + i * 3, -1.0f, 1.0f))};
        const float len = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        for (int k = 0; k < 3; ++k) c.norm[i][k] = B(len > 0.001f ? v[k] / len : (k == 0 ? 1.0f : 0.0f));
    }
    // casillas recorridas (en el orden del EXE: x exterior, y interior) para la region y categorias actuales
    auto scanned = [&](std::vector<u32>& out) {
        out.clear();
        const int rx = int(c.regionX), ry = int(c.regionY);
        const int lowX = catX == 0 ? rx - 1 : rx, highX = catX == 2 ? rx + 1 : rx, lowY = catY == 0 ? ry - 1 : ry, highY = catY == 2 ? ry + 1 : ry;
        const int minX = std::max(lowX, 0), minY = std::max(lowY, 0), maxX = std::min(highX, 7), maxY = std::min(highY, 7);
        for (int x = minX; x <= maxX; ++x)
            for (int y = minY; y <= maxY; ++y) out.push_back(u32(x + y * 8));
    };
    // parametros por familia (antes de montar las casillas)
    u32 linkMax = 3, countMax = 4, farNodes = 6;   // enlaces por nodo (maximo), nodos por casilla (maximo), 1 de cada `farNodes` nodos lejos
    bool allFar = false, specialPos = false;
    switch (fam) {
    case 0: break;
    case 1:   // ventanas: categorias, bordes de 200 y 550 (+-1 ulp), NaN, infinitos, desnormales
        c.regionX = 1 + Rand(n, 52) % 6; c.regionY = 1 + Rand(n, 53) % 6;
        for (int i = 0; i < 3; ++i) { c.mpos[i] = B(float(int(Rand(n, 40 + i) % 4000) - 2000)); c.ppos[i] = B(float(int(Rand(n, 44 + i) % 4000) - 2000)); }
        {
            static constexpr std::array<float, 8> edge{200.0f, 550.0f, 199.0f, 201.0f, 549.0f, 551.0f, 0.0f, -50.0f};
            auto coord = [&](int axis, u32 k) {
                const u32 r = Rand(n, k) % 10;
                float d;
                u32 bits;
                if (r < 4) { d = edge[Rand(n, k + 1) % edge.size()]; bits = B(pos(axis) - d); }
                else if (r < 6) { d = edge[Rand(n, k + 1) % 4]; bits = B(pos(axis) - d); bits += Rand(n, k + 2) % 3; bits -= 1; }   // +-1 ulp alrededor del borde
                else if (r == 6) bits = Pick(kNan, n, k + 3);
                else if (r == 7) bits = Pick(kInf, n, k + 3);
                else if (r == 8) bits = Pick(kTiny, n, k + 3);
                else bits = B(pos(axis) - F(Uni(n, k + 4, -100.0f, 900.0f)));
                return bits;
            };
            c.xCoord = coord(0, 60); c.yCoord = coord(1, 70);
            // casillas vecinas posibles: se cargan todas las del entorno
            catX = 0; catY = 0;
        }
        break;
    case 2:   // clamps de la region: negativos, 0, 7, 8, enormes (incluida la vuelta de 32 bits)
        {
            static constexpr std::array<u32, 14> rv{0u, 7u, 8u, 9u, 100u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x80000001u, 0x7FFFFFFFu, 0x7FFFFFFEu, 6u, 1u, 0xFFFFFFFBu};
            c.regionX = Pick(rv, n, 60); c.regionY = Pick(rv, n, 61);
            catX = int(Rand(n, 62) % 3); catY = int(Rand(n, 63) % 3);
            setCoords();
        }
        break;
    case 3:   // rangos vacios: ninguna casilla (x vacio, y vacio, ambos)
        switch (m % 3) {
        case 0: c.regionX = 8 + Rand(n, 60) % 5; c.regionY = Rand(n, 61) % 8; catX = 1; break;                                  // minX > 7
        case 1: c.regionX = Rand(n, 60) % 8; c.regionY = u32(-3 - int(Rand(n, 61) % 5)); catY = int(Rand(n, 62) % 2); break;     // highY < 0 : bucle x sin filas
        default: c.regionX = u32(-4 - int(Rand(n, 60) % 5)); c.regionY = Rand(n, 61) % 8; catX = int(Rand(n, 62) % 2); break;   // highX < 0
        }
        setCoords();
        break;
    case 4: break;   // cargadas / contadores (abajo)
    case 5: break;   // frontera de distancia 150 (abajo)
    case 6: linkMax = 15; countMax = 2; break;   // numero de enlaces hasta 15
    case 7: break;
    case 8: break;   // empates de puntuacion (abajo)
    case 9: break;   // salidas de Normalise especiales (abajo)
    case 10: break;  // retornos de DistToLine especiales (abajo)
    case 11:         // posicion especial del vehiculo (NaN, infinito, desnormal, ceros, enorme)
        specialPos = true;
        break;
    case 12: break;  // adelante especial (abajo)
    case 13: break;  // puntuaciones en torno al mejor inicial (abajo)
    case 14:         // sin matriz: ningun enlace cumple (nodos lejos)
        c.hasMat = 0; allFar = true;
        break;
    case 15: linkMax = 7; countMax = 5; break;   // muchas casillas, nodos y enlaces
    case 16:         // todos los indices de casilla: region de 0 a 7 en ambos ejes y todas las categorias
        c.regionX = m % 8; c.regionY = (m / 8) % 8; catX = int((m / 64) % 3); catY = int((m / 192) % 3);
        setCoords();
        break;
    default: // 17 mezcla
        c.regionX = Rand(n, 52) % 10; c.regionY = Rand(n, 53) % 10;
        break;
    }
    if (specialPos) {
        for (int i = 0; i < 3; ++i) {
            const u32 r = Rand(n, 80 + i) % 5;
            const u32 v = r == 0 ? Pick(kNan, n, 83 + i) : r == 1 ? Pick(kInf, n, 86 + i) : r == 2 ? Pick(kTiny, n, 89 + i) : r == 3 ? Pick(kBig, n, 92 + i) : B(float(int(Rand(n, 95 + i) % 100) - 50) * 0.125f);
            (c.hasMat ? c.mpos : c.ppos)[i] = v;
            if (Rand(n, 98 + i) % 3 == 0) (c.hasMat ? c.ppos : c.mpos)[i] = v;
        }
        if (Rand(n, 99) % 2) c.hasMat = Rand(n, 100) % 2;
    }
    // casillas
    std::vector<u32> sc;
    scanned(sc);
    c.nSlots = kSlots;
    u32 areas[kSlots];
    for (unsigned i = 0; i < kSlots; ++i) {
        if (i < sc.size()) areas[i] = sc[i];
        else { u32 a, attempt = 0; do { a = Rand(n, 400 + i * 64 + attempt++) % 64; } while (std::find(areas, areas + i, a) != areas + i); areas[i] = a; }
    }
    const u32 unloadedId = 64u + Rand(n, 124) % 8;
    if (!c.hasMat) allFar = true;   // sin matriz el EXE se cae en cuanto un enlace cumple (lee la matriz): ninguno puede cumplir
    for (unsigned i = 0; i < kSlots; ++i) {
        Slot& sl = c.s[i];
        sl.area = areas[i];
        sl.loaded = Rand(n, 130 + i) % 9 != 0 ? 1u : 0u;
        sl.count = 1 + Rand(n, 134 + i) % countMax;
        sl.np = std::min<u32>(kMaxNodes, sl.count + (Rand(n, 138 + i) % 2));
    }
    for (unsigned i = 0; i < kSlots; ++i) {
        Slot& sl = c.s[i];
        sl.nt = 0;
        i32 cursor = i32(Rand(n, 142 + i) % 3);   // siguiente entrada libre de la tabla
        const float px0 = pos(0), py0 = pos(1), pz0 = pos(2);
        const bool okPos = IsFinite(B(px0)) && IsFinite(B(py0)) && IsFinite(B(pz0)) && std::fabs(px0) < 4000.0f && std::fabs(py0) < 4000.0f && std::fabs(pz0) < 4000.0f;
        for (u32 k = 0; k < sl.np; ++k) {
            Node& nd = sl.nd[k];
            const u32 kk = i * 8 + k;
            const bool isFar = allFar || Rand(n, 150 + kk) % farNodes == 0;
            const float rng = isFar ? 900.0f : 120.0f;
            const float px = okPos ? px0 : 0.0f, py = okPos ? py0 : 0.0f, pz = okPos ? pz0 : 0.0f;
            float dx = F(Uni(n, 200 + kk, -rng, rng)), dy = F(Uni(n, 201 + kk, -rng, rng)), dz = F(Uni(n, 202 + kk, -30.0f, 30.0f));
            if (isFar) { dx = (Rand(n, 203 + kk) % 2 ? 1.0f : -1.0f) * (400.0f + std::fabs(dx)); }
            if (fam == 5 && !allFar) {   // en torno al borde de 150: distancia exacta 150, 150 +- 0.125 y vecinos
                static constexpr std::array<float, 9> d{150.0f, 149.875f, 150.125f, 149.0f, 151.0f, 0.0f, 100.0f, 148.5f, 150.25f};
                const float want = d[Rand(n, 204 + kk) % d.size()];
                dx = want * (Rand(n, 205 + kk) % 2 ? 1.0f : -1.0f); dy = 0.0f; dz = 0.0f;
                if (Rand(n, 206 + kk) % 3 == 0) { dx = 0.0f; dy = want; }
                if (Rand(n, 207 + kk) % 4 == 0) { dx = want * 0.6f; dy = want * 0.8f; dz = 0.0f; }
                if (Rand(n, 208 + kk) % 4 == 0) { dx = 0.0f; dy = 0.0f; dz = want; }
            }
            nd.x = std::clamp(i32(std::lround((px + dx) * 8.0f)), -32768, 32767);
            nd.y = std::clamp(i32(std::lround((py + dy) * 8.0f)), -32768, 32767);
            nd.z = std::clamp(i32(std::lround((pz + dz) * 8.0f)), -32768, 32767);
            u32 links = 1 + Rand(n, 160 + kk) % linkMax;
            if (fam == 6 || fam == 15) links = Rand(n, 160 + kk) % (linkMax + 1);
            nd.flags = (Rand(n, 164 + kk) & 0xF0u) | links;
            if (fam == 6 && Rand(n, 165 + kk) % 3 == 0) nd.flags = (Rand(n, 166 + kk) & 0xFFu);   // nibble alto basura y numero de enlaces cualquiera (<= 15)
            const u32 L = nd.flags & 0xFu;
            nd.base = (Rand(n, 168 + kk) % 4 == 0) ? -i32(Rand(n, 169 + kk) % 4) - cursor : cursor;   // algunas bases negativas (con el sesgo de la tabla siguen en la ventana)
            for (u32 j = 0; j < L; ++j) {
                if (sl.nt >= kMaxTable - 1) break;
                const u32 pickSlot = Rand(n, 170 + kk * 16 + j) % (kSlots + 1);
                const u32 area = pickSlot < kSlots ? areas[pickSlot] : unloadedId;
                const u32 node = Rand(n, 171 + kk * 16 + j) % (pickSlot < kSlots ? c.s[pickSlot].np : 5u);   // solo nodos definidos del destino
                sl.tIdx[sl.nt] = nd.base + i32(j);
                sl.tVal[sl.nt] = area | (node << 16);
                ++sl.nt;
            }
            cursor += i32(L);
            if (cursor > 24) cursor = 24;
        }
        // casos de cargada / contadores
        if (fam == 4) {
            switch ((m + i) % 6) {
            case 0: sl.loaded = 0; break;                       // puntero nulo con contador positivo
            case 1: sl.count = 0; break;                        // cargada con contador 0
            case 2: sl.count = 0xFFFFFFFFu; break;              // contador negativo (comparacion con signo)
            case 3: sl.count = 0x80000000u; break;
            default: break;
            }
        }
    }
    // retornos especiales de los colaboradores
    if (fam == 8) {   // empates: todos los retornos iguales y normales iguales
        for (unsigned i = 1; i < kScripts; ++i) { c.dist[i] = c.dist[0]; for (int k = 0; k < 3; ++k) c.norm[i][k] = c.norm[0][k]; }
        if (m % 2) for (unsigned i = 0; i < kScripts; ++i) c.dist[i] = B(F(c.dist[0]) + float(i % 2) * 5.0f);
    }
    if (fam == 9) {   // Normalise devuelve vectores raros
        for (unsigned i = 0; i < kScripts; ++i) {
            const u32 r = Rand(n, 300 + i) % 6;
            for (int k = 0; k < 3; ++k) {
                if (r == 0) c.norm[i][k] = Pick(kNan, n, 310 + i * 3 + k);
                else if (r == 1) c.norm[i][k] = Pick(kInf, n, 310 + i * 3 + k);
                else if (r == 2) c.norm[i][k] = Pick(kTiny, n, 310 + i * 3 + k);
                else if (r == 3) c.norm[i][k] = Pick(kBig, n, 310 + i * 3 + k);
                else if (r == 4) c.norm[i][k] = 0;
            }
        }
    }
    if (fam == 10) {  // DistToLine devuelve valores raros
        for (unsigned i = 0; i < kScripts; ++i) {
            const u32 r = Rand(n, 320 + i) % 8;
            c.dist[i] = r == 0 ? Pick(kNan, n, 330 + i) : r == 1 ? Pick(kInf, n, 330 + i) : r == 2 ? Pick(kTiny, n, 330 + i) : r == 3 ? Pick(kBig, n, 330 + i) : r == 4 ? B(-F(c.dist[i])) : c.dist[i];
        }
    }
    if (fam == 12) {  // adelante especial
        for (int k = 0; k < 3; ++k) {
            const u32 r = Rand(n, 340 + k) % 6;
            c.fwd[k] = r == 0 ? Pick(kNan, n, 350 + k) : r == 1 ? Pick(kInf, n, 350 + k) : r == 2 ? Pick(kTiny, n, 350 + k) : r == 3 ? Pick(kBig, n, 350 + k) : c.fwd[k];
        }
    }
    if (fam == 13) {  // puntuaciones en torno al mejor inicial: dist + (1 - dot) * 5 ~ 99999.99 (unos ulp arriba y abajo)
        for (unsigned i = 0; i < kScripts; ++i) {
            c.norm[i][0] = 0x3F800000u; c.norm[i][1] = 0; c.norm[i][2] = 0;
            c.fwd[0] = 0x3F800000u; c.fwd[1] = 0; c.fwd[2] = 0;
            c.dist[i] = kInitBest + (Rand(n, 360 + i) % 5) - 2;   // dot = 1 -> score = dist
            if (Rand(n, 370 + i) % 4 == 0) c.dist[i] = kInitBest + (Rand(n, 371 + i) % 3) - 1;
        }
    }
    return c;
}

inline std::string JsonC(const C& c) {
    Js j;
    j.U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack", c.stack).U("has_mat", c.hasMat);
    j.U("mpos_x", c.mpos[0]).U("mpos_y", c.mpos[1]).U("mpos_z", c.mpos[2]).U("ppos_x", c.ppos[0]).U("ppos_y", c.ppos[1]).U("ppos_z", c.ppos[2]);
    j.U("fwd_x", c.fwd[0]).U("fwd_y", c.fwd[1]).U("fwd_z", c.fwd[2]);
    j.U("region_x", c.regionX).U("region_y", c.regionY).U("xcoord", c.xCoord).U("ycoord", c.yCoord);
    std::string d = "[";
    for (unsigned i = 0; i < kScripts; ++i) d += (i ? "," : "") + std::to_string(c.dist[i]);
    j.Raw("dist", d + "]");
    std::string nm = "[";
    for (unsigned i = 0; i < kScripts; ++i) nm += (i ? ",[" : "[") + std::to_string(c.norm[i][0]) + ',' + std::to_string(c.norm[i][1]) + ',' + std::to_string(c.norm[i][2]) + ']';
    j.Raw("norm", nm + "]");
    std::string sl = "[";
    for (unsigned i = 0; i < c.nSlots; ++i) {
        const Slot& s = c.s[i];
        if (i) sl += ',';
        sl += '[' + std::to_string(s.area) + ',' + std::to_string(s.loaded) + ',' + std::to_string(s.count) + ',' + std::to_string(s.np) + ",[";
        for (u32 k = 0; k < s.np; ++k) {
            const Node& nd = s.nd[k];
            sl += (k ? ",[" : "[") + std::to_string(nd.x) + ',' + std::to_string(nd.y) + ',' + std::to_string(nd.z) + ',' + std::to_string(nd.base) + ',' + std::to_string(nd.flags) + ']';
        }
        sl += "],[";
        for (u32 k = 0; k < s.nt; ++k) sl += (k ? ",[" : "[") + std::to_string(s.tIdx[k]) + ',' + std::to_string(s.tVal[k]) + ']';
        sl += "]]";
    }
    j.Raw("slots", sl + "]");
    return j.End();
}
}
