#pragma once
// Fixtures del banco Fn191A (lote 191A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 33 reconstruidas (31 del encargo + Clone y CreateNextSubTask).
//   idx  direccion  nombre real                                                                         forma
//    0   0x66BB90   CTaskComplexGotoDoorAndOpen::GetTaskType (V, ranura 4)                              thiscall() `ret`: mov eax,3A4h
//    1   0x66BC80   CTaskComplexGotoDoorAndOpen::MakeAbortable (V, ranura 6)                            thiscall(ped, prioridad, evento) `ret 0Ch`: solo con prioridad 2: salta a MakeAbortable de la subtarea (+8) sin guarda de nulo
//    2   0x66C280   funcion muerta (tarea sin identificar con CEntryExit* en +0xC)                       thiscall(sin uso) `ret 4`: centro del rectangulo (x87), CTask::operator new(0x30), CTaskSimpleGoToPoint 0x667CD0
//    3   0x66C3E0   CTaskSimpleSlideToCoord constructor sin animacion                                   thiscall(pos*, rot, vel) `ret 0Ch`
//    4   0x66C450   CTaskSimpleSlideToCoord constructor con animacion                                   thiscall(pos*, rot, vel, nombre*, grupo*, banderas, mezcla, enSecuencia, tiempo) `ret 24h`
//    5   0x66C4C0   CTaskSimpleSlideToCoord cuerpo del destructor                                        thiscall() `ret`: [this]=86FFECh; jmp 0x61A6F0 (cuerpo del destructor de CTaskSimpleAnim)
//    6   0x619440   RsCameraShowRaster                                                                  cdecl(camara) `ret`: jmp 0x745240 (psCameraShowRaster)
//    7   0x619780   RsSetPresetView                                                                     cdecl(camara, vista) `ret`: avanza por la lista de vistas; 6 funciones RW; devuelve 1/0 en EAX completo
//    8   0x6198C0   RsDestroyPresetViews                                                                cdecl() `ret`: libera descripciones y nodos con CMemoryMgr::Free (0x72F430); lista y contador a cero
//    9   0x43EEB0   CPool<CEntryExit> cuerpo del destructor (copia 1)                                   thiscall() `ret`
//   10   0x43EF00   CEntryExitManager::GetInSlot                                                        cdecl(slot) `ret`
//   11   0x43EF20   CEntryExitManager::SetEntryExitFlagWithName                                         cdecl(nombre, mascara, activar) `ret`
//   12   0x43EF90   CEntryExitManager::SetEntryExitFlagWithIndex                                        cdecl(indice, mascara, activar) `ret`
//   13   0x43F720   CPool<CEntryExit> constructor                                                       thiscall(capacidad, nombre) `ret 8`
//   14   0x43F780   CPool<CEntryExit> cuerpo del destructor (copia 2)                                   thiscall() `ret`
//   15   0x43F7D0   CPool<CEntryExit>::New                                                              thiscall() `ret`
//   16   0x43F950   CPool<CEntryExit> destructor con borrado                                            thiscall(banderas) `ret 4`
//   17   0x43F9B0   CEntryExitManager::SetEnabledByName                                                 cdecl(nombre, activar) `ret`: SetEntryExitFlagWithName(nombre, 0x4000, activar)
//   18   0x4410E0   CPad::IsDPadRightPressed                                                            thiscall() `ret`: palabras +0x16 y +0x46
//   19   0x441130   CPhysical::SetVelocity                                                              thiscall(x, y, z) `ret 0Ch`
//   20   0x6786F0   CTaskComplexFallAndGetUp::GetTaskType (V, ranura 4)                                 thiscall() `ret`: mov eax,0D0h
//   21   0x678790   CTaskComplexFallAndGetUp cuerpo del destructor                                      thiscall() `ret`: [this]=870454h; jmp 0x61A3D0
//   22   0x6789D0   CTaskComplexFallAndStayDown constructor (animacion, grupo)                           thiscall(anim, grupo) `ret 8`
//   23   0x678A00   CTaskComplexFallAndStayDown::GetTaskType (V, ranura 4)                               thiscall() `ret`: mov eax,0D1h
//   24   0x678A10   CTaskComplexFallAndStayDown constructor (direccion)                                  thiscall(direccion) `ret 4`: tabla de saltos 0x678A80
//   25   0x678A90   CTaskComplexFallAndStayDown cuerpo del destructor                                   thiscall() `ret`: [this]=870480h; jmp 0x61A3D0
//   26   0x678AA0   CTaskComplexFallAndStayDown::MakeAbortable (V, ranura 6)                             thiscall(ped, prioridad, evento) `ret 0Ch`
//   27   0x678B20   CTaskComplexFallAndStayDown::CreateFirstSubTask (V, ranura 9)                        thiscall(ped) `ret 4`: new(0x20) + CTaskSimpleFall
//   28   0x678BC0   CTaskComplexFallAndStayDown::ControlSubTask (V, ranura 10)                           thiscall(ped) `ret 4`: [this+8]
//   29   0x678BD0   CTaskComplexFallAndStayDown::CreateSubTask                                           thiscall(tipo) `ret 4`
//   30   0x67CBC0   CTaskComplexFallAndStayDown destructor con borrado                                   thiscall(banderas) `ret 4`
//   31   0x67C270   CTaskComplexFallAndStayDown::Clone (V, ranura 1)                                     thiscall() `ret`: new(0x14) + base 0x61A3B0
//   32   0x67CBE0   CTaskComplexFallAndStayDown::CreateNextSubTask (V, ranura 8)                         thiscall(ped) `ret 4`
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
// Familia = n % 16: tipo = familia % 4 (posicion del objeto: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F191AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 33;
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
// Mitad baja (0..0x7FC) o alta (0x800..) de una arena auxiliar, para colocar dos objetos disjuntos en la misma arena
inline u32 AuxLow(Gen& g, u32 span) { return 4 * g.R((0x800u - span) / 4 + 1); }
inline u32 AuxHigh(Gen& g, u32 span) { return 0x800u + 4 * g.R((0x800u - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const auto avoidStack = [](u32 v) { return (v >= 0x00080000u && v < 0x00400000u) ? (v | 0x80000000u) : v; };   // la pila del hilo esta en esa zona: Tag() la anota aparte
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = avoidStack(g.W()), w6 = avoidStack(g.W()), w7 = avoidStack(g.W());
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un dword no nulo
inline u32 RawNZ(Gen& g) { u32 v = Raw(g); return v ? v : 0x1234567u; }
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN (silenciosos y senalizantes, con cargas distintas), denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.P(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
// Un float sin NaN
inline u32 FloatNoNan(Gen& g) { u32 v = FloatBits(g); while (IsNanBits(v)) v = FloatBits(g); return v; }

// Texto de las cadenas de los casos (nombres de animacion y de entrada/salida): letras y digitos ASCII, igual que el oraculo
inline constexpr char kAlphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
inline constexpr u32 kAlphabetLen = 63;
inline char StrChar(u32 seed, u32 i) { return kAlphabet[Rand(seed, 100 + i) % kAlphabetLen]; }
// 8 bytes del nombre de la entrada i (CEntryExit +0): variante segun la semilla; se compara con el nombre buscado (name[0..len)) por _strnicmp(.., 8)
inline std::array<u8, 8> EntryName(u32 seed, u32 i, u32 nameSeed, u32 nameLen) {
    std::array<u8, 8> out{};
    const u32 variant = Rand(seed, 10 + i) % 6;
    const auto flip = [&](u32 j, char c) -> u8 {
        const bool lowerable = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        if (lowerable && (Rand(seed, 100 + i * 8 + j) & 1)) return u8(c ^ 0x20);
        return u8(c);
    };
    const u32 m = nameLen < 8 ? nameLen : 8;
    if (nameLen == 0 || variant == 4) {
        for (u32 j = 0; j < 8; ++j) out[j] = u8(StrChar(seed + 7 + i, j));
        return out;
    }
    for (u32 j = 0; j < 8; ++j) out[j] = j < m ? flip(j, StrChar(nameSeed, j)) : u8(0);
    if (variant == 2) {
        const u32 p = Rand(seed, 40 + i) % m;
        out[p] = u8(StrChar(nameSeed, p) == 'z' ? 'y' : 'z');
    } else if (variant == 3) {
        const u32 k = Rand(seed, 50 + i) % 8;
        for (u32 j = 0; j < 8; ++j) out[j] = j < k ? out[j] : (j == k ? u8(0) : u8(StrChar(seed + 99, j)));
        if (k >= m) for (u32 j = m; j < k; ++j) out[j] = u8(StrChar(seed + 13, j));
    }
    return out;
}

// Clases de funcion (forma)
enum Kind : u32 { kConst, kGotoAbort, kCenter, kSlide0, kSlide1, kSlideDtor, kRsShow, kRsSet, kRsDestroy, kPoolDtor, kGetSlot, kFlagName, kFlagIndex, kPoolCtor, kPoolNew, kPoolSdd, kSetEnabled, kPadRight,
                  kSetVel, kComplexDtor, kSdCtor1, kSdCtor2, kSdAbort, kSdFirst, kSdControl, kSdCreateSub, kSdSdd, kSdClone, kSdNext };
inline constexpr std::array<Kind, kFunctions> kKind{kConst, kGotoAbort, kCenter, kSlide0, kSlide1, kSlideDtor, kRsShow, kRsSet, kRsDestroy, kPoolDtor, kGetSlot, kFlagName, kFlagIndex, kPoolCtor, kPoolDtor, kPoolNew,
                                                    kPoolSdd, kSetEnabled, kPadRight, kSetVel, kConst, kComplexDtor, kSdCtor1, kConst, kSdCtor2, kComplexDtor, kSdAbort, kSdFirst, kSdControl, kSdCreateSub, kSdSdd, kSdClone, kSdNext};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x66BB90, 0x66BC80, 0x66C280, 0x66C3E0, 0x66C450, 0x66C4C0, 0x619440, 0x619780, 0x6198C0, 0x43EEB0, 0x43EF00, 0x43EF20, 0x43EF90, 0x43F720, 0x43F780, 0x43F7D0,
                                                    0x43F950, 0x43F9B0, 0x4410E0, 0x441130, 0x6786F0, 0x678790, 0x6789D0, 0x678A00, 0x678A10, 0x678A90, 0x678AA0, 0x678B20, 0x678BC0, 0x678BD0, 0x67CBC0, 0x67C270, 0x67CBE0};
// Valor que devuelven las funciones constantes (por indice; 0 si no es constante)
inline constexpr std::array<u32, kFunctions> kConstValue{0x3A4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xD0, 0, 0, 0xD1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
// Bytes que cada funcion toca a partir de su puntero `this` (o del primer objeto)
inline constexpr std::array<u32, kFunctions> kSpan{4, 0x10, 0x10, 0x80, 0x80, 0x80, 4, 4, 4, 0x14, 4, 4, 4, 0x14, 0x14, 0x14, 0x14, 4, 0x50, 0x50, 4, 0x14, 0x14, 4, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14};
// Memoria global del juego tocada
inline constexpr u32 kPoolPtr = 0x96A7D8, kViewList = 0xC1707C, kViewCount = 0xC17080, kViewCurrent = 0x8D2E30;
// Vtable falsa de las subtareas (arena C): ranura 0 destructor con borrado, 4 GetTaskType, 6 MakeAbortable
inline constexpr u32 kFakeVtableSpan = 0x20;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` (o del primer objeto) en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [a[0]], a[36..39] = semillas de las arenas A, B, C y D
//   kConst: nada mas
//   kGotoAbort, kSdAbort: a[4] = ped (pila, crudo), a[5] = prioridad (pila), a[6] = evento (pila, crudo), a[7] = EAX del MakeAbortable falso, a[8] = desplazamiento de la subtarea en B, a[9] = desplazamiento de la vtable falsa en C,
//         a[10] = EAX del GetTaskType falso, a[11] = modo de la subtarea (+8): 0 nula, 1 valida (en B), 2 cruda (a[12]); a[12] = puntero crudo
//   kCenter: a[4] = desplazamiento del CEntryExit en B (+8..+0x18 = a[5..9]), a[10] = memoria de operator new (0 nula, 1 en C), a[11] = desplazamiento en C, a[12] = argumento de pila (crudo)
//   kSlide0: a[4] = desplazamiento de la posicion en B, a[5..7] = posicion, a[8] = rotacion, a[9] = velocidad
//   kSlide1: ademas de lo anterior: a[10] = semilla del nombre, a[11] = longitud, a[12] = desplazamiento en C, a[13] = semilla del grupo, a[14] = longitud, a[15] = desplazamiento en D, a[16] = banderas, a[17] = mezcla, a[18] = enSecuencia (dword de pila),
//         a[19] = tiempo, a[20] = resultado del falso de GetAnimationBlock, a[21] = resultado del falso de GetAnimation
//   kSlideDtor: a[4] = dword en +0xC, a[5] = modo de la animacion (+8: 0 nula, 1 en B), a[6] = desplazamiento en B, a[7] = m_BlendAmount, a[8] = m_BlendDelta, a[9] = dword en anim+0x2C
//   kRsShow: a[4] = camara (pila, cruda), a[5] = EAX del falso de psCameraShowRaster
//   kRsSet: a[4] = camara (0 nula, 1 en C), a[5] = desplazamiento en C, a[6] = fotograma (0 en D (a[7]), si no crudo), a[7] = desplazamiento en D, a[8] = numero de vistas, a[9] = vista, a[10] = desplazamiento de la lista en B,
//         a[11..17] = tx, ty, tz, rotX, rotY, cercano, lejano del nodo elegido, a[18] = descripcion cruda, a[19] = vista actual previa (0x8D2E30)
//   kRsDestroy: a[4] = nodos (0..5), a[5] = desplazamiento de la lista en B, a[6] = mascara de descripciones no nulas, a[7] = contador previo, a[8..12] = descripciones crudas
//   kPoolDtor: a[4] = capacidad, a[5] = dword en +0x10 (el byte bajo es el propietario), a[6] = dword +0, a[7] = dword +4, a[8] = dword +0xC
//   kGetSlot: a[4] = desplazamiento del pool en B, a[5] = almacenamiento (0 en C (a[6]), 1 crudo (a[7])), a[6] = desplazamiento en C, a[7] = crudo, a[8] = desplazamiento de los estados en D, a[9] = slot, a[10] = capacidad,
//         a[11] = puntero previo (0x96A7D8), a[12] = byte de estado del slot
//   kFlagName, kFlagIndex: a[4] = desplazamiento del pool en B (mitad baja), a[5] = desplazamiento del almacenamiento en C, a[6] = desplazamiento de los estados en D, a[7] = capacidad, a[8] = semilla del nombre, a[9] = longitud del nombre,
//         a[10] = semilla de las entradas, a[11] = desplazamiento del nombre en B (mitad alta), a[12] = semilla de los estados, a[13] = indice (kFlagIndex), a[14] = mascara, a[15] = activar (dword de pila), a[16] = puntero previo
//   kPoolCtor: a[4] = capacidad, a[5] = nombre (pila, crudo), a[6] = almacenamiento (0 en C (a[7]), 1 crudo (a[8])), a[7] = desplazamiento en C, a[8] = crudo, a[9] = desplazamiento de los estados en D
//   kPoolNew: a[4] = capacidad (>= 1), a[5] = ultimo hueco (-1..cap-1), a[6] = desplazamiento de los estados en D, a[7] = semilla de los estados, a[8] = porcentaje de huecos libres, a[9] = almacenamiento (0 en C (a[10]), 1 crudo (a[11])),
//         a[10] = desplazamiento en C, a[11] = crudo
//   kPoolSdd: como kPoolDtor mas a[9] = banderas (pila)
//   kSetEnabled: a[4] = nombre (0 en B (a[5]), 1 crudo (a[6])), a[5] = desplazamiento en B, a[6] = crudo, a[7] = activar (dword de pila), a[8] = EAX del falso
//   kPadRight: a[4] = dword en +0x14 (la palabra +0x16 es la mitad alta), a[5] = dword en +0x44 (la palabra +0x46 es la mitad alta)
//   kSetVel: a[4..6] = x, y, z (pila)
//   kComplexDtor: a[4] = subtarea (0 nula, 1 en B), a[5] = desplazamiento en B, a[6] = desplazamiento de la vtable falsa en C, a[7] = EAX del destructor falso
//   kSdCtor1: a[4] = animacion, a[5] = grupo; kSdCtor2: a[4] = direccion
//   kSdFirst: a[4] = ped (pila, crudo), a[5] = animacion (+0xC), a[6] = grupo (+0x10), a[7] = memoria (0 nula, 1 en B), a[8] = desplazamiento en B
//   kSdControl: a[4] = ped (pila, crudo), a[5] = dword en +8
//   kSdCreateSub: a[4] = tipo (pila), a[5] = animacion, a[6] = grupo, a[7] = memoria, a[8] = desplazamiento en B
//   kSdSdd: a[4] = banderas (pila), a[5] = EAX del falso del cuerpo, a[6] = dword que el falso deja en [this]
//   kSdClone: a[4] = animacion, a[5] = grupo, a[6] = memoria, a[7] = desplazamiento en B
//   kSdNext: a[4] = ped (pila, crudo), a[5] = desplazamiento de la subtarea en B, a[6] = desplazamiento de la vtable falsa en C, a[7] = EAX del GetTaskType falso, a[8] = EAX del falso de CreateSubTask
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x86D48Cu, 0x870480u, 0x870454u});
    auto& a = c.a;
    const auto prio = [&] { return g.Pk({0u, 1u, 1u, 2u, 2u, 2u, 3u, 0xFFFFFFFFu, 0x80000002u, 0x80000001u, g.W()}); };
    const auto taskType = [&] { return g.Pk({0xCFu, 0xCFu, 0xCFu, 0xD1u, 0u, 0xCEu, 0x516u, g.W(), 0x389u}); };
    switch (kKind[c.fn]) {
    case kConst:
        break;
    case kGotoAbort:
    case kSdAbort:
        a[4] = Raw(g); a[5] = prio(); a[6] = Raw(g); a[7] = (g.W() & 0xFFFFFF00u) | g.R(2);   // bool devuelto por MakeAbortable: AL = 0 o 1 (los llamadores reales nunca devuelven otro valor), EAX alto basura
        a[8] = AuxOffset(g, 4); a[9] = AuxOffset(g, kFakeVtableSpan); a[10] = taskType();
        a[11] = kKind[c.fn] == kSdAbort ? 1u : (a[5] == 2u ? 1u : g.Pk({0u, 1u, 1u, 2u}));
        a[12] = RawNZ(g);
        break;
    case kCenter:
        a[4] = AuxOffset(g, 0x20);
        for (u32 i = 5; i <= 8; ++i) a[i] = FloatBits(g);
        a[9] = g.W();
        if (IsNanBits(a[7]) && IsNanBits(a[5])) a[7] = FloatNoNan(g);           // una suma con dos NaN: la x87 elige el de mayor mantisa (el oraculo no lo modela)
        if (IsNanBits(a[8]) && IsNanBits(a[6])) a[8] = FloatNoNan(g);
        a[10] = g.Pk({0u, 1u, 1u, 1u});
        a[11] = AuxOffset(g, 0x30);
        a[12] = Raw(g);
        break;
    case kSlide0:
    case kSlide1:
        a[4] = AuxOffset(g, 0xC);
        for (u32 i = 5; i <= 7; ++i) a[i] = FloatBits(g);
        a[8] = FloatBits(g); a[9] = FloatBits(g);
        if (kKind[c.fn] == kSlide1) {
            a[10] = g.W(); a[11] = g.Pk({0u, 1u, 3u, 5u, 8u, 10u, 12u, 15u, 20u, 23u, 24u, 25u, 27u, 30u, 0x2Bu, 0x30u});
            a[12] = AuxOffset(g, 0x40);
            a[13] = g.W(); a[14] = g.Pk({0u, 1u, 3u, 4u, 7u, 8u, 12u, 15u, 16u, 17u, 19u, 20u, 21u, 24u, 0x28u});
            a[15] = AuxOffset(g, 0x40);
            a[16] = g.W(); a[17] = FloatBits(g);
            a[18] = (g.W() & 0xFFFFFF00u) | g.R(2);
            a[19] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
            a[20] = g.Pk({0u, 0u, 1u, Raw(g), Raw(g)});
            a[21] = g.Pk({0u, 1u, Raw(g), Raw(g)});
        }
        break;
    case kSlideDtor:
        a[4] = g.W();
        a[5] = g.Pk({0u, 1u, 1u, 1u});
        a[6] = AuxOffset(g, 0x40);
        a[7] = FloatBits(g); a[8] = FloatBits(g);
        a[9] = g.W();
        break;
    case kRsShow:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kRsSet: {
        a[4] = g.Pk({0u, 1u, 1u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = g.Pk({0u, 0u, 0u, Raw(g)});
        a[7] = AuxOffset(g, 0x10);
        a[8] = g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 5u, 6u, 0xFFFFFFFFu});
        const u32 nn = a[8];
        a[9] = g.Pk({0u, nn - 1u, g.R(nn ? nn : 1u), g.R(nn ? nn : 1u), g.R(nn ? nn : 1u), nn, nn + 1u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 1000u});
        a[10] = 4 * g.R(0xE00u / 4);
        for (u32 i = 11; i <= 17; ++i) a[i] = FloatBits(g);
        a[18] = Raw(g); a[19] = Raw(g);
        break;
    }
    case kRsDestroy:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u});
        a[5] = 4 * g.R(0xE00u / 4);
        a[6] = g.W() & 31u;
        a[7] = Raw(g);
        for (u32 i = 8; i <= 12; ++i) a[i] = Raw(g);
        break;
    case kPoolDtor:
    case kPoolSdd:
        a[4] = g.Pk({0xFFFFFFFBu, 0xFFFFFFFFu, 0u, 1u, 2u, 5u, 100u, 0x7FFFFFFFu, 0x80000000u, g.W()});
        a[5] = g.Pk({0u, 1u, 1u, 0x80u, 0xFFu, g.W(), (g.W() & 0xFFFFFF00u) | 1u});
        a[6] = Raw(g); a[7] = Raw(g); a[8] = Raw(g);
        if (kKind[c.fn] == kPoolSdd) a[9] = g.Pk({0u, 1u, 1u, 3u, 0xFFFFFFFEu, 0x100u, 0x101u, g.W()});
        break;
    case kGetSlot: {
        a[4] = AuxLow(g, 0x14);
        a[5] = g.Pk({0u, 0u, 1u});
        a[6] = AuxOffset(g, 0x3C * 8);
        a[7] = Raw(g);
        a[8] = 8u + AuxOffset(g, 0x100);
        const u32 cap = g.Pk({1u, 2u, 5u, 8u});
        a[9] = g.Pk({0u, 1u, cap - 1u, g.R(cap), g.R(cap), g.R(cap), 7u, 0xFFFFFFFFu, 0xFFFFFFF8u, 20u, 0x80u});
        a[10] = cap;
        a[11] = g.W();
        a[12] = g.Pk({0u, 0x80u, 0x7Fu, 0xFFu, 0x01u, g.W() & 0xFFu});
        break;
    }
    case kFlagName:
    case kFlagIndex: {
        a[4] = AuxLow(g, 0x14);
        a[5] = AuxOffset(g, 0x3C * 12);
        a[6] = AuxOffset(g, 0x20);
        const u32 cap = g.Pk({0u, 1u, 2u, 3u, 5u, 8u, 12u, 12u});
        a[7] = cap;
        a[8] = g.W();
        a[9] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 8u, 9u, 12u});
        a[10] = g.W();
        a[11] = AuxHigh(g, 0x10);
        a[12] = g.W();
        a[13] = cap ? g.R(cap) : 0u;
        a[14] = g.Pk({0x4000u, 0x4000u, 1u, 0xFFFFu, 0xFFFFFFFFu, 0x10000u, 0x8001u, g.W(), g.W()});
        a[15] = (g.W() & 0xFFFFFF00u) | g.R(2);
        a[16] = g.W();
        break;
    }
    case kPoolCtor:
        a[4] = g.Pk({0xFFFFFFFDu, 0xFFFFFFFFu, 0u, 1u, 2u, 3u, 7u, 16u, 30u, 64u, 100u, 150u, 200u, 0x80000000u, 0xF0000000u});
        a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 1u}); a[7] = AuxOffset(g, 0x40); a[8] = Raw(g);
        a[9] = AuxOffset(g, 0x300);
        break;
    case kPoolNew: {
        const u32 cap = g.Pk({1u, 2u, 3u, 4u, 7u, 16u, 30u, 64u, 100u, 150u});
        a[4] = cap;
        a[5] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap)});
        a[6] = AuxOffset(g, 0x100);
        a[7] = g.W();
        a[8] = g.Pk({0u, 2u, 10u, 30u, 60u, 100u});
        a[9] = g.Pk({0u, 1u, 1u});
        a[10] = AuxOffset(g, 0x40);
        a[11] = Raw(g);
        break;
    }
    case kSetEnabled:
        a[4] = g.Pk({0u, 0u, 1u});
        a[5] = AuxOffset(g, 0x10);
        a[6] = Raw(g);
        a[7] = (g.W() & 0xFFFFFF00u) | g.R(2);
        a[8] = g.Pk({0u, 1u, Raw(g), Raw(g)});
        break;
    case kPadRight: {
        const auto word = [&] { return g.Pk({0u, 0u, 1u, 0xFFFFu, 0x8000u, g.R(0x10000), g.R(0x10000)}); };
        a[4] = (word() << 16) | (g.W() & 0xFFFFu);
        a[5] = (word() << 16) | (g.W() & 0xFFFFu);
        break;
    }
    case kSetVel:
        a[4] = FloatBits(g); a[5] = FloatBits(g); a[6] = FloatBits(g);
        break;
    case kComplexDtor:
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, 4);
        a[6] = AuxOffset(g, kFakeVtableSpan);
        a[7] = g.Pk({0u, Raw(g)});
        break;
    case kSdCtor1:
        a[4] = g.Pk({0u, 0x18u, 0x19u, 0x1Au, 0x1Bu, g.W(), 0xFFFFFFFFu}); a[5] = g.Pk({0u, 1u, 2u, 3u, g.W(), 0xFFFFFFFFu});
        break;
    case kSdCtor2:
        a[4] = g.Pk({0u, 1u, 2u, 3u, 0u, 1u, 2u, 3u, 4u, 5u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), 0x100u});
        break;
    case kSdFirst:
        a[4] = Raw(g); a[5] = g.Pk({0u, 0x18u, 0x19u, 0x1Au, 0x1Bu, g.W()}); a[6] = g.Pk({0u, 1u, g.W(), 0xFFFFFFFFu});
        a[7] = g.Pk({0u, 1u, 1u, 1u}); a[8] = AuxOffset(g, 0x20);
        break;
    case kSdControl:
        a[4] = Raw(g); a[5] = Raw(g);
        break;
    case kSdCreateSub:
        a[4] = taskType();
        a[5] = g.Pk({0u, 0x18u, 0x19u, 0x1Au, 0x1Bu, g.W()}); a[6] = g.Pk({0u, 1u, g.W(), 0xFFFFFFFFu});
        a[7] = g.Pk({0u, 1u, 1u, 1u}); a[8] = AuxOffset(g, 0x20);
        break;
    case kSdSdd:
        a[4] = g.Pk({0u, 1u, 1u, 3u, 0xFFFFFFFEu, 0x100u, 0x101u, g.W()});
        a[5] = g.Pk({0u, Raw(g), Raw(g)}); a[6] = g.Pk({0u, g.W(), 0x870480u, 0x86D4CCu});
        break;
    case kSdClone:
        a[4] = g.Pk({0u, 0x18u, 0x19u, 0x1Au, 0x1Bu, g.W()}); a[5] = g.Pk({0u, 1u, g.W(), 0xFFFFFFFFu});
        a[6] = g.Pk({0u, 1u, 1u, 1u}); a[7] = AuxOffset(g, 0x14);
        break;
    case kSdNext:
        a[4] = Raw(g); a[5] = AuxOffset(g, 4); a[6] = AuxOffset(g, kFakeVtableSpan);
        a[7] = taskType(); a[8] = g.Pk({0u, 1u, Raw(g), Raw(g)});
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
