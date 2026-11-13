#pragma once
// Fixtures del banco Fn176A (lote 176A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (reconstruidas en el lote 176A SIN validar: ver reversing/PendienteValidar/176A.md; las clases del listado salen del desensamblado, de las vtables y de los llamadores).
//   idx  direccion  nombre real                                                                         forma
//    0   0x4AD800   CPedDamageResponseCalculator::IsBleedingWeapon                                      thiscall(ped) `ret 4`: CPed::IsPlayer 5DF8F0 (ecx = ped); si no es el jugador: [this+0Ch] en {4, 8, 9}
//    1   0x4B5CF0   CEventDamage destructor con borrado (vtable 0x85AD78)                                thiscall(flags) `ret 4`: cuerpo 4AD960; con el bit 0 del BYTE libera la ranura del pool 0xB744AC EN LINEA (indice /0x44 con signo)
//    2   0x502F80   CAEWaterCannonAudioEntity::UpdateGenericWaterCannonSound                             thiscall(10 dwords) `ret 28h`: ranura [this+90h+id*4]; existente: volumen, velocidad, SetPosition; nueva: Initialise 4EFE50 + RequestNewSound 4EFB10; sin splash: StopSound
//    3   0x591260   FurnitureManager_c::Constructor                                                      thiscall() `ret`: 9 grupos de 12 bytes (3 dwords a 0), 512 elementos de 16 bytes (+0 y +4 a 0) desde +6Ch, lista +206Ch (3 dwords a 0); eax = this
//    4   0x5912E0   FurnitureManager_c::Destructor                                                       thiscall() `ret`: los destructores de elemento son `ret`: no escribe memoria
//    5   0x591360   Interior_c::Destructor                                                               thiscall() `ret`: igual (0x4A8DE0 y 0x4A8DC0 son `ret`)
//    6   0x646F40   CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ComputeTarget                       thiscall(salida) `ret 4`: GetPositionToOpenCarDoor 64E740; posicion del coche (matriz +30h o +4); x87 (door - car)*2 + car
//    7   0x646FE0   CTaskSimpleCarSetPedInAsPassenger::Constructor                                       thiscall(vehiculo, puerta, utilidad) `ret 0Ch`: base 61A390; vtable 86EE04; campos; RegisterReference 571B70 si hay vehiculo
//    8   0x64B5B0   CTaskSimpleCarSetPedInAsPassenger destructor con borrado (vtable 0x86EE04)            thiscall(flags) `ret 4`: cuerpo 647080; delete 61A5B0 si bit 0
//    9   0x653AA0   CTaskComplexHitPedWithCar::HitKillsPed                                               thiscall(ped) `ret 4`: IsPlayer 5DF8F0; fcomp: umbral (20.0 / 12.0) < [this+10h]
//   10   0x6563B0   CTaskSimpleKillPedWithCar destructor con borrado (vtable 0x86F2C0)                    cuerpo 653C90; delete 61A5B0 si bit 0
//   11   0x6563D0   CTaskSimpleKillPedWithCar::ShakePadIfPlayerVehicle                                   thiscall() `ret`: FindPlayerVehicle 56E0D0; x87; GetPad 53FB70; StartShake 53F920
//   12   0x657B60   CTaskComplexWalkRoundCar destructor con borrado (vtable 0x86F308)                    cuerpo 656B00; delete 61A5B0 si bit 0
//   13   0x675C80   CTaskInteriorSitInChair::GetTaskType                                                 thiscall() `ret`: mov eax,57Fh
//   14   0x675C90   CTaskInteriorSitInChair cuerpo del destructor (SEH)                                  vtable 870314h; SetDeleteCallback 4CEBC0 sobre +14h si existe; ~CTask 61A3A0
//   15   0x675CF0   CTaskInteriorSitInChair::Clone                                                       thiscall() `ret`: new(2Ch) 61A5A0; constructor 675C30 (+8, +0Ch, byte +10h)
//   16   0x675D60   CTaskInteriorSitInChair::MakeAbortable                                              thiscall(ped, prioridad, evento) `ret 0Ch`: prioridad 2: IsLooking/AbortLookAt, anim a -1000, SetDeleteCallback; si no: byte +2Ah = 1
//   17   0x691460   CTaskSimpleHoldEntity::GetTaskType (V, ranura 4)                                     thiscall() `ret`: mov eax,133h
//   18   0x691630   CTaskSimpleHoldEntity cuerpo del destructor (SEH)                                    vtable 870B2Ch; entidad +8 (banderas +1Ch); SetFinishCallback 4CEBE0 (+38h); RemoveAnimBlockRef 4D3FD0 (+2Ch); ~CTask 61A3A0
//   19   0x693590   CTaskSimplePickUpEntity destructor con borrado (vtable 0x870B50)                     call 6935B0 (= jmp 691630); delete 61A5B0 si bit 0
//   20   0x6935C0   CTaskSimplePutDownEntity destructor con borrado (vtable 0x870B74)                    call 6935E0 (= jmp 691630); delete 61A5B0 si bit 0
//   21   0x6935F0   CTaskComplexGoPickUpEntity destructor con borrado (vtable 0x870B98)                  cuerpo 691A50; delete 61A5B0 si bit 0
//   22   0x6939D0   CTaskSimpleDuck destructor con borrado (vtable 0x870BC4)                             cuerpo 692030; delete 61A5B0 si bit 0
//   23   0x728AB0   CAEWaterCannonAudioEntity cuerpo del destructor (SEH)                                vtable 872A60h; Terminate 5030A0; vtable 859988h; CAESound::~CAESound 4EF660 sobre +8
//   24   0x7448B0   Input::HandlePadButtonDown (usercall: EAX = RsPadButtonStatus*)                       GetPad 53FB70; UpdateJoyButtonState 52F510; bucle 1..16: UpdateJoy_ButtonDown 530ED0 o UpdateJoyInConfigMenus_ButtonDown 52DAB0; eax = 1
//   25   0x744930   Input::HandlePadButtonUp (usercall: EAX = RsPadButtonStatus*)                         idem con la camara (modo 15 o 17) y bucle 2..16: AffectControllerStateOn_ButtonUp 531070 o UpdateJoyInConfigMenus_ButtonUp 52DC20
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40; A es mayor para FurnitureManager_c) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F176AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 26;
constexpr unsigned kArenas = 4;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;    // ventana de las arenas B, C y D
constexpr std::size_t kWinA = 0x2200;   // ventana de la arena A (FurnitureManager_c ocupa 0x2078 bytes)
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
    std::array<u32, 40> a{};
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

// Desplazamiento de un objeto de `span` bytes en la ventana de la arena A segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWinA);
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
inline u32 RawNz(Gen& g) { return Raw(g) | 1u; }   // distinto de cero y nunca una direccion de arena alineada del oraculo
// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    // magnitudes alrededor de la mitad del maximo (el doble de una diferencia desborda el float pero la suma posterior puede volver al rango)
    const u32 huge = sign | (0x7E800000u + (g.W() & 0x007FFFFFu));
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, huge, huge, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline u32 QuietSig(u32 b) { return (b | 0x00400000u) & 0x7FFFFFu; }   // significando de un NaN ya silenciado
// Un float finito normal positivo con exponente sesgado en [lo, hi)
inline u32 PosNormal(Gen& g, u32 lo, u32 hi) { return ((lo + g.R(hi - lo)) << 23) | (g.W() & 0x7FFFFFu); }

// Clases de funcion (forma)
enum Kind : u32 { kBleed, kEvtSdd, kWater, kFurnCtor, kEmpty, kTarget, kPassCtor, kSdd, kHitKills, kShake, kConst, kSitDtor, kSitClone, kSitAbort, kHoldDtor, kWcDtor, kPadDown, kPadUp };
inline constexpr std::array<Kind, kFunctions> kKind{kBleed, kEvtSdd, kWater, kFurnCtor, kEmpty, kEmpty, kTarget, kPassCtor, kSdd, kHitKills, kSdd, kShake, kSdd, kConst, kSitDtor, kSitClone, kSitAbort,
                                                    kConst, kHoldDtor, kSdd, kSdd, kSdd, kSdd, kWcDtor, kPadDown, kPadUp};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4AD800, 0x4B5CF0, 0x502F80, 0x591260, 0x5912E0, 0x591360, 0x646F40, 0x646FE0, 0x64B5B0, 0x653AA0, 0x6563B0, 0x6563D0, 0x657B60, 0x675C80, 0x675C90,
                                                    0x675CF0, 0x675D60, 0x691460, 0x691630, 0x693590, 0x6935C0, 0x6935F0, 0x6939D0, 0x728AB0, 0x7448B0, 0x744930};
// Cuerpo (colaborador) que llaman los destructores con borrado (0 = ninguno). Los de PickUp y PutDown pasan por `jmp 691630`.
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0x4AD960, 0, 0, 0, 0, 0, 0, 0x647080, 0, 0x653C90, 0, 0x656B00, 0, 0, 0, 0, 0, 0, 0x691630, 0x691630, 0x691A50, 0x692030, 0, 0, 0};
inline constexpr std::array<u32, kFunctions> kConstVal{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x57F, 0, 0, 0, 0x133, 0, 0, 0, 0, 0, 0, 0, 0};
// vtable que escribe la funcion o que puede traer el objeto antes (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0x85AD78, 0x872A60, 0, 0, 0, 0x86EE04, 0x86EE04, 0x86EE04, 0x86F2A4, 0x86F2C0, 0x86F2C0, 0x86F308, 0x870314, 0x870314, 0x870314, 0x870314,
                                                     0x870B2C, 0x870B2C, 0x870B50, 0x870B74, 0x870B98, 0x870BC4, 0x872A60, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this` (o de su salida)
inline constexpr std::array<u32, kFunctions> kSpan{0x10, 4, 0xE0, 0x2078, 0x2078, 0x80, 0x10, 0x20, 4, 0x14, 4, 0x10, 4, 4, 0x18, 0x14, 0x2C, 4, 0x3C, 4, 4, 4, 4, 0x10, 4, 4};
// Tamano de los objetos auxiliares sinteticos (arenas B, C y D)
inline constexpr u32 kObjSpan = 0x40;

// Memoria global del juego tocada
inline constexpr u32 kEventPoolGlobal = 0xB744AC;                       // puntero al pool de eventos (CEventDamage SDD)
inline constexpr u32 kPadNumber = 0xB73400, kMenuActive = 0xBA67A4;     // bytes: CPad::padNumber y FrontEndMenuManager.m_bMenuActive
inline constexpr u32 kButtonStates = 0xB70CF4, kActiveCam = 0xB6F081, kCamModes = 0xB6F1A8;   // 16 bytes de botones, byte de la camara activa y modos (word) con paso 0x238
inline constexpr u32 kCamStride = 0x238, kCamCount = 3;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas A, B, C y D
//   kBleed: a[4] = tipo de arma (+0Ch), a[5] = ped (pila, crudo), a[6] = EAX del falso de CPed::IsPlayer (solo cuenta AL)
//   kEvtSdd: a[4] = desplazamiento del pool en B, a[5] = distancia this - almacenamiento (con signo), a[6] = desplazamiento de los estados en D, a[7] = banderas (pila), a[8] = EAX del falso del cuerpo, a[9] = dword que deja en [this],
//            a[10] = ultimo libre (+0Ch del pool), a[11] = byte previo del estado del slot, a[12] = tamano (+8 del pool)
//   kWater: a[4] = splash (pila, solo el byte bajo), a[5] = id (int16 extendido con signo), a[6] = banco, a[7] = sfx (int16 extendido), a[8..9] = velocidad y volumen (bits), a[10..12] = posicion (bits), a[13] = rolloff (bits),
//           a[14] = modo de la ranura (0 nula, 1 en B), a[15] = desplazamiento del sonido en B, a[16] = EAX del falso de RequestNewSound, a[17] = volumen previo del sonido (+14h), a[18] = velocidad previa (+1Ch)
//   kFurnCtor, kEmpty: nada mas
//   kTarget: a[4] = desplazamiento de la tarea de espera en B, a[5] = desplazamiento del coche en C, a[6] = puerta (dword), a[7..9] = posicion del coche (+4, +8, +0Ch), a[10] = matriz (0 nula, 1 en D), a[11] = desplazamiento de la
//            matriz en D, a[12..14] = posicion de la matriz (+30h..+38h), a[15..17] = lo que escribe el falso de GetPositionToOpenCarDoor, a[18] = desplazamiento de la salida en B
//   kPassCtor: a[4] = vehiculo (pila, crudo), a[5] = puerta, a[6] = utilidad, a[7] = dword que deja el falso del constructor base en [this], a[8] = EAX de ese falso
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que deja en [this], a[6] = banderas (pila)
//   kHitKills: a[4] = impacto (+10h, bits), a[5] = ped (pila, crudo), a[6] = EAX del falso de IsPlayer
//   kShake: a[4] = coche (0 en B, 1 crudo), a[5] = desplazamiento en B, a[6] = coche crudo, a[7] = modo del jugador (0 el mismo coche, 1 nulo, 2 otro crudo), a[8] = otro crudo, a[9..11] = velocidad (bits), a[12] = masa (bits),
//            a[13] = modo del pad (0 en C, 1 crudo), a[14] = desplazamiento en C, a[15] = pad crudo, a[16] = sin uso
//   kConst: nada mas
//   kSitDtor: a[4] = animacion (0 nula, 1 cruda), a[5] = animacion cruda, a[6] = EAX del falso de ~CTask, a[7] = dword que deja en [this]
//   kSitClone: a[4] = informacion (+8), a[5] = duracion (+0Ch), a[6] = modo (+10h, dword: solo cuenta el byte), a[7] = memoria (0 nula, 1 en B), a[8] = desplazamiento en B, a[9] = EAX del falso del constructor, a[10] = dword que deja en [memoria]
//   kSitAbort: a[4] = animacion (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = prioridad (pila), a[7] = ped (pila, crudo), a[8] = evento (pila, crudo), a[9] = EAX del falso de IsLooking, a[10] = byte previo +2Ah
//   kHoldDtor: a[4] = byte +35h, a[5] = entidad (0 nula, 1 en B), a[6] = desplazamiento en B, a[7] = banderas de la entidad (+1Ch), a[8] = animacion (+38h, cruda), a[9] = bloque de animaciones (+2Ch, crudo), a[10] = EAX del falso de ~CTask,
//            a[11] = dword que deja en [this]
//   kWcDtor: nada mas (los falsos no escriben)
//   kPadDown / kPadUp: a[4] = identificador de pad (primer dword del estado), a[5] = padNumber (byte), a[6] = menu (byte), a[7..10] = los 16 botones, a[11] = camara activa (0..2), a[12..14] = modos de las tres camaras (words),
//            a[15] = desplazamiento del estado en B, a[16] = sin uso
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = kVtable[c.fn];
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x86D4A8u, 0x86D48Cu, vt + 1, g.W(), g.W(), 0xDEADBEEFu, 0x86D4CCu}); };
    c.a[3] = prev();
    auto& a = c.a;
    const auto isPlayerRet = [&] { return g.Pk({0u, 0u, 1u, 1u, 0x100u, 0xFF00u, 0xFFFFFF00u, 0xFFFFFFFFu, 0x80u, g.W(), g.W() | 1u, g.W() & ~0xFFu}); };
    switch (kKind[c.fn]) {
    case kBleed:
        a[4] = g.Pk({4u, 8u, 9u, 4u, 8u, 9u, 0u, 1u, 2u, 3u, 5u, 6u, 7u, 10u, 11u, 12u, 0xFFFFFFFFu, 0x80000000u, 0x100000u, g.W(), g.R(64), g.R(64)});
        a[5] = Raw(g); a[6] = isPlayerRet();
        break;
    case kEvtSdd: {
        const i32 idx = i32(g.Pk({0u, 1u, 2u, 3u, 7u, 16u, 100u, 299u, g.R(300), g.R(300), g.R(300), 0xFFFFFFFFu, 0xFFFFFFFEu, 0xFFFFFFF8u, 0xFFFFFFF0u}));
        const u32 extra = g.Pk({0u, 1u, 0x43u, g.R(0x44), g.R(0x44)});
        a[4] = AuxOffset(g, 0x20);
        a[5] = u32(idx * 0x44 + (idx < 0 ? -i32(extra) : i32(extra)));
        a[6] = 0x40u + 4u * g.R((u32(kWin) - 0x40u - 0x300u - 0x40u) / 4u);
        a[7] = FlagsDword(g);
        a[8] = Raw(g); a[9] = Raw(g);
        a[10] = g.Pk({u32(idx - 1), u32(idx), u32(idx + 1), 0u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 300u, g.W(), g.R(400), u32(idx - 1), u32(idx)});
        a[11] = g.Pk({0u, 0x80u, 0x7Fu, 0xFFu, g.R(256), g.R(256)});
        a[12] = g.Pk({300u, 512u, 1000u, g.W() | 0x100u});
        break;
    }
    case kWater: {
        a[4] = g.Pk({0u, 1u, 0u, 1u, 2u, 0x100u, 0xFFu, 0xFFFFFF00u, 0xFFFFFF01u, g.W(), g.W() | 1u, g.W() & ~0xFFu});
        a[5] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0xFFFFFFFEu, 4u, 5u});   // id: normalmente 0..3; fuera de rango lee/escribe memoria vecina de la arena
        a[6] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0x7FFFu, g.R(0x8000), g.R(8)});   // banco: eSoundBankSlot es de 16 bits (la DLL pasa un uint16, el EXE el dword de la pila): solo valores validos
        a[7] = g.Pk({0u, 3u, 1u, 2u, 0x7FFFu, 0x100u, g.R(0x8000), g.R(0x8000), 3u, 0u});   // sfx: identificador de sonido no negativo (un int16 negativo no es un sonido valido: el EXE lo pasa extendido con signo y la DLL como uint16)
        a[8] = FloatBits(g); a[9] = FloatBits(g);
        for (u32 i = 0; i < 3; ++i) a[10 + i] = FloatBits(g);
        a[13] = FloatBits(g);
        a[14] = g.Pk({0u, 1u, 1u, 1u});
        a[15] = AuxOffset(g, kObjSpan);
        a[16] = Raw(g);
        a[17] = FloatBits(g); a[18] = FloatBits(g);
        break;
    }
    case kFurnCtor:
    case kEmpty:
    case kWcDtor:
        break;
    case kTarget: {
        a[4] = 4u * g.R(0x100); a[5] = AuxOffset(g, 0x40);
        a[6] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, g.W(), g.R(8)});
        for (u32 i = 0; i < 3; ++i) { a[7 + i] = FloatBits(g); a[12 + i] = FloatBits(g); a[15 + i] = FloatBits(g); }
        a[10] = g.Pk({0u, 1u, 1u, 1u});
        a[11] = AuxOffset(g, 0x40);
        a[18] = 0x800u + 4u * g.R(0x100);
        if (g.P(15)) {   // doble de la diferencia por encima del maximo de un float pero con suma final finita (x: se guarda como float antes de sumar; y: no): solo la x87 del EXE da estos resultados
            const u32 px = 0x80000000u | (0x7E52A60Au + g.R(0x34C4B3u)), dx = 0x7E8EF0ABu + g.R(0x167BCAu);
            const u32 py = 0x80000000u | (0x7EF0BDC2u + g.R(0x40000u)), dy = 0x7CF0BDC2u + g.R(0x8C097Du);
            a[7] = a[12] = px; a[15] = dx;
            a[8] = a[13] = py; a[16] = dy;
        }
        // solo NaN silenciosos: un NaN senalizante cargado con fld se silencia pero uno en memoria no, y el resultado depende de que operando va en memoria (la x87 elige el QNaN); ningun calculo del juego produce NaN senalizantes
        for (u32 i = 0; i < 3; ++i) { for (u32 k : {7u, 12u, 15u}) if (IsNanBits(a[k + i])) a[k + i] |= 0x00400000u; }
        // dos NaN con el mismo significando dan un resultado dependiente del orden de operandos: se evitan (las sumas y restas con un NaN o con NaN distintos son deterministas)
        for (u32 i = 0; i < 3; ++i) {
            u32& pos = a[10] == 0 ? a[7 + i] : a[12 + i];
            if (IsNanBits(pos) && IsNanBits(a[15 + i]) && QuietSig(pos) == QuietSig(a[15 + i])) pos = 0x7FC12345u;
        }
        break;
    }
    case kPassCtor:
        a[4] = Raw(g); a[5] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, g.W(), g.R(8)}); a[6] = Raw(g);
        a[7] = Raw(g); a[8] = Raw(g);
        break;
    case kSdd:
        a[4] = Raw(g); a[5] = Raw(g); a[6] = FlagsDword(g);
        break;
    case kHitKills: {
        const u32 sign = g.P(25) ? 0x80000000u : 0u;
        a[4] = g.Pk({0x41400000u, 0x41A00000u, 0x41400001u, 0x413FFFFFu, 0x41A00001u, 0x419FFFFFu, 0x41400000u, 0x41A00000u, sign, sign | 0x7F800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u,
                     g.W(), g.W(), sign | (g.W() & 0x7FFFFFu), 0x41000000u, 0x41200000u, 0x41800000u, 0x41F00000u, FloatBits(g), FloatBits(g), sign | PosNormal(g, 120, 140)});
        a[5] = Raw(g); a[6] = isPlayerRet();
        break;
    }
    case kShake: {
        a[4] = g.Pk({0u, 0u, 0u, 1u});
        a[5] = AuxOffset(g, 0x94);
        a[6] = RawNz(g);
        a[7] = g.Pk({0u, 0u, 0u, 1u, 2u});
        if (a[4] != 0 && a[7] == 0) a[7] = 1u;   // el coche crudo nunca es el del jugador (la funcion lo desreferenciaria)
        a[8] = RawNz(g);
        if (a[8] == a[6]) a[8] ^= 0x10u;
        const bool same = a[4] == 0 && a[7] == 0;   // solo entonces la funcion lee la velocidad y la masa del coche
        const auto speed = [&]() -> u32 {
            if (!same) return FloatBits(g);
            const u32 sign = g.P(50) ? 0x80000000u : 0u;
            return g.Pk({0u, 0x80000000u, sign | (g.W() & 0x7FFFFFu), sign | PosNormal(g, 100, 135), sign | PosNormal(g, 120, 130), sign | PosNormal(g, 120, 130), sign | PosNormal(g, 125, 128), sign | PosNormal(g, 60, 160)});
        };
        for (u32 i = 0; i < 3; ++i) a[9 + i] = speed();
        a[12] = same ? g.Pk({PosNormal(g, 110, 150), PosNormal(g, 110, 150), 0x44BB8000u, 0x3F800000u, 0x42C80000u, PosNormal(g, 100, 180), 0x7F7FFFFFu, 0x00800000u, 0x7F800000u}) : FloatBits(g);
        a[13] = g.Pk({0u, 0u, 1u});
        a[14] = AuxOffset(g, 0x40);
        a[15] = RawNz(g);
        a[16] = Raw(g);
        break;
    }
    case kConst:
        a[3] = Raw(g);
        break;
    case kSitDtor:
        a[4] = g.Pk({0u, 1u, 1u, 1u}); a[5] = RawNz(g);
        a[6] = Raw(g); a[7] = Raw(g);
        break;
    case kSitClone:
        a[4] = Raw(g); a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 2u, 0xFFu, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80u, 0xFFFFFFFFu, g.W(), g.W() | 1u, g.W() & ~0xFFu, g.R(4)});
        a[7] = g.Pk({0u, 1u, 1u, 1u}); a[8] = AuxOffset(g, 0x30);
        a[9] = Raw(g); a[10] = Raw(g);
        break;
    case kSitAbort:
        a[4] = g.Pk({0u, 1u, 1u}); a[5] = AuxOffset(g, 0x30);
        a[6] = g.Pk({2u, 2u, 2u, 2u, 0u, 1u, 3u, 4u, 0xFFFFFFFFu, 0x80000000u, 0x00010002u, 0x10002u, g.W(), g.R(8)});
        a[7] = Raw(g); a[8] = Raw(g);
        a[9] = isPlayerRet();
        a[10] = g.Pk({0u, 1u, 2u, 0xFFu, g.R(256)});
        break;
    case kHoldDtor:
        a[4] = g.Pk({0u, 0u, 0u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        a[5] = g.Pk({0u, 1u, 1u, 1u}); a[6] = AuxOffset(g, 0x30);
        a[7] = g.Pk({0u, 0xFFFFFFFFu, 0x400u, 0x800u, 0xC00u, 0xFFFFFBFFu, 0xFFFFF7FFu, g.W(), g.W(), g.W() & ~0xC00u});
        a[8] = g.P(40) ? 0u : RawNz(g);
        a[9] = g.Pk({0xB5D4A0u, 0xB5D4C0u, 0xB5D4A1u, 0xB5D4BFu, 0xB5D500u, 0xB5D480u, 0xB5D000u, 0u, 0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0xB5D4A0u + 0x20u * g.R(0x40), 0xB5D4A0u - 0x20u * g.R(0x40), g.W(), g.W()});
        a[10] = Raw(g); a[11] = Raw(g);
        break;
    case kPadDown:
    case kPadUp: {
        a[4] = Raw(g);
        a[5] = g.Pk({0u, 0u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        a[6] = g.Pk({0u, 0u, 1u, 2u, 0xFFu, 0x80u, g.R(256)});
        for (u32 w = 0; w < 4; ++w) {
            u32 v = 0;
            for (u32 b = 0; b < 4; ++b) v |= g.Pk({0u, 1u, 0u, 1u, 2u, 0xFFu, 0x80u, g.R(256)}) << (8 * b);
            a[7 + w] = v;
        }
        a[11] = g.Pk({0u, 1u, 2u});
        for (u32 i = 0; i < 3; ++i) a[12 + i] = g.Pk({15u, 17u, 15u, 17u, 0u, 1u, 16u, 14u, 18u, 0xFFFFu, 0x8000u, g.R(65536), g.R(65536), 4u, 5u});
        a[15] = AuxOffset(g, 0x10);
        a[16] = Raw(g);
        break;
    }
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
