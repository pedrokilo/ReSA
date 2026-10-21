#pragma once
// Fixtures del banco SmallHelpers64A (lote 64): GetMillisecondTime (0x5617C0), DIReleaseMouse (0x746F70), RemoveUpgradeCB (0x6D3300) y FindUpgradeCB (0x6D3370).
// Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y un programa autonomo de calibracion (fixture_mirror.py / fixture_mirror.cpp).
//
//   GetMillisecondTime  0x5617C0  cdecl() -> uint64 en EDX:EAX (llama a RsTimer 0x619410 y pone EDX = 0)
//   DIReleaseMouse      0x746F70  cdecl() -> void (EAX = retorno de Release o 0; nadie lo lee)
//   RemoveUpgradeCB     0x6D3300  cdecl(RpAtomic*, void* data) -> RpAtomic* (callback de RpClumpForAllAtomics; siempre devuelve el atomo)
//   FindUpgradeCB       0x6D3370  cdecl(RpAtomic*, void* data) -> RpAtomic* (nullptr si lo encuentra: data[1] = atomo)
//
// El caso es un vector `v` de 16 palabras cuyo significado depende de la funcion (ver MakeXxx) y los valores iniciales de EAX/ECX/EDX/pila.
// Todos los objetos manejados por las funciones (atomo, modelos, datos, estructuras ps, dispositivos DirectInput FALSOS) viven en una arena propia de 0x280 bytes
// con guardas; el dispositivo DirectInput real y su estructura psGlobal nunca se tocan.
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

namespace SH64Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using i32 = std::int32_t;

constexpr unsigned kCases = 4096;
constexpr unsigned kFns = 4;
constexpr u32 kTimer = 0, kMouse = 1, kRemove = 2, kFind = 3;
constexpr u32 kMaxEvents = 16;
constexpr u32 kVWords = 16;

// Disposicion de la arena (bytes desde su base). Suma a mano: 0x40 guarda + 0x80 atomo + 0x40 + 0x40 modelos + 0x20 datos + 0x40 + 0x40 estructuras ps + 3 x 0x20 dispositivos + 0x40 guarda = 0x280.
constexpr u32 oG0 = 0x000, oAtom = 0x040, oMi1 = 0x0C0, oMi2 = 0x100, oData = 0x140, oPs1 = 0x160, oPs2 = 0x1A0, oDevA = 0x1E0, oDevB = 0x200, oDevC = 0x220, oG1 = 0x240;
constexpr u32 kArenaSize = 0x280;
constexpr u32 kArenaWords = kArenaSize / 4;
static_assert(oAtom == oG0 + 0x40 && oMi1 == oAtom + 0x80 && oMi2 == oMi1 + 0x40 && oData == oMi2 + 0x40 && oPs1 == oData + 0x20 && oPs2 == oPs1 + 0x40 &&
              oDevA == oPs2 + 0x40 && oDevB == oDevA + 0x20 && oDevC == oDevB + 0x20 && oG1 == oDevC + 0x20 && kArenaSize == oG1 + 0x40, "arena");
constexpr u32 kAtomFrame = 4, kAtomClump = 0x3C;   // RpAtomic: +4 marco (RwObject::parent), +0x3C clump
constexpr u32 kMiFlags = 0x12;                     // CBaseModelInfo: word de banderas (bit 8 = VehDummy, bits 10..14 = CarMod)
constexpr u32 kPsMouse = 0x1C;                     // psGlobalType::diMouse
constexpr u64 kLabel = 0x100000000ull;             // marca de "puntero a la arena": kLabel + desplazamiento

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }

struct Spec {
    u32 fn{};
    u32 family{};
    std::array<u32, kVWords> v{};
    u32 stack{}, eax{}, ecx{}, edx{};
};

struct Gen {
    u32 n, k{0};
    explicit Gen(u32 n_) : n(n_) {}
    u32 R() { return Rand(n, ++k); }
    u32 P(u32 m) { return R() % m; }
    bool C(u32 pct) { return R() % 100 < pct; }
};

constexpr u32 kFamilies = 8;

// ---- GetMillisecondTime: v[0] = retorno de RsTimer (el falso)
inline void MakeTimer(Spec& s, Gen& g) {
    switch (s.family) {
    case 0: s.v[0] = 0; break;
    case 1: s.v[0] = 1; break;
    case 2: s.v[0] = 0x7FFFFFFFu; break;
    case 3: s.v[0] = 0x80000000u; break;
    case 4: s.v[0] = 0xFFFFFFFFu; break;
    case 5: s.v[0] = g.P(1u << 20); break;
    case 6: s.v[0] = g.R(); break;
    default: s.v[0] = 3000000u + g.R() % 1000000000u; break;
    }
}

// ---- DIReleaseMouse
// v[0] = dispositivo inicial de ps1.diMouse (0 nulo, 1 A, 2 B, 3 C); v[1] = el de ps2 (1..3: el EXE lo desreferencia si se llega a el)
// v[2 + 4*d + 0..3] (d = 0..2 para A, B, C) = accion de Unacquire, retorno de Unacquire, accion de Release, retorno de Release.
// Acciones (se aplican a la estructura ps ACTUAL = RsGlobal.ps): 0 nada, 1/2/3 ps.diMouse = A/B/C, 4 RsGlobal.ps = ps2, 5 RsGlobal.ps = ps1, 6 ps.diMouse = nulo
// (Unacquire solo usa 0..4: tras Unacquire el EXE desreferencia el dispositivo sin comprobar nulo).
constexpr u32 kMouseV0 = 2;
inline void MakeMouse(Spec& s, Gen& g) {
    const u32 fam = s.family;
    s.v[0] = (fam == 0 || (fam >= 6 && g.C(40))) ? 0u : 1u + g.P(3);
    s.v[1] = 1u + g.P(3);
    for (u32 d = 0; d < 3; ++d) {
        u32* p = &s.v[kMouseV0 + 4 * d];
        p[0] = 0; p[2] = 0;
        p[1] = g.C(50) ? 0u : g.R();
        p[3] = g.C(30) ? 0u : g.R();
        switch (fam) {
        case 2: p[0] = 1 + g.P(3); break;                                         // Unacquire cambia ps.diMouse
        case 3: p[0] = g.C(70) ? 4u : 1u + g.P(3); break;                         // Unacquire cambia RsGlobal.ps (a ps2)
        case 4: p[2] = g.C(60) ? 4u : (g.C(50) ? 5u : 6u); break;                 // Release cambia RsGlobal.ps / anula
        case 5: p[2] = 1 + g.P(3); break;                                         // Release cambia ps.diMouse (el EXE lo sobrescribe a nulo)
        case 6: p[0] = g.C(50) ? 0u : (g.C(50) ? 1u + g.P(3) : 4u); p[2] = g.P(7); break;
        case 7: p[0] = g.C(40) ? 1u + g.P(4) : 0u; p[2] = g.P(7); break;
        default: break;                                                           // 0, 1: sin acciones
        }
    }
}

// ---- RemoveUpgradeCB / FindUpgradeCB
// v[0] = retorno de GetAtomicId (banderas), v[1] = palabra de banderas del modelo 1 (+0x12: bit 8 VehDummy, bits 10..14 CarMod), v[2] = data (Remove: el valor; Find: data[0]),
// v[3] = segundo GetModelInfo (0 nulo, 1 modelo 2, 2 el mismo modelo 1), v[4] = clump (+0x3C del atomo), v[5] = marco (+4 del atomo),
// v[6..8] = retornos de los falsos (RpClumpRemoveAtomic, RpAtomicDestroy, RwFrameDestroy), v[9] = data[1] inicial (Find).
constexpr u32 kUpgradeBit = 0x800;
inline u32 CarModOf(u32 miWord) { return (miWord >> 10) & 0x1Fu; }

inline u32 Ptrish(Gen& g) {   // clump / marco: nulo, pequeno o patron alto (nunca dentro de la arena)
    switch (g.P(4)) {
    case 0: return 0u;
    case 1: return g.P(0x1000);
    default: return 0xA0000000u | (g.R() & 0x0FFFFFFFu);
    }
}

inline void MakeUpgrade(Spec& s, Gen& g, bool find) {
    const u32 fam = s.family;
    const u32 carMod = g.P(32);
    bool veh = g.C(50);
    u32 flags = g.R() & ~kUpgradeBit;                  // sin el bit 0x800
    u32 data = carMod;
    u32 owner = g.P(3);
    bool wantUpgrade = true;
    if (find) {
        switch (fam) {
        case 0: wantUpgrade = false; break;
        case 1: data = carMod ^ (1u << g.P(5)); break;                       // CarMod distinto (un bit)
        case 2: break;                                                      // coincide
        case 3: data = carMod + (32u << g.P(27)); break;                    // coincide en los 5 bits bajos pero no en 32
        case 4: break;
        case 5: flags = g.C(50) ? 0xFFFFFFFFu : kUpgradeBit; break;
        case 6: veh = true; break;                                          // VehDummy: FindUpgradeCB lo ignora
        default: data = g.R(); break;                                       // al azar
        }
    } else {
        switch (fam) {
        case 0: wantUpgrade = false; break;
        case 1: data = carMod ^ (1u << g.P(5)); veh = false; break;         // CarMod distinto
        case 2: veh = true; break;                                          // VehDummy
        case 3: veh = false; owner = 1; break;                              // se quita; segundo modelo distinto
        case 4: veh = false; owner = 0; break;                              // se quita; segundo modelo nulo
        case 5: veh = false; owner = 2; break;                              // se quita; mismo modelo
        case 6: data = carMod | (1u << (5 + g.P(27))); veh = false; break; // bits altos de data (el EXE compara 32 bits)
        default: flags = g.C(50) ? 0xFFFFFFFFu : kUpgradeBit; veh = false; if (g.C(30)) data = carMod ^ 1u; break;
        }
    }
    if (wantUpgrade) flags |= kUpgradeBit;
    if (g.C(20)) flags = (flags & kUpgradeBit) | (flags & 0xFFFF0000u);   // solo bit 0x800 y bits altos (el EXE mira AH)
    const u32 miWord = (g.R() & 0x82FFu) | (carMod << 10) | (veh ? 0x100u : 0u);   // bits 0..7, 9 y 15 aleatorios; 8 y 10..14 fijados
    s.v[0] = flags; s.v[1] = miWord; s.v[2] = data; s.v[3] = owner;
    s.v[4] = Ptrish(g); s.v[5] = Ptrish(g) | 0x40u;   // el marco nunca es nulo: el envoltorio RwFrameDestroy de la DLL lo comprueba con assert
    s.v[6] = g.R(); s.v[7] = g.R(); s.v[8] = g.R();
    s.v[9] = 0x70000000u | (g.R() & 0x0FFFFFFFu);
}

inline Spec MakeSpec(u32 fn, u32 n) {
    Spec s;
    s.fn = fn;
    Gen g(n + fn * 0x20000u);
    s.family = n % kFamilies;
    s.stack = StackSeed(n);
    s.eax = g.R(); s.ecx = g.R(); s.edx = g.R();
    switch (fn) {
    case kTimer: MakeTimer(s, g); break;
    case kMouse: MakeMouse(s, g); break;
    case kRemove: MakeUpgrade(s, g, false); break;
    default: MakeUpgrade(s, g, true); break;
    }
    return s;
}

// Relleno inicial de la arena: palabras con el bit 31 puesto (nunca coinciden con una direccion de la arena); los campos de cada funcion se escriben encima.
inline u32 FillWord(u32 fn, u32 n, u32 i) { return 0xC0000000u | (Rand(n + fn * 0x20000u + 0x777u, 1000u + i) >> 2); }

inline std::string U(u32 v) { return std::to_string(v); }
inline std::string JsonSpec(const Spec& s) {
    std::string o = "{\"fn\":" + U(s.fn) + ",\"fam\":" + U(s.family) + ",\"v\":[";
    for (u32 i = 0; i < kVWords; ++i) o += (i ? "," : "") + U(s.v[i]);
    o += "],\"regs\":[" + U(s.stack) + "," + U(s.eax) + "," + U(s.ecx) + "," + U(s.edx) + "]}";
    return o;
}
}  // namespace SH64Fix
