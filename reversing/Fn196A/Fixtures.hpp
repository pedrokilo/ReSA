#pragma once
// Fixtures del banco Fn196A (lote 196A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 32 direcciones asignadas: 25 reconstruidas.
//   idx  direccion  nombre real                                                                         forma
//    0   0x6D63D0   CVehicle::GetNumContactWheels (ranura 55 de 0x871E80)                                thiscall() `ret`: mov eax,4
//    1   0x6D63E0   CVehicle::VehicleDamage (ranura 56)                                                   thiscall(6 dwords) `ret 18h`: nada
//    2   0x6E2D90   CVehicle::operator delete(void*)                                                     cdecl(p) `ret`: borrado en linea del almacen 0xB74494 (indice = (p - base) / 0xA18 con signo)
//    3   0x6E2DD0   CVehicle::operator delete(void*, int32)                                              cdecl(p, ref) `ret`: copia exacta de la anterior (el segundo argumento no se lee)
//    4   0x6E38A0   CVehicle destructor con borrado (ranura 0)                                           thiscall(flags) `ret 4`: cuerpo 6E2B40; con bit 0 el mismo borrado en linea; devuelve this
//    5   0x5D24B0   CPlayerInfoSaveStructure::Construct (el listado decia CVehicleSaveStructure)         thiscall(info) `ret 4`: copia por campos CPlayerInfo -> estructura de 0x28 bytes
//    6   0x5D2560   CPlayerInfoSaveStructure::Extract                                                    thiscall(info) `ret 4`: inversa
//    7   0x5D2620   CGangWarsSaveStructure::Construct                                                    thiscall() `ret`: estaticos de CGangWars 0x96AB44..0x96ABD0 -> estructura de 0x58 bytes (indices de zona por bucle)
//    8   0x5D2740   CGangWarsSaveStructure::Extract                                                      thiscall() `ret`: inversa
//    9   0x7457C0   Ps::GetCpuExtendedFeatures (codigo muerto)                                           cdecl() `ret`: EDX de CPUID(0x80000001) si la hoja maxima extendida (sin signo) > 0x80000000; si no 0
//   10   0x7457F0   Ps::GetCpuFeaturesAndVendor (codigo muerto)                                          cdecl() `ret`: CPUID 1, 0x80000000, [0x80000001], 0; guarda EBX, EDX, ECX en 0x8D6238 (12 bytes)
//   11   0x745A70   VideoMode::GetNumVideoModes (_psGetNumVideModes)                                     cdecl() `jmp 0x7F2CC0` (RwEngineGetNumVideoModes)
//   12   0x53F910   CPad::SetDrunkInputDelay                                                              thiscall(delay) `ret 4`: [this+0x74] = delay
//   13   0x5404F0   CPad::sub_5404F0                                                                       thiscall() `ret`: solo Mode (+0x10A, WORD) == 1: (+0x12 != 0) && (+0x42 == 0) (WORD); devuelve AL
//   14   0x541D70   CPad::StopPadsShaking                                                                  cdecl() `ret`
//   15   0x541E50   CPad::UpdatePadsThunk                                                                  cdecl() `jmp 0x541DD0` (CPad::UpdatePads)
//   16   0x541E90   CMatrix::Rotate(const CVector*) (el listado decia CPad)                                thiscall(vec*) `ret 4`: llama a 0x59B460 (Rotate por valor) con x, y, z leidos del puntero
//   17   0x541F40   CTimer::UpdateTimeStep (el listado decia CPad)                                        cdecl(float) `ret`: [0xB7CB5C] = paso si es mayor o no ordenado con 1e-5f; si no 0x3727C5AC
//   18   0x747000   WinInput::InputShutdown (el listado decia CPad)                                       cdecl() `ret`: Release de psGlobal.diInterface (+0x18) y lo pone a nulo (psGlobal se relee)
//   19   0x5FF3A0   CEventSeenCop cuerpo del destructor                                                   thiscall() `ret`: [this]=86CB58h; jmp 4AF890
//   20   0x5FF3B0   CEventSeenCop::GetEventType (ranura 1)                                                  thiscall() `ret`: mov eax,48h
//   21   0x5FF3C0   CEventSeenCop::CloneEditable (ranura 16)                                                thiscall() `ret`: CEvent::operator new 4B5620 (0x18); ctor 4AF820(persona de +0x14); [mem]=86CB58h; eax=mem
//   22   0x5FF420   CEventSeenCop destructor con borrado (ranura 0)                                        thiscall(flags) `ret 4`: cuerpo 5FF3A0; operator delete 4B5630 si bit 0; devuelve this
//   23   0x5FF440   CEventHealthLow::Constructor (el listado decia CEventSeenCop)                          thiscall() `ret`: ctor 4AC450; [this]=86CBA0h; eax=this
//   24   0x5FF460   CEventHealthLow cuerpo del destructor (el listado decia CEventSeenCop)                  thiscall() `ret`: [this]=86CBA0h; jmp 4AC480
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F196AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 25;
constexpr unsigned kArenas = 4;

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
    std::array<u32, 64> a{};
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
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kVDmg, kVDel, kVSdd, kPiCon, kPiExt, kGwCon, kGwExt, kCpuExt, kCpuVend, kVid, kPadDrunk, kPadSub, kNop, kThunk, kRotPtr, kTime, kInShut, kBody, kCloneSc, kSdd, kCtor };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kVDmg, kVDel, kVDel, kVSdd, kPiCon, kPiExt, kGwCon, kGwExt, kCpuExt, kCpuVend, kVid, kPadDrunk, kPadSub, kNop, kThunk, kRotPtr, kTime, kInShut,
                                                    kBody, kConst, kCloneSc, kSdd, kCtor, kBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x6D63D0, 0x6D63E0, 0x6E2D90, 0x6E2DD0, 0x6E38A0, 0x5D24B0, 0x5D2560, 0x5D2620, 0x5D2740, 0x7457C0, 0x7457F0, 0x745A70, 0x53F910, 0x5404F0, 0x541D70, 0x541E50,
                                                    0x541E90, 0x541F40, 0x747000, 0x5FF3A0, 0x5FF3B0, 0x5FF3C0, 0x5FF420, 0x5FF440, 0x5FF460};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0x871E80, 0x871E80, 0, 0, 0x871E80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x86CB58, 0x86CB58, 0x86CB58, 0x86CB58, 0x86CBA0, 0x86CBA0};
// Bytes que cada funcion toca a partir de su puntero `this` (o de la entrada de arena A)
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 4, 0x28, 0x28, 0x58, 0x58, 4, 4, 4, 0x10C, 0x10C, 4, 4, 0x48, 4, 4, 0x14, 4, 0x18, 0x18, 0x14, 0x14};
// Memoria global del juego tocada
inline constexpr u32 kVehPool = 0xB74494, kGwBase = 0x96AB44, kGwBytes = 0x90, kZoneCount1 = 0xBA1DE8, kZoneCount2 = 0xBA3794, kZoneBase1 = 0xBA1DF0, kZoneBase2 = 0xBA3798, kCpuVendor = 0x8D6238,
                        kFlag = 0xA9B0B0, kTimeStep = 0xB7CB5C, kPsGlobal = 0xC17054;
inline constexpr u32 kVehSize = 0xA18;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[60..63] = semillas de las arenas A, B, C y D
//   kConst, kNop, kThunk, kCpuExt, kCpuVend: nada mas (kCpuVend: la region 0x8D6238 se rellena con el patron de a[2] y la semilla a[63])
//   kVDmg: a[4] = float (bits), a[5] = componente, a[6] = causante, a[7] = coordenadas, a[8] = direccion, a[9] = arma (todos crudos; no se leen)
//   kVDel, kVSdd: almacen de vehiculos (struct en B): a[4] = desplazamiento del struct en B, a[5] = indice con signo del hueco, a[6] = base del almacen (cruda), a[7] = desplazamiento en D del byte de estado,
//         a[8] = cursor (+0xC), a[9] = capacidad (+8, no se lee), a[10] = byte de estado previo, a[11] = segundo argumento (crudo; kVDel del 0x6E2DD0) o banderas (kVSdd, dword de pila)
//         El puntero del vehiculo es base + indice * 0xA18 (no se desreferencia; dentro del almacen: 0 <= indice < a[9]); el puntero al mapa es (D + a[7]) - indice, de modo que mapa + indice cae en D. Bit 7 de a[10] a cero.
//   kPiCon: a[4] = desplazamiento del CPlayerInfo en B, a[5] = WORD de +0x144, a[6] = WORD de +0x130, a[7] = WORD de +0x156 (el resto es el patron de B)
//   kPiExt: a[4] = desplazamiento del CPlayerInfo en B, a[5] = dword de +0x10 de la estructura (solo el BYTE bajo se lee), a[6] = WORD de +4, a[7] = WORD de +0x26
//   kGwCon: a[6] = contador WORD de zonas (0xBA1DE8), a[9] = contador WORD de zonas de navegacion (0xBA3794), a[10] = puntero a la informacion de zona (0x96AB8C), a[11] = puntero a la zona de navegacion
//         (0x96AB88); el resto de la region 0x96AB44..0x96ABD4 es el patron de a[2] con la semilla a[63]
//   kGwExt: a[10] = indice de informacion de zona (+0xC de la estructura), a[11] = indice de zona de navegacion (+0x10); el resto de la estructura y de la region es el patron
//   kVid: a[4] = lo que devuelve el falso de RwEngineGetNumVideoModes (0x7F2CC0)
//   kPadDrunk: a[4] = el retardo (dword de pila)
//   kPadSub: a[4] = Mode (WORD, +0x10A), a[5] = WORD de +0x12, a[6] = WORD de +0x42
//   kRotPtr: a[4..6] = x, y, z (bits), a[7] = desplazamiento del vector en B, a[8] = EAX del falso de 0x59B460
//   kTime: a[4] = paso (bits), a[5] = dword previo de 0xB7CB5C
//   kInShut: a[4] = modo (0 interfaz nula, 1 interfaz, 2 interfaz cuyo Release cambia psGlobal al segundo struct), a[5] = desplazamiento del struct en B, a[6] = del objeto COM en C, a[7] = del segundo struct en D,
//         a[8] = EAX del falso de Release, a[9] = dword de +0x18 del segundo struct
//   kBody, kCtor: a[4] = EAX del falso (constructor base / cuerpo de la clase base), a[5] = dword que el falso deja en [this]
//   kCloneSc: a[4] = memoria (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = EAX del falso del constructor, a[7] = dword que el falso deja en [mem], a[8] = persona (+0x14 de this, cruda)
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[60] = c.seed * 13u + 50u; c.a[61] = c.a[60] + 1u; c.a[62] = c.a[60] + 2u; c.a[63] = c.a[60] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kConst: case kNop: case kThunk: case kCpuExt: case kCpuVend:
        break;
    case kVDmg:
        a[4] = FloatBits(g); a[5] = g.Pk({0u, 1u, 2u, 3u, 7u, 0xFFFFFFFFu, g.W(), g.R(16)});
        for (u32 i = 6; i < 10; ++i) a[i] = Raw(g);
        break;
    case kVDel: case kVSdd: {
        // Dominio del juego: el hueco pertenece al almacen (0 <= indice < capacidad), esta en uso (bit 7 del mapa a cero) y base + capacidad * 0xA18 no desborda 32 bits. Fuera de eso (puntero ajeno,
        // hueco ya libre, indice negativo) el CPool::Delete de la DLL dispara asserts de depuracion que el EXE no tiene (cada uno vuelca la pila al log: inviable en un banco de decenas de miles de casos).
        a[4] = AuxOffset(g, 0x10);
        const u32 mode = g.R(7);
        i32 idx;
        switch (mode) {
        case 0: idx = 0; break;
        case 1: idx = 1 + i32(g.R(300)); break;
        case 2: idx = i32(g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 64u, 100u, 109u, 127u, 128u, 300u})); break;
        case 3: idx = i32(g.R(512)); break;
        case 4: idx = i32(g.R(800000)); break;
        case 5: idx = 109; break;
        default: idx = i32(g.R(110)); break;
        }
        a[5] = u32(idx);
        a[9] = u32(idx) + 1u + g.Pk({0u, 0u, 0u, 1u, 5u, g.R(200), g.R(2000)});   // capacidad
        a[6] = g.Pk({0u, 0x7FFFFFFFu, 0xB74000u + g.R(0x1000), 0x00400000u + g.R(0x800000), g.R(0x7FFFFFFFu), g.R(0x7FFFFFFFu), g.R(0x70000000u), 0x10000000u, 0x7FFFF000u});
        // el puntero del vehiculo no debe caer en la pila del hilo (el banco lo etiqueta como "pila del llamador" y el oraculo no la conoce): se aparta del rango 0xC00000..0x1200000
        for (int k = 0; k < 4; ++k) {
            const u32 ptr = a[6] + u32(idx) * kVehSize;
            if (ptr >= 0x00C00000u && ptr < 0x01200000u) a[6] += 0x01400000u; else break;
        }
        a[7] = g.R(u32(kWin));
        a[8] = g.Pk({u32(idx) - 1u, u32(idx), u32(idx) + 1u, u32(idx) + 2u, 0u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 0x80000001u, g.W(), g.R(400), g.R(400), 64u, 128u, u32(idx) + g.R(5) - 2u, u32(idx) + 1u, a[9]});
        a[10] = g.Pk({0u, 0x7Fu, 0x01u, 0x55u, g.R(128), g.R(128)});   // bit 7 a cero: hueco en uso
        a[11] = kKind[c.fn] == kVSdd ? FlagsDword(g) : Raw(g);
        break;
    }
    case kPiCon:
        a[4] = AuxOffset(g, 0x190);
        a[5] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x1FFu, 0xFFFFu, 0x8000u, 0x7FFFu, g.R(65536), g.R(65536), g.R(256)});
        a[6] = g.Pk({0u, 1u, 0xFFFFu, 0x8000u, 0x100u, 0xFFu, g.R(65536), g.R(65536)});
        a[7] = g.Pk({0u, 1u, 0xFFFFu, 0x8000u, 0x100u, 0xFFu, g.R(65536), g.R(65536)});
        break;
    case kPiExt:
        a[4] = AuxOffset(g, 0x190);
        a[5] = g.Pk({0u, 1u, 0xFFu, 0x100u, 0x1FFu, 0xFFFFFFFFu, 0x80000000u, 0x7Fu, 0x80u, g.W(), g.W(), g.R(256)});
        a[6] = g.Pk({0u, 1u, 0xFFFFu, 0x8000u, 0x100u, 0xFFu, g.R(65536), g.R(65536)});
        a[7] = g.Pk({0u, 1u, 0xFFFFu, 0x8000u, 0x100u, 0xFFu, g.R(65536), g.R(65536)});
        break;
    case kGwCon: {
        const u32 cnt1 = g.Pk({0u, 1u, 2u, 5u, 40u, 100u, 0x7FFFu, 0x8000u, 0xFFFFu, g.R(65536), g.R(300), 3u, 255u});
        const u32 cnt2 = g.Pk({0u, 1u, 2u, 5u, 40u, 100u, 0x7FFFu, 0x8000u, 0xFFFFu, g.R(65536), g.R(300), 3u, 255u});
        a[6] = cnt1; a[9] = cnt2;
        // 0 nulo, 1 valido (en el rango), 1, 1, 2 justo pasado el contador, 3 dentro pero desalineado, 4 crudo, 5 base, 6 ultimo valido
        const auto pick = [&](u32 base, u32 step, u32 cnt) {
            const u32 mode = g.R(8);
            switch (mode) {
            case 0: return 0u;
            case 1: case 2: case 3: return cnt ? base + step * g.R(cnt) : base;
            case 4: return base + step * cnt;
            case 5: return base + step * g.R(cnt + 2) + 1 + g.R(step - 1);
            case 6: return g.P(50) ? base : (cnt ? base + step * (cnt - 1) : base);
            default: return Raw(g);
            }
        };
        a[10] = pick(kZoneBase1, 0x11u, cnt1);
        a[11] = pick(kZoneBase2, 0x20u, cnt2);
        break;
    }
    case kGwExt:
        a[10] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, 1u, 2u, 7u, 100u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu, g.W(), g.R(300), g.R(65536)});
        a[11] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, 1u, 2u, 7u, 100u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFEu, g.W(), g.R(300), g.R(65536)});
        break;
    case kVid:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 5u, 0x80000000u, g.W(), g.R(40)});
        break;
    case kPadDrunk:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 100u, g.W(), g.R(1000)});
        break;
    case kPadSub:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u, 2u, 3u, 4u, 5u, 0x8000u, 0x8001u, 0xFFFFu, 0x101u, 0x100u, g.R(65536)});
        a[5] = g.Pk({0u, 0u, 1u, 0x100u, 0x8000u, 0xFFFFu, 0xFFu, 0x80u, g.R(65536)});
        a[6] = g.Pk({0u, 0u, 0u, 1u, 0x100u, 0x8000u, 0xFFFFu, 0xFFu, g.R(65536)});
        break;
    case kRotPtr:
        a[4] = FloatBits(g); a[5] = FloatBits(g); a[6] = FloatBits(g);
        a[7] = AuxOffset(g, 12);
        a[8] = Raw(g);
        break;
    case kTime: {
        const u32 mode = g.R(10);
        u32 v = FloatBits(g);
        if (mode == 0) v = 0x3727C5ACu + g.R(5) - 2u;
        else if (mode == 1) v = 0x3727C5ACu | (g.P(50) ? 0x80000000u : 0u);
        else if (mode == 2) v = 0x3727C5ACu + g.R(65536) - 32768u;
        else if (mode == 3) v = (g.W() & 0x00FFFFFFu) | (g.P(50) ? 0x80000000u : 0u);   // denormales y pequenos
        else if (mode == 4) v = 0x3C23D70Au + g.R(0x400000);                            // pasos plausibles (0.01 en adelante)
        a[4] = v;
        a[5] = g.Pk({0u, 0xFFFFFFFFu, 0x3727C5ACu, g.W(), 0x3F800000u});
        break;
    }
    case kInShut:
        a[4] = g.Pk({0u, 1u, 1u, 2u, 2u});
        a[5] = AuxOffset(g, 0x20); a[6] = AuxOffset(g, 8); a[7] = AuxOffset(g, 0x20);
        a[8] = Raw(g); a[9] = g.Pk({0u, 1u, g.W() | 1u, 0xDEADBEEFu});
        break;
    case kBody: case kCtor:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kCloneSc:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u}); a[5] = AuxOffset(g, 0x18);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[7] = Raw(g);
        a[8] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
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
