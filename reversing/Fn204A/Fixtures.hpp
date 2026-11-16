#pragma once
// Fixtures del banco Fn204A (lote 204A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 28 funciones del EXE (lote 204A: CTaskComplexAvoidEntity (SetUpIK, QuitIK, MakeAbortable, CreateNextSubTask, ControlSubTask, Clone, CreateFirstSubTask), CTaskInteriorSitInChair (destructor con borrado, ProcessPed),
// CTaskInteriorLieInBed (destructor con borrado), cuerpos de destructor de CVehicleScanner, CPedScanner y CObjectScanner, constructor de CObjectScanner, escaner de colisiones con peatones 0x606580,
// CPedAcquaintanceScanner::CreateAcquaintanceEvent 0x606BA0, cuerpo de destructor de CEventDeath, CEventPotentialGetRunOver::AffectsPed y diez destructores con borrado de eventos; ver
// reversing/PendienteValidar/204A.md y README.md).
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B, C y D, guardas de 0x40) y los colaboradores falseados. Una escena (arenas + memoria global) se describe en un guion de escrituras (`s`, ver Script)
// que el banco (C++) y el oraculo (Python) aplican por igual; los punteros a las arenas van simbolicos (P(arena, desplazamiento)) y se resuelven al aplicarlos; FK(i) es la direccion de una funcion falsa (ranura de vtable).
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

namespace F204AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 28;
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
constexpr bool IsSym(u32 v) { return (v >> 20) >= 0xE00u && (v >> 20) <= 0xE04u; }
// Puntero simbolico a una funcion falsa (ranura de vtable falsa): FK(indice); se resuelve en LiveComparison.hpp
constexpr u32 FK(u32 i) { return 0xE0400000u | i; }

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
    std::array<u32, 64> a{};      // parametros de la escena y de los falsos (por funcion, ver abajo)
    std::array<u32, 9> args{};    // argumentos de pila (args[0] arriba); simbolicos permitidos
    u32 nargs{};
    std::vector<u8> s;            // guion de la escena
    u32 vtArena{0xFFu}, vtOff{}, vtExpect{};   // ranura de vtable del objeto recien construido (arena, desplazamiento, vtable del EXE): se compara canonizada
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool Pc(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    u32 W() { u32 v = Rand(n, nk()); if (IsSym(v)) v ^= 0x01000000u; return v; }   // nunca un puntero simbolico por azar
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
enum Fn : u32 { kSetUpIK, kQuitIK, kMakeAbortable, kCreateNext, kControl, kClone, kCreateFirst, kSitSdd, kSitProcess, kLieSdd, kVehScDtor, kPedScDtor, kObjScCtor, kObjScDtor, kPedColl, kAcqEvent, kDeathBody, kRunOver,
                kEvSdd0, kEvSdd1, kEvSdd2, kEvSdd3, kEvSdd4, kEvSdd5, kEvSdd6, kEvSdd7, kEvSdd8, kEvSdd9 };
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x66AB40, 0x66AD10, 0x66AD40, 0x66AD70, 0x66ADE0, 0x66D0C0, 0x672530, 0x676D10, 0x676D30, 0x6772C0, 0x605FB0, 0x606000, 0x606010, 0x606050, 0x606580, 0x606BA0, 0x4ADE90, 0x4AE2D0,
                                                    0x4B5E00, 0x4B5E50, 0x4B5EA0, 0x4B5EF0, 0x4B5F40, 0x4B5F90, 0x4B5FE0, 0x4B6030, 0x4B60D0, 0x4B6120};
// Cuerpo de destructor que llama cada destructor con borrado de evento (0x4B5E00 + 0x50 * i)
inline constexpr std::array<u32, 10> kEvBody{0x4ADF70, 0x4AE070, 0x4AE270, 0x4AE3C0, 0x4AE670, 0x4AE7A0, 0x4AEA50, 0x4AEB90, 0x4AED60, 0x4AEEA0};
// Bytes que limpia el llamador: todas las funciones del lote son `ret n` del callee (thiscall/stdcall)
inline constexpr std::array<u32, kFunctions> kPop{};
// Bytes que toca el objeto `this` (desde su puntero); 0 = la funcion no tiene un `this` en la arena A
inline constexpr std::array<u32, kFunctions> kSpan{0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x30, 0x30, 0x30, 4, 4, 0x50, 4, 0, 0, 4, 0x18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr u32 kEvElem = 0x44;
// Memoria global del juego tocada
inline constexpr u32 kThr = 0xC18D48;        // CTaskSimpleGoTo::ms_fLookAtThresholdDotProduct (float, variable)
inline constexpr u32 kEvPoolPtr = 0xB744AC;  // puntero al pool de eventos
inline constexpr u32 kTimerMs = 0xB7CB84;    // CTimer::m_snTimeInMilliseconds
inline constexpr u32 kRiotByte = 0x96913F;   // byte de disturbios
// Datos de los falsos dentro de los objetos (nada de esto lo lee el codigo real; el falso lo lee de la escena)
inline constexpr u32 kPedAlive = 0x484, kIntelTask = 0x100, kIntelMove = 0x104, kTaskIsGoTo = 0x1C;
inline constexpr u32 kIntelOff = 0x200, kVtOff = 0xF00;   // inteligencia y tabla de vtable falsa (arena C)

inline u32 StatesRand(Gen& g, u32 pFree) { return g.R(100) < pFree ? (0x80u | g.R(128)) : g.R(128); }

// ------------------------------------------------------------------------------------------------ ayudas de escena
inline u32 BitsF(float v) { return std::bit_cast<u32>(v); }
// Un float "moderado" (posicion del mundo): casi siempre entre -2000 y 2000, a veces un valor especial
inline u32 Mod(Gen& g) {
    const u32 k = g.R(20);
    if (k < 16) return std::bit_cast<u32>(float(i32(g.R(4001)) - 2000) / float(1u << g.R(3)));
    if (k == 16) return 0u;
    if (k == 17) return 0x80000000u;
    return FloatBits(g);
}
// Un componente de vector "casi unitario": entre -1 y 1 con valores especiales
inline u32 Unit(Gen& g) {
    const u32 k = g.R(16);
    if (k < 11) return std::bit_cast<u32>(float(i32(g.R(2001)) - 1000) / 1000.0f);
    if (k == 11) return std::bit_cast<u32>(1.0f);
    if (k == 12) return std::bit_cast<u32>(-1.0f);
    if (k == 13) return 0u;
    return FloatBits(g);
}
// Un vector unitario (90%) o con componentes casi unitarias
inline std::array<u32, 3> UnitVec(Gen& g) {
    if (g.Pc(10)) return {Unit(g), Unit(g), Unit(g)};
    double x = double(i32(g.R(2001)) - 1000) / 1000.0, y = double(i32(g.R(2001)) - 1000) / 1000.0, z = double(i32(g.R(2001)) - 1000) / 1000.0;
    double m = std::sqrt(x * x + y * y + z * z);
    if (m < 0.01) { x = 1.0; y = 0.0; z = 0.0; m = 1.0; }
    return {BitsF(float(x / m)), BitsF(float(y / m)), BitsF(float(z / m))};
}
// Un float moderado y bien condicionado (sin valores especiales)
inline u32 ModSafe(Gen& g) { return std::bit_cast<u32>(float(i32(g.R(4001)) - 2000) / float(1u << g.R(3))); }
inline void PutVec(Script& sc, u32 arena, u32 off, u32 x, u32 y, u32 z) { sc.Dw(arena, off, x); sc.Dw(arena, off + 4, y); sc.Dw(arena, off + 8, z); }
// Matriz: avance en +0x10, posicion en +0x30 (el resto lo rellena la arena)
inline void PutMat(Script& sc, u32 arena, u32 off, const std::array<u32, 3>& fwd, const std::array<u32, 3>& pos) {
    PutVec(sc, arena, off + 0x10, fwd[0], fwd[1], fwd[2]);
    PutVec(sc, arena, off + 0x30, pos[0], pos[1], pos[2]);
}
// Suma de floats en C++ (para armar escenas; el valor exacto lo calcula el oraculo con la x87)
inline u32 AddBits(u32 a, u32 b) { return std::bit_cast<u32>(std::bit_cast<float>(a) + std::bit_cast<float>(b)); }
// Vector de desplazamiento con modulo^2 > lim (pass = true) o <= lim, con casos de frontera
inline std::array<u32, 3> DeltaFor(Gen& g, bool pass, float limSq, bool boundary = true) {
    const float lim = std::sqrt(limSq);
    if (!pass) {
        const u32 k = boundary ? g.R(8) : 4 + g.R(4);
        if (k == 0) return {BitsF(lim), 0u, 0u};                                  // exactamente en el limite
        if (k == 1) return {0u, BitsF(-lim), 0u};
        if (k == 2) return {0u, 0u, BitsF(lim)};
        if (k == 3 || (!boundary && k == 7)) return {0x7FC00000u | (g.W() & 0x3FFFFFu), Mod(g), Mod(g)};   // NaN
        const float s = lim * 0.9f / 1.8f;
        return {BitsF(float(i32(g.R(2001)) - 1000) / 1000.0f * s), BitsF(float(i32(g.R(2001)) - 1000) / 1000.0f * s), BitsF(float(i32(g.R(2001)) - 1000) / 1000.0f * s)};
    }
    const u32 k = g.R(10);
    if (k == 0) return {BitsF(lim * 1.0001f), 0u, 0u};
    if (k == 1) return {BitsF(lim * 40.0f), BitsF(lim * 3.0f), 0u};
    float x = float(i32(g.R(2001)) - 1000) / 1000.0f, y = float(i32(g.R(2001)) - 1000) / 1000.0f, z = float(i32(g.R(2001)) - 1000) / 1000.0f;
    double m = std::sqrt(double(x) * x + double(y) * y + double(z) * z);
    if (m < 0.05) { x = 1.0f; y = 0.5f; z = 0.25f; m = std::sqrt(1.0 + 0.25 + 0.0625); }
    const float scale = float((lim * (1.05 + g.R(40) / 10.0)) / m);
    return {BitsF(x * scale), BitsF(y * scale), BitsF(z * scale)};
}
inline u32 NonZeroByteWord(Gen& g) { return (g.W() & 0xFFFFFF00u) | (1u + g.R(255)); }
inline u32 ZeroByteWord(Gen& g) { return g.W() & 0xFFFFFF00u; }

// Escena de CTaskComplexAvoidEntity (SetUpIK y ControlSubTask): `this` en A (desplazamiento o), peaton en B, matrices en D, pad, tarea padre, inteligencia y entidad en C.
// `gate` = primera comprobacion de SetUpIK que debe fallar (0 jugador sin controles, 1 en pantalla, 2 IK ya activa, 3 mira, 4 tarea secundaria, 5 y 6 tipos del padre, 7 distancia, 8 producto escalar); 9 o mas: pasa todas.
// Parametros: a[4] modo de FindPlayerPed (0 el propio peaton, 1 a[12], 2 nulo), a[5] GetIsOnScreen, a[6] GetLookAtEntity, a[7] GetTaskSecondary, a[8] y a[9] tipo del padre (1a y 2a llamada), a[10] IsLooking,
// a[11] puntero del pad, a[12] otro jugador, a[13] retorno de LookAt.
inline void AvScene(C& c, Script& sc, Gen& g, u32 o, u32 gate, bool control) {
    auto& a = c.a;
    const u32 pb = 4 * g.R((u32(kWin) - 0x490) / 4);
    const u32 dm = 4 * g.R(0x1E0), em = 0x800 + 4 * g.R(0x1E0);
    const bool nullMat = gate <= 7 && g.Pc(30);
    u32 padWord;
    if (gate == 0) { padWord = 0; a[4] = 0; }
    else if (g.Pc(50)) { padWord = 1u + g.R(0xFFFF); a[4] = g.R(3); }
    else { padWord = 0; a[4] = 1 + g.R(2); }
    a[11] = P(2, 0x40);
    a[12] = g.Pc(50) ? P(2, 0x700) : Raw(g);
    a[5] = gate == 1 ? ZeroByteWord(g) : NonZeroByteWord(g);
    a[6] = gate == 3 ? NonZeroByteWord(g) : 0u;
    a[7] = gate == 4 ? NonZeroByteWord(g) : 0u;
    a[10] = g.Pc(50) ? NonZeroByteWord(g) : ZeroByteWord(g);
    a[13] = g.W();
    u32 byte50 = g.R(256);
    byte50 = gate == 2 ? (byte50 | 1u) : (byte50 & ~1u);
    bool parent = gate == 5 || gate == 6 || g.Pc(35);
    const u32 oth1[]{0u, 0x3ABu, 0x3E8u, 0xFFFFFFFFu, 0x394u, 0x396u}, oth2[]{0u, 0x395u, 0x3E8u, 0xFFFFFFFFu, 0x3AAu, 0x3ACu};
    a[8] = gate == 5 ? 0x395u : oth1[g.R(6)];
    a[9] = gate == 6 ? 0x3ABu : oth2[g.R(6)];
    if (gate == 6) a[8] = oth1[g.R(6)];
    if (!parent || (gate != 5 && gate != 6 && g.Pc(10))) { a[8] = g.W(); a[9] = g.W(); if (a[8] == 0x395u) a[8] = 0x396u; if (a[9] == 0x3ABu) a[9] = 0x3ACu; }
    // posicion efectiva del peaton, objetivo, avance
    const std::array<u32, 3> pos{Mod(g), Mod(g), Mod(g)};
    const std::array<u32, 3> pedPlace = nullMat ? std::array<u32, 3>{ModSafe(g), ModSafe(g), ModSafe(g)} : std::array<u32, 3>{Mod(g), Mod(g), Mod(g)};
    const auto& eff = nullMat ? pedPlace : pos;
    const bool distPass = gate != 7;
    const std::array<u32, 3> dlt = DeltaFor(g, distPass, 9.0f, !nullMat);
    const std::array<u32, 3> tgt{AddBits(eff[0], dlt[0]), AddBits(eff[1], dlt[1]), AddBits(eff[2], dlt[2])};
    const std::array<u32, 3> fwd{Unit(g), Unit(g), Unit(g)};
    u32 thr;
    if (gate == 8) thr = g.Pk({BitsF(-2.0f), BitsF(-1.5f), 0x7FC00000u, 0xFF800000u, BitsF(-1.0001f)});
    else thr = g.Pk({BitsF(2.0f), BitsF(1.5f), 0x7F800000u, BitsF(0.0f), BitsF(0.5f), BitsF(-0.5f), Unit(g), BitsF(1.0f)});
    sc.GDw(kThr, thr);
    // peaton (B)
    PutVec(sc, 1, pb + 4, pedPlace[0], pedPlace[1], pedPlace[2]);
    if (nullMat) sc.Dw(1, pb + 0x14, 0); else sc.Ptr(1, pb + 0x14, P(3, dm));
    sc.Ptr(1, pb + 0x47C, P(2, kIntelOff));
    PutMat(sc, 3, dm, fwd, pos);
    // pad, padre, vtable falsa (C)
    sc.W16(2, 0x40 + 0x10E, padWord);
    sc.Ptr(2, 0x20, P(2, kVtOff));
    sc.Ptr(2, kVtOff + 0x10, FK(1));
    sc.Ptr(2, kVtOff, FK(0));
    // `this` (A)
    if (parent) sc.Ptr(0, o + 4, P(2, 0x20)); else sc.Dw(0, o + 4, 0);
    PutVec(sc, 0, o + 0x20, tgt[0], tgt[1], tgt[2]);
    sc.B8(0, o + 0x50, byte50);
    if (control) {
        const bool noEnt = g.Pc(12);
        const bool eMat = g.Pc(70);
        const bool distOk = g.Pc(50);
        const std::array<u32, 3> dd = DeltaFor(g, !distOk, 225.0f);
        const std::array<u32, 3> epos{AddBits(eff[0], dd[0]), AddBits(eff[1], dd[1]), AddBits(eff[2], dd[2])};
        const std::array<u32, 3> eplace{Mod(g), Mod(g), Mod(g)};
        if (noEnt) sc.Dw(0, o + 0xC, 0); else sc.Ptr(0, o + 0xC, P(2, 0x500));
        sc.Dw(0, o + 8, Raw(g));
        PutVec(sc, 2, 0x500 + 4, eMat ? eplace[0] : epos[0], eMat ? eplace[1] : epos[1], eMat ? eplace[2] : epos[2]);
        if (eMat) { sc.Ptr(2, 0x500 + 0x14, P(3, em)); PutMat(sc, 3, em, {Unit(g), Unit(g), Unit(g)}, epos); } else sc.Dw(2, 0x500 + 0x14, 0);
        const u32 cnt = g.Pk({0u, 29u, 30u, 5u, 0xFFFFFFFFu, 0x80000000u, 31u, 100u, 0x7FFFFFFFu, 31u});
        sc.Dw(2, kIntelOff + 0x274, cnt);
    }
    c.args[0] = P(1, pb); c.nargs = 1;
    c.ecx = P(0, o);
}

// Escena de un evento en el pool (kEvSdd*): a[4] pool en B, a[5] estados en D, a[6] datos, a[7] capacidad, a[8] cursor, a[9] indice, a[10] delta, a[11] 1 si el estado ya esta libre
inline void EvSddScene(C& c, Script& sc, Gen& g, bool group) {
    auto& a = c.a;
    const u32 cap = g.Pk({1u, 2u, 3u, 7u, 16u, 30u});
    a[4] = AuxOffset(g, 0x14);
    a[5] = AuxOffset(g, cap);
    const u32 idx = g.Pk({0u, cap - 1u, g.R(cap), g.R(cap), g.R(cap)});
    a[9] = idx;
    a[10] = g.Pk({0u, 0u, 0u, 0u, 1u, 7u, kEvElem - 1u, u32(-1), u32(-7), u32(-i32(kEvElem - 1)), g.R(kEvElem), u32(-i32(g.R(kEvElem)))});
    a[7] = cap;
    a[8] = g.Pk({0xFFFFFFFFu, 0u, idx - 1u, idx, idx + 1u, cap - 1u, cap, cap + 5u, g.R(cap), g.R(cap), 0x7FFFFFFFu, 0x80000000u, 0x80000001u});
    a[11] = g.Pk({0u, 0u, 1u});
    u32 objOff = 0;
    if (group) {
        // objetos en la arena A: base de datos tal que el objeto cae dentro de la ventana (con margen para el delta)
        const u32 span = cap * kEvElem;
        const u32 baseMax = u32(kWin) - span - 0x60;
        const u32 base = 0x50 + 4 * g.R((baseMax - 0x50) / 4 + 1);
        a[6] = P(0, base);
        objOff = base + idx * kEvElem + a[10];
    } else {
        a[6] = BigRaw(g);
    }
    std::vector<u8> states(cap);
    for (u32 i = 0; i < cap; ++i) states[i] = u8(StatesRand(g, 50));
    states[idx] = u8((states[idx] & 0x7Fu) | (a[11] ? 0x80u : 0u));
    sc.Bytes(3, a[5], states.data(), cap);
    if (group) sc.Ptr(1, a[4] + 0, a[6]); else sc.Dw(1, a[4] + 0, a[6]);
    sc.Ptr(1, a[4] + 4, P(3, a[5])); sc.Dw(1, a[4] + 8, cap); sc.Dw(1, a[4] + 0xC, a[8]);
    sc.B8(1, a[4] + 0x10, g.R(2)); sc.B8(1, a[4] + 0x11, g.R(2));
    sc.GPtr(kEvPoolPtr, P(1, a[4]));
    c.args[0] = g.Pk({0u, 1u, 1u, 0xFFFFFFFEu, 0xFFFFFFFFu, g.W(), g.W() | 1u, g.W() & ~1u}); c.nargs = 1;
    if (group) {
        // CEventGroupEvent: referencia al peaton (+0xC) y evento interno (+0x10, con la vtable falsa de la arena C)
        const bool ped = g.Pc(60), inner = g.Pc(70);
        if (ped) sc.Ptr(0, objOff + 0xC, g.Pc(50) ? P(1, 0x300) : Raw(g)); else sc.Dw(0, objOff + 0xC, 0);
        if (inner) { sc.Ptr(0, objOff + 0x10, P(2, 0x300)); sc.Ptr(2, 0x300, P(2, kVtOff)); sc.Ptr(2, kVtOff, FK(0)); } else sc.Dw(0, objOff + 0x10, 0);
        sc.Dw(0, objOff, g.W());
        c.ecx = P(0, objOff);
        c.vtArena = 0; c.vtOff = objOff; c.vtExpect = 0x85AB38;
    } else {
        c.ecx = a[6] + idx * kEvElem + a[10];
    }
}

// Grupo de un peaton (para kPedColl y kAcqEvent): peaton en `base` de la arena `ar`; inteligencia en +0x600, matriz en +0x800, tarea en +0x840
inline void PedGroup(Script& sc, Gen& g, u32 ar, u32 base, const std::array<u32, 3>& fwd, const std::array<u32, 3>& pos, bool nullMatrix) {
    sc.Dw(ar, base + 0x1C, g.W());
    PutVec(sc, ar, base + 4, Mod(g), Mod(g), Mod(g));
    PutVec(sc, ar, base + 0x44, Unit(g), Unit(g), Unit(g));
    if (nullMatrix) sc.Dw(ar, base + 0x14, 0); else sc.Ptr(ar, base + 0x14, P(ar, base + 0x800));
    sc.Ptr(ar, base + 0x47C, P(ar, base + 0x600));
    PutMat(sc, ar, base + 0x800, fwd, pos);
}
inline void PutFlagsByte(Script& sc, u32 ar, u32 off, bool bit0, Gen& g) { sc.B8(ar, off, bit0 ? (g.R(256) | 1u) : (g.R(256) & ~1u)); }

// kPedColl: a[20] bits de IntersectEdge, a[21] resultado de CEventGroup::Add
inline void PedCollScene(C& c, Script& sc, Gen& g) {
    auto& a = c.a;
    const u32 baseMain = 4 * g.R(0x1E8);
    const std::array<u32, 3> mainFwd = UnitVec(g);
    const std::array<u32, 3> mainPos{Mod(g), Mod(g), Mod(g)};
    PedGroup(sc, g, 1, baseMain, mainFwd, mainPos, false);
    const bool closestNull = g.Pc(6), flag1 = g.Pc(88), taskNull = g.Pc(6), isGo = g.Pc(92), still = g.Pc(8);
    PutFlagsByte(sc, 1, baseMain + 0x1C, flag1, g);
    const u32 mainTask = P(1, baseMain + 0x840);
    sc.Ptr(1, baseMain + 0x600 + kIntelTask, taskNull ? 0u : mainTask);
    sc.Dw(1, baseMain + 0x600 + kIntelMove, still ? 1u : g.Pk({0u, 2u, 3u, 4u, 0xFFFFFFFFu, g.W()}));
    const u32 target[3]{Mod(g), Mod(g), Mod(g)};
    PutVec(sc, 1, baseMain + 0x840 + 0xC, target[0], target[1], target[2]);
    sc.Dw(1, baseMain + 0x840 + kTaskIsGoTo, isGo ? NonZeroByteWord(g) : ZeroByteWord(g));
    // candidatos en A, C y D
    const u32 cars[3]{0u, 2u, 3u};
    u32 cbase[3];
    const float spread = g.Pk({BitsF(0.4f), BitsF(0.8f), BitsF(1.0f), BitsF(2.0f), BitsF(3.0f)}) == 0 ? 1.0f : std::bit_cast<float>(g.Pk({BitsF(0.4f), BitsF(0.8f), BitsF(1.0f), BitsF(2.0f), BitsF(3.0f)}));
    for (u32 k = 0; k < 3; ++k) {
        cbase[k] = 4 * g.R(0x1E8);
        const bool closeBy = g.Pc(80);
        std::array<u32, 3> fw = mainFwd;
        if (!g.Pc(45)) fw = UnitVec(g);
        else if (g.Pc(50)) fw = {AddBits(mainFwd[0], BitsF(float(i32(g.R(21)) - 10) / 4000.0f)), mainFwd[1], mainFwd[2]};
        const float s = spread * (closeBy ? 1.0f : 8.0f);
        const std::array<u32, 3> cp{AddBits(mainPos[0], BitsF(float(i32(g.R(2001)) - 1000) / 1000.0f * s)), AddBits(mainPos[1], BitsF(float(i32(g.R(2001)) - 1000) / 1000.0f * s)), AddBits(mainPos[2], BitsF(float(i32(g.R(201)) - 100) / 1000.0f))};
        PedGroup(sc, g, cars[k], cbase[k], fw, cp, false);
        PutFlagsByte(sc, cars[k], cbase[k] + 0x1C, g.Pc(88), g);
        sc.Dw(cars[k], cbase[k] + kPedAlive, g.Pc(88) ? NonZeroByteWord(g) : ZeroByteWord(g));
        sc.Ptr(cars[k], cbase[k] + 0x600 + kIntelTask, g.Pc(8) ? 0u : P(cars[k], cbase[k] + 0x840));
        sc.Dw(cars[k], cbase[k] + 0x600 + kIntelMove, g.Pk({0u, 2u, 3u, 1u}));
        sc.Dw(cars[k], cbase[k] + 0x840 + kTaskIsGoTo, g.Pc(85) ? NonZeroByteWord(g) : ZeroByteWord(g));
        PutVec(sc, cars[k], cbase[k] + 0x840 + 0xC, Mod(g), Mod(g), Mod(g));
    }
    // candidatos que no se tocan entre si: no hace falta; la cache de la escena apunta a las tres posiciones o a nulo
    for (u32 i = 0; i < 16; ++i) {
        const u32 pick = g.R(10);
        if (pick < 3) sc.Dw(1, baseMain + 0x600 + 0x130 + 4 * i, 0);
        else { const u32 k = pick % 3; sc.Ptr(1, baseMain + 0x600 + 0x130 + 4 * i, P(cars[k], cbase[k])); }
    }
    a[20] = g.Pc(70) ? (g.W() | g.W()) : g.W();
    a[21] = g.Pc(50) ? 0u : g.Pk({1u, 0x12345678u, g.W() | 1u});
    c.args[0] = P(1, baseMain);
    c.args[1] = closestNull ? 0u : (g.Pc(50) ? P(0, cbase[0]) : Raw(g));
    if (!closestNull && c.args[1] == 0u) c.args[1] = 1u;
    c.nargs = 2;
    c.ecx = g.W();
}

// kAcqEvent: a[20] ST0 de CanSeeEntityWithLights (float), a[21] LaRiotsActiveHere, a[22] CanJoinRiotAgainst, a[23] GetPedsGroup, a[24] AddEvent, a[25] resultado de CEventGroup::Add, byte 0x96913F y reloj globales
inline void AcqScene(C& c, Script& sc, Gen& g) {
    auto& a = c.a;
    const u32 pb = 4 * g.R((u32(kWin) - 0x490) / 4);
    const u32 eb = 4 * g.R((u32(kWin) - 0x5A0) / 4);
    u32 id = g.Pk({0u, 1u, 2u, 3u, 4u, 4u, 4u, 4u, 0u, 1u, 2u, 3u, 5u, 0xFFFFFFFFu, 0x80000000u, 7u});
    if (id < 5 && g.Pc(2)) id += 5;
    a[20] = g.Pk({0u, 0x80000000u, BitsF(1.0f), BitsF(-1.0f), BitsF(0.5f), BitsF(-0.5f), 0x7FC00000u, 0x00000001u, 0x80000001u, FloatBits(g), BitsF(100.0f), BitsF(-100.0f), 0x7F800000u, 0xFF800000u});
    if ((a[20] & 0x7F800000u) == 0x7F800000u && (a[20] & 0x7FFFFFu) != 0) a[20] |= 0x400000u;   // NaN siempre silencioso (el valor de ST0 sale de un falso)
    a[21] = g.Pc(50) ? NonZeroByteWord(g) : ZeroByteWord(g);
    a[22] = g.Pc(50) ? NonZeroByteWord(g) : ZeroByteWord(g);
    a[23] = g.Pc(40) ? 0u : P(2, 0x500);
    a[24] = g.Pc(50) ? NonZeroByteWord(g) : ZeroByteWord(g);
    a[25] = g.Pc(40) ? 0u : g.Pk({1u, 0x12345678u, g.W() | 1u});
    sc.GDw(kRiotByte & ~3u, (g.W() & 0x00FFFFFFu) | (g.Pc(50) ? 0u : (1u + g.R(255)) << 24));   // el byte 0x96913F es el mas alto de la palabra 0x96913C
    sc.GDw(kTimerMs, g.W());
    // peaton (B), entidad (D), su matriz (C)
    sc.Ptr(1, pb + 0x47C, P(2, kIntelOff));
    const bool rawEnt = id < 5 && id != 2 && id != 4 && g.Pc(15);
    const bool nullMat = g.Pc(30);
    sc.Dw(3, eb + 0x598, id == 2 ? (g.Pc(55) ? 6u : g.Pk({0u, 7u, 0xFFFFFFFFu, 18u})) : g.Pk({6u, 0u, 7u, g.W()}));
    PutVec(sc, 3, eb + 4, Mod(g), Mod(g), Mod(g));
    if (nullMat) sc.Dw(3, eb + 0x14, 0); else sc.Ptr(3, eb + 0x14, P(2, 0x600));
    PutMat(sc, 2, 0x600, {Unit(g), Unit(g), Unit(g)}, {Mod(g), Mod(g), Mod(g)});
    c.args[0] = P(1, pb);
    c.args[1] = id;
    c.args[2] = rawEnt ? (g.Pc(30) ? 0u : Raw(g)) : P(3, eb);
    c.nargs = 3;
    c.ecx = g.W();
}

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
    switch (c.fn) {
    case kSetUpIK:
    case kControl: {
        const u32 gate = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 9u, 9u, 9u, 9u});
        AvScene(c, sc, g, o, gate, c.fn == kControl);
        break;
    }
    case kQuitIK:
    case kMakeAbortable:
    case kCreateNext: {
        sc.B8(0, o + 0x50, g.R(256));
        a[10] = g.Pc(50) ? NonZeroByteWord(g) : ZeroByteWord(g);
        const u32 ped = g.Pc(70) ? P(1, 4 * g.R(0x300)) : Raw(g);
        c.args[0] = ped; c.nargs = 1;
        if (c.fn == kMakeAbortable) { c.args[1] = Raw(g); c.args[2] = Raw(g); c.nargs = 3; }
        c.ecx = P(0, o);
        break;
    }
    case kClone: {
        sc.Dw(0, o + 0x10, Raw(g));
        sc.Dw(0, o + 0xC, g.Pc(30) ? 0u : (g.Pc(50) ? P(2, 0x500) : Raw(g)));
        PutVec(sc, 0, o + 0x20, CoordBits(g), CoordBits(g), CoordBits(g));
        a[14] = g.Pc(20) ? 0u : (g.Pc(70) ? P(2, 0x600 + 4 * g.R(0x40)) : Raw(g));
        a[16] = Raw(g);
        c.ecx = P(0, o);
        break;
    }
    case kCreateFirst: {
        const u32 pb = 4 * g.R((u32(kWin) - 0x40) / 4), dm = 4 * g.R(0x1E0);
        const bool nullMat = g.Pc(30);
        PutVec(sc, 1, pb + 4, Mod(g), Mod(g), Mod(g));
        if (nullMat) sc.Dw(1, pb + 0x14, 0); else sc.Ptr(1, pb + 0x14, P(3, dm));
        PutMat(sc, 3, dm, {Unit(g), Unit(g), Unit(g)}, {Mod(g), Mod(g), Mod(g)});
        sc.Dw(0, o + 0x10, Raw(g));
        PutVec(sc, 0, o + 0x2C, CoordBits(g), CoordBits(g), CoordBits(g));
        sc.B8(0, o + 0x50, g.R(256));
        a[10] = g.Pc(50) ? NonZeroByteWord(g) : ZeroByteWord(g);
        a[15] = g.Pc(60) ? NonZeroByteWord(g) : ZeroByteWord(g);
        const u32 nb = 4 * g.R((u32(kWin) - 0x30) / 4);
        a[14] = g.Pc(15) ? 0u : P(2, nb);
        if (a[14]) { c.vtArena = 2; c.vtOff = nb; c.vtExpect = 0x86FD50; }
        c.args[0] = P(1, pb); c.nargs = 1;
        c.ecx = P(0, o);
        break;
    }
    case kSitSdd:
    case kLieSdd:
        sc.Dw(0, o, a[3]);
        c.args[0] = g.Pk({0u, 1u, 1u, 1u, 0xFFFFFFFEu, 0xFFFFFFFFu, g.W(), g.W() | 1u, g.W() & ~1u}); c.nargs = 1;
        c.ecx = P(0, o);
        break;
    case kVehScDtor:
    case kPedScDtor:
    case kObjScDtor:
    case kDeathBody:
        sc.Dw(0, o, a[3]);
        c.ecx = P(0, o);
        break;
    case kObjScCtor:
        sc.Dw(0, o, a[3]);
        for (u32 k = 0; k < 20; ++k) if (g.Pc(60)) sc.Dw(0, o + 4 * k, g.W());
        c.ecx = P(0, o);
        break;
    case kRunOver: {
        const u32 pb = 4 * g.R((u32(kWin) - 0x490) / 4);
        const u32 vo = 4 * g.R(0x100);
        sc.Dw(0, o, a[3]);
        const bool noVeh = g.Pc(15);
        if (noVeh) sc.Dw(0, o + 0x14, 0); else sc.Ptr(0, o + 0x14, P(2, vo));
        sc.Dw(1, pb + 0xFC, g.Pc(15) ? (g.W() | 1u) : 0u);
        sc.Dw(1, pb + 0x46C, g.Pc(15) ? (g.W() | 0x100u) : (g.W() & ~0x100u));
        sc.Dw(1, pb + kPedAlive, g.Pc(85) ? NonZeroByteWord(g) : ZeroByteWord(g));
        sc.Dw(2, vo + 0x590, g.Pk({5u, 5u, 0u, 1u, 4u, 6u, 0xFFFFFFFFu, g.W()}));   // tipo del vehiculo: 5 = barco
        c.args[0] = P(1, pb); c.nargs = 1;
        c.ecx = P(0, o);
        break;
    }
    case kSitProcess: {
        // `this` en A; peaton en B; matriz en D; tres animaciones en C (0x100 * j: +0x1C float, +0x24 float, +0x2C id de 16 bits); informacion del interior en C + 0x400 (+4 posicion, +0x10 direccion)
        // a[4] AreAnimsLoaded (bit k = llamada k), a[5] GetAssociation, a[6..8] animacion que devuelve cada BlendAnimation (0..2), a[9] y a[11] LimitRadianAngle, a[10] GetRadianAngleBetweenPoints, a[12..14] vector de CMatrix * CVector,
        // a[15] IsOutOfTime, a[16] y a[17] rand() de las dos llamadas a GetRandomIntInRange (0..1000 y 10000..20000), a[18] GetPedsInteriorGroup, a[19] informacion de FindClosestInteriorInfo, a[20] interior, a[21] distancia, a[22] IsLooking
        const u32 pb = 4 * g.R((u32(kWin) - 0x580) / 4), dm = 4 * g.R(0x1E0);
        const bool nullMat = g.Pc(10);
        const u32 idsAny[7]{0x157u, 0x158u, 0x159u, 0x157u, 0x158u, 0x159u, 0x156u};
        for (u32 j = 0; j < 3; ++j) {
            u32 id = nullMat ? g.Pk({0x158u, 0x159u, 0x15Au, 0x158u, 0x159u, 0x1u}) : idsAny[g.R(7)];
            if (g.Pc(5) && !nullMat) id = g.W() & 0xFFFFu;
            sc.W16(2, 0x100 * j + 0x2C, id);
            sc.Dw(2, 0x100 * j + 0x1C, g.W());
            sc.Dw(2, 0x100 * j + 0x24, g.W());
        }
        a[4] = g.W(); a[5] = g.Pc(50) ? 0u : g.W() | 1u;
        a[6] = g.R(3); a[7] = g.R(3); a[8] = g.R(3);
        a[9] = FloatBits(g); a[10] = FloatBits(g); a[11] = FloatBits(g);
        a[12] = CoordBits(g); a[13] = CoordBits(g); a[14] = CoordBits(g);
        a[15] = g.Pc(50) ? NonZeroByteWord(g) : ZeroByteWord(g);
        a[16] = (g.W() & 0xFFFF0000u) | (g.Pc(55) ? 32146u + g.R(622) : g.R(32146)); a[17] = (g.W() & 0xFFFF0000u) | g.R(32768);   // valores de rand(): 32146 o mas da > 980 en [0, 1000)
        a[18] = g.Pc(15) ? 0u : P(2, 0x600);
        a[19] = g.Pc(15) ? 0u : P(2, 0x400);
        a[20] = Raw(g); a[21] = FloatBits(g);
        a[22] = g.Pc(35) ? NonZeroByteWord(g) : ZeroByteWord(g);
        sc.GDw(kTimerMs, g.W());
        // `this`
        const u32 dur = g.Pk({0u, 1000u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 5000u, 1u, u32(-i32(g.R(5000))), g.R(30000)});
        sc.Dw(0, o + 8, dur);
        sc.Ptr(0, o + 0xC, P(2, 0x400));
        sc.B8(0, o + 0x10, g.Pc(40) ? (1u + g.R(255)) : 0u);
        if (g.Pc(25)) sc.Dw(0, o + 0x14, 0); else sc.Ptr(0, o + 0x14, P(2, 0x100 * g.R(3)));
        sc.Dw(0, o + 0x18, g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, 0x157u, 0x158u, 0x159u, g.W()}));
        sc.Dw(0, o + 0x1C, g.W()); sc.Dw(0, o + 0x20, g.W());
        sc.B8(0, o + 0x24, g.R(256));
        sc.B8(0, o + 0x28, g.Pc(40) ? (1u + g.R(255)) : 0u);
        sc.B8(0, o + 0x29, g.Pc(30) ? (1u + g.R(255)) : 0u);
        sc.B8(0, o + 0x2A, g.Pc(30) ? (1u + g.R(255)) : 0u);
        // peaton y matriz
        sc.Dw(1, pb + 0x18, Raw(g));
        PutVec(sc, 1, pb + 4, Mod(g), Mod(g), Mod(g));
        if (nullMat) sc.Dw(1, pb + 0x14, 0); else sc.Ptr(1, pb + 0x14, P(3, dm));
        const std::array<u32, 3> mpos{Mod(g), Mod(g), Mod(g)};
        PutMat(sc, 3, dm, {Unit(g), Unit(g), Unit(g)}, mpos);
        sc.Dw(1, pb + 0x10, g.W());
        sc.Dw(1, pb + 0x558, Mod(g));
        // informacion del interior (cerca de la posicion de la matriz el 40%: distancia en torno a 0.02)
        const float tiny[]{0.0f, 0.005f, -0.005f, 0.0141f, 0.02f, -0.02f, 0.03f, 0.0001f, 0.0199f};
        if (g.Pc(40)) PutVec(sc, 2, 0x400 + 4, AddBits(mpos[0], BitsF(tiny[g.R(9)])), AddBits(mpos[1], BitsF(tiny[g.R(9)])), AddBits(mpos[2], BitsF(tiny[g.R(9)])));
        else PutVec(sc, 2, 0x400 + 4, Mod(g), Mod(g), Mod(g));
        sc.Dw(2, 0x400 + 0x10, Mod(g)); sc.Dw(2, 0x400 + 0x14, Mod(g));
        c.args[0] = P(1, pb); c.nargs = 1;
        c.ecx = P(0, o);
        break;
    }
    case kPedColl:
        PedCollScene(c, sc, g);
        break;
    case kAcqEvent:
        AcqScene(c, sc, g);
        break;
    default:
        if (c.fn >= kEvSdd0) EvSddScene(c, sc, g, c.fn == kEvSdd1);
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
    j.U("vt_arena", c.vtArena).U("vt_off", c.vtOff).U("vt_expect", c.vtExpect);
    j.A("a", std::vector<u32>(c.a.begin(), c.a.end()));
    j.A("args", std::vector<u32>(c.args.begin(), c.args.end()));
    j.H("s", c.s);
    return j.End();
}
}
