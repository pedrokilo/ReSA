#pragma once
// Fixtures del banco Fn150A (lote 150A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 0x4F21C0 (CAEStreamingChannel::Stop,
// ranura 7 de 0x85F3F0) ya tenia hook (fila V "Stop-virtual" de docs/hooks.csv) y se salta.
//   idx  direccion  nombre real                                                              forma
//    0   0x4F27F0   CAEStreamingChannel::ScalarDeletingDestructor (vtable 0x85F3F0)       thiscall(flags) `ret 4`: cuerpo 4F2200 (con hook), operator delete del CRT 8214BDh si bit 0
//    1   0x4C4AF0   CBaseModelInfo::ScalarDeletingDestructor (vtable 0x85BB9C)            thiscall(flags) `ret 4`: [this]=85BB9Ch en linea, operator delete del CRT si bit 0
//    2   0x4C4C00   CColModel::ScalarDeletingDestructor (sin vtable ni llamadores)         thiscall(flags) `ret 4`: cuerpo 40F700 (con hook), CColModel::operator delete 40FC40 (con hook) si bit 0
//    3   0x5329A0   CPtrNodeDoubleLink::AddToList                                          thiscall(cabeza) `ret 4`: prev=0; next=*cabeza; si *cabeza: (*cabeza).prev=nodo; *cabeza=nodo
//    4   0x404350   CVector::DefaultConstructor                                           thiscall() `ret`: mov eax,ecx
//    5   0x4043E0   CEntity::GetStatus                                                    thiscall() `ret`: movzx eax,byte [ecx+36h]; shr eax,3
//    6   0x404420   CEntity::GetModelIndex                                                thiscall() `ret`: movsx eax,word [ecx+22h]
//    7   0x404590   CQuadTreeNode::ScalarDeletingDestructor (sin vtable ni llamadores)    thiscall(flags) `ret 4`: cuerpo 552520, liberacion en el pool 552C90 si bit 0 (cdecl)
//    8   0x43E090   CEntryExit::GetPosition                                               thiscall(salida) `ret 4`: x87 (derecha+izquierda)*0.5, (arriba+abajo)*0.5 y Z copiado; eax = salida
//    9   0x61BA80   CTaskSimpleAnim::ScalarDeletingDestructor (vtable 0x86D504)           cuerpo 61A6F0 (sin hook), CTask::operator delete 61A5B0
//   10   0x61BAA0   CTaskSimpleRunAnim::ScalarDeletingDestructor (vtable 0x86D528)        cuerpo 48DE60 (sin hook)
//   11   0x61BEF0   CTaskSimpleRunNamedAnim::ScalarDeletingDestructor (vtable 0x86D54C)   cuerpo 61BF10 (con hook)
//   12   0x64B440   CTaskSimpleCarWaitToSlowDown::ScalarDeletingDestructor (vtable 0x86ED98)  cuerpo 646A00 (con hook)
//   13   0x64B460   CTaskSimpleCarWaitForDoorNotToBeInUse::ScalarDeletingDestructor (vtable 0x86EDBC)  cuerpo 646C00 (con hook)
//   14   0x64B480   CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ScalarDeletingDestructor (vtable 0x86EDE0)  cuerpo 646E50 (con hook)
//   15   0x448650   CGarages::CameraShouldBeOutside                                       cdecl() `ret`: mov al,[0x96BFE4]
//   16   0x448660   CGarages::GivePlayerDetonator                                        cdecl() `ret`: FindPlayerPed(-1), GetWeaponInfo(0x28,1), GiveWeapon(0x28,1,1) y cuatro escrituras
//   17   0x658890   CTaskSimpleGiveCPR::GetTaskType (vtable 0x86F4C8, ranura 4)          thiscall() `ret`: mov eax,66h
//   18   0x6588A0   CTaskSimpleGiveCPR::DestructorBody                                    thiscall() `ret`: vtable 86F4C8h; si [this+10h]: SetFinishCallback(4D1390h, 0) (4CEBE0); ~CTask (61A3A0)
//   19   0x653080   CTaskSimpleHitHead::GetTaskType (vtable 0x86F1D0, ranura 4)           thiscall() `ret`: mov eax,1F4h
//   20   0x653090   CTaskSimpleHitHead::DestructorBody                                    idem con vtable 86F1D0h y animacion en +0Ch
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

namespace F150AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 21;

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
inline u32 FloatBitsOf(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu, w6 = 0x3F800000u + (g.W() & 0x7FFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFFFFFFFFu, w1, w2, w3, w4, w5, w6});
}

// Clases de funcion (forma)
enum Kind : u32 { kSdd, kAddList, kCtorNoop, kStatus, kModelIdx, kPos, kBool, kDetonator, kConstant, kAnimBody };
inline constexpr std::array<Kind, kFunctions> kKind{
    kSdd, kSdd, kSdd, kAddList, kCtorNoop, kStatus, kModelIdx, kSdd, kPos, kSdd,
    kSdd, kSdd, kSdd, kSdd, kSdd, kBool, kDetonator, kConstant, kAnimBody, kConstant, kAnimBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4F27F0, 0x4C4AF0, 0x4C4C00, 0x5329A0, 0x404350, 0x4043E0, 0x404420, 0x404590, 0x43E090, 0x61BA80, 0x61BAA0, 0x61BEF0, 0x64B440, 0x64B460, 0x64B480,
                                                    0x448650, 0x448660, 0x658890, 0x6588A0, 0x653080, 0x653090};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor, ~CTask; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x4F2200, 0, 0x40F700, 0, 0, 0, 0, 0x552520, 0, 0x61A6F0, 0x48DE60, 0x61BF10, 0x646A00, 0x646C00, 0x646E50, 0, 0, 0, 0x61A3A0, 0, 0x61A3A0};
// Funcion de liberacion de los destructores con borrado (operator delete del CRT, CColModel::operator delete, liberacion del pool de CQuadTreeNode, CTask::operator delete)
inline constexpr std::array<u32, kFunctions> kDelete{0x8214BD, 0x8214BD, 0x40FC40, 0, 0, 0, 0, 0x552C90, 0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0, 0, 0, 0, 0, 0};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x85F3F0, 0x85BB9C, 0, 0, 0, 0, 0, 0, 0, 0x86D504, 0x86D528, 0x86D54C, 0x86ED98, 0x86EDBC, 0x86EDE0, 0, 0, 0x86F4C8, 0x86F4C8, 0x86F1D0, 0x86F1D0};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 12, 4, 0x38, 0x24, 4, 0x30, 4, 4, 4, 4, 4, 4, 4, 0xB00, 4, 0x14, 4, 0x10};
// Memoria global del juego que toca CameraShouldBeOutside (un byte)
constexpr u32 kGlobalBool = 0x96BFE4;

inline bool IsNan(u32 w) { return (w & 0x7F800000u) == 0x7F800000u && (w & 0x007FFFFFu) != 0; }
// Un float de 32 bits que no es NaN (sumas con NaN en un solo operando)
inline u32 NonNanBits(Gen& g) {
    for (;;) { const u32 w = FloatBitsOf(g); if (!IsNan(w)) return w; }
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kAddList: a[0] nodo en A (item +0, siguiente +4, anterior +8; a[3..5] valores previos); a[6] = modo de la cabeza (0 nula, 1 apunta a un nodo de C, 2 apunta al propio nodo, 3 cabeza en nodo+4, 4 cabeza en nodo+8);
//             a[7] desplazamiento de la celda de cabeza en B; a[8] desplazamiento del primer nodo en C; a[9..11] relleno previo del primer nodo (anterior +8, siguiente +4, item +0)
//   kCtorNoop: a[3] = dword previo; kStatus: a[3] = dword en +0x34 (el byte +0x36 es su tercer byte); kModelIdx: a[3] = dword en +0x20 (la palabra +0x22 es la alta)
//   kPos: a[3..7] = izquierda, abajo, derecha, arriba, Z (bits) en +8, +0xC, +0x10, +0x14, +0x18; a[8] = modo de la salida (0 en B; 1..9: en A en this + {0,4,8,0xC,0x10,0x14,0x18,0x1C,0x20}); a[9] = desplazamiento en B
//   kBool: a[3] = byte de 0x96BFE4 (0 o 1)
//   kDetonator: a[3] = ranura (int32); a[4] = desplazamiento de los datos del jugador en B; a[5] = arma guardada (+0x70C); a[6] = desplazamiento de la info del arma en C; a[7] = EAX que devuelve GiveWeapon
//   kConstant: a[3] = dword en [this] (no se lee); kAnimBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = animacion (+0x10 de GiveCPR, +0xC de HitHead; 0 o cruda)
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
    case kSdd: case kCtorNoop:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kAddList:
        c.a[3] = Raw(g); c.a[4] = Raw(g); c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 0u, 1u, 1u, 1u, 2u, 3u, 4u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = Offset(g, g.R(4), 12);
        c.a[9] = Raw(g); c.a[10] = Raw(g); c.a[11] = Raw(g);
        break;
    case kStatus:
        c.a[3] = g.P(30) ? ((g.W() & 0xFF00FFFFu) | (g.R(256) << 16)) : g.W();
        break;
    case kModelIdx:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0xFFFF0000u, 0x80000000u, 0x7FFF0000u, 0x00010000u, 0x80010000u, g.W(), g.W(), g.W(), g.W() & 0x0000FFFFu, g.W() | 0xFFFF0000u, (g.W() & 0xFFFFu) | 0x80000000u});
        break;
    case kPos: {
        const u32 mode = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u});
        c.a[8] = mode;
        c.a[9] = Offset(g, g.R(4), 12);
        u32 v[5];
        for (auto& w : v) w = mode ? NonNanBits(g) : FloatBitsOf(g);
        if (g.P(8)) v[2] = v[0] ^ 0x80000000u;                // derecha = -izquierda (cancelacion exacta)
        if (g.P(8)) v[3] = v[1] ^ 0x80000000u;
        if (g.P(15)) v[2] = g.Pk({0x7F7FFFFFu, 0x7F000000u, 0x7F7FFFFEu, 0xFF7FFFFFu, 0x3F800000u});   // sumas cerca del desbordamiento de float (la x87 tiene exponente ampliado)
        if (g.P(15)) v[0] = g.Pk({0x7F7FFFFFu, 0x7F000000u, 0x7F7FFFFEu, 0xFF7FFFFFu, 0x3F800000u});
        if (g.P(10)) { v[2] = g.Pk({0x00000001u, 0x007FFFFFu, 0x00800000u, 0x80000001u}); v[0] = g.Pk({0x00000001u, 0x007FFFFFu, 0x00800000u, 0x80000001u, 0x80800000u}); }   // denormales
        if (g.P(15)) v[3] = g.Pk({0x7F7FFFFFu, 0x7F000000u, 0xFF7FFFFFu, 0x00000001u});
        if (g.P(15)) v[1] = g.Pk({0x7F7FFFFFu, 0x7F000000u, 0xFF7FFFFFu, 0x00000001u});
        if (!mode) {
            if (IsNan(v[0]) && IsNan(v[2])) v[2] = NonNanBits(g);
            if (IsNan(v[1]) && IsNan(v[3])) v[3] = NonNanBits(g);
        }
        for (unsigned i = 0; i < 5; ++i) c.a[3 + i] = v[i];
        break;
    }
    case kBool:
        c.a[3] = g.R(2);
        break;
    case kDetonator:
        c.a[3] = g.Pk({12u, 12u, 12u, 12u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 13u, 0x30u, u32(-1), u32(-2), u32(-0x20), g.R(0x31), u32(int(g.R(0x51)) - 0x20)});
        c.a[4] = Offset(g, g.R(4), 0x24);
        c.a[5] = g.Pk({0x37u, 0x37u, 0x37u, 0x28u, 0u, 1u, 0x36u, 0x38u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(0x40)});
        c.a[6] = Offset(g, g.R(4), 0x18);
        c.a[7] = Raw(g);
        break;
    case kConstant:
        c.a[3] = Raw(g);
        break;
    case kAnimBody:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x86D48Cu, 0x86D4A8u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.P(45) ? 0u : Raw(g);
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
