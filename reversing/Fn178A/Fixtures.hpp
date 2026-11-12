#pragma once
// Fixtures del banco Fn178A (lote 178A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (sin hook antes del lote; las clases reales salen del desensamblado, de las vtables y de los llamadores, ver README.md y fixture_oracle.py). 23 reconstruidas.
//   idx  direccion  funcion real                                                                      forma
//    0   0x4AFB50   CEventVehicleThreat::Constructor                                                  thiscall(vehiculo) `ret 4`: +4 = 0 (dword), +8 = 0, +0Ch = 1, +0Eh = 200 (word), +10h = 0FFFFh (word), vtable 85B298h, +14h = vehiculo; 571B70 si no es nulo
//    1   0x4AFBF0   CEventVehicleThreat cuerpo del destructor                                         thiscall() `ret`: vtable 85B298h; si [+14h]: 571A00; vtable 85AB38h
//    2   0x5984C0   InteriorManager_c::Constructor                                                    thiscall() `ret`: 8 Interior_c (+0, stride 794h), listas +3CA0h / +428Ch / +4298h, 8 InteriorGroup_c (+3CACh, stride 0BCh), CRect +4740h
//    3   0x598580   InteriorManager_c::Destructor                                                     thiscall() `ret`: solo destructores vacios
//    4   0x59CA10   CReplay::ApplyPanelDamageToCar                                                    cdecl(paneles, coche, bandera) `ret`: nibbles de +5B4h; 6B1480 (paneles 0, 1, 4) y 6B1350 (5, 6)
//    5   0x59CBA0   CWinsockLink base destructor body                                                 thiscall() `ret`: [this] = 866DB0h
//    6   0x59CD50   CWinsockLink destructor con borrado (vtable 866DC4h)                              thiscall(banderas) `ret 4`: 59CBA0; delete CRT 8214BD si bit 0; eax = this
//    7   0x5DE2C0   CCopPed destructor con borrado                                                     thiscall(banderas) `ret 4`: 5DE0D0; delete 5E4760 si bit 0; eax = this
//    8   0x5DE2E0   CDummyPed::Constructor (el listado decia CCopPed)                                  thiscall() `ret`: 532540; vtable 86C198h; eax = this
//    9   0x5DE300   CDummyPed cuerpo del destructor                                                    thiscall() `ret`: vtable 86C198h; jmp 532560
//   10   0x5DE310   CDummyPed destructor con borrado                                                   thiscall(banderas) `ret 4`: 5DE300; delete 5326F0 si bit 0; eax = this
//   11   0x5DE340   CEmergencyPed::Constructor (el listado decia CCopPed)                              thiscall(tipo, modelo) `ret 8`: 5E8030(tipo); vtable 86C200h; 5E4880(modelo); 4E68D0 (ECX = +294h, this)
//   12   0x5DE3B0   CEmergencyPed cuerpo del destructor                                               thiscall() `ret`: vtable 86C200h; jmp 5E8620
//   13   0x5DE3E0   CEmergencyPed destructor con borrado                                              thiscall(banderas) `ret 4`: 5DE3B0; delete 5E4760 si bit 0; eax = this
//   14   0x5DE400   CEmergencyPed::ProcessControl (V, ranura 10)                                      thiscall() `ret`: 5E8CD0; bit 6 de +1Ch / estado +530h; CWeapon::Update(0) 73DB40 con el slot de +718h releido; tipo +598h
//   15   0x5FF2C0   CEventAcquaintancePedHateBadlyLit cuerpo del destructor                           thiscall() `ret`: vtable 86CB10h; jmp 4AF890
//   16   0x5FF2D0   CEventAcquaintancePedHateBadlyLit::GetEventType (V)                               thiscall() `ret`: mov eax,3Fh
//   17   0x5FF2E0   CEventAcquaintancePedHateBadlyLit::CloneEditable (V)                               thiscall() `ret`: new(28h) 4B5620; constructor 5FF250(+14h, +18h, &+1Ch); eax = el del constructor
//   18   0x5FF350   CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent (V)                 thiscall() `ret`: mov al,1
//   19   0x5FF360   CEventAcquaintancePedHateBadlyLit destructor con borrado                          thiscall(banderas) `ret 4`: 5FF2C0; delete 4B5630 si bit 0; eax = this
//   20   0x6C9160   CPlane cuerpo del destructor                                                       thiscall() `ret`: vtable 871948h; fogonazos +9D4h; chorro +9E8h; humo +9F8h; Terminate +138h; 6A61E0
//   21   0x6CADB0   CPlane destructor con borrado                                                      thiscall(banderas) `ret 4`: 6C9160; delete 6E2D90 si bit 0; eax = this
//   22   0x6CDC30   CQuadBike cuerpo del destructor                                                    thiscall() `ret`: vtable 871AE8h; 6D0B30 (ECX = +98Ch); 6A61E0
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A de 0x5000 bytes, B, C y D de 0x1000, guardas de 0x40) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el cuerpo de la DLL).
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

namespace F178AFix {
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
constexpr std::size_t kWin = 0x1000;    // ventana de las arenas B, C y D
constexpr std::size_t kWinA = 0x5000;   // ventana de la arena A (el objeto de InteriorManager_c mide 0x4750 bytes)
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
constexpr unsigned kSlots = 64;       // dwords de parametros del caso; a[60..63] son las semillas de las arenas
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
// Un puntero crudo no nulo
inline u32 RawNz(Gen& g) { u32 v; do { v = Raw(g); } while (v == 0); return v; }

// Banderas de los destructores con borrado (el EXE lee solo el BYTE bajo de la ranura de pila): bit 0 puesto o no, y bits altos / de otros bytes
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}

// Clases de funcion (forma)
enum Kind : u32 { kVtCtor, kVtDtor, kImCtor, kImDtor, kReplay, kWsBase, kSdd, kDummyCtor, kBodyJmp, kEmerCtor, kEmerPc, kConstType, kHblClone, kConstBool, kPlaneBody, kQuadBody };
inline constexpr std::array<Kind, kFunctions> kKind{kVtCtor, kVtDtor, kImCtor, kImDtor, kReplay, kWsBase, kSdd, kSdd, kDummyCtor, kBodyJmp, kSdd, kEmerCtor, kBodyJmp, kSdd, kEmerPc, kBodyJmp,
                                                    kConstType, kHblClone, kConstBool, kSdd, kPlaneBody, kSdd, kQuadBody};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x4AFB50, 0x4AFBF0, 0x5984C0, 0x598580, 0x59CA10, 0x59CBA0, 0x59CD50, 0x5DE2C0, 0x5DE2E0, 0x5DE300, 0x5DE310, 0x5DE340, 0x5DE3B0, 0x5DE3E0, 0x5DE400,
                                                    0x5FF2C0, 0x5FF2D0, 0x5FF2E0, 0x5FF350, 0x5FF360, 0x6C9160, 0x6CADB0, 0x6CDC30};
// Bytes que cada funcion toca a partir de su puntero `this`
inline constexpr std::array<u32, kFunctions> kSpan{0x18, 0x18, 0x4750, 0x4750, 4, 4, 4, 4, 4, 4, 4, 0x79C, 4, 4, 0x79C, 4, 4, 0x28, 4, 4, 0xA00, 4, 0x990};
// Un coche sintetico de CReplay (arena B) ocupa 0x5B8 bytes; la tabla de fogonazos del avion, 8 entradas (0x20)
inline constexpr u32 kCarSpan = 0x5B8, kTableSpan = 0x20;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[3] = dword previo en [this], a[60..63] = semillas de las arenas A, B, C y D
//   kVtCtor: a[4] = vehiculo (0 nulo, 1 en B, 2 crudo no nulo), a[5] = desplazamiento en B, a[6] = vehiculo crudo
//   kVtDtor: a[4..6] = igual (el vehiculo se coloca en [this + 14h])
//   kImCtor, kImDtor, kConstType, kConstBool, kWsBase: nada mas (los datos son el relleno de la arena)
//   kReplay: a[4] = paneles empaquetados, a[5] = estado previo (+5B4h del coche), a[6] = bandera (0 o 1), a[7] = desplazamiento del coche en B, a[8..14] = XOR que escribe el falso en el estado en su llamada 0..6
//   kSdd: a[4] = EAX del falso del cuerpo, a[5] = dword que el falso deja en [this], a[6] = banderas (pila)
//   kDummyCtor, kBodyJmp: a[4] = EAX del falso del colaborador, a[5] = dword que deja en [this]
//   kEmerCtor: a[4] = tipo (pila), a[5] = modelo (pila), a[6] = EAX del falso de 5E8030, a[7] = dword que deja en [this], a[8] = EAX de los otros falsos
//   kEmerPc: a[4] = dword de +1Ch tras 5E8CD0, a[5] = estado (+530h) tras 5E8CD0, a[6] = tipo (+598h) tras 5E8CD0, a[7] = slot (+718h, byte) tras 5E8CD0, a[8] = estado tras la 1a Update, a[9] = mascara de escrituras
//         (1: estado tras la 1a Update, 2: slot tras la 1a Update, 4: tipo tras la 2a Update, 8: slot tras la 2a Update), a[10] = slot tras la 1a Update, a[11] = tipo tras la 2a Update, a[12] = slot tras la 2a Update
//   kHblClone: a[4] = ped (+14h, crudo), a[5] = tiempo (+18h), a[6] = memoria de new (0 nula, 1 en B), a[7] = desplazamiento en B, a[8] = EAX del falso del constructor, a[9] = dword que deja en [mem]
//   kPlaneBody: a[4] = tabla de fogonazos (0 nula, 1 en B), a[5] = desplazamiento en B, a[6..13] = las 8 entradas de la tabla, a[14] = desplazamiento de la tabla alternativa en C, a[15..22] = sus entradas,
//         a[23..32] = lo que devuelve GetPlaneNumGuns en sus llamadas 0..9 (con signo), a[33..36] = particulas de chorro, a[37] = particula de humo, a[38..41] = escritura del falso de Kill en su llamada k
//         (0 ninguna, 1 = la entrada j de la tabla vigente := a[42 + k], 2 = el puntero +9D4h := tabla alternativa), j = (a[38 + k] >> 8) & 7, a[42..45] = valores, a[46] = dword que deja el falso del destructor base,
//         a[47] = EAX del falso del destructor base
//   kQuadBody: a[4] = EAX del falso de 6A61E0, a[5] = dword que deja en [this]
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[60] = c.seed * 13u + 50u; c.a[61] = c.a[60] + 1u; c.a[62] = c.a[60] + 2u; c.a[63] = c.a[60] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    c.a[3] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 0x85B298u, 0x85AB38u, 0x866DB0u, 0x86C198u, 0x86C200u, 0x86CB10u, 0x871948u, 0x871AE8u});
    auto& a = c.a;
    switch (kKind[c.fn]) {
    case kVtCtor:
    case kVtDtor:
        a[4] = g.Pk({0u, 1u, 1u, 2u, 2u});
        a[5] = AuxOffset(g, 0x20);
        a[6] = RawNz(g);
        break;
    case kImCtor:
    case kImDtor:
    case kConstType:
    case kConstBool:
    case kWsBase:
        break;
    case kReplay: {
        const u32 status = g.Pk({0u, 0xFFFFFFFFu, 0x11111111u, 0x00F0F0F0u, 0x0F0F0F0Fu, g.W(), g.W(), g.W()});
        u32 panels;
        const u32 how = g.R(10);
        if (how == 0) panels = status;
        else if (how == 1) panels = g.W();
        else {
            panels = status;
            const u32 masks[7] = {0xFu, 0xF0u, 0xF00u, 0xF000u, 0xF0000u, 0xF00000u, 0xF000000u};
            for (u32 m : masks) if (g.P(50)) panels ^= (g.W() | 1u) & m;
            if (g.P(30)) panels = (panels & 0x0FFFFFFFu) | (g.W() & 0xF0000000u);
        }
        a[4] = panels; a[5] = status; a[6] = g.R(2); a[7] = AuxOffset(g, kCarSpan);
        const u32 scr = g.R(4);   // 0 sin escrituras de los falsos, 1 pocas, 2 muchas, 3 una sola
        const u32 only = g.R(7);
        for (u32 k = 0; k < 7; ++k) {
            const bool on = scr == 2 ? g.P(80) : (scr == 1 ? g.P(30) : (scr == 3 ? k == only : false));
            a[8 + k] = on ? (g.W() | 1u) : 0u;
        }
        break;
    }
    case kSdd:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[5] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 1u});
        a[6] = FlagsDword(g);
        break;
    case kDummyCtor:
    case kBodyJmp:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[5] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 1u});
        break;
    case kEmerCtor:
        a[4] = Raw(g); a[5] = Raw(g);
        a[6] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[7] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 1u});
        a[8] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W()});
        break;
    case kEmerPc:
        a[4] = g.Pk({0u, 0x40u, 0xFFFFFFBFu, 0xFFFFFFFFu, 0x80u, 0x20u, 0x41u, 0x7Fu, g.W(), g.W() | 0x40u, g.W() & ~0x40u});
        a[5] = g.Pk({0u, 0x37u, 0x37u, 0x36u, 0x36u, 0x35u, 0x38u, 0xFFFFFFFFu, 1u, 0x1Cu, g.W(), 0x37u ^ 0x100u});
        a[6] = g.Pk({0u, 1u, 0x12u, 0x12u, 0x13u, 0x13u, 0x11u, 0x14u, 0xFFFFFFFFu, g.W(), 0x112u, 0x113u});
        a[7] = g.Pk({0u, 1u, 0x7Fu, 0x80u, 0xFFu, 0x81u, 0x7Eu, 0xFFFFFF00u, g.W(), g.W() & 0xFFu, 5u, 0xFEu});
        a[8] = g.Pk({0u, 0x37u, 0x36u, 0x36u, 0x35u, 0xFFFFFFFFu, g.W(), 0x36u ^ 0x100u});
        a[9] = g.R(16);
        a[10] = g.Pk({0u, 1u, 0x7Fu, 0x80u, 0xFFu, g.W() & 0xFFu, 0x81u, 5u});
        a[11] = g.Pk({0u, 1u, 0x12u, 0x13u, 0x13u, 0x12u, 0xFFFFFFFFu, g.W(), 0x113u});
        a[12] = g.Pk({0u, 1u, 0x7Fu, 0x80u, 0xFFu, g.W() & 0xFFu});
        break;
    case kHblClone:
        a[4] = Raw(g); a[5] = g.W();
        a[6] = g.Pk({0u, 1u, 1u, 1u});
        a[7] = AuxOffset(g, 0x40);
        a[8] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[9] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu});
        break;
    case kPlaneBody: {
        a[4] = g.Pk({0u, 1u, 1u, 1u});
        a[5] = AuxOffset(g, kTableSpan);
        a[14] = AuxOffset(g, kTableSpan);
        const u32 dens = g.Pk({0u, 20u, 50u, 80u, 100u});
        for (u32 i = 0; i < 8; ++i) { a[6 + i] = g.P(dens) ? 0u : RawNz(g); a[15 + i] = g.P(dens) ? 0u : RawNz(g); }
        const u32 guns = g.Pk({0u, 1u, 2u, 3u, 4u, 6u, 8u, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u});
        for (u32 k = 0; k < 10; ++k) a[23 + k] = g.P(55) ? guns : g.Pk({0u, 1u, 2u, 3u, 5u, 7u, 8u, 0xFFFFFFFFu});
        for (u32 i = 0; i < 4; ++i) a[33 + i] = g.P(50) ? 0u : RawNz(g);
        a[37] = g.P(50) ? 0u : RawNz(g);
        for (u32 k = 0; k < 4; ++k) {
            const u32 what = g.Pk({0u, 0u, 1u, 1u, 2u});
            a[38 + k] = what | (g.R(8) << 8);
            a[42 + k] = g.P(50) ? 0u : RawNz(g);
        }
        a[46] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 1u});
        a[47] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W()});
        break;
    }
    case kQuadBody:
        a[4] = g.Pk({0u, 1u, 0xFFFFFFFFu, g.W(), g.W()});
        a[5] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), 0xDEADBEEFu, 1u});
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
