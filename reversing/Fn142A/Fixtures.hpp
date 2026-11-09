#pragma once
// Fixtures del banco Fn142A (lote 142A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py):
//   idx  direccion  nombre real                                                     forma
//   0   0x658830   CTaskComplexWanderMedic::ScalarDeletingDestructor        thiscall(flags) `ret 4`, 30 B: call 658850; (flags&1) -> CTask::operator delete 0x61A5B0; eax = this
//   1   0x658850   CTaskComplexWanderMedic::Destructor (cuerpo)              thiscall() `ret`: mov [ecx],86F48Ch; jmp 669D20 (cuerpo de ~CTaskComplexWander)
//   2   0x48E5E0   CTaskComplexWanderStandard::ScalarDeletingDestructor     idem; cuerpo 0x48E600, delete 0x61A5B0
//   3   0x48E600   CTaskComplexWanderStandard::Destructor (cuerpo)           thiscall() `ret`: mov [ecx],85A200h; jmp 669D20
//   4   0x4636C0   CTaskSimpleFinishBrain::ScalarDeletingDestructor          idem; cuerpo 0x4636E0, delete 0x61A5B0 (vtable 0x859D20)
//   5   0x4636E0   CTaskSimpleFinishBrain::Destructor (cuerpo)                thiscall() `ret`: mov [ecx],859CFCh; jmp 61A3A0 (cuerpo de ~CTask)
//   6   0x492D40   CTaskSimpleTogglePedThreatScanner::Destructor (cuerpo)     thiscall() `ret`: mov [ecx],85A334h; jmp 61A3A0
//   7   0x492DF0   CTaskSimpleTogglePedThreatScanner::ScalarDeletingDestructor  idem; cuerpo 0x492D40, delete 0x61A5B0
//   8   0x4C5680   CTimeModelInfo::Destructor (cuerpo)                        thiscall() `ret`: mov [ecx],85BB9Ch
//   9   0x4C5690   CLodTimeModelInfo::Constructor (el listado decia CTimeModelInfo)  thiscall() `ret`: call 4C4A60 (+8 = 0, +0xA = 0xFFFF); +0x20 = 0 (word), +0x22 = 0 (word); mov [esi],85BCF0h; +0x26 = 0xFFFF; eax = this
//  10   0x5A40E0   CProjectile::Destructor (cuerpo; el listado decia ProcObjectMan_c)  thiscall() `ret`: mov [ecx],867030h; jmp 59F660 (cuerpo de ~CObject, sin hook)
//  11   0x5A40F0   CProjectile::ScalarDeletingDestructor                      idem; cuerpo 0x5A40E0, delete 0x5A1F20 (CObject::operator delete, sin hook)
//  12   0x4F1020   CAEStaticChannel::ScalarDeletingDestructor (27 B)          thiscall(flags) `ret 4`: call 4F0B50 (sin hook); (flags&1) -> operator delete 0x8214BD (CRT); eax = this
//  13   0x4F11D0   CAEStreamThread::Constructor (el listado decia CAEStaticChannel)  thiscall() `ret`: mov eax,ecx
//  14   0x4F11E0   CAEStreamThread::Destructor                                thiscall() `ret`: add ecx,38h; push ecx; call [0x8580E0] (DeleteCriticalSection); ret
//  15   0x697EF0   CTaskComplexWaitForBackup::ScalarDeletingDestructor (el listado decia CTaskComplexReactToGunAimedAt)  cuerpo 0x697330 (con hook), delete 0x61A5B0
//  16   0x6986E0   CPointList::Constructor (el listado decia CTaskComplexReactToGunAimedAt)  thiscall() `ret`: call 699F00 ([ecx] = 0; 6 dwords a cero en +0x124); eax = this
//  17   0x5F6880   CTaskSimpleWaitUntilLeaderAreaCodesMatch::ScalarDeletingDestructor  cuerpo 0x5F67E0 (sin hook), delete 0x61A5B0
//  18   0x5F68A0   CPedList::Constructor (el listado decia CTaskSimpleWaitUntilLeaderAreaCodesMatch)  thiscall() `ret`: call 699DB0 (CPedList::Empty, con hook); eax = this
//  19   0x4B2670   CEventDanger::GetEventType                                 thiscall() `ret`: mov eax,4Bh        (vtable 0x85B848, ranura 1)
//  20   0x4B2680   CEventDanger::GetLifeTime                                  thiscall() `ret`: xor eax,eax        (ranura 3)
//  21   0x4B2690   CEventDanger::GetEventPriority                             thiscall() `ret`: mov eax,14h        (ranura 2)
//  22   0x4B27B0   CEventVehicleHitAndRun::IsCriminalEvent (el listado decia CEventDanger)  thiscall() `ret`: [[this+10h]+460h] != 0 && CPed::IsPlayer (0x5DF8F0, con hook)  (vtable 0x85AFA8, ranura 7)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40).
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

namespace F142AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;   // ventana de cada arena
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }

// Bytes que cada funcion toca a partir de su puntero `this` (para colocarlo al borde de la ventana)
constexpr u32 kSpanThis = 4;          // solo el dword de la vtable (o nada)
constexpr u32 kSpanLodCtor = 0x28;    // CLodTimeModelInfo::Constructor escribe +0, +8, +0xA, +0x20, +0x22 y +0x26
constexpr u32 kSpanStream = 0x3C;     // CAEStreamThread::Destructor: ECX = this; pasa this + 0x38 a DeleteCriticalSection
constexpr u32 kSpanPoint = 0x13C;     // CPointList::Constructor: +0 (dword) y 24 bytes en +0x124
constexpr u32 kSpanEvent = 0x14;      // CEventVehicleHitAndRun::IsCriminalEvent lee +0x10
constexpr u32 kSpanVehicle = 0x464;   // el vehiculo sintetico (arena B): +0x460 = m_pDriver

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

// Indices (ver la cabecera)
inline constexpr unsigned kIdxMedicSdd = 0, kIdxMedicBody = 1, kIdxStandardSdd = 2, kIdxStandardBody = 3, kIdxFinishSdd = 4, kIdxFinishBody = 5, kIdxScannerBody = 6, kIdxScannerSdd = 7,
                          kIdxTimeBody = 8, kIdxLodCtor = 9, kIdxProjectileBody = 10, kIdxProjectileSdd = 11, kIdxStaticSdd = 12, kIdxStreamCtor = 13, kIdxStreamDtor = 14,
                          kIdxBackupSdd = 15, kIdxPointCtor = 16, kIdxLeaderSdd = 17, kIdxPedListCtor = 18, kIdxDangerType = 19, kIdxDangerLife = 20, kIdxDangerPriority = 21,
                          kIdxHitRunCriminal = 22;
inline constexpr bool IsSdd(unsigned fn) {
    return fn == kIdxMedicSdd || fn == kIdxStandardSdd || fn == kIdxFinishSdd || fn == kIdxScannerSdd || fn == kIdxProjectileSdd || fn == kIdxStaticSdd || fn == kIdxBackupSdd || fn == kIdxLeaderSdd;
}
// cuerpos de destructor con un colaborador falso (vtable propia de la clase y salto al cuerpo de la base)
inline constexpr bool IsBodyWithCollab(unsigned fn) {
    return fn == kIdxMedicBody || fn == kIdxStandardBody || fn == kIdxFinishBody || fn == kIdxScannerBody || fn == kIdxProjectileBody;
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y reserva; el resto por funcion
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    switch (c.fn) {
    case kIdxMedicSdd: case kIdxStandardSdd: case kIdxFinishSdd: case kIdxScannerSdd: case kIdxProjectileSdd: case kIdxStaticSdd: case kIdxBackupSdd: case kIdxLeaderSdd: {
        // a[3] = vtable previa en [this]; a[4] = banderas (argumento de pila, solo importa el bit 0); a[5] = dword que el falso del cuerpo deja en [this]; a[6] = EAX del falso del cuerpo (no es this)
        c.a[0] = Offset(g, kind, kSpanThis);
        u32 own = 0;
        switch (c.fn) {
        case kIdxMedicSdd: own = 0x86F48Cu; break;
        case kIdxStandardSdd: own = 0x85A200u; break;
        case kIdxFinishSdd: own = 0x859D20u; break;
        case kIdxScannerSdd: own = 0x85A334u; break;
        case kIdxProjectileSdd: own = 0x867030u; break;
        case kIdxStaticSdd: own = 0x85F3CCu; break;
        case kIdxBackupSdd: own = 0x870DA4u; break;
        default: own = 0x86C7E8u; break;
        }
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, own, own, own + 1u, own - 4u, 0x86D48Cu, 0x86D4CCu, 0x85AB38u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, g.W(), g.W(), g.W() | 1u, g.W() & ~1u});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 0x86D48Cu, 0x86D4CCu, 0x859CFCu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[6] = Raw(g);
        break;
    }
    case kIdxMedicBody: case kIdxStandardBody: case kIdxFinishBody: case kIdxScannerBody: case kIdxProjectileBody: {
        // a[3] = vtable previa en [this]; a[4] = EAX del falso del colaborador (~CTaskComplexWander 0x669D20, ~CTask 0x61A3A0 o ~CObject 0x59F660)
        c.a[0] = Offset(g, kind, kSpanThis);
        u32 own = 0;
        switch (c.fn) {
        case kIdxMedicBody: own = 0x86F48Cu; break;
        case kIdxStandardBody: own = 0x85A200u; break;
        case kIdxFinishBody: own = 0x859CFCu; break;
        case kIdxScannerBody: own = 0x85A334u; break;
        default: own = 0x867030u; break;
        }
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, own, own, own + 1u, own - 4u, 0x86D48Cu, 0x86FE84u, 0x866F60u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = Raw(g);
        break;
    }
    case kIdxTimeBody:      // a[3] = vtable previa
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85BCB0u, 0x85BB9Cu, 0x85BB9Du, 0x85BB98u, 0x85BCF0u, g.W(), g.W(), 0xDEADBEEFu});
        break;
    case kIdxLodCtor:       // a[3] = dword previo en +0; a[4] = dword previo en +8 (el constructor escribe los words +8 y +0xA); a[5] = dword previo en +0x20; a[6] = dword previo en +0x24 (el constructor escribe el word +0x26)
        c.a[0] = Offset(g, kind, kSpanLodCtor);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85BCF0u, 0x85BB9Cu, g.W(), g.W(), 0xDEADBEEFu});
        c.a[4] = g.Pk({0u, 0xFFFF0000u, 0xFFFFFFFFu, 0x0000FFFFu, 0x12345678u, g.W(), g.W()});
        c.a[5] = g.Pk({0u, 0xFFFFFFFFu, 0xFFFF0000u, 0x0000FFFFu, g.W(), g.W()});
        c.a[6] = g.Pk({0u, 0xFFFFFFFFu, 0xFFFF0000u, 0x0000FFFFu, 0x0000FFFFu, g.W(), g.W()});
        break;
    case kIdxStreamCtor:    // a[3] = dword en [this] (no se lee)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);
        break;
    case kIdxStreamDtor:    // a[3] = dword en [this + 0x38] (la seccion critica; el falso de DeleteCriticalSection no la lee)
        c.a[0] = Offset(g, kind, kSpanStream);
        c.a[3] = Raw(g);
        break;
    case kIdxPointCtor:     // a[3] = dword previo en +0; a[4] = dword previo en +0x124
        c.a[0] = Offset(g, kind, kSpanPoint);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 1u, 24u, 0x18u, g.W(), g.W()});
        c.a[4] = g.Pk({0u, 0x01010101u, 0x00000001u, 0x01000000u, 0xFFFFFFFFu, g.W(), g.W()});
        break;
    case kIdxPedListCtor:   // a[3] = dword previo en +0 (el falso de CPedList::Empty no escribe)
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 1u, 30u, g.W(), g.W()});
        break;
    case kIdxHitRunCriminal: {
        // a[3] = desplazamiento del vehiculo sintetico en la arena B; a[4] = selector del conductor (0 nulo, 1 puntero a la arena C, 2 crudo no nulo); a[5] = desplazamiento del conductor en la arena C;
        // a[6] = dword crudo del conductor (selector 2); a[7] = EAX del falso de CPed::IsPlayer; a[8] = dword previo en [this + 0x10] (siempre se sustituye: el vehiculo)
        c.a[0] = Offset(g, kind, kSpanEvent);
        c.a[3] = Offset(g, g.R(4), kSpanVehicle);
        c.a[4] = g.Pk({0u, 1u, 1u, 2u});
        c.a[5] = Offset(g, g.R(4), 4);
        u32 raw = Raw(g);
        if (!raw) raw = 0x00B74000u + g.R(0x1000);
        c.a[6] = raw;
        c.a[7] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF01u, 0xFFu, 0x80u, 0xFFFFFF00u, g.W(), g.W(), 0xDEADBEEFu});
        c.a[8] = Raw(g);
        break;
    }
    default:         // constantes (19..21): `this` en la arena A
        c.a[0] = Offset(g, kind, kSpanThis);
        c.a[3] = Raw(g);   // dword en [this] (el EXE no lo lee)
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
