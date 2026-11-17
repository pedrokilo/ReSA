#pragma once
// Fixtures del banco Fn225A (lote 225A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (CON hook del lote 225A, sin validar hasta este banco). 8 funciones (plantilla: Fn223A; el controlador D3D9 de rasters de RenderWare y dos rutinas de la plataforma Windows; ver PendienteValidar/225A.md):
//   idx  direccion   funcion                                                              convencion
//   0   0x4CC1D0   D3D9RasterFormat::FindCorrectRasterFormat(tipo, banderas)               cdecl(2) -> EAX = formato
//   1   0x4CC580   D3D9RasterFormat::SetRasterFormat(raster, banderas)                     cdecl(2) -> 1
//   2   0x4CC970   D3D9RasterFormat::VideoMemoryRasterListRestore()                        cdecl(0) -> 0/1
//   3   0x4CD050   D3D9RasterFormat::CreateRasterFromD3DFormat(ancho, alto, formato, banderas)  cdecl(4) -> raster o 0
//   4   0x4CD250   D3D9RasterFormat::ConvertRasterToDisplayFormat(raster)                  cdecl(1), sin retorno
//   5   0x4CD360   D3D9RasterFormat::GetRasterMemorySize(salida, raster*)                  cdecl(2) -> 1
//   6   0x746070   WasForegroundAppLastFrame()                                             cdecl(0) -> AL
//   7   0x7460A0   GetBestRefreshRate(ancho, alto, profundidad)                            cdecl(3) -> frecuencia o -1
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL, o solo en la direccion del EXE
// cuando la DLL los llama por ella). Familia = n % 16: tipo = familia % 4 (posicion del raster principal: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas y de las regiones
// globales = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD). Las llamadas a D3D9 van por la tabla virtual a objetos COM falsos del banco (dispositivo, IDirect3D9, texturas, cubo, superficies y cadena de intercambio) que
// anotan cada llamada y devuelven los HRESULT del caso (una secuencia de ocho valores por caso).
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

namespace F225AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 8;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 128;        // dwords de entrada por caso

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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
// Un dword "interesante" para punteros crudos (nunca direcciones de las arenas ni de la pila del hilo del banco)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}

// Clases de funcion (forma)
enum Kind : u32 { kFind, kSetFmt, kRestore, kCreate, kConvert, kMemSize, kForeground, kRefresh };
inline constexpr std::array<Kind, kFunctions> kKind{kFind, kSetFmt, kRestore, kCreate, kConvert, kMemSize, kForeground, kRefresh};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4CC1D0, 0x4CC580, 0x4CC970, 0x4CD050, 0x4CD250, 0x4CD360, 0x746070, 0x7460A0};
// Bytes del raster principal (en la arena A) que cada funcion toca: la extension de la plataforma va en raster + X
inline constexpr u32 kExtSpan = 0x28;     // la extension D3D9 del raster ocupa 0x24 bytes (+0x00 .. +0x23)

// Regiones de memoria global del juego que el banco toca (se guardan al empezar y se restauran tras cada lado)
//   R0 tabla de formatos de pixel (0xB4E7E0, 0x80 dwords) y desplazamiento de la extension de raster (0xB4E9E0)
//   R1 cabeza de la lista de rasters en memoria de video (0xB4E9FC)
//   R2 ventana (0xC97C1C), objeto D3D (0xC97C20), adaptador (0xC97C24), dispositivo (0xC97C28) y superficie de dibujo (0xC97C2C)
//   R3 divisores (0xC9BEE4, 0xC9BEE8), formato del adaptador (0xC9BEF0), profundidad de pantalla (0xC9BEF4), profundidad del z-buffer (0xC9BEFC), byte de capacidades (0xC9BF3C)
//   R4 parametros de presentacion (0xC9C040, 14 dwords)
//   R5 tipo de dispositivo (0x8E2428)
//   R6 bandera de primer plano (0xC920EC) y subsistema seleccionado (0xC920F4)
//   R7 marca del banco (0xA9B0B0)
inline constexpr u32 kExtOffsetAt = 0xB4E9E0, kFmtAt = 0xB4E7E0, kListAt = 0xB4E9FC;
inline constexpr u32 kHwndAt = 0xC97C1C, kD3dAt = 0xC97C20, kAdapterAt = 0xC97C24, kDevAt = 0xC97C28, kRenderSurfAt = 0xC97C2C;
inline constexpr u32 kDivXAt = 0xC9BEE4, kDivYAt = 0xC9BEE8, kAdFmtAt = 0xC9BEF0, kDispAt = 0xC9BEF4, kZAt = 0xC9BEFC, kCapsAt = 0xC9BF3C;
inline constexpr u32 kPpAt = 0xC9C040, kDevTypeAt = 0x8E2428, kForeAt = 0xC920EC, kSelAt = 0xC920F4;
inline constexpr u32 kBenchMarker = 0xA9B0B0;
inline constexpr u32 kMarkerValue = 0x5A;                             // marca que solo pone el banco (el juego real usa 0 o 1): la prueba negativa condiciona sus mutaciones a ella
inline constexpr std::array<u32, 8> kRegAt{kFmtAt, kListAt, kHwndAt, kDivXAt, kPpAt, kDevTypeAt, kForeAt, kBenchMarker};
inline constexpr std::array<u32, 8> kRegBytes{0x204u, 4u, 0x14u, 0x5Cu, 0x38u, 4u, 0x10u, 1u};
// Direcciones del EXE de los colaboradores que anotan eventos
inline constexpr u32 kEvCheckTex = 0x4CBDE0, kEvCheckRt = 0x4CBE20, kEvCheckDepthTex = 0x4CBE80, kEvCheckCubeRt = 0x4CBF20, kEvCheckAutoGen = 0x4CBF80, kEvCheckDepthSurf = 0x4CBD80;
inline constexpr u32 kEvRasterPush = 0x4CB530, kEvRasterDestroyRes = 0x4CBB00, kEvRasterRebuild = 0x4CCE60, kEvRasterCreate = 0x7FB230, kEvRasterDestroy = 0x7FB020;
inline constexpr u32 kEvImageCreate = 0x8026E0, kEvImageAlloc = 0x8027A0, kEvRasterToImage = 0x7FF270, kEvRasterFromImage = 0x8001E0, kEvImageDestroy = 0x802740;
// Ids de los objetos COM falsos (los eventos anotan el id del objeto y no su direccion)
enum ComId : u32 { kComDevice, kComD3d, kComTexture, kComCube, kComSurface0, kComSurface1, kComSwap, kComCount };

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del raster principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = X (desplazamiento de la extension), a[90] = semilla de las regiones globales,
//   a[92..95] = semillas de las arenas A, B, C y D, a[80..87] = secuencia de HRESULT (cada llamada a un metodo COM que devuelve HRESULT toma el siguiente, modulo 8)
//   kFind: a[4] = tipo, a[5] = banderas, a[6] = byte de capacidades (0xC9BF3C), a[7] = profundidad de pantalla (0xC9BEF4), a[8] = profundidad del z-buffer (0xC9BEFC), a[9] = formato del back buffer (0xC9C048),
//         a[10..15] = mascaras de los seis comprobadores (textura 0x4CBDE0, objetivo de render 0x4CBE20, textura de profundidad 0x4CBE80, cubo 0x4CBF20, generacion automatica 0x4CBF80, superficie de profundidad
//         0x4CBD80: el resultado es el bit (formato & 31) de la mascara; si es 1 el valor devuelto sale de a[16]), a[16] = variante de los valores no nulos
//   kSetFmt: a[5] = banderas; el resto como kFind (a[4] sin uso); el raster en A + a[0]
//   kRestore: a[4] = numero de nodos (0..3), a[5] = profundidad del z-buffer, a[6] = formato del back buffer (< 0x80), a[7], a[8] = divisores (0xC9BEE4, 0xC9BEE8), a[9], a[10] = ancho y alto de presentacion (0xC9C040,
//         0xC9C044), a[11], a[12] = tipo y calidad de multimuestreo (0xC9C050, 0xC9C054), a[13] = ventana (0xC97C1C), a[14] = superficie de dibujo (0xC97C2C), a[15] = mascara del comprobador 0x4CBD80,
//         a[16] = variante, a[25..28] = rectangulo de GetClientRect (izquierda, arriba, derecha, abajo); nodo k (0..2) a partir de b = 32 + 14 k: b+0 = desplazamiento del raster en la arena k (k = 0: a[0]),
//         b+1 = modo del padre (0 el propio raster, 1 otro objeto, 2 crudo), b+2 = cType, b+3 = cFormat, b+4 = ancho, b+5 = alto, b+6 = formato D3D (+0x18), b+7 = textura (0 nula, 1 superficie falsa, 2 cruda),
//         b+8 = paleta (0 nula, 1 en la arena k+1), b+9 = byte +9, b+10 = byte +0xA, b+11 = cadena de intercambio (0 nula, resto cruda), b+12 = ventana del raster (cruda), b+13 = desplazamiento de la paleta
//         en la arena k+1 (0x600..0x6FC); a[74 + k] = desplazamiento del nodo en la arena k+1 (0xC00..)
//   kCreate: a[4..7] = ancho, alto, formato D3D y banderas; a[8] = 1 RwRasterCreate devuelve nulo; a[9], a[10] = ancho y alto del raster (+0xC, +0x10); a[11..14] = mascaras de 0x4CBDE0, 0x4CBE20, 0x4CBE80 y 0x4CBF80;
//         a[16] = variante
//   kConvert: a[4], a[5] = ancho y alto del raster, a[6] = modo del padre (0 el propio raster, 1 un objeto de B en a[21], 2 crudo a[22]), a[7] = cType (byte completo), a[8] = alfa (+8), a[9] = byte +9,
//         a[10] = profundidad de pantalla (0xC9BEF4), a[11] = formato del adaptador (0xC9BEF0), a[12] = adaptador (0xC97C24), a[13] = tipo de dispositivo (0x8E2428), a[14] = 1 RwImageCreate devuelve nulo,
//         a[15] = desplazamiento de la imagen en B, a[16], a[17] = mascaras de 0x4CBF20 y 0x4CBE20, a[18] = variante, a[19], a[20] = valores que devuelven 0x4CBB00 y 0x4CCE60
//   kMemSize: a[4] = desplazamiento de la salida en C, a[5] = desplazamiento en B del puntero al raster, a[6] = 1 puntero nulo, a[7] = cFormat, a[8] = byte +9, a[9] = byte +0xA, a[11] = niveles (GetLevelCount),
//         a[12], a[13] = paso del nivel 0 y su incremento por nivel, a[14], a[15] = alto del nivel 0 y su incremento, a[16..21] = campos del D3DSURFACE_DESC (formato, tipo, uso, pool, multimuestreo, calidad), a[22] = ancho
//   kForeground: a[4] = dword de 0xC920EC
//   kRefresh: a[4..6] = ancho, alto, profundidad, a[7] = subsistema (0xC920F4), a[8] = numero de modos (GetAdapterModeCount), modo i (0..15): a[16 + 4i..] = ancho, alto, frecuencia, formato
inline u32 Hr(Gen& g) { return g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 0x80004005u, 0xFFFFFFFFu, 0x8876086Cu, 0x8876017Cu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(100)}); }
inline u32 Dim(Gen& g) { return g.Pk({0u, 1u, 2u, 3u, 7u, 16u, 64u, 255u, 256u, 1024u, 0xFFFFFFFFu, 0x80000000u, g.R(4096), g.R(4096), g.R(4096), g.W()}); }
inline u32 Mask(Gen& g) { return g.Pk({0u, 0xFFFFFFFFu, 0xFFFFFFFFu, g.W(), g.W(), g.W(), 0x55555555u, 0xAAAAAAAAu}); }
inline u32 Extra(Gen& g) { return g.Pk({0u, 0u, 0x2000u, 0x4000u, 0x6000u, 0x8000u, 0x1000u, 0x9000u, 0xFFu, 0x8Fu, g.W() & 0xFFFF0000u, g.W() & 0xFFFFu}); }
inline u32 PixelFlags(Gen& g) {
    if (g.P(15)) return g.W();
    const u32 pf = g.Pk({0u, 0x100u, 0x200u, 0x300u, 0x400u, 0x500u, 0x600u, 0x700u, 0x800u, 0x900u, 0xA00u, 0xB00u, 0xF00u, 0x500u, 0x600u, 0x300u, 0x400u, g.W() & 0xF00u});
    return pf | Extra(g) | (g.P(50) ? g.R(256) : 0u);
}
inline u32 ExtOffset(Gen& g) { return g.Pk({0x30u, 0x34u, 0x38u, 0x40u, 0x44u, 0x50u, 0x7Cu, 0x100u, 0x30u + g.R(0x100), 0x30u + 4 * g.R(0x40), 0x32u + g.R(0x80)}); }
inline u32 Depths(Gen& g) { return g.Pk({0x10u, 0x20u, 0x18u, 0x0Fu, 0x11u, 0u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 8u, g.W()}); }
inline u32 ZDepth(Gen& g) { return g.Pk({0x10u, 0x10u, 0x20u, 0x20u, 0u, 0x18u, 0x0Fu, 0x11u, 0xFFFFFFFFu, g.W()}); }
inline u32 BackFmt(Gen& g) { return g.Pk({0x15u, 0x16u, 0x17u, 0x14u, 0x1Au, 0x19u, 0u, 0x7Fu, g.R(0x80), g.R(0x80)}); }
inline u32 D3dFmt(Gen& g) {
    return g.Pk({0x15u, 0x16u, 0x17u, 0x1Au, 0x19u, 0x32u, 0x29u, 0x45u, 0x46u, 0x47u, 0x49u, 0x4Bu, 0x4Du, 0x4Fu, 0x50u, 0x51u, 0x7Fu, 0x80u, 0u, 0x31545844u, 0x32545844u, 0x33545844u, 0x34545844u, 0x35545844u,
                 0x31545843u, 0x35545845u, 0x80000000u + g.R(0x80), 0x7FFFFFFFu, g.R(0x60), g.R(0x60)});
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    const Kind k = kKind[c.fn];
    a[1] = kind;
    a[2] = f / 4;
    a[90] = c.seed * 7u + 11u;
    a[92] = c.seed * 13u + 50u; a[93] = a[92] + 1u; a[94] = a[92] + 2u; a[95] = a[92] + 3u;
    a[3] = ExtOffset(g);
    a[0] = Offset(g, kind, a[3] + kExtSpan);
    for (u32 i = 0; i < 8; ++i) a[80 + i] = Hr(g);
    switch (k) {
    case kFind: case kSetFmt:
        a[4] = g.Pk({0u, 0u, 0u, 4u, 4u, 4u, 1u, 1u, 2u, 2u, 5u, 5u, 5u, 3u, 6u, 7u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(8)});
        a[5] = PixelFlags(g);
        a[6] = g.Pk({0u, 0x80u, 0x7Fu, 0xFFu, 0x01u, g.W() & 0xFFu});
        a[7] = Depths(g); a[8] = ZDepth(g); a[9] = BackFmt(g);
        for (u32 i = 0; i < 6; ++i) a[10 + i] = Mask(g);
        a[16] = g.W();
        break;
    case kRestore: {
        a[4] = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 3u, 3u});
        a[5] = ZDepth(g); a[6] = BackFmt(g);
        a[7] = g.Pk({0u, 0u, 1u, 640u, 0x10000u, 0xFFFFFFFFu, g.W(), g.R(2000)}); a[8] = g.Pk({0u, 0u, 1u, 480u, 0x10000u, 0xFFFFFFFFu, g.W(), g.R(2000)});
        a[9] = g.Pk({0u, 1u, 640u, 800u, 0xFFFFFFFFu, g.W(), g.R(2000)}); a[10] = g.Pk({0u, 1u, 480u, 600u, 0xFFFFFFFFu, g.W(), g.R(2000)});
        a[11] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, g.W()}); a[12] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W()});
        a[13] = Raw(g);
        a[14] = g.Pk({0u, 0u, 0x00A0A0A0u, 0xFFFFFFFFu, g.W()});
        a[15] = Mask(g); a[16] = g.W();
        for (u32 i = 0; i < 4; ++i) a[25 + i] = g.Pk({0u, 1u, 2u, 640u, 480u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W(), g.R(2000)});
        for (u32 k2 = 0; k2 < 3; ++k2) {
            const u32 b = 32 + 14 * k2;
            a[b + 0] = k2 == 0 ? a[0] : g.R(0x4D8 - a[3]);
            a[b + 1] = g.Pk({0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 2u});
            a[b + 2] = g.Pk({0u, 0u, 4u, 4u, 1u, 1u, 2u, 2u, 5u, 5u, 3u, 6u, 7u, 0x08u | g.R(8), 0xFDu});
            a[b + 3] = g.Pk({0u, 0x10u, 0x20u, 0x40u, 0x60u, 0x80u, 0xA0u, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
            a[b + 4] = Dim(g); a[b + 5] = Dim(g);
            a[b + 6] = g.Pk({0x45u, 0x46u, 0x47u, 0x49u, 0x4Bu, 0x4Du, 0x50u, 0x51u, 0x15u, 0x16u, 0u, 0xFFFFFFFFu, 0x80000000u, 0x31545844u, g.W(), g.R(0x60), 0x46u, 0x50u});
            a[b + 7] = g.Pk({0u, 0u, 1u, 2u});
            a[b + 8] = g.Pk({0u, 1u, 1u});
            a[b + 9] = g.Pk({0u, 0x10u, 0x01u, 0x11u, 0x0Fu, 0xF0u, g.W() & 0xFFu});
            a[b + 10] = g.Pk({0u, 0x10u, 0x01u, 0x0Fu, 0xF0u, 0x11u, g.W() & 0xFFu});
            a[b + 11] = g.Pk({0u, 0u, 1u, Raw(g)});
            a[b + 12] = Raw(g);
            a[b + 13] = 0x600u + 4 * g.R(0x40);
            a[74 + k2] = 0xC00u + 4 * g.R(0x7C);
        }
        break;
    }
    case kCreate:
        a[4] = Dim(g); a[5] = Dim(g);
        a[6] = D3dFmt(g);
        a[7] = g.P(12) ? g.W() : (g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 5u, 0u, 1u}) | (g.P(60) ? g.Pk({0x9000u, 0x8000u, 0x1000u, 0x80u, 0x6000u, 0xF000u, 0x9100u}) : 0u) | (g.P(25) ? (g.W() & 0xFFFFFF00u) : 0u));
        a[8] = g.P(8) ? 1u : 0u;
        a[9] = Dim(g); a[10] = Dim(g);
        for (u32 i = 0; i < 4; ++i) a[11 + i] = Mask(g);
        a[16] = g.W();
        break;
    case kConvert:
        a[4] = Dim(g); a[5] = Dim(g);
        a[6] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u});
        a[7] = g.P(30) ? g.W() & 0xFFu : g.Pk({0u, 0u, 4u, 4u, 5u, 5u, 1u, 2u, 3u, 6u, 7u});
        a[8] = g.Pk({0u, 1u, 0xFFu, 0x80u, g.W() & 0xFFu});
        a[9] = g.Pk({0u, 0x10u, 0x01u, 0x11u, 0x0Fu, 0xF0u, g.W() & 0xFFu});
        a[10] = Depths(g);
        a[11] = g.Pk({0x16u, 0x15u, 0x17u, g.W(), g.R(0x80)});
        a[12] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, g.W()});
        a[13] = g.Pk({1u, 1u, 2u, 0u, g.W()});
        a[14] = g.P(10) ? 1u : 0u;
        a[15] = AuxOffset(g, 4);
        a[16] = Mask(g); a[17] = Mask(g); a[18] = g.W();
        a[19] = g.W(); a[20] = g.W();
        a[21] = AuxOffset(g, 4); a[22] = Raw(g);
        break;
    case kMemSize:
        a[4] = AuxOffset(g, 4); a[5] = AuxOffset(g, 4);
        a[6] = g.P(12) ? 1u : 0u;
        a[7] = g.Pk({0u, 0x20u, 0x40u, 0x60u, 0x10u, 0x80u, 0xFFu, g.W() & 0xFFu});
        a[8] = g.Pk({0u, 0u, 0x10u, 0x01u, 0x11u, 0x0Fu, 0xF0u, g.W() & 0xFFu});
        a[9] = g.Pk({0u, 0u, 0x10u, 0x01u, 0x0Fu, 0xF0u, 0x11u, 0x20u, g.W() & 0xFFu});
        a[11] = g.Pk({0u, 1u, 1u, 2u, 3u, 5u, 8u, 11u, 12u, g.R(13)});
        a[12] = g.Pk({0u, 4u, 64u, 256u, 0xFFFFFFFFu, g.W(), g.R(8192), 0x80000000u}); a[13] = g.Pk({0u, 1u, 0xFFFFFFFCu, g.W(), g.R(100)});
        a[14] = g.Pk({0u, 1u, 4u, 16u, 256u, 0xFFFFFFFFu, g.W(), g.R(4096)}); a[15] = g.Pk({0u, 0xFFFFFFFFu, 1u, 3u, g.W(), g.R(64)});
        for (u32 i = 0; i < 6; ++i) a[16 + i] = g.Pk({0u, 1u, 21u, 22u, 0x31545844u, g.W(), g.R(4)});
        a[22] = Dim(g);
        break;
    case kForeground:
        a[4] = g.Pk({0u, 1u, 0x100u, 0x10000u, 0x01000000u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFF00u, 2u, g.W(), g.W() & 0xFFFFFF00u, g.W() & 0xFFu});
        break;
    case kRefresh: {
        const u32 w = g.Pk({640u, 800u, 1024u, 1280u, 1920u, 0u, 1u, g.W(), 640u, 1024u});
        const u32 h = g.Pk({480u, 600u, 768u, 1024u, 1080u, 0u, 1u, g.W(), 480u, 768u});
        a[4] = w; a[5] = h;
        a[6] = g.Pk({32u, 32u, 24u, 24u, 16u, 16u, 0u, 15u, 8u, 31u, 33u, 0x20u + (g.W() & 0xFFFFFF00u), 0x18u + (g.W() & 0xFFFFFF00u), g.W()});
        a[7] = g.Pk({0u, 0u, 1u, 2u, 0xFFFFFFFFu, g.W()});
        a[8] = g.Pk({0u, 1u, 2u, 3u, 5u, 8u, 12u, 16u, 16u, 0x80000000u, 0xFFFFFFFFu, 0xFFFFFFF0u, g.R(17)});
        const u32 want = a[6] == 32u ? 0x16u : (a[6] == 24u ? 0x14u : 0x17u);
        for (u32 i = 0; i < 16; ++i) {
            const u32 b = 16 + 4 * i;
            const bool match = g.P(55);
            a[b] = match ? w : g.Pk({640u, 800u, 1024u, g.W()});
            a[b + 1] = match ? h : g.Pk({480u, 600u, 768u, g.W()});
            a[b + 2] = g.Pk({0u, 59u, 60u, 61u, 75u, 85u, 100u, 120u, 144u, 0x3Bu, 0x3Cu, 0x7FFFFFFEu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W(), g.R(200), 60u, 60u, 75u});
            a[b + 3] = g.P(75) ? want : g.Pk({0x16u, 0x14u, 0x17u, 0x15u, g.W()});
        }
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
