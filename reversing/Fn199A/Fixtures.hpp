#pragma once
// Fixtures del banco Fn199A (lote 199A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 30 reconstruidas.
//   idx  direccion  nombre real                                                                forma
//    0   0x620BF0   CTaskComplexReactToAttack::MakeAbortable (S)                                thiscall(ped, prioridad, evento) `ret 0Ch`: byte +0Dh; MakeAbortable (ranura +18h) de la subtarea +8
//    1   0x620C30   CTaskComplexReactToAttack::CreateSubTask (S)                                thiscall(tipo) `ret 4`: BeHit 0x3F0 (ctor 620780), None 0xC8 (ctor 61A390 + vtable 859CFC), FallAndGetUp 0xD0 (ctor 6786C0)
//    2   0x620E00   CTaskComplexReactToAttack::ControlSubTask (S)                               thiscall(ped) `ret 4`: byte +0Ch ? CreateFirstSubTask (ranura +24h) : GetTaskType de la subtarea (ranura +10h) y devuelve +8 releido
//    3   0x623300   CTaskComplexReactToAttack::Clone (S)                                        thiscall() `ret`: new(24h) 61A5A0; ctor 620B00(+10h, +14h, +18h, +1Ch, +20h)
//    4   0x623370   CTaskComplexKillPedOnFoot::Clone (V)                                        thiscall() `ret`: new(38h) 61A5A0; ctor 620E30; copia los bits 4 y 5 del byte +0Ch (EAX = el del constructor)
//    5   0x4AEA00   CEventVehicleHitAndRun::GetEventType (V)                                    thiscall() `ret`: mov eax,0Eh
//    6   0x4AEA10   CEventVehicleHitAndRun::GetLifeTime (V)                                     thiscall() `ret`: xor eax,eax
//    7   0x4AEA20   CEventVehicleHitAndRun::AffectsPed (V)                                       thiscall(ped) `ret 4`: CPed::IsAlive (5E0170)
//    8   0x4AEA30   CEventVehicleHitAndRun::GetLocalSoundLevel (V)                              thiscall() `ret`: fld dword [858628h] = 100.0f (ST0)
//    9   0x4AEA40   CEventVehicleHitAndRun::GetEventPriority (V)                                thiscall() `ret`: mov eax,0Eh
//   10   0x4AEC00   CEventPedEnteredMyVehicle::AffectsPed (V)                                    thiscall(ped) `ret 4`: IsAlive; vehiculo +58Ch; bit 8 de +46Ch; vehiculo == +18h; +14h no nulo
//   11   0x4B7170   CEventPedEnteredMyVehicle::CloneEditable (V)                                 thiscall() `ret`: CEventPool::New 4B5570 (ECX = [B744AC]); ctor 4AEAC0(+14h, +18h, +1Ch)
//   12   0x4AEDC0   CEventChatPartner::AffectsPed (V)                                            thiscall(ped) `ret 4`: IsAlive y +10h no nulo
//   13   0x4B7210   CEventChatPartner::Clone (V)                                                 thiscall() `ret`: New 4B5570; ctor 4AECD0(byte +0Ch ampliado, +10h)
//   14   0x4AEF00   CEventSexyPed::AffectsPed (V)                                                thiscall(ped) `ret 4`: IsAlive x2, IsLooking 6181A0 / GetLookAtEntity 6181D0, FindActiveTaskByType 681740 (4B2, 4BF, 4B3), GetTaskType
//   15   0x4B7280   CEventSexyPed::CloneEditable (V)                                             thiscall() `ret`: New 4B5570; ctor 4AEDF0(+14h)
//   16   0x4AF100   CEventSexyVehicle::AffectsPed (V)                                            thiscall(ped) `ret 4`: IsAlive y +0Ch no nulo
//   17   0x4B72F0   CEventSexyVehicle::Clone (V)                                                 thiscall() `ret`: New 4B5570; ctor 4AF010(+0Ch)
//   18   0x4B7360   CEventPedToChase::Clone (V)                                                  thiscall() `ret`: New 4B5570; ctor 4AF130(+0Ch)
//   19   0x4B73D0   CEventPedToFlee::Clone (V)                                                   thiscall() `ret`: New 4B5570; ctor 4AF240(+0Ch)
//   20   0x5E0540   CPool<CPed, CCopPed>::CreateAtRef (S)                                        thiscall(ref) `ret 4`: estado[ref >> 8] = ref & 7Fh; cursor +0Ch = primer hueco libre desde 0
//   21   0x5E47A0   CPed::operator delete(void*, int) (S)                                         cdecl(ptr, ref): pool [B74490]: indice = (ptr - almacenamiento) / 7C4h; estado |= 80h; cursor +0Ch = min
//   22   0x5E88E0   CPed::CanWeRunAndFireWithWeapon (S)                                           thiscall() `ret`: ranura (byte +718h con signo), tipo, GetWeaponSkill 5E3B60, GetWeaponInfo 743C60, bit 1 de +18h
//   23   0x5E8C80   CPed destructor con borrado (S)                                              thiscall(banderas) `ret 4`: cuerpo 5E8620; liberacion EN LINEA en el pool [B74490] si bit 0
//   24   0x50A870   CCamera cuerpo del destructor (S)                                            thiscall() `ret`: vtable 8630E8; +14h = 0; 6 x CMatrix::Destructor 59ACD0; ~CPlaceable 54F490
//   25   0x514010   CCamera destructor con borrado (S)                                           thiscall(banderas) `ret 4`: cuerpo 50A870; operator delete 8214BD si bit 0
//   26   0x513220   TwoPlayerCameraVisible (S, stdcall)                                          stdcall(x, y, z) `ret 0Ch`: [8CCB80] = 5; dos ProcessLineOfSight 56BA00 (jugadores B7CD98 y B7CF28)
//   27   0x5132D0   TwoPlayerCameraGeometry (S, thiscall)                                        thiscall(beta, fuente*, frente*, objetivo*) `ret 10h`: fcos/fsin, Normalise 59C910 x2, x87 a 24 bits
//   28   0x517740   CHandShaker::Constructor (S)                                                 thiscall() `ret`: +7Ch = +80h = 0; SetDefaults 517330
//   29   0x517760   CIdleCam::Constructor (S)                                                    thiscall() `ret`: Init 50E6D0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A de 0x3000 bytes, B, C y D de 0x1000, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F199AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 30;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;    // ventana de las arenas B, C y D
constexpr std::size_t kWinA = 0x3000;   // ventana de la arena A (CCamera mide 0xD7C bytes y el ped, con su tabla de armas, necesita holgura)
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
constexpr unsigned kSlots = 48;      // a[44..47] = semillas de las arenas A, B, C y D
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

// Desplazamiento de un objeto de `span` bytes en la ventana `win` segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span, u32 win = u32(kWin)) {
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar (B, C o D), sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }
// Lo mismo dentro de un tramo [lo, lo + len) de la ventana
inline u32 AuxIn(Gen& g, u32 lo, u32 len, u32 span) { return lo + 4 * g.R((len - span) / 4 + 1); }

// Un dword que no cae en el rango donde puede estar la pila del hilo del banco (alrededor de 0xC00000-0x2000000): el etiquetador de punteros del banco (Tag) sustituye cualquier valor de ese rango por una
// etiqueta de pila y el oraculo no conoce la pila; un valor aleatorio que cayera ahi (1 de cada 4000) se leeria como puntero a la pila.
inline u32 Sane(u32 v) { return (v >= 0x00C00000u && v < 0x02000000u) ? v + 0x10000000u : v; }
// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return Sane(g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu}));
}
// EAX que devuelve el falso de un constructor (se devuelve tal cual como puntero: etiqueta de arena o valor crudo)
inline u32 EaxVal(Gen& g) { return Sane(g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 1u, 0x80000000u})); }
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
// Un float finito de exponente acotado (sesgado): signo aleatorio, exponente en [lo, hi] (exponente sesgado de IEEE, 127 = 1.0) y mantisa aleatoria
inline u32 FloatExp(Gen& g, u32 lo, u32 hi) { return (g.P(50) ? 0x80000000u : 0u) | ((lo + g.R(hi - lo + 1)) << 23) | (g.W() & 0x7FFFFFu); }
// Un float para posiciones del mundo: el grueso en +-3000, tambien +-50000, diminutos, ceros y enormes (hasta 2^60: los cuadrados siguen cabiendo en un float; sin denormales)
inline u32 PosBits(Gen& g) {
    const u32 mode = g.R(20);
    if (mode < 10) return FloatExp(g, 127 - 6, 127 + 11);       // |x| de ~0.015 a ~4000
    if (mode < 13) return FloatExp(g, 127 + 8, 127 + 16);       // hasta ~130000
    if (mode < 15) return FloatExp(g, 127 - 40, 127 - 5);       // diminutos
    if (mode < 17) return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u});
    return FloatExp(g, 127 + 30, 127 + 60);                     // enormes (1e9..1e18): los cuadrados siguen cabiendo en un float
}
// Un angulo: valores notables (0, +-pi/2, +-pi, el -0.5236 del juego, +-1), el grueso en +-pi y algunos de hasta ~16000
inline u32 AngleBits(Gen& g) {
    const u32 mode = g.R(10);
    if (mode < 2) return g.Pk({0u, 0x80000000u, 0x3FC90FDBu, 0xBFC90FDBu, 0x40490FDBu, 0xC0490FDBu, 0xBF060A92u, 0x3F800000u, 0xBF800000u, 0x3F060A92u, 0x40C90FDBu, 0x3FC90FDAu, 0x3FC90FDCu});
    if (mode < 8) return FloatExp(g, 127 - 8, 127 + 2);        // |x| de ~0.004 a ~7
    if (mode < 9) return FloatExp(g, 127 + 2, 127 + 6);        // hasta ~100
    return FloatExp(g, 127 + 6, 127 + 13);                     // hasta ~16000
}

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un tipo de estado del ped (+530h): los dos de IsAlive (0x36 y 0x37) con frecuencia, vecinos y valores arbitrarios
inline u32 PedState(Gen& g) {
    return g.Pk({0x36u, 0x37u, 0x36u, 0x37u, 0u, 1u, 0x35u, 0x38u, 0x1Fu, 0x3Fu, 0xFFFFFFFFu, 0x80000036u, 0x100u, g.W(), g.R(0x60), g.R(0x60), g.R(0x60), g.R(0x60)});
}
// Un eax con el bit bajo (AL) en 0 o 1 y los demas bytes aleatorios
inline u32 BoolEax(Gen& g) { return (g.W() & 0xFFFFFF00u) | (g.P(50) ? 1u : 0u); }

// Funciones (indices)
enum Fn : u32 {
    kRaAbort, kRaCreate, kRaControl, kRaClone, kKpClone, kVhrType, kVhrLife, kVhrAffects, kVhrSound, kVhrPrio, kPemvAffects, kPemvClone, kCpAffects, kCpClone, kSpAffects, kSpClone,
    kSvAffects, kSvClone, kPtcClone, kPtfClone, kPoolCreate, kPedDelete, kPedCanRun, kPedSdd, kCamDtor, kCamSdd, kCamVis, kCamGeo, kHsCtor, kIcCtor
};
// Forma del resultado observado: puntero (etiqueta de arena), AL (bool), EAX completo, bits de ST0, nada
enum Ret : u32 { kRetPtr, kRetAl, kRetEax, kRetFloat, kRetVoid };
inline constexpr std::array<Ret, kFunctions> kRet{kRetAl, kRetPtr, kRetPtr, kRetPtr, kRetPtr, kRetEax, kRetEax, kRetAl, kRetFloat, kRetEax, kRetAl, kRetPtr, kRetAl, kRetPtr, kRetAl, kRetPtr,
                                                  kRetAl, kRetPtr, kRetPtr, kRetPtr, kRetVoid, kRetVoid, kRetAl, kRetPtr, kRetVoid, kRetPtr, kRetAl, kRetVoid, kRetPtr, kRetPtr};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x620BF0, 0x620C30, 0x620E00, 0x623300, 0x623370, 0x4AEA00, 0x4AEA10, 0x4AEA20, 0x4AEA30, 0x4AEA40, 0x4AEC00, 0x4B7170, 0x4AEDC0, 0x4B7210, 0x4AEF00,
                                                    0x4B7280, 0x4AF100, 0x4B72F0, 0x4B7360, 0x4B73D0, 0x5E0540, 0x5E47A0, 0x5E88E0, 0x5E8C80, 0x50A870, 0x514010, 0x513220, 0x5132D0, 0x517740, 0x517760};
// Bytes que cada funcion toca a partir de su puntero `this` (en la arena A)
inline constexpr std::array<u32, kFunctions> kSpan{0x10, 0x24, 0x10, 0x24, 0x2C, 4, 4, 4, 4, 4, 0x1C, 0x20, 0x14, 0x14, 0x18, 0x18, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x20, 4, 0xD80, 0xD80, 4, 0xB0, 0x84, 4};
// Desplazamientos de campos del ped sintetico y constantes de los pools
inline constexpr u32 kPedStateOff = 0x530, kPedVehOff = 0x58C, kPedFlagsOff = 0x46C, kPedIntelOff = 0x47C, kPedSlotOff = 0x718, kPedWeaponsOff = 0x5A0, kWeaponStride = 0x1C;
inline constexpr u32 kPedDeleteStride = 0x7C4;
// Memoria global del juego tocada
inline constexpr u32 kPedPoolPtrAddr = 0xB74490, kPlayer0Addr = 0xB7CD98, kPlayer1Addr = 0xB7CF28, kLosByteAddr = 0x8CCB80, kNumMatricesAddr = 0xB74238, kEventPoolPtrAddr = 0xB744AC;
// Capacidad maxima de los pools falsos
inline constexpr u32 kPoolMaxCap = 0x1F0;

// Estado inicial (byte) del hueco i de un pool falso: 7 bits bajos aleatorios; bit 7 (libre) con un 30%
inline u32 SlotByte(u32 seed, u32 i) {
    const u32 s = Rand(seed, i);
    return (s & 0x7Fu) | ((((s >> 8) % 100u) < 30u) ? 0x80u : 0u);
}

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[44..47] = semillas de las arenas A, B, C y D
//   El resto de `a` lo define cada funcion (ver Setup en LiveComparison.hpp y expected en fixture_oracle.py; las dos usan los mismos indices).
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    auto& a = c.a;
    a[1] = kind;
    a[2] = f / 4;
    a[44] = c.seed * 13u + 50u; a[45] = a[44] + 1u; a[46] = a[44] + 2u; a[47] = a[44] + 3u;
    a[0] = Offset(g, kind, kSpan[c.fn], u32(kWinA));
    a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85AFA8u, 0x86D868u, 0x86D894u, 0x8630E8u, 0x863C40u, 0xDEADBEEFu, g.W(), g.W(), 0x859CFCu, 0x86C358u});
    switch (c.fn) {
    case kRaAbort:
        a[4] = AuxOffset(g, 4);                                                    // subtarea en B
        a[5] = g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, g.R(256)});      // byte +0Dh
        a[6] = g.Pk({0u, 1u, 1u, 1u, 2u, 2u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.R(8), g.W()});   // prioridad
        a[7] = Raw(g); a[8] = Raw(g);                                              // ped y evento
        a[9] = BoolEax(g);                                                         // EAX de MakeAbortable de la subtarea
        break;
    case kRaCreate:
        a[4] = g.Pk({0x3F0u, 0x3F0u, 0x3F0u, 0x3F0u, 0xC8u, 0xC8u, 0xC8u, 0xC8u, 0xD0u, 0xD0u, 0xD0u, 0xD0u, 0x3EFu, 0x3F1u, 0x516u, 0xC9u, 0xCFu, 0xD1u, 0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u,
                     0x100003F0u, 0x100000C8u, 0x100000D0u, 0x400u, 0x3EEu, g.W(), g.R(0x800)});
        a[5] = g.Pk({0u, 1u, 1u, 1u});                                             // agresor: 0 nulo, 1 en B
        a[6] = AuxOffset(g, 0x40);
        a[7] = g.Pk({3u, 3u, 3u, 0xFBu, 0x13u, 0x83u, 0u, 1u, 2u, 4u, 5u, 6u, 7u, 0xFFu, g.R(256), g.R(256)});    // byte +36h del agresor (tipo de entidad en los 3 bits bajos)
        a[8] = FloatBits(g);
        if (g.P(40)) a[8] = FloatExp(g, 127 - 4, 127 + 12);                        // valores normales de dano
        else if (g.P(15)) a[8] = g.Pk({0x4F000000u, 0xCF000000u, 0x4EFFFFFFu, 0xCEFFFFFFu, 0x5F000000u, 0xDF000000u, 0x5EFFFFFFu, 0xDEFFFFFFu, 0x4F800000u, 0x4F7FFFFFu, 0x3F7FFFFFu, 0xBF7FFFFFu, 0x3F800000u, 0xBF800000u});
        a[9] = g.Pk({2u, 2u, 2u, 2u, 0u, 1u, 3u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(8)});    // +1Ch
        a[10] = Raw(g);                                                            // +20h
        a[11] = g.Pk({0u, 1u, 1u, 1u, 1u});                                        // memoria de CTask::operator new: 0 nula, 1 en D
        a[12] = AuxOffset(g, 0x38);
        a[13] = EaxVal(g);            // EAX del constructor
        a[14] = Raw(g);                                                            // dword que deja el constructor en [mem]
        break;
    case kRaControl:
        a[4] = g.Pk({0u, 0u, 1u, 1u, 1u, 2u, 0xFFu, g.R(256)});                    // byte +0Ch
        a[5] = AuxOffset(g, 4);                                                    // subtarea en B
        a[6] = Raw(g);                                                             // ped
        a[7] = Raw(g);                                                             // EAX de CreateFirstSubTask
        a[8] = g.Pk({0u, 0u, 1u, 1u});                                             // la ranura GetTaskType de la subtarea reescribe [this+8]
        a[9] = Raw(g);                                                             // lo que escribe
        break;
    case kRaClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x24);
        a[6] = EaxVal(g);
        a[7] = Raw(g);
        for (unsigned i = 8; i < 13; ++i) a[i] = Raw(g);                           // +10h, +14h, +18h, +1Ch, +20h
        break;
    case kKpClone:
        a[4] = AuxOffset(g, 0x38);
        a[5] = Raw(g);                                                             // dword que deja el constructor en [mem]
        for (unsigned i = 6; i < 12; ++i) a[i] = Raw(g);                           // +10h, +14h, +18h, +1Ch, +20h (solo el byte bajo cuenta), +28h
        a[10] = g.Pk({0u, 1u, 0x7Fu, 0x80u, 0xFFu, 0xFEu, 0x100u, g.R(256), g.W(), 5u, 0x81u});
        a[12] = g.Pk({0u, 0x10u, 0x20u, 0x30u, 0xFFu, 0xCFu, g.R(256), g.R(256), g.R(256)});     // byte +0Ch
        break;
    case kVhrType: case kVhrLife: case kVhrSound: case kVhrPrio:
        break;
    case kVhrAffects:
        a[4] = AuxOffset(g, 0x540);
        a[5] = PedState(g);
        break;
    case kPemvAffects: {
        a[4] = g.P(25) ? 0u : Raw(g);                                              // peaton que entro (+14h)
        a[5] = g.P(15) ? 0u : (g.P(30) ? g.W() | 4u : Raw(g));                    // vehiculo del evento (+18h)
        a[6] = AuxOffset(g, 0x5A0);
        a[7] = PedState(g);
        a[8] = g.Pk({0u, a[5], a[5], a[5], a[5], Raw(g), g.W() | 8u});             // vehiculo del ped (+58Ch)
        a[9] = g.Pk({0x100u, 0x100u, 0x100u, 0xFFFFFFFFu, g.W() | 0x100u, g.W() & ~0x100u, 0u, 0x200u, 0xFFFFFEFFu, 0x80u, 0x101u});       // +46Ch
        break;
    }
    case kPemvClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x20);
        a[6] = EaxVal(g);
        a[7] = Raw(g);
        a[8] = Raw(g); a[9] = Raw(g); a[10] = Raw(g);                              // +14h, +18h, +1Ch
        break;
    case kCpAffects:
        a[4] = g.P(30) ? 0u : Raw(g);                                              // companero (+10h)
        a[5] = AuxOffset(g, 0x540);
        a[6] = PedState(g);
        break;
    case kCpClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x14);
        a[6] = EaxVal(g);
        a[7] = Raw(g);
        a[8] = g.Pk({0u, 1u, 1u, 2u, 0x7Fu, 0x80u, 0xFFu, g.R(256), g.R(256)});    // byte +0Ch
        a[9] = Raw(g);                                                             // +10h
        break;
    case kSpAffects: {
        // A: this (+14h = atractivo en D); B: ped (estado +530h, inteligencia +47Ch en C); D: atractivo (estado, inteligencia en C); C: tramos de 0x400: inteligencia del ped, la del atractivo, tarea del ped y tarea del atractivo
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u, 1u});                                    // atractivo: 0 nulo, 1 en D
        a[5] = AuxOffset(g, 0x540);                                                // atractivo en D
        a[6] = AuxOffset(g, 0x540);                                                // ped en B
        a[7] = g.Pk({0x36u, 0x37u, 0u, 0u, 0u, 0u, 0u, 1u, 0x35u, PedState(g)});    // estado del ped (casi siempre vivo)
        a[8] = AuxIn(g, 0x000, 0x400, 0x10);                                       // inteligencia del ped (en C)
        a[9] = g.Pk({0x36u, 0x37u, 0u, 0u, 0u, 0u, 0u, 1u, 0x35u, PedState(g)});    // estado del atractivo
        a[10] = AuxIn(g, 0x400, 0x400, 0x10);                                      // inteligencia del atractivo
        a[11] = AuxIn(g, 0x800, 0x400, 4);                                         // tarea del ped
        a[12] = AuxIn(g, 0xC00, 0x400, 4);                                         // tarea del atractivo
        a[13] = BoolEax(g);                                                        // EAX de IsLooking
        a[14] = Raw(g);                                                            // lo que devuelve GetLookAtEntity si no es el atractivo
        a[15] = g.Pk({0u, 1u, 1u});                                                // 1: GetLookAtEntity devuelve el atractivo
        for (unsigned i = 16; i < 22; ++i) a[i] = g.Pk({0u, 0u, 1u});              // FindActiveTaskByType por tipo (4B2, 4BF, 4B3): ped (16..18) y atractivo (19..21)
        a[22] = g.Pk({0x4B2u, 0x4BFu, 0x4B3u, 0u, g.W(), g.R(0x600)});             // GetTaskType de la tarea del ped
        a[23] = g.P(35) ? a[22] : g.Pk({0x4B2u, 0x4BFu, 0x4B3u, 0u, g.W(), g.R(0x600)});     // GetTaskType de la tarea del atractivo
        break;
    }
    case kSpClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x18);
        a[6] = EaxVal(g);
        a[7] = Raw(g);
        a[8] = Raw(g);                                                             // +14h
        break;
    case kSvAffects:
        a[4] = g.P(30) ? 0u : Raw(g);                                              // vehiculo (+0Ch)
        a[5] = AuxOffset(g, 0x540);
        a[6] = PedState(g);
        break;
    case kSvClone: case kPtcClone: case kPtfClone:
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = EaxVal(g);
        a[7] = Raw(g);
        a[8] = Raw(g);                                                             // +0Ch
        break;
    case kPoolCreate: {
        // A: objeto del pool en a[0] (+0 almacenamiento, +4 estados, +8 capacidad, +0Ch cursor); B: estados (a[4] = desplazamiento)
        const u32 cap = 0x100u + g.R(kPoolMaxCap - 0x100u + 1);
        a[4] = 4u * g.R((u32(kWin) - kPoolMaxCap - 4u) / 4u);
        a[5] = cap;
        a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.R(cap), g.R(cap), cap, cap - 1u});   // cursor inicial
        a[7] = g.Pk({0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap), g.R(cap), 1u, cap / 2u});                           // indice
        a[8] = g.Pk({0u, 0x7Fu, 0x80u, 0xFFu, g.R(256), g.R(256), g.R(256), 1u, 0x40u});                             // byte bajo de la referencia
        a[9] = Raw(g);                                                             // almacenamiento (+0, no se lee)
        a[10] = g.W();                                                             // semilla de los estados
        break;
    }
    case kPedDelete: {
        // A: objeto del pool en a[0]; B: estados en a[4]; [B74490] = objeto del pool
        const u32 cap = 0x100u + g.R(kPoolMaxCap - 0x100u + 1);
        a[4] = 4u * g.R((u32(kWin) - kPoolMaxCap - 4u) / 4u);
        a[5] = cap;
        a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.R(cap), g.R(cap), g.R(cap), cap, cap - 1u, cap + 3u});   // cursor
        a[7] = g.Pk({0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap), g.R(cap), 1u});    // indice del puntero
        a[8] = g.Pk({0u, 0u, 1u, kPedDeleteStride - 1u, g.R(kPedDeleteStride), g.R(kPedDeleteStride), g.R(kPedDeleteStride)});   // resto dentro del hueco
        a[9] = 0x00010000u + (g.W() % 0x70000000u);                                // almacenamiento (solo aritmetica)
        a[10] = g.W();                                                             // semilla de los estados
        a[11] = Raw(g);                                                            // segundo argumento (no se lee)
        break;
    }
    case kPedCanRun: {
        // `this` (el ped) en A dentro de [0x900, 0x1C00]; el arma activa es el byte +718h con signo
        const u32 lo = 0x900, len = 0x1C00 - 0x900;
        u32 off;
        switch (kind) {
        case 0: off = lo + 4 * g.R(len / 4 + 1); break;
        case 1: off = lo + 4 * g.R(len / 4) + 1 + g.R(3); break;
        case 2: off = lo + g.R(0x40); break;
        default: off = 0x1C00 - g.R(0x40); break;
        }
        a[0] = off;
        a[4] = g.Pk({0u, 0u, 1u, 2u, 3u, 5u, 12u, 0xFFu, 0x80u, 0x7Fu, 0xFEu, g.R(256), g.R(256), 7u, 9u});     // ranura (byte)
        a[5] = g.Pk({0u, 1u, 2u, 3u, 22u, 24u, 30u, 34u, 41u, 46u, g.R(0x30), g.R(0x30), g.W(), 0xFFFFFFFFu});   // tipo de arma en esa ranura
        a[6] = g.Pk({0u, 1u, 2u, 3u, 4u, g.R(256), g.R(5), g.R(5)});                // EAX de GetWeaponSkill (devuelve un eWeaponSkill de 8 bits: sin basura en los bytes altos)
        a[7] = AuxOffset(g, 0x20);                                                 // CWeaponInfo falso en C
        a[8] = g.Pk({0u, 2u, 2u, 2u, 0xFFFFFFFFu, g.W(), g.W() | 2u, g.W() & ~2u, 1u, 3u, 0x80000002u});          // dword +18h (bit 1 = bAimWithArm)
        break;
    }
    case kPedSdd: {
        // `this` (el ped) en A; el pool en C (a[5], objeto de 0x10 bytes) con estados en D (a[6]); [B74490] = pool; almacenamiento = this - indice * 7C4 - resto
        const u32 cap = 0x100u + g.R(kPoolMaxCap - 0x100u + 1);
        a[4] = FlagsDword(g);
        a[5] = AuxOffset(g, 0x10);
        a[6] = 4u * g.R((u32(kWin) - kPoolMaxCap - 4u) / 4u);
        a[7] = cap;
        a[8] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.R(cap), g.R(cap), g.R(cap), cap, cap - 1u, cap + 3u});      // cursor
        a[9] = g.Pk({0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap), g.R(cap), 1u});    // indice
        a[10] = g.Pk({0u, 0u, 1u, kPedDeleteStride - 1u, g.R(kPedDeleteStride), g.R(kPedDeleteStride), g.R(kPedDeleteStride)});
        a[11] = Raw(g);                                                            // dword que deja el falso de 5E8620 en [this]
        a[12] = g.W();                                                             // semilla de los estados
        break;
    }
    case kCamDtor:
        a[4] = g.Pk({0u, 0u, 1u, Raw(g), Raw(g), g.W(), 0xB74240u, 0xFFFFFFFFu});   // +14h (el EXE lo pone a cero antes de destruir los miembros)
        a[5] = Raw(g);                                                             // dword que dejan los falsos de CMatrix::Destructor
        break;
    case kCamSdd:
        a[4] = FlagsDword(g);
        a[5] = Raw(g);                                                             // dword que deja el falso del cuerpo en [this]
        a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), 0xB74240u});                      // +14h previo
        break;
    case kCamVis: {
        // B: ped 0 (+4 posicion, +14h matriz), ped 1, matriz 0 y matriz 1 (posicion en +30h), en cuatro tramos de 0x400; fuente por valor (x, y, z)
        a[4] = AuxIn(g, 0x000, 0x400, 0x20);                                       // ped 0
        a[5] = AuxIn(g, 0x400, 0x400, 0x20);                                       // ped 1
        a[6] = AuxIn(g, 0x800, 0x400, 0x40);                                       // matriz 0
        a[7] = AuxIn(g, 0xC00, 0x400, 0x40);                                       // matriz 1
        a[8] = g.Pk({0u, 1u, 1u});                                                 // ped 0 con matriz
        a[9] = g.Pk({0u, 1u, 1u});                                                 // ped 1 con matriz
        for (unsigned i = 10; i < 16; ++i) a[i] = FloatBits(g);                    // posicion del ped 0 (10..12) y del ped 1 (13..15), tal cual (la funcion solo las pasa)
        for (unsigned i = 16; i < 19; ++i) a[i] = FloatBits(g);                    // fuente (x, y, z)
        a[19] = BoolEax(g); a[20] = BoolEax(g);                                    // EAX de la primera y la segunda ProcessLineOfSight
        a[21] = g.Pk({5u, 0u, 1u, 0xFFu, g.R(256)});                               // byte 8CCB80 antes
        break;
    }
    case kCamGeo: {
        // A: camara (+0ACh angulo vertical); B: ped 0, ped 1, matriz 0, matriz 1 (como kCamVis); C: fuente (a[8]) y frente (a[9]); D: objetivo (a[10])
        a[4] = AngleBits(g);                                                       // alfa (+0ACh)
        a[5] = AngleBits(g);                                                       // beta
        a[6] = AuxIn(g, 0x000, 0x200, 0x20);                                       // ped 0 (en B)
        a[7] = AuxIn(g, 0x200, 0x200, 0x20);                                       // ped 1 (en B)
        a[8] = AuxIn(g, 0x000, 0x400, 0x10);                                       // fuente (en C)
        a[9] = AuxIn(g, 0x800, 0x400, 0x10);                                       // frente (en C)
        a[10] = AuxOffset(g, 0x10);                                                // objetivo (en D)
        a[11] = AuxIn(g, 0x400, 0x200, 0x40);                                      // matriz 0 (en B)
        a[12] = AuxIn(g, 0x600, 0x200, 0x40);                                      // matriz 1 (en B)
        a[13] = g.Pk({0u, 1u, 1u});                                                // ped 0 con matriz
        a[14] = g.Pk({0u, 1u, 1u});                                                // ped 1 con matriz
        for (unsigned i = 15; i < 21; ++i) a[i] = PosBits(g);                      // posicion del ped 0 (15..17) y del ped 1 (18..20)
        if (g.P(8)) { a[18] = a[15]; a[19] = a[16]; a[20] = a[17]; }               // jugadores en el mismo sitio
        else if (g.P(8)) { a[18] = a[15]; }
        if (g.P(5)) { a[17] = a[20] = 0u; }                                        // sin diferencia en z
        break;
    }
    case kHsCtor: case kIcCtor:
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
