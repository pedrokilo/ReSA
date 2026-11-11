#pragma once
// Fixtures del banco Fn166A (lote 166A): 16384 casos por funcion. Solo C++ estandar (sin tipos del juego): las incluyen Core.hpp / LiveComparison.hpp y la autoprueba sin juego.
//
// 24 funciones del EXE SIN hook antes del lote (nombres por desensamblado, vtables y llamadores; el listado del encargo erraba muchas clases):
//   idx  direccion  nombre real                                              forma
//    0   0x622E40   CTaskSimpleFight::Clone (V)                              thiscall() `ret`: CTask::operator new(0x28) 0x61A5A0; ctor 0x61C470(objetivo +18h, movsx byte +27h, movzx word +10h); devuelve el EAX del ctor
//    1   0x6239D0   CTaskSimpleFight::ScalarDeletingDestructor               thiscall(banderas) `ret 4`: cuerpo 0x61C530; delete 0x61A5B0 si bit 0
//    2   0x624B30   CTaskSimpleFightingControl::ScalarDeletingDestructor     idem con 0x61DCA0
//    3   0x62EC10   CTaskSimpleIKChain::ConstructorDefaultBody               thiscall() `ret`: 0x61A390; [this] = 0x86DB48; devuelve this
//    4   0x694EC0   CTaskComplexFollowPedFootsteps::GetTaskType (V)          `mov eax, 0x3A8`
//    5   0x6955D0   SetMaxEntityDist2D (instancia de CTaskComplexSeekEntity) thiscall(float) `ret 4`: si != [+18h] (NaN tambien) guarda y reinicia el temporizador de +28h
//    6   0x695600   CTaskComplexSeekEntityAnyMeansXYOffset::ConstructorExe   thiscall(entidad, periodo, radio) `ret 0Ch`
//    7   0x730B30   GrowableArrayPushBack                                    thiscall(valor) `ret 4`: ampliacion 0x7304B0 mientras numero >= capacidad
//    8   0x7315A0   ScratchDirectoryInit                                     `ret`: operator new 0x82119A (0x800 y 0x10), CDirectory(0x1388) 0x5322A0; AL = 1
//    9   0x731650   CTxdStore::PluginAttach                                  `ret`: RwTexDictionaryRegisterPlugin 0x7F3C10; [0xC88018] = resultado; AL = resultado != -1
//   10   0x4A9CF0   FxPrimBP_c::ConstructorBody                              thiscall() `ret`
//   11   0x4A9D20   FxPrimBP_c::DestructorBody                               thiscall() `ret`: RwTextureDestroy 0x7F3820 de +0Ch, +10h, +14h
//   12   0x4B12E0   CEventHitByWaterCannon::GetEventType (V)                 0x40
//   13   0x4B12F0   CEventHitByWaterCannon::GetLifeTime (V)                  0
//   14   0x4B1300   CEventHitByWaterCannon::GetLocalSoundLevel (V)           ST0 = 55.0f
//   15   0x4B1310   CEventHitByWaterCannon::GetEventPriority (V)             0x40
//   16   0x4B13D0   CEventInWater::AffectsPed (V)                            thiscall(ped) `ret 4`
//   17   0x4B1420   CEventInWater::TakesPriorityOver (V)                     thiscall(evento) `ret 4`
//   18   0x61BB10   CTaskSimpleRunNamedAnim::StartAnim                       thiscall(ped) `ret 4`
//   19   0x61BBC0   CTaskSimpleRunTimedAnim::ScalarDeletingDestructor        cuerpo 0x48E090
//   20   0x46AD50   CTaskComplexSeekEntityStandard::ScalarDeletingDestructor cuerpo 0x46AD70
//   21   0x46AE00   CMatrix::AddTranslation                                  thiscall(vector) `ret 4`: x87
//   22   0x46AE80   CTheScripts::ScriptEntryListAdd                          thiscall(vector, texto) `ret 8`
//   23   0x46AEF0   CTaskSimpleSetCharDecisionMaker::ScalarDeletingDestructor cuerpo 0x46AF10
//
// Familia = n % 16: tipo = familia % 4 (posicion de `this`) y relleno de las arenas = familia / 4 (0 aleatorio, 1 ceros, 2 unos, 3 0xCD).
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

namespace F166AFix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 16384;
constexpr unsigned kFamilies = 16;
constexpr unsigned kFunctions = 24;

constexpr std::size_t kGuard = 0x40;
constexpr std::size_t kWin = 0x1000;
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

struct C {
    u32 fn{}, n{}, family{}, seed{};
    u32 eax{}, ecx{}, edx{}, stack{};
    std::array<u32, 40> a{};
    std::vector<u8> s;
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

inline u32 Offset(Gen& g, u32 kind, u32 span) {
    const u32 win = u32(kWin);
    switch (kind) {
    case 0: return 4 * g.R((win - span) / 4 + 1);
    case 1: return 4 * g.R((win - span) / 4 - 1) + 1 + g.R(3);
    case 2: return win - span - 4 + g.R(0x30);
    default: return g.P(50) ? 4 * g.R(8) : win - span - 4 * g.R(8);
    }
}
inline u32 Raw(Gen& g) {
    const u32 w1 = 0x00400000u + g.R(0x800000), w2 = 0x00400000u + g.R(0x800000), w3 = 0x00B74000u + g.R(0x1000), w4 = g.R(0x10000), w5 = g.W(), w6 = g.W(), w7 = g.W();
    return g.Pk({0u, 0xFFFFFFFFu, 1u, 0x80000000u, 0x7FFFFFFFu, w1, w2, w3, w4, w5, w6, w7, 0xFFFFFFFCu, 0x4u, 0xDEADBEEFu});
}
inline u32 RawNz(Gen& g) { return Raw(g) | 1u; }   // distinto de cero y nunca una direccion de arena alineada del oraculo
inline u32 FlagsDword(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = g.W() | 1u, w4 = g.W() & ~1u;
    return g.Pk({0u, 1u, 2u, 3u, 0xFFu, 0x100u, 0x101u, 0xFFFFFF00u, 0xFFFFFF01u, 0xFFFFFFFFu, 0x80000000u, 0x80000001u, 0xFEu, w1, w2, w3, w4});
}
inline bool IsNan(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu); }
inline u32 FloatBitsOf(Gen& g) {
    const u32 w1 = g.W(), w2 = g.W(), w3 = 0x7F800001u | (g.W() & 0x007FFFFFu), w4 = 0xFF800001u | (g.W() & 0x007FFFFFu), w5 = g.W() & 0x007FFFFFu, w6 = 0x3F800000u + (g.W() & 0x7FFFFu);
    return g.Pk({0u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x7F800000u, 0xFF800000u, 0x7FC00000u, 0xFFC00000u, 0x7F800001u, 0xFF800001u, 0x7FBFFFFFu, 0x00000001u, 0x007FFFFFu, 0x7F7FFFFFu, 0xFFFFFFFFu, w1, w2, w3, w4, w5, w6});
}
inline u32 NonNanBits(Gen& g) {
    for (int i = 0; i < 16; ++i) { const u32 v = FloatBitsOf(g); if (!IsNan(v)) return v; }
    return 0x3F800000u;
}

enum Kind : u32 { kClone, kSdd, kIkCtor, kConst, kFloat, kSetDist, kAnyMeans, kPush, kScratch, kPlugin, kFxCtor, kFxDtor, kAffects, kTakes, kStartAnim, kMatAdd, kEntryAdd };
inline constexpr std::array<Kind, kFunctions> kKind{kClone, kSdd, kSdd, kIkCtor, kConst, kSetDist, kAnyMeans, kPush, kScratch, kPlugin, kFxCtor, kFxDtor, kConst, kConst, kFloat, kConst,
                                                    kAffects, kTakes, kStartAnim, kSdd, kSdd, kMatAdd, kEntryAdd, kSdd};
inline constexpr std::array<u32, kFunctions> kEntry{0x622E40, 0x6239D0, 0x624B30, 0x62EC10, 0x694EC0, 0x6955D0, 0x695600, 0x730B30, 0x7315A0, 0x731650, 0x4A9CF0, 0x4A9D20,
                                                    0x4B12E0, 0x4B12F0, 0x4B1300, 0x4B1310, 0x4B13D0, 0x4B1420, 0x61BB10, 0x61BBC0, 0x46AD50, 0x46AE00, 0x46AE80, 0x46AEF0};
// cuerpo (colaborador) que llaman los destructores con borrado y los constructores de base (0 = ninguno)
inline constexpr std::array<u32, kFunctions> kBodyExe{0, 0x61C530, 0x61DCA0, 0x61A390, 0, 0, 0x61A3B0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x48E090, 0x46AD70, 0, 0, 0x46AF10};
inline constexpr std::array<u32, kFunctions> kConstVal{0, 0, 0, 0, 0x3A8, 0, 0, 0, 0, 0, 0, 0, 0x40, 0, 0, 0x40, 0, 0, 0, 0, 0, 0, 0, 0};
inline constexpr std::array<u32, kFunctions> kSpan{0x30, 4, 4, 4, 4, 0x34, 0x34, 0x10, 4, 4, 0x40, 0x40, 4, 4, 4, 4, 4, 0x10, 0x64, 4, 4, 0x44, 0x200, 4};

inline constexpr u32 kTaskNew = 0x61A5A0, kFightCtor = 0x61C470, kTaskDelete = 0x61A5B0, kRegRef = 0x571B70, kResize = 0x7304B0, kNewScalar = 0x82119A, kDirCtor = 0x5322A0, kRegPlugin = 0x7F3C10;
inline constexpr u32 kTexDestroy = 0x7F3820, kGetActive = 0x681720, kIsPlayer = 0x5DF8F0, kIsAlive = 0x5E0170, kBlend = 0x4D4410, kSetFinish = 0x4CEBE0, kSetDelete = 0x4CEBC0;
inline constexpr u32 kEvTaskType = 0xE0000001u, kEvType = 0xE0000002u, kEvPrioRef = 0xE0000003u, kEvPrioThis = 0xE0000004u;
inline constexpr u32 kPluginGlobal = 0xC88018, kTimeGlobal = 0xB7CB84;

// ------------------------------------------------------------------------------------------------ generador
//   a[0] = desplazamiento de `this` en A, a[1] = tipo, a[2] = relleno, a[12..15] = semillas A, B, C y global; a[16..39] = datos extra por tipo
//   kSdd/kIkCtor: a[3] = dword previo en [this], a[4] = gRet, a[5] = gDtorWrite, a[6] = banderas (dword)
//   kClone: a[3] = dword en +10h (word bajo), a[4] = objetivo (+18h; si a[11]: puntero a B a[10]), a[5] = dword en +24h (byte +27h = a[5]>>24), a[6] = new (0 nulo, 1 B a[7], 2 crudo a[9]), a[8] = retorno del ctor (0 = el nuevo, 1 = crudo a[9], 2 = 0)
//   kSetDist: a[3] = actual (+18h), a[4] = argumento (bits), a[5..7] = dwords previos +28h, +2Ch, +30h
//   kAnyMeans: a[3] = previo, a[4] = gRet, a[5] = gDtorWrite, a[6] = modo de entidad (0 nula, 1 B a[7], 2 cruda a[8]|1), a[9] = periodo, a[10] = radio (bits)
//   kPush: a[3] = capacidad, a[4] = numero, a[5] = valor, a[6] = modo (0 B, 1 C, 2..5 alias en A: this+4, +8, +0, +0xC), a[7] = desplazamiento del destino de la escritura en B/C
//   kScratch: a[3] = retorno 1.o de new (crudo), a[4] = modo 2.o (0 nulo, 1 B a[5], 2 crudo a[6]|1), a[7] = retorno del ctor
//   kPlugin: a[3] = retorno del registro, a[4] = valor previo del global
//   kFxCtor: a[3] = previo; kFxDtor: a[3] = previo, a[4..7] = modo de las 4 texturas (0 nula, 1 B a[8+k], 2 cruda), a[12 es semilla] -> a[16..19] = desplazamientos en B, a[20..23] = crudos
//   kConst/kFloat: sin datos
//   kAffects: a[3] = modo de tarea (0 nula, 1 en C a[4]), a[5] = IsPlayer, a[6] = IsAlive, a[7] = tipo de tarea, a[8] = ped en B, a[9] = inteligencia en C
//   kTakes: a[3] = aceleracion (bits), a[4] = tipo del otro evento, a[5] = prioridad propia, a[6] = prioridad ajena, a[7] = evento ajeno en B
//   kStartAnim: ver Setup; kMatAdd: a[3..5] = vector, a[6] = vector en B, a[7..9] = posicion, a[10], a[11], a[20], a[21] = vecinos (+28h, +2Ch, +3Ch, +40h), a[22] = modo del vector (0 en B, 1..5 alias)
//   kEntryAdd: ver Setup
inline void MakeAll(C& c, unsigned n) {
    const u32 f = c.family;
    Gen g(n, 100 + c.fn * 100);
    const u32 kind = f % 4;
    c.a[1] = kind;
    c.a[2] = f / 4;
    c.a[12] = c.seed * 13u + 50u; c.a[13] = c.a[12] + 1u; c.a[14] = c.a[12] + 2u; c.a[15] = c.a[12] + 3u;
    c.a[0] = Offset(g, kind, kSpan[c.fn]);
    const u32 vt = 0x86D684;
    const auto prev = [&] { return g.Pk({0u, 0xFFFFFFFFu, vt, vt, vt + 4, 0x85AAB4u, 0x8710D0u, 0x86F518u, 0x85AB38u, vt + 1, g.W(), g.W(), 0xDEADBEEFu}); };
    switch (kKind[c.fn]) {
    case kSdd: case kIkCtor:
        c.a[3] = prev(); c.a[4] = Raw(g); c.a[5] = Raw(g); c.a[6] = FlagsDword(g);
        break;
    case kClone:
        c.a[3] = g.W();
        c.a[11] = g.P(25) ? 1u : 0u;
        c.a[10] = Offset(g, g.R(4), 4);
        c.a[4] = c.a[11] ? 0u : Raw(g);
        c.a[5] = g.Pk({0u, 0x7F000000u, 0x80000000u, 0xFF000000u, 0x01000000u, g.W(), g.W(), 0xFFFFFFFFu});
        c.a[6] = g.Pk({0u, 0u, 1u, 1u, 1u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[9] = RawNz(g);
        c.a[8] = g.Pk({0u, 0u, 1u, 2u});
        break;
    case kSetDist: {
        c.a[3] = FloatBitsOf(g);
        c.a[4] = FloatBitsOf(g);
        if (g.P(30)) c.a[4] = c.a[3];
        else if (g.P(10)) c.a[4] = c.a[3] ^ 0x80000000u;
        else if (g.P(8)) c.a[4] = (c.a[3] & 0xFF800000u) | (g.W() & 0x7FFFFFu);
        c.a[5] = g.W(); c.a[6] = g.W(); c.a[7] = g.W();
        break;
    }
    case kAnyMeans:
        c.a[3] = prev(); c.a[4] = Raw(g); c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 1u, 2u});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[8] = RawNz(g);
        c.a[9] = g.Pk({0u, 1u, 1000u, 0xFFFFFFFFu, 0x80000000u, g.W(), g.R(5000)});
        c.a[10] = FloatBitsOf(g);
        break;
    case kPush: {
        const u32 branch = g.R(10);
        i32 cap, count;
        if (branch < 5) { cap = i32(1 + g.R(20)); count = cap + i32(g.R(0x30)); }
        else if (branch < 8) { cap = i32(1 + g.R(0x80)); count = i32(g.R(0x40)) - 8; if (count >= cap) count = cap - 1; }
        else { const i32 k = i32(1 + g.R(10)); cap = -k; count = -k - i32(1 + g.R(4)); }
        c.a[3] = u32(cap); c.a[4] = u32(count);
        c.a[5] = g.W();
        c.a[6] = g.Pk({0u, 0u, 0u, 1u, 1u, 1u, 2u, 3u, 4u, 5u});
        c.a[7] = 0x80 + 4 * g.R(0x300) + (g.P(30) ? 1u + g.R(3) : 0u);
        if (c.a[0] < 0x20) c.a[0] += 0x20;
        if (c.a[0] > u32(kWin) - 0x40) c.a[0] = 4 * g.R(0x300);
        break;
    }
    case kScratch:
        c.a[3] = Raw(g);
        c.a[4] = g.Pk({0u, 1u, 1u, 2u});
        c.a[5] = Offset(g, g.R(4), 4);
        c.a[6] = RawNz(g);
        c.a[7] = Raw(g);
        break;
    case kPlugin:
        c.a[3] = g.Pk({0u, 0xFFFFFFFFu, 1u, 5u, 0x10u, 0x7FFFFFFFu, 0x80000000u, g.W(), g.W(), g.R(64)});
        c.a[4] = g.Pk({0u, 0xFFFFFFFFu, g.W(), g.W(), c.a[3], c.a[3] ^ 0x100u});
        break;
    case kFxCtor:
        c.a[3] = prev();
        break;
    case kFxDtor:
        c.a[3] = prev();
        for (unsigned k = 0; k < 4; ++k) {
            c.a[4 + k] = g.Pk({0u, 1u, 1u, 2u});
            c.a[16 + k] = Offset(g, g.R(4), 4);
            c.a[20 + k] = RawNz(g);
        }
        if (g.P(15)) { c.a[17] = c.a[16]; c.a[5] = c.a[4]; }
        break;
    case kConst: case kFloat:
        break;
    case kAffects:
        c.a[3] = g.Pk({0u, 1u, 1u});
        c.a[4] = Offset(g, g.R(4), 4);
        c.a[5] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, g.W() & ~0xFFu, (g.W() & ~0xFFu) | 1u});   // el AL de un bool es 0 o 1 (el EXE lo normaliza con setne, la DLL no)
        c.a[6] = g.Pk({0u, 1u, 0x100u, 0xFFFFFF00u, 0xFFFFFF01u, g.W() & ~0xFFu, (g.W() & ~0xFFu) | 1u});
        c.a[7] = g.Pk({0x10Cu, 0x10Cu, 0x10Cu, 0x10Bu, 0x10Du, 0u, 0xFFFFFFFFu, 0x20Cu, 0x1000010Cu, g.W(), 0x10Cu | 0x100u});
        c.a[8] = Offset(g, g.R(4), 0x480);
        c.a[9] = Offset(g, g.R(4), 4);
        break;
    case kTakes:
        c.a[3] = g.Pk({0x3F800000u, 0x3F800001u, 0x3F7FFFFFu, 0x40000000u, 0u, 0x7FC00000u, 0x7F800001u, 0x7F800000u, 0xFF800000u, 0x80000000u, 0x00000001u, 0xBF800000u, FloatBitsOf(g), FloatBitsOf(g)});
        c.a[4] = g.Pk({8u, 9u, 0x5Du, 0x5Du, 0x5Du, 0u, 1u, 0x5Cu, 0x5Eu, 0xFFFFFFFFu, 0x108u, 0x109u, 0x15Du, g.W(), 62u});
        c.a[5] = g.Pk({62u, 0u, 1u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, 64u, g.W(), g.R(100)});
        c.a[6] = g.Pk({62u, c.a[5], c.a[5], c.a[5] + 1u, c.a[5] - 1u, 0u, 0xFFFFFFFFu, 0x7FFFFFFFu, 0x80000000u, g.W(), g.R(100)});
        c.a[7] = Offset(g, g.R(4), 4);
        c.a[0] = Offset(g, kind, 0x10);
        break;
    case kStartAnim:
        c.a[3] = g.W();
        c.a[4] = FloatBitsOf(g);
        c.a[5] = Raw(g);
        c.a[6] = g.Pk({0u, 1u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu, 1000u, g.W(), g.R(100000)});
        c.a[7] = g.Pk({0u, 4u, 8u, 0xCu, 1u, 0x10u, 0xFFu, 0xFFFFFFF3u, g.W(), g.W(), 0x20u, 0x28u});
        c.a[8] = Raw(g);
        c.a[9] = g.W();
        c.a[10] = g.W(); c.a[11] = g.W(); c.a[18] = g.W(); c.a[19] = g.W(); c.a[20] = g.W();
        c.a[16] = Offset(g, g.R(4), 0x20);
        c.a[17] = Offset(g, g.R(4), 0x30);
        break;
    case kMatAdd: {
        const u32 mode = g.Pk({0u, 0u, 0u, 0u, 1u, 2u, 3u, 4u, 5u});
        c.a[22] = mode;
        c.a[6] = Offset(g, g.R(4), 12);
        u32 v[3], p[3];
        for (auto& w : p) w = mode ? NonNanBits(g) : FloatBitsOf(g);
        for (auto& w : v) w = mode ? NonNanBits(g) : FloatBitsOf(g);
        for (unsigned k = 0; k < 3; ++k) {
            if (g.P(8)) v[k] = p[k] ^ 0x80000000u;
            if (g.P(12)) p[k] = g.Pk({0x7F7FFFFFu, 0x7F000000u, 0x7F7FFFFEu, 0xFF7FFFFFu, 0x3F800000u});
            if (g.P(12)) v[k] = g.Pk({0x7F7FFFFFu, 0x7F000000u, 0x7F7FFFFEu, 0xFF7FFFFFu, 0x3F800000u});
            if (g.P(8)) { p[k] = g.Pk({0x00000001u, 0x007FFFFFu, 0x00800000u, 0x80000001u}); v[k] = g.Pk({0x00000001u, 0x007FFFFFu, 0x00800000u, 0x80000001u, 0x80800000u}); }
            if (!mode && IsNan(p[k]) && IsNan(v[k])) v[k] = NonNanBits(g);
        }
        for (unsigned k = 0; k < 3; ++k) { c.a[3 + k] = v[k]; c.a[7 + k] = p[k]; }
        c.a[10] = NonNanBits(g); c.a[11] = NonNanBits(g); c.a[20] = NonNanBits(g); c.a[21] = NonNanBits(g);
        if (c.a[0] < 0x10) c.a[0] += 0x10;
        break;
    }
    case kEntryAdd: {
        c.a[3] = g.Pk({0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0u, 1u, 7u, 8u, 9u, 0x7FFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFEu, 0x80000000u, 100u});
        if (i32(c.a[3]) < 0 && i32(c.a[3]) < -3) c.a[3] = 0xFFFFFFFFu;
        if (i32(c.a[3]) >= 8 || i32(c.a[3]) < 0) { /* sin efecto o fuera de la lista: ver abajo */ }
        if (i32(c.a[3]) < 0) { if (c.a[0] < 0x100) c.a[0] += 0x100; }
        if (i32(c.a[3]) > 8 || i32(c.a[3]) < -3) { /* >= 8: no escribe nada */ }
        c.a[4] = Offset(g, g.R(4), 12);
        for (unsigned k = 0; k < 3; ++k) c.a[5 + k] = g.W();
        c.a[8] = Offset(g, g.R(4), 0x60);
        c.a[9] = g.Pk({0u, 0u, 0u, 1u});
        c.a[10] = 0x1A8 + g.R(3);
        c.a[11] = g.Pk({0u, 0u, 0u, 1u});
        c.a[12 + 0] = c.a[12];
        c.a[16] = g.Pk({0u, 1u, 5u, 10u, 17u, 23u, 24u, 25u, 30u, 39u, 40u, 41u, 50u, g.R(60)});
        c.a[17] = g.Pk({0u, 1u, 5u, 10u, 15u, 16u, 17u, 25u, 30u, 40u, g.R(60)});
        c.a[18] = 0x10 + 4 * g.R(0x60);
        c.a[19] = 0x20 + g.R(0x60) * 4;
        c.a[20] = 0x61 + g.R(0x7E);   // semilla de los caracteres (no nulos)
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
