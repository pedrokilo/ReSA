#pragma once
// Fixtures del banco Fn172A (lote 172A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// Funciones del EXE (SIN hook antes del lote; los nombres salen del desensamblado, de los llamadores y de los hooks ya existentes, ver README.md y fixture_oracle.py). 24 asignadas, 23 reconstruidas, 1 saltada (0x509230).
//   idx  direccion  nombre real                                                                         forma
//    0   0x4DBF10   CAECutsceneTrackManager::StartTrackPlayback                                         thiscall() `ret`: DisableBassEq 4D94D0, SetChannelFlags 4D9500 (canal WORD sin signo), SetChannelVolume 4D8870, StartTrackPlayback 4D8F30
//    1   0x4DBF60   CAECutsceneTrackManager::CheckForPause                                              thiscall() `ret`: lee 0xB7CB49 / 0xB7CB48 y el byte +1; SetChannelFrequencyScalingFactor 4D8960 (canal WORD con signo)
//    2   0x4DC0E0   CAECutsceneTrackManager::PreloadCutsceneTrack                                        thiscall(track, wait) `ret 8`: +0xC = track (WORD con signo), +0x14 = +8 = 0; con espera: GetTrackPlayTime 4D8F60, Service 4DBFB0, Service del hardware 4D9870 hasta estado 2
//    3   0x4DC190   CAEStreamTransformer::Constructor (el listado decia CAECutsceneTrackManager)         thiscall() `ret`: Initialise 4F1750; eax = this
//    4   0x4EFD30   CAESoundManager::Constructor                                                        thiscall() `ret`: 300 voces de 0x74 bytes desde +4: dword +8 de cada una a cero (ctor 446FE0 de CAESound); eax = this
//    5   0x4F0B10   CAEStaticChannel::Constructor                                                       thiscall(5 dwords) `ret 14h`: base 4D7890 (campos PCM), +60h/+62h = 0, vtable 85F3CCh, BYTE +8Ah = tercer argumento
//    6   0x4F0B50   CAEStaticChannel cuerpo del destructor (marco SEH)                                  thiscall() `ret`: vtable 85F3CCh, dec 0xB5F898, Release de +8 y +0Ch (COM), cuerpo de la base 4D7910 (vtable 85F03Ch, misma secuencia)
//    7   0x506B20   CAEFrontendAudioEntity cuerpo del destructor (el listado decia CAEWeatherAudioEntity) thiscall() `ret`: vtable 862E54h; 4F2B80 sobre +144h y +9Ch; vtable 859988h; 4EF660 sobre +8
//    8   0x506BA0   CAEScriptAudioEntity cuerpo del destructor (el listado decia CAEWeatherAudioEntity)    thiscall() `ret`: vtable 862E58h; vtable 859988h en +19Ch y 4EF660 sobre +1A4h; igual en +114h/+11Ch; vtable 859988h y 4EF660 sobre +8
//    9   0x506C30   CAEAudioEntity::Destructor-4 (el listado decia CAEWeatherAudioEntity)                thiscall() `jmp`: [ecx] = 859988h; 4EF660 sobre ecx + 8
//   10   0x506C40   CAEAudioEntity::Destructor-5 (el listado decia CAEWeatherAudioEntity)                idem
//   11   0x508930   TextBufferAppend (cadena del volcado de zonas; el listado decia CAudioZones)          thiscall(source, count) `ret 8`: new(len + count) a cero; copia la cadena y source; delete del bloque anterior; len += count
//   12   0x508A10   TextBufferClear                                                                     thiscall() `ret`: con puntero no nulo: delete, puntero y longitud a cero
//   13   0x509140   TextBufferClear-2 (copia identica)                                                  idem
//   14   0x5B19D0   CCutsceneObject::Constructor (el listado decia CCutsceneMgr)                         thiscall() `ret`: ctor de CObject 5A1D10, vtable 868A60h, campos crudos, SetIsStatic(false) 5A0760, bit 0x08000000 de +1Ch a cero, +12Ch = 0.5f
//   15   0x5B1A80   CCutsceneObject cuerpo del destructor                                               thiscall() `jmp`: [ecx] = 868A60h; salto al cuerpo de CObject 59F660
//   16   0x5B1A90   CCutsceneObject::SetupCarPipeAtomicCB (el listado decia CCutsceneMgr)                 cdecl(atomic, data) `ret`: CustomCarPipeAtomicSetup 5D5B20(atomic); eax = atomic
//   17   0x49C8F0   CStuntJumpManager pool: constructor                                                  thiscall(count, x) `ret 8`: new[](count * 0x44), new[](count), +10h = 1, +8 = count, +0Ch = -1, estados = 0x80
//   18   0x49C950   CStuntJumpManager pool: Flush                                                        thiscall() `ret`: con size > 0 y BYTE +10h no nulo: delete[] x2; campos a cero
//   19   0x49C9A0   CStuntJumpManager pool: Clear                                                        thiscall() `ret`: cada estado |= 0x80 y &= 0x80 (relee puntero y tamano en cada vuelta)
//   20   0x49C9E0   CStuntJumpManager pool: New                                                          thiscall() `ret`: siguiente slot con bit 0x80; lo ocupa; devuelve storage + indice * 0x44 o 0
//   21   0x438D60   CCheat::HealthCheat                                                                  cdecl() `jmp`: vida del jugador 0 (0xB7CEE7), vehiculo 1000.0f, tipo 0 / 9, Fix (ranura 0x32)
//   22   0x43A6A0   CCheat::IsFemalePedType (el listado decia CCheat)                                     cdecl(id) `ret`: id == 0x16 || id == 5 (solo AL)
//
// Cada caso se ejecuta por la entrada del EXE con arenas propias (A, B y C, guardas de 0x40; la arena A de la funcion 4 es de 0x9000 bytes) y los colaboradores falseados (el mismo falso en la direccion del EXE y en el
// cuerpo de la DLL). Familia = n % 16: tipo = familia % 4 (posicion de `this`: 0 alineado, 1 desalineado, 2 al borde, 3 cerca del borde o del inicio) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F172AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;      // por funcion
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 23;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;       // ventana de cada arena
constexpr std::size_t kWinBig = 0x9000;    // ventana de la arena A de la funcion 4 (300 voces de 0x74 bytes)
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
    std::array<u32, 16> a{};
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

// Desplazamiento de un objeto de `span` bytes en una ventana de `win` bytes segun el tipo: 0 alineado, 1 desalineado, 2 al borde (la escritura entra en la guarda), 3 alineado cerca del borde o del inicio
inline u32 Offset(Gen& g, u32 kind, u32 span, u32 win = u32(kWin)) {
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
// Desplazamiento alineado de un objeto de `span` bytes en una ventana, sin invadir la guarda: la funcion lo desreferencia de verdad
inline u32 AuxOffset(Gen& g, u32 span, u32 win = u32(kWin)) { return 4 * g.R((win - span) / 4 + 1); }

// Un dword "interesante" para punteros crudos y argumentos: ceros, unos, bordes, valores de la imagen del EXE, aleatorios (nunca direcciones de las arenas: el oraculo no las conoce)
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
// Un dword que se lee como WORD (canales, pistas): bordes del WORD con y sin signo, bits altos
inline u32 WordLike(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.R(64), w4 = g.R(0x10000), w5 = g.R(0x10000) | 0x8000u;
    return g.Pk({0u, 1u, 2u, 0x7FFFu, 0x8000u, 0xFFFFu, 0x10000u, 0x10001u, 0xFFFF8000u, 0xFFFF7FFFu, 0x80000000u, 0xFFFFFFFFu, 0x12345678u, w1, w2, w3, w4, w5});
}
// Un byte "booleano" (se prueba con test al,al): ceros, unos, otros valores
inline u32 ByteBool(Gen& g) { const u32 r = g.R(256); return g.Pk({0u, 0u, 0u, 1u, 1u, 2u, 0x80u, 0xFFu, r}); }
// Valor para el tipo de vehiculo (+0x590): 0 automovil, 9 moto, otros
inline u32 VehType(Gen& g) { const u32 r = g.R(20); return g.Pk({0u, 0u, 0u, 9u, 9u, 9u, 1u, 2u, 3u, 8u, 10u, 11u, 0xFFFFFFFFu, 0x100u, 0x80000009u, r}); }

// Clases de funcion (forma)
enum Kind : u32 { kStart, kPause, kPreload, kXform, kSoundMgr, kChanCtor, kChanDtor, kFrontDtor, kScriptDtor, kAudioDtor, kTextAppend, kTextClear, kCutCtor, kCutDtor, kAtomicCb,
                  kPoolCtor, kPoolFlush, kPoolClear, kPoolNew, kHealth, kFemale };
inline constexpr std::array<Kind, kFunctions> kKind{
    kStart, kPause, kPreload, kXform, kSoundMgr, kChanCtor, kChanDtor, kFrontDtor, kScriptDtor, kAudioDtor, kAudioDtor, kTextAppend, kTextClear, kTextClear, kCutCtor, kCutDtor, kAtomicCb,
    kPoolCtor, kPoolFlush, kPoolClear, kPoolNew, kHealth, kFemale};
// Entrada de cada funcion en el EXE
inline constexpr std::array<u32, kFunctions> kEntry{
    0x4DBF10, 0x4DBF60, 0x4DC0E0, 0x4DC190, 0x4EFD30, 0x4F0B10, 0x4F0B50, 0x506B20, 0x506BA0, 0x506C30, 0x506C40, 0x508930, 0x508A10, 0x509140, 0x5B19D0, 0x5B1A80, 0x5B1A90,
    0x49C8F0, 0x49C950, 0x49C9A0, 0x49C9E0, 0x438D60, 0x43A6A0};
// Bytes que cada funcion toca a partir de su puntero `this` (en la arena A)
inline constexpr std::array<u32, kFunctions> kSpan{
    8, 8, 0x18, 4, 4 + 300 * 0x74, 0x90, 0x10, 0x1EC, 0x21C, 0x10, 0x10, 8, 8, 8, 0x19C, 4, 4,
    0x14, 0x14, 0x14, 0x14, 0x544, 4};
// Ventana de la arena A de cada funcion
constexpr u32 WinA(u32 fn) { return fn == 4 ? u32(kWinBig) : u32(kWin); }
// Desplazamiento de los objetos auxiliares (vtable de los COM falsos y de los vehiculos)
constexpr u32 kVtOff = 0xE00;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en la arena A, a[1] = tipo (familia % 4), a[2] = relleno (familia / 4), a[12..15] = semillas de las arenas A, B, C y de la memoria global
//   kStart:    a[3] = dword en +4 (el canal se lee como WORD); a[4] = EAX que devuelven los falsos; a[5] = dword en +0
//   kPause:    a[3] = dword en +4 (canal, WORD con signo); a[4] = dword en +0 (el byte +1 es m_bPaused); a[5] = byte de 0xB7CB49; a[6] = byte de 0xB7CB48; a[7] = EAX de los falsos
//   kPreload:  a[3] = vuelta (1..5) en la que el falso de Service pone el estado a 2; a[4] = track (dword de pila); a[5] = espera (dword de pila); a[6] = base de los estados previos; a[7] = base de los tiempos;
//              a[8] = dword en +0; a[9] = dword en +4; a[10] = dword en +0x10
//   kXform:    a[3] = dword previo en [this]; a[4] = EAX de Initialise
//   kSoundMgr: sin parametros (el relleno de la arena es el estado previo)
//   kChanCtor: a[3] = DirectSound (crudo); a[4] = id de canal (dword); a[5] = tercer argumento (dword; solo cuenta el byte bajo); a[6] = frecuencia; a[7] = bits por muestra (dword; solo cuenta el WORD bajo)
//   kChanDtor: a[3] = vtable previa; a[4] = modo del +8 (0 nulo, 1 COM en B, 2 COM en C); a[5] = desplazamiento del COM de +8; a[6] = modo de +0xC (0 nulo, 1 COM en B, 2 COM en C, 3 el mismo que +8);
//              a[7] = desplazamiento del COM de +0xC; a[8] = lo que deja el falso del cuerpo de la base en [this]; a[9] = EAX de Release; a[10] = contador global 0xB5F898
//   kFrontDtor, kScriptDtor, kAudioDtor: a[3] = dword previo en [this]; a[4] = EAX de los falsos
//   kTextAppend: a[3] = longitud actual; a[4] = bytes a agregar; a[5] = puntero nulo si la longitud es 0 (1) o bloque de monton (0); a[6] = arena de origen (0 B, 1 C); a[7] = desplazamiento del origen; a[8] = semilla del contenido
//   kTextClear:  a[3] = hay bloque (0 puntero nulo, 1 bloque); a[4] = longitud (dword); a[5] = tamano del bloque (1..64)
//   kCutCtor:  a[3] = EAX de los falsos; a[4] = lo que el falso del constructor de CObject deja en [this]; a[5] = mascara XOR que el falso de SetIsStatic aplica a +0x1C
//   kCutDtor:  a[3] = dword previo en [this]; a[4] = lo que el falso del cuerpo de CObject deja en [this]
//   kAtomicCb: a[3] = atomic (crudo); a[4] = data (crudo); a[5] = EAX del falso
//   kPoolCtor: a[3] = cuenta; a[4] = segundo argumento (crudo)
//   kPoolFlush: a[3] = tamano; a[4] = dword en +0x10 (solo cuenta el byte bajo); a[5] = ultimo libre
//   kPoolClear: a[3] = tamano; a[4] = modo (0 estados en B, 1 estados = this + 8); a[5] = desplazamiento de los estados en B
//   kPoolNew:  a[3] = tamano (1..255); a[4] = ultimo libre; a[5] = desplazamiento de los estados en B; a[6] = storage (crudo); a[7] = densidad (0 todos ocupados, 1 todos libres, 2 un cuarto, 3 tres cuartos, 4 uno);
//              a[8] = semilla de los contadores; a[9] = semilla de la distribucion; a[10] = indice del unico libre
//   kHealth:   a[3] = vida maxima (byte global); a[4] = selector de vehiculo por llamada (bit j: 0 B, 1 C); a[5] = si es multiplo de 4, la primera llamada devuelve nulo; a[6], a[7] = tipos de vehiculo en B y en C;
//              a[8], a[9] = desplazamientos de los vehiculos en B y en C; a[10] = desplazamiento del ped en A; a[11] = EAX de Fix
//   kFemale:   a[3] = id de tipo de peaton
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    const u32 win = WinA(c.fn);
    c.a[0] = Offset(g, kind, kSpan[c.fn], win);
    const auto len = [&] { const u32 r1 = g.R(300), r2 = g.R(300), r3 = g.R(64); return g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 7u, 8u, 15u, 16u, 31u, 100u, 255u, 256u, 300u, r1, r2, r3}); };
    switch (kKind[c.fn]) {
    case kStart:
        c.a[3] = WordLike(g);
        c.a[4] = Raw(g);
        c.a[5] = g.W();
        break;
    case kPause:
        c.a[3] = WordLike(g);
        c.a[4] = g.Pk({0u, 0x100u, 0xFF00u, 0x200u, 0x8000u, 0x1u, 0xFFu, g.W(), g.W() & ~0xFF00u});
        c.a[5] = ByteBool(g);
        c.a[6] = ByteBool(g);
        c.a[7] = Raw(g);
        break;
    case kPreload:
        c.a[3] = 1 + g.R(5);
        c.a[4] = WordLike(g);
        c.a[5] = g.Pk({0u, 0u, 1u, 1u, 1u, 0x100u, 0xFFu, 2u, 0x80u, g.W(), g.W() & ~0xFFu});
        c.a[6] = g.W();
        c.a[7] = g.W();
        c.a[8] = g.W();
        c.a[9] = g.W();
        c.a[10] = g.W();
        break;
    case kXform:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85AAB4u, 0x8710D0u, 0xDEADBEEFu, g.W()});
        c.a[4] = Raw(g);
        break;
    case kSoundMgr:
        break;
    case kChanCtor:
        c.a[3] = Raw(g);
        c.a[4] = g.Pk({0u, 1u, 2u, 0xFFFFu, 0x10000u, 0x10001u, 0xFFFFFFFFu, 0x12345678u, g.W(), g.R(64)});
        c.a[5] = g.Pk({0u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0x80u, g.W()});
        c.a[6] = g.Pk({0u, 1u, 11025u, 22050u, 44100u, 48000u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, g.W(), g.R(100000)});
        c.a[7] = g.Pk({0u, 1u, 7u, 8u, 9u, 16u, 24u, 32u, 0xFFFFu, 0x10000u, 0x10008u, 0x10010u, 0xFFFFFFFFu, g.W(), g.R(64)});
        break;
    case kChanDtor:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x85F3CCu, 0x85F03Cu, 0x85F3CCu, 0xDEADBEEFu, g.W()});
        c.a[4] = g.Pk({0u, 1u, 1u, 2u, 2u});
        c.a[5] = AuxOffset(g, 0x20, kVtOff);
        c.a[6] = g.Pk({0u, 1u, 2u, 3u, 3u});
        c.a[7] = AuxOffset(g, 0x20, kVtOff);
        c.a[8] = g.Pk({0u, 0x85F03Cu, 0xFFFFFFFFu, g.W()});
        c.a[9] = Raw(g);
        c.a[10] = g.Pk({0u, 1u, 2u, 5u, 64u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(100)});
        break;
    case kFrontDtor:
    case kScriptDtor:
    case kAudioDtor:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x859988u, 0x862E54u, 0x862E58u, 0x85AAB4u, 0xDEADBEEFu, g.W()});
        c.a[4] = Raw(g);
        break;
    case kTextAppend:
        c.a[3] = len();
        c.a[4] = len();
        c.a[5] = g.Pk({0u, 1u});
        c.a[6] = g.R(2);
        c.a[7] = g.P(60) ? AuxOffset(g, 304) : g.R(u32(kWin) - 304);
        c.a[8] = g.W();
        break;
    case kTextClear:
        c.a[3] = g.Pk({0u, 1u, 1u, 1u});
        c.a[4] = g.Pk({0u, 1u, 5u, 100u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(64)});
        c.a[5] = 1 + g.R(64);
        break;
    case kCutCtor:
        c.a[3] = Raw(g);
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x868A60u, 0x866F60u, 0xDEADBEEFu, g.W()});
        c.a[5] = g.Pk({0u, 0x08000000u, 0x00000001u, 0x10000400u, 0xFFFFFFFFu, 0xF7FFFFFFu, g.W()});
        break;
    case kCutDtor:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 0x868A60u, 0x85AAB4u, 0xDEADBEEFu, g.W()});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, 0x866F60u, 0xDEADBEEFu, g.W()});
        break;
    case kAtomicCb:
        c.a[3] = Raw(g);
        c.a[4] = Raw(g);
        c.a[5] = Raw(g);
        break;
    case kPoolCtor:
        c.a[3] = g.Pk({0u, 1u, 2u, 3u, 7u, 8u, 100u, 255u, 256u, 1000u, 4095u, 4096u, g.R(300), g.R(5000)});
        c.a[4] = Raw(g);
        break;
    case kPoolFlush:
        c.a[3] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFBu, 0u, 0u, 1u, 2u, 5u, 100u, 256u, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(1000)});
        c.a[4] = g.Pk({0u, 1u, 1u, 2u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, g.W()});
        c.a[5] = g.Pk({0xFFFFFFFFu, 0u, 1u, 100u, g.W()});
        break;
    case kPoolClear:
        c.a[4] = g.Pk({0u, 0u, 0u, 1u});
        if (c.a[4] == 1) {
            c.a[3] = 1 + g.R(255);
            c.a[0] = AuxOffset(g, 0x100 + 0x90, win);        // las escrituras recorren this+8 .. this+8+127: lejos del borde
        } else {
            c.a[3] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFDu, 0u, 1u, 2u, 3u, 8u, 64u, 255u, 256u, 300u, g.R(300), g.R(300), g.R(20)});
        }
        c.a[5] = AuxOffset(g, 304);
        break;
    case kPoolNew:
        c.a[3] = g.Pk({1u, 2u, 3u, 7u, 8u, 100u, 255u, 1 + g.R(255), 1 + g.R(255), 1 + g.R(32)});
        c.a[4] = g.Pk({0xFFFFFFFFu, 0xFFFFFFFFu, c.a[3] - 1, c.a[3] > 1 ? c.a[3] - 2 : 0u, 0u, g.R(c.a[3]), g.R(c.a[3]), 0xFFFFFFFEu, 0xFFFFFFFDu});
        c.a[5] = 4 * (4 + g.R((u32(kWin) - 300) / 4 - 8));
        c.a[6] = Raw(g);
        c.a[7] = g.R(5);
        c.a[8] = g.W();
        c.a[9] = g.W();
        c.a[10] = g.W();
        break;
    case kHealth:
        c.a[3] = g.Pk({0u, 1u, 100u, 255u, 200u, 150u, g.R(256), g.R(256)});
        c.a[4] = g.W();
        c.a[5] = g.W();
        c.a[6] = VehType(g);
        c.a[7] = VehType(g);
        c.a[8] = 4 * g.R(0x500 / 4 + 1);
        c.a[9] = 4 * g.R(0x500 / 4 + 1);
        c.a[10] = AuxOffset(g, 0x548);
        c.a[11] = Raw(g);
        break;
    case kFemale:
        c.a[3] = g.Pk({0x16u, 0x16u, 5u, 5u, 0u, 1u, 4u, 6u, 21u, 23u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x116u, 0x105u, g.W(), g.R(40), g.R(40)});
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
