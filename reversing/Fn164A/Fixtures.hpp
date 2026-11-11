#pragma once
// Fixtures del banco Fn164A (lote 164A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 25 funciones del EXE sin hook al empezar el lote (se comprobo el codigo, docs/hooks.csv y git worktree list). Las clases del listado salen del desensamblado, de las vtables leidas del PE y de los llamadores:
// 22 son de CBike (vtable 0x871360), 0x622680 es el cuerpo del destructor de CTaskSimpleStealthKill (vtable 0x86DA74) y 0x6298E0 / 0x629900 son destructores con borrado de CTaskComplexKillPedFromBoat
// (vtable 0x86DA98, cuerpo 0x622830) y CTaskComplexSeekCoverUntilTargetDead (cuerpo 0x622C20), no de CTaskSimpleStealthKill.
//   idx  direccion  nombre real                                      ranura  forma
//    0   0x6B5820   CBike::OpenDoor                                    27   thiscall(5 args) `ret 14h`: nada
//    1   0x6B5830   CBike::GetDooorAngleOpenRatioU32                   29   thiscall(1 arg) `ret 4`: fld [858B50] (0.0f en ST0)
//    2   0x6B5840   CBike::IsOpenTopCar                                39   thiscall() `ret`: xor al,al
//    3   0x6B5850   CBike::BlowUpCarCutSceneNoExtras                   42   thiscall(4 args) `ret 10h`: nada
//    4   0x6B5860   CBike::AddMovingCollisionSpeed                     49   thiscall(&salida, &punto) `ret 8`: salida = {0,0,0}; eax = salida
//    5   0x6B5880   CBike::SetTowLink                                  61   thiscall(2 args) `ret 8`: xor al,al
//    6   0x6B5890   CBike::BreakTowLink                                62   thiscall() `ret`: xor al,al
//    7   0x6B58A0   CBike::GetNumContactWheels                         55   thiscall() `ret`: movzx eax, byte [ecx+804h]
//    8   0x6B58B0   CBike::GetHeightAboveRoad                          53   thiscall() `ret`: fld dword [ecx+790h]
//    9   0x6B58C0   CBike::GetRideAnimData                             47   thiscall() `ret`: lea eax,[ecx+640h]
//   10   0x6B58E0   CBike::IsDoorReady                                 32   thiscall(1 arg) `ret 4`: mov al,1
//   11   0x6B58F0   CBike::IsDoorFullyOpen                             34   `ret 4`: xor al,al
//   12   0x6B5900   CBike::IsDoorClosed                                36   `ret 4`: xor al,al
//   13   0x6B5910   CBike::IsDoorMissing                               38   `ret 4`: mov al,1
//   14   0x6B5920   CBike::IsDoorReadyU32                              31   `ret 4`: mov al,1
//   15   0x6B5930   CBike::IsDoorFullyOpenU32                          33   `ret 4`: xor al,al
//   16   0x6B5940   CBike::IsDoorClosedU32                             35   `ret 4`: xor al,al
//   17   0x6B5950   CBike::IsDoorMissingU32                            37   `ret 4`: mov al,1
//   18   0x6B59E0   CBike::IsComponentPresent                          26   thiscall(indice) `ret 4`: al = [ecx+indice*4+5A0h] != 0 (sin comprobar el indice)
//   19   0x6B7270   CBike::IsRoomForPedToLeaveCar                      45   thiscall(2 args) `ret 8`: mov al,1
//   20   0x6B8940   CBike::FindWheelWidth                              63   thiscall(1 arg) `ret 4`: fld [858FCC] (0.15f)
//   21   0x6B8950   CBike destructor con borrado (ranura 0)            0    thiscall(flags) `ret 4`: cuerpo 6B57A0 (con hook), CVehicle::operator delete 6E2D90 (sin hook)
//   22   0x622680   CTaskSimpleStealthKill cuerpo del destructor        -    thiscall() marco SEH: [esi]=86DA74h; CleanUpOldReference(+0Ch); SetDeleteCallback(+18h); ~CTask 61A3A0
//   23   0x6298E0   CTaskComplexKillPedFromBoat destructor con borrado  0    thiscall(flags) `ret 4`: cuerpo 622830 (con hook), CTask::operator delete 61A5B0
//   24   0x629900   CTaskComplexSeekCoverUntilTargetDead dtor con borrado 0  thiscall(flags) `ret 4`: cuerpo 622C20 (con hook), CTask::operator delete 61A5B0
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

namespace F164AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;

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

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Un float (bits) "interesante": normales, ceros con signo, denormales, infinitos, NaN silenciosos y senalizantes
inline u32 FloatBits(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W() & 0x807FFFFFu, w3 = (g.W() & 0x807FFFFFu) | 0x7F800000u, w4 = g.W() | 0x7F800000u, w5 = (g.W() & 0x803FFFFFu) | 0x7F800001u;
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3E19999Au, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFFBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x80000001u,
                 0x7F7FFFFFu, w1, w1, w2, w2, w3, w4, w5});
}

// Clases de funcion (forma)
enum Kind : u32 { kVoid, kAL, kFloat0, kFloatC, kFloatField, kAddSpeed, kWheels, kRideAnim, kComp, kSdd, kStealthBody };
inline constexpr std::array<Kind, kFunctions> kKind{
    kVoid, kFloat0, kAL, kVoid, kAddSpeed, kAL, kAL, kWheels, kFloatField, kRideAnim, kAL, kAL, kAL, kAL, kAL, kAL, kAL, kAL, kComp, kAL, kFloatC, kSdd, kStealthBody, kSdd, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x6B5820, 0x6B5830, 0x6B5840, 0x6B5850, 0x6B5860, 0x6B5880, 0x6B5890, 0x6B58A0, 0x6B58B0, 0x6B58C0, 0x6B58E0, 0x6B58F0, 0x6B5900, 0x6B5910, 0x6B5920,
                                                    0x6B5930, 0x6B5940, 0x6B5950, 0x6B59E0, 0x6B7270, 0x6B8940, 0x6B8950, 0x622680, 0x6298E0, 0x629900};
// Argumentos de pila de cada funcion (todas `ret n` del callee)
inline constexpr std::array<u32, kFunctions> kNArgs{5, 1, 0, 4, 2, 2, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1};
// Valor constante de AL (kAL)
inline constexpr std::array<u32, kFunctions> kAlValue{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0};
// Cuerpo que llama el destructor con borrado y liberacion
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x6B57A0, 0x61A3A0, 0x622830, 0x622C20};
inline constexpr std::array<u32, kFunctions> kDelete{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x6E2D90, 0, 0x61A5B0, 0x61A5B0};
// vtable de la clase (informativa: dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360,
                                                     0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x871360, 0x86DA74, 0x86DA98, 0x86DA98};
// Bytes que cada funcion toca a partir de su puntero `this` (CBike mide 0x814; CTaskSimpleStealthKill 0x20)
inline constexpr std::array<u32, kFunctions> kSpan{0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 0x814, 4, 0x20, 4, 4};
inline constexpr u32 kEvDefaultCb = 0x4D1390;                // CDefaultAnimCallback::DefaultAnimCB del EXE (el falso de SetDeleteCallback normaliza la direccion de la DLL a esta)
inline constexpr u32 kOffNodes = 0x5A0, kOffRide = 0x640, kOffHeight = 0x790, kOffWheels = 0x804;
inline constexpr u32 kOffStealthTarget = 0xC, kOffStealthAnim = 0x18;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global; a[16..23] = datos extra
//   kVoid / kAL / kFloat0 / kFloatC / kRideAnim: a[3] = dword en [this] (no se lee), a[4..8] = argumentos de pila crudos
//   kFloatField: a[3] = bits del float que se escribe en this+790h (sin argumentos de pila)
//   kWheels: a[3] = byte en this+804h
//   kAddSpeed: a[3] = desplazamiento de la salida (arena B o C, span 12); a[4] = modo de la salida (0 en B, 1 = this, 2 = this+10h, 3 en C); a[5] = segundo argumento crudo; a[6] = 0 crudo / 1 el mismo puntero que la salida
//   kComp: a[3] = dword que se escribe en la casilla; a[4] = indice de componente (con signo; la casilla cae dentro de la ventana de la arena A); a[5] = dword en [this]
//   kSdd: a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kStealthBody: a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite; a[6] = modo del objetivo (+0Ch: 0 nulo, 1 en B a[7], 2 crudo a[8]); a[9] = modo de la animacion (+18h: 0 nula, 1 en B a[10], 2 cruda a[11])
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
    case kVoid: case kAL: case kFloat0: case kFloatC: case kRideAnim:
        c.a[3] = Raw(g);
        for (unsigned i = 4; i <= 8; ++i) c.a[i] = Raw(g);
        break;
    case kFloatField:
        c.a[3] = FloatBits(g);
        c.a[4] = Raw(g);
        break;
    case kWheels:
        c.a[3] = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFu, 0x80u, 0x7Fu, g.R(256), g.R(256)});
        break;
    case kAddSpeed:
        c.a[3] = Offset(g, g.R(4), 12);
        c.a[4] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u, 3u});
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 0u, 1u});
        break;
    case kComp: {
        const i32 base = i32(c.a[0] + kOffNodes);
        c.a[3] = g.Pk({0u, 0u, 0u, 1u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.W(), 0x00400000u + g.R(0x800000)});
        if (g.P(50)) {
            c.a[4] = g.R(10);                                   // los componentes reales (0..9)
        } else if (g.P(15)) {
            c.a[4] = u32(i32(g.R(0x40)) - 8);                   // alrededor del arreglo, con indices negativos
        } else {
            const i32 t = 4 + i32(g.R(u32(kWin) - 12));         // cualquier casilla dentro de la ventana
            c.a[4] = u32((t - base) >> 2);
        }
        c.a[5] = prev();
        break;
    }
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kStealthBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = g.P(70) ? (Raw(g) | 1u) : Raw(g);
        c.a[9] = g.Pk({0u, 1u, 1u, 2u});
        c.a[10] = Offset(g, g.R(4), 0x20);
        c.a[11] = g.P(70) ? (Raw(g) | 1u) : Raw(g);
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
