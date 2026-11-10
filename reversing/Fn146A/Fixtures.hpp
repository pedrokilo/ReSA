#pragma once
// Fixtures del banco Fn146A (lote 146A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                              forma
//    0   0x676480   CTaskInteriorBeInShop::ScalarDeletingDestructor           thiscall(flags) `ret 4`: call 675460; test byte [esp+8],1; CTask::operator delete 61A5B0; eax=this  (vtable 0x870244, ranura 0)
//    1   0x6764A0   CTaskInteriorShopKeeper::ScalarDeletingDestructor         igual con call 6756A0 (vtable 0x870290)
//    2   0x64D1E0   CTaskSimpleWaitUntilPedIsOutCar::ScalarDeletingDestructor igual con call 6494F0 (vtable 0x86F094)
//    3   0x64D200   CTaskSimpleWaitUntilPedIsInCar::ScalarDeletingDestructor  igual con call 6496A0 (vtable 0x86F0B8)
//    4   0x655EE0   CTaskComplexEvasiveStep::ScalarDeletingDestructor         igual con call 653350 (vtable 0x86F218)
//    5   0x655F00   CTaskSimpleEvasiveDive::ScalarDeletingDestructor          igual con call 6535D0 (vtable 0x86F244)
//    6   0x66DB60   CTaskSimpleAchieveHeading::ScalarDeletingDestructor       igual con call 667E70 (vtable 0x86FD74)
//    7   0x66DB80   CTaskComplexGoToPointAndStandStill::ScalarDeletingDestructor igual con call 668240 (copia de 668630; vtable 0x86FDA8)
//    8   0x4E7C50   CAEMFDecoder::ScalarDeletingDestructor                     igual con call 4E77C0 y operator delete del CRT 8214BD (vtable 0x85F370, ranura 6)
//    9   0x4E7850   CAEMFDecoder::Shutdown                                     cdecl() `ret`: call 819840 (ExitMovies); jmp 819770 (TerminateQTML)
//   10   0x4E7F20   CPad::IsRadioTrackSkipPressed                              thiscall() `ret`: word [ecx+2Eh] != 0 && word [ecx+5Eh] == 0  (AL)
//   11   0x7059E0   CRealTimeShadow::GetShadowCamera                           thiscall() `ret`: lea eax,[ecx+8]
//   12   0x706770   CRealTimeShadowManager::Constructor                        thiscall() `ret`: [+0]=[+1]=0 (bytes), 16 dwords desde +4 y cuatro dwords +44..+50 a cero; eax=this
//   13   0x7067A0   CRealTimeShadowManager::Destructor                         thiscall() `ret`: CShadowCamera::Destroy 705400 en +4Ch y (salto) en +44h
//   14   0x404090   CBuilding::operator new                                    cdecl(1) `ret`: mov ecx,[0B74498h]; jmp 403FA0 (CBuildingPool::New)
//   15   0x4040A0   CBuilding::operator delete                                 cdecl(1) `ret`: ranura del pool 0B74498 (indice = (p - almacenamiento) / 56, estados[i] |= 80h, cursor = min)
//   16   0x5326E0   CDummy::operator new                                       cdecl(1) `ret`: mov ecx,[0B744A0h]; jmp 532630 (CDummyPool::New)
//   17   0x5326F0   CDummy::operator delete                                    igual que 15 con el pool 0B744A0
//   18   0x41B810   CPatrolRoute::operator new                                 cdecl(1) `ret`: mov ecx,[0B744B4h]; jmp 41B660 (CPatrolRoutePool::New)
//   19   0x41B820   CPatrolRoute::operator delete                              igual que 15 con el pool 0B744B4 y elementos de 0x1A4 bytes
//   20   0x5A2E60   ProcSurfaceInfo_c::ScaleMatrix (escalado de 3 filas)        thiscall(x, y, z) `ret 0Ch`: tres filas de 16 bytes {x, y, z, relleno} *= (x, y, z) con fld/fmul/fstp
//   21   0x5A2E90   ProcSurfaceInfo_c::Constructor                             thiscall() `ret`: lista de objetos (+3Ch, tres dwords a cero; call 4A8DD0); eax=this
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, donde hace falta (14..19), memoria global del juego (el puntero del pool; se guarda y se restaura tras cada lado).
// Familia = n % 16: tipo = familia % 4 (posicion de `this` o del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F146AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;
constexpr unsigned kSlots = 32;       // dwords de entrada por caso

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Bytes que cada funcion toca a partir de su puntero `this` / del objeto (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable (o nada)
constexpr u32 kSpanPad = 0x60;        // CPad: lee las palabras +0x2E (estado nuevo) y +0x5E (estado viejo)
constexpr u32 kSpanMgr = 0x54;        // CRealTimeShadowManager completo
constexpr u32 kSpanMgrDtor = 0x50;    // el destructor usa las camaras en +0x44 y +0x4C (dwords)
constexpr u32 kSpanPool = 0x10;       // pool falso: lee +0, +4 y +0xC
constexpr u32 kSpanMatrix = 0x2C;     // tres filas de 16 bytes: toca +0..+0xB, +0x10..+0x1B, +0x20..+0x2B
constexpr u32 kSpanSurf = 0x48;       // ProcSurfaceInfo_c: la lista en +0x3C..+0x47

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

// Indices (ver la cabecera)
inline constexpr unsigned kIdxMfDelete = 8, kIdxShutdown = 9, kIdxPadSkip = 10, kIdxShadowCam = 11, kIdxMgrCtor = 12, kIdxMgrDtor = 13, kIdxBldNew = 14, kIdxBldDel = 15, kIdxDumNew = 16,
                          kIdxDumDel = 17, kIdxPatNew = 18, kIdxPatDel = 19, kIdxScale = 20, kIdxSurfCtor = 21;
constexpr bool IsScalarDeleting(u32 fn) { return fn <= kIdxMfDelete; }
constexpr bool IsPoolNew(u32 fn) { return fn == kIdxBldNew || fn == kIdxDumNew || fn == kIdxPatNew; }
constexpr bool IsPoolDelete(u32 fn) { return fn == kIdxBldDel || fn == kIdxDumDel || fn == kIdxPatDel; }
// tamano del elemento de cada pool (division con signo del indice) y direccion del puntero del pool en el EXE
constexpr u32 ElementSize(u32 fn) { return fn == kIdxPatDel ? 0x1A4u : 56u; }
constexpr u32 PoolPointerAddr(u32 fn) { return (fn == kIdxBldNew || fn == kIdxBldDel) ? 0xB74498u : (fn == kIdxDumNew || fn == kIdxDumDel) ? 0xB744A0u : 0xB744B4u; }

// ------------------------------------------------------------------------------------------------ floats del escalado (20)
// un float "interesante": ceros, unos, constantes del juego, denormales, extremos, infinitos, NaN silenciosos y senalizantes, exponentes uniformes y aleatorios
inline u32 FloatPick(Gen& g) {
    const u32 sign = g.W() & 0x80000000u;
    const u32 uniform = sign | ((g.R(255)) << 23) | (g.W() & 0x7FFFFFu);             // exponente uniforme: productos que desbordan, underflow y denormales
    const u32 near1 = sign | ((120u + g.R(16)) << 23) | (g.W() & 0x7FFFFFu);          // magnitudes cercanas a 1
    const u32 denorm = sign | (g.W() & 0x7FFFFFu);                                    // denormales (y cero)
    const u32 tiny = sign | ((1u + g.R(60)) << 23) | (g.W() & 0x7FFFFFu);            // exponentes pequenos (productos denormales)
    const u32 big = sign | ((190u + g.R(64)) << 23) | (g.W() & 0x7FFFFFu);            // exponentes grandes (desbordamiento)
    const u32 qnan = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu);
    const u32 snan = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x40000000u, 0x3F000000u, 0x41A00000u, 0x00000001u, 0x007FFFFFu, 0x00800000u, 0x7F7FFFFFu, 0x7F800000u, 0xFF800000u,
                 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFFBFFFFFu, uniform, uniform, uniform, uniform, near1, near1, denorm, tiny, big, qnan, snan, g.W()});
}
// En el producto x87 de dos NaN cuyo significando (con el bit de silencio) coincide el resultado depende del orden de los operandos: el banco no lo prueba (los separa en el generador).
inline bool IsNanBits(u32 v) { return (v & 0x7F800000u) == 0x7F800000u && (v & 0x7FFFFFu) != 0; }
inline u32 SepNan(u32 v, u32 other) {
    if (IsNanBits(v) && IsNanBits(other) && ((v | 0x400000u) & 0x7FFFFFu) == ((other | 0x400000u) & 0x7FFFFFu)) return v ^ 0x3u;   // otro significando (el bit de silencio queda igual)
    return v;
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o del pool falso, en la arena B) en su arena, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global;
//   destructores con borrado (0..8): a[3] = vtable previa en [this]; a[4] = EAX del falso del cuerpo; a[5] = dword que deja en [ECX]; a[6] = banderas
//   Shutdown (9): a[4] = EAX que devuelven los dos falsos
//   CPad (10): a[3] = palabra +0x2E (estado nuevo), a[4] = palabra +0x5E (estado viejo)
//   GetShadowCamera (11) y constructores (12, 21): solo `this`; destructor del manager (13): a[3] = dword en +0x44, a[6] = dword en +0x4C, a[4] = EAX del falso, a[5] = dword que deja en [ECX]
//   operator new (14, 16, 18): a[3] = argumento (tamano, no se lee), a[4] = EAX del falso de New, a[6] bit 0 = puntero del pool crudo (a[7]) en lugar de un pool falso de la arena B
//   operator delete (15, 17, 19): pool falso en la arena B (a[0]): a[3] = almacenamiento (+0), a[4] = d = (puntero - almacenamiento) como int32, a[5] = cursor (+0xC), a[6] = desplazamiento en la arena C
//   donde cae estados[indice], a[7] = dword en +8 (no se lee)
//   escalado (20): a[3..5] = x, y, z; a[16..24] = los nueve floats de las tres filas
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    if (IsScalarDeleting(c.fn)) {
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x870244u, 0x870290u, 0x86F094u, 0x86F0B8u, 0x86F218u, 0x86F244u, 0x86FD74u, 0x86FDA8u, 0x85F370u, 0x86D48Cu, 0x85AAB4u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        return;
    }
    switch (c.fn) {
    case kIdxShutdown:
        c.a[0] = 0;
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        break;
    case kIdxPadSkip: {
        c.a[0] = Offset(g, kind, kSpanPad);
        const u32 w1 = g.W() & 0xFFFFu, w2 = (g.W() & 0xFFFFu) | 1u, w3 = g.W() & 0xFF00u;
        auto word = [&] { return g.Pk({0u, 0u, 0u, 1u, 2u, 0x100u, 0x8000u, 0xFFu, 0xFF00u, 0xFFFFu, 0x7FFFu, 0x0101u, w1, w2, w3}); };
        c.a[3] = word(); c.a[4] = word();
        break;
    }
    case kIdxShadowCam: case kIdxMgrCtor: case kIdxSurfCtor:
        c.a[0] = Offset(g, kind, c.fn == kIdxMgrCtor ? kSpanMgr : c.fn == kIdxSurfCtor ? kSpanSurf : kSpanThis);
        c.a[3] = Raw(g);
        break;
    case kIdxMgrDtor:
        c.a[0] = Offset(g, kind, kSpanMgrDtor);
        c.a[3] = Raw(g); c.a[6] = Raw(g);
        c.a[4] = Raw(g); c.a[5] = Raw(g);
        break;
    case kIdxBldNew: case kIdxDumNew: case kIdxPatNew:
        c.a[0] = Offset(g, kind, kSpanPool);
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        c.a[6] = g.P(25) ? 1u : 0u;
        c.a[7] = Raw(g);
        break;
    case kIdxBldDel: case kIdxDumDel: case kIdxPatDel: {
        const i32 e = i32(ElementSize(c.fn));
        c.a[0] = Offset(g, kind, kSpanPool);
        c.a[3] = Raw(g);
        const i32 cursor = i32(g.Pk({0u, 0xFFFFFFFFu, 1u, 5u, 31u, 0x80000000u, 0x7FFFFFFFu, g.W(), g.W() % 40000u, g.W() % 64u}));
        i32 d{};
        const u32 mode = g.R(10);
        // d = t * e + resto con trunc(d / e) = t (resto en [0, e) si t > 0, en (-e, 0] si t < 0 y en (-e, e) si t = 0)
        auto rem = [&](i32 t) { return t == 0 ? i32(g.R(u32(2 * e - 1))) - (e - 1) : (t > 0 ? i32(g.R(u32(e))) : -i32(g.R(u32(e)))); };
        switch (mode) {
        case 0: { const i32 t = i32(g.R(7)) - 3; d = t * e + rem(t); break; }
        case 1: { const i32 t = i32(g.R(10001)); d = t * e + rem(t); break; }
        case 2: { const i32 t = -i32(g.R(10001)); d = t * e + rem(t); break; }
        case 3: { const i32 t = i32(g.R(2001)) - 1000; d = t * e; break; }
        case 4: { const i32 t = i32(g.R(11)) - 5; d = t * e + (g.P(50) ? 1 : -1); break; }
        case 5: d = i32(g.W()); break;
        case 6: d = i32(g.Pk({0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 1u, u32(e - 1), u32(-(e - 1)), u32(e), u32(-e), 0u, u32(e + 1), u32(-e - 1), 0x7FFFFFFFu - u32(e) + 1u})); break;
        case 7: { const i32 t0 = (cursor <= -1000000 || cursor >= 1000000) ? 0 : cursor - 1; const i32 t = t0 + i32(g.R(3)); d = t * e + rem(t); break; }   // alrededor del cursor: t = cursor - 1, cursor, cursor + 1
        case 8: { const i32 t = i32(g.R(1u << 20)) - (1 << 19); d = t * e + rem(t); break; }
        default: { const i32 t = i32(g.W() % 3000000u) - 1500000; d = t * e + rem(t); break; }
        }
        c.a[4] = u32(d);
        c.a[5] = u32(cursor);
        c.a[6] = 4 * g.R(u32(kWin) / 4) + g.R(4);
        c.a[7] = Raw(g);
        break;
    }
    case kIdxScale: {
        c.a[0] = Offset(g, kind, kSpanMatrix);
        c.a[3] = FloatPick(g); c.a[4] = FloatPick(g); c.a[5] = FloatPick(g);
        // cada elemento multiplica a x, y o z segun su columna; los pares de NaN con el mismo significando se separan
        for (unsigned i = 0; i < 9; ++i) c.a[16 + i] = SepNan(FloatPick(g), c.a[3 + (i % 3)]);
        break;
    }
    default: break;
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
