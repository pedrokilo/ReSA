#pragma once
// Fixtures del banco FnS7A1A (lotes S7A y S1A del plan de sustitucion): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (21 entradas, todas con hook y validated=0 antes del banco; ver README.md y fixture_oracle.py).
//   idx  direccion  nombre                                            forma
//    0   0x749000   RpAnisot engine constructor (obj,off,size)        cdecl: [0xC92474] += 1; devuelve obj
//    1   0x749010   RpAnisot engine destructor                        cdecl: [0xC92474] -= 1; devuelve obj
//    2   0x749020   RpAnisot texture constructor                      cdecl: byte [obj + [0xC92478]] = 1; devuelve obj
//    3   0x749030   RpAnisot texture destructor                       cdecl: devuelve obj
//    4   0x749040   RpAnisot texture copy (dst,src,off,size)          cdecl: byte [dst + [0xC92478]] = byte [src + [0xC92478]]; devuelve dst
//    5   0x749060   RpAnisot chunk read (stream,len,obj,off,size)     cdecl: len == 4 y obj != 0: RwStreamReadInt32(stream, &v, 4) (0x7ED540); byte [obj + [0xC92478]] = v; devuelve stream o 0
//    6   0x7490B0   RpAnisot chunk write                              cdecl: obj != 0 y [obj] != 0: RwStreamWriteInt32 (0x7ED460) del byte [obj + [0xC92478]]; devuelve stream o 0
//    7   0x7490F0   RpAnisot chunk size (obj,off,size)                cdecl: 4 si obj != 0, [obj] != 0 y byte [obj + [0xC92478]] != 1; si no 0
//    8   0x4637C0   CMissionCleanup::FindFree()                       thiscall: primera de 75 entradas de 8 bytes con byte de tipo 0 (o 0)
//    9   0x4CF9E0   CQuaternion::Copy(const CQuaternion&)             thiscall, ret 4: copia 4 palabras
//   10   0x4D1390   CDefaultAnimCallback::DefaultAnimCB(anim,data)    cdecl: no hace nada
//   11   0x4E7B80   CAEMFDecoder::FillCallback(outData, refCon)       cdecl: callback de SoundConverterFillBuffer (puente QuickTime 0x81A3C0, sin hook); devuelve AL
//   12   0x61DAE0   CTaskSimpleFight fin de animacion (anim,task)     cdecl
//   13   0x61FD10   CTaskSimpleBeKickedOnGround fin de animacion      cdecl: [task+0xC] = 0, byte [task+8] = 1
//   14   0x61FF30   CTaskSimpleBeHitWhileMoving fin de animacion      cdecl: [task+0x14] = 0, byte [task+8] = 1
//   15   0x621BE0   CTaskSimpleGangDriveBy fin de animacion           cdecl: [task+0x2C] == anim -> 0
//   16   0x675DD0   CTaskInteriorSitInChair fin de animacion          cdecl
//   17   0x6D2950   SetCompAlphaCB(material, data)                    cdecl: byte [material+7] = byte bajo de data; devuelve material
//   18   0x734850   Get2DEffectAtomicCallback(atomo, data)            cdecl: plugin 2dfx de la geometria (offset [0xC3A1E0]); cuenta > 0 -> *data = atomo, 0; si no atomo
//   19   0x735140   SkinGetBonePositions(clump)                       cdecl: tabla del EXE 0xC88258 (+ byte 0xC88658)
//   20   0x7352D0   SkinSetBonePositions(clump)                       cdecl: lee la tabla y escribe las posiciones de la jerarquia
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL cuando tienen
// hook; 0x81A3C0 no lo tiene y solo se parchea en la direccion del EXE). Familia = n % 16: tipo = familia % 4 (posicion del objeto principal en A: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde
// o del inicio) y relleno = familia / 4.
//
// Regiones globales (g, direccion, tamano) que el banco fija y vigila; a[48 + g] = su valor inicial (la region 3 se rellena con la semilla a[62] y su byte final 0x400 vale a[51] & 0xFF):
//   g: 0 0xC92474 (4: contador de objetos de motor del plugin) | 1 0xC92478 (4: desplazamiento del dato de anisotropia) | 2 0xC3A1E0 (4: desplazamiento del plugin 2dfx) | 3 0xC88258 (0x401: tabla de huesos + byte)
//
// Especificaciones de valor: PS(arena, desplazamiento) = ((0xA + arena) << 28) | desplazamiento (el banco la resuelve con la direccion real de la arena); otro valor es crudo (nunca se desreferencia
// salvo donde el caso lo dice).
//
// Efecto lateral generico de los falsos ("poke"): a[40] = id del evento del falso que lo dispara (0 = ninguno), a[41] = que llamada (0, 1, ...) de ese id, a[42] = tipo de destino (0 region global a[43]
// mas el desplazamiento a[44], 1 arena a[43] en el desplazamiento a[44]), a[46] = valor; escribe 4 bytes tras anotar el evento (comprueba el orden de las relecturas).
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

namespace FS7Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 21;
constexpr unsigned kArenas = 4;
constexpr unsigned kWords = 128;
constexpr unsigned kRegions = 4;

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
    std::array<u32, kWords> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar, a partir de `low`, sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 Aligned(Gen& g, u32 span, u32 low) { return low + 4u * g.R((u32(kWin) - low - span) / 4u + 1u); }

// Especificacion de puntero a una arena (0 A, 1 B, 2 C, 3 D)
constexpr u32 PS(u32 arena, u32 off) { return ((0xAu + arena) << 28) | off; }
constexpr bool IsPS(u32 v) { const u32 n = v >> 28; return n >= 0xAu && n <= 0xDu; }

inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 NoPS(u32 v) { return IsPS(v) ? (v ^ 0x50000000u) : v; }
// Un dword "interesante" para valores crudos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas ni de la pila del hilo del banco)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return NoPS(AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu})));
}
inline u32 RawNZ(Gen& g) { const u32 v = Raw(g); return v ? v : 0xDEADBEEFu; }
// Un entero con signo "interesante"
inline u32 IntVal(Gen& g) {
    const u32 w = g.W(), r = g.R(8);
    return g.Pk({0u, 0u, 1u, 1u, 2u, 5u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFEu, w, r, 0x100u, 0xFFFFu, 0x10000u});
}
// Un float (bits) cualquiera, con NaN de senal, infinitos, denormales y ceros (esta funciones solo copian bits)
inline u32 FltRaw(Gen& g) {
    const u32 r = g.R(100);
    if (r < 40) {
        const u32 m = 0x3F800000u | (g.W() & 0x7FFFFFu);
        const int e = int(g.R(25)) - 12;
        float f = std::bit_cast<float>(m);
        f = std::ldexp(f, e);
        if (g.P(35)) f = -f;
        return std::bit_cast<u32>(f);
    }
    if (r < 60) return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u, 0x3F000000u, 0x3DCCCCCDu, 0x42C80000u, 0x447A0000u, 0x3F333333u});
    if (r < 75) return g.Pk({0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0x7FC12345u, 0xFFE00000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F800001u, 0xFF8FFFFFu, 0x7FA00000u});
    if (r < 85) return (g.W() & 0x007FFFFFu) | (g.P(50) ? 0x80000000u : 0u);
    return g.W();
}
inline u32 Idx16(Gen& g) {
    const u32 w = g.R(0x10000);
    return g.Pk({0u, 1u, 2u, 0x157u, 0x158u, 0x159u, 0x2Fu, 0xFFFFu, 0x8158u, 0x8000u, 0x7FFFu, w, w, 0x15Au, 0x156u});
}

enum Kind : u32 { kEngine, kTexCtor, kTexDtor, kTexCopy, kStRead, kStWrite, kStSize, kFindFree, kQuat, kNop, kFill, kFight, kKick, kHit, kGang, kSit, kAlpha, k2dfx, kSkinGet, kSkinSet };
inline constexpr std::array<Kind, kFunctions> kKind{kEngine, kEngine, kTexCtor, kTexDtor, kTexCopy, kStRead, kStWrite, kStSize, kFindFree, kQuat, kNop, kFill, kFight, kKick, kHit, kGang, kSit, kAlpha, k2dfx, kSkinGet, kSkinSet};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x749000, 0x749010, 0x749020, 0x749030, 0x749040, 0x749060, 0x7490B0, 0x7490F0, 0x4637C0, 0x4CF9E0, 0x4D1390, 0x4E7B80, 0x61DAE0, 0x61FD10, 0x61FF30, 0x621BE0, 0x675DD0, 0x6D2950, 0x734850, 0x735140, 0x7352D0};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 4, 4, 4, 0x260, 0x10, 8, 0x60, 0x30, 0x10, 0x18, 0x34, 0x30, 8, 0x20, 8, 8};
// Regiones globales: direccion y tamano
inline constexpr std::array<u32, kRegions> kGlobal{0xC92474, 0xC92478, 0xC3A1E0, 0xC88258};
inline constexpr std::array<u32, kRegions> kGSize{4, 4, 4, 0x401};
// Regiones que vigila (y fija y restaura) cada funcion
inline std::vector<u32> GlobalsOf(u32 fn) {
    switch (fn) {
    case 0: case 1: return {0};
    case 2: case 4: case 5: case 6: case 7: return {1};
    case 18: return {2};
    case 19: case 20: return {3};
    default: return {};
    }
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[36..39] = semillas de las arenas A, B, C y D, a[40..46] = efecto lateral,
//   a[48 + g] = region g (g = 0..2; a[51] = byte final de la region 3), a[62] = semilla de la region 3, a[4..35] = parametros de cada funcion (abajo)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[36] = c.seed * 13u + 50u; a[37] = a[36] + 1u; a[38] = a[36] + 2u; a[39] = a[36] + 3u;
    a[0] = Offset(g, kind, kSpan[c.fn]);
    a[48] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu, g.W()});
    a[49] = Raw(g); a[50] = Raw(g); a[51] = g.P(80) ? 0u : g.Pk({1u, 0xFFu, 0x80u, 2u, g.W()}); a[62] = g.W();
    // un byte objetivo en A (en la ventana o en la guarda) y el desplazamiento de plugin que lo alcanza desde el objeto
    const auto Target = [&]() -> u32 { return Offset(g, g.R(4), 1); };
    switch (kKind[c.fn]) {
    case kEngine:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kTexCtor: {
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        a[7] = Target(); a[49] = a[7] - a[0];
        break;
    }
    case kTexDtor:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = Raw(g);
        break;
    case kTexCopy: {
        // a[7] = byte de destino en A, a[8] = byte de origen en B, a[10] = 1: origen = destino (mismo objeto); a[9] = desplazamiento del objeto origen en B
        a[7] = Target(); a[8] = Target(); a[10] = g.P(10) ? 1u : 0u;
        a[5] = Raw(g); a[6] = Raw(g);
        a[49] = a[7] - a[0];
        a[9] = a[0] + a[8] - a[7];
        if (a[10]) { a[8] = a[7]; a[9] = a[0]; }
        break;
    }
    case kStRead: {
        // a[4] flujo, a[5] longitud, a[6] = 0 objeto nulo, a[7] byte destino en A, a[8] = retorno de la lectura, a[9] = 1 la lectura escribe a[10]; poke: tras la lectura cambia el desplazamiento de plugin (a[47] = nuevo byte destino)
        a[4] = g.P(10) ? 0u : Raw(g);
        a[5] = g.Pk({4u, 4u, 4u, 4u, 4u, 0u, 8u, 3u, 5u, 0xFFFFFFFCu, g.W(), 0x04000000u});
        a[6] = g.P(10) ? 0u : 1u;
        a[7] = Target(); a[49] = a[7] - a[0];
        a[8] = g.P(25) ? 0u : RawNZ(g);
        a[9] = g.P(70) ? 1u : 0u; a[10] = g.W();
        if (g.P(45)) {
            a[47] = Target();
            a[40] = 0x7ED540; a[41] = 0; a[42] = 0; a[43] = 1; a[44] = 0; a[46] = a[47] - a[0];
        }
        break;
    }
    case kStWrite: {
        // a[4] flujo, a[6] = 0 objeto nulo, a[7] primera palabra del objeto, a[8] byte objetivo en A, a[9] = retorno de la escritura
        a[4] = g.P(10) ? 0u : Raw(g); a[5] = Raw(g);
        a[6] = g.P(10) ? 0u : 1u;
        a[7] = g.Pk({0u, 0u, 1u, 0xFFFFFFFFu, g.W(), 0x80000000u});
        a[8] = Target(); a[49] = a[8] - a[0];
        a[9] = g.P(25) ? 0u : RawNZ(g);
        break;
    }
    case kStSize: {
        // a[6] = 0 objeto nulo, a[7] primera palabra, a[8] byte objetivo, a[9] = valor del byte (0 ninguno, 1 -> 1, 2 -> 0, 3 -> 2, 4 -> 0xFF, 5 -> aleatorio)
        a[6] = g.P(10) ? 0u : 1u;
        a[7] = g.Pk({0u, 0u, 1u, 0xFFFFFFFFu, g.W(), 0x80000000u});
        a[8] = Target(); a[49] = a[8] - a[0];
        a[9] = g.R(6); a[10] = g.W();
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    }
    case kFindFree:
        // a[4] = indice de la primera libre (75 = ninguna), a[5] = semilla de los bytes de tipo
        a[4] = g.P(15) ? 75u : (g.P(30) ? g.Pk({0u, 1u, 74u, 73u}) : g.R(75));
        a[5] = g.W();
        break;
    case kQuat: {
        // a[4] arena del origen (0 B, 1 C, 2 A en otro sitio, 3 el mismo objeto), a[5] desplazamiento en esa arena; a[6..9] palabras del origen
        a[4] = g.R(4);
        a[5] = Offset(g, g.R(4), 0x10);
        if (a[4] == 2 && (a[5] + 0x10 > a[0] && a[0] + 0x10 > a[5])) a[4] = 0;
        for (u32 k = 0; k < 4; ++k) a[6 + k] = FltRaw(g);
        break;
    }
    case kNop:
        a[4] = g.W(); a[5] = g.W();
        a[6] = g.R(3);    // primer argumento: 0 en B (desplazamiento 0x20 + 3 bits de a[4]), 1 el objeto en A, 2 en A (0x60 + 2 bits de a[4])
        a[7] = g.R(3);    // segundo argumento: 0 en B (0x40 + 3 bits de a[5]), 1 en C (0x10 + 2 bits de a[5]), 2 en A (0x200 + 3 bits de a[5])
        break;
    case kFill: {
        // refCon = bloque en A (a[0]); a[4] = salida (4 bytes) en B; a[5] posicion (+0x4C), a[6] longitud (+0x50), a[7] manejador (+0x44), a[8] media (+0x48), a[9] tamano del origen (+0x54), a[10] byte +0x58,
        // a[11] byte +0x59, a[12] = EAX del puente (el OSErr es su palabra baja), a[13] = valor de salida del puente, a[14] = 1 el puente la escribe, a[15..17] = A, B y C que escribe el puente
        a[4] = Aligned(g, 8, 0x40);
        const u32 len = g.Pk({0u, 1u, 100u, 0x7FFFFFFFu, 0x80000000u, g.W() % 100000u, 0xFFFFFF00u});
        a[6] = len;
        a[5] = g.Pk({0u, len, len - 1u, len + 1u, len + 100u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.W(), len / 2u});
        a[7] = Raw(g); a[8] = Raw(g); a[9] = Raw(g);
        a[10] = g.Pk({0u, 1u, 1u, 1u, 0x80u, 0xFFu, 2u, g.W() & 0xFFu});
        a[11] = g.Pk({0u, 0u, 1u, 0xFFu, 2u, g.W() & 0xFFu});
        a[12] = g.Pk({0u, 0u, 0u, 0x10000u, 0xABCD0000u, 1u, 0xFFFFu, 0xFFFFFFFFu, 0x8000u, g.W()});
        a[13] = g.Pk({0u, 1u, 0x80000000u, 0xFFFFFFFFu, 0x7FFFFFFFu, g.W(), 1000u, RawNZ(g)});
        a[14] = g.P(70) ? 1u : 0u;
        a[15] = IntVal(g); a[16] = IntVal(g); a[17] = IntVal(g);
        a[18] = g.P(45) ? 1u : 0u;            // poke: tras el puente cambia el byte +0x59 (cuatro bytes desde +0x59)
        a[19] = g.W();
        if (a[18]) { a[40] = 0x81A3C0; a[41] = 0; a[42] = 1; a[43] = 0; a[44] = a[0] + 0x59; a[46] = a[19]; }
        // la division con signo del EXE se produce siempre que el byte +0x59 sea distinto de cero tras el puente: sin B nulo ni INT_MIN / -1 salvo que ese byte no pueda ser distinto de cero
        const bool mayDivide = a[11] != 0 || a[18];
        if (mayDivide) {
            if (a[16] == 0) a[16] = 1u + g.R(5);
            if (a[16] == 0xFFFFFFFFu && a[13] == 0x80000000u) a[13] = 5u;
        }
        break;
    }
    case kFight: {
        // anim en B (a[4]; id de palabra en +0x2C = a[5]); tarea en A: escenario a[6] (0: +0x1C = anim; 1: +0x20 = anim; 2: ninguno; 3: ambos), a[7] puntero ajeno, a[8] valor de +0x20 si no es anim,
        // a[9] byte +0x27, a[10] byte +8 inicial
        a[4] = Aligned(g, 0x30, 0x40);
        a[5] = g.Pk({0x2Fu, 0x2Fu, 0u, 1u, 0x2Eu, 0x30u, g.R(0x10000), 0xFFFFu, 0x802Fu, 0x1002Fu});
        a[6] = g.R(4); a[7] = RawNZ(g);
        a[8] = g.P(50) ? 0u : RawNZ(g);
        a[9] = g.Pk({0x10u, 0x11u, 0xFu, 1u, 0u, 0xFFu, 0x12u, 2u, 0x0Eu, g.R(256)});
        a[10] = g.Pk({0u, 1u, 0xFFu, 0x80u, g.R(256)});
        break;
    }
    case kKick:
    case kHit:
        // tarea en A: a[4] = valor del puntero de animacion (+0xC / +0x14), a[5] = byte +8; anim en B (a[6])
        a[4] = g.P(30) ? 0u : RawNZ(g); a[5] = g.Pk({0u, 1u, 0xFFu, 0x80u, g.R(256)});
        a[6] = Aligned(g, 0x30, 0x40);
        break;
    case kGang:
        // tarea en A: +0x2C = a[4] (a[5] = 1: igual a la animacion); anim en B (a[6]) o nula (a[7] = 1)
        a[4] = g.P(30) ? 0u : RawNZ(g); a[5] = g.P(50) ? 1u : 0u;
        a[6] = Aligned(g, 0x30, 0x40); a[7] = g.P(15) ? 1u : 0u;
        break;
    case kSit: {
        // anim en B (a[4]): id de palabra en +0x2C = a[5], m_BlendDelta (+0x1C) = a[6]; tarea en A: byte +0x2A = a[7], byte +0x29 = a[8], +0x18 = a[9], +0x14 = a[10]
        a[4] = Aligned(g, 0x30, 0x40);
        a[5] = Idx16(g);
        a[6] = FltRaw(g);
        a[7] = g.Pk({0u, 1u, 0x80u, 0xFFu, g.R(256)});
        a[8] = g.Pk({0u, 1u, 0xFFu, g.R(256)});
        a[9] = IntVal(g); a[10] = Raw(g);
        break;
    }
    case kAlpha:
        a[4] = Raw(g);
        break;
    case k2dfx: {
        // atomo en A (+0x18 = geometria en B a[4]); la geometria + [0xC3A1E0] cae en el byte a[5] de B y contiene un puntero a una palabra en C (a[6]; 0 = nulo) cuyo valor es a[7]; data en D (a[8])
        a[4] = Aligned(g, 8, 0x40);
        a[5] = Offset(g, g.R(4), 4); a[50] = a[5] - a[4];
        a[6] = g.P(25) ? 0u : Aligned(g, 4, 0x40);
        a[7] = g.Pk({0u, 1u, 2u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, g.W()});
        a[8] = Aligned(g, 4, 0x40);
        break;
    }
    case kSkinGet:
    case kSkinSet: {
        // a[4] = numero de huesos (con signo, <= 64), a[5] = piel (cruda), a[6] = geometria del primer atomo (cruda), a[7] = atomo falso en B, a[8] = jerarquia falsa en B, a[9] = nodos en C, a[10] = matrices en D,
        // a[11..13] = claves del falso de transformacion, a[14] = semilla de banderas de los nodos y padres, a[15] = marco del clump (cruda)
        const u32 nb = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFFFBu, 64u, 63u, 8u, 20u, 2u, 3u, 5u});
        a[4] = g.P(60) ? nb : (1u + g.R(40));
        const i32 sn = i32(a[4]);
        const u32 nn = sn > 0 ? u32(sn) : 1u;
        a[5] = RawNZ(g); a[6] = RawNZ(g);
        a[7] = Aligned(g, 0x20, 0x40);
        do { a[8] = Aligned(g, 0x20, 0x40); } while (a[8] + 0x20 > a[7] && a[7] + 0x20 > a[8]);
        a[9] = Aligned(g, 16 * nn, 0x40);
        a[10] = Aligned(g, 0x40 * nn, 0);
        a[11] = g.W(); a[12] = g.W(); a[13] = g.W();
        a[14] = g.W();
        a[15] = RawNZ(g);
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
