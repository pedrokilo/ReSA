#pragma once
// Fixtures del banco Fn203A (lote 203A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 23 funciones del EXE (lote 203A: COctTree/COctTreeBase, callbacks de CClothesBuilder, FindAttractorInRange, CVehicleModelInfo/CVehicleStructure y CTaskComplexAvoidEntity; ver reversing/PendienteValidar/203A.md y README.md).
//   idx  direccion  funcion                                                   forma
//    0   0x5A6DB0   COctTree constructor                                      thiscall() `ret`: vtable 867170, +4/+1C/+20/+24 a 0, 8 hijos a -1, BYTE +8 a 0; devuelve this
//    1   0x5A6E60   copia muerta de CPool<T>::Flush                            thiscall() `ret`: con contador (+8, con signo) > 0: BYTE +0x10 != 0 -> delete(+0) y delete(+4); pone +0..+0xC a 0
//    2   0x5A6FC0   COctTree::empty                                           thiscall() `ret`: nivel y sumas a 0; vacia los hijos (recursivo, pool 0xBC12E4); BYTE +8 a 0 al final
//    3   0x5A72B0   copia muerta de CPool<T>::Flush                            igual que 1
//    4   0x5A7350   CPool<COctTree>::New                                      thiscall(pool) `ret`: busca hueco desde cursor+1 (una vuelta); estado: bit7 a 0, id de 7 bits + 1; devuelve datos + idx*0x28
//    5   0x5A73C0   CPool<COctTree>::Delete                                   thiscall(pool, obj) `ret 4`: idx = (obj-datos)/0x28 con signo; estado |= 0x80; cursor = min con signo
//    6   0x5A7490   COctTree destructor con borrado                           thiscall(banderas) `ret 4`: vtable 867170; empty; con bit 0: libera la ranura en el pool 0xBC12E4; devuelve this
//    7   0x5A74F0   COctTree::RemoveChildren                                  thiscall() `ret`: libera los hijos (como empty, sin tocar nivel, sumas ni BYTE +8)
//    8   0x5A75B0   COctTree::InsertTree                                      thiscall(r, g, b) `ret 0Ch`: inserta un color (octante por los bits del nivel ms_level); virtual (ranura 0) al hijo
//    9   0x5A7570   COctTreeBase constructor                                  thiscall() `ret`: como 0 con la vtable 867178 (no toca +0x28 ni +0x2C); devuelve this
//   10   0x5A44A0   CClothesBuilder DestroyTextureCB                          cdecl(tex, dato) `ret`: RwTexDictionaryRemoveTexture; RwTextureDestroy; devuelve la textura
//   11   0x5A47E0   CClothesBuilder GetAtomicWithNameCB                        cdecl(atomic, ctx) `ret`: GetFrameNodeName([atomic+4]); _stricmp(ctx.nombre, nodo) == 0 -> ctx.atomico = atomic, devuelve 0; si no, atomic
//   12   0x6002F0   FindAttractorInRange (CAttractorScanner)                  cdecl(9 args) `ret`: busca el efecto 2D de atractor mas cercano (x87 a 24 bits); devuelve found (AL)
//   13   0x4C75E0   CVehicleModelInfo constructor                             thiscall() `ret`: 4C4A60; +5C, +2D0 (BYTE), +4E (BYTE) a 0; vtable 85C5C8; +304 = -1; 18 mejoras y 4 remaps a -1; devuelve this
//   14   0x4C8E10   CVehicleModelInfo destructor con borrado                   thiscall(banderas) `ret 4`: 4C5920; con bit 0 operator delete (0x8214BD); devuelve this
//   15   0x4C7410   CVehicleStructure destructor                              thiscall() `ret`: contador BYTE +0x30C con signo; para cada extra: marco = [atomico+4]; RpAtomicDestroy; RwFrameDestroy
//   16   0x4C8D60   CVehicleStructure constructor                             thiscall() `ret`: 15 dummies a 0, padres de mejora (+0xD0+0x20*i) a -1, extras (+0x2F4, 6 dwords) a 0, BYTE +0x30C y +0x310 a 0; devuelve this
//   17   0x4C94C0   CPool<CVehicleStructure>::New                              thiscall(pool) `ret`: como 4 con elementos de 0x314
//   18   0x4C9580   CVehicleStructure::operator delete                         cdecl(ptr) `ret`: pool del puntero global 0xB4E680; como 5 con elementos de 0x314
//   19   0x4C9840   CVehicleStructure destructor con borrado                   thiscall(banderas) `ret 4`: 4C7410; con bit 0 libera la ranura en linea (pool 0xB4E680); devuelve this
//   20   0x66AA20   CTaskComplexAvoidEntity constructor                       thiscall(dato, entidad, vector*) `ret 0Ch`: 61A3B0; +10, +0C; vtable 86FF00; copia del vector a +20 y +2C; ceros; RegisterReference al final
//   21   0x66AAD0   CTaskComplexAvoidEntity::GetTaskType                       thiscall() `ret`: eax = 0x3AB
//   22   0x66AAE0   CTaskComplexAvoidEntity cuerpo del destructor              thiscall() `ret`: vtable 86FF00; CleanUpOldReference(+0C) si no es nulo; 61A3D0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados. Una escena (arenas + memoria global) se describe en un guion de escrituras (`s`, ver Script)
// que el banco (C++) y el oraculo (Python) aplican por igual; los punteros a las arenas van simbolicos (P(arena, desplazamiento)) y se resuelven al aplicarlos.
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

namespace F203AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;
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

// Puntero simbolico a una arena: 0xE00 | arena en los 12 bits altos y el desplazamiento (respecto a la ventana, sin la guarda) en los 20 bajos. Ningun valor "crudo" de los fixtures cae en ese rango.
constexpr u32 P(u32 arena, u32 off) { return 0xE0000000u | (arena << 20) | off; }
constexpr bool IsSym(u32 v) { return (v >> 20) >= 0xE00u && (v >> 20) <= 0xE03u; }

// Guion de escrituras de la escena: [op u8][direccion u32][longitud u16][bytes]. op bit 0: 0 = arena (direccion = arena << 24 | desplazamiento de la ventana), 1 = memoria global (direccion absoluta);
// op bit 1: la longitud es 4 y el valor es un puntero (simbolico o crudo) que se resuelve al aplicarlo.
struct Script {
    std::vector<u8> b;
    void Rec(u8 op, u32 addr, const u8* data, u32 len) {
        b.push_back(op);
        for (int i = 0; i < 4; ++i) b.push_back(u8(addr >> (8 * i)));
        b.push_back(u8(len)); b.push_back(u8(len >> 8));
        b.insert(b.end(), data, data + len);
    }
    void Bytes(u32 arena, u32 off, const u8* d, u32 n) { Rec(0, (arena << 24) | off, d, n); }
    void Dw(u32 arena, u32 off, u32 v) { Bytes(arena, off, reinterpret_cast<const u8*>(&v), 4); }
    void W16(u32 arena, u32 off, u32 v) { const u16 w = u16(v); Bytes(arena, off, reinterpret_cast<const u8*>(&w), 2); }
    void B8(u32 arena, u32 off, u32 v) { const u8 w = u8(v); Bytes(arena, off, &w, 1); }
    void Ptr(u32 arena, u32 off, u32 sym) { Rec(2, (arena << 24) | off, reinterpret_cast<const u8*>(&sym), 4); }
    void Str(u32 arena, u32 off, const std::string& s) { Bytes(arena, off, reinterpret_cast<const u8*>(s.c_str()), u32(s.size() + 1)); }
    void GBytes(u32 addr, const u8* d, u32 n) { Rec(1, addr, d, n); }
    void GDw(u32 addr, u32 v) { GBytes(addr, reinterpret_cast<const u8*>(&v), 4); }
    void GPtr(u32 addr, u32 sym) { Rec(3, addr, reinterpret_cast<const u8*>(&sym), 4); }
};

// ------------------------------------------------------------------------------------------------ el caso
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 48> a{};      // parametros de la escena y de los falsos (por funcion, ver abajo)
    std::array<u32, 9> args{};    // argumentos de pila (args[0] arriba); simbolicos permitidos
    u32 nargs{};
    std::vector<u8> s;            // guion de la escena
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool Pc(u32 pct) { return Rand(n, nk()) % 100 < pct; }
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
    default: return g.Pc(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una arena auxiliar, sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span) { return 4 * g.R((u32(kWin) - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca punteros simbolicos)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    u32 v = g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
    if (IsSym(v)) v ^= 0x01000000u;
    if (v >= 0x00C00000u && v < 0x02000000u) v ^= 0x10000000u;   // fuera del rango de la pila del hilo (Tag() lo etiqueta aparte)
    return v;
}
// Un puntero crudo "grande" (nunca simbolico ni en la pila del hilo): base de datos de un pool que el banco no desreferencia
inline u32 BigRaw(Gen& g) { return 0x10000000u + 4 * g.R(0x18000000u / 4); }
// Un float (como bits) "interesante": ceros con signo, unos, infinitos, NaN, denormales, bordes y aleatorios finitos de magnitudes muy distintas
inline u32 FloatBits(Gen& g) {
    const u32 sign = g.Pc(50) ? 0x80000000u : 0u;
    const u32 nanq = sign | 0x7FC00000u | (g.W() & 0x3FFFFFu), nans = sign | 0x7F800000u | ((g.W() & 0x3FFFFFu) | 1u);
    const u32 den = sign | (g.W() & 0x7FFFFFu), nrm = sign | ((1u + g.R(254)) << 23) | (g.W() & 0x7FFFFFu), mid = sign | ((100u + g.R(54)) << 23) | (g.W() & 0x7FFFFFu);
    const u32 one = sign | 0x3F800000u, big = sign | 0x7F7FFFFFu, tiny = sign | 0x00800000u;
    return g.Pk({0u, 0x80000000u, one, sign | 0x7F800000u, nanq, nans, den, nrm, mid, mid, mid, mid, big, tiny, sign | 0x40000000u, sign | 0x3F000000u, g.W() & 0xFF7FFFFFu});
}
// Una coordenada (como bits): casi siempre moderada (el cuadrado de la distancia ronda 1e8) y a veces un valor extremo
inline u32 CoordBits(Gen& g) {
    const u32 k = g.R(10);
    if (k < 7) {
        const float v = float(i32(g.R(40000)) - 20000) / float(1u << g.R(4));
        return std::bit_cast<u32>(v);
    }
    if (k == 7) return 0u;
    return FloatBits(g);
}

// Cadena de prueba: una base (nombres de atractores y de nodos) con las mayusculas cambiadas al azar
inline std::string CaseFlip(Gen& g, const std::string& s) {
    std::string o = s;
    for (auto& ch : o) {
        if (g.Pc(40)) {
            if (ch >= 'a' && ch <= 'z') ch = char(ch - 32);
            else if (ch >= 'A' && ch <= 'Z') ch = char(ch + 32);
        }
    }
    return o;
}
inline std::string NamePick(Gen& g, bool shortOnly) {
    static const char* const shortNames[] = {"ATM", "atm", "Bench", "seat", "stripclb", "drink", "a", "", "SEAT", "bench1", "0", "x_y", "Seat2", "ABCDEFG"};
    static const char* const longNames[] = {"head", "Head", "chest", "Bumper_ok", "wheel_lf", "door_rf", "", "a", "wheel_lb", "hood_dummy", "Seat_front", "TAILLIGHT", "x", "chassis_vlo"};
    if (shortOnly) return shortNames[g.R(14)];
    return longNames[g.R(14)];
}
// Cambia una cadena para que NO sea igual (sin tocar mayusculas): un caracter distinto, un sufijo o un recorte
inline std::string Perturb(Gen& g, const std::string& s, u32 maxLen) {
    std::string o = s;
    const u32 m = g.R(3);
    if (m == 0 && !o.empty()) o[g.R(u32(o.size()))] = char('0' + g.R(10));
    else if (m == 1 && o.size() + 1 <= maxLen) o += char('a' + g.R(26));
    else if (!o.empty()) o.pop_back();
    else o = "z";
    return o;
}

// Funciones (indices)
enum Fn : u32 { kOctCtor, kFlushA, kEmpty, kFlushB, kPoolNewOct, kPoolDelOct, kOctSdd, kRemoveChildren, kInsertTree, kOctBaseCtor, kTexCb, kAtomCb, kFindAttr, kVmiCtor, kVmiSdd, kVsDtor, kVsCtor, kVsPoolNew,
                kVsOpDelete, kVsSdd, kAvCtor, kAvType, kAvDtor };
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x5A6DB0, 0x5A6E60, 0x5A6FC0, 0x5A72B0, 0x5A7350, 0x5A73C0, 0x5A7490, 0x5A74F0, 0x5A75B0, 0x5A7570, 0x5A44A0, 0x5A47E0, 0x6002F0, 0x4C75E0, 0x4C8E10, 0x4C7410,
                                                    0x4C8D60, 0x4C94C0, 0x4C9580, 0x4C9840, 0x66AA20, 0x66AAD0, 0x66AAE0};
// Bytes que limpia el llamador (cdecl: 4 por argumento; `ret n` del callee: 0)
inline constexpr std::array<u32, kFunctions> kPop{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 36, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0};
// Bytes que toca el objeto `this` (desde su puntero); 0 = la funcion no tiene un `this` en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0x28, 0x14, 0x28, 0x14, 0, 0, 0x28, 0x28, 0x28, 0x30, 0, 8, 0, 0x308, 4, 0x314, 0x314, 0, 0, 0, 0x54, 4, 0x54};
// Elemento de los pools
inline constexpr u32 kElemOct = 0x28, kElemVs = 0x314;
// Memoria global del juego tocada
inline constexpr u32 kTreeBase = 0xBC12DC, kTreeBytes = 28;           // ms_bFailed (+0), ms_level (+4), pool de COctTree (+8 .. +0x1B)
inline constexpr u32 kVsPoolPtr = 0xB4E680;
inline constexpr u32 kModelTable = 0xA9B0C8, kModelRegionLo = 0xA9B0B8, kModelRegionBytes = 48;   // CModelInfo::ms_modelInfoPtrs[indice] para indices -4 .. 7
inline constexpr std::array<i32, 8> kModelIdx{-4, -1, 0, 1, 2, 3, 5, 6};

// ------------------------------------------------------------------------------------------------ generador
// Parametros `a` comunes: a[0] = desplazamiento de `this` en la arena A (cuando lo hay), a[1] = tipo de posicion (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[36..39] = semillas de las arenas.
// Parametros de los falsos: a[40] = EAX que devuelve el falso del constructor/cuerpo, a[41] = dword que ese falso deja en [this]; a[42] = EAX del falso de segundo colaborador.
//   kFlushA/B: a[4] = datos, a[5] = estados, a[6] = contador, a[7] = BYTE +0x10, a[8] = cursor (+0xC), a[9] = BYTE +0x11
//   kEmpty/kOctSdd/kRemoveChildren/kInsertTree: a[4] = capacidad, a[5] = desplazamiento de los nodos (C), a[6] = desplazamiento de los estados (D), a[7] = cursor, a[8] = 1 si el puntero de datos del pool es nulo,
//         a[9] = ms_level, a[10] = ms_bFailed, a[11] = indice de la ranura que es `this` (o 0xFFFFFFFF: `this` es el arbol raiz en A)
//   kPoolNewOct/kVsPoolNew: a[4] = desplazamiento del pool en B, a[5] = desplazamiento de los estados en D, a[6] = datos (crudo), a[7] = capacidad, a[8] = cursor
//   kPoolDelOct/kVsOpDelete/kVsSdd: a[4..8] igual; a[9] = indice del nodo, a[10] = delta (con signo) respecto al inicio del nodo, a[11] = 1 si el estado del nodo ya esta libre
//   kVsDtor: a[4] = contador (BYTE), a[5..10] = desplazamientos (C) de los seis atomicos, a[11..16] = marcos (crudos), a[17] = valor que RpAtomicDestroy escribe en [atomico+4] (0 = no escribe), a[18] = numero (1..) de la
//         llamada a RpAtomicDestroy tras la cual el falso cambia el contador (0 = nunca), a[19] = nuevo contador
//   kAtomCb: a[4] = marco (crudo), a[5] = desplazamiento del contexto en B, a[6] = desplazamiento del nodo (nombre del marco) en C, a[7] = desplazamiento del nombre buscado en D, a[8] = atomico previo en el contexto,
//         a[9] = lo que devuelve el falso de GetFrameNodeName (puntero simbolico)
//   kFindAttr: a[4] = cuenta devuelta por FindObjectsInRange (int16), a[5..12] = entidad de cada posicion de la lista, a[13] = numero de la llamada a Get2dEffect que cambia el contador de efectos del modelo (0 = nunca),
//         a[14] = nuevo contador, a[15] = numero de la llamada a Get2dEffect que cambia la cuenta de entidades (0 = nunca), a[16] = nueva cuenta (int16), a[17] = bits de HasEmptySlot, a[18] = mascara de ejes (bit i:
//         el eje i sale de la entidad), a[19] = testigo del gestor, a[20] = relleno de la parte alta del valor de HasEmptySlot
inline u32 StatesRand(Gen& g, u32 pFree) { return g.R(100) < pFree ? (0x80u | g.R(128)) : g.R(128); }

inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[36] = c.seed * 13u + 50u; c.a[37] = c.a[36] + 1u; c.a[38] = c.a[36] + 2u; c.a[39] = c.a[36] + 3u;
    auto& a = c.a;
    Script sc;
    a[0] = kSpan[c.fn] ? Offset(g, kind, kSpan[c.fn]) : 0u;
    a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x867170u, 0x85C5C8u});
    a[40] = Raw(g);
    a[41] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 0x85BB9Cu, 0x867170u});
    a[42] = Raw(g);
    const u32 o = a[0];
    const u32 fam = f / 4;
    switch (c.fn) {
    case kOctCtor:
    case kOctBaseCtor:
    case kVsCtor:
    case kVmiCtor:
        sc.Dw(0, o, a[3]);
        c.ecx = P(0, o);
        break;
    case kFlushA:
    case kFlushB: {
        a[4] = g.Pc(40) ? P(2, AuxOffset(g, 4)) : Raw(g);
        a[5] = g.Pc(40) ? P(3, AuxOffset(g, 4)) : Raw(g);
        a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 2u, g.R(100), u32(-i32(g.R(100))), g.R(5) + 1u});
        a[7] = g.Pk({0u, 1u, 1u, 2u, 0x80u, 0xFFu, g.R(256)});
        a[8] = g.W();
        a[9] = g.R(256);
        sc.Dw(0, o, a[3]);
        sc.Ptr(0, o + 0, a[4]); sc.Ptr(0, o + 4, a[5]); sc.Dw(0, o + 8, a[6]); sc.Dw(0, o + 0xC, a[8]); sc.B8(0, o + 0x10, a[7]); sc.B8(0, o + 0x11, a[9]);
        c.ecx = P(0, o);
        break;
    }
    case kEmpty:
    case kOctSdd:
    case kRemoveChildren:
    case kInsertTree: {
        // Bosque de nodos en el pool de COctTree (global 0xBC12E4): nodos en C, estados en D; los hijos solo apuntan a ranuras de indice MAYOR (sin ciclos) y, en InsertTree, solo a ranuras ocupadas
        const bool insert = c.fn == kInsertTree;
        const u32 cap = g.Pk({1u, 2u, 3u, 4u, 6u, 8u, 12u, 16u, 24u});
        a[4] = cap;
        a[5] = AuxOffset(g, cap * kElemOct);
        a[6] = AuxOffset(g, cap);
        const bool nullData = (c.fn == kEmpty || c.fn == kRemoveChildren) && g.R(24) == 0;
        a[8] = nullData ? 1u : 0u;
        a[7] = insert ? g.Pk({0xFFFFFFFFu, 0u, g.R(cap), g.R(cap), g.R(cap), cap - 1u})
                      : g.Pk({0xFFFFFFFFu, 0u, g.R(cap), u32(-i32(g.R(50))), g.W(), 0x7FFFFFFFu, 0x80000000u, cap, cap - 1u});
        a[9] = insert ? g.Pk({0u, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 7u, 8u, 9u, 12u, 0xFFFFFFF9u, 31u, 32u, 33u, g.W()}) : g.Pk({0u, 1u, 7u, 8u, g.W()});
        a[10] = g.Pk({0u, 0u, 0u, 1u, 2u, 0x80u, g.R(256)});
        const u32 pFree = g.Pk({0u, 10u, 30u, 50u, 80u, 100u});
        std::vector<u8> states(cap);
        for (u32 i = 0; i < cap; ++i) states[i] = u8(StatesRand(g, fam == 0 ? pFree : (fam == 1 ? 0u : (fam == 2 ? 100u : 40u))));
        const u32 density = g.Pk({10u, 40u, 70u, 90u});
        auto children = [&](std::array<i32, 8>& ch, i32 self) {
            for (u32 j = 0; j < 8; ++j) {
                const u32 r = g.R(100);
                i32 v = -1;
                if (r < 50 - density / 2) {
                    v = -1;
                } else if (r < 55 - density / 2) {
                    v = i32(g.Pk({u32(-2), u32(-100), u32(-32768), u32(-16), u32(-5)}));
                } else {
                    std::vector<i32> cand;
                    for (i32 k = self + 1; k < i32(cap); ++k) {
                        const bool occ = (states[u32(k)] & 0x80u) == 0;
                        if (insert && !occ) continue;
                        if (nullData && k != 0 && occ) continue;   // con el puntero de datos nulo solo se puede apuntar a la ranura 0 o a ranuras libres (no se desreferencian)
                        cand.push_back(k);
                    }
                    if (!cand.empty()) v = cand[g.R(u32(cand.size()))];
                }
                ch[j] = v;
            }
        };
        auto fillNode = [&](u8* p, i32 self, bool validVt) {
            const u32 vt = validVt ? 0x867170u : Raw(g);
            const u32 level = g.Pk({0u, 1u, 2u, 3u, 0xFFFFFFFFu, 0x80000000u, g.R(10), g.W()});
            const u32 b8 = g.Pk({0u, 1u, 2u, 0xFFu, g.W() & 0xFFu});
            std::array<i32, 8> ch{};
            children(ch, self);
            std::memcpy(p + 0, &vt, 4); std::memcpy(p + 4, &level, 4);
            p[8] = u8(b8); p[9] = u8(g.W());
            for (u32 j = 0; j < 8; ++j) { const u16 v = u16(ch[j]); std::memcpy(p + 0xA + 2 * j, &v, 2); }
            const u32 noise = g.W(); std::memcpy(p + 0x1A, &noise, 2);
            const u32 rs = g.Pk({0u, g.W(), g.R(256), g.R(2000)}), gs = g.Pk({0u, g.W(), g.R(256), g.R(2000)}), bs = g.Pk({0u, g.W(), g.R(256), g.R(2000)});
            std::memcpy(p + 0x1C, &rs, 4); std::memcpy(p + 0x20, &gs, 4); std::memcpy(p + 0x24, &bs, 4);
        };
        std::vector<u8> nodes(cap * kElemOct);
        for (u32 i = 0; i < cap; ++i) fillNode(nodes.data() + i * kElemOct, i32(i), insert || g.Pc(60));
        u32 thisSlot = 0xFFFFFFFFu;
        if (c.fn == kOctSdd) {
            const u32 flags = g.Pk({0u, 1u, 1u, 1u, 0xFFFFFFFEu, 0xFFFFFFFFu, g.W(), g.W() | 1u, g.W() & ~1u});
            c.args[0] = flags; c.nargs = 1;
            if ((flags & 1u) || g.Pc(35)) thisSlot = g.R(cap);
        }
        a[11] = thisSlot;
        std::array<u8, kElemOct> root{};
        fillNode(root.data(), -1, false);
        if (insert) {
            c.args[0] = g.Pk({0u, 255u, g.R(256), g.R(256), g.W(), 0x80u, 0x7Fu, 1u});
            c.args[1] = g.Pk({0u, 255u, g.R(256), g.R(256), g.W(), 0x80u, 0x7Fu, 2u});
            c.args[2] = g.Pk({0u, 255u, g.R(256), g.R(256), g.W(), 0x80u, 0x7Fu, 4u});
            c.nargs = 3;
        }
        sc.Bytes(3, a[6], states.data(), cap);
        sc.Bytes(2, a[5], nodes.data(), cap * kElemOct);
        if (thisSlot == 0xFFFFFFFFu) sc.Bytes(0, o, root.data(), kElemOct);
        // memoria global: ms_bFailed (+0), ms_level (+4), pool (+8: datos, estados, capacidad, cursor, BYTE +0x10 y +0x11)
        const u32 noise = g.W();
        sc.GDw(kTreeBase + 0, (noise & 0xFFFFFF00u) | (a[10] & 0xFFu));
        sc.GDw(kTreeBase + 4, a[9]);
        if (nullData) sc.GDw(kTreeBase + 8, 0u); else sc.GPtr(kTreeBase + 8, P(2, a[5]));
        sc.GPtr(kTreeBase + 12, P(3, a[6]));
        sc.GDw(kTreeBase + 16, cap);
        sc.GDw(kTreeBase + 20, a[7]);
        sc.GDw(kTreeBase + 24, (g.W() & 0xFFFF0000u) | (((insert || c.fn == kOctSdd) ? 1u : g.R(2)) << 8) | g.R(2));
        c.ecx = thisSlot == 0xFFFFFFFFu ? P(0, o) : P(2, a[5] + thisSlot * kElemOct);
        break;
    }
    case kPoolNewOct:
    case kVsPoolNew: {
        const u32 cap = g.Pk({1u, 2u, 3u, 4u, 7u, 16u, 30u, 64u, 100u, 128u, 300u});
        a[4] = AuxOffset(g, 0x14);
        a[5] = AuxOffset(g, cap);
        a[6] = BigRaw(g);
        a[7] = cap;
        a[8] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0u, g.R(cap), g.R(cap), g.R(cap), cap - 1u, cap - 1u, u32(-2)});
        const u32 pf = g.Pk({0u, 5u, 30u, 50u, 90u, 100u});
        std::vector<u8> states(cap);
        for (u32 i = 0; i < cap; ++i) states[i] = u8(StatesRand(g, pf));
        sc.Bytes(3, a[5], states.data(), cap);
        sc.Dw(1, a[4] + 0, a[6]); sc.Ptr(1, a[4] + 4, P(3, a[5])); sc.Dw(1, a[4] + 8, cap); sc.Dw(1, a[4] + 0xC, a[8]);
        sc.B8(1, a[4] + 0x10, g.R(2)); sc.B8(1, a[4] + 0x11, 1);
        c.ecx = P(1, a[4]);
        break;
    }
    case kPoolDelOct:
    case kVsOpDelete:
    case kVsSdd: {
        const u32 elem = c.fn == kPoolDelOct ? kElemOct : kElemVs;
        const u32 cap = g.Pk({1u, 2u, 3u, 7u, 16u, 30u, 64u, 100u, 128u, 300u});
        a[4] = AuxOffset(g, 0x14);
        a[5] = AuxOffset(g, cap);
        a[6] = BigRaw(g);
        a[7] = cap;
        a[9] = g.Pk({0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap), g.R(cap)});
        const u32 idx = a[9];
        a[10] = g.Pk({0u, 0u, 0u, 0u, 1u, 7u, elem - 1u, u32(-1), u32(-7), u32(-i32(elem - 1)), g.R(elem), u32(-i32(g.R(elem)))});
        a[8] = g.Pk({0xFFFFFFFFu, 0u, idx - 1u, idx, idx + 1u, cap - 1u, cap, cap + 5u, g.R(cap), g.R(cap), 0x7FFFFFFFu, 0x80000000u, 0x80000001u});
        a[11] = g.Pk({0u, 0u, 1u});
        std::vector<u8> states(cap);
        for (u32 i = 0; i < cap; ++i) states[i] = u8(StatesRand(g, 50));
        states[idx] = u8((states[idx] & 0x7Fu) | (a[11] ? 0x80u : 0u));
        sc.Bytes(3, a[5], states.data(), cap);
        sc.Dw(1, a[4] + 0, a[6]); sc.Ptr(1, a[4] + 4, P(3, a[5])); sc.Dw(1, a[4] + 8, cap); sc.Dw(1, a[4] + 0xC, a[8]);
        sc.B8(1, a[4] + 0x10, g.R(2)); sc.B8(1, a[4] + 0x11, g.R(2));
        const u32 obj = a[6] + idx * elem + a[10];
        if (c.fn == kPoolDelOct) {
            c.ecx = P(1, a[4]); c.args[0] = obj; c.nargs = 1;
        } else if (c.fn == kVsOpDelete) {
            sc.GPtr(kVsPoolPtr, P(1, a[4]));
            c.args[0] = obj; c.nargs = 1; c.ecx = g.W();
        } else {
            sc.GPtr(kVsPoolPtr, P(1, a[4]));
            c.args[0] = g.Pk({0u, 1u, 1u, 0xFFFFFFFEu, 0xFFFFFFFFu, g.W(), g.W() | 1u, g.W() & ~1u}); c.nargs = 1; c.ecx = obj;
        }
        break;
    }
    case kVmiSdd: {
        sc.Dw(0, o, a[3]);
        c.args[0] = g.Pk({0u, 1u, 1u, 1u, 0xFFFFFFFEu, 0xFFFFFFFFu, g.W(), g.W() | 1u, g.W() & ~1u}); c.nargs = 1;
        c.ecx = P(0, o);
        break;
    }
    case kVsDtor: {
        a[4] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 6u, 0xFFu, 0x80u, 0xFEu, 0xF0u});   // BYTE con signo: 0x80 = -128, 0xFF = -1
        for (u32 i = 0; i < 6; ++i) { a[5 + i] = 0x10 * g.R(32); const u32 fr = Raw(g); a[11 + i] = fr ? fr : 0xBADF0001u; }   // marco no nulo: el RwFrameDestroy de la DLL comprueba con assert
        a[17] = g.Pk({0u, 0u, 0u, 0xCAFE0000u, g.W() | 1u});
        a[18] = g.Pk({0u, 0u, 0u, 1u, 2u, 3u});
        a[19] = g.Pk({0u, 1u, 2u, 3u, 4u, 0xFFu, 0x80u, 6u});
        sc.Dw(0, o, a[3]);
        for (u32 i = 0; i < 6; ++i) {
            sc.Dw(2, a[5 + i], g.W());
            sc.Dw(2, a[5 + i] + 4, a[11 + i]);
            sc.Ptr(0, o + 0x2F4 + 4 * i, P(2, a[5 + i]));
        }
        sc.B8(0, o + 0x30C, a[4]);
        c.ecx = P(0, o);
        break;
    }
    case kAvCtor: {
        sc.Dw(0, o, a[3]);
        const u32 pos = AuxOffset(g, 0x10);
        a[4] = g.Pk({0u, 0xFFFFFFFFu, g.W(), 1u});
        a[5] = g.Pc(25) ? 0u : Raw(g);
        a[6] = pos;
        sc.Dw(2, pos, CoordBits(g)); sc.Dw(2, pos + 4, CoordBits(g)); sc.Dw(2, pos + 8, CoordBits(g)); sc.Dw(2, pos + 12, g.W());
        c.args[0] = a[4]; c.args[1] = a[5]; c.args[2] = P(2, pos); c.nargs = 3;
        c.ecx = P(0, o);
        break;
    }
    case kAvType:
        sc.Dw(0, o, a[3]);
        c.ecx = g.Pc(70) ? P(0, o) : Raw(g);
        break;
    case kAvDtor: {
        sc.Dw(0, o, a[3]);
        a[4] = g.Pc(30) ? 0u : Raw(g);
        sc.Dw(0, o + 0xC, a[4]);
        c.ecx = P(0, o);
        break;
    }
    case kTexCb:
        c.args[0] = Raw(g); c.args[1] = Raw(g); c.nargs = 2;
        c.ecx = g.W();
        break;
    case kAtomCb: {
        a[4] = Raw(g);
        a[5] = AuxOffset(g, 8);
        a[6] = AuxOffset(g, 16);
        a[7] = AuxOffset(g, 16);
        a[8] = Raw(g);
        a[9] = P(2, a[6]);
        const std::string base = NamePick(g, false);
        const u32 mode = g.R(6);   // 0 igual, 1 igual con otras mayusculas, 2 distinta, 3 distinta con otras mayusculas, 4 sufijo, 5 ambas vacias
        std::string want = base, node = base;
        if (mode == 1) node = CaseFlip(g, base);
        if (mode == 2) node = Perturb(g, base, 14);
        if (mode == 3) node = CaseFlip(g, Perturb(g, base, 14));
        if (mode == 4) node = base + "x";
        if (mode == 5) { want.clear(); node.clear(); }
        sc.Dw(0, o, g.W()); sc.Dw(0, o + 4, a[4]);
        sc.Ptr(1, a[5] + 0, P(3, a[7])); sc.Dw(1, a[5] + 4, a[8]);
        sc.Str(3, a[7], want);
        sc.Str(2, a[6], node);
        c.args[0] = P(0, o); c.args[1] = P(1, a[5]); c.nargs = 2;
        c.ecx = g.W();
        break;
    }
    case kFindAttr: {
        // Entidades (A, 0x100 * j): indice de modelo (u16 en +0x22) y vector del mundo (+0x30). Modelos (B, 0x100 * k): contador (+0xD) y punteros a efectos (+0x20). Efectos (C, 0x80 * e): posicion (+0), tipo (+0xC),
        // tipo de atractor (+0x34) y nombre (+0x38, 8 bytes). Punto (C + 0x600), nombre buscado (D + 0x100).
        const u32 modelSlots = 8;
        std::array<u32, 8> slotModel{};   // objeto de modelo de cada ranura
        for (u32 s = 0; s < modelSlots; ++s) slotModel[s] = g.Pc(75) ? s : g.R(8);
        std::array<u32, 4> entSlot{};     // ranura de modelo de cada entidad
        for (u32 j = 0; j < 4; ++j) entSlot[j] = g.R(modelSlots);
        std::array<std::array<u32, 3>, 6> pos{};
        std::array<u8, 6> typ{}, atype{};
        std::array<std::string, 6> names;
        const u32 boundary = g.R(12);   // 0..1: efectos justo en el limite de 1e8 respecto al punto
        std::array<u32, 3> pt{CoordBits(g), CoordBits(g), CoordBits(g)};
        if (boundary < 2) { for (auto& v : pt) { const float x = float(i32(g.R(2001)) - 1000); v = std::bit_cast<u32>(x); } }
        auto NearTo = [&](u32 base, u32 range) {   // un valor cerca de `base` (mismo orden de magnitud que el punto): casi siempre dentro del radio de 1e4
            if (g.Pc(12)) return CoordBits(g);
            const float bx = std::bit_cast<float>(base);
            if (!(bx > -1.0e6f && bx < 1.0e6f)) return CoordBits(g);
            return std::bit_cast<u32>(bx + float(i32(g.R(2 * range)) - i32(range)) / float(1u << g.R(3)));
        };
        for (u32 e = 0; e < 6; ++e) {
            for (u32 k = 0; k < 3; ++k) pos[e][k] = g.Pc(80) ? NearTo(pt[k], 5000) : CoordBits(g);
            if (boundary < 2) {
                // distancia cuadrada exactamente 1e8 (10000 en un eje; 6000 y 8000), un poco menos o un poco mas (con enteros: exacto)
                const float px = std::bit_cast<float>(pt[0]), py = std::bit_cast<float>(pt[1]), pz = std::bit_cast<float>(pt[2]);
                const u32 m = g.R(8);
                float dx = 0, dy = 0, dz = 0;
                if (m == 0) dx = 10000.0f; else if (m == 1) dy = -10000.0f; else if (m == 2) dz = 10000.0f; else if (m == 3) { dx = 6000.0f; dy = 8000.0f; }
                else if (m == 4) dx = 9999.0f; else if (m == 5) dx = 10001.0f; else if (m == 6) { dx = -6000.0f; dz = 8000.0f; }
                pos[e][0] = std::bit_cast<u32>(px - dx); pos[e][1] = std::bit_cast<u32>(py - dy); pos[e][2] = std::bit_cast<u32>(pz - dz);
            }
            typ[e] = u8(g.Pc(88) ? 3u : g.Pk({0u, 1u, 2u, 4u, 0xFFu}));
            atype[e] = u8(g.Pk({2u, 2u, 2u, 2u, 2u, 2u, 1u, 3u, 0xFFu}));
            names[e] = (e > 0 && g.Pc(50)) ? CaseFlip(g, names[0]) : CaseFlip(g, NamePick(g, true));
        }
        a[4] = g.Pk({0xFFFFu, 0u, 1u, 2u, 3u, 4u, 5u, 6u, 8u, 8u, 3u, 6u, 7u, 4u, 5u, 0xFFFEu});   // int16
        for (u32 i = 0; i < 8; ++i) a[5 + i] = g.R(4);
        a[13] = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 5u});
        a[14] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u});
        a[15] = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 2u, 4u});
        a[16] = g.Pk({0u, 1u, 0xFFFFu, 2u, 0xFFFEu, 8u});
        a[17] = g.W();
        a[18] = boundary < 2 ? 0u : g.R(8);
        a[19] = g.Pk({0u, P(3, 0x200), 0x12345678u, g.W() | 1u});
        a[20] = g.W() & 0xFFFFFF00u;
        std::array<bool, 8> modelDone{};
        for (u32 s = 0; s < modelSlots; ++s) {
            const u32 mo = slotModel[s];
            sc.GPtr(kModelTable + 4 * u32(kModelIdx[s]), P(1, 0x100 * mo));
            if (!modelDone[mo]) {
                modelDone[mo] = true;
                sc.B8(1, 0x100 * mo + 0xD, g.Pk({0u, 1u, 2u, 3u, 3u, 4u, 5u, 6u}));
                for (u32 i = 0; i < 6; ++i) sc.Ptr(1, 0x100 * mo + 0x20 + 4 * i, P(2, 0x80 * g.R(6)));
            }
        }
        for (u32 j = 0; j < 4; ++j) {
            sc.W16(0, 0x100 * j + 0x22, u32(kModelIdx[entSlot[j]]));
            sc.Dw(0, 0x100 * j + 0x30, NearTo(pt[0], 5000));
            sc.Dw(0, 0x100 * j + 0x34, NearTo(pt[1], 5000)); sc.Dw(0, 0x100 * j + 0x38, NearTo(pt[2], 5000));
        }
        for (u32 e = 0; e < 6; ++e) {
            for (u32 k = 0; k < 3; ++k) sc.Dw(2, 0x80 * e + 4 * k, pos[e][k]);
            sc.B8(2, 0x80 * e + 0xC, typ[e]);
            sc.B8(2, 0x80 * e + 0x34, atype[e]);
            sc.Str(2, 0x80 * e + 0x38, names[e].substr(0, 7));
        }
        sc.Dw(2, 0x600, pt[0]); sc.Dw(2, 0x604, pt[1]); sc.Dw(2, 0x608, pt[2]);
        u32 radius = g.Pk({std::bit_cast<u32>(20000.0f), std::bit_cast<u32>(5.0f), 0u, FloatBits(g), CoordBits(g)});
        if ((radius & 0x7F800000u) == 0x7F800000u && (radius & 0x7FFFFFu) != 0) radius |= 0x400000u;   // NaN siempre silencioso: la DLL recibe el radio como float y la x87 silencia los NaN senalizantes (el radio real no es un NaN)
        u32 modelArg = 0xFFFFFFFFu;
        const u32 mk = g.R(12);
        if (mk >= 5 && mk < 9) modelArg = u32(kModelIdx[g.R(8)]);
        else if (mk == 9) modelArg = u32(kModelIdx[g.R(8)]) & 0xFFFFu;   // sin extension de signo: no coincide con los negativos
        else if (mk == 10) modelArg = g.Pk({0x10000u, 0x7FFFu, 0x80000000u, 9u, 100u});
        u32 typeArg = g.Pc(80) ? 2u : atype[g.R(6)];
        if (g.Pc(15)) typeArg = g.Pk({4u, 0x100u | typeArg, 0xFFFFFFFFu, g.W()});
        u32 nameArg = 0;
        const u32 nm = g.R(10);
        if (nm >= 5) {
            const std::string want = nm < 9 ? CaseFlip(g, names[g.Pc(60) ? 0 : g.R(6)].substr(0, 7)) : Perturb(g, names[g.R(6)].substr(0, 7), 7);
            sc.Str(3, 0x100, want);
            nameArg = P(3, 0x100);
        }
        const u32 reqArg = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 0x100u, 0x101u, 0xFFu, g.W()});
        u32 excl = 0;
        const u32 ex = g.R(10);
        if (ex >= 3 && ex < 9) excl = P(2, 0x80 * g.R(6));
        else if (ex == 9) excl = Raw(g);
        sc.Dw(1, 0x800, g.W()); sc.Dw(1, 0x804, g.W());
        c.args = {P(2, 0x600), radius, modelArg, typeArg, nameArg, reqArg, excl, P(1, 0x800), P(1, 0x804)};
        c.nargs = 9;
        c.ecx = g.W();
        break;
    }
    default:
        break;
    }
    c.s = std::move(sc.b);
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
    j.U("fn", c.fn).U("n", c.n).U("family", c.family).U("seed", c.seed).U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack", c.stack).U("nargs", c.nargs);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.A("args", std::vector<u32>(c.args.begin(), c.args.end()));
    j.H("s", c.s);
    return j.End();
}
}
