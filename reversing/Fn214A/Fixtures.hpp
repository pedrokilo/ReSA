#pragma once
// Fixtures del banco Fn214A (lote 214A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 214A, sin validar hasta este banco). 7 funciones (plantilla: Fn212A, con la x87 de 24 bits de SetEnvMapMaterial y los falsos de CPool/RenderWare).
//   idx  direccion  nombre                                                              forma
//    0   0x45C4B0   CPlayerPedData::operator=                                           thiscall(origen) `ret 4`: copia campo a campo (sin relleno); banderas +0x34 mezcladas con ESI del llamador (this - 4 = CPlayerInfo); eax = this
//    1   0x45DEF0   CPlayerInfo::operator=                                              thiscall(origen) `ret 4`: +0, llamada a 0x45C4B0 (REAL en ambos lados), +0xB0..+0x18F por campos; eax = this
//    2   0x45CD10   CReplay: StoreTrainUpdate (paquete de tren)                         thiscall(veh, indice) `ret 8`: tipo 0x12 en +0, 0x45B730 (falso), copias de +0x5A4.. y las referencias +0x40/+0x44 desde el pool de vehiculos [0xB74494]
//    3   0x45CDB0   tReplayTrainBlock::ExtractTrainUpdateData                           thiscall(tren, interpolacion) `ret 8`: 0x45BA90 (falso), copias a +0x5A4.., vagones anterior/siguiente con 0x571A00/0x571B70 (falsos) y la tabla 0x97F680
//    4   0x46B1A0   CTheScripts::AddToSuppressedCarModels                               cdecl(modelo): busca en 0xA44940 (40 dwords), primera ranura == -1 (si no hay, escribe en 0xA449E0)
//    5   0x48B570   CStreaming::SetMissionDoesntRequireAnim                              cdecl(ranura): suma 0x63E7 y salta a 0x409C90 (falso)
//    6   0x4D1610   CQuaternion::Normalise                                              thiscall() `ret`: x87 de 24 bits: suma de cuadrados, 1/raiz, productos por componente
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo comportamiento en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F214AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 7;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 96;         // dwords de entrada por caso

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

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
    std::array<u32, kSlots> a{};
    std::vector<u8> s;          // datos en bytes (sin uso en este lote)
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNZ(Gen& g) { const u32 v = Raw(g); return v ? v : 0x00C00000u + g.R(0x1000); }

// Un float "interesante" (bits): ceros con signo, unos, bordes, denormales, enormes, infinitos, NaN tranquilos y de senalizacion, aleatorios y valores de color
inline u32 FloatBits(Gen& g, bool allowNaN) {
    const u32 r = g.W();
    const u32 w = g.R(0x10000);
    const u32 pick = g.R(24);
    switch (pick) {
    case 0: return 0u;
    case 1: return 0x80000000u;
    case 2: case 3: return 0x3F800000u;                         // 1.0
    case 4: return 0x3F000000u;                                 // 0.5
    case 5: return 0x40000000u;                                 // 2.0
    case 6: return 0x437F0000u;                                 // 255.0
    case 7: return 0x3B808081u;                                 // 1/255
    case 8: case 9: return r & 0x007FFFFFu;                     // denormal positivo
    case 10: return (r & 0x007FFFFFu) | 0x80000000u;            // denormal negativo
    case 11: return 0x7F7FFFFFu - (r & 0xFFu);                  // casi el mayor float
    case 12: return 0xFF7FFFFFu - (r & 0xFFu);
    case 13: return g.P(50) ? 0x7F800000u : 0xFF800000u;        // infinito
    case 14: return allowNaN ? (0x7FC00000u | (r & 0x003FFFFFu) | (g.P(50) ? 0x80000000u : 0u)) : r;   // NaN tranquilo
    case 15: return allowNaN ? (0x7F800000u | ((r & 0x003FFFFFu) ? (r & 0x003FFFFFu) : 1u) | (g.P(50) ? 0x80000000u : 0u)) : r;   // NaN de senalizacion
    case 16: case 17: return 0x3F800000u + ((r & 0xFFFFu) << 7) - 0x00200000u;   // cerca de 1
    case 18: case 19: return 0x3F000000u + ((r & 0x7FFFFFu));   // [0.5, 1)
    case 20: return 0x43000000u + ((r & 0x7FFFFFu) >> 1);       // 128..
    case 21: return (r & 0x00FFFFFFu) | 0x30000000u;            // pequeno
    default: return r;                                           // cualquier patron
    }
    (void)w;
}
inline bool IsNaNBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x007FFFFFu) != 0; }


// Clases de funcion (forma)
enum Kind : u32 { kPedCopy, kInfoCopy, kStoreTrain, kExtractTrain, kSuppress, kSetAnim, kQuat };
inline constexpr std::array<Kind, kFunctions> kKind{kPedCopy, kInfoCopy, kStoreTrain, kExtractTrain, kSuppress, kSetAnim, kQuat};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x45C4B0, 0x45DEF0, 0x45CD10, 0x45CDB0, 0x46B1A0, 0x48B570, 0x4D1610};
// Bytes que cada funcion toca a partir del puntero primario de la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0xAC, 0x190, 0x4C, 0x4C, 4, 4, 0x10};

// Memoria global del juego tocada
inline constexpr u32 kMarker = 0xA9B0B0;                 // marca 0x5A de la prueba negativa (el juego real nunca la pone)
inline constexpr u32 kPoolPtr = 0xB74494;                // CPools::ms_pVehiclePool (puntero al pool de vehiculos)
inline constexpr u32 kConvTable = 0x97F67C, kConvTableBytes = 0x1BC;   // tabla de conversion de CReplay (indice ref - 1 en 0x97F680, 110 dwords) con un dword de colchon antes
inline constexpr u32 kSuppressed = 0xA44940, kSuppressedBytes = 0xC0;  // SuppressedVehicleModels (40 dwords) y los 8 dwords siguientes (0xA449E0..): el EXE escribe en [40] si no hay ranura libre
inline constexpr u32 kConvEntries = 110;

// Ranura de la tabla de conversion de la funcion 3 (perfil p, semilla s, entrada i): el oraculo la repite
inline u32 TableSlot(u32 s, u32 p, u32 i) {
    const u32 r = Rand(s, 500 + i);
    switch (p & 3) {
    case 0: return r % 110u;
    case 1: return (r % 3u == 0u) ? 0xFFFFFFFFu : (Rand(s, 700 + i) % 110u);
    case 2: return (r % 0xC00u) - 0x600u;
    default: return (r % 10u == 0u) ? (Rand(s, 700 + i) % 110u) : 0xFFFFFFFFu;
    }
}

// Origen no solapado dentro de la arena A para un objeto de `span` bytes en `o`: devuelve false si no cabe
inline bool SrcInA(Gen& g, u32 o, u32 span, u32& out) {
    const u32 win = u32(kWin);
    const bool left = o >= span + 4, right = o + 2 * span + 4 <= win;
    if (!left && !right) return false;
    const bool useLeft = left && (!right || g.P(50));
    if (useLeft) { out = 4 * g.R((o - span) / 4 + 1); if (out + span > o) out = o - span; }
    else { const u32 lo = o + span, hi = win - span; out = lo + 4 * g.R((hi - lo) / 4 + 1); }
    return true;
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto primario en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[92..95] = semillas de las arenas A, B, C y D
//   kPedCopy / kInfoCopy: a[4] = modo del origen (0 arena B, 1 arena A sin solapar, 2 el mismo objeto), a[5] = desplazamiento del origen, a[6] = semilla del relleno aleatorio del origen en A (modos 1 y 2). El origen en B es siempre aleatorio.
//   kStoreTrain: el paquete en A (a[0]); el vehiculo (0x5D8 bytes) en B (a[5]); a[6] = indice (dword de pila); vagon anterior: a[7] = modo (0 nulo, 1 relativo al pool, 2 crudo), a[8] = k (con signo) o crudo, a[9] = resto (0..0xA17);
//         vagon siguiente: a[10], a[11], a[12] igual; a[13] = modo de la base del pool (0 cruda, 1 arena C), a[14] = valor o desplazamiento en C; a[15] = desplazamiento de la estructura del pool en D;
//         a[16] = mutacion de los falsos (0 no, 1 si), a[17] = byte que el falso escribe en +0 del paquete, a[18..23] = seis dwords que escribe en +0x34..+0x4B
//   kExtractTrain: el paquete en A (a[0]); el tren (0x5D8 bytes) en B (a[5]); a[4] = referencia del vagon anterior (paquete +0x40), a[6] = del siguiente (+0x44); a[7], a[8] = modo (0 nulo, 1 arena C, 2 crudo) y valor
//         del vagon anterior que ya tenia el tren (+0x5D0); a[9], a[10] = igual para el siguiente (+0x5D4); a[11] = interpolacion (bits de float); a[12] = modo de la base del pool, a[13] = valor, a[14] = desplazamiento de la
//         estructura del pool en D, a[15] = desplazamiento del vector de estados en C; a[16] = semilla de la tabla de conversion, a[17] = perfil; a[18] = mutacion de los falsos, a[19..22] = lo que escribe el falso de 0x45BA90
//         en el tren (+0x5A4, +0x5A8, +0x5AC dwords y +0x5C0 byte); a[23..25] = paquete +0x34, +0x38, +0x3C; a[26] = paquete +0x48
//   kSuppress: a[4] = perfil de ranuras libres (0 llena, 1 algunas, 2 casi todas, 3 todas), a[5] = modo del modelo (0 presente en 0..39, 1 presente solo en 40..47, 2 = -1, 3 crudo, 4 id pequeno), a[6] = selector, a[7] = valor crudo,
//         a[8..55] = los 48 dwords de la region (0xA44940..0xA44A00); a[56] = modelo (derivado)
//   kSetAnim: a[4] = argumento
//   kQuat: a[4..7] = x, y, z, w (bits); a[8] = modo
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    a[0] = Offset(g, kind, kSpan[c.fn]);
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu});
    auto poolMode = [&](u32 base) {   // a[base] = modo de la base del pool, a[base + 1] = valor o desplazamiento
        a[base] = g.Pk({0u, 1u, 1u, 1u});
        a[base + 1] = a[base] == 0 ? Raw(g) : AuxOffset(g, 4);
    };
    switch (kKind[c.fn]) {
    case kPedCopy: case kInfoCopy: {
        const u32 span = kSpan[c.fn];
        a[4] = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 1u, 2u});
        a[5] = AuxOffset(g, span);
        a[6] = g.W();
        if (a[4] == 1) { u32 s{}; if (SrcInA(g, a[0], span, s)) a[5] = s; else a[4] = 0; }
        if (a[4] == 2) a[5] = a[0];
        break;
    }
    case kStoreTrain: {
        a[5] = AuxOffset(g, 0x5D8);
        a[6] = Raw(g);
        for (u32 base : {7u, 10u}) {
            a[base] = g.Pk({0u, 1u, 1u, 1u, 2u});
            a[base + 1] = a[base] == 1 ? g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFFFEu, 109u, 110u, g.R(300), 0u - g.R(300), g.R(20)}) : Raw(g);
            a[base + 2] = g.P(25) ? 0u : g.R(0xA18);
        }
        poolMode(13);
        a[15] = AuxOffset(g, 0x14);
        a[16] = g.Pk({0u, 1u, 1u});
        a[17] = g.W() & 0xFFu;
        for (u32 i = 18; i <= 23; ++i) a[i] = g.W();
        break;
    }
    case kExtractTrain: {
        a[5] = AuxOffset(g, 0x5D8);
        const u32 refs[]{0u, 0u, 1u, 2u, 3u, 108u, 109u, 110u, 1u + g.R(110), 1u + g.R(110), 1u + g.R(110), 1u + g.R(110)};
        a[4] = refs[g.R(12)]; a[6] = refs[g.R(12)];
        for (u32 base : {7u, 9u}) {
            a[base] = g.Pk({0u, 1u, 1u, 2u});
            a[base + 1] = a[base] == 1 ? AuxOffset(g, 4) : Raw(g);
        }
        a[11] = FloatBits(g, true);
        poolMode(12);
        a[14] = AuxOffset(g, 0x14);
        a[15] = 0x800u + g.R(0x40);
        a[16] = g.W();
        a[17] = g.R(4);
        a[18] = g.Pk({0u, 1u, 1u});
        for (u32 i = 19; i <= 22; ++i) a[i] = g.W();
        for (u32 i = 23; i <= 26; ++i) a[i] = g.W();
        break;
    }
    case kSuppress: {
        a[4] = g.Pk({0u, 0u, 1u, 1u, 1u, 2u, 3u});
        a[5] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 4u, 4u});
        a[6] = g.W();
        a[7] = g.W();
        for (u32 i = 0; i < 48; ++i) {
            const u32 v = g.P(50) ? g.R(650) : g.W();
            const u32 vv = v == 0xFFFFFFFFu ? 0u : v;
            bool free = false;
            if (i < 40) {
                switch (a[4]) { case 0: free = false; break; case 1: free = g.P(40); break; case 2: free = g.P(85); break; default: free = true; }
            } else free = g.P(30);
            a[8 + i] = free ? 0xFFFFFFFFu : vv;
        }
        switch (a[5]) {
        case 0: a[56] = a[8 + a[6] % 40u]; break;
        case 1: a[56] = a[8 + 40u + a[6] % 8u]; break;
        case 2: a[56] = 0xFFFFFFFFu; break;
        case 3: a[56] = a[7]; break;
        default: a[56] = g.R(650); break;
        }
        break;
    }
    case kSetAnim:
        a[4] = g.Pk({0u, 1u, 2u, 5u, 100u, g.R(0x100), 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFF9C19u, 0x7FFFFFFFu, 0x80000000u, 0x7FFF9C19u, g.W(), g.W(), g.R(10000)});
        break;
    case kQuat: {
        a[8] = g.Pk({0u, 0u, 0u, 1u, 2u, 2u, 3u, 4u, 5u, 6u});
        const auto comp = [&]() -> u32 {
            switch (a[8]) {
            case 0: return FloatBits(g, true);
            case 1: return g.P(50) ? 0u : 0x80000000u;                                           // todo ceros (con signo)
            case 2: return (g.P(50) ? 0u : 0x80000000u) | (0x3C000000u + g.R(0x03800000u));       // tipico: 2^-7 .. 2
            case 3: return (g.P(50) ? 0u : 0x80000000u) | (0x7E000000u + g.R(0x01800000u));       // enorme: sus cuadrados desbordan el float
            case 4: return (g.P(50) ? 0u : 0x80000000u) | (g.P(40) ? g.R(0x00800000u) : (0x00800000u + g.R(0x04000000u)));   // minusculo: denormales y 2^-126..
            case 5: return g.P(70) ? (g.P(50) ? 0u : 0x80000000u) : FloatBits(g, false);          // casi todo ceros
            default: return g.P(60) ? FloatBits(g, true) : g.Pk({0x7F800000u, 0xFF800000u, 0x7FC00000u, 0x7FA00001u, 0xFFC12345u, 0x3F800000u, 0u});   // infinitos y NaN
            }
        };
        for (u32 i = 4; i <= 7; ++i) a[i] = comp();
        break;
    }
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
