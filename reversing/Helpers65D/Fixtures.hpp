#pragma once
// Fixtures del banco Helpers65D (lote 65, ayudantes sueltos del EXE): 4096 casos por funcion. Solo C++ estandar (sin tipos del juego):
// las incluyen Core.hpp / Cases.hpp / LiveComparison.hpp.
//
//   0 operator*(const CMatrix&, const CVector&)        0x59C890  cdecl (CVector* out, const CMatrix*, const CVector*), EAX = out        (hook CMatrix / operator*-Vec)
//   1 CTaskComplexWalkRoundObject::CreateRouteTask      0x655140  thiscall (this, CPed*), ret 4, EAX = tarea o nulo                       (hook CTaskComplexWalkRoundObject / CreateRouteTask)
//   2 RwTexDictionaryGtaStreamRead                      0x730FC0  cdecl (RwStream*), EAX = diccionario o nulo                              (hook RwHelper / RwTexDictionaryGtaStreamRead)
//   3 FightStrikeDamageExe                              0x61C740  thiscall (CTaskSimpleFight*, CPed*), ret 4, resultado float en ST0      (hook CTaskSimpleFight / FightStrikeDamage)
//
// Todos los floats viajan como patrones de 32 bits sin signo.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>
#include <vector>

namespace Hp65Fix {
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u64 = std::uint64_t;

constexpr unsigned kCases = 4096;

constexpr u32 Mix(u32 x) { x ^= x >> 16; x *= 0x7FEB352Du; x ^= x >> 15; x *= 0x846CA68Bu; x ^= x >> 16; return x; }
constexpr u32 Rand(u32 c, u32 k) { return Mix(c * 0x9E3779B1u + k * 0x85EBCA6Bu + 12345u); }
inline constexpr std::array<u32, 8> kStackSeeds{0u, 0xFFFFFFFFu, 0x14u, 0x10u, 1u, 0x80000000u, 0x9E3779B1u, 0x55555555u};
inline u32 StackSeed(unsigned n) { return n % 13 < 8 ? kStackSeeds[n % 13] : Rand(n + 0x1000u, 99); }
inline void FillWords(u8* dst, std::size_t bytes, u32 seed) {
    for (std::size_t i = 0; i < bytes; i += 4) {
        const u32 w = Rand(seed, u32(i / 4));
        const std::size_t n = bytes - i < 4 ? bytes - i : 4;
        std::memcpy(dst + i, &w, n);
    }
}

// ------------------------------------------------------------------------------------------------ JSON minimo (mismas filas `input` en el juego y en cualquier espejo)
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

// ------------------------------------------------------------------------------------------------ floats
constexpr u32 NanBits(u32 slot, bool signaling) {
    const u32 sign = (slot & 1u) << 31;
    return signaling ? (sign | 0x7F800000u | (0x2000u + slot * 0x41u)) : (sign | 0x7FC00000u | (0x100u + slot * 0x37u));
}
inline constexpr std::array<u32, 20> kFloatEdges{
    0x00000000u, 0x80000000u, 0x3F800000u, 0xBF800000u, 0x3DCCCCCDu, 0xBDCCCCCDu, 0x3D4CCCCDu, 0x00000001u, 0x80000001u, 0x007FFFFFu,
    0x807FFFFFu, 0x00800000u, 0x7F7FFFFFu, 0xFF7FFFFFu, 0x3F000000u, 0x40000000u, 0x3E000000u, 0x45000000u, 0x00400000u, 0x80400000u};
inline u32 OrdinaryFloat(u32 n, u32 k, int minExp, int maxExp) {   // exponentes sesgados (127 = 1.0)
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = u32(minExp) + Rand(n, k + 1) % u32(maxExp - minExp + 1);
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 TinyFloat(u32 n, u32 k) {   // denormal o casi
    const u32 sign = Rand(n, k) & 1u;
    const u32 exp = Rand(n, k + 1) % 3 == 0 ? Rand(n, k + 3) % 6u : 0u;
    return (sign << 31) | (exp << 23) | (Rand(n, k + 2) & 0x7FFFFFu);
}
inline u32 HugeFloat(u32 n, u32 k) {
    const u32 sign = Rand(n, k) & 1u;
    return (sign << 31) | ((200u + Rand(n, k + 1) % 55u) << 23) | (Rand(n, k + 2) & 0x7FFFFFu);   // hasta 0x7F7FFFFF
}
inline u32 InfBits(u32 n, u32 k) { return Rand(n, k) & 1u ? 0xFF800000u : 0x7F800000u; }
inline bool IsNanBits(u32 b) { return (b & 0x7F800000u) == 0x7F800000u && (b & 0x7FFFFFu) != 0; }
inline float F(u32 b) { float f; std::memcpy(&f, &b, 4); return f; }
inline u32 B(float f) { u32 b; std::memcpy(&b, &f, 4); return b; }
inline u32 SeedReg(u32 n, u32 k) {
    static constexpr u32 sp[]{0u, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x00000001u, 0xFFFFFF00u, 0x000000FFu, 0xDEADBEEFu};
    return Rand(n, k) % 5 == 0 ? sp[Rand(n, k + 1) % std::size(sp)] : Rand(n, k + 2);
}
// Resultado de bool de un colaborador: falso = AL 0 con basura en los bits altos, o AL != 0 con basura en los bits altos
inline u32 BoolRet(u32 n, u32 k, bool wantTrue) {
    static constexpr u32 als[]{1u, 2u, 0x80u, 0xFFu, 0x7Fu, 0x81u};
    if (!wantTrue) return Rand(n, k + 1) % 2 ? 0u : (Rand(n, k + 2) & 0xFFFFFF00u);
    return (Rand(n, k + 3) % 2 ? 0u : (Rand(n, k + 4) & 0xFFFFFF00u)) | als[Rand(n, k + 5) % std::size(als)];
}

// ------------------------------------------------------------------------------------------------ arenas (suma a mano: guarda + ventana + guarda)
constexpr u32 kGuard = 0x40;
// 0: matriz (16 dwords = 0x40 bytes), punto (0x10) y destino (0x10). Cada una con guardas de 0x20: 0x20 + 0x40 + 0x20 = 0x80; 0x20 + 0x10 + 0x20 = 0x50.
constexpr u32 kMatGuard = 0x20, kMatWindow = 0x40, kMatArena = kMatGuard + kMatWindow + kMatGuard;          // 0x80
constexpr u32 kVecGuard = 0x20, kVecWindow = 0x10, kVecArena = kVecGuard + kVecWindow + kVecGuard;          // 0x50
// 1: tarea CTaskComplexWalkRoundObject (0x54), ruta CPointRoute (0x64), tarea nueva (CTaskComplexFollowPointRoute 0x3C, ventana 0x40) y ruta nueva (ventana 0x70)
constexpr u32 kWalkWindow = 0x54, kWalkArena = kGuard + kWalkWindow + kGuard;                              // 0x40 + 0x54 + 0x40 = 0x134
constexpr u32 kRouteWindow = 0x64, kRouteArena = kGuard + kRouteWindow + kGuard;                           // 0x40 + 0x64 + 0x40 = 0x144
constexpr u32 kNewTaskWindow = 0x40, kNewTaskArena = kGuard + kNewTaskWindow + kGuard;                     // 0x40 + 0x40 + 0x40 = 0x140
constexpr u32 kNewRouteWindow = 0x70, kNewRouteArena = kGuard + kNewRouteWindow + kGuard;                  // 0x40 + 0x70 + 0x40 = 0x150
// 3: tarea CTaskSimpleFight (ventana 0x30), datos de jugador y estadisticas (ventana 0x40) y ped con 0x900 bytes antes del objeto y 0x1500 despues
//    (el EXE indexa las armas por la ranura con signo: 0x5A0 + ranura * 0x1C, ranura -128..127 -> -0x860..0x1388)
constexpr u32 kFightWindow = 0x30, kFightArena = kGuard + kFightWindow + kGuard;                           // 0x40 + 0x30 + 0x40 = 0xB0
constexpr u32 kSmallWindow = 0x40, kSmallArena = kGuard + kSmallWindow + kGuard;                           // 0x40 + 0x40 + 0x40 = 0xC0
constexpr u32 kPedBefore = 0x900, kPedAfter = 0x1500, kPedArena = kGuard + kPedBefore + kPedAfter + kGuard; // 0x40 + 0x900 + 0x1500 + 0x40 = 0x1E80
// Region de la tabla de combos del EXE que el banco sobrescribe con bytes del caso (y restaura): 0xC170D0 - 0x600 .. + 0x1800
constexpr u32 kTableBase = 0xC170D0u, kTableBefore = 0x600u, kTableBytes = 0x1800u;
// Texturas de una lectura de diccionario
constexpr u32 kTexTokens = 48;

// ================================================================================================ 0: matriz * punto
struct C0 {
    u32 family{}, alias{}, m[16]{}, p[3]{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
// Elemento de la matriz/punto segun la familia.
inline u32 C0Value(u32 n, u32 k, u32 f) {
    switch (f) {
    case 0: return B(float(i32(Rand(n, k) % 4001u) - 2000) / 16.0f + float(Rand(n, k + 1) % 1000u) / 1000.0f);   // mundo
    case 1: return B(float(i32(Rand(n, k) % 5u) - 2));                                                          // enteros pequenos exactos
    case 2: return OrdinaryFloat(n, k, 100, 150);
    case 3: return OrdinaryFloat(n, k, 120, 134);
    case 4: return Rand(n, k) % 3 == 0 ? TinyFloat(n, k + 3) : OrdinaryFloat(n, k + 6, 0, 30);
    case 5: return Rand(n, k) % 3 == 0 ? HugeFloat(n, k + 3) : OrdinaryFloat(n, k + 6, 120, 134);
    case 6: return Rand(n, k) % 4 == 0 ? InfBits(n, k + 3) : OrdinaryFloat(n, k + 6, 120, 134);
    case 7: return OrdinaryFloat(n, k, 110, 140);
    case 8: return OrdinaryFloat(n, k, 110, 140);
    case 9: return Rand(n, k) % 2 ? 0x80000000u : (Rand(n, k + 1) % 2 ? 0u : OrdinaryFloat(n, k + 2, 120, 130));
    default: return Rand(n, k);
    }
}
inline constexpr u32 kReadWords[]{0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14};
inline C0 MakeC0(u32 n) {
    C0 c; const u32 f = n % 16; c.family = f;
    const u32 vf = f < 12 ? f : (f % 3 == 0 ? 0u : f % 3 == 1 ? 2u : 3u);   // las familias 12..15 prueban solapes con valores corrientes
    for (u32 i = 0; i < 16; ++i) c.m[i] = C0Value(n, 10 + i * 4, vf);
    for (u32 i = 0; i < 3; ++i) c.p[i] = C0Value(n, 100 + i * 4, vf);
    // palabras que la funcion no lee (+0xC, +0x1C, +0x2C, +0x3C): basura que no debe cambiar ni influir
    for (u32 i : {3u, 7u, 11u, 15u}) c.m[i] = Rand(n, 200 + i);
    if (f == 3) {   // cancelacion: productos casi opuestos
        const u32 a = OrdinaryFloat(n, 300, 125, 131);
        c.m[8] = a; c.p[2] = OrdinaryFloat(n, 301, 125, 131);
        c.m[4] = a ^ 0x80000000u; c.p[1] = c.p[2] + (Rand(n, 302) % 5);
        c.m[0] = a; c.p[0] = Rand(n, 303) % 2 ? B(1.0f) : B(-1.0f);
        c.m[12] = B(float(Rand(n, 304) % 3));
    }
    if (f == 7) {   // un NaN con carga util propia en un elemento leido
        (Rand(n, 310) % 4 ? c.m[kReadWords[Rand(n, 311) % std::size(kReadWords)]] : c.p[Rand(n, 312) % 3]) = NanBits(Rand(n, 313) % 6, Rand(n, 314) % 4 == 0);
    }
    if (f == 8) {   // varios NaN con cargas distintas
        const u32 count = 2 + Rand(n, 320) % 3;
        for (u32 i = 0; i < count; ++i) c.m[kReadWords[Rand(n, 321 + i) % std::size(kReadWords)]] = NanBits(Rand(n, 330 + i) % 8, Rand(n, 340 + i) % 3 == 0);
        if (Rand(n, 350) % 2) c.p[Rand(n, 351) % 3] = NanBits(Rand(n, 352) % 8, Rand(n, 353) % 3 == 0);
    }
    if (f == 10) {   // un solo elemento especial entre ordinarios
        const u32 idx = Rand(n, 360) % 15;
        const u32 v = kFloatEdges[Rand(n, 361) % kFloatEdges.size()];
        if (idx < 12) c.m[kReadWords[idx]] = v; else c.p[idx - 12] = v;
    }
    c.alias = f >= 12 ? f - 11 : (Rand(n, 370) % 4 == 0 ? Rand(n, 371) % 6 : 0u);   // 0 distinto, 1 destino = punto, 2 = pos (+0x30), 3 = right (+0), 4 = forward (+0x10), 5 = +0x28 (solape en medio)
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC0(const C0& c) {
    Js j; j.U("family", c.family).U("alias", c.alias).A("m", std::vector<u32>(c.m, c.m + 16)).A("p", std::vector<u32>(c.p, c.p + 3))
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 1: CreateRouteTask
struct C1 {
    u32 family{}, moveState{}, numEntries{}, entries[24]{}, taskNull{}, routeNull{}, ped{}, seed{};
    u32 eax{}, edx{}, stack{};
};
inline C1 MakeC1(u32 n) {
    C1 c; const u32 f = n % 16; c.family = f;
    static constexpr u32 moves[]{0, 1, 2, 3, 4, 5, 6, 7, 0xFFFFFFFFu, 0x80000000u, 0x7FFFFFFFu, 0x100u, 0x12345678u, 8, 9, 100};
    c.moveState = Rand(n, 1) % 2 ? Rand(n, 2) % 8 : moves[Rand(n, 3) % std::size(moves)];
    c.numEntries = f % 8 == 0 ? 0u : f % 8 == 7 ? 8u : 1u + Rand(n, 4) % 8u;
    for (u32 i = 0; i < 24; ++i) c.entries[i] = Rand(n, 5 + i) % 6 == 0 ? kFloatEdges[Rand(n, 40 + i) % kFloatEdges.size()] : OrdinaryFloat(n, 80 + i * 3, 110, 140);
    if (f == 13) for (u32 i = 0; i < 24; ++i) if (Rand(n, 150 + i) % 4 == 0) c.entries[i] = NanBits(Rand(n, 180 + i) % 6, false);
    c.taskNull = (f >= 8 && f < 11 && Rand(n, 6) % 3 != 0) ? 1u : Rand(n, 7) % 14 == 0;
    c.routeNull = (f == 11 || f == 12) ? (Rand(n, 8) % 2) : Rand(n, 8) % 16 == 0;
    c.ped = Rand(n, 9);
    c.seed = Rand(n, 10);
    c.eax = SeedReg(n, 70); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC1(const C1& c) {
    Js j; j.U("family", c.family).U("move_state", c.moveState).U("num_entries", c.numEntries).A("entries", std::vector<u32>(c.entries, c.entries + 24))
     .U("task_null", c.taskNull).U("route_null", c.routeNull).U("ped", c.ped).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 2: RwTexDictionaryGtaStreamRead
struct C2 {
    u32 family{}, stream{}, findRet{}, chunkLen{}, version{}, payload[3]{}, readRet{}, createRet{}, texFail{}, texTok[kTexTokens]{}, addRet[kTexTokens]{}, forAllRet{}, destroyRet{};
    u32 eax{}, ecx{}, edx{}, stack{};
};
inline C2 MakeC2(u32 n) {
    C2 c; const u32 f = n % 16; c.family = f;
    c.stream = 0x01000000u + (Rand(n, 1) & 0xFFFFF0u);
    c.findRet = f == 0 || (f == 1 && Rand(n, 2) % 2) ? 0u : (Rand(n, 3) % 3 == 0 ? Rand(n, 4) | 1u : (Rand(n, 5) % 2 ? 1u : 0x100u));
    static constexpr u32 lens[]{4, 4, 4, 4, 4, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 16, 24, 64, 100};
    c.chunkLen = Rand(n, 6) % 3 ? 4u : lens[Rand(n, 7) % std::size(lens)];
    if (f == 4 || f == 5) c.chunkLen = lens[5 + Rand(n, 8) % (std::size(lens) - 5)];   // longitudes que pisan el marco o lo desbordan
    c.version = Rand(n, 9);
    // contador (word bajo) y dispositivo (word alto)
    u32 count = Rand(n, 10) % 8 == 0 ? 0u : 1u + Rand(n, 11) % 6u;
    if (f == 6) count = 7 + Rand(n, 12) % 18u;
    if (f == 7) count = Rand(n, 13) % 2 ? 0xFFFFu : 0x8000u + Rand(n, 14) % 0x100u;   // gigantesco: acaba por fallar una textura
    if (f == 8) count = 1;
    c.payload[0] = (Rand(n, 15) & 0xFFFF0000u) | count;
    c.payload[1] = Rand(n, 16); c.payload[2] = Rand(n, 17);
    // retorno de la lectura: normalmente la longitud pedida; si no, +-1, 0, el dword pisado del marco (payload[1]) o basura
    switch (Rand(n, 18) % 12) {
    case 0: c.readRet = c.chunkLen + 1; break;
    case 1: c.readRet = c.chunkLen - 1; break;
    case 2: c.readRet = 0; break;
    case 3: c.readRet = c.payload[1]; break;
    case 4: c.readRet = Rand(n, 19); break;
    default: c.readRet = c.chunkLen; break;
    }
    if (f == 9) c.readRet = c.chunkLen;
    if (f == 10 && c.chunkLen >= 5) { c.readRet = c.payload[1] = (Rand(n, 20) % 2) ? c.chunkLen : (Rand(n, 21) | 1u); }   // la relectura de la longitud decide
    if (f == 11) c.readRet = c.chunkLen;
    c.createRet = (f == 12 || (f == 11 && Rand(n, 22) % 2)) ? 0u : 0x02000000u + (Rand(n, 23) & 0xFFFFF0u);
    // texturas: indice de la que falla (si count lo alcanza); 0 en un token = lectura fallida
    c.texFail = (f >= 13) ? Rand(n, 24) % 6 : (Rand(n, 25) % 6 == 0 ? Rand(n, 26) % 6 : 0xFFFFu);
    if (f == 7) c.texFail = Rand(n, 27) % kTexTokens;
    for (u32 i = 0; i < kTexTokens; ++i) { c.texTok[i] = 0x03000000u + (Rand(n, 100 + i) & 0xFFFFF0u); c.addRet[i] = Rand(n, 200 + i); }
    c.forAllRet = Rand(n, 28); c.destroyRet = Rand(n, 29);
    c.eax = SeedReg(n, 70); c.ecx = SeedReg(n, 73); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC2(const C2& c) {
    Js j; j.U("family", c.family).U("stream", c.stream).U("find_ret", c.findRet).U("chunk_len", c.chunkLen).U("version", c.version).A("payload", {c.payload[0], c.payload[1], c.payload[2]})
     .U("read_ret", c.readRet).U("create_ret", c.createRet).U("tex_fail", c.texFail).A("tex_tok", std::vector<u32>(c.texTok, c.texTok + kTexTokens))
     .A("add_ret", std::vector<u32>(c.addRet, c.addRet + kTexTokens)).U("for_all_ret", c.forAllRet).U("destroy_ret", c.destroyRet)
     .U("eax", c.eax).U("ecx", c.ecx).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}

// ================================================================================================ 3: FightStrikeDamageExe
struct C3 {
    u32 family{}, combo{}, move{}, tableSeed{}, tableByte{}, isPlayerRet{}, playerByte{}, stat{}, slot{}, weaponType{}, attack{}, ped{}, seed{};
    u32 eax{}, edx{}, stack{};
};
// float para el multiplicador: ordinarios, ceros, diminutos, enormes (producto fuera del rango de float pero no del de la x87), inf y NaN silenciosos (fld de un NaN senalizante de un falso lo silencia)
inline u32 MultFloat(u32 n, u32 k, u32 f, bool allowSignaling) {
    switch ((f + Rand(n, k)) % 14) {
    case 0: return B(0.5f + float(Rand(n, k + 1) % 400u) / 100.0f);
    case 1: return kFloatEdges[Rand(n, k + 1) % kFloatEdges.size()];
    case 2: return TinyFloat(n, k + 2);
    case 3: return HugeFloat(n, k + 2);
    case 4: return InfBits(n, k + 2);
    case 5: return NanBits(Rand(n, k + 2) % 6, allowSignaling && Rand(n, k + 3) % 2);
    case 6: return B(1.0f);
    case 7: return OrdinaryFloat(n, k + 2, 100, 160);
    case 8: return 0x7F000000u | (Rand(n, k + 2) & 0x7FFFFFu);                           // ~1.7e38: por 255 desborda el float pero no la x87
    case 9: return 0x00200000u | (Rand(n, k + 2) & 0x1FFFFFu);                           // denormal grande
    case 10: return 0x00800000u | (Rand(n, k + 2) & 0x7FFFFFu);                          // minimo normal: por un byte pequeno sigue siendo normal
    case 11: return B(1.5f);
    case 12: return B(50.0f);
    default: return OrdinaryFloat(n, k + 2, 120, 134);
    }
}
inline C3 MakeC3(u32 n) {
    C3 c; const u32 f = n % 16; c.family = f;
    // combo (int8): -3..27 (filas -7..23 de la tabla de 0x88 bytes); movimiento (int8) -128..127
    c.combo = u32(i32(Rand(n, 1) % 31u) - 3);
    if (Rand(n, 2) % 2) c.combo = 4u + Rand(n, 3) % 12u;   // filas reales 0..11
    c.move = Rand(n, 4) % 3 == 0 ? u32(i32(Rand(n, 5) % 256u) - 128) : Rand(n, 6) % 8u;
    c.tableSeed = Rand(n, 7);
    static constexpr u32 bytes[]{0, 1, 2, 5, 10, 25, 50, 100, 127, 128, 200, 254, 255};
    c.tableByte = Rand(n, 8) % 3 == 0 ? Rand(n, 9) & 0xFFu : bytes[Rand(n, 10) % std::size(bytes)];
    const bool player = f < 8 ? (f % 2 == 0) : (Rand(n, 11) % 3 == 0);
    c.isPlayerRet = BoolRet(n, 12, player);
    c.playerByte = (f < 8 && (f / 2) % 2 == 1) ? (Rand(n, 13) | 2u) & 0xFFu : (Rand(n, 14) & 0xFFu & ~2u) | ((Rand(n, 15) % 4 == 0) ? 2u : 0u);
    c.stat = MultFloat(n, 16, f, false);
    c.slot = Rand(n, 17) % 3 == 0 ? u32(i32(Rand(n, 18) % 256u) - 128) : Rand(n, 19) % 13u;
    static constexpr u32 weapons[]{0, 1, 2, 3, 4, 5, 9, 15, 16, 22, 31, 40, 46, 0xFFFFFFFFu, 0x80000000u, 0x100u, 0x00000001u};
    c.weaponType = Rand(n, 20) % 3 == 0 ? weapons[Rand(n, 21) % std::size(weapons)] : (f >= 8 ? (f % 3) : Rand(n, 22) % 3);
    c.attack = MultFloat(n, 23, f + 3, true);
    c.ped = Rand(n, 24);
    c.seed = Rand(n, 25);
    c.eax = SeedReg(n, 70); c.edx = SeedReg(n, 76); c.stack = StackSeed(n);
    return c;
}
inline std::string JsonC3(const C3& c) {
    Js j; j.U("family", c.family).U("combo", c.combo).U("move", c.move).U("table_seed", c.tableSeed).U("table_byte", c.tableByte).U("is_player_ret", c.isPlayerRet).U("player_byte", c.playerByte)
     .U("stat", c.stat).U("slot", c.slot).U("weapon_type", c.weaponType).U("attack", c.attack).U("ped", c.ped).U("seed", c.seed).U("eax", c.eax).U("edx", c.edx).U("stack_seed", c.stack); return j.End();
}
}
