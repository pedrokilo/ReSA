#pragma once
// Fixtures del banco Fn186A (lote 186A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado de entrada eran en su mayoria erroneas: las reales salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py).
// 18 asignadas con hook (13 S y 5 V); las 3 saltadas (0x5DBE40, 0x5DD1E0, 0x5DD220: registros EBX/ESI/EAX de entrada) no se prueban.
//   idx  direccion  nombre real                                                                      forma
//    0   0x49ABD0   GetChangingStatIndex (CShopping, funcion suelta)                                 cdecl(cadena) `ret`: seis `repe cmps` con el NUL; "-" -> -1, fat 0 ... calories 5, otro -1
//    1   0x49ACD0   CShopping::RemovePriceModifier(clave)                                            cdecl(clave) `ret`: busca en 0xA98650 (8 bytes), --contador (0xA9A7D0) y el ultimo ocupa el hueco
//    2   0x49BDD0   CShopping::AddPriceModifier(clave, precio)                                       cdecl(2 dwords) `ret`: cambia o anade en [contador]; actualiza el precio cargado (0xA986F0, 0x18 bytes, hasta 0xA9A7CC)
//    3   0x49C3B0   CPool<CStuntJump>::GetAt                                                         thiscall(indice) `ret 4`: BYTE de estado con signo negativo -> 0; si no, almacenamiento + indice * 0x44
//    4   0x49C440   CPool<CStuntJump>::Flush (segunda copia identica de 0x49C950)                     thiscall() `ret`: con tamano > 0 y BYTE +10h no nulo: delete[] x2 (0x8213AE); campos a cero
//    5   0x5A2F80   ProcSurfaceInfo_c::IsPtInTriangle2D                                              x87 thiscall (ECX sin usar) 15 dwords `ret 3Ch`: tres bordes <= 0 y z = (...) / nz
//    6   0x5A3070   ProcObjectListItem::Constructor                                                  thiscall() `ret`: ListItem_c 4A8DB0 (+0 y +4 a cero); eax = this
//    7   0x5A3090   ProcObjectMan_c::~ProcObjectMan_c                                                thiscall() `ret` (marco SEH): destructores de vectores sin efecto; no escribe memoria
//    8   0x5A3E20   ProcObjectMan_c::ProcObjectMan_c                                                 thiscall() `ret` (marco SEH): vectores de 128 ProcSurfaceInfo_c (+8) y 512 registros (+0x2408) y la lista (+0x4C08)
//    9   0x635CF0   CTaskSimpleStandStill::Clone                                                     thiscall() `ret` V: new(20h) 61A5A0; CTask 61A390 y campos en linea (+8, +0Ch.., bytes +18h/+19h, +1Ch)
//   10   0x635DA0   CTaskSimpleDie::Clone                                                            thiscall() `ret` V: new(28h) 61A5A0; con jerarquia (+10h) ctor 62FAF0, si no 62FA00
//   11   0x635E60   CTaskSimpleDrown::Clone                                                          thiscall() `ret` V: new(2Ch) 61A5A0; CTask 61A390 y campos en linea
//   12   0x63B960   CTaskComplexLeaveCar::GetTaskType                                                thiscall() `ret` V: mov eax,2C0h
//   13   0x63BA00   CTaskComplexLeaveCar (RegisterPedLeaving)                                         thiscall(ped) `ret 4`: ComputeDoorFlag 64E550, SetGettingOutFlags 6D3020, IsPlayer 5DF8F0 x2
//   14   0x63BAE0   CTaskComplexLeaveCar (CreateLineUpUtility)                                        thiscall(ped sin usar) `ret 4`: new(1Ch) 82119A; ctor 64FBB0(&vector0, 0, 0, puerta) -> +1Ch
//   15   0x63D9E0   CTaskComplexLeaveCar::Clone                                                      thiscall() `ret` V: new(34h) 61A5A0; ctor 63B8C0(vehiculo, puerta, retraso, bytes ampliados)
//   16   0x6410E0   CTaskComplexLeaveCar destructor con borrado                                       thiscall(flags) `ret 4`: cuerpo 63B970; delete 61A5B0
//   17   0x642250   CTaskComplexLeaveBoat destructor con borrado                                      thiscall(flags) `ret 4`: cuerpo 63BBD0; delete 61A5B0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40; la arena A de las funciones 7 y 8 es de 0x5000 bytes) y los colaboradores falseados (el mismo falso en la direccion
// del EXE y en el cuerpo de la DLL). Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4
// (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F186AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 18;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;       // ventana de cada arena
constexpr std::size_t kWinBig = 0x5000;    // ventana de la arena A de las funciones 7 y 8 (ProcObjectMan_c ocupa 0x4C14 bytes)
constexpr std::size_t Arena(std::size_t win) { return kGuard + win + kGuard; }
constexpr u32 WinA(u32 fn) { return fn == 7 || fn == 8 ? u32(kWinBig) : u32(kWin); }

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
    std::string End() const { return s + "}"; }
};

// ------------------------------------------------------------------------------------------------ el caso
constexpr unsigned kSeed = 60;          // a[60..63] = semillas de las arenas A, B, C y D (y de la memoria global)
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 64> a{};
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

// Desplazamiento de un objeto de `span` bytes en una ventana de `win` bytes segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span, u32 win = u32(kWin)) {
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxIn(Gen& g, u32 lo, u32 size, u32 span) { return lo + 4 * g.R((size - span) / 4 + 1); }
inline u32 AuxOffset(Gen& g, u32 span) { return AuxIn(g, 0, u32(kWin), span); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un dword distinto de cero (para valores que el EXE prueba contra cero): 0x100 y 0x80000000 tienen el BYTE bajo a cero
inline u32 NonZero(Gen& g) {
    const u32 v = Raw(g);
    return v ? v : g.Pk({1u, 0x100u, 0x80000000u, 0xFFFFFF00u});
}
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un dword cuyo BYTE bajo es el dato (la mitad alta es basura: el EXE solo lee el BYTE)
inline u32 ByteDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W();
    return g.Pk({0u, 1u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80000000u, 0xFFFFFFFFu, w1, w2, g.R(256), g.R(256)});
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
inline bool IsInfBits(u32 b) { return (b & 0x7FFFFFFFu) == 0x7F800000u; }
inline u32 F32(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
// double -> float (como bits) con saturacion a +-FLT_MAX en lugar de la conversion indefinida
inline u32 D2F(double d) {
    if (d > 3.4028234663852886e38) d = 3.4028234663852886e38;
    if (d < -3.4028234663852886e38) d = -3.4028234663852886e38;
    return F32(float(d));
}

// Clases de funcion (forma)
enum Kind : u32 { kStat, kRemMod, kAddMod, kGetAt, kFlush, kTri, kLiCtor, kManDtor, kManCtor, kStandClone, kDieClone, kDrownClone, kConst, kRegLeave, kLineUp, kLcClone, kSdd };
inline constexpr std::array<Kind, kFunctions> kKind{kStat, kRemMod, kAddMod, kGetAt, kFlush, kTri, kLiCtor, kManDtor, kManCtor, kStandClone, kDieClone, kDrownClone, kConst, kRegLeave, kLineUp, kLcClone, kSdd, kSdd};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x49ABD0, 0x49ACD0, 0x49BDD0, 0x49C3B0, 0x49C440, 0x5A2F80, 0x5A3070, 0x5A3090, 0x5A3E20, 0x635CF0, 0x635DA0, 0x635E60, 0x63B960, 0x63BA00, 0x63BAE0, 0x63D9E0, 0x6410E0, 0x642250};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0, 0, 0, 0, 0, 0, 0, 0x86DD2C, 0x86DDBC, 0x86DE30, 0x86E828, 0x86E828, 0x86E828, 0x86E828, 0x86E828, 0x86E854};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 0x14, 0x14, 4, 0x14, 0x4C14, 0x4C14, 0x20, 0x20, 4, 4, 0x24, 0x20, 0x1C, 4, 4};

// Memoria global del juego que tocan las funciones 1 y 2 (tabla de modificadores de precio 0xA98650, tabla de precios 0xA986F0, contadores 0xA9A7CC y 0xA9A7D0)
inline constexpr u32 kGlobBase = 0xA98650, kGlobSize = 0x2184;
inline constexpr u32 kModTab = 0xA98650, kPriceTab = 0xA986F0, kNumPrices = 0xA9A7CC, kNumMods = 0xA9A7D0;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3...] = especifico, a[60..63] = semillas de las arenas A, B, C y D (la memoria global usa la de B con el relleno de a[2])
//   kStat:     a[3] = desplazamiento de la cadena en D, a[4..7] = los 16 bytes que se escriben en D (cadena y basura posterior)
//   kRemMod:   a[3] = contador (0xA9A7D0), a[4] = clave, a[5] = modo (0 ausente, 1 presente en a[6], 2 ademas repetida en a[7] > a[6]), a[6], a[7] = posiciones, a[8] = contador de precios (0xA9A7CC)
//   kAddMod:   a[3] = contador, a[4] = clave, a[5] = precio, a[6] = modo (0 ausente, 1 presente en a[7], 2 ademas repetida en a[8]), a[7], a[8] = posiciones, a[9] = contador de precios,
//              a[10] = modo de precios (0 sin la clave, 1 con la clave en a[11], 2 ademas en a[12] > a[11]), a[11], a[12] = posiciones
//   kGetAt:    a[3] = almacenamiento (crudo), a[4] = indice (pila), a[5] = BYTE de estado, a[6] = posicion del estado en D (states = D + a[6] - indice), a[7] = tamano, a[8] = ultimo libre, a[9] = dword +0x10
//   kFlush:    a[3] = tamano, a[4] = dword en +0x10 (solo cuenta el byte bajo), a[5] = ultimo libre
//   kTri:      a[3..16] = x, y, v1 (3), v2 (3), v3 (3), normal (3) como bits de float; a[17] = desplazamiento en D del destino de z
//   kLiCtor, kManDtor, kManCtor, kConst: nada mas
//   kStandClone: a[3] = memoria (0 nula, 1 en B), a[4] = su desplazamiento, a[5] = tiempo (+8), a[6] = repeticion (+18h, dword: solo cuenta el BYTE bajo), a[7] = reposo (+19h, idem), a[8] = mezcla (+1Ch)
//   kDieClone: a[3] = memoria, a[4] = desplazamiento, a[5] = jerarquia (+10h), a[6] = grupo (+8), a[7] = animacion (+0Ch), a[8] = banderas (+14h), a[9] = mezcla (+18h), a[10] = velocidad (+1Ch),
//              a[11] = EAX del falso del constructor, a[12] = dword que deja en [mem]
//   kDrownClone: a[3] = memoria, a[4] = desplazamiento
//   kRegLeave: a[3] = desplazamiento del vehiculo en B, a[4] = puerta (+10h), a[5] = sensata (+18h, dword: solo cuenta el BYTE bajo), a[6] = modo del conductor (0 nulo, 1 el peaton, 2 otro en C, 3 crudo),
//              a[7] = desplazamiento del peaton en C, a[8] = desplazamiento del otro conductor en C, a[9] = conductor crudo, a[10] = EAX de la 1.a IsPlayer, a[11] = EAX de la 2.a IsPlayer, a[12] = EAX de ComputeDoorFlag,
//              a[13] = BYTE +36h del vehiculo, a[14] = BYTE +3BAh, a[15] = BYTE +3D0h, a[16] = BYTE +485h, a[17] = peaton: 0 en C, 1 crudo (a[18]), a[18] = peaton crudo, a[19] = dword +20h de la tarea
//   kLineUp:   a[3] = puerta (+10h), a[4] = memoria (0 nula, 1 en C), a[5] = su desplazamiento en C, a[6] = EAX del falso del constructor, a[7] = dword que deja en [mem], a[8] = peaton (pila, sin usar)
//   kLcClone:  a[3] = memoria, a[4] = desplazamiento en B, a[5] = vehiculo (+0Ch), a[6] = puerta (+10h), a[7] = retraso (+14h), a[8] = sensata (+18h, dword: BYTE), a[9] = forzar (+19h, idem), a[10] = EAX del falso, a[11] = dword que deja en [mem]
//   kSdd:      a[3] = vtable previa, a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
constexpr u32 kStatNames = 7;
// Una cadena de 16 bytes para GetChangingStatIndex: nombres validos, variantes cercanas y basura
inline void MakeStat(Gen& g, std::array<u32, 4>& words) {
    static const char* const names[kStatNames] = {"-", "fat", "respect", "sexy", "health", "stamina", "calories"};
    std::array<u8, 16> b{};
    for (auto& x : b) x = u8(g.W());
    const u32 mode = g.R(11);
    std::string name = names[g.R(kStatNames)];
    const std::size_t len = name.size();
    std::string s;
    switch (mode) {
    case 0: case 1: s = name; break;                                                            // exacta
    case 2: s = len > 1 ? name.substr(0, len - 1) : std::string(); break;                         // sin la ultima letra (prefijo)
    case 3: s = name + char('a' + g.R(26)); break;                                                // una letra de mas
    case 4: s = name; s[g.R(u32(len))] = char(1 + g.R(255)); break;                               // una letra cambiada (puede dar el mismo byte)
    case 5: s = name; s[0] = char(s[0] ^ 0x20); break;                                            // primera letra en mayuscula
    case 6: s = name; for (u32 i = 0; i < 6; ++i) s += char(1 + g.R(255)); break;                 // sin NUL hasta mas alla de la constante
    case 7: s = std::string(); break;                                                             // vacia
    case 8: for (u32 i = 0; i < 15; ++i) s += char(1 + g.R(255)); break;                          // basura
    case 9: s = names[g.R(kStatNames)]; break;                                                    // otro nombre
    default: s = name + ' '; break;                                                               // nombre y espacio
    }
    for (std::size_t i = 0; i < s.size() && i < 15; ++i) b[i] = u8(s[i]);
    if (s.size() < 16) b[s.size()] = 0;
    for (u32 i = 0; i < 4; ++i) words[i] = u32(b[4 * i]) | (u32(b[4 * i + 1]) << 8) | (u32(b[4 * i + 2]) << 16) | (u32(b[4 * i + 3]) << 24);
}
inline u32 ModCount(Gen& g) {
    const i32 v = i32(g.Pk({0u, 0u, 1u, 1u, 2u, 3u, 4u, 5u, 10u, 19u, 20u, 21u, g.R(21), g.R(21), g.R(21), g.R(1001), 1000u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFBu, 0xFFFFFC18u}));
    return u32(std::clamp(v, -1000, 1000));
}
inline u32 PriceCount(Gen& g) {
    const i32 v = i32(g.Pk({0u, 0u, 1u, 2u, 3u, 10u, 100u, 300u, 350u, g.R(351), g.R(351), g.R(351), 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFFDu}));
    return u32(std::clamp(v, -3, 350));
}
inline u32 PosBelow(Gen& g, u32 count) { return count > 0 && i32(count) > 0 ? g.R(count) : 0u; }

// Un float de escala 2^s con mantisa aleatoria en [-1, 1)
inline double UnitD(Gen& g) { return double(g.W() >> 8) / 8388608.0 - 1.0; }
inline u32 ScaledF(Gen& g, int s) { return D2F(std::ldexp(UnitD(g), s)); }
inline void MakeTri(Gen& g, std::array<u32, 64>& a) {
    const u32 mode = g.R(16);
    int s = 0;
    const int scales[] = {0, 0, 0, 0, 5, -5, 20, -20, 60, -60};
    if (mode <= 5 || mode == 15) s = scales[g.R(10)];
    else if (mode <= 8) s = -70 + int(g.R(12)) - 6;       // los productos de dos diferencias caen por debajo del menor denormal del float: -76..-65 por factor -> -152..-130
    else if (mode <= 10) s = 58 + int(g.R(10));           // los productos superan el maximo del float (2^128) pero no el de la x87
    // vertices y punto
    double v[3][2];
    for (auto& p : v) for (auto& c : p) c = g.P(8) ? 0.0 : std::ldexp(UnitD(g), s);
    if (mode == 15) for (auto& p : v) for (auto& c : p) c = std::ldexp(c, int(g.R(40)) - 20);
    double w[3] = {g.P(15) ? 0.0 : double(1 + g.R(1000)), g.P(15) ? 0.0 : double(1 + g.R(1000)), g.P(15) ? 0.0 : double(1 + g.R(1000))};
    if (w[0] + w[1] + w[2] == 0.0) w[g.R(3)] = 1.0;
    const double sum = w[0] + w[1] + w[2];
    double px = (w[0] * v[0][0] + w[1] * v[1][0] + w[2] * v[2][0]) / sum, py = (w[0] * v[0][1] + w[1] * v[1][1] + w[2] * v[2][1]) / sum;
    const double e1 = (v[0][0] - v[1][0]) * (v[0][1] - py) + (v[1][1] - v[0][1]) * (v[0][0] - px);
    if (e1 > 0.0) std::swap(v[1], v[2]);
    if (mode == 12) {                                   // degenerado: v3 alineado con v1 y v2 y el punto sobre la recta
        const double t = double(g.R(200)) / 100.0 - 0.5;
        v[2][0] = v[0][0] + t * (v[1][0] - v[0][0]); v[2][1] = v[0][1] + t * (v[1][1] - v[0][1]);
        const double u = double(g.R(100)) / 100.0;
        px = v[0][0] + u * (v[1][0] - v[0][0]); py = v[0][1] + u * (v[1][1] - v[0][1]);
    } else if (mode == 13) {                            // el punto es un vertice
        const u32 k = g.R(3);
        px = v[k][0]; py = v[k][1];
    }
    a[3] = D2F(px); a[4] = D2F(py);
    a[5] = D2F(v[0][0]); a[6] = D2F(v[0][1]); a[7] = ScaledF(g, s);
    a[8] = D2F(v[1][0]); a[9] = D2F(v[1][1]); a[10] = ScaledF(g, s);
    a[11] = D2F(v[2][0]); a[12] = D2F(v[2][1]); a[13] = ScaledF(g, s);
    if (g.P(12)) a[3 + g.R(2)] += g.Pk({1u, 0xFFFFFFFFu});   // un ulp de diferencia en x o y
    if (mode == 11 || mode == 14) {
        // coordenadas arbitrarias (con ceros con signo, infinitos, NaN, denormales)
        for (u32 i = 3; i <= 13; ++i) a[i] = mode == 11 ? FloatBits(g) : (g.W() & 0xFF7FFFFFu);
        if (mode == 14) for (u32 i = 3; i <= 13; ++i) if (IsNanBits(a[i])) a[i] &= 0xFF7FFFFFu;
    }
    // normal
    const int ns = int(g.Pk({0u, 0u, 5u, 20u, 60u, 100u})) * (g.P(50) ? 1 : -1);
    for (u32 i = 14; i <= 16; ++i) {
        const u32 pick = g.R(12);
        a[i] = pick == 0 ? 0u : (pick == 1 ? 0x80000000u : (pick == 2 ? (g.P(50) ? 0x3F800000u : 0xBF800000u) : (pick == 3 ? (g.W() & 0x807FFFFFu) : ScaledF(g, ns))));
    }
    if (g.P(40)) a[16] = g.Pk({0u, 0x80000000u, 0x00000001u, 0x80000001u, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F800000u});   // nz: cero, denormal, minimo, maximo, uno
    if (g.P(4)) a[16] = g.Pk({0x7F800000u, 0xFF800000u});
    // NaN: solo v1.z puede ser NaN entre las operandos de z (aparece una sola vez en la formula; nx, ny y nz aparecen dos veces y entonces la x87 resuelve distinto un SNaN en memoria y un QNaN en registro). Con NaN no hay infinitos.
    if (g.P(6)) a[7] = g.Pk({0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FC00000u | (g.W() & 0x3FFFFFu), 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u), 0xFFC00000u | (g.W() & 0x3FFFFFu)});
    const bool nan = IsNanBits(a[7]);
    for (u32 k : {14u, 15u, 16u}) if (IsNanBits(a[k])) a[k] = 0x3F800000u;
    for (u32 k : {3u, 4u, 5u, 6u, 7u, 14u, 15u}) if (IsInfBits(a[k])) a[k] = (a[k] & 0x80000000u) | 0x7F7FFFFFu;
    if (nan && IsInfBits(a[16])) a[16] = 0x3F800000u;
    if (g.P(25) && !nan) {   // z diminuto: v1.z, nx y ny entre 2^-150 y 2^-120 y nz moderado (denormales, ceros con signo y desbordamiento por abajo al guardar el float)
        a[7] = ScaledF(g, -150 + int(g.R(26))); a[14] = ScaledF(g, -150 + int(g.R(30))); a[15] = ScaledF(g, -150 + int(g.R(30))); a[16] = ScaledF(g, int(g.R(8)) - 4);
    }
    a[17] = AuxOffset(g, 4);
}

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[kSeed] = c.seed * 13u + 50u; c.a[kSeed + 1] = c.a[kSeed] + 1u; c.a[kSeed + 2] = c.a[kSeed] + 2u; c.a[kSeed + 3] = c.a[kSeed] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn], WinA(c.fn));
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kStat: {
        std::array<u32, 4> w{};
        MakeStat(g, w);
        a[3] = g.Pk({AuxIn(g, 0, u32(kWin), 16), AuxIn(g, 0, u32(kWin), 16), AuxIn(g, 0, u32(kWin), 16), u32(kWin) - 16u + g.R(16), u32(kWin) - 1u - g.R(8), g.R(8)});
        for (u32 i = 0; i < 4; ++i) a[4 + i] = w[i];
        break;
    }
    case kRemMod: {
        a[3] = ModCount(g);
        a[4] = Raw(g);
        a[5] = g.Pk({0u, 1u, 1u, 1u, 2u});
        a[6] = PosBelow(g, a[3]);
        a[7] = i32(a[3]) > 0 ? a[6] + g.R(u32(i32(a[3]) - i32(a[6]))) : 0u;
        a[8] = PriceCount(g);
        break;
    }
    case kAddMod: {
        a[3] = ModCount(g);
        a[4] = Raw(g);
        a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u, 1u, 2u});
        a[7] = PosBelow(g, a[3]);
        a[8] = i32(a[3]) > 0 ? a[7] + g.R(u32(i32(a[3]) - i32(a[7]))) : 0u;
        a[9] = PriceCount(g);
        a[10] = g.Pk({0u, 1u, 1u, 1u, 2u});
        a[11] = i32(a[9]) > 0 ? g.R(a[9]) : 0u;
        a[12] = i32(a[9]) > 0 ? a[11] + g.R(a[9] - a[11]) : 0u;
        break;
    }
    case kGetAt: {
        a[3] = Raw(g);
        const u32 idx = g.Pk({0u, 1u, 2u, 3u, 5u, 100u, 299u, 300u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFF9Cu, g.R(300), g.R(300), g.R(0x1000), g.R(0x1000), 0x7FFFFFFFu, 0x80000000u, 0x40000000u, 0x3C3C3C3Du, 0x3C3C3C3Cu, 0xC3C3C3C3u, g.W(), g.W()});
        a[4] = idx;
        a[5] = g.Pk({0x80u, 0x80u, 0x7Fu, 0u, 0xFFu, 0x01u, 0x81u, 0xFEu, g.R(256), g.R(256)});
        a[6] = g.R(u32(kWin));
        a[7] = g.Pk({300u, 512u, 0u, 0xFFFFFFFFu, g.W()});
        a[8] = g.Pk({0u, 0xFFFFFFFFu, g.R(300), g.W()});
        a[9] = g.W();
        break;
    }
    case kFlush:
        a[3] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFBu, 0u, 0u, 1u, 2u, 5u, 100u, 256u, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(1000)});
        a[4] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, g.W()});
        a[5] = g.Pk({0xFFFFFFFFu, 0u, 1u, 100u, g.W()});
        break;
    case kTri:
        MakeTri(g, a);
        break;
    case kLiCtor: case kManDtor: case kManCtor: case kConst:
        break;
    case kStandClone:
        a[3] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[4] = AuxOffset(g, 0x20);
        a[5] = Raw(g);
        a[6] = ByteDword(g);
        a[7] = ByteDword(g);
        a[8] = g.P(70) ? FloatBits(g) : Raw(g);
        break;
    case kDieClone:
        a[3] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[4] = AuxOffset(g, 0x28);
        a[5] = g.P(45) ? 0u : NonZero(g);
        a[6] = Raw(g); a[7] = Raw(g); a[8] = Raw(g);
        a[9] = g.P(70) ? FloatBits(g) : Raw(g);
        a[10] = g.P(70) ? FloatBits(g) : Raw(g);
        a[11] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[12] = Raw(g);
        break;
    case kDrownClone:
        a[3] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[4] = AuxOffset(g, 0x2C);
        break;
    case kRegLeave:
        a[3] = AuxOffset(g, 0x490);
        a[4] = g.Pk({0u, 1u, 8u, 9u, 10u, 11u, g.R(12), Raw(g)});
        a[5] = ByteDword(g);
        a[6] = g.Pk({0u, 1u, 1u, 1u, 2u, 3u});
        a[7] = AuxOffset(g, 4);
        a[8] = AuxOffset(g, 4);
        a[9] = Raw(g);
        a[10] = g.Pk({0u, 1u, 0x100u, 0xFFu, 0x80000000u, 0xFFFFFF00u, 0x101u, Raw(g)});
        a[11] = g.Pk({0u, 1u, 0x100u, 0xFFu, 0x80000000u, 0xFFFFFF00u, 0x101u, Raw(g)});
        a[12] = g.P(50) ? g.R(16) : Raw(g);
        a[13] = g.Pk({0u, 0xFFu, 0x48u, 0x07u, g.R(256), g.R(256)});
        a[14] = g.Pk({0u, 0xFFu, 0x55u, g.R(256)});
        a[15] = g.Pk({0u, 0xFFu, 0x55u, g.R(256)});
        a[16] = g.Pk({0u, 1u, 0xFFu, 0xFEu, g.R(256)});
        a[17] = g.Pk({0u, 0u, 0u, 1u});
        a[18] = g.Pk({0u, 1u, Raw(g), Raw(g)});
        a[19] = g.W();
        break;
    case kLineUp:
        a[3] = Raw(g);
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x1C);
        a[6] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[7] = Raw(g);
        a[8] = Raw(g);
        break;
    case kLcClone:
        a[3] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[4] = AuxOffset(g, 0x34);
        a[5] = Raw(g); a[6] = Raw(g); a[7] = Raw(g);
        a[8] = ByteDword(g); a[9] = ByteDword(g);
        a[10] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u});
        a[11] = Raw(g);
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
    return j.End();
}
}
