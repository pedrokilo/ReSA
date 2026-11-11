#pragma once
// Fixtures del banco Fn161A (lote 161A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; los nombres salen del desensamblado, de las vtables leidas del PE y de los llamadores, ver README.md y fixture_oracle.py; el listado del encargo atribuia
// las clases por cercania de direcciones y erraba casi todas). 12 asignadas: ninguna tenia hook (se comprobo el codigo, docs/hooks.csv y git worktree list).
//   idx  direccion  nombre real                                                              forma
//    0   0x405900   CIplDefPool::Constructor (CPool<IplDef>(capacidad, nombre))              thiscall(capacidad, nombre) `ret 8`: operator new[] 0x821195 dos veces; cabecera de 0x14 bytes y estados a 0x80
//    1   0x405960   CIplDefPool::DestructorBody (liberacion del pool, sin llamadores)         thiscall() `ret`: delete[] 0x8213AE del almacenamiento y de los estados si hay propietario; a cero +0, +4, +8, +0xC
//    2   0x406360   GetGTA3ImgSize                                                           cdecl() `ret`: GetFileSize([0x8E4010], 0) por la IAT (0x858074)
//    3   0x59EA00   CDummyObject::Constructor (cuerpo por defecto, vtable 0x866E78)           thiscall() `ret`: cuerpo de CDummy 0x532540; [this] = 0x866E78; devuelve this
//    4   0x59EA20   CDummyObject::ConstructorFromObject (CDummyObject(CObject*))             thiscall(obj) `ret 4`: 0x532540, vtable, 0x533700, 0x533ED0, 0x533FB0, copia +2Eh/+2Fh, 0x404C90
//    5   0x59EAB0   CDummyObject::DestructorBody                                            thiscall() `ret`: [this] = 0x866E78; jmp 0x532560 (cuerpo de ~CDummy)
//    6   0x54EB70   CMatrixLinkList::Destructor (seis centinelas de 0x54 bytes)               thiscall() `ret`: 0x59ACD0 sobre this+0x1A4, 0x150, 0xFC, 0xA8, 0x54 y 0
//    7   0x54F1E0   CPlaceable::Constructor (vtable 0x863C40)                                thiscall() `ret`: [this] = 0x863C40; +4..+0x14 a cero; devuelve this
//    8   0x4C44D0   CAtomicModelInfo::CreateInstance(RwMatrix*) (vtable 0x85BBF0, ranura 10) thiscall(matriz) `ret 4`: AddRef, RpAtomicClone, RwFrameCreate, copia 64 bytes a frame+0x10, RpAtomicSetFrame, RemoveRef
//    9   0x4C4530   CAtomicModelInfo::CreateInstance() (ranura 11)                           thiscall() `ret`: igual sin la copia de la matriz
//   10   0x4C5590   CAtomicModelInfo::DestructorBody                                         thiscall() `ret`: [this] = 0x85BB9C
//   11   0x4C55A0   CDamageAtomicModelInfo::ConstructorBody (vtable 0x85BC30)                thiscall() `ret`: cuerpo de CBaseModelInfo 0x4C4A60; [this] = 0x85BC30; devuelve this
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F161AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 12;

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
    std::array<u32, 24> a{};
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

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}


// Clases de funcion (forma)
enum Kind : u32 { kIplCtor, kIplFlush, kImgSize, kCtorBody, kDummyObj, kEvBody, kMll, kPlCtor, kCreate, kCreateM, kVtOnly };
inline constexpr std::array<Kind, kFunctions> kKind{kIplCtor, kIplFlush, kImgSize, kCtorBody, kDummyObj, kEvBody, kMll, kPlCtor, kCreateM, kCreate, kVtOnly, kCtorBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x405900, 0x405960, 0x406360, 0x59EA00, 0x59EA20, 0x59EAB0, 0x54EB70, 0x54F1E0, 0x4C44D0, 0x4C4530, 0x4C5590, 0x4C55A0};
// Colaborador-cuerpo que llama la funcion y falsea FkBody (cuerpo del constructor / destructor de la base; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0x532540, 0x532540, 0x532560, 0x59ACD0, 0, 0, 0, 0, 0x4C4A60};
// vtable que deja la funcion en [this] (0 = ninguna); informativa para la fixture (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0, 0x866E78, 0x866E78, 0x866E78, 0, 0x863C40, 0x85BBF0, 0x85BBF0, 0x85BB9C, 0x85BC30};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x14, 0x14, 4, 4, 0x38, 4, 0x1FC, 0x18, 0x20, 0x20, 4, 4};
inline constexpr u32 kNewArr = 0x821195, kFreeArr = 0x8213AE;   // operator new[] / delete[] del EXE
inline constexpr u32 kGfsSlot = 0x858074;                       // casilla de la IAT del EXE de GetFileSize
inline constexpr u32 kHandleSlot = 0x8E4010;                    // gStreamFileHandles[0]
inline constexpr u32 kAddRef = 0x4C4BA0, kRemoveRef = 0x4C4BB0, kAtomicClone = 0x749E60, kFrameCreate = 0x7F0410, kSetFrame = 0x74BF20;
inline constexpr u32 kSetModelIdx = 0x533700, kAttachRw = 0x533ED0, kDetachRw = 0x533FB0, kIncludeEnt = 0x404C90;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; a[16..23] = datos extra
//   kIplCtor: a[3] = capacidad (int32), a[4] = nombre (crudo, no se lee), a[5] = modo de los punteros (0 normal: almacenamiento en B a[7], estados en C a[8]; 1 almacenamiento nulo; 2 estados = this + a[9] (solapan la
//         cabecera; capacidad <= 16); 3 estados en A a[11] (capacidad <= 16); 4 almacenamiento crudo a[10]; 5 estados crudos a[10] (capacidad <= 0); 6 estados nulos (capacidad <= 0))
//   kIplFlush: a[3], a[4] = almacenamiento y estados (crudos), a[5] = tamano (int32), a[6] = cursor (+0xC), a[7] = byte propietario (+0x10), a[8] = modo (bit 0: almacenamiento en B a[9]; bit 1: estados en C a[10])
//   kImgSize: a[3] = EAX que devuelve el GetFileSize falso
//   kCtorBody / kEvBody / kVtOnly / kPlCtor / kMll: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite
//   kDummyObj: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = desplazamiento del objeto en B (span 0x40); a[7] = indice de modelo (WORD en +0x22); a[8] = RwObject (+0x18) crudo; a[9], a[10] = byte
//         de IPL (+0x2E) y de area (+0x2F); a[16] = modo del objeto (0 en B a[6]; 1 = this; 2 = this + a[17] (con signo)); a[18] = 1: el RwObject es un puntero a la arena C a[19]
//   kCreate / kCreateM: a[3] = modo del atomic (+0x1C: 0 nulo, 1 en B a[4], 2 crudo a[5]); a[6] = 0 el clon en B a[7] / 1 crudo a[10]; a[8] = frame en C (span 0x90); a[9] = EAX de los falsos; a[16] = modo de la matriz (kCreateM:
//         0 en B a[17]; 1 = frame+0x10; 2 = frame+0x10+4k; 3 = frame+0x10-4k; 4 en A a[17]; 5 = frame+0x10+b (b no multiplo de 4)); a[18] = k (1..15) o b (1..60)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    switch (kKind[c.fn]) {
    case kIplCtor: {
        const u32 mode = g.Pk({0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, 2u, 2u, 3u, 3u, 4u, 5u, 6u});
        c.a[5] = mode;
        c.a[4] = Raw(g);
        i32 size;
        if (mode == 5 || mode == 6) {
            size = i32(g.Pk({0u, 0u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFFEu, 0xFFFFF000u, g.W() | 0x80000000u}));
        } else if (mode == 2 || mode == 3) {
            size = i32(g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 9u, 12u, 15u, 16u, g.R(17), g.R(17)}));
            c.a[0] = 4 * g.R(0x100) + 0x100 * g.R(8) + (g.P(30) ? 1u + g.R(3) : 0u);   // lejos del final: el bucle puede reescribir el puntero de estados dentro de la misma pagina de 256 bytes
        } else {
            const u32 r = g.R(100);
            if (r < 6) size = 0;
            else if (r < 14) size = 1;
            else if (r < 30) size = i32(2 + g.R(8));
            else if (r < 52) size = i32(10 + g.R(200));
            else if (r < 68) size = i32(200 + g.R(0xD00));
            else if (r < 72) size = 0xF00;
            else size = i32(g.Pk({0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0xFFFFFFFCu, 0xFFFFF000u, 0xC0000000u, g.W() | 0x80000000u}));
        }
        c.a[3] = u32(size);
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Offset(g, g.R(4), size > 0 ? u32(size) : 4u);
        c.a[9] = u32(i32(g.R(0x19)) - 4);                      // estados = this + d, d en [-4, 0x14]
        c.a[10] = Raw(g);
        c.a[11] = c.a[0] + 0x40 + 4 * g.R(0x40) + (g.P(30) ? 1u + g.R(3) : 0u);   // estados en A, fuera de la cabecera (el bucle no puede tocar los bytes altos del puntero de estados)
        if (mode == 2 && i32(c.a[9]) < 8) {                                      // como mucho el byte bajo del puntero de estados (this+4): el bucle no llega a sus bytes altos (saldria de la arena)
            const i32 lim = i32(c.a[9]) <= 4 ? 5 - i32(c.a[9]) : 0;
            if (size > lim) { size = lim; c.a[3] = u32(size); }
        }
        break;
    }
    case kIplFlush: {
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        c.a[5] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 100u, 0x1000u, g.R(1000), g.R(1000), g.W(), g.W(), 0xFFFFFFFEu, 3u});
        c.a[6] = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x7FFFFFFFu, 0x80000000u, g.R(100), g.W(), g.W()});
        c.a[7] = g.Pk({0u, 0u, 0u, 1u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x100u, g.W(), g.W() & 0xFFu});
        c.a[8] = g.Pk({0u, 0u, 1u, 2u, 3u});
        c.a[9] = Offset(g, g.R(4), 4);
        c.a[10] = Offset(g, g.R(4), 4);
        break;
    }
    case kImgSize:
        c.a[3] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.W(), g.R(0x100000), 0x2D000000u, 0x1000u, g.R(2) ? 0xFFFFFFFFu : 0u});
        break;
    case kCtorBody:
    case kEvBody:
    case kVtOnly:
    case kPlCtor:
    case kMll:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kDummyObj: {
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        const u32 mode = g.Pk({0u, 0u, 0u, 0u, 0u, 0u, 0u, 1u, 1u, 2u, 2u, 2u});
        c.a[16] = mode;
        c.a[6] = Offset(g, g.R(4), 0x40);
        c.a[7] = g.Pk({0u, 1u, 0x7FFFu, 0x8000u, 0xFFFFu, g.R(0x10000), g.R(0x10000), g.R(0x10000), g.W(), 0xFFFF0000u | g.R(0x10000), 0x1234u});
        c.a[8] = Raw(g);
        c.a[9] = g.Pk({0u, 0u, 1u, 0xFFu, 0x80u, g.R(256), g.R(256), g.W()});
        c.a[10] = g.Pk({0u, 0u, 1u, 0xFFu, 0x80u, 13u, g.R(256), g.R(256), g.W()});
        c.a[17] = g.Pk({1u, 2u, 3u, 4u, 0x10u, 0x18u, 0x20u, 0x2Eu, 0x2Fu, 0x30u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFCu, 0xFFFFFFF0u, 0xFFFFFFE0u, 0xFFFFFFD0u, 0xFFFFFFD2u, 0xFFFFFFD1u});
        c.a[18] = g.P(30) ? 1u : 0u;
        c.a[19] = Offset(g, g.R(4), 4);
        if (mode != 0 && c.a[0] > u32(kWin) - 0x100) c.a[0] = 4 * g.R(0x300) + (g.P(30) ? 1u + g.R(3) : 0u);   // el objeto solapado (this + delta) debe caer dentro de la arena
        break;
    }
    case kCreate:
    case kCreateM: {
        c.a[3] = g.Pk({0u, 0u, 0u, 1u, 1u, 1u, 1u, 2u, 2u});
        c.a[4] = Offset(g, g.R(4), 4);
        c.a[5] = Raw(g) | 1u;
        c.a[6] = g.Pk({0u, 0u, 0u, 1u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Offset(g, g.R(4), 0x90);
        c.a[9] = Raw(g);
        c.a[10] = Raw(g);
        c.a[16] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 2u, 3u, 3u, 4u, 5u});
        c.a[17] = Offset(g, g.R(4), 0x40);
        c.a[18] = c.a[16] == 5 ? 1u + g.R(60) : 1u + g.R(15);
        if (c.a[16] == 5 && c.a[18] % 4 == 0) c.a[18] += 1;
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
