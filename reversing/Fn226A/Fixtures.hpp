#pragma once
// Fixtures del banco Fn226A (lote 226A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (17 entradas, todas cdecl con hook y validated=0 antes del banco; ver README.md y fixture_oracle.py).
//   idx  direccion  nombre                                   forma
//    0   0x747DB0   CGamma::SetGammaNoFade(level)             copia sin `fade` de SetGamma: x87 de 24 bits, pow del CRT, rampa de 256 entradas, dispositivo por 0x7F9D50 y vtable (Get/SetGammaRamp)
//    1   0x748F70   RpAnisotPluginAttach()                    RwEngineRegisterPlugin, RwTextureRegisterPlugin (-> 0xC92478), RwTextureRegisterPluginStream (-> 0xC9247C), 0x7FDE60([0xC92478])
//    2   0x749120   AtomicRightsStreamRead(stream,len,..)     RwStreamReadInt32 a 0xC924A8 y (len == 8) a 0xC92480
//    3   0x749160   AtomicRightsStreamWrite(stream,len,obj..) RwStreamWriteInt32 de pipeline->pluginId (+0x2C) y pluginData (+0x30), pipeline = [obj + 0x6C] releido
//    4   0x7491F0   _rpAtomicResyncInterpolatedSphere(atomo)  esfera interpolada (x87 de 24 bits)
//    5   0x749330   RpAtomicGetWorldBoundingSphere(atomo)     esfera del mundo (RwFrameDirty, RwFrameGetLTM, RwV3dTransformPoint, _rwSqrt, x87)
//    6   0x749520   ClumpFreeListDestroyCallBack(clump,data)  destruye atomos, luces y camaras de un clump y lo devuelve a su lista libre
//    7   0x7496A0   AtomicFreeListDestroyCallBack(atomo,data) cuerpo de RpAtomicDestroy sin valor de retorno
//    8   0x749830   ClumpPluginAttach()                       RwCameraRegisterPlugin (-> 0xC924D8), RpLightRegisterPlugin (-> 0xC924DC)
//    9   0x749B20   RpClumpRender(clump)                      RwFrameGetLTM y callback de dibujo de cada atomo con la bandera 4
//   10   0x749BB0   RpClumpForAllCameras(clump,cb,data)       recorre la lista de camaras con el desplazamiento [0xC924D8]
//   11   0x749C00   RpClumpForAllLights(clump,cb,data)        recorre la lista de luces con el desplazamiento [0xC924DC]
//   12   0x749C50   RpAtomicCreate()                          reserva (+0x144) e inicializa un atomo de 0x70 bytes
//   13   0x749D20   AtomicSyncCallBack(atomo)                 resincroniza la esfera si flags & 2 y marca privateFlags |= 1
//   14   0x749D40   RpAtomicSetGeometry(atomo,geom,flags)     AddRef/Destroy de geometrias, esfera del morph target 0, RwFrameUpdateObjects
//   15   0x749DC0   RpAtomicDestroy(atomo)                    destruye el atomo y devuelve 1
//   16   0x749E60   RpAtomicClone(atomo)                      clona el atomo (RpAtomicCreate, SetFrame, geometria, esfera, plugins)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL cuando
// tienen hook, o solo en la direccion del EXE). RwEngineInstance ([0xC97B24]) es una de dos tablas falsas (E1 o E2, cada una con sus ranuras +0x144 y +0x148). Familia = n % 16: tipo = familia % 4
// (posicion del objeto principal en A: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno = familia / 4.
//
// Regiones de memoria global (g, direccion, tamano) que el banco fija y vigila; a[48 + g] = su valor inicial (la region 0 usa ademas a[59], a[60], a[61]; la 2 se rellena con la semilla a[62]):
//   g: 0 0xC92134 (16: m_IsEnabled, nivel actual, objetivo, previo) | 1 0xC920C8 (4: gbGammaChanged) | 2 0xC8D4C8 (0xC00: rampa guardada + tabla de gamma) | 3 0xC92478 | 4 0xC9247C | 5 0xC924A8 | 6 0xC92480
//      7 0xC924AC (desplazamiento de las listas libres en RwEngineInstance) | 8 0xC924D8 (desplazamiento del plugin de camaras) | 9 0xC924DC (de luces) | 10 0xC97B24 (RwEngineInstance)
//
// Especificaciones de valor: PS(arena, desplazamiento) = ((0xA + arena) << 28) | desplazamiento (el banco la resuelve con la direccion real de la arena); 0xE0000001 / 0xE0000002 = RwEngineInstance falsa
// E1 / E2; 0xF00000kk = la funcion falsa kk (callbacks); otro valor es crudo (nunca se desreferencia salvo donde el caso lo dice). E1/E2: la palabra i de la tabla Ek es Rand(a[47] + k - 1, i) salvo las
// ranuras +0x144 (RwFreeListAlloc) y +0x148 (RwFreeListFree), que son funciones falsas con id 0xE144/0xE148 (E1) o 0xE244/0xE248 (E2).
//
// Efecto lateral generico de los falsos ("poke"): a[40] = id del evento del falso que lo dispara (0 = ninguno), a[41] = que llamada (0, 1, ...) de ese id, a[42] = tipo de destino (0 region global a[43] mas
// el desplazamiento a[44], 1 arena a[43] en el desplazamiento a[44], 2 puntero-argumento de la llamada mas a[44], 3 el enlace del nodo actual de RpClumpForAll* mas a[44]), a[45] = modo del valor (0 crudo a[46], 1 especificacion), a[46] = valor; escribe 4 bytes
// tras anotar el evento (comprueba el orden de las relecturas).
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

namespace F226AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 17;
constexpr unsigned kArenas = 4;
constexpr unsigned kWords = 128;
constexpr unsigned kRegions = 11;

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
inline u32 Aligned(Gen& g, u32 span, u32 low) { return low + 4u * g.R((u32(kWin) - low - span) / 4u); }

// Especificacion de puntero a una arena (0 A, 1 B, 2 C, 3 D)
constexpr u32 PS(u32 arena, u32 off) { return ((0xAu + arena) << 28) | off; }
constexpr bool IsPS(u32 v) { const u32 n = v >> 28; return n >= 0xAu && n <= 0xDu; }
constexpr u32 kEngine1 = 0xE0000001u, kEngine2 = 0xE0000002u;
constexpr u32 FS(u32 k) { return 0xF0000000u + k; }
constexpr bool IsFS(u32 v) { return (v >> 8) == 0xF00000u; }
constexpr bool IsSpecial(u32 v) { return IsPS(v) || v == kEngine1 || v == kEngine2 || IsFS(v); }

inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 NoPS(u32 v) { return IsSpecial(v) ? (v ^ 0x50000000u) : v; }
// Un dword "interesante" para valores crudos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas ni de la pila del hilo del banco)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return NoPS(AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu})));
}
inline u32 RawNZ(Gen& g) { const u32 v = Raw(g); return v ? v : 0xDEADBEEFu; }
// Un entero con signo "interesante" (codigos de retorno de colaboradores)
inline u32 RetVal(Gen& g) {
    const u32 w = g.W(), r = g.R(8);
    return g.Pk({0u, 0u, 1u, 1u, 2u, 5u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFFFEu, w, r, 0x100u});
}

// Un float (bits): 62% de "juego" (magnitud 2^-12..2^12), 16% valores notables, 4% infinitos y NaN, 6% denormales, 12% bits cualesquiera
inline u32 FltRaw(Gen& g) {
    const u32 r = g.R(100);
    if (r < 62) {
        const u32 m = 0x3F800000u | (g.W() & 0x7FFFFFu);
        const int e = int(g.R(25)) - 12;
        float f = std::bit_cast<float>(m);
        f = std::ldexp(f, e);
        if (g.P(35)) f = -f;
        return std::bit_cast<u32>(f);
    }
    if (r < 78) return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u, 0x3F000000u, 0x3DCCCCCDu, 0x42C80000u, 0x447A0000u, 0x3F333333u});
    if (r < 82) return g.Pk({0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0x7FC12345u, 0xFFE00000u, 0x7F7FFFFFu, 0xFF7FFFFFu});
    if (r < 88) return (g.W() & 0x007FFFFFu) | (g.P(50) ? 0x80000000u : 0u);
    return g.W();
}
// Sin NaN de senal (exponente 255, mantisa distinta de cero y bit 22 limpio): la x87 los trata distinto segun vengan de memoria o de un registro y el compilador elige la forma; quedan fuera de las fixtures (README.md)
inline u32 Flt(Gen& g) {
    u32 v = FltRaw(g);
    if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x7FFFFFu) && !(v & 0x400000u)) v |= 0x400000u;
    return v;
}
// Un float "de juego" sin NaN ni infinito
inline u32 FltTame(Gen& g) {
    const u32 v = Flt(g);
    return ((v >> 23) & 0xFF) == 0xFF ? (v & 0x807FFFFFu) | 0x40000000u : v;
}
// Un entero de 16 bits (indice de morph target)
inline u32 Idx16(Gen& g) {
    const u32 w = g.R(0x10000);
    return g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 0xFFFFu, 0xFFFEu, 5u, 7u, 9u, 0x7FFFu, 0x8000u, 0xFFFCu, w});
}
inline i32 S16(u32 v) { return i32(std::int16_t(v & 0xFFFFu)); }

enum Kind : u32 { kGamma, kAnisot, kRead, kWrite, kResync, kWorldBS, kClumpFree, kAtomFree, kClumpAttach, kRender, kForAll, kCreate, kSync, kSetGeom, kClone };
inline constexpr std::array<Kind, kFunctions> kKind{kGamma, kAnisot, kRead, kWrite, kResync, kWorldBS, kClumpFree, kAtomFree, kClumpAttach, kRender, kForAll, kForAll, kCreate, kSync, kSetGeom, kAtomFree, kClone};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x747DB0, 0x748F70, 0x749120, 0x749160, 0x7491F0, 0x749330, 0x749520, 0x7496A0, 0x749830, 0x749B20, 0x749BB0, 0x749C00, 0x749C50, 0x749D20, 0x749D40, 0x749DC0, 0x749E60};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 0x70, 0x70, 0x70, 0x20, 0x70, 4, 0x20, 0x20, 0x20, 0x70, 0x70, 0x70, 0x70, 0x70};
// Regiones globales: direccion y tamano
inline constexpr std::array<u32, kRegions> kGlobal{0xC92134, 0xC920C8, 0xC8D4C8, 0xC92478, 0xC9247C, 0xC924A8, 0xC92480, 0xC924AC, 0xC924D8, 0xC924DC, 0xC97B24};
inline constexpr std::array<u32, kRegions> kGSize{0x10, 4, 0xC00, 4, 4, 4, 4, 4, 4, 4, 4};
// Regiones que vigila (y fija y restaura) cada funcion
inline std::vector<u32> GlobalsOf(u32 fn) {
    switch (fn) {
    case 0: return {0, 1, 2};
    case 1: return {3, 4};
    case 2: return {5, 6};
    case 6: return {7, 8, 9, 10};
    case 7: case 15: case 12: return {7, 10};
    case 8: return {8, 9};
    case 10: return {8};
    case 11: return {9};
    default: return {};
    }
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[36..39] = semillas de las arenas A, B, C y D, a[40..46] = efecto lateral, a[47] = semilla
//   de las tablas E1/E2, a[48 + g] = region g, a[59..62] = resto de las regiones 0 y 2, a[4..35] y a[63..127] = parametros de cada funcion (abajo)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[36] = c.seed * 13u + 50u; a[37] = a[36] + 1u; a[38] = a[36] + 2u; a[39] = a[36] + 3u;
    a[47] = g.W();
    a[0] = Offset(g, kind, kSpan[c.fn]);
    for (u32 k = 0; k < 11; ++k) a[48 + k] = Raw(g);
    a[59] = Raw(g); a[60] = Raw(g); a[61] = Raw(g); a[62] = g.W();
    a[48 + 10] = g.P(50) ? kEngine1 : kEngine2;
    a[48 + 7] = 0x40u + 4u * g.R(0x30);            // desplazamiento de las listas libres en la tabla
    a[48 + 8] = 0x20u + 4u * g.R(0x30); a[48 + 9] = 0x20u + 4u * g.R(0x30);
    const auto Frame = [&]() -> u32 { return g.P(30) ? 0u : g.Pk({RawNZ(g), PS(2, Aligned(g, 0x40, 0x40))}); };   // un puntero de marco: nulo, crudo o a la arena (nunca se desreferencia: lo reciben los falsos)
    const auto F = [&](bool tame) -> u32 { return tame ? FltTame(g) : Flt(g); };
    switch (kKind[c.fn]) {
    case kGamma: {
        // a[4] nivel; region 0 = a[48] (m_IsEnabled), a[59] (actual), a[60] (objetivo), a[61] (previo); a[49] = gbGammaChanged; a[10], a[11] = dispositivos de la 1a y la 2a llamada a 0x7F9D50 (0 nulo, 1 A, 2 D);
        //   a[12] = vtable 1 en C, a[13] = dispositivo D en D, a[14] = vtable 2 en C; a[15] = semilla de lo que escribe GetGammaRamp (0 nada); a[62] = semilla de la region 2
        u32 lv;
        if (g.R(100) < 60) lv = std::bit_cast<u32>(float(g.R(3000)) / 1000.0f - 0.5f); else lv = Flt(g);
        a[4] = lv;
        a[48] = g.Pk({0u, 1u, 1u, 1u, 2u, 0x100u, 0xFFFFFFFFu, RawNZ(g)});
        a[59] = Flt(g);
        const float level = std::bit_cast<float>(lv) * 0.7f;
        a[60] = g.P(35) ? std::bit_cast<u32>(level) : Flt(g);
        a[61] = Flt(g);
        a[49] = g.Pk({0u, 1u, 0x100u, 0x01010100u, 0xFFFFFF01u, g.W()});
        a[10] = g.P(10) ? 0u : 1u + g.R(2);
        a[11] = 1u + g.R(2);
        a[12] = 0x40u + 4u * g.R(0x100); a[13] = 0x40u + 4u * g.R(0x100); a[14] = 0x600u + 4u * g.R(0x100);
        a[15] = g.P(50) ? 0u : g.W();
        if (g.P(45)) {
            const u32 m = g.R(3);
            a[40] = 0x7F9D50; a[41] = g.R(2);
            if (m == 0) { a[42] = 0; a[43] = 1; a[44] = 0; a[45] = 0; a[46] = g.Pk({0u, 1u, 0x100u}); }
            else if (m == 1) { a[42] = 1; a[43] = 0; a[44] = a[0]; a[45] = 1; a[46] = PS(2, a[14]); }
            else { a[42] = 1; a[43] = 3; a[44] = a[13]; a[45] = 1; a[46] = PS(2, a[12]); }
        }
        break;
    }
    case kAnisot: {
        // a[4], a[5], a[6] = lo que devuelven RwEngineRegisterPlugin, RwTextureRegisterPlugin y RwTextureRegisterPluginStream; a[48 + 3], a[48 + 4] = 0xC92478 y 0xC9247C iniciales
        a[4] = RetVal(g); a[5] = RetVal(g); a[6] = RetVal(g);
        if (g.P(60)) a[4] = g.Pk({0u, 1u, 7u});
        if (g.P(60)) a[5] = g.Pk({0u, 1u, 12u, 0x100u});
        if (g.P(60)) a[6] = g.Pk({0u, 1u, 3u});
        if (g.P(45)) {
            a[40] = g.Pk({0x7F2BB0u, 0x7F3BB0u, 0x804550u, 0x804550u}); a[41] = 0;
            a[42] = 0; a[43] = g.Pk({3u, 4u}); a[44] = 0; a[45] = 0; a[46] = Raw(g);
        }
        break;
    }
    case kRead: {
        // a[4] flujo, a[5] longitud, a[6..8] objeto, desplazamiento y tamano (sin uso), a[9], a[10] = retorno de la 1a y la 2a lectura, a[11], a[12] = lo que escriben; a[48 + 5], a[48 + 6] = destinos iniciales
        a[4] = g.P(10) ? 0u : Raw(g); a[5] = g.Pk({8u, 8u, 8u, 0u, 4u, 0xFFFFFFF8u, 0x00000008u, g.W(), 7u, 9u});
        a[6] = Raw(g); a[7] = Raw(g); a[8] = Raw(g);
        a[9] = g.P(25) ? 0u : RawNZ(g); a[10] = g.P(25) ? 0u : RawNZ(g);
        a[11] = g.W(); a[12] = g.W();
        break;
    }
    case kWrite: {
        // a[4] flujo, a[5..7] sin uso (longitud, desplazamiento, tamano), a[8] = tuberia 1 en B (id en +0x2C, datos en +0x30), a[9] = tuberia 2 en B, a[10], a[11] = retorno de la 1a y la 2a escritura
        a[4] = g.P(10) ? 0u : Raw(g); a[5] = Raw(g); a[6] = Raw(g); a[7] = Raw(g);
        a[8] = Aligned(g, 0x40, 0x40); a[9] = Aligned(g, 0x40, 0x40);
        a[10] = g.P(25) ? 0u : RawNZ(g); a[11] = g.P(25) ? 0u : RawNZ(g);
        if (g.P(50)) {
            a[40] = 0x7ED460; a[41] = 0;
            const u32 m = g.R(3);
            if (m == 0) { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x6C; a[45] = 1; a[46] = PS(1, a[9]); }          // la tuberia se relee para la 2a escritura
            else if (m == 1) { a[42] = 1; a[43] = 1; a[44] = a[8] + 0x30; a[45] = 0; a[46] = g.W(); }            // el dato de la tuberia cambia entre las dos
            else { a[42] = 1; a[43] = 1; a[44] = a[8] + 0x2C; a[45] = 0; a[46] = 0; }
        }
        break;
    }
    case kResync:
    case kWorldBS: {
        // a[4] = geometria (0 nula, 1 en B a[12]); a[5] = numMorphTargets; a[6], a[7] = indices inicial y final; a[8] = flags del interpolador; a[9] = recipTime; a[10] = posicion; a[11] = privateFlags;
        //   a[12] = geometria en B; a[13] = morph target 0 de la matriz en D (entradas -4..10 de 0x1C bytes); a[64..67] = esfera local del atomo (centro xyz, radio); a[68..127] = 15 esferas de morph targets
        //   (entradas -4..10, 4 floats cada una: centro xyz y radio). WorldBS: a[15] = RwFrameDirty, a[16] = matriz en C, a[17] = banderas de la matriz, a[18..26] = right, up y at, a[27] = _rwSqrt,
        //   a[28..30] = lo que escribe RwV3dTransformPoint, a[31] = marco (+4)
        const bool tame = g.P(50);
        a[4] = g.P(20) ? 0u : 1u;
        a[5] = g.Pk({0u, 1u, 2u, 2u, 3u, 3u, 4u, 5u, 6u, 8u, 0xFFFFFFFFu, 0xFFFFFFFBu, 100u, 0xFFFF0000u});
        a[6] = Idx16(g); a[7] = Idx16(g);
        for (int pass = 0; pass < 2; ++pass) {
            for (u32 k : {6u, 7u}) {   // un indice que se desreferencia queda en [-4, 9]: si esta fuera, numMorphTargets baja hasta el (la comparacion con signo falla y no se lee)
                const i32 idx = S16(a[k]);
                if (idx < i32(a[5]) && (idx < -4 || idx > 9)) a[5] = u32(idx);
            }
        }
        if (g.P(50)) {   // indices validos distintos: la rama de interpolacion
            a[5] = g.Pk({2u, 3u, 4u, 5u, 6u, 8u});
            a[6] = g.R(a[5]); a[7] = g.R(a[5]);
            if (a[6] == a[7] && g.P(80)) a[7] = (a[6] + 1u) % a[5];
        } else if (g.P(25)) a[7] = a[6];
        a[8] = g.Pk({0u, 1u, 2u, 3u, 7u, 0xFFFFFFFDu, 2u, 2u, 2u, g.W()});
        a[9] = tame ? FltTame(g) : Flt(g); a[10] = tame ? FltTame(g) : Flt(g);
        if (g.P(40)) a[9] = 0x3F800000u;
        a[11] = g.Pk({0u, 1u, 0xFEu, 0xFFu, g.W()});
        a[12] = Aligned(g, 0x80, 0x40); a[13] = 0x100u + 4u * g.R(0x300);
        for (u32 k = 0; k < 4; ++k) a[64 + k] = F(tame);
        for (u32 k = 0; k < 60; ++k) a[68 + k] = F(tame);
        if (kKind[c.fn] == kWorldBS) {
            a[15] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, RawNZ(g)});
            a[16] = 0x40u + 4u * g.R(0x300);
            a[17] = g.Pk({0u, 1u, 2u, 3u, 3u, 3u, 0x20003u, 0x10003u, g.W()});
            for (u32 k = 0; k < 9; ++k) a[18 + k] = F(tame);
            a[27] = F(tame);
            a[28] = g.W(); a[29] = g.W(); a[30] = g.W();
            a[31] = Frame();
            if (g.P(55)) {
                const u32 m = g.R(6);
                a[41] = 0; a[42] = 1; a[43] = 0; a[45] = 0;
                if (m == 0) { a[40] = 0x7491F0; a[44] = a[0] + 4; a[45] = 1; a[46] = PS(2, Aligned(g, 0x40, 0x40)); }   // el marco se lee antes de la llamada
                else if (m == 1) { a[40] = 0x7491F0; a[44] = a[0] + 3; a[46] = g.Pk({0u, 1u, g.W()}); }                  // privateFlags se lee despues
                else if (m == 2) { a[40] = 0x7F0340; a[44] = a[0] + 3; a[46] = g.Pk({0u, 1u, 0xFFu}); }
                else if (m == 3) { a[40] = 0x7EDB30; a[44] = a[0] + 0x28; a[46] = F(tame); }                             // el radio se relee tras la raiz
                else if (m == 4) { a[40] = 0x7EDB30; a[44] = a[0] + 3; a[46] = g.Pk({0u, 1u, 0xFFu}); }
                else { a[40] = 0x7EDD60; a[43] = 2; a[44] = a[16] + 0x20 * g.R(2); a[46] = Flt(g); }                       // el LTM cambia tras transformar el punto
            }
        }
        break;
    }
    case kClumpFree: {
        // a[4], a[5], a[6] = atomos, luces y camaras (0..3); a[7], a[8] = base y separacion de los atomos en B; a[9], a[10] = base y separacion de los nodos de luces en C; a[11], a[12] = las de camaras en D;
        //   a[13] = mascara de lo que devuelve RpAtomicGetWorld por llamada; a[14] = marco del clump; a[64 + 4 i ..] = por atomo i (+0x14, +0x18, +4)
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u}); a[5] = g.Pk({0u, 1u, 2u, 3u}); a[6] = g.Pk({0u, 1u, 2u, 3u});
        a[7] = Aligned(g, 0x300, 0x40); a[8] = 0x78u + 4u * g.R(8);
        a[9] = Aligned(g, 0x80, 0x44); a[10] = 0x10u + 4u * g.R(6);
        a[11] = Aligned(g, 0x80, 0x44); a[12] = 0x10u + 4u * g.R(6);
        a[13] = g.W(); a[14] = Frame();
        for (u32 i = 0; i < 3; ++i) {
            a[64 + 4 * i] = g.P(40) ? 0u : Raw(g);
            a[65 + 4 * i] = g.P(40) ? 0u : Raw(g);
            a[66 + 4 * i] = Frame();
        }
        if (g.P(50)) {
            const u32 m = g.R(7);
            a[40] = g.Pk({0x808740u, 0x808740u, 0x807DE0u, 0x74CCC0u, 0x751050u, 0x7F0910u, 0x804F40u, 0x804F40u, 0xE148u, 0xE148u, 0xE248u, 0x7520D0u, 0x7EE4B0u, 0x7F08A0u});
            a[41] = g.R(4);
            if (m == 0) { a[42] = 0; a[43] = 10; a[44] = 0; a[45] = 1; a[46] = a[48 + 10] == kEngine1 ? kEngine2 : kEngine1; }
            else if (m == 1) { a[42] = 0; a[43] = 7; a[44] = 0; a[45] = 0; a[46] = 0x40u + 4u * g.R(0x30); }
            else if (m == 2) { a[42] = 0; a[43] = 8 + g.R(2); a[44] = 0; a[45] = 0; a[46] = g.Pk({0x20u + 4u * g.R(0x30), 0u, 0xFFFFFFFCu}); }
            else if (m == 3) { a[42] = 1; a[43] = 1; a[44] = a[7] + a[8] * g.R(3) + 0x14 + 4u * g.R(2); a[45] = 0; a[46] = g.Pk({0u, RawNZ(g)}); }
            else if (m == 4) { a[42] = 1; a[43] = 0; a[44] = a[0] + 4; a[45] = 0; a[46] = g.Pk({0u, RawNZ(g)}); }
            else if (m == 5) { a[42] = 1; a[43] = 1; a[44] = a[7] + a[8] * g.R(3) + 4; a[45] = 0; a[46] = g.Pk({0u, RawNZ(g)}); }
            else { a[42] = 0; a[43] = 10; a[44] = 0; a[45] = 1; a[46] = g.P(50) ? kEngine1 : kEngine2; }
        }
        break;
    }
    case kAtomFree: {
        // a[4] = +0x14 (entrada de recursos), a[5] = +0x18 (geometria), a[6] = +4 (marco), a[7] = lo que devuelve RpAtomicGetWorld; a[17] = argumento `data` (sin uso)
        a[4] = g.P(40) ? 0u : Raw(g); a[5] = g.P(40) ? 0u : Raw(g); a[6] = Frame(); a[7] = g.Pk({0u, 1u, 0xFFFFFFFFu, RawNZ(g)});
        a[17] = Raw(g);
        if (g.P(50)) {
            const u32 m = g.R(6);
            a[40] = g.Pk({0x808740u, 0x807DE0u, 0x74CCC0u, 0x74CCC0u, 0x751050u, 0x7F0910u, 0x804F40u});
            a[41] = 0;
            if (m == 0) { a[42] = 0; a[43] = 10; a[44] = 0; a[45] = 1; a[46] = a[48 + 10] == kEngine1 ? kEngine2 : kEngine1; }
            else if (m == 1) { a[42] = 0; a[43] = 7; a[44] = 0; a[45] = 0; a[46] = 0x40u + 4u * g.R(0x30); }
            else if (m == 2) { a[42] = 1; a[43] = 0; a[44] = a[0] + 4; a[45] = 0; a[46] = g.Pk({0u, RawNZ(g)}); }       // el marco se lee tras RpGeometryDestroy
            else if (m == 3) { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x18; a[45] = 0; a[46] = g.Pk({0u, RawNZ(g)}); }
            else if (m == 4) { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x14; a[45] = 0; a[46] = g.Pk({0u, RawNZ(g)}); }
            else { a[42] = 0; a[43] = 10; a[44] = 0; a[45] = 1; a[46] = g.P(50) ? kEngine1 : kEngine2; }
        }
        break;
    }
    case kClumpAttach: {
        a[4] = RetVal(g); a[5] = RetVal(g);
        if (g.P(60)) a[4] = g.Pk({0u, 1u, 9u});
        if (g.P(60)) a[5] = g.Pk({0u, 1u, 9u, 0xFFFFFFFFu});
        if (g.P(40)) {
            a[40] = g.Pk({0x7EE450u, 0x751D60u}); a[41] = 0;
            a[42] = 0; a[43] = 8 + g.R(2); a[44] = 0; a[45] = 0; a[46] = Raw(g);
        }
        break;
    }
    case kRender: {
        // a[4] = atomos (0..4); a[5], a[6] = base y separacion en B; por atomo i: a[64 + 4 i] = flags (+2), a[65 + 4 i] = marco (+4), a[66 + 4 i] = callback (funcion falsa 0..3); a[7] = mascara de lo que devuelve cada callback;
        //   a[8] = lo que devuelve RwFrameGetLTM
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u}); a[5] = Aligned(g, 0x300, 0x40); a[6] = 0x78u + 4u * g.R(8);
        a[7] = g.W(); a[8] = g.W();
        for (u32 i = 0; i < 4; ++i) {
            a[64 + 4 * i] = g.Pk({4u, 4u, 5u, 7u, 0u, 1u, 3u, 0xFFu, g.R(256)});
            a[65 + 4 * i] = Frame();
            a[66 + 4 * i] = FS(g.R(4));
        }
        if (g.P(50)) {
            const u32 m = g.R(3);
            const u32 i = g.R(4);
            a[41] = g.R(3);
            if (m == 0) {   // el siguiente enlace del atomo actual se relee tras el callback: queda en la cabeza de la lista (fin del recorrido)
                a[40] = 0xCB10u + g.R(4); a[42] = 2; a[43] = 0; a[44] = 0x40; a[45] = 1;
                a[46] = PS(0, a[0] + 8);
            } else if (m == 1) {   // el callback se lee tras RwFrameGetLTM
                a[40] = 0x7F0990; a[42] = 1; a[43] = 1; a[44] = a[5] + a[6] * i + 0x48; a[45] = 1; a[46] = FS(g.R(4));
            } else {   // banderas de otro atomo
                a[40] = 0xCB10u + g.R(4); a[42] = 1; a[43] = 1; a[44] = a[5] + a[6] * i + 0x2; a[45] = 0; a[46] = g.R(256);
            }
        }
        break;
    }
    case kForAll: {
        // a[4] = objetos de la lista (0..4); a[5], a[6] = base y separacion de los nodos en C; a[7] = mascara de lo que devuelve cada llamada; a[8] = callback (funcion falsa 0..3, o nulo / crudo si la lista esta vacia);
        //   a[9] = `data`; el desplazamiento esta en a[48 + 8] (camaras) o a[48 + 9] (luces)
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 4u}); a[5] = Aligned(g, 0x80, 0x44); a[6] = 0x10u + 4u * g.R(6);
        a[7] = g.W();
        a[8] = (a[4] == 0 && g.P(30)) ? g.Pk({0u, RawNZ(g)}) : FS(g.R(4));
        a[9] = g.Pk({0u, RawNZ(g), PS(1, Aligned(g, 0x40, 0x40))});
        if (g.P(45) && a[4]) {
            const u32 m = g.R(3);
            a[40] = 0xCB20u + g.R(4); a[41] = g.R(a[4]);
            if (m == 0) { a[42] = 0; a[43] = (c.fn == 10) ? 8 : 9; a[44] = 0; a[45] = 0; a[46] = g.Pk({0x20u + 4u * g.R(0x30), 0u, 0xFFFFFFFCu}); }
            else if (m == 1) { a[42] = 3; a[43] = 0; a[44] = 0; a[45] = 0; a[46] = RawNZ(g); }       // el enlace siguiente del nodo actual (el EXE ya lo leyo)
            else { a[42] = 3; a[43] = 0; a[44] = 4; a[45] = 0; a[46] = RawNZ(g); }
        }
        break;
    }
    case kCreate: {
        // a[4] = lo que devuelve RwFreeListAlloc (0 falla, 1 el nodo en A)
        a[4] = g.P(12) ? 0u : 1u;
        if (g.P(45)) {
            const u32 m = g.R(4);
            a[40] = 0x804EF0; a[41] = 0;
            if (m == 0) { a[42] = 1; a[43] = 0; a[44] = a[0] + 3; a[45] = 0; a[46] = g.Pk({0u, 1u, 0xFEu, g.W()}); }
            else if (m == 1) { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x18; a[45] = 0; a[46] = g.Pk({0u, RawNZ(g)}); }
            else if (m == 2) { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x4C; a[45] = 0; a[46] = g.W(); }
            else { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x64; a[45] = 0; a[46] = g.W(); }
        }
        break;
    }
    case kSync: {
        // a[4] = flags del interpolador (+0x4C); a[5] = privateFlags (+3)
        a[4] = g.Pk({0u, 1u, 2u, 3u, 2u, 2u, 6u, 0xFFFFFFFFu, g.W()}); a[5] = g.Pk({0u, 1u, 0xFEu, 0xFFu, g.W()});
        if (g.P(50)) {
            a[40] = 0x7491F0; a[41] = 0; a[42] = 1; a[43] = 0; a[44] = a[0] + 3; a[45] = 0; a[46] = g.Pk({0u, 1u, 0xFEu, g.W()});
        }
        break;
    }
    case kSetGeom: {
        // a[4] = geometria actual del atomo (0 nula, 1 la 1a de B, 2 la 2a, 3 cruda a[13]); a[5] = geometria argumento (0 nula, 1 la 1a, 2 la 2a, 3 cruda a[14], 4 la actual); a[6] = banderas; a[7] = marco (+4);
        //   a[8], a[9] = geometrias en B; a[10], a[11] = morph target 0 de cada una en D (esfera: +4); a[12] = lo que devuelve RpAtomicGetWorld; a[64..67] = esfera local inicial; a[68..71], a[72..75] = esferas
        a[4] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u}); a[5] = g.Pk({0u, 1u, 2u, 3u, 4u, 4u, 1u, 2u});
        a[6] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFFFEu, g.W()});
        if (a[5] == 3 && !(a[6] & 1)) a[6] |= 1;   // la geometria cruda solo se acepta si no se desreferencia (banderas con el bit 0)
        a[7] = Frame(); a[12] = g.Pk({0u, 1u, 0xFFFFFFFFu, RawNZ(g)});
        a[8] = 0x40u + 4u * g.R(0x100); a[9] = 0x600u + 4u * g.R(0x100);
        a[10] = 0x40u + 4u * g.R(0x100); a[11] = 0x600u + 4u * g.R(0x100);
        a[13] = RawNZ(g); a[14] = RawNZ(g);
        for (u32 k = 0; k < 12; ++k) a[64 + k] = Flt(g);
        if (g.P(50)) {
            const u32 m = g.R(5);
            a[41] = 0; a[45] = 0;
            if (m == 0) { a[40] = 0x74CCC0; a[42] = 1; a[43] = 1; a[44] = a[8] + 0x5C; a[45] = 1; a[46] = PS(3, a[11]); }          // la matriz de morph targets se lee tras RpGeometryDestroy
            else if (m == 1) { a[40] = 0x74CCC0; a[42] = 1; a[43] = 1; a[44] = a[9] + 0x5C; a[45] = 1; a[46] = PS(3, a[10]); }
            else if (m == 2) { a[40] = 0x74CCB0; a[42] = 1; a[43] = 0; a[44] = a[0] + 0x18; a[46] = g.Pk({0u, RawNZ(g)}); }       // la geometria actual se relee tras AddRef
            else if (m == 3) { a[40] = g.Pk({0x74CCC0u, 0x74CCB0u}); a[42] = 1; a[43] = 0; a[44] = a[0] + 4; a[46] = g.Pk({0u, RawNZ(g)}); }   // el marco se lee al final
            else { a[40] = 0x74CCC0; a[42] = 1; a[43] = 0; a[44] = a[0] + 0x1C + 4u * g.R(4); a[46] = Flt(g); }                       // la esfera local se sobrescribe antes de la copia
        }
        break;
    }
    case kClone: {
        // el atomo origen esta en A, el clon (lo que devuelve RpAtomicCreate) en B en a[4]; a[5] = geometria del origen (0 nula, 1 la de C a[10], 2 igual que la geometria cruda del clon); a[6] = geometria
        //   inicial del clon (0 nula, 1 cruda a[13]); a[7] = la creacion falla (1); a[8] = marco; a[9] = lo que devuelve RpAtomicGetWorld; a[11] = morph target 0 en D; a[64..67] = esfera de morph target 0
        a[7] = g.P(10) ? 1u : 0u;
        a[4] = Aligned(g, 0x80, 0x40);
        a[5] = g.Pk({0u, 1u, 1u, 2u}); a[6] = g.Pk({0u, 1u, 1u, 0u});
        if (a[5] == 2 && a[6] == 0) a[6] = 1;
        a[13] = RawNZ(g);
        a[8] = Frame(); a[9] = g.Pk({0u, 1u, 0xFFFFFFFFu, RawNZ(g)});
        a[10] = Aligned(g, 0x80, 0x40); a[11] = 0x40u + 4u * g.R(0x100);
        for (u32 k = 0; k < 4; ++k) a[64 + k] = Flt(g);
        if (g.P(50)) {
            u32 m = g.R(4);
            if (m == 1 && a[5] == 2) m = 0;   // la geometria cruda del origen solo se acepta si es la del clon: no se cambia la del clon
            a[41] = 0; a[45] = 0;
            a[40] = 0x804EF0;
            if (m == 0) { a[42] = 1; a[43] = 1; a[44] = a[4] + 4; a[46] = RawNZ(g); }                                         // el marco del clon se lee tras SetFrame
            else if (m == 1) { a[42] = 1; a[43] = 1; a[44] = a[4] + 0x18; a[46] = g.Pk({0u, RawNZ(g)}); }                     // la geometria del clon se lee tras SetFrame
            else if (m == 2) { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x18; a[45] = 1; a[46] = g.Pk({0u, PS(2, a[10])}); }      // la del origen se lee tras SetFrame
            else { a[42] = 1; a[43] = 0; a[44] = a[0] + 0x1C + 4u * g.R(4); a[46] = Flt(g); }                                 // la esfera local del origen se lee al final
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
