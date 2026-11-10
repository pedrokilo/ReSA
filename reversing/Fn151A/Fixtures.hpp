#pragma once
// Fixtures del banco Fn151A (lote 151A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py).
//   idx  direccion  nombre real                                                              forma
//    0   0x661F10   CTaskComplexSeekEntity<XYOffset>::ScalarDeletingDestructor (vtable 0x86FBB4)  thiscall(flags) `ret 4`: cuerpo 661F30 (con hook), CTask::operator delete 61A5B0 si bit 0
//    1   0x665DC0   CTaskComplexSeekEntity<XYOffset>::GetSeekPos                            thiscall(salida, ped) `ret 8`: calculador virtual en this+0x40 (entidad +0xC), temporal copiado a la salida; eax = salida
//    2   0x4AC370   CInformGroupEvent::Destructor-2 (copia de 0x4B29A0)                       thiscall() `ret`: CleanUpOldReference(ped), delete virtual del evento, tiempo = -1
//    3   0x4AC3B0   CInformGroupEvent::Constructor-2 (cuatro argumentos)                      thiscall(ped, grupo, evento, retardo) `ret 10h`: RegisterReference, campos y reloj + retardo (eax = el valor)
//    4   0x684750   CTaskComplexPartnerShove::DestructorBody                                  thiscall() `ret`: [this] = 0x870800; cuerpo 683A40 (con hook)
//    5   0x684760   CTaskComplexWalkAlongsidePed::ScalarDeletingDestructor (vtable 0x8706CC)  cuerpo 6832C0 (con hook)
//    6   0x684780   CTaskComplexWaitForPed::ScalarDeletingDestructor (vtable 0x8706F8)        cuerpo 6833D0 (sin hook)
//    7   0x6847A0   CTaskComplexBeInCouple::ScalarDeletingDestructor (vtable 0x870724)        cuerpo 683780 (con hook)
//    8   0x690CB0   CTaskSimpleFacial::DestructorBody                                         thiscall() `ret`: [this] = 0x870AB8; ~CTask 61A3A0 (con hook)
//    9   0x692F90   CTaskComplexFacial::ScalarDeletingDestructor (vtable 0x870ADC)            cuerpo 690D90 (con hook)
//   10   0x692FB0   CTaskSimpleAffectSecondaryBehaviour::ScalarDeletingDestructor (vtable 0x870B08)  cuerpo 6912C0 (con hook)
//   11   0x692FD0   CTaskSimpleHoldEntity::ScalarDeletingDestructor (vtable 0x870B2C)         cuerpo 691630 (sin hook)
//   12   0x5F65A0   CTaskSimpleChat::Constructor                                              thiscall(duracion) `ret 4`: RunTimedAnim ctor 61ABE0 (con hook), [this] = 0x86C78C; eax = this
//   13   0x5F7200   CPedGroupDefaultTaskAllocators::Get                                       cdecl(tipo): 0xC09908 + {0,4,8,0xC,0x14,0x10}; 0 si tipo > 5; EDX intacto
//   14   0x691A40   CTaskComplexGoPickUpEntity::GetTaskType (vtable 0x870B98, ranura 4)       thiscall() `ret`: mov eax,136h
//   15   0x692C80   CTaskComplexGoPickUpEntity::Clone (ranura 1)                              thiscall() `ret`: CTask::operator new 61A5A0 (con hook), ctor 6919C0 (con hook); eax = lo que deja el ctor
//   16   0x602BE0   CEventEscalator::ScalarDeletingDestructor (vtable 0x86CD00)               cuerpo 5FF860 (sin hook), CEvent::operator delete 4B5630 (sin hook)
//   17   0x602B90   CEventEscalator::Clone (ranura 4)                                         thiscall() `ret`: CEvent::operator new 4B5620 (con hook), ctor 4ABFC0 (con hook), [mem] = 0x86CD00
//   18   0x61A350   CTask::Destructor-2 (copia de 0x61A3A0)                                   thiscall() `ret`: mov [ecx],86D48Ch (EAX, ECX y EDX intactos)
//   19   0x61A430   CTaskComplex::SetSubTask (ranura 7 de unas cien vtables)                  thiscall(subtarea) `ret 4`
//   20   0x61A2E0   GetPlayerRelationshipRecorder                                              cdecl() `ret`: global 0xC17084; operator new del CRT 82119A (sin hook)
//   21   0x48EA90   CTaskSimpleLookAbout::DestructorBody                                      thiscall() `ret`: jmp 61A6F0
//   22   0x48EA40   CTaskSimpleHandsUp::ScalarDeletingDestructor (vtable 0x85A29C)            cuerpo 48EA60 (con hook)
//   23   0x48EA70   CTaskSimpleLookAbout::ScalarDeletingDestructor (vtable 0x85A164)          cuerpo 48EA90 (con hook, funcion 21)
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

namespace F151AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 24;

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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B o C), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

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
// Un dword de 32 bits como float en la salida del calculador: patrones con NaN (silenciosos y senalizantes), infinitos, ceros con signo, denormales, extremos y aleatorios (se copian como bits)
inline u32 FloatBitsOf(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu, w6 = 0x3F800000u + (g.W() & 0x7FFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFFFFFFFFu, w1, w2, w3, w4, w5, w6});
}

// Clases de funcion (forma)
enum Kind : u32 { kSdd, kGetSeek, kInfDtor, kInfCtor, kVtBody, kChat, kAlloc, kConst, kClonePick, kEscClone, kTaskVt, kSetSub, kRelRec };
inline constexpr std::array<Kind, kFunctions> kKind{
    kSdd, kGetSeek, kInfDtor, kInfCtor, kVtBody, kSdd, kSdd, kSdd, kVtBody, kSdd,
    kSdd, kSdd, kChat, kAlloc, kConst, kClonePick, kSdd, kEscClone, kTaskVt, kSetSub,
    kRelRec, kVtBody, kSdd, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x661F10, 0x665DC0, 0x4AC370, 0x4AC3B0, 0x684750, 0x684760, 0x684780, 0x6847A0, 0x690CB0, 0x692F90, 0x692FB0, 0x692FD0,
                                                    0x5F65A0, 0x5F7200, 0x691A40, 0x692C80, 0x602BE0, 0x602B90, 0x61A350, 0x61A430, 0x61A2E0, 0x48EA90, 0x48EA40, 0x48EA70};
// Colaborador que llama la funcion y falsea FkBody (cuerpo del destructor, ~CTask...; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0x661F30, 0, 0, 0, 0x683A40, 0x6832C0, 0x6833D0, 0x683780, 0x61A3A0, 0x690D90, 0x6912C0, 0x691630,
                                                      0, 0, 0, 0, 0x5FF860, 0, 0, 0, 0, 0x61A6F0, 0x48EA60, 0x48EA90};
// Funcion de liberacion de los destructores con borrado (CTask::operator delete, CEvent::operator delete)
inline constexpr std::array<u32, kFunctions> kDelete{0x61A5B0, 0, 0, 0, 0, 0x61A5B0, 0x61A5B0, 0x61A5B0, 0, 0x61A5B0, 0x61A5B0, 0x61A5B0,
                                                     0, 0, 0, 0, 0x4B5630, 0, 0, 0, 0, 0, 0x61A5B0, 0x61A5B0};
// vtable de la clase que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x86FBB4, 0, 0, 0, 0x870800, 0x8706CC, 0x8706F8, 0x870724, 0x870AB8, 0x870ADC, 0x870B08, 0x870B2C,
                                                     0x86C78C, 0, 0x870B98, 0x870B98, 0x86CD00, 0x86CD00, 0x86D48C, 0, 0, 0x85A164, 0x85A29C, 0x85A164};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x48, 0x10, 0x10, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0x30, 4, 4, 4, 0xC, 4, 4, 4, 4};
// Memoria global del juego que tocan InformCtor (reloj de juego) y RelRec (puntero al registrador)
constexpr u32 kGlobalTimer = 0xB7CB84;
constexpr u32 kGlobalRecorder = 0xC17084;
// Desplazamientos de la salida de GetSeekPos en la arena A respecto a `this` (modos 1..5 de a[8])
inline constexpr std::array<u32, 5> kSeekAlias{0x00, 0x08, 0x0C, 0x38, 0x40};

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kGetSeek: a[3] = entidad (+0xC); a[4] = peaton (argumento); a[5..7] = floats que escribe el falso del calculador; a[8] = modo de la salida (0 en B a[9]; 1..5 en A en this + kSeekAlias); a[9] = desplazamiento en B
//   kInfDtor: a[3] = peaton (+0); a[4] = grupo (+4); a[5] = modo del evento (0 nulo, 1 objeto en B a[6]); a[6] = desplazamiento del evento en B; a[7] = tiempo previo (+0xC); a[8] = EAX del falso del destructor del evento
//   kInfCtor: a[3] = peaton; a[4] = grupo; a[5] = evento; a[6] = retardo; a[7] = reloj de juego; a[8..11] = dwords previos en +0, +4, +8, +0xC
//   kVtBody: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite
//   kChat: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite; a[6] = duracion
//   kAlloc: a[3] = tipo (argumento de pila)
//   kConst: a[3] = dword en [this] (no se lee)
//   kClonePick: a[3] = entidad (+0xC); a[4] = grupo de animacion (+0x2C); a[5] = modo de la memoria (0 nula, 1 en B a[6], 2 en A a[6]); a[6] = desplazamiento; a[7] = EAX del constructor (0 memoria, 1 memoria + 4, 2 cero, 3 crudo a[9]); a[8] = gDtorWrite; a[9] = crudo
//   kEscClone: a[3] = modo de la memoria (0 nula, 1 en B a[4]); a[4] = desplazamiento; a[5] = gDtorWrite; a[6] = crudo; a[7] = selector del EAX del constructor (como kClonePick)
//   kTaskVt: a[3] = dword previo en [this]
//   kSetSub: a[3] = modo de la subtarea actual (+8) (0 nula, 1 objeto en B a[5]); a[4] = modo del argumento (0 nulo, 1 objeto en C a[6], 2 = la actual, 3 = this, 4 = objeto en C a[6]);
//            a[5], a[6] = desplazamientos; a[7] = dword previo en +4; a[8] = dword previo en +0; a[9] = EAX del falso del destructor
//   kRelRec: a[3] = modo de la variable global (0 nula, 1 un objeto en C a[4]); a[4] = desplazamiento; a[5] = modo de la reserva (0 nula, 1 bloque en B a[6]); a[6] = desplazamiento
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
    case kSdd:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kGetSeek:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        c.a[5] = FloatBitsOf(g); c.a[6] = FloatBitsOf(g); c.a[7] = FloatBitsOf(g);
        c.a[8] = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u});
        c.a[9] = Offset(g, g.R(4), 12);
        break;
    case kInfDtor:
        c.a[3] = g.Pk({0u, 0u, Raw(g), Raw(g), Raw(g)});
        c.a[4] = Raw(g);
        c.a[5] = g.Pk({0u, 1u, 1u, 1u});
        c.a[6] = AuxOffset(g, 4);
        c.a[7] = Raw(g);
        c.a[8] = Raw(g);
        break;
    case kInfCtor:
        c.a[3] = g.Pk({0u, 0u, Raw(g), Raw(g), Raw(g)});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, 100u, 0x7FFFFFFFu, 0x80000000u, Raw(g), Raw(g), Raw(g)});
        c.a[7] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, Raw(g), Raw(g), Raw(g)});
        c.a[8] = Raw(g); c.a[9] = Raw(g); c.a[10] = Raw(g); c.a[11] = Raw(g);
        break;
    case kVtBody:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x86D48Cu, 0x86D4A8u, 0x86D4CCu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kChat:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x86D504u, 0x86D4A8u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 0xF423Fu, 0xFA0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, Raw(g), Raw(g), Raw(g)});
        break;
    case kAlloc:
        c.a[3] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 0xFFu, 0x100u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0x7FFFFFFFu, g.W(), g.W(), g.R(16)});
        break;
    case kConst:
        c.a[3] = Raw(g);
        break;
    case kClonePick:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        c.a[5] = g.Pk({0u, 1u, 1u, 2u});
        c.a[6] = c.a[5] == 2 ? Offset(g, g.R(4), 0x34) : AuxOffset(g, 0x34);
        c.a[7] = g.R(4);
        c.a[8] = Raw(g);
        c.a[9] = Raw(g);
        break;
    case kEscClone:
        c.a[3] = g.Pk({0u, 1u, 1u, 1u});
        c.a[4] = AuxOffset(g, 0xC);
        c.a[5] = Raw(g);
        c.a[6] = Raw(g);
        c.a[7] = g.R(4);
        break;
    case kTaskVt:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x86D4A8u, 0x86D4CCu, g.W(), g.W(), 0xDEADBEEFu});
        break;
    case kSetSub:
        c.a[3] = g.Pk({0u, 1u, 1u, 1u});
        c.a[4] = g.Pk({0u, 1u, 1u, 2u, 2u, 3u, 4u});
        c.a[5] = AuxOffset(g, 0xC);
        c.a[6] = AuxOffset(g, 0xC);
        c.a[7] = Raw(g);
        c.a[8] = Raw(g);
        c.a[9] = Raw(g);
        break;
    case kRelRec:
        c.a[3] = g.Pk({0u, 0u, 1u});
        c.a[4] = AuxOffset(g, 0x80);
        c.a[5] = g.Pk({0u, 1u, 1u, 1u});
        c.a[6] = AuxOffset(g, 0x80);
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
