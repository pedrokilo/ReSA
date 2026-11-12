#pragma once
// Fixtures del banco Fn174A (lote 174A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; las clases del listado salen del desensamblado, de las vtables leidas del PE y de los llamadores, ver README.md y fixture_oracle.py). 21 asignadas: 20 con banco;
// 0x6F55C0 (un `jmp 0x6D0E60` puro, ranura Render de la vtable de CTrain) se salta y se documenta en el README.
//   idx  direccion  nombre real                                                              forma
//    0   0x57C400   CPad::IsMouseWheelUpPressed                    cdecl() `ret`: byte [B7341B] != 0 y byte [B7342F] == 0 -> AL = 1
//    1   0x57C420   CPad::IsMouseWheelDownPressed                  bytes [B7341C] y [B73430]
//    2   0x57C440   CPad::IsMouseBmx1Pressed                       bytes [B7341D] y [B73431]
//    3   0x57C460   CPad::IsMouseBmx2Pressed                       bytes [B7341E] y [B73432]
//    4   0x57C5E0   CMenuManager::NoDiskInDriveMessage             thiscall() `ret`: ocho colaboradores, [C17050], palabras [B733A8] y [B73138], byte [this+58h]
//    5   0x6C4340   CHeli cuerpo del destructor (vtable 0x871680, marco SEH)  thiscall() `ret`: fogonazos (+A0C), 6D3F30, 4AA3F0, 4A9810, 8213AE, 4FB8C0, 6A61E0
//    6   0x6C4550   CHeli::SetupDamageAfterLoad (ranura virtual)    thiscall() `ret`: and byte [ecx+42Ah],0FEh
//    7   0x6C4810   CHeli destructor con borrado (vtable 0x871680)  thiscall(flags) `ret 4`: cuerpo 6C4340 (funcion 5), CVehicle::operator delete 6E2D90
//    8   0x6C7D10   CMonsterTruck cuerpo del destructor (vtable 0x8717D8)  thiscall() `ret`: [ecx]=8717D8h; jmp 6A61E0
//    9   0x718E50   CFontChar::Set                                  thiscall(origen) `ret 4`: copia 47 bytes, eax = this
//   10   0x719240   CFont::StripTildeTags                          cdecl(texto) `ret`: quita las etiquetas entre '~' en el sitio (buffer local de 0x100)
//   11   0x719360   CFont::MapExtendedChar                         cdecl(byte) `ret`: AL = ch si < 0x80, si no tabla 0x8D5AD8 + 2 * ch
//   12   0x54ED20   Common::TransformPointsSimple                  cdecl(salida, cuenta, transformacion, entrada) `ret`: rotacion por el rumbo mas la posicion (x87)
//   13   0x54EEA0   Common::TransformVectors (CMatrix)             cdecl(salida, cuenta, matriz, entrada) `ret`: CopyToRwMatrix 59B8B0, RwMatrixUpdate 7F18A0, RwV3dTransformVector 7EDDC0 por vector
//   14   0x71DB30   Common::DoesInfiniteLineCrossFiniteLine       cdecl(8 floats) `ret`: ((Ax-ox)*dy-(Ay-oy)*dx) * ((Bx-ox)*dy-(By-oy)*dx) < 0 (x87)
//   15   0x6F4FA0   tHandlingData constructor                      thiscall() `ret`: cTransmission 6D0450 sobre this+2Ch; eax = this
//   16   0x6F5440   CTrainNode::GetPosn                            thiscall(salida) `ret 4`: tres words con signo por 0.125f
//   17   0x6F54D0   CTrain cuerpo del destructor (vtable 0x872370, marco SEH)  thiscall() `ret`: Terminate 4FB8C0 (+138h), ~CVehicle 6E2B40
//   18   0x6F5530   CTrain::SetModelIndex (ranura virtual)          thiscall(indice) `ret 4`: CVehicle::SetModelIndex 6D6A40, memset +668h (17 dwords), FillFrameArray 4C5440
//   19   0x6F5570   CTrain::PreRender (ranura virtual)              thiscall() `ret`: CVehicle::PreRender 6D6480, DoVehicleLights 6E1A60 segun banderas (+5B8h) y modelo (+22h)
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

namespace F174AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 20;

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
inline u32 B(float f) { return std::bit_cast<u32>(f); }
inline float F(u32 b) { return std::bit_cast<float>(b); }

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
constexpr std::size_t kSlots = 48;
struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, kSlots> a{};
    std::vector<u8> s;          // bytes del texto de StripTildeTags
};

struct Gen {
    unsigned n;
    u32 kk;
    explicit Gen(unsigned n_, u32 salt = 100) : n(n_), kk(salt) {}
    u32 nk() { return kk++; }
    bool P(u32 pct) { return Rand(n, nk()) % 100 < pct; }
    u32 R(u32 m) { return Rand(n, nk()) % m; }
    u32 Pk(std::initializer_list<u32> l) { return *(l.begin() + R(u32(l.size()))); }
    float Fl(float lo, float hi) { return lo + (hi - lo) * (float(R(100001)) / 100000.0f); }
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

// ------------------------------------------------------------------------------------------------ floats
inline constexpr std::array<u32, 12> kTiny{0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x807FFFFFu, 0x00400000u, 0x80400000u, 0x00000100u, 0x80012345u, 0x00800000u, 0x80800000u};
inline constexpr std::array<u32, 4> kInf{0x7F800000u, 0xFF800000u, 0x7F800000u, 0xFF800000u};
inline constexpr std::array<u32, 6> kBig{0x7F7FFFFFu, 0xFF7FFFFFu, 0x7F000000u, 0xFF000000u, 0x7E800000u, 0xFE800000u};
// NaN silenciosos (carga util distinta) y senalizantes
inline constexpr std::array<u32, 8> kNans{0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FA00000u, 0xFFBFFFFFu, 0x7FC12345u, 0xFFFFFFFFu};
inline u32 Nan(Gen& g) { return kNans[g.R(8)]; }
// Un float "especial" sin NaN: cero, desnormal, infinito, enorme o finito normal
inline u32 SpecialNoNan(Gen& g) {
    const u32 r = g.R(28);
    if (r < 10) return kTiny[g.R(12)];
    if (r < 16) return kInf[g.R(4)];
    if (r < 22) return kBig[g.R(6)];
    return B(float(int(g.R(2001)) - 1000) / 7.0f);
}
inline u32 Special(Gen& g) { return g.P(25) ? Nan(g) : SpecialNoNan(g); }
inline u32 AnyFloat(Gen& g) { return g.P(60) ? g.W() : Special(g); }
// Float de una coordenada: casi siempre finito y moderado, a veces especial
inline u32 Coord(Gen& g) { return g.P(80) ? B(g.Fl(-100.0f, 100.0f)) : (g.P(50) ? Special(g) : g.W()); }
inline u32 HeadingFinite(Gen& g) { return g.P(80) ? B(g.Fl(-8.0f, 8.0f)) : g.Pk({0u, 0x80000000u, 1u, 0x80000001u, 0x007FFFFFu, 0x00800000u, 0x3F800000u, 0xBF800000u, 0x3F490FDBu, 0xBFC90FDBu, 0x40490FDBu, 0xC0490FDBu}); }
// Rumbo: finito (|h| <= 8, el dominio del modelo del oraculo), cero, o a veces NaN / infinito
inline u32 Heading(Gen& g) { return g.P(12) ? g.Pk({0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0x7F800000u, 0xFF800000u, 0x7FC12345u}) : HeadingFinite(g); }

// Clases de funcion (forma)
enum Kind : u32 { kMouse, kNoDisk, kHeliBody, kHeliDamage, kSdd, kVtBody, kFontSet, kStrip, kMapChar, kPoints, kVecs, kCross, kHandCtor, kNode, kTrainSet, kTrainPre };
inline constexpr std::array<Kind, kFunctions> kKind{
    kMouse, kMouse, kMouse, kMouse, kNoDisk, kHeliBody, kHeliDamage, kSdd, kVtBody, kFontSet,
    kStrip, kMapChar, kPoints, kVecs, kCross, kHandCtor, kNode, kVtBody, kTrainSet, kTrainPre};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{0x57C400, 0x57C420, 0x57C440, 0x57C460, 0x57C5E0, 0x6C4340, 0x6C4550, 0x6C4810, 0x6C7D10, 0x718E50,
                                                    0x719240, 0x719360, 0x54ED20, 0x54EEA0, 0x71DB30, 0x6F4FA0, 0x6F5440, 0x6F54D0, 0x6F5530, 0x6F5570};
// Colaborador que llama la funcion y falsea FkBody (base ~CAutomobile, cuerpo de CHeli, cTransmission, ~CVehicle; 0 si ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0, 0, 0, 0, 0x6A61E0, 0, 0x6C4340, 0x6A61E0, 0, 0, 0, 0, 0, 0, 0x6D0450, 0, 0x6E2B40, 0, 0};
// vtable de la clase (dwords previos plausibles de [this])
inline constexpr std::array<u32, kFunctions> kVtable{0, 0, 0, 0, 0, 0x871680, 0x871680, 0x871680, 0x8717D8, 0, 0, 0, 0, 0, 0, 0, 0, 0x872370, 0x872370, 0x872370};
// Bytes que cada funcion toca a partir de su puntero `this` (o de su objeto principal en la arena A)
inline constexpr std::array<u32, kFunctions> kSpan{4, 4, 4, 4, 0x5C, 0xA10, 0x430, 4, 4, 0x30, 4, 4, 0x10, 4, 4, 0x30, 8, 0x13C, 0x6AC, 0x5BC};
// Constantes del EXE
inline constexpr u32 kExeVtHeli = 0x871680, kExeVtTruck = 0x8717D8, kExeVtTrain = 0x872370;
inline constexpr u32 kMouseNew = 0xB73418, kMouseOld = 0xB7342C, kMouseRegion = 0xB73418, kMouseRegionBytes = 0x28;   // CMouseControllerState (0x14 bytes) nueva y anterior
inline constexpr u32 kAddrQuit = 0xC17050, kAddrEscNew = 0xB733A8, kAddrEscOld = 0xB73138;
inline constexpr u32 kHeliFxTable = 0xA0C, kHeliFlags = 0x428, kTrainNodes = 0x668, kTrainNodeBytes = 0x44, kTrainFlags = 0x5B8, kTrainModel = 0x22, kTrainMatrix = 0x14, kTrainClump = 0x18;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y reserva
//   kMouse:    a[3] = byte nuevo; a[4] = byte anterior (los dos con otros bytes de la region de 0x28 rellenos con la semilla a[15])
//   kNoDisk:   a[3] = dword RsGlobal.quit (0xC17050); a[4] = palabra esc nueva (0xB733A8); a[5] = palabra esc anterior (0xB73138); a[6] = dword previo en this+58h; a[7] = EAX que devuelven los falsos (gRet)
//   kHeliBody: a[3] = dword previo en [this]; a[4] = gRet; a[5] = gDtorWrite (lo que el falso de la base deja en [this]); a[6] = tabla de fogonazos (0 nula, 1 en B a[7]); a[8] = valor de GetPlaneNumGuns (con signo);
//              a[16 + i], a[32 + i] (i < 16) = entrada i de la tabla: modo (a[32+i]: 0 nula, 1 puntero a C con desplazamiento a[16+i], 2 puntero crudo a[16+i])
//   kHeliDamage: a[3] = dword en +424h; a[4] = dword en +428h; a[5] = dword en +42Ch
//   kSdd:      a[3] = dword previo en [this]; a[4] = EAX que devuelven los falsos (gRet); a[5] = dword que el falso del cuerpo deja en [this] (gDtorWrite); a[6] = banderas
//   kVtBody:   a[3] = dword previo en [this]; a[4], a[5] = gRet, gDtorWrite
//   kFontSet:  a[3] = desplazamiento del origen en B; a[4] = 0 origen en B / 1 el origen es el propio objeto
//   kStrip:    s = bytes del texto (sin el NUL; etiquetas '~' por pares); a[3] = desplazamiento del texto en B
//   kMapChar:  a[3] = dword de pila (cuenta el byte bajo)
//   kPoints:   a[3] = cuenta (0..6); a[4] = 0 salida en C / 1 la salida es la entrada (B); a[5..8] = posicion x, y, z y rumbo (bits de float) de la transformacion (en A); a[9] = desplazamiento de la entrada en B;
//              a[10] = desplazamiento de la salida en C; a[16..33] = 18 floats de entrada (bits)
//   kVecs:     a[3] = cuenta (0..6); a[4] = 0 salida en C / 1 la salida es la entrada (B); a[9] = desplazamiento de la entrada en B; a[10] = desplazamiento de la salida en C; a[16..33] = 18 dwords de entrada;
//              a[34] = gRet (EAX de los falsos); la matriz es el objeto de A
//   kCross:    a[3..10] = Ax, Ay, Bx, By, ox, oy, dx, dy (bits de float)
//   kHandCtor: a[3] = dword previo en this+2Ch; a[4] = gRet; a[5] = gDtorWrite
//   kNode:     a[3], a[4], a[5] = words x, y, z (en +0, +2 y +4); a[6] = desplazamiento de la salida en B
//   kTrainSet: a[3] = indice (argumento de pila); a[4] = clump (+18h): 0 nulo / 1 en B (a[5]) con tipo rpCLUMP; a[5] = desplazamiento del clump en B; a[6] = gRet; a[7] = dword que el falso de FillFrameArray deja en frames[0]
//   kTrainPre: a[3] = palabra de banderas (+5B8h); a[4] = palabra de modelo (+22h); a[5] = 0 matriz cruda a[6] / 1 matriz en B (a[7]); a[6] = puntero crudo; a[7] = desplazamiento en B
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
    const auto byteVal = [&] { return g.Pk({0u, 0u, 1u, 1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x100u + g.R(256), g.R(256)}) & 0xFFu; };
    const auto word16 = [&] { return g.Pk({0u, 0u, 1u, 0xFFFFu, 0x8000u, 0x0100u, 0x00FFu, 0x7FFFu, g.W() & 0xFFFFu, g.W() & 0xFFFFu}); };
    switch (kKind[c.fn]) {
    case kMouse:
        c.a[3] = byteVal();
        c.a[4] = g.P(25) ? c.a[3] : byteVal();
        break;
    case kNoDisk:
        c.a[3] = g.Pk({0u, 0u, 0u, 0u, 0u, 1u, 2u, 0x100u, 0xFFFFFFFFu, 0x80000000u, 0x01000000u, g.W()});
        c.a[4] = g.P(45) ? 0u : word16();
        c.a[5] = g.P(40) ? 0u : word16();
        if (g.P(10)) c.a[5] = c.a[4];
        c.a[6] = g.W();
        c.a[7] = Raw(g);
        break;
    case kHeliBody: {
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = g.P(70) ? 1u : 0u;
        c.a[7] = Offset(g, g.R(4), 0x40);
        c.a[8] = g.Pk({0u, 1u, 1u, 1u, 2u, 3u, 4u, 6u, 6u, 8u, 16u, 0xFFFFFFFFu, 0x80000000u, 0xFFFFFFF0u, g.R(17)});
        for (unsigned i = 0; i < 16; ++i) {
            const u32 mode = g.Pk({0u, 0u, 1u, 1u, 1u, 2u});
            c.a[32 + i] = mode;
            c.a[16 + i] = mode == 1 ? Offset(g, g.R(4), 4) : Raw(g);
        }
        break;
    }
    case kHeliDamage:
        c.a[3] = g.W();
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x00010000u, 0xFFFEFFFFu, 0x00FF0000u, 0x00000001u, 0x01000000u, g.W(), g.W(), g.W() | 0x10000u, g.W() & ~0x10000u});
        c.a[5] = g.W();
        break;
    case kSdd:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        c.a[6] = FlagsDword(g);
        break;
    case kVtBody:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kFontSet:
        c.a[3] = Offset(g, g.R(4), 0x30);
        c.a[4] = g.P(15) ? 1u : 0u;
        break;
    case kStrip: {
        // texto: tramos de caracteres (sin NUL ni '~') y etiquetas "~..~" (sin '~' dentro, tambien vacias); longitud total <= 200
        std::vector<u8> s;
        const u32 segs = g.R(7);
        const auto plain = [&](u32 count) {
            for (u32 k = 0; k < count; ++k) {
                u32 ch = g.P(70) ? 0x20u + g.R(0x5F) : 1u + g.R(255);
                if (ch == '~') ch = 0x7Fu;
                s.push_back(u8(ch));
            }
        };
        const u32 mode = g.R(10);
        if (mode == 0) {
            // vacio
        } else if (mode == 1) {
            plain(1 + g.R(60));                       // sin etiquetas
        } else {
            plain(g.R(8));
            for (u32 sIdx = 0; sIdx < segs + 1; ++sIdx) {
                s.push_back('~');
                const u32 inner = g.P(20) ? 0u : 1 + g.R(5);
                plain(inner);
                s.push_back('~');
                plain(g.R(g.P(30) ? 40 : 8));
            }
        }
        if (s.size() > 200) s.resize(200);
        // una etiqueta cortada por el recorte dejaria una '~' sin pareja: se comprueba la paridad y, si hace falta, se quita la ultima '~' y lo que sigue
        u32 tildes = 0;
        for (u8 ch : s) tildes += ch == '~';
        if (tildes & 1) {
            const auto it = std::find(s.rbegin(), s.rend(), u8('~'));
            s.resize(s.size() - u32(it - s.rbegin()) - 1);
        }
        c.s = s;
        c.a[3] = Offset(g, g.R(4), u32(s.size()) + 1);
        break;
    }
    case kMapChar:
        c.a[3] = g.P(50) ? (g.W() & 0xFFFFFF00u) | g.R(256) : (g.W() & 0xFFFFFF00u) | (g.P(50) ? 0x70u + g.R(0x20) : 0x80u + g.R(0x80));
        break;
    case kPoints: {
        c.a[3] = g.P(10) ? 0u : g.R(7);
        c.a[4] = g.P(30) ? 1u : 0u;
        c.a[5] = Coord(g); c.a[6] = Coord(g); c.a[7] = Coord(g);
        c.a[8] = Heading(g);
        c.a[9] = Offset(g, g.R(4), 0x48);
        c.a[10] = Offset(g, g.R(4), 0x48);
        for (unsigned i = 0; i < 18; ++i) c.a[16 + i] = Coord(g);
        break;
    }
    case kVecs:
        c.a[3] = g.P(10) ? 0u : g.R(7);
        c.a[4] = g.P(30) ? 1u : 0u;
        c.a[9] = Offset(g, g.R(4), 0x48);
        c.a[10] = Offset(g, g.R(4), 0x48);
        for (unsigned i = 0; i < 18; ++i) c.a[16 + i] = g.P(70) ? B(g.Fl(-100.0f, 100.0f)) : g.W();
        c.a[34] = Raw(g);
        break;
    case kCross: {
        const u32 mode = g.R(8);
        for (unsigned i = 3; i <= 10; ++i) c.a[i] = Coord(g);
        if (mode == 1) { c.a[5] = c.a[3]; c.a[6] = c.a[4]; }                                       // B = A (producto un cuadrado)
        else if (mode == 2) { c.a[7] = c.a[3]; c.a[8] = c.a[4]; }                                  // origen = A (distancia cero)
        else if (mode == 3) { c.a[9] = 0u; c.a[10] = 0u; }                                         // direccion nula
        else if (mode == 4) { c.a[3] = B(g.Fl(-10, 10)); c.a[4] = c.a[3]; c.a[5] = B(g.Fl(-10, 10)); c.a[6] = c.a[5]; c.a[7] = 0u; c.a[8] = 0u; c.a[9] = B(1.0f); c.a[10] = B(1.0f); }  // diagonal
        else if (mode == 5) { for (unsigned i = 3; i <= 10; ++i) c.a[i] = B(g.Fl(-1.0f, 1.0f)); }  // valores cercanos
        else if (mode == 6) { for (unsigned i = 3; i <= 10; ++i) c.a[i] = g.P(50) ? g.Pk({0x7E800000u, 0xFE800000u, 0x7F000000u, 0xFF000000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x00800000u, 0x80800000u}) : B(g.Fl(-2.0f, 2.0f)); }   // extremos del rango
        break;
    }
    case kHandCtor:
        c.a[3] = prev();
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kNode:
        c.a[3] = word16(); c.a[4] = word16(); c.a[5] = word16();
        c.a[6] = Offset(g, g.R(4), 12);
        break;
    case kTrainSet:
        c.a[3] = Raw(g);
        c.a[4] = g.P(45) ? 1u : 0u;
        c.a[5] = Offset(g, g.R(4), 0x20);
        c.a[6] = Raw(g);
        c.a[7] = Raw(g);
        break;
    case kTrainPre: {
        const u32 flagsWord = g.Pk({0u, 8u, 0x10u, 0x18u, 0xFFFFu, 0x0281u, 0x0289u, 0x0291u, 0x0299u, 0x0001u, 0x0100u, 0x0108u, 0x0110u, g.W() & 0xFFFFu, g.W() & 0xFFFFu, g.W() & 0xFFFFu});
        c.a[3] = flagsWord;
        c.a[4] = g.Pk({449u, 449u, 449u, 538u, 537u, 0u, 0xFFFFu, 0x01C1u, 0x11C1u, 0x81C1u, g.W() & 0xFFFFu, g.W() & 0xFFFFu});
        c.a[5] = g.P(40) ? 1u : 0u;
        c.a[6] = Raw(g);
        c.a[7] = Offset(g, g.R(4), 0x40);
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
