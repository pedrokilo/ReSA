#pragma once
// Fixtures del banco Fn208A (lote 208A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado y de los llamadores, ver README.md y fixture_oracle.py). 15 reconstruidas de 21 direcciones (6 saltadas, ver README.md).
//   idx  direccion  nombre real                                                                forma
//    0   0x453F40   CPhoneInfo::PhoneAnimFinishCallback                                          cdecl(asoc, ped) `ret`: flags |= 4, delta = -1000, GetPad(0), bit 6 del pad, bUpdateAnimHeading, SetPedState(IDLE)
//    1   0x4541E0   CPhoneInfo constructor                                                       thiscall() `ret`: devuelve this (no escribe)
//    2   0x454920   CPickup::IsWeaponSlotExchangeable                                            cdecl(ranura) `ret`: AL = ranura == 3 || 4 || 5
//    3   0x55EB90   SurfaceInfos_c::LoadSurfaceInfos                                             thiscall() `ret`: lee data\surfinfo.dat linea a linea
//    4   0x55F2B0   SurfaceInfos_c::LoadSurfaceAudioInfos                                        thiscall() `ret`: lee data\surfaud.dat
//    5   0x72D850   CZoneDef::IsPointWithin                                                      thiscall(x, y, z) `ret 0Ch`: AL, x87 a 24 bits
//    6   0x72E3A0   RpClumpGtaStreamReadAtOnce                                                   cdecl(stream) `ret`: lectura de un clump en una fase
//    7   0x72E870   CMemoryHeap::JoinAdjacentFreeBlocks                                           thiscall(bloque) `ret 4`
//    8   0x72EC60   CMemoryHeap::Shutdown                                                        thiscall() `ret`: free(m_FirstBlock); m_FirstBlock = 0
//    9   0x72EC80   CMemoryHeap::FillInBlockData                                                 thiscall(desc, sig, tam) `ret 0Ch`
//   10   0x72ED80   CMemoryHeap::Malloc                                                          thiscall(tam) `ret 4`
//   11   0x72F180   CMemoryHeap::Init                                                            thiscall(mem, tam, initSizes) `ret 0Ch`
//   12   0x72F3B0   CMemoryMgr::Init                                                             cdecl() `ret`
//   13   0x4F3940   UserTracksUninitCopy (std::uninitialized_copy de punteros)                   cdecl(primero, fin, destino, +2) `ret`
//   14   0x4F39C0   UserTracksLegacyString::Reallocate (std::string::_Copy)                      thiscall(tamano, longitud) `ret 8`, con SEH
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

namespace F208AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using i16 = std::int16_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 15;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 208001u); }
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
// FNV-1a de 32 bits (cadenas e identificadores del banco)
inline u32 Fnv(const void* p, std::size_t n) { u32 h = 2166136261u; for (std::size_t i = 0; i < n; ++i) { h ^= static_cast<const u8*>(p)[i]; h *= 16777619u; } return h; }

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
    std::array<u32, 48> a{};
    std::vector<u8> s;          // texto de los ficheros de superficies (lineas terminadas en NUL)
    std::vector<u32> v;         // guiones de los falsos de la lectura de clump
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
// Un dword "interesante" para punteros crudos y argumentos
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline float F(u32 bits) { return std::bit_cast<float>(bits); }
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline u32 NextUp(u32 bits) { return (bits & 0x7FFFFFFFu) == 0 ? 1u : ((bits & 0x80000000u) ? bits - 1u : bits + 1u); }
inline u32 NextDown(u32 bits) { return (bits & 0x7FFFFFFFu) == 0 ? 0x80000001u : ((bits & 0x80000000u) ? bits + 1u : bits - 1u); }

// Clases de funcion (forma)
enum Kind : u32 { kPhoneCb, kPhoneCtor, kSlot, kSurfInfo, kSurfAud, kPoint, kClump, kJoin, kShut, kFill, kMalloc, kInit, kMgrInit, kUCopy, kRealloc };
inline constexpr std::array<Kind, kFunctions> kKind{kPhoneCb, kPhoneCtor, kSlot, kSurfInfo, kSurfAud, kPoint, kClump, kJoin, kShut, kFill, kMalloc, kInit, kMgrInit, kUCopy, kRealloc};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x453F40, 0x4541E0, 0x454920, 0x55EB90, 0x55F2B0, 0x72D850, 0x72E3A0, 0x72E870, 0x72EC60, 0x72EC80, 0x72ED80, 0x72F180, 0x72F3B0, 0x4F3940, 0x4F39C0};
// Bytes que cada funcion toca a partir de su puntero `this` (o del primer objeto)
inline constexpr std::array<u32, kFunctions> kSpan{0x40, 0xA30, 4, 0x9C0, 0x9C0, 0x10, 4, 4, 0x48, 0x48, 0x48, 0x48, 4, 4, 0x1C};
// Memoria global del juego tocada por el banco
inline constexpr u32 kMarker = 0xA9B0B0;   // byte que la prueba negativa (mutate.py) lee en CMemoryMgr::Init

// Tamanos de los guiones de la lectura de clump (kClump)
constexpr unsigned kFc = 32, kRd = 8, kGeo = 24, kAtoms = 8, kGa = 32, kFa = 16;
// v[]: 0..31 fcVal, 32..63 fcLen, 64..71 rdMode, 72..103 rdPay (8 x 4), 104..127 geo, 128 atomNull, 129 clumpNull, 130 mallocNull, 131 flRet, 132 flMode, 133 flNum, 134..165 ga, 166..181 fa, 182..189 atomFlags0, 190 clumpParent0
constexpr unsigned kVFcVal = 0, kVFcLen = 32, kVRdMode = 64, kVRdPay = 72, kVGeo = 104, kVAtomNull = 128, kVClumpNull = 129, kVMallocNull = 130, kVFlRet = 131, kVFlMode = 132, kVFlNum = 133, kVGa = 134, kVFa = 166,
                  kVAtomFlags = 182, kVClumpParent = 190, kVSize = 191;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o del primer objeto) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
inline void MakeAll(C& c, unsigned n);

// ---- texto de los ficheros de superficies
inline std::string SurfToken(Gen& g, std::initializer_list<const char*> good, u32 pctGood, u32 maxLen) {
    if (g.P(pctGood)) return *(good.begin() + g.R(u32(good.size())));
    static const char* const junk[] = {"X", "rubber", "RUBBERS", "HAR", "Default", "sand", "ROAD_", "NONEx", "SPARKSS", "WOODEN", "DUS", "A1", "Z9Z9"};
    if (g.P(40)) return junk[g.R(13)];
    std::string t; const u32 len = 1 + g.R(maxLen);
    for (u32 i = 0; i < len; ++i) t += char('A' + g.R(26));
    return t;
}
inline std::string SurfFloat(Gen& g, bool wet) {
    static const char* const common[] = {"0", "1", "0.5", "1.0", "12.7", "12.8", "-0.1", "25.5", "-12.8", "13.0", "0.05", "100", "-100", "1e2", "3.14159", "0.000001", "0.99", "1.27", "1.28", "-0.01", "2.55", "0.9",
                                         "0.7", "0.75", "-1", "-0.5", "9.99", "127", "128", "255", ".5", "+1.5", "1e10", "3e38", "-3e38", "1e30", "5e-3", "12.75", "1.275", "0.3", "0.35"};
    if (g.P(55)) return common[g.R(41)];
    char buf[48];
    const double scale = wet ? 2.0 : 20.0;
    std::snprintf(buf, sizeof buf, "%.*f", int(g.R(4)), (double(g.R(2001)) / 1000.0 - 0.3) * scale);
    return buf;
}
inline std::string SurfInt(Gen& g) {
    static const char* const vals[] = {"0", "1", "1", "0", "2", "3", "-1", "255", "256", "0", "1", "+1", "7", "-2", "65537", "2147483647", "-2147483647", "0", "1", "1"};
    return vals[g.R(20)];
}
inline std::string Sep(Gen& g) { return g.P(80) ? " " : (g.P(50) ? "\t" : "  "); }
inline void AppendLine(std::vector<u8>& s, const std::string& line) { s.insert(s.end(), line.begin(), line.end()); s.push_back(0); }
// Una linea de surfinfo.dat (36 campos) o de surfaud.dat (nombre y 9 enteros)
inline std::string DataLine(Gen& g, bool audio, std::string& nameOut) {
    std::string line;
    if (g.P(10)) line += g.P(50) ? " " : "\t";
    std::string name = SurfToken(g, {"DEFAULT", "TARMAC", "GRASS_SHORT", "DIRT_DRY", "SAND_DEEP", "WATER_SHALLOW", "WOOD_CRATES", "METAL_SOLID", "GLASS_WINDOWS_WEAK", "GRAVEL_SMALL", "TILE", "LONGGRASS"}, 75, 40);
    nameOut = name;
    line += name;
    if (audio) {
        for (int i = 0; i < 9; ++i) { line += Sep(g); line += SurfInt(g); }
    } else {
        line += Sep(g); line += SurfToken(g, {"RUBBER", "HARD", "ROAD", "LOOSE", "SAND", "WET"}, 75, 20);
        line += Sep(g); line += SurfFloat(g, false);
        line += Sep(g); line += SurfFloat(g, true);
        line += Sep(g); line += SurfToken(g, {"DEFAULT", "SANDY", "MUDDY"}, 75, 20);
        line += Sep(g); line += SurfToken(g, {"NONE", "SPARKS"}, 80, 20);
        for (int i = 0; i < 29; ++i) { line += Sep(g); line += SurfInt(g); }
        line += Sep(g); line += SurfToken(g, {"NONE", "SPARKS", "SAND", "WOOD", "DUST"}, 75, 20);
    }
    if (g.P(8)) { line += Sep(g); line += "extra"; }
    if (g.P(5)) line += " ";
    return line;
}
inline void MakeSurf(C& c, Gen& g, bool audio) {
    auto& a = c.a;
    a[4] = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 4u, 5u, 6u});             // lineas leidas antes del final
    a[20] = g.Pk({0u, 1u, 0xDEADu, g.W() | 1u, 0xFFFFFFFFu});        // manejador que devuelve OpenFile
    u32 dataLines = 0;
    for (u32 i = 0; i < a[4]; ++i) {
        const u32 r = g.R(100);
        if (r < 8) AppendLine(c.s, "");
        else if (r < 16) AppendLine(c.s, g.P(50) ? "#" : "# comentario de la linea");
        else if (r < 20) { std::string nm; AppendLine(c.s, "#" + DataLine(g, audio, nm)); }
        else {
            std::string nm; AppendLine(c.s, DataLine(g, audio, nm));
            ++dataLines;
        }
    }
    for (u32 i = 0; i < 6; ++i) {
        const u32 pick = g.R(100);
        a[6 + i] = pick < 70 ? g.R(40) : (pick < 85 ? g.R(195) : (pick < 92 ? 194u : (pick < 97 ? 0u : 195u)));
    }
    (void)dataLines;
}

inline void MakePoint(C& c, Gen& g) {
    auto& a = c.a;
    const u32 mode = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u});
    a[15] = mode;
    auto I16 = [&](i32 v) { return u32(u16(i16(v))); };
    i32 cx = i32(g.R(6001)) - 3000, cy = i32(g.R(6001)) - 3000;
    i32 v1x = i32(g.R(3001)) - 1500, v1y = i32(g.R(3001)) - 1500;
    i32 v2x = -v1y + i32(g.R(41)) - 20, v2y = v1x + i32(g.R(41)) - 20;
    i32 minZ = i32(g.R(401)) - 200, maxZ = minZ + 1 + i32(g.R(300));
    if (g.P(20)) { v2x = i32(g.R(3001)) - 1500; v2y = i32(g.R(3001)) - 1500; }
    if (g.P(5)) std::swap(minZ, maxZ);
    float x, y, z;
    const double s = double(g.R(1001)) / 1000.0, t = double(g.R(1001)) / 1000.0;
    x = float(cx + s * v1x + t * v2x); y = float(cy + s * v1y + t * v2y); z = float(minZ + (maxZ - minZ) * (double(g.R(1001)) / 1000.0));
    switch (mode) {
    case 0: break;
    case 1: {   // bordes exactos de las dos proyecciones: s, t en {0, 1}
        const i32 si = i32(g.R(2)), ti = i32(g.R(2));
        x = float(cx + si * v1x + ti * v2x); y = float(cy + si * v1y + ti * v2y);
        const u32 tweak = g.R(6);
        u32 bx = B(x), by = B(y);
        if (tweak == 1) bx = NextUp(bx); if (tweak == 2) bx = NextDown(bx); if (tweak == 3) by = NextUp(by); if (tweak == 4) by = NextDown(by);
        x = F(bx); y = F(by);
        break;
    }
    case 2: {   // fronteras de Z
        const u32 pick = g.R(6);
        u32 zb = B(float(pick < 3 ? minZ : maxZ));
        if (pick % 3 == 1) zb = NextUp(zb); if (pick % 3 == 2) zb = NextDown(zb);
        z = F(zb);
        break;
    }
    case 3: x = F(FloatBits(g)); y = F(FloatBits(g)); z = F(FloatBits(g)); if (g.P(50)) z = float(minZ + 1); break;
    case 4: {   // coordenadas enormes: los productos con enteros de 16 bits se salen del rango del float pero no de la x87
        const float h = (g.P(50) ? 1.0f : -1.0f) * F(0x71000000u + g.R(0x0D00000u));
        x = h; y = g.P(50) ? h : -h;
        if (g.P(70)) { v1y = -v1x * (y == h ? 1 : -1); v2y = -v2x * (y == h ? 1 : -1); }
        z = float(minZ + (maxZ - minZ) / 2);
        break;
    }
    case 5: {   // extremos de int16 (la longitud al cuadrado se envuelve en 32 bits)
        const i32 ext[] = {-32768, 32767, -32767, 32766, 0, 1, -1, 16384, -16384};
        v1x = ext[g.R(9)]; v1y = ext[g.R(9)]; v2x = ext[g.R(9)]; v2y = ext[g.R(9)];
        cx = ext[g.R(9)]; cy = ext[g.R(9)];
        x = float(cx + v1x * s + v2x * t); y = float(cy + v1y * s + v2y * t);
        break;
    }
    default:    // coordenadas grandes donde el redondeo a 24 bits importa
        x = float(double(g.R(10000000)) + double(g.R(1024)) / 1024.0); y = float(double(g.R(10000000)) + double(g.R(1024)) / 1024.0);
        if (g.P(50)) { x = -x; y = -y; }
        cx = i32(g.R(6001)) - 3000; cy = i32(g.R(6001)) - 3000;
        break;
    }
    a[4] = I16(cx); a[5] = I16(cy); a[6] = I16(v1x); a[7] = I16(v1y); a[8] = I16(v2x); a[9] = I16(v2y); a[10] = I16(minZ); a[11] = I16(maxZ);
    a[12] = B(x); a[13] = B(y); a[14] = B(z);
    for (u32 i = 4; i <= 11; ++i) a[i] |= g.W() & 0xFFFF0000u;   // los 16 bits altos de cada dato son ruido (la funcion lee words)
}

inline void MakeClump(C& c, Gen& g) {
    const u32 f = c.family;
    auto& v = c.v; v.assign(kVSize, 0u);
    auto OkVal = [&] { return g.Pk({1u, 1u, 1u, 1u, 0x100u, 0x80000000u, 0xFFFFFFFFu, 2u, g.W() | 1u}); };
    auto NonZero = [&] { const u32 w = g.W(); return w ? w : 1u; };
    u32 natoms = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 4u, 5u});
    u32 count = g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 6u, 8u, 10u, 12u, 20u, g.R(25)});
    if (f == 0) natoms = 0;
    if (f == 1) count = 0;
    if (f == 2) { count = g.Pk({0xFFFFFFFFu, 0x80000000u, 0xFFFFFF00u, 0xFFFFFFFEu}); }
    if (f == 3) count = g.Pk({33u, 100u, 0x1000u, 0x40000000u, 0x7FFFFFFFu});
    if (f == 15) { natoms = 1 + g.R(5); count = 1 + g.R(12); }
    if (f == 14) natoms = g.Pk({0xFFFFFFFFu, 0x80000000u, 0xFFFFFF00u, 9u, 10u});
    if (i32(count) < 0) natoms = 0;   // con una lista de geometrias de contador negativo el EXE desreferencia un vector nulo en el primer atomico (fuera de definicion)
    const i32 sc = i32(count);
    // estado inicial de los vectores
    for (u32 i = 0; i < kGa; ++i) v[kVGa + i] = g.P(65) ? NonZero() : 0u;
    for (u32 i = 0; i < kFa; ++i) v[kVFa + i] = NonZero();
    for (u32 i = 0; i < kAtoms; ++i) v[kVAtomFlags + i] = g.R(256);
    v[kVClumpParent] = g.W();
    v[kVFlRet] = g.Pk({1u, 1u, 1u, 0xC87AF4u, 0xFFFFFFFFu, g.W() | 1u});
    v[kVFlMode] = 0;
    v[kVFlNum] = g.Pk({0u, 1u, 1u, 2u, 3u, 8u, 0xFFFFFFFFu, g.R(20)});
    if (f == 13) v[kVFlNum] = 0;
    // script nominal
    struct Pt { u32 kind, idx; };   // 0 fc (fcVal=0), 1 geo (geo=0), 2 rd mode, 3 atom create null, 4 clump null, 5 malloc null, 6 flread fail
    std::vector<Pt> pts;
    u32 kfc = 0, kgeo = 0;
    auto Fc = [&](u32 len) { if (kfc < kFc) { v[kVFcVal + kfc] = OkVal(); v[kVFcLen + kfc] = len; pts.push_back({0, kfc}); ++kfc; } };
    Fc(g.Pk({12u, 12u, 0x1Cu, g.W()}));                       // 0: STRUCT del clump (longitud no usada)
    v[kVRdPay + 0] = natoms; v[kVRdPay + 1] = g.R(3); v[kVRdPay + 2] = g.R(3); v[kVRdPay + 3] = g.W();
    pts.push_back({2, 0});
    pts.push_back({4, 0});
    Fc(0);                                                    // 1: FRAMELIST
    pts.push_back({6, 0});
    Fc(0);                                                    // 2: GEOMETRYLIST
    Fc(g.Pk({4u, 4u, g.W()}));                                // 3: STRUCT de la lista de geometrias
    v[kVRdPay + 4] = count; v[kVRdPay + 5] = g.W(); v[kVRdPay + 6] = g.W(); v[kVRdPay + 7] = g.W();
    pts.push_back({2, 1});
    if (sc > 0) pts.push_back({5, 0});
    for (i32 k = 0; k < sc && k < i32(kGeo); ++k) { Fc(0); v[kVGeo + kgeo] = NonZero(); pts.push_back({1, kgeo}); ++kgeo; }
    for (i32 k = 0; k < i32(natoms) && k < i32(kAtoms); ++k) {
        Fc(0);                                                // ATOMIC
        Fc(16);                                               // STRUCT del atomico
        const u32 rd = 2 + k;
        if (rd < kRd) { v[kVRdPay + 4 * rd + 0] = g.R(kFa); v[kVRdPay + 4 * rd + 1] = g.R(kGa); v[kVRdPay + 4 * rd + 2] = g.W(); v[kVRdPay + 4 * rd + 3] = g.W(); pts.push_back({2, rd}); }
        pts.push_back({3, u32(k)});
        if (count <= 0 || sc <= 0) { Fc(0); if (kgeo < kGeo) { v[kVGeo + kgeo] = NonZero(); pts.push_back({1, kgeo}); ++kgeo; } }
    }
    const bool wantFault = (f >= 4 && f <= 12) || (f != 15 && g.P(8));
    if (wantFault && !pts.empty()) {
        Pt p = pts[g.R(u32(pts.size()))];
        if (f == 11) { for (const auto& q : pts) if (q.kind == 3) { p = q; break; } }
        if (f == 10) { for (const auto& q : pts) if (q.kind == 5) { p = q; break; } }
        if (f == 9) p = {4, 0};
        if (f == 8) { for (const auto& q : pts) if (q.kind == 6) { p = q; break; } }
        switch (p.kind) {
        case 0: v[kVFcVal + p.idx] = 0; break;
        case 1: v[kVGeo + p.idx] = 0; break;
        case 2: v[kVRdMode + p.idx] = g.Pk({1u, 2u, 3u, 4u}); break;
        case 3: v[kVAtomNull] |= 1u << p.idx; break;
        case 4: v[kVClumpNull] = 1; break;
        case 5: v[kVMallocNull] = 1; break;
        default: v[kVFlRet] = 0; break;
        }
    }
    // el vector de frames solo se desreferencia con exito; un modo distinto de 0 (nulo o crudo) solo si falla antes
    if (g.P(10) && (v[kVFlRet] == 0 || v[kVMallocNull] || v[kVFcVal + 2] == 0 || v[kVFcVal + 3] == 0)) v[kVFlMode] = 1 + g.R(2);
}

// ---- un montaje de bloques de heap (Join, FillIn, Malloc, Init): vea LiveComparison.hpp (Setup) y fixture_oracle.py para la geometria
inline u32 BlockSize(Gen& g) { return g.P(60) ? 8u * (1u + g.R(24)) : 8u + g.R(0xB8); }
inline void MakeJoin(C& c, Gen& g) {
    auto& a = c.a;
    a[4] = 0x40u + 4u * g.R(0x40);                                  // base de P/D en B
    a[5] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});                          // desalineacion de los bloques
    a[6] = BlockSize(g); a[7] = BlockSize(g); a[8] = BlockSize(g);   // tamanos de P, D, N
    a[10] = g.Pk({0u, 1u, 1u, 1u, 2u});                             // 0 sin anterior, 1 anterior contiguo, 2 anterior en otro sitio
    a[11] = g.Pk({0u, 0u, 1u, 0x80u, 0xFFu, g.R(256)});             // byte en uso de P
    a[12] = g.Pk({0u, 0u, 1u, 0x80u, 0xFFu, g.R(256)});             // byte en uso de N
    for (u32 i = 14; i <= 17; ++i) a[i] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u});   // selectores de enlaces (P.next, P.prev, N.next, N.prev)
    a[18] = 0x40u + 4u * g.R(0x80);                                 // base de los nodos en C
    a[19] = 0x600u + 4u * g.R(0x40);                                // P "en otro sitio" en B
}
inline void MakeShut(C& c, Gen& g) {
    auto& a = c.a;
    a[4] = g.Pk({0u, 1u, 1u, 2u});          // 0 valor de a[3], 1 puntero a B, 2 nulo
    a[5] = AuxOffset(g, 0x10);
}
// Heap sintetico de FillIn/Malloc/Init: a[4] 0 sin CommonSize / 1 con, a[5] desplazamiento del array en B (>= 0x80)...
inline void MakeFill(C& c, Gen& g) {
    auto& a = c.a;
    a[4] = g.Pk({0u, 1u, 1u});                                       // tabla de tamanos comunes
    a[5] = 0x80u + 4u * g.R(0x20);                                   // array de CommonSize en B (0xF00 bytes + antes 0x3C)
    a[6] = g.W(); a[7] = g.W();                                      // m_nMemUsed, m_nMemId (dword)
    a[8] = 0x600u + 4u * g.R(0x40);                                  // descriptor en C (los de B los ocupa el array)
    const u32 size = g.Pk({0u, 0x10u, 0x20u, 0x30u, 0x40u, 0x80u, 0x100u, 0x3F0u, 0x400u, 0x410u, 0x420u, 0x13u, 8u * g.R(0x80), 4u * g.R(0xE0), g.R(0x500)});
    a[9] = size;
    a[10] = g.Pk({0x20u, 0x10u, 0x11u, 0x30u, 0x31u, 0x40u, 0x18u, 0xFFFFFFF0u, 0u, 0x28u, 0xFFFFFFE0u, 0x100u, 0x7Fu, 0x400u, 0x20u + 8u * g.R(0x40)});   // ND = desc + 0x10 + tam + a[10] - 0x20
    a[11] = g.Pk({0u, 0x10u, 0x20u, 0x3F0u, 0x400u, 0x410u, 0x420u, 0x800u, 0x7FFFFFF0u, 8u * g.R(0x80), g.R(0x500), 0x7FFFFFFFu});   // tamano del bloque unido que devuelve el falso de Join
    a[12] = g.W();                                                   // semilla del orden de las listas de tamano comun
    a[13] = g.Pk({0u, 1u, 2u, 3u, 4u});                              // nodos de la lista libre
    for (u32 i = 14; i <= 17; ++i) a[i] = g.Pk({0u, 0x10u, 0x100u, 0x400u, 0x401u, 0x7FFFFFFEu, g.R(0x500), 8u * g.R(0x100), 0x20u});
    a[18] = 0x40u + 4u * g.R(0x40);                                  // J en D
    a[19] = 0x200u + 4u * g.R(0x40);                                 // nodos de la lista libre en D
}
inline void MakeMalloc(C& c, Gen& g) {
    auto& a = c.a;
    a[4] = g.Pk({0u, 1u, 1u});
    a[5] = 0x80u + 4u * g.R(0x20);                                   // array de CommonSize en B
    a[6] = g.W(); a[7] = g.W();
    a[8] = g.Pk({0u, 1u, 8u, 0xFu, 0x10u, 0x11u, 0x20u, 0x3Fu, 0x40u, 0x100u, 0x3F0u, 0x3FFu, 0x400u, 0x401u, 0x410u, 0xFFFFFFF1u, 0xFFFFFFFFu, g.R(0x500), g.R(0x500), 0x7FFFFFF0u, 16u * g.R(0x40)});   // tamano pedido
    a[9] = g.Pk({0u, 1u, 2u, 3u});                                   // modo de las listas de tamano comun (0 vacias, 1 azar, 2 solo la pedida, 3 todas con bloque)
    a[10] = g.W();                                                   // semilla de las listas
    a[11] = 0x40u + 4u * g.R(0x40);                                  // nodos de las listas de tamano comun en C
    a[13] = g.Pk({0u, 1u, 2u, 3u, 4u});                              // nodos de la lista libre
    for (u32 i = 14; i <= 17; ++i) a[i] = g.Pk({0u, 0x10u, 0x20u, 0x100u, 0x3F0u, 0x400u, 0x410u, 0x800u, 0x7FFFFFFFu, g.R(0x500), 16u * g.R(0x80), 0x80000000u, 0xFFFFFFFFu});
    a[19] = 0x200u + 4u * g.R(0x40);                                 // nodos de la lista libre en D
}
inline void MakeInit(C& c, Gen& g) {
    auto& a = c.a;
    a[4] = 0x40u + 4u * g.R(0x40);                                   // mem en B
    a[5] = g.Pk({0x10u, 0x14u, 0x20u, 0x24u, 0x30u, 0x40u, 0x80u, 0x100u, 0x400u, 0x800u, 8u * (2u + g.R(0xF0)), 4u * (4u + g.R(0x1C0))});   // tamano
    a[6] = g.Pk({0u, 1u, 1u, 2u, 0x100u, 0xFFFFFF00u, 0x80u, 0xFFu, g.W(), 1u});   // bInitSizes (dword; la funcion lee el byte bajo)
    a[7] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W()});          // (sin uso: reservado)
    a[8] = 4u * g.R(0x40);                                           // bloque que devuelve el falso de Malloc en C
    a[9] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});                           // desalineacion de ese bloque
}
inline void MakeUCopy(C& c, Gen& g) {
    auto& a = c.a;
    a[4] = g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 5u, 8u, 16u, 31u, 32u, 40u, g.R(41)});   // elementos
    a[5] = AuxOffset(g, 0x100);                                      // origen en B
    a[6] = AuxOffset(g, 0x100);                                      // destino en C
    a[7] = g.Pk({0u, 0u, 0u, 0u, 1u});                               // destino nulo
    a[8] = Raw(g); a[9] = Raw(g);                                    // argumentos de relleno
    a[10] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});                          // desalineacion del destino
    if (a[7] && a[4] > 1) a[4] = g.Pk({0u, 1u});                     // destino nulo: el EXE avanza el destino y a la segunda vuelta ya no es nulo (escribe en 4): fuera de definicion
}
inline void MakeRealloc(C& c, Gen& g) {
    auto& a = c.a;
    const u32 kind = a[1];
    a[4] = g.Pk({0xFu, 0xFu, 0x10u, 0x1Fu, 0x20u, 0x2Fu, 0x40u, 0xFFu, 0u, 1u, 0x11u, g.R(0x80)});   // capacidad inicial
    a[5] = AuxOffset(g, 0x140);                                      // buffer externo en B
    a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});                // tamano inicial (+0x14)
    a[7] = g.Pk({0u, 1u, 2u, 0xEu, 0xFu, 0x10u, 0x11u, 0x1Fu, 0x20u, 0x21u, 0x3Fu, 0x40u, 0xFEu, 0xFFu, 0x100u, 0xFFFFFFF0u, 0xFFFFFFFEu, 0xFFFFFFFFu, 0xFFFFFFFDu, 0x7FFFFFF0u, 0x80000000u, g.R(0x100), g.R(0x100)});   // nuevo tamano
    u32 maxLen = 0x80;
    if (kind == 2 || kind == 3) maxLen = 0x20;
    a[8] = g.Pk({0u, 0u, 1u, 2u, 4u, 0xFu, 0x10u, 0x11u, 0x1Fu, 0x20u, 0x40u, g.R(maxLen + 1), g.R(maxLen + 1)});   // longitud actual
    if (a[8] > maxLen) a[8] = maxLen;
    { const u32 room = u32(kWin) + 0x30u - a[0] - 7u; if (a[8] > room) a[8] = room; }
    a[9] = 0x100u * g.R(8) + 4u * g.R(0x20);                         // bloque nuevo (primera reserva) en C
    a[10] = g.Pk({0u, 0u, 1u, 1u, 2u});                              // 0 ambas reservas bien, 1 la primera lanza, 2 ambas lanzan
    a[11] = 0x100u * g.R(8) + 0x800u + 4u * g.R(0x10) - (g.P(50) ? 0x800u : 0u);   // bloque de la segunda reserva en C
    if (a[11] + a[8] + 8 > u32(kWin)) a[11] = 0x40;
    if (a[9] + a[8] + 8 > u32(kWin)) a[9] = 0x40;
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85BB9Cu, 0x85BC70u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kPhoneCb: {
        a[4] = g.Pk({0u, 0xFFFFFFFFu, 4u << 16, 0xFB0000u, g.W(), g.W()});
        const u32 pick = g.R(12);
        a[5] = pick < 3 ? FloatBits(g) : (pick == 3 ? B(0.5f) : (pick == 4 ? NextUp(B(0.5f)) : (pick == 5 ? NextDown(B(0.5f)) : (pick == 6 ? B(1.0f) : (pick == 7 ? B(0.0f) : (pick == 8 ? B(0.75f) : (pick == 9 ? 0x7FC00000u : (pick == 10 ? 0xFF800000u : B(0.25f)))))))));
        a[6] = g.W();
        a[7] = AuxOffset(g, 0x540);
        a[8] = g.Pk({0x13u, 0x13u, 0x13u, 0x12u, 0x14u, 0u, 1u, 0xFFFFFFFFu, 0x113u, 0x80000013u, g.W()});
        a[9] = g.Pk({0u, 0xFFFFFFFFu, 0x4000u, 0xFFFFBFFFu, g.W()});
        a[10] = AuxOffset(g, 0x120);
        a[11] = g.Pk({0u, 0xFFFFFFFFu, 0x40u, 0xFFBFu, 0x4000u, 0x400000u, g.W(), g.W()});
        a[12] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[13] = FloatBits(g); a[14] = g.Pk({0x13u, 0x12u, 0u, 0x14u, g.W()});
        break;
    }
    case kPhoneCtor:
    case kMgrInit:
        break;
    case kSlot:
        a[4] = g.Pk({3u, 4u, 5u, 2u, 6u, 0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100000003u & 0xFFFFFFFFu, 0x103u, 0xFFFFFFFDu, 0xFFFFFFFCu, 7u, 8u, g.R(10), g.W()});
        break;
    case kSurfInfo: MakeSurf(c, g, false); break;
    case kSurfAud: MakeSurf(c, g, true); break;
    case kPoint: MakePoint(c, g); break;
    case kClump: MakeClump(c, g); break;
    case kJoin: MakeJoin(c, g); break;
    case kShut: MakeShut(c, g); break;
    case kFill: MakeFill(c, g); break;
    case kMalloc: MakeMalloc(c, g); break;
    case kInit: MakeInit(c, g); break;
    case kUCopy: MakeUCopy(c, g); break;
    case kRealloc: MakeRealloc(c, g); break;
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
    j.A("v", c.v);
    return j.End();
}
}
