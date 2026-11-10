#pragma once
// Fixtures del banco Fn143A (lote 143A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                              forma
//    0   0x4ABAA0   CEventGlobalGroup::ScalarDeletingDestructor              thiscall(flags) `ret 4`: call 4AB9B0; test byte [esp+8],1; operator delete 8214BDh; eax=this  (vtable 0x85AAB4, ranura 0)
//    1   0x734EA0   RwIm2DRenderIndexedPrimitiveThunk                         cdecl(5 args): mov edx,[0C97B24h]; jmp [edx+34h]
//    2   0x734EC0   RwIm2DRenderLineThunk                                     cdecl(4 args): mov ecx,[0C97B24h]; jmp [ecx+28h]
//    3   0x4C4A60   CBaseModelInfo::ConstructorBody                           thiscall() `ret`: mov eax,ecx; [eax]=85BB9Ch; word [eax+8]=0; word [eax+0Ah]=0FFFFh
//    4   0x4C5540   CAtomicModelInfo::ConstructorBody                         thiscall() `ret`: push esi; call 4C4A60; mov [esi],85BBF0h; eax=this
//    5   0x4C5920   CVehicleModelInfo::DestructorBody                         thiscall() `ret`: mov dword [ecx],85BB9Ch
//    6   0x4AC6B0   CEventGunShot::GetEventType (ranura 1)                    thiscall() `ret`: mov eax,0Fh
//    7   0x4AC6C0   CEventGunShot::GetLifeTime (ranura 3)                     thiscall() `ret`: xor eax,eax
//    8   0x4AC6D0   CEventGunShot::GetSourceEntity (ranura 10)                thiscall() `ret`: mov eax,[ecx+14h]
//    9   0x4AC6E0   CEventGunShot::GetLocalSoundLevel (ranura 12)             thiscall() `ret`: byte [ecx+30h] ? fld [858B50h] (0.0) : fld [858970h] (160.0)  (ST0)
//   10   0x4AC700   CEventGunShot::CanBeInterruptedBySameEvent (ranura 15)    thiscall() `ret`: mov al,1
//   11   0x4AC710   CEventGunShot::GetEventPriority (ranura 2)                thiscall() `ret`: mov eax,23h
//   12   0x4AC830   CEventGunShot::ReportCriminalEvent (ranura 8)             thiscall(ped) `ret 4`: vacia
//   13   0x6A0650   CEventVehicleDamageCollision::DestructorBody              thiscall() `ret`: mov [ecx],8710D0h; jmp 4B1990
//   14   0x6A0660   CEventVehicleDamageCollision::GetEventType (ranura 1)     thiscall() `ret`: mov eax,49h
//   15   0x6A06D0   CEventVehicleDamageCollision::ScalarDeletingDestructor    thiscall(flags) `ret 4`: call 6A0650; ... CEvent::operator delete 4B5630  (vtable 0x8710D0, ranura 0)
//   16   0x61C310   CEventVehicleDamageWeapon::DestructorBody                 thiscall() `ret`: mov [ecx],86D640h; jmp 4B1990
//   17   0x61C320   CEventVehicleDamageWeapon::GetEventType (ranura 1)        thiscall() `ret`: mov eax,29h
//   18   0x61C390   CEventVehicleDamageWeapon::ScalarDeletingDestructor       thiscall(flags) `ret 4`: call 61C310; ... CEvent::operator delete 4B5630  (vtable 0x86D640, ranura 0)
//   19   0x658AD0   CTaskComplexTreatAccident::GetTaskType (ranura 4)         thiscall() `ret`: mov eax,65h
//   20   0x658AE0   CTaskComplexTreatAccident::DestructorBody                 thiscall() `ret`: mov [ecx],86F4ECh; jmp 61A3D0
//   21   0x659FC0   CTaskComplexMedicTreatInjuredPed::ScalarDeletingDestructor thiscall(flags) `ret 4`: call 658C30; ... CTask::operator delete 61A5B0 (vtable 0x86F518, ranura 0)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40) y, donde hace falta (1 y 2), memoria global del juego (0xC97B24, se guarda y se restaura tras cada lado).
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

namespace F143AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 22;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Bytes que cada funcion toca a partir de su puntero `this` (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable (o nada)
constexpr u32 kSpanCtor = 0xC;        // constructor de CBaseModelInfo: escribe hasta +0xB
constexpr u32 kSpanSource = 0x18;     // GetSourceEntity lee +0x14
constexpr u32 kSpanSound = 0x31;      // GetLocalSoundLevel lee el byte +0x30
constexpr u32 kSpanGlobals = 0x38;    // RwGlobals falso: el EXE lee los punteros de +0x28 y +0x34

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

// Indices (ver la cabecera)
inline constexpr unsigned kIdxGroupDelete = 0, kIdxIndexed = 1, kIdxLine = 2, kIdxBaseCtor = 3, kIdxAtomicCtor = 4, kIdxVehDtor = 5, kIdxShotType = 6, kIdxShotLife = 7, kIdxShotSource = 8,
                          kIdxShotSound = 9, kIdxShotCanInt = 10, kIdxShotPrio = 11, kIdxShotReport = 12, kIdxCollBody = 13, kIdxCollType = 14, kIdxCollDelete = 15, kIdxWeapBody = 16,
                          kIdxWeapType = 17, kIdxWeapDelete = 18, kIdxTreatType = 19, kIdxTreatBody = 20, kIdxMedicDelete = 21;
constexpr bool IsScalarDeleting(u32 fn) { return fn == kIdxGroupDelete || fn == kIdxCollDelete || fn == kIdxWeapDelete || fn == kIdxMedicDelete; }
constexpr bool IsBodyWithCollab(u32 fn) { return fn == kIdxCollBody || fn == kIdxWeapBody || fn == kIdxTreatBody; }
constexpr bool IsRwThunk(u32 fn) { return fn == kIdxIndexed || fn == kIdxLine; }
constexpr bool IsConstantFn(u32 fn) { return fn == kIdxShotType || fn == kIdxShotLife || fn == kIdxShotPrio || fn == kIdxShotCanInt || fn == kIdxCollType || fn == kIdxWeapType || fn == kIdxTreatType; }

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o del RwGlobals falso, en la arena B) en su arena, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global;
//   a[3] = dword previo en [this] / argumento crudo / byte de m_bHasNoSound; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [ECX] (gDtorWrite); a[6] = banderas (destructores con borrado)
//   funciones 1 y 2 (RW): a[3..5] argumentos, a[6] modo (bit 0: vertices en la arena C, bit 1: indices en la arena C), a[7] / a[8] vertices e indices crudos, a[9] / a[10] desplazamientos en la arena C, a[11] EAX del falso
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    if (IsRwThunk(c.fn)) {
        c.a[0] = Offset(g, kind, kSpanGlobals);
        c.a[3] = Raw(g); c.a[4] = Raw(g); c.a[5] = Raw(g);
        c.a[6] = g.R(4);
        c.a[7] = Raw(g); c.a[8] = Raw(g);
        c.a[9] = 4 * g.R((u32(kWin) - 8) / 4); c.a[10] = 4 * g.R((u32(kWin) - 8) / 4);
        c.a[11] = Raw(g);
        return;
    }
    if (IsScalarDeleting(c.fn)) {   // destructores con borrado: a[3] = vtable previa en [this]; a[4] = EAX del falso del cuerpo; a[5] = dword que deja en [ECX]; a[6] = banderas
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85AAB4u, 0x85AAB0u, 0x8710D0u, 0x86D640u, 0x86F518u, 0x86F4ECu, 0x85AB38u, 0x85AAB5u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        return;
    }
    switch (c.fn) {
    case kIdxBaseCtor:   // constructor: toca +0, +8, +0xA (relleno previo de toda la arena)
        c.a[0] = Offset(g, kind, kSpanCtor);
        c.a[3] = Raw(g);
        break;
    case kIdxAtomicCtor: // constructor con colaborador (0x4C4A60 falso): a[3] = dword previo en [this]; a[4] / a[5] = EAX y dword del falso
        c.a[0] = Offset(g, kind, kSpanCtor);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85BB9Cu, 0x85BBF0u, 0x85BCB0u, 0x85BB9Du, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kIdxVehDtor: case kIdxCollBody: case kIdxWeapBody: case kIdxTreatBody:   // cuerpos de destructor: a[3] = dword previo en [this]
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85BB9Cu, 0x8710D0u, 0x86D640u, 0x86F4ECu, 0x85B588u, 0x85AB38u, 0x85BB9Du, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kIdxShotSource:  // a[3] = dword en +0x14
        c.a[0] = Offset(g, kind, kSpanSource);
        c.a[3] = Raw(g);
        break;
    case kIdxShotSound:   // a[3] = byte en +0x30 (m_bHasNoSound): cero o no
        c.a[0] = Offset(g, kind, kSpanSound);
        c.a[3] = g.Pk({0u, 0u, 0u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, g.W() & 0xFFu, g.W() & 0xFFu, (g.W() & 0xFFu) | 1u});
        break;
    case kIdxShotReport:  // ret 4: a[3] = el peaton (argumento de pila, no se lee)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
        break;
    default:              // constantes (6, 7, 10, 11, 14, 17, 19): `this` en la arena A; a[3] = dword que hay en [this] (no se lee)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
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
