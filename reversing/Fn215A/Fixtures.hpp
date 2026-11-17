#pragma once
// Fixtures del banco Fn215A (lote 215A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (sin hook antes del lote; las clases salen del desensamblado, ver README.md y fixture_oracle.py). 22 entradas.
//   idx  direccion  funcion real                                                                    forma
//   0    0x4D5F20   CAnimBlendClumpData: destructor con borrado                                      thiscall(banderas) `ret 4`: 0x4CF100 y, con el bit 0 del byte, operator delete 0x8214BD; devuelve this
//   1    0x4D6240   callback de busqueda por nombre (skinned)                                        cdecl(fd, dato): 0x4D56F0(tag de 32 bits), si hay nombre _stricmp(dato, nombre); 0 -> [0xB5F87C] = fd
//   2    0x4D6270   callback de busqueda por nombre (no skinned)                                     cdecl(fd, dato): 0x72FB30(frame), _stricmp(dato, nombre) sin comprobar nulo
//   3    0x4D6310   callback de busqueda por clave (skinned)                                         cdecl(fd, dato): 0x4D56F0(tag), si hay nombre 0x53CF30(nombre) == dato
//   4    0x4D6340   callback de busqueda por clave (no skinned)                                      cdecl(fd, dato): 0x72FB30(frame), 0x53CF30(nombre) == dato sin comprobar nulo
//   5    0x4D6430   callback de relleno del array de frames (no skinned)                             cdecl(fd, array): array[0x732A20(frame)] = fd
//   6    0x46A900   CScriptsForBrains::SwitchAllObjectBrainsWithThisID                                thiscall(ID, estado) `ret 8`
//   7    0x46A930   CScriptsForBrains::AddNewScriptBrain                                              thiscall(6 argumentos) `ret 0x18`, x87 (fcomp)
//   8    0x46A9C0   CScriptsForBrains::AddNewStreamedScriptBrainForCodeUse                              thiscall(word, char*, byte) `ret 0xC`
//   9-21 0x4A4C20 ... 0x4A57C0  constructores de FxInfoEmSize_c ... FxInfoAttractLine_c               thiscall() sin argumentos
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F215AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;
constexpr unsigned kArenas = 4;
constexpr unsigned kSlots = 128;        // dwords de entrada por caso

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
// Bloque grande de los estados de los pools (los constructores escriben 0x80 en cada estado): puntero que devuelve el falso de operator new[] cuando la capacidad es positiva
constexpr std::size_t kBig = 0x800000 + 0x200;

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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kSlots> a{};
    std::array<u32, 4> sd{};    // semillas de las arenas A..D
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    bool Pm(u32 permille) { return Rand(n, nk()) % 1000 < permille; }
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

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
// La pila del hilo del banco (cerca de 0x00EB0000) se etiqueta aparte (Tag): un puntero crudo que caiga ahi se confundiria con un objeto de la pila, asi que ninguno se genera en esa zona
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
// Un float uniforme en [lo, hi] como bits (la aritmetica en double es determinista; el oraculo solo lee los bits)
inline u32 UniformBits(Gen& g, double lo, double hi) {
    const double u = double(g.R(1u << 24)) / 16777216.0;
    const float v = float(lo + u * (hi - lo));
    return std::bit_cast<u32>(v);
}
// Mezcla: la mitad uniforme en [lo, hi], una cuarta parte de valores especiales de la lista y otra de FloatBits
inline u32 Mixed(Gen& g, double lo, double hi, std::initializer_list<u32> specials) {
    const u32 r = g.R(8);
    if (r < 4) return UniformBits(g, lo, hi);
    if (r < 6) return g.Pk(specials);
    return FloatBits(g);
}


// Clases de funcion (forma)
enum Kind : u32 { kDel, kNameSkin, kNameNon, kKeySkin, kKeyNon, kFill, kSwitch, kAddBrain, kAddStr, kFx };
inline constexpr std::array<Kind, kFunctions> kKind{
    kDel, kNameSkin, kNameNon, kKeySkin, kKeyNon, kFill, kSwitch, kAddBrain, kAddStr,
    kFx, kFx, kFx, kFx, kFx, kFx, kFx, kFx, kFx, kFx, kFx, kFx, kFx};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{
    0x4D5F20, 0x4D6240, 0x4D6270, 0x4D6310, 0x4D6340, 0x4D6430, 0x46A900, 0x46A930, 0x46A9C0,
    0x4A4C20, 0x4A4D00, 0x4A4DC0, 0x4A4E90, 0x4A4F50, 0x4A5010, 0x4A50E0, 0x4A51B0, 0x4A5270, 0x4A5450, 0x4A5550, 0x4A5650, 0x4A57C0};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{
    4, 0x18, 0x18, 0x18, 0x18, 0x18, 0x578, 0x578, 0x5C0,
    0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18};
// Constructores de FxInfo*: tipo (word +4), contador (byte +0xE; tambien el tamano pedido a GetMem es 4 * contador, alineacion 4), vtable que escribe el EXE en +0 y la del interp (+8)
inline constexpr std::array<u32, 13> kFxType{0x1004, 0x1008, 0x1010, 0x1020, 0x1040, 0x1080, 0x1100, 0x1200, 0x2001, 0x2002, 0x2004, 0x2008, 0x2010};
inline constexpr std::array<u32, 13> kFxCount{7, 2, 3, 2, 2, 3, 4, 2, 1, 3, 1, 4, 7};
inline constexpr std::array<u32, 13> kFxVt{0x85A7E8, 0x85A7F4, 0x85A800, 0x85A80C, 0x85A818, 0x85A824, 0x85A830, 0x85A83C, 0x85A848, 0x85A854, 0x85A860, 0x85A86C, 0x85A878};
inline constexpr std::array<u32, 13> kFxInterpVt{0x85A980, 0x85A980, 0x85A980, 0x85A970, 0x85A978, 0x85A970, 0x85A970, 0x85A970, 0x85A980, 0x85A980, 0x85A980, 0x85A980, 0x85A980};

// Caracter k del nombre de AddNewStreamedScriptBrainForCodeUse (1..255, nunca NUL): derivado de la semilla del caso (el oraculo lo reimplementa)
inline u32 StrChar(u32 seed, u32 k) { return 1u + Rand(seed, 0x500u + k) % 255u; }

// ------------------------------------------------------------------------------------------------ disposicion de las entradas (a[])
//   a[0] = desplazamiento del objeto principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]]
//   kDel (0): a[4] = banderas (dword con ruido; solo el bit 0 del byte bajo)
//   callbacks (1-5): el AnimBlendFrameData fd esta en A+a[0] (+0x10 = frame, +0x14 = tag de 32 bits); a[4] = tag; a[5..7] = frame (modo 0 nulo, 1 crudo = a[7], 2 arena D en a[6]);
//         a[8..10] = lo que devuelve el nombre falso (modo, desplazamiento en C, crudo); a[11..13] = dato (modo, desplazamiento en B, crudo); a[14] = lo que devuelve _stricmp falso; a[15] = lo que devuelve
//         GetUppercaseKey falso; a[16] = 1: el dato de las busquedas por clave es a[15] (coincide); a[17] = valor inicial de [0xB5F87C]. kFill (5): a[11..13] = array (modo 2: desplazamiento en B, con signo),
//         a[14] = indice que devuelve 0x732A20 (dword; el producto por 4 se trunca a 32 bits como en el EXE)
//   kSwitch (6): a[4] = ID (dword), a[5] = estado (dword), a[6] = registros forzados (0..8), a[7..14] = sus indices (0..69, ID de grupo +3 = ID)
//   kAddBrain (7): a[4..9] = ImgIndex, Model, priority, attachType, Type, Radius (bits); a[10] = primer registro libre; a[16 + i] = word de +0 del registro i (0xFFFF = libre)
//   kAddStr (8): a[4] = word, a[5] = cadena (2 = arena B, 3 = arena A), a[6] = desplazamiento (en A, relativo al array), a[7] = longitud, a[8] = attachtype, a[10] = primer libre, a[16 + i] como arriba
//   kFx (9-21): a[4..6] = lo que devuelve el falso de GetMem (modo, desplazamiento en D, crudo)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.sd[0] = c.seed * 13u + 50u; c.sd[1] = c.sd[0] + 1u; c.sd[2] = c.sd[0] + 2u; c.sd[3] = c.sd[0] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x887D5Cu, 0x870114u});
    auto& a = c.a;
    // triple (modo, desplazamiento alineado, crudo): modo 0 nulo, 1 crudo, 2 arena
    const auto ptrTriple = [&](u32 at, u32 nullPct) {
        const u32 m = g.P(nullPct) ? 0u : (g.P(50) ? 1u : 2u);
        a[at] = m; a[at + 1] = AuxOffset(g, 0x40); a[at + 2] = Raw(g);
    };
    switch (kKind[c.fn]) {
    case kDel:
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 0xFEu, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80000001u, g.W(), g.W()});
        break;
    case kNameSkin: case kNameNon: case kKeySkin: case kKeyNon: case kFill: {
        a[4] = g.Pk({0u, 1u, 0x7Fu, 0xFFFFu, 0x10000u, 0x8000u, 0xFFFF8000u, 0xFFFFFFFFu, 0x80000000u, 0x1B0u, g.R(0x200), g.R(0x20000), g.W(), g.W()});
        ptrTriple(5, 12);
        ptrTriple(8, 25);
        ptrTriple(11, 12);
        a[14] = g.Pk({0u, 0u, 0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.W()});
        a[15] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), g.W(), 0x3E8u});
        a[16] = g.P(45) ? 1u : 0u;
        a[17] = g.Pk({0u, 0xFFFFFFFFu, Raw(g), Raw(g), Raw(g)});
        if (kKind[c.fn] == kFill) {
            // dato = arena B en un desplazamiento p (con signo); el indice lleva a una direccion dentro de la arena (guardas incluidas); el producto por 4 se trunca a 32 bits
            const i32 eoff = i32(g.R(u32(kWin) + 2 * u32(kGuard) - 3)) - i32(kGuard);                        // destino [-guarda, ventana + guarda - 4]
            const i32 shiftMag = i32(g.Pk({0u, 0u, 1u, 2u, 3u, 5u, 16u, 100u, 0x3FFu, 0x400u, g.R(0x1000)}));  // en dwords
            const i32 k = g.P(50) ? shiftMag : -shiftMag;
            const i32 p = eoff - 4 * k;
            const u32 wrap = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
            a[11] = 2u; a[12] = u32(p); a[13] = 0u;
            a[14] = u32(k) + (wrap << 30);
        }
        break;
    }
    case kSwitch: {
        a[4] = g.Pk({0u, 1u, 2u, 3u, 0x7Fu, 0x80u, 0xFFu, 0xCDu, 0x100u, 0x101u, 0xFFFFFF01u, g.R(256), g.R(256), g.W()});
        a[5] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0x7Fu, 0x80u, 0xFFFFFF00u, g.R(256), g.W()});
        a[6] = g.Pk({0u, 1u, 2u, 3u, 4u, 8u, g.R(9)});
        for (u32 i = 0; i < 8; ++i) a[7 + i] = g.Pk({0u, 69u, 68u, 1u, g.R(70), g.R(70), g.R(70)});
        break;
    }
    case kAddBrain:
    case kAddStr: {
        const bool brain = kKind[c.fn] == kAddBrain;
        const u32 free = g.Pk({0u, 0u, 1u, 2u, 3u, 35u, 68u, 69u, 70u, 70u, g.R(71), g.R(71), g.R(71)});
        a[10] = free;
        for (u32 i = 0; i < 70; ++i) {
            u32 w;
            if (i < free) w = g.Pk({0u, 1u, 0xFFFEu, 0x7FFFu, 0x8000u, 0x100u, g.R(0xFFFF), g.R(0xFFFF), g.R(0xFFFF)});
            else if (i == free) w = 0xFFFFu;
            else w = g.P(40) ? 0xFFFFu : g.R(0xFFFF);
            a[16 + i] = w;
        }
        if (brain) {
            a[4] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, 0xFFFF0000u, g.R(0x10000), g.W(), g.W()});
            a[5] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, g.R(0x10000), g.W(), g.W()});
            a[6] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, g.R(0x10000), g.W(), g.W()});
            a[7] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x80u, g.R(256), g.W()});
            a[8] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x80u, g.R(256), g.W()});
            a[9] = Mixed(g, -2.0, 20.0, {0u, 0x80000000u, 0x3F800000u, 0x40A00000u, 0xBF800000u, 0x00000001u, 0x80000001u, 0x7F800000u, 0xFF800000u});
        } else {
            a[4] = g.Pk({0u, 1u, 0xFFFFu, 0x10000u, 0xFFFF0000u, g.R(0x10000), g.W(), g.W()});
            const bool inA = g.P(15);
            a[5] = inA ? 3u : 2u;
            a[6] = inA ? 0x580u + 4 * g.R(4) : AuxOffset(g, 0x34) + g.R(4);   // en A: relativo al array (detras de el, nunca antes del destino)
            a[7] = g.Pk({0u, 1u, 2u, 3u, 7u, 8u, 9u, 20u, 39u, 40u, g.R(41), g.R(41)});
            a[8] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x80u, g.R(256), g.W()});
        }
        break;
    }
    case kFx: {
        const u32 m = g.P(10) ? 0u : (g.P(50) ? 1u : 2u);
        a[4] = m; a[5] = AuxOffset(g, 0x40); a[6] = Raw(g);
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
    std::size_t used = c.a.size();
    while (used > 0 && c.a[used - 1] == 0) --used;
    j.A("a", std::vector<u32>(c.a.begin(), c.a.begin() + used));
    j.A("sd", std::vector<u32>(c.sd.begin(), c.sd.end()));
    return j.End();
}
}
