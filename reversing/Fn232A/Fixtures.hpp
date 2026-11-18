#pragma once
// Fixtures del banco Fn232A (lote 232A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 15 funciones del EXE (libjpeg 6b embebida; todas con hook y validated=0 antes del banco; ver README.md y fixture_oracle.py):
//   idx direccion nombre
//    0  0x5C6BF0 jpeg_add_quant_table(cinfo, which_tbl, basic_table, scale_factor, force_baseline)
//    1  0x5C7130 jpeg_set_colorspace(cinfo, colorspace)
//    2  0x5C7420 jpeg_default_colorspace(cinfo)
//    3  0x5C7590 jpeg_set_defaults(cinfo)
//    4  0x5C7920 jinit_c_prep_controller(cinfo, need_full_buffer)
//    5  0x5C79F0 expand_right_edge(image_data, num_rows, input_cols, output_cols)
//    6  0x5C7DF0 jinit_downsampler(cinfo)
//    7  0x5C8690 jpeg_read_scanlines(cinfo, scanlines, max_lines) -> filas (EAX)
//    8  0x5C87E0 jpeg_start_decompress(cinfo) -> boolean (AL)
//    9  0x5C88F0 term_destination(cinfo)
//   10  0x5C8E20 jinit_d_coef_controller(cinfo, need_full_buffer)
//   11  0x5C8EE0 build_ycc_rgb_table  (cinfo en EAX, sin argumentos de pila)
//   12  0x5C90D0 null_convert(cinfo, input_buf, input_row, output_buf, num_rows)
//   13  0x5C9350 jinit_color_deconverter(cinfo)
//   14  0x5C9710 jinit_inverse_dct(cinfo)
//
// El caso es DECLARATIVO: la cabecera a[] (argumentos, relleno, semillas, efecto lateral) mas una lista de escrituras p (destino, desplazamiento, ancho, valor) y una lista de retornos programados r (id del
// falso, numero de llamada, valor) que el banco aplica tal cual; el oraculo Python aplica las mismas escrituras a su modelo de memoria y ejecuta la funcion como un programa sobre el. Cuatro arenas A..D
// (guardas de 0x40, ventana de 0x2000). Familia = n % 16: tipo = familia % 4 (posicion del objeto principal en A: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno = familia / 4.
//   a[0] = numero de argumentos, a[1..6] = argumentos (especificaciones de valor), a[7] = this (ECX; 0 = ninguno), a[8] = relleno de las arenas, a[9..12] = semillas de A..D, a[13] = semilla libre,
//   a[14..19] = efecto lateral: id del falso, numero de llamada, arena, desplazamiento, modo (0 crudo, 1 especificacion), valor; a[20] = desplazamiento del objeto en A; a[21] = EAX de entrada (especificacion; 0 = ninguno).
// Escritura: p = {(destino << 8) | ancho, desplazamiento (o direccion absoluta si destino == 4), valor}; destino 0..3 = arena A..D, 4 = region global de 4 bytes (sin uso en este lote).
// Especificaciones de valor: PS(arena, desplazamiento) = ((0xA + arena) << 28) | desplazamiento (el banco la resuelve con la direccion real);
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

namespace F232AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;


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
constexpr unsigned kWords = 32;
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

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 15;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x2000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

constexpr u32 PS(u32 arena, u32 off) { return ((0xAu + arena) << 28) | off; }
constexpr bool IsPS(u32 v) { const u32 n = v >> 28; return n >= 0xAu && n <= 0xDu; }
constexpr u32 FS(u32 k) { return 0xF0000000u + k; }
constexpr bool IsFS(u32 v) { return (v >> 8) == 0xF00000u; }
constexpr bool IsSpecial(u32 v) { return IsPS(v) || IsFS(v); }
inline u32 AvoidStack(u32 v) { return (v >= 0x00800000u && v < 0x02000000u) ? (v ^ 0x40000000u) : v; }
inline u32 NoPS(u32 v) { return IsSpecial(v) ? (v ^ 0x50000000u) : v; }
// Un dword "interesante" para valores crudos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas ni de la pila del hilo del banco)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return NoPS(AvoidStack(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu})));
}
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

// ------------------------------------------------------------------------------------------------ funciones
enum Kind : u32 { kAddQ, kSetCs, kDefCs, kSetDef, kPrep, kExpand, kDown, kReadSl, kStartD, kTerm, kCoefD, kYcc, kNull, kDeconv, kIdct };
inline constexpr std::array<Kind, kFunctions> kKind{kAddQ, kSetCs, kDefCs, kSetDef, kPrep, kExpand, kDown, kReadSl, kStartD, kTerm, kCoefD, kYcc, kNull, kDeconv, kIdct};
inline constexpr std::array<u32, kFunctions> kEntry{0x5C6BF0, 0x5C7130, 0x5C7420, 0x5C7590, 0x5C7920, 0x5C79F0, 0x5C7DF0, 0x5C8690, 0x5C87E0, 0x5C88F0, 0x5C8E20, 0x5C8EE0, 0x5C90D0, 0x5C9350, 0x5C9710};
// Bytes que cada funcion toca a partir del objeto principal en la arena A (cinfo de descompresion: hasta 0x1A8)
inline constexpr std::array<u32, kFunctions> kSpan{0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0, 0x1B0};
inline constexpr std::array<u32, 0> kGlobal{};   // sin regiones globales en este lote
inline std::vector<u32> GlobalsOf(u32) { return {}; }

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
    void Ret(u32 id, u32 call, u32 v) { c.r.push_back(id); c.r.push_back(call); c.r.push_back(v); }
    void Args(std::initializer_list<u32> l) { c.a[0] = u32(l.size()); u32 i = 1; for (u32 v : l) c.a[i++] = v; }
    void Poke(u32 id, u32 call, u32 arena, u32 off, u32 mode, u32 val) { c.a[14] = id; c.a[15] = call; c.a[16] = arena; c.a[17] = off; c.a[18] = mode; c.a[19] = val; }
};

// Estructuras comunes de la libjpeg (en B, cada una en su tramo de 0x80 bytes). Las copias "2" (para los efectos laterales) llevan los punteros a funcion cambiados por otros falsos distintos, de modo que
// cambiar el puntero de la estructura (o la propia estructura) en mitad de la funcion sea observable.
struct Jb { u32 err, err2, mem, mem2, prog, prog2, master, master2, main, main2, dest, dest2, inctl, inctl2, ds, ds2; };
inline Jb MkJ(Cx& x) {
    Jb j{};
    auto& g = x.g;
    u32* f[] = {&j.err, &j.err2, &j.mem, &j.mem2, &j.prog, &j.prog2, &j.master, &j.master2, &j.main, &j.main2, &j.dest, &j.dest2, &j.inctl, &j.inctl2, &j.ds, &j.ds2};
    for (u32 k = 0; k < 16; ++k) *f[k] = 0x100u + 0x80u * k + 4u * g.R(8);
    x.P32(1, j.err + 0, FS(0)); x.P32(1, j.err + 4, FS(1)); x.P32(1, j.err + 0x10, FS(2));
    x.P32(1, j.err2 + 0, FS(2)); x.P32(1, j.err2 + 4, FS(0)); x.P32(1, j.err2 + 0x10, FS(1));
    x.P32(1, j.mem + 0, FS(3)); x.P32(1, j.mem + 4, FS(4)); x.P32(1, j.mem + 8, FS(5));
    x.P32(1, j.mem2 + 0, FS(4)); x.P32(1, j.mem2 + 4, FS(3)); x.P32(1, j.mem2 + 8, FS(5));
    x.P32(1, j.prog + 0, FS(11)); x.P32(1, j.prog2 + 0, FS(10));
    x.P32(1, j.master + 0, FS(9)); x.P32(1, j.master2 + 0, FS(8));
    x.P32(1, j.main + 4, FS(12)); x.P32(1, j.main2 + 4, FS(7));
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
    const Jb j = MkJ(x);
    auto A32 = [&](u32 off, u32 v) { x.A32(off, v); };
    auto A8 = [&](u32 off, u32 v) { x.A8(off, v); };
    A32(0x00, PS(1, j.err)); A32(0x04, PS(1, j.mem));
    const u32 compBase = 0x100u + 4u * g.R(0x40), compBase2 = 0x500u + 4u * g.R(0x40);
    auto StateOr = [&](u32 ok, u32 pct) -> u32 { const bool good = g.P(pct); const u32 w = g.W(); return good ? ok : g.Pk({0u, ok - 1u, ok + 1u, 1u, 0x7FFFFFFFu, 0xFFFFFFFFu, w}); };
    // Si un efecto lateral cambia el puntero de memoria (mem2) las asignaciones pasan por otro falso: el banco programa tambien retornos validos para el.
    auto AltAllocs = [&](u32 base, u32 stride, u32 count) { for (u32 k = 0; k < count; ++k) x.Ret(0xC104, k, PS(3, base + stride * k)); };
    switch (kKind[fn]) {
    case kAddQ: {
        A32(0x14, StateOr(100u, 75));
        i32 tn;
        if (g.P(80)) tn = i32(g.R(4)); else tn = g.P(50) ? -1 - i32(g.R(6)) : 4 + i32(g.R(17));
        const u32 slot = u32(0x48 + 4 * tn);
        const u32 bt = 0x800u + 4u * g.R(8);
        const bool existing = g.P(50);
        const u32 exPtr = PS(3, 0x100u + 0x100u * g.R(8)), exPtr2 = PS(3, 0x900u + 0x100u * g.R(8));
        A32(slot, existing ? exPtr : 0u);
        const u32 scen = g.R(6);
        for (u32 i = 0; i < 64; ++i) {
            u32 v;
            if (scen == 0) { static const u32 q[16]{16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55}; v = q[i % 16]; }
            else if (scen == 1) v = g.R(256);
            else if (scen == 2) v = g.Pk({0u, 1u, 255u, 256u, 0xFFFFu, 0x7FFFu, 0x8000u, 0xFFFFFFFFu, 0x80000000u, g.W()});
            else if (scen == 3) v = g.R(0x10000);
            else if (scen == 4) v = g.Pk({0u, 1u, 99u, 100u, 255u});
            else v = g.W();
            x.P32(2, bt + 4 * i, v);
        }
        const u32 scale = g.Pk({50u, 100u, 200u, 25u, 1u, 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xFFFFFF38u, g.R(10000), g.W(), 75u, 150u, 10u});
        const u32 fb = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x01010100u, 0x80u, g.W()});
        x.Ret(0x5C6BB0, 0, PS(3, 0x1000u + 0x100u * g.R(8)));
        x.Args({thisA, u32(tn), PS(2, bt), scale, fb});
        if (g.P(40)) {
            const bool onErr = g.P(55);
            const u32 id = onErr ? 0xC100u : 0x5C6BB0u;
            const u32 call = onErr ? g.R(2) : 0u;
            const u32 m = g.R(onErr ? 4u : 3u);
            if (m == 0) x.Poke(id, call, 0, x.o + slot, 1, g.P(50) ? 0u : exPtr2);
            else if (m == 1) { const u32 w = bt + 4 * g.R(64); x.Poke(id, call, 2, w, 0, g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.R(300)})); }
            else if (m == 2) { const u32 w = bt + 4 * g.R(64); x.Poke(id, call, 2, w, 0, g.W()); }
            else x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, j.err2));
        }
        break;
    }
    case kSetCs: case kDefCs: case kSetDef: {
        const bool setcs = kKind[fn] == kSetCs, setdef = kKind[fn] == kSetDef;
        A32(0x14, StateOr(100u, 78));
        A32(0x44, PS(2, compBase));
        // input_components (+0x24) para el espacio desconocido: casi siempre valido; 5 y 6 son error pero el EXE sigue escribiendo componentes
        A32(0x24, g.Pk({1u, 2u, 3u, 4u, 3u, 1u, 4u, 0u, 5u, 6u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFDu}));
        A32(0x28, g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 3u, 2u, 1u, 6u, 7u, 0xFFFFFFFFu, 0x80000000u, 100u}));
        const u32 cs = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 3u, 2u, 6u, 7u, 0x80000000u, 0xFFFFFFFFu, 0x100u});
        A32(0x3C, g.W());   // se sobrescribe; el valor previo no debe influir
        A32(0x40, g.W());
        if (setdef) {
            A32(0x38, g.W());
            if (g.P(40)) A32(0x44, 0u);
            // ranuras de tablas de cuantizacion (0..1) y Huffman (DC0, DC1, AC0, AC1): nulas o validas
            for (u32 k = 0; k < 2; ++k) A32(0x48 + 4 * k, g.P(50) ? 0u : PS(3, 0x100u + 0x100u * k + 0x40u * g.R(2)));
            for (u32 k = 0; k < 2; ++k) { A32(0x58 + 4 * k, g.P(50) ? 0u : PS(3, 0x800u + 0x200u * k)); A32(0x68 + 4 * k, g.P(50) ? 0u : PS(3, 0xC00u + 0x200u * k)); }
            for (u32 k = 0; k < 16; ++k) { if (g.P(10)) { A8(0x78 + k, g.R(256)); A8(0x88 + k, g.R(256)); A8(0x98 + k, g.R(256)); } }
            A32(0xA8, g.W()); A32(0xAC, g.W()); A32(0xB0, g.W()); A32(0xB4, g.W()); A32(0xB8, g.W()); A32(0xBC, g.W()); A32(0xC0, g.W()); A32(0xC4, g.W()); A32(0xC8, g.W());
            x.Ret(0xC103, 0, PS(3, 0x1000u + 4u * g.R(0x20)));
            x.Ret(0x5C6BB0, 0, PS(3, 0x1400u));
            x.Ret(0x5C6BB0, 1, PS(3, 0x1500u));
            for (u32 k = 0; k < 4; ++k) x.Ret(0x5C6BD0, k, PS(3, 0x1600u + 0x120u * k));
        }
        x.Args({thisA});
        if (setcs) { x.Args({thisA, cs}); }
        if (g.P(40)) {
            const bool alt = setdef && g.P(50);
            const u32 id = alt ? g.Pk({0xC103u, 0x5C6BB0u, 0x5C6BD0u, 0xC100u}) : 0xC100u;
            const u32 call = g.R(3);
            const u32 m = g.R(5);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x44, 1, PS(2, compBase2));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x24, 0, g.Pk({1u, 2u, 3u, 4u, 0u, 5u, 0xFFFFFFFFu}));
            else if (m == 2) x.Poke(id, call, 0, x.o + (setdef ? 0x38u : 0x3Cu), 0, g.Pk({8u, 9u, 12u, 0u, 0xFFFFFFFFu}));
            else if (m == 3) { if (setdef) x.Poke(id, call, 0, x.o + 0x28, 0, g.Pk({0u, 1u, 3u, 5u, 9u})); else x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, j.err2)); }
            else x.Poke(id, call, 0, x.o + 0x14, 0, g.Pk({100u, 0u, 101u}));
        }
        break;
    }
    case kPrep: {
        const u32 need = g.Pk({0u, 0u, 0u, 0u, 1u, 0xFFu, 0x100u, 0x01010100u, g.W()});
        A32(0x154, PS(1, j.ds));
        x.P8(1, j.ds + 8, g.Pk({0u, 0u, 0u, 0u, 1u, 0xFFu, 0x80u, g.R(256)}));
        x.P8(1, j.ds2 + 8, g.Pk({0u, 1u}));
        const u32 nc = g.Pk({0u, 1u, 1u, 2u, 3u, 3u, 4u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u});
        A32(0x3C, nc);
        A32(0x44, PS(2, compBase));
        const u32 mh = g.Pk({1u, 2u, 2u, 3u, 4u, 0xFFFFFFFFu, 0u, 0x7FFFFFFFu, g.R(8)}), mv = g.Pk({1u, 2u, 2u, 3u, 4u, 0u, 0xFFFFFFFFu, g.R(8)});
        A32(0xD8, mh); A32(0xDC, mv);
        for (u32 i = 0; i < 6; ++i) {
            const u32 e0 = compBase + 0x54u * i, e1 = compBase2 + 0x54u * i;
            for (u32 e : {e0, e1}) {
                x.P32(2, e + 0x08, g.Pk({1u, 2u, 3u, 4u, 1u, 2u, 5u, 8u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000001u, g.R(16) + 1u}));
                x.P32(2, e + 0x1C, g.Pk({g.R(100), g.R(100), 0u, 1u, 0x1FFFFFFFu, 0xFFFFFFFFu, 0x20000000u, g.W(), 0x3FFFFFFu}));
            }
        }
        x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
        x.Ret(0xC104, 0, PS(3, 0x300u + 4u * g.R(0x20)));
        for (u32 i = 0; i < 6; ++i) x.Ret(0xC105, i, g.P(60) ? PS(3, 0xE00u + 8u * i) : Raw(g));
        x.Args({thisA, need});
        if (g.P(45)) {
            const u32 id = g.Pk({0xC103u, 0xC105u, 0xC100u, 0xC105u});
            const u32 call = g.R(3);
            const u32 m = g.R(6);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x154, 1, PS(1, j.ds2));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x3C, 0, g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu}));
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x44, 1, PS(2, compBase2));
            else if (m == 3) x.Poke(id, call, 0, x.o + 0xD8, 0, g.Pk({1u, 2u, 4u, 0xFFFFFFFFu}));
            else if (m == 4) x.Poke(id, call, 0, x.o + 0x04, 1, PS(1, j.mem2));
            else x.Poke(id, call, 0, x.o + 0xDC, 0, g.Pk({1u, 2u, 3u}));
        }
        break;
    }
    case kExpand: {
        const u32 img = 0x800u + 4u * g.R(0x10);
        const u32 rows = g.Pk({0u, 1u, 2u, 3u, 5u, 8u, 0xFFFFFFFFu, 0xFFFFFFFDu, 0x80000000u, 4u, 2u, 1u});
        const u32 inc = g.Pk({0u, 1u, 3u, 8u, 0x20u, 0x40u, 0xFFFFFFFCu, g.R(0x40), 0x10u});
        const u32 nco = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 0x40u, 0x100u, 0x1F0u, g.R(0x1F0), 0xFFFFFFFFu, 0xFFFFFFF0u, 0x80000000u});
        const u32 outc = inc + nco;
        const bool alias = g.P(10);
        for (u32 i = 0; i < 8; ++i) {
            u32 off = 0x40u + 0x280u * i + g.R(8);
            if (alias && i > 0) off = 0x40u + 0x280u * (i - 1) + 0x100u + g.R(8) * 4u;
            x.P32(1, img + 4 * i, PS(3, off));
        }
        x.Args({PS(1, img), rows, inc, outc});
        break;
    }
    case kDown: {
        A32(0x14, g.W());
        const u32 nc = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 4u, 1u, 0xFFFFFFFFu, 5u});
        A32(0x3C, nc);
        A32(0x44, PS(2, compBase));
        A8(0xB3, g.Pk({0u, 0u, 0u, 1u, 0xFFu, 0x80u}));
        const u32 mh = g.Pk({1u, 2u, 2u, 3u, 4u, 6u, 5u, 8u, 0x7FFFFFFFu, 0xFFFFFFFEu}), mv = g.Pk({1u, 2u, 2u, 3u, 4u, 6u, 8u, 0x7FFFFFFEu});
        A32(0xD8, mh); A32(0xDC, mv);
        for (u32 i = 0; i < 6; ++i) {
            for (u32 e : {compBase + 0x54u * i, compBase2 + 0x54u * i}) {
                const u32 sc = g.R(7);
                u32 h, v;
                if (sc == 0) { h = mh; v = mv; }
                else if (sc == 1) { h = mh / 2; v = mv; }
                else if (sc == 2) { h = mh / 2; v = mv / 2; }
                else if (sc == 3) { h = g.Pk({1u, 2u, 3u, 4u}); v = g.Pk({1u, 2u, 3u, 4u}); }
                else if (sc == 4) { h = g.Pk({1u, 2u, 3u, 4u, 0xFFFFFFFFu, 7u, 0x40000000u, 0xC0000000u}); v = g.Pk({1u, 2u, 3u, 4u, 0xFFFFFFFFu, 5u, 0x40000000u}); }
                else if (sc == 5) { h = mh / 2u; v = g.Pk({1u, 2u, 3u, 4u, mv}); }
                else { h = g.Pk({mh, mh / 2u, 1u, 2u}); v = g.Pk({mv, mv / 2u, 1u}); }
                if (h == 0) h = 1;
                if (v == 0) v = 1;
                x.P32(2, e + 0x08, h); x.P32(2, e + 0x0C, v);
            }
        }
        x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
        x.Args({thisA});
        if (g.P(40)) {
            const u32 id = g.Pk({0xC103u, 0xC100u, 0xC100u});
            const u32 call = g.R(3);
            const u32 m = g.R(5);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x3C, 0, g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu}));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x44, 1, PS(2, compBase2));
            else if (m == 2) x.Poke(id, call, 0, x.o + 0xD8, 0, g.Pk({1u, 2u, 4u, 8u}));
            else if (m == 3) x.Poke(id, call, 0, x.o + 0xDC, 0, g.Pk({1u, 2u, 3u}));
            else x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, j.err2));
        }
        break;
    }
    case kReadSl: {
        A32(0x14, StateOr(0xCDu, 80));
        const u32 height = g.Pk({0u, 1u, 5u, 100u, 600u, 0xFFFFFFFFu, g.R(2000), g.W(), 0x80000000u});
        A32(0x60, height);
        const u32 next = g.P(78) ? (height == 0 ? 0u : g.R(height > 3000 ? 3000 : height)) : g.Pk({height, height + 1u, height + 5u, 0xFFFFFFFFu, 0u, g.W(), 0x7FFFFFFFu});
        A32(0x78, next);
        A32(0x08, g.P(40) ? 0u : PS(1, j.prog));
        A32(0x184, PS(1, j.main));
        const u32 maxl = g.Pk({0u, 1u, 5u, 100u, 0xFFFFFFFFu, 0x7FFFFFFFu, g.R(40), g.W()});
        const u32 scan = g.P(30) ? PS(3, 0x100u + 4u * g.R(0x40)) : Raw(g);
        x.Args({thisA, scan, maxl});
        x.Ret(0xC10C, 0, g.Pk({0u, 1u, 2u, 3u, 5u, 40u, maxl, maxl, g.R(50), g.W()}));
        x.Ret(0xC107, 0, g.Pk({0u, 1u, 3u}));
        if (g.P(45)) {
            const u32 id = g.Pk({0xC101u, 0xC10Bu, 0xC10Au, 0xC100u, 0xC10Bu});
            const u32 call = g.R(2);
            const u32 m = g.R(6);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x78, 0, g.R(100));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x60, 0, g.R(100));
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x184, 1, PS(1, j.main2));
            else if (m == 3) x.Poke(id, call, 0, x.o + 0x08, 1, PS(1, j.prog2));
            else if (m == 4) x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, j.err2));
            else x.Poke(id, call, 0, x.o + 0x14, 0, g.Pk({0xCDu, 0u, 0xCCu}));
        }
        break;
    }
    case kStartD: {
        const u32 st = g.Pk({0xCAu, 0xCAu, 0xCAu, 0xCBu, 0xCBu, 0xCCu, 0xCCu, 0xCCu, 0xCDu, 0xC9u, 0u, 0xCFu, 0xFFFFFFFFu, g.W()});
        A32(0x14, st);
        const bool buffered = g.P(25), multi = g.P(25), dummy = g.P(20), rawout = g.P(40);
        A8(0x40, buffered ? g.Pk({1u, 0xFFu, 0x80u}) : 0u);
        A8(0x41, rawout ? g.Pk({1u, 0xFFu, 2u}) : 0u);
        A32(0x7C, g.W()); A32(0x84, g.W()); A32(0x78, g.W());
        A32(0x180, PS(1, j.master)); A32(0x190, PS(1, j.inctl));
        x.P8(1, j.inctl + 0x10, multi ? g.Pk({1u, 0xFFu, 0x80u}) : 0u);
        x.P8(1, j.inctl2 + 0x10, g.Pk({0u, 1u}));
        x.P8(1, j.master + 8, dummy ? g.Pk({1u, 0xFFu, 0x40u}) : 0u);
        x.P8(1, j.master2 + 8, 0u);
        x.Args({thisA});
        if (dummy) {
            // un pase de relleno repite ERREXIT para siempre salvo que el error_exit lo anule: el efecto lateral lo anula en la primera llamada de error_exit que hace el bucle
            // (las anteriores: error_exit por estado erroneo o por varios escaneos)
            const bool eMulti = (st == 0xCAu || st == 0xCBu) && multi && !(st == 0xCAu && buffered);
            const bool eState = !(st == 0xCAu || st == 0xCBu || st == 0xCCu);
            const u32 k = (eMulti ? 1u : 0u) + (eState ? 1u : 0u);
            if (g.P(50)) x.Poke(0xC100, k, 0, x.o + 0x180, 1, PS(1, j.master2));
            else x.Poke(0xC100, k, 1, j.master + 8, 0, 0u);
        } else if (g.P(40)) {
            const u32 id = g.Pk({0x5CCCC0u, 0xC100u, 0xC109u, 0x5CCCC0u});
            const u32 call = g.R(2);
            const u32 m = g.R(6);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x40, 0, g.Pk({0u, 1u}));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x14, 0, g.Pk({0xCBu, 0xCCu, 0xCAu, 0u}));
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x190, 1, PS(1, j.inctl2));
            else if (m == 3) x.Poke(id, call, 0, x.o + 0x180, 1, PS(1, j.master2));
            else if (m == 4) x.Poke(id, call, 0, x.o + 0x41, 0, g.Pk({0u, 1u}));
            else x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, j.err2));
        }
        break;
    }
    case kTerm: {
        A32(0x18, PS(1, j.dest));
        const u32 freeb = g.Pk({0x1000u, 0x1000u, 0x0u, 1u, 0xFFFu, 0x800u, 0x1001u, 0xFFFFFFFFu, g.R(0x1000), g.W(), 0x1000u - 1u});
        x.P32(1, j.dest + 4, freeb);
        x.P32(1, j.dest2 + 4, g.R(0x1000));
        const u32 fo = 0x800u + 0x40u * g.R(4), fo2 = 0x900u + 0x40u * g.R(4);
        x.P32(1, j.dest + 0x14, PS(2, fo)); x.P32(1, j.dest + 0x18, PS(3, 0x100u + 4u * g.R(0x40)));
        x.P32(1, j.dest2 + 0x14, PS(2, fo2)); x.P32(1, j.dest2 + 0x18, PS(3, 0x1000u + 4u * g.R(0x40)));
        x.P8(2, fo + 0xC, g.Pk({0u, 0x20u, 0xFFu, 0x1Fu, 0x40u, g.R(256), 0u}));
        x.P8(2, fo2 + 0xC, g.Pk({0u, 0x20u, 0x21u}));
        const u32 cnt = 0x1000u - freeb;
        x.Ret(0x823674, 0, g.P(60) ? cnt : g.Pk({0u, cnt + 1u, 0xFFFFFFFFu, 0x1000u, g.W()}));
        x.Ret(0x823E86, 0, g.W());
        x.Args({thisA});
        if (g.P(40)) {
            const u32 id = g.Pk({0x823674u, 0xC100u, 0x823E86u, 0xC100u});
            const u32 call = g.R(2);
            const u32 m = g.R(4);
            if (m == 0) x.Poke(id, call, 1, j.dest + 0x14, 1, PS(2, fo2));
            else if (m == 1) x.Poke(id, call, 2, fo + 0xC, 0, g.Pk({0u, 0x20u, 0xFFu}));
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x00, 1, PS(1, j.err2));
            else x.Poke(id, call, 2, fo2 + 0xC, 0, g.Pk({0u, 0x20u}));
        }
        break;
    }
    case kCoefD: {
        x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
        x.Ret(0xC104, 0, PS(3, 0x400u + 4u * g.R(0x100)));
        x.Args({thisA, g.Pk({0u, 1u, 1u, 0xFFu, 0x100u, 0x80u, g.W(), 0u, 0u})});
        if (g.P(45)) {
            const u32 id = g.Pk({0xC103u, 0xC104u, 0xC103u});
            const u32 m = g.R(3);
            if (m == 0) x.Poke(id, 0, 0, x.o + 0x04, 1, PS(1, j.mem2));
            else if (m == 1) x.Poke(id, 0, 0, x.o + 0x188, 0, g.W());
            else x.Poke(id, 0, 0, x.o + 0x00, 1, PS(1, j.err2));
        }
        break;
    }
    case kYcc: {
        A32(0x1A4, PS(1, j.ds));
        x.P32(1, j.ds + 0x08, g.W()); x.P32(1, j.ds + 0x0C, g.W()); x.P32(1, j.ds + 0x10, g.W()); x.P32(1, j.ds + 0x14, g.W());
        const bool alias = g.P(8);
        for (u32 k = 0; k < 4; ++k) x.Ret(0xC103, k, PS(3, 0x100u + 0x440u * ((alias && k == 3) ? 2u : k) + 4u * g.R(4)));
        AltAllocs(0x180u, 0x440u, 4);
        c.a[21] = thisA;
        x.Args({});
        if (g.P(40)) {
            const u32 call = g.R(4);
            const u32 m = g.R(4);
            if (m == 0) { const u32 sl = j.ds + 0x08 + 4 * g.R(4); x.Poke(0xC103, call, 1, sl, 1, PS(3, 0x100u + 0x440u * g.R(4))); }
            else if (m == 1) x.Poke(0xC103, call, 0, x.o + 0x04, 1, PS(1, j.mem2));
            else if (m == 2) { const u32 sl = j.ds + 0x08 + 4 * g.R(4); x.Poke(0xC103, call, 1, sl, 1, PS(3, 0x100u + 0x440u * g.R(4) + 0x20u)); }
            else x.Poke(0xC103, call, 0, x.o + 0x1A4, 1, PS(1, j.ds2));
        }
        break;
    }
    case kNull: {
        const u32 nc = g.Pk({0u, 1u, 1u, 2u, 3u, 3u, 4u, 4u, 0xFFFFFFFFu, 0x80000000u});
        A32(0x24, nc);
        const u32 width = g.Pk({0u, 1u, 2u, 3u, 5u, 8u, 0x10u, 0x20u, 0x40u, g.R(0x41)});
        A32(0x5C, width);
        const u32 ib = 0x800u + 4u * g.R(4), ob = 0x900u + 4u * g.R(0x10);
        const u32 inrow = g.Pk({0u, 1u, 2u, 3u, 0u, 5u});
        const u32 nrows = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000001u, 2u, 1u, 3u});
        for (u32 ci = 0; ci < 4; ++ci) {
            const u32 pl = 0xA00u + 0x40u * ci;                  // matriz de punteros a fila del plano ci (en B)
            x.P32(1, ib + 4 * ci, PS(1, pl));
            for (u32 r = 0; r < 16; ++r) x.P32(1, pl + 4 * r, PS(2, 0x100u + 0x400u * ci + 0x80u * r + g.R(4)));
        }
        const bool alias = g.P(10);
        for (u32 r = 0; r < 8; ++r) x.P32(1, ob + 4 * r, alias && r == 1 ? PS(2, 0x100u + 0x80u + g.R(0x30)) : PS(3, 0x100u + 0x300u * r + g.R(4)));
        x.Args({thisA, PS(1, ib), inrow, PS(1, ob), nrows});
        break;
    }
    case kDeconv: {
        x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
        for (u32 k = 1; k < 5; ++k) x.Ret(0xC103, k, PS(3, 0x400u + 0x440u * (k - 1) + 4u * g.R(4)));
        AltAllocs(0x400u, 0x440u, 5);
        u32 jcs = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu, 0x80000000u, 100u});
        u32 oc = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu, 0x80000000u});
        u32 nc = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u});
        if (g.P(60)) {   // combinacion coherente
            nc = jcs == 1 ? 1u : (jcs == 2 || jcs == 3 ? 3u : (jcs == 4 || jcs == 5 ? 4u : 1u + g.R(4)));
            if (g.P(70)) oc = g.Pk({1u, 2u, 4u, jcs});
        }
        A32(0x24, nc); A32(0x28, jcs); A32(0x2C, oc);
        A8(0x4A, g.Pk({0u, 0u, 1u, 0xFFu}));
        A32(0x64, g.W()); A32(0x68, g.W());
        A32(0xC4, PS(2, compBase));
        x.Args({thisA});
        if (g.P(35)) {
            const u32 id = g.Pk({0xC103u, 0xC100u, 0xC100u});
            const u32 call = g.R(2);
            const u32 m = g.R(6);
            if (m == 0) x.Poke(id, call, 0, x.o + 0x28, 0, g.Pk({0u, 1u, 2u, 3u, 4u, 5u}));
            else if (m == 1) x.Poke(id, call, 0, x.o + 0x24, 0, g.Pk({0u, 1u, 3u, 4u}));
            else if (m == 2) x.Poke(id, call, 0, x.o + 0x2C, 0, g.Pk({0u, 1u, 2u, 3u, 4u, 5u}));
            else if (m == 3) x.Poke(id, call, 0, x.o + 0xC4, 1, PS(2, compBase2));
            else if (m == 4) x.Poke(id, call, 0, x.o + 0x04, 1, PS(1, j.mem2));
            else x.Poke(id, call, 0, x.o + 0x4A, 0, g.Pk({0u, 1u}));
        }
        break;
    }
    case kIdct: {
        x.Ret(0xC103, 0, PS(3, 0x100u + 4u * g.R(0x20)));
        const bool alias = g.P(10);
        for (u32 k = 1; k < 7; ++k) x.Ret(0xC103, k, PS(3, 0x400u + 0x180u * ((alias && k == 2) ? 0u : (k - 1)) + 4u * g.R(8)));
        AltAllocs(0x400u, 0x180u, 7);
        const u32 nc = g.Pk({0u, 1u, 1u, 2u, 3u, 3u, 4u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u});
        A32(0x24, nc);
        A32(0xC4, PS(2, compBase));
        for (u32 i = 0; i < 6; ++i) { x.P32(2, compBase + 0x54u * i + 0x50, g.W()); x.P32(2, compBase2 + 0x54u * i + 0x50, g.W()); }
        x.Args({thisA});
        if (g.P(40)) {
            const u32 call = g.R(4);
            const u32 m = g.R(4);
            if (m == 0) x.Poke(0xC103, call, 0, x.o + 0x24, 0, g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu}));
            else if (m == 1) x.Poke(0xC103, call, 0, x.o + 0xC4, 1, PS(2, compBase2));
            else if (m == 2) x.Poke(0xC103, call, 0, x.o + 0x04, 1, PS(1, j.mem2));
            else x.Poke(0xC103, call, 0, x.o + 0x19C, 0, g.W());
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
    j.A("p", c.p);
    j.A("r", c.r);
    return j.End();
}
}
