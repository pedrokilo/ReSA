#pragma once
// Fixtures del banco Fn230A (lote 230A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 25 funciones del EXE (todas con hook y validated=0 antes del banco; ver README.md y fixture_oracle.py):
//   idx direccion nombre
//    0  0x74FDA0 RpWorldPluginAttach()            ocho RwEngineRegisterPlugin acumulados con OR, 0x750050, ClumpPluginAttach (0x749830), 0x74EA40
//    1  0x74FFA0 _rpLightTieDestroy(tie)          desenlaza +0x0C y +0x00, libera con [RwEngineInstance + 0x148](lista +4, nodo)
//    2  0x74FFF0 _rpTieDestroy(tie)               igual, solo con atomo (+8) y sector (+0x14) no nulos; lista +0
//    3  0x4A18F0 FxEmitterBP_c cuerpo del destructor      [this] = 0x85A788; 0x4A9D20(this)
//    4  0x4A1900 FxEmitterBP_c operator new(size)         0x4A9CA0 (GetMem) con ECX = 0xA9AF2C, (size, 4)
//    5  0x4A1920 FxEmitter_c cuerpo del constructor       0x4A9F20(this); [this] = 0x85A7A4; devuelve this
//    6  0x4A24D0 FxEmitterBP_c::IsFxInfoPresent(tipo)     recorre los FxInfo*; compara la palabra de 16 bits de info+4
//    7  0x4A2550 FxEmitter_c::Init (V)                    +4, +8, +0x10
//    8  0x4A2570 FxEmitter_c::Reset (V)                   +0x10 = 0
//    9  0x4A4460 FxEmitter_c::Update (V)                  habilitado y sin el bit 3 de [sistema + 0x62]: CreateParticles (0x4A41E0)
//   10  0x4A2B20 FxEmitterBP_c destructor con borrado     0x4A18F0(this); flags & 1: 0x8214BD(this)
//   11  0x4A2B90 FxEmitter_c destructor con borrado       0x4A2BB0(this); flags & 1: 0x8214BD(this)
//   12  0x4A4940 FxInfo_c cuerpo del destructor           [this] = 0x85A7DC
//   13  0x4A4950 FxInfoManager_c cuerpo del destructor    ret vacio
//   14  0x5C26B0 jpeg_start_compress(cinfo, todas_las_tablas)
//   15  0x5C2730 jpeg_write_scanlines(cinfo, filas, num)
//   16  0x5C30D0 jinit_c_coef_controller(cinfo, buffer_completo)
//   17  0x5C35F0 grayscale_convert(cinfo, entrada, salida, fila, num)
//   18  0x5C3700 jinit_color_converter(cinfo)
//   19  0x5C4380 jinit_forward_dct(cinfo)
//   20  0x5C43F0 jpeg_make_c_derived_tbl(cinfo, esDC, tblno, pdtbl)
//   21  0x5C4D50 jpeg_gen_optimal_table(cinfo, htbl, freq)
//   22  0x5C5210 jinit_huff_encoder(cinfo)
//   23  0x5C5260 jinit_compress_master(cinfo)
//   24  0x5C5420 jinit_c_main_controller(cinfo, buffer_completo)
//
// El caso es DECLARATIVO: la cabecera a[] (argumentos, relleno, semillas, efecto lateral) mas una lista de escrituras p (destino, desplazamiento, ancho, valor) y una lista de retornos programados r (id del
// falso, numero de llamada, valor) que el banco aplica tal cual; el oraculo Python aplica las mismas escrituras a su modelo de memoria y ejecuta la funcion como un programa sobre el. Cuatro arenas A..D
// (guardas de 0x40, ventana de 0x2000). Familia = n % 16: tipo = familia % 4 (posicion del objeto principal en A: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno = familia / 4.
//   a[0] = numero de argumentos, a[1..6] = argumentos (especificaciones de valor), a[7] = this (ECX; 0 = ninguno), a[8] = relleno de las arenas, a[9..12] = semillas de A..D, a[13] = semilla de las tablas
//   E1/E2 (RwEngineInstance falsas), a[14..19] = efecto lateral: id del falso, numero de llamada, arena, desplazamiento, modo (0 crudo, 1 especificacion), valor; a[20] = desplazamiento del objeto en A.
// Escritura: p = {(destino << 8) | ancho, desplazamiento (o direccion absoluta si destino == 4), valor}; destino 0..3 = arena A..D, 4 = region global de 4 bytes.
// Especificaciones de valor: PS(arena, desplazamiento) = ((0xA + arena) << 28) | desplazamiento (el banco la resuelve con la direccion real); 0xE0000001 / 0xE0000002 = RwEngineInstance falsa E1 / E2;
// 0xF00000kk = la funcion falsa kk que se llama por puntero (0 error_exit, 1 emit_message, 2 reset_error_mgr, 3 alloc_small, 4 alloc_large, 5 alloc_sarray, 6 request_virt_barray, 7 realize_virt_arrays,
// 8 init_destination, 9 prepare_for_pass, 10 pass_startup, 11 progress_monitor, 12 process_data, 13 write_file_header; su id de evento es 0xC100 + kk); otro valor es crudo.
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

namespace F230AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;
constexpr unsigned kArenas = 4;
constexpr unsigned kWords = 32;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x2000;   // ventana de cada arena
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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kWords> a{};
    std::vector<u32> p;   // escrituras (triples)
    std::vector<u32> r;   // retornos programados (triples)
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
// Un float (bits) sin NaN de senal: 62% de "juego", 16% notables, 4% infinitos y NaN silenciosos, 6% denormales, 12% bits cualesquiera
inline u32 Flt(Gen& g) {
    const u32 r = g.R(100);
    u32 v;
    if (r < 62) {
        const u32 m = 0x3F800000u | (g.W() & 0x7FFFFFu);
        const int e = int(g.R(25)) - 12;
        float f = std::ldexp(std::bit_cast<float>(m), e);
        if (g.P(35)) f = -f;
        v = std::bit_cast<u32>(f);
    } else if (r < 78) v = g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u, 0x3F000000u, 0x3DCCCCCDu, 0x42C80000u, 0x447A0000u, 0x3F333333u});
    else if (r < 82) v = g.Pk({0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7FC00001u, 0x7FC12345u, 0xFFE00000u, 0x7F7FFFFFu, 0xFF7FFFFFu});
    else if (r < 88) v = (g.W() & 0x007FFFFFu) | (g.P(50) ? 0x80000000u : 0u);
    else v = g.W();
    if ((v & 0x7F800000u) == 0x7F800000u && (v & 0x7FFFFFu) && !(v & 0x400000u)) v |= 0x400000u;
    return v;
}

// ------------------------------------------------------------------------------------------------ funciones
enum Kind : u32 { kPlugin, kTieK, kThis0, kNew, kCtor, kIsInfo, kInit, kReset, kUpdate, kSdd, kVtbl, kRetOnly, kStart, kWrite, kCoef, kGray, kColor, kDct, kDerived, kGen, kHuffEnc, kMaster, kMain };
inline constexpr std::array<Kind, kFunctions> kKind{kPlugin, kTieK, kTieK, kThis0, kNew, kCtor, kIsInfo, kInit, kReset, kUpdate, kSdd, kSdd, kVtbl, kRetOnly, kStart, kWrite, kCoef, kGray, kColor, kDct, kDerived, kGen, kHuffEnc, kMaster, kMain};
inline constexpr std::array<u32, kFunctions> kEntry{0x74FDA0, 0x74FFA0, 0x74FFF0, 0x4A18F0, 0x4A1900, 0x4A1920, 0x4A24D0, 0x4A2550, 0x4A2570, 0x4A4460, 0x4A2B20, 0x4A2B90, 0x4A4940, 0x4A4950,
                                                  0x5C26B0, 0x5C2730, 0x5C30D0, 0x5C35F0, 0x5C3700, 0x5C4380, 0x5C43F0, 0x5C4D50, 0x5C5210, 0x5C5260, 0x5C5420};
// Bytes que cada funcion toca a partir del objeto principal en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0x40, 0x18, 0x18, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x170, 0x170, 0x170, 0x170, 0x170, 0x170, 0x170, 0x170, 0x170, 0x170, 0x170};
// Regiones globales (RwEngineInstance y el desplazamiento de los datos de ties): direccion; 4 bytes cada una
inline constexpr std::array<u32, 2> kGlobal{0xC97B24, 0xC9259C};
inline std::vector<u32> GlobalsOf(u32 fn) { return fn == 1 || fn == 2 ? std::vector<u32>{0, 1} : std::vector<u32>{}; }

// ------------------------------------------------------------------------------------------------ generador
struct Cx {
    C& c;
    Gen g;
    u32 o{};     // desplazamiento del objeto principal en A
    Cx(C& c_, unsigned n, u32 salt) : c(c_), g(n, salt) {}
    void Put(u32 ar, u32 off, u32 w, u32 v) { c.p.push_back((ar << 8) | w); c.p.push_back(off); c.p.push_back(v); }
    void P32(u32 ar, u32 off, u32 v) { Put(ar, off, 4, v); }
    void P16(u32 ar, u32 off, u32 v) { Put(ar, off, 2, v & 0xFFFFu); }
    void P8(u32 ar, u32 off, u32 v) { Put(ar, off, 1, v & 0xFFu); }
    void A32(u32 off, u32 v) { P32(0, o + off, v); }   // campo del objeto principal (A)
    void A8(u32 off, u32 v) { P8(0, o + off, v); }
    void Glob(u32 addr, u32 v) { c.p.push_back((4u << 8) | 4u); c.p.push_back(addr); c.p.push_back(v); }
    void Ret(u32 id, u32 call, u32 v) { c.r.push_back(id); c.r.push_back(call); c.r.push_back(v); }
    void Args(std::initializer_list<u32> l) { c.a[0] = u32(l.size()); u32 i = 1; for (u32 v : l) c.a[i++] = v; }
    void This(u32 v) { c.a[7] = v; }
    void Poke(u32 id, u32 call, u32 arena, u32 off, u32 mode, u32 val) { c.a[14] = id; c.a[15] = call; c.a[16] = arena; c.a[17] = off; c.a[18] = mode; c.a[19] = val; }
};

// Estructuras comunes de la libjpeg (en B): error_mgr, memory_mgr, destination, master, main, marker, progress. Las copias "2" las usan los efectos laterales.
inline void MkErr(Cx& x, u32 at) { x.P32(1, at + 0, FS(0)); x.P32(1, at + 4, FS(1)); x.P32(1, at + 0x10, FS(2)); }
inline void MkMem(Cx& x, u32 at) { x.P32(1, at + 0, FS(3)); x.P32(1, at + 4, FS(4)); x.P32(1, at + 8, FS(5)); x.P32(1, at + 0x14, FS(6)); x.P32(1, at + 0x18, FS(7)); }
struct Jb { u32 err, mem, err2, mem2, dest, dest2, master, master2, main, main2, marker, marker2, prog, prog2; };
inline Jb MkJ(Cx& x) {
    Jb j{};
    auto& g = x.g;
    // cada estructura ocupa su propio tramo de 0x40 bytes (con una holgura de hasta 0x1C): las copias no se solapan ni pisan los punteros a funcion de otra
    j.err = 0x100u + 4u * g.R(8); j.err2 = 0x200u + 4u * g.R(8);
    j.mem = 0x300u + 4u * g.R(8); j.mem2 = 0x400u + 4u * g.R(8);
    j.dest = 0x500u + 4u * g.R(8); j.dest2 = 0x540u + 4u * g.R(8);
    j.master = 0x580u + 4u * g.R(8); j.master2 = 0x5C0u + 4u * g.R(8);
    j.main = 0x600u + 4u * g.R(8); j.main2 = 0x640u + 4u * g.R(8);
    j.marker = 0x680u + 4u * g.R(8); j.marker2 = 0x6C0u + 4u * g.R(8);
    j.prog = 0x700u + 4u * g.R(8); j.prog2 = 0x740u + 4u * g.R(8);
    MkErr(x, j.err); MkErr(x, j.err2); MkMem(x, j.mem); MkMem(x, j.mem2);
    x.A32(0x00, PS(1, j.err)); x.A32(0x04, PS(1, j.mem));
    for (u32 d : {j.dest, j.dest2}) x.P32(1, d + 8, FS(8));
    for (u32 m : {j.master, j.master2}) { x.P32(1, m + 0, FS(9)); x.P32(1, m + 4, FS(10)); x.P8(1, m + 0xC, 0); }
    for (u32 m : {j.main, j.main2}) x.P32(1, m + 4, FS(12));
    for (u32 m : {j.marker, j.marker2}) x.P32(1, m + 0, FS(13));
    for (u32 p : {j.prog, j.prog2}) x.P32(1, p + 0, FS(11));
    return j;
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Cx x(c, n, 100 + c.fn * 100);
    auto& g = x.g;
    auto& a = c.a;
    const u32 kind = f % 4;
    a[8] = f / 4;
    a[9] = c.seed * 13u + 50u; a[10] = a[9] + 1u; a[11] = a[9] + 2u; a[12] = a[9] + 3u;
    a[13] = g.W();
    x.o = Offset(g, kind, kSpan[c.fn]);
    a[20] = x.o;
    const u32 thisA = PS(0, x.o);
    const u32 fn = c.fn;
    switch (kKind[fn]) {
    case kPlugin: {
        // ocho retornos de RwEngineRegisterPlugin (el OR acumulado puede ser negativo) y los de los tres colaboradores
        for (u32 i = 0; i < 8; ++i) x.Ret(0x7F2BB0, i, g.P(82) ? g.Pk({0u, 0u, 1u, 2u, 7u, 0x100u, 0x7FFFFFFFu}) : RetVal(g));
        x.Ret(0x750050, 0, g.Pk({0u, 1u, 1u, 1u, 5u, 0xFFFFFFFFu, 0x100u, 0x80000000u}));
        x.Ret(0x749830, 0, g.Pk({0u, 1u, 1u, 1u, 9u, 0xFFFFFFFFu, 0x100u}));
        x.Ret(0x74EA40, 0, g.Pk({0u, 1u, 1u, 7u, 0xFFFFFFFFu, 0x100u, 0x80000000u, 0x00010000u}));
        break;
    }
    case kTieK: {
        x.Args({thisA});
        const bool light = fn == 1;
        auto Nb = [&](u32 k) -> u32 {   // vecino valido (8 bytes) en B, C o D; con poca probabilidad dentro del propio nodo
            if (g.P(8)) return PS(0, x.o + g.Pk({0u, 4u, 0xCu, 0x10u}));
            return PS(1 + g.R(3), 0x800u + 0x40u * k + 8u * g.R(8));
        };
        x.A32(0x00, Nb(0)); x.A32(0x04, Nb(1)); x.A32(0x0C, Nb(2)); x.A32(0x10, Nb(3));
        x.A32(0x08, g.P(30) ? 0u : RawNZ(g)); x.A32(0x14, g.P(30) ? 0u : RawNZ(g));
        if (light && g.P(40)) { x.A32(0x08, 0u); x.A32(0x14, 0u); }
        x.Glob(0xC97B24, g.P(50) ? kEngine1 : kEngine2);
        x.Glob(0xC9259C, 0x40u + 4u * g.R(0x30));
        break;
    }
    case kThis0: {
        x.This(thisA);
        if (g.P(35)) x.Poke(0x4A9D20, 0, 0, x.o + (g.P(50) ? 0u : 4u * g.R(4)), 0, g.W());
        break;
    }
    case kNew: {
        x.Args({g.Pk({0u, 1u, 0x40u, 0x100u, 0x7FFFFFFFu, 0xFFFFFFFFu, 8u, 0x80000000u, g.W(), g.R(0x1000)})});
        x.Ret(0x4A9CA0, 0, g.P(70) ? PS(3, 0x100u + 4u * g.R(0x300)) : Raw(g));
        break;
    }
    case kCtor: {
        x.This(thisA);
        x.Ret(0x4A9F20, 0, Raw(g));
        if (g.P(45)) x.Poke(0x4A9F20, 0, 0, x.o + g.Pk({0u, 0x10u, 4u, 8u}), 0, g.W());
        break;
    }
    case kIsInfo: {
        x.This(thisA);
        const u32 arrOff = 0x800u + 4u * g.R(0x40);
        const u32 infoBase = 0x900u + 8u * g.R(0x20);
        const u32 sepI = g.Pk({0x10u, 0x14u, 0x18u, 0x40u});
        const u32 mode = g.R(10);
        i32 count;
        if (mode == 0) count = 0; else if (mode == 1) count = -1 - i32(g.R(4)); else if (mode == 2) count = i32(0x80000000u); else if (mode == 3) count = 0x7FFFFFFF; else count = 1 + i32(g.R(7));
        x.A32(0x2C, u32(count));
        x.A32(0x30, PS(1, arrOff));
        const u32 nVis = (count >= 1 && count <= 8) ? u32(count) : 3u;   // entradas validas (las visitadas); las demas son basura que no debe leerse
        const u32 want = g.R(0x10000);
        const bool match = (count == 0x7FFFFFFF) ? true : (count > 0 && g.P(55));
        const u32 mi = g.R(nVis);
        for (u32 i = 0; i < 8; ++i) {
            if (i < nVis) {
                x.P32(1, arrOff + 4 * i, PS(2, infoBase + sepI * i));
                u32 t = g.P(60) ? g.R(0x40) : g.R(0x10000);
                if (t == want) t ^= 1;
                if (match && i == mi) t = want;
                x.P16(2, infoBase + sepI * i + 4, t);
                x.P16(2, infoBase + sepI * i + 6, g.P(50) ? 0u : g.R(0x10000));
            } else {
                x.P32(1, arrOff + 4 * i, RawNZ(g));
            }
        }
        x.Args({want});
        break;
    }
    case kInit: {
        x.This(thisA);
        x.Args({Raw(g), Raw(g)});
        break;
    }
    case kReset: case kVtbl: case kRetOnly: {
        x.This(thisA);
        break;
    }
    case kUpdate: {
        x.This(thisA);
        const u32 sys = 0x800u + 4u * g.R(0x40);
        x.A8(0x0C, g.Pk({0u, 1u, 1u, 0xFFu, 2u, 0x80u, g.R(256)}));
        x.A32(0x08, PS(1, sys));
        x.P8(1, sys + 0x62, g.P(50) ? (g.W() | 8u) : (g.W() & ~8u));
        x.Args({Flt(g), Flt(g)});
        break;
    }
    case kSdd: {
        x.This(thisA);
        x.Args({g.Pk({0u, 1u, 1u, 0xFFu, 0x100u, 3u, 2u, g.W()})});
        break;
    }
    default: break;
    }
    if (kKind[fn] >= kStart) {
        // ---- libjpeg: cinfo en A, estructuras auxiliares en B, tablas y filas en C y D (las asignaciones devueltas por los falsos van en D)
        const Jb j = MkJ(x);
        auto A32 = [&](u32 off, u32 v) { x.A32(off, v); };
        auto A8 = [&](u32 off, u32 v) { x.A8(off, v); };
        const u32 compBase = 0x100u + 4u * g.R(0x40), compBase2 = 0x500u + 4u * g.R(0x40);
        // nc elementos de comp_info (0x54 bytes) en C
        auto Comps = [&](u32 nc, u32 base) {
            for (u32 i = 0; i < nc && i < 4; ++i) {
                const u32 e = base + 0x54u * i;
                x.P32(2, e + 0x08, g.P(70) ? 1u + g.R(4) : g.Pk({0u, 5u, 8u, 0x7FFFFFFFu, 0xFFFFFFFFu, g.W()}));
                x.P32(2, e + 0x0C, g.P(70) ? 1u + g.R(4) : g.Pk({0u, 5u, 8u, 0x7FFFFFFFu, 0xFFFFFFFFu, g.W()}));
                x.P32(2, e + 0x1C, g.P(70) ? g.R(100) : g.Pk({0u, 0x1FFFFFFFu, 0xFFFFFFFFu, g.W()}));
                x.P32(2, e + 0x20, g.P(70) ? g.R(100) : g.Pk({0u, 0x1FFFFFFFu, 0xFFFFFFFFu, g.W()}));
            }
        };
        switch (kKind[fn]) {
        case kStart: {
            const bool okState = g.P(70);
            A32(0x14, okState ? 100u : g.Pk({0u, 99u, 101u, 102u, 1u, 0x7FFFFFFFu, 0xFFFFFFFFu, g.W()}));
            A32(0x18, PS(1, j.dest)); A32(0x13C, PS(1, j.master));
            A8(0xB0, g.Pk({0u, 1u, 0xFFu, 2u, 0x80u, g.R(256)}));
            A32(0xD0, g.W());
            x.Args({thisA, g.Pk({0u, 1u, 0x80u, 0xFFu, 0x100u, 0x01010100u, g.W()})});
            if (g.P(40)) {
                const u32 id = g.Pk({0x5C5260u, 0xC102u, 0xC108u, 0xC100u, 0x5CFA50u, 0xC109u});
                const u32 m = g.R(6);
                const u32 call = g.R(2);
                if (m == 0) x.Poke(id, call, 0, x.o + 0xB0, 0, g.Pk({0u, 1u, 0xFFu, g.W()}));
                else if (m == 1) x.Poke(id, call, 0, x.o + 0x13C, 1, PS(1, j.master2));
                else if (m == 2) x.Poke(id, call, 0, x.o + 0x14, 0, g.Pk({100u, 0u, 101u, g.W()}));
                else if (m == 3) x.Poke(id, call, 0, x.o + 0x18, 1, PS(1, j.dest2));
                else if (m == 4) x.Poke(id, call, 0, x.o + 0x0, 1, PS(1, j.err2));
                else x.Poke(id, call, 0, x.o + 0xD0, 0, g.W());
            }
            break;
        }
        case kWrite: {
            A32(0x14, g.P(80) ? 101u : g.Pk({100u, 0u, 102u, 0x7FFFFFFFu, 0xFFFFFFFFu, g.W()}));
            const u32 height = g.Pk({0u, 1u, 5u, 100u, 600u, 0xFFFFFFFFu, g.R(2000), g.W()});
            A32(0x20, height);
            const u32 next = g.P(80) ? (height == 0 ? 0u : g.R(height > 3000 ? 3000 : height)) : g.Pk({height, height + 1u, height + 5u, 0xFFFFFFFFu, 0u, g.W()});
            A32(0xD0, next);
            A32(0x08, g.P(40) ? 0u : PS(1, j.prog));
            A32(0x13C, PS(1, j.master)); x.P8(1, j.master + 0xC, g.Pk({0u, 1u, 1u, 0xFFu, 0x80u, g.R(256)}));
            x.P8(1, j.master2 + 0xC, g.Pk({0u, 1u, 0xFFu}));
            A32(0x140, PS(1, j.main));
            const u32 left = height - next;
            const u32 n2 = g.Pk({0u, 1u, 5u, 100u, 0xFFFFFFFFu, 0x7FFFFFFFu, left, left + 1u, left - 1u, g.R(40), g.W()});
            x.Args({thisA, g.P(30) ? PS(3, 0x100u + 4u * g.R(0x40)) : Raw(g), n2});
            const u32 nn = (n2 > left) ? left : n2;
            x.Ret(0xC10C, 0, g.Pk({0u, nn, nn, 1u, 2u, nn + 1u, nn >> 1, g.R(50), g.W()}));
            x.Ret(0xC10C, 1, g.Pk({0u, 1u, 3u}));
            if (g.P(45)) {
                const u32 id = g.Pk({0xC101u, 0xC10Bu, 0xC10Au, 0xC100u});
                const u32 m = g.R(6);
                if (m == 0) x.Poke(id, 0, 0, x.o + 0xD0, 0, g.R(100));
                else if (m == 1) x.Poke(id, 0, 0, x.o + 0x20, 0, g.R(100));
                else if (m == 2) x.Poke(id, 0, 0, x.o + 0x140, 1, PS(1, j.main2));
                else if (m == 3) x.Poke(id, 0, 0, x.o + 0x13C, 1, PS(1, j.master2));
                else if (m == 4) x.Poke(id, 0, 1, j.master + 0xC, 0, g.R(2));
                else x.Poke(id, 0, 0, x.o + 0x08, 1, PS(1, j.prog2));
            }
            break;
        }
        case kCoef: case kMain: {
            const bool coef = kKind[fn] == kCoef;
            const u32 nc = g.Pk({0u, 1u, 1u, 2u, 3u, 3u, 4u, 4u, 0xFFFFFFFFu, 0x80000000u});
            A32(0x3C, nc);
            A32(0x44, PS(2, compBase));
            Comps(nc > 4 ? 0u : nc, compBase); Comps(4, compBase2);
            A8(0xB0, g.Pk({0u, 0u, 0u, 1u, 0xFFu}));
            x.Args({thisA, g.Pk({0u, 1u, 1u, 0xFFu, 0x100u, 0x80u, g.W()})});
            x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
            x.Ret(0xC104, 0, PS(3, 0x400u + 4u * g.R(0x100)));
            for (u32 i = 0; i < 4; ++i) {
                x.Ret(0xC105, i, g.P(50) ? PS(3, 0xE00u + 8u * i) : Raw(g));
                x.Ret(0xC106, i, g.P(50) ? PS(3, 0xF00u + 8u * i) : Raw(g));
                x.Ret(0x5CF8A0, 2 * i, g.Pk({g.R(300), 0u, 0xFFFFFFFFu, g.W()}));
                x.Ret(0x5CF8A0, 2 * i + 1, g.Pk({g.R(300), 0u, 0xFFFFFFFFu, g.W()}));
            }
            if (g.P(45)) {
                const u32 id = coef ? g.Pk({0xC103u, 0xC104u, 0xC106u, 0x5CF8A0u, 0xC106u}) : g.Pk({0xC103u, 0xC105u, 0xC105u});
                const u32 call = g.R(3);
                const u32 m = g.R(5);
                if (m == 0) x.Poke(id, call, 0, x.o + 0x3C, 0, g.Pk({0u, 1u, 2u, 4u, 0xFFFFFFFFu}));
                else if (m == 1) x.Poke(id, call, 0, x.o + 0x04, 1, PS(1, j.mem2));
                else if (m == 2) x.Poke(id, call, 2, compBase + 0x0C + 0x54u * g.R(2), 0, g.Pk({1u, 2u, 3u, g.W()}));
                else if (m == 3) x.Poke(id, call, 2, compBase + 0x1C + 0x54u * g.R(2), 0, g.Pk({0u, 7u, g.W()}));
                else x.Poke(id, call, 2, compBase + 0x08 + 0x54u * g.R(2), 0, g.Pk({1u, 2u, 3u, g.W()}));
            }
            break;
        }
        case kGray: {
            A32(0x1C, g.Pk({0u, 1u, 3u, 8u, 16u, g.R(65)}));
            A32(0x24, g.Pk({0u, 1u, 1u, 2u, 3u, 4u, 5u, 6u}));
            const u32 nrows = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, 0x80000001u, 0xFFFFFFFBu, 1u, 2u});   // (0x80000000: --n daria la vuelta a 0x7FFFFFFF filas, en el EXE tambien)
            const u32 outrow = g.R(4);
            const u32 ib = 0x800u + 4u * g.R(0x10);                // in_buf: punteros a las filas de entrada (en B)
            const u32 ob = 0x900u + 4u * g.R(0x10);                // out_buf: [0] = tabla de punteros a filas de salida
            const u32 ar = 0xA00u + 4u * g.R(0x20);                // tabla de filas de salida (en B)
            x.P32(1, ob, PS(1, ar));
            const bool alias = g.P(8);
            for (u32 i = 0; i < 8; ++i) {
                x.P32(1, ib + 4 * i, PS(2, 0x400u + 0x1C0u * i + g.R(4)));
                const u32 outPtr = (alias && i == outrow + 1) ? PS(2, 0x400u + 0x1C0u * (i - 1) + 8u * g.R(3)) : PS(3, 0x200u + 0x80u * i + g.R(4));
                x.P32(1, ar + 4 * i, outPtr);
            }
            x.Args({thisA, PS(1, ib), PS(1, ob), outrow, nrows});
            break;
        }
        case kColor: {
            u32 ics = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu, 100u});
            u32 ic = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu});
            const u32 jcs = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 9u});
            u32 nc = g.Pk({0u, 1u, 2u, 3u, 4u, 5u});
            if (g.P(40)) {   // combinacion coherente: sin errores de entrada y componentes acordes
                ic = ics == 1 ? 1u : (ics == 2 ? 4u : (ics == 3 ? 3u : (ics == 4 || ics == 5 ? 4u : 1u + g.R(3))));
                nc = jcs == 1 ? 1u : (jcs == 2 || jcs == 3 ? 3u : (jcs == 4 || jcs == 5 ? 4u : ic));
            }
            A32(0x28, ics); A32(0x24, ic); A32(0x40, jcs); A32(0x3C, nc);
            x.Args({thisA});
            x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
            if (g.P(35)) x.Poke(g.Pk({0xC100u, 0xC103u}), g.R(2), 0, x.o + g.Pk({0x28u, 0x24u, 0x40u, 0x3Cu}), 0, g.Pk({0u, 1u, 2u, 3u, 4u, 5u}));
            break;
        }
        case kDct: {
            A32(0xB8, g.Pk({0u, 1u, 2u, 2u, 2u, 3u, 0xFFFFFFFFu, g.W()}));
            x.Args({thisA});
            x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
            break;
        }
        case kHuffEnc: {
            x.Args({thisA});
            x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
            break;
        }
        case kMaster: {
            A8(0xB0, g.Pk({0u, 0u, 1u, 0xFFu, 2u}));
            A8(0xB1, g.Pk({0u, 0u, 0u, 1u, 0x80u}));
            A8(0xD4, g.Pk({0u, 0u, 0u, 1u, 0xFFu}));
            A32(0xA8, g.Pk({0u, 1u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFF00u, 0x80000000u, 0x7FFFFFFFu, 0x00000100u, g.W()}));
            A8(0xB2, g.Pk({0u, 0u, 1u, 0xFFu, 0x80u}));
            A32(0x14C, PS(1, j.marker));
            x.Args({thisA});
            if (g.P(35)) {
                const u32 id = g.Pk({0x5C6A90u, 0x5C3700u, 0x5C4380u, 0x5C5210u, 0x5C30D0u, 0x5C5420u, 0x5C63D0u, 0xC107u, 0x5C7920u});
                const u32 m = g.R(7);
                if (m == 0) x.Poke(id, 0, 0, x.o + 0xB0, 0, g.Pk({0u, 1u}));
                else if (m == 1) x.Poke(id, 0, 0, x.o + 0xB1, 0, g.Pk({0u, 1u}));
                else if (m == 2) x.Poke(id, 0, 0, x.o + 0xD4, 0, g.Pk({0u, 1u}));
                else if (m == 3) x.Poke(id, 0, 0, x.o + 0xA8, 0, g.Pk({0u, 1u, 2u, 0xFFFFFF00u}));
                else if (m == 4) x.Poke(id, 0, 0, x.o + 0xB2, 0, g.Pk({0u, 1u}));
                else if (m == 5) x.Poke(id, 0, 0, x.o + 0x04, 1, PS(1, j.mem2));
                else x.Poke(id, 0, 0, x.o + 0x14C, 1, PS(1, j.marker2));
            }
            break;
        }
        case kDerived: {
            const bool isdc = g.P(50);
            const u32 isdcArg = isdc ? g.Pk({1u, 1u, 0x80u, 0xFFu, 0x101u, 0x01010101u}) : g.Pk({0u, 0u, 0x100u, 0x01000000u});
            const bool outOfRange = g.P(15);
            i32 tn;
            if (!outOfRange) tn = i32(g.R(4));
            else tn = g.P(50) ? -1 - i32(g.R(6)) : 4 + i32(g.R(37));
            // ocho tablas Huffman (0x112 bytes) en C a 0x800 + 0x120 * k: 0..3 las de DC y 4..7 las de AC; la 8 es una tabla propia para las ranuras fuera de ellas
            const auto TblAt = [](u32 k) { return 0x800u + 0x120u * k; };
            for (u32 k = 0; k < 4; ++k) { x.A32(0x58 + 4 * k, PS(2, TblAt(k))); x.A32(0x68 + 4 * k, PS(2, TblAt(4 + k))); }
            const i32 slot = i32(isdc ? 0x58 : 0x68) + 4 * tn;   // desplazamiento en cinfo de la ranura que se lee
            u32 used;
            if (slot >= 0x58 && slot < 0x78) used = TblAt(u32(slot - 0x58) / 4u);
            else { used = TblAt(8); x.A32(u32(slot), PS(2, used)); }
            // contenido de la tabla usada
            const u32 scen = g.R(10);
            u32 bits[17]{};
            u32 total = 0;
            if (scen < 3) {   // tabla estandar de luminancia DC / AC
                if (isdc) { const u32 b[17]{0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}; for (int i = 0; i < 17; ++i) bits[i] = b[i]; }
                else { const u32 b[17]{0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7D}; for (int i = 0; i < 17; ++i) bits[i] = b[i]; }
                for (int i = 1; i <= 16; ++i) total += bits[i];
            } else if (scen < 7) {   // codigo de prefijo valido aleatorio: los recuentos nunca agotan el espacio de codigos
                u32 code = 0;
                for (u32 l = 1; l <= 16; ++l) {
                    const u32 room = (1u << l) - code;
                    u32 cnt = room == 0 ? 0u : g.R(room > 20 ? 20u : room);
                    if (total + cnt > 256) cnt = 256 - total;
                    bits[l] = cnt; total += cnt;
                    code = (code + cnt) << 1;
                }
            } else {   // recuentos arbitrarios (pueden desbordar el espacio de codigos; en pocos casos la suma pasa de 256, hasta 259)
                for (u32 l = 1; l <= 16; ++l) bits[l] = g.P(50) ? 0u : g.R(g.P(40) ? 6u : 40u);
                total = 0; for (u32 l = 1; l <= 16; ++l) total += bits[l];
                const u32 lim = g.P(88) ? 256u : 259u;
                for (u32 l = 16; l >= 1 && total > lim; --l) { const u32 d = std::min(bits[l], total - lim); bits[l] -= d; total -= d; }
                if (lim == 259u && total < 257u) { bits[16] += 257u - total; total = 257u; if (total > 259u) bits[16] -= total - 259u; }
            }
            for (u32 l = 0; l <= 16; ++l) x.P8(2, used + l, l == 0 ? g.R(256) : bits[l]);
            // huffval: permutacion de 0..255 (DC valido: los 16 primeros son 0..15); a veces con repetidos o simbolos fuera de rango
            const bool dirty = g.P(18);
            u32 perm[256];
            for (u32 i = 0; i < 256; ++i) perm[i] = i;
            for (u32 i = 255; i > 0; --i) std::swap(perm[i], perm[g.R(i + 1)]);
            if (isdc && !dirty) {
                std::stable_partition(perm, perm + 256, [](u32 v) { return v < 16; });
            }
            for (u32 i = 0; i < 256; ++i) x.P8(2, used + 0x11 + i, (dirty && g.P(10)) ? g.R(256) : perm[i]);
            // pdtbl: tabla derivada existente en D (0x500; el relleno de ehufsi puede ser no nulo) o nula (se reserva)
            const u32 pd = 0x1000u + 4u * g.R(0x20);
            x.P32(1, pd, g.P(45) ? PS(3, 0x100u + 4u * g.R(0x40)) : 0u);
            x.Ret(0xC103, 0, PS(3, 0x800u + 4u * g.R(0x80)));
            x.Args({thisA, isdcArg, u32(tn), PS(1, pd)});
            if (g.P(25)) x.Poke(g.Pk({0xC103u, 0xC100u, 0xC100u}), g.R(2), 2, used + 0x11 + g.R(8), 0, g.R(256));
            break;
        }
        case kGen: {
            const u32 fq = 0x100u + 4u * g.R(0x40);
            const u32 ht = 0x800u + 4u * g.R(0x20);
            u32 freq[257]{};
            const u32 scen = g.R(10);
            if (scen < 3) { const u32 k = 1 + g.R(40); for (u32 i = 0; i < k; ++i) freq[g.R(256)] = 1 + g.R(g.P(50) ? 100u : 100000u); }
            else if (scen == 3) { for (u32 i = 0; i < 256; ++i) freq[i] = 1 + g.R(1u << 16); }
            else if (scen == 4) { for (u32 i = 0; i < 256; ++i) freq[i] = g.P(70) ? 0u : 1u + g.R(1000); }
            else if (scen == 5) {   // geometrica tipica de JPEG
                for (u32 i = 0; i < 256; ++i) { const u32 v = u32(100000.0 / (1.0 + i * i * 0.2)); freq[i] = g.P(85) ? v : 0u; }
            } else if (scen == 6) {   // cadenas profundas: potencias de dos (profundidad hasta 31: con freq <= 1e9 no se llega a 33) o Fibonacci
                u32 perm[256];
                for (u32 i = 0; i < 256; ++i) perm[i] = i;
                for (u32 i = 255; i > 0; --i) std::swap(perm[i], perm[g.R(i + 1)]);
                if (g.P(55)) {
                    const u32 len = 17 + g.R(14);   // 17..30 simbolos: 1, 1, 2, 4, ..., 2^(len-2)
                    for (u32 i = 0; i < len; ++i) freq[perm[i]] = i == 0 ? 1u : (1u << (i - 1));
                } else {
                    const u32 len = 17 + g.R(19);
                    u32 f1 = 1, f2 = 1;
                    for (u32 i = 0; i < len; ++i) { freq[perm[i]] = f1; const u32 t = f1 + f2; f1 = f2; f2 = t; }
                }
            } else if (scen == 7) { const u32 v = 1 + g.R(5); for (u32 i = 0; i < 256; ++i) freq[i] = g.P(80) ? v : 0u; }   // muchos empates
            else if (scen == 8) { freq[g.R(256)] = 1 + g.R(g.P(50) ? 3u : 0x7FFFFFFFu); }   // un solo simbolo
            else { for (u32 i = 0; i < 256; ++i) freq[i] = g.P(10) ? (g.P(50) ? 0x3B9ACA00u + g.R(3) : 1 + g.R(50)) : 0u; }
            bool sel = false;
            for (u32 i = 0; i < 256; ++i) sel |= freq[i] && freq[i] <= 1000000000u;
            if (!sel) freq[g.R(256)] = 1 + g.R(100);
            freq[256] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, g.W()});
            for (u32 i = 0; i < 257; ++i) x.P32(3, fq + 4 * i, freq[i]);
            for (u32 i = 0; i < 0x112; i += 1) if (g.P(10)) x.P8(2, ht + i, g.R(256));
            x.Args({thisA, PS(2, ht), PS(3, fq)});
            if (g.P(20)) x.Poke(0xC100, g.R(2), 2, ht + g.R(0x112), 0, g.R(256));
            break;
        }
        default: break;
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
    j.A("p", c.p);
    j.A("r", c.r);
    return j.End();
}
}
