#pragma once
// Fixtures del banco Fn209A (lote 209A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (sin hook antes del lote; las clases del listado salen del desensamblado, ver README.md y fixture_oracle.py). 29 entradas.
//   idx  direccion  nombre real                                            forma
//    0   0x4F3AE0   std::fill (vector de punteros)                         cdecl(first,last,&valor) `ret`: [first,last) = *valor (valor releido)
//    1   0x4F3B00   std::copy_backward                                      cdecl(out,first,last,finDestino) `ret`: copia hacia atras; *out = destino final
//    2   0x4F3B30   std::uninitialized_fill_n                               cdecl(dest,n,&valor,x,y) `ret`: n sin signo; dest nulo se salta
//    3   0x4F3CE0   envoltorio de uninitialized_copy                        thiscall(first,last,dest) `ret 0Ch`: copia hacia delante; devuelve el final del destino
//    4   0x4F3D80   fill_n que devuelve el final                            thiscall(dest,n,&valor) `ret 0Ch`: devuelve dest + n*4
//    5   0x4F3F30   vaciado de vector (A)                                   thiscall() `ret`: operator delete([+4]) si no es nulo; +4, +8, +0xC = 0
//    6   0x4F4030   vaciado de vector (B)                                   igual
//    7   0x4F4320   vector<T*>::insert(pos, n, valor)                       thiscall(pos,n,&valor) `ret 0Ch`
//    8   0x4F3BA0   std::string::_Tidy                                      thiscall() `ret`
//    9   0x4F3BD0   std::string::_Grow                                      thiscall(nuevoTamano, vaciar) `ret 8`
//   10   0x4F3DB0   std::string::erase(pos, n)                              thiscall(pos,n) `ret 8`
//   11   0x4F3E80   std::string::assign(str, pos, n)                        thiscall(str,pos,n) `ret 0Ch`
//   12   0x4F3F60   std::string::assign(char*, n)                           thiscall(texto,n) `ret 8`
//   13   0x4F4170   std::string::string(const char*)                        thiscall(texto) `ret 4`
//   14   0x4F3D10   cuerpo de ~runtime_error (A)                            thiscall() `ret`
//   15   0x4F4130   cuerpo de ~runtime_error (B)                            thiscall() `ret`
//   16   0x4F3D50   runtime_error::what                                     thiscall() `ret`
//   17   0x4F3D60   destructor con borrado de runtime_error (A)             thiscall(banderas) `ret 4`
//   18   0x4F4110   destructor con borrado de length_error (B)              thiscall(banderas) `ret 4`
//   19   0x4F4090   runtime_error(const string&)                            thiscall(mensaje) `ret 4`
//   20   0x4F4230   runtime_error(const runtime_error&)                      thiscall(otro) `ret 4`
//   21   0x4F41B0   vector::_Xlen (lanza length_error)                       thiscall() no vuelve
//   22   0x4F4CA0   CVehicle: bIsHandbrakeOn                                 thiscall() `ret`: solo AL
//   23   0x4F4CE0   CVehicle: alarma activa                                  thiscall() `ret`: solo AL
//   24   0x66BA70   CPatrolRoute destructor con borrado                      thiscall(banderas) `ret 4`
//   25   0x66BA90   CTaskComplexFollowPatrolRoute::MakeAbortable             thiscall(ped,prioridad,evento) `ret 0Ch`
//   26   0x66BB00   CTaskComplexFollowPatrolRoute::ControlSubTask            thiscall(ped) `ret 4`
//   27   0x6709F0   CTaskComplexFollowPatrolRoute cuerpo del destructor      thiscall() `ret`
//   28   0x66D300   CTaskSimpleSlideToCoord::Clone (V)                       thiscall() `ret`
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

namespace F209AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 29;
constexpr unsigned kArenas = 4;

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
    std::array<u32, 40> a{};
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

// Clases de funcion (forma)
enum Kind : u32 { kFill, kCopyBack, kFillN, kCopyWrap, kFillNEnd, kVecTidy, kInsert, kStrTidy, kGrow, kErase, kAssignStr, kAssignChars, kFromChars, kRtDtorBody, kRtWhat, kRtScalarDel, kRtFromStr, kRtCopy,
                  kXlen, kHandbrake, kAlarm, kPatrolSD, kAbort, kControl, kFollowDtor, kClone };
inline constexpr std::array<Kind, kFunctions> kKind{kFill, kCopyBack, kFillN, kCopyWrap, kFillNEnd, kVecTidy, kVecTidy, kInsert, kStrTidy, kGrow, kErase, kAssignStr, kAssignChars, kFromChars,
                                                    kRtDtorBody, kRtDtorBody, kRtWhat, kRtScalarDel, kRtScalarDel, kRtFromStr, kRtCopy, kXlen, kHandbrake, kAlarm, kPatrolSD, kAbort, kControl, kFollowDtor, kClone};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4F3AE0, 0x4F3B00, 0x4F3B30, 0x4F3CE0, 0x4F3D80, 0x4F3F30, 0x4F4030, 0x4F4320, 0x4F3BA0, 0x4F3BD0, 0x4F3DB0, 0x4F3E80, 0x4F3F60, 0x4F4170,
                                                    0x4F3D10, 0x4F4130, 0x4F3D50, 0x4F3D60, 0x4F4110, 0x4F4090, 0x4F4230, 0x4F41B0, 0x4F4CA0, 0x4F4CE0, 0x66BA70, 0x66BA90, 0x66BB00, 0x6709F0, 0x66D300};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0x24, 4, 0x24, 0x24, 0x24, 0x10, 0x10, 0x10, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 4, 0x430, 0x460, 4, 0x30, 0x30, 0x30, 0x80};

// Estado de un std::string (disposicion de MSVC 9: +0 asignador sin usar, +4 puntero o buffer de 16 bytes, +0x14 longitud, +0x18 capacidad). Seis valores en a[base..base+5]:
//   mode 0: buffer interno (capacidad 0xF, longitud <= 15); 1: en el monton (capacidad >= 16, el bloque vive en la arena de bloques); 2: en el monton con puntero CRUDO (nunca se lee el bloque);
//   3: capacidad pequena distinta de 0xF (< 15, buffer interno); size = longitud; res = capacidad; blk = desplazamiento del bloque; seed = semilla del texto; raw = puntero crudo (modo 2)
struct StrSpec { u32 mode, size, res, blk, seed, raw; };
inline StrSpec GetSpec(const C& c, unsigned base) { return {c.a[base], c.a[base + 1], c.a[base + 2], c.a[base + 3], c.a[base + 4], c.a[base + 5]}; }
inline u8 StrChar(u32 seed, u32 i) { return u8(0x61 + Rand(seed, i) % 26); }
inline u32 StrCap(const StrSpec& s) { return s.mode == 0 ? 15u : s.res; }

// Genera un std::string; `modes` = mascara de modos permitidos (bit 0..3); `fixedSize` < 0xFFFFFFFF fuerza la longitud
inline StrSpec MakeSpec(Gen& g, unsigned modes) {
    StrSpec s{};
    u32 mode;
    do { mode = g.Pk({0u, 0u, 0u, 1u, 1u, 1u, 1u, 2u, 3u}); } while (!(modes >> mode & 1));
    s.mode = mode;
    s.seed = g.W(); s.raw = AvoidStack(g.Pk({0x00400000u + g.R(0x800000), g.W() | 1u, 0xFFFFFFF0u, 0x1000u}));
    s.blk = 4 * g.R((u32(kWin) - 0x240) / 4);
    switch (mode) {
    case 0: s.res = 0xF; s.size = g.Pk({0u, 0u, 1u, 2u, 7u, 14u, 15u, g.R(16)}); break;
    case 3: s.res = 1 + g.R(13); s.size = g.Pk({0u, s.res, s.res, g.R(s.res + 1)}); break;
    default:
        s.res = g.Pk({16u, 16u, 17u, 20u, 31u, 32u, 47u, 48u, 64u, 100u, 127u, 128u, 200u, 16u + g.R(180)});
        s.size = g.Pk({0u, 1u, 15u, 16u, s.res - 1, s.res, s.res, g.R(s.res + 1), g.R(s.res + 1)});
        if (s.size > s.res) s.size = s.res;
        break;
    }
    return s;
}
inline void PutSpec(const StrSpec& s, u32* out) { out[0] = s.mode; out[1] = s.size; out[2] = s.res; out[3] = s.blk; out[4] = s.seed; out[5] = s.raw; }
// Hace que la capacidad del string alcance `need` caracteres (mas el terminador): si no, lo pasa al monton con la capacidad justa mas un margen
inline void EnsureCapacity(Gen& g, StrSpec& s, u32 need) {
    if (s.mode == 2) { s.mode = 1; }
    if (StrCap(s) >= need && s.mode != 3) return;
    if (s.mode == 3 || s.mode == 0) {
        if (need <= 15) { s.mode = 0; s.res = 0xF; if (s.size > 15) s.size = 15; return; }
        s.mode = 1;
    }
    if (s.res < need) s.res = need + g.R(8);
    if (s.res < 16) s.res = 16;
    if (s.size > s.res) s.size = s.res;
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento del objeto principal en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   kFill (0): a[4] = n (elementos), a[5] = desplazamiento del valor en B, a[6] = valor
//   kCopyBack (1): a[4] = n, a[5] = desplazamiento del origen, a[6] = modo (0 origen en C y destino en D; 1 origen y destino en D solapados), a[7] = desplazamiento del FINAL del destino en D
//   kFillN (2), kFillNEnd (4): a[4] = n, a[5] = desplazamiento del valor en B, a[6] = valor, a[7] = destino (0 en A, 1 nulo (n = 1), 2 crudo (n = 0)), a[8], a[9] = argumentos de relleno de la pila (fill_n)
//   kCopyWrap (3): a[4] = n, a[5] = desplazamiento del origen en C, a[6] = modo (0 destino en A, 1 destino en C solapado con delta de a[7] elementos con signo), a[8] = 1: destino nulo (n = 1)
//   kVecTidy (5, 6): a[4] = modo del puntero (0 nulo, 1 en B, 2 crudo), a[5] = desplazamiento en B, a[6] = puntero crudo, a[7], a[8] = +8 y +0xC previos
//   kInsert (7): a[4] = 0 vector vacio (inicio nulo) / 1 con bloque, a[5] = capacidad, a[6] = tamano, a[7] = desplazamiento de los elementos en B, a[8] = indice de pos, a[9] = n, a[10] = valor,
//         a[11] = desplazamiento del valor en C, a[12] = desplazamiento del bloque nuevo en D, a[13] = EAX del falso de 0x4F3CE0
//   kStrTidy (8): a[4..9] = string; kGrow (9): a[4..9] = string, a[10] = nuevoTamano, a[11] = vaciar (dword con ruido)
//   kErase (10): a[4..9] = string, a[10] = pos, a[11] = n
//   kAssignStr (11): a[4..9] = string propio (bloque en B), a[10..15] = el otro string (objeto en C a[20], bloque en D), a[16] = pos, a[17] = n, a[18] = 1 misma cadena, a[19] = EAX del falso de Grow
//   kAssignChars (12): a[4..9] = string propio (bloque en B), a[10] = modo del texto (0 fuera, en C a[11]; 1 dentro del propio buffer; 2 justo al final; 3 justo antes; 4 final + 1), a[11], a[12] = n, a[13] = EAX del falso de Grow
//   kFromChars (13): a[4] = longitud, a[5] = desplazamiento del texto en C, a[6] = semilla del texto
//   kRtDtorBody (14, 15), kRtWhat (16): a[4..9] = string embebido (en +0xC; bloque en B), a[10] = dword previo en +0
//   kRtScalarDel (17, 18): como kRtDtorBody mas a[11] = banderas (dword con ruido)
//   kRtFromStr (19): a[4..9] = el mensaje (objeto en C a[10], bloque en D)
//   kRtCopy (20): a[4..9] = string del otro runtime_error (objeto en C a[10], su string en +0xC, bloque en D)
//   kXlen (21): nada
//   kHandbrake (22): a[4] = byte +0x428; kAlarm (23): a[4] = word +0x45C, a[5] = byte +0x36
//   kPatrolSD (24): a[4] = banderas
//   kAbort (25): a[4] = subtarea en B, a[5] = vtabla en C, a[6] = ruta en D, a[7] = contador previo, a[8] = banderas (+0x20), a[9] = ped en B, a[10] = 1 con matriz, a[11] = matriz en C, a[12] = prioridad,
//         a[13] = evento (crudo), a[14] = EAX del falso de MakeAbortable de la subtarea (AL 0 o 1)
//   kControl (26): a[5] = vtabla en C, a[8] = banderas (+0x20), a[9] = subtarea (+8, cruda), a[10] = ped (crudo), a[14] = EAX del falso de CreateFirstSubTask
//   kFollowDtor (27): a[4] = ruta (0 nula, 1 en D, 2 cruda), a[5] = desplazamiento en D, a[6] = puntero crudo
//   kClone (28): a[4] = dword +0x78, a[5..7] = posicion +0x64..+0x6C, a[8] = rotacion +0x70, a[9] = velocidad +0x74, a[10] = +0x5C, a[11] = +0x38, a[12] = +0x40, a[13] = byte +0xC, a[14] = 0 sin memoria / 1 con memoria,
//         a[15] = desplazamiento de la memoria nueva en D
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x887D5Cu, 0x870114u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kFill:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 8u, 8u});
        a[5] = AuxOffset(g, 4);
        a[6] = Raw(g);
        break;
    case kCopyBack: {
        const u32 cnt = a[4] = g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 6u, 8u});
        a[6] = g.Pk({0u, 0u, 1u});
        if (a[6] == 0) {
            a[5] = AuxOffset(g, 0x28) + (g.P(25) ? g.R(4) : 0u);
            a[7] = 4 * cnt + 4 * g.R((u32(kWin) - 4 * cnt - 8) / 4) + (g.P(20) ? g.R(4) : 0u);
        } else {
            a[5] = 0x40 + 4 * g.R(0x380) + (g.P(25) ? g.R(4) : 0u);
            const u32 delta = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFDu, 0xFFFFFFFCu});
            a[7] = a[5] + 4 * cnt + 4 * delta;
        }
        break;
    }
    case kFillN:
    case kFillNEnd:
        a[4] = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 8u});
        a[5] = AuxOffset(g, 4);
        a[6] = Raw(g);
        a[7] = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 2u});
        if (a[7] == 1) a[4] = 1;
        if (a[7] == 2) a[4] = 0;
        a[8] = AvoidStack(g.W()); a[9] = g.W();
        break;
    case kCopyWrap: {
        const u32 cnt = a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 8u});
        a[5] = 0x40 + 4 * g.R(0x380) + (g.P(25) ? g.R(4) : 0u);
        a[6] = g.Pk({0u, 0u, 1u});
        a[7] = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFDu, 0xFFFFFFFCu});
        a[8] = g.P(6) ? 1u : 0u;
        if (a[8] == 1) { a[4] = 1; a[6] = 0; }
        (void)cnt;
        break;
    }
    case kVecTidy:
        a[4] = g.Pk({0u, 1u, 1u, 2u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = Raw(g);
        a[7] = g.W(); a[8] = g.W();
        break;
    case kInsert: {
        a[4] = g.P(15) ? 0u : 1u;
        const u32 cap = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 10u, 12u});
        a[5] = a[4] ? cap : 0u;
        a[6] = a[4] ? g.Pk({0u, cap, cap, g.R(cap + 1), g.R(cap + 1)}) : 0u;
        a[7] = AuxOffset(g, 0x40);
        const u32 size = a[6];
        a[8] = g.Pk({0u, size, size, g.R(size + 1), g.R(size + 1)});
        u32 count = g.Pk({0u, 1u, 1u, 1u, 2u, 2u, 3u, 4u, 5u, 7u, 10u, 12u});
        if (g.P(5)) count = g.Pk({0x3FFFFFFFu - size + 1u, 0x3FFFFFFFu - size + 2u, 0xFFFFFFFFu, 0x40000000u, 0x3FFFFFFFu - size, 0x80000000u, 0x3FFFFFFFu - size - 1u});
        if (count > 64u && count <= 0x3FFFFFFFu - size) a[8] = size;   // sin cola que copiar: el destino de la copia de la cola seria una direccion arbitraria
        if (!a[4]) a[8] = 0;
        a[9] = count;
        a[10] = Raw(g);
        a[11] = AuxOffset(g, 4);
        a[12] = AuxOffset(g, 0x100);
        a[13] = Raw(g);
        break;
    }
    case kStrTidy: {
        const StrSpec s = MakeSpec(g, 0xF);
        PutSpec(s, &a[4]);
        break;
    }
    case kGrow: {
        const StrSpec s = MakeSpec(g, 0xB);
        PutSpec(s, &a[4]);
        const u32 res = StrCap(s);
        u32 ns = g.Pk({0u, 0u, 1u, 2u, s.size, s.size + 1u, res, res + 1u, 15u, 16u, 17u, g.R(300), g.R(300), 31u, 32u});
        if (g.P(4)) ns = g.Pk({0xFFFFFFFEu, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFDu});
        a[10] = ns;
        a[11] = g.Pk({0u, 1u, 1u, 0xFFFFFF00u, 0xFFFFFF01u, 0x100u, 0x101u, g.W()});
        break;
    }
    case kErase: {
        const StrSpec s = MakeSpec(g, 0xB);
        PutSpec(s, &a[4]);
        u32 pos = g.Pk({0u, 0u, 1u, s.size, s.size, s.size - (s.size ? 1u : 0u), g.R(s.size + 1), g.R(s.size + 1), g.R(s.size + 1)});
        if (g.P(5)) pos = g.Pk({s.size + 1u, s.size + 5u, 0xFFFFFFFFu, 0x80000000u});
        a[10] = pos;
        const u32 rest = s.size >= pos ? s.size - pos : 0u;
        a[11] = g.Pk({0u, 1u, 2u, rest, rest, rest + 1u, 0xFFFFFFFFu, g.R(40), g.R(40)});
        break;
    }
    case kAssignStr: {
        StrSpec s = MakeSpec(g, 0xB);
        const StrSpec t = MakeSpec(g, 0xB);
        const u32 same = g.P(12) ? 1u : 0u;
        u32 pos = g.Pk({0u, 0u, 1u, t.size, t.size / 2, g.R(t.size + 1), g.R(t.size + 1)});
        if (g.P(5)) pos = g.Pk({t.size + 1u, t.size + 7u, 0xFFFFFFFFu});
        else if (same) pos = g.Pk({0u, s.size, s.size / 2, g.R(s.size + 1)});
        const u32 base = same ? s.size : t.size;
        const u32 rest = base >= pos ? base - pos : 0u;
        const u32 cnt = g.Pk({0u, 1u, 2u, rest, rest, rest + 1u, 0xFFFFFFFFu, g.R(30)});
        const u32 grow = g.Pk({0u, 1u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, 0x12345602u, g.W()});
        const u32 len = cnt < rest ? cnt : rest;
        if (!same && (grow & 0xFF) != 0 && base >= pos) EnsureCapacity(g, s, len);
        PutSpec(s, &a[4]); PutSpec(t, &a[10]);
        a[16] = pos; a[17] = cnt; a[18] = same; a[19] = grow;
        a[20] = AuxOffset(g, 0x1C);
        break;
    }
    case kAssignChars: {
        StrSpec s = MakeSpec(g, 0x3);
        u32 tm = g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u});
        if (tm == 1 && s.size == 0) tm = 2;
        const u32 cnt = g.Pk({0u, 1u, 3u, 4u, 5u, 8u, 15u, 16u, 31u, 40u, 64u});
        const u32 grow = g.Pk({0u, 1u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, 0x12345602u, g.W()});
        if (tm != 1 && (grow & 0xFF) != 0) EnsureCapacity(g, s, cnt);
        PutSpec(s, &a[4]);
        a[10] = tm; a[11] = AuxOffset(g, 0x80) + (g.P(30) ? g.R(4) : 0u); a[12] = cnt; a[13] = grow;
        break;
    }
    case kFromChars:
        a[4] = g.Pk({0u, 1u, 2u, 5u, 14u, 15u, 16u, 17u, 30u, 50u, 100u, g.R(120)});
        a[5] = AuxOffset(g, 0x80) + (g.P(20) ? g.R(4) : 0u);
        a[6] = g.W();
        break;
    case kRtDtorBody:
    case kRtWhat:
    case kRtScalarDel: {
        const StrSpec s = MakeSpec(g, 0xF);
        PutSpec(s, &a[4]);
        a[10] = g.W();
        a[11] = g.Pk({0u, 1u, 1u, 2u, 3u, 0xFFFFFF01u, 0xFFFFFF00u, 0x100u, 0x101u, g.W()});
        break;
    }
    case kRtFromStr:
    case kRtCopy: {
        const StrSpec s = MakeSpec(g, 0xF);
        PutSpec(s, &a[4]);
        a[10] = AuxOffset(g, 0x28);
        break;
    }
    case kXlen:
        break;
    case kHandbrake:
        a[4] = g.Pk({0u, 0x20u, 0xFFu, 0xDFu, 0x40u, 0x80u, g.W() & 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        break;
    case kAlarm:
        a[4] = g.Pk({0u, 0xFFFFu, 1u, 0xFFFEu, 0x8000u, g.W() & 0xFFFFu, g.W() & 0xFFFFu});
        a[5] = g.Pk({0x28u, 0x29u, 0x2Fu, 0x20u, 0x30u, 0u, 0xFFu, 0xF8u, 0x2Eu, g.W() & 0xFFu, g.W() & 0xFFu});
        break;
    case kPatrolSD:
        a[4] = g.Pk({0u, 1u, 1u, 2u, 3u, 0xFFFFFF01u, 0xFFFFFF00u, 0x100u, 0x101u, g.W()});
        break;
    case kAbort:
        a[4] = 4 * g.R(0x40); a[5] = 4 * g.R(0x40); a[6] = 4 * g.R(0x100);
        a[7] = g.Pk({0u, 1u, g.W(), g.W() | 1u, 0xFFFFFFFFu});
        a[8] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x100u, g.W(), g.W()});
        a[9] = 0x200 + 4 * g.R(0x40);
        a[10] = g.Pk({0u, 1u, 1u});
        a[11] = 0x200 + 4 * g.R(0x40);
        a[12] = g.Pk({0u, 0u, 1u, 2u, 3u, 0x80000000u, 0xFFFFFFFFu, g.W()});
        a[13] = Raw(g);
        a[14] = (g.W() & 0xFFFFFF00u) | g.Pk({0u, 1u});
        break;
    case kControl:
        a[5] = 4 * g.R(0x40);
        a[8] = g.Pk({0u, 1u, 2u, 3u, 4u, 0x100u, 0xFCu, 0xFFu, g.W(), g.W() & ~3u});
        a[9] = g.P(50) ? Raw(g) : 0u;
        a[10] = Raw(g);
        a[14] = Raw(g);
        break;
    case kFollowDtor:
        a[4] = g.Pk({0u, 1u, 1u, 2u});
        a[5] = AuxOffset(g, 0x150);
        a[6] = Raw(g);
        break;
    case kClone:
        a[4] = g.Pk({0u, 2u, 2u, 3u, 0xFFFFFFFDu, g.W(), g.W() | 2u, g.W() & ~2u});
        a[5] = FloatBits(g); a[6] = FloatBits(g); a[7] = FloatBits(g); a[8] = FloatBits(g); a[9] = FloatBits(g);
        a[10] = g.W(); a[11] = FloatBits(g); a[12] = g.W();
        a[13] = g.Pk({0u, 0x10u, 0xEFu, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu});
        a[14] = g.P(12) ? 0u : 1u;
        a[15] = AuxOffset(g, 0x80);
        break;
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
