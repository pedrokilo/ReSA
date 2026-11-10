#pragma once
// Fixtures del banco Fn147A (lote 147A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                                              forma
//    0   0x4AB980   CEventGroup::Destructor (cuerpo)                       thiscall() `ret`: push 0; mov [ecx],85AAB0h; call 4AB370 (Flush(false)); ret
//    1   0x4AB9B0   CEventGlobalGroup::Destructor (cuerpo; idem, otra copia)
//    2   0x4ABA20   CEventGroup::ScalarDeletingDestructor (vtable 0x85AAB0, ranura 0)  thiscall(flags) `ret 4`: cuerpo en linea (vtable, Flush(false)), operator delete del CRT 8214BDh si bit 0
//    3   0x5E9800   size() de un vector de elementos de 20 bytes (+4 inicio, +8 fin)  thiscall() `ret`: eax = inicio ? (fin-inicio)/20 : 0
//    4   0x5E9850   size() de un vector de elementos de 12 bytes (CVector)             idem /12
//    5   0x66DFA0   CTaskComplexGoToPointShooting::Destructor (cuerpo)     thiscall() `ret`: jmp 668870 (~CTaskComplexGoToPointAiming)
//    6   0x66DF80   CTaskComplexGoToPointShooting::ScalarDeletingDestructor (vtable 0x86FE2C)  thiscall(flags) `ret 4`: cuerpo 66DFA0, CTask::operator delete 61A5B0
//    7   0x66DFB0   CTaskComplexGoToPointAndStandStillAndAchieveHeading::ScalarDeletingDestructor (vtable 0x86FE58)  cuerpo 668D40
//    8   0x463640   CTaskSimpleFinishBrain::Constructor                    thiscall() `ret`: call 61A390; mov [esi],859D20h; eax=this
//    9   0x5F67E0   CTaskSimpleWaitUntilLeaderAreaCodesMatch::Destructor (cuerpo)  thiscall() `ret`: mov [ecx],86C7E8h; jmp 6355E0
//   10   0x632C60   CTaskComplexSequence::GetTaskType (vtable 0x86E200, ranura 4)           thiscall() `ret`: mov eax,0F4h
//   11   0x638A80   CTaskComplexBeInGroup::ScalarDeletingDestructor (vtable 0x86E22C)       cuerpo 632EA0
//   12   0x635490   CTaskComplexUseSequence::GetTaskType (vtable 0x86E518, ranura 4)        thiscall() `ret`: mov eax,113h
//   13   0x6397A0   CTaskSimpleWaitUntilAreaCodesMatch::ScalarDeletingDestructor (vtable 0x86E544)  cuerpo 6355E0
//   14   0x6397C0   CTaskComplexUseScriptedBrain::ScalarDeletingDestructor (vtable 0x86E568)       cuerpo 635900
//   15   0x4AB240   FxSystem_c::OpenFileThunk                              cdecl(path, modo): jmp 538900 (CFileMgr::OpenFile)
//   16   0x4AB250   FxSystem_c::ReadLineThunk                              cdecl(archivo, buffer, num): jmp 5389B0 (CFileMgr::ReadLine); el valor es un bool (AL)
//   17   0x4AB260   FxSystem_c::CloseFileThunk                             cdecl(archivo): jmp 5389D0 (CFileMgr::CloseFile)
//   18   0x4AB2D0   FxPrtMult_c::SetUp                                      thiscall(7 dwords) `ret 1Ch`: copia los siete argumentos a +0x00..+0x18
//   19   0x492EF0   CTaskComplexDiveFromAttachedEntityAndGetUp::ScalarDeletingDestructor (vtable 0x85A358)  cuerpo 492F10
//   20   0x492F60   CEntitySeekPosCalculator::DestructorCopy (cuerpo)      thiscall() `ret`: mov [ecx],859DC4h
//   21   0x6579F0   CTaskComplexWalkRoundFire::ScalarDeletingDestructor (vtable 0x86F3E8)    cuerpo 655780
//   22   0x48DEA0   CTaskSimpleRunAnim::GetTaskType (vtable 0x86D528, ranura 4)             thiscall() `ret`: movzx eax,word [ecx+1Ch]
//   23   0x48E930   CTaskSimpleScratchHead::Destructor (cuerpo)            thiscall() `ret`: jmp 61A6F0 (~CTaskSimpleAnim)
//   24   0x48E910   CTaskSimpleScratchHead::ScalarDeletingDestructor (vtable 0x85A100)      cuerpo 48E930
//   25   0x48E940   CTaskSimpleUseAtm::ScalarDeletingDestructor (vtable 0x85A134)           cuerpo 48E960
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

namespace F147AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 26;

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
    std::array<u32, 16> a{};
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

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un dword de 32 bits como argumento "float" de SetUp: patrones con NaN (silenciosos y senalizantes), infinitos, ceros con signo, denormales, extremos y aleatorios
inline u32 FloatBits(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu, w6 = 0x3F800000u + (g.W() & 0x7FFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFFFFFFFFu, w1, w2, w3, w4, w5, w6});
}

// Clases de funcion (forma)
enum Kind : u32 { kBody, kScalarDeleting, kScalarDeletingEvent, kSize, kConstant, kWord, kThunkOpen, kThunkRead, kThunkClose, kCtor, kSetUp };
inline constexpr std::array<Kind, kFunctions> kKind{
    kBody, kBody, kScalarDeletingEvent, kSize, kSize, kBody, kScalarDeleting, kScalarDeleting, kCtor, kBody,
    kConstant, kScalarDeleting, kConstant, kScalarDeleting, kScalarDeleting, kThunkOpen, kThunkRead, kThunkClose, kSetUp, kScalarDeleting,
    kBody, kScalarDeleting, kWord, kBody, kScalarDeleting, kScalarDeleting};
// Entrada de cada funcion en el EXE, colaborador que llama (cuerpo del destructor, Flush, constructor base, funcion de CFileMgr; 0 si ninguno) y vtable de la clase (informativa: dwords previos de [this])
inline constexpr std::array<u32, kFunctions> kEntry{0x4AB980, 0x4AB9B0, 0x4ABA20, 0x5E9800, 0x5E9850, 0x66DFA0, 0x66DF80, 0x66DFB0, 0x463640, 0x5F67E0, 0x632C60, 0x638A80, 0x635490, 0x6397A0, 0x6397C0, 0x4AB240, 0x4AB250, 0x4AB260, 0x4AB2D0, 0x492EF0, 0x492F60, 0x6579F0, 0x48DEA0, 0x48E930, 0x48E910, 0x48E940};
inline constexpr std::array<u32, kFunctions> kBodyExe{0x4AB370, 0x4AB370, 0x4AB370, 0, 0, 0x668870, 0x66DFA0, 0x668D40, 0x61A390, 0x6355E0, 0, 0x632EA0, 0, 0x6355E0, 0x635900, 0x538900, 0x5389B0, 0x5389D0, 0, 0x492F10, 0, 0x655780, 0, 0x61A6F0, 0x48E930, 0x48E960};
inline constexpr std::array<u32, kFunctions> kVtable{0x85AAB0, 0x85AAB0, 0x85AAB0, 0, 0, 0x86FE00, 0x86FE2C, 0x86FE58, 0x859D20, 0x86C7E8, 0x86E200, 0x86E22C, 0x86E518, 0x86E544, 0x86E568, 0, 0, 0, 0, 0x85A358, 0x859DC4, 0x86F3E8, 0x86D528, 0x85A100, 0x85A100, 0x85A134};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 12, 12, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x1C, 4, 4, 4, 0x20, 4, 4, 4};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   cuerpos (kBody), destructores con borrado y constructor: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas (destructores con borrado)
//   kSize: a[3] = inicio (+4), a[4] = fin (+8); kConstant: a[3] = dword en [this] (no se lee); kWord: a[3] = dword en +0x1C (se lee la palabra baja); SetUp: a[3..9] = los siete argumentos (bits)
//   thunks: a[3..5] = argumentos crudos (archivo/ruta, modo/buffer, num), a[6] = EAX que devuelven los falsos (gRet)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    switch (kKind[c.fn]) {
    case kBody: case kScalarDeleting: case kScalarDeletingEvent: case kCtor:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kSize: {
        const u32 stride = c.fn == 3 ? 20u : 12u;
        const u32 first = g.Pk({0u, 0u, 0x00400000u + g.R(0x800000), 0x80000000u, 0xFFFFFFF0u, 0x7FFFFFFFu, g.W(), g.W(), 4u, 1u});
        const u32 count = g.Pk({0u, 1u, 2u, 3u, 5u, 7u, 100u, 1000u, 0x7FFFFFFu, 0x10000000u, 0xFFFFFFFFu, g.R(0x2000)});
        const u32 off = g.Pk({0u, 1u, stride - 1, stride + 1, stride / 2, g.R(stride), 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu});
        c.a[3] = first;
        c.a[4] = g.P(55) ? first + count * stride + (g.P(30) ? off : 0u) : g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.W(), first - off, first - stride, first + 1u});
        break;
    }
    case kConstant:
        c.a[3] = Raw(g);
        break;
    case kWord:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0xFFFFu, 0xFFFF0000u, 0x8000u, 0x80000000u, 0x00010000u, 0x7FFFu, g.W(), g.W(), g.W(), g.W() & 0xFFFFu, g.W() | 0xFFFF0000u});
        break;
    case kThunkOpen: case kThunkRead: case kThunkClose:
        c.a[3] = Raw(g); c.a[4] = Raw(g);
        c.a[5] = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x100u, 0x1FFu, g.W(), g.W(), g.R(0x1000)});
        c.a[6] = g.Pk({0u, 1u, 2u, 0xFFFFFFFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0x80000000u, g.W(), g.W(), g.W(), 0x7FFFFFFFu, 0xFFu});
        break;
    case kSetUp:
        for (unsigned i = 3; i <= 9; ++i) c.a[i] = FloatBits(g);
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
